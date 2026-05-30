using System;
using System.Collections.Generic;
using UnityEngine;

namespace DoggyChef
{
    // 플레이어가 구매한 레시피 목록을 PlayerPrefs에 JSON으로 저장·관리합니다.
    [Serializable]
    public class RecipePurchaseData
    {
        // ════════════════════════════════════════════════════════════════
        //  상수
        // ════════════════════════════════════════════════════════════════

        private const string SaveKey = "PurchasedRecipes";

        // ════════════════════════════════════════════════════════════════
        //  private 런타임 상태
        // ════════════════════════════════════════════════════════════════

        [SerializeField] private List<string> m_PurchasedTags = new();

        // ════════════════════════════════════════════════════════════════
        //  public API
        // ════════════════════════════════════════════════════════════════

        // PlayerPrefs에서 구매 데이터를 로드합니다.
        public static RecipePurchaseData Load()
        {
            string json = PlayerPrefs.GetString(SaveKey, "");
            if (!string.IsNullOrEmpty(json))
            {
                try { return JsonUtility.FromJson<RecipePurchaseData>(json); }
                catch (Exception e) { Debug.LogWarning($"[RecipePurchaseData] 로드 실패: {e.Message}"); }
            }
            return new RecipePurchaseData();
        }

        // 레시피를 구매 목록에 추가하고 저장합니다.
        public void PurchaseRecipe(RecipeData recipe)
        {
            if (recipe == null) return;
            string tag = recipe.RecipeTag;
            if (string.IsNullOrEmpty(tag)) return;

            if (!m_PurchasedTags.Contains(tag))
            {
                m_PurchasedTags.Add(tag);
                Save();
                Debug.Log($"[RecipePurchaseData] '{tag}' 구매 완료");
            }
        }

        // Default 레시피는 항상 true, 나머지는 RecipeTag 기준으로 구매 기록을 확인합니다.
        public bool IsPurchased(RecipeData recipe)
        {
            if (recipe == null) return false;
            if (recipe.Type == RecipeType.Default) return true;
            return !string.IsNullOrEmpty(recipe.RecipeTag) && m_PurchasedTags.Contains(recipe.RecipeTag);
        }

        // 모든 구매 기록을 초기화합니다.
        [System.Diagnostics.Conditional("UNITY_EDITOR")]
        public void ResetAll()
        {
            m_PurchasedTags.Clear();
            Save();
            Debug.Log("[RecipePurchaseData] 모든 구매 기록 초기화 완료");
        }

        // 구매한 레시피 태그 목록을 로그로 출력합니다.
        [System.Diagnostics.Conditional("UNITY_EDITOR")]
        public void LogPurchasedRecipes()
        {
            Debug.Log($"[RecipePurchaseData] 구매한 레시피 수: {m_PurchasedTags.Count}");
            foreach (var tag in m_PurchasedTags)
                Debug.Log($"  - {tag}");
        }

        // ════════════════════════════════════════════════════════════════
        //  private 구현
        // ════════════════════════════════════════════════════════════════

        // PlayerPrefs에 JSON으로 저장합니다.
        private void Save()
        {
            PlayerPrefs.SetString(SaveKey, JsonUtility.ToJson(this));
            PlayerPrefs.Save();
        }
    }
}
