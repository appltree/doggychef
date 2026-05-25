using UnityEngine;

[CreateAssetMenu(fileName = "BoardDebugSeed", menuName = "DoggyChef/Debug/Board Debug Seed", order = 1000)]
public class BoardDebugSeedAsset : ScriptableObject
{
    [Header("Rows (bottom = index 0)")]
    [Tooltip("각 문자열은 한 행을 의미하며 index 0이 최하단(y=0)입니다. 공백은 비워둡니다.")]
    public string[] rows;

    [Header("Ice Overlay")]
    [Tooltip("블럭 위에 얼음을 붙일 좌표 목록입니다. (0,0)은 보드의 최하단-좌측이며, 유효 범위 밖이거나 블럭이 없는 칸은 무시됩니다.")]
    public Vector2Int[] icePositions;
}
