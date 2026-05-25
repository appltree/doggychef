using UnityEngine;

namespace MyMatch3
{
    public class TieBlocker : Obstacle
    {
        public override void Init(Vector3Int cell)
        {
            base.Init(cell);

            // we also register the cell as a normal "gem" cell so a gem is spawn under the blocker on start.
            Board.RegisterCell(cell);

            // 1. 이 셀을 잠가서(Lock) 젬이 이동하거나 떨어지지 못하게 함
            Board.ChangeLock(cell, true);

            // 2. 이 셀에서 매치가 발생하면 CellMatch 함수가 호출되도록 등록
            Board.RegisterMatchedCallback(cell, CellMatch);
        }

        public override void Clear()
        {
            // 파괴될 때: 콜백 해제, 잠금 해제, 오브젝트 삭제
            Board.UnregisterMatchedCallback(m_Cell, CellMatch);
            Board.ChangeLock(m_Cell, false);

            // 시각적 오브젝트 파괴
            Destroy(gameObject);
        }

        void CellMatch()
        {
            // 데미지를 입힘 (상태 1 증가). 
            // ChangeState가 true를 반환하면(파괴 완료 시) Clear 호출
            if (ChangeState(m_CurrentState + 1))
            {
                Clear();
            }
        }
    }
}