# GemHunterMatch 사용처 정리

갱신일: 2026-05-26

`Assets/GemHunterMatch` 폴더는 삭제되었고, 원본 전체는 Unity가 임포트하지 않는 `_Reference/GemHunterMatch`에 보관되어 있습니다.

현재 `Assets/MyGame/Imported/GHM` 아래에는 GemHunterMatch 출처 에셋 중 일부만 남아 있습니다.

## 현재 남아 있는 에셋

- `Assets/MyGame/Imported/GHM/ShaderGraphs/TileShader.shadergraph`
- `Assets/MyGame/Imported/GHM/UI/UI Toolkit/UnityThemes/UnityDefaultRuntimeTheme.tss`

## 확인된 사용처

### `Assets/MyGame/UI/MyPannelSettings.asset`

아래 테마 파일을 참조합니다.

- `Assets/MyGame/Imported/GHM/UI/UI Toolkit/UnityThemes/UnityDefaultRuntimeTheme.tss`

## 이번 정리

- `Assets/MyGame/UI/BottomHUD.uxml` 삭제
- `Assets/MyGame/UI/BottomHUD.uxml.meta` 삭제
- `Assets/MyGame/UI/BottomHUD.uss` 삭제
- `Assets/MyGame/UI/BottomHUD.uss.meta` 삭제
- `Assets/MyGame/UI/MyGameUI.uxml` 안의 `BottomHUD` 템플릿/인스턴스 주석 제거
- `Assets/MyGame/Scripts/UIHandler.cs` 안의 구 UI Toolkit 부스터 바 처리 제거
- `Assets/MyGame/Scripts/GameManager.cs` 안의 `UIHandler.UpdateBottomBar()` 폴백 호출 제거

## 주의할 점

현재 `TopHUD.uss`, `EndScreen.uss`에는 `Assets/MyGame/Imported/GHM/Images/UI/...` 경로를 가리키는 문자열이 남아 있습니다. 다만 해당 이미지 폴더는 현재 프로젝트에 없습니다.

- `TopHUD.uss`는 현재 `MyGameUI.uxml`에서 템플릿 연결이 주석 처리되어 있어 직접 사용 중인지 재확인이 필요합니다.
- `EndScreen.uss`는 `MyGameUI.uxml`에서 사용 중이므로, 버튼 배경 이미지를 계속 쓸 거라면 이미지를 복원하거나 자체 UI 이미지로 교체해야 합니다.
