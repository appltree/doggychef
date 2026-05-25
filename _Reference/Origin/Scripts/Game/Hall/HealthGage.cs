using UnityEngine;

public class HealthGage : MonoBehaviour
{

    [System.Serializable]
    private struct HealthGageSlot
    {
        public Transform root;
        public GameObject left;
        public GameObject right;
        public GameObject full;
    }

    [SerializeField]
    private HealthGageSlot[] slots;

    private int health = 10;


    void Start()
    {
        Refresh();
    }

    public void SetHealth(int value)
    {
        value = Mathf.Clamp(value, 0, 10);
        health = value;
        Refresh();
    }

    private void Refresh()
    {
        int clamped = Mathf.Clamp(health, 0, 10);
        for (int i = 0; i < slots.Length; i++)
        {
            var slot = slots[i];

            // Compute health contained in this slot (2 points per slot)
            int slotValue = Mathf.Clamp(clamped - i * 2, 0, 2);

            // Hide all first
            slot.left.SetActive(false);
            slot.right.SetActive(false);
            slot.full.SetActive(false);

            if (slotValue == 2)
            {
                slot.full.SetActive(true);
            }
            else if (slotValue == 1)
            {
                slot.left.SetActive(true);
            }
        }
    }


}
