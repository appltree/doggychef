using System.Collections;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.Serialization;
using SpriteGlow;

namespace DoggyChef
{
    [DefaultExecutionOrder(-1)] // Table(0)보다 먼저 Update 실행
    public class Machine : MonoBehaviour
    {
        // ──────────────────────────────────────────────────────────
        //  상태 정의
        // ──────────────────────────────────────────────────────────

        public enum MachineState { Idle, InProgress, Completed, Active }

        // ──────────────────────────────────────────────────────────
        //  상수
        // ──────────────────────────────────────────────────────────

        private const float ProductionDuration = 10f;
        private const int LedStageCount = 3;
        private const float SideMoveDuration = 0.5f;
        private const float GlowMinBrightness = 0f;
        private const float GlowMaxBrightness = 1f;
        private const float GlowPulseSpeed = 2f;

        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────

        [FormerlySerializedAs("sidePrefab")]
        [SerializeField] private GameObject m_SidePrefab;

        [FormerlySerializedAs("leds")]
        [SerializeField] private GameObject[] m_Leds;

        [FormerlySerializedAs("completeSfx")]
        [Tooltip("생산 완료 시 재생할 효과음")]
        [SerializeField] private AudioClip m_CompleteSfx;

        // ──────────────────────────────────────────────────────────
        //  이벤트
        // ──────────────────────────────────────────────────────────

        // 생산 완료(Completed 상태 진입) 시 발화. StaffMember가 구독합니다.
        public static event System.Action<Machine> OnMachineCompleted;

        // ──────────────────────────────────────────────────────────
        //  싱글턴
        // ──────────────────────────────────────────────────────────

        public static Machine Instance { get; private set; }

        // ──────────────────────────────────────────────────────────
        //  포인터 소비 — Machine이 먼저 탭을 처리하면 Table은 무시
        // ──────────────────────────────────────────────────────────

        private static int s_ConsumedFrame = -1;
        public static bool IsPointerConsumedThisFrame() => s_ConsumedFrame == Time.frameCount;
        private static void ConsumePointerThisFrame() => s_ConsumedFrame = Time.frameCount;

        // ──────────────────────────────────────────────────────────
        //  런타임 상태
        // ──────────────────────────────────────────────────────────

        private MachineState m_CurrentState = MachineState.Idle;
        private SpriteGlowEffect m_Glow;
        private Coroutine m_GlowPulseRoutine;
        private float m_ProductionTimer;
        private Collider2D m_Collider;

        // 씬 내 테이블 캐시 (Start에서 초기화, 이후 재사용)
        private Table[] m_CachedTables;

        // ================================================================
        //  Unity 생명 주기
        // ================================================================

        private void Awake()
        {
            Instance = this;
            m_Glow = GetComponentInChildren<SpriteGlowEffect>(true);
            m_Collider = GetComponent<Collider2D>();
            InitializeLEDs();

            // Machine(본체 + LED)을 Character 레이어로 설정하여 Customer보다 앞에 렌더링
            int characterLayer = SortingLayer.NameToID("Character");
            foreach (var sr in GetComponentsInChildren<SpriteRenderer>(true))
                sr.sortingLayerID = characterLayer;
        }

        private void Start()
        {
            m_CachedTables = FindObjectsByType<Table>(FindObjectsInactive.Exclude);

            StartProduction();
            SubscribeToTableEvents();
            SubscribeToGemEvents();
        }

        private void OnDisable()
        {
            if (Instance == this) Instance = null;
            StopAllCoroutines();
            StopGlowPulse();
            UnsubscribeFromTableEvents();
            UnsubscribeFromGemEvents();
        }

        private void Update()
        {
            var pointer = Pointer.current;
            if (pointer == null || !pointer.press.wasPressedThisFrame) return;
            if (m_Collider == null) return;

            Vector2 worldPos = Camera.main.ScreenToWorldPoint(pointer.position.ReadValue());
            if (!m_Collider.OverlapPoint(worldPos)) return;

            // Machine이 탭을 처리했음을 기록 → Table.Update가 중복 처리하지 않도록
            ConsumePointerThisFrame();

            if (m_CurrentState == MachineState.Completed)
                ActivateMachine();
        }

        // ================================================================
        //  공개 API
        // ================================================================

        // [StaffMember 조회용] 사이드 프리팹을 반환합니다.
        public GameObject GetSidePrefab() => m_SidePrefab;

        // 생산 완료(Completed) 상태인지 반환합니다.
        public bool IsCompleted() => m_CurrentState == MachineState.Completed;
        // 배달 대기(Active) 상태인지 반환합니다.
        public bool IsActive() => m_CurrentState == MachineState.Active;

        // [Table 호출] Active 상태에서 테이블이 탭됐을 때 사이드 배달을 시작합니다.
        public void OnTableTouchedForSide(Table table)
        {
            if (m_CurrentState != MachineState.Active) return;

            DisableHandAnimationOnAllTables();
            StartCoroutine(MoveSideToTable(table));
        }

        // 머신을 Idle로 리셋하고 생산을 재개합니다.
        public void ResetMachine()
        {
            m_CurrentState = MachineState.Idle;
            DisableHandAnimationOnAllTables();
            StartProduction();
        }

        // ================================================================
        //  생산 루프
        // ================================================================

        private void InitializeLEDs()
        {
            if (m_Leds == null) return;
            foreach (var led in m_Leds)
                if (led != null) led.SetActive(false);
        }

        private void StartProduction()
        {
            if (m_CurrentState != MachineState.Idle) return;

            m_CurrentState = MachineState.InProgress;
            m_ProductionTimer = 0f;
            InitializeLEDs();
            StopGlowPulse();

            StartCoroutine(ProductionProcess());
        }

        private IEnumerator ProductionProcess()
        {
            float stageInterval = ProductionDuration / LedStageCount;

            for (int stage = 0; stage < LedStageCount; stage++)
            {
                float stageEndTime = (stage + 1) * stageInterval;
                while (m_ProductionTimer < stageEndTime)
                {
                    m_ProductionTimer += Time.deltaTime;
                    yield return null;
                }

                if (m_Leds != null && stage < m_Leds.Length && m_Leds[stage] != null)
                    m_Leds[stage].SetActive(true);
            }

            OnProductionCompleted();
        }

        private void OnProductionCompleted()
        {
            m_CurrentState = MachineState.Completed;
            AudioSource.PlayClipAtPoint(m_CompleteSfx, Camera.main.transform.position, 0.9f);
            StartGlowPulse();
            OnMachineCompleted?.Invoke(this);
        }

        // ──────────────────────────────────────────────────────────
        //  Glow 펄스 (Completed 상태 시각 피드백)
        // ──────────────────────────────────────────────────────────

        private void StartGlowPulse()
        {
            if (m_Glow == null || m_GlowPulseRoutine != null) return;
            m_Glow.enabled = true;
            m_GlowPulseRoutine = StartCoroutine(CoGlowPulse());
        }

        private void StopGlowPulse()
        {
            if (m_GlowPulseRoutine != null)
            {
                StopCoroutine(m_GlowPulseRoutine);
                m_GlowPulseRoutine = null;
            }
            if (m_Glow != null) m_Glow.enabled = false;
        }

        private IEnumerator CoGlowPulse()
        {
            float t = 0f;
            while (m_CurrentState == MachineState.Completed && m_Glow != null)
            {
                t += Time.deltaTime * GlowPulseSpeed * Mathf.PI * 2f;
                float s = (Mathf.Sin(t) + 1f) * 0.5f;
                m_Glow.GlowBrightness = Mathf.Lerp(GlowMinBrightness, GlowMaxBrightness, s);
                yield return null;
            }
            m_GlowPulseRoutine = null;
        }

        // ──────────────────────────────────────────────────────────
        //  Active 모드 — 테이블 선택 유도
        // ──────────────────────────────────────────────────────────

        private void ActivateMachine()
        {
            m_CurrentState = MachineState.Active;
            StopGlowPulse();
            UpdateHandAnimationsForActiveTables();
        }

        // ──────────────────────────────────────────────────────────
        //  사이드 배달 이동
        // ──────────────────────────────────────────────────────────

        private IEnumerator MoveSideToTable(Table table)
        {
            if (m_SidePrefab == null) { ResetMachine(); yield break; }

            var sideObj = Instantiate(m_SidePrefab);
            sideObj.name = "SideInstance";
            var sr = sideObj.GetComponent<SpriteRenderer>();
            if (sr != null) sr.sortingOrder = 9;

            sideObj.transform.position = transform.position;
            Transform targetPoint = table.GetSidePoint();
            Vector3 startPos = sideObj.transform.position;
            Vector3 targetPos = targetPoint != null ? targetPoint.position : table.transform.position;

            float elapsed = 0f;
            while (elapsed < SideMoveDuration)
            {
                float k = Mathf.SmoothStep(0f, 1f, elapsed / SideMoveDuration);
                sideObj.transform.position = Vector3.Lerp(startPos, targetPos, k);
                elapsed += Time.deltaTime;
                yield return null;
            }
            sideObj.transform.position = targetPos;

            sideObj.transform.SetParent(table.transform);
            table.OnSideMenuDelivered(sideObj);

            ResetMachine();
        }

        // ================================================================
        //  테이블 손 애니메이션 관리
        // ================================================================

        private void DisableHandAnimationOnAllTables()
        {
            foreach (var table in m_CachedTables)
            {
                if (table == null) continue;
                // Cooked/Paying 상태 손 애니는 Table.cs가 직접 관리
                if (table.State == Table.TableState.Cooked || table.State == Table.TableState.Paying)
                    continue;
                table.DisableHandAnimation();
            }
        }

        private void UpdateHandAnimationsForActiveTables()
        {
            foreach (var table in m_CachedTables)
            {
                if (table == null) continue;
                if (table.State == Table.TableState.Ordered && !table.HasSideMenu)
                    table.EnableHandAnimation(true);
                else if (table.State != Table.TableState.Cooked && table.State != Table.TableState.Paying)
                    table.DisableHandAnimation();
            }
        }

        // ================================================================
        //  이벤트 구독 / 해제
        // ================================================================

        private void SubscribeToTableEvents()
        {
            foreach (var table in m_CachedTables)
                if (table != null) table.OnOrderSet += OnAnyTableOrderChanged;
        }

        private void UnsubscribeFromTableEvents()
        {
            foreach (var table in m_CachedTables)
                if (table != null) table.OnOrderSet -= OnAnyTableOrderChanged;
        }

        private void SubscribeToGemEvents() => Board.OnGemDragged += HandleGemDragged;
        private void UnsubscribeFromGemEvents() => Board.OnGemDragged -= HandleGemDragged;

        private void OnAnyTableOrderChanged()
        {
            if (m_CurrentState == MachineState.Active)
                UpdateHandAnimationsForActiveTables();
        }

        private void HandleGemDragged(Gem gem, Vector3Int direction)
        {
            // Active 상태에서 젬이 드래그되면 → Completed로 복귀 (플레이어가 사이드 포기)
            if (m_CurrentState != MachineState.Active) return;

            m_CurrentState = MachineState.Completed;
            DisableHandAnimationOnAllTables();
            StartGlowPulse();
        }
    }
}
