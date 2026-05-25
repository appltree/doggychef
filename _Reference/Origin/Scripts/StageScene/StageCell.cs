using UnityEngine;
using UnityEngine.UI;
using System;
using TMPro;

public class StageCell : MonoBehaviour
{
    // StageCell이 탭되었을 때 발생하는 이벤트 (StageSO와 인덱스 정보 포함)
    public static event Action<StageSO, int> OnStageCellTapped;

    [SerializeField] private RectTransform background; // 사각형 배경 (자식)
    [SerializeField] private TMP_Text stageNumber;
    [SerializeField] private Image markOn;
    [SerializeField] private Image[] stars;
    [SerializeField] private Image lockCover;

    // 이 셀이 가지고 있는 스테이지 데이터
    private StageSO stageData;
    private int stageIndex = -1;
    private ShopSO currentShop;
    private bool isLocked = true; // 잠금 상태

    private Color lineColor = new Color32(0xD7, 0xD7, 0xD7, 0xFF); // 1px 회색 (D7D7D7)
    private readonly float thicknessPixels = 1f; // 디바이스 픽셀 기준 두께
    private readonly bool pixelPerfect = true; // Canvas scaleFactor 반영
    private bool showTop = true;
    private bool showBottom = true;
    private bool showLeft = true;
    private bool showRight = true;

    private RectTransform lineTop;
    private RectTransform lineBottom;
    private RectTransform lineLeft;
    private RectTransform lineRight;

    void Awake()
    {
        AutoAssignBackground();
        EnsureLines();
        UpdateLines();
    }

    void OnEnable()
    {
        EnsureLines();
        UpdateLines();
    }

    void OnRectTransformDimensionsChange()
    {
        UpdateLines();
    }

    public void OnTap()
    {
        // 잠김 상태이면 탭 이벤트 차단
        if (isLocked)
        {
            return;
        }

        // StageCell 탭 이벤트 발생 (스테이지 데이터와 인덱스 전달)
        OnStageCellTapped?.Invoke(stageData, stageIndex);
    }


    public void SetStageData(ShopSO shopData, StageSO stageData)
    {
        this.stageData = stageData;
        this.currentShop = shopData;
        this.stageIndex = stageData.StageNumber;


        // 스테이지 번호 설정
        stageNumber.text = stageData.StageNumber.ToString();

        // 스테이지 상태에 따라 UI 업데이트 (markOn, stars, lockCover)
        UpdateUIVisibility();
        UpdateLockCoverVisibility();
    }

    /// <summary>
    /// 스테이지 상태에 따라 UI(markOn, stars)의 표시 여부를 업데이트합니다.
    /// </summary>
    private void UpdateUIVisibility()
    {
        // 기본 null 체크
        if (currentShop == null || stageData == null)
        {
            // markOn 비활성화
            if (markOn != null)
                markOn.gameObject.SetActive(false);

            // 모든 별 비활성화
            if (stars != null)
            {
                foreach (var star in stars)
                {
                    if (star != null)
                        star.gameObject.SetActive(false);
                }
            }
            return;
        }

        try
        {
            // SaveSystem Instance null 체크
            if (SaveSystem.Instance == null)
            {
                Debug.LogWarning("[StageCell] SaveSystem.Instance가 null입니다.");

                // markOn 비활성화
                if (markOn != null)
                    markOn.gameObject.SetActive(false);

                // 모든 별 비활성화
                if (stars != null)
                {
                    foreach (var star in stars)
                    {
                        if (star != null)
                            star.gameObject.SetActive(false);
                    }
                }
                return;
            }

            // markOn 업데이트 (클리어 여부)
            if (markOn != null)
            {
                bool isCleared = SaveSystem.Instance.IsStageCleared(currentShop.Type, stageData.StageNumber);
                markOn.gameObject.SetActive(isCleared);

                // 클리어된 경우 shop의 main color 적용
                if (isCleared && currentShop != null)
                {
                    markOn.color = currentShop.MainColor;
                }
            }

            // stars 업데이트 (별 개수)
            if (stars != null && stars.Length > 0)
            {
                int starCount = SaveSystem.Instance.GetStageStarCount(currentShop.Type, stageData.StageNumber);

                for (int i = 0; i < stars.Length; i++)
                {
                    if (stars[i] != null)
                    {
                        stars[i].gameObject.SetActive(i < starCount);
                    }
                }
            }
        }
        catch (System.Exception e)
        {
            Debug.LogWarning($"[StageCell] UpdateUIVisibility 오류: {e.Message}");

            // 오류 발생 시 모든 UI 요소 비활성화
            if (markOn != null)
                markOn.gameObject.SetActive(false);

            if (stars != null)
            {
                foreach (var star in stars)
                {
                    if (star != null)
                        star.gameObject.SetActive(false);
                }
            }
        }
    }

    /// <summary>
    /// 이전 스테이지 클리어 여부에 따라 lockCover의 표시 여부를 업데이트합니다.
    /// </summary>
    private void UpdateLockCoverVisibility()
    {
        // 기본 데이터가 없으면 잠금 처리
        if (currentShop == null || stageData == null)
        {
            isLocked = true;
            lockCover.gameObject.SetActive(true);
            return;
        }

        try
        {
            // SaveSystem Instance null 체크
            if (SaveSystem.Instance == null)
            {
                Debug.LogWarning("[StageCell] SaveSystem.Instance가 null입니다.");
                isLocked = true;
                lockCover.gameObject.SetActive(true);
                return;
            }

            // 첫 번째 스테이지는 항상 잠금 해제
            if (stageData.StageNumber <= 1)
            {
                isLocked = false;
                lockCover.gameObject.SetActive(false);
                return;
            }

            // 이전 스테이지 클리어 여부 확인
            int previousStageNumber = stageData.StageNumber - 1;
            bool isPreviousStageCleared = SaveSystem.Instance.IsStageCleared(currentShop.Type, previousStageNumber);

            // 이전 스테이지가 클리어되지 않았으면 잠금
            isLocked = !isPreviousStageCleared;
            lockCover.gameObject.SetActive(isLocked);
        }
        catch (System.Exception e)
        {
            Debug.LogWarning($"[StageCell] UpdateLockCoverVisibility 오류: {e.Message}");
            // 오류 발생 시 안전하게 잠금 처리
            isLocked = true;
            lockCover.gameObject.SetActive(true);
        }
    }

    private void AutoAssignBackground()
    {
        if (background != null) return;
        // 자식 중 첫 번째 Image가 달린 RectTransform을 배경으로 사용
        for (int i = 0; i < transform.childCount; i++)
        {
            var rt = transform.GetChild(i) as RectTransform;
            if (!rt) continue;
            if (rt.GetComponent<Image>() != null)
            {
                background = rt;
                break;
            }
        }
    }

    private void EnsureLines()
    {
        if (!background) return;

        lineTop = EnsureLine(background, "LineTop");
        lineBottom = EnsureLine(background, "LineBottom");
        lineLeft = EnsureLine(background, "LineLeft");
        lineRight = EnsureLine(background, "LineRight");

        ApplyLineColors();
        ApplyVisibility();
    }

    private RectTransform EnsureLine(RectTransform parent, string name)
    {
        Transform t = parent.Find(name);
        RectTransform rt;
        if (!t)
        {
            var go = new GameObject(name, typeof(RectTransform), typeof(Image));
            rt = go.GetComponent<RectTransform>();
            rt.SetParent(parent, false);
            rt.gameObject.layer = parent.gameObject.layer;
            var img = go.GetComponent<Image>();
            img.color = lineColor;
            img.raycastTarget = false;
        }
        else
        {
            rt = t as RectTransform;
            var img = rt.GetComponent<Image>();
            if (img) { img.color = lineColor; img.raycastTarget = false; }
        }

        rt.SetAsLastSibling(); // 선이 배경 위에 보이도록
        return rt;
    }

    private void ApplyLineColors()
    {
        if (lineTop) SetImageColor(lineTop, lineColor);
        if (lineBottom) SetImageColor(lineBottom, lineColor);
        if (lineLeft) SetImageColor(lineLeft, lineColor);
        if (lineRight) SetImageColor(lineRight, lineColor);
    }

    private static void SetImageColor(RectTransform rt, Color c)
    {
        var img = rt ? rt.GetComponent<Image>() : null;
        if (img) img.color = c;
    }

    private void ApplyVisibility()
    {
        if (lineTop) lineTop.gameObject.SetActive(showTop);
        if (lineBottom) lineBottom.gameObject.SetActive(showBottom);
        if (lineLeft) lineLeft.gameObject.SetActive(showLeft);
        if (lineRight) lineRight.gameObject.SetActive(showRight);
    }

    private float GetUnitThickness()
    {
        if (!pixelPerfect) return thicknessPixels;
        var canvas = GetComponentInParent<Canvas>();
        float scale = (canvas && canvas.rootCanvas) ? canvas.rootCanvas.scaleFactor : 1f;
        if (scale <= 0f) scale = 1f;
        return thicknessPixels / scale;
    }

    private void UpdateLines()
    {
        if (!background) return;
        if (!lineTop || !lineBottom || !lineLeft || !lineRight) return;

        float t = Mathf.Max(0.5f, GetUnitThickness());
        float half = t * 0.5f;

        // Top (inside): stretch X, anchor to top, shift -half down
        lineTop.anchorMin = new Vector2(0f, 1f);
        lineTop.anchorMax = new Vector2(1f, 1f);
        lineTop.pivot = new Vector2(0.5f, 0.5f);
        lineTop.sizeDelta = new Vector2(0f, t);
        lineTop.anchoredPosition = new Vector2(0f, -half);

        // Bottom (inside): stretch X, anchor to bottom, shift +half up
        lineBottom.anchorMin = new Vector2(0f, 0f);
        lineBottom.anchorMax = new Vector2(1f, 0f);
        lineBottom.pivot = new Vector2(0.5f, 0.5f);
        lineBottom.sizeDelta = new Vector2(0f, t);
        lineBottom.anchoredPosition = new Vector2(0f, half);

        // Left (inside): stretch Y, anchor to left, shift +half right
        lineLeft.anchorMin = new Vector2(0f, 0f);
        lineLeft.anchorMax = new Vector2(0f, 1f);
        lineLeft.pivot = new Vector2(0.5f, 0.5f);
        lineLeft.sizeDelta = new Vector2(t, 0f);
        lineLeft.anchoredPosition = new Vector2(half, 0f);

        // Right (inside): stretch Y, anchor to right, shift -half left
        lineRight.anchorMin = new Vector2(1f, 0f);
        lineRight.anchorMax = new Vector2(1f, 1f);
        lineRight.pivot = new Vector2(0.5f, 0.5f);
        lineRight.sizeDelta = new Vector2(t, 0f);
        lineRight.anchoredPosition = new Vector2(-half, 0f);

        ApplyVisibility();
    }

    // 외부에서 라인 표시를 제어할 수 있도록 프로퍼티 제공
    public bool ShowTop { get => showTop; set { showTop = value; if (lineTop) lineTop.gameObject.SetActive(value); } }
    public bool ShowBottom { get => showBottom; set { showBottom = value; if (lineBottom) lineBottom.gameObject.SetActive(value); } }
    public bool ShowLeft { get => showLeft; set { showLeft = value; if (lineLeft) lineLeft.gameObject.SetActive(value); } }
    public bool ShowRight { get => showRight; set { showRight = value; if (lineRight) lineRight.gameObject.SetActive(value); } }
}
