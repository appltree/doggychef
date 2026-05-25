// OK
using System.Collections;
using UnityEngine;


public class LightBallBomb : BombBlock
{
    [SerializeField] private Transform rainbowTransform; // 회전/펄스 대상(자식)
    [SerializeField] private SpriteRenderer flashRenderer; // rainbow 위의 뒤집개 스프라이트

    private readonly string flashAmountProperty = "_FlashAmount"; // 보통 _FlashAmount를 사용함

    private readonly float rotationSpeed = 80f;
    private readonly float pulseAmplitude = 0.08f; // 8%
    private readonly float pulseFrequency = 0.2f;

    private readonly float flashCycleMul = 3f; // 플래시 주기 가속도(기존 대비 2배 → 주기 절반)

    private Vector3 rainbowBaseScale;
    private MaterialPropertyBlock flashMPB;
    private int flashPropId;
    private Coroutine flashRoutine;

    private bool isFlashing;


    protected override void Start()
    {
        base.Start();
        isFlashing = false;

        rainbowBaseScale = rainbowTransform.localScale;
        flashMPB = new MaterialPropertyBlock();
        flashPropId = Shader.PropertyToID(flashAmountProperty);
        StartCoroutine(RunIdleVFX());
    }

    void OnDisable()
    {
        // 코루틴/머티리얼 상태 정리
        if (flashRoutine != null)
        {
            StopCoroutine(flashRoutine);
            flashRoutine = null;
        }
    }


    // 외부에서 호출: 순간 플래시 이펙트 시작
    public void StartFlash()
    {
        isFlashing = true;
    }

    // 외부에서 호출: 플래시 종료 및 머티리얼 값 초기화
    public void StopFlash()
    {
        isFlashing = false;
        if (flashRenderer != null)
        {
            flashRenderer.GetPropertyBlock(flashMPB);
            flashMPB.SetFloat(flashPropId, 0f);
            flashRenderer.SetPropertyBlock(flashMPB);
        }
    }


    private IEnumerator RunIdleVFX()
    {
        // 코루틴으로 부드럽게 회전 + 펄스
        float t = 0;
        while (isActiveAndEnabled)
        {
            // 1) 회전(자체 Z축 기준)
            rainbowTransform.Rotate(0f, 0f, rotationSpeed * Time.deltaTime, Space.Self);

            // 2) 펄스(사인파를 스무스하게 매핑)
            t += Time.deltaTime * pulseFrequency * Mathf.PI * 2f; // 라디안 증가
            float s01 = (Mathf.Sin(t) + 1f) * 0.5f; // 0..1
            s01 = Mathf.SmoothStep(0f, 1f, s01); // 조금 더 부드럽게
            float scaleMul = 1f + (s01 - 0.5f) * 2f * pulseAmplitude; // 1±amplitude
            rainbowTransform.localScale = rainbowBaseScale * scaleMul;

            // // 3) 뒤집개 Flash Amount를 펄스에 동기화(0..1)
            // // 동기화된 값 적용
            if (isFlashing)
            {
                // 플래시 전용으로 더 빠른 주기를 사용
                float s01Flash = (Mathf.Sin(t * Mathf.Max(1f, flashCycleMul)) + 1f) * 0.5f; // 0..1
                s01Flash = Mathf.SmoothStep(0f, 1f, s01Flash);

                flashRenderer.GetPropertyBlock(flashMPB);
                flashMPB.SetFloat(flashPropId, s01Flash * 0.03f);
                flashRenderer.SetPropertyBlock(flashMPB);
            }

            yield return null;
        }
    }

}
