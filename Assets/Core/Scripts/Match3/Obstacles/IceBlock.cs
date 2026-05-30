using System.Collections;
using UnityEngine;

namespace DoggyChef
{
  // ════════════════════════════════════════════════════════════════
  //  IceBlock — 보석 위에 쌓이는 얼음 장애물
  // ════════════════════════════════════════════════════════════════
  //
  //  ■ 동작 원리 (Origin 동일)
  //
  //  1. Board.TrySpawnIceBlock() 에서 Attach(gem) 호출
  //     → 보석의 자식 GameObject 로 붙음 (함께 낙하)
  //
  //  2. 얼음이 붙은 보석 상태
  //     - Gem.IsFrozen = true
  //     - 플레이어가 스왑할 수 없음 (BoardCell.CanBeMoved = false)
  //     - 3매칭에는 포함될 수 있음 (CanDelete = true)
  //
  //  3. 매칭 발생 시 (Board.MatchTicking)
  //     - 얼음이 있으면: BreakAndVanish() → 얼음만 깨짐
  //     - 보석은 Destroy 되지 않고 보드에 그대로 남음
  //     - 이후부터는 일반 보석으로 스왑 가능
  //
  //  ■ Inspector 셋업
  //     IceBlock prefab
  //       ├─ Animator   (break 트리거 → Ice_break.anim)
  //       └─ SpriteRenderer (block_ice.png, 보석보다 높은 Order)
  //     BreakSfx = ice_break.mp3
  // ════════════════════════════════════════════════════════════════
  public class IceBlock : MonoBehaviour
  {
    // ──────────────────────────────────────────────────────────
    //  Inspector 연결 필드
    // ──────────────────────────────────────────────────────────

    [Tooltip("얼음이 깨질 때 재생할 효과음 (없으면 생략).")]
    public AudioClip BreakSfx;

    // ──────────────────────────────────────────────────────────
    //  상수
    // ──────────────────────────────────────────────────────────

    /// <summary>Break 애니메이션이 끝나고 오브젝트가 사라질 때까지의 지연 시간(초). Origin 동일.</summary>
    private const float VanishDelay = 0.3f;

    /// <summary>Animator break 트리거 파라미터 이름 (Ice.controller 기준).</summary>
    private const string BreakTrigger = "break";

    // ──────────────────────────────────────────────────────────
    //  내부 컴포넌트 / 상태
    // ──────────────────────────────────────────────────────────

    private Animator m_Animator;
    private Gem m_Gem;        // 이 얼음이 덮고 있는 보석
    private bool m_Breaking;   // 중복 Break 방지 플래그

    // ================================================================
    //  Unity 생명 주기
    // ================================================================

    private void Awake()
    {
      m_Animator = GetComponentInChildren<Animator>();
    }

    // ================================================================
    //  공개 API
    // ================================================================

    /// <summary>
    /// 지정 보석에 얼음을 부착합니다. Board.TrySpawnIceBlock() 에서 호출합니다.
    ///
    /// - 보석의 자식 Transform 으로 설정 → 낙하 시 함께 이동합니다.
    /// - Gem.SetFrozen() 을 호출하여 보석이 얼음 상태임을 알립니다.
    /// </summary>
    public void Attach(Gem gem)
    {
      m_Gem = gem;
      gem.SetFrozen(this);

      // 보석의 자식으로 붙이고 중앙 정렬
      transform.SetParent(gem.transform, worldPositionStays: false);
      transform.localPosition = Vector3.zero;

      // ── 렌더링 순서 자동 동기화 ──────────────────────────────
      // 보석(Gem)보다 SortingOrder 를 1 높게 설정 → 얼음이 보석 위에 그려짐
      var gemSR = gem.GetComponentInChildren<SpriteRenderer>();
      var iceSR = GetComponentInChildren<SpriteRenderer>();
      if (gemSR != null && iceSR != null)
      {
        iceSR.sortingLayerID = gemSR.sortingLayerID;
        iceSR.sortingOrder = gemSR.sortingOrder + 1;
      }
    }

    /// <summary>
    /// 얼음을 깨고 오브젝트를 제거합니다. Board.MatchTicking 에서 호출합니다.
    ///
    /// 1. Gem.ClearFrozen() → 보석이 더 이상 얼음 상태가 아님
    /// 2. 효과음 재생
    /// 3. Break 애니메이션 트리거 → VanishDelay 후 Destroy
    /// </summary>
    public void BreakAndVanish()
    {
      if (m_Breaking) return;  // 이미 Breaking 중이면 무시
      m_Breaking = true;

      // 보석에서 얼음 상태 해제 (즉시 스왑 가능해짐)
      m_Gem?.ClearFrozen();

      GameManager.Instance?.PlaySFXOneShot(BreakSfx);

      // 부모(Gem) 에서 분리 — 보석이 먼저 이동해도 애니메이션은 제자리 유지
      transform.SetParent(null, worldPositionStays: true);

      StartCoroutine(VanishRoutine());
    }

    // ================================================================
    //  내부 루틴
    // ================================================================

    private IEnumerator VanishRoutine()
    {
      if (m_Animator != null)
        m_Animator.SetTrigger(BreakTrigger);

      yield return new WaitForSeconds(VanishDelay);
      Destroy(gameObject);
    }
  }
}
