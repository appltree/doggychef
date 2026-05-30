using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

namespace DoggyChef
{
    [DefaultExecutionOrder(-9999)]
    public class GameManager : MonoBehaviour
    {
        // ════════════════════════════════════════════════════════════════
        //  중첩 타입
        // ════════════════════════════════════════════════════════════════

        // 흐름: Intro → Running → Closing → Result
        //   Running : 타이머·스폰·입력 활성
        //   Closing : 타이머 종료, 손님 정산 대기
        //   Result  : 모든 테이블 Empty → 결과 표시
        public enum GameState { Intro, Running, Closing, Result }

        [Serializable]
        public class BonusItemEntry
        {
            public int Amount;
            public Booster Item;
        }

        [Serializable]
        public class SoundData
        {
            public float MainVolume = 1.0f;
            public float MusicVolume = 1.0f;
            public float SFXVolume = 1.0f;
        }

        // ════════════════════════════════════════════════════════════════
        //  Inspector 연결 필드
        // ════════════════════════════════════════════════════════════════

        public Board Board;
        public InputAction ClickAction;
        public InputAction ClickPosition;
        public GameSettings Settings;
        public List<BonusItemEntry> BonusItems = new();

        // ════════════════════════════════════════════════════════════════
        //  이벤트
        // ════════════════════════════════════════════════════════════════

        // 상태 변경 시 발화. HallManager·LevelData·UIHandler가 구독합니다.
        public static event Action<GameState> OnStateChanged;

        // ════════════════════════════════════════════════════════════════
        //  싱글턴
        // ════════════════════════════════════════════════════════════════

        public static GameManager Instance { get; private set; }

        // ════════════════════════════════════════════════════════════════
        //  public 런타임 상태
        // ════════════════════════════════════════════════════════════════

        // 현재 게임 상태. 내부 SetState()로만 변경됩니다.
        public GameState CurrentState { get; private set; } = GameState.Intro;

        public VFXPoolSystem PoolSystem { get; private set; } = new();

        public UserPersistentData UserData => m_UserData;

        public SoundData Volumes => new()
        {
            MainVolume = m_UserData.MainVolume,
            MusicVolume = m_UserData.MusicVolume,
            SFXVolume = m_UserData.SFXVolume
        };

        public int Coins => m_UserData.Coins;
        public int Stars => m_UserData.Stars;
        public int Lives => m_UserData.Lives;

        public static bool IsShuttingDown() => s_IsShuttingDown;

        // ════════════════════════════════════════════════════════════════
        //  private 런타임 상태
        // ════════════════════════════════════════════════════════════════

        private static bool s_IsShuttingDown = false;

        // SFX 소스 16개 풀(대기열) — 끊기지 않고 돌려씀
        private readonly Queue<AudioSource> m_SFXSourceQueue = new();
        // Hall/UI "반드시 끝까지 재생" 전용 소스
        private AudioSource m_OneShotSource;
        private AudioSource m_MusicSourceActive;
        private AudioSource m_MusicSourceBackground;

        private UserPersistentData m_UserData = new();
        private GameObject m_BonusModePrefab;

        // ════════════════════════════════════════════════════════════════
        //  Unity 생명 주기
        // ════════════════════════════════════════════════════════════════

        private void Awake()
        {
            if (Instance == this) return;

            if (Instance == null)
            {
                Instance = this;
                DontDestroyOnLoad(gameObject);

                ClickAction.Enable();
                ClickPosition.Enable();

                // 크로스페이드용 배경음악 소스 2개 생성
                m_MusicSourceActive = Instantiate(Settings.SoundSettings.MusicSourcePrefab, transform);
                m_MusicSourceBackground = Instantiate(Settings.SoundSettings.MusicSourcePrefab, transform);
                m_MusicSourceActive.volume = 1.0f;
                m_MusicSourceBackground.volume = 0.0f;

                // SFX 소스 16개 풀 생성
                for (int i = 0; i < 16; ++i)
                    m_SFXSourceQueue.Enqueue(Instantiate(Settings.SoundSettings.SFXSourcePrefab, transform));

                m_OneShotSource = Instantiate(Settings.SoundSettings.SFXSourcePrefab, transform);

                LoadAll();

                if (Settings != null && Settings.VisualSettings.BonusModePrefab != null)
                {
                    m_BonusModePrefab = Instantiate(Settings.VisualSettings.BonusModePrefab);
                    m_BonusModePrefab.SetActive(false);
                }

                GetReferences();

                LevelData.OnTimerEnded += HandleTimerEnded;
                HallManager.OnAllTablesCleared += HandleAllTablesCleared;
            }
            else
            {
                Destroy(gameObject);
            }
        }

        private void Update()
        {
            // 배경음악 크로스페이드
            if (m_MusicSourceActive != null && m_MusicSourceBackground != null &&
                m_MusicSourceActive.volume < 1.0f)
            {
                m_MusicSourceActive.volume = Mathf.MoveTowards(m_MusicSourceActive.volume, 1.0f, Time.deltaTime * 0.5f);
                m_MusicSourceBackground.volume = Mathf.MoveTowards(m_MusicSourceBackground.volume, 0.0f, Time.deltaTime * 0.5f);
            }
        }

        private void OnDestroy()
        {
            if (Instance == this)
            {
                s_IsShuttingDown = true;
                LevelData.OnTimerEnded -= HandleTimerEnded;
                HallManager.OnAllTablesCleared -= HandleAllTablesCleared;
            }
        }

        // ════════════════════════════════════════════════════════════════
        //  public API
        // ════════════════════════════════════════════════════════════════

        // [LevelData.Awake 호출] 씬 초기화 완료 → Running으로 전환합니다.
        public void StartLevel()
        {
            GetReferences();
            SetState(GameState.Running);
        }

        // ──────────────────────────────────────────────────────────
        //  오디오
        // ──────────────────────────────────────────────────────────

        // SFX 풀에서 소스를 꺼내 재생합니다.
        public AudioSource PlaySFX(AudioClip clip)
        {
            if (clip == null) return null;

            var source = m_SFXSourceQueue.Dequeue();
            m_SFXSourceQueue.Enqueue(source);
            source.clip = clip;
            source.Play();
            return source;
        }

        // 끊기지 않고 끝까지 재생해야 하는 Hall/UI 효과음에 사용합니다.
        public void PlaySFXOneShot(AudioClip clip)
        {
            if (clip == null) return;
            m_OneShotSource.PlayOneShot(clip);
        }

        // 두 AudioSource를 교체하여 배경음악을 크로스페이드합니다.
        public void SwitchMusic(AudioClip music)
        {
            if (music == null) return;
            m_MusicSourceBackground.clip = music;
            m_MusicSourceBackground.Play();
            (m_MusicSourceActive, m_MusicSourceBackground) = (m_MusicSourceBackground, m_MusicSourceActive);
        }

        // 선형 볼륨(0~1) → 로그 스케일(-80dB~0dB)로 변환 후 Mixer에 전송합니다.
        public void UpdateVolumes()
        {
            if (Settings.SoundSettings.Mixer == null) return;

            float mainVol = Mathf.Log10(Mathf.Max(0.0001f, m_UserData.MainVolume)) * 30.0f;
            float sfxVol = Mathf.Log10(Mathf.Max(0.0001f, m_UserData.SFXVolume)) * 30.0f;
            float musicVol = Mathf.Log10(Mathf.Max(0.0001f, m_UserData.MusicVolume)) * 30.0f;

            Settings.SoundSettings.Mixer.SetFloat("MainVolume", mainVol);
            Settings.SoundSettings.Mixer.SetFloat("SFXVolume", sfxVol);
            Settings.SoundSettings.Mixer.SetFloat("MusicVolume", musicVol);
        }

        // ──────────────────────────────────────────────────────────
        //  재화 / 라이프
        // ──────────────────────────────────────────────────────────

        // 코인을 변경하고 UI와 저장을 즉시 반영합니다.
        public void ChangeCoins(int amount)
        {
            m_UserData.Coins += amount;
            if (m_UserData.Coins < 0) m_UserData.Coins = 0;

            UIHandler.Instance?.UpdateTopBarData();
            SaveAll();
        }

        // 별 1개를 획득하고 저장합니다.
        public void WinStar() { m_UserData.Stars += 1; SaveAll(); }
        // 라이프를 amount만큼 추가하고 저장합니다.
        public void AddLive(int amount) { m_UserData.Lives += amount; SaveAll(); }
        // 라이프를 1 차감하고 저장합니다.
        public void LoseLife() { m_UserData.Lives -= 1; SaveAll(); }

        // ──────────────────────────────────────────────────────────
        //  보너스 아이템
        // ──────────────────────────────────────────────────────────

        public void AddBonusItem(Booster item)
        {
            var existingItem = BonusItems.Find(entry => entry.Item == item);
            if (existingItem != null)
            {
                existingItem.Amount += 1;
            }
            else
            {
                BonusItems.Add(new() { Amount = 1, Item = item });
            }

            m_UserData.SetBoosterAmount(item.name, existingItem?.Amount ?? 1);
            SaveAll();
        }

        // [BoosterPanel 호출] 보드 클릭형 부스터를 활성화하거나 해제합니다.
        public void ActivateBonusItem(Booster item)
        {
            LevelData.Instance?.DarkenBackground(item != null);
            m_BonusModePrefab?.SetActive(item != null);
            Board.ActivateBonusItem(item);
        }

        public void UseBonusItem(Booster item, Vector3Int cell)
        {
            var existingItem = BonusItems.Find(entry => entry.Item == item);
            if (existingItem == null) return;

            existingItem.Amount -= 1;
            m_UserData.SetBoosterAmount(item.name, existingItem.Amount);
            SaveAll();

            m_BonusModePrefab?.SetActive(false);
            LevelData.Instance?.DarkenBackground(false);

            if (BoosterPanel.Instance != null)
            {
                Vector3 worldPos = Board.Grid.GetCellCenterWorld(cell);
                bool animated = BoosterPanel.Instance.TryThrowAnim(existingItem.Item, worldPos, () =>
                {
                    existingItem.Item.MoveTo(cell);
                    existingItem.Item.Use(null, true);
                });

                if (!animated)
                {
                    existingItem.Item.MoveTo(cell);
                    existingItem.Item.Use(null, true);
                    BoosterPanel.Instance.DeselectAll();
                    BoosterPanel.Instance.UpdateAllSlots();
                    BoosterPanel.Instance.BeginCooldownForItem(existingItem.Item);
                }
                else
                {
                    BoosterPanel.Instance.UpdateAllSlots();
                }
            }
            else
            {
                existingItem.Item.MoveTo(cell);
                existingItem.Item.Use(null, true);
            }
        }

        // ──────────────────────────────────────────────────────────
        //  세이브 / 로드
        // ──────────────────────────────────────────────────────────

        // 유저 데이터를 파일에 저장합니다.
        public void SaveAll() => UserDataManager.Save("user_data.json", m_UserData);

        // 유저 데이터를 파일에서 불러오고 볼륨을 적용합니다.
        public void LoadAll()
        {
            m_UserData = UserDataManager.Load<UserPersistentData>("user_data.json");
            UpdateVolumes();
        }

        // ════════════════════════════════════════════════════════════════
        //  private 구현
        // ════════════════════════════════════════════════════════════════

        // 게임 상태를 변경하고 OnStateChanged를 발화합니다. 같은 상태면 무시합니다.
        private void SetState(GameState next)
        {
            if (CurrentState == next) return;
            CurrentState = next;
            OnStateChanged?.Invoke(next);
        }

        // LevelData.OnTimerEnded → Closing / HallManager.OnAllTablesCleared → Result
        private void HandleTimerEnded() => SetState(GameState.Closing);
        private void HandleAllTablesCleared() => SetState(GameState.Result);

        // Board 참조를 씬에서 탐색합니다.
        private void GetReferences() => Board = FindAnyObjectByType<Board>();
    }
}
