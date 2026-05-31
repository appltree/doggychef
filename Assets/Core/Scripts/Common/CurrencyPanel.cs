using TMPro;
using UnityEngine;

namespace DoggyChef
{
    // 로비 씬에서 Gold / Gum / Delly를 표시하고 Gum 회복 타이머를 갱신합니다.
    public class CurrencyPanel : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  Inspector 연결 필드
        // ════════════════════════════════════════════════════════════════

        [Header("재화 텍스트")]
        [Tooltip("Gold 수치 텍스트")]
        [SerializeField] private TextMeshProUGUI m_GoldValueText;
        [Tooltip("Gum 수치 텍스트")]
        [SerializeField] private TextMeshProUGUI m_GumValueText;
        [Tooltip("Gum 회복 타이머 텍스트 (최대치면 숨김)")]
        [SerializeField] private TextMeshProUGUI m_GumTimeText;
        [Tooltip("Delly 수치 텍스트")]
        [SerializeField] private TextMeshProUGUI m_DellyValueText;

        // ════════════════════════════════════════════════════════════════
        //  private 런타임 상태
        // ════════════════════════════════════════════════════════════════

        private PlayerCurrency m_PlayerCurrency;

        // ════════════════════════════════════════════════════════════════
        //  Unity 생명 주기
        // ════════════════════════════════════════════════════════════════

        // PlayerCurrency 참조를 캐시하고, 초기 UI를 설정한 뒤 OnCurrencyChanged를 구독합니다.
        private void Start()
        {
            m_PlayerCurrency = SaveSystem.Instance.GetPlayerCurrency();

            UpdateCurrencyUI(
                SaveSystem.Instance.GetGold(),
                SaveSystem.Instance.GetGum(),
                SaveSystem.Instance.GetDelly()
            );

            SaveSystem.Instance.OnCurrencyChanged += UpdateCurrencyUI;
        }

        // Gum이 최대치 미만일 때마다 회복 타이머 텍스트를 갱신합니다.
        private void Update()
        {
            if (m_PlayerCurrency != null && m_PlayerCurrency.Gum < m_PlayerCurrency.GumMax)
                UpdateGumTimer();
        }

        // OnCurrencyChanged 구독을 해제합니다.
        private void OnDestroy()
        {
            if (SaveSystem.Instance != null)
                SaveSystem.Instance.OnCurrencyChanged -= UpdateCurrencyUI;
        }

        // ════════════════════════════════════════════════════════════════
        //  public API
        // ════════════════════════════════════════════════════════════════

        // [ShopButton] 상점 팝업을 엽니다.
        public void OnShopPressed()
        {
            Debug.Log("OnShopPressed");
            UIManager.Instance?.ShowShopPopup();
        }

        // ════════════════════════════════════════════════════════════════
        //  private 구현
        // ════════════════════════════════════════════════════════════════

        // OnCurrencyChanged 이벤트를 받아 재화 UI를 갱신합니다.
        private void UpdateCurrencyUI(int gold, int gum, int delly)
        {
            if (m_GoldValueText != null) m_GoldValueText.text = gold.ToString();
            if (m_GumValueText != null) m_GumValueText.text = gum.ToString();
            if (m_DellyValueText != null) m_DellyValueText.text = delly.ToString();

            if (m_GumTimeText != null)
                m_GumTimeText.gameObject.SetActive(m_PlayerCurrency != null && gum < m_PlayerCurrency.GumMax);
        }

        // Gum 회복 타이머를 MM:SS 형식으로 갱신합니다.
        private void UpdateGumTimer()
        {
            if (m_GumTimeText == null || m_PlayerCurrency == null) return;

            float remaining = m_PlayerCurrency.GetTimeUntilNextGumRecovery();
            if (remaining <= 0f)
            {
                m_GumTimeText.gameObject.SetActive(false);
                return;
            }

            int minutes = Mathf.FloorToInt(remaining / 60f);
            int seconds = Mathf.FloorToInt(remaining % 60f);
            m_GumTimeText.text = $"{minutes}:{seconds:D2}";
        }
    }
}
