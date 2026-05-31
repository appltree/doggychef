using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace DoggyChef
{
    // 상점 팝업 내 레시피 탭입니다. ShopData 기준으로 ShopRecipeCell을 동적 생성합니다.
    public class ShopTabRecipe : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  Inspector 연결 필드
        // ════════════════════════════════════════════════════════════════

        [Header("레시피 설정")]
        [Tooltip("표시할 상점 목록")]
        [SerializeField] private ShopData[] m_Shops;
        [Tooltip("레시피 셀 프리팹")]
        [SerializeField] private ShopRecipeCell m_CellPrefab;
        [Tooltip("ScrollView의 Content Transform")]
        [SerializeField] private Transform m_ScrollContent;
        [Tooltip("상점별 레시피 필터 버튼들")]
        [SerializeField] private Button[] m_ShopButtons;

        // ════════════════════════════════════════════════════════════════
        //  private 런타임 상태
        // ════════════════════════════════════════════════════════════════

        private readonly List<ShopRecipeCell> m_Cells = new();
        private int m_SelectedShopIndex;

        // ════════════════════════════════════════════════════════════════
        //  Unity 생명 주기
        // ════════════════════════════════════════════════════════════════

        // 스크롤뷰와 상점 탭 버튼을 준비하고 첫 번째 상점 레시피를 표시합니다.
        private void Start()
        {
            ConfigureScrollView();
            WireShopButtons();

            if (m_Shops != null && m_Shops.Length > 0)
                SelectShop(0);
        }

        // 생성된 셀을 정리하고 버튼 리스너를 해제합니다.
        private void OnDestroy()
        {
            if (m_ShopButtons != null)
            {
                foreach (var button in m_ShopButtons)
                    button?.onClick.RemoveAllListeners();
            }

            ClearCells();
        }

        // ════════════════════════════════════════════════════════════════
        //  public API
        // ════════════════════════════════════════════════════════════════

        // [ShopPopup] 지정한 상점의 레시피 셀 목록을 다시 생성합니다.
        public void LoadRecipes(ShopData shop)
        {
            if (shop == null)
            {
                Debug.LogWarning("[ShopTabRecipe] shop이 null입니다.");
                return;
            }

            if (m_CellPrefab == null || m_ScrollContent == null)
            {
                Debug.LogWarning("[ShopTabRecipe] CellPrefab 또는 ScrollContent가 할당되지 않았습니다.");
                return;
            }

            ClearCells();

            if (shop.Recipes == null || shop.Recipes.Length == 0) return;

            foreach (var recipe in shop.Recipes)
            {
                if (recipe == null) continue;
                var cell = Instantiate(m_CellPrefab, m_ScrollContent);
                cell.Setup(recipe, shop.Type);
                m_Cells.Add(cell);
            }
        }

        // [상점 서브탭] 선택된 상점의 레시피를 표시합니다.
        public void SelectShop(int index)
        {
            if (m_Shops == null || index < 0 || index >= m_Shops.Length) return;

            m_SelectedShopIndex = index;
            LoadRecipes(m_Shops[index]);
            RefreshShopButtonState();
        }

        // ════════════════════════════════════════════════════════════════
        //  private 구현
        // ════════════════════════════════════════════════════════════════

        // 상점 탭 버튼을 자동 수집하고 선택 콜백 및 표시 이름을 설정합니다.
        private void WireShopButtons()
        {
            if (m_ShopButtons == null || m_ShopButtons.Length == 0)
                m_ShopButtons = GetComponentsInChildren<Button>(true);

            if (m_ShopButtons == null) return;

            int count = Mathf.Min(m_ShopButtons.Length, m_Shops != null ? m_Shops.Length : 0);
            for (int i = 0; i < count; i++)
            {
                int shopIndex = i;
                var button = m_ShopButtons[i];
                if (button == null) continue;

                button.onClick.RemoveAllListeners();
                button.onClick.AddListener(() => SelectShop(shopIndex));

                var label = button.GetComponentInChildren<TextMeshProUGUI>(true);
                if (label != null && m_Shops[shopIndex] != null)
                    label.text = string.IsNullOrWhiteSpace(m_Shops[shopIndex].ShopName)
                        ? m_Shops[shopIndex].Type.ToString()
                        : m_Shops[shopIndex].ShopName;
            }

            RefreshShopButtonState();
        }

        // 레시피 카드가 가로 한 줄로 배치되도록 ScrollRect와 GridLayoutGroup을 보정합니다.
        private void ConfigureScrollView()
        {
            if (m_ScrollContent == null) return;

            var scrollRect = m_ScrollContent.GetComponentInParent<ScrollRect>(true);
            if (scrollRect != null)
            {
                scrollRect.horizontal = true;
                scrollRect.vertical = false;
                scrollRect.movementType = ScrollRect.MovementType.Clamped;
            }

            var grid = m_ScrollContent.GetComponent<GridLayoutGroup>();
            if (grid != null)
            {
                grid.cellSize = new Vector2(232f, 334f);
                grid.spacing = new Vector2(26f, 0f);
                grid.constraint = GridLayoutGroup.Constraint.FixedRowCount;
                grid.constraintCount = 1;
                grid.childAlignment = TextAnchor.UpperLeft;
                grid.padding.left = 24;
                grid.padding.right = 24;
            }
        }

        // 선택된 상점 버튼은 어둡게, 나머지 버튼은 회색으로 표시합니다.
        private void RefreshShopButtonState()
        {
            if (m_ShopButtons == null) return;

            for (int i = 0; i < m_ShopButtons.Length; i++)
            {
                var button = m_ShopButtons[i];
                if (button == null) continue;

                bool selected = i == m_SelectedShopIndex;
                var image = button.GetComponent<Image>();
                if (image != null)
                    image.color = selected ? new Color(0.25f, 0.25f, 0.25f, 1f) : new Color(0.62f, 0.62f, 0.62f, 1f);

                var label = button.GetComponentInChildren<TextMeshProUGUI>(true);
                if (label != null)
                    label.color = Color.white;
            }
        }

        // 기존 셀 오브젝트를 모두 파괴하고 목록을 비웁니다.
        private void ClearCells()
        {
            foreach (var cell in m_Cells)
            {
                if (cell != null) Destroy(cell.gameObject);
            }
            m_Cells.Clear();
        }
    }
}
