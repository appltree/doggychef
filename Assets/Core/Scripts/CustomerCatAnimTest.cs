using Spine.Unity;
using UnityEngine;

namespace MyMatch3
{
    public class CustomerCatAnimTest : MonoBehaviour
    {
        [SerializeField] private SkeletonAnimation m_SkeletonAnimation;

        private static readonly (string key, string label)[] s_Animations =
        {
            ("walkin", "Walk In"),
            ("order", "Order"),
            ("angry", "Angry"),
            ("eat", "Eat"),
            ("like", "Like"),
            ("walkout", "Walk Out"),
        };

        private string m_CurrentAnimation = string.Empty;

        private void Start()
        {
            if (m_SkeletonAnimation == null)
                m_SkeletonAnimation = FindAnyObjectByType<SkeletonAnimation>();

            if (m_SkeletonAnimation == null)
            {
                Debug.LogWarning("[AnimTest] SkeletonAnimation not found.", this);
                return;
            }

            PlayAnimation("walkin");
        }

        private void OnGUI()
        {
            var buttonStyle = new GUIStyle(GUI.skin.button) { fontSize = 22, fontStyle = FontStyle.Bold };
            var labelStyle = new GUIStyle(GUI.skin.label) { fontSize = 20 };
            var headerStyle = new GUIStyle(GUI.skin.label) { fontSize = 26, fontStyle = FontStyle.Bold };

            const float buttonWidth = 270f;
            const float buttonHeight = 58f;
            const float padding = 10f;
            const float startX = 24f;

            float totalHeight = s_Animations.Length * (buttonHeight + padding) - padding;
            float startY = Screen.height * 0.5f - totalHeight * 0.5f;

            GUI.Label(new Rect(startX, startY - 70f, 400f, 40f), "Customer Animation Test", headerStyle);
            GUI.Label(new Rect(startX, startY - 34f, 400f, 30f),
                $"Current: {(string.IsNullOrEmpty(m_CurrentAnimation) ? "None" : m_CurrentAnimation)}", labelStyle);

            for (int i = 0; i < s_Animations.Length; i++)
            {
                var (key, label) = s_Animations[i];
                GUI.backgroundColor = m_CurrentAnimation == key ? new Color(1f, 0.9f, 0.2f) : Color.white;

                var rect = new Rect(startX, startY + i * (buttonHeight + padding), buttonWidth, buttonHeight);
                if (GUI.Button(rect, label, buttonStyle))
                    PlayAnimation(key);
            }

            GUI.backgroundColor = Color.white;
        }

        private void PlayAnimation(string animationName)
        {
            if (m_SkeletonAnimation == null) return;

            m_CurrentAnimation = animationName;
            m_SkeletonAnimation.AnimationState.SetAnimation(0, animationName, true);
        }
    }
}
