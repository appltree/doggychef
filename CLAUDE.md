# DoggyChef — Claude Code 개발 지침서

## 프로젝트 개요

**장르**: Match-3 퍼즐 + 타이쿤 레스토랑 하이브리드  
**엔진**: Unity 2D, C#  
**네임스페이스**: `DoggyChef`  
**렌더 파이프라인**: URP (Universal Render Pipeline)  
**애니메이션**: Spine (SkeletonAnimation)

---

## 아키텍처 전체 구조

```
GameManager (싱글턴, DontDestroyOnLoad)
    │
    ├── Board (Match-3 핵심 로직)
    │       └── Gem, BonusGem, Match, BoardCell
    │
    ├── LevelData (씬별 게임 데이터)
    │       └── StageData (ScriptableObject)
    │
    └── HallManager (타이쿤 홀 관리)
            ├── Table[0,1,2] (테이블 상태 머신)
            │       └── Customer (손님 상태 머신)
            └── RecipeData[] (ScriptableObject)
```

### 핵심 통신 방식 (Board ↔ Hall 느슨한 연결)

```
Board  --OnGemExploded(Gem, Vector3)-->  HallManager.OnGemExploded()
                                              └─> Table.AcceptGem()
Board  --OnGemExplodedDelayed(tag, pos, sprite, sfx)-->  HallManager
```

Board와 Hall은 **정적 이벤트**로만 통신합니다. 직접 참조(직접 호출) 금지.

---

## 게임 상태 머신

```
GameState: Intro → Running → Closing → Result

- Intro   : 씬 로드 초기화 (LevelData.Awake → GameManager.StartLevel())
- Running : 타이머 작동, 손님 스폰, 보드 입력 활성화
- Closing : 타이머 종료. 새 손님 스폰 중단. 현재 손님 처리 대기
- Result  : 모든 테이블 Empty. 결과 화면 표시
```

상태 변경은 반드시 `GameManager.SetState()`를 통해서만 (내부 private).  
외부 시스템은 이벤트로 상태 전환을 요청: `LevelData.OnTimerEnded`, `HallManager.OnAllTablesCleared`.

---

## 핵심 파일 위치

```
Assets/Core/Scripts/
├── GameManager.cs          — 싱글턴. 오디오·저장·상태·부스터 총괄
├── Board.cs                — Match-3 메인 루프 (Update 6단계)
├── LevelData.cs            — 씬별 타이머·목표·하트·골드
├── UIHandler.cs            — UI 통합 핸들러
│
├── Hall/
│   ├── HallManager.cs      — 손님 스폰·재료 라우팅·수금 처리
│   ├── Table.cs            — 테이블 상태 머신 (Empty→Ordered→Cooked→Served→Paying)
│   ├── Customer.cs         — 손님 상태 머신 (None→Walkin→Ordering→Eating→Walkout)
│   ├── Machine.cs          — 사이드 메뉴 머신
│   └── StaffMember.cs      — 직원 (Cashier, Waiter 등)
│
├── BonusGem/
│   ├── BonusGem.cs         — 보너스 젬 베이스 클래스
│   ├── SmallBomb.cs        — 2×2 폭발
│   ├── LargeBomb.cs        — 3×3 폭발
│   ├── LineRocket.cs       — 라인 제거
│   └── ColorClean.cs       — 색상 전체 제거
│
├── Booster/                — 플레이어 부스터 아이템
│
├── Data/
│   ├── GemData.cs          — 보석 데이터 SO (BlockTag, FoodSfx, GemType)
│   ├── RecipeData.cs       — 레시피 SO (재료 목록, SellPrice)
│   ├── StageData.cs        — 스테이지 SO (TimeLimit, FeverHeartCount, AvailableGems)
│   └── ShopData.cs
│
└── UI/                     — UI 컴포넌트들
```

---

## 코딩 스타일 (필수 준수)

### 네이밍 규칙

```csharp
// private 인스턴스 필드 → m_ 접두사
private float m_ElapsedTime;
private Coroutine m_HealthDrainCoroutine;

// private static 필드 → s_ 접두사
private static bool s_IsShuttingDown = false;
private static Board s_Instance;

// public 프로퍼티 → PascalCase (get은 공개, set은 private)
public CustomerState State { get; private set; }
public int CurrentHealth { get; private set; }

// 상수 → PascalCase 또는 UPPER_CASE (둘 다 사용됨)
private const int MAX_COIN_PRICE = 3000;
private const float FlyDuration = 0.6f;   // 코루틴 지역 상수는 PascalCase도 OK

// 이벤트 → On + 동사 (Action<T> 사용)
public Action<Customer> OnSatisfied;
public static event Action<Gem, Vector3> OnGemExploded;
public event Action<TableState> OnStateChanged;
```

### Inspector 필드 패턴

```csharp
[Header("한국어 섹션 제목")]
[Tooltip("한국어 설명")]
public float WalkSpeed = 1.5f;

[SerializeField] private Grid m_Grid;  // 직렬화가 필요한 private는 SerializeField
```

### 클래스 구조 순서 (이 순서로 작성)

**핵심 원칙: 같은 카테고리 안에서 public → private 순서로 배치합니다.**

```
1. 중첩 타입 (enum, nested class — public이 private보다 앞)
2. 상수 (const — private이더라도 최상단. 다른 필드에서 참조하므로)
3. Inspector 연결 필드 (public 또는 [SerializeField] private)
4. 이벤트 (public Action, public static event)
5. 싱글턴 (public static Instance — Inspector 필드 뒤에 위치)
6. public 런타임 상태 (public 프로퍼티, public 필드)
7. private 런타임 상태
       ├─ readonly 필드 (상수에 준하는 설정값, 먼저)
       └─ 일반 private 필드 (뒤에)
8. Unity 생명 주기 (Awake → Start → OnEnable/OnDisable → OnDestroy → Update)
9. public API (public 메서드)
10. private 구현 (private 메서드, 코루틴)
```

**같은 섹션 내 세부 순서 예시 (private 필드, 섹션 7):**

```csharp
// ── readonly 설정값 (먼저) ──
private readonly float m_FlyDuration   = 0.5f;
private readonly int   m_MaxCount      = 5;

// ── 코루틴 참조 ──
private Coroutine m_SomeRoutine;

// ── 일반 인스턴스 상태 ──
private LevelData m_LevelData;
private bool m_IsInitialized;
```

### 섹션 구분자 스타일

```csharp
// ════════════════════════════════════════════════════════════════
//  대분류 섹션 (이중선)
// ════════════════════════════════════════════════════════════════

// ──────────────────────────────────────────────────────────
//  중분류 섹션 (단선)
// ──────────────────────────────────────────────────────────
```

### 빈 줄 규칙

- 메서드와 메서드 사이: **정확히 1줄** 빈 줄
- 연속 빈 줄 (2줄 이상) 금지
- 클래스 여는 `{` 바로 다음 빈 줄 금지

```csharp
// ✅ 올바른 예
private void Foo()
{
    ...
}

private void Bar()
{
    ...
}

// ❌ 잘못된 예 (빈 줄 2줄)
private void Foo()
{
    ...
}


private void Bar()
{
    ...
}
```

### 주석 스타일

`///` XML 문서 주석을 사용하지 않습니다. 모든 주석은 `//` 일반 주석으로 작성합니다.

**모든 메서드에는 최소 한 줄의 `//` 주석이 있어야 합니다.**

```csharp
// [HallManager 호출] 레시피를 배정하고 Ordered 상태로 전환합니다.
public void SetOrder(RecipeData recipe) { ... }

// Paying 상태 진입 시 CoinMoney 프리팹을 스폰합니다.
private void SpawnCoin() { ... }

// 모든 재료가 충족됐는지 확인합니다.
private bool IsOrderSatisfied() { ... }
```

주석 형식 기준:
- public API: `// [호출자] 역할 설명.` (호출자를 대괄호로 명시)
- private 메서드: `// 동작 한 줄 요약.`
- 반환값이 중요한 경우: `// Returns: true=...`
- Unity lifecycle (Awake/Start/Update 등): 무엇을 초기화·처리하는지 한 줄 설명

**기존 주석 보존 원칙**: 코드를 수정할 때 기존에 있던 주석을 삭제하지 않습니다. 내용이 달라졌다면 주석도 함께 업데이트합니다.

---

## 상태 머신 패턴

모든 상태 머신은 동일한 패턴을 따릅니다:

```csharp
// 상태 enum
public enum MyState { StateA, StateB }

// 현재 상태 (읽기 전용 프로퍼티)
public MyState State { get; private set; }

// 상태 전환 (private 메서드로만)
private void SetState(MyState next)
{
    if (State == next) return;  // Guard: 같은 상태 중복 방지
    State = next;
    OnStateChanged?.Invoke(next);  // 구독자 알림
    // 상태별 후처리 (비주얼, 사운드 등)
}
```

---

## 이벤트 구독/해제 패턴

```csharp
private void Start()
{
    // Start()에서 구독
    Board.OnGemExploded += OnGemExploded;
    GameManager.OnStateChanged += OnGameStateChanged;
}

private void OnDestroy()
{
    // OnDestroy()에서 반드시 해제 (메모리 누수 방지)
    Board.OnGemExploded -= OnGemExploded;
    GameManager.OnStateChanged -= OnGameStateChanged;
}
```

람다 캡처 대신 **명시적 메서드 참조**를 사용해야 해제 가능합니다.

---

## ScriptableObject (SO) 패턴

```csharp
[CreateAssetMenu(fileName = "NewXxxData", menuName = "Tycoon/XxxData")]
public class XxxData : ScriptableObject
{
    [Header("섹션")]
    [Tooltip("설명")]
    public int SomeValue;
}
```

- `GemData`: 보석 타입 정보 (`BlockTag`로 재료 식별)
- `RecipeData`: 레시피 재료·가격 (`IsAvailableInStage(int)` 메서드 포함)
- `StageData`: 스테이지 설정 (`AvailableGems` 리스트)
- `GameSettings`: 전역 설정 (Board, Visual, Sound 중첩 클래스)

---

## 코루틴 패턴

```csharp
// 코루틴 참조 보관 (중복 방지 + 외부 정지 가능)
private Coroutine m_SomeCoroutine;

private void StartSomething()
{
    if (m_SomeCoroutine == null)
        m_SomeCoroutine = StartCoroutine(SomeRoutine());
}

private void StopSomething()
{
    if (m_SomeCoroutine == null) return;
    StopCoroutine(m_SomeCoroutine);
    m_SomeCoroutine = null;
}

private IEnumerator SomeRoutine()
{
    while (condition)
    {
        yield return new WaitForSeconds(interval);
        // 상태 재확인 후 처리
    }
    m_SomeCoroutine = null;  // 자연 종료 시 참조 정리
}
```

---

## 오디오 사용법

```csharp
// 일반 효과음 (16개 풀링, 겹치면 이전 소리 끊김)
GameManager.Instance?.PlaySFX(myClip);

// 반드시 끝까지 재생해야 하는 SFX (Hall/UI 이벤트 등)
GameManager.Instance?.PlaySFXOneShot(myClip);

// 배경음악 크로스페이드
GameManager.Instance?.SwitchMusic(musicClip);
```

---

## 씬 실행 순서 (DefaultExecutionOrder)

```
-9999 : GameManager, Board   (가장 먼저 — 다른 컴포넌트에서 Instance 접근 가능하도록)
 기본 : HallManager, Table, Customer, UIHandler 등
+12000: LevelData            (가장 나중 — GameManager.StartLevel() 호출)
```

---

## 주요 흐름 요약

### 손님 방문 → 수금 전체 사이클

```
HallManager.TickCustomerSpawn()
  → Instantiate(CustomerPrefab) → Customer.Initialize()
  → Customer: Walkin → Ordering 전환
  → HallManager.WaitAndAssignRecipe() → Table.SetOrder(recipe)

Board 보석 매치 발생
  → Board.OnGemExploded 이벤트
  → HallManager.OnGemExploded() → FindBestTableForIngredient()
  → Table.AcceptGem() → FlyIngredient 코루틴 (포물선 비행)
  → IsOrderSatisfied() → TableState.Cooked → AutoServeAfterDelay(0.5s)

Player가 Cooked 테이블 탭 (또는 자동 서빙)
  → Table.ServeToCustomer() → Customer.StartEating()
  → WaitForMealComplete(EatingDuration) → CompleteMeal()
  → TableState.Paying → SpawnCoin()

Player가 코인 탭 (또는 StaffMember Cashier)
  → Table.CollectMoney() → OnMoneyCollected 이벤트
  → HallManager → LevelData.AddMoney(amount)
  → ResetTable() → TableState.Empty
```

### 피버 시스템

```
손님 만족 퇴장 → Customer.OnSatisfied → LevelData.AddHeart(1)
5초마다 → LevelData.HeartDecayLoop → AddHeart(-1)
CurrentHeart > FeverHeartCount * 0.7 → IsFever = true (골드 2배)
```

### Board Update 6단계

```
① CheckInput    — 스와이프 입력 감지
② TickSwap      — 스왑 애니메이션 + 매치 판정
③ MoveGems      — 낙하/바운스 애니메이션
④ DoMatchCheck  — 착지 보석 매치 검사
⑤ MatchTicking  — 매치 보석 삭제 타이머
⑥ EmptyCheck    — 빈 칸 채우기 (낙하 트리거)
```

---

## 새 기능 추가 체크리스트

### Hall 관련 새 기능

- [ ] 새 상태는 enum에 추가하고 SetState() 내부에서 처리
- [ ] 이벤트는 Start()에서 구독, OnDestroy()에서 해제
- [ ] Board와의 통신은 반드시 정적 이벤트를 통해
- [ ] GameState.Running 조건 확인 후 동작 (Closing/Result에서 중단)

### 새 ScriptableObject

- [ ] `[CreateAssetMenu(menuName = "Tycoon/XxxData")]` 추가
- [ ] Inspector 필드에 `[Header]` + `[Tooltip]` 한국어 작성

### 새 MonoBehaviour

- [ ] 네임스페이스 `DoggyChef` 선언
- [ ] 클래스 구조 순서 준수 (중첩타입 → const → Inspector → 이벤트 → 싱글턴 → public 상태 → private 상태 → lifecycle → public API → private 구현)
- [ ] private 필드는 `m_` 접두사, private static은 `s_` 접두사
- [ ] const는 섹션 2, readonly 설정값은 private 필드 섹션 최상단
- [ ] **모든 메서드에 `//` 주석 작성** (public은 `[호출자] 설명`, private은 동작 요약)
- [ ] `///` XML 스타일 주석 금지
- [ ] 코루틴 참조는 `StopCoroutine()` 호출이 필요한 경우에만 필드로 보관
- [ ] Unity lifecycle 메서드에 `private` 키워드 명시
- [ ] `[SerializeField]` 필드 이름 변경 시 `[FormerlySerializedAs("oldName")]` 추가 (씬 데이터 유실 방지)

---

## 자주 쓰는 패턴

### null 안전 호출

```csharp
GameManager.Instance?.PlaySFX(clip);   // ?. 연산자
OnSatisfied?.Invoke(this);             // null 체크 후 이벤트 발사
```

### 튜플 스왑

```csharp
(a, b) = (b, a);  // C# 튜플 구조분해 — 임시 변수 없이 스왑
```

### switch expression (Unity 2021+)

```csharp
string animName = State switch
{
    CustomerState.Walkin   => "walkin",
    CustomerState.Ordering => "order",
    _                      => null
};
```

### TryGetValue 패턴

```csharp
if (CellContent.TryGetValue(cell, out var content) && content.ContainingGem != null)
{
    // 안전하게 사용
}
```

### [SerializeField] 필드 이름 변경

`[SerializeField]` 필드 이름을 변경하면 Unity가 기존 씬·프리팹의 저장 데이터를 잃습니다.
반드시 `[FormerlySerializedAs]`를 추가해 이전 이름을 유지하세요.

```csharp
// 변경 전
[SerializeField] private GameObject sidePrefab;

// 변경 후 (기존 씬 데이터 보호)
[FormerlySerializedAs("sidePrefab")]
[SerializeField] private GameObject m_SidePrefab;
```

### target-typed new (C# 9+)

타입이 좌변에서 이미 명확할 때는 `new(...)` 단축형을 사용합니다.

```csharp
// 좌변에 타입이 명시된 경우
private readonly Vector2 m_Offset = new(0.5f, 0.5f);
List<Vector3Int> list = new();
m_Dict = new();                     // 필드 재할당도 동일

// 메서드 파라미터 타입이 명확한 경우
BonusItems.Add(new() { Amount = 1, Item = item });
sr.color = new(1f, 0.9f, 0.2f, 1f);

// var 선언은 타입 정보가 없으므로 new(...)를 쓸 수 없습니다
var gem = new Gem();    // ← var이므로 full name 필수
```

---

## 금지 사항

- Board와 Hall 간 **직접 참조** 금지 (이벤트로만)
- `FindObjectOfType` 남용 금지 (매 프레임 호출 시 성능 저하)
- `Update()` 내 `new` 할당 최소화 (GC 압박)
- 람다로 이벤트 구독 금지 (해제 불가)
- 싱글턴 외부에서 `Instance = ...` 직접 할당 금지
- `///` XML 문서 주석 금지 — `//` 일반 주석만 사용
- write-only 필드 금지 — 할당만 하고 읽지 않는 private 필드는 제거
- 중단하지 않는 코루틴의 반환값을 필드에 저장 금지 — `StartCoroutine(Foo())` 그대로 사용
- **`?.` 연산자는 적극 활용** — 이벤트·delegate·MonoBehaviour 필드 모두 `?.` 사용 (UNT0008은 `.editorconfig`에서 억제)
- 연속 빈 줄(2줄 이상) 금지 — 메서드 간격은 정확히 1줄
- 기존 주석 삭제 금지 — 코드 수정 시 기존 주석은 반드시 유지하고, 내용이 달라진 경우에만 주석 내용을 업데이트
