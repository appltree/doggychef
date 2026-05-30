using System.Collections;
using UnityEngine;
using UnityEngine.InputSystem;
using SpriteGlow;

namespace DoggyChef
{
    [DefaultExecutionOrder(-1)] // Table(0)보다 먼저 Update 실행
    public class Machine : MonoBehaviour
    {
        public static Machine Instance { get; private set; }

        // 같은 프레임에서 Machine이 포인터를 먼저 처리했으면 Table은 무시
        private static int s_ConsumedFrame = -1;
        public static bool IsPointerConsumedThisFrame() => s_ConsumedFrame == Time.frameCount;
        private static void ConsumePointerThisFrame() => s_ConsumedFrame = Time.frameCount;
        // 생산 완료(Completed 상태 진입) 알림 이벤트
        public static event System.Action<Machine> OnMachineCompleted;

        public enum MachineState
        {
            Idle,
            InProgress,
            Completed,
            Active
        }

        [SerializeField] private GameObject sidePrefab;
        [SerializeField] private GameObject[] leds;
        [SerializeField] private AudioClip completeSfx; // 사이드 메뉴 배달 시 재생할 사운드

        private readonly float productionDuration = 10f; // 30초
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
        private Collider2D m_Collider;


        public GameObject GetSidePrefab() => sidePrefab; // Manager 자동 전달용 공개 Getter

        private void Awake()
        {
            Instance = this;
            glow = GetComponentInChildren<SpriteGlowEffect>(true);
            m_Collider = GetComponent<Collider2D>();
            InitializeLEDs();

            // Machine(본체 + LED)을 Character 레이어로 설정하여 Customer보다 앞에 렌더링
            int characterLayer = SortingLayer.NameToID("Character");
            foreach (var sr in GetComponentsInChildren<SpriteRenderer>(true))
                sr.sortingLayerID = characterLayer;
        }

        private void Start()
        {
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

            // Machine이 탭을 처리했음을 이 프레임에 기록 → Table.Update가 중복 처리하지 않도록
            ConsumePointerThisFrame();

            if (currentState == MachineState.Completed)
                ActivateMachine();
        }

        public bool IsCompleted() => currentState == MachineState.Completed;
        public bool IsActive() => currentState == MachineState.Active;


        public void OnTableTouchedForSide(Table table)
        {
            if (currentState != MachineState.Active) return;

            // 모든 테이블의 손 애니메이션 비활성화
            DisableHandAnimationOnAllTables();

            // 사이드메뉴 이동 애니메이션 시작
            // ※ currentState는 MoveSideToTable → ResetMachine() 에서 Idle로 전환됩니다.
            //   Origin과 동일하게, 여기서 즉시 상태를 바꾸지 않습니다.
            StartCoroutine(MoveSideToTable(table));
        }


        public void ResetMachine()
        {
            currentState = MachineState.Idle;
            DisableHandAnimationOnAllTables();
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

            glow.enabled = true;

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
            if (sidePrefab == null)
            {
                ResetMachine();
                yield break;
            }

            // 사이드 프리팹 인스턴스 생성
            GameObject sideObj = Instantiate(sidePrefab);
            sideObj.name = "SideInstance";

            // 말풍선 배경/레시피(~5) 위, PriceBadge(11) 아래에 표시
            var sr = sideObj.GetComponent<SpriteRenderer>();
            if (sr != null) sr.sortingOrder = 9;

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

            // 머신 다시 시작 (여기서 currentState = Idle 로 전환)
            ResetMachine();
        }

        private void DisableHandAnimationOnAllTables()
        {
            Table[] tables = FindObjectsByType<Table>();
            foreach (var table in tables)
            {
                // Cooked/Paying 상태의 손 애니메이션은 Table.cs가 관리 → 건드리지 않음
                if (table.State == Table.TableState.Cooked || table.State == Table.TableState.Paying)
                    continue;
                table.DisableHandAnimation();
            }
        }

        // 테이블 이벤트 구독
        private void SubscribeToTableEvents()
        {
            Table[] tables = FindObjectsByType<Table>();
            foreach (var table in tables)
            {
                table.OnOrderSet += OnAnyTableOrderChanged;
            }
        }

        // 테이블 이벤트 구독 해제
        private void UnsubscribeFromTableEvents()
        {
            Table[] tables = FindObjectsByType<Table>();
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
            Table[] tables = FindObjectsByType<Table>();
            foreach (var table in tables)
            {
                // Ordered + 사이드 미수령 → 손 애니메이션 활성화 (사이드 배달 유도)
                if (table.State == Table.TableState.Ordered && !table.HasSideMenu)
                {
                    table.EnableHandAnimation(true);
                }
                // Cooked/Paying 상태는 Table.cs가 직접 관리 → 건드리지 않음
                else if (table.State == Table.TableState.Cooked || table.State == Table.TableState.Paying)
                {
                    // no-op
                }
                else
                {
                    table.DisableHandAnimation();
                }
            }
        }

        // 젬 이벤트 구독
        private void SubscribeToGemEvents()
        {
            Board.OnGemDragged += HandleGemDragged;
        }

        // 젬 이벤트 구독 해제
        private void UnsubscribeFromGemEvents()
        {
            Board.OnGemDragged -= HandleGemDragged;
        }

        // 젬 드래그 이벤트 처리
        private void HandleGemDragged(Gem gem, Vector3Int direction)
        {
            // Active 상태에서 젬이 드래그되면 Completed 상태로 변경
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
}
