using System.Collections.Generic;
using UnityEngine;

namespace DoggyChef
{
    /// <summary>
    /// MatchShape는 보석 매칭 시 비교할 '모양'을 정의하는 클래스입니다.
    /// 예를 들어 (0,0), (1,0), (2,0), (3,0) 좌표 리스트를 가지면 '가로 4칸' 모양이 됩니다.
    /// </summary>
    [System.Serializable]
    public class MatchShape : ISerializationCallbackReceiver
    {
        public bool CanMirror; // 좌우/상하 대칭 허용 여부
        public bool CanRotate; // 90도/180도/270도 회전 허용 여부

        // 모양을 구성하는 상대 좌표 리스트 (기준점은 0,0)
        public List<Vector3Int> Cells = new() { Vector3Int.zero };

        // 이 모양 전체를 감싸는 사각형 경계 (자동 계산됨)
        public RectInt Bounds = new RectInt(Vector2Int.zero, Vector2Int.zero);

        // 빠른 비교를 위해 미리 계산해두는 회전/대칭된 좌표 리스트들
        private List<Vector3Int> Cell90Rot = new();
        private List<Vector3Int> Cell180Rot = new();
        private List<Vector3Int> Cell270Rot = new();
        private List<Vector3Int> CellHMirror = new(); // 가로 대칭
        private List<Vector3Int> CellVMirror = new(); // 세로 대칭

        public void OnBeforeSerialize()
        {
            // 직렬화 전에는 특별히 할 작업 없음
        }

        // 인스펙터에서 값을 수정한 직후나 게임 시작 시 호출되어 데이터를 정리합니다.
        public void OnAfterDeserialize()
        {
            // 최소한 1개의 셀(0,0)은 있어야 함
            if (Cells.Count == 0)
            {
                Cells.Add(new Vector3Int(0, 0));
            }

            // 모양의 전체 크기(Bounds) 계산
            Bounds = GetBoundOf(Cells);

            // 회전 및 대칭된 버전의 좌표들을 미리 계산해서 리스트에 저장합니다.
            // 이렇게 하면 매 프레임마다 계산할 필요 없이 빠르게 비교할 수 있습니다.
            Cell90Rot.Clear();
            Cell180Rot.Clear();
            Cell270Rot.Clear();
            CellHMirror.Clear();
            CellVMirror.Clear();

            foreach (var cell in Cells)
            {
                // 90, 180, 270도 회전 계산
                GetRotation((Vector3Int)Bounds.min, cell, out var rot90, out var rot180, out var rot270);

                // 회전 후 좌표가 음수가 되지 않도록 보정하여 저장
                Cell90Rot.Add(rot90 + new Vector3Int(0, Bounds.width, 0));
                Cell180Rot.Add(rot180 + new Vector3Int(Bounds.width, Bounds.height, 0));
                Cell270Rot.Add(rot270 + new Vector3Int(Bounds.height, 0));

                // 가로 대칭 (좌우 반전)
                var x = Bounds.xMax - (cell.x - Bounds.xMin);
                CellHMirror.Add(new Vector3Int(x, cell.y, 0));

                // 세로 대칭 (상하 반전)
                var y = Bounds.yMax - (cell.y - Bounds.yMin);
                CellVMirror.Add(new Vector3Int(cell.x, y, 0));
            }
        }

        /// <summary>
        /// Board에서 매치가 발생했을 때 호출됩니다.
        /// "지금 매치된 보석들(cellList)이 이 모양(Shape)에 딱 들어맞는가?"를 검사합니다.
        /// </summary>
        /// <param name="cellList">현재 매치된 보석들의 좌표 리스트</param>
        /// <param name="matchedCells">모양이 일치한다면, 그 모양을 구성하는 핵심 셀들을 여기에 담아 리턴합니다.</param>
        /// <returns>모양이 일치하면 true, 아니면 false</returns>
        public bool FitIn(List<Vector3Int> cellList, ref List<Vector3Int> matchedCells)
        {
            // 매치된 보석들의 전체 영역 계산
            var targetBound = GetBoundOf(cellList);

            // 검사하려는 모양을 매치 영역 안에서 이리저리 움직이며 맞춰봅니다.
            var largestBoundSize = Mathf.Max(targetBound.width, targetBound.height);
            var smallestBoundSize = Mathf.Min(targetBound.width, targetBound.height);

            for (int y = targetBound.yMin; y <= targetBound.yMax - smallestBoundSize + 1; ++y)
            {
                for (int x = targetBound.xMin; x <= targetBound.xMax - smallestBoundSize + 1; ++x)
                {
                    List<Vector3Int> matchingCells = new();
                    List<Vector3Int> matching90Cells = new();
                    List<Vector3Int> matching180Cells = new();
                    List<Vector3Int> matching270Cells = new();
                    List<Vector3Int> matchingHMirrorCells = new();
                    List<Vector3Int> matchingVMirrorCells = new();

                    for (int iy = 0; iy <= largestBoundSize; ++iy)
                    {
                        for (int ix = 0; ix <= largestBoundSize; ++ix)
                        {
                            var normalShapeCell = new Vector3Int(ix + Bounds.xMin, iy + Bounds.yMin, 0);
                            var localCell = new Vector3Int(x + ix, y + iy, 0);

                            if (cellList.Contains(localCell))
                            {
                                if (Cells.Contains(normalShapeCell))
                                    matchingCells.Add(localCell);
                                if (Cell90Rot.Contains(normalShapeCell))
                                    matching90Cells.Add(localCell);
                                if (Cell180Rot.Contains(normalShapeCell)) matching180Cells.Add(localCell);
                                if (Cell270Rot.Contains(normalShapeCell)) matching270Cells.Add(localCell);
                                if (CellHMirror.Contains(normalShapeCell)) matchingHMirrorCells.Add(localCell);
                                if (CellVMirror.Contains(normalShapeCell)) matchingVMirrorCells.Add(localCell);
                            }
                        }
                    }

                    List<Vector3Int> usableList = null;
                    int count = Cells.Count;

                    if (matchingCells.Count == count) usableList = matchingCells;

                    if (usableList == null && CanRotate)
                    {
                        if (matching90Cells.Count == count) usableList = matching90Cells;
                        else if (matching180Cells.Count == count) usableList = matching180Cells;
                        else if (matching270Cells.Count == count) usableList = matching270Cells;
                    }

                    if (usableList == null && CanMirror)
                    {
                        if (matchingHMirrorCells.Count == count) usableList = matchingHMirrorCells;
                        else if (matchingVMirrorCells.Count == count) usableList = matchingVMirrorCells;
                    }

                    if (usableList != null)
                    {
                        foreach (var cell in usableList)
                        {
                            matchedCells.Add(cell);
                        }
                        return true;
                    }
                }
            }

            return false;
        }

        private void GetRotation(Vector3Int pivot, Vector3Int point,
            out Vector3Int rot90, out Vector3Int rot180, out Vector3Int rot270)
        {
            var toPoint = point - pivot;

            rot90 = new Vector3Int(toPoint.y, -toPoint.x, 0) + pivot;
            rot180 = new Vector3Int(-toPoint.x, -toPoint.y, 0) + pivot;
            rot270 = new Vector3Int(-toPoint.y, toPoint.x, 0) + pivot;
        }

        /// <summary>
        /// 주어진 셀 리스트를 모두 포함하는 최소 사각형(Rect)을 구합니다.
        /// </summary>
        public static RectInt GetBoundOf(List<Vector3Int> cellList)
        {
            if (cellList.Count == 0)
                return new RectInt(0, 0, 0, 0);

            RectInt rect = new RectInt(cellList[0].x, cellList[0].y, 0, 0);

            for (int i = 1; i < cellList.Count; ++i)
            {
                var cell = cellList[i];
                if (rect.xMin > cell.x) rect.xMin = cell.x;
                else if (rect.xMax < cell.x) rect.xMax = cell.x;

                if (rect.yMin > cell.y) rect.yMin = cell.y;
                else if (rect.yMax < cell.y) rect.yMax = cell.y;
            }

            return rect;
        }
    }
}
