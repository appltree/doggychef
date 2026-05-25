using UnityEngine;
using UnityEngine.VFX;

namespace MyMatch3
{
    // 젬 칸을 차지하며, 주변 매치 시 영향을 받는 모든 방해물의 기본 클래스입니다.
    public abstract class Obstacle : MonoBehaviour
    {
        [System.Serializable]
        public class LockStateData
        {
            public Sprite Sprite;
            public VisualEffect UndoneVFX;
        }

        // 상태별 스프라이트와 이펙트 (예: 2번 맞아야 깨지는 벽돌의 각 단계)
        public LockStateData[] LockState;

        protected SpriteRenderer m_SpriteRenderer;
        protected int m_CurrentState = 0;
        protected Vector3Int m_Cell;

        private bool m_Done = false;

        public virtual void Init(Vector3Int cell)
        {
            m_SpriteRenderer = GetComponent<SpriteRenderer>();

            // 데이터가 없으면 에러가 날 수 있으니 체크
            if (LockState != null && LockState.Length > 0 && LockState[0].Sprite != null)
                m_SpriteRenderer.sprite = LockState[0].Sprite;

            m_CurrentState = 0;
            m_Cell = cell;

            // Board에 자신을 등록
            Board.AddObstacle(cell, this);

            // 이펙트 풀링 준비 (GameManager.Instance.PoolSystem이 구현되면 사용)
            /*
            foreach (var state in LockState)
            {
                if(state.UndoneVFX != null)
                    GameManager.Instance.PoolSystem.AddNewInstance(state.UndoneVFX, 4);
            }
            */
        }

        public virtual void Clear()
        {
            // 상속받는 클래스에서 구현 (예: 점수 추가, 오브젝트 제거 등)
        }

        public void Damage(int amount)
        {
            if (ChangeState(m_CurrentState + amount))
            {
                Clear();
            }
        }

        protected bool ChangeState(int newState)
        {
            // 이미 파괴된 상태면 false
            if (m_Done)
                return false;

            m_CurrentState = newState;

            // 이전 단계의 파괴 이펙트 재생
            if (m_CurrentState - 1 >= 0 && m_CurrentState - 1 < LockState.Length)
            {
                // PoolSystem 연동 시 주석 해제
                // GameManager.Instance.PoolSystem.PlayInstanceAt(LockState[m_CurrentState - 1].UndoneVFX, transform.position);
            }

            // 아직 단계가 남았다면 스프라이트 변경
            if (m_CurrentState < LockState.Length)
            {
                m_SpriteRenderer.sprite = LockState[m_CurrentState].Sprite;
                return false;
            }

            // 모든 단계가 끝났으면 파괴 완료
            m_Done = true;
            return true;
        }
    }
}