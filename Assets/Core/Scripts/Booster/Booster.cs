using System.Collections;
using UnityEngine;

namespace DoggyChef
{
    /// <summary>
    /// UI 인벤토리에서 사용하는 부스터 아이템의 기반 클래스.
    /// BonusGem(보드 위 특수 젬)과 달리, 플레이어가 직접 클릭해서 사용하는 아이템입니다.
    /// </summary>
    public class Booster : BonusGem
    {
        [Tooltip("부스터 발동(애니메이션 시작) 시 재생할 효과음.")]
        public AudioClip UseSound;

        [Tooltip("날아가기/던지기 애니메이션에서 사용할 스프라이트. null이면 UISprite를 사용합니다.")]
        public Sprite AnimSprite;

        [Tooltip("true면 버튼 클릭 시 아이콘이 BoosterPanel.m_FlyTarget 으로 날아가는 연출을 재생합니다.")]
        public bool UseFlyAnimation = false;

        public virtual bool IsInstant => false;
        public virtual void UseInstant() { }

        /// <summary>
        /// 즉시형 부스터의 시각 효과 코루틴. BoosterPanel이 StartCoroutine으로 실행합니다.
        /// 기본 구현: UseInstant()를 즉시 호출하고 종료합니다.
        /// 복수 타깃에 날아가는 연출 등 커스텀 효과가 필요하면 오버라이드하세요.
        /// </summary>
        /// <param name="startScreenPos">버튼 아이콘의 스크린 좌표 (연출 시작점).</param>
        /// <param name="canvas">UI 오브젝트를 생성할 Canvas.</param>
        /// <param name="runner">코루틴 실행자 (BoosterPanel MonoBehaviour).</param>
        public virtual IEnumerator InstantEffectRoutine(
            Vector3 startScreenPos, Canvas canvas, MonoBehaviour runner)
        {
            UseInstant();
            yield break;
        }
    }
}
