// OK
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

[RequireComponent(typeof(BoardBlockManager))]
[RequireComponent(typeof(BoardBombExploder))]
[RequireComponent(typeof(BoardAIAssistant))]
public class BoardBlockInteraction : MonoBehaviour
{
    private BoardBlockManager blockManager;
    private BoardBombExploder bombExploder;

    // LightBall 선택 후 타겟 블록을 기다리는 모드 상태
    private bool isSelectingLightBallTarget = false;
    private LightBallBomb pendingLightBall = null;
    private Color selectionGlowColor = new(1f, 0.95f, 0.6f, 1f); // 따뜻한 하이라이트

    private readonly List<Block> selectionHighlightBlocks = new();

    // 외부에서 읽기-only로 접근할 수 있도록 공개 프로퍼티 제공
    public bool IsSelectingLightBallTarget => isSelectingLightBallTarget;


    void Awake()
    {
        blockManager = GetComponent<BoardBlockManager>();
        bombExploder = GetComponent<BoardBombExploder>();
    }


    void Start()
    {
        BlockController.OnBlockDragged += HandleBlockDragged;
        BlockController.OnBlockTouched += HandleBlockTouched;
    }


    void OnDestroy()
    {
        BlockController.OnBlockDragged -= HandleBlockDragged;
        BlockController.OnBlockTouched -= HandleBlockTouched;
        StopSelectionHighlight();
    }


    private void HandleBlockDragged(Block draggedBlock, DragDirection direction)
    {
        // LightBall 타겟 선택 모드에서는 드래그(스왑) 비활성화
        if (isSelectingLightBallTarget)
            return;

        if (!CanBlockBeInteractive(draggedBlock))
            return;

        Vector3 draggedPosition = draggedBlock.transform.localPosition;

        Vector2Int boardPosition = BoardHelpers.WorldToBoard(draggedPosition);
        if (!blockManager.IsValidPosition(boardPosition.x, boardPosition.y))
            return;

        Vector3 targetPosition = GetTargetPosition(draggedPosition, direction);

        // 타겟 위치에 있는 블록 찾기
        Block targetBlock = blockManager.FindBlockAtPosition(targetPosition);

        if (targetBlock == null)
            return;

        if (!CanBlockBeInteractive(targetBlock))
            return;

        if (!draggedBlock.IsInteractive || draggedBlock == targetBlock)
            return;

        // LightBall 끼리 스왑 금지
        var bb1 = draggedBlock as BombBlock;
        var bb2 = targetBlock as BombBlock;
        if (bb1 != null && bb2 != null &&
            bb1.GetBombType() == BombBlock.BombType.LightBall &&
            bb2.GetBombType() == BombBlock.BombType.LightBall)
        {
            return;
        }

        if (targetBlock != null && targetBlock.IsInteractive)
            StartCoroutine(SwapBlocksWithMatchCheck(draggedBlock, targetBlock));
    }



    private void HandleBlockTouched(Block block)
    {
        if (block == null) return;

        // LightBall 타겟 선택 모드일 때의 처리
        if (isSelectingLightBallTarget)
        {
            // 동일 LightBall을 다시 터치하면 선택 모드 해제
            if (pendingLightBall != null && ReferenceEquals(block, pendingLightBall))
            {
                pendingLightBall.StopFlash();
                isSelectingLightBallTarget = false;
                pendingLightBall = null;
                StopSelectionHighlight();
                return;
            }

            // 대상이 유효하고, 라이트볼 자신이 아니며, 매치 가능한 일반 블록이어야 함
            if (block != null && block != pendingLightBall && block.IsMatchable && block is not LightBallBomb)
            {
                // 모드 해제 후 폭발 트리거 (중복 입력 방지)
                var lightBall = pendingLightBall;
                isSelectingLightBallTarget = false;
                pendingLightBall = null;

                // 라이트볼이 아직 존재하고 상호작용 가능한 상태일 때만 실행
                if (lightBall != null && CanBlockBeInteractive(lightBall))
                {
                    StopSelectionHighlight();
                    StartCoroutine(bombExploder.TriggerLightBallExplosionWithTarget(lightBall, block));
                }
            }
            return; // 타겟 선택 모드에서는 여기서 처리 종료
        }

        if (!CanBlockBeInteractive(block))
            return;

        if (block.TryGetComponent<BombBlock>(out var bombBlock))
        {
            // LightBall이면 '타겟 블록 선택 모드'로 진입
            if (bombBlock.GetBombType() == BombBlock.BombType.LightBall)
            {
                // 보드 전체가 Idle일 때만 진입(안전장치)
                if (AreAllBlocksIdle())
                {

                    isSelectingLightBallTarget = true;
                    pendingLightBall = (LightBallBomb)bombBlock;
                    pendingLightBall.StartFlash();
                    BeginSelectionHighlight();
                }
            }

        }
    }


    private Vector3 GetTargetPosition(Vector3 currentPosition, DragDirection direction)
    {
        Vector3 targetPosition = currentPosition;

        switch (direction)
        {
            case DragDirection.Up:
                targetPosition.y += 1;
                break;
            case DragDirection.Down:
                targetPosition.y -= 1;
                break;
            case DragDirection.Left:
                targetPosition.x -= 1;
                break;
            case DragDirection.Right:
                targetPosition.x += 1;
                break;
        }

        return targetPosition;
    }


    private void SwapBlocks(Block block1, Block block2)
    {
        // 방어 코드: 이미 제거되었거나 파괴 중인 블록이면 스왑하지 않음
        if (block1 == null || block2 == null)
            return;

        // Update board array before swapping
        Vector2Int pos1 = BoardHelpers.WorldToBoard(block1.transform.localPosition);
        Vector2Int pos2 = BoardHelpers.WorldToBoard(block2.transform.localPosition);

        blockManager.UpdateBlockPosition(block1, block1.transform.localPosition, new Vector3(pos2.x, pos2.y, 0));
        blockManager.UpdateBlockPosition(block2, block2.transform.localPosition, new Vector3(pos1.x, pos1.y, 0));

        block1.StartSwap(new Vector3(pos2.x, pos2.y, 0));
        block2.StartSwap(new Vector3(pos1.x, pos1.y, 0));
    }


    private IEnumerator SwapBlocksWithMatchCheck(Block block1, Block block2)
    {
        // 원래 위치 저장
        Vector3 originalPos1 = block1.transform.localPosition;
        Vector3 originalPos2 = block2.transform.localPosition;

        // 블록 교체 실행
        SwapBlocks(block1, block2);

        if (block1 == null || block2 == null)
            yield break;

        SoundManager.I.PlayBlockSwap();

        // 블록이 이동을 완료할 때까지 대기 (null-세이프)
        yield return new WaitUntil(() =>
            block1 == null || block2 == null || (block1.IsMovable && block2.IsMovable)
        );


        // 스왑 중 폭탄이 포함된 경우, 매치 여부와 상관없이 즉시 폭발 트리거
        // 매칭 검사 (스왑된 블록을 전달하여 폭탄 중심 우선순위 부여)
        if (block1 != null && block2 != null && !blockManager.PerformSwapMatchCheck(block1, block2))
        {
            // 매칭이 없으면 원위치
            SwapBlocks(block1, block2);
        }
    }


    private bool CanBlockBeInteractive(Block blockToDestroy)
    {
        Vector3 blockPosition = blockToDestroy.transform.localPosition;
        Vector2Int boardPos = BoardHelpers.WorldToBoard(blockPosition);

        // BombBlock이면 폭발 범위 전체(행/열/십자)에 대해 위/아래 스택 안정성 검사
        if (blockToDestroy is BombBlock)
        {
            // 요구사항: BombBlock은 보드의 모든 블록이 정지(Idle) 상태일 때만 상호작용 가능
            return AreAllBlocksIdle();
        }

        // 일반 블록은 해당 칼럼만 검사(기존 로직)
        return IsStackStableAt(boardPos);
    }


    // 보드의 모든 블록이 Idle인지 확인
    private bool AreAllBlocksIdle()
    {
        for (int x = 0; x < blockManager.Width; x++)
        {
            for (int y = 0; y < blockManager.Height; y++)
            {
                var b = blockManager.GetBlockAtPosition(x, y);
                if (b == null || !b.IsMovable)
                    return false;
            }
        }
        return true;
    }


    // 주어진 보드 좌표에서 위/아래 스택이 비어있지 않고 모두 이동 가능(Idle 포함)한지 검사
    private bool IsStackStableAt(Vector2Int boardPos)
    {
        // 위쪽 검사
        for (int y = boardPos.y + 1; ; y++)
        {
            if (!blockManager.IsValidPosition(boardPos.x, y))
                break;

            Vector3 upperWorldPosition = new(boardPos.x, y, 0);
            Block upperBlock = blockManager.FindBlockAtPosition(upperWorldPosition);

            if (upperBlock == null)
                return false;
            if (!upperBlock.IsMovable)
                return false;
        }

        // 아래쪽 검사
        for (int y = boardPos.y - 1; ; y--)
        {
            if (!blockManager.IsValidPosition(boardPos.x, y))
                break;

            Vector3 lowerWorldPosition = new(boardPos.x, y, 0);
            Block lowerBlock = blockManager.FindBlockAtPosition(lowerWorldPosition);

            if (lowerBlock == null)
                return false;
            if (!lowerBlock.IsMovable)
                return false;
        }

        return true;
    }

    // 선택 가능 블록에 Glow Pulse 적용 시작
    private void BeginSelectionHighlight()
    {
        StopSelectionHighlight();

        for (int x = 0; x < blockManager.Width; x++)
        {
            for (int y = 0; y < blockManager.Height; y++)
            {
                var b = blockManager.GetBlockAtPosition(x, y);
                if (b == null) continue;
                if (!b.IsMatchable) continue; // Idle 필요
                if (b is BombBlock) continue; // 일반 블록만 대상
                if (pendingLightBall != null && ReferenceEquals(b, pendingLightBall)) continue;

                // Pulse 시작 (Block이 SpriteGlowEffect를 내부에서 처리)
                b.StartGlowPulse(selectionGlowColor);
                selectionHighlightBlocks.Add(b);
            }
        }
    }

    // 선택 가능 블록의 Glow Pulse 종료 및 정리
    private void StopSelectionHighlight()
    {
        if (selectionHighlightBlocks.Count == 0) return;
        for (int i = 0; i < selectionHighlightBlocks.Count; i++)
        {
            var b = selectionHighlightBlocks[i];
            if (b == null) continue;
            b.StopGlowPulse();
        }
        selectionHighlightBlocks.Clear();
    }
}
