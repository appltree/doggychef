using UnityEngine;

namespace DoggyChef
{
    public class SaveSystem : MonoBehaviour
    {
        private static SaveSystem instance;
        public static SaveSystem Instance
        {
            get
            {
                if (instance == null)
                {
                    var go = new GameObject("SaveSystem");
                    instance = go.AddComponent<SaveSystem>();
                }

                return instance;
            }

            private set => instance = value;
        }

        private void Awake()
        {
            if (instance != null && instance != this)
            {
                Destroy(gameObject);
                return;
            }

            instance = this;
            DontDestroyOnLoad(gameObject);
        }

        public void RecordStageResult(ShopType shopType, int stageNumber, int earnedMoney, int goalMoney, int perfectMoney)
        {
            int stars = earnedMoney >= perfectMoney ? 3 : earnedMoney >= goalMoney ? 1 : 0;
            if (stars <= 0)
                return;

            string key = GetStageKey(shopType, stageNumber);
            int previous = PlayerPrefs.GetInt(key, 0);
            if (stars > previous)
            {
                PlayerPrefs.SetInt(key, stars);
                PlayerPrefs.Save();
            }
        }

        public bool IsStageCleared(ShopType shopType, int stageNumber)
        {
            return GetStageStarCount(shopType, stageNumber) > 0;
        }

        public int GetStageStarCount(ShopType shopType, int stageNumber)
        {
            return PlayerPrefs.GetInt(GetStageKey(shopType, stageNumber), 0);
        }

        private static string GetStageKey(ShopType shopType, int stageNumber)
        {
            return $"StageProgress.{shopType}.{stageNumber}";
        }
    }
}
