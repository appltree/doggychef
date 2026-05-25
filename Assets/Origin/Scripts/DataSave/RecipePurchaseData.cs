using System;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// 플레이어가 구매한 레시피 정보를 관리하는 클래스
/// </summary>
[Serializable]
public class RecipePurchaseData
{
    private const string SAVE_KEY = "PurchasedRecipes";

    [SerializeField] private List<string> purchasedRecipeTags = new();

    /// <summary>
    /// 저장된 데이터를 로드합니다.
    /// </summary>
    public static RecipePurchaseData Load()
    {
        string json = PlayerPrefs.GetString(SAVE_KEY, "");
        if (!string.IsNullOrEmpty(json))
        {
            try
            {
                return JsonUtility.FromJson<RecipePurchaseData>(json);
            }
            catch (Exception e)
            {
                Debug.LogWarning($"[RecipePurchaseData] 로드 실패: {e.Message}");
            }
        }

        return new RecipePurchaseData();
    }

    /// <summary>
    /// 현재 데이터를 저장합니다.
    /// </summary>
    private void Save()
    {
        PlayerPrefs.SetString(SAVE_KEY, JsonUtility.ToJson(this));
        PlayerPrefs.Save();
    }

    /// <summary>
    /// 레시피를 구매 목록에 추가합니다.
    /// </summary>
    public void PurchaseRecipe(RecipeSO recipe)
    {
        if (recipe == null)
        {
            Debug.LogWarning("[RecipePurchaseData] Recipe가 null입니다.");
            return;
        }

        string recipeId = GetRecipeId(recipe);
        if (string.IsNullOrEmpty(recipeId))
        {
            Debug.LogWarning($"[RecipePurchaseData] Recipe '{recipe.RecipeName}'의 ID를 가져올 수 없습니다.");
            return;
        }

        if (!purchasedRecipeTags.Contains(recipeId))
        {
            purchasedRecipeTags.Add(recipeId);
            Save();
            Debug.Log($"[RecipePurchaseData] '{recipe.RecipeName}' 구매 완료");
        }
    }

    /// <summary>
    /// 레시피가 이미 구매되었는지 확인합니다.
    /// </summary>
    public bool IsPurchased(RecipeSO recipe)
    {
        if (recipe == null) return false;

        // 기본 레시피는 항상 구매된 것으로 간주
        if (recipe.Type == RecipeType.Default)
            return true;

        string recipeId = GetRecipeId(recipe);
        if (string.IsNullOrEmpty(recipeId))
            return false;

        return purchasedRecipeTags.Contains(recipeId);
    }

    /// <summary>
    /// RecipeSO의 고유 ID를 반환합니다.
    /// Asset 파일 이름을 사용합니다.
    /// </summary>
    private string GetRecipeId(RecipeSO recipe)
    {
        if (recipe == null) return string.Empty;
        return recipe.RecipeTag; // ScriptableObject의 Asset 이름
    }

    /// <summary>
    /// 모든 구매 기록을 초기화합니다. (디버그/테스트용)
    /// </summary>
    [System.Diagnostics.Conditional("UNITY_EDITOR")]
    public void ResetAll()
    {
        purchasedRecipeTags.Clear();
        Save();
        Debug.Log("[RecipePurchaseData] 모든 구매 기록 초기화 완료");
    }

    /// <summary>
    /// 현재 구매한 레시피 목록을 로그로 출력합니다. (디버그용)
    /// </summary>
    [System.Diagnostics.Conditional("UNITY_EDITOR")]
    public void LogPurchasedRecipes()
    {
        Debug.Log($"[RecipePurchaseData] 구매한 레시피 수: {purchasedRecipeTags.Count}");
        foreach (var tag in purchasedRecipeTags)
        {
            Debug.Log($"  - {tag}");
        }
    }
}
