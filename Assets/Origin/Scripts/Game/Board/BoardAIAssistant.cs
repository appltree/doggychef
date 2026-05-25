// OK
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HintBlock
{
    public Block block;
    public Vector2Int direction;
    public Block targetBlock; // swap할 대상 블록
    public string matchedTag; // 이 스왑으로 만들어지는 매치 태그

    public HintBlock(Block block, Vector2Int direction, Block targetBlock, string matchedTag)
    {
        this.block = block;
        this.direction = direction;
        this.targetBlock = targetBlock;
        this.matchedTag = matchedTag;
    }
}

[RequireComponent(typeof(BoardBlockManager))]
[RequireComponent(typeof(BoardBlockInteraction))]
public class BoardAIAssistant : MonoBehaviour
{
    private static WaitForSeconds _waitForSeconds0_2 = new WaitForSeconds(0.2f);
    private readonly float hintDelayTime = 3.0f; // 힌트 표시 지연 시간
    private readonly float hintAnimationDistance = 0.3f; // 힌트 애니메이션 이동 거리
    private readonly float hintAnimationDuration = 0.8f; // 힌트 애니메이션 지속 시간

    private BoardBlockManager boardManager;
    private BoardBlockInteraction boardInteraction;

    // 힌트 관련
    [SerializeField] private List<HintBlock> hintBlocks = new();
    private readonly List<Coroutine> hintAnimations = new(); // 힌트 애니메이션 코루틴들
    private bool isShowingHint = false;

    private Block[,] clonedBlocks; // 가상 시뮬레이션용 블록 배열
    private Coroutine delayedHintCoroutine; // 지연된 힌트 표시 코루틴


    void Awake()
    {
        boardManager = GetComponent<BoardBlockManager>();
        boardInteraction = GetComponent<BoardBlockInteraction>();
    }


    void Start()
    {
        BlockController.OnPlayerAction += HandlePlayAction;
        BorderBlockDestroyer.OnBlockExploded += HandleBlockExploded;

    }

    void OnDestroy()
    {
        BlockController.OnPlayerAction -= HandlePlayAction;
        BorderBlockDestroyer.OnBlockExploded -= HandleBlockExploded;
    }


    // 모든 블록이 멈췄을 때 호출되는 메서드
    public void HandleCheck()
    {
        FindHintBlocks();
        if (hintBlocks.Count == 0)
        {
            DisableHintBlock();
            // 폭탄이 하나라도 있으면 전체 클리어를 하지 않는다
            if (!HasAnyBombOnBoard())
            {
                ClearAllBlocks();
            }
        }
        else
        {
            HandlePlayAction();
        }
    }



    private void HandleBlockExploded(Block block, Vector3 vector)
    {
        HandlePlayAction();
    }


    private void HandlePlayAction()
    {
        DisableHintBlock();
        if (delayedHintCoroutine != null)
            StopCoroutine(delayedHintCoroutine);
        delayedHintCoroutine = StartCoroutine(ShowHintAfterDelay());
    }


    // 지연된 힌트 표시 코루틴
    private IEnumerator ShowHintAfterDelay()
    {
        yield return new WaitForSeconds(hintDelayTime);
        ShowHint();
        delayedHintCoroutine = null;
    }


    private void ShowHint()
    {
        // LightBall 타겟 선택 중에는 힌트를 표시하지 않음
        if (boardInteraction.IsSelectingLightBallTarget)
            return;
        // 힌트가 없으면 조용히 반환 (Random.Range 에러 방지)
        if (hintBlocks == null || hintBlocks.Count == 0) return;
        // 랜덤한 힌트 선택
        int randomIndex = Random.Range(0, hintBlocks.Count);
        HintBlock selectedHint = hintBlocks[randomIndex];
        EnableHintBlock(selectedHint);
    }


    // 힌트 블록들을 찾는 메서드 (참고 코드 기반)
    private void FindHintBlocks()
    {
        // 현재 보드 상태를 복제
        CloneBoardBlocks();
        hintBlocks.Clear();

        for (int row = 0; row < boardManager.Height; row++)
        {
            for (int col = 0; col < boardManager.Width; col++)
            {
                // 오른쪽으로 swap 체크
                if (col < boardManager.Width - 1)
                {
                    string matchedName = SwapBlocksAndCheck(new Vector2Int(col, row), new Vector2Int(col + 1, row));
                    if (matchedName != null)
                    {
                        // 매칭되는 블록을 힌트로 추가
                        Block block1 = boardManager.GetBlockAtPosition(col, row);
                        Block block2 = boardManager.GetBlockAtPosition(col + 1, row);

                        if (block1 != null && block1.BlockTag == matchedName)
                        {
                            hintBlocks.Add(new HintBlock(block1, new Vector2Int(1, 0), block2, matchedName));
                        }
                        else if (block2 != null)
                        {
                            hintBlocks.Add(new HintBlock(block2, new Vector2Int(-1, 0), block1, matchedName));
                        }
                    }
                }

                // 위쪽으로 swap 체크
                if (row < boardManager.Height - 1)
                {
                    string matchedName = SwapBlocksAndCheck(new Vector2Int(col, row), new Vector2Int(col, row + 1));
                    if (matchedName != null)
                    {
                        Block block1 = boardManager.GetBlockAtPosition(col, row);
                        Block block2 = boardManager.GetBlockAtPosition(col, row + 1);

                        if (block1 != null && block1.BlockTag == matchedName)
                        {
                            hintBlocks.Add(new HintBlock(block1, new Vector2Int(0, 1), block2, matchedName));
                        }
                        else if (block2 != null)
                        {
                            hintBlocks.Add(new HintBlock(block2, new Vector2Int(0, -1), block1, matchedName));
                        }
                    }
                }
            }
        }

        // 주문 재료 우선 필터링
        if (hintBlocks.Count > 0)
        {
            var neededTags = CollectNeededOrderTags();
            if (neededTags.Count > 0)
            {
                bool anyPriority = false;
                for (int i = 0; i < hintBlocks.Count; i++)
                {
                    if (neededTags.Contains(hintBlocks[i].matchedTag)) { anyPriority = true; break; }
                }
                if (anyPriority)
                {
                    // 필요한 태그 매치만 남긴다
                    hintBlocks.RemoveAll(h => !neededTags.Contains(h.matchedTag));
                }
            }
        }
    }


    // 현재 Ordered 상태의 테이블에서 아직 남아있는 재료 태그 수집
    private HashSet<string> CollectNeededOrderTags()
    {
        HashSet<string> needed = new HashSet<string>();
        // 씬 상의 모든 Table 탐색 (HallManager 참조 불필요)
        Table[] tables = FindObjectsByType<Table>(FindObjectsSortMode.None);
        foreach (var t in tables)
        {
            if (t == null) continue;
            if (t.GetState() != Table.TableState.Ordered) continue;
            var order = t.GetCurrentOrder();
            if (order == null || order.Ingredients == null) continue;
            foreach (var ing in order.Ingredients)
            {
                if (ing.block == null) continue;
                string tag = ing.block.Tag;
                if (string.IsNullOrEmpty(tag)) continue;
                if (t.GetRemainingNeed(tag) > 0)
                    needed.Add(tag);
            }
        }
        return needed;
    }


    // 보드 블록들을 복제
    private void CloneBoardBlocks()
    {
        clonedBlocks = new Block[boardManager.Width, boardManager.Height];

        for (int x = 0; x < boardManager.Width; x++)
        {
            for (int y = 0; y < boardManager.Height; y++)
            {
                clonedBlocks[x, y] = boardManager.GetBlockAtPosition(x, y);
            }
        }
    }


    private string SwapBlocksAndCheck(Vector2Int pos1, Vector2Int pos2)
    {
        if (!SwapClonedBlocks(pos1, pos2))
        {
            return null;
        }

        string matchedName = GetMatchedBlockName();

        // 원래대로 되돌리기
        SwapClonedBlocks(pos1, pos2);

        return matchedName;
    }


    // 복제된 블록 배열에서 swap 수행
    private bool SwapClonedBlocks(Vector2Int pos1, Vector2Int pos2)
    {
        Block block1 = clonedBlocks[pos1.x, pos1.y];
        Block block2 = clonedBlocks[pos2.x, pos2.y];

        if (block1 == null || block2 == null)
            return false;

        if (!block1.IsInteractive || !block2.IsInteractive)
            return false;

        // 얼음이 있는 블록은 이동(스왑) 불가
        if (block1.HasIce || block2.HasIce)
            return false;

        // swap 수행
        clonedBlocks[pos1.x, pos1.y] = block2;
        clonedBlocks[pos2.x, pos2.y] = block1;

        return true;
    }


    // 매칭된 블록의 이름을 반환
    private string GetMatchedBlockName()
    {
        for (int row = 0; row < boardManager.Height; row++)
        {
            for (int col = 0; col < boardManager.Width; col++)
            {
                Block block = clonedBlocks[col, row];
                if (block == null) continue;

                // 2x2 정사각 매칭 체크
                if (col < boardManager.Width - 1 && row < boardManager.Height - 1)
                {
                    Block right = clonedBlocks[col + 1, row];
                    Block down = clonedBlocks[col, row + 1];
                    Block downRight = clonedBlocks[col + 1, row + 1];

                    if (right != null && down != null && downRight != null)
                    {
                        string tag = block.BlockTag;
                        if (!string.IsNullOrEmpty(tag) &&
                            tag == right.BlockTag &&
                            tag == down.BlockTag &&
                            tag == downRight.BlockTag)
                        {
                            return tag;
                        }
                    }
                }

                // 가로 매칭 체크 (3개 연속)
                if (col > 0 && col < boardManager.Width - 1)
                {
                    Block leftBlock = clonedBlocks[col - 1, row];
                    Block rightBlock = clonedBlocks[col + 1, row];

                    if (leftBlock != null && rightBlock != null &&
                        block.BlockTag == leftBlock.BlockTag &&
                        block.BlockTag == rightBlock.BlockTag)
                    {
                        return block.BlockTag;
                    }
                }

                // 세로 매칭 체크 (3개 연속)
                if (row > 0 && row < boardManager.Height - 1)
                {
                    Block bottomBlock = clonedBlocks[col, row - 1];
                    Block topBlock = clonedBlocks[col, row + 1];

                    if (bottomBlock != null && topBlock != null &&
                        block.BlockTag == bottomBlock.BlockTag &&
                        block.BlockTag == topBlock.BlockTag)
                    {
                        return block.BlockTag;
                    }
                }
            }
        }
        return null;
    }


    // 힌트 블록 활성화 (참고 코드 기반)
    private void EnableHintBlock(HintBlock hintBlock)
    {
        isShowingHint = true;

        // 힌트 블록에 방향 애니메이션 표시
        if (hintBlock.block != null)
        {
            StartHintAnimation(hintBlock.block, hintBlock.direction);
        }

        // 타겟 블록에 반대 방향 애니메이션 표시
        if (hintBlock.targetBlock != null)
        {
            Vector2Int oppositeDirection = -hintBlock.direction;
            StartHintAnimation(hintBlock.targetBlock, oppositeDirection);
        }
    }


    // 힌트 비활성화
    private void DisableHintBlock()
    {
        isShowingHint = false;

        // 지연된 힌트 코루틴이 있다면 중지
        if (delayedHintCoroutine != null)
        {
            StopCoroutine(delayedHintCoroutine);
            delayedHintCoroutine = null;
        }

        StopAllHintAnimations();
    }


    // 힌트 애니메이션 시작
    private void StartHintAnimation(Block block, Vector2Int direction)
    {
        if (block != null)
        {
            Coroutine animationCoroutine = StartCoroutine(HintDirectionAnimation(block, direction));
            hintAnimations.Add(animationCoroutine);
        }
    }


    // 모든 힌트 애니메이션 중지
    private void StopAllHintAnimations()
    {
        foreach (Coroutine animation in hintAnimations)
        {
            if (animation != null)
            {
                StopCoroutine(animation);
            }
        }
        hintAnimations.Clear();

        // 모든 블록을 원래 위치로 복원
        RestoreAllBlockPositions();
    }


    // 방향 힌트 애니메이션 코루틴
    private IEnumerator HintDirectionAnimation(Block block, Vector2Int direction)
    {
        Vector3 originalPosition = block.transform.localPosition;
        Vector3 targetOffset = new Vector3(direction.x, direction.y, 0) * hintAnimationDistance;
        Vector3 targetPosition = originalPosition + targetOffset;

        while (isShowingHint)
        {
            // 원래 위치에서 타겟 위치로 이동
            float elapsedTime = 0f;
            while (elapsedTime < hintAnimationDuration / 2f && isShowingHint)
            {
                float t = elapsedTime / (hintAnimationDuration / 2f);
                t = Mathf.SmoothStep(0f, 1f, t); // 부드러운 이동

                block.transform.localPosition = Vector3.Lerp(originalPosition, targetPosition, t);

                elapsedTime += Time.deltaTime;
                yield return null;
            }

            if (!isShowingHint) break;

            // 타겟 위치에서 원래 위치로 복귀
            elapsedTime = 0f;
            while (elapsedTime < hintAnimationDuration / 2f && isShowingHint)
            {
                float t = elapsedTime / (hintAnimationDuration / 2f);
                t = Mathf.SmoothStep(0f, 1f, t); // 부드러운 이동

                block.transform.localPosition = Vector3.Lerp(targetPosition, originalPosition, t);

                elapsedTime += Time.deltaTime;
                yield return null;
            }

            // 잠시 대기 후 반복
            yield return _waitForSeconds0_2;
        }

        // 애니메이션 종료 시 원래 위치로 복원
        block.transform.localPosition = originalPosition;
    }


    // 모든 블록 위치 복원
    private void RestoreAllBlockPositions()
    {
        for (int x = 0; x < boardManager.Width; x++)
        {
            for (int y = 0; y < boardManager.Height; y++)
            {
                Block block = boardManager.GetBlockAtPosition(x, y);
                if (block != null)
                {
                    // 블록을 정확한 그리드 위치로 복원
                    block.transform.localPosition = new Vector3(x, y, 0);
                }
            }
        }
    }


    // 모든 블록을 제거하는 메서드
    private void ClearAllBlocks()
    {
        Debug.LogWarning("Clearing all blocks with fade-out (no pang, no delivery) as no moves are available.");

        for (int x = 0; x < boardManager.Width; x++)
        {
            for (int y = 0; y < boardManager.Height; y++)
            {
                Block block = boardManager.GetBlockAtPosition(x, y);
                if (block == null) continue;

                // 파괴 애니메이션(페이드아웃)만 재생하고, 완료 후 보드에서 언레지스터
                Vector3 pos = new(x, y, 0);
                block.DestroyBlock(() =>
                {
                    boardManager.UnregisterBlock(pos);
                });
            }
        }
    }


    // 보드에 폭탄이 존재하는지 확인
    private bool HasAnyBombOnBoard()
    {
        for (int x = 0; x < boardManager.Width; x++)
        {
            for (int y = 0; y < boardManager.Height; y++)
            {
                Block block = boardManager.GetBlockAtPosition(x, y);
                if (block == null) continue;

                if (block is BombBlock)
                {
                    return true;
                }
            }
        }
        return false;
    }
}
