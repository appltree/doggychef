# CCB 포맷 정밀 분석

이 문서는 `data/animal_horse/node_animal_horse.ccb`와 `references` 안의 CocosBuilder 원본 코드를 기준으로, 앞으로 CCB player를 만들기 위해 필요한 `.ccb` 포맷을 정리한 것이다.

## 참고한 파일

주요 근거 파일은 다음과 같다.

| 파일                                                                             | 참고한 내용                                                                                                                                                |
| -------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `data/animal_horse/node_animal_horse.ccb`                                        | 실제 분석 대상 CCB 파일. XML plist 구조, 노드 트리, 프로퍼티, 애니메이션 시퀀스, 키프레임 값을 확인했다.                                                   |
| `data/animal_horse/animal_horse.plist`                                           | `displayFrame`에서 참조하는 스프라이트 아틀라스 메타데이터.                                                                                                |
| `data/animal_horse/animal_horse.png`                                             | `animal_horse.plist`가 가리키는 실제 텍스처 이미지.                                                                                                        |
| `references/CocosBuilder-3.5.0/CocosBuilder/ccBuilder/CCBWriterInternal.m`       | CocosBuilder가 `.ccb` 문서를 저장할 때 node/property/animation을 어떤 dictionary 형태로 직렬화하는지 확인했다.                                             |
| `references/CocosBuilder-3.5.0/CocosBuilder/ccBuilder/CCBReaderInternal.m`       | CocosBuilder가 `.ccb` 문서를 다시 읽을 때 top-level 문서, nodeGraph, properties를 어떻게 해석하는지 확인했다.                                              |
| `references/CocosBuilder-3.5.0/CocosBuilder/ccBuilder/CCNode+NodeInfo.m`         | `animatedProperties`의 저장/복원 구조를 확인했다.                                                                                                          |
| `references/CocosBuilder-3.5.0/CocosBuilder/ccBuilder/SequencerKeyframe.h`       | keyframe type enum 값을 확인했다.                                                                                                                          |
| `references/CocosBuilder-3.5.0/CocosBuilder/ccBuilder/SequencerKeyframe.m`       | property type 문자열이 keyframe type으로 매핑되는 규칙과 keyframe serialization 구조를 확인했다.                                                           |
| `references/CocosBuilder-3.5.0/CocosBuilder/ccBuilder/SequencerNodeProperty.m`   | 시간별 애니메이션 값 샘플링 규칙을 확인했다.                                                                                                               |
| `references/CocosBuilder-3.5.0/CocosBuilder/ccBuilder/SequencerKeyframeEasing.h` | easing type enum 값을 확인했다.                                                                                                                            |
| `references/CocosBuilder-3.5.0/CocosBuilder/ccBuilder/SequencerKeyframeEasing.m` | easing 계산식을 확인했다.                                                                                                                                  |
| `references/CCB-X-Reader-master/Add-To-Your-Cocos2d-X-Project/CCBReader.cpp`     | 구버전/별도 CCB-X reader가 plist dictionary 기반 CCB를 Cocos2d-x 객체로 변환하는 방식을 참고했다. 다만 horse 파일의 직접 근거는 CocosBuilder 3.5.0 쪽이다. |

## 참고한 코드 위치별 핵심 근거

### `CCBWriterInternal.m`

참고 위치:

- `dictionaryFromCCObject`
- property 저장 루프
- children 저장 부분
- `animatedProperties` 저장 부분
- `customClass`, `memberVarAssignmentName`, `memberVarAssignmentType` 저장 부분

확인한 내용:

- 각 node는 `baseClass`, `properties`, `children` 키를 가진 dictionary로 저장된다.
- 각 property는 `name`, `type`, `value`를 가진 dictionary로 저장된다.
- keyframe이 있는 property는 `baseValue`도 저장될 수 있다.
- 기본값과 같은 property는 keyframe이 없으면 저장하지 않는다.
- children은 배열 순서대로 저장된다.
- animation은 node마다 `animatedProperties`로 저장된다.

### `CCBReaderInternal.m`

참고 위치:

- `nodeGraphFromDocumentDictionary`
- `nodeGraphFromDictionary`
- `setProp:ofType:toValue:forNode:parentSize:`

확인한 내용:

- top-level `fileType`은 `CocosBuilder`여야 한다.
- `fileVersion <= 2`는 legacy reader를 쓰고, 그보다 큰 버전은 현재 reader를 쓴다.
- `nodeGraph`를 시작점으로 재귀적으로 node를 생성한다.
- `properties` 배열을 읽어서 property type별 setter로 적용한다.
- `children` 배열은 순서대로 child node로 붙는다.
- `animatedProperties`는 node에 다시 로드된다.

### `CCNode+NodeInfo.m`

참고 위치:

- `serializeAnimatedProperties`
- `loadAnimatedPropertiesFromSerialization`

확인한 내용:

- `animatedProperties`는 sequence id를 문자열 key로 쓰는 dictionary다.
- sequence id 아래에 property 이름별 animation track이 들어간다.
- 각 animation track은 `SequencerNodeProperty` serialization이다.

구조:

```text
animatedProperties: {
  "<sequenceId>": {
    "<propertyName>": {
      name: "<propertyName>",
      type: <keyframeType>,
      keyframes: [...]
    }
  }
}
```

### `SequencerKeyframe.h`, `SequencerKeyframe.m`

참고 위치:

- keyframe type enum
- `keyframeTypeFromPropertyType`
- `serialization`
- `initWithSerialization`

확인한 내용:

- keyframe dictionary는 `value`, `type`, `name`, `time`, `easing`을 가진다.
- `Degrees`는 type `2`, `Position`은 type `3`, `Check`는 type `1`, `SpriteFrame`은 type `7`로 매핑된다.
- `Toggle`, `Undefined`, `SpriteFrame`은 finite-time interpolation을 지원하지 않는다.

### `SequencerNodeProperty.m`

참고 위치:

- `serialization`
- `initWithSerialization`
- `valueAtTime`

확인한 내용:

- animation track은 `name`, `type`, `keyframes`를 가진다.
- 시간별 값을 구할 때 keyframe 개수와 type에 따라 다르게 처리한다.
- `position`, `rotation` 등은 easing 적용 후 보간한다.
- `visible` 같은 toggle은 keyframe value를 그대로 보간하지 않고 keyframe 지점마다 상태를 토글한다.
- `displayFrame`은 보간하지 않고 다음 keyframe 전까지 이전 frame을 유지한다.

### `SequencerKeyframeEasing.h`, `SequencerKeyframeEasing.m`

참고 위치:

- easing type enum
- `easeValue`

확인한 내용:

- easing type은 `Instant`, `Linear`, `Cubic`, `Elastic`, `Bounce`, `Back` 계열로 정의된다.
- horse 파일에서는 확인한 범위상 거의 `type = 1`, 즉 linear easing이 사용된다.
- player 1차 구현은 linear와 instant만 먼저 구현해도 horse 재생에는 충분할 가능성이 높다.

## 1. 파일 종류

CocosBuilder에는 관련 파일 형식이 두 가지 있다.

| 확장자  | 의미                                                                          |
| ------- | ----------------------------------------------------------------------------- |
| `.ccb`  | CocosBuilder 편집 문서. plist 기반일 수 있다. horse 파일이 여기에 해당한다.   |
| `.ccbi` | publish/export된 런타임용 바이너리 파일. horse 파일은 여기에 해당하지 않는다. |

`node_animal_horse.ccb`의 시작 부분은 다음과 같다.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" ...>
<plist version="1.0">
<dict>
```

따라서 현재 만들 player는 우선 XML plist 기반 `.ccb` reader로 시작하는 것이 맞다. 바이너리 `.ccbi` reader는 별도 작업으로 분리하는 편이 좋다.

## 2. Top-Level 문서 구조

`node_animal_horse.ccb`에서 확인한 top-level key는 다음과 같다.

| Key                 | Type    | 의미                                                     |
| ------------------- | ------- | -------------------------------------------------------- |
| `fileType`          | string  | `CocosBuilder`여야 한다.                                 |
| `fileVersion`       | integer | horse 파일은 `4`다.                                      |
| `nodeGraph`         | dict    | 실제 렌더링할 노드 트리. 가장 중요하다.                  |
| `sequences`         | array   | 애니메이션 시퀀스 목록.                                  |
| `currentSequenceId` | integer | 에디터에서 선택된 시퀀스 id. 런타임 autoplay와는 별개다. |
| `currentResolution` | integer | 에디터 resolution index.                                 |
| `resolutions`       | array   | resolution 설정 정보.                                    |
| `guides`            | array   | 에디터 guide 정보. player에서는 무시 가능.               |
| `notes`             | array   | 에디터 note 정보. player에서는 무시 가능.                |
| `centeredOrigin`    | bool    | 에디터/stage 설정.                                       |
| `stageBorder`       | integer | 에디터/stage 설정.                                       |

player에서 우선 읽어야 할 것은 다음뿐이다.

1. `fileType`
2. `fileVersion`
3. `nodeGraph`
4. `sequences`

나머지는 에디터 메타데이터라서 1차 player에서는 무시해도 된다.

## 3. Node Graph 구조

각 node는 dictionary 하나로 표현된다.

| Key                       | Type       | 필수 여부   | 의미                                                     |
| ------------------------- | ---------- | ----------- | -------------------------------------------------------- |
| `baseClass`               | string     | 필수        | Cocos2d node class. 예: `CCNode`, `CCLayer`, `CCSprite`. |
| `properties`              | array      | 필수        | 이 node의 serialized property 목록.                      |
| `children`                | array      | 필수        | child node 목록. 배열 순서가 draw/z 순서다.              |
| `animatedProperties`      | dict       | 선택        | node별 animation track.                                  |
| `displayName`             | string     | 선택        | 에디터 표시 이름. 디버깅에 유용하다.                     |
| `customClass`             | string     | 대체로 있음 | 코드 연결용 class 이름.                                  |
| `memberVarAssignmentName` | string     | 대체로 있음 | 코드 연결용 변수 이름.                                   |
| `memberVarAssignmentType` | integer    | 대체로 있음 | 코드 연결 방식.                                          |
| `customProperties`        | array/dict | 선택        | 사용자 정의 property.                                    |
| `seqExpanded`             | bool       | 선택        | 에디터 UI 상태.                                          |
| `selected`                | bool       | 선택        | 에디터 선택 상태.                                        |
| `usesFlashSkew`           | bool       | 선택        | rotation/skew 호환 플래그.                               |

horse 파일의 node 통계:

| 항목         |  값 |
| ------------ | --: |
| 전체 node 수 |  59 |
| 최대 depth   |   3 |
| `CCNode`     |   1 |
| `CCLayer`    |   1 |
| `CCSprite`   |  57 |

구조상 root는 `CCNode`, 그 아래 주요 layer가 `CCLayerHorse`, 실제 말 파츠는 대부분 `CCSprite`로 구성되어 있다.

## 4. Property 구조

각 property는 다음 형태의 dictionary다.

| Key         | Type        | 필수 여부 | 의미                                                    |
| ----------- | ----------- | --------- | ------------------------------------------------------- |
| `name`      | string      | 필수      | property 이름. 예: `position`, `scale`, `displayFrame`. |
| `type`      | string      | 필수      | CocosBuilder property serializer type.                  |
| `value`     | plist value | 필수      | 실제 값. 형태는 `type`에 따라 다르다.                   |
| `baseValue` | plist value | 선택      | keyframe이 있는 property의 기본값.                      |
| `platform`  | string      | 선택      | platform-specific property 표시.                        |

주의할 점:

- `CCBWriterInternal.m`은 기본값과 같은 property를 저장하지 않을 수 있다.
- 따라서 player는 누락된 property에 대해 Cocos2d 기본값을 적용해야 한다.
- keyframe이 있는 property에는 `baseValue`가 붙을 수 있다.

## 5. horse 파일에서 확인한 property type

| Type          | 값 형태                             | 의미                             |
| ------------- | ----------------------------------- | -------------------------------- |
| `Position`    | `[x, y, positionType]`              | node position.                   |
| `Point`       | `[x, y]`                            | anchor point 등 2D point.        |
| `Size`        | `[width, height, sizeType]`         | content size.                    |
| `ScaleLock`   | `[scaleX, scaleY, lock, scaleType]` | node scale.                      |
| `Degrees`     | number                              | rotation degree.                 |
| `Check`       | bool                                | boolean property. 예: `visible`. |
| `SpriteFrame` | `[plistPath, frameName]`            | atlas plist와 frame 이름.        |
| `Flip`        | `[flipX, flipY]`                    | sprite 좌우/상하 반전.           |

horse 파일의 property 빈도:

| Property / Type                          | 개수 |
| ---------------------------------------- | ---: |
| `scale` / `ScaleLock`                    |   59 |
| `visible` / `Check`                      |   59 |
| `ignoreAnchorPointForPosition` / `Check` |   59 |
| `anchorPoint` / `Point`                  |   59 |
| `position` / `Position`                  |   58 |
| `displayFrame` / `SpriteFrame`           |   57 |
| `rotation` / `Degrees`                   |   34 |
| `flip` / `Flip`                          |   14 |
| layer input flags / `Check`              |    4 |
| `contentSize` / `Size`                   |    1 |

## 6. 예시 property

root `CCNode`:

```text
contentSize: Size [50, 50, 1]
anchorPoint: Point [0, 0]
scale: ScaleLock [1, 1, false, 0]
ignoreAnchorPointForPosition: Check false
visible: Check true
```

main layer `CCLayerHorse`:

```text
position: Position [0, 0, 0], baseValue [0, 0]
anchorPoint: Point [0.5, 0.5]
scale: ScaleLock [1, 1, false, 0]
rotation: Degrees 0, baseValue 0
ignoreAnchorPointForPosition: Check true
visible: Check true, baseValue 1
isTouchEnabled: Check true
isAccelerometerEnabled: Check false
isMouseEnabled: Check true
isKeyboardEnabled: Check false
```

예시 sprite:

```text
baseClass: CCSprite
displayName: CCSpriteLegDanceRight_0
position: Position [-776, 880, 0], baseValue [-776, 880]
anchorPoint: Point [0, 1]
scale: ScaleLock [1, 1, false, 0]
rotation: Degrees 0, baseValue 0
ignoreAnchorPointForPosition: Check false
visible: Check true, baseValue 1
displayFrame: SpriteFrame [animal/animal_horse.plist, animal_horse_leg_02.png]
flip: Flip [true, false]
```

## 7. SpriteFrame와 atlas 참조

`SpriteFrame` property 값은 2개짜리 배열이다.

```text
[spriteSheetFile, spriteFrameName]
```

horse 파일 예시:

```text
[animal/animal_horse.plist, animal_horse_leg_02.png]
[animal/animal_horse.plist, animal_horse_body.png]
[animal/animal_horse.plist, animal_horse_face.png]
```

실제 workspace 파일은 다음 위치에 있다.

```text
data/animal_horse/animal_horse.plist
data/animal_horse/animal_horse.png
```

하지만 CCB 안에서는 다음처럼 참조한다.

```text
animal/animal_horse.plist
```

따라서 player에는 asset path resolver가 필요하다. 이 샘플에서는 우선 다음 매핑을 넣으면 된다.

```text
animal/animal_horse.plist -> data/animal_horse/animal_horse.plist
```

## 8. Sequence 구조

top-level `sequences`는 animation timeline 목록이다.

| Key                 | Type    | 의미                                                              |
| ------------------- | ------- | ----------------------------------------------------------------- |
| `sequenceId`        | integer | animation track에서 참조하는 id.                                  |
| `name`              | string  | animation 이름.                                                   |
| `length`            | real    | 길이, 초 단위.                                                    |
| `resolution`        | real    | timeline FPS/grid. horse는 `30`.                                  |
| `autoPlay`          | bool    | 자동 재생 여부.                                                   |
| `chainedSequenceId` | integer | 재생 완료 후 이어질 sequence id. 자기 자신이면 loop로 볼 수 있다. |
| `offset`            | real    | 에디터 timeline view offset. player에서는 무시 가능.              |
| `position`          | real    | 에디터 playhead/view 상태. player에서는 무시 가능.                |
| `scale`             | real    | 에디터 timeline scale. player에서는 무시 가능.                    |

horse 파일의 sequence:

|  Id | Name      |         Length | AutoPlay | Chained Id |
| --: | --------- | -------------: | -------- | ---------: |
|   1 | `walkout` | `0.6666669846` | true     |          1 |
|   2 | `walkin`  | `0.6666669846` | false    |          2 |
|   3 | `order`   |          `4.0` | false    |          3 |
|   4 | `like`    | `1.3333333731` | false    |          4 |
|   5 | `angry`   | `0.6666669846` | false    |          5 |
|   6 | `eat`     | `0.6666669846` | false    |          6 |

모든 `chainedSequenceId`가 자기 자신을 가리키므로, 기본 동작은 각 animation loop로 보는 것이 자연스럽다.

## 9. Animated Properties 구조

`animatedProperties`는 각 node 안에 들어간다.

구조:

```text
animatedProperties: {
  "<sequenceId>": {
    "<propertyName>": {
      name: "<propertyName>",
      type: <keyframeType>,
      keyframes: [
        {
          name: "<propertyName>",
          type: <keyframeType>,
          time: <seconds>,
          value: <value>,
          easing: {
            type: <easingType>,
            opt: <optional>
          }
        }
      ]
    }
  }
}
```

horse 파일의 animated property 분포:

| Property / Type      | Track 수 | Keyframe 수 |
| -------------------- | -------: | ----------: |
| `visible` / `1`      |       94 |         214 |
| `position` / `3`     |       94 |         230 |
| `rotation` / `2`     |       56 |         183 |
| `displayFrame` / `7` |        1 |           1 |

## 10. Keyframe type enum

`SequencerKeyframe.h`에서 확인한 값이다.

|  값 | 이름         | 값 형태                                 |
| --: | ------------ | --------------------------------------- |
|   0 | Undefined    | 없음                                    |
|   1 | Toggle       | bool-like. horse에서는 `visible`에 사용 |
|   2 | Degrees      | number                                  |
|   3 | Position     | `[x, y]`                                |
|   4 | ScaleLock    | `[x, y]`                                |
|   5 | Byte         | integer 0-255                           |
|   6 | Color3       | `[r, g, b]`                             |
|   7 | SpriteFrame  | `[plistPath, frameName]`                |
|   8 | FloatXY      | `[x, y]`                                |
|   9 | SoundEffects | channel keyframe                        |
|  10 | Callbacks    | channel keyframe                        |

horse player 1차 구현에는 `1`, `2`, `3`, `7`이 중요하다.

## 11. Easing type enum

`SequencerKeyframeEasing.h`에서 확인한 값이다.

|  값 | 이름         |
| --: | ------------ |
|   0 | Instant      |
|   1 | Linear       |
|   2 | CubicIn      |
|   3 | CubicOut     |
|   4 | CubicInOut   |
|   5 | ElasticIn    |
|   6 | ElasticOut   |
|   7 | ElasticInOut |
|   8 | BounceIn     |
|   9 | BounceOut    |
|  10 | BounceInOut  |
|  11 | BackIn       |
|  12 | BackOut      |
|  13 | BackInOut    |

horse 파일에서는 확인한 keyframe들이 대부분 `type = 1`, 즉 linear easing이다.

player 구현 순서는 다음이 좋다.

1. `Instant`, `Linear` 먼저 구현
2. horse 렌더링 검증
3. 필요하면 `Cubic`, `Elastic`, `Bounce`, `Back` 추가

## 12. 시간별 animation sampling 규칙

`SequencerNodeProperty.m`의 `valueAtTime` 기준 규칙이다.

1. keyframe이 없으면 static property 값을 쓴다.
2. keyframe이 1개이고 toggle이 아니면 그 keyframe 값을 계속 쓴다.
3. keyframe이 1개이고 toggle이면 `time >= keyframe.time`일 때 true다.
4. 첫 keyframe 이전이면 첫 keyframe 값을 쓴다. 단 toggle은 false로 시작한다.
5. 마지막 keyframe 이후면 마지막 keyframe 값을 쓴다.
6. 두 keyframe 사이면 type에 따라 보간한다.

type별 처리:

| Type          | 처리 방식                     |
| ------------- | ----------------------------- |
| `Degrees`     | 숫자 보간                     |
| `Position`    | x, y 각각 보간                |
| `ScaleLock`   | x, y 각각 보간                |
| `Byte`        | 보간 후 round                 |
| `Color3`      | r, g, b 각각 보간 후 round    |
| `FloatXY`     | x, y 각각 보간                |
| `SpriteFrame` | 보간하지 않고 이전 frame 유지 |
| `Toggle`      | keyframe 시점마다 상태 반전   |

특히 `visible`은 plist에 bool `value`가 들어있지만, CocosBuilder의 `valueAtTime` 구현은 toggle track을 keyframe마다 상태가 바뀌는 방식으로 계산한다. 원본과 최대한 맞추려면 이 규칙을 따라야 한다.

## 13. 좌표계와 transform 주의점

CCB는 Cocos2d 기준이다.

- position은 Cocos2d 좌표다.
- anchorPoint는 normalized 값이다. 예: `[0.5, 0.5]`.
- rotation은 degree다.
- children 배열 순서가 draw order다.
- `flip`은 sprite를 anchor 기준으로 반전한다.
- `ignoreAnchorPointForPosition`은 Cocos2d에서 position과 anchor 적용 방식에 영향을 준다.

web canvas player를 만들 경우 내부 scene graph를 y-up으로 유지하고 마지막 draw 단계에서 canvas y-down으로 변환하는 방식이 안전하다.

기본 transform 개념:

```text
worldTransform =
  parentTransform
  * translate(position)
  * rotate(rotation)
  * scale(scale)
  * anchorOffset
```

anchor offset:

```text
anchorOffset = [-anchorX * width, -anchorY * height]
```

horse 파일은 position 값이 크거나 음수인 sprite가 많다. 따라서 character를 화면에 맞추기 위한 camera/stage transform도 필요하다.

## 14. Horse player 최소 구현 범위

`node_animal_horse.ccb`를 재생하기 위한 1차 구현 범위는 다음이다.

1. XML plist parser
2. `CCNode`, `CCLayer`, `CCSprite` node 생성
3. static property 적용
   - `position`
   - `anchorPoint`
   - `scale`
   - `rotation`
   - `visible`
   - `ignoreAnchorPointForPosition`
   - `displayFrame`
   - `flip`
   - `contentSize`
4. `animal_horse.plist` atlas parser
5. `animal_horse.png` atlas image loader
6. sequence 선택
   - id 기준
   - name 기준
7. keyframe playback
   - `visible`
   - `position`
   - `rotation`
   - `displayFrame`
8. easing
   - 우선 `Instant`, `Linear`
9. loop/chaining
   - `chainedSequenceId`가 자기 자신이면 loop

## 15. 구현 검증 체크리스트

reader/player 구현 후 다음 값이 맞는지 확인하면 된다.

| 검증 항목                        | 기대값                                               |
| -------------------------------- | ---------------------------------------------------- |
| `fileType`                       | `CocosBuilder`                                       |
| `fileVersion`                    | `4`                                                  |
| 전체 node 수                     | `59`                                                 |
| `CCSprite` 수                    | `57`                                                 |
| sequence 수                      | `6`                                                  |
| sequence names                   | `walkout`, `walkin`, `order`, `like`, `angry`, `eat` |
| autoplay sequence                | `walkout`                                            |
| 주요 atlas 참조                  | `animal/animal_horse.plist`                          |
| animated `visible` track 수      | 약 `94`                                              |
| animated `position` track 수     | 약 `94`                                              |
| animated `rotation` track 수     | 약 `56`                                              |
| animated `displayFrame` track 수 | 약 `1`                                               |

이 값이 크게 다르면 plist parser, dictionary/array 변환, node traversal 중 하나가 잘못됐을 가능성이 높다.
