# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\doggy\node_animal_doggy.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 79
- spriteCount: 77
- sequenceCount: 9

## Tree

- #0 | CCNode | CCNode
  - #1 | CCLayer | CCLayerDoggy | pos=[0, 0, 0] | visible=true
    animations: 1:idle(position) / 2:start(position, visible) / 4:fail(position) / 5:perfect(position) / 7:map_hello(position) / 8:map_enter(position) / 10:ready(position, visible)
    - #2 | CCSprite | CCSpriteHatBack2 | frame=doggy_hat_24_1.png | pos=[50, 170, 0] | visible=false
      animations: 1:idle(position) / 2:start(position) / 3:goal(position) / 4:fail(position) / 5:perfect(position) / 7:map_hello(position) / 8:map_enter(position) / 10:ready(position)
    - #3 | CCSprite | CCSpriteHatBack | frame=doggy_cafe_19.png | pos=[6, 147, 0] | visible=false
      animations: 1:idle(position) / 2:start(position) / 3:goal(position) / 4:fail(position) / 5:perfect(position) / 7:map_hello(position) / 8:map_enter(position) / 10:ready(position)
    - #4 | CCSprite | CCSpriteCafeScarf | frame=doggy_cafe_09.png | pos=[4, 97, 0] | visible=false
      animations: 5:perfect(position) / 6:title(position)
    - #5 | CCSprite | CCSpriteCafeCloth | frame=doggy_cafe_10.png | pos=[1, 110, 0] | visible=false
      animations: 5:perfect(position) / 6:title(position)
    - #6 | CCSprite | CCSpriteTail | frame=doggy_tail.png | pos=[29, 44, 0] | rot=0 | visible=true
      animations: 1:idle(position) / 2:start(position, rotation, visible) / 3:goal(position, rotation) / 4:fail(position) / 5:perfect(position, rotation) / 6:title(position) / 7:map_hello(position) / 8:map_enter(position) / 10:ready(position, visible)
    - #7 | CCSprite | CCSpriteLegRight | frame=doggy_leg_18.png | pos=[-19, 61, 0] | rot=0 | visible=true
      animations: 1:idle(position, rotation) / 2:start(position, rotation, visible) / 3:goal(position, rotation) / 4:fail(position, rotation) / 5:perfect(position, rotation) / 6:title(position) / 7:map_hello(position, rotation) / 8:map_enter(position, rotation) / 10:ready(visible)
    - #8 | CCSprite | CCSpriteLegOutLeft | frame=doggy_leg_01.png | pos=[2195, 107, 0] | rot=-50 | visible=true
      animations: 2:start(position, rotation, visible) / 10:ready(position, rotation, visible)
    - #9 | CCSprite | CCSpriteLegLeft | frame=doggy_leg_18.png | pos=[22, 61, 0] | rot=0
      animations: 1:idle(position) / 3:goal(position) / 4:fail(position) / 5:perfect(position, rotation) / 7:map_hello(position) / 8:map_enter(position)
    - #10 | CCSprite | CCSpriteScarpBack | frame=doggy_scarp_15.png | pos=[1, 99, 0]
      animations: 1:idle(scale) / 3:goal(scale) / 4:fail(scale) / 5:perfect(position) / 6:title(position) / 7:map_hello(scale) / 8:map_enter(scale)
    - #11 | CCSprite | CCSpriteArmLeft | frame=doggy_arm_left_18.png | pos=[-19, 96, 0] | rot=110 | visible=true
      animations: 1:idle(position) / 3:goal(position, rotation) / 4:fail(position, rotation) / 5:perfect(position, rotation) / 7:map_hello(position, rotation, visible) / 8:map_enter(position, rotation, visible)
      - #12 | CCSprite | CCSpriteHand | frame=doggy_best_hand.png | pos=[1890.98876953125, 1182.1539306640625, 0] | rot=-70
        animations: 3:goal(position, rotation)
    - #13 | CCSprite | CCSpriteBodyBack | frame=doggy_body_18_1.png | pos=[55, 119, 0] | visible=false
      animations: 1:idle(position) / 2:start(position) / 3:goal(position) / 4:fail(position) / 5:perfect(position) / 6:title(position) / 7:map_hello(position) / 8:map_enter(position) / 10:ready(position)
    - #14 | CCSprite | CCSpriteBody | frame=doggy_body_18.png | pos=[2, 77, 0]
      animations: 1:idle(position) / 3:goal(position) / 4:fail(position) / 5:perfect(position) / 7:map_hello(position) / 8:map_enter(position)
    - #15 | CCSprite | CCSpriteApron | frame=doggy_apron_15.png | pos=[2, 68, 0]
      animations: 1:idle(position) / 3:goal(position) / 4:fail(position) / 5:perfect(position) / 7:map_hello(position) / 8:map_enter(position)
    - #16 | CCSprite | CCSpriteArmRightOut | frame=doggy_arm.png | pos=[2211, 194, 0] | rot=-130 | visible=true
      animations: 8:map_enter(position, visible)
    - #17 | CCSprite | CCSpriteArmRight | frame=doggy_arm_18.png | pos=[29, 128, 0] | rot=-25 | visible=true
      animations: 1:idle(position) / 3:goal(position, rotation) / 4:fail(position, rotation) / 5:perfect(position, rotation) / 7:map_hello(position, visible)
      - #18 | CCSprite | CCSprite | frame=doggy_best_hand.png | pos=[1917.8560791015625, -814.9886474609375, 0] | rot=100
        animations: 3:goal(position, rotation)
    - #19 | CCSprite | CCSpriteScarp | frame=doggy_scarp_09.png | pos=[10, 107, 0] | visible=false
      animations: 1:idle(scale) / 3:goal(scale) / 4:fail(scale) / 5:perfect(position) / 6:title(position) / 7:map_hello(scale) / 8:map_enter(scale)
    - #20 | CCSprite | CCSpriteFace | frame=doggy_face.png | pos=[5, 107, 0] | rot=0 | visible=true
      animations: 1:idle(position) / 2:start(rotation, visible) / 3:goal(position) / 4:fail(position) / 5:perfect(position) / 6:title(position) / 7:map_hello(position) / 8:map_enter(position) / 10:ready(rotation, visible)
      - #21 | CCSprite | CCSpriteMouseCry_0 | frame=doggy_mouse_cry_01.png | pos=[2184, 19, 0] | visible=true
        animations: 4:fail(position, visible)
      - #22 | CCSprite | CCSpriteMouseCry_1 | frame=doggy_mouse_cry_02.png | pos=[2182, 25, 0] | visible=true
        animations: 4:fail(position, visible)
      - #23 | CCSprite | CCSpriteEyeCry_0 | frame=doggy_eye_cry_01.png | pos=[2196, 86, 0] | visible=true
        animations: 4:fail(position, visible)
      - #24 | CCSprite | CCSpriteEyeCry_1 | frame=doggy_eye_cry_02.png | pos=[2662, 162, 0] | visible=true
        animations: 4:fail(position, visible)
      - #25 | CCSprite | CCSpriteEyeCry_2 | frame=doggy_eye_cry_03.png | pos=[2132, 96, 0] | visible=true
        animations: 4:fail(position, visible)
      - #26 | CCSprite | CCSpriteEyeCry_3 | frame=doggy_eye_cry_04.png | pos=[2256, 210, 0] | visible=true
        animations: 4:fail(position, visible)
      - #27 | CCSprite | CCSpriteEyeGood_0 | frame=doggy_eye_good_01.png | pos=[2210, -47, 0] | visible=true
        animations: 5:perfect(position, visible)
      - #28 | CCSprite | CCSpriteEyeGood_1 | frame=doggy_eye_goodeye_02.png | pos=[2197, 41, 0] | visible=true
        animations: 5:perfect(position, visible)
      - #29 | CCSprite | CCSpriteEyeGoal | frame=doggy_eye_02.png | pos=[2237, 18, 0] | visible=true
        animations: 3:goal(position) / 8:map_enter(position, visible)
      - #30 | CCSprite | CCSpriteEye | frame=doggy_eye_01.png | pos=[1219, 62, 0] | rot=0 | visible=true
        animations: 1:idle(position) / 2:start(position, rotation, visible) / 10:ready(position, rotation, visible)
      - #31 | CCSprite | CCSpriteEyeMap_0 | frame=doggy_eye_03.png | pos=[54, 70, 0] | visible=true
        animations: 7:map_hello(position, visible)
      - #32 | CCSprite | CCSpriteEyeMap_1 | frame=doggy_eye_map_01.png | pos=[54, 70, 0] | visible=false
        animations: 7:map_hello(position, visible)
      - #33 | CCSprite | CCSpriteEyeMap_2 | frame=doggy_eye_map_02.png | pos=[54, 70, 0] | visible=false
        animations: 7:map_hello(position, visible)
      - #34 | CCSprite | CCSpriteMouseMap | frame=doggy_mouse_map.png | pos=[46, 32, 0] | visible=true
        animations: 7:map_hello(position, visible)
      - #35 | CCSprite | CCSpriteMouse | frame=doggy_mouse_01.png | pos=[1793, 300, 0]
        animations: 1:idle(position)
      - #36 | CCSprite | CCSpriteMouseStart | frame=doggy_mouse_03.png | pos=[2171, -82, 0] | rot=0 | visible=true
        animations: 2:start(position, rotation, visible)
      - #37 | CCSprite | CCSpriteMouseSmile | frame=doggy_mouse_04.png | pos=[2302, 69, 0] | rot=0 | visible=true
        animations: 2:start(position, rotation, visible) / 5:perfect(position, visible) / 8:map_enter(position, visible) / 10:ready(position, rotation, visible)
      - #38 | CCSprite | CCSpriteHat | frame=doggy_hat_24.png | pos=[71, 103, 0]
      - #39 | CCSprite | CCSpriteMouseGoal | frame=doggy_mouse_02.png | pos=[2132, 31, 0]
        animations: 3:goal(position)
      - #40 | CCSprite | CCSpriteTul | frame=doggy_scarp_12.png | pos=[75, 30, 0] | visible=false
        animations: 1:idle(position) / 2:start(position) / 3:goal(position) / 4:fail(position) / 5:perfect(position) / 7:map_hello(position) / 8:map_enter(position) / 10:ready(position)
    - #41 | CCSprite | CCSpriteArmRightOutHand | frame=doggy_hand_map.png | pos=[2215, 230, 0] | rot=0 | visible=true
      animations: 8:map_enter(position, rotation, visible)
    - #42 | CCSprite | CCSpriteLeftHand | frame=doggy_hand_map.png | pos=[2195, 138, 0] | rot=-20 | visible=true
      animations: 8:map_enter(position, rotation, visible)
    - #43 | CCSprite | CCSpriteOpenGlow | frame=doggy_open_glow.png | pos=[2326, -17, 0]
      animations: 10:ready(position)
      - #44 | CCSprite | CCSprite | frame=doggy_open_indicator.png | pos=[250, 44, 0]
      - #45 | CCSprite | CCSprite | frame=doggy_open_indicator.png | pos=[-44, 42, 0]
    - #46 | CCSprite | CCSpriteOpen | frame=doggy_open.png | pos=[2142, 88, 0] | rot=0
      animations: 2:start(position, rotation) / 10:ready(position, rotation)
    - #47 | CCSprite | CCSpriteTwinkle_01 | frame=doggy_twinkle_01.png | pos=[2170, 240, 0] | visible=true
      animations: 3:goal(position, visible)
    - #48 | CCSprite | CCSpriteTwinkle_02 | frame=doggy_twinkle_02.png | pos=[2168, 230, 0] | visible=true
      animations: 3:goal(position, visible)
    - #49 | CCSprite | CCSpriteTwinkle_03 | frame=doggy_twinkle_03.png | pos=[2210, 320, 0] | visible=true
      animations: 3:goal(position, visible)
    - #50 | CCSprite | CCSpriteTwinkle_04 | frame=doggy_twinkle_04.png | pos=[2200, 134, 0] | visible=true
      animations: 3:goal(position, visible)
    - #51 | CCSprite | CCSpriteTwinkle_05 | frame=doggy_twinkle_05.png | pos=[2289, 37, 0] | visible=true
      animations: 3:goal(visible)
    - #52 | CCSprite | CCSpriteCryEffect_Left_0 | frame=doggy_cry_effect_01.png | pos=[2308, -50, 0] | visible=true
      animations: 4:fail(position, visible)
    - #53 | CCSprite | CCSpriteCryEffect_Left_1 | frame=doggy_cry_effect_02.png | pos=[2140, -68, 0] | visible=true
      animations: 4:fail(position, visible)
    - #54 | CCSprite | CCSpriteCryEffect_Left_2 | frame=doggy_cry_effect_03.png | pos=[2248, 172, 0] | visible=true
      animations: 4:fail(position, visible)
    - #55 | CCSprite | CCSpriteCryEffect_Right_0 | frame=doggy_cry_effect_01.png | pos=[2080, 116, 0] | visible=true
      animations: 4:fail(position, visible)
    - #56 | CCSprite | CCSpriteCryEffect_Right_1 | frame=doggy_cry_effect_02.png | pos=[2104, 92, 0] | visible=true
      animations: 4:fail(position, visible)
    - #57 | CCSprite | CCSpriteCryEffect_Right_2 | frame=doggy_cry_effect_03.png | pos=[2160, 98, 0] | visible=true
      animations: 4:fail(position, visible)
    - #58 | CCSprite | CCSpritePerfectEffect_1 | frame=doggy_heart_01.png | pos=[2198, 75, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #59 | CCSprite | CCSpritePerfectEffect_2 | frame=doggy_heart_02.png | pos=[2216, 265, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #60 | CCSprite | CCSpritePerfectEffect_3 | frame=doggy_heart_03.png | pos=[2206, 229, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #61 | CCSprite | CCSpritePerfectEffect_4 | frame=doggy_heart_04.png | pos=[2166, 105, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #62 | CCSprite | CCSpritePerfectEffect_5 | frame=doggy_heart_05.png | pos=[2044, 193, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #63 | CCSprite | CCSpritePerfectEffect_6 | frame=doggy_heart_06.png | pos=[2128, 107, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #64 | CCSprite | CCSpritePerfectEffect_7 | frame=doggy_heart_07.png | pos=[2298, 219, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #65 | CCSprite | CCSpritePerfectEffect_8 | frame=doggy_heart_08.png | pos=[2062, 227, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #66 | CCSprite | CCSpritePerfectEffect_9 | frame=doggy_heart_09.png | pos=[2322, 185, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #67 | CCSprite | CCSpritePerfectEffect_10 | frame=doggy_heart_10.png | pos=[2174, 201, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #68 | CCSprite | CCSpritePerfectEffect_11 | frame=doggy_heart_11.png | pos=[2082, -85, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #69 | CCSprite | CCSpritePerfectEffect_12 | frame=doggy_heart_12.png | pos=[2132, 211, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #70 | CCSprite | CCSpritePerfectEffect_13 | frame=doggy_heart_13.png | pos=[2150, 257, 0] | visible=true
      animations: 5:perfect(position, visible)
    - #71 | CCSprite | CCSpriteHatBack2 | frame=doggy_hat_24_1.png | pos=[2461, 400, 0] | visible=false
      animations: 6:title(position)
    - #72 | CCSprite | CCSpriteHatBack | frame=doggy_cafe_19.png | pos=[2461, 400, 0] | visible=false
      animations: 6:title(position)
    - #73 | CCSprite | CCSpriteFaceTitle | frame=doggy_face_2.png | pos=[2180, 144, 0]
      animations: 6:title(position)
      - #74 | CCSprite | CCSpriteEye_0 | frame=doggy_eye_title_01.png | pos=[84, 90, 0] | visible=true
        animations: 6:title(position, visible)
      - #75 | CCSprite | CCSpriteEye_1 | frame=doggy_eye_title_02.png | pos=[84, 90, 0] | visible=true
        animations: 6:title(position, visible)
      - #76 | CCSprite | CCSpriteEye_2 | frame=doggy_eye_title_03.png | pos=[84, 90, 0] | visible=true
        animations: 6:title(position, visible)
      - #77 | CCSprite | CCSpriteHat | frame=doggy_hat_24.png | pos=[101, 126, 0]
      - #78 | CCSprite | CCSpriteTul | frame=doggy_scarp_12.png | pos=[105, 51, 0] | visible=false
        animations: 1:idle(position) / 2:start(position) / 3:goal(position) / 4:fail(position) / 5:perfect(position)
