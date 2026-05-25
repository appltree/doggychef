using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Events;
using System.Collections;
using System.Collections.Generic;
using System;

public class Map : MonoBehaviour
{
    // OnFocus 이벤트: index와 isLeftSide(index==5인지) 정보를 전달
    public static event Action<int, bool> OnMapFocused;
    [SerializeField] private List<Button> shopButtons;

    [SerializeField] private ScrollRect scrollRect; // ScrollView 컴포넌트
    [SerializeField] private RectTransform content;  // scrollRect.content로 자동 보완 가능

    [SerializeField] private Image mapImage;

    [SerializeField] private List<Vector2> targetPositions; // 화면크기가 16:9일 때의 목표 위치
    [SerializeField] private List<Vector2> targetWidePositions; // 화면크기가 20:9일 때의 목표 위치


    private readonly float moveDuration = .5f;
    private readonly float focusScale = 1.82f;
    private readonly float leftPadding = -200f;

    private Coroutine moveCoroutine;
    private bool prevScrollEnabled = true;

    // 복귀를 위한 상태 저장
    private float prevNormalized;
    private Vector2 prevMapPos;
    private Vector3 prevMapScale;
    private bool hasPrevState = false;

    private void Awake()
    {
        if (scrollRect == null)
            scrollRect = GetComponentInChildren<ScrollRect>(true);
        if (scrollRect != null && content == null)
            content = scrollRect.content;
    }

    private void Start()
    {
        // StagePanel의 닫힘 이벤트 구독
        StagePanel.OnStagePanelClosed += OnStagePanelClosed;
    }

    private void OnDestroy()
    {
        // 이벤트 구독 해제
        StagePanel.OnStagePanelClosed -= OnStagePanelClosed;
    }

    // StagePanel이 닫힐 때 호출되어 포커스 복귀 처리
    private void OnStagePanelClosed()
    {
        RestoreFocus();
    }

    public void OnFocus(int index)
    {
        if (shopButtons == null || index < 0 || index >= shopButtons.Count)
        {
            Debug.LogWarning($"Map.Focus: 유효하지 않은 인덱스 {index} 또는 shopButtons 미할당");
            return;
        }

        var btn = shopButtons[index];

        var target = btn.transform as RectTransform;
        if (target == null)
            return;

        // 가로 중심에 오도록 목표 normalizedPosition 계산
        RectTransform viewport = scrollRect.viewport != null
            ? scrollRect.viewport
            : (RectTransform)scrollRect.transform;

        float contentWidth = content.rect.width;
        float viewportWidth = viewport.rect.width;

        // 콘텐츠가 뷰포트보다 작거나 같으면 스크롤할 필요 없음
        float scrollableWidth = Mathf.Max(0f, contentWidth - viewportWidth);
        if (scrollableWidth <= 0f)
            return;

        // 타겟의 월드 기준 중심 → 콘텐츠 로컬 좌표로 변환
        Vector3 worldCenter = target.TransformPoint(target.rect.center);
        Vector3 contentLocalCenter = content.InverseTransformPoint(worldCenter);

        // 콘텐츠의 왼쪽 가장자리로부터의 거리로 변환 (pivot 보정)
        float targetCenterFromLeft = (float)contentLocalCenter.x + contentWidth * content.pivot.x;

        // 뷰포트 중앙에 오도록 필요한 왼쪽 스크롤 거리 계산
        float desiredLeft = targetCenterFromLeft - (viewportWidth * 0.5f);

        // 약간 왼쪽 여백이 필요하면 적용(선택사항)
        desiredLeft -= leftPadding;

        desiredLeft = Mathf.Clamp(desiredLeft, 0f, scrollableWidth);
        float targetNormalized = scrollableWidth > 0f ? desiredLeft / scrollableWidth : 0f;

        // 맵 이미지 목표 위치/스케일 준비 (화면 비율에 따른 보간/스케일 적용)
        Vector2? mapTargetPos = GetResponsiveTargetPosition(index);

        // 현재 상태 저장(한 번만)
        if (!hasPrevState)
        {
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
            hasPrevState = true;

            // 스크롤 입력 비활성화 (사용자 스크롤 잠금)
            if (scrollRect != null)
            {
                prevScrollEnabled = scrollRect.enabled;
                scrollRect.enabled = false;
            }
        }

        if (moveCoroutine != null)
            StopCoroutine(moveCoroutine);
        moveCoroutine = StartCoroutine(SmoothFocus(targetNormalized, mapTargetPos, focusScale, moveDuration));

        // OnFocus 이벤트 발생: index와 왼쪽 배치 여부 전달
        OnMapFocused?.Invoke(index, index == 5);
    }


    private IEnumerator SmoothFocus(float targetNormalized, Vector2? mapTargetAnchored, float targetScale, float duration)
    {
        // 준비: 스크롤 시작/끝
        float startNorm = scrollRect != null ? scrollRect.horizontalNormalizedPosition : 0f;
        float endNorm = Mathf.Clamp01(targetNormalized);

        // 준비: 맵 이미지 위치/스케일 시작/끝
        RectTransform mapRt = mapImage != null ? mapImage.rectTransform : null;
        bool moveMap = mapRt != null && mapTargetAnchored.HasValue;
        Vector2 startPos = moveMap ? mapRt.anchoredPosition : Vector2.zero;
        Vector2 endPos = moveMap ? mapTargetAnchored.Value : Vector2.zero;
        Vector3 startScale = mapRt != null ? mapRt.localScale : Vector3.one;
        Vector3 endScale = mapRt != null ? new Vector3(targetScale, targetScale, 1f) : Vector3.one;

        if (duration <= 0f)
        {
            if (scrollRect != null)
                scrollRect.horizontalNormalizedPosition = endNorm;
            if (moveMap)
                mapRt.anchoredPosition = endPos;
            if (mapRt != null)
                mapRt.localScale = endScale;
            yield break;
        }

        float t = 0f;
        while (t < duration)
        {
            t += Time.unscaledDeltaTime;
            float p = Mathf.Clamp01(t / duration);
            float e = Mathf.SmoothStep(0f, 1f, p);

            if (scrollRect != null)
            {
                scrollRect.horizontalNormalizedPosition = Mathf.LerpUnclamped(startNorm, endNorm, e);
            }

            if (moveMap)
            {
                mapRt.anchoredPosition = Vector2.LerpUnclamped(startPos, endPos, e);
            }
            if (mapRt != null)
            {
                mapRt.localScale = Vector3.LerpUnclamped(startScale, endScale, e);
            }

            yield return null;
        }

        if (scrollRect != null)
            scrollRect.horizontalNormalizedPosition = endNorm;
        if (moveMap)
            mapRt.anchoredPosition = endPos;
        if (mapRt != null)
            mapRt.localScale = endScale;
    }


    // 현재 화면 비율(16:9 ~ 20:9)에 따라 타겟 앵커 위치를 보간하여 반환
    // PanelLetterbox가 상위에서 화면 크기를 관리하므로 여기서는 스케일을 적용하지 않습니다.
    private Vector2? GetResponsiveTargetPosition(int index)
    {
        // 인덱스 유효성 및 리스트 준비
        Vector2? pos16by9 = (targetPositions != null && index >= 0 && index < targetPositions.Count)
            ? targetPositions[index]
            : (Vector2?)null;

        Vector2? pos20by9 = (targetWidePositions != null && index >= 0 && index < targetWidePositions.Count)
            ? targetWidePositions[index]
            : (Vector2?)null;

        // 둘 다 없으면 이동하지 않음
        if (!pos16by9.HasValue && !pos20by9.HasValue)
            return null;

        // 하나만 있으면 그 값 사용
        if (!pos16by9.HasValue)
            return pos20by9.Value;
        if (!pos20by9.HasValue)
            return pos16by9.Value;

        // 화면 비율에 따른 보간 (16:9 -> 20:9), 범위 밖은 클램프
        float aspect = (float)Screen.width / Mathf.Max(1f, (float)Screen.height);
        const float aspect16_9 = 16f / 9f;
        const float aspect20_9 = 20f / 9f;

        float clampedAspect = Mathf.Clamp(aspect, aspect16_9, aspect20_9);
        float t = Mathf.InverseLerp(aspect16_9, aspect20_9, clampedAspect);
        return Vector2.LerpUnclamped(pos16by9.Value, pos20by9.Value, t);
    }


    public void RestoreFocus(float duration = 0.35f)
    {
        if (!hasPrevState)
            return;

        if (moveCoroutine != null)
            StopCoroutine(moveCoroutine);

        // 저장된 상태로 복귀 애니메이션 + 스크롤 입력 원복
        moveCoroutine = StartCoroutine(RestoreFocusRoutine(duration));
    }

    private IEnumerator RestoreFocusRoutine(float duration)
    {
        yield return StartCoroutine(SmoothFocus(prevNormalized, prevMapPos, prevMapScale.x, duration));

        if (scrollRect != null)
            scrollRect.enabled = prevScrollEnabled;

        hasPrevState = false;
    }
}
