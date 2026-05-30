using UnityEngine;

namespace DoggyChef
{
  [CreateAssetMenu(fileName = "NewStageData", menuName = "Tycoon/StageData")]
  public class StageData : ScriptableObject
  {
    [Tooltip("스테이지 번호")]
    public int Number = 1;

    [Tooltip("제한 시간 (초)")]
    public float TimeLimit = 60f;

    [Tooltip("목표 골드")]
    public int GoalGold = 800;

    [Tooltip("퍼펙트 골드")]
    public int GoalPerpect = 1000;

    [Tooltip("피버 발동에 필요한 하트 수")]
    public int FeverHeartCount = 10;

    [Header("Board Config")]
    [Tooltip("이 스테이지의 보드에 출현할 보석 데이터 목록")]
    public System.Collections.Generic.List<GemData> AvailableGems = new();
  }
}
