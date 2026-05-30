using UnityEngine;

namespace DoggyChef
{
    // 화면 비율에 따라 카메라 뷰포트를 조정합니다.
    // 16:9 미만: 레터박스(위아래), 20:9 초과: 필러박스(좌우), 그 사이: 전체 화면.
    [RequireComponent(typeof(Camera))]
    public class CameraLetterbox : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  상수
        // ════════════════════════════════════════════════════════════════

        private const float TargetAspect = 16f / 9f;
        private const float MaxAspect = 20f / 9f;

        // ════════════════════════════════════════════════════════════════
        //  Unity 생명 주기
        // ════════════════════════════════════════════════════════════════

        // 화면 비율을 계산하여 카메라 Rect를 설정합니다.
        private void Start()
        {
            var cam = GetComponent<Camera>();
            float windowAspect = (float)Screen.width / Screen.height;
            float scaleHeight = windowAspect / TargetAspect;

            if (scaleHeight < 1f)
            {
                // 16:9보다 세로가 긴 화면 — 위아래 레터박스
                cam.rect = new Rect(0f, (1f - scaleHeight) / 2f, 1f, scaleHeight);
            }
            else if (windowAspect > MaxAspect)
            {
                // 20:9보다 가로가 넓은 화면 — 좌우 필러박스
                float scaleWidth = MaxAspect / windowAspect;
                cam.rect = new Rect((1f - scaleWidth) / 2f, 0f, scaleWidth, 1f);
            }
            else
            {
                // 16:9 ~ 20:9 사이 — 전체 화면 사용
                cam.rect = new Rect(0f, 0f, 1f, 1f);
            }
        }
    }
}
