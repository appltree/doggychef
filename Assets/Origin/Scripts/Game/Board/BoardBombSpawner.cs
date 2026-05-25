// OK
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(BoardBlockManager))]
[RequireComponent(typeof(BoardBombExploder))]
public class BoardBombSpawner : MonoBehaviour
{
    [Header("Bomb Settings")]
    [SerializeField] private BombBlockSO[] bombBlockSOs;
    private readonly float bombAutoExplodeDelay = 0.2f; // 폭탄 생성 후 자동 폭발 지연 시간

    private BoardBlockManager board;
    private BoardBombExploder bombExploder; // 새로 스폰된 폭탄 즉시 폭발 처리용

    // 파괴 완료 후 생성할 폭탄 큐 (중복 방지 세트 포함)
    private readonly List<(Vector2Int pos, BombBlock.BombType type)> queuedBombs = new();
    private readonly HashSet<Vector2Int> queuedBombPositions = new();
    // 이번 큐잉에서 Double 폭탄을 만들기 위해 사용된 블록들 (파괴 단계에서 제외하기 위함)
    private readonly HashSet<Block> blocksUsedForDouble = new();


    void Awake()
    {
        board = GetComponent<BoardBlockManager>();
        bombExploder = GetComponent<BoardBombExploder>();
    }


    // 외부(디버그 시더 등)가 폭탄 정의 목록을 조회할 수 있도록 공개 getter 제공
    public BombBlockSO[] GetAllBombDefinitions()
    {
        return bombBlockSOs;
    }

    // 이번 큐잉에서 Double 폭탄을 만들기 위해 사용된 블록 목록을 반환하고 내부 기록을 비웁니다.
    // BeginDestructionPhase 직전에 호출되어 해당 블록들이 파괴되지 않도록 필터링하는 데 사용됩니다.
    public IReadOnlyCollection<Block> ConsumeBlocksUsedForDouble()
    {
        var copy = new HashSet<Block>(blocksUsedForDouble);
        blocksUsedForDouble.Clear();
        return copy;
    }


    // 현재 보드 상태에서 폭탄 후보를 찾아 큐잉한다. 스왑 유도를 위해 중심 블록 우선 선택을 지원.
    public void QueueBombsForCurrentMatches(Block swappedA = null, Block swappedB = null)
    {
        // 전역 커버리지: 이미 더 높은 우선순위로 폭탄 후보로 선택된 매치에 포함된 블록들
        // 이후 단계(낮은 우선순위)는 이 커버리지에 겹치면 건너뛴다.
        var coveredBlocks = new HashSet<Block>();

        // 이전 기록 초기화 (이번 라운드에서만 유효)
        blocksUsedForDouble.Clear();

        // 직선 5개 매치로 Light Ball 생성 (가로/세로 모두)
        var fiveInARowMatches = FindStraightFiveOrMoreMatches(swappedA, swappedB);
        foreach (var (centerBlock, matchedBlocks) in fiveInARowMatches)
        {
            bool isValid = IsValidBlocks(coveredBlocks, matchedBlocks);
            if (isValid)
            {
                EnqueueBomb(centerBlock, BombBlock.BombType.LightBall);
            }
        }

        // L/T 매치로 TNT 생성 (LightBall 다음 우선순위)
        var tntMatches = FindLTMatchesForTNT(swappedA, swappedB);
        foreach (var (centerBlock, matchedBlocks) in tntMatches)
        {
            bool isValid = IsValidBlocks(coveredBlocks, matchedBlocks);
            if (isValid)
            {
                EnqueueBomb(centerBlock, BombBlock.BombType.TNT);
            }
        }



        // 2x2 + 인접 2개(총 6개) 매치는 5개보다 우선으로 처리
        var doublePlusTwoMatches = FindSquare2x2PlusTwoMatches(swappedA, swappedB);
        foreach (var (centerBlock, core4, all6) in doublePlusTwoMatches)
        {
            bool isValid = IsValidBlocks(coveredBlocks, all6);
            if (isValid)
            {
                foreach (var b in all6)
                    blocksUsedForDouble.Add(b);
                StartCoroutine(DelayedDoublePlusTwoExplosionWithGroup(all6));
            }
        }

        // 2x2 + 인접 1개(총 5개) 매치는 DoubleDouble로 처리 (Double보다 우선)
        var doubleDoubleMatches = FindSquare2x2PlusOneMatches(swappedA, swappedB);
        foreach (var (centerBlock, core4, all5) in doubleDoubleMatches)
        {
            bool isValid = IsValidBlocks(coveredBlocks, all5);
            if (isValid)
            {
                foreach (var b in all5)
                    blocksUsedForDouble.Add(b);
                // DoubleDouble도 Double과 동일하게 지연 후 bombExploder에서 처리
                StartCoroutine(DelayedDoubleDoubleExplosionWithGroup(all5));
            }
        }

        var doubleMatches = FindSquare2x2Matches(swappedA, swappedB);
        foreach (var (centerBlock, matchedBlocks) in doubleMatches)
        {
            bool isValid = IsValidBlocks(coveredBlocks, matchedBlocks);
            if (isValid)
            {
                // Double 폭탄 생성에 사용된 4블록을 기록하여 파괴 단계에서 제외
                foreach (var b in matchedBlocks)
                    blocksUsedForDouble.Add(b);
                StartCoroutine(DelayedDoubleExplosionWithGroup(matchedBlocks));
            }
        }

        var horizontalMatches = FindHorizontalMatches(4, swappedA, swappedB);
        foreach (var (centerBlock, matchedBlocks) in horizontalMatches)
        {
            bool isValid = IsValidBlocks(coveredBlocks, matchedBlocks);
            if (isValid)
            {
                EnqueueBomb(centerBlock, BombBlock.BombType.Vertical);
            }
        }

        var verticalMatches = FindVerticalMatches(4, swappedA, swappedB);
        foreach (var (centerBlock, matchedBlocks) in verticalMatches)
        {
            bool isValid = IsValidBlocks(coveredBlocks, matchedBlocks);
            if (isValid)
            {
                EnqueueBomb(centerBlock, BombBlock.BombType.Horizontal);
            }
        }

    }


    // 반환값: shouldUnlockGravity (true면 보드가 낙하를 재개해도 됨)
    public bool SpawnQueuedBombs()
    {
        if (queuedBombs.Count == 0)
            return true; // 큐가 비어있다면 잠금 해제해도 안전

        bool scheduledExplosion = false; // 한 번이라도 폭발이 예정되었는지

        foreach (var (pos, type) in queuedBombs)
        {
            if (type == BombBlock.BombType.TNT)
            {
                StartCoroutine(DelayedTntExplosionAtPosition(pos));
                scheduledExplosion = true;
            }
            else if (type == BombBlock.BombType.Horizontal)
            {
                StartCoroutine(DelayedHorizontalExplosionAtPosition(pos));
                scheduledExplosion = true;
            }
            else if (type == BombBlock.BombType.Vertical)
            {
                StartCoroutine(DelayedVerticalExplosionAtPosition(pos));
                scheduledExplosion = true;
            }
            else if (type == BombBlock.BombType.LightBall)
            {
                BombBlockSO bombDef = GetBombPrefabFor(type);
                Vector3 spawnLocalPos = new(pos.x, pos.y, 0);
                GameObject bombObject = Instantiate(bombDef.Prefab, transform);
                bombObject.transform.localPosition = spawnLocalPos;

                BombBlock bombBlock = bombObject.GetComponent<BombBlock>();
                bombDef.ApplyTo(bombBlock);
                board.RegisterBlock(bombBlock, spawnLocalPos);
            }
        }

        // 폭발 예정이 하나도 없다면(아무것도 처리 안했거나 LightBall만 생성) 잠금 해제 신호
        bool shouldUnlock = !scheduledExplosion;

        queuedBombs.Clear();
        queuedBombPositions.Clear();

        return shouldUnlock;
    }


    private bool IsValidBlocks(HashSet<Block> overlapCheckBlocks, List<Block> targetBlocks)
    {
        foreach (var b in targetBlocks)
        {
            if (overlapCheckBlocks.Contains(b))
                return false;
            if (b.WasExplodedByBomb)
                return false;
        }

        foreach (var b in targetBlocks) overlapCheckBlocks.Add(b);
        return true;
    }


    private IEnumerator DelayedTntExplosionAtPosition(Vector2Int pos)
    {
        yield return new WaitForSeconds(bombAutoExplodeDelay);
        // 중심은 이미 제거된 경우가 대부분이므로 destroyCenter=false로 파동 처리
        yield return StartCoroutine(bombExploder.TriggerTntExplosionAtPosition(pos, false));
    }


    private IEnumerator DelayedHorizontalExplosionAtPosition(Vector2Int pos)
    {
        yield return new WaitForSeconds(bombAutoExplodeDelay);
        yield return StartCoroutine(bombExploder.TriggerHorizontalExplosionAtPosition(pos));
    }


    private IEnumerator DelayedVerticalExplosionAtPosition(Vector2Int pos)
    {
        yield return new WaitForSeconds(bombAutoExplodeDelay);
        yield return StartCoroutine(bombExploder.TriggerVerticalExplosionAtPosition(pos));
    }


    private IEnumerator DelayedDoubleExplosionWithGroup(List<Block> group)
    {
        // 그룹 리스트는 큐 클리어 전에 복사해 둔다. (SpawnQueuedBombs 종료 시 딕셔너리 초기화됨)
        var groupCopy = (group != null) ? new List<Block>(group) : null;
        yield return new WaitForSeconds(bombAutoExplodeDelay);

        if (groupCopy != null && groupCopy.Count > 0)
        {
            yield return StartCoroutine(bombExploder.TriggerDoubleExplosionWithGroup(groupCopy, 8));
        }
    }


    private IEnumerator DelayedDoubleDoubleExplosionWithGroup(List<Block> group)
    {
        var groupCopy = (group != null) ? new List<Block>(group) : null;
        yield return new WaitForSeconds(bombAutoExplodeDelay);
        if (groupCopy != null && groupCopy.Count > 0)
        {
            // DoubleDouble은 재료 9회 방출
            yield return StartCoroutine(bombExploder.TriggerDoubleExplosionWithGroup(groupCopy, 9));
        }
    }


    private IEnumerator DelayedDoublePlusTwoExplosionWithGroup(List<Block> group)
    {
        var groupCopy = (group != null) ? new List<Block>(group) : null;
        yield return new WaitForSeconds(bombAutoExplodeDelay);
        if (groupCopy != null && groupCopy.Count > 0)
        {
            // 2x2 + 2개(총 6개)는 재료 10회 방출 (4기본 8 + 추가 2)
            yield return StartCoroutine(bombExploder.TriggerDoubleExplosionWithGroup(groupCopy, 10));
        }
    }


    private void EnqueueBomb(Block originalBlock, BombBlock.BombType bombType)
    {
        if (originalBlock == null)
            return;

        Vector2Int position = board.GetBlockPosition(originalBlock);

        if (queuedBombPositions.Add(position))
        {
            queuedBombs.Add((position, bombType));
        }
    }


    private BombBlockSO GetBombPrefabFor(BombBlock.BombType type)
    {
        if (bombBlockSOs == null) return null;
        foreach (var def in bombBlockSOs)
        {
            if (def == null) continue;
            var defType = def.Type;
            if (defType == type)
            {
                return def;
            }
        }
        return null;
    }


    private List<(Block centerBlock, List<Block> matchedBlocks)> FindHorizontalMatches(int minCount, Block swappedA = null, Block swappedB = null)
    {
        var matches = new List<(Block centerBlock, List<Block> matchedBlocks)>();
        if (board == null) return matches;

        for (int y = 0; y < board.Height; y++)
        {
            for (int x = 0; x <= board.Width - minCount; x++)
            {
                var matchGroup = new List<Block>();
                string targetTag = null;

                for (int i = 0; i < minCount; i++)
                {
                    Block block = board.GetBlockAtPosition(x + i, y);
                    if (block == null || !block.IsMatchable)
                        break;

                    if (targetTag == null)
                        targetTag = block.BlockTag;
                    else if (block.BlockTag != targetTag)
                        break;

                    matchGroup.Add(block);
                }

                if (matchGroup.Count >= minCount)
                {
                    Block centerBlock = null;
                    if (swappedA != null && matchGroup.Contains(swappedA))
                        centerBlock = swappedA;
                    else if (swappedB != null && matchGroup.Contains(swappedB))
                        centerBlock = swappedB;
                    else
                        centerBlock = matchGroup[matchGroup.Count / 2];

                    matches.Add((centerBlock, matchGroup));
                }
            }
        }

        return matches;
    }


    private List<(Block centerBlock, List<Block> matchedBlocks)> FindVerticalMatches(int minCount, Block swappedA = null, Block swappedB = null)
    {
        var matches = new List<(Block centerBlock, List<Block> matchedBlocks)>();
        if (board == null) return matches;

        for (int x = 0; x < board.Width; x++)
        {
            for (int y = 0; y <= board.Height - minCount; y++)
            {
                var matchGroup = new List<Block>();
                string targetTag = null;

                for (int i = 0; i < minCount; i++)
                {
                    Block block = board.GetBlockAtPosition(x, y + i);
                    if (block == null || !block.IsMatchable)
                        break;

                    if (targetTag == null)
                        targetTag = block.BlockTag;
                    else if (block.BlockTag != targetTag)
                        break;

                    matchGroup.Add(block);
                }

                if (matchGroup.Count >= minCount)
                {
                    Block centerBlock = null;
                    if (swappedA != null && matchGroup.Contains(swappedA))
                        centerBlock = swappedA;
                    else if (swappedB != null && matchGroup.Contains(swappedB))
                        centerBlock = swappedB;
                    else
                        centerBlock = matchGroup[matchGroup.Count / 2];

                    matches.Add((centerBlock, matchGroup));
                }
            }
        }

        return matches;
    }



    // 5개 매치의 L/T 형태 탐지 (모두 같은 태그여야 함). 교차/코너 블록을 중심으로 TNT 폭탄을 큐잉.
    private List<(Block centerBlock, List<Block> matchedBlocks)> FindLTMatchesForTNT(Block swappedA = null, Block swappedB = null)
    {
        var results = new List<(Block centerBlock, List<Block> matchedBlocks)>();
        if (board == null) return results;

        // 피벗(교차/코너) 기준 상대 좌표 패턴들
        // L자 4방향
        Vector2Int[][] lPatterns = new Vector2Int[][]
        {
            new [] { new Vector2Int(0,0), new Vector2Int(1,0), new Vector2Int(2,0), new Vector2Int(0,1), new Vector2Int(0,2) }, // └─ type (pivot at corner)
            new [] { new Vector2Int(0,0), new Vector2Int(-1,0), new Vector2Int(-2,0), new Vector2Int(0,1), new Vector2Int(0,2) }, // ┘─ rotated
            new [] { new Vector2Int(0,0), new Vector2Int(1,0), new Vector2Int(2,0), new Vector2Int(0,-1), new Vector2Int(0,-2) }, // ┌─ rotated
            new [] { new Vector2Int(0,0), new Vector2Int(-1,0), new Vector2Int(-2,0), new Vector2Int(0,-1), new Vector2Int(0,-2) }, // ┐─ rotated
        };

        // T자 4방향 (피벗은 교차점)
        Vector2Int[][] tPatterns = new Vector2Int[][]
        {
            new [] { new Vector2Int(0,0), new Vector2Int(-1,0), new Vector2Int(1,0), new Vector2Int(0,1), new Vector2Int(0,2) },  // ㅗ (stem down)
            new [] { new Vector2Int(0,0), new Vector2Int(-1,0), new Vector2Int(1,0), new Vector2Int(0,-1), new Vector2Int(0,-2) }, // ㅜ (stem up)
            new [] { new Vector2Int(0,0), new Vector2Int(0,-1), new Vector2Int(0,1), new Vector2Int(1,0), new Vector2Int(2,0) },  // ㅓ (stem right)
            new [] { new Vector2Int(0,0), new Vector2Int(0,-1), new Vector2Int(0,1), new Vector2Int(-1,0), new Vector2Int(-2,0) }, // ㅏ (stem left)
        };

        var usedPivotPositions = new HashSet<Vector2Int>();

        bool TryMatchPatternAt(int px, int py, Vector2Int[] pattern, out List<Block> matched)
        {
            matched = new List<Block>(5);
            // 첫 블록 기준 태그 결정
            Block first = board.GetBlockAtPosition(px + pattern[0].x, py + pattern[0].y);
            if (first == null || !first.IsMatchable) return false;
            string tag = first.BlockTag;

            for (int i = 0; i < pattern.Length; i++)
            {
                int x = px + pattern[i].x;
                int y = py + pattern[i].y;
                if (x < 0 || y < 0 || x >= board.Width || y >= board.Height)
                    return false;
                var b = board.GetBlockAtPosition(x, y);
                if (b == null || !b.IsMatchable || b.BlockTag != tag)
                    return false;
                matched.Add(b);
            }
            return true;
        }

        // 모든 좌표를 피벗으로 시도
        for (int py = 0; py < board.Height; py++)
        {
            for (int px = 0; px < board.Width; px++)
            {
                var pivotBlock = board.GetBlockAtPosition(px, py);
                if (pivotBlock == null || !pivotBlock.IsMatchable) continue;
                var pivotPos = new Vector2Int(px, py);

                // L 패턴 검사
                foreach (var pattern in lPatterns)
                {
                    if (TryMatchPatternAt(px, py, pattern, out var matchedBlocks))
                    {
                        if (usedPivotPositions.Add(pivotPos))
                        {
                            Block center = (swappedA != null && matchedBlocks.Contains(swappedA)) ? swappedA :
                                           (swappedB != null && matchedBlocks.Contains(swappedB)) ? swappedB :
                                           pivotBlock;
                            results.Add((center, matchedBlocks));
                        }
                        break; // 동일 피벗 중복 방지
                    }
                }

                // 이미 L에서 잡혔다면 T는 생략
                if (usedPivotPositions.Contains(pivotPos))
                    continue;

                // T 패턴 검사
                foreach (var pattern in tPatterns)
                {
                    if (TryMatchPatternAt(px, py, pattern, out var matchedBlocks))
                    {
                        if (usedPivotPositions.Add(pivotPos))
                        {
                            Block center = (swappedA != null && matchedBlocks.Contains(swappedA)) ? swappedA :
                                           (swappedB != null && matchedBlocks.Contains(swappedB)) ? swappedB :
                                           pivotBlock;
                            results.Add((center, matchedBlocks));
                        }
                        break; // 동일 피벗 중복 방지
                    }
                }
            }
        }

        return results;
    }


    // 가로/세로로 같은 태그가 5개 이상 연속된 구간을 찾아 Light Ball 후보를 반환
    private List<(Block centerBlock, List<Block> matchedBlocks)> FindStraightFiveOrMoreMatches(Block swappedA = null, Block swappedB = null)
    {
        var results = new List<(Block centerBlock, List<Block> matchedBlocks)>();
        if (board == null) return results;

        // 가로 스캔
        for (int y = 0; y < board.Height; y++)
        {
            int x = 0;
            while (x < board.Width)
            {
                Block start = board.GetBlockAtPosition(x, y);
                if (start == null || !start.IsMatchable)
                {
                    x++;
                    continue;
                }
                string tag = start.BlockTag;
                var run = new List<Block>();
                int rx = x;
                while (rx < board.Width)
                {
                    var b = board.GetBlockAtPosition(rx, y);
                    if (b == null || !b.IsMatchable || b.BlockTag != tag) break;
                    run.Add(b);
                    rx++;
                }
                if (run.Count >= 5)
                {
                    Block center = (swappedA != null && run.Contains(swappedA)) ? swappedA :
                                   (swappedB != null && run.Contains(swappedB)) ? swappedB :
                                   run[run.Count / 2];
                    results.Add((center, run));
                }
                x = rx; // 다음 런으로 이동
            }
        }

        // 세로 스캔
        for (int x = 0; x < board.Width; x++)
        {
            int y = 0;
            while (y < board.Height)
            {
                Block start = board.GetBlockAtPosition(x, y);
                if (start == null || !start.IsMatchable)
                {
                    y++;
                    continue;
                }
                string tag = start.BlockTag;
                var run = new List<Block>();
                int ry = y;
                while (ry < board.Height)
                {
                    var b = board.GetBlockAtPosition(x, ry);
                    if (b == null || !b.IsMatchable || b.BlockTag != tag) break;
                    run.Add(b);
                    ry++;
                }
                if (run.Count >= 5)
                {
                    Block center = (swappedA != null && run.Contains(swappedA)) ? swappedA :
                                   (swappedB != null && run.Contains(swappedB)) ? swappedB :
                                   run[run.Count / 2];
                    results.Add((center, run));
                }
                y = ry; // 다음 런으로 이동
            }
        }

        return results;
    }


    // 2x2 정사각형 매치 탐지: 같은 태그 4개가 2x2를 이루면 Double 폭탄 후보로 반환
    private List<(Block centerBlock, List<Block> matchedBlocks)> FindSquare2x2Matches(Block swappedA = null, Block swappedB = null)
    {
        var results = new List<(Block centerBlock, List<Block> matchedBlocks)>();
        if (board == null) return results;

        for (int y = 0; y < board.Height - 1; y++)
        {
            for (int x = 0; x < board.Width - 1; x++)
            {
                var b00 = board.GetBlockAtPosition(x, y);
                var b10 = board.GetBlockAtPosition(x + 1, y);
                var b01 = board.GetBlockAtPosition(x, y + 1);
                var b11 = board.GetBlockAtPosition(x + 1, y + 1);

                if (b00 == null || b10 == null || b01 == null || b11 == null)
                    continue;

                // 폭탄 포함 제외 + 매치 가능 상태만
                if (!b00.IsMatchable || !b10.IsMatchable || !b01.IsMatchable || !b11.IsMatchable)
                    continue;
                if (b00 is BombBlock || b10 is BombBlock || b01 is BombBlock || b11 is BombBlock)
                    continue;

                string tag = b00.BlockTag;
                if (string.IsNullOrEmpty(tag))
                    continue;

                if (b10.BlockTag == tag && b01.BlockTag == tag && b11.BlockTag == tag)
                {
                    var group = new List<Block>(4) { b00, b10, b01, b11 };
                    Block center = (swappedA != null && group.Contains(swappedA)) ? swappedA :
                                   (swappedB != null && group.Contains(swappedB)) ? swappedB :
                                   b00; // 기본은 좌상단
                    results.Add((center, group));
                }
            }
        }

        return results;
    }

    // 2x2 + 1(상하좌우 인접) 형태 탐지: 같은 태그 5개로 DoubleDouble 후보를 반환
    // 반환: (중심 블록, 코어 4블록, 전체 5블록)
    private List<(Block centerBlock, List<Block> core4, List<Block> all5)> FindSquare2x2PlusOneMatches(Block swappedA = null, Block swappedB = null)
    {
        var results = new List<(Block centerBlock, List<Block> core4, List<Block> all5)>();
        if (board == null) return results;

        var seen = new HashSet<string>();

        for (int y = 0; y < board.Height - 1; y++)
        {
            for (int x = 0; x < board.Width - 1; x++)
            {
                var b00 = board.GetBlockAtPosition(x, y);
                var b10 = board.GetBlockAtPosition(x + 1, y);
                var b01 = board.GetBlockAtPosition(x, y + 1);
                var b11 = board.GetBlockAtPosition(x + 1, y + 1);

                if (b00 == null || b10 == null || b01 == null || b11 == null)
                    continue;
                if (!b00.IsMatchable || !b10.IsMatchable || !b01.IsMatchable || !b11.IsMatchable)
                    continue;
                if (b00 is BombBlock || b10 is BombBlock || b01 is BombBlock || b11 is BombBlock)
                    continue;

                string tag = b00.BlockTag;
                if (string.IsNullOrEmpty(tag)) continue;
                if (b10.BlockTag != tag || b01.BlockTag != tag || b11.BlockTag != tag) continue;

                var core = new List<Block>(4) { b00, b10, b01, b11 };

                int[] ids = { b00.GetInstanceID(), b10.GetInstanceID(), b01.GetInstanceID(), b11.GetInstanceID() };
                System.Array.Sort(ids);
                string key = $"{ids[0]},{ids[1]},{ids[2]},{ids[3]}";
                if (!seen.Add(key)) continue;

                Block extra = null;
                // 왼쪽
                if (x - 1 >= 0 && extra == null)
                {
                    var c1 = board.GetBlockAtPosition(x - 1, y);
                    var c2 = board.GetBlockAtPosition(x - 1, y + 1);
                    if (c1 != null && c1.IsMatchable && c1.BlockTag == tag && c1 is not BombBlock) extra = c1;
                    else if (c2 != null && c2.IsMatchable && c2.BlockTag == tag && c2 is not BombBlock) extra = c2;
                }
                // 오른쪽
                if (x + 2 < board.Width && extra == null)
                {
                    var c1 = board.GetBlockAtPosition(x + 2, y);
                    var c2 = board.GetBlockAtPosition(x + 2, y + 1);
                    if (c1 != null && c1.IsMatchable && c1.BlockTag == tag && c1 is not BombBlock) extra = c1;
                    else if (c2 != null && c2.IsMatchable && c2.BlockTag == tag && c2 is not BombBlock) extra = c2;
                }
                // 아래
                if (y - 1 >= 0 && extra == null)
                {
                    var c1 = board.GetBlockAtPosition(x, y - 1);
                    var c2 = board.GetBlockAtPosition(x + 1, y - 1);
                    if (c1 != null && c1.IsMatchable && c1.BlockTag == tag && c1 is not BombBlock) extra = c1;
                    else if (c2 != null && c2.IsMatchable && c2.BlockTag == tag && c2 is not BombBlock) extra = c2;
                }
                // 위
                if (y + 2 < board.Height && extra == null)
                {
                    var c1 = board.GetBlockAtPosition(x, y + 2);
                    var c2 = board.GetBlockAtPosition(x + 1, y + 2);
                    if (c1 != null && c1.IsMatchable && c1.BlockTag == tag && c1 is not BombBlock) extra = c1;
                    else if (c2 != null && c2.IsMatchable && c2.BlockTag == tag && c2 is not BombBlock) extra = c2;
                }

                if (extra == null) continue;

                var all5 = new List<Block>(5) { b00, b10, b01, b11, extra };
                Block center = (swappedA != null && all5.Contains(swappedA)) ? swappedA :
                               (swappedB != null && all5.Contains(swappedB)) ? swappedB :
                               b00;
                results.Add((center, core, all5));
            }
        }

        return results;
    }


    // 2x2 + 2(상하좌우 인접) 형태 탐지: 같은 태그 6개로 확장 머지(Double+) 후보를 반환
    // 반환: (중심 블록, 코어 4블록, 전체 6블록)
    private List<(Block centerBlock, List<Block> core4, List<Block> all6)> FindSquare2x2PlusTwoMatches(Block swappedA = null, Block swappedB = null)
    {
        var results = new List<(Block centerBlock, List<Block> core4, List<Block> all6)>();
        if (board == null) return results;

        var seenCore = new HashSet<string>();

        for (int y = 0; y < board.Height - 1; y++)
        {
            for (int x = 0; x < board.Width - 1; x++)
            {
                var b00 = board.GetBlockAtPosition(x, y);
                var b10 = board.GetBlockAtPosition(x + 1, y);
                var b01 = board.GetBlockAtPosition(x, y + 1);
                var b11 = board.GetBlockAtPosition(x + 1, y + 1);

                if (b00 == null || b10 == null || b01 == null || b11 == null)
                    continue;
                if (!b00.IsMatchable || !b10.IsMatchable || !b01.IsMatchable || !b11.IsMatchable)
                    continue;
                if (b00 is BombBlock || b10 is BombBlock || b01 is BombBlock || b11 is BombBlock)
                    continue;

                string tag = b00.BlockTag;
                if (string.IsNullOrEmpty(tag)) continue;
                if (b10.BlockTag != tag || b01.BlockTag != tag || b11.BlockTag != tag) continue;

                // 코어 중복 방지 키
                int[] coreIds = { b00.GetInstanceID(), b10.GetInstanceID(), b01.GetInstanceID(), b11.GetInstanceID() };
                System.Array.Sort(coreIds);
                string coreKey = $"{coreIds[0]},{coreIds[1]},{coreIds[2]},{coreIds[3]}";
                if (!seenCore.Add(coreKey)) continue;

                bool IsGoodExtra(Block b, string t) => b != null && b.IsMatchable && b.BlockTag == t && b is not BombBlock;

                // 각 측면에서 "같은 줄(행/열)로 2칸"을 구성하는 후보 쌍 수집
                var sidePairs = new List<(string side, Block e1, Block e2)>(4);

                // 왼쪽: x-1, x-2 (행 y 또는 y+1)
                if (x - 2 >= 0)
                {
                    var l1 = board.GetBlockAtPosition(x - 1, y);
                    var l2 = board.GetBlockAtPosition(x - 2, y);
                    if (IsGoodExtra(l1, tag) && IsGoodExtra(l2, tag)) sidePairs.Add(("left", l1, l2));

                    var l3 = board.GetBlockAtPosition(x - 1, y + 1);
                    var l4 = board.GetBlockAtPosition(x - 2, y + 1);
                    if (IsGoodExtra(l3, tag) && IsGoodExtra(l4, tag)) sidePairs.Add(("left", l3, l4));
                }

                // 오른쪽: x+2, x+3 (행 y 또는 y+1)
                if (x + 3 < board.Width)
                {
                    var r1 = board.GetBlockAtPosition(x + 2, y);
                    var r2 = board.GetBlockAtPosition(x + 3, y);
                    if (IsGoodExtra(r1, tag) && IsGoodExtra(r2, tag)) sidePairs.Add(("right", r1, r2));

                    var r3 = board.GetBlockAtPosition(x + 2, y + 1);
                    var r4 = board.GetBlockAtPosition(x + 3, y + 1);
                    if (IsGoodExtra(r3, tag) && IsGoodExtra(r4, tag)) sidePairs.Add(("right", r3, r4));
                }

                // 아래: y-1, y-2 (열 x 또는 x+1)
                if (y - 2 >= 0)
                {
                    var d1 = board.GetBlockAtPosition(x, y - 1);
                    var d2 = board.GetBlockAtPosition(x, y - 2);
                    if (IsGoodExtra(d1, tag) && IsGoodExtra(d2, tag)) sidePairs.Add(("bottom", d1, d2));

                    var d3 = board.GetBlockAtPosition(x + 1, y - 1);
                    var d4 = board.GetBlockAtPosition(x + 1, y - 2);
                    if (IsGoodExtra(d3, tag) && IsGoodExtra(d4, tag)) sidePairs.Add(("bottom", d3, d4));
                }

                // 위: y+2, y+3 (열 x 또는 x+1)
                if (y + 3 < board.Height)
                {
                    var u1 = board.GetBlockAtPosition(x, y + 2);
                    var u2 = board.GetBlockAtPosition(x, y + 3);
                    if (IsGoodExtra(u1, tag) && IsGoodExtra(u2, tag)) sidePairs.Add(("top", u1, u2));

                    var u3 = board.GetBlockAtPosition(x + 1, y + 2);
                    var u4 = board.GetBlockAtPosition(x + 1, y + 3);
                    if (IsGoodExtra(u3, tag) && IsGoodExtra(u4, tag)) sidePairs.Add(("top", u3, u4));
                }

                // 가로 양쪽 맞은편(코어에 인접한 x-1과 x+2) 교차 쌍
                if (x - 1 >= 0 && x + 2 < board.Width)
                {
                    var h1 = board.GetBlockAtPosition(x - 1, y);
                    var h2 = board.GetBlockAtPosition(x + 2, y);
                    if (IsGoodExtra(h1, tag) && IsGoodExtra(h2, tag)) sidePairs.Add(("horizontal", h1, h2));

                    var h3 = board.GetBlockAtPosition(x - 1, y + 1);
                    var h4 = board.GetBlockAtPosition(x + 2, y + 1);
                    if (IsGoodExtra(h3, tag) && IsGoodExtra(h4, tag)) sidePairs.Add(("horizontal", h3, h4));
                }

                // 세로 양쪽 맞은편(코어에 인접한 y-1과 y+2) 교차 쌍
                if (y - 1 >= 0 && y + 2 < board.Height)
                {
                    var v1 = board.GetBlockAtPosition(x, y - 1);
                    var v2 = board.GetBlockAtPosition(x, y + 2);
                    if (IsGoodExtra(v1, tag) && IsGoodExtra(v2, tag)) sidePairs.Add(("vertical", v1, v2));

                    var v3 = board.GetBlockAtPosition(x + 1, y - 1);
                    var v4 = board.GetBlockAtPosition(x + 1, y + 2);
                    if (IsGoodExtra(v3, tag) && IsGoodExtra(v4, tag)) sidePairs.Add(("vertical", v3, v4));
                }

                if (sidePairs.Count == 0)
                    continue;

                // 스왑된 블록을 포함하는 쌍을 우선 선택
                int chosenIndex = 0;
                if (swappedA != null || swappedB != null)
                {
                    for (int i = 0; i < sidePairs.Count; i++)
                    {
                        var p = sidePairs[i];
                        if ((swappedA != null && (p.e1 == swappedA || p.e2 == swappedA)) ||
                            (swappedB != null && (p.e1 == swappedB || p.e2 == swappedB)))
                        {
                            chosenIndex = i;
                            break;
                        }
                    }
                }

                var chosen = sidePairs[chosenIndex];
                var core = new List<Block>(4) { b00, b10, b01, b11 };
                var all6 = new List<Block>(6) { b00, b10, b01, b11, chosen.e1, chosen.e2 };

                Block center = (swappedA != null && all6.Contains(swappedA)) ? swappedA :
                               (swappedB != null && all6.Contains(swappedB)) ? swappedB :
                               b00; // 기본 좌상단

                results.Add((center, core, all6));
            }
        }

        return results;
    }
}
