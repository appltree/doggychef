using UnityEngine;

[CreateAssetMenu(fileName = "Block", menuName = "DoggyChef/Block")]
public class BlockSO : ScriptableObject
{
    [Header("Block Data")]
    [SerializeField] private string blockTag;
    [SerializeField] private Sprite sprite;
    [Header("Audio")]
    [Tooltip("이 블럭이 재료로 수집될 때 재생할 고유 음식 소리")]
    [SerializeField] private AudioClip foodSfx;
    [Header("Seed Mapping")]
    [Tooltip("디버그 시딩 시 이 블럭에 매핑할 문자(선택 사항)")]
    [SerializeField] private char mappingKey;

    public string Tag => blockTag;
    public Sprite Sprite => sprite;
    public AudioClip FoodSfx => foodSfx;
    public char Key => mappingKey;


    public void ApplyTo(Block block)
    {
        if (block == null) return;
        block.SetupBlock(sprite, blockTag);
    }
}
