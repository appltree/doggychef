using UnityEngine;
using UnityEngine.UI;
using System.Collections;

namespace MyMatch3
{
    /// <summary>
    /// 캔디 부스터 (game_ui_btn_candy).
    ///
    /// ■ 동작
    ///   → 버튼을 누르면 캔디 스프라이트가 주문 중인 모든 손님에게 동시에 날아갑니다.
    ///   → 캔디 도착 시 하트 이펙트 재생 + 체력 회복.
    ///   → 보드 클릭 없이 바로 발동되는 즉시형(Instant) 부스터입니다.
    ///
    /// ■ 설정
    ///   Inspector에서 HealAmount / TriggerSound / HeartEffectPrefab을 설정하세요.
    ///   UISprite에 game_ui_btn_candy 스프라이트를 연결하면 하단 바에 아이콘이 표시됩니다.
    ///   AnimSprite에 날아가는 캔디 스프라이트(threematch_ui_candy 등)를 연결하세요.
    /// </summary>
    public class CandyBooster : Booster
    {
        [Tooltip("손님 체력 회복량. 기본값 3.")]
        public int HealAmount = 3;

        [Tooltip("발동 시 재생할 효과음 (없으면 생략)")]
        public AudioClip TriggerSound;

        [Tooltip("손님 도착 시 재생할 하트 이펙트 프리팹 (World Space). null이면 생략합니다.")]
        public GameObject HeartEffectPrefab;

        [Tooltip("캔디가 날아갈 목표 오프셋 (발바닥 기준 위쪽 거리). 보통 손님의 가슴~머리 높이로 설정.")]
        public Vector3 TargetOffset = new Vector3(0, 1.2f, 0);

        public override void Awake()
        {
            m_Usable = false;
        }

        public override bool IsInstant => true;

        /// <summary>연출 없이 즉시 발동하는 fallback (canvas 없는 환경 등).</summary>
        public override void UseInstant()
        {
            if (TriggerSound != null)
                GameManager.Instance?.PlaySFXOneShot(TriggerSound);

            HallManager.Instance?.FullHealAllOrderingCustomers();
        }

        /// <summary>
        /// 주문 중인 모든 손님에게 캔디 스프라이트를 동시에 날립니다.
        /// 각 캔디가 도착하면 체력을 회복하고 하트 이펙트를 재생합니다.
        /// </summary>
        public override IEnumerator InstantEffectRoutine(
            Vector3 startScreenPos, Canvas canvas, MonoBehaviour runner)
        {
            var hall = HallManager.Instance;
            if (hall == null || canvas == null || Camera.main == null)
            {
                HallManager.Instance?.HealAllOrderingCustomers(HealAmount);
                yield break;
            }

            // 주문 중인 모든 고객에게 캔디를 동시에 날립니다.
            var subCoroutines = new System.Collections.Generic.List<Coroutine>();
            foreach (var table in hall.Tables)
            {
                var c = table?.AssignedCustomer;
                if (c == null) continue;
                if (c.State != Customer.CustomerState.Ordering &&
                    c.State != Customer.CustomerState.Angry) continue;

                var co = runner.StartCoroutine(FlyCandyToCustomer(startScreenPos, c, canvas));
                subCoroutines.Add(co);
            }

            // 모든 비행 연출이 끝날 때까지 대기
            foreach (var co in subCoroutines)
                yield return co;
        }

        private IEnumerator FlyCandyToCustomer(Vector3 startScreenPos, Customer customer, Canvas canvas)
        {
            // 캔디 스프라이트: AnimSprite → UISprite 순으로 fallback
            Sprite sprite = AnimSprite ?? UISprite;
            if (sprite == null)
            {
                // Fallback: 인스펙터에 할당 안된 경우 씬의 UI에서 캔디 이미지를 찾아 사용
                var images = Object.FindObjectsOfType<UnityEngine.UI.Image>();
                foreach (var img in images)
                {
                    if (img.sprite != null && img.sprite.name.ToLower().Contains("candy"))
                    {
                        sprite = img.sprite;
                        break;
                    }
                }
            }

            if (sprite == null)
            {
                Debug.LogWarning("[CandyBooster] AnimSprite and UISprite are both null, and fallback failed. Healing instantly without flying effect.");
                if (TriggerSound != null)
                    GameManager.Instance?.PlaySFXOneShot(TriggerSound);
                customer.FullHealHealth();
                yield break;
            }

            // 버튼의 스크린 좌표를 월드 좌표(World Space)로 변환
            Vector3 screenPos = startScreenPos;
            screenPos.z = Mathf.Abs(Camera.main.transform.position.z);
            Vector3 startWorldPos = Camera.main.ScreenToWorldPoint(screenPos);
            startWorldPos.z = 0f;

            // 목표 지점: 손님 위치 + 오프셋 (가슴/머리 쪽으로)
            Vector3 endWorldPos = customer.transform.position + TargetOffset;
            endWorldPos.z = 0f;

            Debug.Log($"[CandyBooster] Flying candy from {startWorldPos} to {endWorldPos} for customer.");

            // ── 임시 캔디 월드 오브젝트 생성 ──
            var flyGO = new GameObject("FlyingCandy");
            var flySR = flyGO.AddComponent<SpriteRenderer>();
            flySR.sprite = sprite;
            flySR.sortingLayerName = "Default"; // Layer를 적절히 조절 가능
            flySR.sortingOrder = 30000;         // 위로 렌더링되게 높은 값 배정
            flyGO.transform.position = startWorldPos;

            // ── 포물선 비행 (0.45초) ──
            const float duration = 0.45f;
            float arcHeight = Mathf.Abs(endWorldPos.y - startWorldPos.y) * 0.4f + 2f;
            float elapsed = 0f;

            while (elapsed < duration)
            {
                elapsed += Time.deltaTime;
                float t = Mathf.Clamp01(elapsed / duration);
                float easedT = t < 0.5f ? 2f * t * t : -1f + (4f - 2f * t) * t;

                Vector3 pos = Vector3.Lerp(startWorldPos, endWorldPos, easedT);
                pos.y += Mathf.Sin(Mathf.PI * t) * arcHeight;
                flyGO.transform.position = pos;

                flyGO.transform.localScale = Vector3.one * (1f + 0.3f * Mathf.Sin(Mathf.PI * t));
                yield return null;
            }

            Destroy(flyGO);

            // ── 도착: 체력 회복 + 하트 이펙트 + 효과음 ──
            Debug.Log($"[CandyBooster] Candy reached the customer! Playing sound and healing.");
            if (TriggerSound != null)
                GameManager.Instance?.PlaySFXOneShot(TriggerSound);

            customer.FullHealHealth();

            if (HeartEffectPrefab != null)
            {
                // 하트 이펙트도 캔디가 도착한 지점(가슴/머리)에서 생성
                var effect = Instantiate(HeartEffectPrefab,
                    endWorldPos, Quaternion.identity);
                Destroy(effect, 2f);
            }
            else
            {
                Debug.LogWarning("[CandyBooster] HeartEffectPrefab is null! Cannot instantiate visual effect on arrival.");
            }
        }
    }
}
