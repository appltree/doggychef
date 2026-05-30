using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace DoggyChef
{
    public class CandyBooster : Booster
    {
        public int HealAmount = 3;
        public AudioClip TriggerSound;
        public GameObject HeartEffectPrefab;
        public Vector3 TargetOffset = new Vector3(0, 1.2f, 0);

        private const float FlyDuration = 0.45f;
        private const int CandySortingOrder = 30000;

        public override bool IsInstant => true;

        public override void Awake()
        {
            m_Usable = false;
        }

        public override void UseInstant()
        {
            PlayTriggerSound();
            HallManager.Instance?.FullHealAllOrderingCustomers();
        }

        public override IEnumerator InstantEffectRoutine(Vector3 startScreenPos, Canvas canvas, MonoBehaviour runner)
        {
            var hall = HallManager.Instance;
            var camera = Camera.main;

            if (hall == null || canvas == null || camera == null)
            {
                hall?.HealAllOrderingCustomers(HealAmount);
                yield break;
            }

            var flights = new List<Coroutine>();
            foreach (var table in hall.Tables)
            {
                var customer = table?.AssignedCustomer;
                if (!CanReceiveCandy(customer))
                    continue;

                flights.Add(runner.StartCoroutine(FlyCandyToCustomer(startScreenPos, customer, camera)));
            }

            foreach (var flight in flights)
                yield return flight;
        }

        private IEnumerator FlyCandyToCustomer(Vector3 startScreenPos, Customer customer, Camera camera)
        {
            Sprite sprite = ResolveCandySprite();
            if (sprite == null)
            {
                PlayTriggerSound();
                customer.FullHealHealth();
                yield break;
            }

            Vector3 startWorldPos = ScreenToWorld(startScreenPos, camera);
            Vector3 endWorldPos = customer.transform.position + TargetOffset;
            endWorldPos.z = 0f;

            var flyingCandy = CreateFlyingCandy(sprite, startWorldPos);
            yield return AnimateCandy(flyingCandy.transform, startWorldPos, endWorldPos);

            Destroy(flyingCandy);
            PlayTriggerSound();
            customer.FullHealHealth();
            SpawnHeartEffect(endWorldPos);
        }

        private Sprite ResolveCandySprite()
        {
            if (AnimSprite != null) return AnimSprite;
            if (UISprite != null) return UISprite;

            foreach (var image in Object.FindObjectsByType<Image>(FindObjectsInactive.Exclude))
            {
                if (image.sprite != null && image.sprite.name.ToLowerInvariant().Contains("candy"))
                    return image.sprite;
            }

            return null;
        }

        private static bool CanReceiveCandy(Customer customer)
        {
            return customer != null
                && (customer.State == Customer.CustomerState.Ordering
                    || customer.State == Customer.CustomerState.Angry);
        }

        private static Vector3 ScreenToWorld(Vector3 screenPos, Camera camera)
        {
            screenPos.z = Mathf.Abs(camera.transform.position.z);
            Vector3 worldPos = camera.ScreenToWorldPoint(screenPos);
            worldPos.z = 0f;
            return worldPos;
        }

        private static GameObject CreateFlyingCandy(Sprite sprite, Vector3 startWorldPos)
        {
            var flyingCandy = new GameObject("FlyingCandy");
            var renderer = flyingCandy.AddComponent<SpriteRenderer>();
            renderer.sprite = sprite;
            renderer.sortingLayerName = "Default";
            renderer.sortingOrder = CandySortingOrder;
            flyingCandy.transform.position = startWorldPos;
            return flyingCandy;
        }

        private static IEnumerator AnimateCandy(Transform candy, Vector3 start, Vector3 end)
        {
            float arcHeight = Mathf.Abs(end.y - start.y) * 0.4f + 2f;
            float elapsed = 0f;

            while (elapsed < FlyDuration)
            {
                elapsed += Time.deltaTime;
                float t = Mathf.Clamp01(elapsed / FlyDuration);
                float eased = t < 0.5f ? 2f * t * t : -1f + (4f - 2f * t) * t;

                Vector3 pos = Vector3.Lerp(start, end, eased);
                pos.y += Mathf.Sin(Mathf.PI * t) * arcHeight;
                candy.position = pos;
                candy.localScale = Vector3.one * (1f + 0.3f * Mathf.Sin(Mathf.PI * t));

                yield return null;
            }
        }

        private void SpawnHeartEffect(Vector3 position)
        {
            if (HeartEffectPrefab == null) return;

            var effect = Instantiate(HeartEffectPrefab, position, Quaternion.identity);
            Destroy(effect, 2f);
        }

        private void PlayTriggerSound()
        {
            if (TriggerSound != null)
                GameManager.Instance?.PlaySFXOneShot(TriggerSound);
        }
    }
}
