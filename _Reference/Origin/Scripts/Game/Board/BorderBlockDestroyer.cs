// OK
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

[RequireComponent(typeof(BoardBlockManager))]
public class BorderBlockDestroyer : MonoBehaviour
{
    // 블록 폭발 이벤트
    public static event Action<Block, Vector3> OnBlockExploded;
    // 블록이 완전히 제거(보드에서 Unregister)된 후 알림
    public static event Action<Vector2Int> OnBlockRemoved;

    private BoardBlockManager blockManager;
    // 중복 파괴 연출 방지: 이미 파괴 절차에 들어간 블록 추적
    private readonly HashSet<Block> destroyingBlocks = new();

    public const float BlockDestroyDuration = 0.2f;
    [SerializeField] private GameObject pangEffectPrefab;
    [SerializeField] private GameObject lightballEffectPrefab;
    [SerializeField] private TMP_FontAsset ingredientPopupFont; // Optional: 팝업 숫자 폰트 지정

    private readonly float lightballDestroyDelay = 1.0f;  // 라이트볼 파괴 지연 시간
    private readonly float lightballEffectLifetime = 1.5f; // 라이트볼 이펙트 수명


    void Awake()
    {
        blockManager = GetComponent<BoardBlockManager>();
    }


    public void DestroyOneBlock(Block block, bool byBomb = false)
    {
        if (block == null) return;
        // 이미 파괴 중인 블록이면 연출/이펙트를 중복으로 내지 않음
        if (!destroyingBlocks.Add(block))
        {
            // 얼음만 있는 경우는 별도 처리해야 하므로, HasIce였던 케이스는 최초 호출에서 처리되었을 것이고
            // 이후 중복 호출은 조용히 무시한다.
            return;
        }
        Vector3 blockPosition = block.transform.localPosition;

        // 얼음이 있으면 블록은 파괴하지 않고 얼음만 깨기
        if (block != null && block.HasIce)
        {
            block.BreakIce();
            Vector2Int bp = BoardHelpers.WorldToBoard(blockPosition);
            OnBlockRemoved?.Invoke(bp); // pendingRemovals은 감소시켜야 함으로 호출
            destroyingBlocks.Remove(block);
            return;
        }


        // 폭탄 유발 파괴라면 플래그 설정 (폭탄 생성 로직에서 제외하기 위함)
        if (byBomb)
        {
            block.MarkExplodedByBomb();
        }

        // 블록 폭발 이벤트 발생
        OnBlockExploded?.Invoke(block, blockPosition);


        // Pang Effect 생성
        if (!byBomb)
        {
            SoundManager.I.PlayBlockPang();
        }
        GameObject effect = Instantiate(pangEffectPrefab, block.transform.position, Quaternion.identity);
        Destroy(effect, BlockDestroyDuration);

        block.DestroyBlock(() =>
        {
            blockManager.UnregisterBlock(blockPosition);
            Vector2Int bp = BoardHelpers.WorldToBoard(blockPosition);
            OnBlockRemoved?.Invoke(bp);
            destroyingBlocks.Remove(block);
        });
    }



    public void DestroyOneBlockByLightball(Block block)
    {
        if (block == null) return;
        if (!destroyingBlocks.Add(block))
        {
            return;
        }
        Vector3 blockPosition = block.transform.localPosition;

        // 얼음이 있으면 블록은 파괴하지 않고 얼음만 깨기
        if (block != null && block.HasIce)
        {
            block.BreakIce();
            Vector2Int bp = BoardHelpers.WorldToBoard(blockPosition);
            OnBlockRemoved?.Invoke(bp); // pendingRemovals은 감소시켜야 함으로 호출
            destroyingBlocks.Remove(block);
            return;
        }


        // 라이트볼에 의한 파괴도 폭탄 생성에서 제외
        block.MarkExplodedByBomb();

        SoundManager.I.PlayHalo();

        block.transform.localScale = Vector3.zero;

        SpawnLightballEffect(block);

        // 지연 후 DestroyBlock 호출 (라이트볼 전용 연출 지연)
        StartCoroutine(DelayedDestroyBlock(block, lightballDestroyDelay, () =>
        {
            blockManager.UnregisterBlock(blockPosition);
            Vector2Int bp = BoardHelpers.WorldToBoard(blockPosition);
            OnBlockRemoved?.Invoke(bp);
            destroyingBlocks.Remove(block);
        }));
    }

    private IEnumerator DelayedDestroyBlock(Block block, float delay, Action onComplete)
    {
        if (block == null)
            yield break;
        yield return new WaitForSeconds(delay);

        Vector3 blockPosition = block.transform.localPosition;
        block.MarkExplodedByBomb();
        OnBlockExploded?.Invoke(block, blockPosition);
        if (block == null)
            yield break;
        block.DestroyBlock(onComplete);
    }

    private void SpawnLightballEffect(Block block)
    {
        if (lightballEffectPrefab == null || block == null) return;
        var effect = Instantiate(lightballEffectPrefab, block.transform.position, Quaternion.identity);
        // 블록의 스프라이트를 이펙트의 Ingredient 자식에 적용
        var blockSR = block.GetComponentInChildren<SpriteRenderer>();
        if (blockSR != null && blockSR.sprite != null)
        {
            var ingredient = effect.transform.Find("Ingredient");
            if (ingredient != null)
            {
                var sr = ingredient.GetComponent<SpriteRenderer>();
                if (sr != null)
                {
                    sr.sprite = blockSR.sprite;
                    sr.enabled = true;
                }
            }
        }
        Destroy(effect, lightballEffectLifetime);
    }


    // Double(2x2) 그룹 전용 파괴 연출: 4블럭이 중앙으로 모였다가 메인 블럭이 커지면서 팡. 총 ingredient 8회 방출.
    public void DestroyDoubleGroup(List<Block> group, int ingredientCount = 8)
    {
        if (group == null) return;
        // 코루틴으로 연출 실행
        StartCoroutine(DestroyDoubleGroupRoutine(group, ingredientCount));
    }


    private IEnumerator DestroyDoubleGroupRoutine(List<Block> group, int ingredientCount)
    {
        // 방어 코드 및 유효 블록만 추림
        var valid = new List<Block>(4);
        foreach (var b in group)
        {
            if (b != null) valid.Add(b);
        }
        if (valid.Count == 0) yield break;

        // 원래 위치 캐시(보드 Unregister에 필요)
        var originalPositions = new Dictionary<Block, Vector3>(valid.Count);
        foreach (var b in valid)
            originalPositions[b] = b.transform.localPosition;

        // 중앙 위치 계산 (시각적 중심)
        Vector3 center = Vector3.zero;
        foreach (var b in valid) center += b.transform.localPosition;
        center /= valid.Count;

        // 머지 애니메이션 설정
        float mergeDuration = 0.15f;
        float scaleUpDuration = 0.12f;
        float overshootScale = 2.0f;

        // 시작 상태 저장
        var startPos = new Dictionary<Block, Vector3>(valid.Count);
        var startScale = new Dictionary<Block, Vector3>(valid.Count);
        foreach (var b in valid)
        {
            startPos[b] = b.transform.localPosition;
            startScale[b] = b.transform.localScale;
        }

        // 4개 블럭을 중앙으로 모으는 연출 (그리드 업데이트는 하지 않음: 파괴 직전의 순수 시각 효과)
        float t = 0f;
        while (t < mergeDuration)
        {
            t += Time.deltaTime;
            float u = Mathf.Clamp01(t / mergeDuration);
            float e = Mathf.SmoothStep(0, 1, u);
            foreach (var b in valid)
            {
                // 안전 체크 (파괴 중 외부에서 제거될 수 있음)
                if (b == null) continue;
                b.transform.localPosition = Vector3.Lerp(startPos[b], center, e);
            }
            yield return null;
        }

        // 보조 블럭은 살짝 축소/숨김 처리
        for (int i = 1; i < valid.Count; i++)
        {
            var b = valid[i];
            if (b == null) continue;
            var sr = b.GetComponent<SpriteRenderer>();
            if (sr != null) sr.enabled = false;
            else b.transform.localScale = Vector3.zero;
        }

        // 대표 블럭 선택 (첫번째)
        Block main = valid[0];
        // 대표 블럭 스케일 업(팡 직전 강조)
        Vector3 mainStartScale = startScale[main];
        t = 0f;
        while (t < scaleUpDuration)
        {
            t += Time.deltaTime;
            float u = Mathf.Clamp01(t / scaleUpDuration);
            float e = Mathf.SmoothStep(0, 1, u);
            if (main != null)
                main.transform.localScale = Vector3.Lerp(mainStartScale, mainStartScale * overshootScale, e);
            yield return null;
        }

        // ingredient 방출 (8회, 필요 없으면 수신 측에서 무시됨)
        for (int i = 0; i < Mathf.Max(ingredientCount, 1); i++)
        {
            OnBlockExploded?.Invoke(main, center);
        }
        main.transform.localPosition = center; // 계속 중앙에 위치

        // Pang 이펙트 1회
        SoundManager.I.PlayBlockPang();
        GameObject effect = Instantiate(pangEffectPrefab, transform);
        effect.transform.localPosition = center;
        Destroy(effect, BlockDestroyDuration);

        // 재료 개수를 멋지게 표시 (x8, x9 등)
        if (ingredientCount > 1)
        {
            TrySpawnIngredientCountPopup(center, ingredientCount);
        }

        // 4블럭 모두 정리 (보드에서 원래 좌표로 언레지스터)
        foreach (var b in valid)
        {
            if (b == null) continue;
            Vector3 op = originalPositions[b];
            b.DestroyBlock(() =>
            {
                blockManager.UnregisterBlock(op);
                Vector2Int bp = BoardHelpers.WorldToBoard(op);
                OnBlockRemoved?.Invoke(bp);
            });
        }
    }

    private void TrySpawnIngredientCountPopup(Vector3 worldCenter, int count)
    {
        // TextMeshPro를 런타임으로 생성하여 간단한 팝업 연출
        var go = new GameObject("IngredientCountPopup");
        go.transform.SetParent(transform, false);
        go.transform.localPosition = worldCenter + new Vector3(0f, 0.35f, 0f);

        var tmp = go.AddComponent<TextMeshPro>();
        if (ingredientPopupFont != null)
        {
            tmp.font = ingredientPopupFont;
        }
        tmp.text = $"x{Mathf.Max(count, 1)}";
        tmp.alignment = TextAlignmentOptions.Center;
        tmp.fontSize = 3.5f;
        tmp.textWrappingMode = TextWrappingModes.NoWrap;
        tmp.color = new Color(1f, 0.92f, 0.2f, 1f); // 골드 톤
        tmp.fontStyle = FontStyles.Bold;
        tmp.outlineWidth = 0.15f;
        tmp.outlineColor = new Color(0f, 0f, 0f, 0.75f);

        // 살짝 팝업하는 느낌의 애니메이션
        StartCoroutine(AnimatePopupAndDispose(tmp));
    }

    private IEnumerator AnimatePopupAndDispose(TextMeshPro tmp)
    {
        if (tmp == null) yield break;
        var tr = tmp.transform;
        float dur = 0.8f;
        float t = 0f;

        Vector3 startPos = tr.localPosition;
        Vector3 endPos = startPos + new Vector3(0f, 0.6f, 0f);
        Vector3 startScale = Vector3.one * 0.85f;
        Vector3 midScale = Vector3.one * 1.15f;
        Vector3 endScale = Vector3.one * 1.0f;

        // 초기 투명도 1 → 0으로 페이드
        Color c0 = tmp.color;
        Color c1 = new Color(c0.r, c0.g, c0.b, 0f);

        // 아웃라인도 같이 페이드
        Color o0 = tmp.outlineColor;
        Color o1 = new Color(o0.r, o0.g, o0.b, 0f);

        // 초반 팝업, 후반 페이드
        while (t < dur)
        {
            t += Time.deltaTime;
            float u = Mathf.Clamp01(t / dur);
            // 위치는 부드럽게 위로
            tr.localPosition = Vector3.Lerp(startPos, endPos, Mathf.SmoothStep(0, 1, u));

            // 스케일은 초반에 팝업 후 살짝 안정화
            float scaleU = (u < 0.3f) ? (u / 0.3f) : ((u - 0.3f) / 0.7f);
            if (u < 0.3f)
                tr.localScale = Vector3.Lerp(startScale, midScale, Mathf.SmoothStep(0, 1, scaleU));
            else
                tr.localScale = Vector3.Lerp(midScale, endScale, Mathf.SmoothStep(0, 1, scaleU));

            // 컬러 페이드
            tmp.color = Color.Lerp(c0, c1, u);
            tmp.outlineColor = Color.Lerp(o0, o1, u);

            yield return null;
        }

        if (tmp != null)
        {
            Destroy(tmp.gameObject);
        }
    }
}
