# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_cat\node_animal_cat.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 39
- spriteCount: 37
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode | visible=true
  - #1 | CCLayer | CCLayer | pos=[0, 0, 0] | visible=true
    animations: 1:walkout(position) / 2:walkin(position) / 3:order(position) / 4:like(position)
    - #2 | CCSprite | CCSpriteArmLeftAnrgry | frame=animal_cat_arm_angry.png | pos=[-781, 520, 0] | rot=-10 | visible=true
      animations: 5:angry(position, visible)
    - #3 | CCSprite | CCSpriteLegRight | frame=animal_cat_leg.png | pos=[16, 51, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, visible) / 6:eat(position, visible)
    - #4 | CCSprite | CCSpriteLegLeft | frame=animal_cat_leg.png | pos=[-16, 50, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position) / 6:eat(position, visible)
    - #5 | CCSprite | CCSpriteArmRight | frame=animal_cat_arm_normal.png | pos=[-31, 106, 0] | rot=-35 | visible=true
      animations: 1:walkout(position, rotation, visible) / 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible)
    - #6 | CCSprite | CCSpriteLegOutRight | frame=animal_cat_leg.png | pos=[-704, 508, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #7 | CCSprite | CCSpriteLegOutLeft | frame=animal_cat_leg.png | pos=[-710, 494, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #8 | CCSprite | CCSpriteBodyOut | frame=animal_cat_body.png | pos=[-704, 480, 0] | visible=true
      animations: 1:walkout(position, visible)
    - #9 | CCSprite | CCSpriteFaceOut | frame=animal_cat_face_normal.png | pos=[-755, 496, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
      - #10 | CCSprite | CCSpriteEye | frame=animal_cat_eye_normal.png | pos=[55, 53, 0] | visible=true
      - #11 | CCSprite | CCSpriteMouse | frame=animal_cat_mouth_normal.png | pos=[54, 25, 0] | visible=true
    - #12 | CCSprite | CCSpriteBody | frame=animal_cat_body.png | pos=[-4, 66, 0] | visible=true
      animations: 2:walkin(position, visible) / 3:order(position, visible) / 4:like(position, visible) / 5:angry(position) / 6:eat(position, visible)
    - #13 | CCSprite | CCSpriteArmLeft | frame=animal_cat_arm_normal.png | pos=[-701, 511, 0] | rot=35 | visible=true
      animations: 1:walkout(position, rotation, visible) / 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible)
    - #14 | CCSprite | CCSpriteFaceAngry_0 | frame=animal_cat_face_angry01.png | pos=[-830, 489, 0] | rot=5 | visible=true
      animations: 5:angry(position, visible)
      - #15 | CCSprite | CCSpriteMouseAngry | frame=animal_cat_mouth_angry_01.png | pos=[63, 16, 0] | visible=true
      - #16 | CCSprite | CCSpriteEyeAngry | frame=animal_cat_eye_angry.png | pos=[62, 44, 0] | visible=true
    - #17 | CCSprite | CCSpriteFaceAngry_1 | frame=animal_cat_face_angry02.png | pos=[-758, 485, 0] | rot=5 | visible=true
      animations: 5:angry(position, visible)
      - #18 | CCSprite | CCSpriteEyeAngry | frame=animal_cat_eye_angry.png | pos=[62, 44, 0] | visible=true
      - #19 | CCSprite | CCSpriteMouseAngry | frame=animal_cat_mouth_angry_01.png | pos=[63, 16, 0] | visible=true
    - #20 | CCSprite | CCSpriteFace | frame=animal_cat_face_normal.png | pos=[-3, 77, 0] | visible=true
      animations: 2:walkin(position, visible) / 3:order(position, visible) / 4:like(position, visible) / 6:eat(position, visible)
      - #21 | CCSprite | CCSpriteEye | frame=animal_cat_eye_normal.png | pos=[61, 53, 0] | visible=true
        animations: 2:walkin(position, visible) / 3:order(position, visible) / 6:eat(position, visible)
      - #22 | CCSprite | CCSpriteEyeClose | frame=animal_cat_eye_close.png | pos=[-740, 403, 0] | visible=true
        animations: 3:order(position, visible) / 4:like(position, visible)
      - #23 | CCSprite | CCSpriteMouseNormal | frame=animal_cat_mouth_normal.png | pos=[-702, 392, 0] | visible=true
        animations: 2:walkin(position, visible) / 3:order(position, visible)
      - #24 | CCSprite | CCSpriteMouseTung0 | frame=animal_cat_tung01.png | pos=[-711, 397, 0] | visible=true
        animations: 3:order(position, visible)
      - #25 | CCSprite | CCSpriteMouseTung1 | frame=animal_cat_tung02.png | pos=[-713, 395, 0] | visible=true
        animations: 3:order(position, visible)
      - #26 | CCSprite | CCSpriteMouseTung2 | frame=animal_cat_tung03.png | pos=[-698, 407, 0] | visible=true
        animations: 3:order(position, visible)
      - #27 | CCSprite | CCSpriteMouseTung3 | frame=animal_cat_tung04.png | pos=[-726, 372, 0] | visible=true
        animations: 3:order(position, visible)
      - #28 | CCSprite | CCSpriteMouseTung4 | frame=animal_cat_tung05.png | pos=[-722, 404, 0] | visible=true
        animations: 3:order(position, visible)
      - #29 | CCSprite | CCSpriteMouseSmile | frame=animal_cat_mouth_smile.png | pos=[-769, 392, 0] | visible=true
        animations: 4:like(position, visible)
      - #30 | CCSprite | CCSpriteMouseEat_0 | frame=animal_cat_mouth_eat01.png | pos=[63, 27, 0] | visible=true
        animations: 6:eat(position, visible)
      - #31 | CCSprite | CCSpriteMouseEat_1 | frame=animal_cat_mouth_eat02.png | pos=[63, 27, 0] | visible=false
        animations: 6:eat(position, visible)
    - #32 | CCSprite | CCSpriteArmLeftAnrgry | frame=animal_cat_arm_angry.png | pos=[-753, 507, 0] | rot=10 | visible=true
      animations: 5:angry(position, visible)
    - #33 | CCSprite | CCSpriteArmLeftEat | frame=animal_cat_arm_eat01.png | pos=[-43, 63, 0] | rot=-140 | visible=true
      animations: 6:eat(position, visible)
    - #34 | CCSprite | CCSpriteArmRightEat | frame=animal_cat_arm_eat02.png | pos=[44, 64, 0] | rot=140 | visible=true
      animations: 6:eat(position, visible)
    - #35 | CCSprite | CCSpriteSpitLeft_0 | frame=animal_effect_eat2_01.png | pos=[-35, 97, 0] | visible=true
      animations: 6:eat(position, visible)
    - #36 | CCSprite | CCSpriteSpitLeft_1 | frame=animal_effect_eat2_02.png | pos=[-35, 97, 0] | visible=false
      animations: 6:eat(position, visible)
    - #37 | CCSprite | CCSpriteSpitRight_0 | frame=animal_effect_eat2_01.png | pos=[39, 97, 0] | visible=true
      animations: 6:eat(position, visible)
    - #38 | CCSprite | CCSpriteSpitRight_1 | frame=animal_effect_eat2_02.png | pos=[39, 97, 0] | visible=false
      animations: 6:eat(position, visible)
