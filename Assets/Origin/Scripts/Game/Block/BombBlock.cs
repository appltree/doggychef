// OK
using UnityEngine;

public class BombBlock : Block
{
    public enum BombType
    {
        None,
        Horizontal, // 가로 4개 매치로 생성
        Vertical,   // 세로 4개 매치로 생성
        TNT,
        LightBall,
    }

    [Header("Bomb Settings")]
    [SerializeField] private BombType bombType = BombType.None;
    public BombType GetBombType() => bombType;
}
