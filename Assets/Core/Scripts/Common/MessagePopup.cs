using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace DoggyChef
{
    // 확인/취소 버튼이 있는 범용 메시지 팝업입니다.
    public class MessagePopup : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  Inspector 연결 필드
        // ════════════════════════════════════════════════════════════════

        [Header("버튼")]
        [Tooltip("확인 버튼")]
        [SerializeField] private Button m_ConfirmButton;
        [Tooltip("취소 버튼 (필요 없을 때 숨김)")]
        [SerializeField] private Button m_CancelButton;

        [Header("텍스트")]
        [Tooltip("팝업 제목 텍스트")]
        [SerializeField] private TextMeshProUGUI m_TitleText;
        [Tooltip("팝업 본문 텍스트")]
        [SerializeField] private TextMeshProUGUI m_MessageText;

        // ════════════════════════════════════════════════════════════════
        //  private 런타임 상태
        // ════════════════════════════════════════════════════════════════

        private Action m_OnConfirm;
        private Action m_OnCancel;

        // ════════════════════════════════════════════════════════════════
        //  Unity 생명 주기
        // ════════════════════════════════════════════════════════════════

        // 확인/취소 버튼에 클릭 리스너를 등록합니다.
        private void Awake()
        {
            m_ConfirmButton?.onClick.AddListener(OnConfirmPressed);
            m_CancelButton?.onClick.AddListener(OnCancelPressed);
        }

        // 버튼 클릭 리스너를 해제합니다.
        private void OnDestroy()
        {
            m_ConfirmButton?.onClick.RemoveListener(OnConfirmPressed);
            m_CancelButton?.onClick.RemoveListener(OnCancelPressed);
        }

        // ════════════════════════════════════════════════════════════════
        //  public API
        // ════════════════════════════════════════════════════════════════

        // [UIManager] 팝업 내용을 설정하고 활성화합니다.
        public void Show(string title, string message, Action onConfirm = null, Action onCancel = null, bool showCancelButton = true)
        {
            if (m_TitleText != null) m_TitleText.text = title;
            if (m_MessageText != null) m_MessageText.text = message;

            m_OnConfirm = onConfirm;
            m_OnCancel = onCancel;

            m_CancelButton?.gameObject.SetActive(showCancelButton);
            gameObject.SetActive(true);
        }

        // 팝업을 비활성화하고 콜백 참조를 정리합니다.
        public void Close()
        {
            gameObject.SetActive(false);
            m_OnConfirm = null;
            m_OnCancel = null;
        }

        // 팝업을 다시 활성화합니다.
        public void Open()
        {
            gameObject.SetActive(true);
        }

        // [X 버튼] 콜백 없이 팝업을 닫습니다.
        public void OnClosePressed()
        {
            Close();
        }

        // ════════════════════════════════════════════════════════════════
        //  private 구현
        // ════════════════════════════════════════════════════════════════

        // 확인 콜백을 실행하고 팝업을 닫습니다.
        private void OnConfirmPressed()
        {
            m_OnConfirm?.Invoke();
            Close();
        }

        // 취소 콜백을 실행하고 팝업을 닫습니다.
        private void OnCancelPressed()
        {
            m_OnCancel?.Invoke();
            Close();
        }
    }
}
