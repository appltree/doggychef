using System.Collections;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace DoggyChef
{
    public class StageManager : MonoBehaviour
    {
        [SerializeField] private ShopData[] shops;
        [SerializeField] private string mainSceneName = "MainScene";
        [SerializeField] private string gameSceneName = "Game";

        private int currentShopIndex;
        private bool isLoading;

        private void Awake()
        {
            var saveSystem = SaveSystem.Instance;
        }

        private void Start()
        {
            FrameRateController.ApplyFrameRate();
            Time.timeScale = 1f;
            AudioListener.pause = false;

            Map.OnMapFocused += OnMapFocused;
            StageCell.OnStageCellTapped += OnStageCellTapped;
        }

        private void OnDestroy()
        {
            Map.OnMapFocused -= OnMapFocused;
            StageCell.OnStageCellTapped -= OnStageCellTapped;
        }

        public ShopData GetShop(int index)
        {
            if (shops == null || index < 0 || index >= shops.Length)
                return null;

            return shops[index];
        }

        public void OnBack()
        {
            if (isLoading)
                return;

            isLoading = true;
            StartCoroutine(LoadScene(mainSceneName));
        }

        private void OnMapFocused(int index, bool isLeftSide)
        {
            currentShopIndex = index;
        }

        private void OnStageCellTapped(StageData selectedStage, int stageIndex)
        {
            if (isLoading)
                return;

            var currentShop = GetShop(currentShopIndex);
            if (currentShop == null || selectedStage == null)
            {
                Debug.LogWarning("[StageManager] Missing shop or stage data. Cannot proceed to game scene.");
                return;
            }

            StageSelectionData.Set(currentShop, selectedStage, stageIndex);

            isLoading = true;
            StartCoroutine(LoadScene(gameSceneName));
        }

        private IEnumerator LoadScene(string sceneName)
        {
            var op = SceneManager.LoadSceneAsync(sceneName, LoadSceneMode.Single);
            if (op == null)
            {
                Debug.LogWarning($"[StageManager] Failed to start loading scene '{sceneName}'. Check Build Settings.");
                isLoading = false;
                yield break;
            }

            while (!op.isDone)
                yield return null;
        }
    }

} // namespace DoggyChef
