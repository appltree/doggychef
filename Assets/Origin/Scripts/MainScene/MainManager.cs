using System.Collections;
using UnityEngine;
using UnityEngine.SceneManagement;

public class MainManager : MonoBehaviour
{

    private readonly string stageSceneName = "StageScene";

    private bool isLoading = false;

    void Start()
    {
        // 메인 씬 진입 시 프레임레이트 설정 적용
        FrameRateController.ApplyFrameRate();

    }

    public void OnStagePressed()
    {
        if (isLoading)
        {
            return;
        }

        isLoading = true;
        StartCoroutine(LoadStageScene());
    }


    private IEnumerator LoadStageScene()
    {
        // 비동기 씬 로드: 버튼 중복 클릭 방지 및 프리즈 방지
        AsyncOperation op = SceneManager.LoadSceneAsync(stageSceneName, LoadSceneMode.Single);
        if (op == null)
        {
            Debug.LogWarning($"[MainManager] Failed to start loading scene '{stageSceneName}'. Check Build Settings.");
            isLoading = false; // 로드 실패 시만 되돌림
            yield break;
        }

        // 필요하다면 여기에 로딩 UI 표시/업데이트를 연결할 수 있습니다.
        while (!op.isDone)
        {
            yield return null;
        }
        // 씬 전환이 완료되면 이 오브젝트는 파괴되므로 isLoading 리셋은 불필요.
    }
}
