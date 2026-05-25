using UnityEngine;
using UnityEngine.Tilemaps;

namespace MyMatch3
{
    /// <summary>
    /// 이것은 편집 시간용 타일로, 그 아래쪽으로 셀(보석)을 스폰(생성)하게 될 위치를 정의하는 데 사용됩니다.
    /// 편집 시간과 플레이 모드 밖에서는 미리보기 스프라이트가 타일맵에 표시되지만,
    /// 런타임에는 게임 로직상의 배치일 뿐이므로 타일이 비어 있게 됩니다.
    /// </summary>
    [CreateAssetMenu(fileName = "New Gem Spawner Tile", menuName = "Match3/Gem Spawner Placer")]
    public class GemSpawner : TileBase
    {
        public Sprite EditorPreviewSprite;

        public override void GetTileData(Vector3Int position, ITilemap tilemap, ref TileData tileData)
        {
            tileData.sprite = !Application.isPlaying ? EditorPreviewSprite : null;
        }

        public override bool StartUp(Vector3Int position, ITilemap tilemap, GameObject go)
        {
#if UNITY_EDITOR
            if (!Application.isPlaying)
                return false;
#endif

            // 이 타일은 레벨 디자인을 돕기 위해 에디터에서만 사용됩니다.
            // 런타임에 우리는 보드에게 이 타일이 스포너(생성기)임을 알립니다.
            // 보드(Board)가 그곳에서 보석이 생성되도록 처리할 것입니다.
            Board.RegisterSpawner(position);

            return base.StartUp(position, tilemap, go);
        }
    }
}
