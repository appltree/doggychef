using System;
using System.Collections;
using UnityEngine;

namespace DoggyChef
{
    [DefaultExecutionOrder(12000)]
    public class LevelData : MonoBehaviour
    {
        public static LevelData Instance { get; private set; }

        [Serializable]
        public class GemGoal
        {
            public Gem Gem;
            public int Count;
        }

        public StageData CurrentStage;
        public string LevelName = "Level 1";
        public float TimeLimit = 60f;
        public GemGoal[] Goals;
        public GameObject[] CustomerPrefabs;

        [Header("Ice Block")]
        [Tooltip("스포너에서 보석이 생성될 때 얼음에 감싸인 채로 떨어질 확률 (0 = 없음, 1 = 항상).")]
        [Range(0f, 1f)]
        public float IceSpawnChance = 0f;

        [Header("Visuals")]
        public float BorderMargin = 0.3f;
        public SpriteRenderer Background;

        [Header("Audio")]
        public AudioClip Music;

        [Header("Lobby")]
        [Tooltip("로비 화면에 표시할 퀘스트/목표 텍스트. 예: '얼음 블록 30개 깨기'")]
        public string ObjectiveText = "";

        // ──────────────────────────────────────────────────────────
        //  이벤트
        // ──────────────────────────────────────────────────────────

        public delegate void GoalChangeDelegate(int gemType, int newAmount);
        public Action OnAllGoalFinished;
        public GoalChangeDelegate OnGoalChanged;
        public Action<float, float> OnTimeUpdated;        // (remainingTime, normalizedTime)

        // 수금 완료 시 발화. UIHandler가 ScoreText를 갱신합니다.
        public Action<int, Vector3> OnMoneyChanged;       // (earnedMoney, worldPos)
        // 하트 증가/감소 시 발화. UIHandler가 하트 게이지를 갱신합니다.
        public Action<int, int, Vector3> OnHeartChanged;  // (current, max, worldPos)
        // 피버 발동/해제 시 발화. UIHandler가 ScoreText 색상을 변경합니다.
        public Action<bool> OnFeverChanged;

        // 타이머가 0이 되면 발화. GameManager가 구독하여 Closing으로 전환합니다.
        public static event Action OnTimerEnded;

        // ──────────────────────────────────────────────────────────
        //  런타임 상태
        // ──────────────────────────────────────────────────────────

        public int GoalLeft { get; private set; }
        public float RemainingTime { get; private set; }

        // 이번 판 획득 골드. IsFever 중에는 AddMoney()에서 자동 2배 적용.
        public int EarnedMoney { get; private set; }
        // 현재 하트 수. 손님 만족 퇴장 시 +1, 5초마다 -1 감소.
        public int CurrentHeart { get; private set; }
        // CurrentHeart > FeverHeartCount*0.7 이면 true. AddMoney()에서 골드 2배.
        public bool IsFever { get; private set; }

        // 피버 임계치 (CurrentStage.FeverHeartCount에서 초기화)
        private int m_FeverHeartCount = 10;
        // Running 동안만 동작하는 하트 감소 코루틴
        private Coroutine m_HeartDecayCoroutine;

        private bool m_IsTimerEnded = false;

        private void Awake()
        {
            Instance = this;

            if (StageSelectionData.HasSelection)
                CurrentStage = StageSelectionData.SelectedStage;

            // 스테이지 데이터가 있으면 거기서 시간 제한과 피버 기준치를 가져옵니다.
            if (CurrentStage != null)
            {
                TimeLimit = CurrentStage.TimeLimit;
                m_FeverHeartCount = CurrentStage.FeverHeartCount;
                if (CurrentStage.AvailableCustomerPrefabs != null && CurrentStage.AvailableCustomerPrefabs.Count > 0)
                    CustomerPrefabs = CurrentStage.AvailableCustomerPrefabs.ToArray();
            }

            GoalLeft = Goals != null ? Goals.Length : 0;
            RemainingTime = TimeLimit;

            GameManager.Instance.StartLevel();
        }

        private void Start()
        {
            if (Background != null)
                Background.gameObject.SetActive(false);

            GameManager.OnStateChanged += OnGameStateChanged;
        }

        private void OnDestroy()
        {
            GameManager.OnStateChanged -= OnGameStateChanged;
        }

        // Running 진입 시 HeartDecay 시작, Closing/Result 시 중단
        private void OnGameStateChanged(GameManager.GameState state)
        {
            if (state == GameManager.GameState.Running)
            {
                if (m_HeartDecayCoroutine != null) StopCoroutine(m_HeartDecayCoroutine);
                m_HeartDecayCoroutine = StartCoroutine(HeartDecayLoop());
            }
            else if (state == GameManager.GameState.Closing || state == GameManager.GameState.Result)
            {
                if (m_HeartDecayCoroutine != null)
                {
                    StopCoroutine(m_HeartDecayCoroutine);
                    m_HeartDecayCoroutine = null;
                }
            }
        }

        // 5초 간격으로 하트 -1 감소. 피버를 자연 소진시켜 플레이어가 계속 만족도를 유지하도록 유도합니다.
        private IEnumerator HeartDecayLoop()
        {
            while (true)
            {
                yield return new WaitForSeconds(5f);  // 5초 대기
                if (CurrentHeart > 0)
                {
                    CurrentHeart = Mathf.Max(0, CurrentHeart - 1);
                    EvaluateFever();                        // 피버 상태 재평가
                    OnHeartChanged?.Invoke(CurrentHeart, m_FeverHeartCount, Vector3.zero);
                }
            }
        }

        private void Update()
        {
            // Running 상태에서만 타이머 작동 (Closing/Result 중 타이머 정지)
            if (GameManager.Instance != null &&
                GameManager.Instance.CurrentState != GameManager.GameState.Running)
                return;

            if (RemainingTime > 0)
            {
                RemainingTime -= Time.deltaTime;
                if (RemainingTime <= 0)
                {
                    RemainingTime = 0;
                    if (!m_IsTimerEnded)
                    {
                        m_IsTimerEnded = true;
                        OnTimeUpdated?.Invoke(0f, 0f);
                        OnTimerEnded?.Invoke();
                    }
                }
                else
                {
                    float normalized = TimeLimit > 0 ? Mathf.Clamp01(RemainingTime / TimeLimit) : 0f;
                    OnTimeUpdated?.Invoke(RemainingTime, normalized);
                }
            }

        }

        // ================================================================
        //  공개 API
        // ================================================================

        // [TimeBooster 호출] 타이머 종료 전에만 시간을 추가합니다.
        public void AddTime(float seconds)
        {
            if (m_IsTimerEnded) return;
            RemainingTime = Mathf.Min(RemainingTime + seconds, TimeLimit);
        }

        // [HallManager 호출] 골드를 추가합니다. IsFever 중이면 자동 2배.
        public void AddMoney(int amount, Vector3 worldPos = default)
        {
            if (amount <= 0) return;
            int actual = IsFever ? amount * 2 : amount;
            EarnedMoney += actual;
            OnMoneyChanged?.Invoke(EarnedMoney, worldPos);

            UIHandler.Instance?.UpdateTopBarData();
        }

        // [HallManager 호출] 하트를 추가하고 피버 상태를 재평가합니다.
        public void AddHeart(int amount, Vector3 worldPos = default)
        {
            if (amount <= 0) return;
            CurrentHeart = Mathf.Min(CurrentHeart + amount, m_FeverHeartCount);
            EvaluateFever();
            OnHeartChanged?.Invoke(CurrentHeart, m_FeverHeartCount, worldPos);
        }

        // 피버 조건: CurrentHeart > FeverHeartCount * 0.7. 변경 시에만 OnFeverChanged 발화.
        private void EvaluateFever()
        {
            bool newFever = CurrentHeart > Mathf.FloorToInt(m_FeverHeartCount * 0.7f);
            if (newFever != IsFever)
            {
                IsFever = newFever;
                OnFeverChanged?.Invoke(IsFever);
            }
        }

        // ================================================================
        //  Match-3 목표 처리 (타이쿤 모드에서는 보조적으로 사용)
        // ================================================================

        // [Board 호출] 보석이 매칭될 때 목표 진행도를 업데이트합니다.
        public bool Matched(Gem gem)
        {
            if (Goals == null) return false;

            foreach (var goal in Goals)
            {
                if (goal.Gem.GemType == gem.GemType)
                {
                    if (goal.Count == 0) return false;

                    goal.Count -= 1;
                    OnGoalChanged?.Invoke(gem.GemType, goal.Count);

                    if (UIHandler.Instance != null)
                        UIHandler.Instance.UpdateTopBarData();

                    if (goal.Count == 0)
                    {
                        GoalLeft -= 1;
                        if (GoalLeft == 0)
                        {
                            GameManager.Instance.WinStar();
                            // 타이쿤 모드에서는 타이머가 게임 종료 조건이므로
                            // 보석 목표 달성만으로 보드 입력을 차단하지 않습니다.
                            if (CurrentStage == null)
                                GameManager.Instance.Board.ToggleInput(false);
                            OnAllGoalFinished?.Invoke();
                        }
                    }

                    return true;
                }
            }
            return false;
        }

        // [Booster 호출] 부스터 선택 시 배경을 어둡게/복구합니다.
        public void DarkenBackground(bool darken)
        {
            if (Background == null) return;
            Background.gameObject.SetActive(darken);
        }
    }
}
