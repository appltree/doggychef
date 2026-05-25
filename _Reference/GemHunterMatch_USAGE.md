# GemHunterMatch 사용처 정리

갱신일: 2026-05-26

`Assets/GemHunterMatch` 폴더는 삭제되었습니다.
현재 프로젝트에서 실제로 쓰는 GemHunterMatch 출처 에셋만 `Assets/MyGame/Imported/GHM` 아래로 이관했습니다.
전체 원본은 Unity가 임포트하지 않는 `_Reference/GemHunterMatch`에 그대로 보관되어 있습니다.

## 현재 상태

- 삭제됨: `Assets/GemHunterMatch`
- 이관 위치: `Assets/MyGame/Imported/GHM`
- 이관된 실제 에셋: 9개
- 이관 폴더 파일 수: 18개
  - 실제 에셋 9개
  - 대응 `.meta` 9개

Unity 참조는 GUID 기반이므로, 이관한 파일들은 원본 `.meta`의 GUID를 유지했습니다.
따라서 기존 프리팹, 머티리얼, USS, PanelSettings 참조가 새 위치에서도 이어집니다.

## 이관한 에셋

### 보너스 젬 비주얼

- `Assets/MyGame/Imported/GHM/Prefabs/HorizontalBonusVisual.prefab`
- `Assets/MyGame/Imported/GHM/Images/Gameplay pieces/HorizontalBonus.png`
- `Assets/MyGame/Imported/GHM/Materials/Shader Graphs_Items.mat`
- `Assets/MyGame/Imported/GHM/ShaderGraphs/TileShader.shadergraph`

### UI USS 이미지

- `Assets/MyGame/Imported/GHM/Images/UI/Sprite_Button_Blue.png`
- `Assets/MyGame/Imported/GHM/Images/UI/Sprite_Button_green.png`
- `Assets/MyGame/Imported/GHM/Images/UI/Sprite_UI_booster.png`
- `Assets/MyGame/Imported/GHM/Images/UI/Sprite_UI_booster_glow.png`

### UI Toolkit 테마

- `Assets/MyGame/Imported/GHM/UI/UI Toolkit/UnityThemes/UnityDefaultRuntimeTheme.tss`

## 외부 사용처

### `Assets/MyGame/Prefabs/BonusGems/HorizontalBonus.prefab`

아래 프리팹을 직접 참조합니다.

- `Assets/MyGame/Imported/GHM/Prefabs/HorizontalBonusVisual.prefab`

참조 흐름:

- `HorizontalBonus.prefab`
- `Imported/GHM/Prefabs/HorizontalBonusVisual.prefab`
- `Imported/GHM/Images/Gameplay pieces/HorizontalBonus.png`
- `Imported/GHM/Materials/Shader Graphs_Items.mat`
- `Imported/GHM/ShaderGraphs/TileShader.shadergraph`

### `Assets/MyGame/UI/MyPannelSettings.asset`

아래 USS 테마 파일을 직접 참조합니다.

- `Assets/MyGame/Imported/GHM/UI/UI Toolkit/UnityThemes/UnityDefaultRuntimeTheme.tss`

### `Assets/MyGame/UI/BottomHUD.uss`

아래 이미지를 직접 참조합니다.

- `Assets/MyGame/Imported/GHM/Images/UI/Sprite_UI_booster.png`
- `Assets/MyGame/Imported/GHM/Images/UI/Sprite_UI_booster_glow.png`

### `Assets/MyGame/UI/EndScreen.uss`

아래 이미지를 직접 참조합니다.

- `Assets/MyGame/Imported/GHM/Images/UI/Sprite_Button_Blue.png`
- `Assets/MyGame/Imported/GHM/Images/UI/Sprite_Button_green.png`

## 정리 내용

- `Assets/GemHunterMatch` 폴더와 `Assets/GemHunterMatch.meta`를 삭제했습니다.
- 기존 `project://database/Assets/GemHunterMatch/...` USS 경로는 `Assets/MyGame/Imported/GHM/...`로 변경했습니다.
- 이관된 `.meta` 파일의 `assetPath`도 새 위치 기준으로 변경했습니다.
- `Assets` 내부에서 `Assets/GemHunterMatch` 경로 문자열이 남아 있지 않은 것을 확인했습니다.
- `Assets` 내부 `.meta` GUID 중복이 없는 것을 확인했습니다.

## 다음에 더 줄이려면

- `HorizontalBonus.prefab`의 `VisualPrefab`을 완전히 `MyGame` 제작 에셋으로 교체하면 `Imported/GHM/Prefabs`, `Materials`, `ShaderGraphs`, `HorizontalBonus.png`를 제거할 수 있습니다.
- `BottomHUD.uss`, `EndScreen.uss`의 배경 이미지를 `MyGame` 자체 UI 이미지로 교체하면 `Imported/GHM/Images/UI`를 제거할 수 있습니다.
- `MyPannelSettings.asset`의 `themeUss`를 다른 USS로 교체하면 `UnityDefaultRuntimeTheme.tss`도 제거할 수 있습니다.
