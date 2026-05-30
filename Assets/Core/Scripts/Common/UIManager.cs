using System;
using UnityEngine;

namespace DoggyChef
{
    // 전역 UI 팝업(메시지, 상점)을 중앙에서 관리하는 싱글턴입니다.
    public class UIManager : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  Inspector 연결 필드
        // ════════════════════════════════════════════════════════════════

        [Header("팝업 프리팹")]
        [Tooltip("확인/취소 메시지 팝업")]
        [SerializeField] private MessagePopup m_MessagePopupPrefab;
        [Tooltip("상점 팝업")]
        [SerializeField] private ShopPopup m_ShopPopupPrefab;

        [Header("팝업 부모")]
        [Tooltip("팝업이 스폰될 Canvas Transform")]
        [SerializeField] private Transform m_PopupParent;

        // ════════════════════════════════════════════════════════════════
        //  싱글턴
        // ════════════════════════════════════════════════════════════════

        public static UIManager Instance { get; private set; }

        // ════════════════════════════════════════════════════════════════
        //  private 런타임 상태
        // ════════════════════════════════════════════════════════════════

        private MessagePopup m_CurrentMessagePopup;
        private ShopPopup m_CurrentShopPopup;

        // ════════════════════════════════════════════════════════════════
        //  Unity 생명 주기
        // ════════════════════════════════════════════════════════════════

        // 싱글턴 중복 방지, DontDestroyOnLoad 등록, 팝업 부모 기본값 설정.
        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(gameObject);
                return;
            }

            Instance = this;
            DontDestroyOnLoad(gameObject);

            if (m_PopupParent == null)
                m_PopupParent = transform;
        }

        // ════════════════════════════════════════════════════════════════
        //  public API — 메시지 팝업
        // ════════════════════════════════════════════════════════════════

        // 확인 버튼만 있는 알림 팝업을 표시합니다.
        public void ShowMessage(string message, Action onConfirm = null)
        {
            ShowMessagePopup("알림", message, onConfirm, null, false);
        }

        // 제목과 메시지가 있는 확인 전용 팝업을 표시합니다.
        public void ShowMessage(string title, string message, Action onConfirm = null)
        {
            ShowMessagePopup(title, message, onConfirm, null, false);
        }

        // 확인/취소 버튼이 있는 확인 팝업을 표시합니다.
        public void ShowConfirm(string title, string message, Action onConfirm = null, Action onCancel = null)
        {
            ShowMessagePopup(title, message, onConfirm, onCancel, true);
        }

        // 모든 옵션을 지정하여 메시지 팝업을 표시합니다.
        public void ShowMessagePopup(string title, string message, Action onConfirm = null, Action onCancel = null, bool showCancelButton = true)
        {
            m_CurrentMessagePopup?.Close();

            if (m_MessagePopupPrefab != null)
            {
                m_CurrentMessagePopup = Instantiate(m_MessagePopupPrefab, m_PopupParent);
                m_CurrentMessagePopup.Show(title, message, onConfirm, onCancel, showCancelButton);
            }
            else
            {
                Debug.LogError("[UIManager] MessagePopup 프리팹이 설정되지 않았습니다!");
            }
        }

        // 현재 메시지 팝업을 닫고 제거합니다.
        public void CloseMessagePopup()
        {
            if (m_CurrentMessagePopup == null) return;
            m_CurrentMessagePopup.Close();
            Destroy(m_CurrentMessagePopup.gameObject);
            m_CurrentMessagePopup = null;
        }

        // ════════════════════════════════════════════════════════════════
        //  public API — 상점 팝업
        // ════════════════════════════════════════════════════════════════

        // 상점 팝업을 엽니다. 이미 존재하면 재사용합니다.
        public void ShowShopPopup()
        {
            if (m_CurrentShopPopup != null)
            {
                m_CurrentShopPopup.Open();
                return;
            }

            if (m_ShopPopupPrefab != null)
            {
                m_CurrentShopPopup = Instantiate(m_ShopPopupPrefab, m_PopupParent);
                m_CurrentShopPopup.Open();
            }
            else
            {
                Debug.LogError("[UIManager] ShopPopup 프리팹이 설정되지 않았습니다!");
            }
        }

        // 상점 팝업을 비활성화합니다 (파괴하지 않음).
        public void CloseShopPopup()
        {
            m_CurrentShopPopup?.Close();
        }

        // 상점 팝업을 완전히 파괴합니다 (씬 전환 시 등).
        public void DestroyShopPopup()
        {
            if (m_CurrentShopPopup == null) return;
            Destroy(m_CurrentShopPopup.gameObject);
            m_CurrentShopPopup = null;
        }

        // 열려 있는 모든 팝업을 닫습니다.
        public void CloseAllPopups()
        {
            CloseMessagePopup();
            CloseShopPopup();
        }

        // 모든 팝업을 파괴합니다.
        public void DestroyAllPopups()
        {
            if (m_CurrentMessagePopup != null)
            {
                Destroy(m_CurrentMessagePopup.gameObject);
                m_CurrentMessagePopup = null;
            }

            if (m_CurrentShopPopup != null)
            {
                Destroy(m_CurrentShopPopup.gameObject);
                m_CurrentShopPopup = null;
            }
        }
    }
}
