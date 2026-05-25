using System.Collections;
using UnityEngine;
using UnityEngine.UI;

namespace MyMatch3
{
    public class TimerGauge : MonoBehaviour
    {
        [Header("UI References")]
        [SerializeField] private Image imgRadialFill;
        [SerializeField] private Image imgNiddle;

        [Header("Audio")]
        [SerializeField] private AudioClip timeRunningOutSfx;
        [SerializeField] private AudioClip timeOutSfx;

        [Header("Shake Effect")]
        [SerializeField] private float shakeIntensity = 15f; // 흔들림 강도 (각도)
        [SerializeField] private float shakeDuration = 1f; // 흔들림 지속 시간
        [SerializeField] private float shakeSpeed = 20f; // 흔들림 속도

        private AudioSource audioSource;

        private Coroutine shakeRoutine;

        // Settings
        private const float NeedleStartAngle = 0f;
        private const float NeedleSweepDegrees = 360f;
        private const float RunningOutThreshold = 10f;

        // State
        private bool isRunningOutLooping;
        private bool timeOutPlayed;

        private void Awake()
        {
            // 루핑 사운드(지속음)는 GameManager의 풀링에 의해 씹히지 않도록 전용 AudioSource를 사용합니다.
            audioSource = gameObject.AddComponent<AudioSource>();
            audioSource.playOnAwake = false;
            audioSource.loop = true;
        }

        /// <summary>
        /// 초기 설정. LevelData의 이벤트에 구독하고, 초기 상태를 한 번 갱신합니다.
        /// </summary>
        private void Start()
        {
            ResetSfxState();

            // GameManager 시스템(Volume 설정 등)을 타기 위해, 미리 세팅된 SFX 프리팹의 AudioMixerGroup을 복사해옵니다.
            if (GameManager.Instance != null &&
                GameManager.Instance.Settings != null &&
                GameManager.Instance.Settings.SoundSettings.SFXSourcePrefab != null)
            {
                audioSource.outputAudioMixerGroup = GameManager.Instance.Settings.SoundSettings.SFXSourcePrefab.outputAudioMixerGroup;
            }

            if (LevelData.Instance != null)
            {
                LevelData.Instance.OnTimeUpdated += HandleTimeUpdated;

                float remaining = Mathf.Max(0f, LevelData.Instance.RemainingTime);
                float normalized = LevelData.Instance.TimeLimit > 0 ? Mathf.Clamp01(remaining / LevelData.Instance.TimeLimit) : 0f;

                HandleTimeUpdated(remaining, normalized);
            }
        }

        /// <summary>
        /// 컴포넌트 파괴 시 메모리 누수를 방지하기 위해 이벤트를 해제합니다.
        /// </summary>
        private void OnDestroy()
        {
            if (LevelData.Instance != null)
            {
                LevelData.Instance.OnTimeUpdated -= HandleTimeUpdated;
            }

            ResetShakeState();
            ResetSfxState();
        }

        /// <summary>
        /// LevelData에서 매 프레임 발생하는 시간 갱신 이벤트를 수신하여 UI와 소리를 업데이트합니다.
        /// </summary>
        /// <param name="remainingTime">남은 시간 (초)</param>
        /// <param name="normalizedTime">0.0 ~ 1.0 (0=종료, 1=가득참)</param>
        private void HandleTimeUpdated(float remainingTime, float normalizedTime)
        {
            if (imgRadialFill != null)
            {
                imgRadialFill.fillAmount = normalizedTime;
            }

            if (imgNiddle != null)
            {
                float t = 1f - normalizedTime;
                float angle = NeedleStartAngle + (-NeedleSweepDegrees * t);
                imgNiddle.rectTransform.localRotation = Quaternion.Euler(0f, 0f, angle);
            }

            if (timeOutPlayed) return;

            if (remainingTime <= 0f)
            {
                TriggerTimeOut();
            }
            else if (remainingTime <= RunningOutThreshold)
            {
                StartRunningOutLoopIfNeeded();
            }
            else if (isRunningOutLooping)
            {
                StopRunningOutLoop();
            }
        }

        private void TriggerTimeOut()
        {
            StopRunningOutLoop();

            GameManager.Instance?.PlaySFXOneShot(timeOutSfx);

            StartShakeEffect();
            timeOutPlayed = true;
        }

        private void StartRunningOutLoopIfNeeded()
        {
            if (isRunningOutLooping || timeRunningOutSfx == null) return;

            audioSource.clip = timeRunningOutSfx;
            audioSource.Play();

            isRunningOutLooping = true;
        }

        private void StopRunningOutLoop()
        {
            if (!isRunningOutLooping) return;

            if (audioSource != null)
            {
                audioSource.Stop();
                audioSource.clip = null;
            }

            isRunningOutLooping = false;
        }

        private void ResetSfxState()
        {
            timeOutPlayed = false;
            StopRunningOutLoop();
        }

        private void StartShakeEffect()
        {
            if (shakeRoutine != null) return;
            shakeRoutine = StartCoroutine(CoShakeEffect());
        }

        private IEnumerator CoShakeEffect()
        {
            float elapsedTime = 0f;

            while (elapsedTime < shakeDuration)
            {
                float shakeAngle = Mathf.Sin(elapsedTime * shakeSpeed) * shakeIntensity;
                transform.localRotation = Quaternion.Euler(0f, 0f, shakeAngle);

                elapsedTime += Time.deltaTime;
                yield return null;
            }

            transform.localRotation = Quaternion.identity;
            shakeRoutine = null;
        }

        private void ResetShakeState()
        {
            if (shakeRoutine != null)
            {
                StopCoroutine(shakeRoutine);
                shakeRoutine = null;
            }

            transform.localRotation = Quaternion.identity;
        }
    }
}