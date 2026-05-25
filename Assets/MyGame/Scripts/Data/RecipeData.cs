using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace MyMatch3
{
  public enum RecipeType
  {
    Default,          // 기본 레시피 (일반적으로 사용 가능)
    StageExclusive,   // 특정 스테이지에서만 나오는 레시피
    Secret            // 특수 구매 레시피 등
  }

  [CreateAssetMenu(fileName = "NewRecipeData", menuName = "Tycoon/RecipeData")]
  public class RecipeData : ScriptableObject
  {
    [Header("Recipe Meta")]
    [Tooltip("고유 식별용 태그 (boonsik_kimbob 형태)")]
    public string RecipeTag;

    [Tooltip("레시피 타입 (Default/StageExclusive/Secret)")]
    public RecipeType Type = RecipeType.Default;

    [Tooltip("레시피 이름 (UI 노출용)")]
    public string Name;

    [TextArea(2, 4)]
    [Tooltip("레시피 설명")]
    public string Description;

    [Tooltip("레시피 아이콘 (결과물 완성이미지)")]
    public Sprite ResultSprite;

    [Tooltip("이 레시피가 나오는 스테이지 번호 (Type이 StageExclusive일 때 참조)")]
    public int ExclusiveStageNumber;

    [Header("Economy")]
    [Tooltip("상점에서 구입 시 가격(코인 등)")]
    public int PurchasePrice;

    [Tooltip("비밀 레시피 구매 등에 필요한 특수 재화 가격")]
    public int SpecialPrice;

    [Tooltip("완성 후 수금 시 획득하는 골드(재화)")]
    public int SellPrice = 100;

    [Serializable]
    public struct Ingredient
    {
      [Tooltip("재료로 쓰일 보석 에셋 참조")]
      public GemData MaterialGem;

      [Min(1)]
      [Tooltip("해당 타입의 보석 요구 수량")]
      public int Count;
    }

    [Header("Ingredients")]
    [Tooltip("요리에 필요한 보석 재료 목록")]
    public List<Ingredient> Ingredients = new List<Ingredient>();

    // 총 필요 수량 합계
    public int TotalRequiredCount => Ingredients != null ? Ingredients.Sum(i => Mathf.Max(0, i.Count)) : 0;

    // 특정 GemData 참조로 필요 수량 확인
    public int GetCountByGem(GemData gem)
    {
      if (gem == null || Ingredients == null) return 0;
      foreach (var ing in Ingredients)
      {
        if (ing.MaterialGem == gem)
        {
          return Mathf.Max(0, ing.Count);
        }
      }
      return 0;
    }

    // 보석 매칭 시 넘겨받은 식별 태그 기반(결합성 확보)으로 조회
    public int GetCountByTag(string tag)
    {
      if (string.IsNullOrEmpty(tag) || Ingredients == null) return 0;
      foreach (var ing in Ingredients)
      {
        if (ing.MaterialGem != null && string.Equals(ing.MaterialGem.BlockTag, tag, StringComparison.Ordinal))
        {
          return Mathf.Max(0, ing.Count);
        }
      }
      return 0;
    }

    /// <summary>
    /// 특정 스테이지에서 이 레시피가 사용 가능한지 확인
    /// </summary>
    public bool IsAvailableInStage(int stageNumber)
    {
      switch (Type)
      {
        case RecipeType.Default:
          return true;
        case RecipeType.StageExclusive:
          return ExclusiveStageNumber <= stageNumber;
        case RecipeType.Secret:
          return false; // 구매 여부 등 외부 시스템에서 별도 확인
        default:
          return false;
      }
    }
  }
}
