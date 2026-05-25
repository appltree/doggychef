using System.Collections;
using UnityEngine;

namespace MyMatch3
{
    /// <summary>
    /// 캔디 부스터 도착 시 재생되는 하트 폭발 이펙트.
    ///
    /// ■ Legacy 재현 (effect_explodingheart.ccb)
    ///   8방향(상/하/좌/우 + 대각선 4방향) 하트 스프라이트가
    ///   중심에서 바깥으로 터지며 서서히 사라지는 0.5초 연출입니다.
    ///
    ///   각 하트:
    ///     - 위치: 중심 근처 → 바깥쪽으로 이동 (cardinal: 0.3→1.0u / diagonal: 0.2→0.7u)
    ///     - 스케일: 1.0 → 1.5 (고정)
    ///     - 투명도: 1.0 → 0.0
    ///
    /// ■ 사용법
    ///   CandyBooster.HeartEffectPrefab 에 이 스크립트가 붙은 프리팹을 연결합니다.
    ///   연출 완료 후 자동으로 Destroy됩니다.
    /// </summary>
    public class ExplodingHeartEffect : MonoBehaviour
    {
        [Tooltip("하트 스프라이트. heart.png 또는 game_ui_effect_heart 스프라이트를 연결하세요.")]
        public Sprite HeartSprite;

        [Tooltip("하트 렌더링 레이어 이름 (손님 위에 표시되도록 설정).")]
        public string SortingLayerName = "Default";

        [Tooltip("하트 렌더링 Order (손님 Spine renderer보다 크게 설정).")]
        public int SortingOrder = 32000;

        // ──────────────────────────────────────────────────────────────
        //  CCB에서 파싱한 8방향 설정
        //  (startX, startY, endX, endY, rotationDegrees)
        //  Legacy 단위: 픽셀 → Unity 월드 유닛 ≈ 100px/u 기준으로 환산
        // ──────────────────────────────────────────────────────────────

        private static readonly (float sx, float sy, float ex, float ey, float rot)[] k_Configs =
        {
            ( 0.30f,  0.00f,  1.00f,  0.00f,  90f), // Right  (→)
            ( 0.00f, -0.30f,  0.00f, -1.00f, 180f), // Down   (↓)
            (-0.30f,  0.00f, -1.00f,  0.00f, 270f), // Left   (←)
            ( 0.00f,  0.30f,  0.00f,  1.00f,   0f), // Up     (↑)
            ( 0.20f,  0.20f,  0.70f,  0.70f,  45f), // NE     (↗)
            ( 0.20f, -0.20f,  0.70f, -0.70f, 135f), // SE     (↘)
            (-0.20f, -0.20f, -0.70f, -0.70f, 225f), // SW     (↙)
            (-0.20f,  0.20f, -0.70f,  0.70f, 315f), // NW     (↖)
        };

        private void Start() => StartCoroutine(PlayRoutine());

        private IEnumerator PlayRoutine()
        {
            const float duration = 0.5f;
            const int count = 8;

            // ── 하트 SpriteRenderer 8개 생성 ──
            var renderers = new SpriteRenderer[count];
            var startPositions = new Vector3[count];
            var endPositions = new Vector3[count];

            for (int i = 0; i < count; i++)
            {
                var cfg = k_Configs[i];

                var go = new GameObject($"Heart_{i}");
                go.transform.SetParent(transform, false);

                var sr = go.AddComponent<SpriteRenderer>();
                sr.sprite = HeartSprite;
                sr.sortingLayerName = SortingLayerName;
                sr.sortingOrder = SortingOrder;

                go.transform.localRotation = Quaternion.Euler(0f, 0f, cfg.rot);
                go.transform.localScale = Vector3.one;

                startPositions[i] = new Vector3(cfg.sx, cfg.sy, 0f);
                endPositions[i] = new Vector3(cfg.ex, cfg.ey, 0f);
                go.transform.localPosition = startPositions[i];

                renderers[i] = sr;
            }

            // ── 0.5초 동안 이동 + 스케일 + 페이드 ──
            float elapsed = 0f;
            while (elapsed < duration)
            {
                elapsed += Time.deltaTime;
                float t = Mathf.Clamp01(elapsed / duration);

                float alpha = 1f - t;

                for (int i = 0; i < count; i++)
                {
                    if (renderers[i] == null) continue;
                    renderers[i].transform.localPosition =
                        Vector3.Lerp(startPositions[i], endPositions[i], t);
                    renderers[i].transform.localScale = Vector3.one;
                    renderers[i].color = new Color(1f, 1f, 1f, alpha);
                }

                yield return null;
            }

            Destroy(gameObject);
        }
    }
}
