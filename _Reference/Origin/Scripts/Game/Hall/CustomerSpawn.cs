using UnityEngine;

public class CustomerSpawn : MonoBehaviour
{
    [SerializeField] private Customer[] customerPrefabs;

    public Customer GetRandomPrefab()
    {
        if (customerPrefabs == null || customerPrefabs.Length == 0) return null;
        return customerPrefabs[Random.Range(0, customerPrefabs.Length)];
    }

    public Vector3 GetSpawnPosition() => transform.position;
}
