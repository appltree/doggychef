using System;
using System.Collections;
using UnityEngine;

namespace DoggyChef
{
    // ════════════════════════════════════════════════════════════════
    //  CoinMoney — 수금 코인 오브젝트
    // ════════════════════════════════════════════════════════════════
    //
    //  ■ 역할
    //  식사 완료 후 테이블 위에 스폰되는 코인입니다.
    //  플레이어가 탭(OnMouseDown)하거나 일정 시간 후 자동 처리됩니다.
    //  Origin의 PaiedMoney.cs를 MyGame 구조에 맞게 단순화했습니다.
    //
    //  ■ 생명주기
    //    Setup() 호출
    //      │
    //      ▼
    //    Idle (10초 대기) ── 이 시간 내 탭하면 즉시 수거
    //      │
    //      ▼
    //    Blink (3초 깜빡임) ── 깜빡이는 동안 탭하면 수거
    //      │
    //      ▼
    //    Expired ── 미수거 → OnDone(Expired) 발사 → 골드 없이 테이블 리셋
    //
    //  ■ 수거(Collect) 조건
    //    BoxCollider2D가 붙어 있으면 OnMouseDown 자동 호출 → Collect()
    //    Idle 대기 중에도 탭하면 즉시 수거됩니다.
    //
    //  ■ 이벤트
    //    OnDone : 수거 또는 만료 시 발사되는 단일 결과 이벤트.
    //             state == Idle    → Idle 중 탭 수거. 구독자는 CollectMoney() 호출.
    //             state == Blink   → Blink 중 탭 수거. 구독자는 CollectMoney() 호출.
    //             state == Expired → 시간 초과 미수거. 구독자는 골드 없이 ResetTable() 호출.
    //
    //  ■ Inspector 설정
    //    CoinRenderer  → 코인 스프라이트를 표시할 SpriteRenderer
    //    BlinkInterval → 깜빡임 간격(초). 기본 0.15초
    // ════════════════════════════════════════════════════════════════
    public class CoinMoney : MonoBehaviour
    {
        // ──────────────────────────────────────────────────────────
        //  상태 정의
        // ──────────────────────────────────────────────────────────
        //
        //  Idle    : Setup() 직후. IdleTime 동안 플레이어의 탭을 기다립니다.
        //  Blink   : Idle 대기가 끝난 후. BlinkTime 동안 깜빡이며 탭을 재촉합니다.
        //  Expired : Blink도 지나 수거되지 않은 상태. 만료 이벤트 발사 후 파괴됩니다.
        //
        //  Collect()가 호출되면 StopCoroutine으로 루틴이 즉시 중단되므로,
        //  수거 시점의 State(Idle 또는 Blink)가 OnDone 파라미터로 그대로 전달됩니다.
        //  만료 시에만 State가 Expired로 전이됩니다.
        //
        public enum CoinState { Idle, Blink, Expired }

        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────

        [Header("시각")]
        [Tooltip("코인 이미지를 표시하는 SpriteRenderer. 없어도 동작하지만 코인이 보이지 않습니다.")]
        public SpriteRenderer CoinRenderer;

        [Header("타이밍")]
        [Tooltip("깜빡임 간격(초). 기본값 0.15.")]
        public float BlinkInterval = 0.15f;

        [Header("사운드")]
        [Tooltip("플레이어가 코인을 탭할 때 효과음 (반드시 재생)")]
        public AudioClip SfxCollect;

        // ──────────────────────────────────────────────────────────
        //  이벤트
        // ──────────────────────────────────────────────────────────

        // 수거(Idle/Blink) 또는 만료(Expired) 시 발사되는 단일 결과 이벤트.
        //
        //  파라미터 1 (CoinState): 코인이 종료될 때의 상태
        //    Idle    → Idle 중에 탭 수거됨
        //    Blink   → 깜빡임 중에 탭 수거됨
        //    Expired → 시간 초과, 골드 지급 없음
        //
        //  파라미터 2 (Vector3): 이벤트 발사 시점의 코인 월드 좌표
        //    수거(Collected) 시 → 동전 날아가는 VFX의 출발 위치로 사용합니다.
        //    만료(Expired) 시   → 위치는 전달되지만 무시해도 됩니다.
        //
        //  구독자(Table)는 state == Expired 여부로 수거/만료를 구분합니다.
        //  OnCollected + OnExpired 두 이벤트를 이 하나로 통합했습니다.
        //
        public event Action<CoinState, Vector3> OnDone;

        // ──────────────────────────────────────────────────────────
        //  생명주기 시간 상수
        // ──────────────────────────────────────────────────────────

        // Idle 대기 시간. 이 시간 동안 플레이어의 탭을 기다립니다. (Origin: 10초)
        private const float IdleTime = 10f;

        // Blink 지속 시간. 깜빡임 후 만료 처리됩니다. (Origin: 3초)
        private const float BlinkTime = 3f;

        // ──────────────────────────────────────────────────────────
        //  런타임 상태
        // ──────────────────────────────────────────────────────────

        // 현재 생명주기 상태.
        // Collect()가 호출될 때 이 값이 OnDone 파라미터로 전달됩니다.
        public CoinState State { get; private set; } = CoinState.Idle;

        // 이 코인을 소유한 테이블 (StaffMember Cashier가 Owner로 코인을 검색합니다).
        public Table Owner { get; private set; }

        // 이미 수거됐으면 true. 중복 수거를 방지합니다.
        private bool m_Collected;

        // 생명주기 Coroutine 참조. Collect() 시 StopCoroutine으로 즉시 중단합니다.
        private Coroutine m_LifeRoutine;

        // ================================================================
        //  공개 API
        // ================================================================

        /// <summary>
        /// [Table 호출]
        /// 코인을 초기화하고 생명주기 루틴을 시작합니다.
        /// Table.SpawnCoin()에서 Instantiate 직후 호출합니다.
        ///
        /// 파라미터:
        ///   amount → 수거 시 지급할 골드 (RecipeData.SellPrice).
        ///            현재 Table이 CurrentOrder.SellPrice를 직접 읽으므로
        ///            CoinMoney 내부에서는 저장하지 않습니다.
        ///            향후 금액 표시 VFX 등이 필요하면 필드로 승격하세요.
        /// </summary>
        public void Setup(int amount, Table owner = null)
        {
            Owner = owner;

            // amount는 현재 미사용 (Table이 SellPrice를 직접 읽음)
            _ = amount;

            m_Collected = false;
            State = CoinState.Idle;

            // 혹시 이전 루틴이 남아 있으면 정리 후 재시작
            if (m_LifeRoutine != null) StopCoroutine(m_LifeRoutine);
            m_LifeRoutine = StartCoroutine(LifeCycleRoutine());
        }

        /// <summary>
        /// [플레이어 탭 또는 OnMouseDown에서 호출]
        /// 코인을 수거합니다. OnDone(현재 State)을 발사하고 GameObject를 파괴합니다.
        /// 이미 수거된 경우에는 아무 동작도 하지 않습니다.
        /// </summary>
        public void Collect()
        {
            if (m_Collected) return;
            m_Collected = true;

            // StopCoroutine이 루틴을 즉시 중단하므로,
            // 루틴 내부에서 별도로 m_Collected를 체크할 필요가 없습니다.
            if (m_LifeRoutine != null) StopCoroutine(m_LifeRoutine);

            GameManager.Instance?.PlaySFXOneShot(SfxCollect);

            // 수거 시점의 상태(Idle 또는 Blink)를 그대로 전달합니다.
            // 구독자는 state != Expired 이면 수거로 판단합니다.
            OnDone?.Invoke(State, transform.position);
            Destroy(gameObject);
        }

        /// <summary>
        /// [StaffMember Cashier 호출] 코인을 Staff 위치로 이동 후 수거합니다.
        /// Origin PaiedMoney.CollectViaStaff()와 동일한 방식입니다.
        /// </summary>
        public void CollectViaStaff(Vector3 destPos, float travelDuration = 0.35f)
        {
            if (m_Collected) return;
            m_Collected = true;

            if (m_LifeRoutine != null) StopCoroutine(m_LifeRoutine);
            if (CoinRenderer != null) CoinRenderer.enabled = true; // 깜빡임 중단 원복

            StartCoroutine(CoCollectViaStaff(destPos, travelDuration));
        }

        private IEnumerator CoCollectViaStaff(Vector3 targetPos, float duration)
        {
            // 이동 전 스케일을 60%로 축소 (Origin과 동일)
            Vector3 startScale = transform.localScale;
            Vector3 midScale = startScale * 0.6f;
            float scaleT = 0f;
            float scaleDur = Mathf.Min(0.15f, duration * 0.4f);
            while (scaleT < 1f)
            {
                scaleT += Time.deltaTime / Mathf.Max(0.01f, scaleDur);
                float k = Mathf.SmoothStep(0f, 1f, Mathf.Clamp01(scaleT));
                transform.localScale = Vector3.Lerp(startScale, midScale, k);
                yield return null;
            }

            // Staff 위치로 부드럽게 이동
            Vector3 startPos = transform.position;
            float t = 0f;
            while (t < 1f)
            {
                t += Time.deltaTime / Mathf.Max(0.01f, duration);
                float k = Mathf.SmoothStep(0f, 1f, Mathf.Clamp01(t));
                transform.position = Vector3.Lerp(startPos, targetPos, k);
                yield return null;
            }
            transform.position = targetPos;

            // 수거 이벤트 발화 → Table.CollectMoney() 호출됨
            GameManager.Instance?.PlaySFXOneShot(SfxCollect);
            OnDone?.Invoke(State, targetPos);
            Destroy(gameObject);
        }

        // ================================================================
        //  내부 생명주기
        // ================================================================

        // Idle → Blink → Expired 순서로 진행됩니다.
        // Collect()가 호출되면 StopCoroutine에 의해 즉시 중단되므로,
        // 루틴이 만료(Expired) 단계까지 실행됐다면 m_Collected는 반드시 false입니다.
        private IEnumerator LifeCycleRoutine()
        {
            // ── 1단계: Idle 대기 ──
            // WaitForSeconds로 단순 대기합니다.
            // 이전 구현의 frame-by-frame 루프보다 CPU 비용이 적습니다.
            State = CoinState.Idle;
            yield return new WaitForSeconds(IdleTime);

            // ── 2단계: Blink 깜빡임 ──
            // BlinkTime 동안 코인을 깜빡여 플레이어에게 수거를 독촉합니다.
            State = CoinState.Blink;
            bool visible = true;
            float blinkTimer = 0f;
            while (blinkTimer < BlinkTime)
            {
                visible = !visible;
                SetVisible(visible);
                yield return new WaitForSeconds(BlinkInterval);
                blinkTimer += BlinkInterval;
            }

            // ── 3단계: 만료 처리 ──
            // 루틴이 여기까지 도달했다면 수거되지 않은 상태입니다.
            State = CoinState.Expired;
            SetVisible(true);                    // 깜빡임 상태 원복
            OnDone?.Invoke(CoinState.Expired, transform.position);   // 테이블에 만료 알림 → ResetTable()
            Destroy(gameObject);
        }

        // CoinRenderer의 표시 여부를 전환합니다.
        // Blink 단계에서 BlinkInterval마다 호출됩니다.
        private void SetVisible(bool on)
        {
            if (CoinRenderer != null) CoinRenderer.enabled = on;
        }

        // ================================================================
        //  유니티 클릭 감지
        // ================================================================

        // BoxCollider2D가 붙어 있으면 마우스/터치 클릭 시 자동으로 호출됩니다.
        private void OnMouseDown() => Collect();
    }
}
