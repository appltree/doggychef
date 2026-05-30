using System;
using UnityEngine;

namespace DoggyChef
{
    // 플레이어 재화(Gold, Gum, Delly)를 PlayerPrefs에 JSON으로 저장·관리합니다.
    [Serializable]
    public class PlayerCurrency
    {
        // ════════════════════════════════════════════════════════════════
        //  상수
        // ════════════════════════════════════════════════════════════════

        private const string SaveKey = "PlayerCurrency";
        private const int MaxGum = 5;
        private const int InitialGum = 5;
        private const float GumRecoveryMinutes = 10f;

        // ════════════════════════════════════════════════════════════════
        //  직렬화 필드 ([Serializable] 클래스의 JSON 저장용 필드)
        // ════════════════════════════════════════════════════════════════

        [SerializeField] private int m_Gold;
        [SerializeField] private int m_Gum = InitialGum;
        [SerializeField] private int m_Delly;
        [SerializeField] private long m_LastGumRecoveryTimeTicks;

        // ════════════════════════════════════════════════════════════════
        //  이벤트
        // ════════════════════════════════════════════════════════════════

        // 재화가 변경될 때 (gold, gum, delly) 발행됩니다.
        public event Action<int, int, int> OnChanged;

        // ════════════════════════════════════════════════════════════════
        //  public 런타임 상태
        // ════════════════════════════════════════════════════════════════

        public int Gold => m_Gold;
        public int Gum
        {
            get
            {
                UpdateGumRecovery();
                return m_Gum;
            }
        }
        public int Delly => m_Delly;
        public int GumMax => MaxGum;

        // ════════════════════════════════════════════════════════════════
        //  public API
        // ════════════════════════════════════════════════════════════════

        // PlayerPrefs에서 재화 데이터를 로드합니다. 데이터 없으면 초기값으로 생성합니다.
        public static PlayerCurrency Load()
        {
            string json = PlayerPrefs.GetString(SaveKey, "");
            if (!string.IsNullOrEmpty(json))
            {
                try
                {
                    var loaded = JsonUtility.FromJson<PlayerCurrency>(json);
                    loaded.UpdateGumRecovery();
                    return loaded;
                }
                catch (Exception e) { Debug.LogWarning($"[PlayerCurrency] 로드 실패: {e.Message}"); }
            }

            var newCurrency = new PlayerCurrency();
            newCurrency.m_LastGumRecoveryTimeTicks = DateTime.Now.Ticks;
            return newCurrency;
        }

        // 다음 Gum 회복까지 남은 시간(초)을 반환합니다.
        public float GetTimeUntilNextGumRecovery()
        {
            if (m_Gum >= MaxGum) return 0f;
            var lastRecovery = new DateTime(m_LastGumRecoveryTimeTicks);
            TimeSpan remaining = lastRecovery.AddMinutes(GumRecoveryMinutes) - DateTime.Now;
            return Mathf.Max(0f, (float)remaining.TotalSeconds);
        }

        // Gold를 amount만큼 추가합니다.
        public int AddGold(int amount)
        {
            if (amount <= 0) return 0;
            m_Gold += amount;
            Save();
            return amount;
        }

        // Gold를 amount만큼 차감합니다. 잔액 부족 시 0을 반환합니다.
        public int SpendGold(int amount)
        {
            if (amount <= 0 || m_Gold < amount) return 0;
            m_Gold -= amount;
            Save();
            return amount;
        }

        // Gold 잔액이 amount 이상인지 확인합니다.
        public bool HasEnoughGold(int amount) => m_Gold >= amount;

        // Gum을 amount만큼 추가합니다. MaxGum을 초과하지 않습니다.
        public int AddGum(int amount)
        {
            if (amount <= 0) return 0;
            m_Gum = Mathf.Min(m_Gum + amount, MaxGum);
            m_LastGumRecoveryTimeTicks = DateTime.Now.Ticks;
            Save();
            return amount;
        }

        // 게임 시작 시 Gum 1개를 소모합니다. isContinuePlay=true면 소모하지 않습니다.
        // Returns: false=Gum 부족으로 소모 불가
        public bool SpendGumForGame(bool isContinuePlay = false)
        {
            if (isContinuePlay) return true;

            UpdateGumRecovery();
            if (m_Gum <= 0)
            {
                Debug.LogWarning("[PlayerCurrency] Gum 부족!");
                return false;
            }

            m_Gum--;
            m_LastGumRecoveryTimeTicks = DateTime.Now.Ticks;
            Save();
            Debug.Log($"[PlayerCurrency] Gum 소모: {m_Gum + 1} → {m_Gum}");
            return true;
        }

        // Gum을 amount만큼 차감합니다. 잔액 부족 시 0을 반환합니다.
        public int SpendGum(int amount)
        {
            if (amount <= 0 || m_Gum < amount) return 0;
            m_Gum -= amount;
            m_LastGumRecoveryTimeTicks = DateTime.Now.Ticks;
            Save();
            return amount;
        }

        // Gum 잔액이 amount 이상인지 확인합니다 (자동 회복 후 판단).
        public bool HasEnoughGum(int amount)
        {
            UpdateGumRecovery();
            return m_Gum >= amount;
        }

        // Delly를 amount만큼 추가합니다.
        public int AddDelly(int amount)
        {
            if (amount <= 0) return 0;
            m_Delly += amount;
            Save();
            return amount;
        }

        // Delly를 amount만큼 차감합니다. 잔액 부족 시 0을 반환합니다.
        public int SpendDelly(int amount)
        {
            if (amount <= 0 || m_Delly < amount) return 0;
            m_Delly -= amount;
            Save();
            return amount;
        }

        // Delly 잔액이 amount 이상인지 확인합니다.
        public bool HasEnoughDelly(int amount) => m_Delly >= amount;

        // 모든 재화를 초기값으로 리셋합니다.
        public void ResetAll()
        {
            m_Gold = 0;
            m_Gum = InitialGum;
            m_Delly = 0;
            m_LastGumRecoveryTimeTicks = DateTime.Now.Ticks;
            Save();
        }

        // 현재 재화 상태를 로그로 출력합니다.
        [System.Diagnostics.Conditional("UNITY_EDITOR")]
        public void Log()
        {
            Debug.Log($"[재화] Gold: {m_Gold}, Gum: {m_Gum}/{MaxGum} (다음 회복까지: {GetTimeUntilNextGumRecovery():F0}초), Delly: {m_Delly}");
        }

        // ════════════════════════════════════════════════════════════════
        //  private 구현
        // ════════════════════════════════════════════════════════════════

        // 마지막 회복 시간부터 경과 시간을 계산하여 Gum을 자동 회복합니다. 10분당 1개, 최대 5개.
        private void UpdateGumRecovery()
        {
            if (m_Gum >= MaxGum)
            {
                m_LastGumRecoveryTimeTicks = DateTime.Now.Ticks;
                return;
            }

            var lastRecovery = new DateTime(m_LastGumRecoveryTimeTicks);
            TimeSpan elapsed = DateTime.Now - lastRecovery;
            int recoveryCount = (int)(elapsed.TotalMinutes / GumRecoveryMinutes);

            if (recoveryCount > 0)
            {
                int oldGum = m_Gum;
                m_Gum = Mathf.Min(m_Gum + recoveryCount, MaxGum);
                m_LastGumRecoveryTimeTicks = lastRecovery.AddMinutes(recoveryCount * GumRecoveryMinutes).Ticks;

                if (oldGum != m_Gum)
                {
                    Save();
                    Debug.Log($"[PlayerCurrency] Gum 자동 회복: {oldGum} → {m_Gum}");
                }
            }
        }

        // PlayerPrefs에 JSON으로 저장하고 OnChanged 이벤트를 발행합니다.
        private void Save()
        {
            PlayerPrefs.SetString(SaveKey, JsonUtility.ToJson(this));
            PlayerPrefs.Save();
            OnChanged?.Invoke(m_Gold, m_Gum, m_Delly);
        }
    }
}
