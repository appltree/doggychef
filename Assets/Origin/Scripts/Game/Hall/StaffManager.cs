using UnityEngine;

public class StaffManager : MonoBehaviour
{
    [SerializeField] private Transform staffSpawnPoint;   // Staff가 서 있는 위치
    [SerializeField] private Staff[] staffPrefabs;

    void Start()
    {
        SpawnManagerForTest();
    }

    private void SpawnManagerForTest()
    {
        if (staffPrefabs == null || staffPrefabs.Length < 2) return;
        var managerPrefab = staffPrefabs[1];
        if (managerPrefab == null) return;
        Transform parent = staffSpawnPoint != null ? staffSpawnPoint : transform;
        Vector3 spawnPos = parent.position;
        var existing = parent.GetComponentInChildren<Staff>();
        if (existing != null && existing != managerPrefab)
        {
            Destroy(existing.gameObject);
            existing = null;
        }
        if (existing == null)
        {
            Instantiate(managerPrefab, spawnPos, Quaternion.identity, parent);
        }
    }
}
