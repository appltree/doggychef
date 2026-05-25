using System.Collections;
using TMPro;
using UnityEngine;

/// <summary>
/// Table의 UI 관련 기능을 담당하는 컴포넌트
/// - 상태 텍스트 표시
/// - 레시피 시각화 (풍선, 접시)
/// - 골드 팝업 표시
/// </summary>
public class TableUI : MonoBehaviour
{
    [Header("UI 컴포넌트")]
    [SerializeField] private TextMeshProUGUI stateText;
    [SerializeField] private SpriteRenderer balloonSpriteRender;
    [SerializeField] private SpriteRenderer dishSpriteRender;

    [Header("팝업 설정")]
    [SerializeField] private TMP_FontAsset goldPopupFont;

    private readonly float goldPopupDuration = 0.9f;
    private readonly float goldPopupRise = 1.0f;

    // 색상 및 오프셋 설정
    private Color goldPopupColor = new(1f, 0.9f, 0.2f, 1f);
    private Vector3 goldPopupOffset = new(0f, 0.6f, 0f);
    private Color baseTextColor;

    // 오버레이 렌더러
    private SpriteRenderer balloonRecipeOverlay;
    private SpriteRenderer dishRecipeOverlay;
    private bool overlaysInitialized;

    // 피버 블링크 관련
    private readonly bool feverBlinkEnabled = true;
    private readonly float feverBlinkFrequency = 2.0f;
    private readonly float feverBlinkMinAlpha = 0.35f;
    private Coroutine feverBlinkRoutine;

    // Table 참조
    private Table table;

    private void Awake()
    {
        table = GetComponent<Table>();
    }

    private void Start()
    {
        // Fever 변경 시 즉시 UI 업데이트
        var gm = FindFirstObjectByType<GameManager>();
        if (gm != null)
        {
            gm.OnFeverStateChanged += HandleFeverChangedForUI;
            // 시작 시점 피버 상태 반영
            baseTextColor = stateText != null ? stateText.color : Color.white;
            if (gm.IsFever) TryStartFeverBlink();
        }
        EnsureOverlayRenderers();
        UpdateUI();
    }

    private void OnDestroy()
    {
        var gm = FindFirstObjectByType<GameManager>();
        if (gm != null)
        {
            gm.OnFeverStateChanged -= HandleFeverChangedForUI;
        }
        StopFeverBlink();
    }

    public void UpdateUI()
    {
        if (stateText == null || table == null) return;

        var state = table.GetState();
        var currentOrder = table.GetCurrentOrder();

        if ((state == Table.TableState.Ordered || state == Table.TableState.Cooked) && currentOrder != null)
        {
            int price = currentOrder.SellPrice;
            var gm = FindFirstObjectByType<GameManager>();
            bool fever = gm != null && gm.IsFever;

            if (fever)
            {
                stateText.text = $"{currentOrder.RecipeName} ({price:N0}G <size=90%><color=#FFD700>x2</color></size>)";
                TryStartFeverBlink();
            }
            else
            {
                stateText.text = $"{currentOrder.RecipeName} ({price:N0}G)";
                StopFeverBlink();
            }
        }
        else
        {
            stateText.text = "";
            StopFeverBlink();
        }

        UpdateRecipeVisual();
    }

    public void ShowGoldPopup(int amount, Vector3 worldPos)
    {
        var go = new GameObject("GoldPopupText", typeof(TextMeshPro));
        go.transform.position = worldPos + goldPopupOffset;

        var tmp = go.GetComponent<TextMeshPro>();
        if (goldPopupFont != null) tmp.font = goldPopupFont;

        var gm = FindFirstObjectByType<GameManager>();
        bool fever = gm != null && gm.IsFever;
        tmp.text = fever ? $"<b>+{amount:N0} <size=80%><color=#FFD700>x2</color></size></b>" : $"+{amount:N0}";
        tmp.color = goldPopupColor;
        tmp.fontSize = 3.5f;
        tmp.alignment = TextAlignmentOptions.Center;
        tmp.textWrappingMode = TextWrappingModes.NoWrap;
        tmp.richText = true;

        StartCoroutine(CoGoldPopupAnimate(go, tmp));
    }

    public void ShowCustomGoldPopup(string displayText, Vector3 worldPos)
    {
        var go = new GameObject("GoldPopupText", typeof(TextMeshPro));
        go.transform.position = worldPos + goldPopupOffset;

        var tmp = go.GetComponent<TextMeshPro>();
        if (goldPopupFont != null) tmp.font = goldPopupFont;
        tmp.text = displayText;
        tmp.color = goldPopupColor;
        tmp.fontSize = 3.5f;
        tmp.alignment = TextAlignmentOptions.Center;
        tmp.textWrappingMode = TextWrappingModes.NoWrap;
        tmp.richText = true;

        StartCoroutine(CoGoldPopupAnimate(go, tmp));
    }

    public void HideDishRecipeOverlay()
    {
        if (dishRecipeOverlay != null)
        {
            dishRecipeOverlay.enabled = false;
            dishRecipeOverlay.sprite = null;
        }

        if (dishSpriteRender != null)
        {
            dishSpriteRender.enabled = false;
        }
    }

    private void HandleFeverChangedForUI(bool on)
    {
        UpdateUI();
    }

    private void UpdateRecipeVisual()
    {
        if (table == null) return;

        var currentOrder = table.GetCurrentOrder();
        var state = table.GetState();

        Sprite recipeSprite = currentOrder != null ? currentOrder.ResultSprite : null;
        EnsureOverlayRenderers();
        bool hasRecipe = recipeSprite != null;

        if (balloonRecipeOverlay != null)
        {
            bool showBalloonOverlay = (state == Table.TableState.Ordered || state == Table.TableState.Cooked) && hasRecipe;
            balloonRecipeOverlay.sprite = showBalloonOverlay ? recipeSprite : null;
            balloonRecipeOverlay.enabled = balloonRecipeOverlay.sprite != null;
        }

        if (dishRecipeOverlay != null)
        {
            bool showDish = (state == Table.TableState.Served) && hasRecipe;
            dishRecipeOverlay.sprite = showDish ? recipeSprite : null;
            dishRecipeOverlay.enabled = dishRecipeOverlay.sprite != null;
        }

        if (balloonSpriteRender != null)
            balloonSpriteRender.enabled = (state == Table.TableState.Ordered || state == Table.TableState.Cooked);
        if (dishSpriteRender != null)
            dishSpriteRender.enabled = (state == Table.TableState.Served);
    }

    private void EnsureOverlayRenderers()
    {
        if (overlaysInitialized) return;

        if (balloonSpriteRender != null && balloonRecipeOverlay == null)
        {
            balloonRecipeOverlay = CreateOverlayChild(balloonSpriteRender, "BalloonRecipeOverlay");
        }

        if (dishSpriteRender != null && dishRecipeOverlay == null)
        {
            dishRecipeOverlay = CreateOverlayChild(dishSpriteRender, "DishRecipeOverlay");
        }

        overlaysInitialized = true;
    }

    private SpriteRenderer CreateOverlayChild(SpriteRenderer parent, string name)
    {
        var go = new GameObject(name);
        go.transform.SetParent(parent.transform, false);
        go.transform.localPosition = Vector3.zero;
        var sr = go.AddComponent<SpriteRenderer>();
        sr.sortingLayerID = parent.sortingLayerID;
        sr.sortingOrder = parent.sortingOrder + 1;
        sr.enabled = false;
        return sr;
    }

    private void TryStartFeverBlink()
    {
        if (!feverBlinkEnabled || stateText == null) return;
        if (table == null) return;

        var state = table.GetState();
        if (!(state == Table.TableState.Ordered || state == Table.TableState.Cooked)) return;
        if (table.GetCurrentOrder() == null) return;
        if (feverBlinkRoutine != null) return;

        baseTextColor = stateText.color;
        feverBlinkRoutine = StartCoroutine(CoFeverBlink());
    }

    private void StopFeverBlink()
    {
        if (feverBlinkRoutine != null)
        {
            StopCoroutine(feverBlinkRoutine);
            feverBlinkRoutine = null;
        }
        if (stateText != null)
        {
            stateText.color = baseTextColor == default ? Color.white : baseTextColor;
        }
    }

    private IEnumerator CoFeverBlink()
    {
        float t = 0f;
        float freq = Mathf.Max(0.1f, feverBlinkFrequency);
        float minA = Mathf.Clamp01(feverBlinkMinAlpha);
        Color baseCol = baseTextColor == default ? (stateText != null ? stateText.color : Color.white) : baseTextColor;

        while (isActiveAndEnabled && stateText != null)
        {
            t += Time.deltaTime * freq * Mathf.PI * 2f;
            float s = (Mathf.Sin(t) * 0.5f + 0.5f);
            float a = Mathf.Lerp(minA, 1f, s);
            var c = baseCol; c.a = a;
            stateText.color = c;
            yield return null;
        }
        feverBlinkRoutine = null;
    }

    private IEnumerator CoGoldPopupAnimate(GameObject go, TextMeshPro tmp)
    {
        float dur = Mathf.Max(0.1f, goldPopupDuration);
        Vector3 start = go.transform.position;
        Vector3 end = start + Vector3.up * goldPopupRise;
        Color startColor = tmp.color;
        Color endColor = new Color(startColor.r, startColor.g, startColor.b, 0f);

        float t = 0f;
        Vector3 startScale = go.transform.localScale;
        Vector3 peakScale = startScale * 1.15f;

        while (t < dur)
        {
            t += Time.deltaTime;
            float k = Mathf.Clamp01(t / dur);

            go.transform.position = Vector3.LerpUnclamped(start, end, k);
            tmp.color = Color.LerpUnclamped(startColor, endColor, k);

            float s = k < 0.25f ? (k / 0.25f) : Mathf.Clamp01(1f - (k - 0.25f) / 0.75f);
            go.transform.localScale = Vector3.LerpUnclamped(startScale, peakScale, s);

            if (Camera.main != null)
            {
                go.transform.forward = Camera.main.transform.forward;
            }

            yield return null;
        }

        Destroy(go);
    }
}