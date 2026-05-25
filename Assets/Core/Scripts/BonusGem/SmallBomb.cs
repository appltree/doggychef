using System.Collections;
using System.Collections.Generic;
using System.Linq;
using TMPro;
using UnityEngine;

namespace MyMatch3
{
    /// <summary>
    /// 스몰봄(SmallBomb) 보너스 젬.
    ///
    /// ■ 2×2 머지 모드 (생성 조건)
    ///   → 2×2 정사각형 매치 시 Board가 생성합니다.
    ///   → Board.MatchTicking 이 InitMergeEffect() 를 호출해 소스 데이터를 주입합니다.
    ///   → Start() 에서 Use() 를 자동 호출해 연출을 시작합니다.
    ///
    ///   연출 순서:
    ///     [1] 보드 이동 Lock
    ///     [2] 고스트 4개가 중앙으로 합쳐짐 (0.2s SmoothStep)
    ///     [3] 스케일 1→2배 확대 (0.15s SmoothStep)
    ///     [4] 고스트 제거 + SmallBomb 자신을 보드에서 파괴
    ///     [5] "x8" 금색 팝업 상승 + 페이드아웃 (0.8s)
    ///     [6] 지연 폭발 이벤트 발화 → 재료 토큰 날아가기
    ///     [7] 보드 이동 Unlock
    ///
    ///   소스 젬 4개는 ExplodeCount=2 → 재료 배달 8회(=x8)
    ///
    /// ■ 일반 모드 (보드 위 스왑 / 더블클릭)
    ///   → 자신을 포함한 상·하·좌·우 4개 보석을 즉시 파괴합니다.
    /// </summary>
    public class SmallBomb : BonusGem
    {
        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────

        [Tooltip("발동 시 재생할 효과음")]
        public AudioClip TriggerSound;

        // ──────────────────────────────────────────────────────────
        //  2×2 머지 전용 내부 상태
        // ──────────────────────────────────────────────────────────

        private bool m_IsMergeType;
        private List<(Vector3 pos, Sprite sprite)> m_SourceGems;
        private List<(string tag, Vector3 pos, Sprite sprite, AudioClip foodSfx, int count)> m_PendingExplosions;

        // ================================================================
        //  Unity 생명 주기
        // ================================================================

        public override void Awake()
        {
            m_Usable = true;
        }

        /// <summary>
        /// 머지 모드일 때 Start()에서 자동 발동합니다.
        /// Board.MatchTicking 이 InitMergeEffect()를 주입한 뒤 1프레임 후 호출됩니다.
        /// </summary>
        private void Start()
        {
            if (m_IsMergeType)
                Use(null, false);
        }

        // ================================================================
        //  공개 API
        // ================================================================

        /// <summary>
        /// Board.MatchTicking 이 SpawnedBonus 생성 직후 호출합니다.
        /// 소스 젬 시각 데이터와 지연 폭발 데이터를 주입합니다.
        /// </summary>
        public void InitMergeEffect(
            List<(Vector3 pos, Sprite sprite)> sourceGems,
            List<(string tag, Vector3 pos, Sprite sprite, AudioClip foodSfx, int count)> pendingExplosions)
        {
            m_IsMergeType = true;
            m_SourceGems = sourceGems;
            m_PendingExplosions = pendingExplosions;

            // 머지 모드에서는 SmallBomb 자신의 스프라이트가 보이면 안 됩니다.
            // NewGemAt() 직후 바로 호출되므로 단 한 프레임도 표시되지 않습니다.
            foreach (var sr in GetComponentsInChildren<SpriteRenderer>())
                sr.enabled = false;
        }

        /// <summary>
        /// 보너스 젬 발동 진입점.
        /// m_IsMergeType 여부에 따라 머지 모드 또는 일반 십자형 폭발 중 하나를 실행합니다.
        /// </summary>
        public override void Use(Gem swappedGem, bool isBonus = true)
        {
            // 보드 위 젬의 재귀 폭발 방지
            if (!isBonus && m_Used) return;
            m_Used = true;

            if (m_IsMergeType)
                UseMergeMode();
            else
                UseExplosionMode();
        }

        // ================================================================
        //  내부 — 모드별 발동 로직
        // ================================================================

        /// <summary>2×2 머지 모드: 고스트 애니메이션 후 보드 파괴 + 지연 이벤트.</summary>
        private void UseMergeMode()
        {
            GameManager.Instance.PlaySFX(TriggerSound);

            // 머지 애니메이션이 끝날 때까지 보드를 잠급니다.
            GameManager.Instance.Board.LockMovement();

            // 소스 4개 젬의 무게중심을 중앙 좌표로 사용합니다.
            var center = Vector3.zero;
            foreach (var (pos, _) in m_SourceGems)
                center += pos;
            center /= m_SourceGems.Count;

            // SmallBomb 자신이 Destroy되어도 Board가 코루틴을 호스팅하므로 중단되지 않습니다.
            GameManager.Instance.Board.StartCoroutine(
                MergeAndPopupRoutine(m_SourceGems, center, m_CurrentIndex, m_PendingExplosions));
        }

        /// <summary>일반 모드: 자신 + 상·하·좌·우 4방향 보석을 즉시 파괴합니다.</summary>
        private void UseExplosionMode()
        {
            var board = GameManager.Instance.Board;
            var newMatch = board.CreateCustomMatch(m_CurrentIndex);
            newMatch.ForcedDeletion = true;

            // 자기 자신 처리
            HandleContent(board.CellContent[m_CurrentIndex], newMatch);

            GameManager.Instance.PlaySFX(TriggerSound);

            // 상·하·좌·우 4방향 인접 보석 처리
            Vector3Int[] neighbors =
            {
                m_CurrentIndex + Vector3Int.left,
                m_CurrentIndex + Vector3Int.right,
                m_CurrentIndex + Vector3Int.up,
                m_CurrentIndex + Vector3Int.down
            };

            foreach (var idx in neighbors)
            {
                if (board.CellContent.TryGetValue(idx, out var content) &&
                    content.ContainingGem != null)
                {
                    HandleContent(content, newMatch);
                }
            }
        }

        // ================================================================
        //  2×2 머지 연출 코루틴
        // ================================================================

        /// <summary>
        /// 고스트 이동·확대, SmallBomb 파괴, x8 팝업, 지연 폭발 이벤트 발화를 순서대로 실행합니다.
        ///
        ///   [1] 고스트 4개 생성 → 중앙으로 합쳐짐 (0.2s SmoothStep)
        ///   [2] 스케일 1→2배 확대 (0.15s SmoothStep)
        ///   [3] 고스트 제거 + SmallBomb 보드에서 제거
        ///   [4] "x8" 금색 팝업 0.8s 상승 + 페이드아웃
        ///   [5] 지연 폭발 이벤트 발화
        ///   [6] 보드 Unlock
        /// </summary>
        private static IEnumerator MergeAndPopupRoutine(
            List<(Vector3 pos, Sprite sprite)> sources,
            Vector3 center,
            Vector3Int bombCell,
            List<(string tag, Vector3 pos, Sprite sprite, AudioClip foodSfx, int count)> pendingExplosions)
        {
            // ── [1] 고스트 스프라이트 생성 ──
            var ghosts = new List<(GameObject go, Vector3 start)>(sources.Count);
            foreach (var (pos, sprite) in sources)
            {
                var go = new GameObject("BombGhost");
                var sr = go.AddComponent<SpriteRenderer>();
                sr.sprite = sprite;
                sr.sortingLayerID = 1890678711;  // MyGame Gem 레이어
                sr.sortingOrder = 12;
                go.transform.position = pos;
                go.transform.localScale = Vector3.one;
                ghosts.Add((go, pos));
            }

            // ── [2] 중앙으로 날아가기 (0.2s SmoothStep) ──
            yield return AnimateGhosts(ghosts, center, 0.2f, flyToCenter: true);

            // ── [3] 스케일 1→2배 확대 (0.15s SmoothStep) ──
            yield return ScaleGhosts(ghosts, from: 1f, to: 2f, duration: 0.15f);

            // ── [4] 고스트 제거 + SmallBomb 보드에서 파괴 ──
            foreach (var (go, _) in ghosts)
                Object.Destroy(go);

            GameManager.Instance.Board.DestroyGem(bombCell, true);

            // ── [5] 동적 TextMeshPro 팝업 (0.8s 상승 + 페이드아웃) ──
            int totalGenerated = (pendingExplosions != null) ? pendingExplosions.Sum(p => p.count) : 8;
            yield return ShowMultiplierPopup(center, $"x{totalGenerated}", 0.8f, rise: 1.5f);

            // ── [6] 지연 폭발 이벤트 발화 ──
            // 머지 연출이 완전히 끝난 후 재료 토큰이 레시피 슬롯으로 날아갑니다.
            if (pendingExplosions != null)
            {
                foreach (var (tag, pos, sprite, foodSfx, count) in pendingExplosions)
                {
                    if (string.IsNullOrEmpty(tag)) continue;
                    for (int i = 0; i < count; i++)
                        Board.FireGemExplodedDelayed(tag, pos, sprite, foodSfx);
                }
            }

            // ── [7] 보드 Unlock ──
            GameManager.Instance.Board.UnlockMovement();
        }

        // ──────────────────────────────────────────────────────────
        //  코루틴 헬퍼
        // ──────────────────────────────────────────────────────────

        /// <summary>고스트들을 <paramref name="center"/>로(또는 중앙에서 제자리로) 이동시킵니다.</summary>
        private static IEnumerator AnimateGhosts(
            List<(GameObject go, Vector3 start)> ghosts, Vector3 center,
            float duration, bool flyToCenter)
        {
            float t = 0f;
            while (t < duration)
            {
                t += Time.deltaTime;
                float f = Mathf.SmoothStep(0f, 1f, Mathf.Clamp01(t / duration));
                foreach (var (go, start) in ghosts)
                    go.transform.position = flyToCenter
                        ? Vector3.Lerp(start, center, f)
                        : Vector3.Lerp(center, start, f);
                yield return null;
            }

            // 목표 위치로 스냅
            foreach (var (go, start) in ghosts)
                go.transform.position = flyToCenter ? center : start;
        }

        /// <summary>고스트들의 스케일을 <paramref name="from"/> → <paramref name="to"/>로 변환합니다.</summary>
        private static IEnumerator ScaleGhosts(
            List<(GameObject go, Vector3 start)> ghosts,
            float from, float to, float duration)
        {
            float t = 0f;
            while (t < duration)
            {
                t += Time.deltaTime;
                float f = Mathf.SmoothStep(0f, 1f, Mathf.Clamp01(t / duration));
                float s = Mathf.Lerp(from, to, f);
                var scale = new Vector3(s, s, 1f);
                foreach (var (go, _) in ghosts)
                    go.transform.localScale = scale;
                yield return null;
            }
        }

        /// <summary>"x8" 같은 배율 텍스트를 생성하고 상승 + 페이드아웃한 뒤 파괴합니다.</summary>
        private static IEnumerator ShowMultiplierPopup(
            Vector3 origin, string text, float duration, float rise)
        {
            var popupGo = new GameObject("MultiplierPopup");
            popupGo.transform.position = origin;

            var tmp = popupGo.AddComponent<TextMeshPro>();
            tmp.text = text;
            tmp.fontSize = 5f;
            tmp.fontStyle = FontStyles.Bold;
            tmp.color = new Color(1f, 0.92f, 0.2f, 1f);  // 금색
            tmp.alignment = TextAlignmentOptions.Center;
            tmp.sortingLayerID = 1890678711;
            tmp.sortingOrder = 20;

            var endPos = origin + Vector3.up * rise;
            float t = 0f;
            while (t < duration)
            {
                t += Time.deltaTime;
                float f = Mathf.Clamp01(t / duration);
                popupGo.transform.position = Vector3.Lerp(origin, endPos, f);
                var col = tmp.color;
                col.a = 1f - f;
                tmp.color = col;
                yield return null;
            }

            Object.Destroy(popupGo);
        }
    }
}
