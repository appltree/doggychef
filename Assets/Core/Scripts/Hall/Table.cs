using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.InputSystem;

namespace MyMatch3
{
    // ════════════════════════════════════════════════════════════════
    //  Table — 식당 테이블의 상태 머신
    // ════════════════════════════════════════════════════════════════
    //
    //  ■ 역할
    //    손님 배정 → 주문 → 재료 수집 → 서빙 → 수금의 전체 사이클을 관리합니다.
    //
    //  ■ 상태 흐름 (TableState)
    //
    //  Empty ──────────────────────────────────────────────────────┐
    //    │ HallManager.SpawnCustomerForTable()                     │
    //    ▼                                                          │
    //  Ordered  ← Board 보석 파괴 시 재료가 AcceptGem()으로 누적됨  │
    //    │ IsOrderSatisfied() → true                               │
    //    ▼                                                          │
    //  Cooked   ← 음식이 준비됨 (0.5초 표시)                        │
    //    │ AutoServeAfterDelay() / 플레이어 탭                      │
    //    ▼                                                          │
    //  Served   ← 손님이 식사 중 (EatingDuration 대기)              │
    //    │ CompleteMeal()                                           │
    //    ▼                                                          │
    //  Paying   ← 코인 생성 → 탭(CollectMoney) 또는 5초 자동 수금    │
    //    └──────────────────────────────────────────────────────────┘
    //               ResetTable() → Empty
    //
    //  ■ 외부 연결
    //    HallManager : SpawnCustomerForTable(), SetOrder(), AcceptGem() 호출
    //    Board       : OnGemExploded → HallManager → AcceptGem() 전달
    //    Customer    : StartEating(), FinishEating() 연동
    //    LevelData   : OnMoneyCollected → AddMoney() 호출
    // ════════════════════════════════════════════════════════════════
    public class Table : MonoBehaviour
    {
        // ──────────────────────────────────────────────────────────
        //  상태 정의
        // ──────────────────────────────────────────────────────────

        public enum TableState { Empty, Ordered, Cooked, Served, Paying }

        // ──────────────────────────────────────────────────────────
        //  설정 상수
        // ──────────────────────────────────────────────────────────

        private const int MAX_COIN_PRICE = 3000;  // 코인 30개가 되는 기준 가격
        private const int MAX_COIN_COUNT = 30;    // 테이블에 표시되는 최대 코인 개수

        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────

        [Header("시각 피드백")]
        [Tooltip("손님이 이동해 앉을 좌석 위치.")]
        public Transform SeatPoint;

        [Tooltip("손님이 스폰·퇴장할 위치.")]
        public Transform CustomerSpawn;

        [Tooltip("재료 토큰의 비행 목표 위치 (테이블 중앙).")]
        public Transform IngredientTarget;

        [Tooltip("수금 코인이 생성될 위치.")]
        public Transform CoinSpawnPoint;

        [Header("수금 코인")]
        [Tooltip("CoinMoney 프리팹. 미연결 시 5초 후 자동 수금.")]
        public GameObject CoinMoneyPrefab;

        [Tooltip("테이블 상태를 색상으로 표시할 SpriteRenderer.")]
        public SpriteRenderer StatusRenderer;

        [Tooltip("Cooked 상태 색상.")]
        public Color CookedColor = Color.green;

        [Tooltip("기본 상태 색상 (Empty/Ordered/Served/Paying).")]
        public Color DefaultColor = Color.white;

        [Header("시간 설정")]
        [Tooltip("손님이 음식을 먹는 시간(초).")]
        public float EatingDuration = 3f;

        [Header("사이드 메뉴 (Machine 연동)")]
        [Tooltip("사이드 메뉴가 배달될 위치.")]
        public Transform SidePoint;

        [Tooltip("Machine Active 시 표시할 손 애니메이션 오브젝트.")]
        public GameObject HandAnimationObject;

        [Header("사운드")]
        [Tooltip("음식 완성 효과음 (반드시 재생)")]
        public AudioClip SfxCooked;

        [Tooltip("서빙 효과음")]
        public AudioClip SfxServed;

        [Tooltip("사이드 메뉴 도착 효과음 (side_served.mp3)")]
        public AudioClip SfxSideServed;

        // ──────────────────────────────────────────────────────────
        //  이벤트
        // ──────────────────────────────────────────────────────────

        /// <summary>Cooked 진입 시 발화 (현재 미사용, 향후 UI 확장용).</summary>
        public Action<Table> OnOrderComplete;

        /// <summary>수금 완료 시 발화. HallManager가 구독하여 LevelData.AddMoney()를 호출합니다.
        /// fromPos: 코인이 완전히 도착한 위치 (GoldGauge 코인 비행 시작점).
        /// 플레이어 탭 수금 시에는 코인 위치, Staff 수금 시에는 Staff 월드 좌표입니다.</summary>
        public Action<Table, int, Vector3> OnMoneyCollected;

        /// <summary>SetOrder() 완료 시 발화. TableOrderDisplay가 슬롯 그리드를 재구성합니다.</summary>
        public Action OnOrderSet;

        /// <summary>상태 변경 시 발화. UI 컴포넌트들이 표시/숨김을 처리합니다.</summary>
        public event Action<TableState> OnStateChanged;

        /// <summary>[StaffMember Cashier 구독] Paying 상태 진입 시 발화.</summary>
        public static event Action<Table> OnPayingAdded;

        /// <summary>[StaffMember Cashier 구독] Paying 상태 해제(수금 완료·만료) 시 발화.</summary>
        public static event Action<Table> OnPayingRemoved;

        /// <summary>
        /// AcceptGem() 성공 후 소크인 완료 시 발화.
        /// TableOrderDisplay가 해당 슬롯의 남은 수량을 갱신합니다.
        /// (tag, 남은 필요 수량)
        /// </summary>
        public Action<string, int> OnIngredientRemainingChanged;

        // ──────────────────────────────────────────────────────────
        //  런타임 상태 (읽기 전용)
        // ──────────────────────────────────────────────────────────

        public TableState State { get; private set; } = TableState.Empty;
        public RecipeData CurrentOrder { get; private set; }

        // ──────────────────────────────────────────────────────────
        //  내부 데이터
        // ──────────────────────────────────────────────────────────

        // 클릭 감지
        private Collider2D m_Collider;

        // 사이드 메뉴 상태 (두 번 이상 배달될 수 있으므로 List로 관리)
        private bool m_HasSideMenu = false;
        private List<GameObject> m_SideInstances = new();

        // 레시피 필요 수량. Key: BlockTag, Value: 필요 개수
        private Dictionary<string, int> m_Required = new();

        // 현재까지 수집된 수량. Key: BlockTag, Value: 수집 개수
        private Dictionary<string, int> m_Collected = new();

        private Customer m_AssignedCustomer;
        private Coroutine m_EatRoutine;
        private List<CoinMoney> m_SpawnedCoins = new();

        // Paying 상태 진입 시 캐시된 판매가. CurrentOrder가 null로 클리어된 후에도 안전하게 사용.
        private int m_CachedSellPrice;

        // 손 애니메이션 (Origin CoHandAnimation 방식)
        private SpriteRenderer m_HandRenderer;
        private Vector3 m_HandOriginalPos;
        private bool m_HandAnimationActive;
        private Coroutine m_HandRoutine;
        private readonly float m_HandAnimSpeed = 12f;
        private readonly Vector2 m_HandAnimOffset = new Vector2(0.5f, 0.5f);

        // 현재 비행 중인 재료 토큰 수.
        // > 0 이면 customer 체력 감소가 일시 정지 상태입니다.
        private int m_FlyingTokenCount = 0;

        // 현재 비행 웨이브에서 이미 재생한 foodSfx 목록.
        // 같은 클립이 여러 토큰에서 동시에 도착해도 종류당 한 번만 재생합니다.
        private readonly HashSet<AudioClip> m_PlayedFoodSfx = new();

        // ================================================================
        //  공개 API — HallManager·Customer 가 호출합니다
        // ================================================================

        /// <summary>[HallManager 호출] 손님을 배정합니다. StartEating() 등 연동에 사용됩니다.</summary>
        public void AssignCustomer(Customer customer)
        {
            m_AssignedCustomer = customer;
        }

        /// <summary>
        /// [HallManager 호출] 손님이 Ordering 상태가 된 직후 레시피를 배정합니다.
        /// m_Required/m_Collected를 초기화하고 Ordered 상태로 전환합니다.
        /// </summary>
        public void SetOrder(RecipeData recipe)
        {
            if (recipe == null) return;

            CurrentOrder = recipe;
            m_Required.Clear();
            m_Collected.Clear();

            foreach (var ing in recipe.Ingredients)
            {
                if (ing.MaterialGem == null) continue;
                string tag = ing.MaterialGem.BlockTag;
                m_Required[tag] = ing.Count;
                m_Collected[tag] = 0;
            }

            SetState(TableState.Ordered);
            OnOrderSet?.Invoke();
        }

        /// <summary>
        /// [HallManager 호출] 보석이 파괴되면 HallManager를 통해 이 메서드가 호출됩니다.
        ///
        /// 동작 흐름:
        ///   1. Ordered 상태 확인
        ///   2. gemTag가 현재 주문에 필요한 재료인지 확인
        ///   3. 아직 수량이 덜 찼는지 확인
        ///   4. 수집량 증가 → 비행 애니메이션 시작
        ///   5. 소크인 완료 후 레시피 충족 여부 판정
        /// </summary>
        /// <returns>재료가 접수되었으면 true.</returns>
        public bool AcceptGem(string gemTag, Vector3 fromWorldPos,
                              Sprite gemSprite = null, AudioClip foodSfx = null)
        {
            if (State != TableState.Ordered) return false;
            if (!m_Required.ContainsKey(gemTag)) return false;
            if (m_Collected[gemTag] >= m_Required[gemTag]) return false;

            m_Collected[gemTag]++;

            // 첫 토큰 비행 시작 시 손님 체력 감소를 일시 정지합니다.
            // (토큰이 도착하기 전에 Cooked로 전환되는 것을 방지합니다.)
            if (m_FlyingTokenCount == 0)
                m_AssignedCustomer?.PauseHealthDrain();
            m_FlyingTokenCount++;

            // 해당 재료의 슬롯 위치를 목표로 비행합니다 (없으면 IngredientTarget 폴백).
            var display = IngredientTarget?.GetComponent<TableIngredientDisplay>();
            var target = display?.GetSlotPosition(gemTag)
                ?? (IngredientTarget != null ? IngredientTarget.position : transform.position);

            StartCoroutine(FlyIngredient(fromWorldPos, target, gemSprite, foodSfx, gemTag));
            return true;
        }

        /// <summary>[플레이어 탭 → OnMouseDown] 상태에 따라 서빙 또는 사이드메뉴 배달을 처리합니다.</summary>
        public void OnTableClicked()
        {
            // Machine Active 상태에서 사이드메뉴 미수령 Ordered 테이블이면 사이드메뉴 배달
            if (State == TableState.Ordered && !m_HasSideMenu && m_AssignedCustomer != null)
            {
                Machine machine = FindFirstObjectByType<Machine>();
                if (machine != null && machine.IsActive())
                {
                    machine.OnTableTouchedForSide(this);
                    return;
                }
            }

            if (State == TableState.Cooked)
            {
                ServeToCustomer();
                return;
            }

            if (State == TableState.Paying)
                CollectMoney();
        }

        /// <summary>[코인 탭 또는 자동 수금] 수금하고 테이블을 Empty로 리셋합니다.
        /// fromPos: GoldGauge 코인 비행의 출발 월드 좌표. 기본값은 CoinSpawnPoint입니다.
        /// Staff 수금 시에는 Staff의 월드 위치를 전달합니다.</summary>
        public void CollectMoney(Vector3 fromPos = default)
        {
            if (State != TableState.Paying) return;

            int baseAmount = m_CachedSellPrice;
            int sideBonus = m_HasSideMenu ? 100 : 0;
            int totalAmount = baseAmount + sideBonus;

            // fromPos 미지정 시 CoinSpawnPoint를 기본값으로 사용 (플레이어 직접 수금)
            if (fromPos == default)
                fromPos = CoinSpawnPoint != null ? CoinSpawnPoint.position : transform.position;

            OnMoneyCollected?.Invoke(this, totalAmount, fromPos);

            bool isFever = LevelData.Instance != null && LevelData.Instance.IsFever;
            Vector3 popupPos = CoinSpawnPoint != null ? CoinSpawnPoint.position : transform.position;
            StartCoroutine(ShowGoldPopup(totalAmount, sideBonus, popupPos, isFever));

            ResetTable();
        }

        private IEnumerator ShowGoldPopup(int total, int sideBonus, Vector3 worldPos, bool isFever)
        {
            const float Duration = 1.0f;
            const float RiseDistance = 1.0f;

            var go = new GameObject("GoldPopup");
            var tmp = go.AddComponent<TextMeshPro>();
            tmp.fontSize = 3.5f;
            tmp.alignment = TextAlignmentOptions.Center;
            tmp.richText = true;
            // VFX 레이어에 배치하여 캐릭터/젬 위에 표시
            var renderer = go.GetComponent<Renderer>();
            if (renderer != null)
            {
                renderer.sortingLayerID = 666422525; // VFX
                renderer.sortingOrder = 10;
            }

            int displayed = isFever ? total * 2 : total;
            if (sideBonus > 0)
            {
                if (isFever)
                    tmp.text = $"<b>+{displayed:N0} <size=80%><color=#FFD700>x2</color></size> <size=70%><color=#00FF00>(+{sideBonus})</color></size></b>";
                else
                    tmp.text = $"+{displayed:N0} <size=80%><color=#00FF00>(+{sideBonus})</color></size>";
            }
            else
            {
                tmp.text = isFever
                    ? $"<b>+{displayed:N0} <size=80%><color=#FFD700>x2</color></size></b>"
                    : $"+{displayed:N0}";
            }

            tmp.color = new Color(1f, 0.9f, 0.2f);
            go.transform.position = worldPos;

            float elapsed = 0f;
            Vector3 startPos = worldPos;
            while (elapsed < Duration)
            {
                elapsed += Time.deltaTime;
                float t = elapsed / Duration;
                go.transform.position = startPos + Vector3.up * (RiseDistance * t);
                var c = tmp.color;
                c.a = 1f - t;
                tmp.color = c;
                yield return null;
            }

            Destroy(go);
        }

        /// <summary>[Customer 호출] 손님이 불만족으로 퇴장할 때 테이블을 강제 리셋합니다.</summary>
        public void OnCustomerLeft()
        {
            if (m_EatRoutine != null)
            {
                StopCoroutine(m_EatRoutine);
                m_EatRoutine = null;
            }
            m_AssignedCustomer = null;
            ResetTable();
        }

        // ──────────────────────────────────────────────────────────
        //  읽기 전용 공개 헬퍼
        // ──────────────────────────────────────────────────────────

        /// <summary>
        /// [HallManager 라우팅] 배정된 손님의 현재 체력을 반환합니다.
        /// 손님이 없으면 int.MaxValue (우선순위 최저)를 반환합니다.
        /// </summary>
        public int CustomerCurrentHealth =>
            m_AssignedCustomer != null ? m_AssignedCustomer.CurrentHealth : int.MaxValue;

        /// <summary>[CandyBooster 호출] 배정된 손님의 체력을 amount만큼 회복합니다.</summary>
        public void HealCustomer(int amount) => m_AssignedCustomer?.HealHealth(amount);

        /// <summary>[CandyBooster 호출] 배정된 손님의 체력을 꽉 채웁니다.</summary>
        public void FullHealCustomer() => m_AssignedCustomer?.FullHealHealth();

        /// <summary>[CandyBooster 호출] 배정된 손님을 반환합니다. 없으면 null.</summary>
        public Customer AssignedCustomer => m_AssignedCustomer;

        /// <summary>[Customer.WalkinRoutine] 손님이 이동해야 할 좌석 위치를 반환합니다.</summary>
        public Vector3 GetSeatPosition() =>
            SeatPoint != null ? SeatPoint.position : transform.position;

        /// <summary>
        /// [TableOrderDisplay / HallManager] 특정 재료 태그의 남은 필요 수량을 반환합니다.
        /// 주문이 없거나 해당 태그가 없으면 0을 반환합니다.
        /// </summary>
        public int GetRemainingNeed(string tag)
        {
            if (!m_Required.TryGetValue(tag, out int req)) return 0;
            if (!m_Collected.TryGetValue(tag, out int got)) return req;
            return Mathf.Max(0, req - got);
        }

        // ──────────────────────────────────────────────────────────
        //  Machine 연동 공개 API
        // ──────────────────────────────────────────────────────────

        public Transform GetSidePoint() => SidePoint;

        public bool GetHasSideMenu() => m_HasSideMenu;

        public void OnSideMenuDelivered(GameObject sideInstance)
        {
            m_HasSideMenu = true;
            m_SideInstances.Add(sideInstance);
            GameManager.Instance?.PlaySFX(SfxSideServed);
        }

        public void EnableHandAnimation(bool enable = true)
        {
            if (enable)
            {
                if (!m_HandAnimationActive)
                {
                    m_HandAnimationActive = true;
                    if (m_HandRenderer != null)
                        m_HandRenderer.enabled = true;
                    m_HandRoutine = StartCoroutine(CoHandAnimation());
                }
            }
            else
            {
                StopHandAnimation();
            }
        }

        public void DisableHandAnimation() => EnableHandAnimation(false);

        public bool IsHandAnimationActive() => m_HandAnimationActive;

        private void StopHandAnimation()
        {
            m_HandAnimationActive = false;
            if (m_HandRoutine != null)
            {
                StopCoroutine(m_HandRoutine);
                m_HandRoutine = null;
            }
            if (m_HandRenderer != null)
            {
                HandAnimationObject.transform.localPosition = m_HandOriginalPos;
                m_HandRenderer.enabled = false;
            }
        }

        private IEnumerator CoHandAnimation()
        {
            float time = 0f;
            Vector3 offset = new Vector3(m_HandAnimOffset.x, m_HandAnimOffset.y, 0f);
            while (m_HandAnimationActive && m_HandRenderer != null && isActiveAndEnabled)
            {
                time += Time.deltaTime * m_HandAnimSpeed;
                float progress = (Mathf.Sin(time) + 1f) * 0.5f;
                HandAnimationObject.transform.localPosition =
                    Vector3.Lerp(m_HandOriginalPos, m_HandOriginalPos + offset, progress);
                yield return null;
            }
            m_HandRoutine = null;
        }

        // ================================================================
        //  내부 상태 전환
        // ================================================================

        private void SetState(TableState next)
        {
            State = next;
            UpdateVisual();
            OnStateChanged?.Invoke(next);

            if (next == TableState.Cooked)
            {
                OnOrderComplete?.Invoke(this);
                GameManager.Instance?.PlaySFXOneShot(SfxCooked);
                DisableHandAnimation(); // Cooked: Machine이 켜둔 손 OFF (Paying 시 SpawnCoin 후 다시 ON)
            }
            else if (next == TableState.Served || next == TableState.Empty)
            {
                DisableHandAnimation(); // 서빙 중·빈 테이블 → 손 애니메이션 OFF
            }
            // Ordered 상태는 Machine.cs가 관리하므로 건드리지 않음
        }

        // ──────────────────────────────────────────────────────────
        //  서빙 흐름: Cooked → Served → Paying
        // ──────────────────────────────────────────────────────────

        private void ServeToCustomer()
        {
            SetState(TableState.Served);
            GameManager.Instance?.PlaySFX(SfxServed);
            m_AssignedCustomer?.StartEating();

            if (m_EatRoutine != null) StopCoroutine(m_EatRoutine);
            m_EatRoutine = StartCoroutine(WaitForMealComplete());
        }

        private IEnumerator WaitForMealComplete()
        {
            yield return new WaitForSeconds(EatingDuration);
            CompleteMeal();
        }

        private IEnumerator AutoServeAfterDelay(float delay)
        {
            yield return new WaitForSeconds(delay);
            if (State == TableState.Cooked)
                ServeToCustomer();
        }

        private void CompleteMeal()
        {
            m_AssignedCustomer?.FinishEating();

            // 손님이 퇴장하면 사이드 메뉴 오브젝트를 제거합니다.
            // m_HasSideMenu는 true 유지 → CollectMoney()에서 보너스 계산에 사용됩니다.
            foreach (var s in m_SideInstances) if (s != null) Destroy(s);
            m_SideInstances.Clear();

            SetState(TableState.Paying);
            SpawnCoin();
            EnableHandAnimation(); // 동전 생성 후 수금 유도 손 애니메이션
            OnPayingAdded?.Invoke(this); // Cashier 알림
        }

        // ──────────────────────────────────────────────────────────
        //  수금 처리
        // ──────────────────────────────────────────────────────────

        /// <summary>
        /// 수금 코인을 스폰합니다.
        /// CoinMoneyPrefab이 없으면 5초 후 자동 수금(폴백)으로 동작합니다.
        /// </summary>
        private void SpawnCoin()
        {
            if (CoinMoneyPrefab == null)
            {
                // 폴백: CoinMoneyPrefab이 연결되지 않은 경우
                StartCoroutine(AutoCollectAfterDelay(5f));
                return;
            }

            int price = CurrentOrder?.SellPrice ?? 0;
            m_CachedSellPrice = price; // CollectMoney()에서 CurrentOrder가 null이 되어도 안전하게 사용
            int count = Mathf.Clamp(
                Mathf.CeilToInt((float)price / MAX_COIN_PRICE * MAX_COIN_COUNT), 1, MAX_COIN_COUNT);
            Vector3 basePos = CoinSpawnPoint != null ? CoinSpawnPoint.position : transform.position;

            m_SpawnedCoins.Clear();

            for (int i = 0; i < count; i++)
            {
                var spawnPos = CalculateCoinPosition(basePos, i);
                var coinGo = Instantiate(CoinMoneyPrefab, spawnPos, Quaternion.identity);
                var coin = coinGo.GetComponent<CoinMoney>();

                if (coin == null) continue;

                // SortingOrder: 뒤쪽 줄(col=1)보다 앞쪽 줄이 항상 위에 오도록 설정
                int col = i % 3;
                int idxInCol = i / 3;
                if (coin.CoinRenderer != null)
                    coin.CoinRenderer.sortingOrder = (col == 1 ? 20 : 50) + idxInCol;

                coin.OnDone += (state, coinPos) =>
                {
                    CleanupSpawnedCoins();
                    if (state == CoinMoney.CoinState.Expired)
                    {
                        if (State == TableState.Paying) ResetTable();
                    }
                    else
                    {
                        // coinPos: 플레이어 탭 시 코인 위치, Staff 수금 시 Staff 위치
                        // → GoldGauge가 이 좌표에서 코인을 날립니다
                        CollectMoney(coinPos);
                    }
                };

                coin.Setup(price, this);
                m_SpawnedCoins.Add(coin);
            }
        }

        /// <summary>코인 스택 위치를 계산합니다. 3열 구조, 지그재그, 피라미드 오프셋 적용.</summary>
        private static Vector3 CalculateCoinPosition(Vector3 basePos, int i)
        {
            int col = i % 3;
            int idxInCol = i / 3;

            float x = (col - 1) * 0.28f;               // 3열 기본 간격
            float y = idxInCol * 0.06f;                 // 쌓기 높이

            if (col == 1) y += 0.12f;                 // 가운데 열은 뒤쪽으로 밀어 입체감 부여
            x += (idxInCol % 2 == 0) ? 0.03f : -0.03f; // 지그재그로 엄갈림

            return basePos + new Vector3(x, y, -i * 0.001f);  // Z: Z-fighting 방지
        }

        private void CleanupSpawnedCoins()
        {
            foreach (var coin in m_SpawnedCoins)
            {
                if (coin != null) Destroy(coin.gameObject);
            }
            m_SpawnedCoins.Clear();
        }

        private IEnumerator AutoCollectAfterDelay(float delay)
        {
            yield return new WaitForSeconds(delay);
            if (State == TableState.Paying)
                CollectMoney();
        }

        // ──────────────────────────────────────────────────────────
        //  테이블 리셋
        // ──────────────────────────────────────────────────────────

        private void ResetTable()
        {
            if (m_EatRoutine != null)
            {
                StopCoroutine(m_EatRoutine);
                m_EatRoutine = null;
            }

            CurrentOrder = null;
            m_CachedSellPrice = 0;
            m_AssignedCustomer = null;
            m_FlyingTokenCount = 0;
            m_Required.Clear();
            m_Collected.Clear();
            m_PlayedFoodSfx.Clear();

            // 사이드 메뉴 초기화
            m_HasSideMenu = false;
            foreach (var s in m_SideInstances) if (s != null) Destroy(s);
            m_SideInstances.Clear();
            DisableHandAnimation();
            CleanupSpawnedCoins();

            // Paying → Empty 전환 시 Cashier에 알림
            if (State == TableState.Paying)
                OnPayingRemoved?.Invoke(this);

            SetState(TableState.Empty);
        }

        // ──────────────────────────────────────────────────────────
        //  헬퍼
        // ──────────────────────────────────────────────────────────

        private bool IsOrderSatisfied()
        {
            foreach (var kvp in m_Required)
            {
                if (!m_Collected.TryGetValue(kvp.Key, out int got) || got < kvp.Value)
                    return false;
            }
            return true;
        }

        private void UpdateVisual()
        {
            if (StatusRenderer == null) return;
            StatusRenderer.color = State == TableState.Cooked ? CookedColor : DefaultColor;
        }

        // ──────────────────────────────────────────────────────────
        //  FlyIngredient 코루틴 (Origin FlyTokenRoutine 방식)
        // ──────────────────────────────────────────────────────────
        //
        //  [1] 포물선 비행 (flyDuration=0.6s, arcHeight=1.0)
        //  [2] 도착 후 잠시 정지 (pauseBeforeSoak)
        //  [3] 스케일 1→0 수축 — "스며드는" 소크인 연출
        //  [4] 소크인 완료: 슬롯 수량 이벤트 발화 → 드레인 재개 → Cooked 판정
        // ──────────────────────────────────────────────────────────

        private IEnumerator FlyIngredient(Vector3 from, Vector3 to,
                                          Sprite sprite, AudioClip foodSfx, string gemTag)
        {
            const float FlyDuration = 0.6f;
            const float ArcHeight = 1.0f;
            const float PauseBeforeSoak = 0.12f;
            const float SoakDuration = 0.22f;   // 일반 토큰
            const float SoakDurationCompleting = 0.65f; // 마지막 토큰 (ingredient SFX 대기)

            // ── [1] 토큰 스프라이트 생성 ──
            GameObject token = null;
            if (sprite != null)
            {
                token = new GameObject("IngredientFlyToken");
                var sr = token.AddComponent<SpriteRenderer>();
                sr.sprite = sprite;
                sr.sortingLayerID = 666422525;  // VFX 레이어
                sr.sortingOrder = 0;
                token.transform.position = from;
            }

            // ── [2] 포물선 비행 ──
            float t = 0f;
            while (t < 1f)
            {
                t += Time.deltaTime / FlyDuration;
                float tt = Mathf.Clamp01(t);
                var pos = Vector3.Lerp(from, to, tt);
                pos.y += Mathf.Sin(tt * Mathf.PI) * ArcHeight;

                if (token != null)
                {
                    token.transform.position = pos;
                    token.transform.localScale = Vector3.one;
                }
                yield return null;
            }

            if (token != null) token.transform.position = to;

            // 도착 효과음 — 같은 클립은 이 웨이브에서 한 번만 재생합니다.
            if (foodSfx != null && m_PlayedFoodSfx.Add(foodSfx))
                GameManager.Instance?.PlaySFX(foodSfx);

            // 마지막 재료 토큰이면 소크인을 길게 하여 SFX가 끝난 후 Cooked로 전환합니다.
            float activeSoak = IsOrderSatisfied() ? SoakDurationCompleting : SoakDuration;

            // ── [3] 정지 후 소크인 ──
            yield return new WaitForSeconds(PauseBeforeSoak);

            float elapsed = 0f;
            while (elapsed < activeSoak)
            {
                elapsed += Time.deltaTime;
                float s = Mathf.Lerp(1f, 0f, elapsed / activeSoak);
                if (token != null) token.transform.localScale = Vector3.one * s;
                yield return null;
            }

            if (token != null) Destroy(token);

            // ── [4] 소크인 완료 처리 ──

            // 슬롯 수량 갱신 이벤트 발화
            OnIngredientRemainingChanged?.Invoke(gemTag, GetRemainingNeed(gemTag));

            // 비행 카운터 감소. 모두 도착하면 손님 체력 감소 재개.
            m_FlyingTokenCount = Mathf.Max(0, m_FlyingTokenCount - 1);
            if (m_FlyingTokenCount == 0)
            {
                m_PlayedFoodSfx.Clear();
                m_AssignedCustomer?.ResumeHealthDrain();
            }

            // 레시피 완성 판정 → Cooked 전환
            // m_FlyingTokenCount == 0 조건: 모든 토큰이 착지한 후에만 전환하여
            // 아직 비행 중인 토큰이 이미 완성된 슬롯으로 날아가는 시각적 버그 방지
            if (IsOrderSatisfied() && State == TableState.Ordered && m_FlyingTokenCount == 0)
            {
                SetState(TableState.Cooked);
                StartCoroutine(AutoServeAfterDelay(0.5f));
            }
        }

        // ──────────────────────────────────────────────────────────
        //  Unity 클릭 감지 (New Input System — Canvas/UIDocument 우회)
        // ──────────────────────────────────────────────────────────

        void Awake()
        {
            m_Collider = GetComponent<Collider2D>();

            if (HandAnimationObject != null)
            {
                m_HandRenderer = HandAnimationObject.GetComponent<SpriteRenderer>();
                m_HandOriginalPos = HandAnimationObject.transform.localPosition;
                if (m_HandRenderer != null)
                    m_HandRenderer.enabled = false;
            }
        }

        void Update()
        {
            var pointer = Pointer.current;
            if (pointer == null || !pointer.press.wasPressedThisFrame) return;
            if (m_Collider == null) return;
            // Machine이 같은 프레임에 탭을 이미 처리했으면 무시
            if (Machine.IsPointerConsumedThisFrame()) return;

            Vector2 worldPos = Camera.main.ScreenToWorldPoint(pointer.position.ReadValue());
            if (m_Collider.OverlapPoint(worldPos))
                OnTableClicked();
        }
    }
}
