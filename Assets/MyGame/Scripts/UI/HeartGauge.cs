using System.Collections;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace MyMatch3
{
    public class HeartGauge : MonoBehaviour
    {
        [SerializeField] private Image progress;
        [SerializeField] private Image heartIcon;

        // 게이지 최대치는 스테이지의 feverHeartCount를 사용한다.
        private readonly float animateDuration = 0.35f; // 채움 애니메이션 시간

        private readonly float flyDuration = 0.5f;
        private readonly AnimationCurve flyEase = AnimationCurve.EaseInOut(0, 0, 1, 1);

        private LevelData levelData;
        private Canvas rootCanvas;

        private Coroutine fillRoutine;
        private Coroutine bounceRoutine;
        private Coroutine heartbeatRoutine; // 심장박동 루틴

        private int displayedHeart = 0; // UI가 반영한 하트 누적치
        private readonly float beatScaleAmplitude = 0.12f;
        private readonly float beatFrequency = 1.6f;

        private Vector3 baseHeartScale;
        private Material shinyMaterial; // heartIcon의 shiny material 참조


        void Awake()
        {
            levelData = FindFirstObjectByType<LevelData>();
            rootCanvas = GetComponentInParent<Canvas>();
            // material을 복제하여 다른 곳에 영향을 주지 않도록 함
            shinyMaterial = new Material(heartIcon.material);
            heartIcon.material = shinyMaterial;
        }

        void OnEnable()
        {
            if (levelData == null) levelData = FindFirstObjectByType<LevelData>();
            if (levelData != null)
            {
                levelData.OnHeartChanged += HandleHeartChanged;
                levelData.OnFeverChanged += HandleFeverChanged;
                // 현재 상태 반영
                if (levelData.IsFever) StartHeartbeat(); else StopHeartbeat();
            }

            // 감쇠 등 외부 변화도 매 프레임 반영하기 위해 루틴 시작
            if (isActiveAndEnabled)
                StartCoroutine(CoObserveHeart());

            // 현재 상태로 즉시 동기화
            displayedHeart = Mathf.Max(0, levelData != null ? levelData.CurrentHeart : 0);
            if (progress != null)
                progress.fillAmount = CalcFillFor(displayedHeart);
        }

        void OnDisable()
        {
            if (levelData != null)
            {
                levelData.OnHeartChanged -= HandleHeartChanged;
                levelData.OnFeverChanged -= HandleFeverChanged;
            }
            // 심장박동 정지 및 원복
            StopHeartbeat();
        }

        void OnDestroy()
        {
            // 복제된 material 정리로 메모리 누수 방지
            if (shinyMaterial != null)
            {
                DestroyImmediate(shinyMaterial);
                shinyMaterial = null;
            }
        }

        private IEnumerator CoObserveHeart()
        {
            // LevelData 지연 참조
            if (levelData == null)
                levelData = FindFirstObjectByType<LevelData>();

            while (isActiveAndEnabled)
            {
                int current = levelData != null ? Mathf.Max(0, levelData.CurrentHeart) : displayedHeart;
                if (current != displayedHeart)
                {
                    // 이벤트 없이 변화(감소/증가)가 발생한 경우 부드럽게 보간
                    AnimateProgressTo(CalcFillFor(current));
                    displayedHeart = current;
                }
                yield return null;
            }
        }

        private void HandleFeverChanged(bool on)
        {
            if (on) StartHeartbeat(); else StopHeartbeat();
        }

        private void HandleHeartChanged(int current, int max, Vector3 worldPos)
        {
            int delta = current - displayedHeart;
            if (delta <= 0 || !gameObject.activeInHierarchy) return;
            StartCoroutine(CoFlyOneHeartAndAnimate(worldPos, delta));
        }

        private float CalcFillFor(int heart)
        {
            // LevelData 또는 Stage가 없으면 기본 1로 나눔(0 division 방지)
            int max = 1;
            if (levelData != null && levelData.CurrentStage != null)
                max = Mathf.Max(1, levelData.CurrentStage.FeverHeartCount);
            return Mathf.Clamp01(Mathf.Max(0, heart) / (float)max);
        }

        private void AnimateProgressTo(float target)
        {
            if (progress == null) return;
            if (fillRoutine != null) StopCoroutine(fillRoutine);
            fillRoutine = StartCoroutine(CoAnimateFill(target));
        }

        private IEnumerator CoAnimateFill(float target)
        {
            float start = progress.fillAmount;
            float duration = Mathf.Max(0.01f, animateDuration);

            // 약한 바운스 연출
            float delta = Mathf.Abs(target - start);
            float overshoot = Mathf.Min(0.04f, delta * 0.5f);
            float peak = target + Mathf.Sign(target - start) * overshoot;

            // 1단계: start -> peak (ease-out)
            float t1 = 0f;
            float d1 = duration * 0.6f;
            while (t1 < 1f)
            {
                t1 += Time.deltaTime / Mathf.Max(0.01f, d1);
                float k = 1f - (1f - t1) * (1f - t1); // EaseOutQuad
                progress.fillAmount = Mathf.Clamp01(Mathf.LerpUnclamped(start, peak, k));
                yield return null;
            }

            // 2단계: peak -> target (ease-in-out)
            float t2 = 0f;
            float d2 = duration * 0.4f;
            while (t2 < 1f)
            {
                t2 += Time.deltaTime / Mathf.Max(0.01f, d2);
                float k = Mathf.SmoothStep(0f, 1f, t2);
                progress.fillAmount = Mathf.Clamp01(Mathf.LerpUnclamped(peak, target, k));
                yield return null;
            }

            progress.fillAmount = Mathf.Clamp01(target);
            fillRoutine = null;
        }

        private IEnumerator CoFlyOneHeartAndAnimate(Vector3 worldPos, int amount)
        {
            RectTransform canvasRect = rootCanvas != null ? rootCanvas.transform as RectTransform : null;
            if (canvasRect == null || heartIcon == null)
            {
                // 폴백: 하트 아이콘/캔버스가 없으면 한 번에 게이지만 갱신
                yield return null; // 다음 프레임: LevelData.CurrentHeart 갱신 보장
                int finalByLd = levelData != null ? Mathf.Max(0, levelData.CurrentHeart) : displayedHeart + Mathf.Max(0, amount);
                AnimateProgressTo(CalcFillFor(finalByLd));
                displayedHeart = finalByLd;
                yield break;
            }

            Camera uiCam = null; // ScreenSpaceOverlay 가정

            // 월드(게임) 좌표 -> 스크린 좌표 (하트 시작점은 게임 카메라 기준)
            Vector2 startScreen = (Vector2)Camera.main.WorldToScreenPoint(worldPos);
            RectTransform iconRect = heartIcon.rectTransform;
            Vector2 endScreen = RectTransformUtility.WorldToScreenPoint(uiCam, iconRect.position);

            RectTransformUtility.ScreenPointToLocalPointInRectangle(canvasRect, startScreen, uiCam, out var startLocal);
            RectTransformUtility.ScreenPointToLocalPointInRectangle(canvasRect, endScreen, uiCam, out var endLocal);

            // 시작과 종료가 동일하면 아이콘 근처에서 시작하도록 오프셋 적용
            if ((startLocal - endLocal).sqrMagnitude < 1f)
            {
                startLocal = endLocal + new Vector2(-70f, 90f);
            }

            // 단일 하트만 둥둥 뜨듯 비행 후 도착 시 최종값으로 갱신
            yield return StartCoroutine(CoFlyBobbingHeart(canvasRect, startLocal, endLocal, 0f, null));

            // 도착 반응 & 게이지 최종치로 애니메이션
            BounceHeartIcon();

            // LevelData가 누적을 먼저 갱신하므로, 현재 값을 참조해 최종 fill로 설정
            int finalHeart = levelData != null ? Mathf.Max(0, levelData.CurrentHeart) : displayedHeart + Mathf.Max(0, amount);
            AnimateProgressTo(CalcFillFor(finalHeart));
            displayedHeart = finalHeart;
        }

        private IEnumerator CoFlyBobbingHeart(RectTransform canvasRect, Vector2 startLocal, Vector2 endLocal, float startDelay, System.Action onComplete)
        {
            if (startDelay > 0f)
                yield return new WaitForSecondsRealtime(startDelay);

            // 임시 하트 생성 (heartIcon 스프라이트 사용)
            var go = new GameObject("FlyingHeart", typeof(RectTransform), typeof(CanvasRenderer), typeof(Image));
            go.transform.SetParent(canvasRect, false);
            var heart = go.GetComponent<Image>();
            heart.sprite = heartIcon != null ? heartIcon.sprite : null;
            heart.SetNativeSize();

            var heartRect = heart.rectTransform;
            heartRect.anchoredPosition = startLocal;
            heartRect.localScale = Vector3.one * 0.5f;

            // 둥둥 뜨는 느낌: 진행 방향 법선으로 사인 파동을 더한다
            Vector2 dir = (endLocal - startLocal);
            Vector2 tangent = dir.sqrMagnitude > 0.0001f ? dir.normalized : Vector2.right;
            Vector2 normal = new Vector2(-tangent.y, tangent.x); // 시계방향 90도

            float t = 0f;
            float dur = Mathf.Max(0.01f, flyDuration);
            float amplitude = Mathf.Min(40f, dir.magnitude * 0.15f); // 최대 40px
            float cycles = 1.5f; // 1.5회 진동

            // 살짝 떠오르며 시작 스케일 업
            float startRise = 0.12f;
            float riseT = 0f;
            Vector2 liftedStart = startLocal + Vector2.up * Mathf.Min(20f, amplitude * 0.5f);
            while (riseT < 1f)
            {
                riseT += Time.deltaTime / Mathf.Max(0.01f, startRise);
                float k = Mathf.SmoothStep(0f, 1f, riseT);
                heartRect.anchoredPosition = Vector2.LerpUnclamped(startLocal, liftedStart, k);
                heartRect.localScale = Vector3.LerpUnclamped(Vector3.one * 0.5f, Vector3.one * 0.8f, k);
                yield return null;
            }

            // 본 비행
            while (t < 1f)
            {
                t += Time.deltaTime / dur;
                float baseK = flyEase != null ? flyEase.Evaluate(t) : t;
                Vector2 basePos = Vector2.LerpUnclamped(liftedStart, endLocal, baseK);
                float bob = Mathf.Sin(t * Mathf.PI * 2f * cycles) * amplitude * (1f - baseK); // 종점 근처에서 감소
                heartRect.anchoredPosition = basePos + normal * bob;
                heartRect.localRotation = Quaternion.Euler(0, 0, Mathf.Sin(t * Mathf.PI * 2f) * 8f); // 약간의 흔들림
                yield return null;
            }

            Destroy(heart.gameObject);
            onComplete?.Invoke();
        }

        private void BounceHeartIcon()
        {
            if (heartIcon == null) return;
            // 심장박동 중에는 바운스 생략해 시각 충돌 방지
            if (heartbeatRoutine != null) return;
            if (bounceRoutine != null) StopCoroutine(bounceRoutine);
            bounceRoutine = StartCoroutine(CoBounceIcon());
        }

        private IEnumerator CoBounceIcon()
        {
            var rt = heartIcon.rectTransform;
            Vector3 start = Vector3.one;
            Vector3 over = Vector3.one * 1.2f;

            float upT = 0f; float upDur = 0.08f;
            while (upT < 1f)
            {
                upT += Time.deltaTime / Mathf.Max(0.01f, upDur);
                float k = 1f - (1f - upT) * (1f - upT);
                rt.localScale = Vector3.LerpUnclamped(start, over, k);
                yield return null;
            }

            float downT = 0f; float downDur = 0.12f;
            while (downT < 1f)
            {
                downT += Time.deltaTime / Mathf.Max(0.01f, downDur);
                float k = Mathf.SmoothStep(0f, 1f, downT);
                rt.localScale = Vector3.LerpUnclamped(over, start, k);
                yield return null;
            }
            rt.localScale = start;
            bounceRoutine = null;
        }

        // 피버 시작 시 하트 아이콘 심장박동 시작 (스케일 애니메이션만, 색상 flash 제외)
        private void StartHeartbeat()
        {
            if (heartIcon == null) return;

            var rt = heartIcon.rectTransform;
            baseHeartScale = rt != null ? rt.localScale : Vector3.one;

            // Shiny material 활성화
            EnableShinyMaterial(true);

            // 바운스와 충돌 방지
            if (bounceRoutine != null)
            {
                StopCoroutine(bounceRoutine);
                bounceRoutine = null;
            }

            if (heartbeatRoutine != null) StopCoroutine(heartbeatRoutine);
            heartbeatRoutine = StartCoroutine(CoHeartbeat());
        }

        // 피버 종료 시 심장박동 중단 및 복귀
        private void StopHeartbeat()
        {
            if (heartbeatRoutine != null)
            {
                StopCoroutine(heartbeatRoutine);
                heartbeatRoutine = null;
            }

            // Shiny material 비활성화
            EnableShinyMaterial(false);

            if (heartIcon != null && heartIcon.rectTransform != null)
            {
                heartIcon.rectTransform.localScale = baseHeartScale == Vector3.zero ? Vector3.one : baseHeartScale;
            }
        }

        private IEnumerator CoHeartbeat()
        {
            if (heartIcon == null) yield break;
            var rt = heartIcon.rectTransform;
            Vector3 startScale = baseHeartScale == Vector3.zero ? (rt != null ? rt.localScale : Vector3.one) : baseHeartScale;

            float t = 0f; // 진행 시간(초)
            while (isActiveAndEnabled)
            {
                // 1초당 beatFrequency 번 박동하도록 위상 계산
                t += Time.deltaTime * Mathf.Max(0.1f, beatFrequency);
                float phase = t - Mathf.Floor(t); // 0..1

                // '쿵-짝' 두 번의 펄스: 첫 박동 강, 두 번째 약하게
                float p1 = Pulse(phase, 0.10f, 0.10f);        // 첫 강한 박동
                float p2 = Pulse(phase, 0.45f, 0.14f) * 0.6f; // 두 번째 약한 박동
                float k = Mathf.Clamp01(p1 + p2);             // 0..1 강도

                // 스케일 박동 (부드러운 ease)
                if (rt != null)
                {
                    float scaleMul = 1f + beatScaleAmplitude * (0.6f * k + 0.4f * Mathf.SmoothStep(0f, 1f, k));
                    rt.localScale = startScale * scaleMul;
                }

                yield return null;
            }
        }

        // 가우시안 형태의 펄스 (0~1), center는 주기 내 중심, width는 폭
        private float Pulse(float x, float center, float width)
        {
            float w = Mathf.Max(0.0001f, width);
            float a = (x - center) / w;
            return Mathf.Exp(-0.5f * a * a);
        }

        // Shiny material 활성화/비활성화
        private void EnableShinyMaterial(bool enable)
        {
            shinyMaterial.SetFloat("_CycleTime", enable ? .5f : 0f);
        }

    }

} // namespace MyMatch3
