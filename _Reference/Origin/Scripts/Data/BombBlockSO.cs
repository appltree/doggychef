using UnityEngine;

[CreateAssetMenu(fileName = "BombBlock", menuName = "DoggyChef/Bomb Block")]
public class BombBlockSO : ScriptableObject
{
    [Header("Bomb Data")]
    [SerializeField] private BombBlock.BombType bombType = BombBlock.BombType.None;
    [SerializeField] private GameObject bombPrefab;
    [SerializeField] private string blockTag;

    [Header("Seed Mapping")]
    [SerializeField] private char mappingKey;

    public BombBlock.BombType Type => bombType;
    public GameObject Prefab => bombPrefab;
    public char Key => mappingKey;
    public string Tag => blockTag;


    public void ApplyTo(BombBlock block)
    {
        if (block == null) return;
        block.SetupBlock(null, blockTag);
    }
}
