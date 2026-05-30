using System;
using System.Collections;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UIElements;

namespace DoggyChef
{
    // 원본 GemHunterMatch의 UIHandler를 MyMatch3 구조에 맞게 그대로 복제한 버전입니다.
    // - Win/Lose:  EndTitleContent(타이틀) + EndScreen(결과창) 분리 구조
    // - UpdateTopBarData: CoinLabel / LiveLabel / StarLabel 갱신
    public class UIHandler : MonoBehaviour
    {
        public static UIHandler Instance { get; private set; }

        public UIDocument Document;

        // ── Cover (페이드) ──
        private VisualElement m_CoverElement;
        private Action m_FadeCallback;

        // ── Canvas HUD 페이드인 ──
        private CanvasGroup m_HUDCanvasGroup;

        // ── 종료 화면 (원본: EndTitleContent + EndScreen 분리) ──
        private VisualElement m_EndTitleContent;
        private VisualElement m_WinTitle;
        private VisualElement m_LoseTitle;
        private VisualElement m_EndScreen;

        // 원본 EndScreen 내의 통계 레이블
        private Label m_CoinLabel;
        private Label m_LiveLabel;
        private Label m_StarLabel;

        // ─────────────────────────────────────────────────────────────
        //  Awake: UI 요소 캐싱 (원본 Start와 통합 단순화)
        // ─────────────────────────────────────────────────────────────
        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(gameObject);
                return;
            }
            Instance = this;
        }

        // ─────────────────────────────────────────────────────────────
        //  Start: 원본 UIHandler.Start와 동일한 흐름
        // ─────────────────────────────────────────────────────────────
        private void Start()
        {
            var root = Document.rootVisualElement;

            // 종료 화면
            m_EndTitleContent = root.Q<VisualElement>("EndTitleContent");
            m_WinTitle = root.Q<VisualElement>("WinTitle");
            m_LoseTitle = root.Q<VisualElement>("LoseTitle");
            m_EndScreen = root.Q<VisualElement>("EndScreen");

            m_CoinLabel = root.Q<Label>("CoinLabel");
            m_LiveLabel = root.Q<Label>("LiveLabel");
            m_StarLabel = root.Q<Label>("StarLabel");

            // 초기: 종료화면 숨김
            m_EndTitleContent.style.display = DisplayStyle.None;
            m_EndScreen.style.display = DisplayStyle.None;

            // ReplayButton
            var replayBtn = root.Q<Button>("ReplayButton");
            if (replayBtn != null)
                replayBtn.clicked += () =>
                    FadeOut(() => SceneManager.LoadScene(
                        SceneManager.GetActiveScene().name, LoadSceneMode.Single));

            // SelectLevelButton
            var selectBtn = root.Q<Button>("SelectLevelButton");
            if (selectBtn != null)
                selectBtn.clicked += () =>
                    FadeOut(() => SceneManager.LoadScene(
                        SceneManager.GetActiveScene().name, LoadSceneMode.Single));

            // Cover 초기화
            m_CoverElement = root.Q<VisualElement>("Cover");
            m_CoverElement.AddToClassList("no-transition");
            m_CoverElement.style.opacity = 1.0f;

            // Canvas HUD 초기화: 씬의 Canvas에서 CanvasGroup을 자동으로 가져오거나 추가
            var canvas = FindAnyObjectByType<Canvas>();
            if (canvas != null)
            {
                m_HUDCanvasGroup = canvas.GetComponent<CanvasGroup>();
                if (m_HUDCanvasGroup == null)
                    m_HUDCanvasGroup = canvas.gameObject.AddComponent<CanvasGroup>();
                m_HUDCanvasGroup.alpha = 0f;
            }

            // 페이드인(검은 화면 → 게임 화면)
            m_CoverElement.RegisterCallback<TransitionEndEvent>(evt =>
            {
                m_FadeCallback?.Invoke();
                m_FadeCallback = null;
            });

            // ── GameState 이벤트 구독 ──
            // Closing → 보드 입력 차단 / Result → ShowEnd() 호출
            GameManager.OnStateChanged += OnGameStateChanged;

            // 상단바 초기값
            UpdateTopBarData();

            // 페이드인 시작
            StartCoroutine(WaitToFadeIn());
        }

        private IEnumerator WaitToFadeIn()
        {
            // UI Toolkit이 초기 불투명도 1을 완전히 인지할 수 있도록 짧은 딜레이 부여
            yield return new WaitForSeconds(0.1f);
            m_CoverElement.RemoveFromClassList("no-transition");

            // 딜레이 이후 다시 한 프레임 넘겨서 속성이 반영된 뒤 전환(FadeOut 같은 FadeIn)을 트리거
            yield return null;
            FadeIn(null);

            // Cover의 CSS transition-duration(2s)과 동일하게 Canvas HUD도 페이드인
            if (m_HUDCanvasGroup != null)
                StartCoroutine(FadeCanvasGroup(m_HUDCanvasGroup, 0f, 1f, 2f));
        }

        // ─────────────────────────────────────────────────────────────
        //  Init: 원본 UIHandler.Init() — 레벨 시작 시 호출
        //  (레벨 재시작 시 목표 표시 등을 초기화)
        // ─────────────────────────────────────────────────────────────
        public void Init()
        {
            if (m_EndTitleContent != null)
                m_EndTitleContent.style.display = DisplayStyle.None;
            if (m_EndScreen != null)
                m_EndScreen.style.display = DisplayStyle.None;
        }

        public void UpdateTopBarData()
        {
            int earned = LevelData.Instance != null ? LevelData.Instance.EarnedMoney : GameManager.Instance.Coins;
            if (m_CoinLabel != null) m_CoinLabel.text = earned.ToString();
            if (m_LiveLabel != null) m_LiveLabel.text = GameManager.Instance.Lives.ToString();
            if (m_StarLabel != null) m_StarLabel.text = GameManager.Instance.Stars.ToString();
        }

        // ─────────────────────────────────────────────────────────────
        //  [타이쿤 추가] GameState 이벤트 핸들러
        // ─────────────────────────────────────────────────────────────
        //  GameManager.OnStateChanged를 구독하여 상태별 UI 처리를 합니다.
        //
        //  Closing → 보드 입력 차단 (플레이어가 더 이상 보석을 움직일 수 없음)
        //  Result  → 승/패 결과 화면 표시
        // ─────────────────────────────────────────────────────────────
        private void OnGameStateChanged(GameManager.GameState state)
        {
            if (state == GameManager.GameState.Result)
            {
                // 모든 손님이 나간 후 보드 입력을 차단하고 결과 화면을 표시합니다.
                if (GameManager.Instance != null && GameManager.Instance.Board != null)
                    GameManager.Instance.Board.ToggleInput(false);

                // ShowEnd()에서 EarnedMoney vs GoalGold를 비교하여 승/패를 결정합니다.
                ShowEnd();
            }
        }

        private void OnDestroy()
        {
            // 씬 언로드 시 구독 해제 (메모리 누수 방지)
            GameManager.OnStateChanged -= OnGameStateChanged;
        }

        // ─────────────────────────────────────────────────────────────
        //  [타이쿤 수정] 게임 종료 — ShowEnd / ShowWin / ShowLose
        // ─────────────────────────────────────────────────────────────
        //  기존: GoalLeft == 0 이면 승리
        //  타이쿤: EarnedMoney >= GoalGold 이면 승리
        //
        //  두 조건이 공존할 수 있도록:
        //  1. CurrentStage 있음 → 타이쿤 승리 조건 사용
        //  2. CurrentStage 없음 → 기존 Match-3 승리 조건 사용
        // ─────────────────────────────────────────────────────────────
        public void ShowEnd()
        {
            UpdateTopBarData();

            // 타이쿤: EarnedMoney vs GoalGold 기반으로 승/패 판단
            bool isTycoonWin = LevelData.Instance != null
                && LevelData.Instance.CurrentStage != null
                && LevelData.Instance.EarnedMoney >= LevelData.Instance.CurrentStage.GoalGold;

            // 기존 Match-3 목표 달성 (타이쿤 아닐 때 폴백)
            bool isGoalWin = LevelData.Instance != null && LevelData.Instance.GoalLeft == 0;

            if (isTycoonWin || (LevelData.Instance?.CurrentStage == null && isGoalWin))
                ShowWin();
            else
                ShowLose();
        }

        private void ShowWin()
        {
            if (StageSelectionData.HasSelection && LevelData.Instance?.CurrentStage != null)
            {
                var stage = LevelData.Instance.CurrentStage;
                SaveSystem.Instance.RecordStageResult(
                    StageSelectionData.SelectedShop.Type,
                    stage.Number,
                    LevelData.Instance.EarnedMoney,
                    stage.GoalGold,
                    stage.GoalPerpect);
            }

            m_EndTitleContent.style.display = DisplayStyle.Flex;
            m_LoseTitle.style.display = DisplayStyle.None;
            m_WinTitle.style.display = DisplayStyle.Flex;

            StartCoroutine(ShowEndScreen());
        }

        private void ShowLose()
        {
            m_EndTitleContent.style.display = DisplayStyle.Flex;
            m_WinTitle.style.display = DisplayStyle.None;
            m_LoseTitle.style.display = DisplayStyle.Flex;

            StartCoroutine(ShowEndScreen());
        }

        // 원본: IEnumerator ShowEndControl — 3초 후 EndScreen 표시
        private IEnumerator ShowEndScreen()
        {
            yield return new WaitForSeconds(3.0f);
            UpdateTopBarData();
            m_EndScreen.style.display = DisplayStyle.Flex;
        }

        // ─────────────────────────────────────────────────────────────
        //  페이드인/아웃 — 원본 FadeIn / FadeOut
        // ─────────────────────────────────────────────────────────────
        public void FadeIn(Action onFadeFinished)
        {
            m_CoverElement.style.opacity = 0.0f;
            m_FadeCallback += onFadeFinished;
        }

        public void FadeOut(Action onFadeFinished)
        {
            m_CoverElement.style.opacity = 1.0f;
            m_FadeCallback += onFadeFinished;
        }

        private IEnumerator FadeCanvasGroup(CanvasGroup group, float from, float to, float duration)
        {
            group.alpha = from;
            float elapsed = 0f;
            while (elapsed < duration)
            {
                elapsed += Time.deltaTime;
                group.alpha = Mathf.Lerp(from, to, elapsed / duration);
                yield return null;
            }
            group.alpha = to;
        }

        // ─────────────────────────────────────────────────────────────
        //  전체 표시 토글
        // ─────────────────────────────────────────────────────────────
        public void Display(bool displayed)
        {
            Document.rootVisualElement.style.display =
                displayed ? DisplayStyle.Flex : DisplayStyle.None;
        }
    }
}
