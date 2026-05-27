# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_tiger\node_animal_tiger.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 18
- spriteCount: 16
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode
  - #1 | CCLayer | CCLayer | pos=[0, 0, 0]
    animations: 1:walkout(position) / 2:walkin(position) / 3:order(position) / 4:like(position)
    - #2 | CCSprite | CCSpriteLegLeft | frame=animal_tiger_leg.png | pos=[16, 51, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 4:like(position, rotation, visible) / 5:angry(visible) / 6:eat(visible)
    - #3 | CCSprite | CCSpriteLegRight | frame=animal_tiger_leg.png | pos=[-22, 50, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 4:like(position, rotation, visible) / 5:angry(visible) / 6:eat(visible)
    - #4 | CCSprite | CCSpriteArmRight2 | frame=animal_tiger_arm_02.png | pos=[15, 93, 0] | rot=0 | visible=false
      animations: 4:like(position, rotation, visible)
    - #5 | CCSprite | CCSpriteArmRight | frame=animal_tiger_arm_01.png | pos=[20, 107, 0] | rot=-30 | visible=false
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 4:like(position, rotation, visible)
    - #6 | CCSprite | CCSpriteBody | frame=animal_tiger_body.png | pos=[-5, 68, 0] | rot=0 | visible=true
      animations: 2:walkin(visible) / 3:order(visible) / 4:like(position, rotation, visible) / 5:angry(visible) / 6:eat(visible)
    - #7 | CCSprite | CCSpriteArmLeft2 | frame=animal_tiger_arm_02.png | pos=[-24, 86, 0] | rot=0 | visible=false
      animations: 4:like(position, rotation, visible)
    - #8 | CCSprite | CCSpriteArmLeft | frame=animal_tiger_arm_01.png | pos=[-30, 104, 0] | rot=30 | visible=false
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 4:like(position, rotation, visible)
    - #9 | CCSprite | CCSpriteArm | frame=animal_tiger_arm_03.png | pos=[0, 77, 0] | rot=0 | visible=false
      animations: 5:angry(position, rotation, visible)
    - #10 | CCSprite | CCSpriteFace | frame=animal_tiger_face_01.png | pos=[-4.200000286102295, 74.80000305175781, 0] | rot=3.000000476837158
      animations: 2:walkin(position) / 4:like(position, rotation) / 5:angry(position, rotation) / 6:eat(position, rotation)
      - #11 | CCSprite | CCSpriteEye | frame=animal_tiger_eye_04.png | pos=[60, 67, 0] | visible=true
        animations: 2:walkin(visible) / 3:order(displayFrame, visible) / 4:like(displayFrame, visible) / 5:angry(displayFrame, visible) / 6:eat(displayFrame, visible)
      - #12 | CCSprite | CCSpriteMouse | frame=animal_tiger_mouse_04.png | pos=[61, 38, 0] | rot=0 | visible=true
        animations: 2:walkin(visible) / 3:order(visible) / 4:like(displayFrame, position, visible) / 5:angry(displayFrame, position, visible) / 6:eat(displayFrame, position, rotation, visible)
    - #13 | CCSprite | CCSpriteArmLeft3 | frame=animal_tiger_arm_04.png | pos=[-66.80000305175781, 104.4000015258789, 0] | rot=-12.000001907348633 | visible=true
      animations: 6:eat(position, rotation, visible)
    - #14 | CCSprite | CCSpriteArmRight3 | frame=animal_tiger_arm_05.png | pos=[60.60000228881836, 67.4000015258789, 0] | rot=-30.000003814697266 | visible=true
      animations: 6:eat(position, rotation, visible)
    - #15 | CCSprite | CCSpriteEffAngry | frame=animal_tiger_angry_0003.png | pos=[68, 213, 0] | visible=false
      animations: 5:angry(displayFrame, visible)
    - #16 | CCSprite | CCSpriteEffEat1 | frame=animal_tiger_eat_effect0002.png | pos=[-16, 108, 0] | visible=true
      animations: 6:eat(displayFrame, visible)
    - #17 | CCSprite | CCSpriteEffEat2 | frame=animal_tiger_eat_effect0002.png | pos=[29, 109, 0] | visible=true
      animations: 6:eat(displayFrame, visible)
