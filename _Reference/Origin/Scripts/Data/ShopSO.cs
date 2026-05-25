using UnityEngine;


public enum ShopType
{
    Boonsik,
    Hamburger,
    Coffee,
    Pizza,
    Korean,
    Chinese,
}


[CreateAssetMenu(fileName = "Shop", menuName = "DoggyChef/Shop")]
public class ShopSO : ScriptableObject
{
    [SerializeField, Tooltip("타입")]
    private ShopType type;

    [SerializeField, Tooltip("심볼")]
    private Sprite symbol;

    [SerializeField, Tooltip("색상")]
    private Color mainColor;

    [SerializeField] private string shopName;

    [SerializeField] private StageSO[] stages;

    [SerializeField] private RecipeSO[] recipes;

    // Public Properties
    public ShopType Type => type;
    public Sprite Symbol => symbol;
    public Color MainColor => mainColor;
    public string ShopName => shopName;
    public StageSO[] Stages => stages;
    public RecipeSO[] Recipes => recipes;

}
