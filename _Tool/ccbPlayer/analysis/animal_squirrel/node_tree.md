# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_squirrel\node_animal_squirrel.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 42
- spriteCount: 40
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode | pos=[0, -1, 0] | visible=true
  - #1 | CCLayer | CCLayerBody | pos=[19, 0, 0] | rot=0 | visible=true
    animations: 1:walkout(position) / 3:order(position, rotation) / 4:like(position, visible)
    - #2 | CCSprite | CCSpriteTailOut | frame=animal_squirrel_tail.png | pos=[-760, 840, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #3 | CCSprite | CCSpriteTail | frame=animal_squirrel_tail.png | pos=[-24, 36, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #4 | CCSprite | CCSpriteArmRightOut | frame=animal_squirrel_arm_02.png | pos=[-655, 865, 0] | rot=80 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #5 | CCSprite | CCSpriteArmRight | frame=animal_squirrel_arm_02.png | pos=[-673, 846, 0] | rot=-20 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible)
    - #6 | CCSprite | CCSpriteLegRight | frame=animal_squirrel_leg.png | pos=[9, 46, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #7 | CCSprite | CCSpriteLegLeft | frame=animal_squirrel_leg.png | pos=[-19, 44, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 5:angry(position, visible) / 6:eat(position, rotation, visible)
    - #8 | CCSprite | CCSpriteLegLeftOut | frame=animal_squirrel_leg.png | pos=[-670, 892, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible) / 4:like(position, rotation, visible)
    - #9 | CCSprite | CCSpriteLegRightOut | frame=animal_squirrel_leg.png | pos=[-650, 891, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #10 | CCSprite | CCSpriteBodyOut | frame=animal_squirrel_body.png | pos=[-677, 906, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #11 | CCSprite | CCSpriteBody | frame=animal_squirrel_body.png | pos=[-6, 61, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #12 | CCSprite | CCSpriteFaceOut | frame=animal_squirrel_face_01.png | pos=[-716, 861, 0] | visible=true
      animations: 1:walkout(position, visible)
      - #13 | CCSprite | CCSpriteMouse | frame=animal_squirrel_mouse_01.png | pos=[31, 14, 0] | visible=true
      - #14 | CCSprite | CCSpriteEye | frame=animal_squirrel_eye_01.png | pos=[33, 37, 0] | visible=true
    - #15 | CCSprite | CCSpriteFaceEat | frame=animal_squirrel_face_eat.png | pos=[-6, 76, 0] | rot=5 | visible=true
      animations: 6:eat(position, rotation, visible)
      - #16 | CCSprite | CCSpriteEyeEat | frame=animal_squirrel_eye_04.png | pos=[49, 38, 0] | visible=true
      - #17 | CCSprite | CCSpriteMouseEat | frame=animal_squirrel_mouse_03.png | pos=[49, 16, 0] | visible=true
        animations: 6:eat(position, visible)
    - #18 | CCSprite | CCSpriteFaceEat2 | frame=animal_squirrel_face_01.png | pos=[-6, 76, 0] | rot=5 | visible=false
      animations: 6:eat(position, rotation, visible)
      - #19 | CCSprite | CCSpriteEyeEat | frame=animal_squirrel_eye_04.png | pos=[49, 38, 0] | visible=true
      - #20 | CCSprite | CCSpriteMouseEatBig | frame=animal_squirrel_mouse_02.png | pos=[49, 16, 0] | visible=false
        animations: 6:eat(position, visible)
    - #21 | CCSprite | CCSpriteFace | frame=animal_squirrel_face_01.png | pos=[-721, 774, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible)
      - #22 | CCSprite | CCSpriteEye | frame=animal_squirrel_eye_01.png | pos=[-637.5, 668, 0] | visible=true
        animations: 2:walkin(position, visible) / 3:order(position, visible)
      - #23 | CCSprite | CCSpriteClose_0 | frame=animal_squirrel_eye_02.png | pos=[-629, 748, 0] | visible=true
        animations: 3:order(position, visible)
      - #24 | CCSprite | CCSpriteClose_1 | frame=animal_squirrel_eye_03.png | pos=[-619, 826, 0] | visible=true
        animations: 3:order(position, visible) / 4:like(position, visible)
      - #25 | CCSprite | CCSpriteMouse | frame=animal_squirrel_mouse_01.png | pos=[-642.5, 626.5, 0] | visible=true
        animations: 2:walkin(position, visible) / 3:order(position, visible)
      - #26 | CCSprite | CCSpriteMouse2 | frame=animal_squirrel_mouse_02.png | pos=[-638, 762, 0] | rot=0 | visible=true
        animations: 4:like(position, rotation, visible)
    - #27 | CCSprite | CCSpriteArmLeft | frame=animal_squirrel_arm_01.png | pos=[-750, 872, 0] | rot=10 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible)
    - #28 | CCSprite | CCSpriteArmLeftOut | frame=animal_squirrel_arm_01.png | pos=[-765, 862, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible) / 4:like(position, rotation, visible)
    - #29 | CCSprite | CCSpriteArmAngryRight | frame=animal_squirrel_arm_04.png | pos=[41, 80, 0] | rot=-25 | visible=true
      animations: 5:angry(position, visible) / 6:eat(position, rotation, visible)
    - #30 | CCSprite | CCSpriteArmAngryLeft | frame=animal_squirrel_arm_03.png | pos=[-43, 76, 0] | rot=-10 | visible=true
      animations: 5:angry(position, visible) / 6:eat(position, rotation, visible)
    - #31 | CCSprite | CCSpriteFaceAngry | frame=animal_squirrel_face_angry.png | pos=[-721, 836, 0] | visible=true
      animations: 5:angry(position, visible)
  - #32 | CCSprite | CCSpriteAngryEffectLeft_0 | frame=animal_squirrel_angry_0001.png | pos=[-717, 854, 0] | visible=true
    animations: 5:angry(position, visible)
  - #33 | CCSprite | CCSpriteAngryEffectLeft_1 | frame=animal_squirrel_angry_0002.png | pos=[-704, 857, 0] | visible=true
    animations: 5:angry(position, visible)
  - #34 | CCSprite | CCSpriteAngryEffectLeft_2 | frame=animal_squirrel_angry_0003.png | pos=[-743, 771, 0] | visible=true
    animations: 5:angry(position, visible)
  - #35 | CCSprite | CCSpriteAngryEffectRight_0 | frame=animal_squirrel_angry_0001.png | pos=[-698, 830, 0] | visible=true
    animations: 5:angry(position, visible)
  - #36 | CCSprite | CCSpriteAngryEffectRight_1 | frame=animal_squirrel_angry_0002.png | pos=[-687, 890, 0] | visible=true
    animations: 5:angry(position, visible)
  - #37 | CCSprite | CCSpriteAngryEffectRight_2 | frame=animal_squirrel_angry_0003.png | pos=[-699, 812, 0] | visible=true
    animations: 5:angry(position, visible)
  - #38 | CCSprite | CCSpriteSpitRight0 | frame=animal_squirrel_eat_effect0001.png | pos=[47, 87, 0] | visible=true
    animations: 6:eat(position, visible)
  - #39 | CCSprite | CCSpriteSpitRight1 | frame=animal_squirrel_eat_effect0002.png | pos=[40, 91, 0] | visible=false
    animations: 6:eat(position, visible)
  - #40 | CCSprite | CCSpriteSpitLeft0 | frame=animal_squirrel_eat_effect0001.png | pos=[-10, 92, 0] | visible=false
    animations: 6:eat(position, visible)
  - #41 | CCSprite | CCSpriteSpitLeft1 | frame=animal_squirrel_eat_effect0002.png | pos=[10, 91, 0] | visible=true
    animations: 6:eat(position, visible)
