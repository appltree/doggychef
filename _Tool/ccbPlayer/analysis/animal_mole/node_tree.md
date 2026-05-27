# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_mole\node_animal_mole.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 18
- spriteCount: 16
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode
  - #1 | CCLayer | CCLayer | pos=[0, 0, 0]
    animations: 1:walkout(position) / 2:walkin(position) / 3:order(position) / 4:like(position)
    - #2 | CCSprite | CCSpriteLegLeft | frame=animal_mole_leg.png | pos=[21, 33, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 4:like(visible) / 5:angry(visible) / 6:eat(position, visible)
    - #3 | CCSprite | CCSpriteLegRight | frame=animal_mole_leg.png | pos=[-13, 33, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(visible) / 4:like(visible) / 5:angry(visible) / 6:eat(visible)
    - #4 | CCSprite | CCSpriteBody | frame=animal_mole_body.png | pos=[0, 66, 0] | visible=true
      animations: 2:walkin(visible) / 3:order(visible) / 4:like(visible) / 5:angry(visible) / 6:eat(position, visible)
    - #5 | CCSprite | CCSpriteShoulder | frame=animal_mole_shoulder.png | pos=[1, 91, 0] | rot=0 | visible=true
      animations: 2:walkin(visible) / 3:order(visible) / 4:like(position, visible) / 5:angry(position, visible) / 6:eat(position, rotation, visible)
    - #6 | CCSprite | CCSpriteHeadTail | frame=animal_mole_head_tail.png | pos=[-36, 113, 0] | rot=0 | visible=true
      animations: 2:walkin(visible) / 3:order(visible) / 4:like(position, rotation, visible) / 5:angry(visible) / 6:eat(position, rotation, visible)
    - #7 | CCSprite | CCSpriteHeadNormal | frame=animal_mole_head_normal.png | pos=[4, 133, 0] | rot=10 | visible=true
      animations: 2:walkin(visible) / 3:order(visible) / 6:eat(position, rotation, visible)
    - #8 | CCSprite | CCSpriteHeadLike | frame=animal_mole_head_like.png | pos=[0, 139, 0] | rot=5 | visible=false
      animations: 4:like(position, rotation, visible)
    - #9 | CCSprite | CCSpriteHeadAngry | frame=animal_mole_head_angry.png | pos=[-1, 141, 0] | visible=false
      animations: 5:angry(visible)
      - #10 | CCSprite | CCSpriteHeadAcc | frame=animal_mole_head_acc.png | pos=[22, 58, 0] | visible=false
        animations: 5:angry(scale, visible)
    - #11 | CCSprite | CCSpriteArmNomral | frame=animal_mole_arm_normal.png | pos=[-1, 62, 0] | visible=false
      animations: 2:walkin(visible) / 3:order(visible) / 5:angry(visible)
    - #12 | CCSprite | CCSpriteArmUpper | frame=animal_mole_arm_upper.png | pos=[1, 82, 0] | rot=0 | visible=true
      animations: 4:like(visible) / 6:eat(position, rotation, visible)
    - #13 | CCSprite | CCSpriteEat0 | frame=animal_mole_effect_eat01.png | pos=[10, 104, 0] | visible=false
      animations: 6:eat(visible)
    - #14 | CCSprite | CCSpriteEat1 | frame=animal_mole_effect_eat02.png | pos=[10, 104, 0] | visible=false
      animations: 6:eat(visible)
    - #15 | CCSprite | CCSpriteEat2 | frame=animal_mole_effect_eat03.png | pos=[10, 104, 0] | visible=false
      animations: 6:eat(visible)
    - #16 | CCSprite | CCSpriteEat3 | frame=animal_mole_effect_eat04.png | pos=[10, 104, 0] | visible=false
      animations: 6:eat(visible)
    - #17 | CCSprite | CCSpriteEat4 | frame=animal_mole_effect_eat05.png | pos=[10, 104, 0] | visible=true
      animations: 6:eat(visible)
