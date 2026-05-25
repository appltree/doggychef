using TS.PageSlider;
using UnityEngine;
using System.Collections;
using System;
using UnityEngine.UI;
using TMPro;

[RequireComponent(typeof(RectTransform))]
public class StagePanel : MonoBehaviour
{
    // StagePanel이 닫힐 때 발생하는 이벤트
    public static event Action OnStagePanelClosed;

    [SerializeField] private Image titleBar;
    [SerializeField] private TMP_Text title;
    [SerializeField] private Image shopIcon;

    [SerializeField] private PageSlider pageSlider;
    [SerializeField] private PageView pageViewPrefab;
    [SerializeField] private GameObject stagePagePrefab;

    private readonly float slideDuration = 0.35f;

    private RectTransform rt;

    private Vector2 baseVisiblePos; // 초기 배치 기준 우측 위치
    private Vector2 visiblePos; // 온스크린 위치(현재 선택된 Side)
    private Vector2 hiddenPos;  // 화면 위쪽으로 숨긴 위치

    private Coroutine slideCoroutine;
    private bool isOpen = false;

    private StageManager stageManager;
    private int currentShopIndex = 0; // 현재 선택된 shop의 인덱스

    void Awake()
    {
        stageManager = FindFirstObjectByType<StageManager>();

        rt = GetComponent<RectTransform>();

        // 현재 배치된 위치를 화면상 기준(보이는 위치-우측)으로 사용
        baseVisiblePos = rt.anchoredPosition;
        visiblePos = baseVisiblePos;
        hiddenPos = visiblePos + new Vector2(0f, rt.rect.height);

        // 초기에는 보이지 않음
        rt.anchoredPosition = hiddenPos;
        isOpen = false;
    }

    void Start()
    {
        // 최초 1회만 페이지 생성
        InitPages();

        // Map의 OnFocus 이벤트 구독
        Map.OnMapFocused += OnMapFocused;
    }

    void OnDestroy()
    {
        // 이벤트 구독 해제
        Map.OnMapFocused -= OnMapFocused;
    }

    // UI 버튼에서 호출되는 패널 닫기 메서드
    public void OnStageClose()
    {
        ClosePanel(() =>
        {
            // 패널이 완전히 닫힌 후 이벤트 발생
            OnStagePanelClosed?.Invoke();
        });
    }

    // Map에서 포커스 이벤트 발생 시 호출
    private void OnMapFocused(int index, bool isLeftSide)
    {
        currentShopIndex = index; // 현재 선택된 shop 인덱스 저장
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
        slideCoroutine = StartCoroutine(Slide(hiddenPos, visiblePos, true));
    }

    private void ClosePanel(System.Action onClosed = null)
    {
        if (!isOpen)
        {
            onClosed?.Invoke();
            return;
        }

        isOpen = false;
        if (slideCoroutine != null)
            StopCoroutine(slideCoroutine);
        slideCoroutine = StartCoroutine(Slide(visiblePos, hiddenPos, false, onClosed));
    }

    private IEnumerator Slide(Vector2 from, Vector2 to, bool opening, System.Action onDone = null)
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
        slideCoroutine = null;
        onDone?.Invoke();
    }

    /// <summary>
    /// shops 배열을 참고하여 titleBar 색상, title 텍스트, shopIcon을 업데이트합니다.
    /// </summary>
    /// <param name="index">shops 배열의 인덱스</param>
    private void UpdateShopInfo(int index)
    {

        var shop = stageManager.GetShop(index);
        // titleBar 색상 설정
        titleBar.color = shop.MainColor;

        // title 텍스트 설정
        title.text = shop.ShopName;

        // shopIcon 이미지 설정
        shopIcon.sprite = shop.Symbol;
    }

    /// <summary>
    /// 스테이지 패널이 왼쪽/오른쪽 어느 위치에서 열릴지 설정합니다.
    /// 패널이 닫혀 있을 때 호출하면, 내부 hidden/visible 기준을 갱신합니다.
    /// </summary>
    /// <param name="left">true면 왼쪽, false면 오른쪽</param>
    private void SetSideLeft(bool left)
    {
        if (rt == null) rt = GetComponent<RectTransform>();

        Vector2 chosenVisible;
        // 기본: 현재 배치가 오른쪽 기준. 왼쪽 요청 시 패널 너비+여백만큼 왼쪽으로 이동.
        chosenVisible = left ? new Vector2(-rt.rect.width / 2 + 200, baseVisiblePos.y) : baseVisiblePos;

        // 현재 사이드에 맞춰 visible/hidden 재설정
        visiblePos = chosenVisible;
        hiddenPos = visiblePos + new Vector2(0f, rt.rect.height);

        // 닫혀있는 상태라면 숨김 위치로 바로 이동해 깜빡임 방지
        if (!isOpen)
        {
            rt.anchoredPosition = hiddenPos;
        }
    }

    // PageSlider에 PageView 프리팹으로 현재 shop의 stages 개수에 따라 페이지를 추가
    private void InitPages()
    {
        // 현재 선택된 shop 정보 가져오기
        var shop = stageManager.GetShop(currentShopIndex);
        if (shop == null || shop.Stages == null)
        {
            pageSlider.Clear();
            return;
        }

        // 스테이지 개수에 따라 필요한 페이지 수 계산 (한 페이지당 12개)
        int stagesCount = shop.Stages.Length;


        // 기존 페이지 초기화 (stagesCount가 0이어도 실행)
        pageSlider.Clear();

        // stagesCount가 0인 경우 빈 페이지 하나라도 생성할지, 아니면 그냥 끝낼지 결정
        if (stagesCount == 0)
        {
            return;
        }

        int pagesNeeded = Mathf.CeilToInt((float)stagesCount / 12);
        pagesNeeded = Mathf.Max(1, pagesNeeded); // 최소 1페이지

        for (int i = 0; i < pagesNeeded; i++)
        {
            // 콘텐츠 프리팹 인스턴스화 후 AddPage로 전달
            var view = Instantiate(pageViewPrefab);
            view.name = $"PageContent_{i + 1}";

            var contentRt = view.GetComponent<RectTransform>();
            if (contentRt == null)
            {
                Debug.LogWarning("StagePanel: PageView 프리팹에 RectTransform이 없습니다.");
                Destroy(view.gameObject);
                continue;
            }

            // view의 자식으로 stagePagePrefab 추가 및 풀스트레치 세팅
            if (stagePagePrefab != null)
            {
                var stage = Instantiate(stagePagePrefab, view.transform);
                stage.name = $"StagePage_{i + 1}";
                var srt = stage.GetComponent<RectTransform>();
                if (srt != null)
                {
                    srt.anchorMin = Vector2.zero;
                    srt.anchorMax = Vector2.one;
                    srt.offsetMin = Vector2.zero;
                    srt.offsetMax = Vector2.zero;
                    srt.anchoredPosition = Vector2.zero;
                    srt.localScale = Vector3.one;
                }
                else
                {
                    Debug.LogWarning("StagePanel: stagePagePrefab에 RectTransform이 없습니다.");
                }

                // 이 페이지에 필요한 스테이지 셀 개수 계산
                int startIndex = i * 12;
                int remainingStages = stagesCount - startIndex;
                int cellsForThisPage = Mathf.Min(12, remainingStages);

                // StagePage 컴포넌트에 셀 개수와 스테이지 데이터 설정
                var stagePage = stage.GetComponent<StagePage>();
                if (stagePage != null)
                {
                    stagePage.SetCellCount(cellsForThisPage);
                    stagePage.SetStageData(shop, shop.Stages, i); // 전체 스테이지 데이터와 페이지 인덱스 전달
                }
            }

            // PageSlider가 내부에서 페이지 컨테이너를 만들고 부모를 재지정합니다.
            pageSlider.AddPage(contentRt);
        }

    }
}
