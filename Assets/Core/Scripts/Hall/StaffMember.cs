using System.Collections;
using System.Collections.Generic;
using Spine.Unity;
using UnityEngine;

namespace MyMatch3
{
    // ════════════════════════════════════════════════════════════════
    //  StaffMember — 식당 스태프 (역할별: Cashier / Manager)
    // ════════════════════════════════════════════════════════════════
    //
    //  ■ Cashier (수금 전담)
    //    Table.OnPayingAdded 이벤트를 구독하고, CashierDelay 후
    //    CoinMoney를 Staff 위치로 날려 수금합니다.
    //
    //  ■ Manager (사이드메뉴 배달 + 수금 겸직)
    //    Machine.OnMachineCompleted 이벤트를 구독합니다.
    //    SideDeliveryInterval마다 Ordered 테이블을 탐색해 사이드메뉴를 자동 배달합니다.
    //    수금(Cashier) 기능도 겸직합니다.
    // ════════════════════════════════════════════════════════════════
    public class StaffMember : MonoBehaviour
    {
        // ──────────────────────────────────────────────────────────
        //  역할 정의 (Origin Staff.StaffType 동일)
        // ──────────────────────────────────────────────────────────
        public enum StaffType
        {
            Cashier,  // 수금 전담
            Manager   // 사이드메뉴 배달 + 수금 겸직
        }

        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────

        [Header("역할 (Origin Staff.StaffType 동일)")]
        [Tooltip("Cashier : 수금 전담\nManager : 사이드메뉴 배달 + 수금 겸직")]
        public StaffType Role = StaffType.Cashier;

        [Header("담당 테이블")]
        [Tooltip("이 스태프가 담당하는 테이블 목록.\n비어 있으면 씬의 모든 Table을 자동으로 찾습니다.")]
        public Table[] Tables;

        [Header("사이드메뉴 배달 (Manager 전용)")]
        [Tooltip("머신 완료 후 배달까지 대기 시간(초).\n플레이어가 이 시간 안에 머신을 탭하면 플레이어가 처리.\n그렇지 않으면 스태프가 자동 배달.\nOrigin: level1=10s, level2=5s, level3=3s, level4=1s")]
        public float SideDeliveryInterval = 3f;

        [Header("캐셔 (자동 수금)")]
        [Tooltip("수금 대기 시간(초). Origin: level1=10s, level2=5s, level3=3s, level4=1s")]
        public float CashierDelay = 10f;

        // Spine 애니메이터 (SkeletonAnimation이 있으면 자동으로 사용)
        private SkeletonAnimation m_Animator;

        // ──────────────────────────────────────────────────────────
        //  내부 상태 — 매니저 (사이드메뉴 배달)
        // ──────────────────────────────────────────────────────────
        private Coroutine m_SideDeliveryRoutine;
        private bool m_SideDeliveryProcessing;

        // ──────────────────────────────────────────────────────────
        //  내부 상태 — 캐셔
        // ──────────────────────────────────────────────────────────
        private readonly Queue<Table> m_PendingTables = new();
        private readonly HashSet<Table> m_WaitingSet = new();
        private Coroutine m_CashierRoutine;
        private bool m_CashierProcessing;

        // ================================================================
        //  유니티 생명주기
        // ================================================================

        private void Start()
        {
            m_Animator = GetComponentInChildren<SkeletonAnimation>();
            PlayIdle();

            if (Tables == null || Tables.Length == 0)
                Tables = FindObjectsByType<Table>(FindObjectsSortMode.None);
        }

        private void OnEnable()
        {
            // Cashier·Manager 공통: 수금 이벤트 구독
            Table.OnPayingAdded += HandlePayingAdded;
            Table.OnPayingRemoved += HandlePayingRemoved;

            // Manager 전용: 머신 완료 이벤트 구독
            if (Role == StaffType.Manager)
                Machine.OnMachineCompleted += HandleMachineCompleted;
        }

        private void OnDisable()
        {
            Table.OnPayingAdded -= HandlePayingAdded;
            Table.OnPayingRemoved -= HandlePayingRemoved;

            if (Role == StaffType.Manager)
                Machine.OnMachineCompleted -= HandleMachineCompleted;

            if (m_SideDeliveryRoutine != null)
            {
                StopCoroutine(m_SideDeliveryRoutine);
                m_SideDeliveryRoutine = null;
            }
            if (m_CashierRoutine != null)
            {
                StopCoroutine(m_CashierRoutine);
                m_CashierRoutine = null;
            }
            m_PendingTables.Clear();
            m_WaitingSet.Clear();
            m_CashierProcessing = false;
            m_SideDeliveryProcessing = false;
        }

        // ================================================================
        //  매니저 루프 — 사이드메뉴 자동 배달 (Origin Staff.Manager 방식)
        //
        //  Machine.OnMachineCompleted → SideDeliveryPollingLoop 시작
        //  SideDeliveryInterval 마다:
        //    ① Machine.IsCompleted() 확인 (플레이어가 먼저 처리하면 루프 종료)
        //    ② ChooseSideTargetTable() — 우선순위 적용
        //    ③ CoDeliverSide() — 사이드 오브젝트 머신→테이블 이동 애니메이션
        //    ④ machine.ResetMachine() → 머신 재생산 시작 → 루프 종료
        // ================================================================

        private void HandleMachineCompleted(Machine machine)
        {
            if (m_SideDeliveryProcessing) return;
            // 폴링 루틴이 없으면 시작 (Origin: sidePollingRoutine == null 체크와 동일)
            if (m_SideDeliveryRoutine == null)
                m_SideDeliveryRoutine = StartCoroutine(SideDeliveryPollingLoop());
        }

        private IEnumerator SideDeliveryPollingLoop()
        {
            // Origin과 동일하게 while(true) 영구 폴링
            // — 루틴은 OnDisable 시 StopCoroutine으로만 종료됩니다.
            while (true)
            {
                float interval = SideDeliveryInterval;
                if (interval > 0f)
                    yield return new WaitForSeconds(interval);
                else
                    yield return null;

                if (m_SideDeliveryProcessing) continue;

                // 머신 상태 확인 (매 폴링마다 직접 찾기 — Origin 방식)
                // 플레이어가 탭해서 Active 상태가 된 경우에도 배달을 진행해야 함.
                var machine = FindFirstObjectByType<Machine>();
                if (machine == null || (!machine.IsCompleted() && !machine.IsActive())) continue;

                Table target = ChooseSideTargetTable();
                if (target == null) continue;

                // 배달 수행
                m_SideDeliveryProcessing = true;
                yield return StartCoroutine(CoDeliverSide(machine, target));
                m_SideDeliveryProcessing = false;
            }
        }

        /// <summary>
        /// 사이드메뉴 배달 대상 테이블을 선택합니다.
        /// 우선순위 1: 손 애니메이션 활성 Ordered 테이블 (플레이어가 선택 중인 테이블)
        /// 우선순위 2: 사이드 미수령 Ordered 테이블
        /// </summary>
        private Table ChooseSideTargetTable()
        {
            foreach (var t in Tables)
            {
                if (t != null && t.State == Table.TableState.Ordered
                    && !t.GetHasSideMenu() && t.IsHandAnimationActive())
                    return t;
            }
            foreach (var t in Tables)
            {
                if (t != null && t.State == Table.TableState.Ordered && !t.GetHasSideMenu())
                    return t;
            }
            return null;
        }

        private IEnumerator CoDeliverSide(Machine machine, Table table)
        {
            GameObject sidePrefab = machine.GetSidePrefab();
            if (sidePrefab == null)
            {
                machine.ResetMachine(); // sidePrefab 미설정 시에도 머신을 재가동
                yield break;
            }

            // "side" 애니메이션 (한 번만 재생)
            PlaySide();
            yield return new WaitForSeconds(0.2f);

            // 사이드 오브젝트 생성 — 머신 위치에서 출발
            var sideObj = Instantiate(sidePrefab, machine.transform.position, Quaternion.identity);
            sideObj.name = "SideInstance";
            var sr = sideObj.GetComponent<SpriteRenderer>();
            if (sr != null) sr.sortingOrder = 9;

            // 테이블 SidePoint로 0.5초 SmoothStep 이동
            Transform targetPoint = table.GetSidePoint();
            Vector3 startPos = sideObj.transform.position;
            Vector3 targetPos = targetPoint != null ? targetPoint.position : table.transform.position;

            float elapsed = 0f;
            const float moveDuration = 0.5f;
            while (elapsed < moveDuration)
            {
                float k = Mathf.SmoothStep(0f, 1f, elapsed / moveDuration);
                sideObj.transform.position = Vector3.Lerp(startPos, targetPos, k);
                elapsed += Time.deltaTime;
                yield return null;
            }
            sideObj.transform.position = targetPos;

            // 도착 시 테이블이 Ordered 상태가 아니면 취소
            if (table == null || table.State != Table.TableState.Ordered)
            {
                Destroy(sideObj);
                machine.ResetMachine();
                PlayIdle();
                yield break;
            }

            // 테이블에 부착 및 사이드메뉴 배달 처리
            sideObj.transform.SetParent(table.transform);
            table.OnSideMenuDelivered(sideObj);
            table.DisableHandAnimation(); // Origin: 배달 완료 후 손 애니 OFF

            machine.ResetMachine();
            PlayIdle();
        }

        // ================================================================
        //  캐셔 루프 — Paying 상태 전용 (Origin Staff.Cashier 방식)
        //
        //  Table.OnPayingAdded  → Queue에 추가 → CashierLoop 시작
        //  CashierDelay 후 → CoinMoney(Owner==table) 찾아 CollectViaStaff() 호출
        //  코인이 Staff 위치로 날아와 수거됨
        // ================================================================

        private void HandlePayingAdded(Table table)
        {
            if (table == null) return;
            if (m_WaitingSet.Contains(table)) return;

            m_WaitingSet.Add(table);
            m_PendingTables.Enqueue(table);
            TryStartCashier();
        }

        private void HandlePayingRemoved(Table table)
        {
            // 플레이어가 먼저 수금하거나 만료된 경우 → 대기 목록에서 제거
            if (table == null) return;
            m_WaitingSet.Remove(table);
        }

        private void TryStartCashier()
        {
            if (m_CashierProcessing) return;
            if (m_PendingTables.Count == 0) return;
            m_CashierRoutine = StartCoroutine(CashierLoop());
        }

        private IEnumerator CashierLoop()
        {
            m_CashierProcessing = true;

            while (m_PendingTables.Count > 0)
            {
                var table = m_PendingTables.Dequeue();

                // 이미 수금됐거나 손님이 퇴장한 경우 스킵
                if (table == null || table.State != Table.TableState.Paying)
                {
                    m_WaitingSet.Remove(table);
                    continue;
                }

                // CashierDelay 대기 후 수금 시도
                if (CashierDelay > 0f)
                    yield return new WaitForSeconds(CashierDelay);

                // 대기 중 플레이어가 먼저 수금했으면 스킵
                if (table == null || table.State != Table.TableState.Paying)
                {
                    m_WaitingSet.Remove(table);
                    continue;
                }

                // Owner == table 인 CoinMoney를 찾아 Staff 위치로 날림
                CoinMoney targetCoin = null;
                foreach (var coin in FindObjectsByType<CoinMoney>(FindObjectsSortMode.None))
                {
                    if (coin != null && coin.Owner == table)
                    {
                        targetCoin = coin;
                        break;
                    }
                }

                if (targetCoin != null)
                {
                    PlayCashier();
                    Vector3 dest = transform.position + Vector3.up * 0.5f;
                    targetCoin.CollectViaStaff(dest);
                    yield return new WaitForSeconds(0.4f); // 도착 대기
                    PlayIdle();
                }
                else
                {
                    // CoinMoney가 없으면 직접 수금 (폴백) — Staff 위치에서 GoldGauge로 비행
                    if (table != null && table.State == Table.TableState.Paying)
                    {
                        Vector3 staffPos = transform.position + Vector3.up * 0.5f;
                        table.CollectMoney(staffPos);
                    }
                }

                m_WaitingSet.Remove(table);
                yield return null;
            }

            m_CashierProcessing = false;
            m_CashierRoutine = null;
        }

        // ================================================================
        //  Spine 애니메이션
        // ================================================================

        private void PlayIdle()
        {
            if (m_Animator != null)
                m_Animator.AnimationState.SetAnimation(0, "idle", true);
        }

        private void PlayCashier()
        {
            if (m_Animator != null)
                m_Animator.AnimationState.SetAnimation(0, "cashier", false);
        }

        private void PlaySide()
        {
            if (m_Animator != null)
                m_Animator.AnimationState.SetAnimation(0, "side", false);
        }
    }
}
