using System.Collections.Generic;
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

    [Tooltip("스테이지 선택/시작 화면에 표시할 메시지")]
    public string StageMessage;

    [Header("Customer Config")]
    [Tooltip("이 스테이지에서 출현 가능한 손님/동물 프리팹 목록")]
    public List<GameObject> AvailableCustomerPrefabs = new();
  }
}
