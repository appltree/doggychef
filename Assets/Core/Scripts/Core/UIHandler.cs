using System;
using System.Collections;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UIElements;

namespace DoggyChef
{
    public class UIHandler : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  Inspector 연결 필드
        // ════════════════════════════════════════════════════════════════

        public UIDocument Document;

        // ════════════════════════════════════════════════════════════════
        //  싱글턴
        // ════════════════════════════════════════════════════════════════

        public static UIHandler Instance { get; private set; }

        // ════════════════════════════════════════════════════════════════
        //  private 런타임 상태
        // ════════════════════════════════════════════════════════════════

        // Cover(페이드) + HUD
        private VisualElement m_CoverElement;
        private Action m_FadeCallback;
        private CanvasGroup m_HUDCanvasGroup;

        // 종료 화면 (EndTitleContent + EndScreen 분리)
        private VisualElement m_EndTitleContent;
        private VisualElement m_WinTitle;
        private VisualElement m_LoseTitle;
        private VisualElement m_EndScreen;

        // EndScreen 통계 레이블
        private Label m_CoinLabel;
        private Label m_LiveLabel;
        private Label m_StarLabel;

        // ════════════════════════════════════════════════════════════════
        //  Unity 생명 주기
        // ════════════════════════════════════════════════════════════════

        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(gameObject);
                return;
            }
            Instance = this;
        }

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

            // Closing → 보드 입력 차단, Result → ShowEnd()
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

        // ════════════════════════════════════════════════════════════════
        //  public API
        // ════════════════════════════════════════════════════════════════

        // 레벨 시작 시 종료 화면을 초기화합니다.
        public void Init()
        {
            if (m_EndTitleContent != null)
                m_EndTitleContent.style.display = DisplayStyle.None;
            if (m_EndScreen != null)
                m_EndScreen.style.display = DisplayStyle.None;
        }

        // 상단 바 (획득 골드/라이프/별) 표시값을 갱신합니다.
        public void UpdateTopBarData()
        {
            int earned = LevelData.Instance != null ? LevelData.Instance.EarnedMoney : GameManager.Instance.Coins;
            if (m_CoinLabel != null) m_CoinLabel.text = earned.ToString();
            if (m_LiveLabel != null) m_LiveLabel.text = GameManager.Instance.Lives.ToString();
            if (m_StarLabel != null) m_StarLabel.text = GameManager.Instance.Stars.ToString();
        }

        // ════════════════════════════════════════════════════════════════
        //  private 구현
        // ════════════════════════════════════════════════════════════════

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

        // EarnedMoney >= GoalGold 이면 타이쿤 승리. CurrentStage 없으면 GoalLeft == 0 조건으로 폴백.
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

        // 승리 화면을 표시하고 스테이지 결과를 저장합니다.
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

        // 패배 화면을 표시합니다.
        private void ShowLose()
        {
            m_EndTitleContent.style.display = DisplayStyle.Flex;
            m_WinTitle.style.display = DisplayStyle.None;
            m_LoseTitle.style.display = DisplayStyle.Flex;

            StartCoroutine(ShowEndScreen());
        }

        // 3초 후 최종 결과 패널을 표시합니다.
        private IEnumerator ShowEndScreen()
        {
            yield return new WaitForSeconds(3.0f);
            UpdateTopBarData();
            m_EndScreen.style.display = DisplayStyle.Flex;
        }

        // Cover를 투명하게 만들어 페이드인을 시작합니다.
        public void FadeIn(Action onFadeFinished)
        {
            m_CoverElement.style.opacity = 0.0f;
            m_FadeCallback += onFadeFinished;
        }

        // Cover를 불투명하게 만들어 페이드아웃을 시작합니다.
        public void FadeOut(Action onFadeFinished)
        {
            m_CoverElement.style.opacity = 1.0f;
            m_FadeCallback += onFadeFinished;
        }

        // CanvasGroup 알파값을 from → to로 duration 초 동안 보간합니다.
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

        // UIDocument 전체를 표시하거나 숨깁니다.
        public void Display(bool displayed)
        {
            Document.rootVisualElement.style.display =
                displayed ? DisplayStyle.Flex : DisplayStyle.None;
        }
    }
}
