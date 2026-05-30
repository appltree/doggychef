using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

namespace DoggyChef
{
    // 게임 시작 전 로비 화면.
    // 
    // ■ 동작 흐름
    //   씬 로드 → GameState.Intro → LobbyPanel 표시
    //   [start 버튼] → GameManager.StartLevel() → GameState.Running → LobbyPanel 숨김
    // 
    // ■ 씬 구조 예시 (Canvas 자식으로 배치)
    //   LobbyPanel  ← 이 스크립트 부착 (m_Root = 자기 자신 or 자식 패널)
    //   ├─ TopBar
    //   │   ├─ GoalGoldText     "45,000G"
    //   │   ├─ TimeLimitText    "10:00"
    //   │   └─ ObjectiveText    "얼음 블록 30개 깨기"
    //   ├─ LivesText            뼈 개수
    //   ├─ CenterGroup
    //   │   ├─ OpenSign         (Image)
    //   │   ├─ StartButton      → OnStartClicked()
    //   │   └─ BoosterSection
    //   │       ├─ TapToEditLabel
    //   │       ├─ BoosterSlot0  (Icon + CountLabel)
    //   │       ├─ BoosterSlot1
    //   │       └─ BoosterSlot2
    //   ├─ InviteButton
    //   ├─ UpgradeButton
    //   ├─ EmployButton
    //   └─ ChangeButton
    public class LobbyPanel : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  중첩 타입
        // ════════════════════════════════════════════════════════════════

        [Serializable]
        public class LobbyBoosterSlot
        {
            [Tooltip("부스터 아이콘 Image")]
            public Image Icon;
            [Tooltip("수량 레이블 (우하단 숫자)")]
            public TMP_Text CountLabel;
            [Tooltip("빈 슬롯 표시 ('+' 아이콘 등). null이면 무시.")]
            public GameObject EmptyObject;
        }

        // ════════════════════════════════════════════════════════════════
        //  Inspector 연결 필드
        // ════════════════════════════════════════════════════════════════

        [Header("패널 루트 (Intro 상태에서만 표시)")]
        [SerializeField] private GameObject m_Root;

        [Header("레벨 정보")]
        [Tooltip("목표 골드 텍스트 (예: 45,000G)")]
        [SerializeField] private TMP_Text m_GoalGoldText;
        [Tooltip("제한 시간 텍스트 (예: 10:00)")]
        [SerializeField] private TMP_Text m_TimeLimitText;
        [Tooltip("퀘스트/목표 텍스트 (예: 얼음 블록 30개 깨기)")]
        [SerializeField] private TMP_Text m_ObjectiveText;
        [Tooltip("라이프(뼈) 수 텍스트")]
        [SerializeField] private TMP_Text m_LivesText;

        [Header("버튼")]
        [Tooltip("게임 시작 버튼")]
        [SerializeField] private Button m_StartButton;
        [Tooltip("손님 초대 버튼 (미구현 placeholder)")]
        [SerializeField] private Button m_InviteButton;
        [Tooltip("업그레이드 버튼 (미구현 placeholder)")]
        [SerializeField] private Button m_UpgradeButton;
        [Tooltip("직원 고용 버튼 (미구현 placeholder)")]
        [SerializeField] private Button m_EmployButton;
        [Tooltip("캐릭터 변경 버튼 (미구현 placeholder)")]
        [SerializeField] private Button m_ChangeButton;

        [Header("부스터 슬롯 (Tap to edit — 최대 3개)")]
        [SerializeField] private List<LobbyBoosterSlot> m_BoosterSlots = new();

        // ════════════════════════════════════════════════════════════════
        //  싱글턴
        // ════════════════════════════════════════════════════════════════

        public static LobbyPanel Instance { get; private set; }

        // ════════════════════════════════════════════════════════════════
        //  Unity 생명 주기
        // ════════════════════════════════════════════════════════════════

        private void Awake()
        {
            if (Instance != null && Instance != this) { Destroy(gameObject); return; }
            Instance = this;
        }

        private void Start()
        {
            m_StartButton?.onClick.AddListener(OnStartClicked);
            m_InviteButton?.onClick.AddListener(() => Debug.Log("[LobbyPanel] Invite (미구현)"));
            m_UpgradeButton?.onClick.AddListener(() => Debug.Log("[LobbyPanel] Upgrade (미구현)"));
            m_EmployButton?.onClick.AddListener(() => Debug.Log("[LobbyPanel] Employ (미구현)"));
            m_ChangeButton?.onClick.AddListener(() => Debug.Log("[LobbyPanel] Change (미구현)"));

            GameManager.OnStateChanged += OnGameStateChanged;

            // BoosterPanel.InitSlots()가 완료된 후(1프레임 뒤)에 부스터 정보를 읽습니다.
            StartCoroutine(ShowAfterFrame());
        }

        private void OnDestroy()
        {
            GameManager.OnStateChanged -= OnGameStateChanged;
            if (Instance == this) Instance = null;
        }

        // ─────────────────────────────────────────────────────────────
        //  GameState 변경 처리
        // ─────────────────────────────────────────────────────────────

        private void OnGameStateChanged(GameManager.GameState state)
        {
            if (state == GameManager.GameState.Intro)
                Show();
            else
                Hide();
        }

        // ─────────────────────────────────────────────────────────────
        //  표시 / 숨김
        // ─────────────────────────────────────────────────────────────

        private IEnumerator ShowAfterFrame()
        {
            yield return null; // BoosterPanel.InitSlots() 완료 대기
            if (GameManager.Instance?.CurrentState == GameManager.GameState.Intro)
                Show();
        }

        private void Show()
        {
            if (m_Root != null) m_Root.SetActive(true);
            RefreshLevelInfo();
            RefreshBoosterSlots();
        }

        private void Hide()
        {
            if (m_Root != null) m_Root.SetActive(false);
        }

        // ─────────────────────────────────────────────────────────────
        //  데이터 갱신
        // ─────────────────────────────────────────────────────────────

        private void RefreshLevelInfo()
        {
            var ld = LevelData.Instance;
            if (ld == null) return;

            // 목표 골드
            if (m_GoalGoldText != null)
            {
                int goal = ld.CurrentStage != null ? ld.CurrentStage.GoalGold : 0;
                m_GoalGoldText.text = $"{goal:N0}G";
            }

            // 제한 시간
            if (m_TimeLimitText != null)
            {
                int total = Mathf.FloorToInt(ld.TimeLimit);
                m_TimeLimitText.text = $"{total / 60}:{total % 60:00}";
            }

            // 퀘스트 텍스트 (LevelData에 설정된 ObjectiveText 사용)
            if (m_ObjectiveText != null)
                m_ObjectiveText.text = ld.ObjectiveText;

            // 라이프(뼈) 수
            if (m_LivesText != null && GameManager.Instance != null)
                m_LivesText.text = GameManager.Instance.Lives.ToString();
        }

        private void RefreshBoosterSlots()
        {
            if (GameManager.Instance == null) return;
            var items = GameManager.Instance.BonusItems;

            for (int i = 0; i < m_BoosterSlots.Count; i++)
            {
                var slot = m_BoosterSlots[i];
                if (slot == null) continue;

                bool hasItem = i < items.Count && items[i].Item != null;

                slot.EmptyObject?.SetActive(!hasItem);

                if (hasItem)
                {
                    var entry = items[i];
                    if (slot.Icon != null)
                    {
                        slot.Icon.sprite = entry.Item.UISprite;
                        slot.Icon.color = entry.Amount > 0 ? Color.white : new Color(1f, 1f, 1f, 0.4f);
                        slot.Icon.enabled = true;
                    }
                    if (slot.CountLabel != null)
                        slot.CountLabel.text = entry.Amount.ToString();
                }
                else
                {
                    if (slot.Icon != null) slot.Icon.enabled = false;
                    if (slot.CountLabel != null) slot.CountLabel.text = "";
                }
            }
        }

        // ─────────────────────────────────────────────────────────────
        //  버튼 핸들러
        // ─────────────────────────────────────────────────────────────

        private void OnStartClicked()
        {
            GameManager.Instance?.StartLevel();
        }
    }
}
