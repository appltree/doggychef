using UnityEngine;

public class GameMenuPanel : MonoBehaviour
{
    private GameManager gameManager;

    void Awake()
    {
        gameManager = FindFirstObjectByType<GameManager>();
    }

    void OnEnable()
    {
        // 패널이 활성화될 때 게임 일시정지
        if (gameManager != null)
        {
            gameManager.PauseGame();
        }
    }

    void OnDisable()
    {
        // 패널이 비활성화될 때 게임 재개
        if (gameManager != null)
        {
            gameManager.ResumeGame();
        }
    }

    public void QuitGame()
    {
        gameManager.QuitGame();
    }
}