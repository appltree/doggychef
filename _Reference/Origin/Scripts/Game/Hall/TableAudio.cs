using System.Collections.Generic;
using UnityEngine;

// 오디오 처리
public partial class TableAudio : MonoBehaviour
{
    [SerializeField] private AudioClip cookedSfx; // 요리 완료 시 재생할 사운드
    [SerializeField] private AudioClip sideMenuDeliveredSfx; // 사이드 메뉴 배달 시 재생할 사운드   

    private const float sfxDedupInterval = 0.2f; // 같은 소리 중복 방지 시간(초)
    private static readonly Dictionary<int, float> sfxLastPlayTimes = new(32);

    public void TryPlaySfx(AudioClip clip, Vector3 position)
    {
        if (clip == null) return;
        var now = Time.unscaledTime;
        int key = clip.GetInstanceID();

        if (sfxLastPlayTimes.TryGetValue(key, out var lastTime))
        {
            if (now - lastTime < sfxDedupInterval)
            {
                // 최근에 같은 클립을 재생했다면 무시
                return;
            }
        }

        sfxLastPlayTimes[key] = now;

        // 2D처럼 들리게 카메라 위치에서 재생, 카메라가 없으면 요청된 위치에서 재생
        AudioSource.PlayClipAtPoint(clip, Camera.main.transform.position, 0.9f);
    }

    // 요리 완료 사운드 재생
    public void PlayCookedSfx()
    {
        AudioSource.PlayClipAtPoint(cookedSfx, Camera.main.transform.position, 0.9f);
    }


    public void PlaySideMenuDeliveredSfx()
    {
        AudioSource.PlayClipAtPoint(sideMenuDeliveredSfx, Camera.main.transform.position, 0.9f);
    }

}
