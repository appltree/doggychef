using UnityEngine;

public class BoardBlockSpawner : MonoBehaviour
{
    [SerializeField] private Block blockPrefab;
    [SerializeField] private Board board;
    public Block Prefab => blockPrefab;

    private BoardCreator boardCreator;
    private Block currentBlock;
    private BoardBlockManager boardBlockManager;

    private readonly float iceChance = 0.03f;

    // [Range(0f, 1f), Tooltip("필요 재료 가중치를 적용할 확률. 0이면 항상 균등 랜덤, 1이면 항상 필요도 기반 가중치 적용.")]
    private readonly float tableNeedBiasChance = 0.4f;
    // [Min(0f), Tooltip("필요 수량에 곱해지는 스케일. 0이면 가중치가 1(균등)로 수렴.")]
    private readonly float tableNeedWeightScale = 1.0f;


    void Awake()
    {
        board = FindFirstObjectByType<Board>();
        boardCreator = board.GetComponent<BoardCreator>();
        boardBlockManager = board.GetComponent<BoardBlockManager>();
    }


    void Start()
    {
        PreSpawnBlock();
    }


    public Block PopCurrentBlock(int targetX, int targetY)
    {
        if (currentBlock == null)
            PreSpawnBlock();

        if (currentBlock == null || !currentBlock.IsMovable)
            return null;

        // ScriptableObject 정의 기반 안전 선택 (Tag 기준)
        var defs = boardCreator.GetAllDefinitions();
        BlockSO chosenDef = ChooseSafeDefinition(targetX, targetY, defs);
        if (chosenDef == null)
            chosenDef = boardCreator.GetRandomSO();

        if (chosenDef != null)
        {
            chosenDef.ApplyTo(currentBlock);
            // 고유 수집 사운드 설정
            currentBlock.SetFoodSfx(chosenDef.FoodSfx);
        }

        // 아주 작은 확률로 얼음(아이싱) 적용
        TryApplyIce(currentBlock);

        Block blockToReturn = currentBlock;
        currentBlock = null;
        // 다음 호출을 위해 미리 생성
        PreSpawnBlock();
        return blockToReturn;
    }



    private void PreSpawnBlock()
    {
        currentBlock = Instantiate(blockPrefab, board.transform);
        currentBlock.transform.position = transform.position;
    }


    private void TryApplyIce(Block block)
    {
        if (block == null) return;
        // 이미 얼음이 있으면 중복 적용 금지
        var existingIce = block.GetComponentInChildren<Ice>();
        if (existingIce != null) return;
        if (Random.value > iceChance) return;

        var icePrefab = boardCreator.IcePrefab;
        if (icePrefab == null)
        {
            Debug.LogWarning("Ice Prefab is not set on BoardCreator.");
            return;
        }

        // 인스턴스 생성 및 블록 자식으로 부착
        Ice ice = Instantiate(icePrefab, block.transform);
        ice.transform.localPosition = Vector3.zero;
        block.AttachIce(ice);

    }


    private BlockSO ChooseSafeDefinition(int x, int y, BlockSO[] candidates)
    {
        // 입력 검증
        if (candidates == null || candidates.Length == 0)
            return null;

        // 1) 즉시 매치 유발하지 않는 안전 후보 수집
        var safe = new System.Collections.Generic.List<BlockSO>(candidates.Length);
        for (int i = 0; i < candidates.Length; i++)
        {
            var def = candidates[i];
            if (def == null) continue;
            if (!WouldCauseImmediateMatch(x, y, def.Tag))
                safe.Add(def);
        }
        if (safe.Count == 0) return null;

        // 2) 테이블들의 재료 필요도 수집 (1회 조회)
        var tables = FindObjectsByType<Table>(FindObjectsSortMode.None);
        var weightByTag = new System.Collections.Generic.Dictionary<string, int>();

        // 태그별 남은 필요 수량 합계 계산
        int GetTagWeight(string tag)
        {
            if (string.IsNullOrEmpty(tag)) return 0;
            if (weightByTag.TryGetValue(tag, out int cached)) return cached;
            int sum = 0;
            if (tables != null)
            {
                for (int i = 0; i < tables.Length; i++)
                {
                    var t = tables[i];
                    if (t == null) continue;
                    sum += Mathf.Max(0, t.GetRemainingNeed(tag));
                }
            }
            weightByTag[tag] = sum;
            return sum;
        }

        // 3) 가중치가 있는 후보를 우선 선택 (가중치 랜덤)
        int totalWeight = 0;
        var weighted = new System.Collections.Generic.List<(BlockSO def, int weight)>();
        bool useNeedBias = Random.value < Mathf.Clamp01(tableNeedBiasChance);
        for (int i = 0; i < safe.Count; i++)
        {
            var s = safe[i];
            // 확률적으로 필요도 가중치 사용. baseline 1을 더해 지나친 쏠림을 방지하고, 필요도 0도 최소 확률을 갖게 함
            if (useNeedBias)
            {
                int need = GetTagWeight(s.Tag);
                int w = 1 + Mathf.Max(0, Mathf.RoundToInt(need * Mathf.Max(0f, tableNeedWeightScale))); // 최소 1
                weighted.Add((s, w));
                totalWeight += w;
            }
        }
        BlockSO selected = null;

        if (totalWeight > 0 && weighted.Count > 0)
        {
            // 가중치 선택을 하되, 최종 반환 전 다시 안전성 검사 (보드 변화 대비)
            var pool = new System.Collections.Generic.List<(BlockSO def, int weight)>(weighted);
            int remaining = totalWeight;
            while (pool.Count > 0 && remaining > 0)
            {
                int r = Random.Range(0, remaining);
                int acc = 0;
                int chosenIndex = -1;
                for (int i = 0; i < pool.Count; i++)
                {
                    acc += pool[i].weight;
                    if (r < acc)
                    {
                        chosenIndex = i;
                        break;
                    }
                }
                if (chosenIndex < 0) break;

                var cand = pool[chosenIndex].def;
                if (!WouldCauseImmediateMatch(x, y, cand.Tag))
                {
                    selected = cand;
                    break;
                }
                // 제외하고 재시도
                remaining -= pool[chosenIndex].weight;
                pool.RemoveAt(chosenIndex);
            }
        }

        if (selected == null)
        {
            // 4) 모든 가중치가 0이거나 가중 후보가 모두 부적합하면 균등 랜덤으로 시도
            var pool = new System.Collections.Generic.List<BlockSO>(safe);
            while (pool.Count > 0)
            {
                int idx = Random.Range(0, pool.Count);
                var cand = pool[idx];
                if (!WouldCauseImmediateMatch(x, y, cand.Tag))
                {
                    selected = cand;
                    break;
                }
                pool.RemoveAt(idx);
            }
        }

        return selected; // 없으면 null 반환 → 상위에서 랜덤 대체(원 구현 유지)
    }


    // (x,y)에 candidateTag 블록을 놓았을 때 즉시 3매치 또는 2x2 매치가 발생하는지 검사
    private bool WouldCauseImmediateMatch(int x, int y, string candidateTag)
    {
        // 가로 검사: 좌2, 우2, 좌우1-1
        int sameLeft = 0;
        for (int dx = 1; dx <= 2; dx++)
        {
            var b = boardBlockManager.GetBlockAtPosition(x - dx, y);
            if (b != null && b.BlockTag == candidateTag) sameLeft++;
            else break;
        }
        int sameRight = 0;
        for (int dx = 1; dx <= 2; dx++)
        {
            var b = boardBlockManager.GetBlockAtPosition(x + dx, y);
            if (b != null && b.BlockTag == candidateTag) sameRight++;
            else break;
        }
        if (sameLeft >= 2 || sameRight >= 2 || (sameLeft >= 1 && sameRight >= 1))
            return true;

        // 세로 검사: 하2, 상2, 상하1-1
        int sameDown = 0;
        for (int dy = 1; dy <= 2; dy++)
        {
            var b = boardBlockManager.GetBlockAtPosition(x, y - dy);
            if (b != null && b.BlockTag == candidateTag) sameDown++;
            else break;
        }
        int sameUp = 0;
        for (int dy = 1; dy <= 2; dy++)
        {
            var b = boardBlockManager.GetBlockAtPosition(x, y + dy);
            if (b != null && b.BlockTag == candidateTag) sameUp++;
            else break;
        }

        if (sameDown >= 2 || sameUp >= 2 || (sameDown >= 1 && sameUp >= 1))
            return true;

        // 2x2 정사각형 매치 검사: (x,y)를 포함하는 네 가지 앵커를 확인
        // 앵커 (ax, ay)를 기준으로 {(ax,ay), (ax+1,ay), (ax,ay+1), (ax+1,ay+1)}가 모두 candidateTag이면 매치 성립
        bool Forms2x2(int ax, int ay)
        {
            // 네 칸 중 (x,y)는 이번에 놓일 후보이므로 자동으로 일치로 간주하고,
            // 나머지 세 칸이 모두 candidateTag인지 검사
            for (int ox = 0; ox <= 1; ox++)
            {
                for (int oy = 0; oy <= 1; oy++)
                {
                    int px = ax + ox;
                    int py = ay + oy;
                    if (px == x && py == y) continue; // 후보 위치는 일치로 간주
                    var nb = boardBlockManager.GetBlockAtPosition(px, py);
                    if (nb == null || nb.BlockTag != candidateTag)
                        return false;
                }
            }
            return true;
        }

        if (Forms2x2(x - 1, y - 1) ||
            Forms2x2(x - 1, y) ||
            Forms2x2(x, y - 1) ||
            Forms2x2(x, y))
        {
            return true;
        }

        return false;
    }

}
