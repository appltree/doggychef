using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace MyMatch3
{
    public class GoldGauge : MonoBehaviour
    {
        [SerializeField] private TMP_Text goldText;
        [SerializeField] private Image progress;
        [SerializeField] private Image goldIcon;

        [SerializeField] private List<Image> stars; // 별 이미지들 (3개 고정)

        [SerializeField] private Animator goalEffectPrefab;

        private readonly float flyDuration = 0.5f;
        private readonly AnimationCurve flyEase = AnimationCurve.EaseInOut(0, 0, 1, 1);

        private readonly int coinUnit = 600;           // 3000원 기준 5개 정도 날아가도록 조정
        private readonly int maxCoinsPerBurst = 5;     // 한 번에 최대 날아가는 코인 수
        private readonly float coinSpawnInterval = 0.06f; // 코인 간 간격(초)

        private LevelData levelData;
        private Canvas rootCanvas;

        private Coroutine fillRoutine;

        [SerializeField] private AudioClip goalSfx;

        private int lastStarCount = -1;
        private int m_LastEarned = 0; // OnMoneyChanged delta 계산용

        void Awake()
        {
            levelData = FindAnyObjectByType<LevelData>();
            rootCanvas = GetComponentInParent<Canvas>();
        }

        void OnEnable()
        {
            if (levelData == null) levelData = FindAnyObjectByType<LevelData>();
            if (levelData != null)
                levelData.OnMoneyChanged += HandleMoneyChanged;

            m_LastEarned = levelData != null ? levelData.EarnedMoney : 0;
            RefreshText();
            progress.fillAmount = CalcTargetFill();
        }

        void OnDisable()
        {
            if (levelData != null)
                levelData.OnMoneyChanged -= HandleMoneyChanged;
        }


        private void HandleMoneyChanged(int totalEarned, Vector3 worldPos)
        {
            if (!gameObject.activeInHierarchy) return;

            int delta = totalEarned - m_LastEarned;
            m_LastEarned = totalEarned;

            if (delta > 0)
                StartCoroutine(CoFlyAndUpdate(worldPos, delta));
            else
            {
                RefreshText();
                AnimateProgressTo(CalcTargetFill());
            }
        }


        private IEnumerator CoFlyAndUpdate(Vector3 worldPos, int amount)
        {
            // UI 좌표 변환
            RectTransform canvasRect = rootCanvas.transform as RectTransform;
            if (canvasRect == null) yield break;

            // ScreenSpaceOverlay일 때는 UI 카메라가 null이어야 정확함
            Camera uiCam = null;

            // 월드(게임) 좌표 -> 스크린 좌표 (코인 시작점은 게임 카메라 기준)
            Vector2 startScreen = (Vector2)Camera.main.WorldToScreenPoint(worldPos);

            // UI 아이콘은 UI 좌표이므로 UI 카메라(null for overlay)로 스크린 좌표 계산
            RectTransform iconRect = goldIcon.rectTransform;
            Vector2 endScreen = RectTransformUtility.WorldToScreenPoint(uiCam, iconRect.position);

            RectTransformUtility.ScreenPointToLocalPointInRectangle(canvasRect, startScreen, uiCam, out var startLocal);
            RectTransformUtility.ScreenPointToLocalPointInRectangle(canvasRect, endScreen, uiCam, out var endLocal);

            // amount 기준으로 코인 개수 산정 (100에 1코인).
            // 요구사항: 100 이하는 1개가 되어야 하므로 ceil을 사용하고, 최소 1개 보장(양수 금액일 때).
            int coinCount = 0;
            if (coinUnit > 0 && amount > 0)
            {
                // 실제 테이블에 쌓인 30개 동전이 날아갈 때 가독성을 위해 5개 정도로 요약해서 표현
                coinCount = Mathf.CeilToInt(amount / (float)coinUnit);
                coinCount = Mathf.Clamp(coinCount, 1, maxCoinsPerBurst);
            }

            // 코인이 0개면 바로 텍스트/게이지만 갱신하고 종료
            if (coinCount <= 0)
            {
                RefreshText();
                AnimateProgressTo(CalcTargetFill());
                yield break;
            }

            // 여러 코인을 간격을 두고 날리며 모두 끝날 때까지 대기
            int pending = coinCount;
            for (int i = 0; i < coinCount; i++)
            {
                float delay = Mathf.Max(0f, coinSpawnInterval) * i;
                StartCoroutine(CoFlyOneCoin(canvasRect, startLocal, endLocal, delay, () => pending--));
            }

            // 모두 끝날 때까지 대기
            while (pending > 0)
                yield return null;

            // 전체 연출 종료 후 텍스트/게이지 갱신
            RefreshText();
            AnimateProgressTo(CalcTargetFill());
        }

        private IEnumerator CoFlyOneCoin(RectTransform canvasRect, Vector2 startLocal, Vector2 endLocal, float startDelay, System.Action onComplete)
        {
            // 시작 지연 (실시간)
            if (startDelay > 0f)
                yield return new WaitForSecondsRealtime(startDelay);

            // 임시 코인 생성 (goldIcon 스프라이트 사용)
            var go = new GameObject("FlyingCoin", typeof(RectTransform), typeof(CanvasRenderer), typeof(Image));
            go.transform.SetParent(canvasRect, false);
            var coin = go.GetComponent<Image>();
            coin.sprite = goldIcon != null ? goldIcon.sprite : null;
            coin.SetNativeSize();

            var coinRect = coin.rectTransform;
            coinRect.anchoredPosition = startLocal;
            coinRect.localScale = Vector3.one * 0.5f; // 시작 스케일

            // 랜덤한 곡선 경로를 위한 제어점들 생성
            Vector2 direction = (endLocal - startLocal).normalized;
            Vector2 perpendicular = new Vector2(-direction.y, direction.x); // 수직 벡터

            // 랜덤한 곡선 강도와 방향
            float curveStrength = Random.Range(50f, 150f); // 곡선의 강도
            float curveDirection = Random.Range(-1f, 1f); // 곡선 방향 (왼쪽 또는 오른쪽)

            // 베지어 곡선의 제어점들
            Vector2 controlPoint1 = Vector2.Lerp(startLocal, endLocal, 0.3f) + perpendicular * curveStrength * curveDirection;
            Vector2 controlPoint2 = Vector2.Lerp(startLocal, endLocal, 0.7f) + perpendicular * curveStrength * curveDirection * 0.5f;

            // 비행 (베지어 곡선 사용)
            float t = 0f;
            while (t < 1f)
            {
                t += Time.deltaTime / Mathf.Max(0.01f, flyDuration);
                float k = flyEase != null ? flyEase.Evaluate(t) : t;

                // 3차 베지어 곡선으로 부드러운 곡선 경로 생성
                Vector2 curvePos = CalculateBezierPoint(k, startLocal, controlPoint1, controlPoint2, endLocal);
                coinRect.anchoredPosition = curvePos;

                // 코인이 이동 방향으로 살짝 회전하도록 (선택사항)
                if (k > 0.01f)
                {
                    Vector2 prevPos = CalculateBezierPoint(k - 0.01f, startLocal, controlPoint1, controlPoint2, endLocal);
                    Vector2 moveDir = (curvePos - prevPos).normalized;
                    float angle = Mathf.Atan2(moveDir.y, moveDir.x) * Mathf.Rad2Deg;
                    coinRect.rotation = Quaternion.Euler(0, 0, angle);
                }

                yield return null;
            }

            // 도착 바운스 연출
            float upT = 0f; float upDur = 0.1f;
            Vector3 halfScale = Vector3.one * 0.5f;
            Vector3 overScale = Vector3.one * 1.4f;
            while (upT < 1f)
            {
                upT += Time.deltaTime / Mathf.Max(0.01f, upDur);
                float k2 = upT;
                coinRect.localScale = Vector3.LerpUnclamped(halfScale, overScale, k2);
                yield return null;
            }

            float downT = 0f; float downDur = 0.12f;
            while (downT < 1f)
            {
                downT += Time.deltaTime / Mathf.Max(0.01f, downDur);
                float k3 = downT;
                coinRect.localScale = Vector3.LerpUnclamped(overScale, Vector3.one, k3);
                yield return null;
            }
            coinRect.localScale = Vector3.one;

            // 정리
            Destroy(coin.gameObject);
            onComplete?.Invoke();
        }

        private void RefreshText()
        {
            int value = levelData != null ? levelData.EarnedMoney : 0;
            goldText.text = value.ToString("N0");
            UpdateStars();
        }

        // 목표 fillAmount 계산: 0.74은 GoalGold 도달, 1.0은 GoalPerpect 도달
        private float CalcTargetFill()
        {
            if (levelData == null) return progress != null ? progress.fillAmount : 0f;
            var stage = levelData.CurrentStage;
            if (progress == null || stage == null) return 0f;

            int earned = Mathf.Max(0, levelData.EarnedMoney);
            int goal = Mathf.Max(1, stage.GoalGold); // 0 division 방지

            // 기본 진행률(0~1): GoalGold 까지는 0~1로 계산
            float baseRatio = Mathf.Clamp01((float)earned / goal);

            // 0~0.74로 매핑
            float mapped = baseRatio * 0.74f;

            // 퍼펙트 구간 처리: GoalPerpect에 도달하면 1.0, 그 사이는 0.74~1.0 선형
            int perfect = Mathf.Max(goal, stage.GoalPerpect);
            if (earned >= goal)
            {
                if (perfect <= goal) return 1f; // 방어적: perfect 설정이 goal 이하인 경우
                float extra = Mathf.Clamp01((float)(earned - goal) / (perfect - goal));
                mapped = Mathf.Lerp(0.74f, 1f, extra);
            }

            return Mathf.Clamp01(mapped);
        }

        // EarnedMoney가 목표를 넘으면 1개, goal~perfect 중간값 이상이면 2개, perfect 도달 시 3개 노출
        private void UpdateStars()
        {
            var stage = levelData != null ? levelData.CurrentStage : null;
            if (stage == null) return;

            int earned = Mathf.Max(0, levelData.EarnedMoney);
            int goal = Mathf.Max(1, stage.GoalGold);
            int perfect = Mathf.Max(goal, stage.GoalPerpect); // 안전장치: perfect는 goal 이상으로

            // 중간값(실수) 기준 계산
            float mid = (goal + perfect) * 0.5f;

            int starCount = 0;
            if (earned >= perfect)
            {
                starCount = 3;
            }
            else if (earned >= mid)
            {
                starCount = 2;
            }
            else if (earned >= goal)
            {
                starCount = 1;
            }
            else
            {
                starCount = 0;
            }

            if (lastStarCount < 3 && starCount >= 3)
            {
                GameManager.Instance?.PlaySFXOneShot(goalSfx);
                PlayGoalEffect(true); // perfect 도달
            }
            else if (lastStarCount < 1 && starCount >= 1)
            {
                GameManager.Instance?.PlaySFXOneShot(goalSfx);
                PlayGoalEffect(false); // goal 도달
            }

            // 표시 업데이트
            for (int i = 0; i < stars.Count; i++)
            {
                if (stars[i] == null) continue;
                stars[i].enabled = i < starCount;
            }

            // 상태 저장 (하강 시에는 소리 없음, 재상승 때만 재생)
            lastStarCount = starCount;
        }

        private void PlayGoalEffect(bool isPerfect)
        {
            if (goalEffectPrefab == null) return;
            var instGO = Instantiate(goalEffectPrefab.gameObject, transform);
            var instRect = instGO.transform as RectTransform;

            instRect.anchorMin = new Vector2(0.5f, 0.5f);
            instRect.anchorMax = new Vector2(0.5f, 0.5f);
            instRect.anchoredPosition = Vector2.zero;
            instRect.localScale = Vector3.one;
            instRect.localRotation = Quaternion.identity;
            instRect.localPosition = new Vector3(55, 0, 0f);

            // Animator 파라미터 세팅
            var anim = instGO.GetComponent<Animator>();
            if (anim != null)
            {
                anim.Rebind();
                anim.Update(0f);
                anim.SetBool("perfect", isPerfect);
                anim.Update(0f); // 즉시 반영
            }

            // timeScale과 무관하게 1초 뒤 파괴
            StartCoroutine(DestroyAfterDelay(instGO, 1f));
        }

        private IEnumerator DestroyAfterDelay(GameObject obj, float delay)
        {
            yield return new WaitForSecondsRealtime(delay);
            if (obj != null)
            {
                Destroy(obj);
            }
        }

        // 3차 베지어 곡선 계산 메서드
        private Vector2 CalculateBezierPoint(float t, Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3)
        {
            float u = 1 - t;
            float tt = t * t;
            float uu = u * u;
            float uuu = uu * u;
            float ttt = tt * t;

            Vector2 p = uuu * p0; // (1-t)^3 * P0
            p += 3 * uu * t * p1; // 3 * (1-t)^2 * t * P1
            p += 3 * u * tt * p2; // 3 * (1-t) * t^2 * P2
            p += ttt * p3; // t^3 * P3

            return p;
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
            float duration = 0.35f; // 기본 상승 시간

            // 살짝 바운스 주기: Overshoot 후 되돌아오기
            float delta = Mathf.Abs(target - start);
            float overshoot = Mathf.Min(0.04f, delta * 0.5f); // 변화폭이 작을수록 과도한 오버슈트 방지
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
    }

} // namespace MyMatch3
