using UnityEngine;

namespace DoggyChef
{
    // 스테이지 진행·재화·레시피 구매를 통합 관리하는 싱글턴 매니저입니다.
    public class SaveSystem : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  싱글턴
        // ════════════════════════════════════════════════════════════════

        private static SaveSystem s_Instance;
        private static bool s_IsQuitting;

        public static SaveSystem Instance
        {
            get
            {
                if (s_IsQuitting) return null;
                if (s_Instance == null)
                {
                    var go = new GameObject("SaveSystem");
                    s_Instance = go.AddComponent<SaveSystem>();
                    DontDestroyOnLoad(go);
                }
                return s_Instance;
            }
        }

        // ════════════════════════════════════════════════════════════════
        //  이벤트
        // ════════════════════════════════════════════════════════════════

        // 재화가 변경될 때 (gold, gum, delly) 발행됩니다.
        public event System.Action<int, int, int> OnCurrencyChanged
        {
            add { if (!m_IsInitialized) Initialize(); m_PlayerCurrency.OnChanged += value; }
            remove { if (m_PlayerCurrency != null) m_PlayerCurrency.OnChanged -= value; }
        }

        // ════════════════════════════════════════════════════════════════
        //  private 런타임 상태
        // ════════════════════════════════════════════════════════════════

        private PlayerCurrency m_PlayerCurrency;
        private RecipePurchaseData m_RecipePurchaseData;
        private bool m_IsInitialized;

        // ════════════════════════════════════════════════════════════════
        //  Unity 생명 주기
        // ════════════════════════════════════════════════════════════════

        // 싱글턴 중복 방지, DontDestroyOnLoad 등록, 데이터 초기화.
        private void Awake()
        {
            if (s_Instance != null && s_Instance != this)
            {
                Destroy(gameObject);
                return;
            }

            s_Instance = this;
            DontDestroyOnLoad(gameObject);
            Initialize();
        }

        // 앱 종료 감지 플래그를 설정해 Instance 접근 시 null을 반환하게 합니다.
        private void OnDestroy()
        {
            s_IsQuitting = true;
        }

        // ════════════════════════════════════════════════════════════════
        //  public API — 스테이지 진행
        // ════════════════════════════════════════════════════════════════

        // [UIHandler] 스테이지 결과를 별(1-3)로 기록합니다. 이전 기록보다 높을 때만 갱신합니다.
        public void RecordStageResult(ShopType shopType, int stageNumber, int earnedMoney, int goalMoney, int perfectMoney)
        {
            int stars = earnedMoney >= perfectMoney ? 3 : earnedMoney >= goalMoney ? 1 : 0;
            if (stars <= 0) return;

            string key = GetStageKey(shopType, stageNumber);
            int previous = PlayerPrefs.GetInt(key, 0);
            if (stars > previous)
            {
                PlayerPrefs.SetInt(key, stars);
                PlayerPrefs.Save();
            }
        }

        // 스테이지 클리어 여부(별 1개 이상)를 반환합니다.
        public bool IsStageCleared(ShopType shopType, int stageNumber)
        {
            return GetStageStarCount(shopType, stageNumber) > 0;
        }

        // 스테이지 별 개수(0-3)를 반환합니다.
        public int GetStageStarCount(ShopType shopType, int stageNumber)
        {
            return PlayerPrefs.GetInt(GetStageKey(shopType, stageNumber), 0);
        }

        // ════════════════════════════════════════════════════════════════
        //  public API — 재화 (PlayerCurrency 위임)
        // ════════════════════════════════════════════════════════════════

        // PlayerCurrency 인스턴스에 직접 접근합니다 (Gum 타이머 등 고급 기능용).
        public PlayerCurrency GetPlayerCurrency()
        {
            if (!m_IsInitialized) Initialize();
            return m_PlayerCurrency;
        }

        // ──────────────────────────────────────────────────────────
        //  Gold
        // ──────────────────────────────────────────────────────────

        // Gold 잔액을 반환합니다.
        public int GetGold() { if (!m_IsInitialized) Initialize(); return m_PlayerCurrency.Gold; }

        // Gold를 amount만큼 추가합니다.
        public int AddGold(int amount) { if (!m_IsInitialized) Initialize(); return m_PlayerCurrency.AddGold(amount); }

        // Gold를 amount만큼 차감합니다.
        public int SpendGold(int amount) { if (!m_IsInitialized) Initialize(); return m_PlayerCurrency.SpendGold(amount); }

        // Gold 잔액이 amount 이상인지 확인합니다.
        public bool HasEnoughGold(int amount) { if (!m_IsInitialized) Initialize(); return m_PlayerCurrency.HasEnoughGold(amount); }

        // ──────────────────────────────────────────────────────────
        //  Gum
        // ──────────────────────────────────────────────────────────

        // Gum 현재 개수를 반환합니다 (자동 회복 후 판단).
        public int GetGum() { if (!m_IsInitialized) Initialize(); return m_PlayerCurrency.Gum; }

        // Gum을 amount만큼 추가합니다.
        public int AddGum(int amount) { if (!m_IsInitialized) Initialize(); return m_PlayerCurrency.AddGum(amount); }

        // Gum을 amount만큼 차감합니다.
        public int SpendGum(int amount) { if (!m_IsInitialized) Initialize(); return m_PlayerCurrency.SpendGum(amount); }

        // Gum 잔액이 amount 이상인지 확인합니다.
        public bool HasEnoughGum(int amount) { if (!m_IsInitialized) Initialize(); return m_PlayerCurrency.HasEnoughGum(amount); }

        // 게임 시작 시 Gum을 소모합니다. Returns: false=Gum 부족
        public bool SpendGumForGame(bool isContinuePlay = false)
        {
            if (!m_IsInitialized) Initialize();
            return m_PlayerCurrency.SpendGumForGame(isContinuePlay);
        }

        // ──────────────────────────────────────────────────────────
        //  Delly
        // ──────────────────────────────────────────────────────────

        // Delly 잔액을 반환합니다.
        public int GetDelly() { if (!m_IsInitialized) Initialize(); return m_PlayerCurrency.Delly; }

        // Delly를 amount만큼 추가합니다.
        public int AddDelly(int amount) { if (!m_IsInitialized) Initialize(); return m_PlayerCurrency.AddDelly(amount); }

        // Delly를 amount만큼 차감합니다.
        public int SpendDelly(int amount) { if (!m_IsInitialized) Initialize(); return m_PlayerCurrency.SpendDelly(amount); }

        // Delly 잔액이 amount 이상인지 확인합니다.
        public bool HasEnoughDelly(int amount) { if (!m_IsInitialized) Initialize(); return m_PlayerCurrency.HasEnoughDelly(amount); }

        // ════════════════════════════════════════════════════════════════
        //  public API — 레시피 구매 (RecipePurchaseData 위임)
        // ════════════════════════════════════════════════════════════════

        // 레시피를 구매합니다. Returns: true=구매 성공
        public bool PurchaseRecipe(RecipeData recipe)
        {
            if (!m_IsInitialized) Initialize();
            if (recipe == null) { Debug.LogWarning("[SaveSystem] recipe가 null입니다."); return false; }
            if (m_RecipePurchaseData.IsPurchased(recipe)) { Debug.Log($"[SaveSystem] '{recipe.RecipeTag}'은(는) 이미 구매했습니다."); return false; }
            if (recipe.Type == RecipeType.Default) { Debug.LogWarning($"[SaveSystem] '{recipe.RecipeTag}'은(는) 기본 레시피입니다."); return false; }

            bool paid = false;

            if (recipe.Type == RecipeType.StageExclusive && m_PlayerCurrency.HasEnoughGold(recipe.PurchasePrice))
            {
                m_PlayerCurrency.SpendGold(recipe.PurchasePrice);
                paid = true;
            }
            else if (recipe.Type == RecipeType.Secret && m_PlayerCurrency.HasEnoughDelly(recipe.SpecialPrice))
            {
                m_PlayerCurrency.SpendDelly(recipe.SpecialPrice);
                paid = true;
            }

            if (paid)
            {
                m_RecipePurchaseData.PurchaseRecipe(recipe);
                return true;
            }

            return false;
        }

        // 레시피가 이미 구매되었는지 확인합니다.
        public bool IsRecipePurchased(RecipeData recipe)
        {
            if (!m_IsInitialized) Initialize();
            return m_RecipePurchaseData.IsPurchased(recipe);
        }

        // ════════════════════════════════════════════════════════════════
        //  private 구현
        // ════════════════════════════════════════════════════════════════

        // PlayerCurrency와 RecipePurchaseData를 로드합니다. 중복 호출은 무시합니다.
        private void Initialize()
        {
            if (m_IsInitialized) return;
            m_PlayerCurrency = PlayerCurrency.Load();
            m_RecipePurchaseData = RecipePurchaseData.Load();
            m_IsInitialized = true;
        }

        // PlayerPrefs 저장 키를 반환합니다.
        private static string GetStageKey(ShopType shopType, int stageNumber)
        {
            return $"StageProgress.{shopType}.{stageNumber}";
        }

        // 모든 재화와 레시피 구매 기록을 초기화합니다.
        [System.Diagnostics.Conditional("UNITY_EDITOR")]
        public void ResetAllProgress()
        {
            if (!m_IsInitialized) Initialize();
            m_PlayerCurrency.ResetAll();
            m_RecipePurchaseData.ResetAll();
        }

        // 현재 재화 상태를 로그로 출력합니다.
        [System.Diagnostics.Conditional("UNITY_EDITOR")]
        public void LogCurrency()
        {
            if (!m_IsInitialized) Initialize();
            m_PlayerCurrency.Log();
        }

        // 구매한 레시피 목록을 로그로 출력합니다.
        [System.Diagnostics.Conditional("UNITY_EDITOR")]
        public void LogPurchasedRecipes()
        {
            if (!m_IsInitialized) Initialize();
            m_RecipePurchaseData.LogPurchasedRecipes();
        }
    }
}
