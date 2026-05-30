# DoggyChef — Refactoring Plan

> SOLID · DRY 원칙 적용, 디렉터리 구조 개선, Unity 링크 보호
> 진행 상황: 체크박스([ ] → [x])로 추적

---

## Phase 1-C — 불필요 코드 제거 (위험도 ★☆☆)

- [x] `HallManager.cs` — 빈 `OnGameStateChanged` 메서드 + 구독 제거
- [x] `GameManager.cs` — 중복 래퍼 `SaveSoundData()` 제거
- [x] `LevelData.cs` — 주석된 LobbyPanel if 블록 제거 (L117)
- [x] `LevelData.cs` — 주석된 `ComputeCamera()` 블록 제거 (L224)
- [x] `CustomerCatAnimTest.cs` — 테스트 파일 삭제

---

## Phase 1-A — 네임스페이스 통일 (위험도 ★☆☆)

- [x] `StageScene/StageManager.cs` — `namespace MyMatch3` 추가, `using MyMatch3` 제거
- [x] `StageScene/StageCell.cs` — 동일
- [x] `StageScene/Map.cs` — 동일
- [x] `StageScene/StagePage.cs` — 동일
- [x] `StageScene/StagePanel.cs` — 동일

---

## Phase 1-B — 다중 클래스 파일 분리 (위험도 ★★☆)

- [x] `BonusGem/MatchShape.cs` — `MatchShape` 클래스를 별도 파일로 분리
- [x] `Booster/Booster.cs` — `Booster` 클래스를 별도 파일로 분리
- [x] `BonusGem/BonusGem.cs` — `BonusGem` 클래스만 남기도록 정리

---

## Phase 1-D — 코드 품질 수정 (위험도 ★★☆)

- [x] `Hall/Table.cs` — `Awake()` → `private void Awake()`
- [x] `Hall/Table.cs` — `Update()` → `private void Update()`
- [x] `Hall/Table.cs` — `SetState()` Guard 추가 (`if (State == next) return;`)
- [ ] `Hall/Table.cs` — `SpawnCoin()` 람다 이벤트 → 명시적 메서드로 교체 (CoinMoney 단발성 이벤트라 기능 문제 없음, 스타일 규칙 위반)
- [x] `Hall/Table.cs` — `sortingLayerID = 666422525` 상수화 (`VfxSortingLayerID`)
- [x] `Hall/Table.cs` — `FindAnyObjectByType<Machine>()` → `Machine.Instance`
- [x] `Hall/Table.cs` — `GetHasSideMenu()` → `HasSideMenu` 프로퍼티로 교체
- [x] `Hall/Table.cs` — `IsHandAnimationActive()` → `IsHandAnimationActive` 프로퍼티로 교체
- [x] `Hall/Machine.cs` — 싱글턴 패턴 추가 (Table에서 참조용)

---

## Phase 2-A — Scripts 폴더 구조 재편 (위험도 ★★★)

> 규칙: `.cs` 이동 시 `.cs.meta` 반드시 동시 이동. GUID 보존으로 씬/프리팹 링크 유지.

### Core/ 폴더 (핵심 시스템)
- [x] `GameManager.cs` + `.meta` → `Core/`
- [x] `LevelData.cs` + `.meta` → `Core/`
- [x] `UIHandler.cs` + `.meta` → `Core/`
- [x] `GameSettings.cs` + `.meta` → `Core/`

### Match3/ 폴더 (보드 시스템)
- [x] `Board.cs` + `.meta` → `Match3/`
- [ ] `Board.Input.cs` + `.meta` → `Match3/` (파일 미생성 — 향후 Board 분리 시)
- [x] `BoardCell.cs` + `.meta` → `Match3/`
- [x] `Gem.cs` + `.meta` → `Match3/`
- [x] `Match.cs` + `.meta` → `Match3/`

### Match3/Obstacles/ 폴더
- [x] `Obstacle.cs` + `.meta` → `Match3/Obstacles/`
- [x] `IceBlock.cs` + `.meta` → `Match3/Obstacles/`
- [x] `Crate.cs` + `.meta` → `Match3/Obstacles/`
- [x] `TieBlocker.cs` + `.meta` → `Match3/Obstacles/`

### Utility/ 폴더
- [x] `FrameRateController.cs` + `.meta` → `Utility/`
- [x] `VFXPoolSystem.cs` + `.meta` → `Utility/`
- [x] `UserDataManager.cs` + `.meta` → `Utility/`
- [x] `UserPersistentData.cs` + `.meta` → `Utility/`
- [x] `Common/PanelLetterbox.cs` + `.meta` → `Utility/`
- [x] `Common/` 빈 폴더 삭제

---

## Phase 3 — 코멘트 보강 (위험도 ★☆☆)

- [x] Board ↔ Hall 이벤트 경계 진입점 코멘트
- [x] `DefaultExecutionOrder` -9999 / +12000 이유 명시
- [x] VFX sortingLayer 상수 유래 코멘트
- [x] `m_FlyingTokenCount` 역할 (체력 드레인 일시정지) 코멘트
- [x] `FindBestTableForIngredient` 우선순위 전략 코멘트

---

## Unity 링크 보호 체크리스트

- [ ] Phase 2 완료 후 Unity Editor에서 Console 오류 없음 확인
- [ ] 씬 Game.unity 열었을 때 Missing Script 없음 확인
- [ ] 씬 Stage.unity 열었을 때 Missing Script 없음 확인
- [ ] Prefabs 폴더 전체 링크 이상 없음 확인

---

*생성일: 2026-05-30 | 대상: Assets/Core/Scripts/*
