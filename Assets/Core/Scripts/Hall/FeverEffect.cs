using System.Collections;
using UnityEngine;

namespace DoggyChef
{
    // ════════════════════════════════════════════════════════════════
    //  FeverEffect — 피버 모드 시각 효과
    // ════════════════════════════════════════════════════════════════
    //
    //  ■ 역할
    //  LevelData.OnFeverChanged 이벤트를 구독하여
    //  피버 발동/해제 시 화면에 시각 효과를 표시합니다.
    //
    //  ■ 효과 목록
    //  ① 오버레이 플래시 (flashOverlay): 피버 진입 시 강렬한 색상 플래시
    //  ② 오버레이 Pulse (pulseOverlay): 피버 지속 중 가장자리를 맥박처럼 표시
    //  ③ 보드 배경색 변경 (boardBackground): 피버 중 보드가 황금빛으로 변함
    //  ④ Camera Shake: 피버 진입 시 카메라를 짧게 흔듦
    //
    //  ■ 씬 셋업
    //  빈 GameObject에 이 컴포넌트를 부착합니다.
    //  Inspector에서 시각 요소들을 선택적으로 연결합니다. (모두 선택 사항)
    //    - FlashOverlay   : 전체 화면을 덮는 SpriteRenderer (흰색 또는 황금색)
    //    - PulseOverlay   : 가장자리 테두리 SpriteRenderer
    //    - BoardBackground: 보드 뒤쪽 SpriteRenderer
    //    - MainCamera     : 셰이크에 사용할 카메라 (null이면 Camera.main 사용)
    // ════════════════════════════════════════════════════════════════
    public class FeverEffect : MonoBehaviour
    {
        // ──────────────────────────────────────────────────────────
        //  Inspector 연결 필드
        // ──────────────────────────────────────────────────────────
        [Header("오버레이 렌더러 (선택)")]

        [Tooltip("피버 진입 시 플래시 효과를 표시할 SpriteRenderer.\n흰색/황금색 전체화면 스프라이트를 연결하세요.")]
        public SpriteRenderer FlashOverlay;

        [Tooltip("피버 지속 중 가장자리 맥박 효과를 표시할 SpriteRenderer.")]
        public SpriteRenderer PulseOverlay;

        [Tooltip("피버 중 황금빛으로 변할 보드 배경 SpriteRenderer.")]
        public SpriteRenderer BoardBackground;

        [Header("색상 설정")]

        [Tooltip("피버 플래시 색상. 기본값: 황금색")]
        public Color FeverFlashColor = new Color(1f, 0.85f, 0f, 0.7f);

        [Tooltip("피버 중 Pulse 색상 (테두리 맥박).")]
        public Color FeverPulseColor = new Color(1f, 0.6f, 0f, 0.4f);

        [Tooltip("피버 중 보드 배경색.")]
        public Color FeverBoardColor = new Color(1f, 0.9f, 0.2f, 1f);

        [Tooltip("기본 보드 배경색 (피버 해제 시 복원).")]
        public Color DefaultBoardColor = Color.white;

        [Header("애니메이션 파라미터")]

        [Tooltip("플래시 지속 시간(초). 피버 진입 시 이 시간 동안 플래시합니다.")]
        public float FlashDuration = 0.3f;

        [Tooltip("Pulse 한 주기 시간(초). 숫자가 작을수록 빠르게 맥박칩니다.")]
        public float PulsePeriod = 0.8f;

        [Header("카메라 셰이크 (선택)")]

        [Tooltip("피버 진입 시 사용할 카메라. null이면 Camera.main을 사용합니다.")]
        public Camera MainCamera;

        [Tooltip("셰이크 강도(Unity 유닛).")]
        public float ShakeMagnitude = 0.08f;

        [Tooltip("셰이크 지속 시간(초).")]
        public float ShakeDuration = 0.25f;

        // ──────────────────────────────────────────────────────────
        //  내부 상태
        // ──────────────────────────────────────────────────────────
        private Coroutine m_FlashRoutine;
        private Coroutine m_PulseRoutine;
        private Coroutine m_ShakeRoutine;
        private Vector3   m_CameraOrigin;

        // ================================================================
        //  유니티 생명주기
        // ================================================================

        private void Start()
        {
            // FlashOverlay와 PulseOverlay는 초기에 투명하게 시작합니다.
            SetAlpha(FlashOverlay, 0f);
            SetAlpha(PulseOverlay, 0f);

            // LevelData 이벤트 구독
            if (LevelData.Instance != null)
                LevelData.Instance.OnFeverChanged += OnFeverChanged;

            // 카메라 원본 위치 저장
            if (MainCamera == null) MainCamera = Camera.main;
            if (MainCamera != null) m_CameraOrigin = MainCamera.transform.localPosition;
        }

        private void OnDestroy()
        {
            if (LevelData.Instance != null)
                LevelData.Instance.OnFeverChanged -= OnFeverChanged;
        }

        // ================================================================
        //  이벤트 핸들러
        // ================================================================

        // LevelData.OnFeverChanged(bool isFever) 수신
        private void OnFeverChanged(bool isFever)
        {
            if (isFever)
                StartFever();
            else
                EndFever();
        }

        // ================================================================
        //  피버 진입
        // ================================================================

        private void StartFever()
        {
            // ① 플래시 (FlashOverlay)
            if (FlashOverlay != null)
            {
                if (m_FlashRoutine != null) StopCoroutine(m_FlashRoutine);
                m_FlashRoutine = StartCoroutine(FlashRoutine());
            }

            // ② Pulse 루프 시작 (PulseOverlay)
            if (PulseOverlay != null)
            {
                if (m_PulseRoutine != null) StopCoroutine(m_PulseRoutine);
                m_PulseRoutine = StartCoroutine(PulseRoutine());
            }

            // ③ 보드 배경색 변경
            if (BoardBackground != null)
                BoardBackground.color = FeverBoardColor;

            // ④ 카메라 셰이크
            if (MainCamera != null)
            {
                if (m_ShakeRoutine != null) StopCoroutine(m_ShakeRoutine);
                m_ShakeRoutine = StartCoroutine(CameraShakeRoutine());
            }
        }

        // ================================================================
        //  피버 해제
        // ================================================================

        private void EndFever()
        {
            // Pulse 중단
            if (m_PulseRoutine != null)
            {
                StopCoroutine(m_PulseRoutine);
                m_PulseRoutine = null;
            }
            SetAlpha(PulseOverlay, 0f);

            // 보드 배경색 복원
            if (BoardBackground != null)
                BoardBackground.color = DefaultBoardColor;
        }

        // ================================================================
        //  코루틴
        // ================================================================

        // 피버 진입 시 FlashOverlay를 빠르게 밝혔다가 사라지게 합니다.
        private IEnumerator FlashRoutine()
        {
            if (FlashOverlay == null) yield break;

            FlashOverlay.color = FeverFlashColor;

            // 투명 → 불투명 (0 → target alpha)
            float targetAlpha = FeverFlashColor.a;
            float elapsed = 0f;
            float halfDuration = FlashDuration * 0.5f;

            while (elapsed < halfDuration)
            {
                elapsed += Time.deltaTime;
                SetAlpha(FlashOverlay, Mathf.Lerp(0f, targetAlpha, elapsed / halfDuration));
                yield return null;
            }

            // 불투명 → 투명 (target alpha → 0)
            elapsed = 0f;
            while (elapsed < halfDuration)
            {
                elapsed += Time.deltaTime;
                SetAlpha(FlashOverlay, Mathf.Lerp(targetAlpha, 0f, elapsed / halfDuration));
                yield return null;
            }

            SetAlpha(FlashOverlay, 0f);
            m_FlashRoutine = null;
        }

        // 피버 지속 중 PulseOverlay를 맥박처럼 반복 점멸합니다.
        // EndFever()에서 StopCoroutine으로 중단합니다.
        private IEnumerator PulseRoutine()
        {
            if (PulseOverlay == null) yield break;

            PulseOverlay.color = FeverPulseColor;
            float maxAlpha = FeverPulseColor.a;

            while (true)
            {
                // 페이드 인
                float elapsed = 0f;
                float half = PulsePeriod * 0.5f;
                while (elapsed < half)
                {
                    elapsed += Time.deltaTime;
                    SetAlpha(PulseOverlay, Mathf.Lerp(0f, maxAlpha, elapsed / half));
                    yield return null;
                }

                // 페이드 아웃
                elapsed = 0f;
                while (elapsed < half)
                {
                    elapsed += Time.deltaTime;
                    SetAlpha(PulseOverlay, Mathf.Lerp(maxAlpha, 0f, elapsed / half));
                    yield return null;
                }
            }
        }

        // 카메라를 ShakeDuration 동안 무작위 방향으로 흔듭니다.
        private IEnumerator CameraShakeRoutine()
        {
            if (MainCamera == null) yield break;

            float elapsed = 0f;
            while (elapsed < ShakeDuration)
            {
                elapsed += Time.deltaTime;
                // 남은 시간 비율로 강도를 점점 줄입니다 (감쇠 셰이크).
                float strength = ShakeMagnitude * (1f - elapsed / ShakeDuration);
                MainCamera.transform.localPosition = m_CameraOrigin
                    + new Vector3(
                        Random.Range(-strength, strength),
                        Random.Range(-strength, strength),
                        0f);
                yield return null;
            }

            // 원래 위치로 복원
            MainCamera.transform.localPosition = m_CameraOrigin;
            m_ShakeRoutine = null;
        }

        // ──────────────────────────────────────────────────────────
        //  헬퍼
        // ──────────────────────────────────────────────────────────

        // SpriteRenderer의 Color.a만 변경합니다.
        private static void SetAlpha(SpriteRenderer sr, float a)
        {
            if (sr == null) return;
            Color c = sr.color;
            c.a = a;
            sr.color = c;
        }
    }
}
