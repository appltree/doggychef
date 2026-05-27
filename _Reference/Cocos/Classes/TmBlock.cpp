/*
TmBlock.cpp
Three Match Lib
@initialize at 120907

@history
120907	yoonsr	initialize
*/

#include "TmBlock.h"
#include "TmBoard.h"
#include "TmLayerMain.h"
#include "CCextAnimatePacker.h"
#include "YtSpriteEx.h"
#include "TmCommand.h"
#include "TmLib.h"
#include "DyTycoon.h"

TmBlock::TmBlock()
{
	
}

TmBlock::TmBlock(std::string tag, int id, const char *pszSpriteFileName, const char *pszSpriteGlowFileName, TmBoard* board)
{
    m_tag = tag;
    
    m_isGrilled = false;
    m_board = board;
	m_nodeForAni = CCLayerColor::create(ccc4(255, 0, 0, 0), 5, 5);

	m_layerBlock = CCLayer::create();
	m_cntMatchH = 0;
	m_cntMatchV = 0;
	m_blockCheckPointV = NULL;
	m_blockCheckPointH = NULL;
	m_cntFall = 0;
    m_checkFall = false;
	
	m_id = id;
    m_isStartBlock = false;
    
    m_blockAttr = TmBlockAttr_None;

	m_delayFallPre = 0.f;
	m_delayFallPost = 0.f;
	m_delayDiePost = 0.f;

	m_dieType = TmBlockDieType_None;

	m_skullAni = NULL;
	m_smog = NULL;
    
    m_fixAni = NULL;

	m_blocktype = TmBlockType_Normal;

	m_sprFood = CCSprite::createWithSpriteFrameName(pszSpriteFileName);
	m_sprGlow = CCSprite::createWithSpriteFrameName(pszSpriteGlowFileName);
	m_sprBombTarget = CCSprite::createWithSpriteFrameName(pszSpriteGlowFileName);
	m_sprCross = CCSprite::createWithSpriteFrameName(pszSpriteGlowFileName);

	m_sprUnder = YtBlankSprite::spriteWithSizeAndColor(m_sprFood->getContentSize(), ccWHITE);
	m_sprUpon = YtBlankSprite::spriteWithSizeAndColor(m_sprFood->getContentSize(), ccWHITE);

	m_layerBlock->setUserData(this);
	m_sprFood->setColor(ccWHITE);
	m_sprGlow->setVisible(false);
	m_sprBombTarget->setVisible(false);
	m_sprCross->setVisible(false);
	m_sprCross->setColor(ccYELLOW);

	m_layerBlock->addChild(m_sprFood, TMBOARD_FOOD_ZORDER);
    if(m_board->m_isHard)
        m_sprFood->setScale(SCALE_HARD);
    
    
	m_layerBlock->addChild(m_sprGlow, TMBOARD_FOOD_ZORDER);
    if(m_board->m_isHard)
        m_sprGlow->setScale(SCALE_HARD);
    
	m_layerBlock->addChild(m_sprCross, TMBOARD_FOOD_ZORDER);
    if(m_board->m_isHard)
        m_sprCross->setScale(SCALE_HARD);
    
	m_layerBlock->addChild(m_sprBombTarget, TMBOARD_FOOD_ZORDER);
    if(m_board->m_isHard)
        m_sprBombTarget->setScale(SCALE_HARD);
    
	m_status = TmBlockStatus_Normal;

	m_sprUnder->setPosition(ccp(m_sprFood->getContentSize().width/2, m_sprFood->getContentSize().height/2));
	m_layerBlock->addChild(m_sprUnder, TMBOARD_FOOD_ZORDER-1);
    if(m_board->m_isHard)
        m_sprUnder->setScale(SCALE_HARD);
    
	m_sprUpon->setPosition(ccp(m_sprFood->getContentSize().width/2, m_sprFood->getContentSize().height/2));
	m_layerBlock->addChild(m_sprUpon, TMBOARD_FOOD_ZORDER+1);
    if(m_board->m_isHard)
        m_sprUpon->setScale(SCALE_HARD);
    
    m_sprAttr = YtBlankSprite::spriteWithSizeAndColor(m_sprFood->getContentSize(), ccWHITE);
    m_sprAttr->setPosition(ccp(15, -15));
	m_layerBlock->addChild(m_sprAttr, TMBOARD_FOOD_ZORDER+1);
    m_sprAttr->setVisible(false);
    if(m_board->m_isHard)
        m_sprAttr->setScale(SCALE_HARD);
    
	m_layerBlock->addChild(m_nodeForAni);
    
//    m_lblDebug = CCLabelTTF::create("T", "Helvetica", 20);
//    m_layerBlock->addChild(m_lblDebug, TMBOARD_FOOD_ZORDER + 10);
}


TmBlock::~TmBlock()
{
	destoryAni();
    m_layerBlock->removeFromParentAndCleanup(true);
}


void TmBlock::destoryAni()
{
    if(m_skullAni)
    {
        m_skullAni->removeFromParentAndCleanup(true);
        YtAniObject::unloadAniObject(m_skullAni);
        m_skullAni = NULL;
    }
    
    if(m_fixAni)
    {
        m_fixAni->removeFromParentAndCleanup(true);
        YtAniObject::unloadAniObject(m_fixAni);
        m_fixAni = NULL;
    }
    
    if(m_smog)
    {
        m_smog->removeFromParentAndCleanup(true);
        m_smog = NULL;
    }
}


void TmBlock::setAction(int col, int row, TmBlockActionCmd action)
{
	CCSize sizeBoard = m_board->m_layerMain->m_size;
	int colCnt = m_board->m_cntCol;
	int colRow = m_board->m_cntRow;

	float blockWidth = sizeBoard.width/colCnt;
	float blockHeight = sizeBoard.height/colRow;
    
    CCPoint posLeftBottom = ccp(0, 0);
    
    if(m_board->m_isHard)
    {
//        blockWidth += 0.8;
        blockHeight += 0.4;
        
        posLeftBottom = ccp(-10, -10);
    }
	else
    {
        blockWidth -= 0.4;
        blockHeight -= 0.4;
    }
	if(action == TmBlockActionCmd_Add)
	{
		m_col = col;
		m_row = row;

		m_posCur = ccp(blockWidth/2 + blockWidth*m_col, blockHeight/2 + blockHeight*m_row);
        m_posCur = ccpAdd(m_posCur, posLeftBottom);
        
		m_layerBlock->setPosition(m_posCur);
	}
	else if(action == TmBlockActionCmd_Set)
	{
		m_col = col;
		m_row = row;

		m_posCur = ccp(blockWidth/2 + blockWidth*m_col, blockHeight/2 + blockHeight*m_row);
        m_posCur = ccpAdd(m_posCur, posLeftBottom);
        
		m_layerBlock->setPosition(m_posCur);
	}
	else if(action == TmBlockActionCmd_Fall)
	{
		if(this == m_board->m_blockHighlight)
		{
			setAction(m_posCur.x, m_posCur.y, TmBlockActionCmd_Normal);
			m_board->m_blockHighlight = NULL;
		}
		if(this == m_board->m_blockSelected)
		{
			setAction(m_posCur.x, m_posCur.y, TmBlockActionCmd_Normal);
			m_board->m_sprCursor->setVisible(false);
			m_board->m_blockSelected = NULL;
		}
		m_col = col;
		m_row = row;

		m_posTgt = ccp(blockWidth/2 + blockWidth*m_col, blockHeight/2 + blockHeight*m_row);
        m_posTgt = ccpAdd(m_posTgt, posLeftBottom);
        
		m_status = TmBlockStatus_Falling;

		startAnimation();
	}
	else if(action == TmBlockActionCmd_Swap)
	{
		if(this == m_board->m_blockHighlight)
		{
			setAction(m_posCur.x, m_posCur.y, TmBlockActionCmd_Normal);
			m_board->m_blockHighlight = NULL;
		}
		if(this == m_board->m_blockSelected)
		{
			setAction(m_posCur.x, m_posCur.y, TmBlockActionCmd_Normal);
			m_board->m_sprCursor->setVisible(false);
			m_board->m_blockSelected = NULL;
		}
		m_col = col;
		m_row = row;

		m_posTgt = ccp(blockWidth/2 + blockWidth*m_col, blockHeight/2 + blockHeight*m_row);
        m_posTgt = ccpAdd(m_posTgt, posLeftBottom);
        
		m_status = TmBlockStatus_Swap;

		startAnimation();
	}
	else if(action == TmBlockActionCmd_SwapReverse)
	{
		m_col = col;
		m_row = row;

		m_posTgt = ccp(blockWidth/2 + blockWidth*m_col, blockHeight/2 + blockHeight*m_row);
        m_posTgt = ccpAdd(m_posTgt, posLeftBottom);
        
		m_status = TmBlockStatus_SwapReverse;

		startAnimation();
	}
	else if(action == TmBlockActionCmd_Select)
	{
		m_status = TmBlockStatus_Select;
		m_board->m_layerMain->reorderChild(m_layerBlock, 2);

		setForm();
	}
	else if(action == TmBlockActionCmd_Highlight)
	{
		m_status = TmBlockStatus_Highlight;

		setForm();
	}
	else if(action == TmBlockActionCmd_Unselect || action == TmBlockActionCmd_Normal)
	{
		m_status = TmBlockStatus_Normal;
		m_board->m_layerMain->reorderChild(m_layerBlock, 1);

		setForm();
	}
}


void TmBlock::destory()
{
	m_status = TmBlockStatus_Dying;

	if(this == m_board->m_blockHighlight)
		m_board->m_blockHighlight = NULL; 
	if(this == m_board->m_blockSelected)
	{
		m_board->m_sprCursor->setVisible(false);
		m_board->m_blockSelected = NULL; 
	}
	startAnimation();

	//for(unsigned int i = 0;i < m_board->m_commandManager->m_poolCommands->count();i++)
	//{
	//	TmCommand* command = (TmCommand*)m_board->m_commandManager->m_poolCommands->objectAtIndex(i);

	//	if(command->m_commandtype == TmCommandType_SwapBlock)
	//	{
	//		//TmBoard* board = (TmBoard*)command->m_param0;
	//		TmBlock* block0 = (TmBlock*)command->m_param1;
	//		TmBlock* block1 = (TmBlock*)command->m_param2;

	//		if(this == block0 || this == block1)
	//		{
	//			m_board->m_commandManager->m_poolCommands->removeObject(command);
	//		}
	//	}
	//	else if(command->m_commandtype == TmCommandType_HintReal)
	//	{
	//		TmBlock* block0 = (TmBlock*)command->m_param1;
	//		if(this == block0)
	//		{
	//			m_board->m_commandManager->m_poolCommands->removeObject(command);
	//		}
	//	}
	//}
}


void TmBlock::disappear(bool end)
{
	m_status = end ? TmBlockStatus_EndAndDisappear : TmBlockStatus_Disappear;

	if(this == m_board->m_blockHighlight)
		m_board->m_blockHighlight = NULL; 
	if(this == m_board->m_blockSelected)
	{
		m_board->m_sprCursor->setVisible(false);
		m_board->m_blockSelected = NULL; 
	}

	startAnimation();
}


void TmBlock::endAnimationFalling(CCNode* node)
{
	TmBlock* block = (TmBlock*)node->getUserData();

	TmCommand* command = new TmCommand(TmCommandType_FallingBlockReal, block, NULL, NULL, 0, 0);
	block->m_board->m_commandManager->addLast(command);
	command->release();
}


void TmBlock::endAnimationSwap(CCNode* node)
{
	TmBlock* block = (TmBlock*)node->getUserData();
	
	TmCommand* command = new TmCommand(TmCommandType_SwapBlockReal, block, NULL, NULL, 0, 0);
	block->m_board->m_commandManager->addLast(command);
	command->release();
}


void TmBlock::endAnimationSwapReverse(CCNode* node)
{
	TmBlock* block = (TmBlock*)node->getUserData();

	TmCommand* command = new TmCommand(TmCommandType_SwapReverseBlockReal, block, NULL, NULL, 0, 0);
	block->m_board->m_commandManager->addLast(command);
	command->release();
}


void TmBlock::endAnimationDie(CCNode* node)
{
	TmBlock* block = (TmBlock*)node->getUserData();

	TmCommand* command = new TmCommand(TmCommandType_DestoryBlockReal, block, NULL, NULL, 0, 0);
	block->m_board->m_commandManager->addLast(command);
	command->release();
}


void TmBlock::endAnimationDisappear(CCNode* node)
{
	TmBlock* block = (TmBlock*)node->getUserData();

	TmCommand* command = new TmCommand(TmCommandType_DestoryBlockReal, block, NULL, NULL, 0, 0);
	block->m_board->m_commandManager->addLast(command);
	command->release();
}


void TmBlock::initForTraverse()
{
	m_cntMatchV = 0;
	m_cntMatchH = 0;
	m_blockCheckPointV = NULL;
	m_blockCheckPointH = NULL;
	m_cntFall = 0;
   m_checkFall = false;
}


void TmBlock::update(float dt)
{
	if(m_blocktype == TmBlockType_Decay)
	{
		//YtLib* lib = YtLib::sharedLib();

		m_dtDecay -= dt;
		if(m_status == TmBlockStatus_Normal && m_dtDecay <= 0)
		{
			m_dtDecay = m_board->m_decay_duration;

			TmCommand* command = new TmCommand(TmCommandType_DecayAroundBlock, m_board, this, NULL, 0, 0);
			m_board->m_commandManager->addLast(command);
			command->release();
		}
	}
    
    if(m_blockAttr > TmBlockAttr_None)
    {
        m_dtAttr += dt;
        
        if(m_dtAttr > 20.f && m_startBlinkAttr == false)
        {
            CCActionInterval* blink = CCBlink::create(1.f, 5);
            m_sprAttr->runAction(CCRepeat::create(CCSequence::create(blink, NULL), 1000));
            
            m_startBlinkAttr = true;
        }
        else if(m_dtAttr > 25.f)
        {
            setAttr(TmBlockAttr_None, 0);
        }
    }
}

void TmBlock::pause()
{
	m_layerBlock->pauseSchedulerAndActions();

    CCObject* pObj = NULL;
    CCArray* pChildren = m_layerBlock->getChildren();

    CCARRAY_FOREACH(pChildren, pObj)
    {
        CCNode* pChild = (CCNode*)pObj;
        pChild->pauseSchedulerAndActions();
    }
}


void TmBlock::resume()
{
	m_layerBlock->resumeSchedulerAndActions();

    CCObject* pObj = NULL;
    CCArray* pChildren = m_layerBlock->getChildren();

    CCARRAY_FOREACH(pChildren, pObj)
    {
        CCNode* pChild = (CCNode*)pObj;
        pChild->resumeSchedulerAndActions();
    }
}



void TmBlock::endAnimationEffect(CCNode* node)
{
	node->removeFromParentAndCleanup(true);
}



void TmBlock::grillAndDestory()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    CCSprite* sprFood;
    
    m_sprFood->setVisible(false);
    m_sprGlow->setVisible(false);
    m_sprBombTarget->setVisible(false);
    m_sprCross->setVisible(false);
    
    
    if(m_tag.compare("pork") == 0)
    {
        sprFood = CCSprite::createWithSpriteFrameName("block_korean_roast_pork.png");
        m_id = 11;
    }
    
    if(m_tag.compare("beef") == 0)
    {
        sprFood = CCSprite::createWithSpriteFrameName("block_korean_roast_beef.png");
        m_id = 10;
    }
    
    destory();
    m_board->addPoint(this);
    
    CCActionInterval* seq = (CCActionInterval*)(CCSequence::create(CCMoveTo::create(.1f, ccp(-5, 0)), CCMoveTo::create(.1f, ccp(5, 0)), NULL));
    sprFood->runAction(CCRepeat::create(seq, 5));
    
    
    CCPoint curPos = m_board->getGlobalPosition(this);
    CCLayer* layer = CCLayer::create();
    
    CCSprite* sprSmoke0 = CCSprite::createWithSpriteFrameName("game_ui_korean_smoke.png");
    CCSprite* sprSmoke1 = CCSprite::createWithSpriteFrameName("game_ui_korean_smoke.png");
    CCSprite* sprSmoke2 = CCSprite::createWithSpriteFrameName("game_ui_korean_smoke.png");
    
    sprSmoke0->setPosition(ccp(-49, 41));
    sprSmoke0->setScale(0.8);
    sprSmoke0->runAction(CCMoveTo::create(0.5, ccp(-73, 70)));
    sprSmoke0->runAction(CCScaleTo::create(0.5, 1));
    sprSmoke0->runAction(CCFadeOut::create(0.5));
    
    sprSmoke1->setPosition(ccp(29, 43));
    sprSmoke1->setScale(0.6);
    
    sprSmoke1->runAction(CCMoveTo::create(0.5, ccp(49, 54)));
    sprSmoke1->runAction(CCScaleTo::create(0.5, 0.8));
    sprSmoke1->runAction(CCFadeOut::create(0.5));
    
    
    sprSmoke2->setPosition(ccp(15, 86));
    sprSmoke2->setScale(1.0);
    sprSmoke2->runAction(CCMoveTo::create(0.5, ccp(38, 116)));
    sprSmoke2->runAction(CCScaleTo::create(0.5, 1.2));
    sprSmoke2->runAction(CCFadeOut::create(0.5));
    
    
    layer->addChild(sprSmoke0);
    layer->addChild(sprSmoke1);
    layer->addChild(sprSmoke2);
    
    layer->addChild(sprFood);
    
    sprFood->runAction(CCSequence::create(CCDelayTime::create(.45), CCFadeOut::create(.05), NULL));
    
    CCCallFunc* callfunHide =  CCCallFuncN::create(this, callfuncN_selector(TmBlock::endAnimationEffect));
    
    layer->runAction(CCSequence::create(CCDelayTime::create(1), callfunHide, NULL));
    
    tycoonManager->m_layer->addChild(layer, ZORDER_GAME_MOVEBLOCKTOCUSTOMER);
    layer->setPosition(curPos);
    
    tycoonManager->questProcess(QUESTTAG_REMOVE, "GRILL", 1);
    
}