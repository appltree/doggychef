using System;
using UnityEngine;

/// <summary>
/// 전역 UI (팝업, 다이얼로그 등)를 관리하는 싱글톤 매니저
/// MessagePopup, ShopPopup 등 여러 UI를 중앙에서 관리합니다.
/// </summary>
public class UIManager : MonoBehaviour
{
    private static UIManager instance;
    public static UIManager Instance
    {
        get
        {
            if (instance == null)
            {
                // 씬에서 찾아보기
                instance = FindFirstObjectByType<UIManager>();

                // if (instance == null)
                // {
                //     Debug.LogWarning("UIManager가 씬에 없습니다. Resources에서 로드를 시도합니다.");

                //     // Resources에서 프리팹 로드 시도
                //     var prefab = Resources.Load<GameObject>("Prefabs/UIManager");
                //     if (prefab != null)
                //     {
                //         var go = Instantiate(prefab);
                //         instance = go.GetComponent<UIManager>();
                //         DontDestroyOnLoad(go);
                //     }
                //     else
                //     {
                //         Debug.LogError("UIManager 프리팹을 찾을 수 없습니다.");
                //     }
                // }
            }
            return instance;
        }
    }

    [Header("팝업 프리팹")]
    [SerializeField] private MessagePopup messagePopupPrefab;
    [SerializeField] private ShopPopup shopPopupPrefab;

    [Header("UI 부모")]
    [SerializeField] private Transform popupParent; // Canvas 등

    private MessagePopup currentMessagePopup;
    private ShopPopup currentShopPopup;

    private void Awake()
    {
        if (instance != null && instance != this)
        {
            Destroy(gameObject);
            return;
        }

        instance = this;
        DontDestroyOnLoad(gameObject);

        // 부모가 없으면 자신의 자식으로 생성
        if (popupParent == null)
        {
            popupParent = transform;
        }
    }

    #region MessagePopup 관리

    /// <summary>
    /// 확인 버튼만 있는 간단한 메시지 팝업
    /// </summary>
    public void ShowMessage(string message, Action onConfirm = null)
    {
        ShowMessagePopup("알림", message, onConfirm, null, false);
    }

    /// <summary>
    /// 제목과 메시지가 있는 확인 전용 팝업
    /// </summary>
    public void ShowMessage(string title, string message, Action onConfirm = null)
    {
        ShowMessagePopup(title, message, onConfirm, null, false);
    }

    /// <summary>
    /// 확인/취소 버튼이 있는 확인 팝업
    /// </summary>
    public void ShowConfirm(string title, string message, Action onConfirm = null, Action onCancel = null)
    {
        ShowMessagePopup(title, message, onConfirm, onCancel, true);
    }

    /// <summary>
    /// 전체 옵션을 지정할 수 있는 메시지 팝업
    /// </summary>
    public void ShowMessagePopup(string title, string message, Action onConfirm = null, Action onCancel = null, bool showCancelButton = true)
    {
        // 기존 메시지 팝업이 있으면 닫기
        if (currentMessagePopup != null)
        {
            currentMessagePopup.Close();
        }

        // 새 팝업 생성
        if (messagePopupPrefab != null)
        {
            currentMessagePopup = Instantiate(messagePopupPrefab, popupParent);
            currentMessagePopup.Show(title, message, onConfirm, onCancel, showCancelButton);
        }
        else
        {
            Debug.LogError("MessagePopup 프리팹이 설정되지 않았습니다!");
        }
    }

    /// <summary>
    /// 현재 표시 중인 메시지 팝업 닫기
    /// </summary>
    public void CloseMessagePopup()
    {
        if (currentMessagePopup != null)
        {
            currentMessagePopup.Close();
            Destroy(currentMessagePopup.gameObject);
            currentMessagePopup = null;
        }
    }

    #endregion

    #region ShopPopup 관리

    /// <summary>
    /// 상점 팝업 열기
    /// </summary>
    public void ShowShopPopup()
    {
        // 이미 열려있으면 그대로 사용
        if (currentShopPopup != null)
        {
            currentShopPopup.Open();
            return;
        }

        // 새 상점 팝업 생성
        if (shopPopupPrefab != null)
        {
            currentShopPopup = Instantiate(shopPopupPrefab, popupParent);
            currentShopPopup.Open();
        }
        else
        {
            Debug.LogError("ShopPopup 프리팹이 설정되지 않았습니다!");
        }
    }

    /// <summary>
    /// 상점 팝업 닫기
    /// </summary>
    public void CloseShopPopup()
    {
        if (currentShopPopup != null)
        {
            currentShopPopup.Close();
        }
    }

    /// <summary>
    /// 상점 팝업 완전히 제거 (씬 전환 시 등)
    /// </summary>
    public void DestroyShopPopup()
    {
        if (currentShopPopup != null)
        {
            Destroy(currentShopPopup.gameObject);
            currentShopPopup = null;
        }
    }

    #endregion

    #region 전체 UI 관리

    /// <summary>
    /// 모든 팝업 닫기
    /// </summary>
    public void CloseAllPopups()
    {
        CloseMessagePopup();
        CloseShopPopup();
    }

    /// <summary>
    /// 모든 팝업 제거
    /// </summary>
    public void DestroyAllPopups()
    {
        if (currentMessagePopup != null)
        {
            Destroy(currentMessagePopup.gameObject);
            currentMessagePopup = null;
        }

        if (currentShopPopup != null)
        {
            Destroy(currentShopPopup.gameObject);
            currentShopPopup = null;
        }
    }

    #endregion
}
