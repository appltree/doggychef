using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

[Serializable]
public class StageProgress
{
    [SerializeField] private ShopType shopType;
    [SerializeField] private int stageNumber;
    [SerializeField] private int bestMoney;
    [SerializeField] private int goalMoney;
    [SerializeField] private int perfectMoney;

    public ShopType ShopType => shopType;
    public int StageNumber => stageNumber;
    public int BestMoney => bestMoney;
    public int GoalMoney => goalMoney;
    public int PerfectMoney => perfectMoney;
    public bool IsStageCleared => bestMoney >= goalMoney;
    public bool IsStagePerfect => bestMoney >= perfectMoney;

    private static readonly Dictionary<string, StageProgress> cache = new();

    public StageProgress() { }

    public StageProgress(ShopType shopType, int stageNumber, int earnedMoney, int goalMoney, int perfectMoney)
    {
        this.shopType = shopType;
        this.stageNumber = stageNumber;
        this.bestMoney = earnedMoney;
        this.goalMoney = goalMoney;
        this.perfectMoney = perfectMoney;
    }

    private static string GetKey(ShopType shopType, int stageNumber)
        => $"Stage_{shopType}_{stageNumber}";

    private string GetKey() => GetKey(shopType, stageNumber);

    public static void LoadAll()
    {
        cache.Clear();
        foreach (ShopType shopType in Enum.GetValues(typeof(ShopType)))
        {
            for (int stage = 1; stage <= 20; stage++)
            {
                string key = GetKey(shopType, stage);
                string json = PlayerPrefs.GetString(key, "");
                if (!string.IsNullOrEmpty(json))
                {
                    try
                    {
                        StageProgress progress = JsonUtility.FromJson<StageProgress>(json);
                        if (progress != null) cache[key] = progress;
                    }
                    catch { }
                }
            }
        }
    }

    public static void Record(ShopType shopType, int stageNumber, int earnedMoney, int goalMoney, int perfectMoney)
    {
        string key = GetKey(shopType, stageNumber);

        if (cache.TryGetValue(key, out StageProgress existing))
        {
            if (earnedMoney > existing.bestMoney)
            {
                existing.bestMoney = earnedMoney;
                existing.Save();
            }
        }
        else
        {
            StageProgress newProgress = new StageProgress(shopType, stageNumber, earnedMoney, goalMoney, perfectMoney);
            cache[key] = newProgress;
            newProgress.Save();
        }
    }

    private void Save()
    {
        PlayerPrefs.SetString(GetKey(), JsonUtility.ToJson(this));
        PlayerPrefs.Save();
    }

    public static bool IsCleared(ShopType shopType, int stageNumber)
    {
        return cache.TryGetValue(GetKey(shopType, stageNumber), out StageProgress progress) && progress.IsStageCleared;
    }

    public static int GetStarCount(ShopType shopType, int stageNumber)
    {
        if (!cache.TryGetValue(GetKey(shopType, stageNumber), out StageProgress progress))
            return 0;
        return CalculateStars(progress.bestMoney, progress.goalMoney, progress.perfectMoney);
    }

    public static int CalculateStars(int earned, int goal, int perfect)
    {
        if (earned < goal) return 0;
        if (earned >= perfect) return 3;
        if (earned > (goal + perfect) / 2) return 2;
        return 1;
    }

    public int GetStarCount() => CalculateStars(bestMoney, goalMoney, perfectMoney);

    public static void ResetAll()
    {
        foreach (var key in cache.Keys.ToList())
        {
            PlayerPrefs.DeleteKey(key);
        }
        cache.Clear();
        PlayerPrefs.Save();
    }

    [System.Diagnostics.Conditional("UNITY_EDITOR")]
    public static void LogAll()
    {
        foreach (var progress in cache.Values.OrderBy(p => p.shopType).ThenBy(p => p.stageNumber))
        {
            Debug.LogFormat($"Stage {progress.shopType}-{progress.stageNumber}: {progress.bestMoney}골드 " +
                     $"(목표: {progress.goalMoney}, 퍼펙트: {progress.perfectMoney}) [{progress.GetStarCount()}별]");
        }
    }
}
