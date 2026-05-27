# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_lion\node_animal_lion.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 21
- spriteCount: 19
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode
  - #1 | CCLayer | CCLayer | pos=[0, 0, 0]
    animations: 1:walkout(position) / 2:walkin(position) / 3:order(position) / 4:like(position)
    - #2 | CCSprite | CCSpriteHair | frame=animal_lion_hair_03.png | pos=[-7, 92, 0] | rot=0
      animations: 2:walkin(position) / 3:order(position) / 4:like(position) / 5:angry(displayFrame, position) / 6:eat(position, rotation)
    - #3 | CCSprite | CCSpriteLegLeft | frame=animal_lion_leg_01.png | pos=[13, 55, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 4:like(visible) / 5:angry(visible) / 6:eat(visible)
    - #4 | CCSprite | CCSpriteArmLeft | frame=animal_lion_arm_01.png | pos=[24, 84, 0] | rot=-140 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 5:angry(position, rotation, visible)
    - #5 | CCSprite | CCSpriteLegRight | frame=animal_lion_leg_01.png | pos=[-19, 55, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 4:like(visible) / 5:angry(visible) / 6:eat(visible)
    - #6 | CCSprite | CCSpriteArmLeft2 | frame=animal_lion_arm_02.png | pos=[41, 102, 0] | visible=false
      animations: 4:like(position, visible) / 6:eat(visible)
    - #7 | CCSprite | CCSpriteBody | frame=animal_lion_body_01.png | pos=[-2, 68, 0]
      animations: 4:like(position) / 5:angry(position)
    - #8 | CCSprite | CCSpriteArmRight2 | frame=animal_lion_arm_02.png | pos=[-44, 99, 0] | visible=false
      animations: 4:like(position, visible)
    - #9 | CCSprite | CCSpriteArmLeft2 | frame=animal_lion_arm_02.png | pos=[44, 100, 0] | visible=false
      animations: 4:like(position, visible)
    - #10 | CCSprite | CCSpriteHand | frame=animal_lion_hand_01.png | pos=[0, 71, 0] | visible=false
      animations: 4:like(position, visible)
    - #11 | CCSprite | CCSpriteFace | frame=animal_lion_face_02.png | pos=[-4, 91, 0] | rot=0
      animations: 4:like(position) / 5:angry(displayFrame, position) / 6:eat(position, rotation)
      - #12 | CCSprite | CCSpriteEye | frame=animal_lion_eye_03.png | pos=[54, 45, 0] | visible=true
        animations: 2:walkin(displayFrame, visible) / 3:order(displayFrame, visible) / 4:like(displayFrame, visible) / 5:angry(displayFrame, position, visible) / 6:eat(displayFrame, visible)
      - #13 | CCSprite | CCSpriteMouse | frame=animal_lion_mouse_04.png | pos=[54, 40, 0] | rot=0
        animations: 2:walkin(displayFrame, position) / 3:order(displayFrame, position) / 4:like(displayFrame, position) / 5:angry(displayFrame, position, rotation) / 6:eat(displayFrame, position)
    - #14 | CCSprite | CCSpriteArmRight | frame=animal_lion_arm_01.png | pos=[-31, 77, 0] | rot=140 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 5:angry(position, rotation, visible)
    - #15 | CCSprite | CCSpriteArmRight3 | frame=animal_lion_arm_02.png | pos=[-46, 97, 0] | rot=0 | visible=false
      animations: 6:eat(position, rotation, visible)
    - #16 | CCSprite | CCSpriteArmLeft3 | frame=animal_lion_arm_03.png | pos=[38, 65, 0] | rot=-45 | visible=false
      animations: 6:eat(position, rotation, visible)
    - #17 | CCSprite | CCSpriteEffectSmile | frame=animal_lion_smile_effect_02.png | pos=[73, 181, 0] | visible=false
      animations: 4:like(displayFrame, position, visible)
    - #18 | CCSprite | CCSpriteEffectEat1 | frame=animal_lion_eat_effect0002.png | pos=[-18, 99, 0] | visible=false
      animations: 6:eat(displayFrame, visible)
    - #19 | CCSprite | CCSpriteEffectEat2 | frame=animal_lion_eat_effect0001.png | pos=[28, 103, 0] | visible=false
      animations: 6:eat(displayFrame, visible)
    - #20 | CCSprite | CCSpriteEffectAngry | frame=animal_lion_angry_03.png | pos=[-4, 258, 0] | visible=false
      animations: 5:angry(displayFrame, position, visible)
