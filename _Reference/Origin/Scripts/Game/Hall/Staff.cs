using System.Collections;
using System.Collections.Generic;
using Spine.Unity;
using UnityEngine;

/// <summary>
/// 테이블에 돈이 놓였을 때(Player 대신) 자동으로 일정 시간 후 수금해주는 단일 Staff.
/// 레벨별 '체크 주기': level1=10s, level2=5s, level3=3s, level4=1s (추후 확장 시 배열 수정)
/// 동작 변경: 기존에는 테이블마다 대기 시간을 채운 뒤 수금했으나 이제는 주기마다 대기열을 폴링하여 바로 수금 시도.
/// 수금 절차: (주기 대기) -> cashier 애니 시작 -> 코인 Staff쪽으로 이동(PaiedMoney.CollectViaStaff) -> idle 복귀
/// </summary>
public class Staff : MonoBehaviour
{
    public enum StaffType
    {
        Cashier,
        Manager
    }

    [SerializeField] private int level = 1;
    [SerializeField] private StaffType staffType;
    private readonly float[] delayByLevel = new float[] { 10f, 5f, 3f, 1f }; // 인덱스 0이 level1 (이제 폴링 주기)

    private SkeletonAnimation animator; // Spine Animator

    private readonly Queue<Table> pendingTables = new();
    private Coroutine processRoutine;
    private bool isProcessing;

    // 테이블 -> Staff 도중 플레이어가 먼저 수금했는지 추적용
    private readonly HashSet<Table> waitingSet = new();

    void Awake()
    {
        animator = GetComponentInChildren<SkeletonAnimation>();
    }

    void OnEnable()
    {
        Table.OnPayingAdded += HandlePayingAdded;
        Table.OnPayingRemoved += HandlePayingRemoved;
        PlayIdle();
        // Manager 타입이면 Machine 완료 이벤트 구독
        if (staffType == StaffType.Manager)
        {
            Machine.OnMachineCompleted += HandleMachineCompleted;
        }
    }

    void OnDisable()
    {
        Table.OnPayingAdded -= HandlePayingAdded;
        Table.OnPayingRemoved -= HandlePayingRemoved;
        if (processRoutine != null) StopCoroutine(processRoutine);
        pendingTables.Clear();
        waitingSet.Clear();
        if (staffType == StaffType.Manager)
        {
            Machine.OnMachineCompleted -= HandleMachineCompleted;
            if (sidePollingRoutine != null)
            {
                StopCoroutine(sidePollingRoutine);
                sidePollingRoutine = null;
            }
        }
    }

    private void HandlePayingAdded(Table table)
    {
        if (table == null) return;
        if (waitingSet.Contains(table)) return;
        waitingSet.Add(table);
        pendingTables.Enqueue(table);
        TryStartProcess();
    }

    private void HandlePayingRemoved(Table table)
    {
        // 플레이어가 직접 수금하거나 만료된 경우 대기 목록에서 제거
        if (table == null) return;
        waitingSet.Remove(table);
        // Queue 내부의 중간 요소 제거는 어려우므로 처리 루틴에서 스킵
    }

    private void TryStartProcess()
    {
        if (isProcessing) return;
        if (pendingTables.Count == 0) return;
        processRoutine = StartCoroutine(ProcessLoop());
    }

    private IEnumerator ProcessLoop()
    {
        isProcessing = true;
        while (pendingTables.Count > 0)
        {
            var table = pendingTables.Dequeue();
            if (table == null || table.GetState() != Table.TableState.Paying)
            {
                waitingSet.Remove(table);
                continue; // 스킵
            }

            // 폴링 주기 만큼 대기 후 상태 검사 (주기 = 레벨별 체크 간격)
            float pollInterval = GetDelayForCurrentLevel();
            if (pollInterval > 0f)
                yield return new WaitForSeconds(pollInterval);

            // 여전히 Paying이면 즉시 수금 시도
            if (table != null && table.GetState() == Table.TableState.Paying)
            {
                // 애니 시작
                PlayCashier();

                // 실제 PaiedMoney 찾기
                PaiedMoney targetMoney = null;
                var all = FindObjectsByType<PaiedMoney>(FindObjectsSortMode.None);
                foreach (var pm in all)
                {
                    if (pm != null && pm.Owner == table)
                    {
                        targetMoney = pm;
                        break;
                    }
                }

                if (targetMoney != null)
                {
                    Vector3 dest = transform.position + Vector3.up * 0.5f;
                    targetMoney.CollectViaStaff(this, dest);
                    // 코인 Staff에 도착 후 OnCollected 발생 -> GoldGauge 처리
                    // 잠깐 대기 후 idle 복귀 (도착 코루틴이 자체 destroy 후 이벤트 발생)
                    yield return new WaitForSeconds(0.4f);
                }
                PlayIdle();
            }
            waitingSet.Remove(table);
            yield return null;
        }
        isProcessing = false;
        processRoutine = null;
    }

    private float GetDelayForCurrentLevel() // 폴링 주기 계산
    {
        if (delayByLevel == null || delayByLevel.Length == 0) return 10f;
        int idx = Mathf.Clamp(level - 1, 0, delayByLevel.Length - 1);
        return Mathf.Max(0.1f, delayByLevel[idx]);
    }


    private void PlayCashier()
    {
        if (animator != null)
        {
            animator.AnimationState.SetAnimation(0, "cashier", false);
        }
    }

    private void PlayIdle()
    {
        if (animator != null)
        {
            animator.AnimationState.SetAnimation(0, "idle", true);
        }
    }

    // ================= Manager: 사이드 자동 배달 =================
    private bool sideDeliveryInProgress = false;
    private Coroutine sidePollingRoutine;
    private void HandleMachineCompleted(Machine machine)
    {
        if (staffType != StaffType.Manager) return;
        if (machine == null) return;
        // 이벤트는 단순히 Completed 상태 진입 알림으로 사용
        // 폴링 루틴이 없으면 시작
        if (sidePollingRoutine == null)
            sidePollingRoutine = StartCoroutine(SideDeliveryPollingLoop());
    }

    private IEnumerator SideDeliveryPollingLoop()
    {
        while (true)
        {
            // 폴링 주기 대기
            float interval = GetDelayForCurrentLevel();
            if (interval > 0f)
                yield return new WaitForSeconds(interval);
            else
                yield return null;

            if (staffType != StaffType.Manager)
                continue;

            if (sideDeliveryInProgress)
                continue;

            var machine = FindFirstObjectByType<Machine>();
            if (machine == null || !machine.IsCompleted())
                continue; // 아직 생산 완료 안 됨

            Table target = ChooseSideTargetTable();
            if (target == null)
                continue; // 줄 대상 없음

            sideDeliveryInProgress = true;
            PlaySideOnce();
            yield return new WaitForSeconds(0.2f);
            yield return StartCoroutine(SpawnAndMoveSide(machine.transform.position, target));
            target.DisableHandAnimation();
            sideDeliveryInProgress = false;
        }
    }

    private Table ChooseSideTargetTable()
    {
        Table[] tables = FindObjectsByType<Table>(FindObjectsSortMode.None);
        Table handActiveCandidate = null;
        foreach (var t in tables)
        {
            if (t == null) continue;
            if (t.GetState() == Table.TableState.Ordered && !t.GetHasSideMenu() && t.IsHandAnimationActive())
            {
                handActiveCandidate = t; // 손 애니 있는 테이블을 우선 사용
                break;
            }
        }
        if (handActiveCandidate != null) return handActiveCandidate;
        // 손 애니 활성 테이블 없으면 조건 맞는 아무 Ordered 테이블
        foreach (var t in tables)
        {
            if (t == null) continue;
            if (t.GetState() == Table.TableState.Ordered && !t.GetHasSideMenu())
            {
                return t;
            }
        }
        return null;
    }

    private void PlaySideOnce()
    {
        if (animator != null)
        {
            // side 애니 한 번만 재생 (요구사항) -> 종료 후 idle
            var track = animator.AnimationState.SetAnimation(0, "side", false);
            track.Complete += _ => { PlayIdle(); };
        }
    }

    private IEnumerator SpawnAndMoveSide(Vector3 fromPos, Table table)
    {
        // Machine에서 인스펙터로 지정된 sidePrefab을 재사용
        var machine = FindFirstObjectByType<Machine>();
        Side sidePrefab = machine != null ? machine.GetSidePrefab() : null;
        if (sidePrefab == null) yield break; // 프리팹 없으면 중단

        var sideObj = Instantiate(sidePrefab.gameObject);
        sideObj.name = "SideInstance";
        sideObj.transform.position = fromPos;

        Transform targetPoint = table.GetSidePoint();
        Vector3 targetPos = targetPoint != null ? targetPoint.position : table.transform.position;
        float moveDuration = 0.5f;
        float t = 0f; Vector3 start = fromPos;
        while (t < 1f)
        {
            t += Time.deltaTime / moveDuration;
            float ease = Mathf.SmoothStep(0f, 1f, Mathf.Clamp01(t));
            sideObj.transform.position = Vector3.Lerp(start, targetPos, ease);
            yield return null;
        }
        sideObj.transform.position = targetPos;
        sideObj.transform.SetParent(table.transform);
        table.OnSideMenuDelivered(sideObj);

        // 사이드 메뉴 전달이 완료되었으므로 Machine 생산 재시작
        if (machine != null)
        {
            machine.ResetMachine();
        }
    }
}
