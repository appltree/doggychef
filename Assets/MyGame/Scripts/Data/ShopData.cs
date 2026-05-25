using UnityEngine;

namespace MyMatch3
{
  public enum ShopType
  {
    Boonsik,
    Hamburger,
    Coffee,
    Pizza,
    Korean,
    Chinese,
  }

  [CreateAssetMenu(fileName = "NewShopData", menuName = "Tycoon/ShopData")]
  public class ShopData : ScriptableObject
  {
    [Tooltip("상점 고유 식별/분류 타입")]
    public ShopType Type;

    [Tooltip("상점 심볼(아이콘) 이미지")]
    public Sprite Symbol;

    [Tooltip("상점 대표 색상 (UI 등에 사용)")]
    public Color MainColor = Color.white;

    [Tooltip("상점 노출 이름")]
    public string ShopName;

    [Header("Shop Content")]
    [Tooltip("이 상점에 속한 각 단계별 스테이지 목록")]
    public StageData[] Stages;

    [Tooltip("이 상점에서 판매 및 사용하는 레시피(요리) 목록")]
    public RecipeData[] Recipes;
  }
}
