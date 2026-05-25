using System.Collections.Generic;
using UnityEngine;

public class ShopTabRecipe : MonoBehaviour
{
    [SerializeField] private ShopSO[] shops;
    [SerializeField] private ShopRecipeCell shopRecipeCellPrefab;
    [SerializeField] private Transform scrollViewContent;

    private readonly List<ShopRecipeCell> recipeCells = new();

    void Start()
    {
        // 임시로 첫 번째 shop 선택
        if (shops != null && shops.Length > 0)
        {
            LoadRecipes(shops[0]);
        }
    }

    /// <summary>
    /// 선택된 shop의 레시피들을 ScrollView에 표시합니다.
    /// </summary>
    private void LoadRecipes(ShopSO shop)
    {
        if (shop == null)
        {
            Debug.LogWarning("[ShopTabRecipe] shop이 null입니다.");
            return;
        }

        if (shopRecipeCellPrefab == null)
        {
            Debug.LogWarning("[ShopTabRecipe] shopRecipeCellPrefab이 할당되지 않았습니다.");
            return;
        }

        if (scrollViewContent == null)
        {
            Debug.LogWarning("[ShopTabRecipe] scrollViewContent가 할당되지 않았습니다.");
            return;
        }

        // 기존 셀들 제거
        ClearRecipeCells();

        // 레시피가 없으면 종료
        RecipeSO[] recipes = shop.Recipes;
        if (recipes == null || recipes.Length == 0)
        {
            Debug.LogWarning($"[ShopTabRecipe] {shop.ShopName}에 레시피가 없습니다.");
            return;
        }

        // 각 레시피마다 셀 생성
        foreach (RecipeSO recipe in recipes)
        {
            if (recipe == null) continue;

            ShopRecipeCell cell = Instantiate(shopRecipeCellPrefab, scrollViewContent);
            cell.Setup(recipe);
            recipeCells.Add(cell);
        }

    }

    /// <summary>
    /// 모든 레시피 셀을 제거합니다.
    /// </summary>
    private void ClearRecipeCells()
    {
        foreach (ShopRecipeCell cell in recipeCells)
        {
            if (cell != null)
            {
                Destroy(cell.gameObject);
            }
        }
        recipeCells.Clear();
    }

    void OnDestroy()
    {
        ClearRecipeCells();
    }
}
