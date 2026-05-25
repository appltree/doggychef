using System;
using System.Collections.Generic;
using UnityEngine;

namespace MyMatch3
{
  /// <summary>
  /// 게임 내 모든 사용자의 휘발성 데이터를 담는 컨테이너 클래스입니다.
  /// 사운드 설정, 재화, 부스터 인벤토리 등을 한꺼번에 관리합니다.
  /// </summary>
  [Serializable]
  public class UserPersistentData
  {
    [Header("사운드 설정 (0.0~1.0)")]
    public float MainVolume = 1.0f;
    public float MusicVolume = 1.0f;
    public float SFXVolume = 1.0f;

    [Header("재화 및 수치")]
    public int Coins = 0;
    public int Stars = 0;
    public int Lives = 5;

    [Header("부스터 인벤토리 (이름-수량 매핑)")]
    public List<BoosterSaveData> Boosters = new();

    /// <summary>
    /// 특정 부스터의 보유량을 설정하고 저장 준비를 합니다.
    /// </summary>
    public void SetBoosterAmount(string name, int amount)
    {
      var data = Boosters.Find(b => b.Name == name);
      if (data != null)
      {
        data.Amount = amount;
      }
      else
      {
        Boosters.Add(new BoosterSaveData { Name = name, Amount = amount });
      }
    }

    /// <summary>
    /// 특정 부스터의 보유량을 가져옵니다. 저장된 정보가 없으면 기본값을 반환합니다.
    /// </summary>
    public int GetBoosterAmount(string name, int defaultAmount)
    {
      var data = Boosters.Find(b => b.Name == name);
      return data != null ? data.Amount : defaultAmount;
    }

  }

  [Serializable]
  public class BoosterSaveData
  {
    public string Name;   // 부스터 프리팹/스크립트의 이름 (ID 대용)
    public int Amount;    // 현재 보유 수량
  }
}
