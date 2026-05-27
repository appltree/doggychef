# CCB analysis reference

이 문서는 `tools/ccb-analyzer.ts`가 만든 `analysis` 폴더의 산출물을 사람이 분석하기 쉽게 읽는 방법을 정리한다.

원본 `.ccb` 포맷 자체는 `docs/ccb_format_analysis.md`를 기준으로 본다. 이 문서는 그 원본 포맷을 다시 설명하기보다, 원본에서 뽑아낸 가공 파일을 어떤 순서로 보면 되는지에 집중한다.

## 생성 방법

기본 horse 샘플은 다음 명령으로 다시 만들 수 있다.

```powershell
npm run analyze:ccb
```

`package.json` 기준 실제 실행은 다음과 같다.

```powershell
node tools/ccb-analyzer.mjs data/animal_horse/node_animal_horse.ccb analysis/animal_horse
```

다른 CCB 파일을 분석할 때는 입력 `.ccb`와 출력 폴더를 직접 넘긴다.

```powershell
node tools/ccb-analyzer.mjs data/animal_cat/node_animal_cat.ccb analysis/animal_cat
```

## 산출물 구성

현재 `analysis/animal_horse`에는 다음 파일이 있다.

| File             | 용도                                                                                                |
| ---------------- | --------------------------------------------------------------------------------------------------- |
| `node_tree.md`   | 노드 계층을 사람이 읽기 쉽게 펼친 문서. 부모-자식 관계, 대략적인 파츠 배치, 노드 id 확인에 쓴다.    |
| `sequences.csv`  | CCB animation sequence 목록. 동작 이름, 길이, loop 여부를 확인한다.                                 |
| `sprites.csv`    | sprite 파츠 목록. 이미지 프레임, 기본 위치, anchor, flip, 어떤 sequence에서 움직이는지 빠르게 본다. |
| `animations.csv` | animation track과 keyframe 상세 목록. 특정 파츠가 특정 동작에서 언제 어떤 값으로 바뀌는지 확인한다. |

보통은 `node_tree.md` -> `sequences.csv` -> `sprites.csv` -> `animations.csv` 순서로 보면 빠르다.

## 공통 식별자

여러 산출물은 같은 node 식별자를 공유한다.

| Column        | 의미                                                                                     |
| ------------- | ---------------------------------------------------------------------------------------- |
| `nodeId`      | analyzer가 node graph를 DFS 순서로 순회하며 붙인 내부 id. `node_tree.md`의 `#id`와 같다. |
| `path`        | node graph 안에서의 위치. root는 `0`, 첫 child는 `0.0`, 그 아래 첫 child는 `0.0.0`이다.  |
| `parentId`    | 부모 node의 `nodeId`. 비어 있으면 root다.                                                |
| `depth`       | tree 깊이. root는 `0`, root의 child는 `1`이다.                                           |
| `displayName` | CocosBuilder editor 표시 이름. 파츠 이름을 추정할 때 가장 유용하다.                      |

`nodeId`는 분석 산출물 안에서 cross reference를 하기 위한 값이다. 원본 CCB에 저장된 고유 id가 아니라 analyzer가 순서대로 붙인 값이므로, node tree가 바뀌면 달라질 수 있다.

## node_tree.md 읽기

`node_tree.md`는 전체 node graph를 트리 형태로 보여준다.

예시:

```text
- #20 | CCSprite | CCSpriteBody | frame=animal_horse_body.png | pos=[2, 62, 0] | rot=0 | visible=true
  animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible)
  - #21 | CCSprite | CCSpriteLegLeft | frame=animal_horse_horse_leg.png | pos=[17, 24, 0] | rot=0 | visible=true
```

읽는 방법:

- `#20`은 `nodeId`다.
- `CCSprite`는 CCB node class다.
- `CCSpriteBody`는 `displayName`이다.
- `frame=...`은 사용하는 sprite frame 이름이다.
- `pos`, `rot`, `visible`은 static property 값이다.
- `animations:` 줄은 sequence별로 이 노드에 어떤 animated property가 있는지 요약한다.
- 들여쓰기는 부모-자식 관계다. child의 position은 parent local 좌표로 해석해야 한다.

파츠가 어느 몸통이나 얼굴 아래 붙어 있는지 확인할 때는 이 파일이 제일 빠르다.

## sequences.csv 읽기

`sequences.csv`는 top-level `sequences` 배열을 CSV로 편 것이다.

| Column              | 의미                                                                 |
| ------------------- | -------------------------------------------------------------------- |
| `sequenceId`        | animation track에서 참조하는 id.                                     |
| `name`              | 동작 이름. 예: `walkout`, `walkin`, `order`, `like`, `angry`, `eat`. |
| `length`            | sequence 길이. 초 단위다.                                            |
| `resolution`        | timeline 기준 FPS/grid. horse는 `30`이다.                            |
| `autoPlay`          | CCB에서 자동 재생으로 표시된 sequence 여부.                          |
| `chainedSequenceId` | 재생 완료 후 이어질 sequence id. 자기 자신이면 loop로 보면 된다.     |
| `offset`            | editor timeline view 값. player 분석에서는 보통 무시한다.            |
| `position`          | editor playhead/view 값. player 분석에서는 보통 무시한다.            |
| `scale`             | editor timeline scale 값. player 분석에서는 보통 무시한다.           |

분석할 동작을 정할 때 먼저 이 파일에서 `sequenceId`를 확인한다. 예를 들어 `eat`은 `sequenceId=6`이다.

## sprites.csv 읽기

`sprites.csv`는 sprite 파츠를 한 줄에 하나씩 모은 요약표다. “어떤 이미지 파츠가 어디에 있고 어떤 동작에서 움직이는가”를 빠르게 보기 위한 파일이다.

| Column                  | 의미                                                                     |
| ----------------------- | ------------------------------------------------------------------------ |
| `nodeId`                | sprite node id. `node_tree.md`, `animations.csv`와 연결할 때 쓴다.       |
| `path`                  | node graph path.                                                         |
| `parentId`              | 부모 node id.                                                            |
| `depth`                 | tree 깊이.                                                               |
| `displayName`           | 파츠 이름.                                                               |
| `atlasPlist`            | `displayFrame`의 atlas plist 경로. 원본 CCB 안의 참조 문자열이다.        |
| `frameName`             | atlas 안의 sprite frame 이름. 실제 그려질 이미지 조각이다.               |
| `position`              | static `position` property 값. 보통 `[x, y, positionType]`이다.          |
| `positionBase`          | animated `position` track이 있을 때의 `baseValue`. 보통 `[x, y]`다.      |
| `anchorPoint`           | normalized anchor. 예: `[0.5, 0.5]`, `[0, 1]`.                           |
| `scale`                 | `ScaleLock` 값. `[scaleX, scaleY, lock, scaleType]` 형태다.              |
| `rotation`              | static rotation degree.                                                  |
| `rotationBase`          | animated `rotation` track이 있을 때의 `baseValue`.                       |
| `flipX`                 | sprite 좌우 반전 여부.                                                   |
| `flipY`                 | sprite 상하 반전 여부.                                                   |
| `visible`               | static visible 값.                                                       |
| `visibleBase`           | animated `visible` track이 있을 때의 `baseValue`.                        |
| `visibleSequences`      | 이 sprite의 `visible`이 animated 되는 sequence 목록. `id:name` 형식이다. |
| `positionSequences`     | 이 sprite의 `position`이 animated 되는 sequence 목록.                    |
| `rotationSequences`     | 이 sprite의 `rotation`이 animated 되는 sequence 목록.                    |
| `displayFrameSequences` | 이 sprite의 `displayFrame`이 animated 되는 sequence 목록.                |

`position`과 `positionBase`가 다를 수 있다. CCB property에는 현재 editor/static 값이 있고, animation track이 있는 property에는 별도의 base value가 붙을 수 있기 때문이다. 실제 재생 상태를 보려면 `animations.csv`의 keyframe 값을 함께 봐야 한다.

`visibleSequences`, `positionSequences`, `rotationSequences`, `displayFrameSequences`는 상세 keyframe 값이 아니라 “이 property가 어느 sequence에서 애니메이션 되는지”만 알려주는 요약이다.

## animations.csv 읽기

`animations.csv`는 node별, sequence별, property별 animation track을 한 줄로 펼친 상세표다.

| Column          | 의미                                                                           |
| --------------- | ------------------------------------------------------------------------------ | ------------------------ |
| `nodeId`        | animated property를 가진 node id.                                              |
| `path`          | node graph path.                                                               |
| `parentId`      | 부모 node id.                                                                  |
| `depth`         | tree 깊이.                                                                     |
| `baseClass`     | CCB node class. 예: `CCLayer`, `CCSprite`.                                     |
| `displayName`   | node 표시 이름.                                                                |
| `sequenceId`    | animation sequence id.                                                         |
| `sequenceName`  | animation sequence 이름.                                                       |
| `property`      | animated property 이름. 예: `position`, `rotation`, `visible`, `displayFrame`. |
| `keyframeType`  | CocosBuilder keyframe type enum 값.                                            |
| `keyframeCount` | keyframe 개수.                                                                 |
| `times`         | keyframe 시간 목록. `                                                          | `로 구분된다. 초 단위다. |
| `values`        | keyframe value 목록. `times`와 같은 순서로 대응한다.                           |
| `easingTypes`   | keyframe easing type 목록. `times`와 같은 순서로 대응한다.                     |
| `baseValue`     | 해당 property의 CCB `baseValue`. 없으면 빈 값이다.                             |

주요 `keyframeType` 값은 기존 포맷 문서의 keyframe type enum과 연결된다.

| keyframeType | 의미         | value 형태               |
| -----------: | ------------ | ------------------------ |
|          `1` | Toggle/Check | boolean                  |
|          `2` | Degrees      | number                   |
|          `3` | Position     | `[x, y]`                 |
|          `7` | SpriteFrame  | `[plistPath, frameName]` |

`times`, `values`, `easingTypes`는 같은 인덱스끼리 묶어서 읽는다.

예시:

```text
times: 0 | 0.1666666716337204 | 0.3333333432674408
values: [0, 18] | [0, 13] | [0, 18]
easingTypes: 1 | 1 | 1
```

이 경우 첫 keyframe은 `0초 -> [0, 18]`, 두 번째 keyframe은 `0.1666초 -> [0, 13]`이다.

## 분석 흐름

특정 동작을 분석할 때:

1. `sequences.csv`에서 동작 이름의 `sequenceId`를 찾는다.
2. `sprites.csv`에서 해당 sequence가 들어간 `visibleSequences`, `positionSequences`, `rotationSequences`, `displayFrameSequences`를 찾는다.
3. 관심 있는 파츠의 `nodeId`를 잡는다.
4. `node_tree.md`에서 그 node의 부모-자식 관계를 확인한다.
5. `animations.csv`에서 같은 `nodeId`와 `sequenceId`를 필터링해 keyframe 값을 확인한다.

특정 이미지 파츠를 분석할 때:

1. `sprites.csv`에서 `frameName` 또는 `displayName`으로 파츠를 찾는다.
2. `nodeId`, `parentId`, `position`, `anchorPoint`, `flipX`, `flipY`를 확인한다.
3. `node_tree.md`에서 parent 아래 어디에 붙는지 확인한다.
4. `animations.csv`에서 그 `nodeId`의 animated property를 확인한다.

특정 시간의 pose를 재구성할 때:

1. `sequences.csv`에서 sequence 길이를 확인하고 loop 여부를 본다.
2. `animations.csv`에서 해당 `sequenceId`의 track을 모두 모은다.
3. 각 node property별로 `baseValue` 또는 static value에서 시작한다.
4. `times`와 `values`를 기준으로 해당 시간의 값을 계산한다.
5. `position`, `rotation`은 보간 대상이고, `visible`, `displayFrame`은 보간하지 않는 값으로 처리한다.
6. `node_tree.md`의 부모-자식 순서대로 local transform을 누적한다.

## 주의할 점

- CSV의 `nodeId`는 analyzer가 만든 임시 id다. 원본 CCB의 안정적인 id라고 보면 안 된다.
- `position`은 대부분 `[x, y, positionType]`이고, keyframe/base value의 position은 보통 `[x, y]`다.
- child node의 position은 parent local 좌표다. 화면 좌표처럼 바로 비교하면 헷갈릴 수 있다.
- `children` 배열 순서가 draw order 판단에 중요하다. 이 순서는 `node_tree.md`의 같은 depth 내 나열 순서와 같다.
- `displayFrameSequences`가 비어 있으면 해당 sprite는 이미지 프레임을 바꾸지 않고 transform/visible만 바뀐다.
- `visible` track은 단순 숫자 보간이 아니라 toggle/check 성격이다. 자세한 재생 규칙은 `docs/ccb_format_analysis.md`의 keyframe playback 부분을 기준으로 본다.
- `easingTypes`가 대부분 `1`이면 linear easing으로 보면 된다. instant/linear 외 easing을 정확히 재현하려면 원본 CocosBuilder easing 규칙을 확인해야 한다.

## 원본 포맷 문서와 연결

이 분석 산출물은 다음 원본 CCB 개념을 사람이 보기 쉽게 펼친 것이다.

| 분석 파일        | 원본 CCB 개념                                                         |
| ---------------- | --------------------------------------------------------------------- |
| `node_tree.md`   | `nodeGraph`, `children`, node `properties`, `animatedProperties` 요약 |
| `sequences.csv`  | top-level `sequences`                                                 |
| `sprites.csv`    | `CCSprite` node와 `displayFrame`, transform 관련 static/base property |
| `animations.csv` | node별 `animatedProperties`, `SequencerNodeProperty`, keyframe 배열   |

원본 구조나 player 구현 규칙이 궁금하면 `docs/ccb_format_analysis.md`를 보고, 실제 파츠와 동작을 빠르게 찾고 싶으면 이 문서의 흐름대로 `analysis` 파일을 보면 된다.
