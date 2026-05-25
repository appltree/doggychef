using UnityEngine;

namespace MyMatch3
{
    public class BoardCell
    {
        public static readonly Vector3Int[] Neighbours = {
            Vector3Int.up,
            Vector3Int.right,
            Vector3Int.down,
            Vector3Int.left,
        };

        public Gem ContainingGem;
        public Gem IncomingGem;
        public Obstacle Obstacle;

        // 이 칸이 보석을 받아들일 수있는 상태인지 판정
        public bool CanFall => (ContainingGem == null || (ContainingGem.CanMove && ContainingGem.CurrentMatch == null)) && !Locked;

        // 이 칸이 물리적으로 낙하를 막고 있는지 판정
        public bool BlockFall => Locked || (ContainingGem != null && !ContainingGem.CanMove);

        // 플레이어가 조작 가능한지 판정 (매치 중이거나 얼음에 덮인 보석은 조작 불가)
        public bool CanBeMoved => !Locked && ContainingGem != null && ContainingGem.CanMove
                               && ContainingGem.CurrentMatch == null && !ContainingGem.IsFrozen;

        public bool Locked = false;

        public bool CanMatch()
        {
            // 움직일 수 없는 젬(Crate 등)은 매치 대상에서 제외합니다.
            return ContainingGem != null && ContainingGem.IsMovable;
        }

        public bool CanDelete()
        {
            return !Locked;
        }

        public bool IsEmpty()
        {
            return ContainingGem == null && IncomingGem == null;
        }
    }
}