using System;
using System.Collections;
using System.Collections.Generic;
using Spine.Unity;
using TMPro;
using UnityEngine;

[RequireComponent(typeof(AudioSource))]
public class Customer : MonoBehaviour
{
    public enum CustomerState
    {
        None,
        Walkin,   // 테이블비워있음
        Ordering, // 주문 (대기)
        Eating,   // 먹는중
        Happy,    // 만족
        Angry,    // 화남
        Walkout   // 퇴장
    }

    public enum CustomerType
    {
        Cat,
    }


    [SerializeField] private AudioClip enterSfx;
    [SerializeField] private List<AudioClip> angrySfxList;
    [SerializeField] private List<AudioClip> eatSfxList;

    [SerializeField] private CustomerType type;

    [SerializeField] private HealthGage healthGage;

    public event Action<Customer> OnWalkInCompleted;          // 자리 도착(주문 대기 전환)
    public event Action<Customer> OnWalkOutCompleted;         // 퇴장 완료
    public static event Action<int, Customer, Vector3> OnHeartGained;

    private SkeletonAnimation animator;

    private Table table; // 앉아있는 테이블 (주문 중 체력 0시 비우기 용)

    private AudioSource voiceSource; // 손님 전용 보이스/효과음 소스 (중복 방지용 단일 채널)

    private readonly int angryThreshold = 3; // 체력이 이 값 이하로 떨어지면 Angry 전환
    private readonly int happyBeforeEatThreshold = 6; // 먹기 전에 해피 연출 조건 (체력 >= 이 값)

    private readonly int maxHealth = 10;
    private readonly float healthDecreaseInterval = 3f; // 초 단위 체력 감소 주기
    private int health = 10;

    private readonly float walkSpeed = 1.5f;
    private readonly float walkOutSpeed = 2.5f;

    private Coroutine healthDrainRoutine;

    private CustomerState state;
    private bool hasEaten; // 손님이 먹었는지 여부(하트 지급 조건)
    private bool heartGrantedOnExit; // 조건 충족 퇴장 시 하트 중복 지급 방지

    void Awake()
    {
        state = CustomerState.None;
        animator = GetComponentInChildren<SkeletonAnimation>();
        health = Mathf.Clamp(health, 0, Mathf.Max(1, maxHealth));

        voiceSource = GetComponent<AudioSource>();
        voiceSource.playOnAwake = false;
        voiceSource.loop = false; // 상태별 단발 재생
        // 필요 시 2D/3D 믹스 조정 (기본 2D)
        // voiceSource.spatialBlend = 0f;

        hasEaten = false;
        heartGrantedOnExit = false;
    }


    void Start()
    {
        UpdateHealthUI();
        UpdateAnimator();

        // 입장 시 효과음 재생 (Walkin 상태에서 시작하면)
        if (state == CustomerState.Walkin)
        {
            PlayVoice(enterSfx);
        }
    }


    public void SetState(CustomerState newState)
    {
        if (state == newState) return; // 중복 전환 방지
        state = newState;

        // 상태 전환에 따른 체력 감소 루틴 관리 (Ordering/Angry 상태에서만 감소 유지)
        if (state == CustomerState.Ordering || state == CustomerState.Angry)
        {
            StartHealthDrainIfNeeded();
        }
        else
        {
            StopHealthDrainIfNeeded();
        }

        UpdateAnimator();
        UpdateHealthUI();

        // 상태 전환 SFX 처리 (중복 방지: 같은 소스에서 Stop 후 Play)
        switch (state)
        {
            case CustomerState.Walkin:
                WalkInTo(table, walkSpeed);
                PlayVoice(enterSfx);
                break;
            case CustomerState.Eating:
                hasEaten = true;
                PlayRandomVoice(eatSfxList);
                break;
            case CustomerState.Angry:
                PlayRandomVoice(angrySfxList);
                break;
            case CustomerState.Walkout:
                WalkOutFromTable();
                break;
            case CustomerState.Happy:
                hasEaten = true; // 먹고 만족했음
                break;
        }
    }


    // 체력 변경 (delta는 음수/양수 모두 허용)
    public void ChangeHealth(int delta)
    {
        int old = health;
        health = Mathf.Clamp(health + delta, 0, maxHealth);
        if (health != old)
        {
            UpdateHealthUI();
            // 체력이 angryThreshold 이하(>0)로 떨어지면 분노 상태 전환 (주문 대기 중)
            if (health > 0 && health <= angryThreshold && state == CustomerState.Ordering)
            {
                SetState(CustomerState.Angry);
            }
            if (health == 0)
            {
                HandleHealthDepleted();
            }
        }
    }


    public void AssignTable(Table t)
    {
        table = t;
    }

    public int GetHealth() => health;
    public int HappyBeforeEatThreshold => happyBeforeEatThreshold;
    public Table GetTable() => table;


    // 일정한 속도로 걷기 (초당 유닛)
    private Coroutine WalkToBySpeed(Vector3 worldTarget, float speedUnitsPerSecond, Action onComplete = null)
    {
        return StartCoroutine(WalkToSpeedRoutine(worldTarget, speedUnitsPerSecond, onComplete));
    }

    // 테이블로 Walk-in 전용 진입: 내부에서 도착 시 Ordering 상태 전환 및 이벤트 알림
    private void WalkInTo(Table targetTable, float speedUnitsPerSecond)
    {
        if (targetTable == null) return;
        AssignTable(targetTable);
        Vector3 arrivePos = targetTable.GetLeftEdgeWorldPosition();
        WalkToBySpeed(arrivePos, speedUnitsPerSecond, () =>
        {
            // 도착 시점에 Walkin 상태였다면 주문 대기로 전환
            if (state == CustomerState.Walkin)
            {
                SetState(CustomerState.Ordering);
            }
            OnWalkInCompleted?.Invoke(this);
        });
    }

    // 퇴장 전용: 상태가 Walkout으로 전환되면 호출되어 퇴장 이동 및 완료 이벤트를 보낸 뒤 파괴
    private void WalkOutFromTable()
    {
        // 하트 지급 조건: angryThreshold 이상(즉 분노 임계 이상 유지), 그리고 손님이 먹은 뒤(또는 Happy) 퇴장하는 경우
        // 상태 전환 직전에 SetState로 Walkout이 호출되므로, 여기서 판단.
        if (!heartGrantedOnExit && (state == CustomerState.Walkout))
        {
            bool satisfied = hasEaten && (GetHealth() >= angryThreshold);
            if (satisfied)
            {
                heartGrantedOnExit = true;
                Vector3 startPos = (healthGage != null) ? healthGage.transform.position : transform.position;
                OnHeartGained?.Invoke(GetHealth(), this, startPos);
            }
        }

        Vector3 exitPos = table != null ? table.GetLeftEdgeWorldPosition() + Vector3.left * 1.5f : transform.position + Vector3.left * 2f;
        WalkToBySpeed(exitPos, Mathf.Max(0.01f, walkOutSpeed), () =>
        {
            OnWalkOutCompleted?.Invoke(this);
            Destroy(gameObject);
        });
    }


    private IEnumerator WalkToSpeedRoutine(Vector3 worldTarget, float speedUnitsPerSecond, Action onComplete)
    {
        float speed = Mathf.Max(0.01f, speedUnitsPerSecond);
        // MoveTowards로 등속 이동
        while ((transform.position - worldTarget).sqrMagnitude > 0.0001f)
        {
            transform.position = Vector3.MoveTowards(transform.position, worldTarget, speed * Time.deltaTime);
            yield return null;
        }
        transform.position = worldTarget;
        onComplete?.Invoke();
    }


    // 체력 감소 루틴 (기본 3초 간격; healthDecreaseInterval 사용)
    private IEnumerator HealthDrainLoop()
    {
        float interval = Mathf.Max(0.1f, healthDecreaseInterval);
        while (health > 0 && (state == CustomerState.Ordering || state == CustomerState.Angry))
        {
            yield return new WaitForSeconds(interval);
            ChangeHealth(-1);
        }
        healthDrainRoutine = null; // 종료 표시
    }


    private void StartHealthDrainIfNeeded()
    {
        if ((state == CustomerState.Ordering || state == CustomerState.Angry) && healthDrainRoutine == null && health > 0)
        {
            healthDrainRoutine = StartCoroutine(HealthDrainLoop());
        }
    }


    private void StopHealthDrainIfNeeded()
    {
        if (healthDrainRoutine != null)
        {
            StopCoroutine(healthDrainRoutine);
            healthDrainRoutine = null;
        }
    }


    private void HandleHealthDepleted()
    {
        // Ordering 중이라면 손님이 화내고 그냥 나감 (요구사항: 즉시 퇴장 / 테이블 empty)
        if (state == CustomerState.Ordering || state == CustomerState.Angry)
        {
            // 체력 감소 루틴 중지
            StopHealthDrainIfNeeded();
            table.ForceEmpty();
            // 퇴장 상태로 전환 (이동/파괴는 SetState 분기에서 처리)
            SetState(CustomerState.Walkout);
        }
    }


    private void UpdateHealthUI()
    {
        healthGage.SetHealth(health);
    }



    private void UpdateAnimator()
    {
        if (animator == null) return;

        switch (state)
        {
            case CustomerState.Walkin:
                animator.AnimationState.SetAnimation(0, "walk_in", true);
                break;
            case CustomerState.Ordering:
                animator.AnimationState.SetAnimation(0, "idle", true);
                break;
            case CustomerState.Eating:
                animator.AnimationState.SetAnimation(0, "eat", true);
                break;
            case CustomerState.Happy:
                animator.AnimationState.SetAnimation(0, "like", true);

                break;
            case CustomerState.Angry:
                animator.AnimationState.SetAnimation(0, "angry", true);
                break;
            case CustomerState.Walkout:
                animator.AnimationState.SetAnimation(0, "walk_out", true);
                break;
        }
    }


    private void PlayVoice(AudioClip clip)
    {
        if (voiceSource == null || clip == null) return;
        // 단일 소스에서만 재생하여 중복 방지
        voiceSource.Stop();
        voiceSource.clip = clip;
        voiceSource.Play();
    }

    private void PlayRandomVoice(List<AudioClip> clips)
    {
        var clip = GetRandomClip(clips);
        if (clip != null)
        {
            PlayVoice(clip);
        }
    }

    private static AudioClip GetRandomClip(List<AudioClip> clips)
    {
        if (clips == null || clips.Count == 0) return null;
        // null 제거
        int tries = 0;
        const int maxTries = 10;
        while (tries < maxTries)
        {
            int idx = UnityEngine.Random.Range(0, clips.Count);
            if (clips[idx] != null) return clips[idx];
            tries++;
        }
        // 모든 항목이 null일 가능성 방지
        foreach (var c in clips)
        {
            if (c != null) return c;
        }
        return null;
    }
}
