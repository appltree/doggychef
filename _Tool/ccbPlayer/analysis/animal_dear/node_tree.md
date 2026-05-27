# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_dear\node_animal_dear.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 40
- spriteCount: 38
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode
  - #1 | CCLayer | CCLayer | pos=[0, 0, 0]
    animations: 1:walkout(position) / 2:walkin(position) / 3:order(position) / 4:like(position)
    - #2 | CCSprite | CCSpriteLegLeft | frame=animal_dear_leg_01.png | pos=[16, 50, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation) / 4:like(position, rotation, visible)
    - #3 | CCSprite | CCSpriteArmLeft | frame=animal_dear_arm_02.png | pos=[8, 94, 0] | rot=-110 | visible=false
      animations: 3:order(visible)
    - #4 | CCSprite | CCSpriteArmLeft2 | frame=animal_dear_arm_01.png | pos=[20.5, 100.5, 0] | rot=0 | visible=false
      animations: 2:walkin(position, rotation, visible)
    - #5 | CCSprite | CCSpriteTail | frame=animal_dear_tail_01.png | pos=[-23.5, 46, 0]
    - #6 | CCSprite | CCSpriteArmLike2 | frame=animal_dear_arm_01.png | pos=[24.5, 86, 0] | rot=-140 | visible=false
      animations: 4:like(position, rotation, visible)
    - #7 | CCSprite | CCSpriteArmLike | frame=animal_dear_arm_02.png | pos=[18, 111.5, 0] | rot=0 | visible=false
      animations: 4:like(position, rotation, visible)
    - #8 | CCSprite | CCSpriteBody | frame=animal_dear_body_01.png | pos=[2, 71, 0]
    - #9 | CCSprite | CCSpriteLegRight | frame=animal_dear_leg_01.png | pos=[-13, 50, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 5:angry(visible) / 6:eat(visible)
    - #10 | CCSprite | CCSpriteArmRight | frame=animal_dear_arm_02.png | pos=[-9, 98, 0] | rot=-80 | visible=false
      animations: 3:order(visible)
    - #11 | CCSprite | CCSpriteArmRight2 | frame=animal_dear_arm_01.png | pos=[-14, 100.5, 0] | rot=0 | visible=false
      animations: 2:walkin(position, rotation, visible)
    - #12 | CCSprite | CCSpriteArmLike2 | frame=animal_dear_arm_01.png | pos=[-22, 84, 0] | rot=140 | visible=false
      animations: 4:like(position, rotation, visible)
    - #13 | CCSprite | CCSpriteArmLike | frame=animal_dear_arm_02.png | pos=[-14, 109, 0] | rot=0 | visible=false
      animations: 4:like(position, rotation, visible)
    - #14 | CCSprite | CCSpriteFace | frame=animal_dear_face_01.png | pos=[3, 144, 0] | rot=0
      animations: 2:walkin(position, rotation) / 4:like(position, rotation) / 5:angry(position, rotation) / 6:eat(position, rotation)
      - #15 | CCSprite | CCSpriteEyeNormal | frame=animal_dear_eye_05.png | pos=[62.5, 52, 0] | visible=false
        animations: 3:order(visible)
      - #16 | CCSprite | CCSpriteEyeNormal2 | frame=animal_dear_eye_01.png | pos=[62, 52, 0] | visible=true
        animations: 2:walkin(visible) / 6:eat(visible)
      - #17 | CCSprite | CCSpriteEyeClose | frame=animal_dear_eye_02.png | pos=[62, 52, 0] | visible=false
        animations: 3:order(visible) / 4:like(visible)
      - #18 | CCSprite | CCSpriteEyeCry0 | frame=animal_dear_eye_03.png | pos=[62, 52, 0] | visible=false
        animations: 5:angry(visible)
      - #19 | CCSprite | CCSpriteEyeCry1 | frame=animal_dear_eye_04.png | pos=[62, 52, 0] | visible=false
        animations: 5:angry(visible)
      - #20 | CCSprite | CCSpriteMouseNormal | frame=animal_dear_mouse_01.png | pos=[67, 17.5, 0] | visible=false
        animations: 2:walkin(visible) / 3:order(visible) / 6:eat(visible)
      - #21 | CCSprite | CCSpriteMouseEat | frame=animal_dear_mouse_02.png | pos=[67, 17, 0] | visible=true
        animations: 4:like(visible) / 6:eat(visible)
      - #22 | CCSprite | CCSpriteMouseCry0 | frame=animal_dear_mouse_03.png | pos=[67, 17, 0] | visible=false
        animations: 5:angry(visible)
      - #23 | CCSprite | CCSpriteMouseCry1 | frame=animal_dear_mouse_04.png | pos=[67, 17, 0] | visible=false
        animations: 5:angry(visible)
    - #24 | CCSprite | CCSpriteArmLeft | frame=animal_dear_arm_03.png | pos=[51, 72, 0] | rot=0 | visible=true
      animations: 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #25 | CCSprite | CCSpriteArmRight | frame=animal_dear_arm_02.png | pos=[-27, 108, 0] | rot=180 | visible=true
      animations: 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #26 | CCSprite | CCSpriteLeg | frame=animal_dear_leg_01.png | pos=[-16, 24, 0] | rot=0 | visible=false
      animations: 4:like(position, rotation, visible)
  - #27 | CCSprite | CCSpriteSpitLeft_0 | frame=animal_dear_effect_eat2_01.png | pos=[-19, 101.5, 0] | visible=true
    animations: 6:eat(position, visible)
  - #28 | CCSprite | CCSpriteSpitLeft_1 | frame=animal_dear_effect_eat2_02.png | pos=[-19, 101.5, 0] | visible=false
    animations: 6:eat(position, visible)
  - #29 | CCSprite | CCSpriteSpitRight_0 | frame=animal_dear_effect_eat2_01.png | pos=[41, 103, 0] | visible=true
    animations: 6:eat(position, visible)
  - #30 | CCSprite | CCSpriteSpitRight_1 | frame=animal_dear_effect_eat2_02.png | pos=[41, 103, 0] | visible=false
    animations: 6:eat(position, visible)
  - #31 | CCSprite | CCSpriteMusicEffect0 | frame=animal_dear_music_0001.png | pos=[-767, 880, 0] | visible=true
    animations: 4:like(position, visible)
  - #32 | CCSprite | CCSpriteMusicEffect1 | frame=animal_dear_music_0002.png | pos=[-690, 905, 0] | visible=true
    animations: 4:like(position, visible)
  - #33 | CCSprite | CCSpriteMusicEffect2 | frame=animal_dear_music_0003.png | pos=[-779, 859, 0] | visible=true
    animations: 4:like(position, visible)
  - #34 | CCSprite | CCSpriteMusicEffect3 | frame=animal_dear_music_0004.png | pos=[-704, 908, 0] | visible=true
    animations: 4:like(position, visible)
  - #35 | CCSprite | CCSpriteMusicEffect4 | frame=animal_dear_music_0005.png | pos=[-720, 916, 0] | visible=true
    animations: 4:like(position, visible)
  - #36 | CCSprite | CCSpriteMusicEffect5 | frame=animal_dear_music_0006.png | pos=[-699, 875, 0] | visible=true
    animations: 4:like(position, visible)
  - #37 | CCSprite | CCSpriteMusicEffect6 | frame=animal_dear_music_0007.png | pos=[-766, 922, 0] | visible=true
    animations: 4:like(position, visible)
  - #38 | CCSprite | CCSpriteMusicEffect7 | frame=animal_dear_music_0008.png | pos=[-790, 834, 0] | visible=true
    animations: 4:like(position, visible)
  - #39 | CCSprite | CCSpriteMusicEffect8 | frame=animal_dear_music_0009.png | pos=[-740, 851, 0] | visible=true
    animations: 4:like(position, visible)
