using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace DoggyChef
{

    public class Map : MonoBehaviour
    {
        public static event Action<int, bool> OnMapFocused;

        [SerializeField] private List<Button> shopButtons;
        [SerializeField] private ScrollRect scrollRect;
        [SerializeField] private RectTransform content;
        [SerializeField] private Image mapImage;
        [SerializeField] private List<Vector2> targetPositions;
        [SerializeField] private List<Vector2> targetWidePositions;

        private readonly float moveDuration = 0.5f;
        private readonly float focusScale = 1.82f;
        private readonly float leftPadding = -200f;

        private Coroutine moveCoroutine;
        private bool prevScrollEnabled = true;
        private float prevNormalized;
        private Vector2 prevMapPos;
        private Vector3 prevMapScale;
        private bool hasPrevState;

        private void Awake()
        {
            if (scrollRect == null)
                scrollRect = GetComponentInChildren<ScrollRect>(true);
            if (scrollRect != null && content == null)
                content = scrollRect.content;
        }

        private void Start()
        {
            StagePanel.OnStagePanelClosed += OnStagePanelClosed;
        }

        private void OnDestroy()
        {
            StagePanel.OnStagePanelClosed -= OnStagePanelClosed;
        }

        public void OnFocus(int index)
        {
            if (shopButtons == null || index < 0 || index >= shopButtons.Count)
            {
                Debug.LogWarning($"Map.Focus: invalid shop index {index} or missing shop buttons.");
                return;
            }

            if (scrollRect == null || content == null)
                return;

            var target = shopButtons[index].transform as RectTransform;
            if (target == null)
                return;

            var viewport = scrollRect.viewport != null ? scrollRect.viewport : (RectTransform)scrollRect.transform;
            float contentWidth = content.rect.width;
            float viewportWidth = viewport.rect.width;
            float scrollableWidth = Mathf.Max(0f, contentWidth - viewportWidth);
            if (scrollableWidth <= 0f)
                return;

            Vector3 worldCenter = target.TransformPoint(target.rect.center);
            Vector3 contentLocalCenter = content.InverseTransformPoint(worldCenter);
            float targetCenterFromLeft = contentLocalCenter.x + contentWidth * content.pivot.x;
            float desiredLeft = targetCenterFromLeft - viewportWidth * 0.5f - leftPadding;
            desiredLeft = Mathf.Clamp(desiredLeft, 0f, scrollableWidth);
            float targetNormalized = desiredLeft / scrollableWidth;
            Vector2? mapTargetPos = GetResponsiveTargetPosition(index);

            CapturePreviousState();

            if (moveCoroutine != null)
                StopCoroutine(moveCoroutine);
            moveCoroutine = StartCoroutine(SmoothFocus(targetNormalized, mapTargetPos, focusScale, moveDuration));

            OnMapFocused?.Invoke(index, index == shopButtons.Count - 1);
        }

        public void RestoreFocus(float duration = 0.35f)
        {
            if (!hasPrevState)
                return;

            if (moveCoroutine != null)
                StopCoroutine(moveCoroutine);

            moveCoroutine = StartCoroutine(RestoreFocusRoutine(duration));
        }

        private void OnStagePanelClosed()
        {
            RestoreFocus();
        }

        private void CapturePreviousState()
        {
            if (hasPrevState)
                return;

            prevNormalized = scrollRect != null ? scrollRect.horizontalNormalizedPosition : 0f;
            if (mapImage != null)
            {
                var rt = mapImage.rectTransform;
                prevMapPos = rt.anchoredPosition;
                prevMapScale = rt.localScale;
            }
            else
            {
                prevMapPos = Vector2.zero;
                prevMapScale = Vector3.one;
            }

            if (scrollRect != null)
            {
                prevScrollEnabled = scrollRect.enabled;
                scrollRect.enabled = false;
            }

            hasPrevState = true;
        }

        private IEnumerator RestoreFocusRoutine(float duration)
        {
            yield return SmoothFocus(prevNormalized, prevMapPos, prevMapScale.x, duration);

            if (scrollRect != null)
                scrollRect.enabled = prevScrollEnabled;

            hasPrevState = false;
        }

        private IEnumerator SmoothFocus(float targetNormalized, Vector2? mapTargetAnchored, float targetScale, float duration)
        {
            float startNorm = scrollRect != null ? scrollRect.horizontalNormalizedPosition : 0f;
            float endNorm = Mathf.Clamp01(targetNormalized);

            RectTransform mapRt = mapImage != null ? mapImage.rectTransform : null;
            bool moveMap = mapRt != null && mapTargetAnchored.HasValue;
            Vector2 startPos = moveMap ? mapRt.anchoredPosition : Vector2.zero;
            Vector2 endPos = moveMap ? mapTargetAnchored.Value : Vector2.zero;
            Vector3 startScale = mapRt != null ? mapRt.localScale : Vector3.one;
            Vector3 endScale = mapRt != null ? new Vector3(targetScale, targetScale, 1f) : Vector3.one;

            if (duration <= 0f)
            {
                ApplyFocus(endNorm, mapRt, moveMap, endPos, endScale);
                yield break;
            }

            float t = 0f;
            while (t < duration)
            {
                t += Time.unscaledDeltaTime;
                float p = Mathf.Clamp01(t / duration);
                float e = Mathf.SmoothStep(0f, 1f, p);

                float norm = Mathf.LerpUnclamped(startNorm, endNorm, e);
                Vector2 pos = Vector2.LerpUnclamped(startPos, endPos, e);
                Vector3 scale = Vector3.LerpUnclamped(startScale, endScale, e);
                ApplyFocus(norm, mapRt, moveMap, pos, scale);

                yield return null;
            }

            ApplyFocus(endNorm, mapRt, moveMap, endPos, endScale);
        }

        private void ApplyFocus(float normalized, RectTransform mapRt, bool moveMap, Vector2 pos, Vector3 scale)
        {
            if (scrollRect != null)
                scrollRect.horizontalNormalizedPosition = normalized;
            if (moveMap)
                mapRt.anchoredPosition = pos;
            if (mapRt != null)
                mapRt.localScale = scale;
        }

        private Vector2? GetResponsiveTargetPosition(int index)
        {
            Vector2? pos16By9 = targetPositions != null && index >= 0 && index < targetPositions.Count
                ? targetPositions[index]
                : null;
            Vector2? pos20By9 = targetWidePositions != null && index >= 0 && index < targetWidePositions.Count
                ? targetWidePositions[index]
                : null;

            if (!pos16By9.HasValue)
                return pos20By9;
            if (!pos20By9.HasValue)
                return pos16By9;

            float aspect = Screen.width / Mathf.Max(1f, (float)Screen.height);
            float t = Mathf.InverseLerp(16f / 9f, 20f / 9f, Mathf.Clamp(aspect, 16f / 9f, 20f / 9f));
            return Vector2.LerpUnclamped(pos16By9.Value, pos20By9.Value, t);
        }
    }

} // namespace DoggyChef
