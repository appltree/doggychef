using UnityEngine;
using UnityEngine.VFX;

namespace DoggyChef
{
    /// <summary>
    /// 보드 위에 배치되는 보석의 기반 클래스.
    ///
    /// ■ 역할 분담
    ///   - Gem        : 이동/낙하/매치 등 순수 게임플레이 데이터
    ///   - BonusGem   : 특수 능력(Use)을 가진 파생 클래스
    ///   - Board      : Gem의 이동·매치·파괴 로직을 총괄 관리
    ///
    /// ■ 상태 흐름
    ///   Still → Falling → Bouncing → Still (→ Disappearing)
    /// </summary>
    public class Gem : MonoBehaviour
    {
        // ──────────────────────────────────────────────────────────
        //  상태 정의
        // ──────────────────────────────────────────────────────────

        public enum State
        {
            Still,          // 정지 중
            Falling,        // 낙하 중
            Bouncing,       // 착지 후 바운스 중
            Disappearing    // 파괴 중 (Destroy 대기)
        }

        // ──────────────────────────────────────────────────────────
        //  Inspector 직렬화 필드
        // ──────────────────────────────────────────────────────────

        /// <summary>매치 시 재생되는 VFX 프리팹 배열.</summary>
        public VisualEffect[] MatchEffectPrefabs;

        /// <summary>UI 창에 표시될 보석 스프라이트.</summary>
        public Sprite UISprite;

        /// <summary>낙하 속도 배율. Board가 설정하며 외부에서 직접 수정하지 않습니다.</summary>
        [HideInInspector]
        public float SpeedMultiplier = 1.0f;

        // ──────────────────────────────────────────────────────────
        //  공개 상태 (읽기 전용)
        // ──────────────────────────────────────────────────────────

        /// <summary>보석의 색상/종류 번호. GemData.GemType 과 동일.</summary>
        public int GemType;

        /// <summary>
        /// SmallBomb(2×2) 폭발 시 OnGemExploded 이벤트가 발화되는 횟수.
        /// 기본값 1(일반 보석). SmallBomb 소스 젬은 2로 설정되어 배달 횟수를 2배로 만듭니다.
        /// </summary>
        public int ExplodeCount = 1;

        /// <summary>
        /// 재료 태그(BlockTag), 음식 효과음(FoodSfx) 등 타이쿤 시스템에 필요한 데이터를 담습니다.
        /// HallManager가 OnGemExploded 이벤트를 받을 때 gem.Data.BlockTag 로 재료를 식별합니다.
        /// Setup()에서 설정되며 외부에서는 읽기만 가능합니다.
        /// </summary>
        public GemData Data { get; private set; }

        /// <summary>현재 가입된 매치. null 이면 매치 미소속.</summary>
        public Match CurrentMatch = null;

        // ──────────────────────────────────────────────────────────
        //  프로퍼티
        // ──────────────────────────────────────────────────────────

        public bool CanMove => m_CanMove && m_CurrentState is State.Still;
        /// <summary>State에 관계없이 이 젬이 이동 가능한 종류인지 반환합니다 (Crate 등 고정 젬 판별용).</summary>
        public bool IsMovable => m_CanMove;
        public Vector3Int CurrentIndex => m_CurrentIndex;
        public State CurrentState => m_CurrentState;
        public bool Usable => m_Usable;
        public bool Used => m_Used;
        public float FallTime => m_FallTime;
        public int HitPoint => m_HitPoints;

        // ── 얼음 상태 ──
        /// <summary>이 보석 위에 얼음 블록이 덮여 있으면 true.</summary>
        public bool IsFrozen => FrozenBy != null;
        /// <summary>현재 이 보석을 덮고 있는 IceBlock 참조. 없으면 null.</summary>
        public IceBlock FrozenBy { get; private set; }

        /// <summary>IceBlock이 부착될 때 호출합니다.</summary>
        public void SetFrozen(IceBlock ice) => FrozenBy = ice;
        /// <summary>IceBlock이 깨질 때 호출합니다.</summary>
        public void ClearFrozen() => FrozenBy = null;

        // ──────────────────────────────────────────────────────────
        //  내부 필드
        // ──────────────────────────────────────────────────────────

        protected bool m_Usable = false;
        protected bool m_Used = false;
        protected bool m_CanMove = true;
        protected Vector3Int m_CurrentIndex;
        protected int m_HitPoints = 1;

        private State m_CurrentState = State.Still;
        private float m_FallTime = 0f;

        // ================================================================
        //  공개 API
        // ================================================================

        /// <summary>보석을 그리드 좌표 startIdx에 배치합니다.</summary>
        public virtual void Init(Vector3Int startIdx)
        {
            m_CurrentIndex = startIdx;
        }

        /// <summary>
        /// GemData에서 스프라이트·타입 등을 읽어 비주얼을 초기화합니다.
        /// Board.NewGemAt()이 Instantiate 직후 호출합니다.
        /// </summary>
        public virtual void Setup(GemData data)
        {
            if (data == null) return;

            Data = data;
            GemType = data.GemType;
            UISprite = data.Sprite;

            var renderer = GetComponentInChildren<SpriteRenderer>();
            if (renderer != null)
                renderer.sprite = data.Sprite;
        }

        /// <summary>
        /// 보너스 젬 발동 시 호출됩니다.
        /// 기반 Gem은 아무 동작도 하지 않으며, BonusGem 파생 클래스에서 오버라이드합니다.
        /// </summary>
        /// <param name="swappedGem">스왑으로 발동된 경우 상대 젬, 더블클릭이면 null.</param>
        /// <param name="isBonus">인벤토리 아이템으로 직접 사용 시 true.</param>
        public virtual void Use(Gem swappedGem, bool isBonus = false) { }

        /// <summary>
        /// 데미지를 입힙니다.
        /// </summary>
        /// <returns>true = 체력 남음, false = 체력 0 → 파괴 대상.</returns>
        public virtual bool Damage(int damage)
        {
            m_HitPoints -= damage;
            return m_HitPoints > 0;
        }

        /// <summary>그리드 좌표를 newCell로 갱신합니다.</summary>
        public void MoveTo(Vector3Int newCell)
        {
            m_CurrentIndex = newCell;
        }

        /// <summary>파괴 직전 Disappearing 상태로 진입합니다.</summary>
        public void Destroyed()
        {
            m_CurrentState = State.Disappearing;
        }

        /// <summary>낙하를 시작합니다. 낙하 경과 시간을 0으로 초기화합니다.</summary>
        public void StartMoveTimer()
        {
            m_FallTime = 0f;
            m_CurrentState = State.Falling;
        }

        /// <summary>매 프레임 낙하 경과 시간을 accumulate합니다.</summary>
        public void TickMoveTimer(float deltaTime)
        {
            m_FallTime += deltaTime;
        }

        /// <summary>낙하가 완료되어 바운스 단계로 진입합니다.</summary>
        public void StopFalling()
        {
            m_CurrentState = State.Bouncing;
            m_FallTime = 0f;
        }

        /// <summary>바운스가 끝나고 완전 정지 상태가 됩니다.</summary>
        public void StopBouncing()
        {
            m_CurrentState = State.Still;
        }
    }
}
