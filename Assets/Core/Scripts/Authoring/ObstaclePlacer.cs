using UnityEngine;
using UnityEngine.Tilemaps;

namespace MyMatch3
{
    /// <summary>
    /// 에디터에서 장애물을 배치하기 위한 타일입니다. 
    /// GemPlacer와 달리 Board에 등록하는 대신, StartUp에서 즉시 Obstacle 프리팹을 인스턴스화합니다.
    /// </summary>
    [CreateAssetMenu(fileName = "New Obstacle Placer", menuName = "Match3/Obstacle Placer")]
    public class ObstaclePlacer : TileBase
    {
        public Sprite PreviewEditorSprite;
        public Obstacle ObstaclePrefab;

        public override void GetTileData(Vector3Int position, ITilemap tilemap, ref TileData tileData)
        {
            // 플레이 모드가 아닐 때 프리뷰 표시
            tileData.sprite = !Application.isPlaying ? PreviewEditorSprite : null;
        }

        public override bool StartUp(Vector3Int position, ITilemap tilemap, GameObject go)
        {
#if UNITY_EDITOR
            if (!Application.isPlaying)
                return false;
#endif


            // 플레이 모드 시작 시 해당 위치에 장애물 생성
            var newObstacle = Instantiate(ObstaclePrefab);
            newObstacle.Init(position);

            return true;
        }
    }
}
