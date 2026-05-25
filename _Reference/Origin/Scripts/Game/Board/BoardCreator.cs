// OK
using UnityEngine;

[RequireComponent(typeof(BoardBlockManager))]
public class BoardCreator : MonoBehaviour
{
    [SerializeField] private Tile tilePrefab;
    [SerializeField] private BoardBlockSpawner blockSpawnerPrefab;

    [SerializeField] private BlockSO[] blockSOs;

    [SerializeField] private Ice icePrefab;

    [SerializeField] private Sprite tile0Sprite;
    [SerializeField] private Sprite tile1Sprite;

    private BoardBlockManager blockManager;

    // 외부에서 아이스 프리팹을 가져올 수 있도록 공개 접근자 제공
    public Ice IcePrefab => icePrefab;

    void Awake()
    {
        blockManager = GetComponent<BoardBlockManager>();
        CreateBoard();
    }


    private void CreateBoard()
    {
        CreateBlockSpawners();
        CreateTiles();
    }


    private void CreateBlockSpawners()
    {
        // BlockSpawners를 담을 빈 부모 오브젝트 생성
        GameObject spawnersParent = new("BlockSpawners");
        spawnersParent.transform.SetParent(transform);
        spawnersParent.transform.localPosition = Vector3.zero;

        for (int x = 0; x < blockManager.Width; x++)
        {
            BoardBlockSpawner blockSpawner = Instantiate(blockSpawnerPrefab, spawnersParent.transform);
            blockSpawner.transform.localPosition = new Vector3(x, blockManager.Height, 0);
            blockSpawner.name = $"BlockSpawner_{x}";
        }
    }


    private void CreateTiles()
    {
        // Tiles를 담을 빈 부모 오브젝트 생성
        GameObject tilesParent = new("Tiles");
        tilesParent.transform.SetParent(transform);
        tilesParent.transform.localPosition = Vector3.zero;

        for (int x = 0; x < blockManager.Width; x++)
        {
            for (int y = 0; y < blockManager.Height; y++)
            {
                Tile tile = Instantiate(tilePrefab, tilesParent.transform);
                tile.transform.localPosition = new Vector3(x, y, 0);
                tile.name = $"Tile_{x}_{y}";

                // 체스판 패턴: 스프라이트 번갈아 적용 (없으면 색상 패턴)
                SpriteRenderer sr = tile.GetComponentInChildren<SpriteRenderer>();
                if (sr != null)
                {
                    sr.sprite = ((x + y) % 2 == 0) ? tile0Sprite : tile1Sprite;
                    sr.color = Color.white; // 스프라이트 원색 유지
                    FitSpriteToCell(sr);

                }
            }
        }
    }

    // 타일 한 칸(1x1) 크기에 스프라이트가 꽉 차도록 스케일 조정
    private void FitSpriteToCell(SpriteRenderer sr)
    {
        if (sr == null || sr.sprite == null) return;
        // 단순 드로우 모드에서 스케일로 맞춤
        sr.drawMode = SpriteDrawMode.Simple;
        Vector2 spriteSize = sr.sprite.bounds.size; // 로컬 유닛 기준 크기
        if (spriteSize.x <= 0f || spriteSize.y <= 0f) return;
        sr.transform.localScale = new Vector3(1f / spriteSize.x, 1f / spriteSize.y, 1f);
    }


    // Optional: expose definitions for systems that want to use Tag too
    public BlockSO GetRandomSO()
    {
        if (blockSOs == null || blockSOs.Length == 0) return null;
        int idx = Random.Range(0, blockSOs.Length);
        return blockSOs[idx];
    }


    public BlockSO[] GetAllDefinitions()
    {
        return blockSOs;
    }
}
