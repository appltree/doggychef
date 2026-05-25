// OK
using UnityEngine;


public class Ice : MonoBehaviour
{
    private readonly string breakTriggerName = "break"; // Animator 트리거명
    private readonly float vanishDelay = 0.3f; // 깨지고 사라질 때까지 지연

    private Animator animator;


    void Awake()
    {
        animator = GetComponentInChildren<Animator>();
    }


    public void BreakAndVanish()
    {
        SoundManager.I.PlayIceBreak();
        animator.SetTrigger(breakTriggerName);
        Destroy(gameObject, vanishDelay);
    }
}
