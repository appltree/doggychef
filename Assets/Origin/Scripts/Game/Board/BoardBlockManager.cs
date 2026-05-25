// OK
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

[RequireComponent(typeof(BoardAIAssistant))]
[RequireComponent(typeof(BoardBombSpawner))]
[RequireComponent(typeof(BorderBlockDestroyer))]
public class BoardBlockManager : MonoBehaviour
{
    // 모든 블록이 멈췄을 때 propagate하는 이벤트
    [SerializeField] private int width = 8;
    [SerializeField] private int height = 8;

    // 2D array to manage blocks efficiently
    [SerializeField] private Block[,] boardBlocks;
    private readonly List<Block> currentMatches = new();

    public int Width => width;
    public int Height => height;

    private BoardBlockSpawner[] boardBlockSpawners;
    private BoardAIAssistant boardAIAssistant;
    private BorderBlockDestroyer borderBlockDestroyer;

    // 효율성을 위한 추가 변수들
    private readonly HashSet<Block> movingBlocks = new(); // 현재 움직이는 블록들 추적
    private bool needsMatchCheck = false; // 매치 체크가 필요한지 플래그
    // 파괴-폭탄생성-낙하의 순서를 보장하기 위한 상태
    private bool gravityLocked = false; // 파괴 및 폭탄 생성 중에는 낙하 금지
    private int pendingRemovals = 0; // 현재 턴에서 파괴 대기 블록 수

    // 폭탄 생성 담당 컴포넌트
    private BoardBombSpawner bombSpawner;

    // 낙하 스캔 디바운스
    private readonly float fallScanInterval = 0.1f;
    private float fallScanTimer = 0f;

    // 보드가 안정(모든 블록 이동/낙하 끝 + 파괴단계 아님) 상태인지 여부
    public bool IsBoardStable => movingBlocks.Count == 0 && !gravityLocked;

    void Awake()
    {
        boardBlocks = new Block[width, height];
        boardBlockSpawners = GetComponentsInChildren<BoardBlockSpawner>();
        boardAIAssistant = GetComponent<BoardAIAssistant>();
        bombSpawner = GetComponent<BoardBombSpawner>();
        borderBlockDestroyer = GetComponent<BorderBlockDestroyer>();
    }


    void OnEnable()
    {
        BorderBlockDestroyer.OnBlockRemoved += HandleBlockRemoved;
    }


    void OnDisable()
    {
        BorderBlockDestroyer.OnBlockRemoved -= HandleBlockRemoved;
    }



    void Update()
    {
        // 파괴/폭탄 생성 과정에서는 낙하와 매치 체크를 잠시 정지
        if (!gravityLocked)
        {
            // 디바운스된 간격으로만 낙하 스캔 실행 (이동 중에도 주기적으로 실행하여 동시 낙하 유지)
            fallScanTimer += Time.deltaTime;
            if (fallScanTimer >= fallScanInterval)
            {
                fallScanTimer = 0f;
                FindAndFallBlocks();
            }

            // 매치 체크는 조건부로만 실행
            if (needsMatchCheck)
            {
                var hasMatches = PerformFallMatchCheck();
                if (!hasMatches)
                    boardAIAssistant.HandleCheck();
            }
        }
    }


    // SwapBlocks에서 매칭 검사용 public 메서드
    // 스왑으로 인해 생성된 매치인 경우, 폭탄의 중심은 스왑된 블록을 우선으로 함
    public bool PerformSwapMatchCheck(Block swappedA, Block swappedB)
    {
        currentMatches.Clear();
        FindThreeMatches();
        FindSquareMatches2x2();
        bool hasMatches = currentMatches.Count > 0;

        if (hasMatches)
        {
            // 폭탄을 미리 큐잉하고 파괴를 시작(낙하 잠시 정지)
            bombSpawner.QueueBombsForCurrentMatches(swappedA, swappedB);
            BeginDestructionPhase();
        }

        currentMatches.Clear();
        return hasMatches;
    }


    // Public method for blocks to register themselves when created or moved
    public void RegisterBlock(Block block, Vector3 position)
    {
        Vector2Int boardPos = BoardHelpers.WorldToBoard(position);
        SetBlockAtPosition(boardPos.x, boardPos.y, block);
        block.gameObject.name = $"Block_{boardPos.x}_{boardPos.y}";
    }


    // Public method for blocks to unregister themselves when destroyed or moved
    public void UnregisterBlock(Vector3 position)
    {
        Vector2Int boardPos = BoardHelpers.WorldToBoard(position);
        RemoveBlockAtPosition(boardPos.x, boardPos.y);
    }


    // Update block position in the board array when it moves
    public void UpdateBlockPosition(Block block, Vector3? oldPosition, Vector3 newPosition)
    {
        if (oldPosition.HasValue)
        {
            var prevBlock = FindBlockAtPosition(oldPosition.Value);
            if (prevBlock == block)
                UnregisterBlock(oldPosition.Value);
        }
        RegisterBlock(block, newPosition);
    }


    // 블록이 움직임을 멈췄을 때 호출되는 메서드
    public void OnBlockStopMoving(Block block)
    {
        movingBlocks.Remove(block);

        // 모든 블록이 멈춘 후 매치 체크 플래그 설정
        if (movingBlocks.Count == 0)
        {
            needsMatchCheck = true;
        }
    }


    // 폭탄 등 외부에서 수동 파괴를 시작할 때 사용
    public void BeginManualDestructionPhase(int totalToRemove)
    {
        if (totalToRemove <= 0)
            return;
        gravityLocked = true;
        needsMatchCheck = false;
        pendingRemovals = totalToRemove;
    }


    public void UnlockGravity()
    {
        gravityLocked = false;
    }



    public Block FindBlockAtPosition(Vector3 position)
    {
        Vector2Int boardPos = BoardHelpers.WorldToBoard(position);
        return GetBlockAtPosition(boardPos.x, boardPos.y);
    }


    // 폭탄 블록 위치 가져오기
    public Vector2Int GetBlockPosition(Block block)
    {
        for (int x = 0; x < Width; x++)
        {
            for (int y = 0; y < Height; y++)
            {
                if (boardBlocks[x, y] == block)
                {
                    return new Vector2Int(x, y);
                }
            }
        }
        return new Vector2Int(-1, -1); // 찾지 못한 경우
    }


    public bool IsValidPosition(int x, int y)
    {
        return x >= 0 && x < width && y >= 0 && y < height;
    }


    private bool PerformFallMatchCheck()
    {
        currentMatches.Clear();
        FindThreeMatches();
        FindSquareMatches2x2();

        if (currentMatches.Count > 0)
        {
            bombSpawner.QueueBombsForCurrentMatches(null, null);
            BeginDestructionPhase();
        }

        needsMatchCheck = false;

        return currentMatches.Count > 0;
    }


    private void FindThreeMatches()
    {
        // 가로 매치 검사 (왼쪽부터 시작해서 중복 방지)
        for (int y = 0; y < Height; y++)
        {
            for (int x = 0; x < Width - 2; x++) // 마지막 2칸은 검사할 필요 없음
            {
                Block block1 = GetBlockAtPosition(x, y);
                Block block2 = GetBlockAtPosition(x + 1, y);
                Block block3 = GetBlockAtPosition(x + 2, y);

                // 폭탄이 하나라도 포함되면 매치로 취급하지 않음
                bool hasBomb = (block1 is BombBlock) || (block2 is BombBlock) || (block3 is BombBlock);
                if (!hasBomb &&
                    block1 != null && block2 != null && block3 != null &&
                    block1.IsMatchable && block2.IsMatchable && block3.IsMatchable &&
                    block1.BlockTag == block2.BlockTag && block2.BlockTag == block3.BlockTag)
                {
                    AddToMatches(block1, block2, block3);
                }
            }
        }

        // 세로 매치 검사 (아래부터 시작해서 중복 방지)
        for (int x = 0; x < Width; x++)
        {
            for (int y = 0; y < Height - 2; y++) // 마지막 2칸은 검사할 필요 없음
            {
                Block block1 = GetBlockAtPosition(x, y);
                Block block2 = GetBlockAtPosition(x, y + 1);
                Block block3 = GetBlockAtPosition(x, y + 2);

                // 폭탄이 하나라도 포함되면 매치로 취급하지 않음
                bool hasBomb = (block1 is BombBlock) || (block2 is BombBlock) || (block3 is BombBlock);
                if (!hasBomb &&
                    block1 != null && block2 != null && block3 != null &&
                    block1.IsMatchable && block2.IsMatchable && block3.IsMatchable &&
                    block1.BlockTag == block2.BlockTag && block2.BlockTag == block3.BlockTag)
                {
                    AddToMatches(block1, block2, block3);
                }
            }
        }
    }


    private void AddToMatches(Block block1, Block block2, Block block3)
    {
        if (!currentMatches.Contains(block1))
            currentMatches.Add(block1);
        if (!currentMatches.Contains(block2))
            currentMatches.Add(block2);
        if (!currentMatches.Contains(block3))
            currentMatches.Add(block3);
    }


    // 같은 태그 4개로 이루어진 2x2 정사각형 매치 스캔
    private void FindSquareMatches2x2()
    {
        for (int y = 0; y < Height - 1; y++)
        {
            for (int x = 0; x < Width - 1; x++)
            {
                var b00 = GetBlockAtPosition(x, y);
                var b10 = GetBlockAtPosition(x + 1, y);
                var b01 = GetBlockAtPosition(x, y + 1);
                var b11 = GetBlockAtPosition(x + 1, y + 1);

                if (b00 == null || b10 == null || b01 == null || b11 == null)
                    continue;

                // 폭탄 포함 제외
                if (b00 is BombBlock || b10 is BombBlock || b01 is BombBlock || b11 is BombBlock)
                    continue;

                // 상호작용 가능(Idle) 상태만 매치로 인정
                if (!b00.IsMatchable || !b10.IsMatchable || !b01.IsMatchable || !b11.IsMatchable)
                    continue;

                string tag = b00.BlockTag;
                if (string.IsNullOrEmpty(tag))
                    continue;

                if (b10.BlockTag == tag && b01.BlockTag == tag && b11.BlockTag == tag)
                {
                    if (!currentMatches.Contains(b00)) currentMatches.Add(b00);
                    if (!currentMatches.Contains(b10)) currentMatches.Add(b10);
                    if (!currentMatches.Contains(b01)) currentMatches.Add(b01);
                    if (!currentMatches.Contains(b11)) currentMatches.Add(b11);
                }
            }
        }
    }


    private void FindAndFallBlocks()
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                Block block = GetBlockAtPosition(x, y);
                if (block == null)
                {
                    var aboveBlock = FindFirstStaticBlockAbove(x, y);
                    if (aboveBlock != null)
                    {
                        UpdateBlockPosition(aboveBlock, aboveBlock.transform.localPosition, new Vector3(x, y, 0));
                        aboveBlock.StartFall(new Vector3(x, y, 0));
                        movingBlocks.Add(aboveBlock);
                    }
                }
            }
        }
    }


    // Block management methods
    private void SetBlockAtPosition(int x, int y, Block block)
    {
        if (IsValidPosition(x, y))
            boardBlocks[x, y] = block;
    }


    public Block GetBlockAtPosition(int x, int y)
    {
        if (IsValidPosition(x, y))
            return boardBlocks[x, y];
        return null;
    }


    private void RemoveBlockAtPosition(int x, int y)
    {
        if (IsValidPosition(x, y))
            boardBlocks[x, y] = null;
    }


    private Block FindFirstStaticBlockAbove(int x, int y)
    {
        // 보드 내에서 현재 위치 위쪽의 모든 칸을 순차적으로 검사
        for (int checkY = y + 1; checkY < height; checkY++)
        {
            Block block = GetBlockAtPosition(x, checkY);
            if (block != null)
                // 블록이 존재하는 경우 움직일 수 있는지 확인
                if (block.IsMovable)
                    return block; // 움직일 수 있는 블록을 찾으면 반환
                else
                    return null; // 움직일 수 없는 블록이 있으면 더 이상 진행하지 않음
        }

        // 보드 내에서 찾지 못한 경우, 스포너에서 새 블록 확인
        // x 좌표에 해당하는 스포너에서 현재 블록 확인
        var spawner = boardBlockSpawners[x];
        if (spawner != null)
        {
            // 스포너에서 새 블록을 가져옴
            var spawnedBlock = spawner.PopCurrentBlock(x, y);
            if (spawnedBlock != null && spawnedBlock.IsMovable)
                return spawnedBlock; // 생성된 블록 반환 (낙하 애니메이션에서 Idle 전환됨)
        }

        // 찾을 수 있는 블록이 없으면 null 반환
        return null;
    }


    private void BeginDestructionPhase()
    {
        gravityLocked = true;
        needsMatchCheck = false;

        var remaining = new HashSet<Block>(currentMatches);

        var excludeForDouble = bombSpawner.ConsumeBlocksUsedForDouble();
        if (excludeForDouble != null)
        {
            foreach (var b in excludeForDouble)
            {
                if (remaining.Contains(b))
                    remaining.Remove(b);
            }
        }
        pendingRemovals = remaining.Count;

        foreach (var block in remaining)
        {
            borderBlockDestroyer.DestroyOneBlock(block);
        }
    }


    private void HandleBlockRemoved(Vector2Int removedPos)
    {
        if (pendingRemovals <= 0)
            return;

        pendingRemovals--;
        if (pendingRemovals == 0)
        {
            // 모든 파괴가 끝났으므로 폭탄을 먼저 배치
            bool shouldUnlock = bombSpawner.SpawnQueuedBombs();

            if (shouldUnlock)
            {
                UnlockGravity();
            }
        }
    }
}

