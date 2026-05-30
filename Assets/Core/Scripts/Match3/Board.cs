using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using Random = UnityEngine.Random;

namespace DoggyChef
{
    [DefaultExecutionOrder(-9999)]
    public partial class Board : MonoBehaviour
    {
        private static Board s_Instance;

        [Header("Grid References")]
        [SerializeField] private Grid m_Grid;
        [SerializeField] private UnityEngine.Tilemaps.Tilemap m_BoardTilemap;

        [Header("Board Setup")]
        public Gem BaseGemPrefab;
        public GameObject IceBlockPrefab;
        public List<Vector3Int> SpawnerPosition = new();
        [SerializeField, Min(0f)] private float m_BorderPadding = 0.3f;

        [Header("Board Runtime Data")]
        public Dictionary<Vector3Int, BoardCell> CellContent = new();
        private BoundsInt m_BoundsInt;
        private Dictionary<int, GemData> m_GemDataLookup;
        private List<int> m_AvailableGemTypes = new();

        [Header("Board Visuals")]
        [SerializeField] private SpriteRenderer m_BorderSpriteRenderer;
        [SerializeField] private SpriteRenderer m_ShineLeft;
        [SerializeField] private SpriteRenderer m_ShineRight;

        private Vector3Int m_LastBorderSize = Vector3Int.zero;
        private Vector2Int m_LastBorderMin = Vector2Int.zero;
        private float m_LastBorderPadding = float.MinValue;

        [Header("Runtime Queues")]
        private List<Vector3Int> m_TickingCells = new();
        private List<Vector3Int> m_NewTickingCells = new();
        private List<Vector3Int> m_EmptyCells = new();
        private List<Vector3Int> m_CellToMatchCheck = new();
        private List<Match> m_TickingMatch = new();
        private List<IBoardAction> m_BoardActions = new();
        private float m_PostMatchDelayTimer = 0.0f;

        [Header("State")]
        private SwapStage m_SwapStage = SwapStage.None;
        private (Vector3Int, Vector3Int) m_SwappingCells;
        private int m_FreezeMoveLock = 0;
        private bool m_FinalStretch = false;

        private enum SwapStage { None, Forward, Return }

        [Header("Input")]
        private bool m_InputEnabled = true;
        private bool m_SwipeQueued;
        private Vector3Int m_StartSwipe;
        private Vector3Int m_EndSwipe;
        private float m_LastClickTime = 0.0f;
        public Vector3 m_StartClickPosition;

        [Header("Hint")]
        private float m_SinceLastHint = 0.0f;
        private bool m_BoardChanged = true;
        private GameObject m_HintIndicator;
        private List<PossibleSwap> m_PossibleSwaps = new();
        private int m_PickedSwap = 0;

        public class PossibleSwap
        {
            public Vector3Int StartPosition;
            public Vector3Int Direction;
            public int BonusPriority;
        }

        [Header("Bonus and Callbacks")]
        private Booster m_ActivatedBonusItem = null;
        private Dictionary<Vector3Int, System.Action> m_MatchedCallback = new();
        private Dictionary<Vector3Int, System.Action> m_CellsCallbacks = new();

        public static event Action<Gem, Vector3> OnGemExploded;
        public static event Action<Gem, Vector3Int> OnGemDragged;

        public static event Action<string, Vector3, Sprite, AudioClip> OnGemExplodedDelayed;

        public static void FireGemExplodedDelayed(string tag, Vector3 pos, Sprite sprite, AudioClip foodSfx)
            => OnGemExplodedDelayed?.Invoke(tag, pos, sprite, foodSfx);
        private IEnumerator FireExplodeAfterDelay(
            string tag, Vector3 pos, Sprite sprite, AudioClip foodSfx, int count, float delay)
        {
            yield return new WaitForSeconds(delay);
            for (int ec = 0; ec < count; ec++)
                OnGemExplodedDelayed?.Invoke(tag, pos, sprite, foodSfx);
        }

        public Grid Grid => m_Grid;
        public BoundsInt Bounds => m_BoundsInt;
        //  Awake / Start
        void Awake()
        {
            s_Instance = this;
            GetReference();
        }

#if UNITY_EDITOR
        void OnEnable()
        {
            if (Application.isPlaying) return;
            UnityEditor.EditorApplication.delayCall += RefreshBorderInEditor;
        }
#endif

        void Start()
        {
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

            m_HintIndicator = Instantiate(GameManager.Instance.Settings.VisualSettings.HintPrefab);
            m_HintIndicator.SetActive(false);
        }
        //
        void Update()
        {
            for (int i = 0; i < m_BoardActions.Count; ++i)
            {
                if (!m_BoardActions[i].Tick())
                {
                    m_BoardActions.RemoveAt(i);
                    i--;
                }
            }

            if (m_PostMatchDelayTimer > 0)
            {
                m_PostMatchDelayTimer -= Time.deltaTime;
            }

            CheckInput();

            if (m_SwapStage != SwapStage.None)
            {
                TickSwap();
            }

            bool incrementHintTimer = m_ActivatedBonusItem == null;

            if (m_TickingCells.Count > 0)
            {
                MoveGems();

                incrementHintTimer = false;
                m_BoardChanged = true;
            }

            if (m_CellToMatchCheck.Count > 0)
            {
                DoMatchCheck();
                incrementHintTimer = false;
                m_BoardChanged = true;
            }

            if (m_TickingMatch.Count > 0)
            {
                MatchTicking();
                incrementHintTimer = false;
                m_BoardChanged = true;
            }

            if (m_EmptyCells.Count > 0 && m_PostMatchDelayTimer <= 0)
            {
                EmptyCheck();
                incrementHintTimer = false;
                m_BoardChanged = true;
            }

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

            if (m_NewTickingCells.Count > 0)
            {
                m_TickingCells.AddRange(m_NewTickingCells);
                m_NewTickingCells.Clear();
                incrementHintTimer = false;
            }

            if (incrementHintTimer)
            {

                if (m_FinalStretch)
                {
                    m_FinalStretch = false;
                    return;
                }

                if (m_BoardChanged)
                {
                    FindAllPossibleMatch();
                    m_BoardChanged = false;
                }

                if (m_PossibleSwaps.Count == 0)
                {
                    m_BoardChanged = true;
                    m_SinceLastHint = 0.0f;
                    if (m_HintIndicator != null)
                        m_HintIndicator.SetActive(false);
                    return;
                }

                var match = m_PossibleSwaps[m_PickedSwap];

                if (m_HintIndicator.activeSelf)
                {
                    var startPos = m_Grid.GetCellCenterWorld(match.StartPosition);
                    var endPos = m_Grid.GetCellCenterWorld(match.StartPosition + match.Direction);

                    var current = m_HintIndicator.transform.position;
                    current = Vector3.MoveTowards(current, endPos, 1.0f * Time.deltaTime);

                    m_HintIndicator.transform.position = current == endPos ? startPos : current;
                }
                else
                {
                    m_SinceLastHint += Time.deltaTime;

                    if (m_SinceLastHint >= GameManager.Instance.Settings.InactivityBeforeHint && m_InputEnabled)
                    {
                        m_HintIndicator.transform.position = m_Grid.GetCellCenterWorld(match.StartPosition);
                        m_HintIndicator.SetActive(true);
                    }
                }
            }
            else
            {
                m_HintIndicator.SetActive(false);
                m_SinceLastHint = 0.0f;
            }



        }
        //
        //
        void FindAllPossibleMatch()
        {
            m_PossibleSwaps.Clear();

            for (int y = m_BoundsInt.yMin; y <= m_BoundsInt.yMax; ++y)
            {
                for (int x = m_BoundsInt.xMin; x <= m_BoundsInt.xMax; ++x)
                {
                    var idx = new Vector3Int(x, y, 0);

                    if (CellContent.TryGetValue(idx, out var cell) && cell.CanBeMoved)
                    {
                        var topIdx = idx + Vector3Int.up;
                        var rightIdx = idx + Vector3Int.right;

                        if (CellContent.TryGetValue(topIdx, out var topCell) && topCell.CanBeMoved)
                        {
                            (CellContent[idx].ContainingGem, CellContent[topIdx].ContainingGem) =
                                (CellContent[topIdx].ContainingGem, CellContent[idx].ContainingGem);

                            if (DoCheck(topIdx, false))
                                m_PossibleSwaps.Add(new PossibleSwap() { StartPosition = idx, Direction = Vector3Int.up, BonusPriority = EvalBonusPriority(topIdx) });
                            if (DoCheck(idx, false))
                                m_PossibleSwaps.Add(new PossibleSwap() { StartPosition = topIdx, Direction = Vector3Int.down, BonusPriority = EvalBonusPriority(idx) });

                            (CellContent[idx].ContainingGem, CellContent[topIdx].ContainingGem) =
                                (CellContent[topIdx].ContainingGem, CellContent[idx].ContainingGem);
                        }

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

            m_PickedSwap = PickSwapWithOrderPriority();
        }
        //          ColorClean(4) > LargeBomb(3) > LineRocket(2) > SmallBomb(1) > default match(0)
        private int PickSwapWithOrderPriority()
        {
            if (m_PossibleSwaps.Count == 0) return 0;

            var tables = FindObjectsByType<Table>();
            var orderedTables = new List<Table>();
            foreach (var t in tables)
                if (t != null && t.State == Table.TableState.Ordered) orderedTables.Add(t);
            orderedTables.Sort((a, b) => a.CustomerCurrentHealth.CompareTo(b.CustomerCurrentHealth));

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

                if (candidates.Count > 0) break;
            }

            if (candidates.Count == 0)
                for (int i = 0; i < m_PossibleSwaps.Count; i++) candidates.Add(i);

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






        public void LockMovement()
        {
            m_FreezeMoveLock += 1;
        }

        public void UnlockMovement()
        {
            m_FreezeMoveLock -= 1;
            if (m_FreezeMoveLock < 0)
                m_FreezeMoveLock = 0;
        }

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

        public void DestroyGem(Vector3Int cell, bool isBonusEffect = false)
        {
            if (CellContent.TryGetValue(cell, out var content) && content.ContainingGem != null)
            {
                if (content.ContainingGem.CurrentMatch != null) return;

                var match = CreateCustomMatch(cell);
                match.ForcedDeletion = true;
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
        //
        //
        void MoveGems()
        {
            m_TickingCells.Sort((a, b) =>
            {
                int yCmp = a.y.CompareTo(b.y);
                return yCmp != 0 ? yCmp : a.x.CompareTo(b.x);
            });

            for (int i = 0; i < m_TickingCells.Count; ++i)
            {
                var cellIdx = m_TickingCells[i];
                var cell = CellContent[cellIdx];

                if (cell.IncomingGem?.CurrentState == Gem.State.Falling)
                {
                    var gem = cell.IncomingGem;
                    gem.TickMoveTimer(Time.deltaTime);

                    Vector3 targetPos = m_Grid.GetCellCenterWorld(cellIdx);
                    var maxDistance = GameManager.Instance.Settings.VisualSettings.FallAccelerationCurve.Evaluate(gem.FallTime) *
                                      Time.deltaTime * GameManager.Instance.Settings.VisualSettings.FallSpeed * gem.SpeedMultiplier;

                    gem.transform.position = Vector3.MoveTowards(gem.transform.position, targetPos,
                        maxDistance);


                    if (gem.transform.position == targetPos)
                    {
                        m_TickingCells.RemoveAt(i);
                        i--;

                        cell.IncomingGem = null;
                        cell.ContainingGem = gem;
                        gem.MoveTo(cellIdx);

                        if (m_EmptyCells.Contains(cellIdx + Vector3Int.down)
                            && CellContent.TryGetValue(cellIdx + Vector3Int.down, out var belowCell))
                        {
                            cell.ContainingGem = null;
                            belowCell.IncomingGem = gem;

                            gem.SpeedMultiplier = 1.0f;

                            var target = cellIdx + Vector3Int.down;
                            m_NewTickingCells.Add(target);

                            m_EmptyCells.Remove(target);
                            m_EmptyCells.Add(cellIdx);

                            if (SpawnerPosition.Contains(cellIdx + Vector3Int.up))
                            {
                                ActivateSpawnerAt(cellIdx);
                            }
                        }
                        else if ((!CellContent.TryGetValue(cellIdx + Vector3Int.left, out var leftCell) ||
                                  leftCell.BlockFall) &&
                                 m_EmptyCells.Contains(cellIdx + Vector3Int.down + Vector3Int.left) &&
                                 CellContent.TryGetValue(cellIdx + Vector3Int.down + Vector3Int.left, out var belowLeftCell))
                        {
                            cell.ContainingGem = null;
                            belowLeftCell.IncomingGem = gem;

                            gem.SpeedMultiplier = 1.41421356237f;

                            var target = cellIdx + Vector3Int.down + Vector3Int.left;
                            m_NewTickingCells.Add(target);

                            m_EmptyCells.Remove(target);
                            m_EmptyCells.Add(cellIdx);

                            if (SpawnerPosition.Contains(cellIdx + Vector3Int.up))
                            {
                                ActivateSpawnerAt(cellIdx);
                            }
                        }
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
                        else
                        {
                            gem.StopFalling();
                            m_NewTickingCells.Add(cellIdx);

                            var sfx = GameManager.Instance.PlaySFX(GameManager.Instance.Settings.SoundSettings.FallSound);
                            if (sfx != null) sfx.pitch = Random.Range(0.8f, 1.2f);
                        }
                    }
                }
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
                        gem.transform.position =
                            center + Vector3.up * (GameManager.Instance.Settings.VisualSettings.BounceCurve.Evaluate(gem.FallTime) * 0.3f);
                        gem.transform.localScale =
                            new Vector3(1, GameManager.Instance.Settings.VisualSettings.SquishCurve.Evaluate(gem.FallTime), 1);
                    }
                }
                else if (cell.ContainingGem?.CurrentState == Gem.State.Still)
                {
                    m_TickingCells.RemoveAt(i);
                    i--;
                }
            }
        }
        //
        //
        //
        void EmptyCheck()
        {
            if (m_FreezeMoveLock > 0)
                return;

            for (int i = 0; i < m_EmptyCells.Count; ++i)
            {
                var emptyCell = m_EmptyCells[i];

                if (!CellContent[emptyCell].IsEmpty())
                {
                    m_EmptyCells.RemoveAt(i);
                    i--;
                    continue;
                }

                var aboveCellIdx = emptyCell + Vector3Int.up;
                bool aboveCellExist = CellContent.TryGetValue(aboveCellIdx, out var aboveCell);

                if (aboveCellExist && aboveCell.ContainingGem != null && aboveCell.CanFall)
                {
                    var fallingGem = aboveCell.ContainingGem;

                    CellContent[emptyCell].IncomingGem = fallingGem;
                    aboveCell.ContainingGem = null;

                    fallingGem.StartMoveTimer();

                    m_NewTickingCells.Add(emptyCell);

                    m_EmptyCells.Add(aboveCellIdx);
                    m_EmptyCells.RemoveAt(i);
                    i--;
                }
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
                else if (SpawnerPosition.Contains(aboveCellIdx))
                {
                    ActivateSpawnerAt(emptyCell);
                    i--;
                }
            }
        }
        //
        //
        //
        //
        void TickSwap()
        {
            var gemToStart = CellContent[m_SwappingCells.Item1].IncomingGem;
            var gemToEnd = CellContent[m_SwappingCells.Item2].IncomingGem;

            var startPosition = m_Grid.GetCellCenterWorld(m_SwappingCells.Item1);
            var endPosition = m_Grid.GetCellCenterWorld(m_SwappingCells.Item2);

            gemToStart.transform.position = Vector3.MoveTowards(
                gemToStart.transform.position, startPosition, Time.deltaTime * 5.0f);
            gemToEnd.transform.position = Vector3.MoveTowards(
                gemToEnd.transform.position, endPosition, Time.deltaTime * 5.0f);

            if (gemToStart.transform.position == startPosition)
            {
                if (m_SwapStage == SwapStage.Forward)
                {
                    CellContent[m_SwappingCells.Item1].Locked = false;
                    CellContent[m_SwappingCells.Item2].Locked = false;

                    CellContent[m_SwappingCells.Item1].ContainingGem = CellContent[m_SwappingCells.Item1].IncomingGem;
                    CellContent[m_SwappingCells.Item2].ContainingGem = CellContent[m_SwappingCells.Item2].IncomingGem;

                    CellContent[m_SwappingCells.Item1].ContainingGem.MoveTo(m_SwappingCells.Item1);
                    CellContent[m_SwappingCells.Item2].ContainingGem.MoveTo(m_SwappingCells.Item2);

                    bool firstCheck = false;
                    bool secondCheck = false;

                    if (CellContent[m_SwappingCells.Item1].ContainingGem.Usable)
                    {
                        CellContent[m_SwappingCells.Item1].ContainingGem.Use(CellContent[m_SwappingCells.Item2].ContainingGem);
                        firstCheck = true;
                    }
                    else
                    {
                        firstCheck = DoCheck(m_SwappingCells.Item1);
                    }

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
                        CellContent[m_SwappingCells.Item1].IncomingGem = null;
                        CellContent[m_SwappingCells.Item2].IncomingGem = null;

                        m_SwapStage = SwapStage.None;
                    }
                    else
                    {
                        (CellContent[m_SwappingCells.Item1].IncomingGem,
                         CellContent[m_SwappingCells.Item2].IncomingGem) =
                            (CellContent[m_SwappingCells.Item2].IncomingGem,
                             CellContent[m_SwappingCells.Item1].IncomingGem);

                        (m_SwappingCells.Item1, m_SwappingCells.Item2) =
                            (m_SwappingCells.Item2, m_SwappingCells.Item1);

                        m_SwapStage = SwapStage.Return;

                        if (GameManager.Instance.Settings.SoundSettings.SwapBackSound != null)
                        {
                            GameManager.Instance.PlaySFX(GameManager.Instance.Settings.SoundSettings.SwapBackSound);
                        }

                        CellContent[m_SwappingCells.Item1].Locked = true;
                        CellContent[m_SwappingCells.Item2].Locked = true;
                    }
                }
                else
                {
                    CellContent[m_SwappingCells.Item1].ContainingGem = CellContent[m_SwappingCells.Item1].IncomingGem;
                    CellContent[m_SwappingCells.Item2].ContainingGem = CellContent[m_SwappingCells.Item2].IncomingGem;

                    CellContent[m_SwappingCells.Item1].ContainingGem.MoveTo(m_SwappingCells.Item1);
                    CellContent[m_SwappingCells.Item2].ContainingGem.MoveTo(m_SwappingCells.Item2);

                    CellContent[m_SwappingCells.Item1].IncomingGem = null;
                    CellContent[m_SwappingCells.Item2].IncomingGem = null;

                    CellContent[m_SwappingCells.Item1].Locked = false;
                    CellContent[m_SwappingCells.Item2].Locked = false;

                    m_SwapStage = SwapStage.None;
                }
            }
        }
        void DoMatchCheck()
        {
            foreach (var cell in m_CellToMatchCheck)
            {
                DoCheck(cell);
            }
            m_CellToMatchCheck.Clear();
        }
        //
        //
        //
        //
        //
        bool DoCheck(Vector3Int startCell, bool createMatch = true)
        {
            if (!CellContent.TryGetValue(startCell, out var centerGem) || centerGem.ContainingGem == null)
                return false;

            if (centerGem.ContainingGem.CurrentMatch != null)
                return false;

            Vector3Int[] offsets = new[]
            {
                Vector3Int.up, Vector3Int.right, Vector3Int.down, Vector3Int.left
            };

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
                            if (matchedShape == null || matchedShape.Cells.Count < shape.Cells.Count)
                            {
                                matchedShape = shape;
                                matchedBonusGem.Clear();
                                matchedBonusGem.Add(bonusGem);
                            }
                            else if (matchedShape.Cells.Count == shape.Cells.Count)
                            {
                                matchedBonusGem.Add(bonusGem);
                            }
                        }
                    }
                }
            }

            //
            //
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

            if (lineList.Count == 0 && temporaryShapeMatch.Count == 0)
                return false;

            if (createMatch)
            {
                var finalMatch = CreateCustomMatch(startCell);

                finalMatch.SpawnedBonus = matchedBonusGem.Count == 0
                    ? null
                    : SelectBonusByPriority(matchedBonusGem);

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

                foreach (var cell in lineList)
                {
                    if (m_MatchedCallback.TryGetValue(cell, out var callback))
                        callback.Invoke();

                    if (CellContent[cell].CanDelete())
                        finalMatch.AddGem(CellContent[cell].ContainingGem);
                }

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
        //
        //
        //
        void MatchTicking()
        {
            bool playedPangSound = false;

            for (int i = 0; i < m_TickingMatch.Count; ++i)
            {
                var match = m_TickingMatch[i];

                const float deletionSpeed = 1.0f / 0.3f;
                match.DeletionTimer += Time.deltaTime * deletionSpeed;

                List<(Vector3 pos, Sprite sprite)> squareSourceData = null;
                List<(string tag, Vector3 pos, Sprite sprite, AudioClip foodSfx, int count)> squarePendingExplosions = null;
                if (match.SpawnedBonus is SmallBomb
                    && (match.ForcedDeletion || match.DeletionTimer > 1.0f))
                {
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
                            squarePendingExplosions.Add((
                                g.Data?.BlockTag,
                                g.transform.position,
                                sr?.sprite,
                                g.Data?.FoodSfx,
                                2));  // ExplodeCount = 2
                        }
                    }
                }

                for (int j = 0; j < match.MatchingGem.Count; j++)
                {
                    var gemIdx = match.MatchingGem[j];
                    var gem = CellContent[gemIdx].ContainingGem;

                    if (gem == null)
                    {
                        match.MatchingGem.RemoveAt(j);
                        j--;
                        continue;
                    }

                    if (gem.CurrentState == Gem.State.Bouncing)
                    {
                        if (m_TickingCells.Contains(gemIdx)) m_TickingCells.Remove(gemIdx);
                        if (m_NewTickingCells.Contains(gemIdx)) m_NewTickingCells.Remove(gemIdx);

                        gem.transform.position = m_Grid.GetCellCenterWorld(gemIdx);
                        gem.StopBouncing();
                    }

                    if (match.ForcedDeletion || match.DeletionTimer > 1.0f)
                    {
                        var visualSettings = GameManager.Instance.Settings.VisualSettings;
                        var soundSettings = GameManager.Instance.Settings.SoundSettings;

                        if (visualSettings.MatchPangVFX != null && !match.ForcedDeletion)
                        {
                            var pang = Instantiate(visualSettings.MatchPangVFX, gem.transform.position, Quaternion.identity);
                            Destroy(pang, 0.5f);
                        }

                        m_PostMatchDelayTimer = visualSettings.PostMatchFallDelay;

                        if (soundSettings.MatchPangSound != null && !match.ForcedDeletion && !playedPangSound)
                        {
                            GameManager.Instance.PlaySFX(soundSettings.MatchPangSound);
                            playedPangSound = true;
                        }

                        if (gem.IsFrozen)
                        {
                            gem.FrozenBy.BreakAndVanish();

                            gem.CurrentMatch = null;

                            match.MatchingGem.RemoveAt(j);
                            j--;
                            continue;
                        }

                        Destroy(gem.gameObject);
                        CellContent[gemIdx].ContainingGem = null;

                        if (match.ForcedDeletion && CellContent[gemIdx].Obstacle != null)
                        {
                            CellContent[gemIdx].Obstacle.Clear();
                        }

                        if (!match.ForcedDeletion && m_CellsCallbacks.TryGetValue(gemIdx, out var clbk))
                        {
                            clbk.Invoke();
                        }

                        match.MatchingGem.RemoveAt(j);
                        j--;

                        match.DeletedCount += 1;

                        //
                        // 1. match.DeletedCount >= 4
                        //
                        // 2. !match.ForcedDeletion
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

                        //
                        if (squarePendingExplosions == null)
                        {
                            if (match.ExplodeDelay > 0f)
                            {
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

                if (match.MatchingGem.Count == 0)
                {
                    m_TickingMatch.RemoveAt(i);
                    i--;
                }
            }
        }
        public Match CreateCustomMatch(Vector3Int newCell)
        {
            var newMatch = new Match()
            {
                DeletionTimer = 0.0f,
                MatchingGem = new(),
                OriginPoint = newCell
            };

            m_TickingMatch.Add(newMatch);

            return newMatch;
        }
        //
        //
        //
        public void ToggleInput(bool enable)
        {
            m_InputEnabled = enable;
        }

        void CheckInput()
        {
            if (!m_InputEnabled)
                return;

            var mainCam = Camera.main;

            var pressedThisFrame = GameManager.Instance.ClickAction.WasPressedThisFrame();
            var releasedThisFrame = GameManager.Instance.ClickAction.WasReleasedThisFrame();

            var clickPos = GameManager.Instance.ClickPosition.ReadValue<Vector2>();
            var worldPos = mainCam.ScreenToWorldPoint(clickPos);
            worldPos.z = 0;

            if (pressedThisFrame)
            {
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
            else if (releasedThisFrame)
            {
                if (m_SwipeQueued || m_SwapStage != SwapStage.None)
                    return;

                float clickDelta = Time.time - m_LastClickTime;
                m_LastClickTime = Time.time;

                var worldStart = mainCam.ScreenToWorldPoint(m_StartClickPosition);
                var startCell = m_Grid.WorldToCell(worldStart);
                startCell.z = 0;


                if (clickDelta < 0.3f)
                {
                    if (CellContent.TryGetValue(startCell, out var content)
                        && content.ContainingGem != null
                        && content.ContainingGem.Usable
                        && content.ContainingGem.CurrentMatch == null)
                    {
                        content.ContainingGem.Use(null);
                        return;
                    }
                }

                var endWorldPos = mainCam.ScreenToWorldPoint(clickPos);

                var swipe = endWorldPos - worldStart;
                if (swipe.sqrMagnitude < 0.5f * 0.5f)
                    return;

                // CanBeMoved = !Locked && ContainingGem != null && CanMove && CurrentMatch == null
                if (!CellContent.TryGetValue(startCell, out var startCellContent) || !startCellContent.CanBeMoved)
                    return;

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

                if (CellContent.TryGetValue(endCell, out var endCellContent) && endCellContent.CanBeMoved)
                {
                    startCellContent.Locked = true;
                    endCellContent.Locked = true;

                    m_StartSwipe = startCell;
                    m_EndSwipe = endCell;
                    m_SwipeQueued = true;
                    OnGemDragged?.Invoke(startCellContent.ContainingGem, endCell - startCell);
                }
            }
        }
        private void GetReference()
        {
            m_Grid = GetComponent<Grid>();
        }

        private static Board EnsureInstance()
        {
            if (s_Instance != null)
                return s_Instance;

            s_Instance = FindAnyObjectByType<Board>();
            if (s_Instance == null)
            {
                Debug.LogError("[Board] No Board instance found in the scene.");
                return null;
            }

            s_Instance.GetReference();
            return s_Instance;
        }
        //
        private void SetupBorder()
        {
            m_LastBorderSize = Vector3Int.zero;
            m_LastBorderMin = Vector2Int.zero;
            m_LastBorderPadding = float.MinValue;

            ApplyBorderLayout(m_Grid, m_BoundsInt.xMin, m_BoundsInt.yMin,
                                      m_BoundsInt.xMax, m_BoundsInt.yMax);
        }

        private void ApplyBorderLayout(Grid grid, int xMin, int yMin, int xMax, int yMax)
        {
            if (grid == null) return;

            var newSize = new Vector3Int(xMax - xMin + 1, yMax - yMin + 1, 0);
            var newMin = new Vector2Int(xMin, yMin);
            if (newSize == m_LastBorderSize && newMin == m_LastBorderMin &&
                Mathf.Approximately(m_BorderPadding, m_LastBorderPadding))
                return;

            m_LastBorderSize = newSize;
            m_LastBorderMin = newMin;
            m_LastBorderPadding = m_BorderPadding;

            Vector3 cornerMin = grid.CellToWorld(new Vector3Int(xMin, yMin, 0));
            Vector3 cornerMax = grid.CellToWorld(new Vector3Int(xMax + 1, yMax + 1, 0));
            Vector3 center = (cornerMin + cornerMax) * 0.5f;
            float boardW = cornerMax.x - cornerMin.x;
            float boardH = cornerMax.y - cornerMin.y;
            float totalH = boardH + m_BorderPadding * 2f;

            if (m_BorderSpriteRenderer != null)
            {
                var ls = m_BorderSpriteRenderer.transform.lossyScale;
                m_BorderSpriteRenderer.drawMode = SpriteDrawMode.Sliced;
                m_BorderSpriteRenderer.size = new Vector2(
                    (boardW + m_BorderPadding * 2f) / Mathf.Abs(ls.x),
                    totalH / Mathf.Abs(ls.y));
                var p = m_BorderSpriteRenderer.transform.position;
                m_BorderSpriteRenderer.transform.position = new Vector3(center.x, center.y, p.z);
            }

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
            float parentScaleY = shine.transform.parent != null
                ? Mathf.Abs(shine.transform.parent.lossyScale.y) : 1f;
            shine.transform.localScale = new Vector3(s.x, targetH / (spriteH * parentScaleY), s.z);
        }

#if UNITY_EDITOR
        private void OnValidate()
        {
            UnityEditor.EditorApplication.delayCall += RefreshBorderInEditor;
        }

        private void RefreshBorderInEditor()
        {
            if (this == null) return;

            var grid = GetComponent<Grid>();
            if (grid == null) return;


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

            if (xMin == int.MaxValue) return;

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

            var gem = Instantiate(gemPrefab, m_Grid.GetCellCenterWorld(cell), Quaternion.identity, m_Grid.transform);

            int finalType = overrideType != -1 ? overrideType : gemPrefab.GemType;
            if (m_GemDataLookup != null && m_GemDataLookup.TryGetValue(finalType, out var data))
            {
                gem.Setup(data);
            }

            CellContent[cell].ContainingGem = gem;
            gem.Init(cell);

            return gem;
        }
        //
        //
        //
        public void GenerateBoard()
        {
            CalculateBounds();

            for (int y = m_BoundsInt.yMin; y <= m_BoundsInt.yMax; ++y)
            {
                for (int x = m_BoundsInt.xMin; x <= m_BoundsInt.xMax; ++x)
                {
                    var idx = new Vector3Int(x, y, 0);

                    if (!CellContent.TryGetValue(idx, out var current) || current.ContainingGem != null)
                        continue;

                    var availableGems = new List<int>(m_AvailableGemTypes);


                    for (int i = availableGems.Count - 1; i >= 0; i--)
                    {
                        int type = availableGems[i];

                        bool hMatch = (HasGemType(idx + Vector3Int.left, type) && HasGemType(idx + Vector3Int.left * 2, type))
                                   || (HasGemType(idx + Vector3Int.right, type) && HasGemType(idx + Vector3Int.right * 2, type))
                                   || (HasGemType(idx + Vector3Int.left, type) && HasGemType(idx + Vector3Int.right, type));

                        bool vMatch = (HasGemType(idx + Vector3Int.down, type) && HasGemType(idx + Vector3Int.down * 2, type))
                                   || (HasGemType(idx + Vector3Int.up, type) && HasGemType(idx + Vector3Int.up * 2, type))
                                   || (HasGemType(idx + Vector3Int.down, type) && HasGemType(idx + Vector3Int.up, type));

                        bool sqMatch =
                            (HasGemType(idx + Vector3Int.right, type) && HasGemType(idx + Vector3Int.down, type) && HasGemType(idx + Vector3Int.right + Vector3Int.down, type))
                         || (HasGemType(idx + Vector3Int.left, type) && HasGemType(idx + Vector3Int.down, type) && HasGemType(idx + Vector3Int.left + Vector3Int.down, type))
                         || (HasGemType(idx + Vector3Int.right, type) && HasGemType(idx + Vector3Int.up, type) && HasGemType(idx + Vector3Int.right + Vector3Int.up, type))
                         || (HasGemType(idx + Vector3Int.left, type) && HasGemType(idx + Vector3Int.up, type) && HasGemType(idx + Vector3Int.left + Vector3Int.up, type));

                        if (hMatch || vMatch || sqMatch)
                        {
                            availableGems.RemoveAt(i);
                        }
                    }

                    if (availableGems.Count > 0)
                    {
                        int chosenType = availableGems[Random.Range(0, availableGems.Count)];
                        NewGemAt(idx, null, chosenType);
                    }
                    else
                    {
                        if (m_AvailableGemTypes.Count > 0)
                        {
                            int fallbackType = m_AvailableGemTypes[0];
                            NewGemAt(idx, null, fallbackType);
                        }
                    }
                }
            }
        }
        private int EvalBonusPriority(Vector3Int startCell)
        {
            if (!CellContent.TryGetValue(startCell, out var center) || center.ContainingGem == null)
                return 0;

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

        private BonusGem SelectBonusByPriority(List<BonusGem> candidates)
        {
            foreach (var b in candidates) if (b is ColorClean) return b;
            foreach (var b in candidates) if (b is LargeBomb) return b;
            foreach (var b in candidates) if (b is LineRocket) return b;
            foreach (var b in candidates) if (b is SmallBomb) return b;
            return candidates[0];
        }

        private bool HasGemType(Vector3Int pos, int type)
        {
            if (CellContent.TryGetValue(pos, out var cell) && cell.ContainingGem != null)
            {
                return cell.ContainingGem.GemType == type;
            }
            return false;
        }
        public static void RegisterCell(Vector3Int cellPosition, GemData startingGemData = null)
        {
            // Not super happy with that, but Startup is called before all Awake....
            if (s_Instance == null)
            {
                if (EnsureInstance() == null) return;
            }

            if (!s_Instance.CellContent.ContainsKey(cellPosition))
            {
                s_Instance.CellContent.Add(cellPosition, new BoardCell());
            }

            if (startingGemData != null)
            {
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
                if (EnsureInstance() == null) return;
            }
            if (!s_Instance.SpawnerPosition.Contains(position))
            {
                s_Instance.SpawnerPosition.Add(position);
            }
        }
        public static void RegisterGemPrefab(Vector3Int cellPosition, Gem gemPrefab)
        {
            if (s_Instance == null)
            {
                if (EnsureInstance() == null) return;
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
        void ActivateSpawnerAt(Vector3Int cellIdx)
        {
            if (m_AvailableGemTypes.Count == 0) return;

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

            var priorityTypes = new List<int>();
            var tables = FindObjectsByType<Table>();
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

            Vector3 spawnPos = m_Grid.GetCellCenterWorld(cellIdx + Vector3Int.up);
            var newObj = Instantiate(prefab, spawnPos, Quaternion.identity, transform);

            Gem newGem = newObj.GetComponent<Gem>();

            if (m_GemDataLookup != null && m_GemDataLookup.TryGetValue(chosenType, out var data))
            {
                newGem.Setup(data);
            }

            newGem.Init(cellIdx);

            CellContent[cellIdx].IncomingGem = newGem;

            newGem.StartMoveTimer();
            newGem.SpeedMultiplier = 1.0f;
            m_NewTickingCells.Add(cellIdx);

            if (m_EmptyCells.Contains(cellIdx)) m_EmptyCells.Remove(cellIdx);

            TrySpawnIceBlock(cellIdx);
        }
        private void TrySpawnIceBlock(Vector3Int cellIdx)
        {
            if (IceBlockPrefab == null) return;

            var levelData = LevelData.Instance;
            if (levelData == null || levelData.IceSpawnChance <= 0f) return;

            if (Random.value > levelData.IceSpawnChance) return;

            if (!CellContent.TryGetValue(cellIdx, out var cell)) return;
            var gem = cell.IncomingGem ?? cell.ContainingGem;
            if (gem == null || gem.IsFrozen) return;

            var iceGo = Instantiate(IceBlockPrefab);
            var ice = iceGo.GetComponent<IceBlock>();

            if (ice == null)
            {
                Debug.LogWarning("[Board] IceBlockPrefab is missing the MyMatch3.IceBlock component.");
                Destroy(iceGo);
                return;
            }

            ice.Attach(gem);
        }

        public static void AddObstacle(Vector3Int cell, Obstacle obstacle)
        {
            if (s_Instance == null) return;

            RegisterCell(cell);

            if (s_Instance.CellContent.ContainsKey(cell))
            {
                s_Instance.CellContent[cell].Obstacle = obstacle;
            }

            obstacle.transform.position = s_Instance.m_Grid.GetCellCenterWorld(cell);
        }


        public static void ChangeLock(Vector3Int cell, bool locked)
        {
            if (s_Instance != null && s_Instance.CellContent.ContainsKey(cell))
            {
                s_Instance.CellContent[cell].Locked = locked;
            }
        }

        public static void RegisterDeletedCallback(Vector3Int cell, System.Action callback)
        {
            if (s_Instance == null) return;
            AddCallback(s_Instance.m_CellsCallbacks, cell, callback);
        }

        public static void UnregisterDeletedCallback(Vector3Int cell, System.Action callback)
        {
            if (s_Instance == null || !s_Instance.m_CellsCallbacks.ContainsKey(cell))
                return;
            RemoveCallback(s_Instance.m_CellsCallbacks, cell, callback);
        }

        public static void RegisterMatchedCallback(Vector3Int cell, System.Action callback)
        {
            if (s_Instance == null) return;
            AddCallback(s_Instance.m_MatchedCallback, cell, callback);
        }

        public static void UnregisterMatchedCallback(Vector3Int cell, System.Action callback)
        {
            if (s_Instance == null || !s_Instance.m_MatchedCallback.ContainsKey(cell))
                return;
            RemoveCallback(s_Instance.m_MatchedCallback, cell, callback);
        }

        private static void AddCallback(Dictionary<Vector3Int, System.Action> callbacks, Vector3Int cell, System.Action callback)
        {
            if (callbacks.ContainsKey(cell))
                callbacks[cell] += callback;
            else
                callbacks[cell] = callback;
        }

        private static void RemoveCallback(Dictionary<Vector3Int, System.Action> callbacks, Vector3Int cell, System.Action callback)
        {
            callbacks[cell] -= callback;
            if (callbacks[cell] == null)
                callbacks.Remove(cell);
        }



        public void AddNewBoardAction(IBoardAction action)
        {
            m_BoardActions.Add(action);
        }
        public interface IBoardAction
        {
            bool Tick();
        }
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
        public void ActivateBonusItem(Booster item)
        {
            m_ActivatedBonusItem = item;
        }
        public void ShuffleGems()
        {
            StartCoroutine(ShuffleRoutine());
        }

        private IEnumerator ShuffleRoutine()
        {
            var positions = new List<Vector3Int>();
            foreach (var kvp in CellContent)
            {
                var gem = kvp.Value.ContainingGem;
                if (gem == null || gem is BonusGem || gem is Crate) continue;
                if (gem.CurrentMatch != null) continue;

                if (kvp.Value.Obstacle != null) continue;
                if (gem.IsFrozen) continue;

                positions.Add(kvp.Key);
            }

            if (positions.Count < 2) yield break;

            LockMovement();

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

            foreach (var pos in positions)
            {
                var gem = CellContent[pos].ContainingGem;
                if (gem != null)
                {
                    gem.Destroyed();
                    Destroy(gem.gameObject);

                    CellContent[pos].ContainingGem = null;
                    if (!m_EmptyCells.Contains(pos))
                        m_EmptyCells.Add(pos);
                }
            }

            UnlockMovement();
        }
    }

}
