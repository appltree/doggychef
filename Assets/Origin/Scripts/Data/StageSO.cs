using UnityEngine;

[CreateAssetMenu(fileName = "Stage", menuName = "DoggyChef/Stage")]
public class StageSO : ScriptableObject
{
    [SerializeField, Min(1), Tooltip("스테이지번호")]
    private int stageNumber;

    [SerializeField, Min(0), Tooltip("목표골드")]
    private int goalGold;

    [SerializeField, Min(0), Tooltip("퍼펙트골드")]
    private int goalPerpect;

    [SerializeField, Min(0), Tooltip("장사시간")]
    private int timeLimit;

    [SerializeField, Min(0), Tooltip("피버 하트수")]
    private int feverHeartCount;

    // 읽기 전용 프로퍼티 (필요시 노출)
    public int StageNumber => stageNumber;
    public int GoalGold => goalGold;
    public int GoalPerpect => goalPerpect;
    public int TimeLimit => timeLimit;
    public int FeverHeartCount => feverHeartCount;
}
