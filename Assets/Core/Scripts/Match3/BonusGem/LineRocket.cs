using UnityEngine;

namespace DoggyChef
{
    /// <summary>
    /// 라인로켓(LineRocket) 보너스 젬.
    /// 발동 시 빔 이펙트 2개가 반대 방향으로 날아가며 경로의 모든 보석을 파괴합니다.
    ///
    /// ■ 생성 조건 (GameSettings 인스펙터에서 Shapes로 설정)
    ///   → 4개 직선 매치 시 생성됩니다.
    ///   → Vertical=false(가로형) 또는 Vertical=true(세로형) 두 종류의 프리팹이 필요합니다.
    ///
    /// ■ 발동 방법
    ///   → 보드 위 보석과 스왑하거나 더블클릭으로 발동합니다.
    /// </summary>
    public class LineRocket : BonusGem
    {
        [Tooltip("로켓 경로에 표시할 빔 이펙트 프리팹 (Origin BeamEffect 참고)")]
        public GameObject BeamEffectPrefab;

        [Tooltip("true = 세로 방향 / false = 가로 방향")]
        public bool Vertical;

        [Tooltip("발동 시 재생할 효과음")]
        public AudioClip TriggerSound;

        // ──────────────────────────────────────────────────────────

        public override void Awake()
        {
            m_Usable = true;
        }

        /// <summary>
        /// 라인로켓 발동 메서드.
        /// 자기 자신을 즉시 삭제하고, 양쪽 방향으로 RocketAction을 생성합니다.
        /// </summary>
        /// <param name="swappedGem">스왑 상대 젬 (없으면 null)</param>
        /// <param name="isBonus">인벤토리 아이템 사용 여부</param>
        public override void Use(Gem swappedGem, bool isBonus = true)
        {
            if (!isBonus && m_Used) return;
            m_Used = true;

            // Vertical=true → 위/아래, false → 왼쪽/오른쪽
            var dir = Vertical ? Vector3Int.up : Vector3Int.right;

            GameManager.Instance.PlaySFX(TriggerSound);

            // 자기 자신을 즉시 삭제 (ForcedDeletion=true → 기본 팡 이펙트 생략)
            var newMatch = GameManager.Instance.Board.CreateCustomMatch(m_CurrentIndex);
            newMatch.ForcedDeletion = true;
            HandleContent(GameManager.Instance.Board.CellContent[m_CurrentIndex], newMatch);

            // 양쪽 방향으로 RocketAction 발사
            var board = GameManager.Instance.Board;
            if (board.CellContent.ContainsKey(m_CurrentIndex + dir))
                board.AddNewBoardAction(new RocketAction(m_CurrentIndex, dir, BeamEffectPrefab));

            if (board.CellContent.ContainsKey(m_CurrentIndex - dir))
                board.AddNewBoardAction(new RocketAction(m_CurrentIndex, -dir, BeamEffectPrefab));
        }
    }

    // ════════════════════════════════════════════════════════════════
    //  RocketAction — 매 프레임 빔을 이동시키는 보드 액션
    // ════════════════════════════════════════════════════════════════
    //
    //  ■ 동작 흐름 (Board.Update → Tick() 매 프레임 호출)
    //    1. 현재 좌표(m_Position)를 MoveSpeed로 이동합니다.
    //    2. 이동한 좌표를 그리드 셀 인덱스로 변환합니다.
    //    3. 새 셀에 진입할 때마다 해당 칸의 보석을 파괴/연쇄 발동합니다.
    //    4. 보드 경계 도달 시 빔을 제거하고 false를 반환합니다.
    //       → Board가 false를 받으면 이 액션을 목록에서 제거합니다.
    // ════════════════════════════════════════════════════════════════
    class RocketAction : Board.IBoardAction
    {
        // ──────────────────────────────────────────────────────────
        //  상태
        // ──────────────────────────────────────────────────────────

        /// <summary>현재 로켓이 처리한 마지막 그리드 셀.</summary>
        protected Vector3Int m_CurrentCell;

        /// <summary>이동 방향 (Vector3Int.right / up 또는 그 반전).</summary>
        protected Vector3Int m_Direction;

        /// <summary>현재 월드 좌표.</summary>
        protected Vector3 m_Position;

        /// <summary>빔 이펙트 인스턴스.</summary>
        private GameObject m_Beam;

        /// <summary>이동 속도 (초당 5칸).</summary>
        private const float MoveSpeed = 5f;

        // ──────────────────────────────────────────────────────────
        //  생성자
        // ──────────────────────────────────────────────────────────

        /// <summary>
        /// RocketAction 생성자.
        /// 빔 이펙트를 생성하고 보드 이동 잠금을 겁니다.
        /// </summary>
        /// <param name="startCell">로켓 시작 셀</param>
        /// <param name="direction">이동 방향</param>
        /// <param name="beamEffectPrefab">빔 이펙트 프리팹 (null 이면 생략)</param>
        public RocketAction(Vector3Int startCell, Vector3Int direction, GameObject beamEffectPrefab = null)
        {
            m_CurrentCell = startCell;
            m_Direction = direction;

            // 로켓이 날아가는 동안 보드 낙하·스왑을 잠급니다.
            GameManager.Instance.Board.LockMovement();
            m_Position = GameManager.Instance.Board.GetCellCenter(m_CurrentCell);

            if (beamEffectPrefab != null)
                m_Beam = SpawnBeam(beamEffectPrefab, m_Position, direction);
        }

        // ──────────────────────────────────────────────────────────
        //  IBoardAction
        // ──────────────────────────────────────────────────────────

        /// <summary>
        /// Board.Update()에서 매 프레임 호출됩니다.
        /// </summary>
        /// <returns>true = 계속 진행 / false = 종료 (Board가 제거합니다)</returns>
        public bool Tick()
        {
            // 위치 이동 및 빔 동기화
            m_Position += (Vector3)m_Direction * (Time.deltaTime * MoveSpeed);
            if (m_Beam != null)
                m_Beam.transform.position = m_Position;

            // 현재 좌표를 그리드 셀로 변환
            var cell = GameManager.Instance.Board.WorldToCell(m_Position);

            // 새 셀로 진입할 때마다 처리 (한 프레임에 여러 칸을 건너뛸 수 있으므로 while)
            while (m_CurrentCell != cell)
            {
                m_CurrentCell += m_Direction;

                if (GameManager.Instance.Board.CellContent.TryGetValue(m_CurrentCell, out var content) &&
                    content.ContainingGem != null)
                {
                    ProcessCell(content);
                }

                // 다음 칸이 보드 밖이면 종료
                if (!GameManager.Instance.Board.CellContent.ContainsKey(m_CurrentCell + m_Direction))
                {
                    Cleanup();
                    return false;
                }
            }

            return true;
        }

        // ──────────────────────────────────────────────────────────
        //  내부 헬퍼
        // ──────────────────────────────────────────────────────────

        /// <summary>셀 내용에 따라 장애물 데미지, 보너스 연쇄, 일반 젬 파괴 중 하나를 실행합니다.</summary>
        private void ProcessCell(BoardCell content)
        {
            if (content.Obstacle != null)
            {
                // 장애물: 데미지 1
                content.Obstacle.Damage(1);
            }
            else if (content.ContainingGem.Usable)
            {
                // 보너스 젬: 연쇄 발동
                content.ContainingGem.Use(null);
            }
            else if (!content.ContainingGem.Damage(1))
            {
                // 일반 젬: 데미지 후 체력 0 → 강제 파괴
                GameManager.Instance.Board.DestroyGem(m_CurrentCell, true);
            }
        }

        /// <summary>빔 오브젝트를 파괴하고 보드 잠금을 해제합니다.</summary>
        private void Cleanup()
        {
            if (m_Beam != null) GameObject.Destroy(m_Beam);
            GameManager.Instance.Board.UnlockMovement();
        }

        /// <summary>
        /// 방향에 맞게 회전·반전된 빔 이펙트 인스턴스를 생성합니다.
        ///
        ///   right  →  0°
        ///   left   →  0° + flipX
        ///   up     →  90°
        ///   down   → -90°
        /// </summary>
        private static GameObject SpawnBeam(GameObject prefab, Vector3 pos, Vector3Int direction)
        {
            float rotZ = 0f;
            bool flipX = false;

            if (direction.y > 0) rotZ = 90f;
            else if (direction.y < 0) rotZ = -90f;
            else if (direction.x < 0) flipX = true;

            var beam = GameObject.Instantiate(prefab, pos, Quaternion.Euler(0f, 0f, rotZ));

            if (flipX)
            {
                var ls = beam.transform.localScale;
                ls.x = -ls.x;
                beam.transform.localScale = ls;
            }

            // Origin 프리팹의 SortingLayer가 MyGame에 없으므로 Gem 레이어로 보정합니다.
            foreach (var sr in beam.GetComponentsInChildren<SpriteRenderer>())
            {
                sr.sortingLayerID = 1890678711;  // MyGame Gem 레이어
                sr.sortingOrder = 15;          // 젬(10) 위에 표시
            }

            return beam;
        }
    }
}