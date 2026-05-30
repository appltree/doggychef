using UnityEngine;
using UnityEngine.Tilemaps;


namespace DoggyChef
{
    /// <summary>
    /// 편집/로드 시에만 사용되는 타일로, 보석이 들어갈 셀을 정의합니다. 
    /// PlacedGem이 null이면 랜덤 보석으로 간주되어, 사용 가능한 목록에서 무작위로 하나가 선택됩니다.
    /// 이 타일은 편집 중에만 유용하며, 플레이 모드나 빌드에서는 시각적인 컴포넌트를 갖지 않습니다.
    /// </summary>
    [CreateAssetMenu(fileName = "New Gem Place Tile", menuName = "Match3/Gem Placer Tile")]
    public class GemPlacerTile : TileBase
    {
        public Sprite PreviewEditorSprite;
        [Tooltip("비워두면(null) 랜덤 보석이 됩니다")]
        public GemData PlacedGem = null;
        [Tooltip("Crate처럼 직접 프리팹을 배치해야 할 때 설정합니다. 설정 시 PlacedGem은 무시됩니다.")]
        public Gem GemPrefabOverride = null;

        public override void GetTileData(Vector3Int position, ITilemap tilemap, ref TileData tileData)
        {
            // 플레이 중이 아닐 때(즉, 플레이 모드 밖의 에디터 상에서)는 미리보기 스프라이트를 반환하고,
            // 그렇지 않으면(플레이 중이면) null을 반환하여 타일이 보이지 않게 합니다.
            // (보석은 타일맵이 아니라 우리 시스템에 의해 관리되는 게임 오브젝트입니다)
            tileData.sprite = !Application.isPlaying ? PreviewEditorSprite : null;
        }

        public override bool StartUp(Vector3Int position, ITilemap tilemap, GameObject go)
        {
#if UNITY_EDITOR
            if (!Application.isPlaying)
                return false;
#endif
            // 이 타일은 레벨 디자인을 돕기 위해 에디터에서만 사용됩니다.
            // 런타임에 우리는 보드에게 이 타일이 보석이 놓일 자리임을 알립니다.
            // 보드(Board)가 그곳에 보석을 생성하는 일을 담당할 것입니다.
            if (GemPrefabOverride != null)
            {
                Board.RegisterGemPrefab(position, GemPrefabOverride);
                return true;
            }

            Board.RegisterCell(position, PlacedGem);

            return base.StartUp(position, tilemap, go);
        }
    }
}
