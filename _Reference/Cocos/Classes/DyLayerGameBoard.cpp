/*
DyLayerGameBoard.cpp
Doggy chef
@initialize at 120907

@history
120907	yoonsr	initialize
*/

#include "DyLayerGameBoard.h"

#include "DyLib.h"
#include "TmLib.h"
#include "DyStage.h"
#include "DyRecipe.h"
#include "DyBlockInfo.h"
#include "DyTycoon.h"
#include "DyTycoonCommand.h"
#include "CCextAnimatePacker.h"
#include "DyItem.h"
#include "YtSpriteEx.h"
#include "DyCustomer.h"
#include "DySceneGame.h"
#include "DyTutorial.h"

DyBlock::DyBlock()
{
}


DyBlock::DyBlock(std::string tag, int id, const char *pszSpriteFileName, const char *pszSpriteGlowFileName, TmBoard* board) : TmBlock(tag, id, pszSpriteFileName, pszSpriteGlowFileName, board)
{
}


void DyBlock::endAnimationEffect(CCNode* node)
{
	node->removeFromParentAndCleanup(true);
}



void DyBlock::endAnimationHide(CCNode* node)
{
    node->setVisible(false);
}


void DyBlock::endAnimationHint(CCNode* node)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_HINT);

	DyBlock* block = (DyBlock*)node->getUserData();

	TmCommand* command = new TmCommand(TmCommandType_HintReal, block, NULL, NULL, 0, 0);
	block->m_board->m_commandManager->addLast(command);
	command->release();
}


void DyBlock::startAnimation()
{
	switch(m_status)
	{
		case TmBlockStatus_Dying:
			{
				CCCallFunc* callfun = CCCallFuncN::create(m_board->m_layerMain, callfuncN_selector(TmBlock::endAnimationDie));

                
                if(m_isGrilled)
                {
                    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_BOILED);
                    
                    CCFiniteTimeAction *seq = CCSequence::create(CCDelayTime::create(.5f), callfun, NULL);
                    m_sprFood->runAction(seq);
                    m_sprFood->setUserData(this);
                }
                else
                {
                    switch(m_dieType)
                    {
                        case TmBlockDieType_ByHalo:
                            m_sprGlow->setVisible(true);
                            m_sprGlow->setOpacity(0);

                            m_sprGlow->runAction(CCSequence::create(CCFadeTo::create(.3f, 255), CCFadeTo::create(.05f, 0), NULL));
                            //m_delayDiePost += 0.3f;
                            break;
                        case TmBlockDieType_ByCross:
                    
                            m_sprGlow->setColor(ccYELLOW);
                            m_sprGlow->setVisible(true);
                            m_sprGlow->setOpacity(0);

                            m_sprGlow->runAction(CCSequence::create(CCFadeTo::create(.3f, 255), CCFadeTo::create(.05f, 0), NULL));
                            //m_delayDiePost += 0.3f;
                            break;
                        default:
                            break;
                    }
                    
                    CCActionInterval* fadeout = CCFadeOut::create(.3f);
                    CCFiniteTimeAction *seq = CCSequence::create(CCDelayTime::create(m_delayDiePost), fadeout, callfun, NULL);
                    //m_layer->runAction(seq);
                    
                    m_sprFood->runAction(seq);
                    m_sprFood->setUserData(this);
                    
                    CCCallFunc* callfun_effect = CCCallFuncN::create(m_board->m_layerMain, callfuncN_selector(DyBlock::endAnimationEffect));
                    
                    CCSprite* sprEffect = CCSprite::createWithSpriteFrameName("threematch_ui_effect_bang00.png");
                    sprEffect->setAnchorPoint(ccp(0.5f, 0.5f));
                    sprEffect->setPosition(m_layerBlock->getPosition());
                    m_board->m_layerMain->addChild(sprEffect);
                    
                    if(m_board->m_isHard)
                        sprEffect->setScale(SCALE_HARD);
                    
                    sprEffect->setOpacity(0);
                    CCActionInterval* fadeout_effect = CCFadeOut::create(.1f);
                    sprEffect->runAction(CCSequence::create(CCDelayTime::create(m_delayDiePost), CCFadeIn::create(.05f), CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_bang"), fadeout_effect, callfun_effect, NULL));

                }
                
			}
			break;
		case TmBlockStatus_Disappear:
        case TmBlockStatus_EndAndDisappear:
			{
				CCCallFunc* callfun = CCCallFuncN::create(m_board->m_layerMain, callfuncN_selector(TmBlock::endAnimationDisappear));
                float delay = .1f;
                if(m_status == TmBlockStatus_EndAndDisappear)
                {
                    delay = 1.0f;
                    callfun = NULL;
                }
				CCActionInterval* fadeout = CCFadeOut::create(delay);
				CCFiniteTimeAction *seq = CCSequence::create(fadeout, callfun, NULL);
				m_sprFood->runAction(seq);
				m_sprFood->setUserData(this);
                
                
                if(m_sprGlow)
                    m_sprGlow->runAction(CCSequence::create(CCFadeOut::create(delay), NULL));
                if(m_sprBombTarget)
                    m_sprBombTarget->runAction(CCSequence::create(CCFadeOut::create(delay), NULL));
            
                if(m_sprCross)
                    m_sprCross->runAction(CCSequence::create(CCFadeOut::create(delay), NULL));
                if(m_sprUnder)
                    m_sprUnder->runAction(CCSequence::create(CCFadeOut::create(delay), NULL));
                if(m_sprUpon)
                    m_sprUpon->runAction(CCSequence::create(CCFadeOut::create(delay), NULL));
                if(m_sprAttr)
                    m_sprAttr->runAction(CCSequence::create(CCFadeOut::create(delay), NULL));
               
                destoryAni();

			}
			break;
		case TmBlockStatus_Falling:
			{
				CCCallFunc* callfun = CCCallFuncN::create(m_board->m_layerMain, callfuncN_selector(TmBlock::endAnimationFalling));
				
                float distance = ccpDistance(m_posCur, m_posTgt);
				CCActionInterval* moveto = CCMoveTo::create(0.001f*distance, m_posTgt);
                
                CCActionInterval* move_ease_in;
                if(m_isStartBlock)
                    move_ease_in = CCEaseOut::create(moveto, 0.2f);
                else
                    move_ease_in = CCEaseIn::create(moveto, 0.2f);

                //
                // 대각선 이동
                if(m_posCur.x != m_posTgt.x)
                {
                    move_ease_in = CCEaseIn::create(moveto, 0.5f);
                }
                
                m_isStartBlock = false;
                
				CCFiniteTimeAction *seq = CCSequence::create(CCDelayTime::create(m_delayFallPre), move_ease_in, CCDelayTime::create(m_delayFallPost), callfun, NULL);
				m_layerBlock->runAction(seq);
			}
			break;
		case TmBlockStatus_Swap:
			{	
				CCCallFunc* callfun = CCCallFuncN::create(m_board->m_layerMain, callfuncN_selector(TmBlock::endAnimationSwap));
				float distance = ccpDistance(m_posCur, m_posTgt);
				CCActionInterval* moveto = CCMoveTo::create(0.001f*distance, m_posTgt);
				//CCActionInterval* moveto = CCMoveTo::create(0.01f*distance, m_posTgt);
				CCFiniteTimeAction *seq = CCSequence::create(moveto, callfun, NULL);
				m_layerBlock->runAction(seq);
			}
			break;
		case TmBlockStatus_SwapReverse:
			{
				CCCallFunc* callfun = CCCallFuncN::create(m_board->m_layerMain, callfuncN_selector(TmBlock::endAnimationSwapReverse));
				float distance = ccpDistance(m_posCur, m_posTgt);
				CCActionInterval* moveto = CCMoveTo::create(0.001f*distance, m_posTgt);
				//CCActionInterval* moveto = CCMoveTo::create(0.01f*distance, m_posTgt);
				CCFiniteTimeAction *seq = CCSequence::create(moveto, callfun, NULL);
				m_layerBlock->runAction(seq);
                
                DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
                tycoonManager->m_board->m_cntCombo = 0;
			}
			break;
        default:
            break;
        
	}
}


void DyBlock::setForm()
{
	switch(m_status)
	{
		case TmBlockStatus_Select:
		case TmBlockStatus_Normal:
			m_sprGlow->setVisible(false);
			m_sprGlow->setOpacity(255);
			m_sprGlow->stopAllActions();

            if(m_board->m_isHard)
                m_sprFood->setScale(SCALE_HARD);
			m_sprFood->stopAllActions();

			if(m_status == TmBlockStatus_Select)
			{
				m_board->m_sprCursor->setVisible(true);
				m_board->m_sprCursor->setPosition(m_layerBlock->getPosition());
			}
			break;

		case TmBlockStatus_Highlight:
			{
				DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
				DyItemManager* itemManager = DyItemManager::sharedItemManager();

				CCMenuItemImage* ctrlBtnHint = (CCMenuItemImage*)tycoonManager->m_menuItem->getChildByTag(itemManager->getSlotIndex(DyItemType_Hint)+GAME_UI_TAG_MENUITEM_SLOT0);

				CCPoint srcPos = ctrlBtnHint->getPosition();
				CCPoint tgtPos = m_board->m_layerMain->convertToWorldSpace(m_posCur);
				tgtPos = m_board->m_layerMain->getParent()->convertToNodeSpace(tgtPos);

				CCSprite* sprite = CCSprite::createWithSpriteFrameName("game_ui_item_hint.png");
				sprite->setPosition(srcPos);

				float distance = ccpDistance(srcPos, tgtPos);

				CCCallFunc* callfun =  CCCallFuncN::create(m_board->m_layerMain, callfuncN_selector(DyBlock::endAnimationHint));
				CCCallFunc* callfunHide =  CCCallFuncN::create(m_board->m_layerMain, callfuncN_selector(DyBlock::endAnimationEffect));
				
				sprite->runAction(CCSequence::create(CCFadeTo::create(.0005f*distance, 153), NULL));

				sprite->runAction(CCSequence::create(CCMoveTo::create(.0005f*distance, tgtPos), callfun, CCScaleTo::create(0.35f, 3.5f), CCScaleTo::create(0.1f, 0.5f), callfunHide, NULL));
				tycoonManager->m_layer->addChild(sprite, ZORDER_GAME_MOVE_OBJECT_UNDER);
				sprite->setUserData(this);
			}
			break;
	}
}



void DyBlock::setAttr(TmBlockAttr attr, int attrValue)
{
    m_blockAttr = attr;
    m_blockAttrValue = attrValue;
    
    m_dtAttr = 0.f;
    m_startBlinkAttr = false;
    
    if(attr > TmBlockAttr_None)
    {
        m_sprUnder->removeFromParentAndCleanup(true);
        CCCallFunc* callfun_effect = CCCallFuncN::create(m_board->m_layerMain, callfuncN_selector(DyBlock::endAnimationHide));
        m_sprUnder = CCSprite::createWithSpriteFrameName("threematch_ui_effect_change00.png");
        m_layerBlock->addChild(m_sprUnder, TMBOARD_FOOD_ZORDER-1);
        
        if(m_board->m_isHard)
            m_sprUnder->setScale(SCALE_HARD);
        
        m_sprUnder->runAction(CCSequence::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_change"), callfun_effect, NULL));
        
        m_sprAttr->setVisible(true);
        
        YtLib::sharedLib()->playEffect(YtLibSndEff_APPEAR_SPECIAL_BLOCK);
    }

    switch(m_blockAttr)
    {
        case TmBlockAttr_Gold:
            m_sprAttr->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("crazy_icon_gold.png"));
            break;
        case TmBlockAttr_Delly:
            m_sprAttr->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("crazy_icon_delly.png"));
            break;
        case TmBlockAttr_Heart:
            m_sprAttr->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("crazy_icon_heart.png"));
            break;
        default:
            m_sprAttr->stopAllActions();
            m_sprAttr->setVisible(false);
            break;
    }
}


void DyBlock::setBlockType(TmBlockType type)
{
    TmBlockType beforeType = m_blocktype;
//	DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
   
    m_blocktype = type;
    
    
	switch(type)
	{
		case TmBlockType_Normal:
			m_sprUnder->setVisible(false);
			m_sprUpon->setVisible(false);
			break;
		case TmBlockType_Ice:
            if(beforeType == TmBlockType_Fix)
            {
                m_fixAni->m_actionManager->runAnimations("untie");
                YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_FIX_BREAK);
            }
            else if(rand()%100 < tycoonManager->m_functionGame.m_ice)
            {
                setBlockType(TmBlockType_Normal);
            }
            else
            {
                m_sprUnder->setVisible(false);
                m_sprUpon->removeFromParentAndCleanup(true);
                m_sprUpon = CCSprite::createWithSpriteFrameName("threematch_ui_effect_ice.png");
                m_layerBlock->addChild(m_sprUpon, TMBOARD_FOOD_ZORDER+1);
                if(m_board->m_isHard)
                    m_sprUpon->setScale(SCALE_HARD);
            }
			break;
        case TmBlockType_Blank:
            m_sprFood->setVisible(false);
            m_sprUnder->removeFromParentAndCleanup(true);
            if(m_board->m_isHard)
                m_sprUnder = CCSprite::createWithSpriteFrameName("game_ui_board_hole.png");
            else
            {
                m_sprUnder = CCSprite::createWithSpriteFrameName("game_ui_board_hole_big.png");
                m_sprUnder->setPosition(ccp(-1, 1));
                
            }
            m_layerBlock->addChild(m_sprUnder, TMBOARD_FOOD_ZORDER-1);
            break;
        case TmBlockType_Fix:
            m_sprUpon->removeFromParentAndCleanup(true);
			m_sprUpon = CCSprite::createWithSpriteFrameName("threematch_ui_effect_ice.png");
			m_layerBlock->addChild(m_sprUpon, TMBOARD_FOOD_ZORDER+1);
            if(m_board->m_isHard)
                m_sprUpon->setScale(SCALE_HARD);
            
            m_fixAni = YtAniObject::loadAniObject(this, "effect_tie.ccbi");
            m_fixAni->m_actionManager->runAnimations("normal");
            m_fixAni->setPosition(ccp(0, 0));
            m_layerBlock->addChild(m_fixAni, TMBOARD_FOOD_ZORDER+2);
            
            if(m_board->m_isHard)
                m_fixAni->setScale(SCALE_HARD);

            break;
            
		case TmBlockType_Halo:

            destoryAni();

			m_sprFood->setColor(ccWHITE);

			m_sprUpon->setVisible(false);
            m_sprAttr->setVisible(false);

			m_sprUnder->removeFromParentAndCleanup(true);
			m_sprUnder = CCSprite::createWithSpriteFrameName("threematch_ui_effect_halo00.png");
			m_layerBlock->addChild(m_sprUnder, TMBOARD_FOOD_ZORDER-1);

            if(m_board->m_isHard)
                m_sprUnder->setScale(SCALE_HARD);
            
			m_sprUnder->runAction(CCRepeatForever::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_halo")));

            YtLib::sharedLib()->playEffect(YtLibSndEff_APPEAR_SPECIAL_BLOCK);
			break;
		case TmBlockType_Cross:

			destoryAni();
            
			m_sprFood->setColor(ccWHITE);

			m_sprUpon->setVisible(false);
            
            m_sprAttr->setVisible(false);
			m_sprCross->setVisible(true);
			m_sprCross->setOpacity(0);

			m_sprCross->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCFadeTo::create(.5f, 153), CCFadeTo::create(.5f, 0), NULL))));
            
            if(m_board->m_isHard)
            {
                m_sprCross->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCScaleTo::create(.5f, SCALE_HARD+0.05f), CCScaleTo::create(.5f, SCALE_HARD), NULL))));
                m_sprCross->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCScaleTo::create(.5f, SCALE_HARD+0.05f), CCScaleTo::create(.5f, SCALE_HARD), NULL))));
            }
            else
            {
                m_sprCross->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCScaleTo::create(.5f, 1.05f), CCScaleTo::create(.5f, 1.f), NULL))));
                m_sprCross->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCScaleTo::create(.5f, 1.05f), CCScaleTo::create(.5f, 1.f), NULL))));
            }
            YtLib::sharedLib()->playEffect(YtLibSndEff_APPEAR_SPECIAL_BLOCK);
            
			break;
		case TmBlockType_Fire:
            m_sprUnder->removeFromParentAndCleanup(true);
			m_sprUnder = CCSprite::createWithSpriteFrameName("threematch_ui_effect_fire_down00.png");
			m_sprUnder->setPosition(ccp(0, -m_sprFood->getContentSize().height/2));
			m_sprUnder->setAnchorPoint(ccp(0.5,0));
			m_layerBlock->addChild(m_sprUnder, TMBOARD_FOOD_ZORDER-1);
            
            if(m_board->m_isHard)
                m_sprUnder->setScale(SCALE_HARD);
            
			m_sprUnder->runAction(CCRepeatForever::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_fire_down")));
		
			m_sprUpon->removeFromParentAndCleanup(true);
			m_sprUpon = CCSprite::createWithSpriteFrameName("threematch_ui_effect_fire_upon00.png");
			m_sprUpon->setPosition(ccp(0, -m_sprFood->getContentSize().height/2));
			m_sprUpon->setAnchorPoint(ccp(0.5, 0));
			m_layerBlock->addChild(m_sprUpon, TMBOARD_FOOD_ZORDER+1);
            
            if(m_board->m_isHard)
                m_sprUpon->setScale(SCALE_HARD);
            
			m_sprUpon->runAction(CCRepeatForever::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_fire_upon")));

            YtLib::sharedLib()->playEffect(YtLibSndEff_APPEAR_SPECIAL_BLOCK);
            
			break;
		case TmBlockType_Decay:
			{
                if(rand()%100 < tycoonManager->m_functionGame.m_decay)
                {
                    setBlockType(TmBlockType_Normal);
                    return;
                }
                
				m_dtDecay = m_board->m_decay_duration;

				m_sprFood->setColor(ccc3(200, 200, 200));

                destoryAni();
				m_skullAni = YtAniObject::loadAniObject(this, "effect_decay_scull.ccbi");				
				m_skullAni->m_actionManager->runAnimations("decay");
				m_skullAni->setPosition(ccp(0, -m_sprFood->getContentSize().height/2));
				m_layerBlock->addChild(m_skullAni, TMBOARD_FOOD_ZORDER+2);

                if(m_board->m_isHard)
                    m_skullAni->setScale(SCALE_HARD);
                
				m_smog = CCSprite::createWithSpriteFrameName("threematch_ui_effect_decay_smog00.png");
				m_smog->setPosition(ccp(0, -m_sprFood->getContentSize().height/2));
				m_smog->setAnchorPoint(ccp(0.5, 0));
				m_layerBlock->addChild(m_smog, TMBOARD_FOOD_ZORDER+3);
                
                if(m_board->m_isHard)
                    m_smog->setScale(SCALE_HARD);
               
                
				m_smog->runAction(CCRepeatForever::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_decay_smog")));
                
                YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_DECAY);
			}

			break;
		case TmBlockType_Candy:
			{
				m_sprUnder->removeFromParentAndCleanup(true);
				CCCallFunc* callfun_effect = CCCallFuncN::create(m_board->m_layerMain, callfuncN_selector(DyBlock::endAnimationHide));
				m_sprUnder = CCSprite::createWithSpriteFrameName("threematch_ui_effect_change00.png");
				m_layerBlock->addChild(m_sprUnder, TMBOARD_FOOD_ZORDER-1);
                
                if(m_board->m_isHard)
                    m_sprUnder->setScale(SCALE_HARD);
               
                
				m_sprUnder->runAction(CCSequence::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_change"), callfun_effect, NULL));
			
				m_id = 10000 + rand()%100;
				m_sprFood->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("threematch_ui_candy.png"));
				m_sprBombTarget->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("threematch_ui_candy_glow.png"));
				m_sprCross->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("threematch_ui_candy_glow.png"));
				m_sprGlow->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("threematch_ui_candy_glow.png"));
				m_sprGlow->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("threematch_ui_candy_glow.png"));
                
                YtLib::sharedLib()->playEffect(YtLibSndEff_APPEAR_SPECIAL_BLOCK);
			}
			break;
		case TmBlockType_MSG:
			{
				m_sprUnder->removeFromParentAndCleanup(true);
				CCCallFunc* callfun_effect = CCCallFuncN::create(m_board->m_layerMain, callfuncN_selector(DyBlock::endAnimationHide));
				m_sprUnder = CCSprite::createWithSpriteFrameName("threematch_ui_effect_change00.png");
				m_layerBlock->addChild(m_sprUnder, TMBOARD_FOOD_ZORDER-1);
                
                if(m_board->m_isHard)
                    m_sprUnder->setScale(SCALE_HARD);
               
                
				m_sprUnder->runAction(CCSequence::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_change"), callfun_effect, NULL));

				m_id = 10000 + rand()%100;
				m_sprFood->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("threematch_ui_msg.png"));
				m_sprBombTarget->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("threematch_ui_msg_glow.png"));
				m_sprCross->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("threematch_ui_msg_glow.png"));
				m_sprGlow->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("threematch_ui_msg_glow.png"));
				m_sprGlow->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("threematch_ui_msg_glow.png"));
                YtLib::sharedLib()->playEffect(YtLibSndEff_APPEAR_SPECIAL_BLOCK);

			}
	}
}

DyBoard::DyBoard()
{
}


DyBoard::DyBoard(int cntCol, int cntRow):TmBoard(cntCol, cntRow)
{
	m_isStartNoMoveEvent = false;
	//TmBoard::TmBoard(cntCol, cntRow);
}


//0	0	cheese	block_cheese_normal.png	block_cheese.xml
//1	1	dduk	block_dduk_normal.png	block_dduk.xml
//2	2	kimbob	block_kimbob_normal.png	block_kimbob.xml
//3	3	mandu	block_mandu_normal.png	block_mandu.xml
//4	4	pepper	block_pepper_normal.png	block_pepper.xml
//5	5	ramen	block_ramen_normal.png	block_ramen.xml
int g_tutorialData[98] =
{
0,4,1,4,1,4,3,//0
2,0,1,3,3,4,4,//1
3,1,0,2,0,2,0,//2
1,3,1,0,2,1,0,//3
2,3,0,4,3,2,2,//4
1,0,1,0,1,1,0,//5
2,0,3,1,2,2,1,//6
    
2,0,4,3,2,1,3,//7
3,1,1,2,3,4,0,//8
0,3,1,2,2,3,0,//9
4,2,3,2,1,0,1,//10
1,2,1,0,1,0,3,//11
4,1,4,0,0,1,3,//12
4,0,2,2,4,0,0,//13
    
//5,1,3,5,2,5,1, //0
//2,4,0,4,3,5,2, //1
//1,2,4,0,0,4,0, //2
//3,0,2,3,1,1,3, //3
//3,1,3,1,1,4,4, //4 
//1,5,0,4,3,0,3, //5
//0,3,3,1,3,2,4, //6
//
//4,3,0,4,4,0,0, //7
//1,4,2,0,5,3,4, 
//4,2,4,1,2,0,0, 
//1,4,5,1,3,3,3, 
//0,0,5,4,3,4,2, 
//3,0,2,0,3,0,5, 
//0,3,5,4,2,2,5, 
};

int g_tutorial_cursorX = 0;

TmBlock* DyBoard::getBlock(bool ignoreICE)
{
	DyBlockManager* blockManager = DyBlockManager::sharedBlockManager();
    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    
    int index;
    
    DyTutorial* tutorial = tutorialManager->getCurrentTutorial();
    
    if(tutorial && tutorial->m_tutorial_type == DyTutorialType_BlockSettingDefault)
    {
        index = g_tutorialData[g_tutorial_cursorX];
        g_tutorial_cursorX++;
        if(g_tutorial_cursorX >= 98)
        {
            g_tutorial_cursorX = 0;
            tutorialManager->removeTutorial(tutorial);
        }
    }
    else
    {
        //
        // 주문하게 되면 확률을 다시 정해야 함.
        int proba = rand()%(int)blockManager->m_totalCntForCreate;
        
        index = -1;
        for(unsigned int i = 0;i < blockManager->m_arrBlockInfo->count()-1;i++)
        {
            DyBlockInfo* blockInfo = (DyBlockInfo*)blockManager->m_arrBlockInfo->objectAtIndex(i);
            DyBlockInfo* blockInfo2 = (DyBlockInfo*)blockManager->m_arrBlockInfo->objectAtIndex(i+1);
            
            if(blockInfo->m_isBlockAvailable && blockInfo2->m_isBlockAvailable)
            {
                if((proba >= blockInfo->m_probability && proba < blockInfo2->m_probability))
                {
                    index = i;
                    break;
                }
            }
        }
        
        if(index == -1)
        {
            index = blockManager->m_cntBlockAvailable - 1;
        }
        
        //CCLog("index = %d, m_cntBlockAvailable = %d, m_totalCntForCreate = %d, proba = %d", index, blockManager->m_cntBlockAvailable, blockManager->m_totalCntForCreate, proba);
        
    }
    
	DyBlockInfo* blockInfo = (DyBlockInfo*)blockManager->m_arrBlockInfo->objectAtIndex(index);
	
	TmBlock* block = new DyBlock(blockInfo->m_tag, blockInfo->m_id, blockInfo->m_urlImage, blockInfo->m_urlGlowImage, this);
    
	int probability = rand()%100;

	if(!ignoreICE && probability < m_ice_probability)
	{
		block->setBlockType(TmBlockType_Ice);
	}
	else
	{
		block->setBlockType(TmBlockType_Normal);
	}

//	YtLib* lib = YtLib::sharedLib();

//	sprintf(lib->m_strTemp, "%d", g_cursorX);
//	block->m_lblDebug->setString(lib->m_strTemp);
//	block->m_lblDebug->setVisible(true);

//	CCLog("%d, ", index);

	return block;
}


TmBlock* DyBoard::getBlockByIndex(int index)
{
	DyBlockManager* blockManager = DyBlockManager::sharedBlockManager();

	DyBlockInfo* blockInfo = (DyBlockInfo*)blockManager->m_arrBlockInfo->objectAtIndex(index);
	
	TmBlock* block = new DyBlock(blockInfo->m_tag, blockInfo->m_id, blockInfo->m_urlImage, blockInfo->m_urlGlowImage, this);
    
    block->setBlockType(TmBlockType_Normal);
	
	return block;
}


void DyBoard::endAnimationCombo(CCNode* node)
{
	node->removeFromParentAndCleanup(true);
}


void DyBoard::endAnimationNoMove(CCNode* node)
{
	node->removeFromParentAndCleanup(true);

	m_isStartNoMoveEvent = false;

    TmCommand* command = new TmCommand(TmCommandType_ClearBlocks, this, NULL, NULL, 0, 0);
	m_commandManager->addLast(command);
	command->release();
}


void DyBoard::startNoMoreMoveEvent()
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

	if(m_isStartNoMoveEvent)
		return;
    
    if(tycoonManager->m_checkLastPang)
        return;

	m_isStartNoMoveEvent = true;

	CCSprite* sprNoMore = CCSprite::createWithSpriteFrameName("game_ui_no_more_move.png");
	CCSprite* sprNoMoreGlow = CCSprite::createWithSpriteFrameName("game_ui_no_more_move_glow.png");

	sprNoMore->setPosition(ccp(670, 300));
	sprNoMoreGlow->setPosition(ccp(670, 300));

	tycoonManager->m_layer->addChild(sprNoMore, ZORDER_GAME_EVENT);
	tycoonManager->m_layer->addChild(sprNoMoreGlow, ZORDER_GAME_EVENT);

	sprNoMoreGlow->setOpacity(0);
	sprNoMoreGlow->runAction(CCSequence::create(
		CCFadeIn::create(.2f),
		CCFadeOut::create(.2f),
		CCFadeIn::create(.2f),
		CCFadeOut::create(.2f),
		CCCallFuncN::create(this, callfuncN_selector(DyBoard::endAnimationEffect)),
		NULL));

	sprNoMore->runAction(CCSequence::create(
		CCDelayTime::create(.5f),
		CCCallFuncN::create(this, callfuncN_selector(DyBoard::endAnimationNoMove)),
		NULL));
}


void DyBoard::displayStatus()
{
	//DyLib *lib = DyLib::sharedLib();
	//sprintf(lib->m_strTemp, "combo %d", this->m_cntCombo);
	//lib->m_layerLog->addLog(lib->m_strTemp);

	if(m_cntCombo <= 1)
		return;

	YtLib* lib = YtLib::sharedLib();

	sprintf(lib->m_strTemp, "%dC", this->m_cntCombo);
	CCLabelBMFont* label = CCLabelBMFont::create(lib->m_strTemp,  "font_combo.fnt");
	
	CCPoint pointSrc = ccp(m_layerMain->m_size.width/2, m_layerMain->m_size.height/2+220);
	CCPoint pointTgt = ccp(pointSrc.x, pointSrc.y + 30);
	label->setPosition(pointSrc);

	m_layerMain->addChild(label);

	float d = 0.04f;

	label->setPosition(pointSrc);
	label->setScale(0.8f);

	CCCallFunc* callfun = CCCallFuncN::create(this, callfuncN_selector(DyBoard::endAnimationCombo));

	label->runAction(CCSequence::create(CCMoveTo::create(0.1f, pointTgt), NULL));
		
	label->runAction(CCSequence::create(
		CCFadeTo::create(d*3, 255*0.7),
		CCDelayTime::create(d*10), //--
		CCFadeTo::create(d*1, 0),
		callfun,
		NULL));

	label->runAction(CCSequence::create(
		CCScaleTo::create(d*1, 1.0f, 1.0f),
		CCScaleTo::create(d*1, 0.98f, 0.98f),
		CCScaleTo::create(d*1, 0.88f, 0.88f),
		CCDelayTime::create(d*10), //--
		CCScaleTo::create(d*1, 0.7f, 0.7f),
		NULL));

	label->runAction(CCSequence::create(
		CCRotateTo::create(d*2, -5.f),
		CCRotateTo::create(d*2, 2.f),
		CCRotateTo::create(d*1, 0.f),
		//--
		NULL));

	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(m_cntCombo > 0)
    {
        if(m_cntCombo%5 == 0)
        {
            tycoonManager->questProcess(QUESTTAG_COMBO, "5", 1);
        }
        if(m_cntCombo%10 == 0)
        {
            tycoonManager->questProcess(QUESTTAG_COMBO, "10", 1);
        }
        if(m_cntCombo%15 == 0)
        {
            tycoonManager->questProcess(QUESTTAG_COMBO, "15", 1);
        }
        if(m_cntCombo%20 == 0)
        {
            tycoonManager->questProcess(QUESTTAG_COMBO, "20", 1);
        }
        if(m_cntCombo%30 == 0)
        {
            tycoonManager->questProcess(QUESTTAG_COMBO, "30", 1);
        }
        if(m_cntCombo%40 == 0)
        {
            tycoonManager->questProcess(QUESTTAG_COMBO, "40", 1);
        }
    }
}


void DyBoard::update(float dt)
{
	TmBoard::update(dt);
}


void DyBoard::startAnimationAddCustomer(CCNode* node)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

	TmBlock* block = (TmBlock*)node->getUserData();

	CCPoint curPos = block->m_board->getGlobalPosition(block);
    
	DyTycoonCommand* command = new DyTycoonCommand(DyTycoonCommandType_AddBlockToCustomer, &curPos, NULL, NULL, block->m_id, 0, 0);
	tycoonManager->m_commandManager->addLast(command);
	command->release();
}


void DyBoard::addPoint(TmBlock* block)
{	
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(block->m_blockAttr != TmBlockAttr_None)
    {
        CCPoint curPos = block->m_board->getGlobalPosition(block);
        
        switch(block->m_blockAttr)
        {
            case TmBlockAttr_None:
                break;
            case TmBlockAttr_Gold:
                tycoonManager->earnGoldCrazy(curPos, block->m_blockAttrValue);
                break;
            case TmBlockAttr_Delly:
                tycoonManager->earnDellyCrazy(curPos, block->m_blockAttrValue);
                
                break;
            case TmBlockAttr_Heart:
                if(tycoonManager->m_checkLastPang)
                {
                    tycoonManager->earnGold(curPos, 1000, 0, 0, 2);
                }
                else
                {
                    tycoonManager->earnHeartCrazy(curPos);
                }
                   
                break;
        }
                
    }
    
    DyBlockManager* blockManager = DyBlockManager::sharedBlockManager();
    
    std::string material_tag = blockManager->getTagByID(block->m_id);
    
    tycoonManager->questProcess(QUESTTAG_BLOCK, material_tag.c_str(), 1);
    
    
	if(block->m_blocktype == TmBlockType_Decay)
		return;

	CCCallFunc* callfun = CCCallFuncN::create(m_layerMain, callfuncN_selector(DyBoard::startAnimationAddCustomer));

	block->m_nodeForAni->setUserData(block);
	block->m_nodeForAni->runAction(CCSequence::create(CCDelayTime::create(block->m_delayDiePost), callfun, NULL));
}


bool DyBoard::selectSpecialBlock(TmBlock* block)
{
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
	//DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

    CCPoint curPos = getGlobalPosition(block);
    
	DyCustomer* customerTarget = NULL;
	if(block->m_blocktype == TmBlockType_Candy)
	{
		int m_min_heart = 20;

		for(int i = MAX_SLOT-1;i >= 0;i--)
		{
			DyCustomer* customer = customerManager->m_arrCustomer[i];

			if(customer && customer->m_customerType != DyCustomerType_Delivery && customer->m_character->m_status == DyCharacterStatus_Order)
			{
				if(m_min_heart > customer->m_character->m_countHeart)
				{
					customerTarget = customer;
					m_min_heart = customer->m_character->m_countHeart;
				}
			}
		}

		if(customerTarget)
		{
        	TmCommand* command = new TmCommand(TmCommandType_DestoryBlock, this, block, NULL, 0, 0);
			m_commandManager->addLast(command);
			command->release();

			DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddSpecialBlockToCustomer, &curPos, NULL, NULL, TmBlockType_Candy, customerTarget->m_idCharacter, 0);
			m_commandManager->addLast(commandTycoon);
			commandTycoon->release();

			destoryBlock("CANDY", curPos);
            
            DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
            
            if(tutorial && (tutorial->m_command_type == DyTutorialCommandType_RemoveCurrentBlock))
            {
                if(block == DyTutorialManager::sharedTutorialManager()->m_currentBlock)
                {
                    DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
                }
            }

			return true;
		}
	}
	else if(block->m_blocktype == TmBlockType_MSG)
	{
		int m_max_price = 0;

		for(int i = MAX_SLOT-1;i >= 0;i--)
		{
			DyCustomer* customer = customerManager->m_arrCustomer[i];

			if(customer && customer->m_character->m_status == DyCharacterStatus_Order)
			{
				if(m_max_price < customer->m_pay)
				{
					customerTarget = customer;
					m_max_price = customer->m_pay;
				}
			}
		}

		if(customerTarget)
		{
         	TmCommand* command = new TmCommand(TmCommandType_DestoryBlock, this, block, NULL, 0, 0);
			m_commandManager->addLast(command);
			command->release();

			DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddSpecialBlockToCustomer, &curPos, NULL, NULL, TmBlockType_MSG, customerTarget->m_idCharacter, 0);
			m_commandManager->addLast(commandTycoon);
			commandTycoon->release();

			destoryBlock("MSG", curPos);
            
            DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
            
            if(tutorial && (tutorial->m_command_type == DyTutorialCommandType_RemoveCurrentBlock))
            {
                if(block == DyTutorialManager::sharedTutorialManager()->m_currentBlock)
                {
                    DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
                }
            }

			return true;
		}
	}
	return false;
}

void DyBoard::commandResult(TmCommandType type, bool success)
{
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

	switch(type)
	{
		case TmCommandType_Refresh:
			if(itemManager->useItem(DyItemType_Refresh, 1) == 0)
			{
				CCMenuItemImage* menuItem = (CCMenuItemImage *)tycoonManager->m_menuItem->getChildByTag(GAME_UI_TAG_MENUITEM_SLOT0+itemManager->getSlotIndex(DyItemType_Refresh));
				menuItem->setEnabled(false);
            }
            tycoonManager->questProcess(QUESTTAG_ITEM, "refresh", 1);
			itemManager->dataSave();
            tycoonManager->slotItemUse(DyItemType_Refresh);
            
            
			break;
		case TmCommandType_Hint:
			if(itemManager->useItem(DyItemType_Hint, 1) == 0)
			{
                CCMenuItemImage* menuItem = (CCMenuItemImage *)tycoonManager->m_menuItem->getChildByTag(GAME_UI_TAG_MENUITEM_SLOT0+itemManager->getSlotIndex(DyItemType_Hint));
				menuItem->setEnabled(false);
 			}
            tycoonManager->questProcess(QUESTTAG_ITEM, "hint", 1);
			itemManager->dataSave();
            tycoonManager->slotItemUse(DyItemType_Hint);
			break;
		case TmCommandType_Bomb:
			if(itemManager->useItem(DyItemType_Bomb, 1) == 0)
			{	
				CCMenuItemImage* menuItem = (CCMenuItemImage *)tycoonManager->m_menuItem->getChildByTag(GAME_UI_TAG_MENUITEM_SLOT0+itemManager->getSlotIndex(DyItemType_Bomb));
				menuItem->setEnabled(false);
 			}
            tycoonManager->questProcess(QUESTTAG_ITEM, "bomb", 1);
			itemManager->dataSave();
            tycoonManager->slotItemUse(DyItemType_Bomb);
            
			break;
        default:
            break;
	}
}


void DyBoard::bombEnable(bool value)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
	DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(DyItemType_Bomb);
		
	if(itemManager->getSlotIndex(DyItemType_Bomb) >= 0)
	{
		CCMenuItemImage* menuItem = (CCMenuItemImage *)tycoonManager->m_menuItem->getChildByTag(GAME_UI_TAG_MENUITEM_SLOT0+itemManager->getSlotIndex(DyItemType_Bomb));
		
		if(item->m_cntAvailable == 0)
		{	
			menuItem->setEnabled(false);
		}
		else
			menuItem->setEnabled(value);
	}
}


void DyBoard::endAnimationEffect(CCNode* node)
{
	node->removeFromParentAndCleanup(true);
}


void DyBoard::endAnimationBomb(CCNode* node)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

	DyBlock* block = (DyBlock*)node->getUserData();
	YtLib::sharedLib()->stopEffect(block->m_tempValue);
	YtLib::sharedLib()->playEffect(YtLibSndEff_THROWBOMB_ANDBOMB);

    CCCallFunc* callfun =  CCCallFuncN::create(this, callfuncN_selector(DyBoard::endAnimationEffect));
	
    
	CCSprite* sprBomb = CCSprite::createWithSpriteFrameName("threematch_ui_effect_bomb00.png");
	sprBomb->setScale(2.0f);					
	sprBomb->setPosition(node->getPosition());
	sprBomb->runAction(CCSequence::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_bomb"), CCFadeOut::create(.1f), callfun, NULL));

	tycoonManager->m_layer->addChild(sprBomb, ZORDER_GAME_MOVE_OBJECT_UNDER);

	TmCommand* command = new TmCommand(TmCommandType_BombReal, block->m_board, block, NULL, 0, 0);
	block->m_board->m_commandManager->addLast(command);
	command->release();
}


void DyBoard::comboEvent()
{
    if(m_cntCombo > 1)
    {
        DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
        
        DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
        DyTutorial* tutorial = tutorialManager->getCurrentTutorial();
        
        if(tutorial && tutorial->m_tutorial_type == DyTutorialType_ReadyCommand)
        {
        }
        else
        {
            if(m_cntCombo > 0 && m_cntCombo%10 == 0)
            {
                tycoonManager->m_combo_point += COMBO_POINT_10;
                tycoonManager->addCrazyPoint(COMBO_POINT_10, 3);
            }
            else if(m_cntCombo > 0 && m_cntCombo%5 == 0)
            {
                tycoonManager->m_combo_point += COMBO_POINT_5;
                tycoonManager->addCrazyPoint(COMBO_POINT_5, 3);
            }
            else
            {
                tycoonManager->m_combo_point += COMBO_POINT_1;
                tycoonManager->addCrazyPoint(COMBO_POINT_1, 3);
            }
        }
    }
}

void DyBoard::bombArround(TmBlock* block)
{
	block->m_tempValue = YtLib::sharedLib()->playEffect(YtLibSndEff_THROWBOMB);

	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	DyItemManager* itemManager = DyItemManager::sharedItemManager();

	CCMenuItemImage* ctrlBtnBomb = (CCMenuItemImage*)tycoonManager->m_menuItem->getChildByTag(itemManager->getSlotIndex(DyItemType_Bomb)+GAME_UI_TAG_MENUITEM_SLOT0);

	CCPoint srcPos = ctrlBtnBomb->getPosition();
    
    CCPoint tgtPos = getGlobalPosition(block);
    
	CCSprite* sprite = CCSprite::createWithSpriteFrameName("threematch_ui_effect_bomb.png");
	sprite->setPosition(srcPos);
	sprite->setAnchorPoint(ccp(0.5, 1.0));
	float distance = ccpDistance(srcPos, tgtPos);

	CCCallFunc* callfun =  CCCallFuncN::create(m_layerMain, callfuncN_selector(DyBoard::endAnimationBomb));
	CCCallFunc* callfunHide =  CCCallFuncN::create(m_layerMain, callfuncN_selector(DyBoard::endAnimationEffect));
				
    ccBezierConfig bezier;

	bezier.controlPoint_1 = CCPointMake((tgtPos.x - srcPos.x)/2, (tgtPos.y - srcPos.y)/2+distance*0.3);
    bezier.controlPoint_2 = bezier.controlPoint_1;
    bezier.endPosition = CCPointMake(tgtPos.x - srcPos.x, tgtPos.y - srcPos.y);
	
	sprite->runAction(CCSequence::create(CCRotateBy::create(.001f*distance+0.1, 180), NULL));

	//sprite->runAction(CCSequence::create(CCMoveTo::create(.001f*distance, tgtPos), callfun, CCFadeOut::create(0.01f), callfunHide, NULL));
	sprite->runAction(CCSequence::create(CCBezierBy::create(.001f*distance, bezier), callfun, CCFadeOut::create(0.01f), callfunHide, NULL));

	tycoonManager->m_layer->addChild(sprite, ZORDER_GAME_MOVE_OBJECT_UNDER);
	sprite->setUserData(block);
	commandResult(TmCommandType_Bomb, true);
}


void DyBoard::destoryBlock(std::string block_tag, CCPoint position)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(tycoonManager->m_checkLastPang)
    {
        if(block_tag.compare("CANDY_B") == 0)
        {
            tycoonManager->m_bonustime_gold += tycoonManager->m_last_candy;
            tycoonManager->earnGold(position, tycoonManager->m_last_candy, 0, 0, 2);
            return;
        }
        else if(block_tag.compare("MSG_B") == 0)
        {
            tycoonManager->m_bonustime_gold += tycoonManager->m_last_msg;
            tycoonManager->earnGold(position, tycoonManager->m_last_msg, 0, 0, 2);
            return;
        }
        else if(block_tag.compare("HALO") == 0)
        {
            tycoonManager->m_bonustime_gold += tycoonManager->m_last_halo;
            tycoonManager->earnGold(position, tycoonManager->m_last_halo, 0, 0, 2);
        }
        else if(block_tag.compare("CROSS") == 0)
        {
            tycoonManager->m_bonustime_gold += tycoonManager->m_last_cross;
            tycoonManager->earnGold(position, tycoonManager->m_last_cross, 0, 0, 2);
        }
        else if(block_tag.compare("FIRE") == 0)
        {
            tycoonManager->m_bonustime_gold += tycoonManager->m_last_fire;
            tycoonManager->earnGold(position, tycoonManager->m_last_fire, 0, 0, 2);
        }
    }
    else
    {
        if(block_tag.compare("CANDY") == 0)
            tycoonManager->questProcess(QUESTTAG_REMAIN, "candy", 1);
    }
    
	if(block_tag.compare("DECAY") == 0 || block_tag.compare("ICE") == 0)
	{
        tycoonManager->addCrazyPoint(CRAZY_REMOVEBLOCK_ADD, 2);
		tycoonManager->questProcess(QUESTTAG_REMOVE, block_tag.c_str(), 1);
        if(!tycoonManager->m_checkLastPang)
            tycoonManager->questProcess(QUESTTAG_REMAIN, block_tag.c_str(), 1);
	}
	else if(block_tag.compare("FIX") == 0)
    {
        tycoonManager->questProcess(QUESTTAG_REMOVE, block_tag.c_str(), 1);
    }
    else
		tycoonManager->questProcess(QUESTTAG_BLOCK, block_tag.c_str(), 1);
}


void DyLayerGameBoard::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
	CCSetIterator it;
	CCTouch* touch;
    
    if(tycoonManager->m_lockInput)
        return;

    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    DyTutorial* tutorial = tutorialManager->getCurrentTutorial();
    
    if(tutorial && tutorial->m_tutorial_type == DyTutorialType_ReadyCommand)
    {
        switch(tutorial->m_command_type)
        {
            case DyTutorialCommandType_ThreeMatch:
                break;
            case DyTutorialCommandType_RemoveCurrentBlock:
                break;
            case DyTutorialCommandType_Sell:
                break;
            case DyTutorialCommandType_DeliveryMake:
                break;
            default:
                return;
        }
    }
    
	for(it = touches->begin();it != touches->end();it++)
	{
		touch = (CCTouch*)(*it);
		if(!touch)
			break;
        
		m_board->ccTouchesBegan(touch);
		return;
	}
}


void DyLayerGameBoard::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	CCSetIterator it;
	CCTouch* touch;
    
    if(tycoonManager->m_lockInput)
        return;
    
	for(it = touches->begin();it != touches->end();it++)
	{
		touch = (CCTouch*)(*it);
        
		if(!touch)
			break;
		m_board->ccTouchesEnded(touch);
		return;
	}
}


void DyLayerGameBoard::ccTouchesMoved(CCSet* touches, CCEvent* event)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	CCSetIterator it;
	CCTouch* touch;
	
    if(tycoonManager->m_lockInput)
        return;
    
	for(it = touches->begin();it != touches->end();it++)
	{
		touch = (CCTouch*)(*it);
        
		if(!touch)
			break;
		m_board->ccTouchesMoved(touch);
		return;
	}
}


DyLayerGameBoard::DyLayerGameBoard(bool isHard):TmLayerMain(CCSizeMake(565, 565))//TmLayerMain(CCSizeMake(552, 554))
{
    if(isHard)
    {
        m_board = new DyBoard(8, 8);
       
    }
    else
    {
        m_size = CCSizeMake(552, 554);
        m_board = new DyBoard(7, 7);
    }
    
    m_board->m_isHard = isHard;
    
	m_board->setLayerMain(this);
}


DyLayerGameBoard::~DyLayerGameBoard()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	
    if(tycoonManager->m_nottimesnd >= 0)
    {
        YtLib::sharedLib()->stopEffect(tycoonManager->m_nottimesnd);
        tycoonManager->m_nottimesnd = -1;
    }
    
    
	m_board->release();
}


void DyLayerGameBoard::pause()
{
}


void DyLayerGameBoard::resume()
{
}


void DyLayerGameBoard::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerGameBoard::show()
{
}
