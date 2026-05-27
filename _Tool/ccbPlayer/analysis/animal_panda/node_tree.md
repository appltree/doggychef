# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_panda\node_animal_panda.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 34
- spriteCount: 32
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode
  - #1 | CCLayer | CCLayer | pos=[0, 0, 0]
    animations: 1:walkout(position) / 2:walkin(position) / 3:order(position) / 4:like(position)
    - #2 | CCSprite | CCSpriteLegLeft | frame=animal_panda_leg_01.png | pos=[17, 59, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 6:eat(visible)
    - #3 | CCSprite | CCSpriteLegLeft_In | frame=animal_panda_leg_01.png | pos=[17, 58, 0] | visible=false
      animations: 4:like(visible) / 5:angry(visible)
    - #4 | CCSprite | CCSpriteLegRight | frame=animal_panda_leg_01.png | pos=[-20, 57, 0] | rot=0
      animations: 2:walkin(position, rotation)
    - #5 | CCSprite | CCSpriteArmLeft | frame=animal_panda_arm_left_01.png | pos=[20, 115, 0] | rot=-30 | visible=false
      animations: 2:walkin(position, rotation, visible) / 3:order(visible)
    - #6 | CCSprite | CCSpriteArmLeftTop | frame=animal_panda_arm_left_top.png | pos=[32, 122, 0] | visible=true
      animations: 4:like(visible) / 5:angry(visible) / 6:eat(position, visible)
    - #7 | CCSprite | CCSpriteBody | frame=animal_panda_body_01.png | pos=[-3, 73, 0]
    - #8 | CCSprite | CCSpriteArmRightTop | frame=animal_panda_arm_left_top.png | pos=[-37, 121, 0] | visible=true
      animations: 4:like(visible) / 5:angry(visible) / 6:eat(position, visible)
    - #9 | CCSprite | CCSpriteFace | frame=animal_panda_face_01.png | pos=[-3, 149, 0] | rot=5
      animations: 2:walkin(rotation) / 4:like(position, rotation) / 5:angry(position, rotation) / 6:eat(position, rotation)
      - #10 | CCSprite | CCSpriteEyeClose | frame=animal_panda_eye_03.png | pos=[63, 51, 0] | visible=false
        animations: 3:order(visible)
      - #11 | CCSprite | CCSpriteEyeOpen | frame=animal_panda_eye_01.png | pos=[63, 51, 0] | visible=false
        animations: 2:walkin(visible) / 3:order(visible)
      - #12 | CCSprite | CCSpriteEyeCry | frame=animal_panda_eye_04.png | pos=[63, 51, 0] | visible=false
        animations: 5:angry(visible)
      - #13 | CCSprite | CCSpriteEyeSmiile | frame=animal_panda_eye_02.png | pos=[63, 51, 0] | visible=true
        animations: 4:like(visible) / 6:eat(visible)
      - #14 | CCSprite | CCSpriteMouse | frame=animal_panda_mouse_01.png | pos=[65, 19, 0] | visible=true
        animations: 2:walkin(visible) / 3:order(visible) / 6:eat(visible)
      - #15 | CCSprite | CCSpriteMouseCry | frame=animal_panda_mouse_05.png | pos=[62, 19, 0] | visible=false
        animations: 5:angry(visible)
      - #16 | CCSprite | CCSpriteMouseSmile | frame=animal_panda_mouse_02.png | pos=[65, 19, 0] | visible=false
        animations: 4:like(visible)
      - #17 | CCSprite | CCSpriteMouseEat | frame=animal_panda_mouse_04.png | pos=[65, 19, 0] | visible=false
        animations: 6:eat(visible)
    - #18 | CCSprite | CCSpriteArmRight | frame=animal_panda_arm_right_01.png | pos=[-36, 117, 0] | rot=25 | visible=false
      animations: 2:walkin(position, rotation, visible) / 3:order(visible)
    - #19 | CCSprite | CCSpriteArmRightTopUnder | frame=animal_panda_arm_right_02.png | pos=[-44, 73, 0] | rot=40 | visible=false
      animations: 4:like(visible) / 5:angry(position, rotation, visible)
    - #20 | CCSprite | CCSprite | frame=animal_panda_arm_right_03.png | pos=[-48, 110, 0] | rot=49 | visible=true
      animations: 6:eat(position, rotation, visible)
    - #21 | CCSprite | CCSpriteArmRightTopUnder | frame=animal_panda_arm_right_02.png | pos=[58, 91, 0] | rot=-100 | visible=true
      animations: 4:like(visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
  - #22 | CCSprite | CCSpriteTearLeft0 | frame=animal_panda_tears_0.png | pos=[-645, 639, 0] | visible=true
    animations: 5:angry(position, visible)
  - #23 | CCSprite | CCSpriteTearLeft1 | frame=animal_panda_tears_1.png | pos=[-632, 647, 0] | visible=true
    animations: 5:angry(position, visible)
  - #24 | CCSprite | CCSpriteTearLeft2 | frame=animal_panda_tears_2.png | pos=[-740, 675, 0] | visible=true
    animations: 5:angry(position, visible)
  - #25 | CCSprite | CCSpriteTearLeft3 | frame=animal_panda_tears_3.png | pos=[-724, 635, 0] | visible=true
    animations: 5:angry(position, visible)
  - #26 | CCSprite | CCSpriteTearRight0 | frame=animal_panda_tears_0.png | pos=[-858, 595, 0] | visible=true
    animations: 5:angry(position, visible)
  - #27 | CCSprite | CCSpriteTearRight1 | frame=animal_panda_tears_1.png | pos=[-728, 611, 0] | visible=true
    animations: 5:angry(position, visible)
  - #28 | CCSprite | CCSpriteTearRight2 | frame=animal_panda_tears_2.png | pos=[-822, 717, 0] | visible=true
    animations: 5:angry(position, visible)
  - #29 | CCSprite | CCSpriteTearRight3 | frame=animal_panda_tears_3.png | pos=[-730, 615, 0] | visible=true
    animations: 5:angry(position, visible)
  - #30 | CCSprite | CCSpriteSpitLeft_0 | frame=animal_panda_eat2_01.png | pos=[-23, 123, 0] | visible=true
    animations: 6:eat(position, visible)
  - #31 | CCSprite | CCSpriteSpitLeft_1 | frame=animal_panda_eat2_02.png | pos=[-23, 123, 0] | visible=false
    animations: 6:eat(position, visible)
  - #32 | CCSprite | CCSpriteSpitRight_0 | frame=animal_panda_eat2_01.png | pos=[35, 121, 0] | visible=true
    animations: 6:eat(position, visible)
  - #33 | CCSprite | CCSpriteSpitRight_1 | frame=animal_panda_eat2_02.png | pos=[35, 121, 0] | visible=false
    animations: 6:eat(position, visible)
