using System.Collections;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(AudioSource))]
public class TimerGauge : MonoBehaviour
{
    [SerializeField] private Image imgRadialFill;
    [SerializeField] private Image imgNiddle;

    [SerializeField] private AudioClip timeRunningOutSfx;
    [SerializeField] private AudioClip timeOutSfx;

    [Header("Shake Effect")]
    [SerializeField] private float shakeIntensity = 15f; // 흔들림 강도 (각도)
    [SerializeField] private float shakeDuration = 1f; // 흔들림 지속 시간
    [SerializeField] private float shakeSpeed = 20f; // 흔들림 속도

    private AudioSource soundSource;

    private readonly float needleStartAngle = 0f; // 바늘의 기준 각도(Z)
    private readonly float needleSweepDegrees = 360f; // 전체 시간 동안 시계방향 회전할 각도

    private GameManager gameManager; // 남은 시간 표시를 위해 참조

    private Coroutine timeUiRoutine;
    private Coroutine shakeRoutine;
    private float initialTimeLimit = -1f; // 게이지 정규화를 위한 초기 총 시간

    // SFX 상태
    private readonly float runningOutThreshold = 10f; // 10초 남으면 경고 시작
    private bool isRunningOutLooping = false;
    private bool timeOutPlayed = false;

    // 흔들림 상태
    private Vector3 originalPosition;
    private bool isShaking = false;


    void Awake()
    {
        soundSource = GetComponent<AudioSource>();
        if (soundSource != null)
        {
            soundSource.playOnAwake = false;
            soundSource.loop = false;
            soundSource.clip = null;
        }

        // 원본 위치 저장 및 피봇 설정
        originalPosition = transform.localPosition;
    }

    void OnEnable()
    {

        // 시간 표시 코루틴 시작
        timeUiRoutine ??= StartCoroutine(CoUpdateTimeText());

        // SFX 초기화
        ResetSfxState();

    }


    void OnDisable()
    {
        if (timeUiRoutine != null)
        {
            StopCoroutine(timeUiRoutine);
            timeUiRoutine = null;
        }

        if (shakeRoutine != null)
        {
            StopCoroutine(shakeRoutine);
            shakeRoutine = null;
        }

        // SFX 정리
        ResetSfxState();

        // 흔들림 정리
        ResetShakeState();
    }


    private IEnumerator CoUpdateTimeText()
    {

        if (gameManager == null)
            gameManager = FindFirstObjectByType<GameManager>();

        while (enabled)
        {
            // 초기 총 시간 확보 (Running 시작 직후 RemainingTime 값을 기준으로 가정)
            if (initialTimeLimit <= 0f && gameManager != null)
            {
                if (gameManager.RemainingTime > 0f)
                    initialTimeLimit = gameManager.RemainingTime;
            }

            // 게이지 업데이트
            if (gameManager != null && initialTimeLimit > 0f)
            {
                float remaining = Mathf.Max(0f, gameManager.RemainingTime);
                float normalized = Mathf.Clamp01(remaining / initialTimeLimit); // 1(시작) -> 0(끝)

                if (imgRadialFill != null)
                {
                    imgRadialFill.fillAmount = normalized; // 1 에서 0으로 감소
                }

                if (imgNiddle != null)
                {
                    // 시간 경과 비율(0~1): 0 = 시작, 1 = 종료
                    float t = 1f - normalized;
                    // 시계방향 회전은 Z축 음수 방향이므로 부호를 -로 둔다
                    float angle = needleStartAngle + (-needleSweepDegrees * t);
                    imgNiddle.rectTransform.localRotation = Quaternion.Euler(0f, 0f, angle);
                }
            }

            // SFX 처리: 10초 이하 경고 루프 & 0초 타임아웃 단발 (soundSource 사용, 겹침 방지)
            if (gameManager != null && initialTimeLimit > 0f)
            {
                float remaining = Mathf.Max(0f, gameManager.RemainingTime);

                // 0초: 타임아웃 단발
                if (!timeOutPlayed && remaining <= 0f)
                {
                    if (isRunningOutLooping)
                        StopRunningOutLoop();

                    if (timeOutSfx != null && soundSource != null)
                        soundSource.PlayOneShot(timeOutSfx);

                    // 타이머 끝날 때 흔들림 효과 시작
                    StartShakeEffect();

                    timeOutPlayed = true;
                }
                else if (!timeOutPlayed)
                {
                    // 10초 이하에서만 루프, 아니면 정지
                    if (remaining <= runningOutThreshold)
                        StartRunningOutLoopIfNeeded();
                    else if (isRunningOutLooping)
                        StopRunningOutLoop();
                }
            }


            // 프레임마다 체크하지만 텍스트는 초 단위 변화시에만 갱신
            yield return null;
        }
    }

    private void StartRunningOutLoopIfNeeded()
    {
        if (isRunningOutLooping || soundSource == null || timeRunningOutSfx == null)
            return;

        soundSource.Stop();
        soundSource.clip = timeRunningOutSfx;
        soundSource.loop = true;
        soundSource.Play();
        isRunningOutLooping = true;
    }

    private void StopRunningOutLoop()
    {
        if (!isRunningOutLooping || soundSource == null)
            return;

        soundSource.Stop();
        soundSource.clip = null;
        soundSource.loop = false;
        isRunningOutLooping = false;
    }

    private void ResetSfxState()
    {
        timeOutPlayed = false;
        if (soundSource != null)
        {
            soundSource.Stop();
            soundSource.clip = null;
            soundSource.loop = false;
        }
        isRunningOutLooping = false;
    }


    private void StartShakeEffect()
    {
        if (isShaking) return;

        shakeRoutine = StartCoroutine(CoShakeEffect());
    }

    private IEnumerator CoShakeEffect()
    {
        isShaking = true;
        float elapsedTime = 0f;

        while (elapsedTime < shakeDuration)
        {
            // 좌우 흔들림을 위한 회전 각도 계산 (사인파 사용)
            float shakeAngle = Mathf.Sin(elapsedTime * shakeSpeed) * shakeIntensity;

            // Z축 회전으로 좌우 흔들림 구현
            transform.localRotation = Quaternion.Euler(0f, 0f, shakeAngle);

            elapsedTime += Time.deltaTime;
            yield return null;
        }

        // 흔들림 종료 후 원래 회전으로 복귀
        transform.localRotation = Quaternion.identity;
        isShaking = false;
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
        transform.localPosition = originalPosition;
        isShaking = false;
    }
}

