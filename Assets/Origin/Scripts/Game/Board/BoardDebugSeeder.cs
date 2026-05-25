// OK
using UnityEngine;


[RequireComponent(typeof(BoardBlockManager))]
[RequireComponent(typeof(BoardCreator))]
[RequireComponent(typeof(BoardBombSpawner))]
public class BoardDebugSeeder : MonoBehaviour
{
    [SerializeField] private BoardDebugSeedAsset seedConfig;

    private BoardBlockManager board;
    private BoardBlockSpawner[] boardBlockSpawners;
    private BoardCreator borderCreator;
    private BoardBombSpawner bombSpawner;


    void Start()
    {
        board = GetComponent<BoardBlockManager>();
        boardBlockSpawners = GetComponentsInChildren<BoardBlockSpawner>();
        borderCreator = GetComponent<BoardCreator>();
        bombSpawner = GetComponent<BoardBombSpawner>();

        if (seedConfig != null)
        {
            SeedBottomRows();
            ApplyIceOverlay();
        }
    }


    [ContextMenu("Apply Seed (Bottom Rows)")]
    public void ApplySeedBottomRowsContextMenu()
    {
        SeedBottomRows();
        ApplyIceOverlay();
    }


    private void SeedBottomRows()
    {
        if (seedConfig == null || seedConfig.rows == null || seedConfig.rows.Length == 0) return;
        if (boardBlockSpawners == null || boardBlockSpawners.Length == 0) return;

        // 프리팹은 아무 스포너에서나 가져옴
        var prefab = boardBlockSpawners[0].Prefab;
        if (prefab == null) return;

        int rowsToSeed = Mathf.Min(seedConfig.rows.Length, board.Height);
        for (int row = 0; row < rowsToSeed; row++)
        {
            string line = seedConfig.rows[row] ?? string.Empty;
            for (int x = 0; x < board.Width; x++)
            {
                char ch = (x < line.Length) ? line[x] : '\0';
                if (char.IsWhiteSpace(ch) || ch == '\0' || ch == 'O')
                    continue; // 공백은 비움

                int y = row; // 최하단부터 채움

                // 기존 블럭 제거(있다면)
                var existing = board.GetBlockAtPosition(x, y);
                if (existing != null)
                {
                    // 즉시 제거
                    board.UnregisterBlock(new Vector3(x, y, 0));
                    Destroy(existing.gameObject);
                }

                // 정의 찾기 (ScriptableObject 키 기반)
                if (!TryGetDefinitionForChar(ch, out BlockSO def, out BombBlockSO bombDef))
                    continue; // 못 찾으면 스킵

                // 블럭 생성 및 등록 (폭탄/일반)
                if (bombDef != null && bombDef.Prefab != null)
                {
                    // 폭탄은 프리팹에서 생성
                    GameObject go = Instantiate(bombDef.Prefab, transform);
                    go.transform.localPosition = new Vector3(x, y, 0);
                    var bombBlock = go.GetComponent<BombBlock>();

                    bombDef.ApplyTo(bombBlock);
                    board.RegisterBlock(bombBlock, bombBlock.transform.localPosition);
                }
                else if (def != null)
                {
                    Block b = Instantiate(prefab, transform);
                    b.transform.localPosition = new Vector3(x, y, 0);
                    def.ApplyTo(b);
                    board.RegisterBlock(b, b.transform.localPosition);
                }
            }
        }
    }

    private void ApplyIceOverlay()
    {
        if (seedConfig == null || seedConfig.icePositions == null || seedConfig.icePositions.Length == 0) return;
        if (borderCreator == null) return;

        var icePrefab = borderCreator.IcePrefab;
        if (icePrefab == null)
        {
            Debug.LogWarning("Ice Prefab is not set on BoardCreator. Ice overlay skipped.");
            return;
        }

        foreach (var pos in seedConfig.icePositions)
        {
            // 범위 체크
            if (pos.x < 0 || pos.y < 0 || pos.x >= board.Width || pos.y >= board.Height) continue;

            var block = board.GetBlockAtPosition(pos.x, pos.y);
            if (block == null) continue; // 블럭이 없으면 스킵
            if (block.HasIce) continue;   // 이미 얼음이 있으면 스킵

            // 얼음 인스턴스 생성 및 부착
            var ice = Instantiate(icePrefab, block.transform);
            ice.transform.localPosition = Vector3.zero;
            block.AttachIce(ice);
        }
    }

    private bool TryGetDefinitionForChar(char ch, out BlockSO blockDef, out BombBlockSO bombDef)
    {
        blockDef = null;
        bombDef = null;

        // 1) 폭탄 정의 먼저 키로 검색
        var bombDefs = bombSpawner.GetAllBombDefinitions();
        if (bombDefs != null)
        {
            foreach (var bd in bombDefs)
            {
                if (bd == null) continue;
                if (bd.Key == ch)
                {
                    bombDef = bd;
                    return true;
                }
            }
        }

        // 2) 일반 블록 정의 키로 검색
        var defs = borderCreator.GetAllDefinitions();
        if (defs != null)
        {
            foreach (var d in defs)
            {
                if (d == null) continue;
                if (d.Key == ch)
                {
                    blockDef = d;
                    return true;
                }
            }
        }

        return false;
    }
}
