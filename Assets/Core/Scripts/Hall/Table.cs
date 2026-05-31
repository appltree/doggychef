using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.InputSystem;

namespace DoggyChef
{
    public class Table : MonoBehaviour
    {

        public enum TableState { Empty, Ordered, Cooked, Served, Paying }

        private const int MAX_COIN_PRICE = 3000;
        private const int MAX_COIN_COUNT = 30;
        // SortingLayer "VFX"의 GUID 기반 ID (Unity Editor에서 생성된 값)
        private const int VfxSortingLayerID = 666422525;

        [Header("Scene References")]
        [Tooltip("Customer seat position.")]
        public Transform SeatPoint;

        [Tooltip("Customer spawn position.")]
        public Transform CustomerSpawn;

        [Tooltip("Ingredient token flight target.")]
        public Transform IngredientTarget;

        [Tooltip("Coin spawn position.")]
        public Transform CoinSpawnPoint;

        [Header("Payment")]
        [Tooltip("If unset, payment is collected automatically after a delay.")]
        public GameObject CoinMoneyPrefab;

        [Tooltip("Renderer used to show table state.")]
        public SpriteRenderer StatusRenderer;

        [Tooltip("Color used when the order is ready.")]
        public Color CookedColor = Color.green;

        [Tooltip("Default table color.")]
        public Color DefaultColor = Color.white;

        [Header("Timing")]
        [Tooltip("Seconds spent eating.")]
        public float EatingDuration = 3f;

        [Header("Side Menu")]
        [Tooltip("Side menu delivery position.")]
        public Transform SidePoint;

        [Tooltip("Hand animation object shown while machine interaction is available.")]
        public GameObject HandAnimationObject;

        [Header("Sound")]
        [Tooltip("Played when an order becomes cooked.")]
        public AudioClip SfxCooked;

        [Tooltip("Played when an order is served.")]
        public AudioClip SfxServed;

        [Tooltip("Played when a side menu is delivered.")]
        public AudioClip SfxSideServed;

        public Action<Table> OnOrderComplete;

        public Action<Table, int, Vector3> OnMoneyCollected;

        public Action OnOrderSet;

        public event Action<TableState> OnStateChanged;

        public static event Action<Table> OnPayingAdded;

        public static event Action<Table> OnPayingRemoved;
        public Action<string, int> OnIngredientRemainingChanged;

        public TableState State { get; private set; } = TableState.Empty;
        public RecipeData CurrentOrder { get; private set; }

        private Collider2D m_Collider;

        private bool m_HasSideMenu = false;
        private readonly List<GameObject> m_SideInstances = new();

        private readonly Dictionary<string, int> m_Required = new();

        private readonly Dictionary<string, int> m_Collected = new();

        private Customer m_AssignedCustomer;
        private Coroutine m_EatRoutine;
        private readonly List<CoinMoney> m_SpawnedCoins = new();

        private int m_CachedSellPrice;

        private SpriteRenderer m_HandRenderer;
        private Vector3 m_HandOriginalPos;
        private bool m_HandAnimationActive;
        private Coroutine m_HandRoutine;
        private readonly float m_HandAnimSpeed = 12f;
        private readonly Vector2 m_HandAnimOffset = new(0.5f, 0.5f);

        private int m_FlyingTokenCount = 0;

        private readonly HashSet<AudioClip> m_PlayedFoodSfx = new();

        // [HallManager 호출] 이 테이블에 손님을 배정합니다.
        public void AssignCustomer(Customer customer)
        {
            m_AssignedCustomer = customer;
        }

        // [HallManager 호출] 레시피를 배정하고 Ordered 상태로 전환합니다.
        public void SetOrder(RecipeData recipe)
        {
            if (recipe == null) return;

            CurrentOrder = recipe;
            ResetIngredientProgress(recipe);

            SetState(TableState.Ordered);
            OnOrderSet?.Invoke();
        }

        // [HallManager 호출] 재료를 수령하고 토큰을 슬롯으로 날립니다.
        public bool AcceptGem(string gemTag, Vector3 fromWorldPos,
                              Sprite gemSprite = null, AudioClip foodSfx = null)
        {
            if (!TryCollectIngredient(gemTag))
                return false;

            if (m_FlyingTokenCount == 0)
                m_AssignedCustomer?.PauseHealthDrain();
            m_FlyingTokenCount++;

            var display = IngredientTarget?.GetComponent<TableIngredientDisplay>();
            var target = display?.GetSlotPosition(gemTag)
                ?? (IngredientTarget != null ? IngredientTarget.position : transform.position);

            StartCoroutine(FlyIngredient(fromWorldPos, target, gemSprite, foodSfx, gemTag));
            return true;
        }

        // 플레이어 탭 시 상태에 따라 서빙·수금·사이드메뉴 중 하나를 수행합니다.
        public void OnTableClicked()
        {
            if (State == TableState.Ordered && !m_HasSideMenu && m_AssignedCustomer != null)
            {
                var machine = Machine.Instance;
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

        // 코인을 수거하고 OnMoneyCollected를 발화합니다. fromPos는 GoldGauge 비행 출발점입니다.
        public void CollectMoney(Vector3 fromPos = default)
        {
            if (State != TableState.Paying) return;

            int baseAmount = m_CachedSellPrice;
            int sideBonus = m_HasSideMenu ? 100 : 0;
            int totalAmount = baseAmount + sideBonus;

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
            var renderer = go.GetComponent<Renderer>();
            if (renderer != null)
            {
                renderer.sortingLayerID = VfxSortingLayerID; // VFX
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

            tmp.color = new(1f, 0.9f, 0.2f);
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

        public int CustomerCurrentHealth =>
            m_AssignedCustomer != null ? m_AssignedCustomer.CurrentHealth : int.MaxValue;

        public void HealCustomer(int amount) => m_AssignedCustomer?.HealHealth(amount);

        public void FullHealCustomer() => m_AssignedCustomer?.FullHealHealth();

        public Customer AssignedCustomer => m_AssignedCustomer;

        public Vector3 GetSeatPosition() =>
            SeatPoint != null ? SeatPoint.position : transform.position;
        // 지정 재료 태그의 남은 필요 수량을 반환합니다.
        public int GetRemainingNeed(string tag)
        {
            if (!m_Required.TryGetValue(tag, out int req)) return 0;
            if (!m_Collected.TryGetValue(tag, out int got)) return req;
            return Mathf.Max(0, req - got);
        }

        public Transform GetSidePoint() => SidePoint;

        public bool HasSideMenu => m_HasSideMenu;

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

        public bool IsHandAnimationActive => m_HandAnimationActive;

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
            Vector3 offset = new(m_HandAnimOffset.x, m_HandAnimOffset.y, 0f);
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

        // 테이블 상태를 변경하고 비주얼·사운드·이벤트를 갱신합니다.
        private void SetState(TableState next)
        {
            if (State == next) return;
            State = next;
            UpdateVisual();
            OnStateChanged?.Invoke(next);

            if (next == TableState.Cooked)
            {
                OnOrderComplete?.Invoke(this);
                GameManager.Instance?.PlaySFXOneShot(SfxCooked);
                DisableHandAnimation();
            }
            else if (next == TableState.Served || next == TableState.Empty)
            {
                DisableHandAnimation();
            }
        }

        // Served 상태로 전환하고 손님 식사를 시작합니다.
        private void ServeToCustomer()
        {
            SetState(TableState.Served);
            GameManager.Instance?.PlaySFX(SfxServed);
            m_AssignedCustomer?.StartEating();

            if (m_EatRoutine != null) StopCoroutine(m_EatRoutine);
            m_EatRoutine = StartCoroutine(WaitForMealComplete());
        }

        // EatingDuration 후 CompleteMeal()을 호출합니다.
        private IEnumerator WaitForMealComplete()
        {
            yield return new WaitForSeconds(EatingDuration);
            CompleteMeal();
        }

        // delay 초 후 자동 서빙합니다 (주문 완성 즉시 자동 제공 시 사용).
        private IEnumerator AutoServeAfterDelay(float delay)
        {
            yield return new WaitForSeconds(delay);
            if (State == TableState.Cooked)
                ServeToCustomer();
        }

        // 식사 완료 처리 — FinishEating, Paying 상태 전환, 코인 스폰을 순서대로 수행합니다.
        private void CompleteMeal()
        {
            m_AssignedCustomer?.FinishEating();

            foreach (var s in m_SideInstances) if (s != null) Destroy(s);
            m_SideInstances.Clear();

            SetState(TableState.Paying);
            SpawnCoin();
            EnableHandAnimation();
            OnPayingAdded?.Invoke(this);
        }

        // Paying 상태 진입 시 CoinMoney 프리팹을 스폰합니다.
        private void SpawnCoin()
        {
            if (CoinMoneyPrefab == null)
            {
                StartCoroutine(AutoCollectAfterDelay(5f));
                return;
            }

            int price = CurrentOrder?.SellPrice ?? 0;
            m_CachedSellPrice = price;
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
                        CollectMoney(coinPos);
                    }
                };

                coin.Setup(price, this);
                m_SpawnedCoins.Add(coin);
            }
        }

        private static Vector3 CalculateCoinPosition(Vector3 basePos, int i)
        {
            int col = i % 3;
            int idxInCol = i / 3;

            float x = (col - 1) * 0.28f;
            float y = idxInCol * 0.06f;

            if (col == 1) y += 0.12f;
            x += (idxInCol % 2 == 0) ? 0.03f : -0.03f;

            return basePos + new Vector3(x, y, -i * 0.001f);
        }

        private void CleanupSpawnedCoins()
        {
            foreach (var coin in m_SpawnedCoins)
            {
                if (coin != null) Destroy(coin.gameObject);
            }
            m_SpawnedCoins.Clear();
        }

        // CoinMoneyPrefab 미설정 시 delay 초 후 자동 수금합니다.
        private IEnumerator AutoCollectAfterDelay(float delay)
        {
            yield return new WaitForSeconds(delay);
            if (State == TableState.Paying)
                CollectMoney();
        }

        // 테이블을 Empty 상태로 초기화합니다.
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

            m_HasSideMenu = false;
            foreach (var s in m_SideInstances) if (s != null) Destroy(s);
            m_SideInstances.Clear();
            DisableHandAnimation();
            CleanupSpawnedCoins();

            if (State == TableState.Paying)
                OnPayingRemoved?.Invoke(this);

            SetState(TableState.Empty);
        }

        // 주문 레시피 기반으로 Required/Collected 딕셔너리를 초기화합니다.
        private void ResetIngredientProgress(RecipeData recipe)
        {
            m_Required.Clear();
            m_Collected.Clear();

            foreach (var ingredient in recipe.Ingredients)
            {
                if (ingredient.MaterialGem == null) continue;

                string tag = ingredient.MaterialGem.GemTag;
                m_Required[tag] = ingredient.Count;
                m_Collected[tag] = 0;
            }
        }

        // 재료를 수집합니다. 이미 충족된 재료나 잘못된 태그면 false를 반환합니다.
        private bool TryCollectIngredient(string gemTag)
        {
            if (State != TableState.Ordered) return false;
            if (!m_Required.TryGetValue(gemTag, out int required)) return false;

            int collected = m_Collected.TryGetValue(gemTag, out int current) ? current : 0;
            if (collected >= required) return false;

            m_Collected[gemTag] = collected + 1;
            return true;
        }

        // 모든 재료가 충족됐는지 확인합니다.
        private bool IsOrderSatisfied()
        {
            foreach (var kvp in m_Required)
            {
                if (!m_Collected.TryGetValue(kvp.Key, out int got) || got < kvp.Value)
                    return false;
            }
            return true;
        }

        // 현재 상태에 맞게 테이블 색상을 갱신합니다.
        private void UpdateVisual()
        {
            if (StatusRenderer == null) return;
            StatusRenderer.color = State == TableState.Cooked ? CookedColor : DefaultColor;
        }

        //

        private IEnumerator FlyIngredient(Vector3 from, Vector3 to,
                                          Sprite sprite, AudioClip foodSfx, string gemTag)
        {
            const float FlyDuration = 0.6f;
            const float ArcHeight = 1.0f;
            const float PauseBeforeSoak = 0.12f;
            const float SoakDuration = 0.22f;
            const float SoakDurationCompleting = 0.65f;

            GameObject token = null;
            if (sprite != null)
            {
                token = new GameObject("IngredientFlyToken");
                var sr = token.AddComponent<SpriteRenderer>();
                sr.sprite = sprite;
                sr.sortingLayerID = VfxSortingLayerID;
                sr.sortingOrder = 0;
                token.transform.position = from;
            }

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

            if (foodSfx != null && m_PlayedFoodSfx.Add(foodSfx))
                GameManager.Instance?.PlaySFX(foodSfx);

            float activeSoak = IsOrderSatisfied() ? SoakDurationCompleting : SoakDuration;

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

            OnIngredientRemainingChanged?.Invoke(gemTag, GetRemainingNeed(gemTag));

            m_FlyingTokenCount = Mathf.Max(0, m_FlyingTokenCount - 1);
            if (m_FlyingTokenCount == 0)
            {
                m_PlayedFoodSfx.Clear();
                m_AssignedCustomer?.ResumeHealthDrain();
            }

            if (IsOrderSatisfied() && State == TableState.Ordered && m_FlyingTokenCount == 0)
            {
                SetState(TableState.Cooked);
                StartCoroutine(AutoServeAfterDelay(0.5f));
            }
        }

        private void Awake()
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

        private void Update()
        {
            var pointer = Pointer.current;
            if (pointer == null || !pointer.press.wasPressedThisFrame) return;
            if (m_Collider == null) return;
            if (Machine.IsPointerConsumedThisFrame()) return;

            Vector2 worldPos = Camera.main.ScreenToWorldPoint(pointer.position.ReadValue());
            if (m_Collider.OverlapPoint(worldPos))
                OnTableClicked();
        }
    }
}
