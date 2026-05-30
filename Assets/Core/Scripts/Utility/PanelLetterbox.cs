using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(RectTransform))]
public class PanelLetterbox : MonoBehaviour
{
    [Header("화면 비율 설정")]
    [SerializeField] private float targetAspect = 16.0f / 9.0f;
    [SerializeField] private float maxAspect = 20.0f / 9.0f;

    private RectTransform rectTransform;
    private Camera mainCamera;

    void Start()
    {
        rectTransform = GetComponent<RectTransform>();
        mainCamera = Camera.main;

        // 패널 밖으로 나가는 UI가 보이지 않도록 RectMask2D 보장
        if (!TryGetComponent<RectMask2D>(out var rectMask))
        {
            rectMask = gameObject.AddComponent<RectMask2D>();
        }
        rectMask.enabled = true;

        if (mainCamera == null)
        {
            mainCamera = FindAnyObjectByType<Camera>();
        }

        AdjustPanelSize();
    }

    void Update()
    {
        // 화면 크기가 변경될 수 있는 환경에서는 매 프레임 체크
#if UNITY_EDITOR || UNITY_STANDALONE
        AdjustPanelSize();
#endif
    }

    private void AdjustPanelSize()
    {
        if (rectTransform == null || mainCamera == null) return;

        float windowAspect = (float)Screen.width / (float)Screen.height;
        float scaleHeight = windowAspect / targetAspect;

        Vector2 sizeDelta = rectTransform.sizeDelta;
        Vector2 anchoredPosition = rectTransform.anchoredPosition;

        if (scaleHeight < 1.0f)
        {
            // Letterbox 상황: 위아래 검은띠
            // 실제 게임 화면 높이만큼 패널 크기 조정
            float gameScreenHeight = Screen.height * scaleHeight;
            float letterboxHeight = (Screen.height - gameScreenHeight) * 0.5f;

            // Canvas의 스케일 팩터 고려
            Canvas parentCanvas = GetComponentInParent<Canvas>();
            float scaleFactor = parentCanvas != null ? parentCanvas.scaleFactor : 1.0f;

            sizeDelta.y = gameScreenHeight / scaleFactor;
            sizeDelta.x = Screen.width / scaleFactor;

            // 중앙 정렬
            anchoredPosition = Vector2.zero;
        }
        else if (windowAspect > maxAspect)
        {
            // Pillarbox 상황: 좌우 검은띠
            float scaleWidth = maxAspect / windowAspect;
            float gameScreenWidth = Screen.width * scaleWidth;

            Canvas parentCanvas = GetComponentInParent<Canvas>();
            float scaleFactor = parentCanvas != null ? parentCanvas.scaleFactor : 1.0f;

            sizeDelta.x = gameScreenWidth / scaleFactor;
            sizeDelta.y = Screen.height / scaleFactor;

            // 중앙 정렬
            anchoredPosition = Vector2.zero;
        }
        else
        {
            // 16:9 ~ 20:9 사이: 전체 화면 사용
            Canvas parentCanvas = GetComponentInParent<Canvas>();
            float scaleFactor = parentCanvas != null ? parentCanvas.scaleFactor : 1.0f;

            sizeDelta.x = Screen.width / scaleFactor;
            sizeDelta.y = Screen.height / scaleFactor;

            anchoredPosition = Vector2.zero;
        }

        rectTransform.sizeDelta = sizeDelta;
        rectTransform.anchoredPosition = anchoredPosition;

        // 앵커를 중앙으로 설정하여 화면 중앙에 배치
        rectTransform.anchorMin = new Vector2(0.5f, 0.5f);
        rectTransform.anchorMax = new Vector2(0.5f, 0.5f);
        rectTransform.pivot = new Vector2(0.5f, 0.5f);
    }

    // 화면 회전이나 크기 변경 시 호출될 수 있는 공개 메서드
    public void RefreshPanelSize()
    {
        AdjustPanelSize();
    }
}
