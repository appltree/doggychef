using UnityEngine;

public class GameResultPanel : MonoBehaviour
{
    private GameManager gameManager;

    void Awake()
    {
        gameManager = FindFirstObjectByType<GameManager>();
    }


    public void QuitGame()
    {
        gameManager.QuitGame();
    }
}
