using UnityEngine;

[RequireComponent(typeof(Camera))]
public class CameraLetterbox : MonoBehaviour
{
    void Start()
    {
        Camera cam = GetComponent<Camera>();

        float targetAspect = 16.0f / 9.0f;   // 기준 비율 (16:9)
        float maxAspect = 20.0f / 9.0f;      // 최대 허용 비율 (20:9)
        float windowAspect = (float)Screen.width / (float)Screen.height;
        float scaleHeight = windowAspect / targetAspect;

        if (scaleHeight < 1.0f)
        {
            // Letterbox: 위아래 검은띠 (세로가 긴 화면)
            Rect rect = cam.rect;
            rect.width = 1.0f;
            rect.height = scaleHeight;
            rect.x = 0;
            rect.y = (1.0f - scaleHeight) / 2.0f;
            cam.rect = rect;
        }
        else if (windowAspect > maxAspect)
        {
            // Pillarbox: 좌우 검은띠 (20:9보다 넓은 화면에만 적용)
            float scaleWidth = maxAspect / windowAspect;
            Rect rect = cam.rect;
            rect.width = scaleWidth;
            rect.height = 1.0f;
            rect.x = (1.0f - scaleWidth) / 2.0f;
            rect.y = 0;
            cam.rect = rect;
        }
        else
        {
            // 16:9 ~ 20:9 사이는 전체 화면 사용
            cam.rect = new Rect(0, 0, 1, 1);
        }
    }
}
