using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(SpriteRenderer))]
public class PaiedMoney : MonoBehaviour
{
    [SerializeField] private List<SpriteRenderer> coinSprites;
    [SerializeField] private int amount;

    [SerializeField] private AudioClip collectSfx; // 수거 시 재생할 효과음 (클립/볼륨은 인스펙터에서 설정)
    [SerializeField] private AudioClip collectByStaff;

    private readonly int amountPerCoin = 100; // 한 코인 당 금액 (예: 30코인 x 100 = 최대 3000)

    private readonly float idleTimeBeforeBlink = 10f; // 수거 안되면 이 시간 후 깜빡임 시작
    private readonly float blinkDuration = 3f;       // 깜빡이는 총 시간
    private readonly float blinkInterval = 0.15f;    // 깜빡임 간격

    private Table owner;
    private bool collected = false;
    private Coroutine lifeRoutine;

    // 수거 콜백(Action): (amount, ownerTable, worldPosition)
    public static event Action<int, Table, Vector3> OnCollected;

    // owner Table을 외부에서 확인할 수 있는 프로퍼티
    public Table Owner => owner;


    public void Collect()
    {
        if (collected) return;
        if (owner == null) return;
        collected = true;

        if (lifeRoutine != null)
        {
            StopCoroutine(lifeRoutine);
        }

        // 수거 사운드 재생 (오브젝트 파괴 시에도 끊기지 않도록 PlayClipAtPoint 사용)
        PlayCollectSfx();

        // 정적 이벤트 우선 처리
        OnCollected.Invoke(amount, owner, transform.position);
        // 코인은 클릭 시 항상 제거 (UI/상태 정리는 구독자 측에서 처리)
        Destroy(gameObject);
    }

    /// <summary>
    /// Staff가 대신 수금할 때 호출. 코인을 Staff 위치로 짧게 이동 연출 후 OnCollected 발생.
    /// </summary>
    public void CollectViaStaff(Staff staff, Vector3 staffHoldWorldPos, float travelDuration = 0.35f)
    {
        if (collected) return;
        if (owner == null || staff == null) return;
        collected = true;
        if (lifeRoutine != null) StopCoroutine(lifeRoutine);
        SetBlinkVisible(true); // 깜빡임 중단 및 원복
        StartCoroutine(CoCollectViaStaff(staffHoldWorldPos, travelDuration));
    }

    private IEnumerator CoCollectViaStaff(Vector3 targetPos, float duration)
    {
        // 이동 전 약간 작게 만들기
        Vector3 startScale = transform.localScale;
        Vector3 midScale = startScale * 0.6f;
        float scaleT = 0f;
        float scaleDur = Mathf.Min(0.15f, duration * 0.4f);
        while (scaleT < 1f)
        {
            scaleT += Time.deltaTime / Mathf.Max(0.01f, scaleDur);
            float k = Mathf.SmoothStep(0f, 1f, scaleT);
            transform.localScale = Vector3.Lerp(startScale, midScale, k);
            yield return null;
        }

        Vector3 startPos = transform.position;
        float t = 0f;
        while (t < 1f)
        {
            t += Time.deltaTime / Mathf.Max(0.01f, duration);
            float k = Mathf.SmoothStep(0f, 1f, t);
            transform.position = Vector3.Lerp(startPos, targetPos, k);
            yield return null;
        }
        transform.position = targetPos;

        // Staff 손에 도착한 뒤 수금 사운드/이벤트 -> Gauge 비행은 기존 GoldGauge가 worldPos 기준 처리
        PlayCollectByStaffSfx();
        OnCollected?.Invoke(amount, owner, targetPos);
        Destroy(gameObject);
    }


    public void SetupPaiedMoney(Table table, int coinAmount)
    {
        owner = table;
        amount = Mathf.Max(0, coinAmount);

        // 재사용(오브젝트 풀) 대비 초기화
        collected = false;
        if (lifeRoutine != null)
        {
            StopCoroutine(lifeRoutine);
        }

        // 금액에 따른 코인 표시 갱신
        RefreshCoinsVisual();
        lifeRoutine = StartCoroutine(LifeCycleRoutine());
    }

    private void PlayCollectSfx()
    {
        AudioSource.PlayClipAtPoint(collectSfx, Camera.main.transform.position, 0.9f);
    }

    private void PlayCollectByStaffSfx()
    {
        AudioSource.PlayClipAtPoint(collectByStaff, Camera.main.transform.position, 0.9f);
    }



    private IEnumerator LifeCycleRoutine()
    {
        // 1) 대기
        float t = 0f;
        while (t < idleTimeBeforeBlink)
        {
            if (collected) yield break; // 수거되면 종료
            t += Time.deltaTime;
            yield return null;
        }

        // 2) 깜빡임
        float blinkElapsed = 0f;
        bool visible = true;
        while (blinkElapsed < blinkDuration)
        {
            if (collected) yield break;
            visible = !visible;
            // 표시 대상 코인만 깜빡임
            SetBlinkVisible(visible);
            yield return new WaitForSeconds(blinkInterval);
            blinkElapsed += blinkInterval;
        }

        // 3) 파괴 (미수거 상태)
        if (!collected)
        {
            // 파괴 직전 가시성 원복(선택)
            SetBlinkVisible(true);
            // 테이블에 소멸 알림 (미수거 처리)
            if (owner != null)
            {
                owner.OnMoneyExpired(this);
            }
            Destroy(gameObject);
        }
    }

    // 금액(amount)에 따라 coinSprites의 표시/비표시를 갱신
    private void RefreshCoinsVisual()
    {
        if (coinSprites == null || coinSprites.Count == 0) return;
        int showCount = GetVisibleCoinCount();
        for (int i = 0; i < coinSprites.Count; i++)
        {
            if (coinSprites[i] == null) continue;
            coinSprites[i].enabled = i < showCount;
        }
    }

    // 깜빡임 상태를 적용하되, 표시 대상 코인에만 적용
    private void SetBlinkVisible(bool on)
    {
        if (coinSprites == null || coinSprites.Count == 0) return;
        int showCount = GetVisibleCoinCount();
        for (int i = 0; i < coinSprites.Count; i++)
        {
            var sr = coinSprites[i];
            if (sr == null) continue;
            sr.enabled = (i < showCount) && on;
        }
    }

    private int GetVisibleCoinCount()
    {
        int count = Mathf.FloorToInt(amount / (float)amountPerCoin);
        int maxCoins = Mathf.Min(30, coinSprites.Count);
        count = Mathf.Clamp(count, 1, maxCoins);
        return count;
    }
}
