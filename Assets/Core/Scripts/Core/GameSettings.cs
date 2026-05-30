using UnityEngine;
using UnityEngine.Audio;
using UnityEngine.VFX;

namespace DoggyChef
{
    [System.Serializable]
    public class GameSettings
    {
        public float InactivityBeforeHint = 2.0f;
        public BonusSetting BonusSettings;
        public VisualSetting VisualSettings;
        public SoundSetting SoundSettings;
    }

    [System.Serializable]
    public class VisualSetting
    {
        public float FallSpeed = 10.0f;
        public AnimationCurve FallAccelerationCurve;
        public AnimationCurve BounceCurve;
        public AnimationCurve SquishCurve;

        public AnimationCurve MatchFlyCurve;
        public AnimationCurve CoinFlyCurve;
        public GameObject BonusModePrefab;
        public GameObject HintPrefab;
        public GameObject MatchPangVFX;
        public float PostMatchFallDelay = 0.2f;
        public VisualEffect CoinVFX;
        public VisualEffect WinEffect;
        public VisualEffect LoseEffect;
    }

    [System.Serializable]
    public class BonusSetting
    {
        public BonusGem[] Bonuses;
    }

    [System.Serializable]
    public class SoundSetting
    {
        public AudioMixer Mixer;
        public AudioSource SFXSourcePrefab;
        public AudioSource MusicSourcePrefab;
        public AudioClip MenuSound;
        public AudioClip SwapSound;
        public AudioClip SwapBackSound;
        public AudioClip FallSound;
        public AudioClip MatchPangSound;
        public AudioClip CoinSound;
        public AudioClip WinVoice;
        public AudioClip LooseVoice;
        public AudioClip WinSound;
        public AudioClip LooseSound;
    }
}