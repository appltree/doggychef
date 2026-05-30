using System;
using System.Collections;
using TMPro;
using TS.PageSlider;
using UnityEngine;
using UnityEngine.UI;

namespace DoggyChef
{
    [RequireComponent(typeof(RectTransform))]
    public class StagePanel : MonoBehaviour
    {
        public static event Action OnStagePanelClosed;

        [SerializeField] private Image titleBar;
        [SerializeField] private TMP_Text title;
        [SerializeField] private Image shopIcon;
        [SerializeField] private PageSlider pageSlider;
        [SerializeField] private PageView pageViewPrefab;
        [SerializeField] private GameObject stagePagePrefab;

        private const int StagesPerPage = 12;
        private readonly float slideDuration = 0.35f;

        private RectTransform rt;
        private Vector2 baseVisiblePos;
        private Vector2 visiblePos;
        private Vector2 hiddenPos;
        private Coroutine slideCoroutine;
        private bool isOpen;
        private StageManager stageManager;
        private int currentShopIndex;

        private void Awake()
        {
            stageManager = FindAnyObjectByType<StageManager>();
            rt = GetComponent<RectTransform>();

            baseVisiblePos = rt.anchoredPosition;
            visiblePos = baseVisiblePos;
            hiddenPos = visiblePos + new Vector2(0f, rt.rect.height);

            rt.anchoredPosition = hiddenPos;
            isOpen = false;
        }

        private void Start()
        {
            InitPages();
            Map.OnMapFocused += OnMapFocused;
        }

        private void OnDestroy()
        {
            Map.OnMapFocused -= OnMapFocused;
        }

        public void OnStageClose()
        {
            ClosePanel(() => OnStagePanelClosed?.Invoke());
        }

        private void OnMapFocused(int index, bool isLeftSide)
        {
            currentShopIndex = index;
            SetSideLeft(isLeftSide);
            UpdateShopInfo(index);
            InitPages();
            Open();
        }

        private void Open()
        {
            if (isOpen)
                return;

            isOpen = true;
            if (slideCoroutine != null)
                StopCoroutine(slideCoroutine);

            slideCoroutine = StartCoroutine(Slide(hiddenPos, visiblePos, () => slideCoroutine = null));
        }

        private void ClosePanel(Action onClosed = null)
        {
            if (!isOpen)
            {
                onClosed?.Invoke();
                return;
            }

            isOpen = false;
            if (slideCoroutine != null)
                StopCoroutine(slideCoroutine);

            slideCoroutine = StartCoroutine(Slide(visiblePos, hiddenPos, () =>
            {
                slideCoroutine = null;
                onClosed?.Invoke();
            }));
        }

        private IEnumerator Slide(Vector2 from, Vector2 to, Action onDone = null)
        {
            float duration = Mathf.Max(0.01f, slideDuration);
            float t = 0f;

            while (t < duration)
            {
                t += Time.unscaledDeltaTime;
                float p = Mathf.Clamp01(t / duration);
                float e = Mathf.SmoothStep(0f, 1f, p);
                rt.anchoredPosition = Vector2.LerpUnclamped(from, to, e);
                yield return null;
            }

            rt.anchoredPosition = to;
            onDone?.Invoke();
        }

        private void UpdateShopInfo(int index)
        {
            var shop = stageManager != null ? stageManager.GetShop(index) : null;
            if (shop == null)
                return;

            if (titleBar != null)
                titleBar.color = shop.MainColor;
            if (title != null)
                title.text = shop.ShopName;
            if (shopIcon != null)
                shopIcon.sprite = shop.Symbol;
        }

        private void SetSideLeft(bool left)
        {
            if (rt == null)
                rt = GetComponent<RectTransform>();

            visiblePos = left
                ? new Vector2(-rt.rect.width / 2f + 200f, baseVisiblePos.y)
                : baseVisiblePos;
            hiddenPos = visiblePos + new Vector2(0f, rt.rect.height);

            if (!isOpen)
                rt.anchoredPosition = hiddenPos;
        }

        private void InitPages()
        {
            if (pageSlider == null)
                return;

            var shop = stageManager != null ? stageManager.GetShop(currentShopIndex) : null;
            if (shop == null || shop.Stages == null)
            {
                pageSlider.Clear();
                return;
            }

            pageSlider.Clear();
            int stagesCount = shop.Stages.Length;
            if (stagesCount == 0)
                return;

            int pagesNeeded = Mathf.Max(1, Mathf.CeilToInt(stagesCount / (float)StagesPerPage));
            for (int i = 0; i < pagesNeeded; i++)
            {
                var view = Instantiate(pageViewPrefab);
                view.name = $"PageContent_{i + 1}";

                var contentRt = view.GetComponent<RectTransform>();
                if (contentRt == null)
                {
                    Destroy(view.gameObject);
                    continue;
                }

                if (stagePagePrefab != null)
                {
                    var stage = Instantiate(stagePagePrefab, view.transform);
                    stage.name = $"StagePage_{i + 1}";

                    if (stage.transform is RectTransform srt)
                    {
                        srt.anchorMin = Vector2.zero;
                        srt.anchorMax = Vector2.one;
                        srt.offsetMin = Vector2.zero;
                        srt.offsetMax = Vector2.zero;
                        srt.anchoredPosition = Vector2.zero;
                        srt.localScale = Vector3.one;
                    }

                    int remainingStages = stagesCount - (i * StagesPerPage);
                    int cellsForThisPage = Mathf.Min(StagesPerPage, remainingStages);
                    var stagePage = stage.GetComponent<StagePage>();
                    if (stagePage != null)
                    {
                        stagePage.SetCellCount(cellsForThisPage);
                        stagePage.SetStageData(shop, shop.Stages, i);
                    }
                }

                pageSlider.AddPage(contentRt);
            }
        }
    }

} // namespace DoggyChef
