using System.Collections.Generic;
using UnityEngine;

namespace DoggyChef
{
    // ════════════════════════════════════════════════════════════════
    //  TableIngredientDisplay — 재료 수집 현황 그리드
    // ════════════════════════════════════════════════════════════════
    //
    //  ■ 역할
    //  Origin의 Slots.cs를 참고하여,
    //  현재 주문에 필요한 재료 슬롯을 그리드로 배치합니다.
    //  각 슬롯은 재료 아이콘과 남은 필요 수량을 표시합니다.
    //
    //  ■ 그리드 규칙
    //    재료 종류 1~4개 → 2×2 그리드 (최대 4 슬롯)
    //    재료 종류 5~9개 → 3×3 그리드 (최대 9 슬롯, 셀 크기 축소)
    //
    //  ■ 표시 상태
    //    Ordered + 남은 재료 있음 → 표시
    //    그 외 모든 상태           → 숨김
    //
    //  ■ 부착 위치
    //    Table 자식의 IngredientTarget GameObject에 부착합니다.
    //
    //  ■ Inspector 연결 필요
    //    SlotPrefab : IngredientSlot 컴포넌트가 붙은 프리팹
    // ════════════════════════════════════════════════════════════════
    public class TableIngredientDisplay : MonoBehaviour
    {
        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────

        [Header("슬롯 프리팹")]
        [Tooltip("IngredientSlot 컴포넌트가 붙은 프리팹. Inspector에서 반드시 연결하세요.")]
        public IngredientSlot SlotPrefab;

        [Header("레이아웃 설정")]
        [Tooltip("슬롯 간격(Unity 유닛). 2×2 그리드에서 사용됩니다.")]
        public float CellSize = 0.7f;

        [Tooltip("3×3 그리드 시 셀 크기 축소 비율.")]
        public float LargeGridScale = 0.5f;

        // ──────────────────────────────────────────────────────────
        //  런타임 상태
        // ──────────────────────────────────────────────────────────

        private Table m_Table;
        private readonly List<IngredientSlot> m_Slots = new();

        // ================================================================
        //  유니티 생명주기
        // ================================================================

        private void Awake()
        {
            m_Table = GetComponentInParent<Table>();
            if (m_Table == null)
                Debug.LogWarning("[TableIngredientDisplay] 부모 계층에 Table 컴포넌트가 없습니다.", this);
        }

        private void Start()
        {
            if (m_Table == null) return;

            m_Table.OnOrderSet += OnOrderSet;
            m_Table.OnIngredientRemainingChanged += OnIngredientChanged;
            m_Table.OnStateChanged += OnStateChanged;

            RebuildAndRefresh();
        }

        private void OnDestroy()
        {
            if (m_Table == null) return;
            m_Table.OnOrderSet -= OnOrderSet;
            m_Table.OnIngredientRemainingChanged -= OnIngredientChanged;
            m_Table.OnStateChanged -= OnStateChanged;
        }

        // ================================================================
        //  이벤트 핸들러
        // ================================================================

        private void OnOrderSet() => RebuildAndRefresh();

        private void OnStateChanged(Table.TableState state)
        {
            // Empty 상태가 되면 슬롯을 즉시 파괴합니다 (Origin Slots.cs의 Clear-on-reset 패턴).
            // 주문이 없는 상태에서 슬롯 인스턴스가 메모리에 남지 않도록 합니다.
            if (state == Table.TableState.Empty)
                ClearSlots();

            UpdateVisibility();
        }

        // gem 파괴로 재료 수량이 변경될 때 → 해당 슬롯만 갱신
        private void OnIngredientChanged(string tag, int remaining)
        {
            foreach (var slot in m_Slots)
            {
                if (slot != null && slot.IngredientTag == tag)
                {
                    slot.UpdateRemaining(remaining);
                    break;
                }
            }
            UpdateVisibility();
        }

        // ================================================================
        //  슬롯 빌드
        // ================================================================

        // 슬롯을 재구성하고 가시성을 갱신합니다.
        private void RebuildAndRefresh()
        {
            Rebuild();
            UpdateVisibility();
        }

        // 재료 종류에 따라 2×2 또는 3×3 그리드로 슬롯을 생성합니다.
        private void Rebuild()
        {
            ClearSlots();

            if (m_Table == null || SlotPrefab == null) return;

            var recipe = m_Table.CurrentOrder;
            if (recipe?.Ingredients == null) return;

            // 유효한 재료(MaterialGem이 있고 Count > 0)만 추립니다.
            var valid = new List<RecipeData.Ingredient>();
            foreach (var ing in recipe.Ingredients)
                if (ing.MaterialGem != null && ing.Count > 0) valid.Add(ing);

            int count = valid.Count;
            if (count == 0) return;

            // ── 그리드 파라미터 ──
            // 4개 이하: 2×2 (최소 4 슬롯)   5개 이상: 3×3 (최대 9 슬롯)
            int grid = count <= 4 ? 2 : 3;
            float cell = grid == 2 ? CellSize : CellSize * LargeGridScale;
            float extent = (grid - 1) * cell;
            Vector3 origin = transform.position - new Vector3(extent * 0.5f, extent * 0.5f, 0f);

            // ── 슬롯 생성 (왼쪽 위 → 오른쪽 아래) ──
            for (int i = 0; i < count; i++)
            {
                int r = i / grid;
                int c = i % grid;
                var gem = valid[i].MaterialGem;
                var slot = Instantiate(SlotPrefab, transform);

                slot.transform.position = origin + new Vector3(
                    c * cell,
                    (grid - 1 - r) * cell,
                    0f);
                slot.transform.localScale = Vector3.one * (cell / CellSize);

                slot.Setup(gem.Sprite, gem.BlockTag, m_Table.GetRemainingNeed(gem.BlockTag));
                m_Slots.Add(slot);
            }
        }

        // 지정된 tag 를 가진 IngredientSlot 의 월드 좌표를 반환합니다.
        // 슬롯이 없거나 비활성화 상태면 null 을 반환합니다.
        // Table.FlyIngredient 가 정확한 착탄 위치로 사용합니다.
        public Vector3? GetSlotPosition(string tag)
        {
            foreach (var slot in m_Slots)
            {
                if (slot != null && slot.IngredientTag == tag)
                    return slot.transform.position;
            }
            return null;
        }

        // 기존 슬롯을 모두 파괴하고 목록을 비웁니다.
        private void ClearSlots()
        {
            foreach (var slot in m_Slots)
                if (slot != null) Destroy(slot.gameObject);
            m_Slots.Clear();
        }

        // ================================================================
        //  가시성 제어
        // ================================================================

        // Ordered 상태이고 아직 남은 재료가 있을 때만 표시합니다.
        private void UpdateVisibility()
        {
            bool show = m_Table != null
                     && m_Table.State == Table.TableState.Ordered
                     && HasAnyRemaining();
            gameObject.SetActive(show);
        }

        // 아직 필요한 재료가 하나라도 남아 있는지 확인합니다.
        private bool HasAnyRemaining()
        {
            foreach (var slot in m_Slots)
                if (slot != null && m_Table.GetRemainingNeed(slot.IngredientTag) > 0) return true;
            return false;
        }
    }
}
