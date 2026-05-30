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

        // ── 기존 이벤트 ──
        public delegate void GoalChangeDelegate(int gemType, int newAmount);

        public Action OnAllGoalFinished;
        public GoalChangeDelegate OnGoalChanged;
        public Action<float, float> OnTimeUpdated; // remainingTime, normalizedTime

        // ══════════════════════════════════════════════════════════
        //  [타이쿤 추가] 타이쿤 전용 이벤트
        // ══════════════════════════════════════════════════════════
        //  UIHandler가 이 이벤트들을 Start()에서 구독합니다.
        //  이벤트가 발사되면 UI를 즉시 갱신합니다.
        //
        //  OnMoneyChanged  : 수금 완료 시 → ScoreText 갱신
        //  OnHeartChanged  : 하트 증가/감소 시 → MoveText 갱신 (♥ current/max)
        //  OnFeverChanged  : 피버 발동/해제 시 → ScoreText 색상 변경
        // ══════════════════════════════════════════════════════════
        public Action<int, Vector3> OnMoneyChanged;           // 파라미터: (현재 EarnedMoney, worldPos)
        public Action<int, int, Vector3> OnHeartChanged;      // 파라미터: (현재 하트, 최대 하트, worldPos)
        public Action<bool> OnFeverChanged;          // 파라미터: 피버 발동 여부

        // 타이머가 0이 됐을 때 발사되는 이벤트.
        // GameManager가 구독하여 SetState(Closing)을 내부에서 호출합니다.
        public static event Action OnTimerEnded;

        // ── 기존 런타임 데이터 ──
        public int GoalLeft { get; private set; }
        public float RemainingTime { get; private set; }

        // ══════════════════════════════════════════════════════════
        //  [타이쿤 추가] 타이쿤 런타임 상태
        // ══════════════════════════════════════════════════════════
        //  EarnedMoney : 이번 판에 획득한 총 골드.
        //                AddMoney() 를 통해서만 증가합니다.
        //                IsFever가 true이면 AddMoney() 내부에서 자동으로 2배 적용.
        //
        //  CurrentHeart : 손님이 만족 퇴장할 때 +1, 5초마다 -1 감소.
        //                 피버 발동 기준 = FeverHeartCount * 0.7 초과.
        //
        //  IsFever : CurrentHeart가 임계값을 넘으면 true.
        //            true 동안 AddMoney()에서 골드가 2배 지급됩니다.
        // ══════════════════════════════════════════════════════════
        public int EarnedMoney { get; private set; }
        public int CurrentHeart { get; private set; }
        public bool IsFever { get; private set; }

        // 피버 발동에 필요한 최대 하트 수 (CurrentStage.FeverHeartCount에서 읽음)
        private int m_FeverHeartCount = 10;

        // 하트 5초마다 -1 감소 Coroutine 참조 (Running 동안만 실행)
        private Coroutine m_HeartDecayCoroutine;

        private int m_StartingWidth;
        private int m_StartingHeight;
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
            }

            GoalLeft = Goals != null ? Goals.Length : 0;
            RemainingTime = TimeLimit;

            GameManager.Instance.StartLevel();
        }

        private void Start()
        {
            m_StartingWidth = Screen.width;
            m_StartingHeight = Screen.height;

            if (Background != null)
                Background.gameObject.SetActive(false);

            // [타이쿤 추가] GameState가 바뀔 때마다 알림을 받아 Coroutine을 관리합니다.
            // Running → HeartDecayLoop 시작
            // Closing/Result → HeartDecayLoop 중단
            GameManager.OnStateChanged += OnGameStateChanged;
        }

        private void OnDestroy()
        {
            GameManager.OnStateChanged -= OnGameStateChanged;
        }

        // ══════════════════════════════════════════════════════════
        //  [타이쿤 추가] GameState 변경에 따른 HeartDecay 관리
        // ══════════════════════════════════════════════════════════
        //  하트 감소 루틴은 Running 상태에서만 동작합니다.
        //  게임이 끝나면(Closing/Result) 루틴을 멈춰야
        //  불필요한 연산과 이벤트 발사를 막을 수 있습니다.
        // ══════════════════════════════════════════════════════════
        private void OnGameStateChanged(GameManager.GameState state)
        {
            if (state == GameManager.GameState.Running)
            {
                // Running 진입 시 하트 감소 루틴 시작
                if (m_HeartDecayCoroutine != null) StopCoroutine(m_HeartDecayCoroutine);
                m_HeartDecayCoroutine = StartCoroutine(HeartDecayLoop());
            }
            else if (state == GameManager.GameState.Closing || state == GameManager.GameState.Result)
            {
                // 게임 종료 단계 진입 시 루틴 중단
                if (m_HeartDecayCoroutine != null)
                {
                    StopCoroutine(m_HeartDecayCoroutine);
                    m_HeartDecayCoroutine = null;
                }
            }
        }

        // ══════════════════════════════════════════════════════════
        //  [타이쿤 추가] 하트 감소 루틴
        // ══════════════════════════════════════════════════════════
        //  Origin 참고: CoHeartDecay() — 5초 간격으로 하트 -1 감소
        //
        //  이 루틴의 목적:
        //  손님이 계속 만족해서 하트가 쌓이면 피버가 영구히 유지되지 않도록
        //  시간이 지날수록 하트가 자연 감소합니다.
        //  → 플레이어가 꾸준히 손님을 만족시켜야 피버를 유지할 수 있습니다.
        // ══════════════════════════════════════════════════════════
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
            // [타이쿤 추가] Running 상태에서만 타이머를 작동합니다.
            // Closing/Result 상태에서 타이머가 계속 깎이는 것을 방지합니다.
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

                        // [타이쿤 수정] 기존에는 OnNoMoveLeft를 호출했지만,
                        // 타이쿤에서는 "시간 종료 = 게임 종료 준비(Closing)" 입니다.
                        // GameManager가 OnTimerEnded를 구독하여 SetState(Closing)을 호출합니다.
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

        // ══════════════════════════════════════════════════════════
        //  [부스터 추가] 시간 추가
        // ══════════════════════════════════════════════════════════
        /// <summary>
        /// TimeBooster에서 호출합니다. 타이머가 아직 종료되지 않은 경우에만 시간을 추가합니다.
        /// </summary>
        public void AddTime(float seconds)
        {
            if (m_IsTimerEnded) return;
            // 경과 시간을 되돌리는 개념: 초기 시간(TimeLimit) 초과 금지
            RemainingTime = Mathf.Min(RemainingTime + seconds, TimeLimit);
        }

        // ══════════════════════════════════════════════════════════
        //  [타이쿤 추가] 골드 추가
        // ══════════════════════════════════════════════════════════
        //  Table.CollectMoney() → HallManager.OnTableMoneyCollected()
        //  → 이 메서드를 호출하는 흐름입니다.
        //
        //  IsFever가 true면 amount를 2배로 적용합니다. (피버 보너스)
        //  최종 금액을 EarnedMoney에 누적하고 OnMoneyChanged 이벤트를 발사합니다.
        // ══════════════════════════════════════════════════════════
        public void AddMoney(int amount, Vector3 worldPos = default)
        {
            if (amount <= 0) return;
            int actual = IsFever ? amount * 2 : amount;  // 피버 중이면 2배
            EarnedMoney += actual;
            OnMoneyChanged?.Invoke(EarnedMoney, worldPos);  // UIHandler에서 ScoreText 갱신

            if (UIHandler.Instance != null)
                UIHandler.Instance.UpdateTopBarData();
        }

        // ══════════════════════════════════════════════════════════
        //  [타이쿤 추가] 하트 추가
        // ══════════════════════════════════════════════════════════
        //  손님이 만족 퇴장할 때 HallManager가 이 메서드를 호출합니다.
        //  하트가 최대치(m_FeverHeartCount)를 초과하지 않도록 Clamp합니다.
        //  추가 후 EvaluateFever()로 피버 발동 여부를 즉시 체크합니다.
        // ══════════════════════════════════════════════════════════
        public void AddHeart(int amount, Vector3 worldPos = default)
        {
            if (amount <= 0) return;
            CurrentHeart = Mathf.Min(CurrentHeart + amount, m_FeverHeartCount);
            EvaluateFever();
            OnHeartChanged?.Invoke(CurrentHeart, m_FeverHeartCount, worldPos);
        }

        // ══════════════════════════════════════════════════════════
        //  [타이쿤 추가] 피버 상태 평가
        // ══════════════════════════════════════════════════════════
        //  Origin 참고: EvaluateAndBroadcastFever()
        //
        //  피버 발동 조건: CurrentHeart > FeverHeartCount * 0.7
        //    예) FeverHeartCount = 10 → 하트가 8개 이상이면 피버 발동
        //
        //  IsFever가 바뀔 때만 OnFeverChanged를 발사합니다.
        //  (바뀌지 않으면 이벤트를 중복 발사하지 않음)
        // ══════════════════════════════════════════════════════════
        private void EvaluateFever()
        {
            bool newFever = CurrentHeart > Mathf.FloorToInt(m_FeverHeartCount * 0.7f);
            if (newFever != IsFever)
            {
                IsFever = newFever;
                OnFeverChanged?.Invoke(IsFever);  // UIHandler에서 ScoreText 색상 변경
            }
        }

        // ================================================================
        //  기존 Match-3 목표 처리 (타이쿤에서는 주로 사용하지 않음)
        // ================================================================

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

        public void DarkenBackground(bool darken)
        {
            if (Background == null) return;
            Background.gameObject.SetActive(darken);
        }
    }
}
