using System;
using MyMatch3;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class StageCell : MonoBehaviour
{
    public static event Action<StageData, int> OnStageCellTapped;

    [SerializeField] private RectTransform background;
    [SerializeField] private TMP_Text stageNumber;
    [SerializeField] private Image markOn;
    [SerializeField] private Image[] stars;
    [SerializeField] private Image lockCover;

    private readonly Color lineColor = new Color32(0xD7, 0xD7, 0xD7, 0xFF);
    private readonly float thicknessPixels = 1f;
    private readonly bool pixelPerfect = true;

    private StageData stageData;
    private int stageIndex = -1;
    private ShopData currentShop;
    private bool isLocked = true;
    private bool showTop = true;
    private bool showBottom = true;
    private bool showLeft = true;
    private bool showRight = true;
    private RectTransform lineTop;
    private RectTransform lineBottom;
    private RectTransform lineLeft;
    private RectTransform lineRight;

    public bool ShowTop { get => showTop; set { showTop = value; if (lineTop) lineTop.gameObject.SetActive(value); } }
    public bool ShowBottom { get => showBottom; set { showBottom = value; if (lineBottom) lineBottom.gameObject.SetActive(value); } }
    public bool ShowLeft { get => showLeft; set { showLeft = value; if (lineLeft) lineLeft.gameObject.SetActive(value); } }
    public bool ShowRight { get => showRight; set { showRight = value; if (lineRight) lineRight.gameObject.SetActive(value); } }

    private void Awake()
    {
        AutoAssignBackground();
        EnsureLines();
        UpdateLines();
    }

    private void OnEnable()
    {
        EnsureLines();
        UpdateLines();
    }

    private void OnRectTransformDimensionsChange()
    {
        UpdateLines();
    }

    public void OnTap()
    {
        if (isLocked || stageData == null)
            return;

        OnStageCellTapped?.Invoke(stageData, stageIndex);
    }

    public void SetStageData(ShopData shopData, StageData stageData)
    {
        this.stageData = stageData;
        currentShop = shopData;
        stageIndex = stageData != null ? stageData.Number : -1;

        if (stageNumber != null)
            stageNumber.text = stageData != null ? stageData.Number.ToString() : string.Empty;

        UpdateUIVisibility();
        UpdateLockCoverVisibility();
    }

    private void UpdateUIVisibility()
    {
        if (currentShop == null || stageData == null || SaveSystem.Instance == null)
        {
            SetMark(false);
            SetStars(0);
            return;
        }

        bool isCleared = SaveSystem.Instance.IsStageCleared(currentShop.Type, stageData.Number);
        SetMark(isCleared);
        if (isCleared && markOn != null)
            markOn.color = currentShop.MainColor;

        SetStars(SaveSystem.Instance.GetStageStarCount(currentShop.Type, stageData.Number));
    }

    private void UpdateLockCoverVisibility()
    {
        if (currentShop == null || stageData == null || SaveSystem.Instance == null)
        {
            SetLocked(true);
            return;
        }

        if (stageData.Number <= 1)
        {
            SetLocked(false);
            return;
        }

        bool previousCleared = SaveSystem.Instance.IsStageCleared(currentShop.Type, stageData.Number - 1);
        SetLocked(!previousCleared);
    }

    private void SetMark(bool active)
    {
        if (markOn != null)
            markOn.gameObject.SetActive(active);
    }

    private void SetStars(int starCount)
    {
        if (stars == null)
            return;

        for (int i = 0; i < stars.Length; i++)
        {
            if (stars[i] != null)
                stars[i].gameObject.SetActive(i < starCount);
        }
    }

    private void SetLocked(bool locked)
    {
        isLocked = locked;
        if (lockCover != null)
            lockCover.gameObject.SetActive(locked);
    }

    private void AutoAssignBackground()
    {
        if (background != null)
            return;

        for (int i = 0; i < transform.childCount; i++)
        {
            if (transform.GetChild(i) is RectTransform rt && rt.GetComponent<Image>() != null)
            {
                background = rt;
                break;
            }
        }
    }

    private void EnsureLines()
    {
        if (background == null)
            return;

        lineTop = EnsureLine(background, "LineTop");
        lineBottom = EnsureLine(background, "LineBottom");
        lineLeft = EnsureLine(background, "LineLeft");
        lineRight = EnsureLine(background, "LineRight");
        ApplyVisibility();
    }

    private RectTransform EnsureLine(RectTransform parent, string name)
    {
        var existing = parent.Find(name);
        RectTransform rt;
        if (existing == null)
        {
            var go = new GameObject(name, typeof(RectTransform), typeof(Image));
            rt = go.GetComponent<RectTransform>();
            rt.SetParent(parent, false);
            rt.gameObject.layer = parent.gameObject.layer;
        }
        else
        {
            rt = (RectTransform)existing;
        }

        var image = rt.GetComponent<Image>();
        image.color = lineColor;
        image.raycastTarget = false;
        rt.SetAsLastSibling();
        return rt;
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
        if (!pixelPerfect)
            return thicknessPixels;

        var canvas = GetComponentInParent<Canvas>();
        float scale = canvas != null && canvas.rootCanvas != null ? canvas.rootCanvas.scaleFactor : 1f;
        return thicknessPixels / Mathf.Max(1f, scale);
    }

    private void UpdateLines()
    {
        if (background == null || lineTop == null || lineBottom == null || lineLeft == null || lineRight == null)
            return;

        float t = Mathf.Max(0.5f, GetUnitThickness());
        float half = t * 0.5f;

        SetHorizontalLine(lineTop, 1f, -half, t);
        SetHorizontalLine(lineBottom, 0f, half, t);
        SetVerticalLine(lineLeft, 0f, half, t);
        SetVerticalLine(lineRight, 1f, -half, t);
        ApplyVisibility();
    }

    private static void SetHorizontalLine(RectTransform rt, float anchorY, float y, float thickness)
    {
        rt.anchorMin = new Vector2(0f, anchorY);
        rt.anchorMax = new Vector2(1f, anchorY);
        rt.pivot = new Vector2(0.5f, 0.5f);
        rt.sizeDelta = new Vector2(0f, thickness);
        rt.anchoredPosition = new Vector2(0f, y);
    }

    private static void SetVerticalLine(RectTransform rt, float anchorX, float x, float thickness)
    {
        rt.anchorMin = new Vector2(anchorX, 0f);
        rt.anchorMax = new Vector2(anchorX, 1f);
        rt.pivot = new Vector2(0.5f, 0.5f);
        rt.sizeDelta = new Vector2(thickness, 0f);
        rt.anchoredPosition = new Vector2(x, 0f);
    }
}
