# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_sister\node_animal_sister.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 32
- spriteCount: 30
- sequenceCount: 5

## Tree

- #0 | CCNode | CCNode
  - #1 | CCLayer | CCLayer | pos=[8, 0, 0]
    animations: 1:walkout(position) / 2:walkin(position) / 3:order(position) / 4:like(position)
    - #2 | CCSprite | CCSpriteTail | frame=animal_sister_tail.png | pos=[-25, 29, 0] | rot=-20
      animations: 2:walkin(position, rotation) / 3:order(position, rotation) / 4:like(position, rotation) / 6:eat(position)
    - #3 | CCSprite | CCSpriteLeg | frame=animal_sister_leg.png | pos=[9, 31, 0] | rot=0
      animations: 2:walkin(position, rotation) / 3:order(position, rotation) / 4:like(position) / 6:eat(position)
    - #4 | CCSprite | CCSpriteLeg | frame=animal_sister_leg.png | pos=[-18, 30, 0] | rot=0
      animations: 2:walkin(position, rotation) / 3:order(position, rotation) / 4:like(position) / 6:eat(position)
    - #5 | CCSprite | CCSpriteArm | frame=animal_sister_arm_normal.png | pos=[-317, 49, 0] | rot=-30
      animations: 2:walkin(position, rotation) / 3:order(position, rotation) / 4:like(position, rotation)
    - #6 | CCSprite | CCSpriteBody | frame=animal_sister_body.png | pos=[-6, 48, 0] | rot=0
      animations: 2:walkin(position) / 3:order(position, rotation) / 4:like(position) / 6:eat(position)
    - #7 | CCSprite | CCSpriteHead | frame=animal_sister_head.png | pos=[-12, 62, 0] | rot=0
      animations: 2:walkin(position, rotation) / 3:order(position, rotation) / 4:like(position, rotation) / 6:eat(position)
      - #8 | CCSprite | CCSpriteEyeNormal | frame=animal_sister_face_normal.png | pos=[-324, 10, 0]
        animations: 2:walkin(position) / 3:order(position)
      - #9 | CCSprite | CCSpriteEyeClosed | frame=animal_sister_face_close_eye.png | pos=[-257, -95, 0] | rot=0 | visible=false
        animations: 3:order(position, rotation, visible)
      - #10 | CCSprite | CCSpriteEat0 | frame=animal_sister_face_eat01.png | pos=[66, 39, 0] | visible=true
        animations: 6:eat(position, visible)
      - #11 | CCSprite | CCSpriteEat1 | frame=animal_sister_face_eat02.png | pos=[66, 39, 0] | visible=false
        animations: 6:eat(position, visible)
      - #12 | CCSprite | CCSpriteLike | frame=animal_sister_face_like.png | pos=[-285, -56, 0]
        animations: 4:like(position)
    - #13 | CCSprite | CCSpriteArm | frame=animal_sister_arm_bend02.png | pos=[-313, 18, 0] | rot=30
      animations: 2:walkin(position, rotation) / 3:order(position, rotation) / 4:like(position, rotation)
      - #14 | CCSprite | CCSprite | frame=animal_sister_arm_bend01.png | pos=[12, 48, 0]
        animations: 3:order(position)
    - #15 | CCSprite | CCSpriteArm | frame=animal_sister_arm_bend01.png | pos=[-34, 73, 0] | rot=-20
      animations: 6:eat(position)
    - #16 | CCSprite | CCSpriteArm | frame=animal_sister_arm_bend02.png | pos=[-33, 45, 0] | rot=-140
      animations: 6:eat(position)
    - #17 | CCSprite | CCSpriteArm | frame=animal_sister_arm_bend02.png | pos=[41, 50, 0] | rot=140
      animations: 6:eat(position)
    - #18 | CCSprite | CCSpriteEatEffect0 | frame=animal_sister_effect_eat01.png | pos=[10, 81, 0] | visible=true
      animations: 6:eat(position, visible)
    - #19 | CCSprite | CCSpriteEatEffect1 | frame=animal_sister_effect_eat02.png | pos=[10, 81, 0] | rot=-5 | visible=true
      animations: 6:eat(position, visible)
    - #20 | CCSprite | CCSpriteEatEffect2 | frame=animal_sister_effect_eat03.png | pos=[10, 81, 0] | rot=-5 | visible=false
      animations: 6:eat(position, visible)
    - #21 | CCSprite | CCSpriteEatEffect3 | frame=animal_sister_effect_eat04.png | pos=[10, 81, 0] | rot=-5 | visible=false
      animations: 6:eat(position, visible)
    - #22 | CCSprite | CCSpriteEatEffect4 | frame=animal_sister_effect_eat05.png | pos=[10, 81, 0] | rot=-5 | visible=false
      animations: 6:eat(position, visible)
    - #23 | CCSprite | CCSpriteEatEffect5 | frame=animal_sister_effect_eat05.png | pos=[9, 81, 0] | rot=-5 | visible=false
      animations: 6:eat(position, visible)
    - #24 | CCSprite | CCSpriteTailOut | frame=animal_sister_tail.png | pos=[-363, -17, 0] | rot=-30
      animations: 1:walkout(position, rotation)
    - #25 | CCSprite | CCSpriteLegOut | frame=animal_sister_leg.png | pos=[-367, -27, 0] | rot=20
      animations: 1:walkout(position, rotation)
    - #26 | CCSprite | CCSpriteLegOut | frame=animal_sister_leg.png | pos=[-339, 16, 0] | rot=-30
      animations: 1:walkout(position, rotation)
    - #27 | CCSprite | CCSpriteArmOut | frame=animal_sister_arm_normal.png | pos=[-352, 17, 0] | rot=-20
      animations: 1:walkout(position, rotation)
    - #28 | CCSprite | CCSpriteBodyOut | frame=animal_sister_body.png | pos=[-352, 0, 0] | rot=0
      animations: 1:walkout(position, rotation)
    - #29 | CCSprite | CCSpriteheadOut | frame=animal_sister_head.png | pos=[-333, -15, 0] | rot=0
      animations: 1:walkout(position, rotation)
      - #30 | CCSprite | CCSprite | frame=animal_sister_face_normal.png | pos=[41, 38, 0]
    - #31 | CCSprite | CCSpriteArmOut | frame=animal_sister_arm_normal.png | pos=[-295, 46, 0] | rot=35
      animations: 1:walkout(position, rotation)
