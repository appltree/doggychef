using System.Collections.Generic;
using MyMatch3;
using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(GridLayoutGroup))]
public class StagePage : MonoBehaviour
{
    [SerializeField] private StageCell stageCellPrefab;

    private const int StagesPerPage = 12;
    private GridLayoutGroup grid;
    private int cellCount = StagesPerPage;
    private ShopData shop;
    private StageData[] stageData;
    private int pageIndex;

    private void Awake()
    {
        grid = GetComponent<GridLayoutGroup>();
    }

    private void Start()
    {
        Populate();
        UpdateCellBorders();
    }

    public void SetCellCount(int count)
    {
        cellCount = Mathf.Max(1, count);
    }

    public void SetStageData(ShopData shop, StageData[] stages, int pageIdx)
    {
        this.shop = shop;
        stageData = stages;
        pageIndex = pageIdx;
    }

    private void Populate()
    {
        if (stageCellPrefab == null)
        {
            Debug.LogWarning("[StagePage] StageCell prefab is not assigned.", this);
            return;
        }

        var parent = grid != null ? grid.transform : transform;
        for (int i = 0; i < cellCount; i++)
        {
            var cell = Instantiate(stageCellPrefab, parent);
            cell.name = $"StageCell_{i + 1}";

            int globalStageIndex = pageIndex * StagesPerPage + i;
            StageData stageToSet = null;
            if (stageData != null && globalStageIndex < stageData.Length)
                stageToSet = stageData[globalStageIndex];

            cell.SetStageData(shop, stageToSet);
        }
    }

    private void OnTransformChildrenChanged()
    {
        UpdateCellBorders();
    }

    private void UpdateCellBorders()
    {
        if (grid == null)
            grid = GetComponent<GridLayoutGroup>();
        if (grid == null)
            return;

        int childCount = grid.transform.childCount;
        if (childCount == 0)
            return;

        var cells = new List<StageCell>(childCount);
        for (int i = 0; i < childCount; i++)
        {
            var cell = grid.transform.GetChild(i).GetComponent<StageCell>();
            if (cell != null)
                cells.Add(cell);
        }

        int count = cells.Count;
        if (count == 0)
            return;

        int columns = Mathf.Max(1, ComputeColumnCount(count));
        for (int i = 0; i < count; i++)
        {
            int row = i / columns;
            bool hasRightNeighbor = (i + 1 < count) && ((i + 1) / columns == row);
            bool hasBelowNeighbor = (i + columns) < count;

            cells[i].ShowLeft = true;
            cells[i].ShowTop = true;
            cells[i].ShowRight = !hasRightNeighbor;
            cells[i].ShowBottom = !hasBelowNeighbor;
        }
    }

    private int ComputeColumnCount(int itemCount)
    {
        switch (grid.constraint)
        {
            case GridLayoutGroup.Constraint.FixedColumnCount:
                return Mathf.Max(1, grid.constraintCount);
            case GridLayoutGroup.Constraint.FixedRowCount:
                int rows = Mathf.Max(1, grid.constraintCount);
                return Mathf.CeilToInt(itemCount / (float)rows);
            default:
                var rt = grid.transform as RectTransform;
                if (rt == null)
                    return 1;

                float width = rt.rect.width - grid.padding.left - grid.padding.right;
                float cellW = Mathf.Max(1f, grid.cellSize.x);
                float spacing = Mathf.Max(0f, grid.spacing.x);
                return Mathf.Max(1, Mathf.FloorToInt((width + spacing) / (cellW + spacing)));
        }
    }
}
