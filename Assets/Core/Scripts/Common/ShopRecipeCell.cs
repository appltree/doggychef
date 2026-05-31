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
        [Tooltip("레시피 재료 목록 루트")]
        [SerializeField] private RectTransform m_IngredientsRoot;
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

            EnsureIngredientRoot();

            if (m_RecipeImage != null)
            {
                m_RecipeImage.enabled = recipe.ResultSprite != null;
                m_RecipeImage.preserveAspect = true;
                if (recipe.ResultSprite != null)
                    m_RecipeImage.sprite = recipe.ResultSprite;
            }

            if (m_RecipeName != null)
                m_RecipeName.text = GetDisplayName();

            if (m_RecipeDescription != null)
                m_RecipeDescription.text = recipe.Description;

            if (m_SellPriceText != null)
                m_SellPriceText.text = $"판매가격: {recipe.SellPrice}G";

            RebuildIngredients();
            RefreshPurchaseButton();
        }

        // [구매 버튼] 유효성 확인 후 레시피를 구매합니다.
        public void OnButtonClicked()
        {
            if (m_CurrentRecipe == null || SaveSystem.Instance == null) return;

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

            if (!IsStageConditionMet())
            {
                UIManager.Instance?.ShowMessage($"스테이지 {m_CurrentRecipe.ExclusiveStageNumber}을(를) 먼저 클리어해야 합니다.");
                return;
            }

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

        // 구매 가능 여부에 따라 버튼 상태와 가격 레이블을 갱신합니다.
        private void RefreshPurchaseButton()
        {
            if (m_PurchaseButton == null) return;

            bool purchased = SaveSystem.Instance != null && SaveSystem.Instance.IsRecipePurchased(m_CurrentRecipe);

            if (m_CurrentRecipe.Type == RecipeType.Default || purchased)
            {
                SetButton(false, "보유중");
                return;
            }

            if (!IsStageConditionMet())
            {
                SetButton(false, "조건달성후 구입가능");
                return;
            }

            if (m_CurrentRecipe.Type == RecipeType.StageExclusive)
                SetButton(true, $"{m_CurrentRecipe.PurchasePrice}G");
            else
                SetButton(true, $"{m_CurrentRecipe.SpecialPrice}D");
        }

        // Returns: true=레시피 구매에 필요한 스테이지 조건을 만족했습니다.
        private bool IsStageConditionMet()
        {
            if (m_CurrentRecipe == null) return false;
            if (m_CurrentRecipe.Type == RecipeType.Default) return true;
            if (SaveSystem.Instance == null) return false;

            return SaveSystem.Instance.IsStageCleared(m_ShopType, m_CurrentRecipe.ExclusiveStageNumber);
        }

        // 버튼 interactable 상태와 레이블 텍스트·색상을 설정합니다.
        private void SetButton(bool interactable, string label)
        {
            m_PurchaseButton.interactable = interactable;

            var colors = m_PurchaseButton.colors;
            colors.normalColor = interactable ? new Color(0.27f, 0.78f, 0.42f, 1f) : new Color(0.74f, 0.80f, 0.82f, 1f);
            colors.highlightedColor = interactable ? new Color(0.32f, 0.86f, 0.48f, 1f) : colors.normalColor;
            colors.pressedColor = interactable ? new Color(0.19f, 0.62f, 0.32f, 1f) : colors.normalColor;
            colors.disabledColor = new Color(0.74f, 0.80f, 0.82f, 1f);
            m_PurchaseButton.colors = colors;

            var image = m_PurchaseButton.GetComponent<Image>();
            if (image != null)
                image.color = interactable ? new Color(0.27f, 0.78f, 0.42f, 1f) : new Color(0.74f, 0.80f, 0.82f, 1f);

            if (m_PurchaseButtonText != null)
            {
                m_PurchaseButtonText.text = label;
                m_PurchaseButtonText.color = interactable ? m_EnabledColor : m_DisabledColor;
                m_PurchaseButtonText.fontStyle = FontStyles.Bold;
            }
        }

        // 재료 아이콘 표시 루트가 프리팹에 없으면 런타임에 생성합니다.
        private void EnsureIngredientRoot()
        {
            if (m_IngredientsRoot != null) return;

            var existing = transform.Find("Ingredients");
            if (existing != null)
            {
                m_IngredientsRoot = existing as RectTransform;
                return;
            }

            var go = new GameObject("Ingredients", typeof(RectTransform), typeof(HorizontalLayoutGroup));
            go.transform.SetParent(transform, false);
            m_IngredientsRoot = go.GetComponent<RectTransform>();
            m_IngredientsRoot.anchorMin = new Vector2(0.5f, 0f);
            m_IngredientsRoot.anchorMax = new Vector2(0.5f, 0f);
            m_IngredientsRoot.pivot = new Vector2(0.5f, 0.5f);
            m_IngredientsRoot.anchoredPosition = new Vector2(0f, 74f);
            m_IngredientsRoot.sizeDelta = new Vector2(210f, 42f);

            var layout = go.GetComponent<HorizontalLayoutGroup>();
            layout.childAlignment = TextAnchor.MiddleCenter;
            layout.childControlHeight = false;
            layout.childControlWidth = false;
            layout.childForceExpandHeight = false;
            layout.childForceExpandWidth = false;
            layout.spacing = 8f;
        }

        // 레시피 재료 목록을 아이콘과 필요 수량 배지로 다시 그립니다.
        private void RebuildIngredients()
        {
            if (m_IngredientsRoot == null) return;

            for (int i = m_IngredientsRoot.childCount - 1; i >= 0; i--)
                Destroy(m_IngredientsRoot.GetChild(i).gameObject);

            if (m_CurrentRecipe.Ingredients == null) return;

            foreach (var ingredient in m_CurrentRecipe.Ingredients)
            {
                if (ingredient.MaterialGem == null) continue;

                var slot = new GameObject("IngredientSlot", typeof(RectTransform), typeof(Image), typeof(LayoutElement));
                slot.transform.SetParent(m_IngredientsRoot, false);
                var slotRect = slot.GetComponent<RectTransform>();
                slotRect.sizeDelta = new Vector2(38f, 38f);

                var slotImage = slot.GetComponent<Image>();
                slotImage.sprite = ingredient.MaterialGem.Sprite;
                slotImage.preserveAspect = true;

                var layoutElement = slot.GetComponent<LayoutElement>();
                layoutElement.preferredWidth = 38f;
                layoutElement.preferredHeight = 38f;

                var badge = new GameObject("Count", typeof(RectTransform), typeof(Image));
                badge.transform.SetParent(slot.transform, false);
                var badgeRect = badge.GetComponent<RectTransform>();
                badgeRect.anchorMin = new Vector2(0f, 1f);
                badgeRect.anchorMax = new Vector2(0f, 1f);
                badgeRect.pivot = new Vector2(0.5f, 0.5f);
                badgeRect.anchoredPosition = new Vector2(3f, -3f);
                badgeRect.sizeDelta = new Vector2(22f, 22f);
                badge.GetComponent<Image>().color = Color.black;

                var countText = new GameObject("Text", typeof(RectTransform), typeof(TextMeshProUGUI));
                countText.transform.SetParent(badge.transform, false);
                var countRect = countText.GetComponent<RectTransform>();
                countRect.anchorMin = Vector2.zero;
                countRect.anchorMax = Vector2.one;
                countRect.sizeDelta = Vector2.zero;

                var text = countText.GetComponent<TextMeshProUGUI>();
                text.text = Mathf.Max(1, ingredient.Count).ToString();
                text.alignment = TextAlignmentOptions.Center;
                text.color = Color.white;
                text.fontSize = 17f;
                text.fontStyle = FontStyles.Bold;
            }
        }

        // 레시피 표시 이름을 반환합니다. Name이 비어 있으면 RecipeTag를 사용합니다.
        private string GetDisplayName()
        {
            return string.IsNullOrWhiteSpace(m_CurrentRecipe.Name) ? m_CurrentRecipe.RecipeTag : m_CurrentRecipe.Name;
        }
    }
}
