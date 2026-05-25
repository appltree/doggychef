using UnityEngine;
using UnityEngine.VFX;

namespace MyMatch3
{
    // 상자: 움직일 수 없는(CanMove=false) 젬.
    // 인접한 4방향에서 매치(삭제)가 발생하면 데미지를 입음.
    public class Crate : Gem
    {
        public bool CanBeDestroyedWithAdjacentMatch = true;
        public Sprite[] HealthStates; // 체력 상태별 스프라이트 (깨지는 순서)
        public int Health = 3;        // 체력

        public AudioClip DamagedClip;
        public VisualEffect DamageEffect;

        protected SpriteRenderer m_Renderer;

        private void Awake()
        {
            // 상자는 움직일 수 없음
            m_CanMove = false;
            m_Renderer = GetComponent<SpriteRenderer>();
            m_HitPoints = Health;
        }

        public override void Init(Vector3Int startIdx)
        {
            base.Init(startIdx);

            // 시각 효과 풀링 (나중에 구현되면 주석 해제)
            // if(DamageEffect != null)
            //    GameManager.Instance.PoolSystem.AddNewInstance(DamageEffect, 6);

            // 주변 4칸(상하좌우)에 삭제 감지 콜백(AdjacentMatch) 등록
            if (CanBeDestroyedWithAdjacentMatch)
            {
                foreach (var neighbour in BoardCell.Neighbours)
                {
                    var adjacentCell = startIdx + neighbour;
                    Board.RegisterDeletedCallback(adjacentCell, AdjacentMatch);
                }
            }
        }

        public override bool Damage(int damage)
        {
            // 사운드 재생
            //-- GameManager.Instance.PlaySFX(DamagedClip);

            // 이펙트 재생 (PoolSystem 연동 필요)
            // ...

            // 부모(Gem)의 데미지 처리 (HitPoints 감소)
            var ret = base.Damage(damage);

            // 상태 업데이트
            UpdateState();

            return ret;
        }

        private void OnDestroy()
        {
            // 파괴될 때 콜백 해제
            if (CanBeDestroyedWithAdjacentMatch)
            {
                foreach (var neighbour in BoardCell.Neighbours)
                {
                    var adjacentCell = m_CurrentIndex + neighbour;
                    Board.UnregisterDeletedCallback(adjacentCell, AdjacentMatch);
                }
            }
        }

        // 주변에서 매치가 발생했다는 알림을 받으면 호출됨
        void AdjacentMatch()
        {
            // 1의 데미지를 입힘. 
            // 만약 Damage가 false를 반환(=체력이 0이 됨)하면, 보드를 통해 자신을 파괴
            if (!Damage(1))
            {
                // 보드에 자신(Crate)을 파괴해달라고 요청
                GameManager.Instance.Board.DestroyGem(m_CurrentIndex);
            }
        }

        void UpdateState()
        {
            // 남은 체력 비율에 따라 스프라이트 교체
            float ratio = m_HitPoints / (float)Health;
            int state = Mathf.RoundToInt((1.0f - ratio) * HealthStates.Length);

            if (state >= 0 && state < HealthStates.Length)
                m_Renderer.sprite = HealthStates[state];
        }
    }
}