using System;
using UnityEngine;
using UnityEngine.SceneManagement;


public enum GameState
{
    // 게임 인트로: 스테이지 설명, 연출, 준비 화면 등
    Intro,

    // 장사 시작: 타이머가 감소하고, 손님/보드 플레이가 진행되는 메인 게임 상태
    Running,

    // 장사 종료: 영업 종료. 남은 손님은 계속 먹을 수 있음(후처리/소거 단계)
    Closing,

    // 게임 결과: 점수/보상/리절트 UI 표시 상태
    Result
}


public class GameManager : MonoBehaviour
{
    // ========== 설정 및 참조 ==========
    [Header("Current Stage/Shop")]
    [SerializeField] private ShopSO currentShop;
    [SerializeField] private StageSO currentStage;

    [Header("External References")]
    [SerializeField] private HallManager hallManager;
    [SerializeField] private Board board;
    [SerializeField] private GameResultPanel resultPanel;

    // ========== 게임 상태 관리 ==========
    public GameState CurrentState { get; private set; } = GameState.Intro;
    public static event Action<GameState, GameState> OnStateChanged; // (prev, current)

    // ========== 타이머 시스템 ==========
    public float RemainingTime { get; private set; }
    private Coroutine timerRoutine;

    // ========== 게임 데이터 (점수/하트) ==========
    public int EarnedMoney { get; set; } = 0;
    public int CurrentHeart { get; set; } = 0;

    private readonly float heartDecayInterval = 5f; // 하트 자동 감소 주기(초)
    private Coroutine heartDecayRoutine;

    // ========== 피버 모드 시스템 ==========
    private bool isFever = false;
    private const float FeverThresholdRatio = 0.7f;
    public static event Action<bool> OnFeverStateChangedStatic; // 전역 브로드캐스트
    public event Action<bool> OnFeverStateChanged; // 인스턴스 브로드캐스트

    // ========== 일시정지 시스템 ==========
    private bool paused; // 현재 일시정지 여부
    private float prevTimeScale = 1f;

    // ========== 씬 전환 시스템 ==========
    private readonly string stageSceneName = "StageScene";
    private bool isLoading = false;
    private bool endGameScheduled = false; // 게임 종료가 예약되었는지 여부

    // ========== 프로퍼티 ==========
    public StageSO CurrentStage => currentStage;
    public ShopSO CurrentShop => currentShop;
    public bool IsPaused => paused;
    public bool IsFever => isFever;


    void OnEnable()
    {
        PaiedMoney.OnCollected += HandleMoneyCollected;
        Customer.OnHeartGained += HandleHeartGained;
    }

    void OnDisable()
    {
        PaiedMoney.OnCollected -= HandleMoneyCollected;
        Customer.OnHeartGained -= HandleHeartGained;
    }

    void Start()
    {
        // 게임 시작 시 프레임레이트 설정 적용
        FrameRateController.ApplyFrameRate();

        // GameData에서 선택된 shop과 stage 정보를 가져와서 설정
        LoadSelectedData();

        // SetState(GameState.Intro);
        // RemainingTime = currentStage.TimeLimit;

        StartRunning();
        // 초기 피버 상태 동기화
        EvaluateAndBroadcastFever();
    }

    void Update()
    {
        // Closing 상태에서 게임 종료 조건 체크
        if (CurrentState == GameState.Closing)
        {
            CheckGameEndCondition();
        }
    }

    void OnApplicationFocus(bool hasFocus)
    {
        // 일부 디바이스에서 포커스 전환 시 설정이 초기화되는 경우가 있어 재적용
        if (Application.isPlaying && hasFocus)
            FrameRateController.ApplyFrameRate();
    }

    /// <summary>
    /// 게임 일시정지/재개를 토글합니다. (Button 이벤트용)
    /// </summary>
    public void PauseGame()
    {
        // 다시 호출하면 토글되어 재개
        SetPaused(true);
    }

    public void ResumeGame()
    {
        // 다시 호출하면 토글되어 재개
        SetPaused(false);
    }


    /// <summary>
    /// 게임을 종료하고 스테이지 씬으로 돌아갑니다. (Button 이벤트용)
    /// </summary>
    public void QuitGame()
    {
        if (isLoading)
        {
            return;
        }

        isLoading = true;
        StartCoroutine(LoadStageScene());
    }


    private void HandleMoneyCollected(int amount, Table table, Vector3 worldPos)
    {
        if (amount <= 0) return;
        EarnedMoney += amount;
    }

    private void HandleHeartGained(int amount, Customer customer, Vector3 worldPos)
    {
        if (amount <= 0) return;
        CurrentHeart += amount;
        EvaluateAndBroadcastFever();
    }


    private void StartRunning()
    {
        // 이미 달리는 중이거나 종료 처리 중이면 무시
        if (CurrentState == GameState.Running || CurrentState == GameState.Closing)
            return;

        // 타이머 초기화: StageSO의 TimeLimit 사용 (초 단위)
        int stageLimit = currentStage != null ? Mathf.Max(0, currentStage.TimeLimit) : 0;
        RemainingTime = stageLimit;

        // Gum 소모 (일반 플레이)
        if (!SaveSystem.Instance.SpendGumForGame(isContinuePlay: false))
        {
            Debug.LogWarning("[GameManager] Gum이 부족하여 게임을 시작할 수 없습니다!");
            // TODO: Gum 부족 시 처리 (상점으로 이동, 팝업 표시 등)
            // return;
        }

        // 상태 전환 및 코루틴 시작
        SetState(GameState.Running);

        if (timerRoutine != null) StopCoroutine(timerRoutine);
        timerRoutine = StartCoroutine(CoCountdown());

        // 하트 자동 감소 시작
        if (heartDecayRoutine != null) StopCoroutine(heartDecayRoutine);
        heartDecayRoutine = StartCoroutine(CoHeartDecay());

        // 스테이지 기준치가 있을 수 있으므로 시작 시점에 피버 판정 수행
        EvaluateAndBroadcastFever();

        // 필요한 경우 외부 시스템에 알림(예: 홀 매니저, 보드)
        // 여기서는 안전하게 존재 여부만 확인하고 콜을 추가할 수 있음
        // ex) hallManager?.OnBusinessStarted(); board?.OnGameStarted();
    }

    private System.Collections.IEnumerator CoCountdown()
    {
        // 남은 시간이 0보다 크면 감소시키고, 0 이하가 되면 Closing으로 전환
        while (RemainingTime > 0f)
        {
            // Time.deltaTime 기준으로 감소 (일시정지 시에는 Time.timeScale 로직을 UI에서 처리)
            RemainingTime = Mathf.Max(0f, RemainingTime - Time.deltaTime);
            yield return null;
        }

        timerRoutine = null;

        // 영업 종료: 남은 손님은 계속 먹을 수 있음 → 시스템은 Closing 상태에서 정리/후처리
        SetState(GameState.Closing);
        // 자동 감소 정지
        if (heartDecayRoutine != null)
        {
            StopCoroutine(heartDecayRoutine);
            heartDecayRoutine = null;
        }
        // ex) hallManager?.OnBusinessClosed();
    }

    private void SetState(GameState next)
    {
        if (CurrentState == next) return;
        var prev = CurrentState;
        CurrentState = next;

        OnStateChanged?.Invoke(prev, CurrentState);
    }

    /// <summary>
    /// 게임 종료 조건을 확인하고 조건이 만족되면 결과 화면으로 전환합니다.
    /// </summary>
    private void CheckGameEndCondition()
    {
        // RemainingTime이 0이고 모든 테이블이 Empty 상태라면 1초 후 게임 종료
        if (RemainingTime <= 0f && hallManager.AreAllTablesEmpty() && !endGameScheduled)
        {
            endGameScheduled = true;
            StartCoroutine(EndGameAfterDelay(2f));
        }
    }

    /// <summary>
    /// 지정된 시간 후에 게임을 종료합니다.
    /// </summary>
    private System.Collections.IEnumerator EndGameAfterDelay(float delay)
    {
        yield return new WaitForSeconds(delay);
        EndGame();
    }

    /// <summary>
    /// 게임을 종료하고 결과 패널을 표시합니다.
    /// </summary>
    private void EndGame()
    {
        // 이미 Result 상태라면 중복 실행 방지
        if (CurrentState == GameState.Result) return;

        // 스테이지 결과 기록
        RecordStageResult();

        SetPaused(true);

        // 상태를 Result로 전환
        SetState(GameState.Result);

        // 결과 패널 표시
        resultPanel.gameObject.SetActive(true);
    }

    /// <summary>
    /// 현재 스테이지 결과를 PlayerProgress에 기록합니다.
    /// </summary>
    private void RecordStageResult()
    {
        // 유효한 스테이지 정보가 없으면 기록하지 않음
        if (currentShop == null || currentStage == null)
        {
            Debug.LogWarning("[GameManager] 스테이지 정보가 없어 결과를 기록할 수 없습니다.");
            return;
        }

        try
        {
            // 스테이지 결과를 PlayerProgress에 기록
            SaveSystem.Instance.RecordStageResult(
                shopType: currentShop.Type,
                stageNumber: currentStage.StageNumber,
                earnedMoney: EarnedMoney,
                goalMoney: currentStage.GoalGold,
                perfectMoney: currentStage.GoalPerpect
            );

            // 획득한 골드를 PlayerCurrency에 추가
            if (EarnedMoney > 0)
            {
                int addedGold = SaveSystem.Instance.AddGold(EarnedMoney);
                Debug.Log($"[GameManager] 골드 획득: +{addedGold} (총 {SaveSystem.Instance.GetGold()})");
            }
        }
        catch (System.Exception e)
        {
            Debug.LogError($"[GameManager] 스테이지 결과 기록 중 오류 발생: {e.Message}");
        }
    }

    private System.Collections.IEnumerator CoHeartDecay()
    {
        float interval = Mathf.Max(0.1f, heartDecayInterval);
        float t = 0f;
        while (CurrentState == GameState.Running)
        {
            t += Time.deltaTime;
            if (t >= interval)
            {
                t = 0f;
                if (CurrentHeart > 0)
                {
                    CurrentHeart = Mathf.Max(0, CurrentHeart - 1);
                    EvaluateAndBroadcastFever();
                }
            }
            yield return null;
        }
        heartDecayRoutine = null;
    }

    private void EvaluateAndBroadcastFever()
    {
        int cap = Mathf.Max(1, currentStage != null ? currentStage.FeverHeartCount : 1);
        bool next = CurrentHeart > Mathf.FloorToInt(cap * FeverThresholdRatio);
        if (next == isFever) return;
        isFever = next;
        try
        {
            OnFeverStateChanged?.Invoke(isFever);
        }
        catch (System.Exception) { /* 구독자 예외로 전체 중단 방지 */ }
        try
        {
            OnFeverStateChangedStatic?.Invoke(isFever);
        }
        catch (System.Exception) { }
    }

    /// <summary>
    /// GameData에서 선택된 shop과 stage 정보를 로드합니다.
    /// </summary>
    private void LoadSelectedData()
    {
        var gameData = GameData.Instance;
        if (gameData.HasValidData())
        {
            currentShop = gameData.SelectedShop;
            currentStage = gameData.SelectedStage;
        }
    }


    /// <summary>
    /// 스테이지 씬으로 비동기 전환합니다.
    /// </summary>
    private System.Collections.IEnumerator LoadStageScene()
    {
        // 비동기 씬 전환 (StageCell 방식 참고)
        AsyncOperation op = SceneManager.LoadSceneAsync(stageSceneName, LoadSceneMode.Single);
        if (op == null)
        {
            Debug.LogWarning($"[GameManager] Failed to start loading scene '{stageSceneName}'. Check Build Settings.");
            isLoading = false; // 로드 시작 실패 시만 해제
            yield break;
        }

        while (!op.isDone)
        {
            yield return null;
        }
        // 씬 전환 완료 시 이 오브젝트는 파괴되므로 플래그 초기화 불필요
    }

    /// <summary>
    /// 일시정지 상태를 설정합니다.
    /// </summary>
    private void SetPaused(bool value)
    {
        if (paused == value) return;
        paused = value;

        if (paused)
        {
            prevTimeScale = Time.timeScale <= 0f ? 1f : Time.timeScale;
            Time.timeScale = 0f;
            AudioListener.pause = true;
        }
        else
        {
            Time.timeScale = Mathf.Approximately(prevTimeScale, 0f) ? 1f : prevTimeScale;
            AudioListener.pause = false;
        }
    }


}
