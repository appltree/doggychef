using UnityEngine;

namespace DoggyChef
{
    /// <summary>
    /// 새로고침 부스터 (game_ui_btn_refresh).
    ///
    /// ■ 동작
    ///   → 버튼을 누르면 즉시 보드의 모든 일반 보석 타입을 무작위로 재배치합니다.
    ///   → BonusGem은 재배치 대상에서 제외됩니다.
    ///   → 보석 오브젝트는 유지되고 타입/스프라이트만 교체됩니다(성능 최적화).
    ///   → 보드 클릭 없이 바로 발동되는 즉시형(Instant) 부스터입니다.
    ///
    /// ■ 설정
    ///   Inspector에서 TriggerSound를 설정하세요.
    ///   UISprite에 game_ui_btn_refresh 스프라이트를 연결하면 하단 바에 아이콘이 표시됩니다.
    /// </summary>
    public class RefreshBooster : Booster
    {
        [Tooltip("발동 시 재생할 효과음 (없으면 생략)")]
        public AudioClip TriggerSound;

        public override void Awake()
        {
            // 보드 위에 자연 생성되거나 스왑으로 사용되지 않음 — Usable=false
            m_Usable = false;
        }

        /// <summary>즉시형 부스터임을 UIHandler에 알립니다.</summary>
        public override bool IsInstant => true;

        /// <summary>UIHandler가 버튼 클릭 시 직접 호출합니다.</summary>
        public override void UseInstant()
        {
            if (TriggerSound != null)
                GameManager.Instance.PlaySFXOneShot(TriggerSound);

            if (GameManager.Instance?.Board != null)
                GameManager.Instance.Board.ShuffleGems();
        }
    }
}
