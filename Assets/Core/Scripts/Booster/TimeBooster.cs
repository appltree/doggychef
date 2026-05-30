using UnityEngine;

namespace DoggyChef
{
    // 시간 부스터 (game_ui_btn_time).
    // 
    // ■ 동작
    //   → 버튼을 누르면 즉시 타이머에 <see cref="AddSeconds"/>초를 추가합니다.
    //   → 보드 클릭 없이 바로 발동되는 즉시형(Instant) 부스터입니다.
    //   → 타이머가 이미 종료된 경우(Closing/Result 상태)에는 발동되지 않습니다.
    // 
    // ■ 설정
    //   Inspector에서 AddSeconds와 TriggerSound를 설정하세요.
    //   UISprite에 game_ui_btn_time 스프라이트를 연결하면 하단 바에 아이콘이 표시됩니다.
    public class TimeBooster : Booster
    {
        [Tooltip("추가할 시간 (초). 기본값 30초.")]
        public float AddSeconds = 30f;

        [Tooltip("발동 시 재생할 효과음 (없으면 생략)")]
        public AudioClip TriggerSound;

        public override void Awake()
        {
            // 보드 위에 자연 생성되거나 스왑으로 사용되지 않음 — Usable=false
            m_Usable = false;
        }

        // 즉시형 부스터임을 UIHandler에 알립니다.
        public override bool IsInstant => true;

        // UIHandler가 버튼 클릭 시 직접 호출합니다.
        public override void UseInstant()
        {
            if (TriggerSound != null)
                GameManager.Instance.PlaySFXOneShot(TriggerSound);

            if (LevelData.Instance != null)
                LevelData.Instance.AddTime(AddSeconds);
        }
    }
}
