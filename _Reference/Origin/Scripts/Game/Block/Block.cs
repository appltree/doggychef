// OK
using System.Collections;
using UnityEngine;
using SpriteGlow;

public class Block : MonoBehaviour
{
    public enum BlockState
    {
        Spawning,
        Idle,
        Moving,
        Falling,
        Destroying,
    }

    //
    // state
    [SerializeField] protected BlockState currentState;
    public bool IsMovable => currentState == BlockState.Idle;
    public bool IsInteractive => currentState == BlockState.Idle && !HasIce;
    public bool IsMatchable => currentState == BlockState.Idle;

    [SerializeField] private string blockTag;
    public string BlockTag => blockTag;

    private Ice ice;
    public bool HasIce => ice != null;

    private Board board; // Board 참조
    private SpriteRenderer sr; // 자식 SpriteRenderer 참조
    private AudioClip foodSfx; // 이 블럭의 고유 수집 사운드

    private readonly float spawningDelayTime = 0.1f;
    private readonly float moveAnimationDuration = 0.1f;
    private readonly float fallSpeedMultiplier = 0.03f;
    private readonly float bounceDuration = 0.1f;
    private readonly float bounceHeight = 0.2f;
    private readonly float destroyDuration = 0.2f;

    private readonly float glowMinBrightness = .2f; // SpriteGlow 권장 범위 1~10
    private readonly float glowMaxBrightness = 1.2f;
    private readonly float glowPulseSpeed = 1.1f;    // 초당 사이클 속도 계수

    // SpriteGlow 상태 관리
    private SpriteGlowEffect glow;
    private Coroutine glowPulseRoutine;

    // 폭탄에 의해 파괴 판정되었는지 여부 (폭탄 생성 조건에서 제외하기 위해)
    private bool explodedByBomb = false;
    public bool WasExplodedByBomb => explodedByBomb;


    void Awake()
    {
        board = FindFirstObjectByType<Board>();
        sr = GetComponentInChildren<SpriteRenderer>();
        glow = GetComponentInChildren<SpriteGlowEffect>(true);
    }


    protected virtual void Start()
    {
        StartCoroutine(SpawningToIdle());
    }


    public void StartGlowPulse(Color color, float minBrightness = -1f, float maxBrightness = -1f, float speed = -1f)
    {
        if (glow == null) return;

        // 파라미터 기본값 보정
        float minB = (minBrightness > 0f) ? minBrightness : glowMinBrightness;
        float maxB = (maxBrightness > 0f) ? maxBrightness : glowMaxBrightness;
        float spd = (speed > 0f) ? speed : glowPulseSpeed;

        // 유효 범위 클램프 (SpriteGlow 권장 범위 1~10)
        minB = Mathf.Clamp(minB, 1f, 10f);
        maxB = Mathf.Clamp(maxB, 1f, 10f);
        if (minB > maxB) (minB, maxB) = (maxB, minB);

        // 기존 루틴 중지 및 원복은 하지 않음(연속 호출 시 색과 범위만 갱신함)
        if (glowPulseRoutine != null)
        {
            StopCoroutine(glowPulseRoutine);
            glowPulseRoutine = null;
        }

        glow.OutlineWidth = 1;
        glow.enabled = true;
        glow.GlowColor = color;

        glowPulseRoutine = StartCoroutine(CoGlowPulse(minB, maxB, spd));
    }

    public void StopGlowPulse()
    {
        if (glow == null) return;
        if (glowPulseRoutine != null)
        {
            StopCoroutine(glowPulseRoutine);
            glowPulseRoutine = null;
        }

        glow.enabled = false;
    }

    private IEnumerator CoGlowPulse(float minBrightness, float maxBrightness, float speed)
    {
        // 사인 파형 기반 펄스: Time.time을 활용해 프레임과 무관한 연속 애니메이션
        while (true)
        {
            // 0..1 범위로 변환된 사인값
            float s = (Mathf.Sin(Time.time * speed * Mathf.PI * 2f) + 1f) * 0.5f;
            float b = Mathf.Lerp(minBrightness, maxBrightness, s);
            if (glow != null)
            {
                glow.GlowBrightness = b;
            }
            yield return null;
        }
    }

    void OnDisable()
    {
        // 비활성화 시 펄스 중단 및 원복
        StopGlowPulse();
    }


    public void AttachIce(Ice newIce)
    {
        if (newIce == null) return;
        if (ice != null) return; // 이미 얼음이 있음
        ice = newIce;
    }


    public void BreakIce()
    {
        if (ice == null) return;
        ice.BreakAndVanish();
        ice = null;
    }


    public virtual void DestroyBlock(System.Action onDestroyComplete = null)
    {
        if (currentState == BlockState.Destroying)
            return;

        currentState = BlockState.Destroying;
        StartCoroutine(DestroyBlockAnimated(onDestroyComplete));
    }

    // 폭탄 파괴 플래그 설정 (BorderBlockDestroyer 등에서 호출)
    public void MarkExplodedByBomb()
    {
        explodedByBomb = true;
    }


    public void SetupBlock(Sprite sprite, string tag)
    {
        sr.sprite = sprite;
        blockTag = tag;
    }

    // BlockSO에서 주입되는 고유 SFX
    public void SetFoodSfx(AudioClip clip)
    {
        foodSfx = clip;
    }
    public AudioClip GetFoodSfx() => foodSfx;


    public void StartFall(Vector3 targetPosition)
    {
        currentState = BlockState.Falling;
        StartCoroutine(FallBlocksAnimated(transform.localPosition, targetPosition));
    }


    public void StartSwap(Vector3 targetPosition)
    {
        currentState = BlockState.Moving;
        StartCoroutine(SwapBlocksAnimated(transform.localPosition, targetPosition));
    }


    private IEnumerator SpawningToIdle()
    {
        yield return new WaitForSeconds(spawningDelayTime);
        currentState = BlockState.Idle;
    }


    private IEnumerator SwapBlocksAnimated(Vector3 fromPosition, Vector3 toPosition)
    {
        float elapsedTime = 0f;

        while (elapsedTime < moveAnimationDuration)
        {
            float t = elapsedTime / moveAnimationDuration;
            // Ease-in-out 곡선 적용
            t = Mathf.SmoothStep(0f, 1f, t);

            // 블록들을 서로의 위치로 이동
            transform.localPosition = Vector3.Lerp(fromPosition, toPosition, t);

            elapsedTime += Time.deltaTime;
            yield return null;
        }

        // 최종 위치로 정확히 설정
        transform.localPosition = toPosition;
        currentState = BlockState.Idle;
    }


    private IEnumerator FallBlocksAnimated(Vector3 fromPosition, Vector3 toPosition)
    {
        float distance = Vector3.Distance(fromPosition, toPosition);
        float fallDuration = Mathf.Sqrt(distance * fallSpeedMultiplier);

        // === 낙하 단계 ===
        float elapsedTime = 0f;

        while (elapsedTime < fallDuration)
        {
            float t = elapsedTime / fallDuration;

            // 초기 속도를 빠르게, 중간 이후 중력 가속도 효과 (t^2 + t^3)로 부드럽게
            float gravityT = Mathf.Clamp01((t * t * 0.7f) + (t * t * t * 0.3f));

            Vector3 currentPosition = Vector3.Lerp(fromPosition, toPosition, gravityT);
            transform.localPosition = currentPosition;

            elapsedTime += Time.deltaTime;
            yield return null;
        }

        // 착지 위치로 정확히 설정
        transform.localPosition = toPosition;

        // === 바운스 단계 (한 번만) ===
        elapsedTime = 0f;

        while (elapsedTime < bounceDuration)
        {
            float bounceT = elapsedTime / bounceDuration;

            // 단일 바운스 효과 (사인파 한 번)
            float bounceWave = Mathf.Sin(bounceT * Mathf.PI) * bounceHeight * (1f - bounceT);

            // Y축에만 바운스 적용
            Vector3 bouncePosition = toPosition;
            bouncePosition.y += Mathf.Max(0, bounceWave); // 아래로는 바운스하지 않음

            transform.localPosition = bouncePosition;

            elapsedTime += Time.deltaTime;
            yield return null;
        }

        // 최종 위치로 정확히 설정
        transform.localPosition = toPosition;
        currentState = BlockState.Idle;
        board.OnBlockStopMoving(this);
    }


    private IEnumerator DestroyBlockAnimated(System.Action onDestroyComplete = null)
    {
        float elapsedTime = 0f;

        SpriteRenderer sr = GetComponentInChildren<SpriteRenderer>();
        if (sr == null)
        {
            onDestroyComplete?.Invoke();
            Destroy(gameObject);
            yield break;
        }

        Color originalColor = sr.color;

        while (elapsedTime < destroyDuration)
        {
            float t = elapsedTime / destroyDuration;
            // 페이드 아웃 효과 (알파값 감소)
            float alpha = Mathf.Lerp(1f, 0f, t);
            sr.color = new Color(originalColor.r, originalColor.g, originalColor.b, alpha);

            elapsedTime += Time.deltaTime;
            yield return null;
        }

        // 애니메이션 완료 후 콜백 호출
        onDestroyComplete?.Invoke();
        Destroy(gameObject);
    }


    void OnDrawGizmos()
    {
        // 블록 상태에 따라 색상 변경
        Gizmos.color = currentState switch
        {
            BlockState.Spawning => Color.gray,
            BlockState.Idle => Color.green,
            BlockState.Moving => Color.blue,
            BlockState.Falling => Color.red,
            _ => Color.yellow,
        };
        Gizmos.DrawWireCube(transform.position, Vector3.one);
    }
}
