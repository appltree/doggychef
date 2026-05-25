using UnityEngine;
using UnityEngine.UI;

public class ShopPopup : MonoBehaviour
{
    [Header("UI 요소")]
    [SerializeField] private Button closeButton;

    private void Awake()
    {
        if (closeButton != null)
        {
            closeButton.onClick.AddListener(OnClosePressed);
        }
    }

    private void OnDestroy()
    {
        if (closeButton != null)
        {
            closeButton.onClick.RemoveListener(OnClosePressed);
        }
    }

    public void Close()
    {
        gameObject.SetActive(false);
    }

    public void Open()
    {
        gameObject.SetActive(true);
    }

    public void OnClosePressed()
    {
        // UIManager를 통해 닫기 (UIManager가 있으면)
        if (UIManager.Instance != null)
        {
            UIManager.Instance.CloseShopPopup();
        }
        else
        {
            Close();
        }
    }
}