using UnityEngine;

/// <summary>
/// 씬 간 게임 데이터 전달을 위한 싱글톤 클래스
/// StageManager에서 선택된 shop과 stage 정보를 GameManager에 전달
/// </summary>
public class GameData : MonoBehaviour
{
    private static GameData instance;
    public static GameData Instance
    {
        get
        {
            if (instance == null)
            {
                GameObject go = new GameObject("GameData");
                instance = go.AddComponent<GameData>();
                DontDestroyOnLoad(go);
            }
            return instance;
        }
    }

    [Header("Selected Game Settings")]
    [SerializeField] private ShopSO selectedShop;
    [SerializeField] private StageSO selectedStage;
    [SerializeField] private int selectedStageIndex = -1;

    public ShopSO SelectedShop => selectedShop;
    public StageSO SelectedStage => selectedStage;
    public int SelectedStageIndex => selectedStageIndex;

    private void Awake()
    {
        if (instance == null)
        {
            instance = this;
            DontDestroyOnLoad(gameObject);
        }
        else if (instance != this)
        {
            Destroy(gameObject);
        }
    }

    /// <summary>
    /// 선택된 shop과 stage 정보를 설정합니다.
    /// </summary>
    /// <param name="shop">선택된 ShopSO</param>
    /// <param name="stage">선택된 StageSO</param>
    /// <param name="stageIndex">스테이지 인덱스</param>
    public void SetSelectedData(ShopSO shop, StageSO stage, int stageIndex = -1)
    {
        selectedShop = shop;
        selectedStage = stage;
        selectedStageIndex = stageIndex;

    }

    /// <summary>
    /// 저장된 데이터를 초기화합니다.
    /// </summary>
    public void ClearData()
    {
        selectedShop = null;
        selectedStage = null;
        selectedStageIndex = -1;
    }

    /// <summary>
    /// 유효한 데이터가 설정되어 있는지 확인합니다.
    /// </summary>
    /// <returns>shop과 stage가 모두 설정되어 있으면 true</returns>
    public bool HasValidData()
    {
        return selectedShop != null && selectedStage != null;
    }
}