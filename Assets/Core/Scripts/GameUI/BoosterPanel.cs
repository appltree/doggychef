using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

namespace DoggyChef
{
    // Canvas 기반 부스터 패널.
    // 
    // ■ 씬 구조 (Canvas 자식으로 직접 배치)
    //   BoosterPanel  ← 이 스크립트 부착
    //   └─ Slot0      ← Button 컴포넌트 부착
    //        ├─ Icon           ← Image (BoosterSlot.Icon)
    //        ├─ CountLabel     ← TMP_Text (BoosterSlot.CountLabel)
    //        └─ CooldownMask   ← Image, Type=Filled, Radial360, Clockwise, Origin=Top
    //                             (BoosterSlot.CooldownOverlay)
    //   └─ Slot1 / Slot2 ...
    // 
    // ■ 날아가는 연출 (FlyTarget 설정 시)
    //   버튼 클릭 → 아이콘이 포물선을 그리며 회전해 FlyTarget 으로 날아감
    //   → 도착 즉시 UseInstant() 발동 → 쿨타임 시작
    // 
    // ■ 쿨타임 오버레이
    //   CooldownOverlay 이미지의 fillAmount 가 0→1 로 채워지는 동안 버튼 비활성화
    //   채워지면(= 쿨타임 종료) 버튼 재활성화
    public class BoosterPanel : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  중첩 타입
        // ════════════════════════════════════════════════════════════════

        [System.Serializable]
        public class BoosterSlot
        {
            [Header("부스터 장착 (편집·탈부착)")]
            [Tooltip("이 슬롯에 장착할 Booster 프리팹. null 이면 슬롯이 비활성(탈착) 상태가 됩니다.")]
            public Booster BonusGemPrefab;
            [Tooltip("게임 시작 시 초기 보유 수량.")]
            public int InitialAmount = 3;

            [Header("UI 연결")]
            public Button Button;
            public Image Icon;
            public TMP_Text CountLabel;
            public GameObject SelectedGlow;

            [Header("Cooldown")]
            [Tooltip("Radial360 Fill 이미지. Type=Filled / FillMethod=Radial360 / Clockwise / Origin=Top 으로 설정.")]
            public Image CooldownOverlay;
            [Tooltip("쿨타임 초. 0 이면 쿨타임 없음.")]
            public float CooldownDuration = 0f;

            // 런타임 상태 (BoosterPanel만 접근)
            [HideInInspector] public bool IsOnCooldown;
            [HideInInspector] public Coroutine CooldownRoutine;
        }

        // ════════════════════════════════════════════════════════════════
        //  Inspector 연결 필드
        // ════════════════════════════════════════════════════════════════

        [SerializeField] private List<BoosterSlot> m_Slots = new();
        [Tooltip("날아가는 연출의 도착 목표 Transform (예: TimeGauge).")]
        [SerializeField] private RectTransform m_FlyTarget;

        // ════════════════════════════════════════════════════════════════
        //  싱글턴
        // ════════════════════════════════════════════════════════════════

        public static BoosterPanel Instance { get; private set; }

        // ════════════════════════════════════════════════════════════════
        //  private 런타임 상태
        // ════════════════════════════════════════════════════════════════

        private BoosterSlot m_SelectedSlot;
        private Canvas m_Canvas;

        // 보드 셀 깜빡임 오버레이
        private readonly List<GameObject> m_BlinkOverlays = new();
        private Coroutine m_BlinkRoutine;

        // ─────────────────────────────────────────────────────────────
        //  Unity 생명 주기
        // ─────────────────────────────────────────────────────────────

        private void Awake()
        {
            if (Instance != null && Instance != this) { Destroy(gameObject); return; }
            Instance = this;
            m_Canvas = GetComponentInParent<Canvas>();
        }

        private void Start() { InitSlots(); }

        private void OnDestroy()
        {
            if (Instance == this) Instance = null;
        }

        // ─────────────────────────────────────────────────────────────
        //  초기화
        // ─────────────────────────────────────────────────────────────

        // [Start 호출] 부스터 슬롯을 초기화하고 버튼 이벤트를 연결합니다.
        public void InitSlots()
        {
            if (GameManager.Instance == null) return;

            foreach (var slot in m_Slots)
            {
                if (slot?.Button == null) continue;

                slot.Button.onClick.RemoveAllListeners();
                SetSlotSelected(slot, false);

                // 쿨타임 오버레이 초기화
                if (slot.CooldownOverlay != null)
                {
                    slot.CooldownOverlay.fillAmount = 0f;
                    slot.CooldownOverlay.gameObject.SetActive(false);
                }

                // BonusGemPrefab이 설정된 슬롯만 활성화 (탈착된 슬롯은 숨김)
                if (slot.BonusGemPrefab != null)
                {
                    var entry = GameManager.Instance.BonusItems
                        .Find(e => e.Item == slot.BonusGemPrefab);
                    if (entry == null)
                    {
                        string boosterName = slot.BonusGemPrefab.name;
                        int savedAmount = GameManager.Instance.UserData.GetBoosterAmount(boosterName, slot.InitialAmount);

                        entry = new() { Item = slot.BonusGemPrefab, Amount = savedAmount };
                        GameManager.Instance.BonusItems.Add(entry);

                        // UserData에도 즉시 등록하여 상태를 일치시킵니다.
                        GameManager.Instance.UserData.SetBoosterAmount(boosterName, savedAmount);
                    }

                    var capturedEntry = entry;
                    var capturedSlot = slot;

                    if (slot.Icon != null && slot.BonusGemPrefab.UISprite != null)
                    {
                        slot.Icon.sprite = slot.BonusGemPrefab.UISprite;
                        slot.Icon.enabled = true;
                    }

                    slot.Button.onClick.AddListener(() =>
                        OnSlotClicked(capturedEntry, capturedSlot));

                    slot.Button.gameObject.SetActive(true);
                }
                else
                {
                    // BonusGemPrefab이 없으면 슬롯 비활성화 (탈착 상태)
                    slot.Button.gameObject.SetActive(false);
                }
            }

            UpdateAllSlots();
        }

        // ─────────────────────────────────────────────────────────────
        //  버튼 클릭 처리
        // ─────────────────────────────────────────────────────────────

        private void OnSlotClicked(GameManager.BonusItemEntry entry, BoosterSlot slot)
        {
            if (slot.IsOnCooldown) return;
            if (entry == null || entry.Amount <= 0) return;
            var item = entry.Item;
            if (item == null) return;

            // ── 즉시형 부스터 ──
            if (item.IsInstant)
            {
                entry.Amount -= 1;
                GameManager.Instance.UserData.SetBoosterAmount(item.name, entry.Amount);
                GameManager.Instance.SaveAll();

                // 연타 방지: UpdateAllSlots() 전에 플래그를 먼저 세워
                // → UpdateAllSlots 가 interactable = false 로 설정하게 함
                if (slot.CooldownDuration > 0f)
                    slot.IsOnCooldown = true;

                UpdateAllSlots();

                if (item.UseFlyAnimation)
                {
                    // 아이콘이 날아간 뒤 효과 발동 + 쿨타임 시작
                    StartCoroutine(FlyAndActivate(slot, item));
                }
                else
                {
                    StartCoroutine(RunInstantEffect(slot, item));
                }
                return;
            }

            // ── 보드 클릭형 부스터 ──
            if (m_SelectedSlot == slot)
            {
                DeselectAll();
                GameManager.Instance.ActivateBonusItem(null);
            }
            else
            {
                DeselectAll();
                m_SelectedSlot = slot;
                SetSlotSelected(slot, true);
                GameManager.Instance.ActivateBonusItem(item);
                StartCellBlink();
            }
        }

        // ─────────────────────────────────────────────────────────────
        //  즉시형 부스터 효과 실행 (날아가기 없는 경로)
        // ─────────────────────────────────────────────────────────────

        private IEnumerator RunInstantEffect(BoosterSlot slot, Booster item)
        {
            Vector3 startPos = slot.Icon != null
                ? slot.Icon.transform.position
                : slot.Button.transform.position;
            yield return StartCoroutine(item.InstantEffectRoutine(startPos, m_Canvas, this));
            BeginCooldown(slot);
        }

        // ─────────────────────────────────────────────────────────────
        //  날아가는 아이콘 연출
        // ─────────────────────────────────────────────────────────────

        private IEnumerator FlyAndActivate(BoosterSlot slot, Booster item)
        {
            // ── 효과음 즉시 재생 ──
            if (slot.BonusGemPrefab?.UseSound != null && GameManager.Instance != null)
                GameManager.Instance.PlaySFXOneShot(slot.BonusGemPrefab?.UseSound);

            Sprite flySprite = slot.BonusGemPrefab?.AnimSprite ?? slot.BonusGemPrefab?.UISprite ?? slot.Icon?.sprite;

            // 스프라이트나 캔버스가 없으면 연출 없이 즉시 실행
            if (flySprite == null || m_Canvas == null)
            {
                item.UseInstant();
                BeginCooldown(slot);
                yield break;
            }

            // ── 임시 오브젝트 생성 ──
            var flyGO = new GameObject("FlyingBoosterIcon");
            flyGO.transform.SetParent(m_Canvas.transform, false);
            flyGO.transform.SetAsLastSibling();

            var flyRT = flyGO.AddComponent<RectTransform>();
            flyRT.sizeDelta = new(80f, 80f);
            flyRT.position = slot.Icon != null
                ? slot.Icon.transform.position
                : slot.Button.transform.position;

            var flyImg = flyGO.AddComponent<Image>();
            flyImg.sprite = flySprite;
            flyImg.preserveAspect = true;
            flyImg.raycastTarget = false;

            Vector3 startPos = flyRT.position;
            // 화면 중앙 (Screen Space Overlay 기준)
            Vector3 centerPos = new(Screen.width * 0.5f, Screen.height * 0.5f, 0f);
            Vector3 endPos = m_FlyTarget != null ? m_FlyTarget.position : centerPos;

            float elapsed;

            // ────────────────────────────────────────────────────
            // Phase 1: 버튼 → 화면 중앙 (0.3초, 튀어나오며 이동)
            // ────────────────────────────────────────────────────
            const float phase1 = 0.3f;
            elapsed = 0f;
            while (elapsed < phase1)
            {
                elapsed += Time.deltaTime;
                float t = EaseInOut(Mathf.Clamp01(elapsed / phase1));
                flyRT.position = Vector3.Lerp(startPos, centerPos, t);
                float scale = 1f + 0.5f * Mathf.Sin(Mathf.PI * (elapsed / phase1));
                flyGO.transform.localScale = Vector3.one * scale;
                yield return null;
            }
            flyRT.position = centerPos;

            // ────────────────────────────────────────────────────
            // Phase 2: 중앙에서 1바퀴 회전 (0.45초)
            // ────────────────────────────────────────────────────
            const float phase2 = 0.45f;
            elapsed = 0f;
            while (elapsed < phase2)
            {
                elapsed += Time.deltaTime;
                float t = Mathf.Clamp01(elapsed / phase2);
                flyGO.transform.rotation = Quaternion.Euler(0f, 0f, -360f * t);
                // 회전 중 살짝 커졌다 돌아오는 펄스
                float scale = 1.4f + 0.2f * Mathf.Sin(Mathf.PI * t * 2f);
                flyGO.transform.localScale = Vector3.one * scale;
                yield return null;
            }

            // ────────────────────────────────────────────────────
            // Phase 3: 중앙 → TimeGauge 흡수 (0.35초, 줄어들며 빨려 들어감)
            // ────────────────────────────────────────────────────
            const float phase3 = 0.35f;
            elapsed = 0f;
            Vector3 absorbStart = centerPos;
            float startAngle = -360f;
            while (elapsed < phase3)
            {
                elapsed += Time.deltaTime;
                float t = EaseInOut(Mathf.Clamp01(elapsed / phase3));
                flyRT.position = Vector3.Lerp(absorbStart, endPos, t);
                // 줄어들며 사라짐
                float scale = Mathf.Lerp(1.4f, 0f, t);
                flyGO.transform.localScale = Vector3.one * scale;
                // 빨려 들어가는 동안 추가 회전
                flyGO.transform.rotation = Quaternion.Euler(0f, 0f, startAngle - 180f * t);
                yield return null;
            }

            Destroy(flyGO);

            // 흡수 완료 → 효과 발동 & 쿨타임 시작
            item.UseInstant();
            BeginCooldown(slot);
        }

        private static float EaseInOut(float t)
            => t < 0.5f ? 2f * t * t : -1f + (4f - 2f * t) * t;

        // ─────────────────────────────────────────────────────────────
        //  보드 셀 깜빡임 (Board-target 부스터 선택 중)
        // ─────────────────────────────────────────────────────────────

        private void StartCellBlink()
        {
            StopCellBlink();

            var board = GameManager.Instance?.Board;
            if (board == null) return;

            // 1×1 흰색 텍스처로 오버레이 스프라이트 생성
            var tex = new Texture2D(1, 1, TextureFormat.RGBA32, false);
            tex.SetPixel(0, 0, Color.white);
            tex.Apply();
            var cellSize = board.Grid.cellSize;
            var sprite = Sprite.Create(tex, new Rect(0, 0, 1, 1),
                                       new Vector2(0.5f, 0.5f), 1f);

            foreach (var kvp in board.CellContent)
            {
                if (kvp.Value.ContainingGem == null) continue;

                var go = new GameObject("CellBlink");
                var sr = go.AddComponent<SpriteRenderer>();
                sr.sprite = sprite;
                sr.color = new(1f, 0.9f, 0.2f, 0f);
                sr.sortingLayerName = "Default";
                sr.sortingOrder = 20;

                go.transform.position = board.Grid.GetCellCenterWorld(kvp.Key);
                go.transform.localScale = new(cellSize.x * 0.9f, cellSize.y * 0.9f, 1f);
                m_BlinkOverlays.Add(go);
            }

            m_BlinkRoutine = StartCoroutine(BlinkCellsRoutine());
        }

        private IEnumerator BlinkCellsRoutine()
        {
            while (true)
            {
                float alpha = (Mathf.Sin(Time.time * Mathf.PI * 3f) + 1f) * 0.25f;
                foreach (var go in m_BlinkOverlays)
                {
                    if (go == null) continue;
                    var sr = go.GetComponent<SpriteRenderer>();
                    if (sr != null) sr.color = new(1f, 0.9f, 0.2f, alpha);
                }
                yield return null;
            }
        }

        private void StopCellBlink()
        {
            if (m_BlinkRoutine != null)
            {
                StopCoroutine(m_BlinkRoutine);
                m_BlinkRoutine = null;
            }
            foreach (var go in m_BlinkOverlays)
                if (go != null) Destroy(go);
            m_BlinkOverlays.Clear();
        }

        // ─────────────────────────────────────────────────────────────
        //  던지기 연출 (Board-target 부스터)
        // ─────────────────────────────────────────────────────────────

        // 보드 클릭형 부스터에 ThrowSprite가 설정된 경우 던지기 애니메이션을 재생합니다.
        // 코루틴을 시작하고 true를 반환. 연출 없으면 false 반환.
        public bool TryThrowAnim(Booster item, Vector3 worldTarget, System.Action onLanded)
        {
            if (GameManager.Instance == null) return false;
            var slot = m_Slots.Find(s => s.BonusGemPrefab == item);
            if (slot == null || item.IsInstant) return false;

            StopCellBlink();
            StartCoroutine(ThrowCoroutine(slot, worldTarget, onLanded));
            return true;
        }

        private IEnumerator ThrowCoroutine(BoosterSlot slot, Vector3 worldTarget,
                                           System.Action onLanded)
        {
            if (m_Canvas == null) { onLanded?.Invoke(); yield break; }

            // 날아가는 효과음 — 착지 시 정지할 수 있도록 AudioSource 보관
            AudioSource flyAudio = null;
            if (slot.BonusGemPrefab?.UseSound != null && GameManager.Instance != null)
                flyAudio = GameManager.Instance.PlaySFX(slot.BonusGemPrefab?.UseSound);

            Vector3 startPos = slot.Icon != null
                ? slot.Icon.transform.position
                : slot.Button.transform.position;
            Vector3 endPos = Camera.main != null
                ? Camera.main.WorldToScreenPoint(worldTarget)
                : startPos;
            endPos.z = 0f;

            // 임시 오브젝트 생성
            var flyGO = new GameObject("FlyingBomb");
            flyGO.transform.SetParent(m_Canvas.transform, false);
            flyGO.transform.SetAsLastSibling();

            var flyRT = flyGO.AddComponent<RectTransform>();
            flyRT.sizeDelta = new(80f, 80f);
            flyRT.position = startPos;

            var flyImg = flyGO.AddComponent<Image>();
            flyImg.sprite = slot.BonusGemPrefab?.AnimSprite ?? slot.BonusGemPrefab?.UISprite ?? slot.Icon?.sprite;
            flyImg.preserveAspect = true;
            flyImg.raycastTarget = false;

            // 포물선 던지기 (0.5초)
            const float duration = 0.5f;
            float arcHeight = Mathf.Abs(endPos.y - startPos.y) * 0.5f + 120f;
            float elapsed = 0f;

            while (elapsed < duration)
            {
                elapsed += Time.deltaTime;
                float t = Mathf.Clamp01(elapsed / duration);
                float easedT = EaseInOut(t);

                Vector3 pos = Vector3.Lerp(startPos, endPos, easedT);
                pos.y += Mathf.Sin(Mathf.PI * t) * arcHeight;
                flyRT.position = pos;

                // 회전 + 크기 변화
                flyGO.transform.rotation = Quaternion.Euler(0f, 0f, -540f * t);
                float scale = 1f + 0.4f * Mathf.Sin(Mathf.PI * t);
                flyGO.transform.localScale = Vector3.one * scale;

                yield return null;
            }

            Destroy(flyGO);

            // 날아가는 소리 정지 후 폭발 발동
            flyAudio?.Stop();
            DeselectAll();
            UpdateAllSlots();
            onLanded?.Invoke();
            BeginCooldown(slot);
        }

        // ─────────────────────────────────────────────────────────────
        //  쿨타임
        // ─────────────────────────────────────────────────────────────

        private void BeginCooldown(BoosterSlot slot)
        {
            if (slot.CooldownDuration <= 0f) return;
            if (slot.CooldownRoutine != null) StopCoroutine(slot.CooldownRoutine);
            slot.CooldownRoutine = StartCoroutine(CooldownCoroutine(slot));
        }

        private IEnumerator CooldownCoroutine(BoosterSlot slot)
        {
            slot.IsOnCooldown = true;
            slot.Button.interactable = false;

            if (slot.CooldownOverlay != null)
            {
                slot.CooldownOverlay.gameObject.SetActive(true);
                slot.CooldownOverlay.fillAmount = 1f; // 1에서 시작 (전체 가림)
            }

            float elapsed = 0f;
            while (elapsed < slot.CooldownDuration)
            {
                elapsed += Time.deltaTime;
                // 1 -> 0 으로 감소하는 비율
                float t = 1f - Mathf.Clamp01(elapsed / slot.CooldownDuration);

                if (slot.CooldownOverlay != null)
                    slot.CooldownOverlay.fillAmount = t;

                yield return null;
            }

            // 쿨타임 완료
            slot.IsOnCooldown = false;
            slot.CooldownRoutine = null;

            if (slot.CooldownOverlay != null)
            {
                slot.CooldownOverlay.fillAmount = 0f;
                slot.CooldownOverlay.gameObject.SetActive(false);
            }

            UpdateAllSlots();
        }

        // ─────────────────────────────────────────────────────────────
        //  외부 API
        // ─────────────────────────────────────────────────────────────

        public void DeselectAll()
        {
            if (m_SelectedSlot != null) SetSlotSelected(m_SelectedSlot, false);
            m_SelectedSlot = null;
            StopCellBlink();
        }

        // 보드 클릭형 부스터를 애니메이션 없이 즉시 사용한 뒤 쿨타임을 시작합니다.
        // GameManager.UseBonusItem의 non-animated 경로에서 호출합니다.
        public void BeginCooldownForItem(Booster item)
        {
            var slot = m_Slots.Find(s => s.BonusGemPrefab == item);
            if (slot != null) BeginCooldown(slot);
        }

        public void UpdateAllSlots()
        {
            if (GameManager.Instance == null) return;

            foreach (var slot in m_Slots)
            {
                if (slot == null || slot.BonusGemPrefab == null) continue;

                var entry = GameManager.Instance.BonusItems
                    .Find(e => e.Item == slot.BonusGemPrefab);

                if (entry != null)
                {
                    if (slot.CountLabel != null)
                        slot.CountLabel.text = entry.Amount.ToString();

                    if (slot.Button != null)
                        slot.Button.interactable = (entry.Amount > 0 && !slot.IsOnCooldown);
                }
                else
                {
                    if (slot.CountLabel != null) slot.CountLabel.text = "";
                    if (slot.Button != null) slot.Button.interactable = false;
                }
            }
        }

        // ─────────────────────────────────────────────────────────────
        //  선택 상태 시각화
        // ─────────────────────────────────────────────────────────────

        private void SetSlotSelected(BoosterSlot slot, bool selected)
        {
            if (slot == null) return;

            if (slot.SelectedGlow != null)
                slot.SelectedGlow.SetActive(selected);

            if (slot.Icon != null)
            {
                slot.Icon.color = selected
                    ? new Color(1f, 0.9f, 0.3f, 1f)
                    : Color.white;
            }
        }
    }
}
