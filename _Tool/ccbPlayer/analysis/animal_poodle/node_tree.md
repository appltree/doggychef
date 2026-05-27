# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_poodle\node_animal_poodle.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 43
- spriteCount: 41
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode | visible=true
  - #1 | CCLayer | CCLayerPoodle | pos=[0, 0, 0] | visible=true
    animations: 1:walkout(position, visible) / 2:walkin(position, visible) / 4:like(position, visible) / 5:angry(position, visible) / 6:eat(position, visible) / 7:order(position, visible)
    - #2 | CCSprite | CCSpriteLegOutLeft | frame=animal_poodle_leg.png | pos=[-657, 672, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #3 | CCSprite | CCSpriteLegOutRight | frame=animal_poodle_leg.png | pos=[-693, 600, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #4 | CCSprite | CCSpriteArmOutRight | frame=animal_poodle_arm_normal.png | pos=[-664, 659, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #5 | CCSprite | CCSpriteEarOutLeft | frame=animal_poodle_ear.png | pos=[-687, 661, 0] | rot=20 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #6 | CCSprite | CCSpriteBodyOut | frame=animal_poodle_body.png | pos=[-594, 745, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #7 | CCSprite | CCSpriteFaceOut | frame=animal_poodle_face.png | pos=[-664, 555, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
      - #8 | CCSprite | CCSpriteEarRIght | frame=animal_poodle_ear.png | pos=[-325, 121, 0] | rot=-30 | visible=true
        animations: 1:walkout(position, rotation, visible)
      - #9 | CCSprite | CCSpriteRibbon | frame=animal_poodle_ribbon.png | pos=[61, 53, 0] | rot=0 | visible=true
        animations: 1:walkout(position, rotation, visible)
      - #10 | CCSprite | CCSpriteEye | frame=animal_poodle_face_eye_normal.png | pos=[30, 35, 0] | rot=0 | visible=true
        animations: 1:walkout(position, rotation, visible)
      - #11 | CCSprite | CCSpriteMouseNormal | frame=animal_poodle_face_mouth_normal.png | pos=[27, 18, 0] | rot=0 | visible=true
        animations: 1:walkout(position, rotation, visible)
    - #12 | CCSprite | CCSpriteArmOutLeft | frame=animal_poodle_arm_normal.png | pos=[-649, 687, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #13 | CCSprite | CCSpriteLegLeft | frame=animal_poodle_leg.png | pos=[16, 42, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible) / 7:order(position, rotation, visible)
    - #14 | CCSprite | CCSpriteLegRight | frame=animal_poodle_leg.png | pos=[-15, 42, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible) / 7:order(position, rotation, visible)
    - #15 | CCSprite | CCSpriteArmAngryRight | frame=animal_poodle_arm_angry.png | pos=[-655, 618, 0] | rot=-20 | visible=true
      animations: 5:angry(position, rotation, visible)
    - #16 | CCSprite | CCSpriteArmBendRight | frame=animal_poodle_arm_bend.png | pos=[-675, 603, 0] | rot=-50 | visible=true
      animations: 2:walkin(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #17 | CCSprite | CCSpriteArmRight | frame=animal_poodle_arm_normal.png | pos=[24, 93, 0] | rot=-20 | visible=true
      animations: 4:like(position, rotation, visible) / 7:order(position, rotation, visible)
    - #18 | CCSprite | CCSpriteBody | frame=animal_poodle_body.png | pos=[0, 61, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible) / 7:order(position, rotation, visible)
    - #19 | CCSprite | CCSpriteEarRight | frame=animal_poodle_ear.png | pos=[32, 155, 0] | rot=-20 | visible=true
      animations: 2:walkin(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible) / 7:order(position, rotation, visible)
    - #20 | CCSprite | CCSpriteFace | frame=animal_poodle_face.png | pos=[-2, 79, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible) / 7:order(position, rotation, visible)
      - #21 | CCSprite | CCSpriteEyeClose | frame=animal_poodle_face_eye_close.png | pos=[56, 44, 0] | rot=0 | visible=false
        animations: 7:order(position, rotation, visible)
      - #22 | CCSprite | CCSpriteMouseSmile | frame=animal_poodle_face_mouth_smile.png | pos=[-656, 543, 0] | rot=0 | visible=true
        animations: 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
      - #23 | CCSprite | CCSpriteEyeSmile | frame=animal_poodle_face_eye_smile.png | pos=[-683, 555, 0] | rot=0 | visible=true
        animations: 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
      - #24 | CCSprite | CCSpriteEye | frame=animal_poodle_face_eye_normal.png | pos=[56, 44, 0] | rot=0 | visible=true
        animations: 2:walkin(position, rotation, visible) / 6:eat(position, visible) / 7:order(position, rotation, visible)
      - #25 | CCSprite | CCSpriteMouseEat0 | frame=animal_poodle_face_mouth_eat01.png | pos=[-624, 553, 0] | visible=true
        animations: 6:eat(position, visible)
      - #26 | CCSprite | CCSpriteMouseEat1 | frame=animal_poodle_face_mouth_eat02.png | pos=[-627, 529, 0] | visible=true
        animations: 6:eat(position, visible)
      - #27 | CCSprite | CCSpriteMouse | frame=animal_poodle_face_mouth_normal.png | pos=[57, 23, 0] | rot=0 | visible=true
        animations: 2:walkin(position, rotation, visible) / 7:order(position, rotation, visible)
      - #28 | CCSprite | CCSpriteEarLeft | frame=animal_poodle_ear.png | pos=[12, 73, 0] | rot=20 | visible=true
        animations: 2:walkin(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible) / 7:order(position, rotation, visible)
      - #29 | CCSprite | CCSpriteRibbon | frame=animal_poodle_ribbon.png | pos=[18, 66, 0] | rot=0 | visible=true
        animations: 2:walkin(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible) / 7:order(position, rotation, visible)
      - #30 | CCSprite | CCSpriteEyeAngry | frame=animal_poodle_face_eye_angry.png | pos=[-661, 500, 0] | rot=0 | visible=true
        animations: 5:angry(position, rotation, visible)
      - #31 | CCSprite | CCSpriteMouseAngry | frame=animal_poodle_face_mouth_angry.png | pos=[-623, 534, 0] | rot=0 | visible=true
        animations: 5:angry(position, rotation, visible)
    - #32 | CCSprite | CCSpriteArmLeft | frame=animal_poodle_arm_normal.png | pos=[-25, 92, 0] | rot=20 | visible=true
      animations: 4:like(position, rotation, visible) / 7:order(position, rotation, visible)
    - #33 | CCSprite | CCSpriteArmBendLeft | frame=animal_poodle_arm_bend.png | pos=[-658, 676, 0] | rot=-50 | visible=true
      animations: 2:walkin(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #34 | CCSprite | CCSpriteArmAngryLeft | frame=animal_poodle_arm_angry.png | pos=[-654, 680, 0] | rot=20 | visible=true
      animations: 5:angry(position, rotation, visible)
    - #35 | CCSprite | CCSpriteAngryEffect0 | frame=animal_poodle_effect_angry01.png | pos=[-637, 606, 0] | rot=0 | visible=true
      animations: 5:angry(position, rotation, visible)
    - #36 | CCSprite | CCSpriteAngryEffect1 | frame=animal_poodle_effect_angry02.png | pos=[-671, 621, 0] | rot=0 | visible=true
      animations: 5:angry(position, rotation, visible)
    - #37 | CCSprite | CCSpriteAngryEffect2 | frame=animal_poodle_effect_angry03.png | pos=[-643, 705, 0] | rot=0 | visible=true
      animations: 5:angry(position, rotation, visible)
    - #38 | CCSprite | CCSpriteEatEffect0 | frame=animal_poodle_effect_eat01.png | pos=[-677, 605, 0] | visible=true
      animations: 6:eat(position, visible)
    - #39 | CCSprite | CCSpriteEatEffect1 | frame=animal_poodle_effect_eat02.png | pos=[-666, 693, 0] | visible=true
      animations: 6:eat(position, visible)
    - #40 | CCSprite | CCSpriteEatEffect2 | frame=animal_poodle_effect_eat03.png | pos=[-719, 688, 0] | visible=true
      animations: 6:eat(position, visible)
    - #41 | CCSprite | CCSpriteEatEffect3 | frame=animal_poodle_effect_eat04.png | pos=[-669, 733, 0] | visible=true
      animations: 6:eat(position, visible)
    - #42 | CCSprite | CCSpriteEatEffect4 | frame=animal_poodle_effect_eat05.png | pos=[-599, 639, 0] | rot=0 | visible=true
      animations: 6:eat(rotation, visible)
