using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace DoggyChef
{
    // 상점 레시피 탭의 개별 레시피 셀입니다.
    public class ShopRecipeCell : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  Inspector 연결 필드
        // ════════════════════════════════════════════════════════════════

        [Header("UI References")]
        [Tooltip("레시피 결과물 이미지")]
        [SerializeField] private Image m_RecipeImage;
        [Tooltip("레시피 이름 텍스트")]
        [SerializeField] private TextMeshProUGUI m_RecipeName;
        [Tooltip("레시피 설명 텍스트")]
        [SerializeField] private TextMeshProUGUI m_RecipeDescription;
        [Tooltip("판매 가격 텍스트")]
        [SerializeField] private TextMeshProUGUI m_SellPriceText;
        [Tooltip("구매 버튼")]
        [SerializeField] private Button m_PurchaseButton;
        [Tooltip("구매 버튼 텍스트")]
        [SerializeField] private TextMeshProUGUI m_PurchaseButtonText;

        [Header("버튼 색상")]
        [Tooltip("구매 가능 상태의 버튼 텍스트 색상")]
        [SerializeField] private Color m_EnabledColor = Color.white;
        [Tooltip("구매 불가 상태의 버튼 텍스트 색상")]
        [SerializeField] private Color m_DisabledColor = new Color(0.533f, 0.533f, 0.533f, 1f);

        // ════════════════════════════════════════════════════════════════
        //  private 런타임 상태
        // ════════════════════════════════════════════════════════════════

        private RecipeData m_CurrentRecipe;
        private ShopType m_ShopType;

        // ════════════════════════════════════════════════════════════════
        //  public API
        // ════════════════════════════════════════════════════════════════

        // [ShopTabRecipe] 레시피 정보를 셀에 표시합니다.
        public void Setup(RecipeData recipe, ShopType shopType)
        {
            if (recipe == null)
            {
                Debug.LogWarning("[ShopRecipeCell] recipe가 null입니다.");
                return;
            }

            m_CurrentRecipe = recipe;
            m_ShopType = shopType;

            if (m_RecipeImage != null)
            {
                m_RecipeImage.enabled = recipe.ResultSprite != null;
                if (recipe.ResultSprite != null)
                {
                    m_RecipeImage.sprite = recipe.ResultSprite;
                    m_RecipeImage.SetNativeSize();
                }
            }

            if (m_RecipeName != null)
                m_RecipeName.text = string.IsNullOrWhiteSpace(recipe.Name) ? recipe.RecipeTag : recipe.Name;

            if (m_RecipeDescription != null) m_RecipeDescription.text = recipe.Description;
            if (m_SellPriceText != null) m_SellPriceText.text = $"판매가격: {recipe.SellPrice} G";

            RefreshPurchaseButton();
        }

        // [구매 버튼] 유효성 확인 후 레시피를 구매합니다.
        public void OnButtonClicked()
        {
            if (m_CurrentRecipe == null) return;

            if (SaveSystem.Instance.IsRecipePurchased(m_CurrentRecipe))
            {
                UIManager.Instance?.ShowMessage($"{GetDisplayName()} 은(는) 이미 구매한 레시피입니다.");
                return;
            }

            if (m_CurrentRecipe.Type == RecipeType.Default)
            {
                UIManager.Instance?.ShowMessage("기본 레시피는 구매할 수 없습니다.");
                return;
            }

            // StageExclusive / Secret: 해당 스테이지 클리어 여부 확인
            if (m_CurrentRecipe.Type == RecipeType.StageExclusive || m_CurrentRecipe.Type == RecipeType.Secret)
            {
                if (!SaveSystem.Instance.IsStageCleared(m_ShopType, m_CurrentRecipe.ExclusiveStageNumber))
                {
                    UIManager.Instance?.ShowMessage($"스테이지 {m_CurrentRecipe.ExclusiveStageNumber}을(를) 먼저 클리어해야 합니다.");
                    return;
                }
            }

            // 재화 부족 확인
            bool isGoldPurchase = m_CurrentRecipe.Type == RecipeType.StageExclusive;
            int required = isGoldPurchase ? m_CurrentRecipe.PurchasePrice : m_CurrentRecipe.SpecialPrice;
            int current = isGoldPurchase ? SaveSystem.Instance.GetGold() : SaveSystem.Instance.GetDelly();
            string currency = isGoldPurchase ? "Gold" : "Delly";

            if (current < required)
            {
                UIManager.Instance?.ShowMessage($"{currency}가 부족합니다. (필요: {required}, 보유: {current})");
                return;
            }

            bool success = SaveSystem.Instance.PurchaseRecipe(m_CurrentRecipe);
            UIManager.Instance?.ShowMessage(success
                ? $"{GetDisplayName()} 레시피를 구매했습니다!"
                : $"{GetDisplayName()} 구매에 실패했습니다.");

            if (success) Setup(m_CurrentRecipe, m_ShopType);
        }

        // ════════════════════════════════════════════════════════════════
        //  private 구현
        // ════════════════════════════════════════════════════════════════

        // 구매 가능 여부에 따라 버튼 상태를 갱신합니다.
        private void RefreshPurchaseButton()
        {
            if (m_PurchaseButton == null) return;

            bool purchased = SaveSystem.Instance.IsRecipePurchased(m_CurrentRecipe);

            if (m_CurrentRecipe.Type == RecipeType.Default)
            {
                SetButton(false, "기본 레시피");
                return;
            }

            if (purchased)
            {
                SetButton(false, "구매 완료");
                return;
            }

            bool stageCleared = SaveSystem.Instance.IsStageCleared(m_ShopType, m_CurrentRecipe.ExclusiveStageNumber);

            if (m_CurrentRecipe.Type == RecipeType.StageExclusive || m_CurrentRecipe.Type == RecipeType.Secret)
            {
                if (!stageCleared)
                {
                    SetButton(false, $"스테이지 {m_CurrentRecipe.ExclusiveStageNumber} 클리어 필요");
                    return;
                }
            }

            if (m_CurrentRecipe.Type == RecipeType.StageExclusive)
                SetButton(true, $"구매 ({m_CurrentRecipe.PurchasePrice} G)");
            else
                SetButton(true, $"구매 ({m_CurrentRecipe.SpecialPrice} D)");
        }

        // 버튼 interactable 상태와 레이블 텍스트·색상을 설정합니다.
        private void SetButton(bool interactable, string label)
        {
            m_PurchaseButton.interactable = interactable;
            if (m_PurchaseButtonText != null)
            {
                m_PurchaseButtonText.text = label;
                m_PurchaseButtonText.color = interactable ? m_EnabledColor : m_DisabledColor;
            }
        }

        // 레시피 표시 이름을 반환합니다. Name이 비어 있으면 RecipeTag를 사용합니다.
        private string GetDisplayName()
        {
            return string.IsNullOrWhiteSpace(m_CurrentRecipe.Name) ? m_CurrentRecipe.RecipeTag : m_CurrentRecipe.Name;
        }
    }
}
