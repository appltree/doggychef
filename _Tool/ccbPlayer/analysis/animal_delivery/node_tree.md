# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_delivery\node_animal_delivery.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 19
- spriteCount: 17
- sequenceCount: 3

## Tree

- #0 | CCNode | CCNode
  - #1 | CCLayer | CCLayer | pos=[0, 0, 0]
    animations: 1:walkout(position) / 2:walkin(position) / 3:walkout_pizza(position)
    - #2 | CCSprite | CCSpriteArmRight | frame=animal_delivery_arm_02.png | pos=[-787, 458, 0] | rot=0
      animations: 2:walkin(position, rotation)
    - #3 | CCSprite | CCSpriteTail | frame=animal_delivery_tail.png | pos=[-748, 486, 0] | rot=0
      animations: 2:walkin(position, rotation)
    - #4 | CCSprite | CCSpriteLegLeft | frame=animal_delivery_leg.png | pos=[-747, 445, 0] | rot=0
      animations: 2:walkin(position, rotation)
    - #5 | CCSprite | CCSpriteLegRight | frame=animal_delivery_leg.png | pos=[-726, 434, 0] | rot=0
      animations: 2:walkin(position, rotation)
    - #6 | CCSprite | CCSpriteBody | frame=animal_delivery_body.png | pos=[-812, 462, 0]
      animations: 2:walkin(position)
    - #7 | CCSprite | CCSpriteArmLeft | frame=animal_delivery_arm_01.png | pos=[-727, 413, 0] | rot=0
      animations: 2:walkin(position, rotation)
    - #8 | CCSprite | CCSpriteHead | frame=animal_delivery_head.png | pos=[-724, 359, 0]
      animations: 2:walkin(position)
    - #9 | CCSprite | CCSpriteLegRightOut | frame=animal_delivery_leg.png | pos=[-11, 53, 0] | rot=0
      animations: 1:walkout(position, rotation) / 3:walkout_pizza(position, rotation)
    - #10 | CCSprite | CCSpriteLegLeftOut | frame=animal_delivery_leg.png | pos=[18, 52, 0] | rot=0
      animations: 1:walkout(position, rotation) / 3:walkout_pizza(position, rotation)
    - #11 | CCSprite | CCSpriteArmRightOut | frame=animal_delivery_arm_02.png | pos=[-7, 84, 0] | rot=0
      animations: 1:walkout(position, rotation) / 3:walkout_pizza(position, rotation)
    - #12 | CCSprite | CCSpriteTailOut | frame=animal_delivery_tail.png | pos=[32, 35, 0] | rot=0
      animations: 1:walkout(position, rotation) / 3:walkout_pizza(position, rotation)
    - #13 | CCSprite | CCSpriteBodyOut | frame=animal_delivery_body.png | pos=[2, 58, 0]
      animations: 1:walkout(position) / 3:walkout_pizza(position)
    - #14 | CCSprite | CCSpriteBag | frame=animal_delivery_bag.png | pos=[-646, 392, 0]
      animations: 1:walkout(position)
    - #15 | CCSprite | CCSpriteArmLeftOut | frame=animal_delivery_arm_01.png | pos=[-592, 593, 0] | rot=0
      animations: 1:walkout(position, rotation)
    - #16 | CCSprite | CCSpriteHeadOut | frame=animal_delivery_head.png | pos=[-1, 123, 0]
      animations: 1:walkout(position) / 3:walkout_pizza(position)
    - #17 | CCSprite | CCSpriteArmLeftOutOut | frame=animal_delivery_arm_01.png | pos=[27, 51, 0] | rot=-120
      animations: 3:walkout_pizza(position)
    - #18 | CCSprite | CCSpriteBagPizza | frame=animal_delivery_pizza.png | pos=[51, 114, 0]
      animations: 3:walkout_pizza(position)
