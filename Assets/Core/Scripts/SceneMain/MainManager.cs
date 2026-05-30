using System.Collections;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace DoggyChef
{
    // 로비(Main) 씬의 매니저입니다. 스테이지 씬으로의 전환을 담당합니다.
    public class MainManager : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  Inspector 연결 필드
        // ════════════════════════════════════════════════════════════════

        [Header("씬 이름")]
        [Tooltip("전환할 스테이지 씬 이름 (Build Settings와 일치해야 합니다)")]
        [SerializeField] private string m_StageSceneName = "Stage";

        // ════════════════════════════════════════════════════════════════
        //  private 런타임 상태
        // ════════════════════════════════════════════════════════════════

        private bool m_IsLoading;

        // ════════════════════════════════════════════════════════════════
        //  Unity 생명 주기
        // ════════════════════════════════════════════════════════════════

        // 씬 진입 시 프레임레이트 설정을 적용합니다.
        private void Start()
        {
            FrameRateController.ApplyFrameRate();
        }

        // ════════════════════════════════════════════════════════════════
        //  public API
        // ════════════════════════════════════════════════════════════════

        // [StartButton] 스테이지 씬을 비동기로 로드합니다. 중복 호출을 방지합니다.
        public void OnStagePressed()
        {
            if (m_IsLoading) return;
            m_IsLoading = true;
            StartCoroutine(LoadStageScene());
        }

        // ════════════════════════════════════════════════════════════════
        //  private 구현
        // ════════════════════════════════════════════════════════════════

        // 스테이지 씬을 비동기로 로드합니다.
        private IEnumerator LoadStageScene()
        {
            var op = SceneManager.LoadSceneAsync(m_StageSceneName, LoadSceneMode.Single);
            if (op == null)
            {
                Debug.LogWarning($"[MainManager] '{m_StageSceneName}' 씬 로드 실패. Build Settings를 확인하세요.");
                m_IsLoading = false;
                yield break;
            }

            while (!op.isDone)
                yield return null;
        }
    }
}
