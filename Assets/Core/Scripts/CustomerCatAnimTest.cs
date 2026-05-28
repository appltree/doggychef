using Spine.Unity;
using UnityEngine;

namespace MyMatch3
{
    // ════════════════════════════════════════════════════════════════
    //  CustomerCatAnimTest — 고양이 손님 애니메이션 전환 테스트
    //  AnimTest 씬 전용. 빌드에서는 제외하세요.
    // ════════════════════════════════════════════════════════════════
    public class CustomerCatAnimTest : MonoBehaviour
    {
        [Header("참조")]
        [Tooltip("테스트할 SkeletonAnimation. 비워두면 씬에서 자동 검색합니다.")]
        [SerializeField] private SkeletonAnimation m_SkeletonAnimation;

        private static readonly (string key, string label)[] s_Anims =
        {
            ("walkin",  "① Walkin  (입장)"),
            ("order",   "② Order   (주문 대기)"),
            ("angry",   "③ Angry   (화남)"),
            ("eat",     "④ Eat     (식사)"),
            ("like",    "⑤ Like    (만족)"),
            ("walkout", "⑥ Walkout (퇴장)"),
        };

        private string m_CurrentAnim = "";

        // ──────────────────────────────────────────────────────────
        //  Unity 생명 주기
        // ──────────────────────────────────────────────────────────

        private void Start()
        {
            if (m_SkeletonAnimation == null)
                m_SkeletonAnimation = FindAnyObjectByType<SkeletonAnimation>();

            if (m_SkeletonAnimation == null)
                Debug.LogWarning("[AnimTest] SkeletonAnimation을 찾을 수 없습니다. 씬에 CustomerCat을 배치하세요.");
            else
                PlayAnim("walkin");
        }

        private void OnGUI()
        {
            var btnStyle = new GUIStyle(GUI.skin.button) { fontSize = 22, fontStyle = FontStyle.Bold };
            var labelStyle = new GUIStyle(GUI.skin.label) { fontSize = 20 };
            var headerStyle = new GUIStyle(GUI.skin.label) { fontSize = 26, fontStyle = FontStyle.Bold };

            const float btnW = 270f, btnH = 58f, pad = 10f;
            const float startX = 24f;
            float totalH = s_Anims.Length * (btnH + pad) - pad;
            float startY = Screen.height * 0.5f - totalH * 0.5f;

            GUI.Label(new Rect(startX, startY - 70f, 400f, 40f), "CustomerCat 애니메이션 테스트", headerStyle);
            GUI.Label(new Rect(startX, startY - 34f, 400f, 30f),
                $"현재: {(m_CurrentAnim == "" ? "없음" : m_CurrentAnim)}", labelStyle);

            for (int i = 0; i < s_Anims.Length; i++)
            {
                var (key, label) = s_Anims[i];
                bool isCurrent = m_CurrentAnim == key;
                GUI.backgroundColor = isCurrent ? new Color(1f, 0.9f, 0.2f) : Color.white;
                Rect r = new Rect(startX, startY + i * (btnH + pad), btnW, btnH);
                if (GUI.Button(r, label, btnStyle))
                    PlayAnim(key);
            }

            GUI.backgroundColor = Color.white;
        }

        // ──────────────────────────────────────────────────────────
        //  내부 구현
        // ──────────────────────────────────────────────────────────

        private void PlayAnim(string animName)
        {
            if (m_SkeletonAnimation == null) return;
            m_CurrentAnim = animName;
            m_SkeletonAnimation.AnimationState.SetAnimation(0, animName, true);
        }
    }
}
