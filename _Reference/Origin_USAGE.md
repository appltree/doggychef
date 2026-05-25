# Origin 사용처 정리

갱신일: 2026-05-26

`Assets/Origin` 폴더는 삭제되었습니다.
현재 프로젝트에서 실제로 쓰는 Origin 출처 에셋만 `Assets/MyGame/Imported/Origin` 아래로 이관했습니다.
전체 원본은 Unity가 임포트하지 않는 `_Reference/Origin`에 그대로 보관되어 있습니다.

## 현재 상태

- 삭제됨: `Assets/Origin`
- 이관 위치: `Assets/MyGame/Imported/Origin`
- 이관된 실제 에셋: 54개
- 이관 폴더 파일 수: 108개
  - 실제 에셋 54개
  - 대응 `.meta` 54개
- `_Reference/Origin`: 파일 1553개, 폴더 115개

Unity 참조는 GUID 기반이므로, 이관한 파일들은 원본 `.meta`의 GUID를 유지했습니다.
따라서 기존 씬, 프리팹, 애니메이션, 머티리얼 참조가 새 위치에서도 이어집니다.

## 별도 처리한 항목

`_Reference/Origin/Scripts/GameScene/HeartGauge.cs`는 복사하지 않았습니다.
이미 `Assets/MyGame/Scripts/UI/HeartGauge.cs`에 MyGame 구조에 맞춘 버전이 있으므로, 기존 Origin `HeartGauge` GUID 참조를 MyGame `HeartGauge` GUID로 교체했습니다.

- 제거한 구 GUID: `9c13275ac5ad9494b8595069cf52a687`
- 교체한 MyGame GUID: `a3ac7a3186923884aba2373e2e89af9b`

Spine 관련 GUID는 `Assets/Spine`에 이미 존재하므로 `Origin`에서 다시 복사하지 않았습니다.
중복 복사하면 같은 타입/셰이더가 중복될 수 있어 제외했습니다.

## 이관한 에셋 범위

### 애니메이션

- `Core/Animations/Beam.anim`
- `Core/Animations/BeamEffect.controller`
- `Core/Animations/Explosion.anim`
- `Core/Animations/Explosion.controller`
- `Core/Animations/GoalEffect.controller`
- `Core/Animations/GoalEffect_goal.anim`
- `Core/Animations/GoalEffect_perfect.anim`
- `Core/Animations/Ice.controller`
- `Core/Animations/Ice_break.anim`
- `Core/Animations/Ice_idle.anim`
- `Core/Animations/LightBallExplosion.anim`
- `Core/Animations/LightBallExplosion.controller`
- `Core/Animations/Pang.anim`

### 오디오/폰트

- `Core/Audio/SFX/goal.mp3`
- `Core/Fonts/Maplestory Light SDF.asset`

### 보드/블록/이펙트 이미지

- `Core/Graphics/Blocks/Bomb/rainbow.png`
- `Core/Graphics/Blocks/Bomb/spatula.png`
- `Core/Graphics/Blocks/Boonsik/ddukbokki.png`
- `Core/Graphics/Board/border.png`
- `Core/Graphics/Board/shine.png`
- `Core/Graphics/Effects/beam.png`
- `Core/Graphics/Effects/block_ice.png`
- `Core/Graphics/Effects/explosion.png`
- `Core/Graphics/Effects/pang.png`

### 게임 UI 이미지

- `Core/Graphics/GameUI/GoldGauge/*`
- `Core/Graphics/GameUI/HeartGauge/*`
- `Core/Graphics/GameUI/TimeGauage/*`

### 홀/머신/사이드 메뉴 이미지

- `Core/Graphics/Hall/animal_hart.png`
- `Core/Graphics/Hall/block_count_bg.png`
- `Core/Graphics/Hall/Boonsik/shop_boonsik_curtain.png`
- `Core/Graphics/Hall/Boonsik/shop_ui_machine.png`
- `Core/Graphics/Hall/Boonsik/shop_ui_side.png`
- `Core/Graphics/Hall/game_ui_hand.png`
- `Core/Graphics/Hall/game_ui_machine_button.png`
- `Core/Graphics/UI/symbol-boonsik.png`

### 프리팹/셰이더

- `Core/Prefabs/Effects/GoalEffect.prefab`
- `Core/Shader/ShinyUI.mat`
- `Core/Shader/ShinyUI.shadergraph`

## 정리 내용

- `Assets/Origin` 폴더와 `Assets/Origin.meta`를 삭제했습니다.
- Origin에서만 존재하던 GUID 참조는 `Assets/MyGame/Imported/Origin`에 같은 GUID로 이관했습니다.
- 이관된 `.meta` 파일의 `assetPath`는 새 위치 기준으로 변경했습니다.
- `Assets` 내부에서 `Assets/Origin` 경로 문자열이 남아 있지 않은 것을 확인했습니다.
- `Assets` 내부에서 Origin `HeartGauge` 구 GUID가 남아 있지 않은 것을 확인했습니다.
- `Assets` 내부 `.meta` GUID 중복이 없는 것을 확인했습니다.

## 다음에 더 줄이려면

- `MyGame` 프리팹/씬이 이관된 이미지와 애니메이션을 직접 참조하고 있으므로, 완전히 자체 제작 에셋으로 교체하면 `Imported/Origin`을 더 줄일 수 있습니다.
- Spine은 이미 `Assets/Spine`에 있으므로 `Imported/Origin`으로 옮기지 않습니다.
