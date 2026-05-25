using TMPro;
using UnityEngine;

public class CurrencyPanel : MonoBehaviour
{
    [SerializeField] private TextMeshProUGUI goldValueText;
    [SerializeField] private TextMeshProUGUI gumValueText;
    [SerializeField] private TextMeshProUGUI gumTimeText;
    [SerializeField] private TextMeshProUGUI dellyValueText;

    private PlayerCurrency playerCurrency;

    private void Start()
    {
        playerCurrency = SaveSystem.Instance.GetPlayerCurrency();

        // 초기 값 표시
        UpdateCurrencyUI(
            SaveSystem.Instance.GetGold(),
            SaveSystem.Instance.GetGum(),
            SaveSystem.Instance.GetDelly()
        );

        // 변경 이벤트 구독
        SaveSystem.Instance.OnCurrencyChanged += UpdateCurrencyUI;
    }

    private void Update()
    {
        // Gum이 최대치 미만일 때만 타이머 업데이트
        if (playerCurrency != null && playerCurrency.Gum < playerCurrency.MaxGum)
        {
            UpdateGumTimer();
        }
    }

    private void OnDestroy()
    {
        // 이벤트 구독 해제
        if (SaveSystem.Instance != null)
        {
            SaveSystem.Instance.OnCurrencyChanged -= UpdateCurrencyUI;
        }
    }

    private void UpdateCurrencyUI(int gold, int gum, int delly)
    {
        if (goldValueText != null)
            goldValueText.text = gold.ToString();

        if (gumValueText != null)
            gumValueText.text = gum.ToString();

        if (dellyValueText != null)
            dellyValueText.text = delly.ToString();

        // Gum이 최대치면 타이머 숨김
        if (gumTimeText != null)
        {
            if (gum >= playerCurrency.MaxGum)
            {
                gumTimeText.gameObject.SetActive(false);
            }
            else
            {
                gumTimeText.gameObject.SetActive(true);
            }
        }
    }

    /// <summary>
    /// Gum 회복 타이머를 업데이트합니다. (MM:SS 형식)
    /// </summary>
    private void UpdateGumTimer()
    {
        if (gumTimeText == null || playerCurrency == null) return;

        float remainingSeconds = playerCurrency.GetTimeUntilNextGumRecovery();

        if (remainingSeconds <= 0f)
        {
            gumTimeText.gameObject.SetActive(false);
            return;
        }

        // MM:SS 형식으로 변환
        int minutes = Mathf.FloorToInt(remainingSeconds / 60f);
        int seconds = Mathf.FloorToInt(remainingSeconds % 60f);
        gumTimeText.text = $"{minutes}:{seconds:D2}";
    }

    public void OnShopPressed()
    {
        UIManager.Instance.ShowShopPopup();
    }

}
