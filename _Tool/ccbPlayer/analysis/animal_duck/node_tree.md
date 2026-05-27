# CCB Node Tree

- Source: C:\prj\doggy\ccbPlayer\data\animal_duck\node_animal_duck.ccb
- fileType: CocosBuilder
- fileVersion: 4
- nodeCount: 32
- spriteCount: 30
- sequenceCount: 6

## Tree

- #0 | CCNode | CCNode | visible=true
  - #1 | CCLayer | CCLayerBody | pos=[0, 62, 0] | visible=true
    animations: 1:walkout(position) / 2:walkin(position) / 3:order(position)
    - #2 | CCSprite | CCSpriteArmRightOut | frame=animal_duck_arm.png | pos=[-654, 552, 0] | rot=30 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #3 | CCSprite | CCSpriteArmRight | frame=animal_duck_arm.png | pos=[22, 14, 0] | rot=-50 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #4 | CCSprite | CCSpriteLegRight | frame=animal_duck_leg.png | pos=[17, -42, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, visible) / 4:like(position, visible) / 5:angry(position, visible) / 6:eat(position, visible)
    - #5 | CCSprite | CCSpriteLegRightOut | frame=animal_duck_leg.png | pos=[-650, 562, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #6 | CCSprite | CCSpriteBody | frame=animal_duck_body.png | pos=[0, 0, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 5:angry(position, rotation, visible) / 6:eat(position, visible)
    - #7 | CCSprite | CCSpriteBodyOut | frame=animal_duck_body.png | pos=[-664, 530, 0] | visible=true
      animations: 1:walkout(position, visible)
    - #8 | CCSprite | CCSpriteLegLeftOut | frame=animal_duck_leg.png | pos=[-647, 585, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #9 | CCSprite | CCSpriteLegLeft | frame=animal_duck_leg.png | pos=[-7, -42, 0] | rot=0 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, visible) / 4:like(position, visible) / 5:angry(position, visible) / 6:eat(position, visible)
    - #10 | CCSprite | CCSpriteArmLeft | frame=animal_duck_arm.png | pos=[-10, 13, 0] | rot=50 | visible=true
      animations: 2:walkin(position, rotation, visible) / 3:order(position, rotation, visible) / 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #11 | CCSprite | CCSpriteFace | frame=animal_duck_normal.png | pos=[-623, 519, 0] | visible=true
      animations: 2:walkin(position, visible) / 3:order(position, visible)
    - #12 | CCSprite | CCSpriteFaceCloseEye | frame=animal_duck_normal_close_eye.png | pos=[-744, 502, 0] | visible=true
      animations: 3:order(position, visible)
    - #13 | CCSprite | CCSpriteFaceOut | frame=animal_duck_normal.png | pos=[-655, 506, 0] | rot=0 | visible=true
      animations: 1:walkout(position, rotation, visible)
    - #14 | CCSprite | CCSpriteFaceSmile_0 | frame=animal_duck_normal_smile_0.png | pos=[16, 5, 0] | rot=-10 | visible=true
      animations: 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #15 | CCSprite | CCSpriteFaceSmile_1 | frame=animal_duck_normal_smile_1.png | pos=[16, 5, 0] | rot=-10 | visible=false
      animations: 4:like(position, rotation, visible) / 6:eat(position, rotation, visible)
    - #16 | CCSprite | CCSpriteFaceCry | frame=animal_duck_normal_cry.png | pos=[-707, 537, 0] | rot=0 | visible=true
      animations: 5:angry(position, rotation, visible)
    - #17 | CCSprite | CCSpriteArmCry | frame=animal_duck_arm.png | pos=[-720, 504, 0] | rot=-120 | visible=true
      animations: 5:angry(position, rotation, visible)
    - #18 | CCSprite | CCSpriteArmLeftOut | frame=animal_duck_arm.png | pos=[-681, 566, 0] | rot=-50 | visible=true
      animations: 1:walkout(position, rotation, visible)
  - #19 | CCSprite | CCSpriteTearLeft0 | frame=animal_duck_tears_0.png | pos=[-645, 639, 0] | visible=true
    animations: 5:angry(position, visible)
  - #20 | CCSprite | CCSpriteTearLeft1 | frame=animal_duck_tears_1.png | pos=[-614, 623, 0] | visible=true
    animations: 5:angry(position, visible)
  - #21 | CCSprite | CCSpriteTearLeft2 | frame=animal_duck_tears_2.png | pos=[-643, 662, 0] | visible=true
    animations: 5:angry(position, visible)
  - #22 | CCSprite | CCSpriteTearLeft3 | frame=animal_duck_tears_3.png | pos=[-695, 595, 0] | visible=true
    animations: 5:angry(position, visible)
  - #23 | CCSprite | CCSpriteTearRight0 | frame=animal_duck_tears_0.png | pos=[-706, 648, 0] | visible=true
    animations: 5:angry(position, visible)
  - #24 | CCSprite | CCSpriteTearRight1 | frame=animal_duck_tears_1.png | pos=[-722, 626, 0] | visible=true
    animations: 5:angry(position, visible)
  - #25 | CCSprite | CCSpriteTearRight2 | frame=animal_duck_tears_2.png | pos=[-717, 653, 0] | visible=true
    animations: 5:angry(position, visible)
  - #26 | CCSprite | CCSpriteTearRight3 | frame=animal_duck_tears_3.png | pos=[-708, 632, 0] | visible=true
    animations: 5:angry(position, visible)
  - #27 | CCSprite | CCSpriteSpit_0 | frame=animal_duck_spit_0.png | pos=[80, 106, 0] | visible=false
    animations: 6:eat(position, visible)
  - #28 | CCSprite | CCSpriteSpit_1 | frame=animal_duck_spit_1.png | pos=[80, 106, 0] | visible=false
    animations: 6:eat(position, visible)
  - #29 | CCSprite | CCSpriteSpit_2 | frame=animal_duck_spit_2.png | pos=[80, 106, 0] | visible=false
    animations: 6:eat(position, visible)
  - #30 | CCSprite | CCSpriteSpit_3 | frame=animal_duck_spit_3.png | pos=[80, 106, 0] | visible=false
    animations: 6:eat(position, visible)
  - #31 | CCSprite | CCSpriteSpit_4 | frame=animal_duck_spit_4.png | pos=[80, 106, 0] | visible=false
    animations: 6:eat(position, visible)
