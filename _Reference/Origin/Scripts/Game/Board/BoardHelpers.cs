using System.Collections.Generic;
using UnityEngine;

// 공용 보드 유틸리티: 중복 로직을 모아둔 헬퍼
public static class BoardHelpers
{
    // 보드 경계 체크
    public static bool InBounds(BoardBlockManager board, int x, int y)
    {
        return x >= 0 && y >= 0 && x < board.Width && y < board.Height;
    }

    // 체비쇼프 링의 유효 좌표 열거 (d == radius)
    public static IEnumerable<Vector2Int> ChebyshevRing(BoardBlockManager board, Vector2Int center, int radius)
    {
        int cx = center.x; int cy = center.y;
        if (radius == 0)
        {
            if (InBounds(board, cx, cy)) yield return new Vector2Int(cx, cy);
            yield break;
        }
        int minX = cx - radius; int maxX = cx + radius;
        int minY = cy - radius; int maxY = cy + radius;
        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                if (!InBounds(board, x, y)) continue;
                int cheb = Mathf.Max(Mathf.Abs(x - cx), Mathf.Abs(y - cy));
                if (cheb == radius)
                    yield return new Vector2Int(x, y);
            }
        }
    }

    // 맨해튼 거리 기준 버킷 분류
    public static Dictionary<int, List<Block>> BucketByManhattan(BoardBlockManager board, Vector2Int center, IEnumerable<Block> blocks)
    {
        var buckets = new Dictionary<int, List<Block>>();
        foreach (var b in blocks)
        {
            if (b == null) continue;
            var pos = board.GetBlockPosition(b);
            if (pos.x < 0) continue;
            int d = Mathf.Abs(pos.x - center.x) + Mathf.Abs(pos.y - center.y);
            if (!buckets.TryGetValue(d, out var list))
            {
                list = new List<Block>();
                buckets[d] = list;
            }
            list.Add(b);
        }
        return buckets;
    }


    // Convert world position to board coordinates
    public static Vector2Int WorldToBoard(Vector3 worldPosition)
    {
        int x = Mathf.RoundToInt(worldPosition.x);
        int y = Mathf.RoundToInt(worldPosition.y);
        return new Vector2Int(x, y);
    }

}
