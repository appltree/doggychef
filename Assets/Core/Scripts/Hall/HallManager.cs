using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace DoggyChef
{
    // ════════════════════════════════════════════════════════════════
    //  HallManager — 식당 홀 전체를 관리하는 매니저
    // ════════════════════════════════════════════════════════════════
    //
    //  ■ 역할 (5가지)
    //  1. 손님 스폰 타이밍 관리
    //       Running 중 빈 테이블의 언락 시간이 지나면 손님을 스폰합니다.
    //  2. Board → Hall 재료 라우팅
    //       Board.OnGemExploded / OnGemExplodedDelayed 구독
    //       → 적절한 테이블에 AcceptGem()을 전달합니다.
    //  3. 레시피 필터링
    //       LevelData.CurrentStage.Number 기반으로 현재 스테이지에서
    //       사용 가능한 레시피만 추려 손님에게 랜덤 배정합니다.
    //  4. 하트 지급
    //       손님 만족 퇴장 시 LevelData.AddHeart(1) 호출.
    //  5. 수금 처리
    //       Table.OnMoneyCollected 이벤트 수신 → LevelData.AddMoney(amount) 호출.
    //
    //  ■ 씬 셋업 (Inspector 연결 필요)
    //       Tables[0,1,2]  → Table_0, Table_1, Table_2 GameObject
    //       AllRecipes     → 스테이지에서 쓸 RecipeData 에셋 배열
    //
    //  ■ 테이블 언락 시스템
    //       게임 시작 후 TableUnlockTimes에 지정된 시간이 될 때마다 순차 언락됩니다.
    //       기본: {1초, 10초, 20초}
    // ════════════════════════════════════════════════════════════════
    public class HallManager : MonoBehaviour
    {
        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────

        [Header("테이블 및 스폰 포인트")]
        [Tooltip("씬에 배치된 Table 컴포넌트들 (Table_0, Table_1, Table_2 순서).")]
        public Table[] Tables;

        [Header("프리팹")]
        [Tooltip("레시피 슬롯 UI 프리팹 (TableOrderDisplay에서 사용)")]
        public GameObject IngredientSlotPrefab;

        [Header("레시피 데이터")]
        [Tooltip("이 스테이지에서 사용할 모든 RecipeData 에셋.")]
        public RecipeData[] AllRecipes;

        [Header("테이블 언락 타이밍 (초)")]
        [Tooltip("각 테이블이 활성화될 경과 시간. 예) {1, 10, 20}")]
        public float[] TableUnlockTimes = { 1f, 10f, 20f };

        // ──────────────────────────────────────────────────────────
        //  이벤트
        // ──────────────────────────────────────────────────────────

        // 모든 테이블이 비워졌을 때 발화됩니다. GameManager가 구독하여 Result 씬으로 전환합니다.
        public static event Action OnAllTablesCleared;

        // ──────────────────────────────────────────────────────────
        //  싱글턴
        // ──────────────────────────────────────────────────────────

        public static HallManager Instance { get; private set; }

        // ──────────────────────────────────────────────────────────
        //  런타임 상태
        // ──────────────────────────────────────────────────────────

        // Running 상태 경과 시간 (테이블 언락 판별)
        private float m_ElapsedTime;

        // 스폰 진행 중인 테이블 인덱스 집합 (중복 스폰 방지)
        private readonly HashSet<int> m_ReservedTables = new();

        // Customer → 테이블 인덱스 매핑 (람다 캡처 없이 명시적 구독 해제를 위해 사용)
        private readonly Dictionary<Customer, int> m_CustomerTableMap = new();

        // 현재 스테이지에서 사용 가능한 레시피 캐시 (Start에서 초기화)
        private readonly List<RecipeData> m_AvailableRecipes = new();

        // ================================================================
        //  Unity 생명 주기
        // ================================================================

        private void Awake()
        {
            if (Instance != null && Instance != this) { Destroy(gameObject); return; }
            Instance = this;
        }

        private void Start()
        {
            // 보드 이벤트 구독 (느슨한 연결 — 보드와 홀은 이벤트로만 통신합니다)
            Board.OnGemExploded += OnGemExploded;
            Board.OnGemExplodedDelayed += OnGemExplodedDelayed;

            // 테이블 수금 이벤트 연결
            foreach (var table in Tables)
            {
                if (table != null)
                    table.OnMoneyCollected += OnTableMoneyCollected;
            }

            RefreshAvailableRecipes();
        }

        private void OnDestroy()
        {
            if (Instance == this) Instance = null;
            Board.OnGemExploded -= OnGemExploded;
            Board.OnGemExplodedDelayed -= OnGemExplodedDelayed;
            if (Tables != null)
            {
                foreach (var table in Tables)
                {
                    if (table != null)
                        table.OnMoneyCollected -= OnTableMoneyCollected;
                }
            }
        }

        // ================================================================
        //  Update — Running/Closing 단계 처리
        // ================================================================

        private void Update()
        {
            if (GameManager.Instance == null) return;

            switch (GameManager.Instance.CurrentState)
            {
                case GameManager.GameState.Running:
                    m_ElapsedTime += Time.deltaTime;
                    TickCustomerSpawn();
                    break;

                case GameManager.GameState.Closing:
                    TickClosingCheck();
                    break;
            }
        }

        // ================================================================
        //  Tick 1) 손님 스폰
        // ================================================================

        // 비어 있고 언락된 테이블에 순서대로 손님을 스폰합니다.
        private void TickCustomerSpawn()
        {
            for (int i = 0; i < Tables.Length; i++)
            {
                var table = Tables[i];
                if (table == null) continue;
                if (table.State != Table.TableState.Empty) continue;  // 비어 있지 않음
                if (m_ReservedTables.Contains(i)) continue;  // 스폰 진행 중

                // 언락 시간 확인 (배열 범위 초과 시 즉시 언락)
                float unlockTime = i < TableUnlockTimes.Length ? TableUnlockTimes[i] : 0f;
                if (m_ElapsedTime < unlockTime) continue;

                SpawnCustomerForTable(i);
            }
        }

        // 지정 인덱스 테이블에 손님을 생성하고 초기화합니다.
        private void SpawnCustomerForTable(int tableIndex)
        {
            if (m_AvailableRecipes.Count == 0) return;  // 레시피 없으면 스폰 불가

            var table = Tables[tableIndex];
            var spawnPoint = table.CustomerSpawn;
            if (spawnPoint == null) return;

            var prefabs = LevelData.Instance.CustomerPrefabs;
            if (prefabs == null || prefabs.Length == 0)
            {
                Debug.LogError("[HallManager] LevelData에 CustomerPrefabs가 설정되지 않았습니다.");
                return;
            }

            // 중복 스폰 방지를 위해 먼저 예약합니다.
            m_ReservedTables.Add(tableIndex);

            var prefab = prefabs[UnityEngine.Random.Range(0, prefabs.Length)];
            var customerObj = Instantiate(prefab, spawnPoint.position, Quaternion.identity);
            var customer = customerObj.GetComponent<Customer>();

            if (customer == null)
            {
                Destroy(customerObj);
                m_ReservedTables.Remove(tableIndex);
                return;
            }

            // 명시적 메서드 참조로 구독 → OnCustomerWalkoutComplete에서 구독 해제
            m_CustomerTableMap[customer] = tableIndex;
            customer.OnSatisfied += OnCustomerSatisfied;
            customer.OnWalkoutComplete += OnCustomerWalkoutComplete;

            table.AssignCustomer(customer);
            customer.Initialize(table, spawnPoint.position, tableIndex + 1);

            // Walkin이 완료(Ordering 상태)된 뒤 레시피를 배정합니다.
            StartCoroutine(WaitAndAssignRecipe(customer, table));
        }

        // 손님이 Ordering 상태가 될 때까지 기다린 후 레시피를 배정합니다.
        private IEnumerator WaitAndAssignRecipe(Customer customer, Table table)
        {
            while (customer != null && customer.State != Customer.CustomerState.Ordering)
                yield return null;

            if (customer == null || table == null) yield break;

            var recipe = GetRandomAvailableRecipe();
            if (recipe != null)
                table.SetOrder(recipe);
        }

        // ================================================================
        //  Board 이벤트 처리 — 재료 라우팅
        // ================================================================

        // Board.OnGemExploded 핸들러.
        // 파괴된 보석의 BlockTag를 확인하고, 해당 재료가 필요한 최우선 테이블에 전달합니다.
        private void OnGemExploded(Gem gem, Vector3 worldPos)
        {
            if (gem?.Data == null) return;

            string tag = gem.Data.BlockTag;
            if (string.IsNullOrEmpty(tag)) return;

            var sr = gem.GetComponentInChildren<SpriteRenderer>();
            FindBestTableForIngredient(tag)?.AcceptGem(tag, worldPos, sr?.sprite, gem.Data.FoodSfx);
        }

        // Board.OnGemExplodedDelayed 핸들러.
        // SmallBomb 머지 연출 완료 후 지연 발화됩니다.
        // Gem 오브젝트가 이미 파괴된 상태이므로 데이터를 직접 받아 처리합니다.
        private void OnGemExplodedDelayed(string tag, Vector3 worldPos, Sprite sprite, AudioClip foodSfx)
        {
            if (string.IsNullOrEmpty(tag)) return;
            FindBestTableForIngredient(tag)?.AcceptGem(tag, worldPos, sprite, foodSfx);
        }

        // 지정 재료(gemTag)가 필요한 테이블 중 손님 체력이 가장 낮은 테이블을 반환합니다.
        // 
        // ■ 우선순위 전략
        //   체력이 낮은 손님(= 더 급한 손님)을 먼저 채워줍니다.
        //   GetRemainingNeed()는 AcceptGem() 내에서 즉시 갱신되므로,
        //   SmallBomb x8처럼 같은 프레임 내 다중 호출에도 정확히 분산됩니다.
        private Table FindBestTableForIngredient(string gemTag)
        {
            Table best = null;
            int lowestHealth = int.MaxValue;

            foreach (var table in Tables)
            {
                if (table == null) continue;
                if (table.State != Table.TableState.Ordered) continue;

                int remaining = table.GetRemainingNeed(gemTag);
                if (remaining <= 0) continue;  // 이미 충족됐거나 이 재료가 필요 없음

                int health = table.CustomerCurrentHealth;
                if (health < lowestHealth)
                {
                    lowestHealth = health;
                    best = table;
                }
            }

            return best;
        }

        // ================================================================
        //  이벤트 핸들러
        // ================================================================

        // 손님 만족 퇴장 시 호출됩니다. → LevelData.AddHeart(1)
        private void OnCustomerSatisfied(Customer customer)
        {
            if (LevelData.Instance == null) return;
            var pos = customer != null ? customer.transform.position : Vector3.zero;
            LevelData.Instance.AddHeart(1, pos);
        }

        // 손님 퇴장 완료 시 호출됩니다. (Destroy 직전)
        // 예약 슬롯을 해제하고 이벤트 구독을 정리합니다.
        private void OnCustomerWalkoutComplete(Customer customer)
        {
            if (m_CustomerTableMap.TryGetValue(customer, out int idx))
            {
                m_ReservedTables.Remove(idx);
                m_CustomerTableMap.Remove(customer);
            }

            customer.OnSatisfied -= OnCustomerSatisfied;
            customer.OnWalkoutComplete -= OnCustomerWalkoutComplete;
        }

        // <summary>테이블 수금 완료 시 호출됩니다. → LevelData.AddMoney(amount)
        // fromPos: GoldGauge 코인 비행 출발 위치 (코인 탭 시 코인 위치, Staff 수금 시 Staff 위치)</summary>
        private void OnTableMoneyCollected(Table table, int amount, Vector3 fromPos)
        {
            if (LevelData.Instance == null) return;
            LevelData.Instance.AddMoney(amount, fromPos);
        }

        // ================================================================
        //  Tick 2) 종료 판정 (Closing 상태에서 매 프레임 폴링)
        // ================================================================

        // 모든 테이블이 Empty이고 스폰 진행 중인 테이블이 없으면 OnAllTablesCleared를 발화합니다.
        // 
        // 이벤트 기반 단발 체크 대신 폴링을 사용하는 이유:
        // 테이블이 Closing 진입 이후 나중에 Empty가 되는 경우도 놓치지 않기 위함입니다.
        private void TickClosingCheck()
        {
            bool allEmpty = Tables.All(t => t == null || t.State == Table.TableState.Empty);
            if (allEmpty && m_ReservedTables.Count == 0)
                OnAllTablesCleared?.Invoke();
        }

        // ================================================================
        //  외부 API — 부스터 등에서 호출
        // ================================================================

        // 주문 중(Ordering/Angry) 상태인 모든 손님의 체력을 amount만큼 회복합니다.
        // CandyBooster.UseInstant()에서 호출합니다.
        public void HealAllOrderingCustomers(int amount)
        {
            foreach (var table in Tables)
            {
                if (table == null) continue;
                table.HealCustomer(amount);
            }
        }

        // 주문 중(Ordering/Angry) 상태인 모든 손님의 체력을 꽉 채웁니다.
        // CandyBooster.UseInstant()에서 호출합니다.
        public void FullHealAllOrderingCustomers()
        {
            foreach (var table in Tables)
            {
                if (table == null) continue;
                table.FullHealCustomer();
            }
        }

        // ================================================================
        //  레시피 관리
        // ================================================================

        // 현재 스테이지 번호를 기준으로 사용 가능한 레시피를 필터링합니다.
        // 필터링 결과가 없으면 모든 레시피를 사용합니다 (빈 레시피 방지).
        private void RefreshAvailableRecipes()
        {
            m_AvailableRecipes.Clear();
            if (AllRecipes == null) return;

            int stageNumber = LevelData.Instance?.CurrentStage?.Number ?? 1;

            foreach (var recipe in AllRecipes)
            {
                if (recipe != null && recipe.IsAvailableInStage(stageNumber))
                    m_AvailableRecipes.Add(recipe);
            }

            // 필터링 결과가 없으면 전체 레시피를 사용합니다.
            if (m_AvailableRecipes.Count == 0)
                m_AvailableRecipes.AddRange(AllRecipes.Where(r => r != null));
        }

        // 필터링된 레시피 중 랜덤으로 하나를 반환합니다.
        private RecipeData GetRandomAvailableRecipe()
        {
            if (m_AvailableRecipes.Count == 0) return null;
            return m_AvailableRecipes[UnityEngine.Random.Range(0, m_AvailableRecipes.Count)];
        }
    }
}
