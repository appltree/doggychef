using UnityEngine;
using UnityEngine.Audio;
using System.Collections.Generic;

/// <summary>
/// 경량/범용 SFX 매니저
/// - 권장 패턴: AudioClip 직렬화 + 소수의 AudioSource 풀 + PlayOneShot
/// - 장점: 씬마다 다수의 AudioSource를 유지하지 않아 메모리/관리 부담 감소, 중첩 재생 안전
/// - 기존 API(PlayXXX) 유지하여 레거시 호출 호환
/// </summary>
public class SoundManager : MonoBehaviour
{
    // Singleton (씬 전환 유지)
    public static SoundManager I { get; private set; }

    [Header("Mixer (Optional)")]
    [SerializeField] private AudioMixerGroup sfxMixerGroup;

    [Header("Pool Settings")]
    [SerializeField] private int poolSize = 8; // 동시 재생 여유 수
    [SerializeField, Range(0f, 1f)] private float sfxVolume = 1f;
    [Tooltip("피치 랜덤 변조 기본값 (재생 시 가벼운 변조)")]
    [SerializeField] private bool defaultPitchVary = true;
    [SerializeField, Range(0.5f, 1.5f)] private float pitchMin = 0.9f;
    [SerializeField, Range(0.5f, 1.5f)] private float pitchMax = 1.1f;
    [Tooltip("같은 클립은 동시에 겹치지 않게 함 (재생 중이면 무시)")]
    [SerializeField] private bool preventSameClipOverlap = true;
    [Tooltip("같은 클립 연속 재생 최소 간격(초). 0이면 제한 없음")]
    [SerializeField] private float sameClipMinInterval = 0.1f;

    [Header("Ingredient Clips (index 기반)")]
    [SerializeField] private List<AudioClip> ingredientClips = new List<AudioClip>();

    [Header("Named SFX Clips")]
    [SerializeField] private AudioClip blockPangClip;
    [SerializeField] private AudioClip blockSwapClip;
    [SerializeField] private AudioClip blockSwapBackClip;
    [SerializeField] private AudioClip fireClip;
    [SerializeField] private AudioClip bombClip;

    [SerializeField] private AudioClip iceBreakClip;
    [SerializeField] private AudioClip chainBreakClip;
    [SerializeField] private AudioClip haloClip;
    [SerializeField] private AudioClip earnGoldClip;
    [SerializeField] private AudioClip earnGoldByCashierClip;

    // 내부 소스 풀
    private readonly List<AudioSource> _pool = new List<AudioSource>();
    private int _nextIndex;
    private readonly Dictionary<AudioClip, float> _clipLastPlayed = new Dictionary<AudioClip, float>(); // clip -> lastPlayedUnscaledTime

    private void Awake()
    {
        if (I != null && I != this)
        {
            // 중복 인스턴스 방지
            Destroy(gameObject);
            return;
        }
        I = this;
        DontDestroyOnLoad(gameObject);

        BuildPool();
    }

    private void BuildPool()
    {
        _pool.Clear();

        // 풀 보관용 자식 노드 (정리 목적)
        var holder = new GameObject("SFX_Sources");
        holder.transform.SetParent(transform, false);

        for (int i = 0; i < Mathf.Max(1, poolSize); i++)
        {
            var src = holder.AddComponent<AudioSource>();
            src.playOnAwake = false;
            src.loop = false;
            src.spatialBlend = 0f; // UI/2D SFX 기본
            src.volume = 1f;
            src.outputAudioMixerGroup = sfxMixerGroup;
            _pool.Add(src);
        }
        _nextIndex = 0;
    }

    private AudioSource GetSource()
    {
        // 1) 먼저 비어있는 소스 탐색
        for (int i = 0; i < _pool.Count; i++)
        {
            if (!_pool[i].isPlaying) return _pool[i];
        }

        // 2) 모두 재생 중이면 라운드로빈으로 재사용 (PlayOneShot은 믹싱 가능)
        var src = _pool[_nextIndex];
        _nextIndex = (_nextIndex + 1) % _pool.Count;
        return src;
    }

    private void PlaySfx(AudioClip clip, float volume = 1f, bool varyPitch = true)
    {
        if (clip == null) return;
        if (sfxVolume <= 0f) return;

        // 겹침 방지: 같은 클립은 최소 간격 내 재생 요청 무시
        if (preventSameClipOverlap)
        {
            float minInterval = Mathf.Max(0f, sameClipMinInterval);
            if (minInterval > 0f && _clipLastPlayed.TryGetValue(clip, out var lastTime))
            {
                if (Time.unscaledTime - lastTime < minInterval)
                {
                    return;
                }
            }
        }
        var src = GetSource();

        // 피치/볼륨 설정 후 OneShot
        float usedPitch = 1f;
        if (varyPitch && pitchMax > 0f)
        {
            var min = Mathf.Min(pitchMin, pitchMax);
            var max = Mathf.Max(pitchMin, pitchMax);
            usedPitch = Random.Range(min, max);
            src.pitch = usedPitch;
        }
        else
        {
            usedPitch = 1f;
            src.pitch = usedPitch;
        }

        src.PlayOneShot(clip, Mathf.Clamp01(volume) * sfxVolume);

        // 마지막 재생 시간 기록 (최소 간격 기반)
        if (preventSameClipOverlap)
        {
            _clipLastPlayed[clip] = Time.unscaledTime;
        }
    }

    // ========== 공개 API (레거시 호환) ==========
    public void PlayIngredient(int index)
    {
        if (index < 0 || index >= ingredientClips.Count)
        {
            return; // 잘못된 인덱스는 무시
        }
        PlaySfx(ingredientClips[index], 1f, defaultPitchVary);
    }

    public void PlayBlockPang() => PlaySfx(blockPangClip, 1f, defaultPitchVary);
    public void PlayBlockSwap() => PlaySfx(blockSwapClip, 1f, defaultPitchVary);
    public void PlayBlockSwapBack() => PlaySfx(blockSwapBackClip, 1f, defaultPitchVary);
    public void PlayFire() => PlaySfx(fireClip, 1f, defaultPitchVary);
    public void PlayIceBreak() => PlaySfx(iceBreakClip, 1f, defaultPitchVary);
    public void PlayHalo() => PlaySfx(haloClip, 1f, defaultPitchVary);
    public void PlayEarnGold() => PlaySfx(earnGoldClip, 1f, defaultPitchVary);
    public void PlayEarnGoldByCashier() => PlaySfx(earnGoldByCashierClip, 1f, defaultPitchVary);
    public void PlayTileChangeBreak() => PlaySfx(chainBreakClip, 1f, defaultPitchVary);
    public void PlayBomb() => PlaySfx(bombClip, 1f, defaultPitchVary);

    // ========== 품질-of-생명 추가 ==========
    /// <summary>
    /// 런타임에 전체 볼륨을 조절합니다. (0~1)
    /// </summary>
    public void SetSfxVolume(float volume01)
    {
        sfxVolume = Mathf.Clamp01(volume01);
    }

    /// <summary>
    /// 특정 위치에서 3D처럼 재생 (간단히 spatialBlend=1 적용)
    /// 필요시 사용. 기본은 2D 재생을 권장.
    /// </summary>
    public void PlaySfxAtPosition(AudioClip clip, Vector3 position, float volume = 1f)
    {
        if (clip == null) return;
        if (sfxVolume <= 0f) return;

        // 겹침 방지: 같은 클립은 최소 간격 내 재생 요청 무시
        if (preventSameClipOverlap)
        {
            float minInterval = Mathf.Max(0f, sameClipMinInterval);
            if (minInterval > 0f && _clipLastPlayed.TryGetValue(clip, out var lastTime))
            {
                if (Time.unscaledTime - lastTime < minInterval)
                {
                    return;
                }
            }
        }
        var src = GetSource();
        src.transform.position = position;
        var prevBlend = src.spatialBlend;
        src.spatialBlend = 1f;
        src.pitch = 1f;
        src.PlayOneShot(clip, Mathf.Clamp01(volume) * sfxVolume);
        // 다음 사용을 위해 다시 2D로 복원
        src.spatialBlend = prevBlend;

        if (preventSameClipOverlap)
        {
            _clipLastPlayed[clip] = Time.unscaledTime;
        }
    }
}
