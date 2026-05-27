# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_hippo\node_animal_hippo.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 37
- spriteCount: 35
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode | visible=true
  - #1 | CCLayer | CCLayerHippo | pos=[0, 0, 0] | visible=true
    animations: 1:walkout(position, visible) / 2:walkin(position, visible) / 3:order(position, visible) / 4:like(position, visible) / 5:angry(position, visible) / 6:eat(position, visible)
    - #2 | CCSprite | CCSpriteArmBendRight | frame=animal_hippo_arm_bend.png | pos=[-875, 923, 0] | rot=-30 | visible=true
      animations: 5:angry(position, rotation, visible)
    - #3 | CCSprite | CCSpriteArmRight | frame=animal_hippo_arm_normal.png | pos=[30, 106, 0] | rot=-25 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #4 | CCSprite | CCSpriteLegRight | frame=animal_hippo_leg.png | pos=[26, 56, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #5 | CCSprite | CCSpriteLegLeft | frame=animal_hippo_leg.png | pos=[-22, 55, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #6 | CCSprite | CCSpriteLegOutLeft | frame=animal_hippo_leg.png | pos=[-782, 957, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #7 | CCSprite | CCSpriteLegOutRight | frame=animal_hippo_leg.png | pos=[-793, 936, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #8 | CCSprite | CCSpriteArmOutRight | frame=animal_hippo_arm_normal.png | pos=[-820, 980, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #9 | CCSprite | CCSpriteEarOutRight | frame=animal_hippo_ear.png | pos=[-847, 899, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #10 | CCSprite | CCSpriteEarOutLeft | frame=animal_hippo_ear.png | pos=[-797, -14, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #11 | CCSprite | CCSpriteBodyOut | frame=animal_hippo_body.png | pos=[-920, 960, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #12 | CCSprite | CCSpriteFaceOut | frame=animal_hippo_face_normal.png | pos=[-862, 866, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
      - #13 | CCSprite | CCSpriteEyeNormal | frame=animal_hippo_eye_normal.png | pos=[63, 67, 0] | rot=0 | visible=true
        animations: 1:walkout(position, rotation, visible)
      - #14 | CCSprite | CCSpriteMouse | frame=animal_hippo_mouth_normal_eat02.png | pos=[64, 24, 0] | rot=0 | visible=true
        animations: 1:walkout(position, rotation, visible)
    - #15 | CCSprite | CCSpriteArmOutLeft | frame=animal_hippo_arm_normal.png | pos=[-858, 960, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #16 | CCSprite | CCSpriteBody | frame=animal_hippo_body.png | pos=[-3, 61, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #17 | CCSprite | CCSpriteEarLeft | frame=animal_hippo_ear.png | pos=[-47, 169, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #18 | CCSprite | CCSpriteEarRight | frame=animal_hippo_ear.png | pos=[-6, 171, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #19 | CCSprite | CCSpriteFaceAngry | frame=animal_hippo_face_angry.png | pos=[-894, 871, 0] | rot=0 | visible=true
      animations: 5:angry(position, rotation, visible)
      - #20 | CCSprite | CCSpriteMouse | frame=animal_hippo_mouth_angry.png | pos=[60, 19, 0] | visible=true
        animations: 5:angry(position, visible)
      - #21 | CCSprite | CCSpriteEye | frame=animal_hippo_eye_angry.png | pos=[66, 65, 0] | visible=true
        animations: 5:angry(position, visible)
    - #22 | CCSprite | CCSpriteFace | frame=animal_hippo_face_normal.png | pos=[-5, 85, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
      - #23 | CCSprite | CCSpriteMouse | frame=animal_hippo_mouth_normal_eat02.png | pos=[65, 23, 0] | rot=0 | visible=false
        animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 6:eat(position, visible)
      - #24 | CCSprite | CCSpriteMouseEat | frame=animal_hippo_mouth_eat01.png | pos=[71, 19, 0] | rot=0 | visible=true
        animations: 6:eat(position, rotation, visible)
      - #25 | CCSprite | CCSpriteMouseSmile | frame=animal_hippo_mouth_smile.png | pos=[-747, 827, 0] | rot=0 | visible=true
        animations: 4:like(position, rotation, visible)
      - #26 | CCSprite | CCSpriteEye | frame=animal_hippo_eye_normal.png | pos=[-769.7052001953125, 814.2783203125, 0] | rot=0 | visible=true
        animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible)
      - #27 | CCSprite | CCSpriteEyeClose | frame=animal_hippo_eye_close.png | pos=[-811, 820, 0] | visible=true
        animations: 3:order(position, visible)
      - #28 | CCSprite | CCSpriteEyeSmile | frame=animal_hippo_eye_smile.png | pos=[66, 69, 0] | rot=0 | visible=true
        animations: 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #29 | CCSprite | CCSpriteArmBend | frame=animal_hippo_arm_bend.png | pos=[-49, 103, 0] | rot=-80 | visible=true
      animations: 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #30 | CCSprite | CCSpriteArmLeft | frame=animal_hippo_arm_normal.png | pos=[-839, 923, 0] | rot=20 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible)
    - #31 | CCSprite | CCSpriteArmLeftSmile | frame=animal_hippo_arm_smile.png | pos=[-891, 900, 0] | rot=195 | visible=true
      animations: 4:like(position, rotation, visible)
    - #32 | CCSprite | CCSpriteEatEffect0 | frame=animal_hippo_effect_eat01.png | pos=[2, 99, 0] | visible=false
      animations: 6:eat(position, visible)
    - #33 | CCSprite | CCSpriteEatEffect1 | frame=animal_hippo_effect_eat02.png | pos=[2, 99, 0] | visible=true
      animations: 6:eat(position, visible)
    - #34 | CCSprite | CCSpriteAngryEffect0 | frame=animal_hippo_effect_angry01.png | pos=[-894, 888, 0] | visible=true
      animations: 5:angry(position, visible)
    - #35 | CCSprite | CCSpriteAngryEffect1 | frame=animal_hippo_effect_angry02.png | pos=[-794, 939, 0] | visible=true
      animations: 5:angry(position, visible)
    - #36 | CCSprite | CCSpriteAngryEffect2 | frame=animal_hippo_effect_angry03.png | pos=[-809, 901, 0] | visible=true
      animations: 5:angry(position, visible)
