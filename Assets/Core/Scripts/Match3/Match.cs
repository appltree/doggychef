using System.Collections.Generic;
using UnityEngine;

namespace DoggyChef
{
    /// <summary>
    /// 하나의 '삭제 단위'를 나타냅니다.
    /// Board가 Match를 생성하고 MatchTicking 단계에서 소속 보석들을 순서대로 파괴합니다.
    ///
    /// ■ 생성 경로
    ///   - 일반 매칭  : Board.DoMatchCheck() → Board.CreateCustomMatch()
    ///   - 보너스 발동 : BonusGem.Use()     → Board.CreateCustomMatch()
    ///
    /// ■ ForcedDeletion
    ///   true 이면 DeletionTimer 없이 즉시 파괴 + 기본 팡 이펙트/사운드 생략.
    ///   보너스 젬이 강제 파괴할 때 사용합니다.
    /// </summary>
    public class Match
    {
        /// <summary>이 매치에 속한 보석들의 그리드 좌표 목록.</summary>
        public List<Vector3Int> MatchingGem = new();

        /// <summary>매치가 발생한 기준 셀 (보너스 젬을 여기에 생성합니다).</summary>
        public Vector3Int OriginPoint;

        /// <summary>삭제 애니메이션 타이머 (1.0f를 초과하면 실제 파괴).</summary>
        public float DeletionTimer = 0f;

        /// <summary>조건 충족 시 생성될 보너스 젬 프리팹 (없으면 null).</summary>
        public BonusGem SpawnedBonus = null;

        /// <summary>true 이면 타이머 없이 즉시 파괴 (보너스 젬 강제 삭제 모드).</summary>
        public bool ForcedDeletion = false;

        /// <summary>
        /// > 0 이면 gem 파괴는 즉시 하되, OnGemExploded(→ 재료 비행) 발화를
        /// 이 시간(초)만큼 지연합니다. ColorClean 이펙트 대기에 사용합니다.
        /// </summary>
        public float ExplodeDelay = 0f;

        /// <summary>이 매치에서 실제로 파괴된 보석 수 (점수 계산에 사용).</summary>
        public int DeletedCount = 0;

        // ──────────────────────────────────────────────────────────

        /// <summary>
        /// 보석을 이 매치에 추가합니다.
        /// 이미 다른 매치에 속한 보석은 중복 등록을 방지하기 위해 무시합니다.
        /// </summary>
        /// <param name="gem">추가할 보석.</param>
        public void AddGem(Gem gem)
        {
            if (gem.CurrentMatch != null) return;

            MatchingGem.Add(gem.CurrentIndex);
            gem.CurrentMatch = this;
        }
    }
}