# GemHunterMatch 사용처 정리

갱신일: 2026-05-26

이 문서는 축소된 `Assets/GemHunterMatch` 폴더가 Unity 프로젝트 안에 남아 있는 이유를 정리한 문서입니다.
전체 원본은 Unity가 임포트하지 않는 `_Reference/GemHunterMatch`에 그대로 보관되어 있습니다.

## 정리 결과

SpriteAtlas를 모두 삭제한 뒤, `Assets/GemHunterMatch` 안에서 더 이상 필요 없는 파일을 삭제했습니다.

현재 유지 상태:

- `Assets/GemHunterMatch`: 파일 18개, 폴더 8개
- `_Reference/GemHunterMatch`: 파일 942개, 폴더 58개

`Assets/GemHunterMatch`에 남은 실제 에셋은 5개입니다.
나머지 파일은 Unity GUID 보존을 위한 `.meta` 파일입니다.

## 남긴 실제 에셋

- `Images/Gameplay pieces/HorizontalBonus.png`
- `Materials/Shader Graphs_Items.mat`
- `Prefabs/HorizontalBonusVisual.prefab`
- `ShaderGraphs/TileShader.shadergraph`
- `UI/UI Toolkit/UnityThemes/UnityDefaultRuntimeTheme.tss`

## 외부 사용처

### `Assets/MyGame/Prefabs/BonusGems/HorizontalBonus.prefab`

아래 프리팹을 직접 참조합니다.

- `Assets/GemHunterMatch/Prefabs/HorizontalBonusVisual.prefab`

참조 흐름:

- `HorizontalBonus.prefab`
- `GemHunterMatch/Prefabs/HorizontalBonusVisual.prefab`
- `GemHunterMatch/Images/Gameplay pieces/HorizontalBonus.png`
- `GemHunterMatch/Materials/Shader Graphs_Items.mat`
- `GemHunterMatch/ShaderGraphs/TileShader.shadergraph`

### `Assets/MyGame/UI/MyPannelSettings.asset`

아래 USS 테마 파일을 직접 참조합니다.

- `Assets/GemHunterMatch/UI/UI Toolkit/UnityThemes/UnityDefaultRuntimeTheme.tss`

## 내부 의존성

아래 항목들은 외부에서 직접 참조되지는 않지만, 유지된 에셋이 내부에서 다시 사용하므로 남겼습니다.

- `Prefabs/HorizontalBonusVisual.prefab`
  - `Images/Gameplay pieces/HorizontalBonus.png` 사용
  - `Materials/Shader Graphs_Items.mat` 사용

- `Materials/Shader Graphs_Items.mat`
  - `ShaderGraphs/TileShader.shadergraph` 사용

## 삭제된 기준

기존에는 SpriteAtlas가 `GemHunterMatch`의 여러 스프라이트와 폴더를 참조하고 있어서 많은 항목을 유지해야 했습니다.
이제 `Assets/Settings/Sprite Atlases`가 삭제되었으므로, SpriteAtlas 때문에 유지하던 배경 이미지, 보드 이미지, 폴더 단위 packable 관련 파일들은 삭제했습니다.

현재는 아래 두 외부 참조에서 시작되는 의존성만 유지합니다.

- `Assets/MyGame/Prefabs/BonusGems/HorizontalBonus.prefab`
- `Assets/MyGame/UI/MyPannelSettings.asset`

## 주의 사항

- `Assets/GemHunterMatch` 안의 파일을 더 줄이려면 먼저 위 두 외부 참조를 `MyGame` 쪽 에셋으로 교체해야 합니다.
- 특히 `HorizontalBonus.prefab`의 `VisualPrefab` 참조를 `Assets/MyGame/Prefabs/HorizontalBonusVisual.prefab` 같은 내부 프리팹으로 바꾸면 `GemHunterMatch/Prefabs/HorizontalBonusVisual.prefab` 관련 의존성을 제거할 수 있습니다.
- `MyPannelSettings.asset`의 `themeUss`를 다른 USS로 교체하면 `UnityDefaultRuntimeTheme.tss`도 제거할 수 있습니다.
