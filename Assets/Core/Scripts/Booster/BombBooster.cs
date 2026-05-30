using UnityEngine;

namespace DoggyChef
{
    /// <summary>
    /// 폭탄 부스터 (game_ui_btn_bomb).
    ///
    /// ■ 동작
    ///   → 버튼 클릭 후 보드에서 원하는 칸을 선택하면 5×5 범위 보석을 파괴합니다.
    ///   → LargeBomb과 동일한 폭발 범위이지만, UI 인벤토리에서만 사용하는 전용 부스터입니다.
    ///   → 보드 클릭이 필요한 비즉시형(Board-target) 부스터입니다.
    ///
    /// ■ 설정
    ///   Inspector에서 TriggerSound를 설정하세요.
    ///   UISprite에 game_ui_btn_bomb 스프라이트를 연결하면 하단 바에 아이콘이 표시됩니다.
    /// </summary>
    public class BombBooster : Booster
    {
        [Tooltip("발동 시 재생할 효과음 (없으면 생략)")]
        public AudioClip TriggerSound;

        [Tooltip("발동 시 생성할 폭발 이펙트 프리팹 (LargeBomb과 동일한 프리팹 사용 권장)")]
        public GameObject ExplosionEffectPrefab;

        public override void Awake()
        {
            // 보드 위에 자연 생성되지 않고 인벤토리에서만 사용
            m_Usable = true;
        }

        /// <summary>보드 클릭이 필요한 비즉시형 부스터입니다.</summary>
        public override bool IsInstant => false;

        /// <summary>
        /// GameManager.UseBonusItem()이 보드 클릭 좌표에서 호출합니다.
        /// 자신을 포함한 5×5 범위(반경 2칸)의 보석을 파괴합니다.
        /// </summary>
        public override void Use(Gem swappedGem, bool isBonus = true)
        {
            if (!isBonus && m_Used) return;
            m_Used = true;

            var board = GameManager.Instance.Board;
            var newMatch = board.CreateCustomMatch(m_CurrentIndex);
            newMatch.ForcedDeletion = true;

            // 중심 셀 처리
            HandleContent(board.CellContent[m_CurrentIndex], newMatch);

            // 효과음 + 폭발 이펙트 (LargeBomb과 동일)
            GameManager.Instance.PlaySFX(TriggerSound);
            SpawnExplosionEffect();

            // 5×5 범위 (x: -2~+2, y: -2~+2) 순회
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
        private void SpawnExplosionEffect()
        {
            if (ExplosionEffectPrefab == null) return;
            var pos = GameManager.Instance.Board.GetCellCenter(m_CurrentIndex);
            var effect = Instantiate(ExplosionEffectPrefab, pos, Quaternion.identity);
            Destroy(effect, 2f);
        }
    }
}
