using UnityEngine;

/// <summary>
/// 플레이어의 게임 진행 상황을 관리하는 싱글톤 클래스
/// StageProgress와 PlayerCurrency를 통합하여 관리합니다.
/// </summary>
public class SaveSystem : MonoBehaviour
{
    private static SaveSystem instance;
    public static SaveSystem Instance
    {
        get
        {
            if (applicationIsQuitting)
            {
                return null;
            }
            if (instance == null)
            {
                GameObject go = new("SaveSystem");
                instance = go.AddComponent<SaveSystem>();
                DontDestroyOnLoad(go);
            }

            return instance;
        }
    }

    private PlayerCurrency playerCurrency;
    private RecipePurchaseData recipePurchaseData;
    private bool isInitialized = false;

    // https://drehzr.tistory.com/1566
    private static bool applicationIsQuitting = false;

    public void OnDestroy()
    {
        applicationIsQuitting = true;
    }

    // 재화 변경 이벤트 (UI 업데이트용)
    public event System.Action<int, int, int> OnCurrencyChanged
    {
        add => playerCurrency.OnChanged += value;
        remove => playerCurrency.OnChanged -= value;
    }

    private void Awake()
    {
        if (instance == null)
        {
            instance = this;
            DontDestroyOnLoad(gameObject);
            Initialize();
        }
        else if (instance != this)
        {
            Destroy(gameObject);
        }
    }

    /// <summary>
    /// SaveSystem 시스템을 초기화합니다.
    /// </summary>
    private void Initialize()
    {
        if (isInitialized) return;

        // 데이터 로드
        StageProgress.LoadAll();
        playerCurrency = PlayerCurrency.Load();
        recipePurchaseData = RecipePurchaseData.Load();

        isInitialized = true;
    }

    #region 스테이지 관리 (StageProgress 위임)

    /// <summary>
    /// 스테이지 결과를 기록합니다.
    /// </summary>
    public void RecordStageResult(ShopType shopType, int stageNumber, int earnedMoney, int goalMoney, int perfectMoney)
    {
        if (!isInitialized) Initialize();
        StageProgress.Record(shopType, stageNumber, earnedMoney, goalMoney, perfectMoney);
    }

    /// <summary>
    /// 특정 스테이지가 클리어되었는지 확인합니다.
    /// </summary>
    public bool IsStageCleared(ShopType shopType, int stageNumber)
    {
        if (!isInitialized) Initialize();
        return StageProgress.IsCleared(shopType, stageNumber);
    }

    /// <summary>
    /// 특정 스테이지의 별 개수를 반환합니다.
    /// </summary>
    public int GetStageStarCount(ShopType shopType, int stageNumber)
    {
        if (!isInitialized) Initialize();
        return StageProgress.GetStarCount(shopType, stageNumber);
    }

    #endregion

    #region 재화 관리 (PlayerCurrency 위임)

    // Gold
    public int GetGold()
    {
        if (!isInitialized) Initialize();
        return playerCurrency.Gold;
    }

    public int AddGold(int amount)
    {
        if (!isInitialized) Initialize();
        return playerCurrency.AddGold(amount);
    }

    public int SpendGold(int amount)
    {
        if (!isInitialized) Initialize();
        return playerCurrency.SpendGold(amount);
    }

    public bool HasEnoughGold(int amount)
    {
        if (!isInitialized) Initialize();
        return playerCurrency.HasEnoughGold(amount);
    }

    // Gum
    public int GetGum()
    {
        if (!isInitialized) Initialize();
        return playerCurrency.Gum;
    }

    public int AddGum(int amount)
    {
        if (!isInitialized) Initialize();
        return playerCurrency.AddGum(amount);
    }

    public int SpendGum(int amount)
    {
        if (!isInitialized) Initialize();
        return playerCurrency.SpendGum(amount);
    }

    public bool HasEnoughGum(int amount)
    {
        if (!isInitialized) Initialize();
        return playerCurrency.HasEnoughGum(amount);
    }

    /// <summary>
    /// PlayerCurrency 인스턴스에 직접 접근합니다 (타이머 등 고급 기능용)
    /// </summary>
    public PlayerCurrency GetPlayerCurrency()
    {
        if (!isInitialized) Initialize();
        return playerCurrency;
    }

    /// <summary>
    /// 게임 시작 시 Gum을 소모합니다.
    /// </summary>
    /// <param name="isContinuePlay">클리어 후 연속 플레이 여부 (true면 소모 안 함)</param>
    /// <returns>소모 성공 여부 (Gum 부족 시 false)</returns>
    public bool SpendGumForGame(bool isContinuePlay = false)
    {
        if (!isInitialized) Initialize();
        return playerCurrency.SpendGumForGame(isContinuePlay);
    }

    // Delly
    public int GetDelly()
    {
        if (!isInitialized) Initialize();
        return playerCurrency.Delly;
    }

    public int AddDelly(int amount)
    {
        if (!isInitialized) Initialize();
        return playerCurrency.AddDelly(amount);
    }

    public int SpendDelly(int amount)
    {
        if (!isInitialized) Initialize();
        return playerCurrency.SpendDelly(amount);
    }

    public bool HasEnoughDelly(int amount)
    {
        if (!isInitialized) Initialize();
        return playerCurrency.HasEnoughDelly(amount);
    }

    #endregion

    #region 레시피 구매 관리 (RecipePurchaseData 위임)

    /// <summary>
    /// 레시피를 구매합니다.
    /// </summary>
    /// <param name="recipe">구매할 레시피</param>
    /// <returns>구매 성공 여부</returns>
    public bool PurchaseRecipe(RecipeSO recipe)
    {
        if (!isInitialized) Initialize();

        if (recipe == null)
        {
            Debug.LogWarning("[SaveSystem] 구매할 레시피가 null입니다.");
            return false;
        }

        // 이미 구매한 레시피인지 확인
        if (recipePurchaseData.IsPurchased(recipe))
        {
            Debug.Log($"[SaveSystem] '{recipe.RecipeName}'은(는) 이미 구매한 레시피입니다.");
            return false;
        }

        // 기본 레시피는 구매할 수 없음
        if (recipe.Type == RecipeType.Default)
        {
            Debug.LogWarning($"[SaveSystem] '{recipe.RecipeName}'은(는) 기본 레시피이므로 구매할 수 없습니다.");
            return false;
        }

        // 재화 확인 및 차감
        bool paymentSuccess = false;

        if (recipe.Type == RecipeType.StageExclusive)
        {
            // Gold로 구매
            if (HasEnoughGold(recipe.PurchasePrice))
            {
                SpendGold(recipe.PurchasePrice);
                paymentSuccess = true;
            }
            else
            {
                Debug.LogWarning($"[SaveSystem] Gold가 부족합니다. 필요: {recipe.PurchasePrice}, 보유: {GetGold()}");
            }
        }
        else if (recipe.Type == RecipeType.Secret)
        {
            // Delly로 구매
            if (HasEnoughDelly(recipe.DellyPrice))
            {
                SpendDelly(recipe.DellyPrice);
                paymentSuccess = true;
            }
            else
            {
                Debug.LogWarning($"[SaveSystem] Delly가 부족합니다. 필요: {recipe.DellyPrice}, 보유: {GetDelly()}");
            }
        }

        // 구매 성공 시 기록
        if (paymentSuccess)
        {
            recipePurchaseData.PurchaseRecipe(recipe);
            Debug.Log($"[SaveSystem] '{recipe.RecipeName}' 구매 완료!");
            return true;
        }

        return false;
    }

    /// <summary>
    /// 레시피가 구매되었는지 확인합니다.
    /// </summary>
    public bool IsRecipePurchased(RecipeSO recipe)
    {
        if (!isInitialized) Initialize();
        return recipePurchaseData.IsPurchased(recipe);
    }

    #endregion

    #region 디버그 및 유틸리티

    /// <summary>
    /// 모든 진행 상황을 초기화합니다. (개발/테스트용)
    /// </summary>
    [System.Diagnostics.Conditional("UNITY_EDITOR")]
    public void ResetAllProgress()
    {
        if (!isInitialized) Initialize();

        StageProgress.ResetAll();
        playerCurrency.ResetAll();
        recipePurchaseData.ResetAll();
    }

    /// <summary>
    /// 현재 캐시된 모든 결과를 로그로 출력합니다. (디버그용)
    /// </summary>
    [System.Diagnostics.Conditional("UNITY_EDITOR")]
    public void LogAllResults()
    {
        if (!isInitialized) Initialize();
        StageProgress.LogAll();
    }

    /// <summary>
    /// 현재 재화 정보를 로그로 출력합니다. (디버그용)
    /// </summary>
    [System.Diagnostics.Conditional("UNITY_EDITOR")]
    public void LogCurrency()
    {
        if (!isInitialized) Initialize();
        playerCurrency.Log();
    }

    /// <summary>
    /// 구매한 레시피 목록을 로그로 출력합니다. (디버그용)
    /// </summary>
    [System.Diagnostics.Conditional("UNITY_EDITOR")]
    public void LogPurchasedRecipes()
    {
        if (!isInitialized) Initialize();
        recipePurchaseData.LogPurchasedRecipes();
    }

    #endregion
}