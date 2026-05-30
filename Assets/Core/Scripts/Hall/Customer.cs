using System;
using System.Collections;
using Spine.Unity;
using UnityEngine;

namespace DoggyChef
{
    // ════════════════════════════════════════════════════════════════
    //  Customer — 식당 손님의 상태 머신
    // ════════════════════════════════════════════════════════════════
    //
    //  ■ 프리팹 구조
    //    CustomerXxx
    //      ├── HealthGage  (CustomerHealthGage)
    //      └── cat         (SkeletonAnimation)
    //
    //  ■ 상태 흐름
    //    None → Walkin → Ordering → (체력 감소 시) Angry
    //                             → (음식 도착 시) Eating → Walkout
    //                 ↑ 체력 0
    //            불만족 Walkout
    // ════════════════════════════════════════════════════════════════
    public class Customer : MonoBehaviour
    {
        // ──────────────────────────────────────────────────────────
        //  상태 정의
        // ──────────────────────────────────────────────────────────

        public enum CustomerState { None, Walkin, Ordering, Eating, Happy, Angry, Walkout }

        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────

        [Header("손님 설정")]
        [Tooltip("최대 체력. Ordering 상태에서 시간이 지날수록 감소합니다.")]
        public int MaxHealth = 10;

        [Tooltip("이 값 이하로 체력이 떨어지면 Angry 상태로 전환됩니다.")]
        public int AngryThreshold = 3;

        [Tooltip("체력 1 감소 간격(초).")]
        public float HealthDecreaseInterval = 3f;

        [Header("이동")]
        [Tooltip("테이블로 걸어오는 속도 (units/sec).")]
        public float WalkSpeed = 1.5f;

        [Tooltip("퇴장하는 속도 (units/sec).")]
        public float WalkOutSpeed = 2.5f;

        [Header("사운드")]
        [Tooltip("입장 효과음")]
        public AudioClip SfxWalkin;

        [Tooltip("화남 상태 효과음 (랜덤 재생)")]
        public AudioClip[] SfxAngry;

        [Tooltip("식사 효과음 (랜덤 재생)")]
        public AudioClip[] SfxEating;

        [Tooltip("만족 퇴장 효과음")]
        public AudioClip SfxHappy;

        // ──────────────────────────────────────────────────────────
        //  이벤트
        // ──────────────────────────────────────────────────────────

        /// <summary>만족 퇴장 시 발화. HallManager가 구독하여 AddHeart()를 호출합니다.</summary>
        public Action<Customer> OnSatisfied;

        /// <summary>퇴장 완료(Destroy 직전) 시 발화. HallManager가 구독하여 스폰 슬롯을 해제합니다.</summary>
        public Action<Customer> OnWalkoutComplete;

        /// <summary>
        /// 체력이 변경될 때마다 발화 (currentHealth, maxHealth).
        /// CustomerHealthGage가 구독하여 UI를 갱신합니다.
        /// </summary>
        public event Action<int, int> OnHealthChanged;

        // ──────────────────────────────────────────────────────────
        //  런타임 상태
        // ──────────────────────────────────────────────────────────

        public CustomerState State { get; private set; } = CustomerState.None;
        public int CurrentHealth { get; private set; }

        private Table m_AssignedTable;
        private bool m_HasEaten;
        private Coroutine m_HealthDrainCoroutine;
        private Vector3 m_WalkOutTarget;

        // 자식 컴포넌트 (Awake에서 자동 탐색)
        private SkeletonAnimation m_SpineAnimator;
        private MeshRenderer m_SpineRenderer;

        // ================================================================
        //  Unity 생명 주기
        // ================================================================

        private void Awake()
        {
            m_SpineAnimator = GetComponentInChildren<SkeletonAnimation>();
            m_SpineRenderer = m_SpineAnimator != null
                ? m_SpineAnimator.GetComponent<MeshRenderer>()
                : null;
        }

        // ================================================================
        //  공개 API
        // ================================================================

        /// <summary>[HallManager 호출] 스폰 직후 초기화하고 Walkin을 시작합니다.</summary>
        /// <param name="table">배정된 테이블.</param>
        /// <param name="walkOutPos">퇴장 목표 위치.</param>
        /// <param name="sortingOrder">렌더링 우선순위 (테이블 인덱스 + 1).</param>
        public void Initialize(Table table, Vector3 walkOutPos, int sortingOrder)
        {
            m_AssignedTable = table;
            m_WalkOutTarget = walkOutPos;
            CurrentHealth = MaxHealth;
            m_HasEaten = false;
            ApplySortingOrder(sortingOrder);
            TransitionTo(CustomerState.Walkin);
        }

        /// <summary>[SideMenuMachine 호출] 체력을 amount만큼 회복합니다. Angry → Ordering 복귀를 포함합니다.</summary>
        public void HealHealth(int amount)
        {
            if (State != CustomerState.Ordering && State != CustomerState.Angry) return;

            CurrentHealth = Mathf.Min(CurrentHealth + amount, MaxHealth);
            UpdateHealthUI();

            // 충분히 회복되면 Angry 상태 해제 (드레인 루프는 유지)
            if (State == CustomerState.Angry && CurrentHealth > AngryThreshold)
                TransitionTo(CustomerState.Ordering);
        }

        /// <summary>[CandyBooster 전용] 하트를 끝까지 꽉 채웁니다.</summary>
        public void FullHealHealth()
        {
            if (State != CustomerState.Ordering && State != CustomerState.Angry) return;

            CurrentHealth = MaxHealth;
            UpdateHealthUI();

            if (State == CustomerState.Angry)
                TransitionTo(CustomerState.Ordering);
        }

        /// <summary>
        /// [Table 호출] 재료 토큰이 비행 중인 동안 체력 감소를 일시 정지합니다.
        /// ResumeHealthDrain()과 쌍으로 사용합니다.
        /// </summary>
        public void PauseHealthDrain() => StopHealthDrain();

        /// <summary>
        /// [Table 호출] 재료 토큰이 모두 도착한 후 체력 감소를 재개합니다.
        /// Ordering/Angry 상태일 때만 재시작합니다.
        /// </summary>
        public void ResumeHealthDrain()
        {
            if (State == CustomerState.Ordering || State == CustomerState.Angry)
                StartHealthDrainIfNeeded();
        }

        /// <summary>[Table 호출] 음식 완성 시 Eating 상태로 전환합니다.</summary>
        public void StartEating()
        {
            if (State != CustomerState.Ordering && State != CustomerState.Angry) return;
            StopHealthDrain();
            m_HasEaten = true;
            TransitionTo(CustomerState.Eating);
        }

        /// <summary>[Table 호출] 식사 완료 후 만족/불만족 퇴장을 시작합니다.</summary>
        public void FinishEating()
        {
            bool satisfied = m_HasEaten && CurrentHealth >= AngryThreshold;
            BeginWalkout(satisfied);
        }

        // ================================================================
        //  내부 상태 전환
        // ================================================================

        private void TransitionTo(CustomerState next)
        {
            State = next;

            // Ordering/Angry 상태에서만 체력 감소 활성화
            if (next == CustomerState.Ordering || next == CustomerState.Angry)
                StartHealthDrainIfNeeded();
            else
                StopHealthDrain();

            UpdateAnimator();
            UpdateHealthUI();
            PlayStateSound(next);

            if (next == CustomerState.Walkin)
                StartCoroutine(WalkinRoutine());
        }

        private void PlayStateSound(CustomerState state)
        {
            var gm = GameManager.Instance;
            if (gm == null) return;

            switch (state)
            {
                case CustomerState.Walkin:
                    gm.PlaySFX(SfxWalkin);
                    break;
                case CustomerState.Angry:
                    if (SfxAngry?.Length > 0)
                        gm.PlaySFXOneShot(SfxAngry[UnityEngine.Random.Range(0, SfxAngry.Length)]);
                    break;
                case CustomerState.Eating:
                    if (SfxEating?.Length > 0)
                        gm.PlaySFX(SfxEating[UnityEngine.Random.Range(0, SfxEating.Length)]);
                    break;
            }
        }

        // ──────────────────────────────────────────────────────────
        //  입장 이동 (Walkin)
        // ──────────────────────────────────────────────────────────

        private IEnumerator WalkinRoutine()
        {
            if (m_AssignedTable == null) yield break;
            var target = m_AssignedTable.GetSeatPosition();

            while (Vector3.Distance(transform.position, target) > 0.05f)
            {
                transform.position = Vector3.MoveTowards(
                    transform.position, target, WalkSpeed * Time.deltaTime);
                yield return null;
            }

            transform.position = target;
            TransitionTo(CustomerState.Ordering);
        }

        // ──────────────────────────────────────────────────────────
        //  체력 감소 루프 (Ordering / Angry)
        // ──────────────────────────────────────────────────────────

        private void StartHealthDrainIfNeeded()
        {
            if (m_HealthDrainCoroutine == null && CurrentHealth > 0)
                m_HealthDrainCoroutine = StartCoroutine(HealthDrainLoop());
        }

        private IEnumerator HealthDrainLoop()
        {
            while (CurrentHealth > 0 &&
                   (State == CustomerState.Ordering || State == CustomerState.Angry))
            {
                yield return new WaitForSeconds(HealthDecreaseInterval);

                // 대기 중 상태가 바뀌었을 수 있으므로 재확인
                if (State != CustomerState.Ordering && State != CustomerState.Angry) break;

                CurrentHealth = Mathf.Max(0, CurrentHealth - 1);
                UpdateHealthUI();

                // 임계값 이하 → Angry 전환
                if (CurrentHealth > 0 && CurrentHealth <= AngryThreshold &&
                    State == CustomerState.Ordering)
                {
                    TransitionTo(CustomerState.Angry);
                }

                // 체력 0 → 불만족 퇴장
                if (CurrentHealth <= 0)
                {
                    BeginWalkout(satisfied: false);
                    break;
                }
            }

            m_HealthDrainCoroutine = null;
        }

        private void StopHealthDrain()
        {
            if (m_HealthDrainCoroutine == null) return;
            StopCoroutine(m_HealthDrainCoroutine);
            m_HealthDrainCoroutine = null;
        }

        // ──────────────────────────────────────────────────────────
        //  퇴장 (Walkout)
        // ──────────────────────────────────────────────────────────

        private void BeginWalkout(bool satisfied)
        {
            StopHealthDrain();
            State = CustomerState.Walkout;
            UpdateAnimator();

            if (satisfied)
            {
                GameManager.Instance?.PlaySFXOneShot(SfxHappy);
                OnSatisfied?.Invoke(this);
            }
            else
            {
                m_AssignedTable?.OnCustomerLeft();
            }

            StartCoroutine(WalkoutRoutine());
        }

        private IEnumerator WalkoutRoutine()
        {
            var target = m_WalkOutTarget;

            while (Vector3.Distance(transform.position, target) > 0.05f)
            {
                transform.position = Vector3.MoveTowards(
                    transform.position, target, WalkOutSpeed * Time.deltaTime);
                yield return null;
            }

            OnWalkoutComplete?.Invoke(this);
            Destroy(gameObject);
        }

        // ================================================================
        //  Draw Order (렌더 우선순위)
        // ================================================================

        /// <summary>
        /// 테이블 번호 기반 sortingOrder를 Spine MeshRenderer와 모든 자식 SpriteRenderer에 적용합니다.
        /// Table_0 → 1, Table_1 → 2, Table_2 → 3 순으로 겹침 처리를 보장합니다.
        /// </summary>
        private void ApplySortingOrder(int order)
        {
            if (m_SpineRenderer != null)
                m_SpineRenderer.sortingOrder = order;

            // 체력 게이지 등 자식 SpriteRenderer는 기본 sortingOrder에 order*10 오프셋을 더합니다.
            // 이렇게 하면 캐릭터 몸체(order 1~N)보다 항상 위에 렌더링됩니다.
            foreach (var sr in GetComponentsInChildren<SpriteRenderer>())
                sr.sortingOrder += order * 10;
        }

        // ================================================================
        //  UI / 애니메이션 갱신
        // ================================================================

        private void UpdateHealthUI()
        {
            OnHealthChanged?.Invoke(CurrentHealth, MaxHealth);
        }

        private void UpdateAnimator()
        {
            if (m_SpineAnimator == null) return;

            string animName = State switch
            {
                CustomerState.Walkin => "walkin",
                CustomerState.Ordering => "order",
                CustomerState.Angry => "angry",
                CustomerState.Eating => "eat",
                CustomerState.Happy => "like",
                CustomerState.Walkout => "walkout",
                _ => null
            };

            if (animName != null)
                m_SpineAnimator.AnimationState.SetAnimation(0, animName, true);
        }
    }
}
