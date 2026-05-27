/*
TmBoard.cpp
Three Match Lib
@initialize at 120907

@history
120907	yoonsr	initialize
*/

#include "TmBoard.h"
#include "TmLayerMain.h"
#include "YtLib.h"
#include "TmLib.h"

#include "CCextAnimatePacker.h"

#include "DyTutorial.h"
#include "DyBlockInfo.h"


TmLayerMain* g_layerMain;

TmBoard::TmBoard()
{
}


TmBoard::TmBoard(int cntCol, int cntRow)
{
    m_isGrill = false;
    
    m_commandManager = new YtCommandManager();
	m_blockSelected = NULL;
	m_isPause = false;
	m_isBombSelectMode = false;
	m_cntCol = cntCol;
	m_cntRow = cntRow;
	m_cntRowBack = m_cntRow*2;

	m_arrMustclearrow = new CCArray();
	m_arrMustclearblock = new CCArray();
	m_arrMustdestory = new CCArray();
	m_arrHaloBlock = new CCArray();
	m_arrFireBlock = new CCArray();
	m_arrMustCrossBlock = new CCArray();
	
	m_arrMustCreateSpecialBlock = new CCArray();
    m_arrMustCreateSpecialAttr = new CCArray();

	m_blocks = new CCArray();
	m_tblBlock = new TmBlock*[m_cntCol*m_cntRowBack];
	m_tblBlockBack = new TmBlock*[m_cntCol*m_cntRowBack];
    
    m_tblGril = new bool[m_cntCol*m_cntRow];
	
    memset(m_tblBlock, 0, sizeof(TmBlock*)*m_cntCol*m_cntRowBack);
	memset(m_tblBlockBack, 0, sizeof(TmBlock*)*m_cntCol*m_cntRowBack);
	
	clearBlocks(true, false);

	m_sprCursor = NULL;
}


TmBoard::~TmBoard()
{
	m_arrMustclearblock->release();
	m_arrMustclearrow->release();
	m_arrMustdestory->release();
	m_arrMustCrossBlock->release();
	m_blocks->release();
	m_arrHaloBlock->release();
	m_arrFireBlock->release();
	
	m_arrMustCreateSpecialBlock->release();
    m_arrMustCreateSpecialAttr->release();
    
    m_commandManager->release();
	delete m_tblBlock;
	delete m_tblBlockBack;
    delete m_tblGril;
}




void TmBoard::disableSelectBlock(TmBlock* block)
{
	m_sprCursor->setVisible(false);
	m_blockSelected = NULL;
}


void TmBoard::bombUnReady()
{
	if(m_isBombSelectMode == false)
		return;

	m_isBombSelectMode = false;
	for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = 0;y < m_cntRow;y++)
		{
			TmBlock* block = m_tblBlock[x + y*m_cntCol];

            if(block == NULL)
                continue;
            
            if(block->m_blocktype == TmBlockType_Blank)
                continue;

            if(block->m_blocktype == TmBlockType_Fix)
                continue;

            block->m_sprBombTarget->stopAllActions();
            block->m_sprBombTarget->setVisible(false);
 		}
	}
}


void TmBoard::bombReady()
{
    if(!isAllBlockUserMovable())
        return;

	m_isBombSelectMode = true;
	
    for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = 0;y < m_cntRow;y++)
		{
			TmBlock* block = m_tblBlock[x + y*m_cntCol];

            if(block == NULL)
                continue;
            
            if(block->m_blocktype == TmBlockType_Blank)
                continue;
            
            if(block->m_blocktype == TmBlockType_Fix)
                continue;

            
			block->m_sprBombTarget->stopAllActions();

			block->m_sprBombTarget->setVisible(true);
			block->m_sprBombTarget->setOpacity(0);

			CCActionInterval* seq = (CCActionInterval*)(CCSequence::create(CCFadeTo::create(.3f, 100), CCFadeTo::create(.3f, 0), NULL));
			block->m_sprBombTarget->runAction(CCRepeatForever::create(seq));
			CCActionInterval* seq2 = (CCActionInterval*)(CCSequence::create(CCScaleTo::create(.3f, 1.00f), CCScaleTo::create(.3f, 1.f), NULL));
			block->m_sprBombTarget->runAction(CCRepeatForever::create(seq2));
		}
	}
}


bool TmBoard::isThreeMathable(TmBlock* block)
{
    if((block->m_status&TmBlockStatus_MaskIdleMove) == TmBlockStatus_Move)
        return false;
    
    if(block->m_blocktype == TmBlockType_Blank)
        return false;
    
    if(block->m_isGrilled)
        return false;
    
    
    
    if(block->m_row > 0)
    {
        if(block->m_col >= 1)
        {
            TmBlock* blockLB = m_tblBlock[block->m_col-1 + (block->m_row-1)*m_cntCol];
            TmBlock* blockL = m_tblBlock[block->m_col-1 + (block->m_row)*m_cntCol];
            
            if(blockLB == NULL && blockL == NULL)
            {
                return false;
            }
        }
        if(block->m_col < m_cntCol-1)
        {
            TmBlock* blockRB = m_tblBlock[block->m_col+1 + (block->m_row-1)*m_cntCol];
            
            TmBlock* blockR = m_tblBlock[block->m_col+1 + (block->m_row)*m_cntCol];
            
            if(blockRB == NULL && blockR == NULL)
            {
                return false;
            }
        }
    }
    return true;
}



bool TmBoard::isAllBlockUserMovable()
{
   	for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = 0;y < m_cntRow;y++)
		{
			TmBlock* block = m_tblBlock[x + y*m_cntCol];
            
			if(block && (block->m_status&TmBlockStatus_MaskLock) == TmBlockStatus_Locked)
				return false;
		}
	}
    return true;
}


bool TmBoard::pangSpecialBlock(bool pang)
{
    for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = 0;y < m_cntRow;y++)
		{
			TmBlock* block = m_tblBlock[x + y*m_cntCol];
			if(block)
            {
                switch(block->m_blocktype)
                {
                    case TmBlockType_Halo:
                    case TmBlockType_Fire:
                    case TmBlockType_Cross:
                        if(pang)
                        {
                            TmCommand* command = new TmCommand(TmCommandType_DestoryBlockLastPang, this, block, NULL, 0, 0);
                            m_commandManager->addLast(command);
                            command->release();
                        }
                        return true;
                    case TmBlockType_Candy:
                    case TmBlockType_MSG:
                        if(pang)
                        {
                            TmCommand* command = new TmCommand(TmCommandType_DestoryBlock, this, block, NULL, 0, 0);
                            m_commandManager->addLast(command);
                            command->release();
                            
                            destoryBlock(block->m_blocktype == TmBlockType_Candy ? "CANDY_B":"MSG_B", getGlobalPosition(block));
                        }
                        return true;
                    default:
// 라스트 보너스 일경우 델리/골드/하트 안먹히도록
//                        if(pang && block->m_blockAttr != TmBlockAttr_None)
//                        {
//                            TmCommand* command = new TmCommand(TmCommandType_DestoryBlockLastPang, this, block, NULL, 0, 0);
//                            m_commandManager->addLast(command);
//                            command->release();
//                            return true;
//                        }
                        break;
                }
                
            }
        }
	}
  
    return false;
}


void TmBoard::endAndClearBlocks()
{
    for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = 0;y < m_cntRow;y++)
		{
			TmBlock* block = m_tblBlock[x + y*m_cntCol];
            
			if(block == NULL)
				continue;
            
            if(block->m_status == TmBlockStatus_Dying || block->m_status == TmBlockStatus_Died)
                continue;
            
			block->disappear(true);
		}
	}
}


void TmBoard::clearBlocks(bool isInit, bool isByCommand)
{
    if(!isAllBlockUserMovable())
        return;
    if(m_blockSelected)
		m_sprCursor->setVisible(false);

    m_arrMustclearblock->removeAllObjects();
	m_arrMustclearrow->removeAllObjects();
	m_arrMustdestory->removeAllObjects();
	m_arrHaloBlock->removeAllObjects();
	m_arrFireBlock->removeAllObjects();
	m_arrMustCrossBlock->removeAllObjects();

	m_arrMustCreateSpecialBlock->removeAllObjects();
    m_arrMustCreateSpecialAttr->removeAllObjects();
    
	m_dtDecay = 0.f;
    
	m_blockHighlight = NULL; 
	m_blockSelected = NULL; 

	CCTime::gettimeofdayCocos2d(&m_timeCombo, NULL);
	m_cntCombo = 0;

	for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = 0;y < m_cntRow;y++)
		{
			TmBlock* block = m_tblBlock[x + y*m_cntCol];

			if(block == NULL)
				continue;
			if(block->m_blocktype == TmBlockType_Candy)
				continue;
			if(block->m_blocktype == TmBlockType_MSG)
				continue;
            if(block->m_blocktype == TmBlockType_Blank)
                continue;
            
            if(block->m_blocktype == TmBlockType_Fix)
                continue;

            
            if(block->m_blockAttr != TmBlockAttr_None)
                continue;

			block->disappear(false);
		}
	}

	if(isByCommand)
	{
        YtLib::sharedLib()->playEffect(YtLibSndEff_REFRESH);
		commandResult(TmCommandType_Refresh, true);
	}
    
    
    if(!isInit)
    {
        for(int x = 0;x < m_cntCol;x++)
        {
            for(int y = m_cntRow;y < m_cntRowBack;y++)
            {
                TmBlock* block = m_tblBlock[x + y*m_cntCol];
                
                if(block == NULL)
                {
                    block = getBlock(false);
                    addBlock(x, y, block);
                    block = arrangeBlocksNotMatch(block, x, y);
                }
                block->m_isStartBlock = true;
            }
        }
    }

    
    
    //
    // 남은 블럭이 매칭 되지 않도록 재정렬 해야함

//    for(int y = m_cntRow;y < m_cntRowBack;y++)
//	{
//		for(int x = 0;x < m_cntCol;x++)
//		{
// 			TmBlock* block = m_tblBlock[x + y*m_cntCol];
//            
//            if(block == NULL)
//            {
//                block = getBlock();
//                addBlock(x, y, block);
//            }
//            
//			if(x > 0 && y > m_cntRow)
//			{
//                TmBlock* blockPrevX = m_tblBlock[x-1 + y*m_cntCol];
//                TmBlock* blockPrevY = m_tblBlock[x + (y-1)*m_cntCol];
//                
//                while(blockPrevX->m_id == block->m_id || blockPrevY->m_id == block->m_id)
//                {
//                    removeBlock(block, true);
//                    block = getBlock();
//                    addBlock(x, y, block);
//                }
//			}
//            
//		}
//	}

    
}


void TmBoard::clearGill()
{
    for(int col = 0;col < m_cntCol;col++)
        for(int row = 0;row < m_cntRow;row++)
        {
            m_tblGril[col + row*m_cntCol] = false;
            
        }
}


void TmBoard::setGrill(int col, int row)
{
    m_tblGril[col + row*m_cntCol] = true;
    
}


bool TmBoard::isGrillZone(int col, int row)
{
    return m_tblGril[col + row*m_cntCol];
}


void TmBoard::displayGrill()
{
    for(int col = 0;col < m_cntCol;col++)
        for(int row = 0;row < m_cntRow;row++)
        {
            if(m_tblGril[col + row*m_cntCol])
            {
                float blockWidth = m_layerMain->m_size.width/m_cntCol;
                float blockHeight = m_layerMain->m_size.height/m_cntRow;
                
                
                CCSprite* sprGrill = CCSprite::createWithSpriteFrameName("game_ui_korean_grill.png");
                m_boardSprite->addChild(sprGrill);
                sprGrill->setPosition(ccp(blockWidth/2 + blockWidth*col + 28, blockHeight/2 + blockHeight*row + 21));
                
                
                sprGrill->setOpacity(60);
                
                CCActionInterval* seq = (CCActionInterval*)(CCSequence::create(CCFadeTo::create(1.f, 120), CCFadeTo::create(1.f, 60), NULL));
                sprGrill->runAction(CCRepeatForever::create(seq));
                
                m_isGrill = true;
            }
        }
}


void TmBoard::setLayerMain(TmLayerMain* layerMain)
{
	m_layerMain = layerMain;

	m_widthBlock = m_layerMain->m_size.width/m_cntCol;
	m_heightBlock = m_layerMain->m_size.height/m_cntRow;

	if(m_widthBlock > m_heightBlock)
		m_distanceBlock = m_widthBlock;
	else
		m_distanceBlock = m_heightBlock;
	
	if(m_sprCursor == NULL)
	{
		m_sprCursor = CCSprite::createWithSpriteFrameName("threematch_ui_cursor.png");
		m_layerMain->addChild(m_sprCursor, 2);
		m_sprCursor->setVisible(false);
        
        if(m_isHard)
            m_sprCursor->setScale(SCALE_HARD);
	}
	g_layerMain = layerMain;
}


void TmBoard::initBlocks()
{
	memset(m_tblBlock, 0, sizeof(TmBlock*)*m_cntCol*m_cntRowBack);
	m_blocks->removeAllObjects();


}



void TmBoard::createInitBlocks()
{
	int countY = 0;
	int countX = 0;
    
	for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = m_cntRow;y < m_cntRowBack;y++)
		{
			TmBlock* block = getBlock(false);
            addBlock(x, y, block);
			
            block = arrangeBlocksNotMatch(block, x, y);
            
			block->m_delayFallPre = 1.0f + 0.08f*(countY + countX);
			
            if(m_isHard)
                block->m_delayFallPost = 0.08f*(28 - (countY + countX));
            else
                block->m_delayFallPost = 0.08f*(25 - (countY + countX));
            
            
            if(block->m_delayFallPost > .5)
                block->m_delayFallPost = .5;
            
            block->m_isStartBlock = true;
            
            //CCLog("x=%d, y=%d, pre=%f, post=%f, countY + countX = %d", x, y, block->m_delayFallPre, block->m_delayFallPost, countY + countX);
			countY++;
		}
		countX += 3;
		countY = 0;
	}
    
    
    
    
    //
    // blank block
    
    //    for(int i = 1;i<3;i++)
    //    {
    //        TmBlock* block = getBlock();
    //        addBlock(i, 4, block);
    //        block->setBlockType(TmBlockType_Blank);
    //    }
    //    for(int i = 4;i<6;i++)
    //    {
    //        TmBlock* block = getBlock();
    //        addBlock(i, 4, block);
    //        block->setBlockType(TmBlockType_Blank);
    //    }
    //
    //    TmBlock* block = getBlock();
    //    addBlock(1, 2, block);
    //    block->setBlockType(TmBlockType_Fix);
    
    
    
    //    if(!checkDestoryableBlock(m_cntRow, false, false))
    //    {
    //        initBlocks();
    //    }
    
}


void TmBoard::addBlock(int col, int row, TmBlock* block)
{
	m_blocks->addObject(block);
	m_tblBlock[col + row*m_cntCol] = block;
	m_layerMain->addChild(block->m_layerBlock, 1);

	block->setAction(col, row, TmBlockActionCmd_Add);
	block->release();

	if(row < m_cntRow)
		block->m_layerBlock->setVisible(true);
	else
		block->m_layerBlock->setVisible(false);
}


void TmBoard::removeBlock(TmBlock* block, bool cleanNode)
{
	m_tblBlock[block->m_col + block->m_row*m_cntCol] = NULL;
	m_blocks->removeObject(block);
}


void TmBoard::moveBlockTo(int col, int row, TmBlock* block)
{
	m_tblBlock[block->m_col + block->m_row*m_cntCol] = NULL;
	m_tblBlock[col + row*m_cntCol] = block;
	block->setAction(col, row, TmBlockActionCmd_Fall);

	if(row < m_cntRow)
		block->m_layerBlock->setVisible(true);
	else
		block->m_layerBlock->setVisible(false);
}


void TmBoard::traverseRight(TmBlock* block, int col, int row)
{
	int count = 1;
	for(int i = col;i < m_cntCol-1;i++)
	{
		TmBlock* blockRight = m_tblBlock[i+1 + row*m_cntCol];  

		if(blockRight == NULL)
			break;

		if(!isThreeMathable(blockRight))
			break;
		
		if(block->m_id == blockRight->m_id)
			count++;
		else
			break;
	}

	block->m_cntMatchH = count;

	for(int i = col;i < m_cntCol-1;i++)
	{
		TmBlock* blockRight = m_tblBlock[i+1 + row*m_cntCol];  

		if(blockRight == NULL)
			break;

		if(!isThreeMathable(blockRight))
			break;
		
		if(block->m_id == blockRight->m_id)
		{
			blockRight->m_cntMatchH = count;
			blockRight->m_blockCheckPointH = block;
		}
		else
			break;
	}
}


void TmBoard::traverseUp(TmBlock* block, int startY, int col, int row)
{
	int count = 1;
	for(int i = row;i < startY+m_cntRow-1;i++)
	{
		TmBlock* blockUp = m_tblBlock[col + (i+1)*m_cntCol];  

		if(blockUp == NULL)
			break;

		if(!isThreeMathable(blockUp))
			break;

		if(block->m_id == blockUp->m_id)
			count++;
		else 
			break;
	}

	block->m_cntMatchV = count;

	for(int i = row;i < startY+m_cntRow-1;i++)
	{
		TmBlock* blockUp = m_tblBlock[col + (i+1)*m_cntCol];  

		if(blockUp == NULL)
			break;

		if(!isThreeMathable(blockUp))
			break;
		
		if(block->m_id == blockUp->m_id)
		{
			blockUp->m_cntMatchV = count;
			blockUp->m_blockCheckPointV = block;
		}
		else
			break;
	}
}


void TmBoard::traverse(int startY)
{
	for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = 0;y < m_cntRowBack;y++)
		{
			TmBlock* block = m_tblBlock[x + y*m_cntCol];
			if(block)
			{
				block->initForTraverse();
			}
		}
	}

	for(int y = startY;y < startY+m_cntRow;y++)
	{
		for(int x = 0;x < m_cntCol;x++)	
		{
			TmBlock* block = m_tblBlock[x + y*m_cntCol];

			if(block == NULL || !isThreeMathable(block))
				continue;

			if(block->m_blockCheckPointH == NULL)
			{
				traverseRight(block, x, y);
			}

			if(block->m_blockCheckPointV == NULL)
			{
				traverseUp(block, startY, x, y);
			}
		}
	}
}


bool TmBoard::pushValue(CCArray* array, int value, int param)
{
	for(unsigned int i = 0;i < array->count();i++)
	{
		CCArray* inner = (CCArray*)array->objectAtIndex(i);

		if(((CCInteger*)inner->objectAtIndex(0))->getValue() == value)
		{
			return false;
		}
	}	
	CCArray* arrInner = new CCArray();
	arrInner->addObject(CCInteger::create(value));
	arrInner->addObject(CCInteger::create(param));

	array->addObject(arrInner);
	arrInner->release();

	return true;
}


bool TmBoard::removeValue(CCArray* array, int value)
{
	for(unsigned int i = 0;i < array->count();i++)
	{
		CCArray* inner = (CCArray*)array->objectAtIndex(i);

		if(((CCInteger*)inner->objectAtIndex(0))->getValue() == value)
		{
			array->removeObjectAtIndex(i);
			return true;
		}
	}		
	return false;
}


void TmBoard::playeralarmCombo1(float dt)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_PANG);

	g_layerMain->unschedule(schedule_selector(TmBoard::playeralarmCombo1));
}



void TmBoard::playeralarmCombo2(float dt)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_PANG);
	g_layerMain->unschedule(schedule_selector(TmBoard::playeralarmCombo2));
}



void TmBoard::playeralarmCombo3(float dt)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_PANG);
	g_layerMain->unschedule(schedule_selector(TmBoard::playeralarmCombo3));
}

void TmBoard::playeralarmComboNormal(float dt)
{
   	YtLib::sharedLib()->playEffect(YtLibSndEff_Combo_Normal);
	g_layerMain->unschedule(schedule_selector(TmBoard::playeralarmComboNormal));
}
void TmBoard::playeralarmComboHigh(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_Combo_High);
	g_layerMain->unschedule(schedule_selector(TmBoard::playeralarmComboHigh));
}


void TmBoard::checkCombo(bool dieByHalo, bool dieByCross)
{
	m_cntCombo++;	
	CCTime::gettimeofdayCocos2d(&m_timeCombo, NULL);
	
	if(m_cntCombo == 1)
	{
        m_layerMain->unschedule(schedule_selector(TmBoard::playeralarmCombo1));
		if(dieByHalo || dieByCross)
		{
			YtLib::sharedLib()->playEffect(dieByHalo ? YtLibSndEff_BLOCK_HALO : YtLibSndEff_BLOCK_CLOSS);
			m_layerMain->schedule(schedule_selector(TmBoard::playeralarmCombo1), .3f);
		}
		else
            m_layerMain->schedule(schedule_selector(TmBoard::playeralarmCombo1), .1f);
	}
	else if(m_cntCombo == 2)
	{
        m_layerMain->unschedule(schedule_selector(TmBoard::playeralarmCombo2));
		if(dieByHalo || dieByCross)
		{
			YtLib::sharedLib()->playEffect(dieByHalo ? YtLibSndEff_BLOCK_HALO : YtLibSndEff_BLOCK_CLOSS);
			m_layerMain->schedule(schedule_selector(TmBoard::playeralarmCombo2), .3f);
		}
		else
			m_layerMain->schedule(schedule_selector(TmBoard::playeralarmCombo2), .1f);
	}
	else
	{
        m_layerMain->unschedule(schedule_selector(TmBoard::playeralarmCombo3));
        if(dieByHalo || dieByCross)
		{
			YtLib::sharedLib()->playEffect(dieByHalo ? YtLibSndEff_BLOCK_HALO : YtLibSndEff_BLOCK_CLOSS);
            m_layerMain->schedule(schedule_selector(TmBoard::playeralarmCombo3), .3f);
		}
		else
			m_layerMain->schedule(schedule_selector(TmBoard::playeralarmCombo3), .1f);
	}

    if(m_cntCombo > 0 && m_cntCombo%10 == 0)
    {
        m_layerMain->unschedule(schedule_selector(TmBoard::playeralarmComboHigh));
        m_layerMain->schedule(schedule_selector(TmBoard::playeralarmComboHigh), .1f);
    }
    else if(m_cntCombo > 0 && m_cntCombo%5 == 0)
    {
        m_layerMain->unschedule(schedule_selector(TmBoard::playeralarmComboNormal));
        m_layerMain->schedule(schedule_selector(TmBoard::playeralarmComboNormal), .1f);
    }
    
	displayStatus();
	if(m_cntCombo >= CNT_COMBO_FIRE && m_cntCombo%CNT_COMBO_FIRE == 0)
	{
		TmBlock* block = getRandomNomalBlock();

		if(block)
        {
			block->setBlockType(TmBlockType_Fire);
        }
	}
    
    comboEvent();
}



TmBlock* TmBoard::getRandomNomalBlock()
{
	TmBlock* blockFound = NULL;
	int count = 0;
	while(blockFound == NULL)
	{
		int row = rand()%m_cntRow;
		int col = rand()%m_cntCol;

		TmBlock* block = m_tblBlock[col + row*m_cntCol];

		if(block && block->m_status == TmBlockStatus_Normal && block->m_blocktype == TmBlockType_Normal && block->m_dieType == TmBlockDieType_None && block->m_blockAttr == TmBlockAttr_None)
		{  
			blockFound = block;
		}
		count++;
		if(count > m_cntRow*m_cntCol)
			break;
	}
	return blockFound;
}


bool TmBoard::addDestoryableBlock(TmBlock* block, TmBlockDieType dieType, bool checkIce)
{
	if(block && m_arrMustdestory->containsObject(block) == false && block->m_status != TmBlockStatus_Dying && block->m_status != TmBlockStatus_Died)
	{
		m_arrMustdestory->addObject(block);
		if(block->m_status == TmBlockStatus_Highlight)
			block->setAction(block->m_posCur.x, block->m_posCur.y, TmBlockActionCmd_Normal);
		block->m_dieType = dieType;

		if(checkIce)
		{
            if(block->m_blocktype == TmBlockType_Ice)
            {
                destoryBlock("ICE", getGlobalPosition(block));
                block->setBlockType(TmBlockType_Normal);
            }
            
            if(block->m_blocktype == TmBlockType_Fix)
            {
                destoryBlock("FIX", getGlobalPosition(block));
                block->setBlockType(TmBlockType_Ice);
            }
		}
		return true;
	}
	return false;
}


CCPoint TmBoard::getGlobalPosition(TmBlock* block)
{
    CCPoint curPos = m_layerMain->convertToWorldSpace(block->m_posCur);
    return m_layerMain->getParent()->convertToNodeSpace(curPos);
}


void TmBoard::endAnimationEffect(CCNode* node)
{
	node->removeFromParentAndCleanup(true);
}



void TmBoard::checkDestory()
{
	bool hasDestoryBlock = false;
	static int checkcnt[10] = {0, };
	static int checkhalo[10] = {0, };

	static cc_timeval _time;
	CCTime::gettimeofdayCocos2d(&_time, NULL);
	float interval = CCTime::timersubCocos2d(&m_timeCombo, &_time);

	if(interval > COMBO_TIME)
	{
		m_cntCombo = 0;
		displayStatus();
	}
	memset(checkcnt, 0, sizeof(int)*10);
	memset(checkhalo, 0, sizeof(int)*10);

	for(unsigned int i = 0;i < m_arrMustdestory->count();)
	{
		TmBlock* block = (TmBlock*)m_arrMustdestory->objectAtIndex(i);

//#ifdef WIN32
//		assert(block->m_id < 10);
//#else
//		if(block->m_id >= 10)
//		{
//			m_arrMustdestory->removeObjectAtIndex(i);
//			continue;
//		}
//#endif
		switch(block->m_blocktype)
		{
			case TmBlockType_Decay:
			case TmBlockType_Normal:
				if(block->m_dieType == TmBlockDieType_None)
					checkcnt[block->m_id]++;
			case TmBlockType_Halo:
			case TmBlockType_Cross:
				break;
			case TmBlockType_Ice:
				break;
			case TmBlockType_Fix:
				break;
			case TmBlockType_Fire:
				break;
            default:
                break;
		}
		i++;
	}
	
	bool dieByHalo = false;
	bool dieByCross = false;
		
	while(m_arrMustdestory->count() > 0)
	{
		TmBlock* block = (TmBlock*)m_arrMustdestory->objectAtIndex(0);

		switch(block->m_blocktype)
		{
			case TmBlockType_Decay:
            {
                YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_DECAY_DESTROY);
				destoryBlock("DECAY", getGlobalPosition(block));
            }
			case TmBlockType_Normal:

				if(block->m_status == TmBlockStatus_Normal || block->m_status == TmBlockStatus_SwapCheck)
				{   
                    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
                    
                    if(tutorial && (tutorial->m_command_type == DyTutorialCommandType_ThreeMatch))
                    {
                        if(block->m_id == tutorial->m_paramInt0)
                        {
                            tutorial->m_value_internal++;
                            
                            if(tutorial->m_value_internal == 3)
                            {
                                DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    
                    if(tutorial && (tutorial->m_command_type == DyTutorialCommandType_RemoveCurrentBlock))
                    {
                        if(block == DyTutorialManager::sharedTutorialManager()->m_currentBlock)
                        {
                            DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
                        }
                        else
                        {
                            break;
                        }
                    }
                    
					if(checkcnt[block->m_id] >= CNT_BLOCK_HALO)
					{
						if(pushValue(m_arrHaloBlock, block->m_id, 0))
						{
							if(block->m_status == TmBlockStatus_SwapCheck)
							{
								block->m_blockSwap->setAction(block->m_blockSwap->m_posCur.x, block->m_blockSwap->m_posCur.y, TmBlockActionCmd_Normal);
								block->setAction(block->m_posCur.x, block->m_posCur.y, TmBlockActionCmd_Normal);
							}
                            
							block->setBlockType(TmBlockType_Halo);

							m_arrMustdestory->removeObjectAtIndex(0);
							checkcnt[block->m_id] = 0;
							addPoint(block);
							continue;
						}
					}
					else if(checkcnt[block->m_id] >= CNT_BLOCK_CROSS)
					{
						if(block->m_status == TmBlockStatus_SwapCheck)
						{
							block->m_blockSwap->setAction(block->m_blockSwap->m_posCur.x, block->m_blockSwap->m_posCur.y, TmBlockActionCmd_Normal);
							block->setAction(block->m_posCur.x, block->m_posCur.y, TmBlockActionCmd_Normal);
						}
                        
						block->setBlockType(TmBlockType_Cross);

						m_arrMustdestory->removeObjectAtIndex(0);
						checkcnt[block->m_id] = 0;
						addPoint(block);
						continue;
					}

					if(checkhalo[block->m_id] == 0)
					{
						if(block->m_dieType == TmBlockDieType_ByCross)
							dieByCross = true;
						if(block->m_dieType == TmBlockDieType_ByHalo)
							dieByHalo = true;
                        
						hasDestoryBlock = true;
						block->destory();		
						addPoint(block);
					}
				}
				break;
			case TmBlockType_Candy:
			case TmBlockType_MSG:
                {
                    if(selectSpecialBlock(block) == false)
                    {
                        block->destory();
                        
                        destoryBlock(block->m_blocktype == TmBlockType_Candy ? "CANDY_B":"MSG_B", getGlobalPosition(block));
                        
                    }
                }
				break;
			case TmBlockType_Fix:
				destoryBlock("FIX", getGlobalPosition(block));
				block->m_status = TmBlockStatus_Normal;
				block->setBlockType(TmBlockType_Ice);
               break;

			case TmBlockType_Ice:
				destoryBlock("ICE", getGlobalPosition(block));
				block->m_status = TmBlockStatus_Normal;
				block->setBlockType(TmBlockType_Normal);

				{
					CCSprite* sprIce = CCSprite::createWithSpriteFrameName("threematch_ui_effect_ice00.png");
						
					sprIce->setPosition(block->m_layerBlock->getPosition());

                    CCCallFunc* callfunHide =  CCCallFuncN::create(this, callfuncN_selector(TmBoard::endAnimationEffect));
    
                    
					sprIce->runAction(CCSequence::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_ice"), CCFadeOut::create(.1f), callfunHide, NULL));
					this->m_layerMain->addChild(sprIce);
                    
                    if(m_isHard)
                        sprIce->setScale(SCALE_HARD);


				}
				YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_ICE_BREAK);
				break;
			case TmBlockType_Halo:
				destoryBlock("HALO", getGlobalPosition(block));
				block->setBlockType(TmBlockType_Normal);
				if(block->m_status == TmBlockStatus_SwapCheck)
				{
					block->m_blockSwap->setAction(block->m_blockSwap->m_posCur.x, block->m_blockSwap->m_posCur.y, TmBlockActionCmd_Normal);
					block->setAction(block->m_posCur.x, block->m_posCur.y, TmBlockActionCmd_Normal);
				}
				removeValue(m_arrHaloBlock, block->m_id);
				pushValue(m_arrMustclearblock, block->m_id, 0);
				break;
			case TmBlockType_Cross:
				destoryBlock("CROSS", getGlobalPosition(block));
				block->setBlockType(TmBlockType_Normal);

				if(block->m_status == TmBlockStatus_SwapCheck)
				{
					block->m_blockSwap->setAction(block->m_blockSwap->m_posCur.x, block->m_blockSwap->m_posCur.y, TmBlockActionCmd_Normal);
					block->setAction(block->m_posCur.x, block->m_posCur.y, TmBlockActionCmd_Normal);
				}

				m_arrMustCrossBlock->addObject(block);

				break;
			case TmBlockType_Fire:
				destoryBlock("FIRE", getGlobalPosition(block));
				block->setBlockType(TmBlockType_Normal);

				if(block->m_status == TmBlockStatus_SwapCheck)
				{
					block->m_blockSwap->setAction(block->m_blockSwap->m_posCur.x, block->m_blockSwap->m_posCur.y, TmBlockActionCmd_Normal);
					block->setAction(block->m_posCur.x, block->m_posCur.y, TmBlockActionCmd_Normal);
				}

				if(pushValue(m_arrMustclearrow, block->m_row, block->m_col))
				{
					YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_FIRE);

					if(block->m_col < this->m_cntCol-1)
					{
						CCSprite* fireRight = CCSprite::createWithSpriteFrameName("threematch_ui_effect_beam00.png");
						
						fireRight->setAnchorPoint(ccp(0, 0.5f));
						
						CCPoint sourcePoint = ccp(block->m_layerBlock->getPosition().x - m_widthBlock, block->m_layerBlock->getPosition().y);
						fireRight->setPosition(sourcePoint);

						CCPoint tagetPoint = ccp(sourcePoint.x + m_widthBlock*(m_cntCol - block->m_col - 1) + m_widthBlock/2, sourcePoint.y);
						float distance = ccpDistance(block->m_layerBlock->getPosition(), tagetPoint);
						CCActionInterval* moveto = CCMoveTo::create(0.002f*distance, tagetPoint);
						CCActionInterval* fadeout = CCFadeOut::create(.5f);

                        CCCallFunc* callfunHide =  CCCallFuncN::create(this, callfuncN_selector(TmBoard::endAnimationEffect));
    
                        
						fireRight->runAction(CCRepeatForever::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_beam")));
						fireRight->runAction(CCSequence::create(moveto, fadeout, callfunHide, NULL));
						this->m_layerMain->addChild(fireRight);
                        
                        if(m_isHard)
                            fireRight->setScale(SCALE_HARD);

					}
					if(block->m_col > 0)
					{
						CCSprite* fireLeft = CCSprite::createWithSpriteFrameName("threematch_ui_effect_beam00.png");
						
						fireLeft->setAnchorPoint(ccp(1, 0.5f));
						fireLeft->setFlipX(true);
						CCPoint sourcePoint = ccp(block->m_layerBlock->getPosition().x + m_widthBlock, block->m_layerBlock->getPosition().y);
						fireLeft->setPosition(sourcePoint);
						CCPoint tagetPoint = ccp(0, sourcePoint.y);
						float distance = ccpDistance(block->m_layerBlock->getPosition(), tagetPoint);
						CCActionInterval* moveto = CCMoveTo::create(0.002f*distance, tagetPoint);
						CCActionInterval* fadeout = CCFadeOut::create(.5f);

                        CCCallFunc* callfunHide =  CCCallFuncN::create(this, callfuncN_selector(TmBoard::endAnimationEffect));
                        
                        
						fireLeft->runAction(CCRepeatForever::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_beam_flip")));
						fireLeft->runAction(CCSequence::create(moveto, fadeout, callfunHide, NULL));
						this->m_layerMain->addChild(fireLeft);
                        
                        if(m_isHard)
                            fireLeft->setScale(SCALE_HARD);

					}
				}
				break;
            default:
                break;
		}
		m_arrMustdestory->removeObjectAtIndex(0);
	}

	while(m_arrMustCrossBlock->count() > 0)
	{
		TmBlock* block[5];
		block[0] = (TmBlock*)m_arrMustCrossBlock->objectAtIndex(0);
		block[1] = NULL;
		block[2] = NULL;
		block[3] = NULL;
		block[4] = NULL;
		
		if(block[0]->m_row > 0)
			block[1] = m_tblBlock[block[0]->m_col + (block[0]->m_row-1)*m_cntCol];

		if(block[0]->m_row < m_cntRow - 1)
			block[2] = m_tblBlock[block[0]->m_col + (block[0]->m_row+1)*m_cntCol];

		if(block[0]->m_col > 0)
			block[3] = m_tblBlock[(block[0]->m_col-1) + (block[0]->m_row)*m_cntCol];

		if(block[0]->m_col < m_cntCol - 1)
			block[4] = m_tblBlock[block[0]->m_col+1 + (block[0]->m_row)*m_cntCol];


		for(int i = 0;i < 5;i++)
		{
			if(block[i] && (block[i]->m_status == TmBlockStatus_Normal || block[i]->m_status == TmBlockStatus_SwapCheck))
			{
				addDestoryableBlock(block[i], TmBlockDieType_ByCross, true);
			}
		}
		m_arrMustCrossBlock->removeObjectAtIndex(0);
	}

	while(m_arrMustclearblock->count() > 0)
	{
		CCArray* arrInner = (CCArray*)m_arrMustclearblock->objectAtIndex(0);

		int block_id = ((CCInteger*)(arrInner->objectAtIndex(0)))->getValue();

		for(int x = 0;x < m_cntCol;x++)
		{
			for(int y = 0;y < m_cntRow;y++)
			{
				TmBlock* block = m_tblBlock[x + y*m_cntCol];
				if(block == NULL)
					continue;

				if(block->m_id == block_id && (block->m_status == TmBlockStatus_Normal || block->m_status == TmBlockStatus_SwapCheck))
				{
					addDestoryableBlock(block, TmBlockDieType_ByHalo, true);
				}
			}
		}

		m_arrMustclearblock->removeObjectAtIndex(0);
	}

	while(m_arrMustclearrow->count() > 0)
	{
		CCArray* arrInner = (CCArray*)m_arrMustclearrow->objectAtIndex(0);

		int row = ((CCInteger*)(arrInner->objectAtIndex(0)))->getValue();
		int col = ((CCInteger*)(arrInner->objectAtIndex(1)))->getValue();
		for(int x = 0;x < m_cntCol;x++)
		{
			TmBlock* block = m_tblBlock[x + row*m_cntCol];
			if(block == NULL)
				continue;

			if(block->m_status == TmBlockStatus_Normal || block->m_status == TmBlockStatus_SwapCheck)
			{
				if(addDestoryableBlock(block, TmBlockDieType_ByFire, true))
				{
					block->m_delayDiePost = abs(col-block->m_col)*.1f;
				}
			}
		}
		m_arrMustclearrow->removeObjectAtIndex(0);
	}

	if(hasDestoryBlock)
	{	
		checkCombo(dieByHalo, dieByCross);
        
        while(m_arrMustCreateSpecialAttr->count() > 0)
		{
			TmBlockAttr attr = (TmBlockAttr)((CCInteger *)m_arrMustCreateSpecialAttr->objectAtIndex(0))->getValue();
            int attrValue = (int)((CCInteger *)m_arrMustCreateSpecialAttr->objectAtIndex(1))->getValue();
			
			TmBlock* block = getRandomNomalBlock();
			if(block)
            {
                //
                // 캔디나 MSG
				block->setAttr(attr, attrValue);
            }
			m_arrMustCreateSpecialAttr->removeObjectAtIndex(0);
            m_arrMustCreateSpecialAttr->removeObjectAtIndex(0);
		}
	}
    
    while(m_arrMustCreateSpecialBlock->count() > 0)
    {
        TmBlockType type = (TmBlockType)((CCInteger *)m_arrMustCreateSpecialBlock->objectAtIndex(0))->getValue();
        TmBlock* block = getRandomNomalBlock();
        if(block)
        {
            //
            // 캔디나 MSG
            
            block->setBlockType(type);
        }
        m_arrMustCreateSpecialBlock->removeObjectAtIndex(0);
    }
}


int TmBoard::checkMatching(int startY, TmBlock* blockSelect, bool matchAndDie)
{
    int count = 0;
    if(!matchAndDie)
    {
        if((blockSelect->m_status&TmBlockStatus_MaskIdleMove) == TmBlockStatus_Idle && (blockSelect->m_cntMatchH > 2 || blockSelect->m_cntMatchV > 2))
            count = 1;
        
    }
    else
    {
        for(int y = startY;y < startY+m_cntRow;y++)
        {
            for(int x = 0;x < m_cntCol;x++)	
            {
                TmBlock* block = m_tblBlock[x + y*m_cntCol];

                if(block == NULL)
                    continue;

                if((block->m_status&TmBlockStatus_MaskIdleMove) == TmBlockStatus_Idle && (block->m_cntMatchH > 2 || block->m_cntMatchV > 2))
                {
                    if(block->m_status == TmBlockStatus_SwapCheck)
                    {
                        block->m_blockSwap->setAction(block->m_blockSwap->m_posCur.x, block->m_blockSwap->m_posCur.y, TmBlockActionCmd_Normal);
                    }

                    addDestoryableBlock(block, TmBlockDieType_None, false);	
                }
            }
        }
    }
	return count;
}


void TmBoard::checkDie()
{
	for(int y = 0;y < m_cntRowBack;y++)
	{
		for(int x = 0;x < m_cntCol;x++)	
		{
 			TmBlock* block = m_tblBlock[x + y*m_cntCol];

			if(block == NULL)
				continue;

			if(block->m_status == TmBlockStatus_Died)
			{
				removeBlock(block, false);
			}
		}
	}
}

void TmBoard::fallBlocksCheckUp(TmBlock* block, int fallCnt)
{
	if(block->m_row >= this->m_cntRowBack)
		return;

    block->m_cntFall = fallCnt;
	block->m_checkFall = true;
    
    if(block->m_row >= this->m_cntRowBack-1)
		return;

	TmBlock* blockUp = m_tblBlock[block->m_col + (block->m_row+1)*m_cntCol];
    
    
	if(blockUp != NULL && isFallable(blockUp))
	{
        blockUp->m_rowGrill = block->m_rowGrill;
        
        bool isGrillableBlock = (blockUp->m_tag.compare("pork") == 0 || blockUp->m_tag.compare("beef") == 0);
        
        if(isGrillableBlock && m_isGrill)
        {
            int distanceGrill = blockUp->m_row - blockUp->m_rowGrill;
            
            if(fallCnt >= distanceGrill)
            {
                fallCnt = distanceGrill;
                blockUp->m_isGrilled = true;
            }
        }

		blockUp->m_cntFall = fallCnt;
		fallBlocksCheckUp(blockUp, fallCnt);
        
	}
	else if(blockUp == NULL)
	{
		int offstY = 2;
		while(block->m_row+offstY < m_cntRowBack && (blockUp = m_tblBlock[block->m_col + (block->m_row+offstY)*m_cntCol]) == NULL)
		{
			offstY++;
		}

		if(blockUp != NULL && isFallable(blockUp))
        {
            blockUp->m_rowGrill = block->m_rowGrill;
            
            fallCnt = fallCnt+offstY-1;
            
            bool isGrillableBlock = (blockUp->m_tag.compare("pork") == 0 || blockUp->m_tag.compare("beef") == 0);
            
            if(isGrillableBlock && m_isGrill)
            {
                int distanceGrill = blockUp->m_row - blockUp->m_rowGrill;
                
                if(fallCnt >= distanceGrill)
                {
                    fallCnt = distanceGrill;
                    blockUp->m_isGrilled = true;
                }
            }
            
            blockUp->m_cntFall = fallCnt;
			fallBlocksCheckUp(blockUp, fallCnt);
        }
	}
}


bool TmBoard::isBlank(int x, int y)
{
    TmBlock* block = m_tblBlock[x + y*m_cntCol];
    
    if(block == NULL)
        return true;
    
    if(block->m_blocktype == TmBlockType_Blank)
        return false;
    
    if(block->m_blocktype == TmBlockType_Fix)
        return false;

    return false;
}


bool TmBoard::isFallable(TmBlock* block)
{
    if(block->m_blocktype == TmBlockType_Blank)
        return false;

    if(block->m_blocktype == TmBlockType_Fix)
        return false;
    
    if((block->m_status&TmBlockStatus_MaskLock) == TmBlockStatus_UnLocked)
        return true;
    
    return false;
}




void TmBoard::fallBlocks()
{
	for(int y = 1;y < m_cntRowBack;y++)
	{
		for(int x = 0;x < m_cntCol;x++)	
		{
 			TmBlock* block = m_tblBlock[x + y*m_cntCol];

			if(block == NULL)
				continue;
            
            bool isGrillableBlock = block->m_tag.compare("pork") == 0 || block->m_tag.compare("beef") == 0;
            
			
			if(isFallable(block) && block->m_checkFall == false && block->m_isGrilled == false)
			{
				int curY = y;
				block->m_cntFall = 0;
                block->m_rowGrill = -100;
                
                for(int rowGrill = 0;rowGrill < m_cntRow;rowGrill++)
                {
                    if(isGrillZone(x, rowGrill))
                        block->m_rowGrill = rowGrill;
                }
			  //
                // 밑의 빈 공간을 찾고
				while(curY-1 >= 0 && isBlank(x, curY-1))
				{
                    bool grillZone = isGrillZone(x, curY-1);
                    
                    curY--;
					block->m_cntFall++;
                    
                    if(grillZone && m_isGrill)
                    {
                        //block->m_rowGrill = curY;
                        
                        if(isGrillableBlock)
                        {
                            block->m_isGrilled = true;
                            break;
                        }
                    }
				}

                //
                // 상단 블럭을 그 갯수만큼 떨어트린다.
				if(block->m_cntFall > 0)
				{	
					fallBlocksCheckUp(block, block->m_cntFall);
				}
			}
		}
	}
	memcpy(m_tblBlockBack, m_tblBlock, sizeof(TmBlock*)*m_cntCol*m_cntRowBack);
	for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = 1;y < m_cntRowBack;y++)
		{
 			TmBlock* block = m_tblBlock[x + y*m_cntCol];

			if(block == NULL)
            {
				continue;
            }

			if(block->m_cntFall > 0)
			{
				moveBlockTo(block->m_col, block->m_row-block->m_cntFall, block);
			}
		}
	}
    
    if(m_hasBlank)
    {
        for(int y = 1;y < m_cntRowBack;y++)
        {
            for(int x = 0;x < m_cntCol;x++)
            {
                TmBlock* block = m_tblBlock[x + y*m_cntCol];
                
                if(block == NULL)
                {
                    continue;
                }
                
                if(!isFallable(block))
                    continue;
                
                if(x >= 1)
                {
                    TmBlock* blockLB = m_tblBlock[x-1 + (y-1)*m_cntCol];
                    
                    if(blockLB == NULL)
                    {
                        moveBlockTo(x-1, y-1, block);
                        return;
                    }
                }
                if(x < m_cntCol-1)
                {
                    TmBlock* blockRB = m_tblBlock[x+1 + (y-1)*m_cntCol];
                    
                    if(blockRB == NULL)
                    {
                        moveBlockTo(x+1, y-1, block);
                        return;
                    }
                }
            }
        }
    }

}


void TmBoard::updateBlocks(float dt)
{	
	int countDecay = 0;
	for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = 0;y < m_cntRow;y++)
		{
 			TmBlock* block = m_tblBlock[x + y*m_cntCol];
			if(block)
			{
				if(block->m_status == TmBlockStatus_SwapCheck)
				{
                    //
                    // 헛 스윙
					swapBlock(block, block->m_blockSwap, true);
				}
				block->update(dt);

				if(block->m_blocktype == TmBlockType_Decay)
					countDecay++;
                
                
                bool isGrillableBlock = block->m_tag.compare("pork") == 0 || block->m_tag.compare("beef") == 0;
                bool grillZone = isGrillZone(x, y);
                
                if(isGrillableBlock && grillZone && m_isGrill && !block->m_isGrilled && (block->m_status&TmBlockStatus_MaskLock) == TmBlockStatus_UnLocked)
                {
                    block->m_isGrilled = true;
                    block->grillAndDestory();
                }
			}
		}
	}

	if(countDecay == 0)
		m_dtDecay += dt;
	else
		m_dtDecay = 0;

	if(m_dtDecay > m_decay_idle_duration)
	{
		m_dtDecay = 0;

		TmBlock* block = getRandomNomalBlock();
		if(block)
			block->setBlockType(TmBlockType_Decay);
	}
}


TmBlock* TmBoard::arrangeBlocksNotMatch(TmBlock* block, int x, int y)
{
    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    
    DyTutorial* tutorial = tutorialManager->getCurrentTutorial();
    
    if(tutorial && tutorial->m_tutorial_type == DyTutorialType_BlockSettingDefault)
    {
        return block;
    }
    
    int try_cnt = 0;
    if(x > 1 && y == m_cntRow)
    {
        TmBlock* blockPrevX = m_tblBlock[x-1 + y*m_cntCol];
        TmBlock* blockPrevPrevX = m_tblBlock[x-2 + y*m_cntCol];
        
        try_cnt = 0;
        while(blockPrevPrevX->m_id == block->m_id && blockPrevX->m_id == block->m_id && try_cnt < 5)
        {
            removeBlock(block, true);
            block = getBlock(false);
            addBlock(x, y, block);
            try_cnt++;
        }
    }
    else if(y > m_cntRow+1 && x == 0)
    {
        TmBlock* blockPrevY = m_tblBlock[x + (y-1)*m_cntCol];
        TmBlock* blockPrevPrevY = m_tblBlock[x + (y-2)*m_cntCol];
        
        try_cnt = 0;
        while(blockPrevPrevY->m_id == block->m_id && blockPrevY->m_id == block->m_id && try_cnt < 5)
        {
            removeBlock(block, true);
            block = getBlock(false);
            addBlock(x, y, block);
            try_cnt++;
        }
    }
    
    else if(x > 1 && y > m_cntRow+1)
    {
        TmBlock* blockPrevPrevX = m_tblBlock[x-2 + y*m_cntCol];
        TmBlock* blockPrevX = m_tblBlock[x-1 + y*m_cntCol];
        TmBlock* blockPrevPrevY = m_tblBlock[x + (y-2)*m_cntCol];
        
        TmBlock* blockPrevY = m_tblBlock[x + (y-1)*m_cntCol];
        
        try_cnt = 0;
        while(((blockPrevPrevX->m_id == block->m_id && blockPrevX->m_id == block->m_id) || (blockPrevPrevY->m_id == block->m_id && blockPrevY->m_id == block->m_id)) && try_cnt < 5)
        {
            removeBlock(block, true);
            block = getBlock(false);
            addBlock(x, y, block);
            try_cnt++;
        }
    }
    
    return block;
}


void TmBoard::createBlocks()
{
    //
	//create new block
	for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = m_cntRow;y < m_cntRowBack;y++)
		{
 			TmBlock* block = m_tblBlock[x + y*m_cntCol];

			if(block == NULL)
			{
				block = getBlock(false);
                addBlock(x, y, block);
                block = arrangeBlocksNotMatch(block, x, y);
			}
		}
	}
}


void TmBoard::printBlockInfo()
{
    YtLib* lib = YtLib::sharedLib();
    
    extern bool g_isDebug;
    
    if(!g_isDebug)
        return;
    
    for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = 0;y < m_cntRow;y++)
		{
 			TmBlock* block = m_tblBlock[x + y*m_cntCol];
            
            std::string debug_info;
			if(block != NULL)
			{
//                switch(block->m_status)
//                {
//                        
//                    case TmBlockStatus_SwapCheck:
//                        debug_info = "SC";
//                        break;
//                    case TmBlockStatus_Normal:
//                        debug_info = "N";
//                        break;
//                    case TmBlockStatus_Select:
//                        debug_info = "S";
//                        break;
//                    case TmBlockStatus_Highlight:
//                        debug_info = "H";
//                        break;
//                    case TmBlockStatus_Falling:
//                        debug_info = "F";
//                        break;
//                    case TmBlockStatus_Died:
//                        debug_info = "D";
//                        break;
//                    case TmBlockStatus_Swap:
//                        debug_info = "SW";
//                        break;
//                    case TmBlockStatus_SwapReverse:
//                        debug_info = "SWR";
//                        break;
//                    case TmBlockStatus_Disappear:
//                        debug_info = "DI";
//                        break;
//                    case TmBlockStatus_EndAndDisappear:
//                        debug_info = "END";
//                        break;
//
//                }
//                
//                sprintf(lib->m_strTemp, "%s%d", debug_info.c_str(), block->m_blocktype);
                //sprintf(lib->m_strTemp, "%d/%d", block->m_cntMatchH, block->m_cntMatchV);
                sprintf(lib->m_strTemp, "%d,%d,%d", block->m_col, block->m_row, block->m_isGrilled);
                
                block->m_lblDebug->setString(lib->m_strTemp);
			}
		}
	}

}


void TmBoard::bomArroundReal(TmBlock* block)
{
	int col = block->m_col;
	int row = block->m_row;

	for(int x = col - 1;x <= col + 1;x++)
	{
		for(int y = row - 1;y <= row + 1;y++)
		{
			if(x >= 0 && x < this->m_cntCol && y >= 0 && y < this->m_cntRow)
			{
				TmBlock* block = m_tblBlock[x + y*m_cntCol];

                if(block && block->m_blocktype != TmBlockType_Fix)
                    addDestoryableBlock(block, TmBlockDieType_ByBomb, true);
			}
		}
	}
	bombEnable(true);
}




bool TmBoard_checkDestoryableBlock(TmBlock* block)
{
    if(block == NULL)
        return false;
    if(block->m_blocktype == TmBlockType_Ice)
        return false;
    if(block->m_blocktype == TmBlockType_Blank)
        return false;
    
    if(block->m_blocktype == TmBlockType_Fix)
        return false;

    return true;
}


bool TmBoard::checkDestoryableBlock(int startY, bool highlight, bool action)
{
	int count = 0;
	for(int y = startY;y < startY+m_cntRow;y++)
	{
		for(int x = 0;x < m_cntCol;x++)	
		{			
			TmBlock* blockCur = m_tblBlock[x + y*m_cntCol];
            
			//if((blockCur->m_status&TmBlockStatus_MaskLock) == TmBlockStatus_Locked)
			//	return;
            
            if(!TmBoard_checkDestoryableBlock(blockCur))
                continue;
            
			if(count == 0 && x >= 1)
			{				
				if(TmBoard_checkDestoryableBlock(m_tblBlock[x-1 + y*m_cntCol]))
                {
                    m_tblBlock[x + y*m_cntCol] = m_tblBlock[x-1 + y*m_cntCol];
                    m_tblBlock[x-1 + y*m_cntCol] = blockCur;

                    traverse(startY);	
                    count += checkMatching(startY, blockCur, false);

                    m_tblBlock[x-1 + y*m_cntCol] = m_tblBlock[x + y*m_cntCol];
                    m_tblBlock[x + y*m_cntCol] = blockCur;
                }
			}
			if(count == 0 && x < m_cntCol-1)
			{
				if(TmBoard_checkDestoryableBlock(m_tblBlock[x+1 + y*m_cntCol]))
				{
                    m_tblBlock[x + y*m_cntCol] = m_tblBlock[x+1 + y*m_cntCol];
                    m_tblBlock[x+1 + y*m_cntCol] = blockCur;
                    traverse(startY);	
                    count += checkMatching(startY, blockCur, false);

                    m_tblBlock[x+1 + y*m_cntCol] = m_tblBlock[x + y*m_cntCol];
                    m_tblBlock[x + y*m_cntCol] = blockCur;
                }
			}
			if(count == 0 && y >= startY+1)
			{
				if(TmBoard_checkDestoryableBlock(m_tblBlock[x + (y-1)*m_cntCol]))
				{
                    m_tblBlock[x + y*m_cntCol] = m_tblBlock[x + (y-1)*m_cntCol];
                    m_tblBlock[x + (y-1)*m_cntCol] = blockCur;
                    traverse(startY);	
                    count += checkMatching(startY, blockCur, false);
                    m_tblBlock[x + (y-1)*m_cntCol] = m_tblBlock[x + y*m_cntCol];
                    m_tblBlock[x + y*m_cntCol] = blockCur;
                }
			}
			if(count == 0 && y < startY+m_cntRow-1)
			{
				if(TmBoard_checkDestoryableBlock(m_tblBlock[x + (y+1)*m_cntCol]))
				{
                    m_tblBlock[x + y*m_cntCol] = m_tblBlock[x + (y+1)*m_cntCol];
                    m_tblBlock[x + (y+1)*m_cntCol] = blockCur;
                    traverse(startY);
                    count += checkMatching(startY, blockCur, false);
                    m_tblBlock[x + (y+1)*m_cntCol] = m_tblBlock[x + y*m_cntCol];
                    m_tblBlock[x + y*m_cntCol] = blockCur;
                }
			}

			if(count > 0)
			{	
				if(highlight && action)
				{	
					m_blockHighlight = blockCur;
					m_blockHighlight->setAction(blockCur->m_posCur.x, blockCur->m_posCur.y, TmBlockActionCmd_Highlight);
					commandResult(TmCommandType_Hint, true);
					break;
				}
			}
			else
				count = 0;
		}
		if(count > 0)
			break;
	}

	if(count == 0)
	{
		//
		// auto refresh
        if(action)
        {
            startNoMoreMoveEvent();
            bombUnReady();
        }
        
        return false;
    }
    return true;
}

//LARGE_INTEGER liCounter1, liCounter2, liFrequency;
//QueryPerformanceFrequency(&liFrequency);
//
//QueryPerformanceCounter(&liCounter1);
//m_commandManager->update(dt);//0.4
//QueryPerformanceCounter(&liCounter2);
////CCLog("step1 %fs\n", (double)(liCounter2.QuadPart - liCounter1.QuadPart) / (double)liFrequency.QuadPart);
//static double step1 = 0;
//step1 += (double)(liCounter2.QuadPart - liCounter1.QuadPart) / (double)liFrequency.QuadPart;

void TmBoard::update(float dt)
{
	if(m_isPause)
		return;

    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    
    if(tutorialManager->m_pauseBoard)
    {
        return;
    }
    
    m_commandManager->update(dt);
	traverse(0);
	checkMatching(0, NULL, true);
	checkDestory();
	checkDie();
	fallBlocks();
	updateBlocks(dt);
    
    DyBlockManager::sharedBlockManager()->arrangeBlocks();
    
    createBlocks();
    
    
    m_isAllBlockMovable = isAllBlockUserMovable();
    
    if(m_blockHighlight == NULL)
    {
        if(m_isAllBlockMovable)
        {
            checkDestoryableBlock(0, false, true);//0.002
        }
    }
    
}


bool TmBoard::lowerblockmove(TmBlock* block)
{
	for(int i = block->m_row-1;i >= 0;i--)
	{
		TmBlock* _block = m_tblBlock[block->m_col + i*m_cntCol];
        if(_block == NULL)
            return true;
        
        if(_block->m_blocktype == TmBlockType_Blank)
            return false;
        if(_block->m_blocktype == TmBlockType_Fix)
            return false;

        
		if((_block->m_status&TmBlockStatus_MaskIdleMove) == TmBlockStatus_Move)
		{
			return true;
		}
	}
	return false;
}


void TmBoard::swapBlock(TmBlock* blockOne, TmBlock* blockTwo, bool reverse)
{
	if(reverse)
	{
		blockOne->m_blockSwap = NULL;
		blockTwo->m_blockSwap = NULL;
	}
	else
	{
		if((blockOne->m_status&TmBlockStatus_MaskLock) == TmBlockStatus_Locked)
			return;
        if(blockOne->m_blocktype == TmBlockType_Blank)
			return;
        if(blockOne->m_blocktype == TmBlockType_Fix)
			return;
		if(lowerblockmove(blockOne))
			return;

        
        if((blockTwo->m_status&TmBlockStatus_MaskLock) == TmBlockStatus_Locked)
			return;
        if(blockTwo->m_blocktype == TmBlockType_Blank)
			return;
        if(blockTwo->m_blocktype == TmBlockType_Fix)
			return;
        if(lowerblockmove(blockTwo))
			return;

		blockOne->m_blockSwap = blockTwo;
		blockTwo->m_blockSwap = blockOne;
	}

	//YtLib* lib = YtLib::sharedLib();

	m_tblBlock[blockOne->m_col + blockOne->m_row*m_cntCol] = blockTwo;
	m_tblBlock[blockTwo->m_col + blockTwo->m_row*m_cntCol] = blockOne;

	int one_row = blockOne->m_row;
	int one_col = blockOne->m_col;

	blockOne->setAction(blockTwo->m_col,  blockTwo->m_row, reverse ? TmBlockActionCmd_SwapReverse : TmBlockActionCmd_Swap);
	blockTwo->setAction(one_col, one_row, reverse ? TmBlockActionCmd_SwapReverse : TmBlockActionCmd_Swap);

	if(reverse)
		YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_MOVEBACK);
	else
		YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_MOVE);
}


//
// direction	0; RIGHT
//				1; UP
//				2; LEFT
//				3; DOWN
void TmBoard::trySwapBlockByDirection(TmBlock* blockOne, int direction)
{
	TmBlock* blockTwo;

	if(blockOne->m_blocktype == TmBlockType_Ice)
		return;
	if(blockOne->m_blocktype == TmBlockType_Fix)
		return;
	if(blockOne->m_blocktype == TmBlockType_Blank)
		return;


	switch(direction)
	{
		case 0:
			if(blockOne->m_col+1 >= m_cntCol)
				return;

			blockTwo = m_tblBlock[(blockOne->m_col+1) + blockOne->m_row*m_cntCol];
			break;
		case 1:

			if(blockOne->m_row+1 >= m_cntRow)
				return;

			blockTwo = m_tblBlock[blockOne->m_col + (blockOne->m_row+1)*m_cntCol];

			break;
		case 2:
			if(blockOne->m_col-1 < 0)
				return;

			blockTwo = m_tblBlock[blockOne->m_col-1 + (blockOne->m_row)*m_cntCol];
			break;
		case 3:

			if(blockOne->m_row-1 < 0)
				return;

			blockTwo = m_tblBlock[blockOne->m_col + (blockOne->m_row-1)*m_cntCol];
			break;
	}

	if(blockTwo == NULL)
		return;

	if(blockTwo->m_blocktype == TmBlockType_Ice)
		return;
	if(blockTwo->m_blocktype == TmBlockType_Fix)
		return;

	if(blockTwo->m_blocktype == TmBlockType_Blank)
		return;


	TmCommand* command = new TmCommand(TmCommandType_SwapBlock, this, blockOne, blockTwo, blockOne->m_col<<16|blockOne->m_row, blockTwo->m_col<<16|blockTwo->m_row);
	m_commandManager->addLast(command);
	command->release();
}


void TmBoard::trySwapBlock(TmBlock* blockOne, TmBlock* blockTwo, CCPoint actionPoint)
{
	if(blockTwo)
	{
		float distance = ccpDistance(blockTwo->m_posCur, blockOne->m_posCur);
		if(distance > m_distanceBlock)
			return;
	}

	CCPoint delta = ccpSub(actionPoint, m_blockSelected->m_posCur);
		
	float angle = CC_RADIANS_TO_DEGREES(ccpToAngle(delta));
			
	if(angle > -45 && angle <= 45)
		trySwapBlockByDirection(blockOne, 0);
	else if(angle > 45 && angle <= 135)
		trySwapBlockByDirection(blockOne, 1);
	else if((angle > 135 && angle <= 180) || (angle < -135))
		trySwapBlockByDirection(blockOne, 2);
	else
		trySwapBlockByDirection(blockOne, 3);
}


TmBlock* TmBoard::getBlockByPoistion(CCPoint position)
{
	if(position.x < 0 || position.x >= m_layerMain->m_size.width)
		return NULL;
	else if(position.y < 0 || position.y >= m_layerMain->m_size.height)
		return NULL;

	int col = position.x/m_widthBlock;
	int row = position.y/m_heightBlock;

	if(row >= m_cntRow)
		return NULL;

	if(col >= m_cntCol)
		return NULL;

	return m_tblBlock[col + row*m_cntCol];
}


void TmBoard::ccTouchesBegan(CCTouch* touch)
{
	if(m_isPause)
		return;

	CCPoint touchLocation = touch->getLocation();
	CCPoint nodePosition = m_layerMain->convertToNodeSpace(touchLocation);

	TmBlock* block = getBlockByPoistion(nodePosition);

	if(block == NULL || (block->m_status&TmBlockStatus_MaskLock) == TmBlockStatus_Locked)
		return;
    
    if(block->m_blocktype == TmBlockType_Blank)
        return;
    
	if(block->m_blocktype == TmBlockType_Fix)
        return;
    
	if(m_isBombSelectMode)
	{
		TmCommand* command = new TmCommand(TmCommandType_Bomb, this, block, NULL, 0, 0);
		m_commandManager->addLast(command);
		command->release();
		bombUnReady();
		return;
	}
	//m_dtCheckStatus = 0.f;

    if(m_isStartNoMoveEvent)
        return;
    
	if(m_blockHighlight)
	{
		m_blockHighlight->setAction(m_blockHighlight->m_posCur.x, m_blockHighlight->m_posCur.y, TmBlockActionCmd_Normal);
		m_blockHighlight = NULL;
	}

	if(m_blockSelected)
	{
		if(m_blockSelected != block)
		{
			float distance = ccpDistance(block->m_posCur, m_blockSelected->m_posCur);
			if(distance <= m_distanceBlock)
			{
				trySwapBlock(m_blockSelected, block, nodePosition);
				m_blockSelected->setAction(m_blockSelected->m_posCur.x, m_blockSelected->m_posCur.y, TmBlockActionCmd_Unselect);
				m_sprCursor->setVisible(false);
				m_blockSelected = NULL;
			}
			else
			{
				m_blockSelected->setAction(m_blockSelected->m_posCur.x, m_blockSelected->m_posCur.y, TmBlockActionCmd_Unselect);
				m_blockSelected = block;
				m_blockSelected->setAction(m_blockSelected->m_posCur.x, m_blockSelected->m_posCur.y, TmBlockActionCmd_Select);
			}
		}
		else
		{
			if(m_blockSelected->m_blocktype == TmBlockType_Candy)
			{
				selectSpecialBlock(m_blockSelected);
			}
			else if(m_blockSelected->m_blocktype == TmBlockType_MSG)
			{
				selectSpecialBlock(m_blockSelected);
			}
		}
	}
	else
	{
		m_blockSelected = block;
		m_blockSelected->setAction(m_blockSelected->m_posCur.x, m_blockSelected->m_posCur.y, TmBlockActionCmd_Select);
	}
}


void TmBoard::ccTouchesEnded(CCTouch* touch)
{
	if(m_isPause)
		return;

	CCPoint touchLocation = touch->getLocation();
	CCPoint nodePosition = m_layerMain->convertToNodeSpace(touchLocation);

	TmBlock* block = getBlockByPoistion(nodePosition);
	
	if(block == NULL)
		return;

	if(m_blockSelected && m_blockSelected != block)
	{
        trySwapBlock(m_blockSelected, NULL, nodePosition);
		m_blockSelected->setAction(m_blockSelected->m_posCur.x, m_blockSelected->m_posCur.y, TmBlockActionCmd_Unselect);
		m_sprCursor->setVisible(false);
		m_blockSelected = NULL;	
	}
}


void TmBoard::ccTouchesMoved(CCTouch* touch)
{
	if(m_isPause)
		return;

	if(m_blockSelected == NULL)
		return;
    
	CCPoint touchLocation = touch->getLocation();

	CCPoint nodePosition = m_layerMain->convertToNodeSpace(touchLocation);
    
	//if(nodePosition.x < 0 || nodePosition.y < 0)
	//	return;

	TmBlock* block = getBlockByPoistion(nodePosition);
	do
	{
		if(block == m_blockSelected)
			break;

		trySwapBlock(m_blockSelected, NULL, nodePosition);
		m_blockSelected->setAction(m_blockSelected->m_posCur.x, m_blockSelected->m_posCur.y, TmBlockActionCmd_Unselect);
		m_sprCursor->setVisible(false);
		m_blockSelected = NULL;

	}while(0);
}


void TmBoard::pause()
{
	m_isPause = true;
	for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = 0;y < m_cntRowBack;y++)
		{
			TmBlock* block = m_tblBlock[x + y*m_cntCol];
			if(block)
			{
				block->pause();
			}
		}
	}
}


void TmBoard::resume()
{
	m_isPause = false;
	for(int x = 0;x < m_cntCol;x++)
	{
		for(int y = 0;y < m_cntRowBack;y++)
		{
			TmBlock* block = m_tblBlock[x + y*m_cntCol];
			if(block)
			{
				block->resume();
			}
		}
	}
}
