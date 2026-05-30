using UnityEngine;
using UnityEngine.UI;

namespace DoggyChef
{
    // 상점 팝업 컨테이너입니다. 닫기 버튼을 UIManager에 위임합니다.
    public class ShopPopup : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  Inspector 연결 필드
        // ════════════════════════════════════════════════════════════════

        [Header("UI 요소")]
        [Tooltip("팝업 닫기 버튼")]
        [SerializeField] private Button m_CloseButton;

        // ════════════════════════════════════════════════════════════════
        //  Unity 생명 주기
        // ════════════════════════════════════════════════════════════════

        // 닫기 버튼에 클릭 리스너를 등록합니다.
        private void Awake()
        {
            m_CloseButton?.onClick.AddListener(OnClosePressed);
        }

        // 닫기 버튼 클릭 리스너를 해제합니다.
        private void OnDestroy()
        {
            m_CloseButton?.onClick.RemoveListener(OnClosePressed);
        }

        // ════════════════════════════════════════════════════════════════
        //  public API
        // ════════════════════════════════════════════════════════════════

        // [UIManager] 팝업을 활성화합니다.
        public void Open()
        {
            gameObject.SetActive(true);
        }

        // [UIManager] 팝업을 비활성화합니다.
        public void Close()
        {
            gameObject.SetActive(false);
        }

        // [CloseButton] UIManager를 통해 팝업을 닫습니다.
        public void OnClosePressed()
        {
            if (UIManager.Instance != null)
                UIManager.Instance.CloseShopPopup();
            else
                Close();
        }
    }
}
