using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

/// <summary>
/// 레시피 타입: Default(기본), StageExclusive(특정 스테이지 전용), Secret(델리로 구매)
/// </summary>
public enum RecipeType
{
    Default,          // 기본 레시피 (일반적으로 사용 가능)
    StageExclusive,   // 특정 스테이지에서만 나오는 레시피
    Secret            // 비밀 레시피 (Delly로 구매 필요)
}

// 특정 레시피: BlockSO와 필요한 개수의 집합
[CreateAssetMenu(fileName = "Recipe", menuName = "DoggyChef/Recipe")]
public class RecipeSO : ScriptableObject
{
    [Header("Recipe Meta")]
    [SerializeField, Tooltip("레시피 저장이름 (boonsik_kimbob 형태)")]
    private string recipeTag;
    [SerializeField, Tooltip("상점 타입 (어느 상점에 속하는지)")]
    private ShopType shopType;
    [SerializeField, Tooltip("레시피 타입 (Default/StageExclusive/Secret)")]
    private RecipeType recipeType = RecipeType.Default;

    [SerializeField, Tooltip("레시피 표시 이름 (비우면 Asset 이름 사용)")]
    private string recipeName;
    [SerializeField, TextArea(2, 4), Tooltip("레시피 설명")]
    private string description;
    [SerializeField, Tooltip("결과 아이템/요리의 대표 스프라이트")]
    private Sprite resultSprite;

    [SerializeField, Min(1), Tooltip("이 레시피가 나오는 스테이지 번호")]
    private int exclusiveStageNumber;

    [Header("Economy")]
    [SerializeField, Min(0), Tooltip("구입 가격(코인)")]
    private int purchasePrice;
    [SerializeField, Min(0), Tooltip("비밀 레시피 구매에 필요한 Delly 가격")]
    private int dellyPrice;
    [SerializeField, Min(0), Tooltip("판매 가격(코인)")]
    private int sellPrice;

    [Serializable]
    public struct Ingredient
    {
        [Tooltip("필요한 블록 타입")]
        public BlockSO block;

        [Min(1)]
        [Tooltip("필요 수량 (최소 1)")]
        public int count;

        public Ingredient(BlockSO block, int count)
        {
            this.block = block;
            this.count = Mathf.Max(1, count);
        }
    }

    [Header("Ingredient (BlockSO + count)")]
    [SerializeField] private List<Ingredient> ingredients = new();


    public IReadOnlyList<Ingredient> Ingredients => ingredients;

    public string RecipeName => string.IsNullOrWhiteSpace(recipeName) ? name : recipeName;
    public string Description => description;
    public Sprite ResultSprite => resultSprite;
    public RecipeType Type => recipeType;
    public ShopType ShopType => shopType;
    public string RecipeTag => string.IsNullOrWhiteSpace(recipeTag) ? name : recipeTag;

    // Stage Exclusive 정보
    public int ExclusiveStageNumber => exclusiveStageNumber;

    // Secret Recipe 정보
    public int DellyPrice => Mathf.Max(0, dellyPrice);

    // Economy
    public int PurchasePrice => Mathf.Max(0, purchasePrice);
    public int SellPrice => Mathf.Max(0, sellPrice);

    // 총 필요 수량 합계
    public int TotalRequiredCount => ingredients != null ? ingredients.Sum(i => Mathf.Max(0, i.count)) : 0;

    // 해당 블록이 필요 목록에 포함되는지
    public bool Contains(BlockSO block) => GetCount(block) > 0;

    // 해당 블록의 필요 수량 반환 (없으면 0)
    private int GetCount(BlockSO block)
    {
        if (block == null || ingredients == null) return 0;
        for (int i = 0; i < ingredients.Count; i++)
        {
            if (ingredients[i].block == block)
                return Mathf.Max(0, ingredients[i].count);
        }
        return 0;
    }

    // 태그로 조회 (BlockSO.Tag 기준)
    public int GetCountByTag(string tag)
    {
        if (string.IsNullOrEmpty(tag) || ingredients == null) return 0;
        for (int i = 0; i < ingredients.Count; i++)
        {
            var b = ingredients[i].block;
            if (b != null && string.Equals(b.Tag, tag, StringComparison.Ordinal))
                return Mathf.Max(0, ingredients[i].count);
        }
        return 0;
    }

    /// <summary>
    /// 특정 스테이지에서 이 레시피가 사용 가능한지 확인
    /// </summary>
    public bool IsAvailableInStage(int stageNumber)
    {
        switch (recipeType)
        {
            case RecipeType.Default:
                return true; // 기본 레시피는 항상 사용 가능

            case RecipeType.StageExclusive:
                // 지정된 스테이지에서만 사용 가능
                return exclusiveStageNumber <= stageNumber;

            case RecipeType.Secret:
                // Secret 레시피는 별도 구매 확인 필요 (여기서는 타입만 체크)
                return false; // 구매 여부는 SaveSystem 등에서 별도 관리

            default:
                return false;
        }
    }

    /// <summary>
    /// 비밀 레시피인지 확인
    /// </summary>
    public bool IsSecretRecipe() => recipeType == RecipeType.Secret;

    /// <summary>
    /// 스테이지 전용 레시피인지 확인
    /// </summary>
    public bool IsStageExclusiveRecipe() => recipeType == RecipeType.StageExclusive;
}
