using TMPro;
using UnityEngine;

namespace DoggyChef
{
    // ════════════════════════════════════════════════════════════════
    //  IngredientSlot — 주문 재료 한 칸을 담당하는 컴포넌트
    // ════════════════════════════════════════════════════════════════
    //
    //  ■ 역할
    //  TableOrderDisplay가 생성·관리하는 슬롯 오브젝트.
    //  재료 아이콘(SpriteRenderer) + 남은 수량(TextMeshPro)을 표시합니다.
    //
    //  ■ 상태 표시 규칙 (Origin Slot.cs 참고)
    //    ┌─────────────────────────────────────────────────────┐
    //    │ 남은 수량 >= 1  │ 텍스트: 노란색, 아이콘: 불투명(100%)  │
    //    │ 남은 수량  = 0  │ 텍스트: 회색,   아이콘: 반투명(40%)   │
    //    └─────────────────────────────────────────────────────┘
    //
    //  ■ 사용 흐름
    //    1. TableOrderDisplay.Rebuild() → Instantiate → Setup() 호출
    //    2. gem 파괴 시 TableOrderDisplay.OnIngredientChanged()
    //                  → UpdateRemaining() 호출 → 시각 갱신
    // ════════════════════════════════════════════════════════════════
    public class IngredientSlot : MonoBehaviour
    {
        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────

        [Header("슬롯 구성")]
        [Tooltip("재료 아이콘을 표시할 SpriteRenderer")]
        public SpriteRenderer IngredientRenderer;

        [Tooltip("남은 수량을 표시할 TextMeshPro. 없어도 동작하지만 수량이 보이지 않습니다.")]
        public TextMeshPro CountText;

        // ──────────────────────────────────────────────────────────
        //  색상 상수
        // ──────────────────────────────────────────────────────────

        // 남은 수량이 1 이상일 때의 텍스트 색상 — 노란색
        private static readonly Color ActiveColor = Color.yellow;

        // 수량이 0이 됐을 때의 텍스트 색상 — 회색 (완료 표시)
        private static readonly Color CompletedColor = new Color(0.5f, 0.5f, 0.5f, 1f);

        // 수량이 0이 됐을 때의 아이콘 색상 — 40% 반투명 흰색 (완료 표시)
        private static readonly Color FadedWhite = new Color(1f, 1f, 1f, 0.4f);

        // ──────────────────────────────────────────────────────────
        //  런타임 상태
        // ──────────────────────────────────────────────────────────

        // 이 슬롯이 담당하는 재료 BlockTag (TableOrderDisplay에서 gem과 매칭할 때 사용)
        public string IngredientTag { get; private set; }

        // 현재 남은 필요 수량
        private int m_Remaining;

        // ================================================================
        //  공개 API
        // ================================================================

        /// <summary>
        /// [TableOrderDisplay 호출]
        /// 슬롯 초기화. 스프라이트·태그·초기 수량을 설정하고 시각을 갱신합니다.
        ///
        /// 파라미터:
        ///   sprite       → GemData.Sprite (재료 아이콘)
        ///   tag          → GemData.BlockTag (재료 식별 태그)
        ///   remainingNeed → 현재 필요한 수량
        /// </summary>
        public void Setup(Sprite sprite, string tag, int remainingNeed)
        {
            IngredientTag = tag;
            if (IngredientRenderer != null) IngredientRenderer.sprite = sprite;
            SetRemaining(remainingNeed);  // 수량 저장 + 시각 초기화
        }

        /// <summary>
        /// [TableOrderDisplay 호출]
        /// 남은 수량을 갱신합니다. gem 파괴 시 OnIngredientChanged에서 호출됩니다.
        /// </summary>
        public void UpdateRemaining(int remainingNeed) => SetRemaining(remainingNeed);

        // ================================================================
        //  내부 시각 갱신
        // ================================================================

        // Setup()과 UpdateRemaining() 모두 이 메서드를 통해 수량을 변경합니다.
        // 수량 저장과 시각 갱신을 한 곳에서 처리해 중복을 제거합니다.
        private void SetRemaining(int value)
        {
            m_Remaining = Mathf.Max(0, value);
            Refresh();
        }

        // 텍스트 색상과 아이콘 알파를 m_Remaining 값에 맞게 갱신합니다.
        private void Refresh()
        {
            bool done = m_Remaining <= 0;

            // 완료 여부에 따라 텍스트 색상 전환: 미완료 → 노란색, 완료 → 회색
            if (CountText != null)
            {
                CountText.text = m_Remaining.ToString();
                CountText.color = done ? CompletedColor : ActiveColor;
            }

            // 완료 여부에 따라 아이콘 투명도 전환: 미완료 → 불투명, 완료 → 40% 반투명
            if (IngredientRenderer != null)
                IngredientRenderer.color = done ? FadedWhite : Color.white;
        }
    }
}
