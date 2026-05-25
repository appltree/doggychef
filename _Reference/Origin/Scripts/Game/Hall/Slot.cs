using TMPro;
using UnityEngine;

public class Slot : MonoBehaviour
{
    [Header("Slot 설정")]
    [SerializeField] private SpriteRenderer ingredientSpriteRenderer; // 인스펙터에서 지정할 슬롯 스프라이트
    [SerializeField] private TextMeshProUGUI countText;

    private Color activeColor = Color.yellow; // 남은 수량이 1 이상일 때 색
    private Color completedColor = new(0.5f, 0.5f, 0.5f, 1f); // 0일 때 회색

    private string ingredientTag;
    private int remaining; // 남은 필요 수량

    public string IngredientTag => ingredientTag;


    public void Setup(Sprite sprite, string tag, int remainingNeed)
    {
        ingredientTag = tag;
        remaining = Mathf.Max(0, remainingNeed);
        if (ingredientSpriteRenderer != null) ingredientSpriteRenderer.sprite = sprite;
        Refresh();
    }


    public void UpdateRemaining(int remainingNeed)
    {
        remaining = Mathf.Max(0, remainingNeed);
        Refresh();
    }


    private void Refresh()
    {
        // 텍스트 갱신
        if (countText != null)
        {
            countText.text = remaining > 0 ? remaining.ToString() : "0"; // 0이면 완료
            // 남은 수량: 1 이상이면 노란색, 0이면 회색 (텍스트 색상 변경)
            countText.color = remaining > 0 ? activeColor : completedColor;
        }
        // 아이콘 페이드
        if (ingredientSpriteRenderer != null)
        {
            ingredientSpriteRenderer.color = remaining > 0 ? Color.white : new Color(1f, 1f, 1f, 0.4f);
        }
    }

}
