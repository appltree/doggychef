using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

namespace MyMatch3
{
    // [SaveSystem Integration]
    [DefaultExecutionOrder(-9999)]
    public class GameManager : MonoBehaviour
    {
        // ══════════════════════════════════════════════════════════
        //  [타이쿤 추가] 게임 상태 머신 (State Machine)
        // ══════════════════════════════════════════════════════════
        //  타이쿤 게임은 "이동 횟수 제한" 대신 "제한 시간" 으로 진행됩니다.
        //  게임 진행 상황을 명확한 상태로 구분하여 각 시스템이
        //  현재 어떤 단계인지 쉽게 파악할 수 있게 합니다.
        //
        //  상태 흐름:
        //  Intro → Running → Closing → Result
        //    │         │          │         │
        //    │     게임 진행    시간 종료  최종 결과
        //    │     타이머 작동  손님 정산  승/패 판정
        //  씬 로드
        //
        //  · Intro   : 씬 로드 후 초기화 단계 (현재는 바로 Running으로 전환)
        //  · Running : 타이머 작동, 손님 스폰, 보드 입력 활성화
        //  · Closing : 타이머 만료. 새 손님 스폰 중단, 현재 손님 정산 대기
        //  · Result  : 모든 테이블이 비워짐. 결과 화면 표시
        // ══════════════════════════════════════════════════════════
        public enum GameState { Intro, Running, Closing, Result }

        // 현재 게임 상태 (읽기 전용. 내부 SetState()로만 변경)
        public GameState CurrentState { get; private set; } = GameState.Intro;

        // 상태가 바뀔 때마다 발사되는 이벤트.
        // HallManager, LevelData, UIHandler 등이 구독하여 상태별 처리를 합니다.
        public static event Action<GameState> OnStateChanged;

        // 상태를 변경하고 이벤트를 발사하는 유일한 메서드.
        // 같은 상태를 중복 설정하면 이벤트가 발사되지 않습니다(Guard).
        private void SetState(GameState next)
        {
            if (CurrentState == next) return;   // 이미 같은 상태면 무시
            CurrentState = next;
            OnStateChanged?.Invoke(next);       // 구독자들에게 상태 변경 알림
        }

        // ──── 상태 전환 트리거 핸들러 (이벤트 구독/해제용 명시적 메서드) ────
        // LevelData.OnTimerEnded  → Closing 전환
        // HallManager.OnAllTablesCleared → Result 전환
        private void HandleTimerEnded() => SetState(GameState.Closing);
        private void HandleAllTablesCleared() => SetState(GameState.Result);

        private static bool s_IsShuttingDown = false;

        public static GameManager Instance { get; private set; }

        public static bool IsShuttingDown() => s_IsShuttingDown;

        // ──── 보너스 아이템 관리 ────
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

        // ──── 인스펙터 필드 ────
        public Board Board;
        public InputAction ClickAction;
        public InputAction ClickPosition;
        public GameSettings Settings;

        // ──── 게임 상태 ────
        // ──── 데이터 ────
        public List<BonusItemEntry> BonusItems = new();
        public VFXPoolSystem PoolSystem { get; private set; } = new();




        // ──── 오디오 ────
        // 효과음이 끊기지 않도록 16개를 돌려쓰는 풀(대기열)
        private Queue<AudioSource> m_SFXSourceQueue = new();
        // Hall/UI의 "반드시 재생" 효과음용 — PlayOneShot으로 절대 끊기지 않음
        private AudioSource m_OneShotSource;

        // ──── 세이브/로드 데이터 ────
        private UserPersistentData m_UserData = new();
        public UserPersistentData UserData => m_UserData;


        // 하위 호환성 (기본 GameManager 속성들과 동기화)
        public SoundData Volumes => new SoundData
        {
            MainVolume = m_UserData.MainVolume,
            MusicVolume = m_UserData.MusicVolume,
            SFXVolume = m_UserData.SFXVolume
        };
        private AudioSource MusicSourceActive;
        private AudioSource MusicSourceBackground;


        // ──── 기타 ────
        private GameObject m_BonusModePrefab;

        private void Awake()
        {
            if (Instance == this) return;

            if (Instance == null)
            {
                Instance = this;
                DontDestroyOnLoad(gameObject);

                ClickAction.Enable();
                ClickPosition.Enable();

                // 크로스페이드를 위한 배경음악 소스 2개 생성
                MusicSourceActive = Instantiate(Settings.SoundSettings.MusicSourcePrefab, transform);
                MusicSourceBackground = Instantiate(Settings.SoundSettings.MusicSourcePrefab, transform);
                MusicSourceActive.volume = 1.0f;
                MusicSourceBackground.volume = 0.0f;
                // SFX 소스 16개 풀 생성 (프리팹 기준)
                for (int i = 0; i < 16; ++i)
                {
                    var sourceInst = Instantiate(Settings.SoundSettings.SFXSourcePrefab, transform);
                    m_SFXSourceQueue.Enqueue(sourceInst);
                }
                // Hall/UI "반드시 재생" 전용 소스 — SFXSourcePrefab과 동일한 MixerGroup 사용
                m_OneShotSource = Instantiate(Settings.SoundSettings.SFXSourcePrefab, transform);
                // 데이터 불러오기 (사운드 및 부스터 잔여량 등)
                LoadAll();

                // 보너스 모드 이펙트 프리팹
                if (Settings != null && Settings.VisualSettings.BonusModePrefab != null)
                {
                    m_BonusModePrefab = Instantiate(Settings.VisualSettings.BonusModePrefab);
                    m_BonusModePrefab.SetActive(false);
                }

                GetReferences();

                // 외부 시스템이 상태 전환을 요청할 때 사용하는 이벤트 구독.
                // 실제 SetState()는 GameManager 내부에서만 호출합니다.
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
            // 배경음악 크로스페이드 (서서히 볼륨 변환)
            if (MusicSourceActive != null && MusicSourceBackground != null && MusicSourceActive.volume < 1.0f)
            {
                MusicSourceActive.volume = Mathf.MoveTowards(MusicSourceActive.volume, 1.0f, Time.deltaTime * 0.5f);
                MusicSourceBackground.volume = Mathf.MoveTowards(MusicSourceBackground.volume, 0.0f, Time.deltaTime * 0.5f);
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

        void GetReferences()
        {
            Board = FindFirstObjectByType<Board>();
        }

        public void StartLevel()
        {
            GetReferences();


            // [타이쿤 추가] 초기화 완료 → Running 상태로 전환
            // LevelData.Awake()에서 StartLevel()을 호출하므로,
            // 씬 로드 → LevelData 초기화 → Running 상태 시작 순서로 진행됩니다.
            // Running이 되면 LevelData의 타이머가 작동하고,
            // HallManager가 손님 스폰을 시작합니다.
            SetState(GameState.Running);
        }

        // ================================================================
        //  오디오
        // ================================================================
        /// <summary>
        /// 효과음을 재생합니다. AudioSource를 풀에서 꺼내 재생 후 다시 넣습니다.
        /// </summary>
        public AudioSource PlaySFX(AudioClip clip)
        {
            if (clip == null) return null;

            var source = m_SFXSourceQueue.Dequeue();
            m_SFXSourceQueue.Enqueue(source);

            source.clip = clip;
            source.Play();

            return source;
        }

        /// <summary>
        /// Hall/UI 효과음처럼 반드시 끝까지 재생되어야 하는 소리에 사용합니다.
        /// PlayOneShot은 기존 재생을 끊지 않고 레이어로 쌓습니다.
        /// </summary>
        public void PlaySFXOneShot(AudioClip clip)
        {
            if (clip == null) return;
            m_OneShotSource.PlayOneShot(clip);
        }

        // ================================================================
        //  게임 상태 관리
        // ================================================================
        public void ChangeCoins(int amount)
        {
            m_UserData.Coins += amount;
            if (m_UserData.Coins < 0) m_UserData.Coins = 0;

            if (UIHandler.Instance != null)
            {
                UIHandler.Instance.UpdateTopBarData();
            }
            SaveAll(); // 재화 변경 시 즉시 저장
        }

        public int Coins => m_UserData.Coins;
        public int Stars => m_UserData.Stars;
        public int Lives => m_UserData.Lives;

        public void WinStar()
        {
            m_UserData.Stars += 1;
            SaveAll();
        }

        public void AddLive(int amount)
        {
            m_UserData.Lives += amount;
            SaveAll();
        }

        public void LoseLife()
        {
            m_UserData.Lives -= 1;
            SaveAll();
        }

        // ================================================================
        //  보너스 아이템
        // ================================================================
        public void AddBonusItem(Booster item)
        {
            var existingItem = BonusItems.Find(entry => entry.Item == item);

            if (existingItem != null)
            {
                existingItem.Amount += 1;
            }
            else
            {
                BonusItems.Add(new BonusItemEntry()
                {
                    Amount = 1,
                    Item = item
                });
            }

            // [Save] 변경된 수량 저장
            m_UserData.SetBoosterAmount(item.name, existingItem?.Amount ?? 1);
            SaveAll();
        }

        public void ActivateBonusItem(Booster item)
        {
            if (LevelData.Instance != null)
                LevelData.Instance.DarkenBackground(item != null);
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
            if (LevelData.Instance != null)
                LevelData.Instance.DarkenBackground(false);

            if (BoosterPanel.Instance != null)
            {
                // 던지기 연출 시도 — 연출이 있으면 폭발은 착지 후 발동
                Vector3 worldPos = Board.Grid.GetCellCenterWorld(cell);
                bool animated = BoosterPanel.Instance.TryThrowAnim(existingItem.Item, worldPos, () =>
                {
                    existingItem.Item.MoveTo(cell);
                    existingItem.Item.Use(null, true);
                });

                if (!animated)
                {
                    // 연출 없음 — 즉시 발동
                    existingItem.Item.MoveTo(cell);
                    existingItem.Item.Use(null, true);
                    BoosterPanel.Instance.DeselectAll();
                    BoosterPanel.Instance.UpdateAllSlots();
                    BoosterPanel.Instance.BeginCooldownForItem(existingItem.Item);
                }
                else
                {
                    // 개수는 즉시 갱신, UI 선택 해제는 ThrowCoroutine 완료 후 처리
                    BoosterPanel.Instance.UpdateAllSlots();
                }
            }
            else
            {
                // Canvas BoosterPanel 없음 — 즉시 발동 후 UIHandler로 폴백
                existingItem.Item.MoveTo(cell);
                existingItem.Item.Use(null, true);
                if (UIHandler.Instance != null)
                {
                    UIHandler.Instance.DeselectBonusItem();
                    UIHandler.Instance.UpdateBottomBar();
                }
            }
        }

        // ================================================================
        //  오디오 - 음악 조절 및 설정 저장/불러오기
        // ================================================================

        // 배경음악을 부드럽게 전환(크로스페이드)하기 위해 두 소스를 교체합니다.
        public void SwitchMusic(AudioClip music)
        {
            if (music == null) return;

            MusicSourceBackground.clip = music;
            MusicSourceBackground.Play();

            // 두 변수의 레퍼런스를 스왑 (튜플 사용)
            (MusicSourceActive, MusicSourceBackground) = (MusicSourceBackground, MusicSourceActive);
        }

        // Mixer의 볼륨을 실제 믹서 파라미터로 전송합니다. 
        // 선형 볼륨(0~1)을 로그 스케일(-80dB ~ 0dB)로 변환합니다.
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

        public void SaveAll()
        {
            UserDataManager.Save("user_data.json", m_UserData);
        }

        public void LoadAll()
        {
            m_UserData = UserDataManager.Load<UserPersistentData>("user_data.json");
            UpdateVolumes();
        }


        // 구 버전 사운드 세이브 메서드 래핑
        public void SaveSoundData() => SaveAll();

    }
}
