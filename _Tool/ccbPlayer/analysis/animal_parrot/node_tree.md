# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_parrot\node_animal_parrot.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 46
- spriteCount: 44
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode | visible=true
  - #1 | CCLayer | CCLayerParrot | pos=[0, 0, 0] | visible=true
    animations: 1:walkout(position, visible) / 2:walkin(position, visible) / 3:order(position, visible) / 4:like(visible) / 5:angry(position, visible) / 6:eat(position, visible)
    - #2 | CCSprite | CCSpriteArmRight | frame=animal_parrot_arm.png | pos=[-734, 859, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible) / 3:order(position, rotation, visible)
    - #3 | CCSprite | CCSpriteArmAngryRight | frame=animal_parrot_arm_angry.png | pos=[-721, 796, 0] | rot=0 | visible=true
      animations: 5:angry(position, rotation, visible)
    - #4 | CCSprite | CCSpriteArmOutRight | frame=animal_parrot_arm.png | pos=[18, 102, 0] | rot=-60 | visible=true
      animations: 2:walkin(position, rotation, visible) / 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #5 | CCSprite | CCSpriteLegRight | frame=animal_parrot_leg.png | pos=[18, 42, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #6 | CCSprite | CCSpriteLegOutLeft | frame=animal_parrot_leg.png | pos=[-755, 827, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #7 | CCSprite | CCSpriteBodyOut | frame=animal_parrot_body.png | pos=[-755, 805, 0] | visible=true
      animations: 1:walkout(position, visible)
    - #8 | CCSprite | CCSpriteBody | frame=animal_parrot_body.png | pos=[-7, 73, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #9 | CCSprite | CCSpriteLegOutRight | frame=animal_parrot_leg.png | pos=[-723, 811, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #10 | CCSprite | CCSpriteLegLeft | frame=animal_parrot_leg.png | pos=[-18, 40, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #11 | CCSprite | CCSpriteArmAngryLeft | frame=animal_parrot_arm_angry.png | pos=[-668, 809, 0] | rot=0 | visible=true
      animations: 5:angry(position, rotation, visible)
    - #12 | CCSprite | CCSpriteHeadOut | frame=animal_parrot_head_normal.png | pos=[-787, 772, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
      - #13 | CCSprite | CCSpriteEye | frame=animal_parrot_eye_normal.png | pos=[62, 49, 0] | rot=0 | visible=true
        animations: 1:walkout(position, rotation, visible)
      - #14 | CCSprite | CCSpriteMouse | frame=animal_parrot_mouth_normal.png | pos=[31, 37, 0] | rot=0 | visible=true
        animations: 1:walkout(position, rotation, visible)
    - #15 | CCSprite | CCSpriteHeadAngry | frame=animal_parrot_head_angry.png | pos=[-797, 849, 0] | rot=0 | visible=true
      animations: 5:angry(position, rotation, visible)
      - #16 | CCSprite | CCSpriteEyeAngry | frame=animal_parrot_eye_angry.png | pos=[-274.3497009277344, 13.862504959106445, 0] | visible=true
        animations: 5:angry(position, visible)
      - #17 | CCSprite | CCSpriteMouse | frame=animal_parrot_mouth_normal.png | pos=[-259, -113, 0] | rot=0 | visible=true
        animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 5:angry(position, visible)
    - #18 | CCSprite | CCSpriteHead | frame=animal_parrot_head_normal.png | pos=[10, 84, 0] | rot=-20 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
      - #19 | CCSprite | CCSpriteEye | frame=animal_parrot_eye_normal.png | pos=[-392.10675048828125, 916.2512817382812, 0] | rot=0 | visible=true
        animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible)
      - #20 | CCSprite | CCSpriteEyeAngry | frame=animal_parrot_eye_angry.png | pos=[-448.4414978027344, 917.4393920898438, 0] | visible=true
      - #21 | CCSprite | CCSpriteEyeClose | frame=animal_parrot_eye_close.png | pos=[-407.0976257324219, 908.12158203125, 0] | visible=true
        animations: 3:order(position, visible)
      - #22 | CCSprite | CCSpriteEyeSmile | frame=animal_parrot_eye_smile.png | pos=[41, 49, 0] | rot=0 | visible=true
        animations: 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
      - #23 | CCSprite | CCSpriteMouse | frame=animal_parrot_mouth_normal.png | pos=[-392.9697265625, 925.3590087890625, 0] | rot=0 | visible=true
        animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible)
      - #24 | CCSprite | CCSpriteMouseSmile | frame=animal_parrot_mouth_smile.png | pos=[-446.77655029296875, 923.827392578125, 0] | rot=0 | visible=true
        animations: 4:like(position, rotation, visible)
      - #25 | CCSprite | CCSpriteMouseEat1 | frame=animal_parrot_mouth_eat01.png | pos=[74, 39, 0] | rot=0 | visible=true
        animations: 6:eat(position, rotation, visible)
      - #26 | CCSprite | CCSpriteMouseEat1 | frame=animal_parrot_mouth_eat02.png | pos=[74, 39, 0] | rot=0 | visible=false
        animations: 6:eat(position, rotation, visible)
    - #27 | CCSprite | CCSpriteArmLeft | frame=animal_parrot_arm.png | pos=[-723, 761, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #28 | CCSprite | CCSpriteArmOutLeft | frame=animal_parrot_arm.png | pos=[-32, 99, 0] | rot=50 | visible=true
      animations: 1:walkout(position, rotation, visible) / 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #29 | CCSprite | CCSpriteEatEffect_0 | frame=animal_parrot_effect_eat_01.png | pos=[63, 119, 0] | visible=true
      animations: 6:eat(position, visible)
    - #30 | CCSprite | CCSpriteEatEffect_1 | frame=animal_parrot_effect_eat_02.png | pos=[63, 119, 0] | visible=false
      animations: 6:eat(position, visible)
    - #31 | CCSprite | CCSpriteEatEffect_2 | frame=animal_parrot_effect_eat_03.png | pos=[63, 119, 0] | visible=false
      animations: 6:eat(position, visible)
    - #32 | CCSprite | CCSpriteEatEffect_3 | frame=animal_parrot_effect_eat_04.png | pos=[63, 119, 0] | visible=false
      animations: 6:eat(position, visible)
    - #33 | CCSprite | CCSpriteEatEffect_4 | frame=animal_parrot_effect_eat_05.png | pos=[63, 119, 0] | visible=false
      animations: 6:eat(position, visible)
    - #34 | CCSprite | CCSpriteLikeEffect_0 | frame=animal_parrot_music_0001.png | pos=[-702, 764, 0] | visible=true
      animations: 4:like(position, visible)
    - #35 | CCSprite | CCSpriteLikeEffect_1 | frame=animal_parrot_music_0002.png | pos=[-735, 802, 0] | visible=true
      animations: 4:like(position, visible)
    - #36 | CCSprite | CCSpriteLikeEffect_2 | frame=animal_parrot_music_0003.png | pos=[-703, 842, 0] | visible=true
      animations: 4:like(position, visible)
    - #37 | CCSprite | CCSpriteLikeEffect_3 | frame=animal_parrot_music_0004.png | pos=[-692, 803, 0] | visible=true
      animations: 4:like(position, visible)
    - #38 | CCSprite | CCSpriteLikeEffect_4 | frame=animal_parrot_music_0005.png | pos=[-767, 807, 0] | visible=true
      animations: 4:like(position, visible)
    - #39 | CCSprite | CCSpriteLikeEffect_5 | frame=animal_parrot_music_0006.png | pos=[-742, 777, 0] | visible=true
      animations: 4:like(position, visible)
    - #40 | CCSprite | CCSpriteLikeEffect_6 | frame=animal_parrot_music_0007.png | pos=[-800, 858, 0] | visible=true
      animations: 4:like(position, visible)
    - #41 | CCSprite | CCSpriteLikeEffect_7 | frame=animal_parrot_music_0008.png | pos=[-720, 857, 0] | visible=true
      animations: 4:like(position, visible)
    - #42 | CCSprite | CCSpriteLikeEffect_8 | frame=animal_parrot_music_0009.png | pos=[-713, 824, 0] | visible=true
      animations: 4:like(position, visible)
    - #43 | CCSprite | CCSpriteAngryEffect_0 | frame=animal_parrot_effect_angry_01.png | pos=[-777, 804, 0] | visible=true
      animations: 5:angry(position, visible)
    - #44 | CCSprite | CCSpriteAngryEffect_1 | frame=animal_parrot_effect_angry_02.png | pos=[-669, 832, 0] | visible=true
      animations: 5:angry(position, visible)
    - #45 | CCSprite | CCSpriteAngryEffect_2 | frame=animal_parrot_effect_angry_03.png | pos=[-680, 888, 0] | visible=true
      animations: 5:angry(position, visible)
