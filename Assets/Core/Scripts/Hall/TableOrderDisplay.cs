using System.Collections;
using TMPro;
using UnityEngine;

namespace DoggyChef
{
    // ════════════════════════════════════════════════════════════════
    //  TableOrderDisplay — 주문 말풍선 UI (레시피 이미지 + 판매 가격)
    // ════════════════════════════════════════════════════════════════
    //
    //  ■ 표시 상태
    //    Ordered          → 말풍선 표시 (레시피 이미지 + 가격)
    //    Cooked/Served/Paying → 말풍선 숨김, 접시 위에 레시피 이미지 표시
    //    Empty            → 모두 숨김
    //
    //  ■ Inspector 연결 필요
    //    RecipeRenderer      : 말풍선 안 레시피 이미지 SpriteRenderer
    //    PriceText           : 판매 가격 TextMeshPro
    //    PlateRecipeRenderer : 접시 위에 표시할 SpriteRenderer (테이블 자식)
    // ════════════════════════════════════════════════════════════════
    public class TableOrderDisplay : MonoBehaviour
    {
        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────

        [Header("말풍선 — 레시피 표시")]
        [Tooltip("말풍선 안 레시피 완성 이미지를 표시할 SpriteRenderer.")]
        public SpriteRenderer RecipeRenderer;

        [Header("말풍선 — 가격 표시")]
        [Tooltip("판매 금액을 표시할 TextMeshPro 컴포넌트.")]
        public TextMeshPro PriceText;

        [Tooltip("가격을 감싸는 캡슐 배경 (Transform). 내부적으로 LineRenderer를 사용합니다.")]
        public Transform PriceBadge;

        [Tooltip("골드 아이콘 SpriteRenderer.")]
        public SpriteRenderer GoldIcon;

        [Header("접시 표시 (Cooked 이후)")]
        [Tooltip("접시 이미지 SpriteRenderer. Cooked/Served/Paying 동안 표시되고 Empty 시 숨김.\n" +
                 "Table 자식 PlateDisplay의 SpriteRenderer를 연결하세요.")]
        public SpriteRenderer PlateRenderer;

        [Tooltip("접시 위 음식 이미지 SpriteRenderer. Cooked/Served 동안 레시피 ResultSprite를 표시하고,\n" +
                 "손님이 먹고 나면(Paying) 사라집니다.\n" +
                 "PlateDisplay 자식 PlateFood의 SpriteRenderer를 연결하세요.")]
        public SpriteRenderer PlateRecipeRenderer;

        // ──────────────────────────────────────────────────────────
        //  런타임 상태
        // ──────────────────────────────────────────────────────────

        private Table m_Table;

        // ──────────────────────────────────────────────────────────
        //  피버 UI 관련 런타임 상태 (Origin TableUI.cs 방식)
        // ──────────────────────────────────────────────────────────
        private Coroutine m_FeverBlinkRoutine;
        private Color m_BasePriceColor = Color.white;
        private bool m_BaseColorCaptured;
        private const float FeverBlinkFrequency = 2.0f;
        private const float FeverBlinkMinAlpha = 0.35f;

        // ================================================================
        //  유니티 생명주기
        // ================================================================

        private void Awake()
        {
            m_Table = GetComponentInParent<Table>();
            if (m_Table == null)
                Debug.LogWarning("[TableOrderDisplay] 부모 계층에 Table 컴포넌트가 없습니다.", this);

            if (PriceBadge == null)
                PriceBadge = transform.Find("PriceBadge");
        }

        private void Start()
        {
            if (m_Table == null) return;

            m_Table.OnOrderSet += Refresh;
            m_Table.OnStateChanged += OnStateChanged;

            // 피버 변경 이벤트 구독 (Origin TableUI.HandleFeverChangedForUI 방식)
            if (LevelData.Instance != null)
                LevelData.Instance.OnFeverChanged += HandleFeverChanged;

            Refresh();
        }

        private void OnDestroy()
        {
            if (m_Table != null)
            {
                m_Table.OnOrderSet -= Refresh;
                m_Table.OnStateChanged -= OnStateChanged;
            }
            if (LevelData.Instance != null)
                LevelData.Instance.OnFeverChanged -= HandleFeverChanged;

            StopFeverBlink();
        }

        // ================================================================
        //  이벤트 핸들러
        // ================================================================

        private void OnStateChanged(Table.TableState state) => UpdateVisibility();

        // ================================================================
        //  내부 갱신
        // ================================================================

        private void Refresh()
        {
            var recipe = m_Table?.CurrentOrder;

            if (RecipeRenderer != null)
                RecipeRenderer.sprite = recipe?.ResultSprite;

            // ── 가격 텍스트: 피버 시 x2 표시 (Origin TableUI.UpdateUI 방식) ──
            if (PriceText != null)
            {
                if (recipe != null)
                {
                    bool isFever = LevelData.Instance != null && LevelData.Instance.IsFever;
                    RefreshPriceText(recipe, isFever);
                }
                else
                {
                    StopFeverBlink();
                    PriceText.text = "";
                }
            }

            // 가격 뉴스 레이아웃 업데이트
            UpdatePriceLayout(recipe != null);

            // 접시 위 음식 스프라이트 동기화 (가시성은 UpdateVisibility에서 결정)
            if (PlateRecipeRenderer != null)
                PlateRecipeRenderer.sprite = recipe?.ResultSprite;

            UpdateVisibility();
        }

        /// <summary>
        /// 가격 뱃지의 크기와 아이콘/텍스트 위치를 가격에 맞춰 조정합니다.
        /// </summary>
        private void UpdatePriceLayout(bool hasOrder)
        {
            if (!hasOrder)
            {
                if (PriceBadge != null) PriceBadge.gameObject.SetActive(false);
                if (GoldIcon != null) GoldIcon.gameObject.SetActive(false);
                if (PriceText != null) PriceText.gameObject.SetActive(false);
                return;
            }

            if (PriceBadge != null) PriceBadge.gameObject.SetActive(true);
            if (GoldIcon != null) GoldIcon.gameObject.SetActive(true);
            if (PriceText != null) PriceText.gameObject.SetActive(true);

            if (PriceText == null || PriceBadge == null || GoldIcon == null) return;

            // 1. 텍스트/아이콘 실제 표시 크기 (world space)
            PriceText.ForceMeshUpdate();
            float textWidth = PriceText.preferredWidth;
            float iconWidth = GoldIcon.sprite != null ? GoldIcon.bounds.size.x : 0.26f;
            float iconHeight = GoldIcon.sprite != null ? GoldIcon.bounds.size.y : 0.24f;

            // 2. 배지 전체 크기 (아이콘 + 간격 + 텍스트 + 여백)
            // 캡슐의 양쪽 반원 곡선에 텍스트가 겹치지 않도록 높이의 절반 정도를 좌우 여백으로 잡습니다.
            const float spacing = 0.06f;
            const float padX = 0.18f;
            const float padY = 0.08f;
            float totalW = iconWidth + spacing + textWidth + padX * 2f;
            float totalH = iconHeight + padY * 2f;

            // 3. PriceBadge 캡슐 크기 설정 (Sprite 없이 LineRenderer의 Round 캡 활용)
            if (PriceBadge != null)
            {
                LineRenderer lr = PriceBadge.GetComponent<LineRenderer>();
                if (lr == null)
                {
                    // 불필요한 기존 MeshFilter, MeshRenderer 정리 (Missing Script 문제 회피용)
                    var mf = PriceBadge.GetComponent<MeshFilter>(); if (mf) Destroy(mf);
                    var mr = PriceBadge.GetComponent<MeshRenderer>(); if (mr) Destroy(mr);

                    lr = PriceBadge.gameObject.AddComponent<LineRenderer>();
                    lr.sharedMaterial = new Material(Shader.Find("Sprites/Default"));
                    lr.startColor = new Color(0f, 0f, 0f, 0.65f);
                    lr.endColor = new Color(0f, 0f, 0f, 0.65f);
                    lr.numCapVertices = 16;
                    lr.useWorldSpace = false;
                    lr.sortingOrder = 11;
                }

                // 캡슐의 둥근 모서리는 Line 폭의 절반(totalH) 반경이므로 전체 폭에서 totalH를 뺀 길이만큼 직선을 긋습니다.
                float lineLength = Mathf.Max(0f, totalW - totalH);
                lr.startWidth = totalH;
                lr.endWidth = totalH;
                lr.positionCount = 2;
                lr.SetPosition(0, new Vector3(-lineLength * 0.5f, 0f, 0f));
                lr.SetPosition(1, new Vector3(lineLength * 0.5f, 0f, 0f));
            }

            // 4. PriceBadge X = 콘텐츠 중심(x=0) 고정
            if (PriceBadge != null)
            {
                Vector3 badgePos = PriceBadge.localPosition;
                badgePos.x = 0f;
                PriceBadge.localPosition = badgePos;
            }

            // 5. 아이콘/텍스트 배치 (배지 중심 x=0 기준)
            float startX = -totalW * 0.5f + padX;
            float badgeY = PriceBadge != null ? PriceBadge.localPosition.y : 0f;

            Vector3 iconPos = GoldIcon.transform.localPosition;
            iconPos.x = startX + iconWidth * 0.5f;
            iconPos.y = badgeY;
            GoldIcon.transform.localPosition = iconPos;

            Vector3 textPos = PriceText.transform.localPosition;
            textPos.x = startX + iconWidth + spacing + textWidth * 0.5f;
            textPos.y = badgeY;
            PriceText.transform.localPosition = textPos;
        }

        private void UpdateVisibility()
        {
            if (m_Table == null)
            {
                gameObject.SetActive(false);
                if (PlateRenderer != null) PlateRenderer.gameObject.SetActive(false);
                if (PlateRecipeRenderer != null) PlateRecipeRenderer.gameObject.SetActive(false);
                return;
            }

            var s = m_Table.State;

            // 말풍선: Ordered 상태에서만 표시
            gameObject.SetActive(s == Table.TableState.Ordered);

            // 접시 + 음식: Cooked/Served 동안 표시 → 손님이 먹고 나면(Paying) 둘 다 사라짐
            bool showPlate = s == Table.TableState.Cooked || s == Table.TableState.Served;
            if (PlateRenderer != null)
                PlateRenderer.gameObject.SetActive(showPlate);
            if (PlateRecipeRenderer != null)
                PlateRecipeRenderer.gameObject.SetActive(showPlate);
        }

        // ================================================================
        //  피버 UI 갱신 (Origin TableUI.HandleFeverChangedForUI / TryStartFeverBlink)
        // ================================================================

        private void HandleFeverChanged(bool isFever)
        {
            if (m_Table == null || m_Table.State != Table.TableState.Ordered) return;
            var recipe = m_Table.CurrentOrder;
            if (recipe == null) return;
            RefreshPriceText(recipe, isFever);
        }

        // 가격 텍스트를 피버 상태에 맞게 표시하고 깨뱡임을 시작/중단합니다.
        // Origin TableUI.UpdateUI() 방식
        private void RefreshPriceText(RecipeData recipe, bool isFever)
        {
            if (PriceText == null || recipe == null) return;

            if (isFever)
            {
                // 피버 중: 가격 x2 표시 + 깨뱡임
                int feverPrice = recipe.SellPrice * 2;
                PriceText.text =
                    $"<b>{feverPrice:N0} <size=80%><color=#FFD700>x2</color></size></b>";
                TryStartFeverBlink();
            }
            else
            {
                // 피버 해제: 원래 가격
                PriceText.text = recipe.SellPrice.ToString();
                StopFeverBlink();
            }
        }

        private void TryStartFeverBlink()
        {
            if (m_FeverBlinkRoutine != null) return; // 이미 실행 중
            if (PriceText == null) return;

            if (!m_BaseColorCaptured)
            {
                m_BasePriceColor = PriceText.color;
                m_BaseColorCaptured = true;
            }
            m_FeverBlinkRoutine = StartCoroutine(CoFeverBlink());
        }

        private void StopFeverBlink()
        {
            if (m_FeverBlinkRoutine != null)
            {
                StopCoroutine(m_FeverBlinkRoutine);
                m_FeverBlinkRoutine = null;
            }
            if (PriceText != null && m_BaseColorCaptured)
                PriceText.color = m_BasePriceColor;
        }

        // Origin TableUI.CoFeverBlink() 동일 — 피버 중 가격 텍스트가 리듬과 함께 깨뱡입니다.
        private IEnumerator CoFeverBlink()
        {
            float t = 0f;
            float freq = FeverBlinkFrequency;
            float minA = FeverBlinkMinAlpha;
            Color baseCol = m_BaseColorCaptured
                ? m_BasePriceColor
                : (PriceText != null ? PriceText.color : Color.white);

            while (isActiveAndEnabled && PriceText != null)
            {
                t += Time.deltaTime * freq * Mathf.PI * 2f;
                float s = Mathf.Sin(t) * 0.5f + 0.5f;
                float a = Mathf.Lerp(minA, 1f, s);
                var c = baseCol; c.a = a;
                PriceText.color = c;
                yield return null;
            }
            m_FeverBlinkRoutine = null;
        }
    }
}
