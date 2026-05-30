using System.Collections.Generic;
using UnityEngine;

namespace DoggyChef
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
            else if (cell.ContainingGem.CurrentMatch == null && !cell.ContainingGem.Damage(1))
            {
                receivingMatch.AddGem(cell.ContainingGem);
            }
        }

        // 보드 위의 이펙트는 보통 보석이 파괴될 때(Destroyed) 나오지만,
        // 보너스 아이템을 사용해서 강제로 터뜨릴 때는 파괴 이벤트가 아닌 이 함수를 호출해 이펙트를 재생합니다.
        public void BonusTriggerEffect()
        {
            var position = GameManager.Instance.Board.GetCellCenter(m_CurrentIndex);

            foreach (var effectPrefab in MatchEffectPrefabs)
            {
                GameManager.Instance.PoolSystem.AddNewInstance(effectPrefab, 8);
                GameManager.Instance.PoolSystem.PlayInstanceAt(effectPrefab, position);
            }
        }
    }
}
