using System.Collections;
using UnityEngine;
using UnityEngine.EventSystems;
using SpriteGlow;

public class Machine : MonoBehaviour, IPointerDownHandler
{
    // 생산 완료(Completed 상태 진입) 알림 이벤트
    public static event System.Action<Machine> OnMachineCompleted;

    public enum MachineState
    {
        Idle,
        InProgress,
        Completed,
        Active
    }

    [SerializeField] private Side sidePrefab;
    [SerializeField] private GameObject[] leds;
    [SerializeField] private AudioClip completeSfx; // 사이드 메뉴 배달 시 재생할 사운드   


    private readonly float productionDuration = 30f; // 30초
    private readonly int ledStageCount = 3; // 3단계
    private readonly float sideMoveDuration = 0.5f; // 사이드 메뉴 이동 시간

    private readonly float glowMinBrightness = 0f;
    private readonly float glowMaxBrightness = 1f;
    private readonly float glowPulseSpeed = 2f;

    private MachineState currentState = MachineState.Idle;
    private SpriteGlowEffect glow;
    private Coroutine productionRoutine;
    private Coroutine glowPulseRoutine;
    private float productionTimer;


    public Side GetSidePrefab() => sidePrefab; // Manager 자동 전달용 공개 Getter

    void Awake()
    {
        glow = GetComponentInChildren<SpriteGlowEffect>(true);
        InitializeLEDs();
    }

    void Start()
    {
        StartProduction();
        SubscribeToTableEvents();
        SubscribeToBlockEvents();
    }


    void OnDisable()
    {
        StopAllCoroutines();
        StopGlowPulse();
        UnsubscribeFromTableEvents();
        UnsubscribeFromBlockEvents();
    }

    public void OnPointerDown(PointerEventData eventData)
    {
        if (currentState == MachineState.Completed)
        {
            ActivateMachine();
        }
    }

    public bool IsCompleted() => currentState == MachineState.Completed; // 상태 폴링용


    public void OnTableTouchedForSide(Table table)
    {
        if (currentState != MachineState.Active) return;

        // 모든 테이블의 손 애니메이션 비활성화
        DisableHandAnimationOnAllTables();

        // 사이드메뉴 이동 애니메이션 시작
        StartCoroutine(MoveSideToTable(table));
    }


    public void ResetMachine()
    {
        currentState = MachineState.Idle;
        StartProduction();
    }

    private void InitializeLEDs()
    {
        // 모든 LED를 초기화 (비활성화)
        if (leds != null)
        {
            foreach (var led in leds)
            {
                if (led != null)
                    led.SetActive(false);
            }
        }
    }

    private void StartProduction()
    {
        if (currentState != MachineState.Idle) return;

        currentState = MachineState.InProgress;
        productionTimer = 0f;
        InitializeLEDs();
        StopGlowPulse();

        productionRoutine = StartCoroutine(ProductionProcess());
    }

    private IEnumerator ProductionProcess()
    {
        float stageInterval = productionDuration / ledStageCount; // 10초씩

        for (int stage = 0; stage < ledStageCount; stage++)
        {
            float stageEndTime = (stage + 1) * stageInterval;

            // 현재 단계까지 기다림
            while (productionTimer < stageEndTime)
            {
                productionTimer += Time.deltaTime;
                yield return null;
            }

            // LED 켜기
            if (leds != null && stage < leds.Length && leds[stage] != null)
            {
                leds[stage].SetActive(true);
            }
        }

        // 생산 완료
        OnProductionCompleted();
    }

    private void OnProductionCompleted()
    {
        currentState = MachineState.Completed;
        AudioSource.PlayClipAtPoint(completeSfx, Camera.main.transform.position, 0.9f);
        StartGlowPulse();
        // 생산 완료 브로드캐스트 (매니저 Staff가 자동 배달을 위해 사용)
        OnMachineCompleted?.Invoke(this);
    }


    private void StartGlowPulse()
    {
        if (glow == null) return;
        if (glowPulseRoutine != null) return;

        // glow.OutlineWidth = 1;
        glow.enabled = true;
        // glow.GlowColor = completedGlowColor;

        glowPulseRoutine = StartCoroutine(CoGlowPulse());
    }

    private void StopGlowPulse()
    {
        if (glowPulseRoutine != null)
        {
            StopCoroutine(glowPulseRoutine);
            glowPulseRoutine = null;
        }

        if (glow != null)
        {
            glow.enabled = false;
        }
    }

    private IEnumerator CoGlowPulse()
    {
        float t = 0f;
        while (currentState == MachineState.Completed && glow != null)
        {
            t += Time.deltaTime * glowPulseSpeed * Mathf.PI * 2f;
            float s = (Mathf.Sin(t) + 1f) * 0.5f; // 0..1
            float brightness = Mathf.Lerp(glowMinBrightness, glowMaxBrightness, s);
            glow.GlowBrightness = brightness;
            yield return null;
        }
        glowPulseRoutine = null;
    }


    private void ActivateMachine()
    {
        currentState = MachineState.Active;
        StopGlowPulse();

        // 모든 테이블에 손 애니메이션 상태 업데이트
        UpdateHandAnimationsForActiveTables();
    }


    private IEnumerator MoveSideToTable(Table table)
    {
        if (sidePrefab == null) yield break;

        // 사이드 프리팹 인스턴스 생성
        GameObject sideObj = Instantiate(sidePrefab.gameObject);
        sideObj.name = "SideInstance";

        // 머신 위치에서 시작
        sideObj.transform.position = transform.position;

        // 테이블의 sidePoint로 이동
        Transform targetPoint = table.GetSidePoint();
        Vector3 targetPos = targetPoint != null ? targetPoint.position : table.transform.position;

        float elapsedTime = 0f;
        Vector3 startPos = sideObj.transform.position;

        while (elapsedTime < sideMoveDuration)
        {
            float t = elapsedTime / sideMoveDuration;
            t = Mathf.SmoothStep(0f, 1f, t); // 부드러운 이동
            sideObj.transform.position = Vector3.Lerp(startPos, targetPos, t);
            elapsedTime += Time.deltaTime;
            yield return null;
        }

        sideObj.transform.position = targetPos;

        // 도착 후 테이블의 자식으로 설정
        sideObj.transform.SetParent(table.transform);

        // 테이블에 사이드메뉴 추가 효과 적용 (사이드 오브젝트 전달)
        table.OnSideMenuDelivered(sideObj);

        // 머신 다시 시작
        ResetMachine();
    }

    private void DisableHandAnimationOnAllTables()
    {
        Table[] tables = FindObjectsByType<Table>(FindObjectsSortMode.None);
        foreach (var table in tables)
        {
            table.DisableHandAnimation();
        }
    }

    // 테이블 이벤트 구독
    private void SubscribeToTableEvents()
    {
        Table[] tables = FindObjectsByType<Table>(FindObjectsSortMode.None);
        foreach (var table in tables)
        {
            table.OnOrderSet += OnAnyTableOrderChanged;
        }
    }

    // 테이블 이벤트 구독 해제
    private void UnsubscribeFromTableEvents()
    {
        Table[] tables = FindObjectsByType<Table>(FindObjectsSortMode.None);
        foreach (var table in tables)
        {
            table.OnOrderSet -= OnAnyTableOrderChanged;
        }
    }

    // 모든 테이블의 주문 변경 이벤트 처리
    private void OnAnyTableOrderChanged()
    {
        // Machine이 Active 상태일 때만 처리
        if (currentState == MachineState.Active)
        {
            // 모든 테이블을 다시 체크해서 손 애니메이션 상태 업데이트
            UpdateHandAnimationsForActiveTables();
        }
    }

    // Active 상태에서 모든 테이블의 손 애니메이션 상태 업데이트
    private void UpdateHandAnimationsForActiveTables()
    {
        Table[] tables = FindObjectsByType<Table>(FindObjectsSortMode.None);
        foreach (var table in tables)
        {
            // Ordered 상태이고 손님이 있으며, 아직 사이드메뉴를 받지 않은 테이블이면 손 애니메이션 활성화
            if (table.GetState() == Table.TableState.Ordered && !table.GetHasSideMenu())
            {
                table.EnableHandAnimation(true);
            }
            // 그렇지 않으면 손 애니메이션 비활성화
            else
            {
                table.DisableHandAnimation();
            }
        }
    }

    // 블록 이벤트 구독
    private void SubscribeToBlockEvents()
    {
        BlockController.OnBlockDragged += HandleBlockDragged;
    }

    // 블록 이벤트 구독 해제
    private void UnsubscribeFromBlockEvents()
    {
        BlockController.OnBlockDragged -= HandleBlockDragged;
    }

    // 블록 드래그 이벤트 처리
    private void HandleBlockDragged(Block block, DragDirection direction)
    {
        // Active 상태에서 블록이 드래그되면 Completed 상태로 변경
        if (currentState == MachineState.Active)
        {
            currentState = MachineState.Completed;
            // 모든 테이블의 손 애니메이션 비활성화
            DisableHandAnimationOnAllTables();
            // Completed 상태 시각적 효과 시작
            StartGlowPulse();
        }
    }

}
