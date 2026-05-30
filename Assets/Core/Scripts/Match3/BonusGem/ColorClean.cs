using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace DoggyChef
{
    /// <summary>
    /// 컬러클린(ColorClean) 보너스 젬. (레인보우 젬)
    /// 발동 시 보드 위의 특정 색상(GemType)에 해당하는 보석을 전부 삭제합니다.
    ///
    /// ■ 삭제 색상 결정 규칙
    ///   1순위: 스왑한 상대 젬의 GemType
    ///   2순위: 스왑 없이 더블클릭한 경우 → 보드에서 가장 많이 분포한 GemType 자동 선택
    ///
    /// ■ 생성 조건
    ///   → 5개 이상 직선 매치 시 생성됩니다.
    ///
    /// ■ 폭발 VFX
    ///   → 삭제되는 각 보석 위치에 LightBallExplosion 프리팹을 생성합니다.
    ///   → 발동 후 EffectLifetime 동안 보드 이동을 잠급니다.
    ///
    /// ■ 외형 (Origin 참고)
    ///   → rainbowTransform이 천천히 회전하면서 사인파로 펄스합니다.
    /// </summary>
    public class ColorClean : BonusGem
    {
        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────

        [SerializeField]
        [Tooltip("회전·펄스 대상 Transform (Background 자식)")]
        private Transform rainbowTransform;

        [Tooltip("삭제되는 보석 위치에 생성할 폭발 이펙트 프리팹")]
        public GameObject LightBallExplosionPrefab;

        [Tooltip("발동 시 재생할 효과음")]
        public AudioClip TriggerSound;

        // ──────────────────────────────────────────────────────────
        //  상수 / 내부 상태
        // ──────────────────────────────────────────────────────────

        // 폭발 이펙트 표시 시간(초). 이 시간 동안 보드를 잠급니다.
        private const float EffectLifetime = 1.5f;

        // 이펙트 애니메이션 중 gem 스프라이트가 시각적으로 사라지는 시점(초).
        // 이 시점에 맞춰 재료가 테이블로 날아갑니다. 값을 조절해 타이밍을 조정할 수 있습니다.
        private const float GemDisappearTime = 0.6f;

        private readonly float rotationSpeed = 80f;
        private readonly float pulseAmplitude = 0.08f;  // 8%
        private readonly float pulseFrequency = 0.2f;

        private Vector3 m_RainbowBaseScale;

        // ──────────────────────────────────────────────────────────

        public override void Awake()
        {
            m_Usable = true;
        }

        private void Start()
        {
            if (rainbowTransform != null)
            {
                m_RainbowBaseScale = rainbowTransform.localScale;
                StartCoroutine(RunIdleVFX());
            }
        }

        // ──────────────────────────────────────────────────────────
        //  유휴 VFX — rainbow 회전 + 사인파 펄스
        // ──────────────────────────────────────────────────────────

        /// <summary>컬러클린이 보드 위에 놓여 있는 동안 rainbow를 지속 회전·펄스합니다.</summary>
        private IEnumerator RunIdleVFX()
        {
            float t = 0f;
            while (isActiveAndEnabled)
            {
                // 회전
                rainbowTransform.Rotate(0f, 0f, rotationSpeed * Time.deltaTime, Space.Self);

                // 사인파 → SmoothStep 펄스
                t += Time.deltaTime * pulseFrequency * Mathf.PI * 2f;
                float s01 = Mathf.SmoothStep(0f, 1f, (Mathf.Sin(t) + 1f) * 0.5f);
                float scaleMul = 1f + (s01 - 0.5f) * 2f * pulseAmplitude;
                rainbowTransform.localScale = m_RainbowBaseScale * scaleMul;

                yield return null;
            }
        }

        // ──────────────────────────────────────────────────────────
        //  발동 로직
        // ──────────────────────────────────────────────────────────

        /// <summary>
        /// 컬러클린 발동 메서드.
        /// </summary>
        /// <param name="swappedGem">스왑 상대 젬 → 이 GemType을 삭제 대상으로 사용.</param>
        /// <param name="isBonus">인벤토리 아이템 사용 여부.</param>
        public override void Use(Gem swappedGem, bool isBonus = true)
        {
            if (!isBonus && m_Used) return;
            m_Used = true;

            // ── Step 1: 삭제할 GemType 결정 ──
            int targetType = -1;
            if (swappedGem != null)
                targetType = swappedGem.GemType;

            // 스왑 대상이 보너스 젬(GemType < 0)이거나 제자리 터치인 경우 가장 많은 색상 찾기
            if (targetType < 0)
                targetType = FindMostFrequentGemType();

            // ── Step 2: 자기 자신 포함 매치 생성 ──
            var board = GameManager.Instance.Board;
            var newMatch = board.CreateCustomMatch(m_CurrentIndex);
            newMatch.ForcedDeletion = true;
            // gem이 시각적으로 사라지는 시점(GemDisappearTime)에 재료가 날아갑니다.
            // 0f = 즉시, EffectLifetime = 이펙트 완전 종료 후.
            newMatch.ExplodeDelay = GemDisappearTime;
            HandleContent(board.CellContent[m_CurrentIndex], newMatch);

            // ── Step 3: 지정 색상 보석 전체 매치에 추가 + 이펙트 생성 ──
            foreach (var (_, content) in board.CellContent)
            {
                if (content.ContainingGem?.GemType != targetType) continue;

                // ── 얼음으로 덮인 보석: 얼음만 깨고 보석은 살립니다 ──
                // Obstacle 슬롯이 아닌 gem.IsFrozen 으로 판별합니다.
                if (content.ContainingGem.IsFrozen)
                {
                    // 얼음 파괴 연출 (BreakAndVanish → 0.3s 후 Destroy)
                    content.ContainingGem.FrozenBy.BreakAndVanish();
                    // 보석은 매치에 추가하지 않으므로 Destroy 되지 않음
                    continue;
                }

                if (content.Obstacle != null)
                {
                    content.Obstacle.Damage(1);
                }
                else if (content.ContainingGem.CurrentMatch == null)
                {
                    HandleContent(content, newMatch);
                    SpawnExplosionEffect(content.ContainingGem);
                }
            }

            // ── Step 4: 효과음 재생 + 폭발 연출 동안 보드 잠금 ──
            GameManager.Instance.PlaySFX(TriggerSound);
            board.LockMovementFor(EffectLifetime);
        }

        // ──────────────────────────────────────────────────────────
        //  내부 헬퍼
        // ──────────────────────────────────────────────────────────

        /// <summary>주문에 포함된 레시피 젬 중 가장 체력이 낮은 손님의 타겟을 반환합니다. 보드에 없다면 가장 많은 색상을 반환합니다.</summary>
        private int FindMostFrequentGemType()
        {
            var board = GameManager.Instance.Board;

            // 1. 보드에 존재하는 색상 수집
            var availableGemTypes = board.CellContent.Values
                .Where(c => c.ContainingGem != null && c.ContainingGem.GemType >= 0)
                .Select(c => c.ContainingGem.GemType)
                .ToHashSet();

            // 2. 가장 급한 손님의 주문 재료 검색
            var tables = FindObjectsByType<Table>();
            if (tables != null && tables.Length > 0)
            {
                var urgentTables = tables
                    .Where(t => t.State == Table.TableState.Ordered && t.CurrentOrder != null)
                    .OrderBy(t => t.CustomerCurrentHealth);

                foreach (var table in urgentTables)
                {
                    foreach (var ingredient in table.CurrentOrder.Ingredients)
                    {
                        if (ingredient.MaterialGem != null)
                        {
                            string tag = ingredient.MaterialGem.BlockTag;
                            int type = ingredient.MaterialGem.GemType;

                            // 해당 재료가 아직 필요하고, 보드에 존재한다면 그 색상을 삭제 대상으로 지정
                            if (table.GetRemainingNeed(tag) > 0 && availableGemTypes.Contains(type))
                            {
                                return type;
                            }
                        }
                    }
                }
            }

            // 3. 주문 중인 젬이 없거나 보드에 없다면 기존처럼 가장 많은 색상을 반환
            return board.CellContent.Values
                .Where(c => c.ContainingGem != null && c.ContainingGem.GemType >= 0)
                .GroupBy(c => c.ContainingGem.GemType)
                .OrderByDescending(g => g.Count())
                .Select(g => g.Key)
                .FirstOrDefault();
        }

        /// <summary>
        /// 보석 위치에 LightBallExplosion 이펙트를 생성합니다.
        /// 보석의 스프라이트를 이펙트 Ingredient 자식에 복사합니다.
        /// </summary>
        private void SpawnExplosionEffect(Gem gem)
        {
            if (LightBallExplosionPrefab == null || gem == null) return;

            var effect = Instantiate(LightBallExplosionPrefab, gem.transform.position, Quaternion.identity);
            var gemSR = gem.GetComponentInChildren<SpriteRenderer>();

            // 보석 스프라이트를 이펙트의 Ingredient 자식에 복사합니다.
            if (gemSR?.sprite != null)
            {
                var ingredient = effect.transform.Find("Ingredient");
                var sr = ingredient?.GetComponent<SpriteRenderer>();
                if (sr != null)
                {
                    sr.sprite = gemSR.sprite;
                    sr.enabled = true;
                }
            }

            // Origin 프리팹의 SortingLayer가 MyGame에 없으므로 Gem 레이어로 보정합니다.
            if (gemSR != null)
            {
                int layerID = gemSR.sortingLayerID;
                int baseOrder = gemSR.sortingOrder + 10;
                foreach (var sr in effect.GetComponentsInChildren<SpriteRenderer>())
                {
                    sr.sortingLayerID = layerID;
                    sr.sortingOrder = baseOrder + sr.sortingOrder;
                }
            }

            Destroy(effect, EffectLifetime);
        }
    }
}
