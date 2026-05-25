using System;
using UnityEngine;

[Serializable]
public class PlayerCurrency
{
    private const string SAVE_KEY = "PlayerCurrency";
    private const int MAX_GUM = 5;
    private const int INITIAL_GUM = 5;
    private const float GUM_RECOVERY_TIME_MINUTES = 10f;

    [SerializeField] private int gold;
    [SerializeField] private int gum = INITIAL_GUM;
    [SerializeField] private int delly;
    [SerializeField] private long lastGumRecoveryTimeTicks; // DateTime.Ticks로 저장

    public int Gold => gold;
    public int Gum
    {
        get
        {
            UpdateGumRecovery();
            return gum;
        }
    }
    public int Delly => delly;
    public int MaxGum => MAX_GUM;

    public event Action<int, int, int> OnChanged;
    public event Action<int, int> OnGumChanged; // (현재 gum, 최대 gum)

    public static PlayerCurrency Load()
    {
        string json = PlayerPrefs.GetString(SAVE_KEY, "");
        if (!string.IsNullOrEmpty(json))
        {
            try
            {
                PlayerCurrency loaded = JsonUtility.FromJson<PlayerCurrency>(json);
                loaded.UpdateGumRecovery();
                return loaded;
            }
            catch (Exception e) { Debug.LogWarning($"[PlayerCurrency] 로드 실패: {e.Message}"); }
        }

        var newCurrency = new PlayerCurrency();
        newCurrency.lastGumRecoveryTimeTicks = DateTime.Now.Ticks;
        return newCurrency;
    }

    /// <summary>
    /// 마지막 회복 시간부터 경과한 시간을 계산하여 Gum을 자동 회복합니다.
    /// 10분당 1개씩, 최대 5개까지만 회복됩니다.
    /// </summary>
    private void UpdateGumRecovery()
    {
        if (gum >= MAX_GUM)
        {
            lastGumRecoveryTimeTicks = DateTime.Now.Ticks;
            return;
        }

        DateTime lastRecovery = new DateTime(lastGumRecoveryTimeTicks);
        DateTime now = DateTime.Now;
        TimeSpan elapsed = now - lastRecovery;

        int recoveryMinutes = (int)(elapsed.TotalMinutes / GUM_RECOVERY_TIME_MINUTES);

        if (recoveryMinutes > 0)
        {
            int oldGum = gum;
            gum = Mathf.Min(gum + recoveryMinutes, MAX_GUM);

            // 회복된 만큼 시간 업데이트
            lastGumRecoveryTimeTicks = lastRecovery.AddMinutes(recoveryMinutes * GUM_RECOVERY_TIME_MINUTES).Ticks;

            if (oldGum != gum)
            {
                Save();
                Debug.Log($"[PlayerCurrency] Gum 자동 회복: {oldGum} → {gum}");
            }
        }
    }

    /// <summary>
    /// 다음 Gum 회복까지 남은 시간(초)을 반환합니다.
    /// </summary>
    public float GetTimeUntilNextGumRecovery()
    {
        if (gum >= MAX_GUM) return 0f;

        DateTime lastRecovery = new DateTime(lastGumRecoveryTimeTicks);
        DateTime nextRecovery = lastRecovery.AddMinutes(GUM_RECOVERY_TIME_MINUTES);
        TimeSpan remaining = nextRecovery - DateTime.Now;

        return Mathf.Max(0f, (float)remaining.TotalSeconds);
    }

    private void Save()
    {
        PlayerPrefs.SetString(SAVE_KEY, JsonUtility.ToJson(this));
        PlayerPrefs.Save();
        OnChanged?.Invoke(gold, gum, delly);
        OnGumChanged?.Invoke(gum, MAX_GUM);
    }

    public int AddGold(int amount)
    {
        if (amount <= 0) return 0;
        gold += amount;
        Save();
        return amount;
    }

    public int SpendGold(int amount)
    {
        if (amount <= 0 || gold < amount) return 0;
        gold -= amount;
        Save();
        return amount;
    }

    public bool HasEnoughGold(int amount) => gold >= amount;

    public int AddGum(int amount)
    {
        if (amount <= 0) return 0;
        gum = Mathf.Min(gum + amount, MAX_GUM);
        lastGumRecoveryTimeTicks = DateTime.Now.Ticks;
        Save();
        return amount;
    }

    /// <summary>
    /// 게임 시작 시 Gum을 소모합니다.
    /// </summary>
    /// <param name="isContinuePlay">클리어 후 연속 플레이 여부 (true면 소모 안 함)</param>
    /// <returns>소모 성공 여부</returns>
    public bool SpendGumForGame(bool isContinuePlay = false)
    {
        if (isContinuePlay)
        {
            Debug.Log("[PlayerCurrency] 연속 플레이 - Gum 소모 없음");
            return true;
        }

        UpdateGumRecovery();

        if (gum <= 0)
        {
            Debug.LogWarning("[PlayerCurrency] Gum 부족!");
            return false;
        }

        gum--;
        lastGumRecoveryTimeTicks = DateTime.Now.Ticks;
        Save();
        Debug.Log($"[PlayerCurrency] Gum 소모: {gum + 1} → {gum}");
        return true;
    }

    public int SpendGum(int amount)
    {
        if (amount <= 0 || gum < amount) return 0;
        gum -= amount;
        lastGumRecoveryTimeTicks = DateTime.Now.Ticks;
        Save();
        return amount;
    }

    public bool HasEnoughGum(int amount)
    {
        UpdateGumRecovery();
        return gum >= amount;
    }

    public int AddDelly(int amount)
    {
        if (amount <= 0) return 0;
        delly += amount;
        Save();
        return amount;
    }

    public int SpendDelly(int amount)
    {
        if (amount <= 0 || delly < amount) return 0;
        delly -= amount;
        Save();
        return amount;
    }

    public bool HasEnoughDelly(int amount) => delly >= amount;

    public void ResetAll()
    {
        gold = gum = delly = 0;
        gum = INITIAL_GUM;
        lastGumRecoveryTimeTicks = DateTime.Now.Ticks;
        Save();
    }

    [System.Diagnostics.Conditional("UNITY_EDITOR")]
    public void Log()
    {
        float nextRecoverySeconds = GetTimeUntilNextGumRecovery();
        Debug.LogFormat($"[재화] Gold: {gold}, Gum: {gum}/{MAX_GUM} (다음 회복까지: {nextRecoverySeconds:F0}초), Delly: {delly}");
    }
}
