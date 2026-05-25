using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class ShopRecipeCell : MonoBehaviour
{
    [Header("UI References")]
    [SerializeField] private Image recipeImage;
    [SerializeField] private TextMeshProUGUI recipeName;
    [SerializeField] private TextMeshProUGUI recipeDescription;
    [SerializeField] private TextMeshProUGUI sellPriceText;

    [SerializeField] private Button purchaseButton;
    [SerializeField] private TextMeshProUGUI purchaseButtonText;

    [Header("Button Colors")]
    [SerializeField, Tooltip("버튼 활성화 시 텍스트 색상")]
    private Color enabledTextColor = Color.white;
    [SerializeField, Tooltip("버튼 비활성화 시 텍스트 색상")]
    private Color disabledTextColor = new Color(0.533f, 0.533f, 0.533f, 1f); // #888888

    private RecipeSO currentRecipe;

    /// <summary>
    /// 레시피 정보를 셀에 표시합니다.
    /// </summary>
    public void Setup(RecipeSO recipe)
    {
        if (recipe == null)
        {
            Debug.LogWarning("[ShopRecipeCell] Recipe가 null입니다.");
            return;
        }

        currentRecipe = recipe;

        // 레시피 이미지 설정
        if (recipeImage != null && recipe.ResultSprite != null)
        {
            recipeImage.sprite = recipe.ResultSprite;
            recipeImage.SetNativeSize();
            recipeImage.enabled = true;
        }
        else if (recipeImage != null)
        {
            recipeImage.enabled = false;
        }

        // 레시피 이름 설정
        if (recipeName != null)
        {
            recipeName.text = recipe.RecipeName;
        }

        // 레시피 설명 설정
        if (recipeDescription != null)
        {
            recipeDescription.text = recipe.Description;
        }

        // 판매 가격 설정
        if (sellPriceText != null)
        {
            sellPriceText.text = $"판매가격: {recipe.SellPrice} G";
        }

        // 구매 상태 확인
        bool isPurchased = SaveSystem.Instance.IsRecipePurchased(recipe);

        // StageExclusive 및 Secret 레시피의 스테이지 클리어 여부 확인
        bool isStageCleared = true;
        if (recipe.Type == RecipeType.StageExclusive || recipe.Type == RecipeType.Secret)
        {
            isStageCleared = SaveSystem.Instance.IsStageCleared(recipe.ShopType, recipe.ExclusiveStageNumber);
        }

        // 구매 버튼 상태 설정
        if (recipe.Type == RecipeType.Default)
        {
            purchaseButton.interactable = false;
            purchaseButtonText.color = disabledTextColor;
            purchaseButtonText.text = "기본 레시피";
        }
        else if (isPurchased)
        {
            // 이미 구매한 레시피
            purchaseButton.interactable = false;
            purchaseButtonText.color = disabledTextColor;
            purchaseButtonText.text = "구매 완료";
        }
        else if (recipe.Type == RecipeType.StageExclusive)
        {
            if (!isStageCleared)
            {
                // 스테이지를 아직 클리어하지 않음
                purchaseButton.interactable = false;
                purchaseButtonText.color = disabledTextColor;
                purchaseButtonText.text = $"스테이지 {recipe.ExclusiveStageNumber} 클리어 필요";
            }
            else
            {
                // 스테이지 클리어 완료
                purchaseButton.interactable = true;
                purchaseButtonText.color = enabledTextColor;
                purchaseButtonText.text = $"구매 ({recipe.PurchasePrice} G)";
            }
        }
        else
        {
            // Secret 레시피
            if (!isStageCleared)
            {
                // 스테이지를 아직 클리어하지 않음
                purchaseButton.interactable = false;
                purchaseButtonText.color = disabledTextColor;
                purchaseButtonText.text = $"스테이지 {recipe.ExclusiveStageNumber} 클리어 필요";
            }
            else
            {
                // 스테이지 클리어 완료
                purchaseButton.interactable = true;
                purchaseButtonText.color = enabledTextColor;
                purchaseButtonText.text = $"구매 ({recipe.DellyPrice} D)";
            }
        }
    }

    /// <summary>
    /// 셀이 클릭되었을 때 호출됩니다.
    /// </summary>
    public void OnButtonClicked()
    {
        if (currentRecipe == null)
        {
            Debug.LogWarning("[ShopRecipeCell] 현재 레시피가 null입니다.");
            return;
        }

        // 이미 구매한 레시피인지 확인
        if (SaveSystem.Instance.IsRecipePurchased(currentRecipe))
        {
            UIManager.Instance.ShowMessage($"{currentRecipe.RecipeName}은(는) 이미 구매한 레시피입니다.");
            return;
        }

        // 기본 레시피는 구매 불가
        if (currentRecipe.Type == RecipeType.Default)
        {
            UIManager.Instance.ShowMessage("기본 레시피는 구매할 수 없습니다.");
            return;
        }

        // StageExclusive 및 Secret 레시피: 스테이지 클리어 여부 확인
        if (currentRecipe.Type == RecipeType.StageExclusive || currentRecipe.Type == RecipeType.Secret)
        {
            bool isStageCleared = SaveSystem.Instance.IsStageCleared(currentRecipe.ShopType, currentRecipe.ExclusiveStageNumber);
            if (!isStageCleared)
            {
                UIManager.Instance.ShowMessage($"스테이지 {currentRecipe.ExclusiveStageNumber}을(를) 먼저 클리어해야 합니다.");
                return;
            }
        }

        // 재화 확인 메시지
        string currencyType = "";
        int requiredAmount = 0;
        int currentAmount = 0;

        if (currentRecipe.Type == RecipeType.StageExclusive)
        {
            currencyType = "Gold";
            requiredAmount = currentRecipe.PurchasePrice;
            currentAmount = SaveSystem.Instance.GetGold();
        }
        else if (currentRecipe.Type == RecipeType.Secret)
        {
            currencyType = "Delly";
            requiredAmount = currentRecipe.DellyPrice;
            currentAmount = SaveSystem.Instance.GetDelly();
        }

        // 재화 부족 체크
        if (currentAmount < requiredAmount)
        {
            UIManager.Instance.ShowMessage($"{currencyType}가 부족합니다. (필요: {requiredAmount}, 보유: {currentAmount})");
            return;
        }

        // 구매 시도
        bool success = SaveSystem.Instance.PurchaseRecipe(currentRecipe);

        if (success)
        {
            UIManager.Instance.ShowMessage($"{currentRecipe.RecipeName} 레시피를 구매했습니다!");

            // UI 업데이트 (구매 완료 상태로 변경)
            Setup(currentRecipe);
        }
        else
        {
            UIManager.Instance.ShowMessage($"{currentRecipe.RecipeName} 구매에 실패했습니다.");
        }
    }
}
