using System.Collections.Generic;
using UnityEngine;

public class Slots : MonoBehaviour
{
    [SerializeField] private Slot slotPrefab;

    private readonly float cellSize = 0.7f;
    private readonly float largeGridScale = 0.75f; // 3x3(9칸) 레이아웃 시 축소 비율

    private Table table;
    private readonly List<Slot> slots = new();
    private int lastHash;

    void Awake()
    {
        table = GetComponentInParent<Table>();
    }


    void Start()
    {
        Rebuild(force: true);
        UpdateVisibility();

        table.OnOrderSet += HandleOrderSet;
        table.OnIngredientRemainingChanged += HandleRemainingChanged;

    }

    void OnDestroy()
    {
        table.OnOrderSet -= HandleOrderSet;
        table.OnIngredientRemainingChanged -= HandleRemainingChanged;
    }


    private void HandleOrderSet()
    {
        Rebuild(force: true);
        UpdateVisibility();
    }


    private void HandleRemainingChanged(string tag, int remaining)
    {
        // 해당 슬롯만 업데이트
        for (int i = 0; i < slots.Count; i++)
        {
            var s = slots[i];
            if (s != null && s.IngredientTag == tag)
            {
                s.UpdateRemaining(remaining);
                break;
            }
        }
        UpdateVisibility();
    }


    private void Rebuild(bool force = false)
    {
        if (table == null) return;
        var recipe = table.GetCurrentOrder();
        if (recipe == null)
        {
            Clear();
            return;
        }
        if (slotPrefab == null)
        {
            Debug.LogWarning("Slots: slotPrefab is not assigned.", this);
            Clear();
            return;
        }
        var ings = recipe.Ingredients;
        int count = 0; int hash = 17;
        for (int i = 0; i < ings.Count; i++)
        {
            var ing = ings[i];
            if (ing.block == null || ing.count <= 0) continue;
            count++;
            hash = hash * 31 + ing.block.GetInstanceID();
            hash = hash * 31 + ing.count;
        }
        if (!force && hash == lastHash) return;
        lastHash = hash;
        Clear();
        if (count == 0) return;

        int targetCells = count <= 4 ? 4 : 9; // 요청 사양
        int grid = targetCells == 4 ? 2 : 3;

        // 9슬롯(3x3)일 때는 셀 크기를 축소
        float effectiveCellSize = (targetCells == 9) ? cellSize * largeGridScale : cellSize;
        float w = (grid - 1) * effectiveCellSize;
        float h = (grid - 1) * effectiveCellSize;
        Vector3 origin = transform.position - new Vector3(w * 0.5f, h * 0.5f, 0f);

        int created = 0;
        for (int r = 0; r < grid; r++)
        {
            for (int c = 0; c < grid; c++)
            {
                // 재료 실제 개수 이상은 만들지 않음
                if (created >= count) break;
                // 해당 순서의 실제 유효 재료 찾기
                var ing = GetNthValidIngredient(ings, created);
                if (ing.block == null) { created++; continue; }
                var s = Instantiate(slotPrefab, transform);
                s.transform.position = origin + new Vector3(c * effectiveCellSize, (grid - 1 - r) * effectiveCellSize, 0f);
                int remain = table.GetRemainingNeed(ing.block.Tag);
                s.Setup(ing.block.Sprite, ing.block.Tag, remain);
                slots.Add(s);
                created++;
            }
        }
    }


    private RecipeSO.Ingredient GetNthValidIngredient(IReadOnlyList<RecipeSO.Ingredient> list, int nth)
    {
        int idx = 0;
        for (int i = 0; i < list.Count; i++)
        {
            var ing = list[i];
            if (ing.block == null || ing.count <= 0) continue;
            if (idx == nth) return ing;
            idx++;
        }
        return default;
    }


    private void Clear()
    {
        for (int i = 0; i < slots.Count; i++)
        {
            if (slots[i] != null) Destroy(slots[i].gameObject);
        }
        slots.Clear();
    }


    private void UpdateVisibility()
    {
        bool shouldShow = table.GetState() == Table.TableState.Ordered;
        if (!shouldShow)
        {
            gameObject.SetActive(false);
            return;
        }
        // 남은 재료가 모두 0이면 숨김
        bool anyRemaining = false;
        for (int i = 0; i < slots.Count; i++)
        {
            var s = slots[i];
            if (s == null) continue;
            if (table.GetRemainingNeed(s.IngredientTag) > 0)
            {
                anyRemaining = true; break;
            }
        }
        if (gameObject.activeSelf != anyRemaining)
            gameObject.SetActive(anyRemaining);
    }
}
