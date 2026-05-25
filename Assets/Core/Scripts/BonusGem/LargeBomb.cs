using UnityEngine;

namespace MyMatch3
{
    /// <summary>
    /// 라지봄(LargeBomb) 보너스 젬.
    /// 자신을 중심으로 5×5 범위(반경 2칸) 안의 모든 보석을 한 번에 제거합니다.
    ///
    /// ■ 생성 조건 (GameSettings 인스펙터에서 Shapes로 설정)
    ///   → 5개 이상의 T자/L자 복합 매치 시 생성됩니다.
    ///
    /// ■ 발동 방법
    ///   → 보드 위 보석과 스왑하거나 더블클릭으로 발동합니다.
    ///   → UI 인벤토리에서 꺼내 사용할 수도 있습니다(isBonus=true).
    /// </summary>
    public class LargeBomb : BonusGem
    {
        [Tooltip("발동 시 재생할 효과음")]
        public AudioClip TriggerSound;

        [Tooltip("발동 시 발생할 폭발 이펙트 프리팹 (없으면 생략)")]
        public GameObject ExplosionEffectPrefab;

        // ──────────────────────────────────────────────────────────

        public override void Awake()
        {
            m_Usable = true;
        }

        /// <summary>
        /// 라지봄 발동 메서드.
        /// 자기 자신 처리 → 효과음/이펙트 재생 → 5×5 범위 보석 처리 순으로 실행합니다.
        /// </summary>
        /// <param name="swappedGem">스왑 상대 젬 (없으면 null)</param>
        /// <param name="isBonus">인벤토리 아이템 사용 여부 (true = 재귀 방지 불필요)</param>
        public override void Use(Gem swappedGem, bool isBonus = true)
        {
            // 5×5 범위 내 다른 LargeBomb의 연쇄 발동으로 인한 재귀 폭발 방지
            if (!isBonus && m_Used) return;
            m_Used = true;

            var board = GameManager.Instance.Board;
            var newMatch = board.CreateCustomMatch(m_CurrentIndex);
            newMatch.ForcedDeletion = true;  // 기본 팡 이펙트/사운드 생략

            // 자기 자신 처리
            // 인벤토리 아이템으로 사용 시 이 칸에 자신이 없을 수 있으므로 칸의 실제 내용을 읽습니다.
            HandleContent(board.CellContent[m_CurrentIndex], newMatch);

            // 효과음 + 폭발 이펙트 재생
            GameManager.Instance.PlaySFX(TriggerSound);
            SpawnExplosionEffect();

            // 5×5 범위 (x: -2~+2, y: -2~+2) 순회
            // 자기 자신(0,0)은 이미 처리했지만, HandleContent가 중복 처리를 내부적으로 막으므로 안전합니다.
            for (int x = -2; x <= 2; ++x)
            {
                for (int y = -2; y <= 2; ++y)
                {
                    var idx = m_CurrentIndex + new Vector3Int(x, y, 0);

                    if (board.CellContent.TryGetValue(idx, out var content) &&
                        content.ContainingGem != null)
                    {
                        HandleContent(content, newMatch);
                    }
                }
            }
        }

        // ──────────────────────────────────────────────────────────

        /// <summary>폭발 위치에 ExplosionEffectPrefab을 생성하고 2초 후 파괴합니다.</summary>
        private void SpawnExplosionEffect()
        {
            if (ExplosionEffectPrefab == null) return;

            var pos = GameManager.Instance.Board.GetCellCenter(m_CurrentIndex);
            var effect = Instantiate(ExplosionEffectPrefab, pos, Quaternion.identity);
            Destroy(effect, 2f);
        }
    }
}