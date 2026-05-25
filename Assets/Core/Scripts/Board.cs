using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using Random = UnityEngine.Random;

namespace MyMatch3
{
    // ================================================================
    //  Board — Match-3 게임의 핵심 로직을 담당하는 클래스
    // ================================================================
    //
    //  ■ 전체 구조 요약
    //  ──────────────────────────────────────────────────────────────
    //  이 클래스는 매 프레임(Update)마다 아래 6단계를 순서대로 실행합니다.
    //
    //  1) CheckInput   — 플레이어의 스와이프 입력을 감지하고 검증
    //  2) TickSwap      — 두 보석이 자리를 바꾸는 애니메이션 + 매치 판정
    //  3) MoveGems      — 낙하(Falling) / 착지 반동(Bouncing) 애니메이션
    //  4) DoMatchCheck  — 착지한 보석 주변에 3개 이상 매치가 있는지 확인
    //  5) MatchTicking  — 매치된 보석들을 타이머로 삭제하고 빈 칸을 등록
    //  6) EmptyCheck    — 빈 칸 위의 보석을 떨어뜨리거나, 스포너에서 새 보석 생성
    //
    //  그 뒤에 SwipeQueue 처리와 NewTickingCells 머지가 이루어집니다.
    //
    //  ■ 핵심 데이터 구조
    //  ──────────────────────────────────────────────────────────────
    //  • CellContent (Dictionary<Vector3Int, BoardCell>)
    //      보드의 모든 칸 정보. 각 칸은 ContainingGem(현재 보석)과
    //      IncomingGem(이동 중인 보석)을 가집니다.
    //
    //  • m_TickingCells / m_NewTickingCells
    //      현재 움직이는 보석이 있는 칸 목록.
    //      MoveGems()에서 처리하고, 새로 생긴 것은 NewTickingCells에 넣어서
    //      Update() 맨 마지막에 합칩니다 (같은 프레임에 중복 처리 방지).
    //
    //  • m_EmptyCells
    //      비어 있는 칸 목록. EmptyCheck()에서 위의 보석을 떨어뜨립니다.
    //
    //  • m_CellToMatchCheck
    //      착지(바운스 완료)한 보석의 좌표 목록. DoMatchCheck()에서
    //      이 칸들을 대상으로 매치 검사를 실행합니다.
    //
    //  • m_TickingMatch
    //      현재 삭제 애니메이션이 진행 중인 매치 객체 목록.
    //
    //  ■ 보석의 생명주기 (Gem.State)
    //  ──────────────────────────────────────────────────────────────
    //  Still → (스왑 or 낙하 시작) → Falling → (목표 도달) → Bouncing
    //        → (바운스 완료) → Still → (매치 발견) → Disappearing
    //
    //  ■ 셀(BoardCell)의 이중 참조 시스템
    //  ──────────────────────────────────────────────────────────────
    //  • ContainingGem : 이 칸에 "도착 완료"한 보석
    //  • IncomingGem   : 이 칸으로 "이동 중"인 보석
    //  → 하나의 칸에 동시에 둘 다 있으면 안 됩니다.
    //  → 낙하 중: 출발 칸의 ContainingGem = null, 도착 칸의 IncomingGem = gem
    //  → 도착 시: IncomingGem → ContainingGem 으로 전환
    //
    // ================================================================

    [DefaultExecutionOrder(-9999)]  // 다른 스크립트보다 먼저 실행
    public class Board : MonoBehaviour
    {
        // ────────────── 싱글턴 ──────────────
        private static Board s_Instance;

        // ────────────── 레퍼런스 및 설정 ──────────────
        [Header("핵심 레퍼런스")]
        // Unity Grid 컴포넌트 참조 (좌표 ↔ 월드 위치 변환에 사용)
        [SerializeField] private Grid m_Grid;
        // GemPlacerTile 만 포함된 보드 전용 타일맵. 비워두면 자식에서 자동 탐색
        [SerializeField] private UnityEngine.Tilemaps.Tilemap m_BoardTilemap;

        [Header("보드 구성")]
        // 보너스를 제외한 일반 보석들의 기본 베이스 프리팹
        public Gem BaseGemPrefab;
        // 얼음 블록 프리팹 (IceBlock 컴포넌트가 붙은 GameObject)
        // LevelData.IceSpawnChance 가 0 이면 스폰되지 않습니다.
        public GameObject IceBlockPrefab;
        // 보석이 새로 생성되는 위치 (보드 맨 윗줄 + 1칸 위)
        public List<Vector3Int> SpawnerPosition = new();
        // 보드 바깥으로 더 늘릴 여백 (유닛). 테두리 이미지 두께에 맞게 조절
        [SerializeField, Min(0f)] private float m_BorderPadding = 0.3f;

        [Header("보드 내부 데이터")]
        // 좌표 → 셀 데이터 매핑. 보드의 모든 칸 정보가 여기에 있습니다.
        public Dictionary<Vector3Int, BoardCell> CellContent = new();
        // 보드 전체의 좌표 범위 (xMin~xMax, yMin~yMax)
        private BoundsInt m_BoundsInt;
        // GemType(int) → GemData(SO)로 빠르게 찾기 위한 딕셔너리
        private Dictionary<int, GemData> m_GemDataLookup;
        // 현재 보드에 생성 가능한 보석 타입 목록 (랜덤 선택용 캐시)
        private List<int> m_AvailableGemTypes = new();

        [Header("테두리 및 시각 효과")]
        [SerializeField] private SpriteRenderer m_BorderSpriteRenderer;
        [SerializeField] private SpriteRenderer m_ShineLeft;
        [SerializeField] private SpriteRenderer m_ShineRight;

        // 테두리 dirty-check 캐시 (에디터/런타임 공용)
        private Vector3Int m_LastBorderSize = Vector3Int.zero;
        private Vector2Int m_LastBorderMin = Vector2Int.zero;
        private float m_LastBorderPadding = float.MinValue;

        [Header("실시간 생명주기 리스트")]
        // 현재 프레임에서 애니메이션을 처리할 칸 목록 (낙하 or 바운스 중인 보석)
        private List<Vector3Int> m_TickingCells = new();
        // 이번 프레임 도중 새로 생긴 움직임. Update() 끝에서 m_TickingCells에 합침 (중복 방지).
        private List<Vector3Int> m_NewTickingCells = new();
        // 현재 비어 있는 칸 목록. EmptyCheck()에서 위의 보석을 끌어내림.
        private List<Vector3Int> m_EmptyCells = new();
        // 바운스가 끝난 뒤 매치 검사를 해야 하는 칸 좌표 목록
        private List<Vector3Int> m_CellToMatchCheck = new();
        // 현재 삭제 애니메이션이 진행 중인 Match 객체 목록
        private List<Match> m_TickingMatch = new();
        private List<IBoardAction> m_BoardActions = new();
        // 매치(폭발) 연출을 위해 낙하를 잠시 지연시키는 타이머
        private float m_PostMatchDelayTimer = 0.0f;

        [Header("진행 상태")]
        //  None: 스왑 없음 / Forward: 이동 중 / Return: 원위치 중
        private SwapStage m_SwapStage = SwapStage.None;
        // 스왑 중인 두 칸의 좌표 (Item1 ↔ Item2)
        private (Vector3Int, Vector3Int) m_SwappingCells;
        // 낙석 일시 정지용 잠금 카운터. RocketAction 등 진행 시 1 이상이 되어 낙하 차단.
        private int m_FreezeMoveLock = 0;
        // 게임 종료 직전 상태인지 여부
        private bool m_FinalStretch = false;

        private enum SwapStage { None, Forward, Return }

        [Header("입력 관련")]
        private bool m_InputEnabled = true;
        // 스와이프가 검증되어 다음 Update에서 처리 대기 중인지 여부
        private bool m_SwipeQueued;
        // 스와이프의 시작 칸, 끝 칸 좌표
        private Vector3Int m_StartSwipe;
        private Vector3Int m_EndSwipe;
        private float m_LastClickTime = 0.0f;
        // 마우스/터치를 처음 누른 화면 좌표 (스와이프 방향 계산에 사용)
        public Vector3 m_StartClickPosition;

        [Header("힌트 시스템")]
        // 마지막 힌트 이후 경과 시간
        private float m_SinceLastHint = 0.0f;
        // 보드가 변경되어 가능한 매치 목록을 다시 계산해야 하는지 여부
        private bool m_BoardChanged = true;
        // 힌트 인디케이터 게임오브젝트 (코드에서 동적 생성)
        private GameObject m_HintIndicator;
        // 가능한 스왑 목록
        private List<PossibleSwap> m_PossibleSwaps = new();
        // 목록 중 무작위로 선택된 힌트 인덱스
        private int m_PickedSwap = 0;

        public class PossibleSwap
        {
            public Vector3Int StartPosition;
            public Vector3Int Direction;
            /// <summary>4=ColorClean, 3=LargeBomb, 2=LineRocket, 1=SmallBomb, 0=일반3매치</summary>
            public int BonusPriority;
        }

        [Header("보너스 및 콜백")]
        private Booster m_ActivatedBonusItem = null;
        private Dictionary<Vector3Int, System.Action> m_MatchedCallback = new();
        // 특정 칸의 보석이 삭제될 때 호출될 콜백 목록
        private Dictionary<Vector3Int, System.Action> m_CellsCallbacks = new();

        // ══════════════════════════════════════════════════════════
        //  [타이쿤 추가] Board → Hall 통신 이벤트
        // ══════════════════════════════════════════════════════════
        //  역할:
        //  보석이 매치로 파괴되는 순간, 이 이벤트를 발사합니다.
        //  HallManager가 이 위치 및 정보를 받아 테이블에 재료를 전달합니다.
        //
        //  파라미터:
        //    Gem    → gem.Data.BlockTag 로 재료 식별
        //    Vector3 → 파괴 위치 (날아가는 애니메이션 시작점)
        // ══════════════════════════════════════════════════════════
        public static event Action<Gem, Vector3> OnGemExploded;
        public static event Action<Gem, Vector3Int> OnGemDragged;

        // SmallBomb 머지 연출이 끝난 후 지연 발화하는 폭발 이벤트.
        // Gem 오브젝트가 이미 파괴된 시점이므로 필요한 데이터를 미리 추출해 전달합니다.
        // HallManager가 구독하여 AcceptGem()을 호출합니다.
        public static event Action<string, Vector3, Sprite, AudioClip> OnGemExplodedDelayed;

        /// <summary>SmallBomb 코루틴에서 외부 발화용 (event는 선언 클래스 내부에서만 Invoke 가능)</summary>
        public static void FireGemExplodedDelayed(string tag, Vector3 pos, Sprite sprite, AudioClip foodSfx)
            => OnGemExplodedDelayed?.Invoke(tag, pos, sprite, foodSfx);

        /// <summary>
        /// delay초 후에 OnGemExplodedDelayed를 count회 발화합니다.
        /// ColorClean처럼 이펙트 재생이 끝난 후 재료를 날려야 할 때 사용합니다.
        /// </summary>
        private IEnumerator FireExplodeAfterDelay(
            string tag, Vector3 pos, Sprite sprite, AudioClip foodSfx, int count, float delay)
        {
            yield return new WaitForSeconds(delay);
            for (int ec = 0; ec < count; ec++)
                OnGemExplodedDelayed?.Invoke(tag, pos, sprite, foodSfx);
        }

        public Grid Grid => m_Grid;
        public BoundsInt Bounds => m_BoundsInt;

        // ================================================================
        //  Awake / Start
        // ================================================================
        void Awake()
        {
            s_Instance = this;
            GetReference();
        }

#if UNITY_EDITOR
        void OnEnable()
        {
            if (Application.isPlaying) return;
            // 컴포넌트 활성화 / 씬 로드 시 에디터에서 즉시 border 갱신
            UnityEditor.EditorApplication.delayCall += RefreshBorderInEditor;
        }
#endif

        void Start()
        {
            // ── 타이쿤 스테이지 데이터 기반 보석 세팅 ──
            if (LevelData.Instance != null && LevelData.Instance.CurrentStage != null)
            {
                var stageGems = LevelData.Instance.CurrentStage.AvailableGems;
                if (stageGems != null && stageGems.Count > 0)
                {
                    m_GemDataLookup = new Dictionary<int, GemData>();
                    m_AvailableGemTypes = new List<int>();

                    foreach (var data in stageGems)
                    {
                        if (data != null && !m_GemDataLookup.ContainsKey(data.GemType))
                        {
                            m_GemDataLookup.Add(data.GemType, data);
                            m_AvailableGemTypes.Add(data.GemType);
                        }
                    }
                }
            }

            GenerateBoard();
            SetupBorder();
            FindAllPossibleMatch();

            // HintPrefab을 씬에 동적 생성하고 기본 비활성화
            // (인스펙터에서 직접 연결하는 방식이 아닌, 프리팹에서 Instantiate)
            m_HintIndicator = Instantiate(GameManager.Instance.Settings.VisualSettings.HintPrefab);
            m_HintIndicator.SetActive(false);
        }

        // ================================================================
        //  Update — 매 프레임 실행되는 메인 루프
        // ================================================================
        //  실행 순서가 매우 중요합니다! 아래 순서를 바꾸면 버그가 생깁니다.
        //
        //  ① CheckInput    : 플레이어 입력 감지 → m_SwipeQueued = true
        //  ② TickSwap      : 스왑 애니메이션 진행 → 매치 검사 → 실패 시 Return
        //  ③ MoveGems      : 낙하/바운스 애니메이션 → 착지 시 m_CellToMatchCheck에 추가
        //  ④ DoMatchCheck   : 매치 검사 → 매치 발견 시 m_TickingMatch에 등록
        //  ⑤ MatchTicking   : 매치된 보석 삭제 타이머 → 삭제 시 m_EmptyCells에 추가
        //  ⑥ EmptyCheck     : 빈 칸 위의 보석을 떨어뜨림 → m_NewTickingCells에 추가
        //  ⑦ SwipeQueue     : 예약된 스와이프를 IncomingGem에 세팅
        //  ⑧ NewTickingCells: 새로 생긴 움직임을 m_TickingCells에 합침
        // ================================================================
        void Update()
        {
            // 액션 처리 (로켓 날아가기 등)
            for (int i = 0; i < m_BoardActions.Count; ++i)
            {
                if (!m_BoardActions[i].Tick())
                {
                    m_BoardActions.RemoveAt(i);
                    i--;
                }
            }

            // [추가] 낙하 지연 타이머 갱신
            if (m_PostMatchDelayTimer > 0)
            {
                m_PostMatchDelayTimer -= Time.deltaTime;
            }

            // ① 입력 감지
            CheckInput();

            // ② 스왑 애니메이션 (진행 중일 때만)
            if (m_SwapStage != SwapStage.None)
            {
                TickSwap();
            }

            // ③ 이번 프레임에 아무것도 일어나지 않을 때만 카운터를 올림
            bool incrementHintTimer = m_ActivatedBonusItem == null;

            if (m_TickingCells.Count > 0)
            {
                MoveGems();

                incrementHintTimer = false;
                m_BoardChanged = true;
            }

            // ④ 매치 검사 (착지한 보석이 있을 때만)
            if (m_CellToMatchCheck.Count > 0)
            {
                DoMatchCheck();
                incrementHintTimer = false;
                m_BoardChanged = true;
            }

            // ⑤ 매치된 보석 삭제 애니메이션 (삭제 대기 중인 매치가 있을 때만)
            if (m_TickingMatch.Count > 0)
            {
                MatchTicking();
                incrementHintTimer = false;
                m_BoardChanged = true;
            }

            // ⑥ 빈 칸 채우기 (빈 칸이 있을 때만 + 폭발 연출 지연이 끝났을 때만)
            if (m_EmptyCells.Count > 0 && m_PostMatchDelayTimer <= 0)
            {
                EmptyCheck();
                incrementHintTimer = false;
                m_BoardChanged = true;
            }

            // ⑦ 스와이프 예약 처리
            if (m_SwipeQueued)
            {
                CellContent[m_StartSwipe].IncomingGem = CellContent[m_EndSwipe].ContainingGem;
                CellContent[m_EndSwipe].IncomingGem = CellContent[m_StartSwipe].ContainingGem;

                CellContent[m_StartSwipe].ContainingGem = null;
                CellContent[m_EndSwipe].ContainingGem = null;

                m_SwapStage = SwapStage.Forward;
                m_SwappingCells = (m_StartSwipe, m_EndSwipe);

                GameManager.Instance.PlaySFX(GameManager.Instance.Settings.SoundSettings.SwapSound);

                m_SwipeQueued = false;
                incrementHintTimer = false;
            }

            // ⑧ 이번 프레임에서 새로 등록된 움직임을 합침
            if (m_NewTickingCells.Count > 0)
            {
                m_TickingCells.AddRange(m_NewTickingCells);
                m_NewTickingCells.Clear();
                incrementHintTimer = false;
            }

            // [아무 일도 일어나지 않는 고요한 프레임인 경우]
            if (incrementHintTimer)
            {
                // [게임 종료 처리]
                // TriggerFinalStretch()가 호출되었다면(목표 달성 or 이동 소진),
                // 보드가 완전히 멈춘 이 순간 종료 화면을 표시합니다.
                if (m_FinalStretch)
                {
                    m_FinalStretch = false; // 종료가 두 번 실행되는 것을 방지하기 위해 즉시 리셋
                    //-- UIHandler.Instance.ShowEnd(); // 추후 UIHandler 완성 시 주석 해제
                    return;
                }

                // [가능한 매치 목록 갱신]
                // 보석이 삭제/이동되어 보드 상태가 변했다면, 이 시점에 힌트 후보를 다시 계산합니다.
                // (보드가 움직이는 도중에 계산하지 않고, 완전히 멈춘 뒤 한 번만 계산하는 것이 핵심!)
                if (m_BoardChanged)
                {
                    FindAllPossibleMatch(); // 추후 주석 해제
                    m_BoardChanged = false; // 갱신 완료 → 다음에 또 변화가 생기기 전까지는 갱신 불필요
                }

                // 현재 힌트로 보여줄 스왑 정보를 목록에서 하나 꺼냄
                var match = m_PossibleSwaps[m_PickedSwap];

                // [힌트 인디케이터가 이미 화면에 켜져 있는 경우 → 왕복 애니메이션]
                if (m_HintIndicator.activeSelf)
                {
                    // 힌트 시작 위치(startPos)와 스왑 목표 위치(endPos)를 월드 좌표로 계산
                    var startPos = m_Grid.GetCellCenterWorld(match.StartPosition);
                    var endPos = m_Grid.GetCellCenterWorld(match.StartPosition + match.Direction);

                    // 현재 인디케이터(화살표) 위치를 초당 1.0 속도로 endPos 방향으로 이동시킴
                    var current = m_HintIndicator.transform.position;
                    current = Vector3.MoveTowards(current, endPos, 1.0f * Time.deltaTime);

                    // endPos에 도달하면 다시 startPos로 순간이동 → 왕복하는 것처럼 보임
                    m_HintIndicator.transform.position = current == endPos ? startPos : current;
                }
                // [힌트 인디케이터가 꺼져 있는 경우 → 대기 시간 카운트]
                else
                {
                    // 아무것도 안 한 시간을 누적
                    m_SinceLastHint += Time.deltaTime;

                    // 설정된 대기 시간(InactivityBeforeHint, 기본 2초)이 지나면 힌트 표시
                    if (m_SinceLastHint >= GameManager.Instance.Settings.InactivityBeforeHint && m_InputEnabled)
                    {
                        // 힌트 화살표를 힌트 보석 위치로 이동시키고 활성화
                        m_HintIndicator.transform.position = m_Grid.GetCellCenterWorld(match.StartPosition);
                        m_HintIndicator.SetActive(true);
                    }
                }
            }
            // [보드가 움직이는 중(= 플레이어가 입력했거나 애니메이션 진행 중)인 경우]
            else
            {
                // 플레이어가 뭔가 했으므로 힌트 화살표를 즉시 숨김
                m_HintIndicator.SetActive(false);
                // 타이머도 초기화 → 다시 2초를 기다려야 힌트가 나옴
                m_SinceLastHint = 0.0f;
            }



        }


        // ================================================================
        //  FindAllPossibleMatch — 현재 보드에서 가능한 모든 매치를 탐색
        // ================================================================
        //
        //  핵심 아이디어: 실제로 스왑을 해보고, 매치가 생기면 후보에 추가하고, 원상복구!
        //
        //  최적화 포인트: x, y를 증가 순서로만 순회하므로
        //  "위쪽"과 "오른쪽"으로만 스왑 테스트해도 충분합니다.
        //  왜냐하면 (A→아래)는 이미 (아래칸→위)로 이전 루프에서 검사했기 때문입니다.
        // ================================================================
        void FindAllPossibleMatch()
        {
            m_PossibleSwaps.Clear(); // 이전 탐색 결과를 전부 지우고 새로 시작

            for (int y = m_BoundsInt.yMin; y <= m_BoundsInt.yMax; ++y)
            {
                for (int x = m_BoundsInt.xMin; x <= m_BoundsInt.xMax; ++x)
                {
                    var idx = new Vector3Int(x, y, 0);

                    // 이 칸이 존재하고, 보석이 이동 가능한 상태인지 확인
                    if (CellContent.TryGetValue(idx, out var cell) && cell.CanBeMoved)
                    {
                        var topIdx = idx + Vector3Int.up;
                        var rightIdx = idx + Vector3Int.right;

                        // ── 1. 위쪽 칸과 스왑 시뮬레이션 ──
                        if (CellContent.TryGetValue(topIdx, out var topCell) && topCell.CanBeMoved)
                        {
                            // 두 칸의 보석을 실제로 교체 (C# 튜플 구조분해 할당)
                            (CellContent[idx].ContainingGem, CellContent[topIdx].ContainingGem) =
                                (CellContent[topIdx].ContainingGem, CellContent[idx].ContainingGem);

                            // DoCheck(cell, false): 두 번째 인자 false = 실제로 터트리지 않고 검사만!
                            if (DoCheck(topIdx, false)) // 위 칸에서 매치가 생겼으면
                                m_PossibleSwaps.Add(new PossibleSwap() { StartPosition = idx, Direction = Vector3Int.up, BonusPriority = EvalBonusPriority(topIdx) });
                            if (DoCheck(idx, false))    // 현재 칸에서 매치가 생겼으면
                                m_PossibleSwaps.Add(new PossibleSwap() { StartPosition = topIdx, Direction = Vector3Int.down, BonusPriority = EvalBonusPriority(idx) });

                            // 다시 원래대로 복구 (보드 상태를 실제로 바꾸면 안 됨)
                            (CellContent[idx].ContainingGem, CellContent[topIdx].ContainingGem) =
                                (CellContent[topIdx].ContainingGem, CellContent[idx].ContainingGem);
                        }

                        // ── 2. 오른쪽 칸과 스왑 시뮬레이션 ──
                        if (CellContent.TryGetValue(rightIdx, out var rightCell) && rightCell.CanBeMoved)
                        {
                            (CellContent[idx].ContainingGem, CellContent[rightIdx].ContainingGem) =
                                (CellContent[rightIdx].ContainingGem, CellContent[idx].ContainingGem);

                            if (DoCheck(rightIdx, false))
                                m_PossibleSwaps.Add(new PossibleSwap() { StartPosition = idx, Direction = Vector3Int.right, BonusPriority = EvalBonusPriority(rightIdx) });
                            if (DoCheck(idx, false))
                                m_PossibleSwaps.Add(new PossibleSwap() { StartPosition = rightIdx, Direction = Vector3Int.left, BonusPriority = EvalBonusPriority(idx) });

                            (CellContent[idx].ContainingGem, CellContent[rightIdx].ContainingGem) =
                                (CellContent[rightIdx].ContainingGem, CellContent[idx].ContainingGem);
                        }
                    }
                }
            }

            // ── 힌트 우선순위: Ordered 테이블의 필요 재료 태그 우선 (Origin BoardAIAssistant 방식) ──
            // 현재 Ordered 상태인 테이블에서 아직 채워지지 않은 재료 태그를 수집합니다.
            // 해당 태그와 매치되는 스왑이 있으면 그 중 하나를 힌트로 선택합니다.
            // 없으면 기존처럼 전체 후보 중 랜덤 선택합니다.
            m_PickedSwap = PickSwapWithOrderPriority();
        }

        // ================================================================
        //  PickSwapWithOrderPriority — Ordered 테이블 재료 우선 힌트 선택
        // ================================================================
        //  우선순위:
        //  1순위 — heart(체력)가 가장 낮은 손님 테이블의 재료 태그와 일치하는 스왑
        //          (체력 오름차순으로 테이블을 순회, 첫 번째 매칭 집합에서 결정)
        //  2순위 — 손님 매칭 없으면 전체 후보
        //  최종  — 후보 중 BonusPriority 최고값 선택
        //          ColorClean(4) > LargeBomb(3) > LineRocket(2) > SmallBomb(1) > 3매치(0)
        // ================================================================
        private int PickSwapWithOrderPriority()
        {
            if (m_PossibleSwaps.Count == 0) return 0;

            // ── 1단계: Ordered 테이블을 CustomerCurrentHealth 오름차순 정렬 ──
            var tables = FindObjectsByType<Table>(FindObjectsSortMode.None);
            var orderedTables = new List<Table>();
            foreach (var t in tables)
                if (t != null && t.State == Table.TableState.Ordered) orderedTables.Add(t);
            orderedTables.Sort((a, b) => a.CustomerCurrentHealth.CompareTo(b.CustomerCurrentHealth));

            // ── 2단계: 체력 낮은 손님부터 매칭 스왑 탐색 ──
            var candidates = new List<int>();
            foreach (var table in orderedTables)
            {
                var order = table.CurrentOrder;
                if (order?.Ingredients == null) continue;

                var tableTags = new HashSet<string>();
                foreach (var ing in order.Ingredients)
                {
                    if (ing.MaterialGem == null) continue;
                    string tag = ing.MaterialGem.BlockTag;
                    if (!string.IsNullOrEmpty(tag) && table.GetRemainingNeed(tag) > 0)
                        tableTags.Add(tag);
                }
                if (tableTags.Count == 0) continue;

                for (int i = 0; i < m_PossibleSwaps.Count; i++)
                {
                    var swap = m_PossibleSwaps[i];
                    if (CellContent.TryGetValue(swap.StartPosition, out var cell)
                        && cell.ContainingGem?.Data != null
                        && !string.IsNullOrEmpty(cell.ContainingGem.Data.BlockTag)
                        && tableTags.Contains(cell.ContainingGem.Data.BlockTag))
                    {
                        candidates.Add(i);
                    }
                }

                // 가장 체력 낮은 손님에 맞는 후보를 찾았으면 이후 테이블은 무시
                if (candidates.Count > 0) break;
            }

            // 손님 매칭 후보가 없으면 전체 후보 사용
            if (candidates.Count == 0)
                for (int i = 0; i < m_PossibleSwaps.Count; i++) candidates.Add(i);

            // ── 3단계: 후보 중 BonusPriority 최고값 선택 ──
            int maxPriority = -1;
            foreach (int i in candidates)
                if (m_PossibleSwaps[i].BonusPriority > maxPriority)
                    maxPriority = m_PossibleSwaps[i].BonusPriority;

            var best = new List<int>();
            foreach (int i in candidates)
                if (m_PossibleSwaps[i].BonusPriority == maxPriority)
                    best.Add(i);

            return best[Random.Range(0, best.Count)];
        }






        // 낙하 잠금 — RocketAction 시작 시 호출합니다.
        // m_FreezeMoveLock > 0 이면 EmptyCheck가 실행되지 않아 보석이 낙하하지 않습니다.
        public void LockMovement()
        {
            m_FreezeMoveLock += 1;
        }

        // 낙하 잠금 해제 — RocketAction 종료 시 호출합니다.
        public void UnlockMovement()
        {
            m_FreezeMoveLock -= 1;
            if (m_FreezeMoveLock < 0)
                m_FreezeMoveLock = 0;
        }

        // [추가] 공통 Lock 시스템을 이용해 특정 시간 동안 보드를 잠급니다.
        public void LockMovementFor(float seconds)
        {
            StartCoroutine(CoLockMovement(seconds));
        }

        private IEnumerator CoLockMovement(float seconds)
        {
            LockMovement();
            yield return new WaitForSeconds(seconds);
            UnlockMovement();
        }

        // 보너스 등에 의해 강제로 보석 파괴
        public void DestroyGem(Vector3Int cell, bool isBonusEffect = false)
        {
            if (CellContent.TryGetValue(cell, out var content) && content.ContainingGem != null)
            {
                // 1. 매치 진행 중이면 중복 파괴 방지
                if (content.ContainingGem.CurrentMatch != null) return;

                // Match 시스템을 경유해서 삭제 (그래야 점수도 오르고 VFX도 나옴)
                var match = CreateCustomMatch(cell);
                match.ForcedDeletion = true; // 즉시 삭제 모드
                match.AddGem(content.ContainingGem);
            }
        }

        public Vector3 GetCellCenter(Vector3Int cell)
        {
            return m_Grid.GetCellCenterWorld(cell);
        }
        public Vector3Int WorldToCell(Vector3 pos)
        {
            return m_Grid.WorldToCell(pos);
        }

        // ================================================================
        //  MoveGems — 낙하(Falling) / 착지 반동(Bouncing) 애니메이션
        // ================================================================
        //
        //  m_TickingCells에 등록된 각 칸을 순회하며:
        //  - Falling 상태: 보석을 목표 위치로 이동. 도착하면 아래가 비었는지
        //    확인하여 계속 낙하하거나, 바운스 상태로 전환.
        //  - Bouncing 상태: 착지 반동 애니메이션. 완료 시 매치 검사 대기열에 등록.
        //  - Still 상태: 이미 정지한 보석이면 목록에서 제거 (비정상 상황 방어).
        //
        // ================================================================
        void MoveGems()
        {
            // ── 정렬: 아래쪽(y가 작은) 보석부터 처리 ──
            // 이유: 위의 보석이 먼저 처리되면, 아래 칸이 아직 비워지지 않아서
            //       "아래가 비어있는지" 체크가 잘못될 수 있음.
            m_TickingCells.Sort((a, b) =>
            {
                int yCmp = a.y.CompareTo(b.y);
                return yCmp != 0 ? yCmp : a.x.CompareTo(b.x);
            });

            for (int i = 0; i < m_TickingCells.Count; ++i)
            {
                var cellIdx = m_TickingCells[i];
                var cell = CellContent[cellIdx];

                // ──────────────────────────────────────────
                //  Falling 상태: 목표 칸을 향해 아래로 이동 중
                // ──────────────────────────────────────────
                //  이 칸의 IncomingGem이 Falling이면, 보석이 이 칸으로 오고 있는 것.
                //  (출발 칸의 ContainingGem은 이미 null로 비워져 있음)
                if (cell.IncomingGem?.CurrentState == Gem.State.Falling)
                {
                    var gem = cell.IncomingGem;
                    gem.TickMoveTimer(Time.deltaTime);  // 낙하 경과 시간 누적

                    // 이 칸의 월드 좌표(중앙)를 목표로 이동
                    Vector3 targetPos = m_Grid.GetCellCenterWorld(cellIdx);
                    var maxDistance = GameManager.Instance.Settings.VisualSettings.FallAccelerationCurve.Evaluate(gem.FallTime) *
                                      Time.deltaTime * GameManager.Instance.Settings.VisualSettings.FallSpeed * gem.SpeedMultiplier;

                    gem.transform.position = Vector3.MoveTowards(gem.transform.position, targetPos,
                        maxDistance);


                    // ── 목표 지점에 도달했을 때 ──
                    if (gem.transform.position == targetPos)
                    {
                        // 이 칸의 처리가 끝났으므로 m_TickingCells에서 제거
                        m_TickingCells.RemoveAt(i);
                        i--;

                        // IncomingGem → ContainingGem 전환 (도착 확정)
                        cell.IncomingGem = null;
                        cell.ContainingGem = gem;
                        gem.MoveTo(cellIdx);  // Gem 내부 좌표도 갱신

                        // ── 아래 칸이 비어있으면: 멈추지 않고 계속 낙하 ──
                        //    매치3에서 보석은 바닥에 닿을 때까지 쭉 떨어져야 합니다.
                        //    한 칸 한 칸 멈추면 뚝뚝 끊겨보이기 때문입니다.
                        if (m_EmptyCells.Contains(cellIdx + Vector3Int.down)
                            && CellContent.TryGetValue(cellIdx + Vector3Int.down, out var belowCell))
                        {
                            // 이 칸을 다시 비우고, 아래 칸의 IncomingGem으로 보석을 넘김
                            cell.ContainingGem = null;
                            belowCell.IncomingGem = gem;

                            gem.SpeedMultiplier = 1.0f;

                            var target = cellIdx + Vector3Int.down;
                            m_NewTickingCells.Add(target);  // 다음 프레임에 아래 칸 처리

                            // 빈 칸 목록 갱신: 아래 칸은 더 이상 비지 않고, 이 칸이 비게 됨
                            m_EmptyCells.Remove(target);
                            m_EmptyCells.Add(cellIdx);

                            // 이 칸이 비었으니, 바로 위가 스포너면 새 보석 생성
                            if (SpawnerPosition.Contains(cellIdx + Vector3Int.up))
                            {
                                ActivateSpawnerAt(cellIdx);
                            }
                        }
                        // ── 왼쪽 아래로 흐르기 (대각선 낙하) ──
                        //    (왼쪽이 벽이거나 막혀있어서) + (왼쪽 아래가 비어있으면)
                        else if ((!CellContent.TryGetValue(cellIdx + Vector3Int.left, out var leftCell) ||
                                  leftCell.BlockFall) &&
                                 m_EmptyCells.Contains(cellIdx + Vector3Int.down + Vector3Int.left) &&
                                 CellContent.TryGetValue(cellIdx + Vector3Int.down + Vector3Int.left, out var belowLeftCell))
                        {
                            cell.ContainingGem = null;
                            belowLeftCell.IncomingGem = gem;

                            gem.SpeedMultiplier = 1.41421356237f; // 대각선 거리(√2) 보정

                            var target = cellIdx + Vector3Int.down + Vector3Int.left;
                            m_NewTickingCells.Add(target);

                            m_EmptyCells.Remove(target);
                            m_EmptyCells.Add(cellIdx);

                            if (SpawnerPosition.Contains(cellIdx + Vector3Int.up))
                            {
                                ActivateSpawnerAt(cellIdx);
                            }
                        }
                        // ── 오른쪽 아래로 흐르기 (대각선 낙하) ──
                        //    (오른쪽이 벽이거나 막혀있어서) + (오른쪽 아래가 비어있으면)
                        else if ((!CellContent.TryGetValue(cellIdx + Vector3Int.right, out var rightCell) ||
                                  rightCell.BlockFall) &&
                                 m_EmptyCells.Contains(cellIdx + Vector3Int.down + Vector3Int.right) &&
                                 CellContent.TryGetValue(cellIdx + Vector3Int.down + Vector3Int.right, out var belowRightCell))
                        {
                            cell.ContainingGem = null;
                            belowRightCell.IncomingGem = gem;

                            gem.SpeedMultiplier = 1.41421356237f;

                            var target = cellIdx + Vector3Int.down + Vector3Int.right;
                            m_NewTickingCells.Add(target);

                            m_EmptyCells.Remove(target);
                            m_EmptyCells.Add(cellIdx);

                            if (SpawnerPosition.Contains(cellIdx + Vector3Int.up))
                            {
                                ActivateSpawnerAt(cellIdx);
                            }
                        }
                        // ── 아래가 막혀있으면: 착지 → 바운스 시작 ──
                        else
                        {
                            gem.StopFalling();              // State → Bouncing, FallTime 리셋
                            m_NewTickingCells.Add(cellIdx); // 바운스 처리를 위해 다시 등록

                            // [번외 추가] 그룹 착지가 아닌, 개별 보석이 1개씩 착지할 때마다 즉시 사운드를 발생시킵니다.
                            // 그냥 들으면 밋밋하므로 랜덤 피치(음높이)를 주어 16개 풀링 시스템의 매력을 극대화합니다.
                            var sfx = GameManager.Instance.PlaySFX(GameManager.Instance.Settings.SoundSettings.FallSound);
                            if (sfx != null) sfx.pitch = Random.Range(0.8f, 1.2f);
                        }
                    }
                }
                // ──────────────────────────────────────────
                //  Bouncing 상태: 착지 후 통통 튀는 연출
                // ──────────────────────────────────────────
                //  착지한 보석이 살짝 위로 튀었다가 내려오는 시각 효과.
                //  0.3초 동안 사인파 바운스 + 스케일 찌그러짐(squash/stretch)을
                //  적용한 뒤, 완료되면 매치 검사 대기열에 추가합니다.
                else if (cell.ContainingGem?.CurrentState == Gem.State.Bouncing)
                {
                    var gem = cell.ContainingGem;
                    gem.TickMoveTimer(Time.deltaTime);
                    Vector3 center = m_Grid.GetCellCenterWorld(cellIdx);

                    float maxTime = GameManager.Instance.Settings.VisualSettings.BounceCurve
                        .keys[GameManager.Instance.Settings.VisualSettings.BounceCurve.length - 1].time;

                    if (gem.FallTime >= maxTime)
                    {
                        gem.transform.position = center;
                        gem.transform.localScale = Vector3.one;
                        gem.StopBouncing();

                        m_TickingCells.RemoveAt(i);
                        i--;
                        m_CellToMatchCheck.Add(cellIdx);
                    }
                    else
                    {
                        // 바운스 높이가 너무 크다는 피드백을 반영하여 임의의 배수인 0.3f를 곱해 크게 튀어오르지 않도록 조절합니다.
                        gem.transform.position =
                            center + Vector3.up * (GameManager.Instance.Settings.VisualSettings.BounceCurve.Evaluate(gem.FallTime) * 0.3f);
                        gem.transform.localScale =
                            new Vector3(1, GameManager.Instance.Settings.VisualSettings.SquishCurve.Evaluate(gem.FallTime), 1);
                    }
                }
                // ──────────────────────────────────────────
                //  Still 상태: 이미 정지한 보석 (비정상 상황 방어용)
                // ──────────────────────────────────────────
                //  정상적이라면 여기 오지 않지만, 혹시 m_TickingCells에
                //  Still 보석이 남아있다면 제거합니다.
                else if (cell.ContainingGem?.CurrentState == Gem.State.Still)
                {
                    m_TickingCells.RemoveAt(i);
                    i--;
                }
            }
        }

        // ================================================================
        //  EmptyCheck — 빈 칸을 감지하고 위의 보석을 떨어뜨리기
        // ================================================================
        //
        //  m_EmptyCells 목록의 각 빈 칸에 대해:
        //  1) 이미 보석이 들어오고 있으면 → 목록에서 제거
        //  2) 바로 윗칸에 떨어질 수 있는 보석이 있으면 → 낙하 시작
        //  3) 윗칸이 스포너 위치면 → 새 보석 생성
        //
        //  ※ 이 함수가 매 프레임 호출되므로, 위에서부터 차례로 보석이
        //     한 칸씩 떨어지며 연쇄적으로 빈 칸이 전파됩니다.
        //
        // ================================================================
        void EmptyCheck()
        {
            // ── 로켓 등 BoardAction이 진행 중이면 낙하를 일시 차단 ──
            // (원본과 동일: RocketAction이 날아가는 동안 보석이 떨어지면 안 되므로)
            if (m_FreezeMoveLock > 0)
                return;

            for (int i = 0; i < m_EmptyCells.Count; ++i)
            {
                var emptyCell = m_EmptyCells[i];

                // ── 이미 보석이 들어오고 있으면 더 이상 빈 칸이 아님 ──
                if (!CellContent[emptyCell].IsEmpty())
                {
                    m_EmptyCells.RemoveAt(i);
                    i--;
                    continue;
                }

                // 바로 윗칸의 좌표와 셀 데이터
                var aboveCellIdx = emptyCell + Vector3Int.up;
                bool aboveCellExist = CellContent.TryGetValue(aboveCellIdx, out var aboveCell);

                // ── 케이스 1: 윗칸에 떨어질 수 있는 보석이 있다 ──
                //    CanFall = 보석이 있고, 잠기지 않았고, 매치 중이 아니고, Still 상태
                if (aboveCellExist && aboveCell.ContainingGem != null && aboveCell.CanFall)
                {
                    var fallingGem = aboveCell.ContainingGem;

                    // 빈 칸의 IncomingGem으로 보석을 예약
                    CellContent[emptyCell].IncomingGem = fallingGem;
                    // 윗칸에서 보석을 제거
                    aboveCell.ContainingGem = null;

                    // 보석의 낙하 상태 시작 (State → Falling)
                    fallingGem.StartMoveTimer();

                    // 다음 프레임에 MoveGems()에서 처리되도록 등록
                    m_NewTickingCells.Add(emptyCell);

                    // 빈 칸 목록 갱신: 윗칸이 새로 비었고, 이 칸은 보석이 오고 있으므로 제거
                    m_EmptyCells.Add(aboveCellIdx);
                    m_EmptyCells.RemoveAt(i);
                    i--;
                }
                // [추가] 윗칸이 막혔을 때, 오른쪽 위에서 대각선으로 가져오기 (보석 입장에선 왼쪽 아래로 낙하)
                else if ((!aboveCellExist || aboveCell.BlockFall) &&
                         CellContent.TryGetValue(aboveCellIdx + Vector3Int.right, out var aboveRightCell) &&
                         aboveRightCell.ContainingGem != null && aboveRightCell.CanFall)
                {
                    var incomingGem = aboveRightCell.ContainingGem;
                    CellContent[emptyCell].IncomingGem = incomingGem;
                    aboveRightCell.ContainingGem = null;

                    incomingGem.StartMoveTimer();
                    incomingGem.SpeedMultiplier = 1.41421356237f;

                    m_NewTickingCells.Add(emptyCell);

                    m_EmptyCells.Add(aboveCellIdx + Vector3Int.right);
                    m_EmptyCells.RemoveAt(i);
                    i--;
                }
                // [추가] 윗칸이 막혔을 때, 왼쪽 위에서 대각선으로 가져오기 (보석 입장에선 오른쪽 아래로 낙하)
                else if ((!aboveCellExist || aboveCell.BlockFall) &&
                         CellContent.TryGetValue(aboveCellIdx + Vector3Int.left, out var aboveLeftCell) &&
                         aboveLeftCell.ContainingGem != null && aboveLeftCell.CanFall)
                {
                    var incomingGem = aboveLeftCell.ContainingGem;
                    CellContent[emptyCell].IncomingGem = incomingGem;
                    aboveLeftCell.ContainingGem = null;

                    incomingGem.StartMoveTimer();
                    incomingGem.SpeedMultiplier = 1.41421356237f;

                    m_NewTickingCells.Add(emptyCell);

                    m_EmptyCells.Add(aboveCellIdx + Vector3Int.left);
                    m_EmptyCells.RemoveAt(i);
                    i--;
                }
                // ── 케이스 2: 윗칸이 스포너 위치다 ──
                //    보드 위쪽 경계 밖(yMax+1)에 스포너가 있으면 새 보석을 생성
                else if (SpawnerPosition.Contains(aboveCellIdx))
                {
                    // ActivateSpawnerAt 내부에서 m_EmptyCells.Remove(cellIdx)를 처리하므로
                    // 여기서는 인덱스만 보정 (i--)
                    ActivateSpawnerAt(emptyCell);
                    i--;
                }
                // ── 그 외: 윗칸이 없거나 막혀있으면 이번 프레임에선 아무것도 안 함 ──
                //    다음 프레임에 다시 시도 (위의 보석이 떨어져 올 수도 있으므로)
            }
        }

        // ================================================================
        //  TickSwap — 스왑 애니메이션 + 매치 판정
        // ================================================================
        //
        //  두 보석이 자리를 바꾸는 과정:
        //
        //  [Forward 단계]
        //  1. 두 보석이 서로의 위치로 이동 (IncomingGem으로 이동 중)
        //  2. 도착하면 잠금 해제 후 매치 검사
        //  3-A. 매치 성공 → IncomingGem 해제, 스왑 종료
        //  3-B. 매치 실패 → IncomingGem을 교차하여 Return 단계로
        //
        //  [Return 단계]
        //  1. 두 보석이 원래 자리로 돌아감
        //  2. 도착하면 IncomingGem → ContainingGem 확정, 잠금 해제, 스왑 종료
        //
        // ================================================================
        void TickSwap()
        {
            // 현재 이동 중인 두 보석 가져오기
            var gemToStart = CellContent[m_SwappingCells.Item1].IncomingGem;
            var gemToEnd = CellContent[m_SwappingCells.Item2].IncomingGem;

            // 각 칸의 월드 좌표 (이동 목표)
            var startPosition = m_Grid.GetCellCenterWorld(m_SwappingCells.Item1);
            var endPosition = m_Grid.GetCellCenterWorld(m_SwappingCells.Item2);

            // 두 보석을 각자의 목표 위치로 이동 (속도 5.0)
            gemToStart.transform.position = Vector3.MoveTowards(
                gemToStart.transform.position, startPosition, Time.deltaTime * 5.0f);
            gemToEnd.transform.position = Vector3.MoveTowards(
                gemToEnd.transform.position, endPosition, Time.deltaTime * 5.0f);

            // 두 보석 모두 목표에 도달했는지 확인 (하나만 체크해도 됨 — 같은 거리이므로)
            if (gemToStart.transform.position == startPosition)
            {
                // ── Forward 단계 완료 ──
                if (m_SwapStage == SwapStage.Forward)
                {
                    // 매치 검사를 위해 잠금 해제 (Locked이면 CanDelete가 false)
                    CellContent[m_SwappingCells.Item1].Locked = false;
                    CellContent[m_SwappingCells.Item2].Locked = false;

                    // IncomingGem → ContainingGem 확정 (도착 완료)
                    CellContent[m_SwappingCells.Item1].ContainingGem = CellContent[m_SwappingCells.Item1].IncomingGem;
                    CellContent[m_SwappingCells.Item2].ContainingGem = CellContent[m_SwappingCells.Item2].IncomingGem;

                    // Gem 내부 좌표 갱신
                    CellContent[m_SwappingCells.Item1].ContainingGem.MoveTo(m_SwappingCells.Item1);
                    CellContent[m_SwappingCells.Item2].ContainingGem.MoveTo(m_SwappingCells.Item2);

                    // 양쪽 모두 매치 검사
                    bool firstCheck = false;
                    bool secondCheck = false;

                    // == 1. 첫 번째 칸 검사 ==
                    // 보너스 아이템(Usable)이면 일반적인 매치(DoCheck)를 검사하지 않고,
                    // 대신 Use() 함수를 호출해 특수 능력을 발동시킨 뒤 무조건 매치 성공(firstCheck = true)으로 처리합니다.
                    if (CellContent[m_SwappingCells.Item1].ContainingGem.Usable)
                    {
                        // 스왑한 상대방 보석의 정보를 Use()의 파라미터로 넘겨줍니다.
                        // (예: ColorClean 보너스의 경우, 스왑한 대상 보석의 색상을 기준으로 전체 삭제 능력 발동)
                        CellContent[m_SwappingCells.Item1].ContainingGem.Use(CellContent[m_SwappingCells.Item2].ContainingGem);
                        firstCheck = true;
                    }
                    else
                    {
                        // 일반 보석이면 3개 이상 연결되었는지 매치 검사를 수행합니다.
                        firstCheck = DoCheck(m_SwappingCells.Item1);
                    }

                    // == 2. 두 번째 칸 검사 ==
                    if (CellContent[m_SwappingCells.Item2].ContainingGem.Usable)
                    {
                        CellContent[m_SwappingCells.Item2].ContainingGem.Use(CellContent[m_SwappingCells.Item1].ContainingGem);
                        secondCheck = true;
                    }
                    else
                    {
                        secondCheck = DoCheck(m_SwappingCells.Item2);
                    }

                    if (firstCheck || secondCheck)
                    {
                        // ── 매치 성공! ──
                        // IncomingGem 참조를 정리하고 스왑 종료
                        CellContent[m_SwappingCells.Item1].IncomingGem = null;
                        CellContent[m_SwappingCells.Item2].IncomingGem = null;

                        m_SwapStage = SwapStage.None;
                    }
                    else
                    {
                        // ── 매치 실패 → 되돌리기 준비 ──
                        // IncomingGem을 교차하여 원래 자리로 돌아가게 함
                        // (A칸의 Incoming을 B보석으로, B칸의 Incoming을 A보석으로)
                        (CellContent[m_SwappingCells.Item1].IncomingGem,
                         CellContent[m_SwappingCells.Item2].IncomingGem) =
                            (CellContent[m_SwappingCells.Item2].IncomingGem,
                             CellContent[m_SwappingCells.Item1].IncomingGem);

                        // 좌표도 교체 (Item1이 되돌아갈 곳, Item2가 되돌아갈 곳)
                        (m_SwappingCells.Item1, m_SwappingCells.Item2) =
                            (m_SwappingCells.Item2, m_SwappingCells.Item1);

                        m_SwapStage = SwapStage.Return;

                        if (GameManager.Instance.Settings.SoundSettings.SwapBackSound != null)
                        {
                            GameManager.Instance.PlaySFX(GameManager.Instance.Settings.SoundSettings.SwapBackSound);
                        }

                        // Return 동안 다른 조작 방지를 위해 다시 잠금
                        CellContent[m_SwappingCells.Item1].Locked = true;
                        CellContent[m_SwappingCells.Item2].Locked = true;
                    }
                }
                // ── Return 단계 완료 ──
                else
                {
                    // 원래 위치로 복구 확정
                    CellContent[m_SwappingCells.Item1].ContainingGem = CellContent[m_SwappingCells.Item1].IncomingGem;
                    CellContent[m_SwappingCells.Item2].ContainingGem = CellContent[m_SwappingCells.Item2].IncomingGem;

                    CellContent[m_SwappingCells.Item1].ContainingGem.MoveTo(m_SwappingCells.Item1);
                    CellContent[m_SwappingCells.Item2].ContainingGem.MoveTo(m_SwappingCells.Item2);

                    // IncomingGem 참조 정리
                    CellContent[m_SwappingCells.Item1].IncomingGem = null;
                    CellContent[m_SwappingCells.Item2].IncomingGem = null;

                    // 잠금 해제
                    CellContent[m_SwappingCells.Item1].Locked = false;
                    CellContent[m_SwappingCells.Item2].Locked = false;

                    m_SwapStage = SwapStage.None;
                }
            }
        }

        // ================================================================
        //  DoMatchCheck — 착지한 보석들의 매치 검사
        // ================================================================
        //  m_CellToMatchCheck에 등록된 모든 칸에 대해 DoCheck()를 호출합니다.
        //  MoveGems()에서 바운스가 완료될 때 이 목록에 추가됩니다.
        // ================================================================
        void DoMatchCheck()
        {
            foreach (var cell in m_CellToMatchCheck)
            {
                DoCheck(cell);
            }
            m_CellToMatchCheck.Clear();
        }

        // ================================================================
        //  DoCheck — 특정 칸을 기준으로 매치 검사 (Flood Fill + 라인 검출)
        // ================================================================
        //
        //  알고리즘:
        //  1단계 (Flood Fill): 시작 칸과 같은 타입의 인접 보석을 모두 찾기
        //      → BFS(너비 우선 탐색)로 상하좌우 연결된 같은 보석을 gemList에 수집
        //
        //  2단계 (라인 검출): gemList 안에서 가로/세로 3개 이상 연속인 것만 추출
        //      → 각 보석에서 4방향 끝점을 찾아, 반대로 걸어가며 라인 길이 측정
        //      → 3개 이상이면 lineList에 합집합
        //
        //  3단계 (매치 생성): lineList가 비어있지 않으면 Match 객체 생성
        //      → CreateCustomMatch()로 매치 등록, AddGem()으로 보석 등록
        //      → 등록된 보석은 CurrentMatch가 설정되어 이후 이동/재매치 방지
        //
        //  반환값: 매치가 발견되었으면 true, 아니면 false
        //
        // ================================================================
        bool DoCheck(Vector3Int startCell, bool createMatch = true)
        {
            // 셀이 없거나 보석이 없으면 검사 불필요
            if (!CellContent.TryGetValue(startCell, out var centerGem) || centerGem.ContainingGem == null)
                return false;

            // 이미 다른 매치에 소속된 보석이면 중복 검사 방지
            if (centerGem.ContainingGem.CurrentMatch != null)
                return false;

            Vector3Int[] offsets = new[]
            {
                Vector3Int.up, Vector3Int.right, Vector3Int.down, Vector3Int.left
            };

            // ── 1단계: Flood Fill (BFS) ──
            // 시작 칸과 같은 GemType인 인접 보석을 모두 수집합니다.
            // gemList = 연결된 같은 색 보석 전체 (L자/T자 포함)
            List<Vector3Int> gemList = new List<Vector3Int>();
            List<Vector3Int> checkedCells = new();

            Queue<Vector3Int> toCheck = new();
            toCheck.Enqueue(startCell);

            while (toCheck.Count > 0)
            {
                var current = toCheck.Dequeue();

                gemList.Add(current);
                checkedCells.Add(current);

                foreach (var dir in offsets)
                {
                    var nextCell = current + dir;

                    if (checkedCells.Contains(nextCell))
                        continue;

                    if (CellContent.TryGetValue(nextCell, out var content)
                        && content.CanMatch()
                        && content.ContainingGem != null
                        && content.ContainingGem.CurrentMatch == null
                        && content.ContainingGem.GemType == centerGem.ContainingGem.GemType)
                    {
                        if (!toCheck.Contains(nextCell))
                            toCheck.Enqueue(nextCell);
                    }
                }
            }

            // ── 2단계: 보너스 Shape 체크 ──
            // gemList (전체 연결 젬)를 FitIn에 넘겨서 보너스 모양과 일치하는지 검사합니다.
            // ※ 중요: lineList가 아닌 gemList로 체크해야 L/T자 모양을 올바르게 매칭합니다.
            // 여러 보너스가 겹칠 경우 더 많은 젬을 포함하는(더 큰) 보너스를 우선합니다.
            List<Vector3Int> temporaryShapeMatch = new();
            MatchShape matchedShape = null;
            List<BonusGem> matchedBonusGem = new();

            var bonuses = GameManager.Instance.Settings.BonusSettings.Bonuses;
            if (bonuses != null)
            {
                foreach (var bonusGem in bonuses)
                {
                    if (bonusGem == null) continue;
                    foreach (var shape in bonusGem.Shapes)
                    {
                        if (shape.FitIn(gemList, ref temporaryShapeMatch))
                        {
                            // 더 큰 Shape을 우선 (더 많은 젬을 포함하는 보너스)
                            if (matchedShape == null || matchedShape.Cells.Count < shape.Cells.Count)
                            {
                                matchedShape = shape;
                                matchedBonusGem.Clear();
                                matchedBonusGem.Add(bonusGem);
                            }
                            else if (matchedShape.Cells.Count == shape.Cells.Count)
                            {
                                // 같은 크기면 후보에 추가 (랜덤 선택)
                                matchedBonusGem.Add(bonusGem);
                            }
                        }
                    }
                }
            }

            // ── 3단계: 3개 이상 직선 라인 검출 ──
            //
            // 각 방향에 대해: 해당 방향에 같은 보석이 없으면 → 라인의 끝점
            // 반대 방향으로 걸어가며 연속 보석 수집 → 3개 이상이면 lineList에 저장
            //
            // 원본 동작: 여러 라인 중 마지막으로 발견된 라인을 사용합니다.
            // (Union 대신 단순 대입 → 가장 최근 라인만 남음)
            List<Vector3Int> lineList = new();

            foreach (var idx in gemList)
            {
                foreach (var dir in offsets)
                {
                    if (!gemList.Contains(idx + dir))
                    {
                        var currentList = new List<Vector3Int>() { idx };
                        var next = idx - dir;
                        while (gemList.Contains(next))
                        {
                            currentList.Add(next);
                            next -= dir;
                        }

                        if (currentList.Count >= 3)
                        {
                            lineList = currentList;
                        }
                    }
                }
            }

            // ── 매치 성립 조건 ──
            // lineList(3개 이상 직선)도 없고 temporaryShapeMatch(보너스 Shape 매칭)도 없으면 매치 없음
            if (lineList.Count == 0 && temporaryShapeMatch.Count == 0)
                return false;

            // ── 4단계: Match 객체 생성 및 보석 등록 ──
            if (createMatch)
            {
                var finalMatch = CreateCustomMatch(startCell);

                // 보너스 젬 결정: ColorClean > LargeBomb > LineRocket > SmallBomb 우선순위로 선택
                finalMatch.SpawnedBonus = matchedBonusGem.Count == 0
                    ? null
                    : SelectBonusByPriority(matchedBonusGem);

                // 보너스 생성 위치 결정: 스왑한 젬이 매치에 포함되면 그 위치 우선
                if (finalMatch.SpawnedBonus != null)
                {
                    var allMatchCells = new List<Vector3Int>(lineList);
                    foreach (var c in temporaryShapeMatch)
                        if (!allMatchCells.Contains(c)) allMatchCells.Add(c);

                    if (m_SwapStage != SwapStage.None)
                    {
                        if (allMatchCells.Contains(m_SwappingCells.Item2))
                            finalMatch.OriginPoint = m_SwappingCells.Item2;
                        else if (allMatchCells.Contains(m_SwappingCells.Item1))
                            finalMatch.OriginPoint = m_SwappingCells.Item1;
                    }

                    if (finalMatch.OriginPoint == Vector3Int.zero && allMatchCells.Count > 0)
                        finalMatch.OriginPoint = allMatchCells[0];
                }

                // lineList의 보석 등록
                foreach (var cell in lineList)
                {
                    if (m_MatchedCallback.TryGetValue(cell, out var callback))
                        callback.Invoke();

                    if (CellContent[cell].CanDelete())
                        finalMatch.AddGem(CellContent[cell].ContainingGem);
                }

                // temporaryShapeMatch의 보석 등록 (lineList에 없는 것만 추가)
                // ※ lineList와 겹치는 셀은 이미 위에서 callback 호출했으므로 스킵
                foreach (var cell in temporaryShapeMatch)
                {
                    if (!lineList.Contains(cell))
                    {
                        if (m_MatchedCallback.TryGetValue(cell, out var callback))
                            callback.Invoke();

                        if (CellContent[cell].CanDelete())
                            finalMatch.AddGem(CellContent[cell].ContainingGem);
                    }
                }
            }

            return true;
        }

        // ================================================================
        //  MatchTicking — 매치된 보석의 삭제 타이머 처리
        // ================================================================
        //
        //  각 Match 객체에 대해:
        //  1. DeletionTimer를 매 프레임 증가 (0.3초 만에 1.0에 도달하도록)
        //  2. 바운스 중인 보석이 있으면 즉시 정지 (매치에 포함되었으므로)
        //  3. 타이머가 1.0을 넘으면 보석을 실제로 파괴(Destroy)
        //     → 해당 칸을 m_EmptyCells에 추가하여 새 보석이 채워지게 함
        //  4. 매치의 모든 보석이 삭제되면 매치 객체 자체를 제거
        //
        //  ※ 타이머 0 ~ 1.0 사이에 보석 축소/페이드 등의 시각 효과를
        //     추가할 수 있습니다 (현재는 즉시 삭제).
        //
        // ================================================================
        void MatchTicking()
        {
            bool playedPangSound = false;

            for (int i = 0; i < m_TickingMatch.Count; ++i)
            {
                var match = m_TickingMatch[i];

                // deletionSpeed = 1/0.3 ≈ 3.33 → 0.3초 후 DeletionTimer가 1.0 도달
                const float deletionSpeed = 1.0f / 0.3f;
                match.DeletionTimer += Time.deltaTime * deletionSpeed;

                // [SmallBomb 2x2] 삭제 직전 소스 데이터 캡처 & ExplodeCount x2 설정
                // SmallBomb 매치가 삭제되기 바로 직전(한 번만) 소스 4개 젬의 위치·스프라이트를 캡처합니다.
                // InitMergeEffect() 에서 SmallBomb 인스턴스로 전달되어 머지 애니메이션에 사용됩니다.
                List<(Vector3 pos, Sprite sprite)> squareSourceData = null;
                List<(string tag, Vector3 pos, Sprite sprite, AudioClip foodSfx, int count)> squarePendingExplosions = null;
                if (match.SpawnedBonus is SmallBomb
                    && (match.ForcedDeletion || match.DeletionTimer > 1.0f))
                {
                    // ForcedDeletion=true → MatchPangVFX·PangSound·코인 보상을 모두 생략합니다.
                    // 머지 연출 전용이므로 일반 매치 이펙트가 겹쳐 보여서는 안 됩니다.
                    match.ForcedDeletion = true;
                    squareSourceData = new();
                    squarePendingExplosions = new();
                    foreach (var idx in match.MatchingGem)
                    {
                        if (CellContent.TryGetValue(idx, out var c) && c.ContainingGem != null)
                        {
                            var g = c.ContainingGem;
                            var sr = g.GetComponentInChildren<SpriteRenderer>();
                            squareSourceData.Add((g.transform.position, sr?.sprite));
                            g.ExplodeCount = 2;
                            // Gem 파괴 전에 필요한 데이터를 미리 캡처합니다.
                            // OnGemExplodedDelayed로 머지 연출 완료 후 발화됩니다.
                            squarePendingExplosions.Add((
                                g.Data?.BlockTag,
                                g.transform.position,
                                sr?.sprite,
                                g.Data?.FoodSfx,
                                2));  // ExplodeCount = 2
                        }
                    }
                }

                // 이 매치에 속한 각 보석 처리
                for (int j = 0; j < match.MatchingGem.Count; j++)
                {
                    var gemIdx = match.MatchingGem[j];
                    var gem = CellContent[gemIdx].ContainingGem;

                    // 보석이 이미 다른 이유로 사라졌으면 목록에서 제거
                    if (gem == null)
                    {
                        match.MatchingGem.RemoveAt(j);
                        j--;
                        continue;
                    }

                    // ── 바운스 중인 보석 강제 정지 ──
                    //    매치에 포함된 보석이 아직 바운스 중이라면
                    //    (예: 매치 직전에 착지한 경우) 즉시 정지시킵니다.
                    if (gem.CurrentState == Gem.State.Bouncing)
                    {
                        // 움직임 목록에서 제거 (더 이상 MoveGems에서 처리하지 않음)
                        if (m_TickingCells.Contains(gemIdx)) m_TickingCells.Remove(gemIdx);
                        if (m_NewTickingCells.Contains(gemIdx)) m_NewTickingCells.Remove(gemIdx);

                        // 정확한 위치로 스냅하고 정지
                        gem.transform.position = m_Grid.GetCellCenterWorld(gemIdx);
                        gem.StopBouncing();  // State → Still
                    }

                    // ── 타이머 완료 → 실제 삭제 ──
                    if (match.ForcedDeletion || match.DeletionTimer > 1.0f)
                    {
                        // [VFX/SFX] Pang Effect 스폰 (Origin 스타일)
                        var visualSettings = GameManager.Instance.Settings.VisualSettings;
                        var soundSettings = GameManager.Instance.Settings.SoundSettings;

                        // 강제 삭제(보너스 젬 등)인 경우에는 기본 매치 이펙트를 생략합니다. (전용 이펙트가 따로 있으므로)
                        if (visualSettings.MatchPangVFX != null && !match.ForcedDeletion)
                        {
                            var pang = Instantiate(visualSettings.MatchPangVFX, gem.transform.position, Quaternion.identity);
                            Destroy(pang, 0.5f);
                        }

                        // [추가] 보석이 삭제될 때 낙하 지연 타이머 설정
                        m_PostMatchDelayTimer = visualSettings.PostMatchFallDelay;

                        if (soundSettings.MatchPangSound != null && !match.ForcedDeletion && !playedPangSound)
                        {
                            GameManager.Instance.PlaySFX(soundSettings.MatchPangSound);
                            playedPangSound = true;
                        }

                        // ── 얼음 블록 처리 ──────────────────────────────────────
                        // 얼음이 덮인 보석이 매칭되면:
                        //   - 얼음만 깨짐 (BreakAndVanish)
                        //   - 보석은 Destroy 되지 않고 보드에 그대로 남음
                        //   - 이후 일반 보석으로 스왑 가능
                        if (gem.IsFrozen)
                        {
                            gem.FrozenBy.BreakAndVanish();

                            // ★ 핵심: match 참조를 해제해야 CanBeMoved = true 로 복원됨
                            // gem.Destroyed()를 건너뛰므로 여기서 직접 해제해야 합니다.
                            // 해제하지 않으면 gem.CurrentMatch != null → CanBeMoved = false → 영구 이동 불가
                            gem.CurrentMatch = null;

                            // 보석이 남으므로 빈 칸으로 등록하지 않음
                            match.MatchingGem.RemoveAt(j);
                            j--;
                            continue;  // gem.Destroyed() 호출 생략
                        }

                        // ① 오브젝트 파괴 및 셀 참조 제거
                        Destroy(gem.gameObject);
                        CellContent[gemIdx].ContainingGem = null;

                        // ① 강제 삭제(폭탄 등)일 때 TieBlocker 같은 장애물도 함께 제거
                        if (match.ForcedDeletion && CellContent[gemIdx].Obstacle != null)
                        {
                            CellContent[gemIdx].Obstacle.Clear();
                        }

                        // ② 이 칸의 삭제 콜백 호출 (강제 삭제가 아닐 때만)
                        if (!match.ForcedDeletion && m_CellsCallbacks.TryGetValue(gemIdx, out var clbk))
                        {
                            clbk.Invoke();
                        }

                        // ③ 매치 목록에서 이 보석 제거
                        match.MatchingGem.RemoveAt(j);
                        j--;

                        match.DeletedCount += 1;

                        // ── 코인(점수) 획득 및 이펙트 스폰 조건 ──
                        // 코인은 플레이어가 보석을 4개 이상 연결했을 때 주어지는 추가 보상입니다.
                        //
                        // 1. match.DeletedCount >= 4
                        //    이 조건은 매치된 보석의 삭제를 '순차적'으로 세면서(For문), 4번째로 삭제되는
                        //    보석부터 코인을 발생시킵니다. 즉, 4개 매치 시 1번 발생(총 1점 추가), 
                        //    5개 매치 시 4번째, 5번째 총 2번 발생(총 2점 추가)하는 효과를 가져옵니다.
                        //
                        // 2. !match.ForcedDeletion
                        //    '강제 삭제(폭탄, 열십자 빔 등 특수 아이템의 연쇄 폭발 효과)'가 아닐 때만 발동합니다.
                        //    즉, 플레이어의 직접적인 스와이프나 이어지는 순수 캐스케이드(Cascade)로 
                        //    자연스럽게 매치가 일어난 경우에만 점수를 추가로 인정해 줍니다.
                        if (match.DeletedCount >= 4 && !match.ForcedDeletion)
                        {
                            GameManager.Instance.ChangeCoins(1);

                            if (GameManager.Instance.Settings != null && GameManager.Instance.Settings.VisualSettings.CoinVFX != null)
                            {
                                GameManager.Instance.PoolSystem.PlayInstanceAt(
                                    GameManager.Instance.Settings.VisualSettings.CoinVFX,
                                    gem.transform.position);
                            }
                        }

                        // ④ 보너스 젬 스폰: 이 칸이 보너스 생성 위치이면 새 젬으로 채움
                        //    그렇지 않으면 빈 칸으로 등록하여 위에서 보석이 떨어지게 함
                        if (match.SpawnedBonus != null && match.OriginPoint == gemIdx)
                        {
                            var spawnedGem = NewGemAt(match.OriginPoint, match.SpawnedBonus);
                            if (squareSourceData != null && spawnedGem is SmallBomb spawnedBomb)
                                spawnedBomb.InitMergeEffect(squareSourceData, squarePendingExplosions);
                        }
                        else
                        {
                            if (!m_EmptyCells.Contains(gemIdx))
                                m_EmptyCells.Add(gemIdx);
                        }

                        // ⑤ 보석 파괴 상태로 전환 (State → Disappearing)
                        //
                        // [타이쿤 추가] gem.Destroyed() 직전에 OnGemExploded 이벤트를 발사합니다.
                        //  - gem.Destroyed() 이후에는 transform이 이미 비활성화될 수 있으므로
                        //    반드시 먼저 발사해야 위치(position)가 정확합니다.
                        //  - HallManager.OnGemExploded()가 이 이벤트를 받아
                        //    재료가 필요한 테이블을 찾고 AcceptGem()을 호출합니다.
                        // SmallBomb 소스 젬은 폭발 이벤트를 여기서 발화하지 않습니다.
                        // (squarePendingExplosions에 캡처된 데이터가 머지 연출 완료 후 발화됩니다.)
                        // 일반 젬만 즉시 발화합니다.
                        if (squarePendingExplosions == null)
                        {
                            // ExplodeDelay > 0 (ColorClean 등): gem 파괴는 즉시,
                            // 재료 비행 이벤트는 이펙트가 끝난 후 발화합니다.
                            if (match.ExplodeDelay > 0f)
                            {
                                // gem 파괴 전에 필요한 데이터를 미리 캡처합니다.
                                string capturedTag = gem.Data?.BlockTag;
                                Vector3 capturedPos = gem.transform.position;
                                Sprite capturedSprite = gem.GetComponentInChildren<SpriteRenderer>()?.sprite;
                                AudioClip capturedSfx = gem.Data?.FoodSfx;
                                int capturedCount = gem.ExplodeCount;
                                float delay = match.ExplodeDelay;
                                StartCoroutine(FireExplodeAfterDelay(
                                    capturedTag, capturedPos, capturedSprite, capturedSfx,
                                    capturedCount, delay));
                            }
                            else
                            {
                                for (int ec = 0; ec < gem.ExplodeCount; ec++)
                                    OnGemExploded?.Invoke(gem, gem.transform.position);
                            }
                        }
                        gem.Destroyed();
                    }
                }

                // 매치의 모든 보석이 삭제 완료되면 매치 객체 자체를 제거
                if (match.MatchingGem.Count == 0)
                {
                    m_TickingMatch.RemoveAt(i);
                    i--;
                }
            }
        }

        // ================================================================
        //  CreateCustomMatch — 새 Match 객체를 생성하고 m_TickingMatch에 등록
        // ================================================================
        public Match CreateCustomMatch(Vector3Int newCell)
        {
            var newMatch = new Match()
            {
                DeletionTimer = 0.0f,
                MatchingGem = new(),
                OriginPoint = newCell  // 매치의 기준점 (보너스 보석 생성 위치 등에 활용)
            };

            m_TickingMatch.Add(newMatch);

            return newMatch;
        }

        // ================================================================
        //  CheckInput — 플레이어의 스와이프(드래그) 입력 처리
        // ================================================================
        //
        //  1. 마우스/터치 누름 → 시작 위치 기록
        //  2. 놓음 → 스와이프 방향 계산 → 유효성 검증 → m_SwipeQueued = true
        //
        //  스와이프가 거부되는 경우:
        //  - 이미 스왑이 진행 중이거나 예약되어 있을 때
        //  - 드래그 거리가 너무 짧을 때 (0.5 미만)
        //  - 시작 칸이나 끝 칸의 보석이 이동 불가일 때 (Locked, 매치 중, 낙하 중 등)
        //
        // ================================================================
        public void ToggleInput(bool enable)
        {
            m_InputEnabled = enable;
        }

        void CheckInput()
        {
            if (!m_InputEnabled)
                return;

            var mainCam = Camera.main;

            // New Input System에서 입력 상태 읽기
            var pressedThisFrame = GameManager.Instance.ClickAction.WasPressedThisFrame();
            var releasedThisFrame = GameManager.Instance.ClickAction.WasReleasedThisFrame();

            var clickPos = GameManager.Instance.ClickPosition.ReadValue<Vector2>();
            var worldPos = mainCam.ScreenToWorldPoint(clickPos);
            worldPos.z = 0;

            // ── 누른 순간: 시작 위치 기록 ──
            if (pressedThisFrame)
            {
                // 보너스 아이템이 활성화되어 있다면, 클릭한 위치의 젬에 해당 보너스를 발동
                if (m_ActivatedBonusItem != null)
                {
                    var clickedCell = m_Grid.WorldToCell(mainCam.ScreenToWorldPoint(clickPos));
                    if (CellContent.TryGetValue(clickedCell, out var content) && content.ContainingGem != null)
                    {
                        GameManager.Instance.UseBonusItem(m_ActivatedBonusItem, clickedCell);
                        m_ActivatedBonusItem = null;
                        return;
                    }
                }

                m_StartClickPosition = clickPos;
            }
            // ── 놓은 순간: 스와이프 판정 ──
            else if (releasedThisFrame)
            {
                // 이미 스왑 진행 중이거나 예약되어 있으면 무시
                if (m_SwipeQueued || m_SwapStage != SwapStage.None)
                    return;

                // 더블 클릭 감지 로직
                float clickDelta = Time.time - m_LastClickTime;
                m_LastClickTime = Time.time;

                // 시작 위치를 월드 좌표로 변환 → 그리드 셀 좌표로 변환
                var worldStart = mainCam.ScreenToWorldPoint(m_StartClickPosition);
                var startCell = m_Grid.WorldToCell(worldStart);
                startCell.z = 0;


                // 0.3초 안에 다시 클릭했고 + 보너스 젬이라면 발동!
                if (clickDelta < 0.3f)
                {
                    if (CellContent.TryGetValue(startCell, out var content)
                        && content.ContainingGem != null
                        && content.ContainingGem.Usable
                        && content.ContainingGem.CurrentMatch == null)
                    {
                        content.ContainingGem.Use(null);
                        return; // 스와이프 로직 실행 안 하고 종료
                    }
                }

                var endWorldPos = mainCam.ScreenToWorldPoint(clickPos);

                // 스와이프 벡터 계산 (시작 → 끝)
                var swipe = endWorldPos - worldStart;
                // 드래그 거리가 너무 짧으면 무시 (실수 터치 방지)
                if (swipe.sqrMagnitude < 0.5f * 0.5f)
                    return;

                // 시작 칸의 보석이 이동 가능한지 확인
                // CanBeMoved = !Locked && ContainingGem != null && CanMove && CurrentMatch == null
                if (!CellContent.TryGetValue(startCell, out var startCellContent) || !startCellContent.CanBeMoved)
                    return;

                // 스와이프 방향 결정 (가로/세로 중 더 큰 쪽)
                var endCell = startCell;

                if (Mathf.Abs(swipe.x) > Mathf.Abs(swipe.y))
                {
                    if (swipe.x < 0) endCell += Vector3Int.left;
                    else endCell += Vector3Int.right;
                }
                else
                {
                    if (swipe.y < 0) endCell += Vector3Int.down;
                    else endCell += Vector3Int.up;
                }

                // 끝 칸의 보석도 이동 가능한지 확인
                if (CellContent.TryGetValue(endCell, out var endCellContent) && endCellContent.CanBeMoved)
                {
                    // 두 칸을 잠금 → EmptyCheck/MoveGems에서 건드리지 못하게 방지
                    startCellContent.Locked = true;
                    endCellContent.Locked = true;

                    // 스와이프 예약 → Update()의 SwipeQueue 블록에서 처리됨
                    m_StartSwipe = startCell;
                    m_EndSwipe = endCell;
                    m_SwipeQueued = true;
                    OnGemDragged?.Invoke(startCellContent.ContainingGem, endCell - startCell);
                }
            }
        }


        // ================================================================
        //  GetReference — 컴포넌트 참조 초기화
        // ================================================================
        private void GetReference()
        {
            m_Grid = GetComponent<Grid>();
        }


        // ================================================================
        //  SetupBorder — 보드 크기에 맞춰 Border / Shine 자동 배치
        // ================================================================
        //  GenerateBoard() → CalculateBounds() 완료 후 호출됩니다.
        //
        //  ■ Border  : 9-slice SpriteRenderer. 보드 크기 + padding 으로 size 설정.
        //  ■ Shine_Left/Right : Simple SpriteRenderer. 보드 좌/우 엣지에 위치,
        //                       Y scale 로 보드 높이에 맞춤 (Origin 방식 동일).
        // ================================================================
        // ── 런타임 호출 (Start) ──────────────────────────────────────────
        private void SetupBorder()
        {
            // 게임 시작 시에는 캐시를 무효화하여 항상 레이아웃을 새로 적용
            m_LastBorderSize = Vector3Int.zero;
            m_LastBorderMin = Vector2Int.zero;
            m_LastBorderPadding = float.MinValue;

            ApplyBorderLayout(m_Grid, m_BoundsInt.xMin, m_BoundsInt.yMin,
                                      m_BoundsInt.xMax, m_BoundsInt.yMax);
        }

        // ── 공용 레이아웃 계산 (런타임 + 에디터) ──────────────────────────
        private void ApplyBorderLayout(Grid grid, int xMin, int yMin, int xMax, int yMax)
        {
            if (grid == null) return;

            // dirty-check: 보드 범위와 패딩이 모두 같으면 건너뜀
            var newSize = new Vector3Int(xMax - xMin + 1, yMax - yMin + 1, 0);
            var newMin = new Vector2Int(xMin, yMin);
            if (newSize == m_LastBorderSize && newMin == m_LastBorderMin &&
                Mathf.Approximately(m_BorderPadding, m_LastBorderPadding))
                return;

            m_LastBorderSize = newSize;
            m_LastBorderMin = newMin;
            m_LastBorderPadding = m_BorderPadding;

            // CellToWorld: 셀의 좌-하단 모서리 월드 좌표 반환
            // → (xMin,yMin) 좌-하단 ~ (xMax+1, yMax+1) 우-상단 으로 실제 보드 영역 결정
            // cellSize/cellGap/Grid 스케일에 무관하게 항상 정확합니다.
            Vector3 cornerMin = grid.CellToWorld(new Vector3Int(xMin, yMin, 0));
            Vector3 cornerMax = grid.CellToWorld(new Vector3Int(xMax + 1, yMax + 1, 0));
            Vector3 center = (cornerMin + cornerMax) * 0.5f;
            float boardW = cornerMax.x - cornerMin.x;
            float boardH = cornerMax.y - cornerMin.y;
            float totalH = boardH + m_BorderPadding * 2f;

            // ── Border (9-slice) ─────────────────────────────────────
            if (m_BorderSpriteRenderer != null)
            {
                // SpriteRenderer.size 는 로컬 공간 값이므로, 부모 Grid의 lossyScale 만큼 나눠야
                // 월드 공간에서 boardW+padding 크기로 정확히 렌더링됩니다.
                var ls = m_BorderSpriteRenderer.transform.lossyScale;
                m_BorderSpriteRenderer.drawMode = SpriteDrawMode.Sliced;
                m_BorderSpriteRenderer.size = new Vector2(
                    (boardW + m_BorderPadding * 2f) / Mathf.Abs(ls.x),
                    totalH / Mathf.Abs(ls.y));
                var p = m_BorderSpriteRenderer.transform.position;
                m_BorderSpriteRenderer.transform.position = new Vector3(center.x, center.y, p.z);
            }

            // ── Shine (Simple, Y-scale to board height) ──────────────
            ApplyShine(m_ShineLeft, center.x - boardW * 0.5f, center.y, totalH);
            ApplyShine(m_ShineRight, center.x + boardW * 0.5f, center.y, totalH);
        }

        private void ApplyShine(SpriteRenderer shine, float x, float centerY, float targetH)
        {
            if (shine == null || shine.sprite == null) return;
            float spriteH = shine.sprite.bounds.size.y;
            if (spriteH <= 0f) return;
            var p = shine.transform.position;
            var s = shine.transform.localScale;
            shine.transform.position = new Vector3(x, centerY, p.z);
            // localScale.y * parentLossyScale.y * spriteH = targetH 가 되도록 역산
            float parentScaleY = shine.transform.parent != null
                ? Mathf.Abs(shine.transform.parent.lossyScale.y) : 1f;
            shine.transform.localScale = new Vector3(s.x, targetH / (spriteH * parentScaleY), s.z);
        }

#if UNITY_EDITOR
        // ── 에디터 전용: Inspector 값 변경 시 즉시 반영 ─────────────────
        private void OnValidate()
        {
            // delayCall: OnValidate는 직렬화 도중 호출될 수 있으므로 지연 실행
            UnityEditor.EditorApplication.delayCall += RefreshBorderInEditor;
        }

        // ── 에디터 전용: 타일 페인팅 중 실시간 반영 ──────────────────────
        // (Update에서 !isPlaying 분기로 호출됨)
        private void RefreshBorderInEditor()
        {
            if (this == null) return;

            var grid = GetComponent<Grid>();
            if (grid == null) return;


            // dirty-check 캐시를 무효화해 항상 레이아웃이 적용되도록 함
            m_LastBorderSize = Vector3Int.zero;
            m_LastBorderMin = Vector2Int.zero;
            m_LastBorderPadding = float.MinValue;

            int xMin = int.MaxValue, xMax = int.MinValue;
            int yMin = int.MaxValue, yMax = int.MinValue;

            var all = m_BoardTilemap.cellBounds;
            for (int x = all.xMin; x < all.xMax; x++)
            {
                for (int y = all.yMin; y < all.yMax; y++)
                {
                    if (m_BoardTilemap.GetTile(new Vector3Int(x, y, 0)) is GemPlacerTile)
                    {
                        if (x < xMin) xMin = x;
                        if (x > xMax) xMax = x;
                        if (y < yMin) yMin = y;
                        if (y > yMax) yMax = y;
                    }
                }
            }

            if (xMin == int.MaxValue) return;   // GemPlacerTile 없음

            ApplyBorderLayout(grid, xMin, yMin, xMax, yMax);
        }
#endif


        public Gem NewGemAt(Vector3Int cell, Gem gemPrefab, int overrideType = -1)
        {
            if (gemPrefab == null && overrideType == -1 && m_AvailableGemTypes.Count > 0)
            {
                overrideType = m_AvailableGemTypes[Random.Range(0, m_AvailableGemTypes.Count)];
                gemPrefab = BaseGemPrefab;
            }
            else if (gemPrefab == null && overrideType != -1)
            {
                gemPrefab = BaseGemPrefab;
            }

            if (gemPrefab == null) return null;

            // Grid를 부모로 지정하면 Grid의 transform.scale이 Gem에 자동 상속됩니다.
            var gem = Instantiate(gemPrefab, m_Grid.GetCellCenterWorld(cell), Quaternion.identity, m_Grid.transform);

            // 데이터 기반 아키텍처 지원: 입력된 타입 또는 프리팹 데이터 기반 세팅
            int finalType = overrideType != -1 ? overrideType : gemPrefab.GemType;
            if (m_GemDataLookup != null && m_GemDataLookup.TryGetValue(finalType, out var data))
            {
                gem.Setup(data);
            }

            CellContent[cell].ContainingGem = gem;
            gem.Init(cell);

            return gem;
        }

        // ================================================================
        //  GenerateBoard — 게임 시작 시 보드 전체를 보석으로 채우기
        // ================================================================
        //
        //  1. CalculateBounds()로 보드 크기 계산
        //  2. 각 칸에 랜덤 보석 배치 (초기 매치 방지 로직 포함)
        //  3. 스포너 위치 등록 (보드 맨 윗줄 + 1칸 위)
        //
        //  ■ 초기 매치 방지:
        //     왼쪽 2칸이 같은 타입이면 그 타입을 후보에서 제외
        //     아래 2칸이 같은 타입이면 그 타입을 후보에서 제외
        //     → 게임 시작 시 3개 이상 매치가 이미 있는 상황 방지
        //
        // ================================================================
        public void GenerateBoard()
        {
            CalculateBounds();

            // 아래쪽부터 위로, 왼쪽부터 오른쪽으로 채우기
            for (int y = m_BoundsInt.yMin; y <= m_BoundsInt.yMax; ++y)
            {
                for (int x = m_BoundsInt.xMin; x <= m_BoundsInt.xMax; ++x)
                {
                    var idx = new Vector3Int(x, y, 0);

                    // 셀이 등록되지 않았거나 이미 보석이 있으면 스킵
                    // (미리 배치된 젬은 여기서 건너뜀)
                    if (!CellContent.TryGetValue(idx, out var current) || current.ContainingGem != null)
                        continue;

                    var availableGems = new List<int>(m_AvailableGemTypes);

                    // ── [강화된 3매치 방지 로직] ──
                    // 미리 배치된 젬(Pre-placed Gem)이 있을 수 있으므로, 
                    // 단순 왼쪽/아래쪽 뿐만 아니라, 오른쪽/위쪽 및 "중간 끼임"까지 검사해야 합니다.
                    // (예: ■ □ ■ 상황에서 가운데를 채우면 매치됨)

                    for (int i = availableGems.Count - 1; i >= 0; i--)
                    {
                        int type = availableGems[i];

                        // 1. 가로 방향 매치 가능성 검사
                        // - (좌측2, 좌측1)이 같은 색
                        // - (우측1, 우측2)가 같은 색
                        // - (좌측1, 우측1)이 같은 색 (샌드위치)
                        bool hMatch = (HasGemType(idx + Vector3Int.left, type) && HasGemType(idx + Vector3Int.left * 2, type))
                                   || (HasGemType(idx + Vector3Int.right, type) && HasGemType(idx + Vector3Int.right * 2, type))
                                   || (HasGemType(idx + Vector3Int.left, type) && HasGemType(idx + Vector3Int.right, type));

                        // 2. 세로 방향 매치 가능성 검사
                        // - (하단2, 하단1)이 같은 색
                        // - (상단1, 상단2)가 같은 색
                        // - (하단1, 상단1)이 같은 색 (샌드위치)
                        bool vMatch = (HasGemType(idx + Vector3Int.down, type) && HasGemType(idx + Vector3Int.down * 2, type))
                                   || (HasGemType(idx + Vector3Int.up, type) && HasGemType(idx + Vector3Int.up * 2, type))
                                   || (HasGemType(idx + Vector3Int.down, type) && HasGemType(idx + Vector3Int.up, type));

                        // 3. 2×2 정사각형 매치 가능성 검사
                        // 현재 셀(idx)을 포함한 2×2 블록이 모두 같은 타입인지 확인합니다.
                        // 현재 셀이 2×2 블록의 4가지 코너 위치 각각에 있을 수 있으므로
                        // 4가지 경우(우하, 좌하, 우상, 좌상)를 모두 검사합니다.
                        bool sqMatch =
                            (HasGemType(idx + Vector3Int.right, type) && HasGemType(idx + Vector3Int.down, type) && HasGemType(idx + Vector3Int.right + Vector3Int.down, type))  // idx=좌상
                         || (HasGemType(idx + Vector3Int.left, type) && HasGemType(idx + Vector3Int.down, type) && HasGemType(idx + Vector3Int.left + Vector3Int.down, type))  // idx=우상
                         || (HasGemType(idx + Vector3Int.right, type) && HasGemType(idx + Vector3Int.up, type) && HasGemType(idx + Vector3Int.right + Vector3Int.up, type))  // idx=좌하
                         || (HasGemType(idx + Vector3Int.left, type) && HasGemType(idx + Vector3Int.up, type) && HasGemType(idx + Vector3Int.left + Vector3Int.up, type)); // idx=우하

                        // 위 경우 중 하나라도 해당되면 이 젬 타입은 후보에서 제외
                        if (hMatch || vMatch || sqMatch)
                        {
                            availableGems.RemoveAt(i);
                        }
                    }

                    // 남은 후보 중 랜덤 선택
                    if (availableGems.Count > 0)
                    {
                        int chosenType = availableGems[Random.Range(0, availableGems.Count)];
                        NewGemAt(idx, null, chosenType);
                    }
                    else
                    {
                        // 매우 드물게 모든 색이 불가능한 경우 (사방이 막힘)
                        // 어쩔 수 없이 아무거나 선택 (매치 발생 허용)
                        if (m_AvailableGemTypes.Count > 0)
                        {
                            int fallbackType = m_AvailableGemTypes[0];
                            NewGemAt(idx, null, fallbackType);
                        }
                    }
                }
            }
        }

        // ================================================================
        //  EvalBonusPriority — 스왑 시뮬레이션 중 보너스 생성 우선순위 평가
        // ================================================================
        //  DoCheck 와 동일한 BFS + Shape 매칭 로직을 사용하되,
        //  Match 객체는 만들지 않고 우선순위(int)만 반환합니다.
        //  4=ColorClean, 3=LargeBomb, 2=LineRocket, 1=SmallBomb, 0=일반3매치
        // ================================================================
        private int EvalBonusPriority(Vector3Int startCell)
        {
            if (!CellContent.TryGetValue(startCell, out var center) || center.ContainingGem == null)
                return 0;

            // BFS: 연결된 같은 색 보석 수집
            var dirs = new[] { Vector3Int.up, Vector3Int.right, Vector3Int.down, Vector3Int.left };
            var gemList = new List<Vector3Int>();
            var visited = new List<Vector3Int>();
            var queue = new Queue<Vector3Int>();
            queue.Enqueue(startCell);

            while (queue.Count > 0)
            {
                var cur = queue.Dequeue();
                if (visited.Contains(cur)) continue;
                visited.Add(cur);
                gemList.Add(cur);

                foreach (var d in dirs)
                {
                    var next = cur + d;
                    if (visited.Contains(next)) continue;
                    if (CellContent.TryGetValue(next, out var c)
                        && c.CanMatch()
                        && c.ContainingGem != null
                        && c.ContainingGem.CurrentMatch == null
                        && c.ContainingGem.GemType == center.ContainingGem.GemType
                        && !queue.Contains(next))
                    {
                        queue.Enqueue(next);
                    }
                }
            }

            // Shape 매칭 → 최고 보너스 후보 결정
            var bonuses = GameManager.Instance.Settings.BonusSettings.Bonuses;
            if (bonuses == null) return 0;

            var bonusCandidates = new List<BonusGem>();
            MatchShape bestShape = null;
            var shapeMatch = new List<Vector3Int>();

            foreach (var bonus in bonuses)
            {
                if (bonus == null) continue;
                foreach (var shape in bonus.Shapes)
                {
                    if (!shape.FitIn(gemList, ref shapeMatch)) continue;
                    if (bestShape == null || bestShape.Cells.Count < shape.Cells.Count)
                    {
                        bestShape = shape;
                        bonusCandidates.Clear();
                        bonusCandidates.Add(bonus);
                    }
                    else if (bestShape.Cells.Count == shape.Cells.Count)
                    {
                        bonusCandidates.Add(bonus);
                    }
                }
            }

            if (bonusCandidates.Count == 0) return 0;

            var best = SelectBonusByPriority(bonusCandidates);
            if (best is ColorClean) return 4;
            if (best is LargeBomb) return 3;
            if (best is LineRocket) return 2;
            if (best is SmallBomb) return 1;
            return 1;
        }

        // 보너스 젬 우선순위 선택: ColorClean > LargeBomb > LineRocket > SmallBomb
        private BonusGem SelectBonusByPriority(List<BonusGem> candidates)
        {
            foreach (var b in candidates) if (b is ColorClean) return b;
            foreach (var b in candidates) if (b is LargeBomb) return b;
            foreach (var b in candidates) if (b is LineRocket) return b;
            foreach (var b in candidates) if (b is SmallBomb) return b;
            return candidates[0];
        }

        // 해당 좌표에 특정 타입의 젬이 존재하는지 확인하는 헬퍼 함수
        private bool HasGemType(Vector3Int pos, int type)
        {
            if (CellContent.TryGetValue(pos, out var cell) && cell.ContainingGem != null)
            {
                return cell.ContainingGem.GemType == type;
            }
            return false;
        }

        // ================================================================
        //  RegisterCell — 타일이 자신을 보드에 등록할 때 호출 (에디터 시점)
        // ================================================================
        //  Unity 타일맵의 각 타일이 StartUp()에서 이 메서드를 호출하여
        //  자기 좌표를 CellContent에 등록합니다.
        //  → 이를 통해 보드가 어떤 칸들로 구성되어 있는지 알 수 있습니다.
        // ================================================================
        public static void RegisterCell(Vector3Int cellPosition, GemData startingGemData = null)
        {
            // Not super happy with that, but Startup is called before all Awake....
            if (s_Instance == null)
            {
                s_Instance = GameObject.Find("Grid").GetComponent<Board>();
                s_Instance.GetReference();
            }

            if (!s_Instance.CellContent.ContainsKey(cellPosition))
            {
                s_Instance.CellContent.Add(cellPosition, new BoardCell());
            }

            if (startingGemData != null)
            {
                // 데이터 룩업에 등록 (NewGemAt에서 Setup할 때 필요)
                if (s_Instance.m_GemDataLookup == null) s_Instance.m_GemDataLookup = new();
                if (!s_Instance.m_GemDataLookup.ContainsKey(startingGemData.GemType))
                {
                    s_Instance.m_GemDataLookup.Add(startingGemData.GemType, startingGemData);
                }

                s_Instance.NewGemAt(cellPosition, null, startingGemData.GemType);
            }
        }


        public static void RegisterSpawner(Vector3Int position)
        {
            if (s_Instance == null)
            {
                s_Instance = GameObject.Find("Grid").GetComponent<Board>();
                s_Instance.GetReference();
            }
            // 중복 등록 방지
            if (!s_Instance.SpawnerPosition.Contains(position))
            {
                s_Instance.SpawnerPosition.Add(position);
            }
        }

        /// <summary>
        /// Crate 등 직접 프리팹이 필요한 젬을 보드 셀에 인스턴스화합니다.
        /// GemPlacerTile.GemPrefabOverride가 설정된 경우 StartUp()에서 호출됩니다.
        /// </summary>
        public static void RegisterGemPrefab(Vector3Int cellPosition, Gem gemPrefab)
        {
            if (s_Instance == null)
            {
                s_Instance = GameObject.Find("Grid").GetComponent<Board>();
                s_Instance.GetReference();
            }

            if (!s_Instance.CellContent.ContainsKey(cellPosition))
                s_Instance.CellContent.Add(cellPosition, new BoardCell());

            var gem = Instantiate(gemPrefab,
                s_Instance.m_Grid.GetCellCenterWorld(cellPosition),
                Quaternion.identity,
                s_Instance.m_Grid.transform);
            gem.Init(cellPosition);
            s_Instance.CellContent[cellPosition].ContainingGem = gem;
        }



        // ================================================================
        //  CalculateBounds — 보드의 좌표 범위 계산
        // ================================================================
        //  등록된 모든 셀의 x, y 좌표를 순회하여
        //  m_BoundsInt의 xMin/xMax/yMin/yMax를 결정합니다.
        // ================================================================
        public void CalculateBounds()
        {
            m_BoundsInt = new BoundsInt();
            var listOfCells = CellContent.Keys.ToList();

            if (listOfCells.Count == 0) return;

            m_BoundsInt.xMin = listOfCells[0].x;
            m_BoundsInt.xMax = m_BoundsInt.xMin;
            m_BoundsInt.yMin = listOfCells[0].y;
            m_BoundsInt.yMax = m_BoundsInt.yMin;

            foreach (var content in listOfCells)
            {
                if (content.x > m_BoundsInt.xMax) m_BoundsInt.xMax = content.x;
                else if (content.x < m_BoundsInt.xMin) m_BoundsInt.xMin = content.x;

                if (content.y > m_BoundsInt.yMax) m_BoundsInt.yMax = content.y;
                else if (content.y < m_BoundsInt.yMin) m_BoundsInt.yMin = content.y;
            }
        }

        // ================================================================
        //  ActivateSpawnerAt — 지정된 칸의 위에서 새 보석을 생성하여 낙하 시작
        // ================================================================
        //  EmptyCheck()나 MoveGems()에서 호출됩니다.
        //  보석은 cellIdx + up (스포너 위치)에서 생성되어 cellIdx로 떨어집니다.
        // ================================================================
        void ActivateSpawnerAt(Vector3Int cellIdx)
        {
            if (m_AvailableGemTypes.Count == 0) return;

            // ── 매치 방지: 인접 보석과 3-match가 생기지 않는 타입 선택 ──
            var availableGems = new List<int>(m_AvailableGemTypes);
            for (int i = availableGems.Count - 1; i >= 0; i--)
            {
                int type = availableGems[i];

                bool hMatch = (HasGemType(cellIdx + Vector3Int.left, type) && HasGemType(cellIdx + Vector3Int.left * 2, type))
                           || (HasGemType(cellIdx + Vector3Int.right, type) && HasGemType(cellIdx + Vector3Int.right * 2, type))
                           || (HasGemType(cellIdx + Vector3Int.left, type) && HasGemType(cellIdx + Vector3Int.right, type));

                bool vMatch = (HasGemType(cellIdx + Vector3Int.down, type) && HasGemType(cellIdx + Vector3Int.down * 2, type))
                           || (HasGemType(cellIdx + Vector3Int.up, type) && HasGemType(cellIdx + Vector3Int.up * 2, type))
                           || (HasGemType(cellIdx + Vector3Int.down, type) && HasGemType(cellIdx + Vector3Int.up, type));

                if (hMatch || vMatch)
                    availableGems.RemoveAt(i);
            }

            // ── 손님 주문 기반 우선순위 산출 ──
            // 현재 보드에 나타날 수 있는 타입 중, 손님이 주문 중인 태그와 일치하는 타입을 가려냄
            var priorityTypes = new List<int>();
            var tables = FindObjectsByType<Table>(FindObjectsSortMode.None);
            foreach (var table in tables)
            {
                if (table != null && table.State == Table.TableState.Ordered)
                {
                    foreach (var gemType in availableGems)
                    {
                        if (m_GemDataLookup.TryGetValue(gemType, out var gd))
                        {
                            if (table.GetRemainingNeed(gd.BlockTag) > 0)
                                priorityTypes.Add(gemType);
                        }
                    }
                }
            }

            int chosenType;
            // 우선순위 보석이 있고 70% 확률(원하는 비중)에 당첨되면 우선순위 그룹에서 선택
            if (priorityTypes.Count > 0 && Random.value < 0.7f)
            {
                chosenType = priorityTypes[Random.Range(0, priorityTypes.Count)];
            }
            else
            {
                chosenType = availableGems.Count > 0
                    ? availableGems[Random.Range(0, availableGems.Count)]
                    : m_AvailableGemTypes[Random.Range(0, m_AvailableGemTypes.Count)];
            }

            var prefab = BaseGemPrefab;

            // 1칸 위(스포너 위치)의 월드 좌표에서 생성
            Vector3 spawnPos = m_Grid.GetCellCenterWorld(cellIdx + Vector3Int.up);
            var newObj = Instantiate(prefab, spawnPos, Quaternion.identity, transform);

            Gem newGem = newObj.GetComponent<Gem>();

            // 데이터 기반 아키텍처 지원: 선택된 타입에 맞는 데이터 주입
            if (m_GemDataLookup != null && m_GemDataLookup.TryGetValue(chosenType, out var data))
            {
                newGem.Setup(data);
            }

            newGem.Init(cellIdx);  // 목표 칸 좌표로 초기화

            // 이 칸의 IncomingGem으로 설정 → MoveGems()에서 낙하 처리됨
            CellContent[cellIdx].IncomingGem = newGem;

            newGem.StartMoveTimer();       // State → Falling
            newGem.SpeedMultiplier = 1.0f;
            m_NewTickingCells.Add(cellIdx); // 다음 프레임에 MoveGems()에서 처리

            // 이 칸은 더 이상 비어있지 않으므로 빈 칸 목록에서 제거
            if (m_EmptyCells.Contains(cellIdx)) m_EmptyCells.Remove(cellIdx);

            // ── 얼음 블록 스폰 ──
            // LevelData.IceSpawnChance 확률로 새 보석 위에 얼음 블록을 올립니다.
            // 이미 해당 셀에 Obstacle이 있으면 중복 생성하지 않습니다.
            TrySpawnIceBlock(cellIdx);
        }

        /// <summary>
        /// LevelData.IceSpawnChance 확률로 방금 스폰된 보석에 얼음을 씌웁니다.
        /// 얼음은 보석의 자식 GameObject 로 부착되어 낙하 시 함께 이동합니다.
        /// </summary>
        private void TrySpawnIceBlock(Vector3Int cellIdx)
        {
            // 프리팹은 Board 인스펙터에서 연결합니다 (BaseGemPrefab과 같은 방식).
            if (IceBlockPrefab == null) return;

            // 확률은 LevelData에서 읽습니다 (스테이지마다 다르게 설정 가능).
            var levelData = LevelData.Instance;
            if (levelData == null || levelData.IceSpawnChance <= 0f) return;

            // 확률 롤 (0~1 사이 균등 분포)
            if (Random.value > levelData.IceSpawnChance) return;

            // 방금 스폰된 보석 참조 (IncomingGem = 낙하 시작 직후)
            if (!CellContent.TryGetValue(cellIdx, out var cell)) return;
            var gem = cell.IncomingGem ?? cell.ContainingGem;
            if (gem == null || gem.IsFrozen) return;  // 이미 얼음이 있으면 중복 스킵

            // IceBlock 생성 후 보석에 부착 (Gem 자식 → 낙하 시 함께 이동)
            var iceGo = Instantiate(IceBlockPrefab);
            var ice = iceGo.GetComponent<IceBlock>();

            if (ice == null)
            {
                Debug.LogWarning("[Board] IceBlockPrefab에 IceBlock 컴포넌트가 없습니다. 프리팹에 MyMatch3.IceBlock 스크립트를 추가하세요.");
                Destroy(iceGo);
                return;
            }

            // Attach: 보석의 자식이 되며 gem.SetFrozen() 호출까지 처리합니다.
            ice.Attach(gem);
        }

        public static void AddObstacle(Vector3Int cell, Obstacle obstacle)
        {
            if (s_Instance == null) return; // 안전장치

            // 해당 셀이 아직 등록되지 않았다면 등록 (젬 생성 대기)
            RegisterCell(cell);

            // 해당 셀에 Obstacle 참조 연결
            if (s_Instance.CellContent.ContainsKey(cell))
            {
                s_Instance.CellContent[cell].Obstacle = obstacle;
            }

            // 월드 좌표로 위치 이동
            obstacle.transform.position = s_Instance.m_Grid.GetCellCenterWorld(cell);
        }


        // 특정 셀의 잠금 상태(Locked) 변경 (장애물이 있으면 이동 불가 처리)
        public static void ChangeLock(Vector3Int cell, bool locked)
        {
            if (s_Instance != null && s_Instance.CellContent.ContainsKey(cell))
            {
                s_Instance.CellContent[cell].Locked = locked;
            }
        }

        // 삭제 콜백 등록
        public static void RegisterDeletedCallback(Vector3Int cell, System.Action callback)
        {
            if (s_Instance == null) return;
            if (!s_Instance.m_CellsCallbacks.ContainsKey(cell))
            {
                s_Instance.m_CellsCallbacks[cell] = callback;
            }
            else
            {
                s_Instance.m_CellsCallbacks[cell] += callback;
            }
        }

        // 삭제 콜백 해제
        public static void UnregisterDeletedCallback(Vector3Int cell, System.Action callback)
        {
            if (s_Instance == null || !s_Instance.m_CellsCallbacks.ContainsKey(cell))
                return;
            s_Instance.m_CellsCallbacks[cell] -= callback;
            if (s_Instance.m_CellsCallbacks[cell] == null)
                s_Instance.m_CellsCallbacks.Remove(cell);
        }

        // 매치 발생 콜백 등록
        public static void RegisterMatchedCallback(Vector3Int cell, System.Action callback)
        {
            if (s_Instance == null) return;
            if (!s_Instance.m_MatchedCallback.ContainsKey(cell))
            {
                s_Instance.m_MatchedCallback[cell] = callback;
            }
            else
            {
                s_Instance.m_MatchedCallback[cell] += callback;
            }
        }

        // 매치 발생 콜백 해제
        public static void UnregisterMatchedCallback(Vector3Int cell, System.Action callback)
        {
            if (s_Instance == null || !s_Instance.m_MatchedCallback.ContainsKey(cell))
                return;
            s_Instance.m_MatchedCallback[cell] -= callback;
            if (s_Instance.m_MatchedCallback[cell] == null)
                s_Instance.m_MatchedCallback.Remove(cell);
        }



        public void AddNewBoardAction(IBoardAction action)
        {
            m_BoardActions.Add(action);
        }
        public interface IBoardAction
        {
            bool Tick();
        }




        // ================================================================
        //  OnDrawGizmos — 에디터에서 보드 상태를 시각적으로 표시 (디버그용)
        // ================================================================
        //  Scene 뷰에서:
        //  - 각 칸의 좌표, ContainingGem[C]/IncomingGem[I] 여부 표시
        //  - CanFall(F) / CanBeMoved(M) 상태 표시
        //  - 빨간 반투명 = 진짜 빈 칸 (ContainingGem과 IncomingGem 모두 null)
        //  - 노란 와이어프레임 = m_EmptyCells에 등록된 칸
        // ================================================================
        // #if UNITY_EDITOR
        //         private void OnDrawGizmos()
        //         {
        //             if (CellContent == null || m_Grid == null) return;

        //             GUIStyle labelStyle = new GUIStyle();
        //             labelStyle.alignment = TextAnchor.MiddleCenter;
        //             labelStyle.fontStyle = FontStyle.Bold;
        //             labelStyle.normal.textColor = Color.white;

        //             GUIStyle shadowStyle = new GUIStyle(labelStyle);
        //             shadowStyle.normal.textColor = Color.black;

        //             foreach (var kvp in CellContent)
        //             {
        //                 var cell = kvp.Value;
        //                 Vector3 worldPos = m_Grid.GetCellCenterWorld(kvp.Key);
        //                 string cellInfo = $"{kvp.Key.x}, {kvp.Key.y}";

        //                 if (cell.ContainingGem != null) cellInfo += "\n[C]";
        //                 if (cell.IncomingGem != null) cellInfo += "\n[I]";

        //                 string status = $"{(cell.CanFall ? "F" : "-")}/{(cell.CanBeMoved ? "M" : "-")}";
        //                 cellInfo += $"\n{status}";

        //                 float offset = 0.02f;
        //                 Vector3 labelPos = worldPos + Vector3.back * 0.1f;
        //                 UnityEditor.Handles.Label(labelPos + new Vector3(offset, offset, 0), cellInfo, shadowStyle);
        //                 UnityEditor.Handles.Label(labelPos + new Vector3(-offset, offset, 0), cellInfo, shadowStyle);
        //                 UnityEditor.Handles.Label(labelPos + new Vector3(offset, -offset, 0), cellInfo, shadowStyle);
        //                 UnityEditor.Handles.Label(labelPos + new Vector3(-offset, -offset, 0), cellInfo, shadowStyle);

        //                 UnityEditor.Handles.Label(labelPos, cellInfo, labelStyle);

        //                 if (cell.IsEmpty())
        //                 {
        //                     Gizmos.color = new Color(1, 0, 0, 0.3f);
        //                     Gizmos.DrawCube(worldPos, Vector3.one * 0.8f);
        //                 }

        //                 if (m_EmptyCells.Contains(kvp.Key))
        //                 {
        //                     Gizmos.color = Color.yellow;
        //                     Gizmos.DrawWireCube(worldPos, Vector3.one * 0.9f);
        //                 }
        //             }
        //         }
        // #endif



        /// <summary>
        /// UI에서 보너스 아이템을 선택했을 때 호출됩니다.
        /// 보드 클릭 시 해당 아이템이 발동됩니다.
        /// </summary>
        public void ActivateBonusItem(Booster item)
        {
            m_ActivatedBonusItem = item;
        }

        // ================================================================
        //  ShuffleGems — RefreshBooster에서 호출
        // ================================================================
        /// <summary>
        /// 보드 위의 일반 보석(BonusGem 제외) 타입을 무작위로 재배치합니다.
        /// 보석 오브젝트는 그 자리에 유지되고 Setup()으로 타입/스프라이트만 교체합니다.
        /// </summary>
        public void ShuffleGems()
        {
            StartCoroutine(ShuffleRoutine());
        }

        private IEnumerator ShuffleRoutine()
        {
            // 1. 이동 가능한 일반 보석 위치 수집
            var positions = new List<Vector3Int>();
            foreach (var kvp in CellContent)
            {
                var gem = kvp.Value.ContainingGem;
                if (gem == null || gem is BonusGem || gem is Crate) continue;
                if (gem.CurrentMatch != null) continue;  // 매치 중인 보석 제외

                // 장애물(TieBlocker 등)이 있는 칸이거나, 얼려있는(IceBlock) 보석은 제외 (그대로 둠)
                if (kvp.Value.Obstacle != null) continue;
                if (gem.IsFrozen) continue;

                positions.Add(kvp.Key);
            }

            if (positions.Count < 2) yield break;

            LockMovement();

            // 1.5 애니메이션: 모든 보석 스케일 0으로 축소 (0.2s)
            float t = 0f;
            float duration = 0.2f;
            while (t < duration)
            {
                t += Time.deltaTime;
                float f = Mathf.SmoothStep(1f, 0f, Mathf.Clamp01(t / duration));
                foreach (var pos in positions)
                {
                    var gem = CellContent[pos].ContainingGem;
                    if (gem != null)
                        gem.transform.localScale = Vector3.one * f;
                }
                yield return null;
            }

            // 2. 스케일 축소 완료 후 보석을 삭제하고 빈 칸으로 설정
            foreach (var pos in positions)
            {
                var gem = CellContent[pos].ContainingGem;
                if (gem != null)
                {
                    // 상태 파괴 중으로 변경 후 오브젝트 파괴
                    gem.Destroyed();
                    Destroy(gem.gameObject);

                    // 칸 비우기: 이후 MoveGems()에서 자동으로 빈 칸을 채움 (점수/폭발 콤보 미발생)
                    CellContent[pos].ContainingGem = null;
                    if (!m_EmptyCells.Contains(pos))
                        m_EmptyCells.Add(pos);
                }
            }

            UnlockMovement();
        }
    }

}
