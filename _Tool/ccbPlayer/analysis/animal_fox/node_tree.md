# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_fox\node_animal_fox.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 20
- spriteCount: 18
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode
  - #1 | CCLayer | CCLayer | pos=[0, 0, 0]
    animations: 1:walkout(position) / 2:walkin(position) / 3:order(position) / 4:like(position)
    - #2 | CCSprite | CCSpriteTail2 | frame=animal_fox_tail.png | pos=[5, 44, 0] | rot=-80 | visible=false
      animations: 5:angry(position, rotation, visible)
    - #3 | CCSprite | CCSpriteTail | frame=animal_fox_tail.png | pos=[-7, 61, 0] | rot=0
      animations: 2:walkin(position, rotation) / 3:order(position, rotation) / 4:like(position, rotation) / 5:angry(position, rotation) / 6:eat(position, rotation)
    - #4 | CCSprite | CCSpriteLegLeft | frame=animal_fox_leg.png | pos=[13, 48, 0] | rot=0
      animations: 2:walkin(position, rotation) / 4:like(position, rotation)
    - #5 | CCSprite | CCSpriteLegRight | frame=animal_fox_leg.png | pos=[-17, 47, 0] | rot=0
      animations: 2:walkin(position, rotation) / 4:like(position, rotation)
    - #6 | CCSprite | CCSpriteArmLeft2 | frame=animal_fox_arm_01.png | pos=[9, 100, 0] | rot=-100 | visible=false
      animations: 4:like(position, rotation, visible)
    - #7 | CCSprite | CCSpriteArmLeft | frame=animal_fox_arm_01.png | pos=[13, 102, 0] | rot=30 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 6:eat(position, rotation, visible)
    - #8 | CCSprite | CCSpriteBody | frame=animal_fox_body.png | pos=[-4, 60, 0] | rot=0
      animations: 4:like(position, rotation)
    - #9 | CCSprite | CCSpriteArmRight2 | frame=animal_fox_arm_01.png | pos=[-17, 101, 0] | rot=100 | visible=false
      animations: 4:like(position, rotation, visible)
    - #10 | CCSprite | CCSpriteArmRight | frame=animal_fox_arm_01.png | pos=[-8, 86, 0] | rot=20 | visible=false
      animations: 2:walkin(position, rotation, visible) / 3:order(visible)
    - #11 | CCSprite | CCSpriteFace | frame=animal_fox_face.png | pos=[-6, 74, 0] | rot=10
      animations: 2:walkin(position, rotation) / 3:order(position, rotation) / 4:like(position, rotation) / 5:angry(position) / 6:eat(position, rotation)
      - #12 | CCSprite | CCSpriteEye | frame=animal_fox_eye_05.png | pos=[50, 41, 0]
        animations: 4:like(displayFrame) / 5:angry(displayFrame)
      - #13 | CCSprite | CCSpriteMouse | frame=animal_fox_mouse_03.png | pos=[52, 14, 0]
        animations: 4:like(displayFrame) / 5:angry(displayFrame) / 6:eat(displayFrame)
    - #14 | CCSprite | CCSpriteArm | frame=animal_fox_arm_05.png | pos=[-57, 95, 0] | rot=0 | visible=true
      animations: 6:eat(position, rotation, visible)
    - #15 | CCSprite | CCSpriteArm | frame=animal_fox_arm_06.png | pos=[41, 50, 0] | rot=-45 | visible=true
      animations: 6:eat(position, rotation, visible)
    - #16 | CCSprite | CCSpriteArmAngry | frame=animal_fox_arm_03.png | pos=[-22, 64, 0] | rot=10 | visible=false
      animations: 5:angry(displayFrame, position, rotation, visible)
    - #17 | CCSprite | CCSpriteArmAngry | frame=animal_fox_arm_02.png | pos=[17, 49, 0] | rot=0 | visible=false
      animations: 5:angry(displayFrame, position, rotation, visible)
    - #18 | CCSprite | CCSpriteEffEat | frame=animal_fox_eat_effect0002.png | pos=[-18, 93, 0] | visible=true
      animations: 6:eat(displayFrame, position, visible)
    - #19 | CCSprite | CCSpriteEffEat | frame=animal_fox_eat_effect0002.png | pos=[26, 91, 0] | rot=0 | visible=true
      animations: 6:eat(displayFrame, position, rotation, visible)
