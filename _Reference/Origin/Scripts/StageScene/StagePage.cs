using UnityEngine;
using UnityEngine.UI;

[RequireComponent(typeof(GridLayoutGroup))]
public class StagePage : MonoBehaviour
{
    private GridLayoutGroup grid; // 이 페이지가 가진 GridLayoutGroup

    [SerializeField] private StageCell stageCellPrefab; // 등록 받을 StageCell 프리팹

    private int cellCount = 12; // 기본 12개 생성 (외부에서 설정 가능)
    private ShopSO shop;
    private StageSO[] stageData; // 이 페이지에 표시할 스테이지 데이터
    private int pageIndex = 0; // 이 페이지의 인덱스 (0부터 시작)


    private void Awake()
    {
        grid = GetComponent<GridLayoutGroup>();
    }

    private void Start()
    {
        Populate();
        UpdateCellBorders();
    }

    // 외부에서 셀 개수를 설정하는 메서드
    public void SetCellCount(int count)
    {
        cellCount = Mathf.Max(1, count); // 최소 1개
    }

    /// <summary>
    /// 이 페이지의 스테이지 데이터와 페이지 인덱스를 설정합니다.
    /// </summary>
    /// <param name="stages">전체 스테이지 데이터 배열</param>
    /// <param name="pageIdx">이 페이지의 인덱스 (0부터 시작)</param>
    public void SetStageData(ShopSO shop, StageSO[] stages, int pageIdx)
    {
        this.shop = shop;
        stageData = stages;
        pageIndex = pageIdx;
    }

    // StageCell 프리팹을 사용해 셀을 생성하여 Grid 하위에 추가
    private void Populate()
    {
        if (!stageCellPrefab)
        {
            Debug.LogWarning("[StagePage] StageCell Prefab이 지정되지 않았습니다.", this);
            return;
        }

        var parent = grid ? grid.transform : transform;
        for (int i = 0; i < cellCount; i++)
        {
            var cell = Instantiate(stageCellPrefab, parent);
            cell.name = $"StageCell_{i + 1}";

            // 해당 인덱스의 스테이지 데이터를 찾아서 설정
            int globalStageIndex = pageIndex * 12 + i; // 전체 스테이지 배열에서의 인덱스
            StageSO stageToSet = null;

            if (stageData != null && globalStageIndex < stageData.Length)
            {
                stageToSet = stageData[globalStageIndex];
            }

            // StageCell에 데이터 설정
            cell.SetStageData(shop, stageToSet);
        }
    }


    private void OnTransformChildrenChanged()
    {
        // 동적으로 자식이 변할 때 보더 갱신
        UpdateCellBorders();
    }

    // 현재 Grid와 자식 StageCell 구성에 따라 보더 표시 규칙 적용
    private void UpdateCellBorders()
    {
        if (!grid) grid = GetComponent<GridLayoutGroup>();
        if (!grid) return;

        // 자식 중 StageCell만 수집 (그리드 순서 유지)
        int childCount = grid.transform.childCount;
        if (childCount == 0) return;

        var cells = new System.Collections.Generic.List<StageCell>(childCount);
        for (int i = 0; i < childCount; i++)
        {
            var t = grid.transform.GetChild(i);
            var sc = t.GetComponent<StageCell>();
            if (sc) cells.Add(sc);
        }

        int count = cells.Count;
        if (count == 0) return;

        int columns = ComputeColumnCount(count);
        if (columns < 1) columns = 1;

        // 규칙: 항상 Left/Top은 표시. Right는 오른쪽에 이웃이 없을 때만 표시. Bottom은 아래에 이웃이 없을 때만 표시.
        for (int i = 0; i < count; i++)
        {
            int row = i / columns;
            int col = i % columns;

            bool hasRightNeighbor = (i + 1 < count) && ((i + 1) / columns == row);
            bool hasBelowNeighbor = (i + columns) < count;

            var sc = cells[i];
            sc.ShowLeft = true;
            sc.ShowTop = true;
            sc.ShowRight = !hasRightNeighbor;
            sc.ShowBottom = !hasBelowNeighbor;
        }
    }

    private int ComputeColumnCount(int itemCount)
    {
        switch (grid.constraint)
        {
            case GridLayoutGroup.Constraint.FixedColumnCount:
                return Mathf.Max(1, grid.constraintCount);
            case GridLayoutGroup.Constraint.FixedRowCount:
                {
                    int rows = Mathf.Max(1, grid.constraintCount);
                    return Mathf.CeilToInt(itemCount / (float)rows);
                }
            case GridLayoutGroup.Constraint.Flexible:
            default:
                {
                    // 컨테이너 너비 기준으로 대략적인 열 수 산출
                    var rt = grid.transform as RectTransform;
                    if (!rt) return 1;
                    float width = rt.rect.width - grid.padding.left - grid.padding.right;
                    float cellW = Mathf.Max(1f, grid.cellSize.x);
                    float spacing = Mathf.Max(0f, grid.spacing.x);
                    int cols = Mathf.FloorToInt((width + spacing) / (cellW + spacing));
                    if (cols < 1) cols = 1;
                    return cols;
                }
        }
    }
}
