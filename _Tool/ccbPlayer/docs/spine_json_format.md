# CCB Export용 Spine JSON 형식 메모

## 참고 자료

- Spine JSON export/import 공식 문서: https://esotericsoftware.com/spine-json-format
- Spine 4 rotate timeline 필드 설명: https://uk.esotericsoftware.com/forum/d/17924-json-spine-3-to-4-change-for-bone-timeline-rotate/3
- 로컬 DragonBones 변환 도구 진입점: `references/Tools-master/README.md`
- 로컬 Spine JSON 객체 모델: `references/Tools-master/src/format/spineFormat.ts`
- 로컬 DragonBones to Spine 변환기: `references/Tools-master/src/action/toSpine.ts`
- 현재 CCB 데이터 모델과 샘플러: `src/ccb/model.ts`
- 현재 CCB canvas renderer transform 동작: `src/ccb/renderer.ts`
- 현재 atlas plist parser: `src/ccb/atlas.ts`

README는 제공된 도구 위치를 찾고 DragonBones에서 Spine으로 변환을 지원하는지 확인하는 용도로만 사용했다. 실제 구현 참고 대상은 `toSpine.ts`와 `spineFormat.ts`이다.

## Spine JSON 최상위 구조

Spine JSON은 런타임 인스턴스가 아니라 skeleton 데이터를 직렬화한다. 공식 형식은 다음 요소를 중심으로 구성된다.

- `skeleton`: `spine`, `fps`, `width`, `height`, `images`, `hash` 같은 메타데이터.
- `bones`: setup pose 기준의 bone 목록. 부모 bone은 자식보다 먼저 나와야 한다.
- `slots`: setup draw order. 각 slot은 bone에 묶이고 setup attachment 이름을 가질 수 있다.
- `skins`: skin, slot, attachment 이름 순서로 그룹화된 attachment 모음.
- `animations`: bone, slot, draw order, event, constraint에 대한 이름 있는 animation timeline.

Spine 4.3 import를 위해 이 exporter는 최신 형식인 배열 형태의 `skins`를 쓴다.

```json
{
  "skins": [
    {
      "name": "default",
      "attachments": {
        "slotName": {
          "attachmentName": {
            "path": "textureRegion",
            "x": 0,
            "y": 0,
            "width": 64,
            "height": 64
          }
        }
      }
    }
  ]
}
```

`references/Tools-master/src/format/spineFormat.ts`는 `skins`가 map인 예전 Spine 호환 구조를 모델링한다. 현재 공식 문서는 위 배열 구조를 사용하므로, 로컬 변환기는 필드 이름과 transform 규칙 참고용으로 사용하고 실제 출력 컨테이너 구조는 공식 문서를 따른다.

## 좌표와 Transform 규칙

로컬 DragonBones 변환기는 Spine으로 옮길 때 Y와 rotation을 뒤집는다.

- DragonBones bone `y`는 Spine bone `-y`가 된다.
- DragonBones bone `skY`는 Spine bone `-rotation`이 된다.
- Region attachment의 `y`와 `rotation`도 음수로 바꾼다.

현재 CCB renderer는 최종 viewport를 Y-up world로 뒤집는 canvas transform을 거친 뒤 내부적으로 `-pose.rotation`을 사용해 그린다. exporter는 이 동작을 bone 레벨에서 맞춘다. setup bone은 `rotation: -pose.rotation`을 쓰고, Spine 4.x rotate timeline은 상대값을 `setupPose.rotation - framePose.rotation`으로 쓴다. 이 값은 Spine 4의 rotate 필드인 `value`와 짝을 이루도록 의도된 것이다. 예전 `angle` 필드를 쓰면 Spine 4.3에서 rotate key가 무시된다.

CCB sprite는 anchor만큼 local origin이 이동된 사각형으로 그려진다.

```text
local rectangle = [-anchor.x * width, -anchor.y * height] to [(1-anchor.x) * width, (1-anchor.y) * height]
```

Spine region attachment는 중심 기준 사각형이다. 동일한 위치를 만드는 attachment offset은 다음과 같다.

```text
attachment.x = (0.5 - anchor.x) * width
attachment.y = (0.5 - anchor.y) * height
```

이 계산은 trim되지 않은 frame에서 현재 renderer의 setup pose와 일치한다. atlas frame에 `sourceWidth/sourceHeight`가 있으면 attachment 크기로 사용한다.

## CCB에서 Spine으로의 매핑

| CCB 개념                    | Spine 출력                                 | 설명                                                                                                                                         |
| --------------------------- | ------------------------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------- |
| Root CCB document           | `skeleton`                                 | `spine`은 `4.3.00`으로 쓰고, `fps` 기본값은 30이다.                                                                                          |
| CCB node                    | `bone`                                     | Node tree 순서는 이미 부모가 자식보다 먼저 나온다.                                                                                           |
| `position`                  | bone `x`, `y`와 `translate` timeline       | 값은 CCB model에서 샘플링한다.                                                                                                               |
| `rotation`                  | bone `rotation`과 `rotate` timeline        | CCB가 렌더링하는 방향과 맞추기 위해 setup rotation을 음수로 쓴다. Spine 4.x rotate animation key는 예전 `angle`이 아니라 `value`를 사용한다. |
| `scale`                     | bone `scaleX`, `scaleY`와 `scale` timeline | 자식 transform이 scale을 자연스럽게 상속하도록 CCB scale은 bone에 유지한다.                                                                  |
| `flip`                      | region attachment `scaleX`, `scaleY`       | 눈, 입, 손 같은 자식 bone이 의도치 않게 mirror되거나 이동하지 않도록 flip은 bone이 아니라 attachment에 둔다.                                 |
| `CCSprite`와 `displayFrame` | `slot` + region attachment                 | Slot은 해당 node의 bone에 붙는다.                                                                                                            |
| `visible`                   | slot `attachment` timeline                 | `false`는 `name: null`로 매핑한다.                                                                                                           |
| `displayFrame` 변경         | slot `attachment` timeline                 | 참조된 각 frame은 region attachment가 된다.                                                                                                  |
| Plist atlas frame           | `images/*.png` region path                 | Spine Editor import를 위해 frame을 개별 PNG로 풀어낸다.                                                                                      |

## Renderer 전용 CCB 규칙

`src/ccb/renderer.ts`에는 CCB keyframe만 봐서는 명확하지 않은 동작이 들어 있으며, exporter도 이를 맞춰야 한다.

- 자식이 있는 sprite node는 Spine bone 두 개로 분리한다. 하나는 node pivot bone이고, 다른 하나는 content-offset bone이다. 눈, 입, 팔 같은 자식은 content-offset bone 아래에 붙여 렌더링된 sprite anchor를 따라가게 한다.
- 실제 visibility는 부모에게서 상속된다. 부모 face sprite가 숨겨졌거나 화면 밖으로 이동한 경우 자식 slot도 `null`로 key를 찍는다.
- 일부 `walkout` animation은 합성된다. `walkout` sequence에 sprite-level track이 없으면 renderer는 `walkin`에서 자식 sprite pose를 샘플링한 뒤 sequence camera bounds 중심을 기준으로 전체 drawing을 mirror한다. exporter는 CCB root 위에 export 전용 `__mirror` bone을 쓰고, 이런 합성 `walkout` animation에서만 `scale.x = -1`과 `translate.x = 2 * centerX`를 key로 기록한다.

현재 CCB animation model은 linear, stepped toggle, degrees, position, sprite-frame 변경에 대해서만 keyframe interpolation을 노출한다. exporter는 단순 linear keyframe과 stepped attachment 변경을 출력한다. Bezier curve는 복원하지 않는다.

## Export 파일

각 export 대상은 `spine/<source-name>/`에 기록된다. 예시는 `spine/animal_cat/`이다.

- `<asset-key>.json`: Spine 4.3 skeleton JSON.
- `images/*.png`: Spine Editor import를 위해 atlas page에서 잘라낸 attachment image.

풀어낸 `images` 폴더는 Spine Editor import에 중요하다. 현재 목표는 Spine 4.3에서 import하고 편집하는 것이므로 exporter는 기본적으로 runtime atlas file을 만들지 않는다. Spine Editor에서 보이는 attachment는 attachment path와 일치하는 실제 image file이 필요하다.

## 현재 제한 사항

- 현재 이 프로젝트에서 렌더링하는 CCB 데이터가 sprite-node 기반이므로 mesh, IK, transform constraint, path constraint, event, draw-order timeline은 생성하지 않는다.
- plist metadata의 sprite trimming offset은 아직 완전히 반영하지 않는다. `sourceWidth/sourceHeight`는 사용하지만 source rect offset은 공용 atlas parser에서 아직 파싱하지 않는다.
- exporter의 목표는 현재 browser player와 시각적으로 최대한 맞고 Spine으로 import 가능한 결과를 만드는 것이다. 최적화된 bone 구조를 가진 수작업 Spine rig를 만드는 것이 목표는 아니다.
