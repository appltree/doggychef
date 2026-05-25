using UnityEngine;
using System.Collections.Generic;

public class HallManager : MonoBehaviour
{
    [SerializeField] private RecipeSO[] recipes;
    [SerializeField] private Table[] tables;          // 반드시 3개(또는 그 이하)라 가정, 많아도 Unlock 수까지만 활용
    [SerializeField] private CustomerSpawn[] customerSpawns; // 테이블 인덱스와 동일 길이 권장

    private readonly List<Customer> activeCustomers = new();
    private readonly HashSet<Table> reservedTables = new(); // 이동 중/도착 직전 중복 스폰 방지

    private readonly float firstCustomerDelay = 1f;
    private readonly float secondTableUnlockTime = 10f; // 2번째 테이블 활성화 시간 (Running 시작 기준 경과초)
    private readonly float thirdTableUnlockTime = 20f; // 3번째 테이블 활성화 시간 (Running 시작 기준 경과초)

    // GameState 기반 스폰 제어
    private bool spawnAllowed = false;
    private float runningStartTime = -1f;

    void Update()
    {
        // 활성 고객 정리(스폰 여부와 무관)
        CleanupActiveCustomers();

        // Running 상태에서만 스폰 로직 수행
        if (!spawnAllowed) return;
        EnsureSpawnForUnlockedTables();
    }


    void OnEnable()
    {
        BorderBlockDestroyer.OnBlockExploded += OnBlockExploded;

        GameManager.OnStateChanged += HandleGameStateChanged;

        runningStartTime = -1f;
    }

    void OnDisable()
    {
        BorderBlockDestroyer.OnBlockExploded -= OnBlockExploded;
        GameManager.OnStateChanged -= HandleGameStateChanged;
    }


    /// <summary>
    /// 모든 테이블이 Empty 상태인지 확인합니다.
    /// </summary>
    /// <returns>모든 테이블이 Empty이면 true, 아니면 false</returns>
    public bool AreAllTablesEmpty()
    {
        if (tables == null) return true;

        for (int i = 0; i < tables.Length; i++)
        {
            var table = tables[i];
            if (table != null && !table.IsEmpty())
            {
                return false;
            }
        }
        return true;
    }


    private void CleanupActiveCustomers()
    {
        for (int i = activeCustomers.Count - 1; i >= 0; i--)
        {
            if (activeCustomers[i] == null) activeCustomers.RemoveAt(i);
        }
    }


    private void OnBlockExploded(Block block, Vector3 pos)
    {
        if (block == null) return;
        string tag = block.BlockTag;
        if (string.IsNullOrEmpty(tag)) return;
        Table best = null;
        int bestHealth = int.MaxValue;
        // 우선 순위: 필요한 재료 남아있는 테이블 중 고객 체력 가장 낮은
        for (int i = 0; i < tables.Length; i++)
        {
            var t = tables[i];
            if (t == null) continue;
            if (t.GetRemainingNeed(tag) <= 0) continue;
            int h = t.GetCustomerHealth();
            if (h < bestHealth)
            {
                bestHealth = h;
                best = t;
            }
        }
        if (best != null)
        {
            best.AcceptExplodedBlock(block);
        }
    }


    private void EnsureSpawnForUnlockedTables()
    {
        int unlocked = GetUnlockedTableCount();
        for (int i = 0; i < unlocked; i++)
        {
            TrySpawnForIndex(i);
        }
    }


    private int GetUnlockedTableCount()
    {
        // Running이 아니면 해제 테이블 0으로 간주
        if (!spawnAllowed || runningStartTime < 0f) return 0;

        float t = Time.time - runningStartTime; // Running 시작 이후 경과 시간
        // 첫 손님 지연 전에는 0테이블로 간주하여 스폰을 막음
        int count = (t >= firstCustomerDelay) ? 1 : 0;
        if (t >= secondTableUnlockTime) count = 2;
        if (t >= thirdTableUnlockTime) count = 3;
        return Mathf.Min(count, tables != null ? tables.Length : 0);
    }


    private bool TrySpawnForIndex(int index)
    {
        var targetTable = tables[index];
        if (!targetTable.IsEmpty())
        {
            return false; // 이미 고객 있음
        }

        if (reservedTables.Contains(targetTable)) return false; // 이동 중 예약됨

        var spawn = customerSpawns[index];
        var prefab = spawn.GetRandomPrefab();

        reservedTables.Add(targetTable);

        Vector3 spawnPos = spawn.GetSpawnPosition();
        var customer = Instantiate(prefab, spawnPos, Quaternion.identity);
        activeCustomers.Add(customer);
        customer.AssignTable(targetTable);
        customer.SetState(Customer.CustomerState.Walkin);

        // Walk-in 완료시 처리 로직은 Customer 내부 OnWalkInCompleted 이벤트를 통해 수행
        customer.OnWalkInCompleted += HandleWalkInCompleted;

        return true;
    }

    private void HandleGameStateChanged(GameState prev, GameState curr)
    {
        // Running 진입 시부터 스폰 허용 및 기준 시간 리셋
        if (curr == GameState.Running)
        {
            spawnAllowed = true;
            runningStartTime = Time.time;
            // 즉시 한 번 체크하여 필요한 경우 스폰 시작
            EnsureSpawnForUnlockedTables();
        }
        else
        {
            spawnAllowed = false;
            runningStartTime = -1f;
        }
    }

    private void HandleWalkInCompleted(Customer customer)
    {
        if (customer == null) return;
        var targetTable = customer.GetTable();
        if (targetTable == null) return;

        // 주문 생성 및 배치
        if (recipes != null && recipes.Length > 0)
        {
            RecipeSO order = recipes[Random.Range(0, recipes.Length)];
            targetTable.AssignCustomer(customer);
            targetTable.SetOrder(order);
        }

        // 예약 해제
        if (reservedTables.Contains(targetTable))
        {
            reservedTables.Remove(targetTable);
        }

        // 일회성 구독 해제
        customer.OnWalkInCompleted -= HandleWalkInCompleted;
    }
}
