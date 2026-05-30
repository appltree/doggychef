using UnityEngine;

namespace DoggyChef
{
    /// <summary>
    /// 손님 머리 위의 체력 게이지 UI.
    /// Customer.OnHealthChanged 이벤트를 구독하여 슬롯 표시를 갱신합니다.
    ///
    /// ■ 슬롯 구조 (하트 1개 = 2포인트)
    ///   slotValue == 2 → full    (꽉 찬 하트)
    ///   slotValue == 1 → left    (반 찬 하트)
    ///   slotValue == 0 → 모두 숨김 (빈 슬롯)
    /// </summary>
    public class CustomerHealthGage : MonoBehaviour
    {
        // ──────────────────────────────────────────────────────────
        //  슬롯 구조체
        // ──────────────────────────────────────────────────────────

        [System.Serializable]
        private struct Slot
        {
            public Transform root;
            public GameObject left;
            public GameObject right;
            public GameObject full;
        }

        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────

        [SerializeField] private Slot[] slots;

        // ──────────────────────────────────────────────────────────
        //  내부 상태
        // ──────────────────────────────────────────────────────────

        private int m_MaxHealth = 10;
        private int m_Health = 10;
        private Customer m_Customer;

        // ================================================================
        //  Unity 생명 주기
        // ================================================================

        private void Awake()
        {
            m_Customer = GetComponentInParent<Customer>();
            if (m_Customer != null)
                m_Customer.OnHealthChanged += HandleHealthChanged;
        }

        private void OnDestroy()
        {
            if (m_Customer != null)
                m_Customer.OnHealthChanged -= HandleHealthChanged;
        }

        // ================================================================
        //  이벤트 핸들러
        // ================================================================

        private void HandleHealthChanged(int current, int max)
        {
            m_MaxHealth = Mathf.Max(1, max);
            m_Health = Mathf.Clamp(current, 0, m_MaxHealth);
            Refresh();
        }

        // ================================================================
        //  내부 렌더링
        // ================================================================

        /// <summary>현재 체력을 슬롯 배열에 반영합니다.</summary>
        private void Refresh()
        {
            for (int i = 0; i < slots.Length; i++)
            {
                // 이 슬롯이 담당하는 포인트 값 (0~2)
                int slotValue = Mathf.Clamp(m_Health - i * 2, 0, 2);
                ref var slot = ref slots[i];

                slot.left.SetActive(false);
                slot.right.SetActive(false);
                slot.full.SetActive(false);

                if (slotValue == 2) slot.full.SetActive(true);
                else if (slotValue == 1) slot.left.SetActive(true);
            }
        }
    }
}
