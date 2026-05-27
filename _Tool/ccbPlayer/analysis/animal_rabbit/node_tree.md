# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_rabbit\node_animal_rabbit.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 47
- spriteCount: 44
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode | visible=true
  - #1 | CCLayer | CCLayerBody | pos=[0, 0, 0] | visible=true
    animations: 1:walkout(position) / 2:walkin(position) / 3:order(position) / 4:like(position) / 5:angry(position) / 6:eat(position)
    - #2 | CCSprite | CCSpriteArmRight | frame=animal_rabbit_arm_0.png | pos=[-796, 841, 0] | rot=-20 | visible=true
      animations: 1:walkout(position, rotation, visible) / 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible)
    - #3 | CCSprite | CCSpriteLegLeftFlip | frame=animal_rabbit_leg.png | pos=[-725, 770, 0] | rot=30 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #4 | CCSprite | CCSpriteLegRight | frame=animal_rabbit_leg.png | pos=[11, 30, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, visible) / 4:like(position, rotation, visible) / 5:angry(position, visible) / 6:eat(position, visible)
    - #5 | CCSprite | CCSpriteBody | frame=animal_rabbit_body.png | pos=[0, 52, 0] | visible=true
      animations: 2:walkin(position, visible) / 3:order(position, visible) / 4:like(position, visible) / 5:angry(position, visible) / 6:eat(position, visible)
    - #6 | CCSprite | CCSpriteBodyFlip | frame=animal_rabbit_body.png | pos=[-777, 903, 0] | visible=true
      animations: 1:walkout(position, visible)
    - #7 | CCSprite | CCSpriteLegLeft | frame=animal_rabbit_leg.png | pos=[-16, 30, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, visible) / 4:like(position, rotation, visible) / 5:angry(position, visible) / 6:eat(position, visible)
    - #8 | CCSprite | CCSpriteArmLeft | frame=animal_rabbit_arm_0.png | pos=[-698, 784, 0] | rot=20 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible)
    - #9 | CCSprite | CCSpriteArmLeft2 | frame=animal_rabbit_arm_0.png | pos=[-762, 885, 0] | rot=-50 | visible=true
      animations: 1:walkout(position, rotation, visible) / 4:like(position, rotation, visible)
    - #10 | CCSprite | CCSpriteLegRightFlip | frame=animal_rabbit_leg.png | pos=[-762, 854, 0] | rot=-30 | visible=true
      animations: 1:walkout(position, rotation, visible)
  - #11 | CCLayer | CCLayerHead | pos=[-3, 65, 0] | rot=0 | visible=true
    animations: 1:walkout(position) / 2:walkin(position, rotation) / 3:order(position) / 4:like(position) / 5:angry(position) / 6:eat(position, rotation)
    - #12 | CCSprite | CCSpriteTearRight0 | frame=animal_rabbit_tear_0.png | pos=[-793, 849, 0] | visible=true
      animations: 5:angry(position, visible)
    - #13 | CCSprite | CCSpriteTearRight1 | frame=animal_rabbit_tear_2.png | pos=[-721, 792, 0] | visible=true
      animations: 5:angry(position, visible)
    - #14 | CCSprite | CCSpriteTearRight2 | frame=animal_rabbit_tear_2.png | pos=[-739, 816, 0] | visible=true
      animations: 5:angry(position, visible)
    - #15 | CCSprite | CCSpriteTearLeft0 | frame=animal_rabbit_tear_0.png | pos=[-690, 836, 0] | visible=true
      animations: 5:angry(position, visible)
    - #16 | CCSprite | CCSpriteTearLeft1 | frame=animal_rabbit_tear_1.png | pos=[-702, 893, 0] | visible=true
      animations: 5:angry(position, visible)
    - #17 | CCSprite | CCSpriteTearLeft2 | frame=animal_rabbit_tear_2.png | pos=[-758, 889, 0] | visible=true
      animations: 5:angry(position, visible)
    - #18 | CCSprite | CCSpriteEarRight | frame=animal_rabbit_ear_1.png | pos=[22, 67, 0] | rot=10 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, visible) / 6:eat(position, rotation, visible)
    - #19 | CCSprite | CCSpriteEarRight2 | frame=animal_rabbit_ear_0.png | pos=[-740, 772, 0] | rot=20 | visible=true
      animations: 4:like(position, rotation, visible)
    - #20 | CCSprite | CCSpriteFace | frame=animal_rabbit_face_0.png | pos=[-789, 795, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible)
    - #21 | CCSprite | CCSpriteFaceLike | frame=animal_rabbit_face_1.png | pos=[-759, 842, 0] | visible=true
      animations: 4:like(position, visible)
    - #22 | CCSprite | CCSpriteEarAngryLeft | frame=animal_rabbit_ear_2.png | pos=[-827, 926, 0] | rot=200 | visible=true
      animations: 5:angry(position, rotation, visible)
    - #23 | CCSprite | CCSpriteEarAngryRight | frame=animal_rabbit_ear_2.png | pos=[-709, 779, 0] | rot=160 | visible=true
      animations: 5:angry(position, rotation, visible)
    - #24 | CCSprite | CCSpriteEarLeftFlip | frame=animal_rabbit_ear_1.png | pos=[-708, 890, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #25 | CCSprite | CCSpriteFaceFlip | frame=animal_rabbit_face_0.png | pos=[-747, 857, 0] | visible=true
      animations: 1:walkout(position, visible)
    - #26 | CCSprite | CCSpriteFaceAngry | frame=animal_rabbit_face_2.png | pos=[-786, 835, 0] | visible=true
      animations: 5:angry(position, visible)
    - #27 | CCSprite | CCSpriteFaceVacant | frame=animal_rabbit_face_3.png | pos=[1, 0, 0] | rot=0 | visible=true
      animations: 3:order(position, visible) / 6:eat(position, rotation, visible)
      - #28 | CCSprite | CCSpriteEyeEat | frame=animal_rabbit_eye_eat.png | pos=[50, 39, 0] | visible=true
        animations: 6:eat(position, visible)
      - #29 | CCSprite | CCSpriteMouseEat_0 | frame=animal_rabbit_mouse_eat_0.png | pos=[49, 32, 0] | visible=true
        animations: 6:eat(position, visible)
      - #30 | CCSprite | CCSpriteMouseEat_1 | frame=animal_rabbit_mouse_eat_1.png | pos=[49, 32, 0] | visible=false
        animations: 6:eat(position, visible)
    - #31 | CCSprite | CCSpriteEarLeft | frame=animal_rabbit_ear_0.png | pos=[-12, 65, 0] | rot=-20 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, visible) / 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #32 | CCSprite | CCSpriteMouseCry0 | frame=animal_rabbit_mouse_cry_0.png | pos=[-794, 761, 0] | visible=true
      animations: 5:angry(position, visible)
    - #33 | CCSprite | CCSpriteMouseCry1 | frame=animal_rabbit_mouse_cry_1.png | pos=[-797, 823, 0] | visible=true
      animations: 5:angry(position, visible)
    - #34 | CCSprite | CCSpriteMouseNormal | frame=animal_rabbit_mouse_normal.png | pos=[-710, 731, 0] | visible=true
      animations: 3:order(position, visible)
    - #35 | CCSprite | CCSpriteEyeNormal_0 | frame=animal_rabbit_eye_normal_0.png | pos=[-745, 793, 0] | visible=true
      animations: 3:order(position, visible)
    - #36 | CCSprite | CCSpriteEyeNormal_1 | frame=animal_rabbit_eye_normal_1.png | pos=[-721, 734, 0] | visible=true
      animations: 3:order(position, visible)
    - #37 | CCSprite | CCSpriteEyeNormal_2 | frame=animal_rabbit_eye_normal_2.png | pos=[-752, 743, 0] | visible=true
      animations: 3:order(position, visible)
    - #38 | CCSprite | CCSpriteEarRightFlip | frame=animal_rabbit_ear_0.png | pos=[-786, 799, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
  - #39 | CCSprite | CCSpriteCryArmLeft | frame=animal_rabbit_arm_0.png | pos=[-745, 947, 0] | rot=180 | visible=true
    animations: 5:angry(position, rotation, visible)
  - #40 | CCSprite | CCSpriteCryArmRight | frame=animal_rabbit_arm_0.png | pos=[-766, 929, 0] | rot=180 | visible=true
    animations: 5:angry(position, rotation, visible)
  - #41 | CCSprite | CCSpriteEatArmLeft | frame=animal_rabbit_arm_1.png | pos=[-36, 70, 0] | rot=0 | visible=true
    animations: 6:eat(position, rotation, visible)
  - #42 | CCSprite | CCSpriteEatArmRight | frame=animal_rabbit_arm_2.png | pos=[38, 67, 0] | rot=0 | visible=true
    animations: 6:eat(position, rotation, visible)
  - #43 | CCSprite | CCSpriteSpitRight_0 | frame=animal_rabbit_spit_0.png | pos=[-21, 67, 0] | visible=true
    animations: 6:eat(position, visible)
  - #44 | CCSprite | CCSpriteSpitRight_1 | frame=animal_rabbit_spit_1.png | pos=[-27, 67, 0] | visible=false
    animations: 6:eat(position, visible)
  - #45 | CCSprite | CCSpriteSpitLeft_0 | frame=animal_rabbit_spit_0.png | pos=[26, 69, 0] | visible=true
    animations: 6:eat(position, visible)
  - #46 | CCSprite | CCSpriteSpitLeft_1 | frame=animal_rabbit_spit_1.png | pos=[26, 69, 0] | visible=false
    animations: 6:eat(position, visible)
