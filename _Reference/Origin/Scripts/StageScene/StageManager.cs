using TS.PageSlider;
using UnityEngine;
using UnityEngine.SceneManagement;

public class StageManager : MonoBehaviour
{
    private readonly string mainSceneName = "MainScene";
    private readonly string gameSceneName = "GameScene";

    [SerializeField] private ShopSO[] shops;

    private bool isLoading = false;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        // 스테이지 씬 진입 시 프레임레이트 설정 적용
        FrameRateController.ApplyFrameRate();

        // 게임에서 빠져나올때 일시정지 해제
        Time.timeScale = 1;
        AudioListener.pause = false;

        // StageCell 이벤트 구독
        StageCell.OnStageCellTapped += OnStageCellTapped;
    }

    private void OnDestroy()
    {
        // 이벤트 구독 해제
        StageCell.OnStageCellTapped -= OnStageCellTapped;
    }

    public void OnBack()
    {
        if (isLoading)
        {
            return;
        }

        isLoading = true;
        StartCoroutine(LoadMainScene());
    }

    /// <summary>
    /// 특정 인덱스의 ShopSO를 반환합니다.
    /// </summary>
    /// <param name="index">shops 배열의 인덱스</param>
    /// <returns>해당 인덱스의 ShopSO, 유효하지 않으면 null</returns>
    public ShopSO GetShop(int index)
    {
        if (shops == null || index < 0 || index >= shops.Length)
        {
            return null;
        }
        return shops[index];
    }

    // StageCell에서 탭 이벤트 발생 시 호출
    private void OnStageCellTapped(StageSO selectedStage, int stageIndex)
    {
        if (isLoading)
        {
            return;
        }

        // 선택된 stage와 현재 shop 정보를 GameData에 저장
        // TODO: 현재 선택된 shop을 어떻게 가져올지 결정 필요
        // 임시로 첫 번째 shop을 사용하거나, 별도의 shop 선택 로직이 필요
        ShopSO currentShop = GetCurrentShop();
        if (currentShop != null && selectedStage != null)
        {
            GameData.Instance.SetSelectedData(currentShop, selectedStage, stageIndex);
        }
        else
        {
            Debug.LogWarning("[StageManager] Missing shop or stage data. Cannot proceed to game scene.");
            return;
        }

        isLoading = true;
        StartCoroutine(LoadGameScene());
    }

    /// <summary>
    /// 현재 선택된 shop을 반환합니다. 
    /// 추후 shop 선택 UI가 구현되면 해당 로직으로 대체 필요
    /// </summary>
    /// <returns>현재 선택된 ShopSO</returns>
    private ShopSO GetCurrentShop()
    {
        // 임시로 첫 번째 shop 반환
        // TODO: 실제 shop 선택 로직 구현 필요
        if (shops != null && shops.Length > 0)
        {
            return shops[0];
        }
        return null;
    }

    private System.Collections.IEnumerator LoadGameScene()
    {
        // 게임 씬으로 비동기 전환
        AsyncOperation op = SceneManager.LoadSceneAsync(gameSceneName, LoadSceneMode.Single);
        if (op == null)
        {
            Debug.LogWarning($"[StageManager] Failed to start loading scene '{gameSceneName}'. Check Build Settings.");
            isLoading = false; // 로드 시작 실패 시만 해제
            yield break;
        }

        while (!op.isDone)
        {
            yield return null;
        }
        // 씬 전환 후에는 이 오브젝트가 파괴되며 플래그 초기화 불필요
    }

    private System.Collections.IEnumerator LoadMainScene()
    {
        // 메인 씬으로 비동기 전환
        AsyncOperation op = SceneManager.LoadSceneAsync(mainSceneName, LoadSceneMode.Single);
        if (op == null)
        {
            Debug.LogWarning($"[StageManager] Failed to start loading scene '{mainSceneName}'. Check Build Settings.");
            isLoading = false; // 로드 시작 실패 시만 해제
            yield break;
        }

        while (!op.isDone)
        {
            yield return null;
        }
        // 씬 전환 후에는 이 오브젝트가 파괴되며 플래그 초기화 불필요
    }
}
