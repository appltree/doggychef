using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering.Universal; // Light2D 사용을 위한 임포트

[RequireComponent(typeof(BoardBlockManager))]
[RequireComponent(typeof(BorderBlockDestroyer))]
public class BoardBombExploder : MonoBehaviour
{
    [Header("Effects")]
    [SerializeField] private GameObject tntEffectPrefab;
    [SerializeField] private GameObject beamEffectPrefab;

    private readonly float bombWaveInterval = 0.05f; // 기본 파동 간격(초)
    private readonly float waveSpeedScale = 3f;

    private BoardBlockManager board;
    private BorderBlockDestroyer borderBlockDestroyer;
    private readonly HashSet<int> explodingBombIds = new();

    void Awake()
    {
        board = GetComponent<BoardBlockManager>();
        borderBlockDestroyer = GetComponent<BorderBlockDestroyer>();
    }


    // LightBall을 선택한 후, 사용자가 특정 블록을 지정했을 때 해당 블록의 태그를 목표로 폭발을 일으킵니다.
    public IEnumerator TriggerLightBallExplosionWithTarget(BombBlock lightBall, Block selectedBlock)
    {
        if (lightBall == null || selectedBlock == null || board == null)
            yield break;

        // 동시 트리거 방지
        int id = lightBall.GetInstanceID();
        if (!explodingBombIds.Add(id))
            yield break;

        try
        {
            // 타겟 태그 획득
            string targetTag = selectedBlock.BlockTag;

            // LightBall 위치
            Vector2Int bombPosition = board.GetBlockPosition(lightBall);
            if (bombPosition.x < 0)
            {
                yield break;
            }

            // 보드에서 같은 태그의 모든 타겟 수집
            var targets = new List<Block>();
            for (int x = 0; x < board.Width; x++)
            {
                for (int y = 0; y < board.Height; y++)
                {
                    var b = board.GetBlockAtPosition(x, y);
                    if (b == null) continue;
                    if (b.BlockTag == targetTag) targets.Add(b);
                }
            }

            int total = 1 + targets.Count; // LightBall 자신 + 타겟들
            if (total <= 0)
                yield break;

            board.BeginManualDestructionPhase(total);

            // LightBall 제거
            borderBlockDestroyer.DestroyOneBlock(lightBall, true);
            yield return new WaitForSeconds(EffectiveInterval);

            // 맨해튼 버켓으로 파동 제거
            var buckets = BoardHelpers.BucketByManhattan(board, bombPosition, targets);
            int maxD = 0;
            foreach (var k in buckets.Keys) if (k > maxD) maxD = k;
            for (int d = 0; d <= maxD; d++)
            {
                if (!buckets.TryGetValue(d, out var list) || list.Count == 0) continue;
                foreach (var t in list)
                {
                    if (t == null) continue;
                    else borderBlockDestroyer.DestroyOneBlockByLightball(t);
                }
                if (d < maxD) yield return null;
            }
        }
        finally
        {
            explodingBombIds.Remove(id);
        }
    }


    public IEnumerator TriggerTntExplosionAtPosition(Vector2Int center, bool destroyCenter = false)
    {
        Block centerBlock = null;
        if (destroyCenter)
            centerBlock = board.GetBlockAtPosition(center.x, center.y);
        yield return StartCoroutine(ExplodeTntWaves(center, centerBlock, destroyCenter));
    }


    public IEnumerator TriggerHorizontalExplosionAtPosition(Vector2Int center)
    {
        SoundManager.I?.PlayFire();
        // 빔 이펙트 시작 (오른쪽/왼쪽)
        TryStartRowBeams(center);

        int y = center.y;
        // 총 제거 수 계산 (중앙 제외)
        int rowCount = 0;
        for (int x = 0; x < board.Width; x++)
        {
            if (x == center.x) continue;
            var b = board.GetBlockAtPosition(x, y);
            if (b != null && b is not BombBlock) rowCount++;
        }
        if (rowCount <= 0) yield break;

        board.BeginManualDestructionPhase(rowCount);

        int centerX = center.x;
        var wait = new WaitForSeconds(EffectiveInterval);
        for (int d = 1; d < Mathf.Max(centerX + 1, board.Width - centerX); d++)
        {
            bool any = false;
            int rx = centerX + d;
            if (rx < board.Width)
            {
                var right = board.GetBlockAtPosition(rx, y);
                if (right != null && right is not BombBlock)
                {
                    borderBlockDestroyer.DestroyOneBlock(right, true);
                    any = true;
                }
            }
            int lx = centerX - d;
            if (lx >= 0)
            {
                var left = board.GetBlockAtPosition(lx, y);
                if (left != null && left is not BombBlock)
                {
                    borderBlockDestroyer.DestroyOneBlock(left, true);
                    any = true;
                }
            }
            if (any) yield return wait;
        }
    }


    public IEnumerator TriggerVerticalExplosionAtPosition(Vector2Int center)
    {
        SoundManager.I?.PlayFire();
        // 빔 이펙트 시작 (위/아래)
        TryStartColumnBeams(center);

        int x = center.x;
        // 총 제거 수 계산 (중앙 제외)
        int colCount = 0;
        for (int y = 0; y < board.Height; y++)
        {
            if (y == center.y) continue;
            var b = board.GetBlockAtPosition(x, y);
            if (b != null && b is not BombBlock) colCount++;
        }
        if (colCount <= 0) yield break;

        board.BeginManualDestructionPhase(colCount);

        int centerY = center.y;
        var wait = new WaitForSeconds(EffectiveInterval);
        for (int d = 1; d < Mathf.Max(centerY + 1, board.Height - centerY); d++)
        {
            bool any = false;
            int uy = centerY + d;
            if (uy < board.Height)
            {
                var up = board.GetBlockAtPosition(x, uy);
                if (up != null && up is not BombBlock)
                {
                    borderBlockDestroyer.DestroyOneBlock(up, true);
                    any = true;
                }
            }
            int dy = centerY - d;
            if (dy >= 0)
            {
                var down = board.GetBlockAtPosition(x, dy);
                if (down != null && down is not BombBlock)
                {
                    borderBlockDestroyer.DestroyOneBlock(down, true);
                    any = true;
                }
            }
            if (any) yield return wait;
        }
    }


    // Double(2x2) 머지형 폭탄 처리: 지정된 그룹을 병합 연출 후 제거한다.
    // BoardBombSpawner에서 지연 후 호출된다.
    public IEnumerator TriggerDoubleExplosionWithGroup(List<Block> group, int ingredientCount = 8)
    {
        if (group == null || group.Count == 0)
            yield break;

        int validCount = 0;
        foreach (var b in group)
        {
            if (b != null) validCount++;
        }
        if (validCount == 0)
            yield break;

        // 수동 파괴 단계 시작: 그룹 내 유효 블록 수만큼 등록
        board.BeginManualDestructionPhase(validCount);

        // 요구사항: 얼음이 있는 블록은 이동(머지 연출)에 참여하지 않고 얼음만 깨진다.
        // - 얼음 블록: 제자리에서 Ice만 파괴하여 pendingRemovals를 감소시킴
        // - 비얼음 블록: 기존 더블 머지 연출에 참여 후 파괴
        var iced = new List<Block>();
        var normal = new List<Block>();
        foreach (var b in group)
        {
            if (b == null) continue;
            if (b.HasIce) iced.Add(b);
            else normal.Add(b);
        }

        // 얼음만 깨기 (보드 상태/그리드는 유지됨)
        if (iced.Count > 0)
        {
            foreach (var ib in iced)
            {
                // DestroyOneBlock은 HasIce면 블록 파괴 대신 얼음만 깨고 OnBlockRemoved를 호출한다.
                borderBlockDestroyer.DestroyOneBlock(ib, true);
            }
        }

        // 남은 블록만 머지 연출 수행
        if (normal.Count > 0)
        {
            borderBlockDestroyer.DestroyDoubleGroup(normal, ingredientCount);
        }

        yield break;
    }


    private float EffectiveInterval
    {
        get
        {
            float scale = waveSpeedScale;
            if (scale < 0.01f) scale = 0.01f;
            return bombWaveInterval * scale;
        }
    }


    private IEnumerator ExplodeTntWaves(Vector2Int center, Block centerBlock, bool destroyCenter)
    {
        TrySpawnTntCenterEffect(center);

        int total = 0;
        for (int d = 0; d <= 2; d++)
        {
            foreach (var p in BoardHelpers.ChebyshevRing(board, center, d))
            {
                if (!destroyCenter && d == 0) continue;
                var b = board.GetBlockAtPosition(p.x, p.y);
                if (b != null && b is not BombBlock) total++;
            }
        }
        if (total == 0)
        {
            yield break;
        }

        board.BeginManualDestructionPhase(total);

        int maxRing = 2;
        var wait = new WaitForSeconds(EffectiveInterval);
        for (int d = 0; d <= maxRing; d++)
        {
            bool any = false;
            if (d == 0)
            {
                if (destroyCenter && centerBlock != null)
                {
                    borderBlockDestroyer.DestroyOneBlock(centerBlock, true);
                    any = true;
                }
            }
            else
            {
                foreach (var p in BoardHelpers.ChebyshevRing(board, center, d))
                {
                    Block target = board.GetBlockAtPosition(p.x, p.y);
                    if (target == null || target is BombBlock) continue;
                    else borderBlockDestroyer.DestroyOneBlock(target, true);
                    any = true;
                }
            }
            if (any && d < maxRing) yield return wait;
        }
    }


    private void TrySpawnTntCenterEffect(Vector2Int center)
    {
        if (tntEffectPrefab == null) return;
        var go = Instantiate(tntEffectPrefab, transform);
        go.transform.localPosition = new Vector3(center.x, center.y, 0f);

        // 자식에서 Light2D 컴포넌트 찾기
        var light2D = go.GetComponentInChildren<Light2D>();
        if (light2D != null)
        {
            StartCoroutine(TntLight2DExplosionAnim(light2D));
        }

        // 파티클이 자동으로 사라지지 않는 경우를 대비해 안전 타임아웃
        Destroy(go, 1.0f);
        SoundManager.I?.PlayBomb();

    }

    // TNT 폭발 Light2D 애니메이션 코루틴
    private IEnumerator TntLight2DExplosionAnim(Light2D light2D)
    {
        // 초기값 저장
        float startIntensity = light2D.intensity;
        float startRadius = light2D.pointLightOuterRadius;
        float peakIntensity = Mathf.Max(3f, startIntensity * 2f);
        float peakRadius = Mathf.Max(5f, startRadius * 1.5f);

        // 빠르게 강해짐
        float upTime = 0.12f;
        float timer = 0f;
        while (timer < upTime)
        {
            light2D.intensity = Mathf.Lerp(startIntensity, peakIntensity, timer / upTime);
            light2D.pointLightOuterRadius = Mathf.Lerp(startRadius, peakRadius, timer / upTime);
            timer += Time.deltaTime;
            yield return null;
        }
        light2D.intensity = peakIntensity;
        light2D.pointLightOuterRadius = peakRadius;

        // 서서히 약해짐
        float fadeTime = 0.3f;
        float fadeTimer = 0f;
        while (fadeTimer < fadeTime)
        {
            light2D.intensity = Mathf.Lerp(peakIntensity, 0f, fadeTimer / fadeTime);
            light2D.pointLightOuterRadius = Mathf.Lerp(peakRadius, 0f, fadeTimer / fadeTime);
            fadeTimer += Time.deltaTime;
            yield return null;
        }
        light2D.intensity = 0f;
        light2D.pointLightOuterRadius = 0f;
    }

    private void TryStartRowBeams(Vector2Int center)
    {
        if (beamEffectPrefab == null) return;
        int maxLeft = center.x - 0; // 좌측 끝까지 칸 수
        int maxRight = (board.Width - 1) - center.x; // 우측 끝까지 칸 수
        float interval = EffectiveInterval;
        float speed = (interval > 0f) ? (1f / interval) : 9999f; // 칸/초

        // 오른쪽(+X): 회전 0, flipX=false
        StartCoroutine(AnimateBeam(center, new Vector2(1f, 0f), maxRight, false, 0f, speed));
        // 왼쪽(-X): 회전 0, flipX=true
        StartCoroutine(AnimateBeam(center, new Vector2(-1f, 0f), maxLeft, true, 0f, speed));
    }

    private void TryStartColumnBeams(Vector2Int center)
    {
        if (beamEffectPrefab == null) return;
        int maxDown = center.y - 0; // 아래 끝까지 칸 수
        int maxUp = (board.Height - 1) - center.y; // 위 끝까지 칸 수
        float interval = EffectiveInterval;
        float speed = (interval > 0f) ? (1f / interval) : 9999f; // 칸/초

        // 위(+Y): 90도 회전, flipX=false
        StartCoroutine(AnimateBeam(center, new Vector2(0f, 1f), maxUp, false, 90f, speed));
        // 아래(-Y): -90도 회전, flipX=false
        StartCoroutine(AnimateBeam(center, new Vector2(0f, -1f), maxDown, false, -90f, speed));
    }

    private IEnumerator AnimateBeam(Vector2Int center, Vector2 dir, int cells, bool flipX, float rotZ, float unitsPerSec)
    {
        if (cells <= 0) yield break;
        var go = Instantiate(beamEffectPrefab, transform);
        var tr = go.transform as Transform;
        tr.localPosition = new Vector3(center.x, center.y, 0f);
        tr.localRotation = Quaternion.Euler(0f, 0f, rotZ);
        var ls = Vector3.one;
        if (flipX) ls.x = -1f;
        tr.localScale = ls;

        float totalDist = cells; // 칸 수 == 이동할 유닛 수
        float traveled = 0f;
        var localDir = (Vector3)dir;
        while (traveled < totalDist && go != null)
        {
            float step = unitsPerSec * Time.deltaTime;
            traveled = Mathf.Min(traveled + step, totalDist);
            tr.localPosition = new Vector3(center.x, center.y, 0f) + localDir * traveled;
            yield return null;
        }

        // 끝에 도달하면 즉시 제거
        if (go != null) Destroy(go);
    }
}
