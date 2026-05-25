using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace MyMatch3
{
    /// <summary>
    /// BonusGem은 일반 보석과 달리 특별한 능력과 '모양(Shape)' 데이터를 가진 보석입니다.
    /// GameManager의 게임 설정(Game Settings)에 등록되어 관리됩니다.
    /// 플레이어가 보석을 매칭했을 때, 시스템은 모든 BonusGem을 순회하며
    /// "지금 만들어진 매치 모양이 이 보석의 모양 조건(Shape)과 일치하는가?"를 검사합니다.
    /// 만약 일치하면 해당 위치에 이 보석을 소환합니다.
    /// </summary>
    public class BonusGem : Gem
    {
        // 이 보너스 젬이 만들어지기 위한 조건 모양들 (예: 일자형 4개, 사각형, T자형 등)
        public List<MatchShape> Shapes;

        public virtual void Awake() { }

        // 자식 클래스(LineRocket, Bomb 등)가 보석 파괴 로직을 구현할 때 사용하는 도우미 함수입니다.
        // 장애물(Obstacle)이나 일반 보석을 안전하게 처리하고, 점수나 매치에 반영합니다.
        protected void HandleContent(BoardCell cell, Match receivingMatch)
        {
            // 1. 장애물이 있다면 데미지를 줍니다.
            if (cell.Obstacle != null)
            {
                cell.Obstacle.Damage(1);
            }

            // 2. 보석이 없다면 패스
            if (cell.ContainingGem == null)
                return;

            // 3. 만약 다른 '사용 가능한' 아이템(BonusGem 등)이라면 연쇄 발동시킵니다.
            if (cell.ContainingGem.Usable && !cell.ContainingGem.Used)
            {
                cell.ContainingGem.Use(null);
            }
            // 4. 일반 보석이고 아직 다른 매치에 안 속해 있다면, 폭발 매치(receivingMatch)에 포함시킵니다.
            //    (이미 깨지는 중인 보석은 건드리지 않습니다)
            else if (cell.ContainingGem.CurrentMatch == null && !cell.ContainingGem.Damage(1))
            {
                receivingMatch.AddGem(cell.ContainingGem);
            }
        }

        // 보드 위의 이펙트는 보통 보석이 파괴될 때(Destroyed) 나오지만,
        // 보너스 아이템을 사용해서 강제로 터뜨릴 때는 파괴 이벤트가 아닌 이 함수를 호출해 이펙트를 재생합니다.
        public void BonusTriggerEffect()
        {
            // 현재 보석 위치를 월드 좌표로 가져옵니다.
            var position = GameManager.Instance.Board.GetCellCenter(m_CurrentIndex);

            foreach (var effectPrefab in MatchEffectPrefabs)
            {
                // 일반적으로 보너스 젬이 처음 생성될 때 이펙트 풀이 초기화되지만,
                // 스폰되기도 전에 아이템으로 강제 사용될 경우를 대비해 풀(Pool)을 먼저 채워둡니다.
                GameManager.Instance.PoolSystem.AddNewInstance(effectPrefab, 8);

                // 해당 위치에서 이펙트를 재생합니다.
                GameManager.Instance.PoolSystem.PlayInstanceAt(effectPrefab, position);
            }
        }
    }

    /// <summary>
    /// UI 인벤토리에서 사용하는 부스터 아이템의 기반 클래스.
    /// BonusGem(보드 위 특수 젬)과 달리, 플레이어가 직접 클릭해서 사용하는 아이템입니다.
    /// </summary>
    public class Booster : BonusGem
    {
        [Tooltip("부스터 발동(애니메이션 시작) 시 재생할 효과음.")]
        public AudioClip UseSound;

        [Tooltip("날아가기/던지기 애니메이션에서 사용할 스프라이트. null이면 UISprite를 사용합니다.")]
        public Sprite AnimSprite;

        [Tooltip("true면 버튼 클릭 시 아이콘이 BoosterPanel.m_FlyTarget 으로 날아가는 연출을 재생합니다.")]
        public bool UseFlyAnimation = false;

        public virtual bool IsInstant => false;
        public virtual void UseInstant() { }

        /// <summary>
        /// 즉시형 부스터의 시각 효과 코루틴. BoosterPanel이 StartCoroutine으로 실행합니다.
        /// 기본 구현: UseInstant()를 즉시 호출하고 종료합니다.
        /// 복수 타깃에 날아가는 연출 등 커스텀 효과가 필요하면 오버라이드하세요.
        /// </summary>
        /// <param name="startScreenPos">버튼 아이콘의 스크린 좌표 (연출 시작점).</param>
        /// <param name="canvas">UI 오브젝트를 생성할 Canvas.</param>
        /// <param name="runner">코루틴 실행자 (BoosterPanel MonoBehaviour).</param>
        public virtual IEnumerator InstantEffectRoutine(
            Vector3 startScreenPos, Canvas canvas, MonoBehaviour runner)
        {
            UseInstant();
            yield break;
        }
    }

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
            // (Shape의 크기만큼 여유를 두고 루프를 돕니다)
            var largestBoundSize = Mathf.Max(targetBound.width, targetBound.height);
            var smallestBoundSize = Mathf.Min(targetBound.width, targetBound.height);

            for (int y = targetBound.yMin; y <= targetBound.yMax - smallestBoundSize + 1; ++y)
            {
                for (int x = targetBound.xMin; x <= targetBound.xMax - smallestBoundSize + 1; ++x)
                {
                    // 각 변형(회전/대칭)별로 일치하는 셀들을 임시 저장할 리스트
                    List<Vector3Int> matchingCells = new();
                    List<Vector3Int> matching90Cells = new();
                    List<Vector3Int> matching180Cells = new();
                    List<Vector3Int> matching270Cells = new();
                    List<Vector3Int> matchingHMirrorCells = new();
                    List<Vector3Int> matchingVMirrorCells = new();

                    // 모양의 크기만큼 순회하며 셀이 있는지 확인
                    for (int iy = 0; iy <= largestBoundSize; ++iy)
                    {
                        for (int ix = 0; ix <= largestBoundSize; ++ix)
                        {
                            var normalShapeCell = new Vector3Int(ix + Bounds.xMin, iy + Bounds.yMin, 0);
                            var localCell = new Vector3Int(x + ix, y + iy, 0);

                            if (cellList.Contains(localCell))
                            {
                                // 기본 모양과 일치하는가?
                                if (Cells.Contains(normalShapeCell))
                                    matchingCells.Add(localCell);

                                // 90도 회전 모양과 일치하는가?
                                if (Cell90Rot.Contains(normalShapeCell))
                                    matching90Cells.Add(localCell);

                                // ... (나머지 회전/대칭 확인)
                                if (Cell180Rot.Contains(normalShapeCell)) matching180Cells.Add(localCell);
                                if (Cell270Rot.Contains(normalShapeCell)) matching270Cells.Add(localCell);
                                if (CellHMirror.Contains(normalShapeCell)) matchingHMirrorCells.Add(localCell);
                                if (CellVMirror.Contains(normalShapeCell)) matchingVMirrorCells.Add(localCell);
                            }
                        }
                    }

                    // 일치하는 셀의 개수가 모양(Cells)의 전체 개수와 같은지 확인
                    List<Vector3Int> usableList = null;
                    int count = Cells.Count;

                    // 1. 기본 모양 확인
                    if (matchingCells.Count == count) usableList = matchingCells;

                    // 2. 회전 허용 시 회전 모양 확인
                    if (usableList == null && CanRotate)
                    {
                        if (matching90Cells.Count == count) usableList = matching90Cells;
                        else if (matching180Cells.Count == count) usableList = matching180Cells;
                        else if (matching270Cells.Count == count) usableList = matching270Cells;
                    }

                    // 3. 대칭 허용 시 대칭 모양 확인
                    if (usableList == null && CanMirror)
                    {
                        if (matchingHMirrorCells.Count == count) usableList = matchingHMirrorCells;
                        else if (matchingVMirrorCells.Count == count) usableList = matchingVMirrorCells;
                    }

                    // 일치하는 모양을 찾았다면 결과 리스트에 담고 true 반환
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

            // 아무 모양도 맞지 않음
            return false;
        }

        // 회전된 좌표를 계산하는 헬퍼 함수
        void GetRotation(Vector3Int pivot, Vector3Int point,
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