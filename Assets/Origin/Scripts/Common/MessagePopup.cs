using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class MessagePopup : MonoBehaviour
{
    [SerializeField] private Button cancelButton;
    [SerializeField] private Button confirmButton;

    [SerializeField] private TextMeshProUGUI titleText;
    [SerializeField] private TextMeshProUGUI messageText;

    private Action onConfirmCallback;
    private Action onCancelCallback;

    private void Awake()
    {
        if (cancelButton != null)
        {
            cancelButton.onClick.AddListener(OnCancelPressed);
        }

        if (confirmButton != null)
        {
            confirmButton.onClick.AddListener(OnConfirmPressed);
        }
    }

    private void OnDestroy()
    {
        if (cancelButton != null)
        {
            cancelButton.onClick.RemoveListener(OnCancelPressed);
        }

        if (confirmButton != null)
        {
            confirmButton.onClick.RemoveListener(OnConfirmPressed);
        }
    }

    /// <summary>
    /// 메시지 팝업을 설정하고 표시합니다.
    /// </summary>
    public void Show(string title, string message, Action onConfirm = null, Action onCancel = null, bool showCancelButton = true)
    {
        if (titleText != null)
        {
            titleText.text = title;
        }

        if (messageText != null)
        {
            messageText.text = message;
        }

        onConfirmCallback = onConfirm;
        onCancelCallback = onCancel;

        if (cancelButton != null)
        {
            cancelButton.gameObject.SetActive(showCancelButton);
        }

        gameObject.SetActive(true);
    }

    public void Close()
    {
        gameObject.SetActive(false);
        onConfirmCallback = null;
        onCancelCallback = null;
    }

    public void Open()
    {
        gameObject.SetActive(true);
    }

    private void OnConfirmPressed()
    {
        onConfirmCallback?.Invoke();
        Close();
    }

    private void OnCancelPressed()
    {
        onCancelCallback?.Invoke();
        Close();
    }

    public void OnClosePressed()
    {
        Close();
    }
}
