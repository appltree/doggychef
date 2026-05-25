using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

// 기능 분리 리팩토링: 이 파일은 코어 상태/공유 데이터만 보관합니다.
public partial class Table : MonoBehaviour, IPointerDownHandler
{
    public enum TableState
    {
        Empty,   // 테이블비워있음
        Ordered, // 주문
        Cooked,  // 요리됨 (터치 대기)
        Served,  // 제공함
        Paying   // 계산중
    }

    [Header("테이블 상태")]
    [SerializeField] private TableState state = TableState.Empty;

    [Header("Transform 참조")]
    [SerializeField] private Transform leftEntryPoint;
    [SerializeField] private Transform sidePoint;

    [Header("프리팹 및 에셋")]
    [SerializeField] private PaiedMoney paiedMoneyPrefab;

    public event Action OnOrderSet; // 주문이 새로 설정되거나 초기화될 때
    public event Action<string, int> OnIngredientRemainingChanged; // (tag, remainingNeed)

    private readonly float eatDuration = 1.5f;
    private readonly float happyPreviewDuration = 0.5f;

    // 고객 및 주문 관리
    private Customer seatedCustomer;
    private RecipeSO currentOrder;
    private bool isLeaving;
    private bool hasSideMenu = false;
    private GameObject sidePrefabInstance; // 사이드 프리팹 인스턴스 추적

    // 진행도(태그별): 수집 완료와 비행 중 수량
    private readonly Dictionary<string, int> progressByTag = new();
    private readonly Dictionary<string, int> inflightByTag = new();

    // 컴포넌트 참조
    private TableAudio tableAudio;
    private TableUI tableUI;
    private TableAnimation tableAnimation;

    void Awake()
    {
        tableAudio = GetComponent<TableAudio>();
        tableUI = GetComponent<TableUI>();
        tableAnimation = GetComponent<TableAnimation>();
    }

    // Staff 자동 수금을 위한 Paying 상태 브로드캐스트
    public static event Action<Table> OnPayingAdded;   // 테이블이 Paying 상태로 전환
    public static event Action<Table> OnPayingRemoved; // Paying 상태에서 벗어남 / 돈 사라짐

    void OnEnable()
    {
        PaiedMoney.OnCollected += HandleMoneyCollected;
    }

    void OnDisable()
    {
        PaiedMoney.OnCollected -= HandleMoneyCollected;
    }


    public void OnPointerDown(PointerEventData eventData)
    {
        // Machine 활성화 상태에서 Ordered 상태이고 손 애니메이션이 활성화된 테이블 터치 처리
        Machine machine = FindFirstObjectByType<Machine>();
        if (machine != null && state == TableState.Ordered && IsHandAnimationActive() && seatedCustomer && currentOrder != null)
        {
            machine.OnTableTouchedForSide(this);
            return;
        }

        if (state == TableState.Cooked)
        {
            ServeFromCooked();
            return;
        }

        if (state == TableState.Paying)
        {
            CollectMoney();
            return;
        }
    }

    public TableState GetState() => state;
    public bool IsEmpty() => state == TableState.Empty;

    public void SetState(TableState newState)
    {
        if (state == newState) return;
        var prev = state;
        state = newState;

        if (newState == TableState.Cooked)
        {
            if (tableAnimation != null) tableAnimation.StartCookedPulse();
            if (tableAudio != null) tableAudio.PlayCookedSfx();
            // Cooked 상태로 변경되면 손 애니메이션 비활성화
            DisableHandAnimation();
        }
        else if (prev == TableState.Cooked)
        {
            if (tableAnimation != null) tableAnimation.StopCookedPulse();
        }

        // Paying 또는 Empty 상태가 되면 사이드 인스턴스 제거
        if (newState == TableState.Paying || newState == TableState.Empty)
        {
            RemoveSideInstance();
        }

        // Paying 상태 변경 이벤트 (prev, new)
        if (newState == TableState.Paying)
        {
            OnPayingAdded?.Invoke(this);
        }
        else if (prev == TableState.Paying && newState != TableState.Paying)
        {
            OnPayingRemoved?.Invoke(this);
        }

        if (tableUI != null) tableUI.UpdateUI();
    }

    public void ForceEmpty()
    {
        SetEmptyAndReset();
    }

    public void AssignCustomer(Customer customer)
    {
        if (seatedCustomer != null)
        {
            seatedCustomer.OnWalkOutCompleted -= HandleCustomerWalkOutCompleted;
        }
        seatedCustomer = customer;
        if (seatedCustomer != null)
        {
            seatedCustomer.OnWalkOutCompleted += HandleCustomerWalkOutCompleted;
        }
    }

    public int GetCustomerHealth()
    {
        return seatedCustomer != null ? seatedCustomer.GetHealth() : int.MaxValue;
    }

    public void SetOrder(RecipeSO recipe)
    {
        currentOrder = recipe;
        ResetProgress();
        SetState(TableState.Ordered);
        OnOrderSet?.Invoke();
    }

    public RecipeSO GetCurrentOrder() => currentOrder;

    public int GetRemainingNeed(string tag)
    {
        if (state != TableState.Ordered || currentOrder == null || string.IsNullOrEmpty(tag)) return 0;
        int required = currentOrder.GetCountByTag(tag);
        if (required <= 0) return 0;
        progressByTag.TryGetValue(tag, out int cur);
        inflightByTag.TryGetValue(tag, out int inflight);
        int remaining = required - (cur + inflight);
        return remaining > 0 ? remaining : 0;
    }

    public bool AcceptExplodedBlock(Block block)
    {
        if (block == null) return false;
        if (state != TableState.Ordered || currentOrder == null) return false;
        string tag = block.BlockTag;
        if (string.IsNullOrEmpty(tag)) return false;
        int required = currentOrder.GetCountByTag(tag);
        if (required <= 0) return false;

        progressByTag.TryGetValue(tag, out int cur);
        inflightByTag.TryGetValue(tag, out int inflight);
        if (cur + inflight >= required) return false;

        inflightByTag[tag] = inflight + 1;
        Sprite sprite = null;
        var sr = block.GetComponentInChildren<SpriteRenderer>();
        if (sr != null) sprite = sr.sprite;

        Vector3 fromWorld = block.transform.position;
        Vector3 toWorld = GetCollectWorldPosition();
        tableAudio.TryPlaySfx(block.GetFoodSfx(), fromWorld);

        if (tableAnimation != null)
        {
            tableAnimation.SpawnFlyToken(sprite, fromWorld, toWorld, () =>
            {
                inflightByTag.TryGetValue(tag, out int infl);
                inflightByTag[tag] = Mathf.Max(0, infl - 1);
                progressByTag.TryGetValue(tag, out int have);
                if (have < required)
                {
                    progressByTag[tag] = Mathf.Min(required, have + 1);
                    if (tableUI != null) tableUI.UpdateUI();
                    OnIngredientRemainingChanged?.Invoke(tag, GetRemainingNeed(tag));
                    if (IsOrderSatisfied())
                    {
                        SetState(TableState.Cooked);
                    }
                }
            });
        }

        return true;
    }

    public Vector3 GetLeftEdgeWorldPosition() => leftEntryPoint.position;
    public Transform GetSidePoint() => sidePoint;

    public bool GetHasSideMenu()
    {
        return hasSideMenu;
    }

    // 사이드메뉴가 배달되었을 때 호출 (사이드 인스턴스 전달)
    public void OnSideMenuDelivered(GameObject sideInstance)
    {
        hasSideMenu = true;
        sidePrefabInstance = sideInstance;
        tableAudio.PlaySideMenuDeliveredSfx();
    }

    public void EnableHandAnimation(bool enable = true)
    {
        if (tableAnimation != null)
            tableAnimation.EnableHandAnimation(enable);
    }

    public void DisableHandAnimation()
    {
        EnableHandAnimation(false);
    }

    public bool IsHandAnimationActive()
    {
        return tableAnimation != null && tableAnimation.IsHandAnimationActive();
    }

    public void OnMoneyExpired(PaiedMoney coin)
    {
        if (state == TableState.Paying)
        {
            SetEmptyAndReset();
        }
    }

    private bool IsOrderSatisfied()
    {
        if (currentOrder == null) return false;
        var ings = currentOrder.Ingredients;
        if (ings == null) return false;

        for (int i = 0; i < ings.Count; i++)
        {
            var ing = ings[i];
            if (ing.block == null || ing.count <= 0) continue;
            string tag = ing.block.Tag;
            int need = Mathf.Max(1, ing.count);
            progressByTag.TryGetValue(tag, out int have);
            if (have < need) return false;
        }
        return true;
    }

    private void ResetProgress()
    {
        progressByTag.Clear();
        inflightByTag.Clear();
    }

    // (구버전 SetEmptyAndReset / WithSideMenu 구현 삭제됨 - Staff 이벤트 버전 사용)

    private void ServeFromCooked()
    {
        if (state != TableState.Cooked) return;
        SetState(TableState.Served);
        OnServedCompleted();
    }

    private void OnServedCompleted()
    {
        if (!gameObject.activeInHierarchy) return;
        StartCoroutine(ServeEatAndLeaveRoutine());
    }

    private IEnumerator ServeEatAndLeaveRoutine()
    {
        float eatTime = Mathf.Max(1.0f, eatDuration);
        if (seatedCustomer != null)
        {
            if (seatedCustomer.GetHealth() >= seatedCustomer.HappyBeforeEatThreshold && happyPreviewDuration > 0f)
            {
                seatedCustomer.SetState(Customer.CustomerState.Happy);
                float elapsed = 0f;
                while (elapsed < happyPreviewDuration)
                {
                    elapsed += Time.deltaTime;
                    yield return null;
                }
            }
            seatedCustomer.SetState(Customer.CustomerState.Eating);
        }

        float waited = 0f;
        while (waited < eatTime)
        {
            waited += Time.deltaTime;
            yield return null;
        }

        if (tableUI != null)
        {
            tableUI.HideDishRecipeOverlay();
        }

        SpawnMoneyAtTable();

        if (seatedCustomer != null && !isLeaving)
        {
            isLeaving = true;
            seatedCustomer.SetState(Customer.CustomerState.Walkout);
        }
    }

    private void HandleCustomerWalkOutCompleted(Customer customer)
    {
        if (seatedCustomer == customer)
        {
            seatedCustomer.OnWalkOutCompleted -= HandleCustomerWalkOutCompleted;
            seatedCustomer = null;
        }
    }

    private void SpawnMoneyAtTable()
    {
        Vector3 pos = GetCollectWorldPosition();
        PaiedMoney money = Instantiate(paiedMoneyPrefab, pos, Quaternion.identity);
        int amount = currentOrder != null ? currentOrder.SellPrice : 0;

        // Fever mode 보너스: x2
        var gm = FindFirstObjectByType<GameManager>();
        if (gm != null && gm.IsFever)
        {
            amount *= 2;
        }

        money.SetupPaiedMoney(this, amount);
        SetState(TableState.Paying);
    }

    private void CollectMoney()
    {
        PaiedMoney[] moneyObjects = FindObjectsByType<PaiedMoney>(FindObjectsSortMode.None);
        foreach (var money in moneyObjects)
        {
            if (money.Owner == this)
            {
                money.Collect();
                break;
            }
        }
    }

    private void HandleMoneyCollected(int amount, Table table, Vector3 worldPos)
    {
        if (table != this) return;

        if (hasSideMenu)
        {
            HandleMoneyCollectedWithSideBonus(amount, table, worldPos);
        }
        else
        {
            if (tableUI != null) tableUI.ShowGoldPopup(amount, worldPos);
            SetEmptyAndReset();
        }
    }

    private void HandleMoneyCollectedWithSideBonus(int amount, Table table, Vector3 worldPos)
    {
        if (table != this) return;

        int finalAmount = amount;
        string displayText;

        if (hasSideMenu)
        {
            int sideBonus = 100;
            finalAmount += sideBonus;

            var gm = FindFirstObjectByType<GameManager>();
            bool fever = gm != null && gm.IsFever;

            if (fever)
            {
                displayText = $"<b>+{finalAmount:N0} <size=80%><color=#FFD700>x2</color></size> <size=70%><color=#00FF00>(+{sideBonus})</color></size></b>";
            }
            else
            {
                displayText = $"+{finalAmount:N0} <size=80%><color=#00FF00>(+{sideBonus})</color></size>";
            }
        }
        else
        {
            var gm = FindFirstObjectByType<GameManager>();
            bool fever = gm != null && gm.IsFever;
            displayText = fever ? $"<b>+{finalAmount:N0} <size=80%><color=#FFD700>x2</color></size></b>" : $"+{finalAmount:N0}";
        }

        var gameManager = FindFirstObjectByType<GameManager>();
        if (gameManager != null && hasSideMenu)
        {
            gameManager.EarnedMoney += 100;
        }

        if (tableUI != null) tableUI.ShowCustomGoldPopup(displayText, worldPos);
        SetEmptyAndResetWithSideMenu();
    }

    // Staff가 아닌 플레이어 직접 수금 등으로 Paying 종료 시 브로드캐스트 보강
    private void SetEmptyAndReset()
    {
        currentOrder = null;
        progressByTag.Clear();
        inflightByTag.Clear();
        isLeaving = false;
        hasSideMenu = false;
        RemoveSideInstance(); // 사이드 인스턴스 제거
        bool wasPaying = state == TableState.Paying;
        if (state != TableState.Empty)
            state = TableState.Empty; // 무한 재귀 방지 위해 직접 할당 후 UI 갱신
        if (wasPaying)
            OnPayingRemoved?.Invoke(this);
        if (tableUI != null) tableUI.UpdateUI();
        OnOrderSet?.Invoke();
    }

    private void SetEmptyAndResetWithSideMenu()
    {
        currentOrder = null;
        progressByTag.Clear();
        inflightByTag.Clear();
        isLeaving = false;
        hasSideMenu = false;
        RemoveSideInstance(); // 사이드 인스턴스 제거
        bool wasPaying = state == TableState.Paying;
        if (state != TableState.Empty)
            state = TableState.Empty;
        if (wasPaying)
            OnPayingRemoved?.Invoke(this);
        if (tableUI != null) tableUI.UpdateUI();
        OnOrderSet?.Invoke();
    }

    private void RemoveSideInstance()
    {
        if (sidePrefabInstance != null)
        {
            Destroy(sidePrefabInstance);
            sidePrefabInstance = null;
        }
    }


    private Vector3 GetCollectWorldPosition()
    {
        return transform.position + Vector3.up * 0.3f;
    }
}