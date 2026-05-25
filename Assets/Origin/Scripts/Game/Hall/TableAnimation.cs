using System;
using System.Collections;
using UnityEngine;

/// <summary>
/// Table의 애니메이션 관련 기능을 담당하는 컴포넌트
/// - 손 애니메이션
/// - 요리 완료시 펄스 애니메이션
/// - 플라이 토큰 애니메이션
/// </summary>
public class TableAnimation : MonoBehaviour
{
    [Header("애니메이션 컴포넌트")]
    [SerializeField] private SpriteRenderer handSpriteRender;
    [SerializeField] private SpriteRenderer balloonSpriteRender;

    private readonly float handAnimationSpeed = 12f;
    private readonly float cookedPulseMaxScale = 1.3f;
    private readonly float cookedPulseMinScale = 0.9f;
    private readonly float cookedPulseSpeed = 4f;
    private readonly float cookedPulseTintStrength = 0.45f;
    private readonly float flyDuration = 0.6f;
    private readonly float flyArcHeight = 1.0f;

    // 색상 및 벡터 설정
    private Vector2 handAnimationOffset = new Vector2(0.5f, 0.5f);
    private Color cookedPulseTint = Color.yellow;
    private Vector3 balloonBaseScale = Vector3.one;
    private Color balloonBaseColor = Color.white;

    // 애니메이션 관련
    private Vector3 handOriginalPosition;
    private bool handAnimationActive = false;
    private bool pulseCaptured;

    // 코루틴 관리
    private Coroutine handAnimationRoutine;
    private Coroutine pulseRoutine;

    // 기타
    private readonly AnimationCurve cookedPulseCurve;

    // Table 참조
    private Table table;

    private void Awake()
    {
        table = GetComponent<Table>();
    }

    private void Start()
    {
        InitializeHandAnimation();
        StopHandAnimation();
    }

    private void OnDisable()
    {
        StopCookedPulse();
    }

    public void EnableHandAnimation(bool enable = true)
    {
        if (enable)
        {
            if (!handAnimationActive)
            {
                handAnimationActive = true;
                if (handSpriteRender != null)
                    handSpriteRender.enabled = true;
                handAnimationRoutine = StartCoroutine(CoHandAnimation());
            }
        }
        else
        {
            StopHandAnimation();
        }
    }

    public void DisableHandAnimation()
    {
        EnableHandAnimation(false);
    }

    public bool IsHandAnimationActive()
    {
        return handAnimationActive;
    }

    public void StartCookedPulse()
    {
        if (balloonSpriteRender == null) return;
        if (!pulseCaptured)
        {
            balloonBaseScale = balloonSpriteRender.transform.localScale;
            balloonBaseColor = balloonSpriteRender.color;
            pulseCaptured = true;
        }
        StopCookedPulse();
        pulseRoutine = StartCoroutine(CookedPulseRoutine());
    }

    public void StopCookedPulse()
    {
        if (pulseRoutine != null)
        {
            StopCoroutine(pulseRoutine);
            pulseRoutine = null;
        }
        if (balloonSpriteRender != null && pulseCaptured)
        {
            balloonSpriteRender.transform.localScale = balloonBaseScale;
            balloonSpriteRender.color = balloonBaseColor;
        }
    }

    public void SpawnFlyToken(Sprite sprite, Vector3 start, Vector3 end, Action onArrive)
    {
        StartCoroutine(FlyTokenRoutine(sprite, start, end, onArrive));
    }

    private void InitializeHandAnimation()
    {
        if (handSpriteRender != null)
        {
            handOriginalPosition = handSpriteRender.transform.localPosition;
        }
    }

    private void StopHandAnimation()
    {
        handAnimationActive = false;
        if (handAnimationRoutine != null)
        {
            StopCoroutine(handAnimationRoutine);
            handAnimationRoutine = null;
        }
        if (handSpriteRender != null)
        {
            handSpriteRender.transform.localPosition = handOriginalPosition;
            handSpriteRender.enabled = false;
        }
    }

    private IEnumerator CoHandAnimation()
    {
        float time = 0f;
        Vector3 targetOffset = new Vector3(handAnimationOffset.x, handAnimationOffset.y, 0f);

        while (handAnimationActive && handSpriteRender != null && isActiveAndEnabled)
        {
            time += Time.deltaTime * handAnimationSpeed;
            float progress = (Mathf.Sin(time) + 1f) * 0.5f;
            Vector3 currentPos = Vector3.Lerp(handOriginalPosition, handOriginalPosition + targetOffset, progress);
            handSpriteRender.transform.localPosition = currentPos;
            yield return null;
        }
        handAnimationRoutine = null;
    }

    private IEnumerator CookedPulseRoutine()
    {
        float maxS = Mathf.Max(1.05f, cookedPulseMaxScale);
        float minS = Mathf.Clamp(cookedPulseMinScale, 0.1f, maxS - 0.001f);
        float speed = Mathf.Max(0.1f, cookedPulseSpeed);

        while (table.GetState() == Table.TableState.Cooked && balloonSpriteRender != null)
        {
            float raw = (Mathf.Sin(Time.time * speed) + 1f) * 0.5f;
            if (cookedPulseCurve != null && cookedPulseCurve.keys.Length > 0)
                raw = Mathf.Clamp01(cookedPulseCurve.Evaluate(raw));

            float scale = Mathf.Lerp(minS, maxS, raw);
            balloonSpriteRender.transform.localScale = balloonBaseScale * scale;

            if (cookedPulseTintStrength > 0f)
                balloonSpriteRender.color = Color.Lerp(balloonBaseColor, cookedPulseTint, raw * cookedPulseTintStrength);

            yield return null;
        }

        if (balloonSpriteRender != null && pulseCaptured)
        {
            balloonSpriteRender.transform.localScale = balloonBaseScale;
            balloonSpriteRender.color = balloonBaseColor;
        }
        pulseRoutine = null;
    }

    private IEnumerator FlyTokenRoutine(Sprite sprite, Vector3 start, Vector3 end, Action onArrive)
    {
        GameObject go = new GameObject("IngredientFlyToken");
        var sr = go.AddComponent<SpriteRenderer>();
        sr.sprite = sprite;
        sr.sortingOrder = 2000;
        go.transform.position = start;

        float t = 0f;
        while (t < 1f)
        {
            t += Time.deltaTime / Mathf.Max(0.05f, flyDuration);
            float tt = Mathf.Clamp01(t);
            Vector3 pos = Vector3.Lerp(start, end, tt);
            pos.y += Mathf.Sin(tt * Mathf.PI) * flyArcHeight;
            go.transform.position = pos;
            yield return null;
        }

        go.transform.position = end;
        onArrive?.Invoke();
        UnityEngine.Object.Destroy(go);
    }
}