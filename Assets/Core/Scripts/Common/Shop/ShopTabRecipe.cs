using System.Collections.Generic;
using UnityEngine;

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

        // ════════════════════════════════════════════════════════════════
        //  private 런타임 상태
        // ════════════════════════════════════════════════════════════════

        private readonly List<ShopRecipeCell> m_Cells = new();

        // ════════════════════════════════════════════════════════════════
        //  Unity 생명 주기
        // ════════════════════════════════════════════════════════════════

        // 첫 번째 상점의 레시피 셀을 초기 생성합니다.
        private void Start()
        {
            if (m_Shops != null && m_Shops.Length > 0)
                LoadRecipes(m_Shops[0]);
        }

        // 생성된 셀을 모두 정리합니다.
        private void OnDestroy()
        {
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

        // ════════════════════════════════════════════════════════════════
        //  private 구현
        // ════════════════════════════════════════════════════════════════

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
