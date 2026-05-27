/*
DySceneGame.cpp
Doggy chef
@initialize at 120907

@history
120907	yoonsr	initialize
*/

#include "DySceneGame.h"
#include "DyLib.h"
//#include "DyGame.h"

#include "DyLoader.h"
#include "YtSpriteEx.h"

#include "CCextAnimatePacker.h"

#include "DyStage.h"
#include "DyRecipe.h"
#include "DyBlockInfo.h"
#include "DyTycoon.h"
#include "DyItem.h"

#include "DyTycoonCommand.h"
#include "DyMoney.h"
#include "DyLayerGameStart.h"
#include "DyLayerShop.h"

#include "DyLayerOption.h"
#include "DyCostume.h"
#include "DyPopup.h"
#include "DyTutorial.h"
#include "DyRank.h"
#include "DyNetwork.h"
#include "DyLayerGameStartItem.h"
#include "DyLayerGameStartMachine.h"
#include "DyMachine.h"
#include "DyCustomer.h"

DyLayerGame* g_layerGame;


#define GAME_UI_TAG_ITEM_BUTTON   10
#define GAME_UI_TAG_ITEM_REMOVE   11

void DyLayerGame::setBlocks()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyRecipeManager* recipeManager = DyRecipeManager::sharedRecipeManager();
	DyBlockManager* blockManager = DyBlockManager::sharedBlockManager();

	recipeManager->loadRecipeResource(stageManager->m_curShopType);

	int id_shop = atoi(stageManager->m_shopinfo->getValue("id", stageManager->m_curShopType));

	recipeManager->reorderingAvailable(id_shop);
    
    for(unsigned int i = 0;i < blockManager->m_arrBlockInfo->count();i++)
    {
        DyBlockInfo* blockInfo = (DyBlockInfo*)blockManager->m_arrBlockInfo->objectAtIndex(i);
        blockInfo->m_isBlockAvailable = false;
    }
    
    //
    // 가능 블럭 셋팅
	for(unsigned int i = 0;i < recipeManager->m_arrAvailableRecipes->count();i++)
	{
		DyRecipeInfo *recipe = (DyRecipeInfo *)recipeManager->m_arrAvailableRecipes->objectAtIndex(i);
        for(unsigned j = 0;j < recipe->m_arrMaterial->count();j++)
        {
            DyMaterial* material = (DyMaterial*)recipe->m_arrMaterial->objectAtIndex(j);
            
            blockManager->setBlockAvailable(material->m_tag.c_str());
        }
    }
    
    //
    // 쓰레기 블럭 추가
    CCArray* dummy_blocks = stageManager->m_curShopInfo->getArray("dummy_blocks", stageManager->m_curLevel);
	for(unsigned int i = 0;i < dummy_blocks->count();i++)
	{
		CCString* dummy_block = (CCString*)dummy_blocks->objectAtIndex(i);
        
        if(strcmp(dummy_block->getCString(), "danmooji") == 0)
        {
            //DyBlockInfo* blockInfo = blockManager->getBlockInfoByTag("danmooji");
            
            blockManager->m_dummyblock_weight = atof(((CCString*)dummy_blocks->objectAtIndex(i+1))->getCString());
            
            blockManager->setBlockAvailable(dummy_block->getCString());
            
        }
	}
    
    blockManager->changeStatus();
	blockManager->arrangeBlocks();
    
    tycoonManager->m_recipe_addable_cnt = recipeManager->getRecipeTotalCnt(stageManager->m_curShopType)-recipeManager->getDefaultCnt(stageManager->m_curShopType);
    tycoonManager->m_recipe_add_cnt = (recipeManager->getRecipeHasCnt(stageManager->m_curShopType)-recipeManager->getDefaultCnt(stageManager->m_curShopType));
    tycoonManager->m_crazy_time_add = CRAZY_TIME_ADD*(1.0f + 0.2*tycoonManager->m_recipe_add_cnt);
}


void DyLayerGame::setCharacters(bool isCrazy)
{
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyCharacterManager* characterManager = DyCharacterManager::sharedCharacterManager();

	characterManager->allInvailable();

    if(isCrazy)
    {
        DyDataManager* dataManager = DyDataManager::sharedDataManager();
        
        CCArray* animals = stageManager->m_shopinfo->getArray("animals", stageManager->m_curShopType);
        
        for(unsigned int i = 0;i < animals->count();i++)
        {
            CCString* animal = (CCString*)animals->objectAtIndex(i);
            
            DyCharacterInfo* infoAnimal = characterManager->getCharacterInfo(animal->getCString());
            
            if(dataManager->awards_get_complete(4, infoAnimal->m_id))
                characterManager->setAvailable(animal->getCString(), true);
        }
        
        CCArray* animals_default_crazy = stageManager->m_shopinfo->getArray("animals_default_crazy", stageManager->m_curShopType);
        for(unsigned int i = 0;i < animals_default_crazy->count();i++)
        {
            CCString* animal = (CCString*)animals_default_crazy->objectAtIndex(i);
            
            DyCharacterInfo* infoAnimal = characterManager->getCharacterInfo(animal->getCString());
            
            characterManager->setAvailable(infoAnimal->m_tag.c_str(), true);
            characterManager->setDangol(infoAnimal->m_id);
        }
        dataManager->dataSave();
    }
    else
    {
        CCArray* animals = stageManager->m_curShopInfo->getArray("animal", stageManager->m_curLevel);
        
        for(unsigned int i = 0;i < animals->count();i++)
        {
            CCString* animal = (CCString*)animals->objectAtIndex(i);
            
            characterManager->setAvailable(animal->getCString(), true);
        }
    }
}


void DyLayerGame::loadQuest()
{
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
 	CCArray* quests = stageManager->m_curShopInfo->getArray("quest", stageManager->m_curLevel);
    
	if(tycoonManager->m_quest)
    {
		tycoonManager->m_quest->release();
        tycoonManager->m_quest = NULL;
    }
    
	//
	// quest load
	tycoonManager->m_quest = new DyQuest();
	tycoonManager->m_quest->m_questMainTag = ((CCString*)quests->objectAtIndex(0))->getCString();
	tycoonManager->m_quest->m_questSubTag = ((CCString*)quests->objectAtIndex(1))->getCString();
	int quest_cnt =  ((CCString*)quests->objectAtIndex(2))->intValue();
  
    
    tycoonManager->m_quest->m_questNotStaff = !atoi(stageManager->m_curShopInfo->getValue("staff", stageManager->m_curLevel));
    
    
//    if(tycoonManager->m_quest->m_questSubTag.compare("GOLD") == 0 || tycoonManager->m_quest->m_questSubTag.compare("SIDESELF") == 0)
//        tycoonManager->m_quest->m_questNotStaff = true;
//    if(quest_cnt < 0)
//    {
//        quest_cnt = abs(quest_cnt);
//        tycoonManager->m_quest->m_questNotStaff = true;
//    }
	tycoonManager->m_quest->m_countQuest = quest_cnt;
    
	if(quest_cnt == 0)
	{
        if(tycoonManager->m_quest->m_questMainTag.compare("NULL") == 0)
            tycoonManager->m_quest->m_questtype = DyQuestType_Ignore;
        else
            tycoonManager->m_quest->m_questtype = DyQuestType_All;
		tycoonManager->m_quest->m_countQuest = 10000;
	}
	else
	{
		tycoonManager->m_quest->m_questtype = DyQuestType_Number;
		tycoonManager->m_quest->m_countQuest = quest_cnt;
		tycoonManager->m_quest->m_countComplete = quest_cnt;
	}
    
    tycoonManager->m_quest->m_countComplete = 0;
    
    
    if(tycoonManager->m_quest->m_questMainTag.compare(QUESTTAG_REMAIN) == 0)
    {
        tycoonManager->m_quest->m_questtype = DyQuestType_NumberLimit;
		tycoonManager->m_quest->m_countQuest = quest_cnt;
        tycoonManager->m_quest->m_countComplete = quest_cnt;
    }
    
	tycoonManager->m_quest->m_queststatus = DyQuestStatus_Progress;
	 
}


DyLayerGame::DyLayerGame()
{
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    tycoonManager->m_lockInput = false;
    
    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    tutorialManager->clearTutorial();
    
    if(!tycoonManager->m_isCrazyMode)
    {
        if(stageManager->m_curShopType == DyStageType_Snack)
        {
            if(stageManager->m_curLevel == 0)
                tutorialManager->buildTutorial(0);
            else if(stageManager->m_curLevel == 1)
                tutorialManager->buildTutorial(1);
            else if(stageManager->m_curLevel == 2)
            {
                if(stageManager->getStageState(0, 2) <= DyStageState_Fail)
                {
                    tutorialManager->buildTutorial(2);
                }
            }
            //
            // 슬롯아이템 퀘스트
            else if(stageManager->m_curLevel == 3)
            {
                if(stageManager->getStageState(0, 3) <= DyStageState_Fail)
                {
                    tutorialManager->buildTutorial(3);
                }
            }
        }
        else if(stageManager->m_curShopType == DyStageType_China)
        {
            if(!dataManager->m_local.m_isViewDeliveryTutorial && stageManager->m_curLevel == 4)
            {
                tutorialManager->buildTutorial(6);
            }
        }
        else if(stageManager->m_curShopType == DyStageType_Korean)
        {
            if(!dataManager->m_local.m_isViewGrilTutorial && stageManager->m_curLevel == 12)
            {
                tutorialManager->buildTutorial(8);
            }
        }
    }

	DyLib *lib = DyLib::sharedLib();
	initWithColor(ccc4(0, 0, 0, 255), lib->m_sizeWin.width, lib->m_sizeWin.height);
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("threematch_ui.plist");
	CCextAnimatePacker::getInstance()->loadAnimations("threematch_ui_effect.xml");

    
    
    bool isHard = strcmp(stageManager->m_shopinfo->getValue("board_type", stageManager->m_curShopType), "hard") == 0;
    
    m_layerGameBoard = new DyLayerGameBoard(isHard);
	addChild(m_layerGameBoard, ZORDER_GAME_BASE_LEVEL2);

	m_layerGameBoard->setPosition(395, 18);

    tycoonManager->m_crazy_point_time = 0;
    tycoonManager->m_crazy_point_removeblock = 0;
    tycoonManager->m_crazy_point_gold = 0;
    tycoonManager->m_crazy_point_combo = 0;
    tycoonManager->m_crazy_point = 0;

    tycoonManager->m_crazy_createGold = 0;
    tycoonManager->m_crazy_createDelly = 0;
    tycoonManager->m_crazy_createHeart = 0;
    
    tycoonManager->m_crazy_bonusGold = 0;
    tycoonManager->m_crazy_bonusDelly = 0;
    
    tycoonManager->m_dtCrazyOrder = 0;
    
    tycoonManager->m_stage_point = 0;
    
    tycoonManager->m_bouns_heart_point = 0;
    tycoonManager->m_combo_point = 0;
    
	tycoonManager->m_current_gold = 0;
	tycoonManager->m_sold_gold = 0;
	tycoonManager->m_tip_gold = 0;
	tycoonManager->m_cashier_gold = 0;
    tycoonManager->m_bonustime_gold = 0;
	tycoonManager->m_questProcessComplete = false;
    tycoonManager->m_heartComplete = false;
    tycoonManager->m_isStartWarning = false;
    
    
    DyRecipeManager::sharedRecipeManager()->m_removeCakeType = -1;
    setBlocks();
    
    if(tycoonManager->m_isCrazyMode)
    {
        tycoonManager->m_crazy_status = 0;
        
        m_layerGameBoard->m_board->m_ice_probability = atoi(lib->m_crazy->getValue("ice_probability", tycoonManager->m_crazy_status));
        m_layerGameBoard->m_board->m_decay_idle_duration = atoi(lib->m_crazy->getValue("decay_idle_duration", tycoonManager->m_crazy_status));
        m_layerGameBoard->m_board->m_decay_duration = atoi(lib->m_crazy->getValue("decay_duration", tycoonManager->m_crazy_status));
        customerManager->m_customer_term = atoi(lib->m_crazy->getValue("customer_term", tycoonManager->m_crazy_status));
        tycoonManager->m_crazy_time = atoi(lib->m_crazy->getValue("time", tycoonManager->m_crazy_status));
        tycoonManager->m_crazyHeartVel = atoi(lib->m_crazy->getValue("heart_vel", tycoonManager->m_crazy_status));
        
        tycoonManager->m_bgmIdx = (YtLibSndBGM)atoi(lib->m_parameters->getValue("value_integer", DyLibParam_Crazy_bgm_id));
        tycoonManager->m_goal_gold = 0;
        tycoonManager->m_perfect_gold = 0;
        tycoonManager->m_questbonus_gold = 0;
        
        //
        // 보너스 하트
        tycoonManager->m_bouns_heart = 0;
        tycoonManager->m_bonus_heart_cnt = 0;
        tycoonManager->m_current_heart = 0;
        
        tycoonManager->m_time_limit = 0;
        tycoonManager->m_dtOpenTime = tycoonManager->m_time_limit;
        
        tycoonManager->m_max_block_count = 9;
        tycoonManager->m_block_weight =  atof(stageManager->m_shopinfo->getValue("block_weight", stageManager->m_curShopType));
        
        
        tycoonManager->m_quest = NULL;
        
        tycoonManager->m_double_probability = atoi(stageManager->m_shopinfo->getValue("double_probability", stageManager->m_curShopType));
     
        for(int i = 0;i < 3;i++)
        {
            sprintf(lib->m_strTemp, "association_%d", i+1);
            tycoonManager->m_association[i] = atoi(stageManager->m_shopinfo->getValue(lib->m_strTemp, stageManager->m_curShopType));
        }
        
        tycoonManager->m_trouble_table_cnt = 0;
        tycoonManager->m_heart_minus = 0;
        tycoonManager->m_fix_block_cnt = 0;
    }
    else
    {
        int curLevel = stageManager->m_curLevel;
        
        
        m_layerGameBoard->m_board->m_ice_probability = atoi(stageManager->m_curShopInfo->getValue("ice_probability", curLevel));
        m_layerGameBoard->m_board->m_decay_idle_duration = atoi(stageManager->m_curShopInfo->getValue("decay_idle_duration", curLevel));
        m_layerGameBoard->m_board->m_decay_duration = atoi(stageManager->m_curShopInfo->getValue("decay_duration", curLevel));
        
        tycoonManager->m_bgmIdx = (YtLibSndBGM)atoi(stageManager->m_shopinfo->getValue("bgm_id", stageManager->m_curShopType));
        
        customerManager->m_customer_term = atoi(stageManager->m_curShopInfo->getValue("customer_term", curLevel));
        tycoonManager->m_goal_gold = atoi(stageManager->m_curShopInfo->getValue("goal_gold", curLevel));
        tycoonManager->m_perfect_gold = atoi(stageManager->m_curShopInfo->getValue("perfect_gold", curLevel));
        tycoonManager->m_questbonus_gold = atoi(stageManager->m_curShopInfo->getValue("quest_bonus", curLevel));
        
        loadQuest();
        //
        // 보너스 하트(나중에 제거하자)
        tycoonManager->m_bouns_heart = 100;//atoi(stageManager->m_curShopInfo->getValue("heart_bonus", curLevel));
        tycoonManager->m_bonus_heart_cnt = 10;//atoi(stageManager->m_curShopInfo->getValue("bonus_heart_cnt", curLevel));
        tycoonManager->m_current_heart = 0;
        
        
        
        tycoonManager->m_time_limit = atoi(stageManager->m_curShopInfo->getValue("time_limit", curLevel));
        tycoonManager->m_dtOpenTime = tycoonManager->m_time_limit;
        
        tycoonManager->m_max_block_count = atoi(stageManager->m_curShopInfo->getValue("max_block_count", curLevel));
        
        tycoonManager->m_block_weight = atof(stageManager->m_curShopInfo->getValue("block_weight", curLevel));
        
        tycoonManager->m_double_probability = atoi(stageManager->m_curShopInfo->getValue("double_probability", curLevel));
        
        for(int i = 0;i < 3;i++)
        {
            sprintf(lib->m_strTemp, "association_%d", i+1);
            tycoonManager->m_association[i] = atoi(stageManager->m_curShopInfo->getValue(lib->m_strTemp, curLevel));
        }
        
        tycoonManager->m_trouble_table_cnt = atoi(stageManager->m_curShopInfo->getValue("trouble_table_cnt", curLevel));
        
        if(tycoonManager->m_trouble_table_cnt > 0)
            tycoonManager->m_trouble_table_idx[0] = rand()%3;
        
        if(tycoonManager->m_trouble_table_cnt > 1)
        {
            tycoonManager->m_trouble_table_idx[1] = tycoonManager->m_trouble_table_idx[0];
            while(tycoonManager->m_trouble_table_idx[0] == tycoonManager->m_trouble_table_idx[1])
            {
                tycoonManager->m_trouble_table_idx[1] = rand()%3;
            }
        }
        
        tycoonManager->m_heart_minus = atoi(stageManager->m_curShopInfo->getValue("heart_minus", curLevel));
        tycoonManager->m_fix_block_cnt = atoi(stageManager->m_curShopInfo->getValue("fix_block_cnt", curLevel));
        

    }
    
	setCharacters(tycoonManager->m_isCrazyMode);
    
    m_layerGameBoard->m_board->initBlocks();
    
    m_layerGameBoard->m_board->m_hasBlank = false;
    
    m_layerGameBoard->m_board->clearGill();
    
    if(!tycoonManager->m_isCrazyMode)
    {
        const char *map = stageManager->m_curShopInfo->getValue("map", stageManager->m_curLevel);
        
        if(strcmp(map, "NULL") != 0)
        {
            YtObjectGroup* mapInfo = DyLoader::sharedLoader()->loadObjects(map);
            
            for(int row = m_layerGameBoard->m_board->m_cntRow-1;row >= 0;row--)
            {
                for(int col = 0;col < m_layerGameBoard->m_board->m_cntCol;col++)
                {
                    sprintf(lib->m_strTemp, "col_%d", col);
                    
                    const char* tag = mapInfo->getValue(lib->m_strTemp, row);
                    
                    if(strcmp(tag, "B") == 0)
                    {
                        m_layerGameBoard->m_board->setGrill(col, m_layerGameBoard->m_board->m_cntRow-row-1);
                    }
                    else
                    {
                        int blockType = atoi(tag);
                    
                        if(blockType == 0)
                        {
                            TmBlock* block = m_layerGameBoard->m_board->getBlock(true);
                            m_layerGameBoard->m_board->addBlock(col, m_layerGameBoard->m_board->m_cntRow-row-1, block);
                            block->setBlockType(TmBlockType_Blank);
                            m_layerGameBoard->m_board->m_layerMain->reorderChild(block->m_layerBlock, 0);
                        
                            m_layerGameBoard->m_board->m_hasBlank = true;
                        }
                    }
                }
            }
        }
    }
    
	
    m_layerGameBoard->m_board->createInitBlocks();
    
	tycoonManager->setLayerAndBoard(this, (DyBoard*)m_layerGameBoard->m_board);
    
   
    for(int i = 0;i < tycoonManager->m_fix_block_cnt;i++)
    {
        TmBlock* blockFound = NULL;
//        int count = 0;
        while(blockFound == NULL)
        {
            int row = rand()%m_layerGameBoard->m_board->m_cntRow;
            int col = rand()%m_layerGameBoard->m_board->m_cntCol;
            
            TmBlock* block = m_layerGameBoard->m_board->m_tblBlock[col + row*m_layerGameBoard->m_board->m_cntCol];
            
            int countBlank = 0;
            if(row == 0 || row == m_layerGameBoard->m_board->m_cntRow-1)
                countBlank++;
            
            if(row > 0)
            {
                TmBlock* blockCheck = m_layerGameBoard->m_board->m_tblBlock[col + (row-1)*m_layerGameBoard->m_board->m_cntCol];
                if(blockCheck && (blockCheck->m_blocktype == TmBlockType_Blank || blockCheck->m_blocktype == TmBlockType_Fix))
                    countBlank++;
            }

            if(row < m_layerGameBoard->m_board->m_cntRow-1)
            {
                TmBlock* blockCheck = m_layerGameBoard->m_board->m_tblBlock[col + (row+1)*m_layerGameBoard->m_board->m_cntCol];
                if(blockCheck && (blockCheck->m_blocktype == TmBlockType_Blank || blockCheck->m_blocktype == TmBlockType_Fix))
                    countBlank++;
            }
            
            
            if(col == 0 || col == m_layerGameBoard->m_board->m_cntCol-1)
                countBlank++;
            
            if(col > 0)
            {
                TmBlock* blockCheck = m_layerGameBoard->m_board->m_tblBlock[col-1 + (row)*m_layerGameBoard->m_board->m_cntCol];
                if(blockCheck && (blockCheck->m_blocktype == TmBlockType_Blank || blockCheck->m_blocktype == TmBlockType_Fix))
                    countBlank++;
            }
            
            if(col < m_layerGameBoard->m_board->m_cntCol-1)
            {
                TmBlock* blockCheck = m_layerGameBoard->m_board->m_tblBlock[col+1 + (row)*m_layerGameBoard->m_board->m_cntCol];
                if(blockCheck && (blockCheck->m_blocktype == TmBlockType_Blank || blockCheck->m_blocktype == TmBlockType_Fix))
                    countBlank++;
            }
            
            
            if(countBlank < 2 && block == NULL)
            {
                blockFound = m_layerGameBoard->m_board->getBlock(true);
                m_layerGameBoard->m_board->addBlock(col, row, blockFound);
                blockFound->setBlockType(TmBlockType_Fix);
                m_layerGameBoard->m_board->m_hasBlank = true;
            }
            
//            count++;
//            if(count > m_layerGameBoard->m_board->m_cntRow*m_layerGameBoard->m_board->m_cntCol)
//                break;
        }
    }
    
//    
//    int __tbl[8][8] =
//    {
//        {8, 2, 4, 4, 7, 3, 3, 5},
//        {7, 4, 8, 2, 6, 3, 6, 4},
//        {-1,8, 5, 6, 6, -1, 2, -1},
//        {2,-1,3, 4, 6, 3, -1, 8},
//        {-1,5,3,5,4,7,8,-1},
//        {4,2,0,9,4,5,4,3},
//        {2,2,4,6,5,0,8,6},
//        {2,3,8,9,5,3,6,7},
//    };
//    
//    
//    
//    int __tblType[8][8] =
//    {
//        {0,0,0,0,0,0,0,0},
//        {0,0,0,0,0,0,0,0},
//        {8,0,9,0,0,-1,0,8},
//        {0,8,0,0,9,9,8,0},
//        {8,0,0,0,0,0,0,8},
//        {0,0,0,0,0,0,1,0},
//        {9,0,0,0,0,0,0,9},
//        {0,0,1,0,0,0,0,0},
//    };
//    
//
//    for(int col = 0;col < 8;col++)
//        for(int row = 0;row < 8;row++)
//        {
//            if(__tblType[col][row] >= 0)
//            {
//                TmBlock *_block;
//                
//                if(__tbl[col][row] < 0)
//                    _block = m_layerGameBoard->m_board->getBlockByIndex(0);
//                else
//                    _block = m_layerGameBoard->m_board->getBlockByIndex(__tbl[col][row]);
//                m_layerGameBoard->m_board->addBlock(row, col, _block);
//                _block->setBlockType((TmBlockType)__tblType[col][row]);
//            }
//            
//        }
    
//    TmBlock *_block = m_layerGameBoard->m_board->getBlockByIndex(3);
//    m_layerGameBoard->m_board->addBlock(0, 0, _block);
//    _block->setBlockType(TmBlockType_Normal);
//    
//    _block = m_layerGameBoard->m_board->getBlockByIndex(3);
//    m_layerGameBoard->m_board->addBlock(0, 1, _block);
//    _block->setBlockType(TmBlockType_Fix);
//
//    
//    
//    _block = m_layerGameBoard->m_board->getBlockByIndex(0);
//    m_layerGameBoard->m_board->addBlock(1, 0, _block);
//    _block->setBlockType(TmBlockType_Normal);
//    
//    _block = m_layerGameBoard->m_board->getBlockByIndex(0);
//    m_layerGameBoard->m_board->addBlock(1, 1, _block);
//    _block->setBlockType(TmBlockType_Fix);
//    
//    
//    
//    
//    _block = m_layerGameBoard->m_board->getBlockByIndex(1);
//    m_layerGameBoard->m_board->addBlock(2, 0, _block);
//    _block->setBlockType(TmBlockType_Normal);
//    
//    _block = m_layerGameBoard->m_board->getBlockByIndex(1);
//    m_layerGameBoard->m_board->addBlock(2, 1, _block);
//    _block->setBlockType(TmBlockType_Normal);

    
//    _block = m_layerGameBoard->m_board->getBlockByIndex(0);
//    m_layerGameBoard->m_board->addBlock(2, 2, _block);
//    _block->setBlockType(TmBlockType_Normal);
//    
//    
//    
//    _block = m_layerGameBoard->m_board->getBlockByIndex(1);
//    m_layerGameBoard->m_board->addBlock(3, 0, _block);
//    _block->setBlockType(TmBlockType_Normal);
//
//    
//    _block = m_layerGameBoard->m_board->getBlockByIndex(0);
//    m_layerGameBoard->m_board->addBlock(3, 1, _block);
//    _block->setBlockType(TmBlockType_Normal);
//
	
	setKeypadEnabled(true);	
	setTouchEnabled(true);
		
	scheduleUpdate();

	m_isGamePause = false;
    g_layerGame = this;

	for(int i = 0;i < 3;i++)
	{
		m_sprBattery[i] = NULL;
	}

	m_gamestatus = DyGameStatus_Fail;
    m_pausedByGaest = false;
    
    //schedule(schedule_selector(DyLayerGame::displayGrill), 1.0f);
    
    
}



void DyLayerGame::displayGrill(float dt)
{
    unschedule(schedule_selector(DyLayerGame::displayGrill));
	//YtLib* lib = YtLib::sharedLib();
    m_layerGameBoard->m_board->displayGrill();
}




DyLayerGame::~DyLayerGame()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();

	DyItemManager* itemManager = DyItemManager::sharedItemManager();
    
    if(tycoonManager->m_aniCashier)
		YtAniObject::unloadAniObject(tycoonManager->m_aniCashier);
	tycoonManager->m_aniCashier = NULL;

    
    customerManager->m_arrCustomerOrder->removeAllObjects();

	itemManager->reset();
	g_layerGame = NULL;
	m_layerGameBoard->release();
}


void DyLayerGame::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
	CCSetIterator it;
	CCTouch* touch;

	for(it = touches->begin();it != touches->end();it++) 
	{
		touch = (CCTouch*)(*it);
		if(!touch)
			break;

		DyTycoonManager::sharedTycoonManager()->ccTouchesBegan(touch);
    
		return;
	}
}


void DyLayerGame::keyBackClicked()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	
    if(tycoonManager->m_menuPause->isEnabled())
    {
        onPauseClicked((CCObject *)1);
    }
	else if(tycoonManager->m_menuPauseMenu2->isEnabled())
    {
        onPlayClicked(NULL);
    }
}


void DyLayerGame::update(float dt)
{
	DyTycoonManager::sharedTycoonManager()->update(dt);
    DyNetworkManager::sharedNetworkManager()->update(dt);
}


void DyLayerGame_updateBattery(int level)
{
	if(g_layerGame && g_layerGame->m_sprBattery[0])
	{
		if(level > 90)
		{
			g_layerGame->m_sprBattery[0]->setColor(ccWHITE);
			g_layerGame->m_sprBattery[0]->setVisible(true);
			g_layerGame->m_sprBattery[1]->setVisible(true);
			g_layerGame->m_sprBattery[2]->setVisible(true);
		}
		else if(level > 50)
		{
			g_layerGame->m_sprBattery[0]->setColor(ccWHITE);
			g_layerGame->m_sprBattery[0]->setVisible(true);
			g_layerGame->m_sprBattery[1]->setVisible(true);
			g_layerGame->m_sprBattery[2]->setVisible(false);
		}
		else if(level > 20)
		{
			g_layerGame->m_sprBattery[0]->setColor(ccWHITE);
			g_layerGame->m_sprBattery[0]->setVisible(true);
			g_layerGame->m_sprBattery[1]->setVisible(false);
			g_layerGame->m_sprBattery[2]->setVisible(false);
		}
		else if(level > 5)
		{
			g_layerGame->m_sprBattery[0]->setColor(ccRED);
			g_layerGame->m_sprBattery[0]->setVisible(true);
			g_layerGame->m_sprBattery[1]->setVisible(false);
			g_layerGame->m_sprBattery[2]->setVisible(false);
		}
		else
		{
			g_layerGame->m_sprBattery[0]->setVisible(false);
			g_layerGame->m_sprBattery[1]->setVisible(false);
			g_layerGame->m_sprBattery[2]->setVisible(false);
		}
	}
}


void DyLayerGame::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
    DyItemManager* itemManager = DyItemManager::sharedItemManager();
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	//DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
   
    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    
	YtLib* lib = YtLib::sharedLib();

	CCArray* children = this->getChildren();

	for(unsigned int i = 0;i < children->count();i++)
	{
		CCNode* node = (CCNode*)children->objectAtIndex(i);
		if(node->getTag() >= 200 && node->getTag() != GAME_UI_TAG_GROUND && node->getTag() != GAME_UI_TAG_CASHIER && node->getTag() != GAME_UI_TAG_CASHIER_DESK)
		{
			reorderChild(node, ZORDER_GAME_BASE_LEVEL1);
		}
	}
    reorderChild(getChildByTag(GAME_UI_TAG_GROUND), ZORDER_GAME_BASE_LEVEL0);
    reorderChild(getChildByTag(GAME_UI_TAG_CASHIER), ZORDER_GAME_BASE_LEVEL0+1);
    reorderChild(getChildByTag(GAME_UI_TAG_CASHIER_DESK), ZORDER_GAME_BASE_LEVEL0+2);
    
    
	tycoonManager->m_status = DyTycoonStatus_Idle;
	tycoonManager->m_gameStarted = false;
	tycoonManager->m_gameStartForce = false;
    tycoonManager->m_gameSetting = false;
    
	tycoonManager->m_commandManager->removeAll();

    tycoonManager->destoryGameObject();

	tycoonManager->m_menuItem = (CCMenu*)getChildByTag(GAME_UI_TAG_MENU_ITEM);
	tycoonManager->m_menuPause = (CCMenu*)getChildByTag(GAME_UI_TAG_MENU_PAUSE);
	tycoonManager->m_menuPauseMenu = (CCMenu*)getChildByTag(GAME_UI_TAG_MENU_PAUSEMENU);
    tycoonManager->m_menuPauseMenu2 = (CCMenu*)getChildByTag(GAME_UI_TAG_MENU_PAUSEMENU2)->getChildByTag(0);

	tycoonManager->m_menuPause->setEnabled(true);
	setKeypadEnabled(true);
    
    tycoonManager->m_menuPauseMenu->setEnabled(false);
    tycoonManager->m_menuPauseMenu2->setEnabled(false);
    
	//
	// pause 
	CCMenuItemImage* menuItemPause = (CCMenuItemImage*)tycoonManager->m_menuPause->getChildByTag(GAME_UI_TAG_MENU_PAUSE+1);
	CCSprite* sprPause = CCSprite::createWithSpriteFrameName("game_ui_menu_pause.png");
	sprPause->setPosition(ccp(54, 20));
	menuItemPause->addChild(sprPause);

	CCSprite* sprLine = CCSprite::createWithSpriteFrameName("game_ui_menu_item_line.png");
	sprLine->setPosition(ccp(54, 50));
	menuItemPause->addChild(sprLine, -1);

	CCSprite* sprPauseSelBg = CCSprite::createWithSpriteFrameName("game_ui_menu_item.png");
	menuItemPause->setSelectedImage(sprPauseSelBg);

	CCSprite* sprPauseSel = CCSprite::createWithSpriteFrameName("game_ui_menu_pause_sel.png");
	sprPauseSel->setPosition(ccp(54, 20));
	sprPauseSelBg->addChild(sprPauseSel);

	CCSprite* sprLineSel = CCSprite::createWithSpriteFrameName("game_ui_menu_item_line.png");
	sprLineSel->setPosition(ccp(54, 50));
	sprPauseSelBg->addChild(sprLineSel, -1);

	static const char* tag_menuitem[4] =
	{
		"game_ui_menu_myroom",
		"game_ui_menu_shop",
		"game_ui_menu_home",
		"game_ui_menu_option"
	};
	
	//
	// pause menu
	for(int i = 0;i < 4;i++)
	{
		CCMenuItemImage* menuItem = (CCMenuItemImage*)tycoonManager->m_menuPauseMenu->getChildByTag(GAME_UI_TAG_MENU_PAUSEMENU+1+i);

		sprintf(lib->m_strTemp, "%s_text.png", tag_menuitem[i]);
		CCSprite* sprText = CCSprite::createWithSpriteFrameName(lib->m_strTemp);
		sprText->setPosition(ccp(54, 20));
		menuItem->addChild(sprText);

		sprintf(lib->m_strTemp, "%s_icon.png", tag_menuitem[i]);
		CCSprite* sprIcon = CCSprite::createWithSpriteFrameName(lib->m_strTemp);
		sprIcon->setPosition(ccp(54, 58));
		menuItem->addChild(sprIcon);

		CCSprite* sprLine = CCSprite::createWithSpriteFrameName("game_ui_menu_item_line.png");
		sprLine->setPosition(ccp(54, 60+5));
		menuItem->addChild(sprLine, -1);

		CCSprite* sprSelBg = CCSprite::createWithSpriteFrameName("game_ui_menu_item.png");
		menuItem->setSelectedImage(sprSelBg);

		sprintf(lib->m_strTemp, "%s_text.png", tag_menuitem[i]);
		CCSprite* sprTextSel = CCSprite::createWithSpriteFrameName(lib->m_strTemp);
		sprTextSel->setPosition(ccp(54, 20));
		sprSelBg->addChild(sprTextSel);

		sprintf(lib->m_strTemp, "%s_icon_sel.png", tag_menuitem[i]);
		CCSprite* sprIconSel = CCSprite::createWithSpriteFrameName(lib->m_strTemp);
		sprIconSel->setPosition(ccp(54, 52));
		sprSelBg->addChild(sprIconSel);

		CCSprite* sprLineSel = CCSprite::createWithSpriteFrameName("game_ui_menu_item_line.png");
		sprLineSel->setPosition(ccp(54, 60+5));
		sprSelBg->addChild(sprLineSel, -1);
	}
	
	//
	// battery
	CCMenuItemImage* menuItem = (CCMenuItemImage*)tycoonManager->m_menuPauseMenu->getChildByTag(GAME_UI_TAG_MENU_PAUSEMENU+5);
	
	CCSprite* batteryLine = CCSprite::createWithSpriteFrameName("game_ui_menu_item_line.png");
	batteryLine->setPosition(ccp(54, 60+5));
	menuItem->addChild(batteryLine, -1);

	CCSprite* pauseTimeBg = CCSprite::createWithSpriteFrameName("game_ui_menu_pannel_time.png");
	pauseTimeBg->setPosition(ccp(54, 20));
	menuItem->addChild(pauseTimeBg);
	
	m_labelTimeHour = CCLabelBMFont::create("67", "font_battery.fnt");
	m_labelTimeHour->setPosition(ccp(54-17, 20-10));
	menuItem->addChild(m_labelTimeHour);

	m_labelTimeMinute = CCLabelBMFont::create("89", "font_battery.fnt");
	m_labelTimeMinute->setPosition(ccp(54+17, 20-10));	
	menuItem->addChild(m_labelTimeMinute);

	CCSprite* pauseBatteryBg = CCSprite::createWithSpriteFrameName("game_ui_menu_battery.png");
	pauseBatteryBg->setPosition(ccp(54, 20+34));
	menuItem->addChild(pauseBatteryBg);

	for(int i = 0;i < 3;i++)
	{
		sprintf(lib->m_strTemp, "game_ui_menu_battery_progress_%d.png", i);
		m_sprBattery[i] = CCSprite::createWithSpriteFrameName(lib->m_strTemp);

		m_sprBattery[i]->setPosition(ccp(54-11+i*13, 20+34));
		menuItem->addChild(m_sprBattery[i]);
	}

	lib->setBatteryCallFunc(DyLayerGame_updateBattery);

	DyLayerGame_updateBattery(g_YtLib_currentBatteryLevel);

	//DyItemManager* itemManager = DyItemManager::sharedItemManager();
	
	for(unsigned int i = 0;i < DYITEM_SLOT_CNT;i++)
	{
		CCMenuItemImage* menuItem = (CCMenuItemImage *)tycoonManager->m_menuItem->getChildByTag(GAME_UI_TAG_MENUITEM_SLOT0+i);
		CCLabelTTF* labelRemain = (CCLabelTTF *)getChildByTag(GAME_UI_TAG_DISP_SLOT0_COUNT+i);
		labelRemain->setColor(ccc3(204, 204, 204));
		menuItem->setVisible(false);
		labelRemain->setVisible(false);
	}
	
	for(int i = 0;i < 3;i++)
	{
		getChildByTag(GAME_UI_TAG_SLOT_DISH_0+i)->setVisible(false);
		getChildByTag(GAME_UI_TAG_SLOT_HEART_0+i)->setVisible(false);

		reorderChild(getChildByTag(GAME_UI_TAG_SLOT_HEART_0+i), ZORDER_GAME_BASE_LEVEL2);
		
		getChildByTag(GAME_UI_TAG_SLOT_BALLOON_0+i)->setVisible(false);
		getChildByTag(GAME_UI_TAG_SLOT_BLOCK_0+i)->setVisible(false);
		getChildByTag(GAME_UI_TAG_SLOT_PRICE_0+i)->setVisible(false);
	}

	//sprintf(lib->m_strTemp, "Stage %d", atoi(stageManager->m_curShopInfo->getValue("level", stageManager->m_curLevel))+1);
	sprintf(lib->m_strTemp, "%02d", stageManager->m_curLevel+1);
	CCLabelTTF* labelStage = (CCLabelTTF*)getChildByTag(GAME_UI_TAG_DISP_LEVEL);
	labelStage->setString(lib->m_strTemp);
	labelStage->setColor(ccWHITE);
	reorderChild(labelStage, ZORDER_GAME_MENU_DECO);

	sprintf(lib->m_strTemp, "%d", tycoonManager->m_goal_gold);
	CCLabelTTF* labelGoal = (CCLabelTTF*)getChildByTag(GAME_UI_TAG_DISP_GOAL);
	labelGoal->setString(lib->m_strTemp);
	labelGoal->setColor(ccc3(252, 152, 2));
	reorderChild(labelGoal, ZORDER_GAME_MENU_DECO);

    stageManager->setupShopUI(this);
    
 	CCLayer* layerCashier = (CCLayer*)getChildByTag(GAME_UI_TAG_CASHIER);
    
    tycoonManager->m_aniCashier = DyStaffManager::sharedStaffManager()->getStaffAniSprite(this);

    layerCashier->addChild(tycoonManager->m_aniCashier);

	tycoonManager->m_aniCashier->setVisible(false);

	tycoonManager->m_sprCashierDesk = (CCSprite*)getChildByTag(GAME_UI_TAG_CASHIER_DESK);
    
	CCLayer* layerCurtain = (CCLayer*)getChildByTag(GAME_UI_TAG_CURTAIN);
	CCArray* curtains = layerCurtain->getChildren();
	CCSprite* curtainOnMenu = (CCSprite*)getChildByTag(GAME_UI_TAG_MENU_OVER_CURTAIN);
	switch(stageManager->m_curShopType)
	{
		case DyStageType_Snack:
			tycoonManager->m_sprCashierDesk->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_counter_back.png")->displayFrame());
			tycoonManager->m_sprCashierDesk->setVisible(true);
			tycoonManager->m_urlCounter = "shop_ui_counter.png";
			break;
		case DyStageType_Hamburger:
            tycoonManager->m_sprCashierDesk->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_hamburger_counter.png")->displayFrame());
			tycoonManager->m_sprCashierDesk->setVisible(false);
			tycoonManager->m_urlCounter = "shop_ui_hamburger_counter.png";

			for(unsigned int i = 0;i < curtains->count();i++)
			{
				CCSprite* sprCurtainPiese = (CCSprite*)curtains->objectAtIndex(i);
				sprCurtainPiese->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_hamburger_curtain.png")->displayFrame());
			}
			curtainOnMenu->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_hamburger_curtain_piese.png")->displayFrame());
			
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_GROUND))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_hamburger_ground.png")->displayFrame());
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_hamburger_desk_main.png")->displayFrame());
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK_BOTTOM))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_hamburger_desk_bottom.png")->displayFrame());

            break;
		case DyStageType_Coffee:
            tycoonManager->m_sprCashierDesk->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_coffee_counter.png")->displayFrame());
			
			tycoonManager->m_sprCashierDesk->setVisible(false);
			tycoonManager->m_urlCounter = "shop_ui_coffee_counter.png";
			
			for(unsigned int i = 0;i < curtains->count();i++)
			{
				CCSprite* sprCurtainPiese = (CCSprite*)curtains->objectAtIndex(i);
				sprCurtainPiese->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_coffee_curtain.png")->displayFrame());
			}

			curtainOnMenu->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_coffee_curtain_piese.png")->displayFrame());

			((CCSprite*)this->getChildByTag(GAME_UI_TAG_GROUND))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_coffee_ground.png")->displayFrame());
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_coffee_desk_main.png")->displayFrame());
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK_BOTTOM))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_coffee_desk_bottom.png")->displayFrame());
            
            for(int i = 0;i < 3;i++)
                ((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK)->getChildByTag(i))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_coffee_crashtable.png")->displayFrame());

			break;
        case DyStageType_China:
            tycoonManager->m_sprCashierDesk->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_counter.png")->displayFrame());
			
			tycoonManager->m_sprCashierDesk->setVisible(false);
			tycoonManager->m_urlCounter = "shop_ui_china_counter.png";
            
            tycoonManager->m_urlBag = "shop_ui_china_bag.png";
            tycoonManager->m_urlBag_disable = "shop_ui_china_bag_call.png";
			
			for(unsigned int i = 0;i < curtains->count();i++)
			{
				CCSprite* sprCurtainPiese = (CCSprite*)curtains->objectAtIndex(i);
				sprCurtainPiese->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_curtain.png")->displayFrame());
			}
            
			curtainOnMenu->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_curtain_piese.png")->displayFrame());
            
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_GROUND))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_ground.png")->displayFrame());
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_desk_main.png")->displayFrame());
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK_BOTTOM))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_desk_bottom.png")->displayFrame());
            
            for(int i = 0;i < 3;i++)
                ((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK)->getChildByTag(i))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_crashtable.png")->displayFrame());
			break;
        case DyStageType_Pizza:
            tycoonManager->m_sprCashierDesk->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_counter.png")->displayFrame());
			
			tycoonManager->m_sprCashierDesk->setVisible(false);
			tycoonManager->m_urlCounter = "shop_ui_pizza_counter.png";
			
            tycoonManager->m_urlBag = "shop_ui_pizza_bag.png";
            tycoonManager->m_urlBag_disable = "shop_ui_pizza_bag_call.png";

            
			for(unsigned int i = 0;i < curtains->count();i++)
			{
				CCSprite* sprCurtainPiese = (CCSprite*)curtains->objectAtIndex(i);
				sprCurtainPiese->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_curtain.png")->displayFrame());
			}
            
			curtainOnMenu->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_curtain_piese.png")->displayFrame());
            
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_GROUND))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_ground.png")->displayFrame());
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_desk_main.png")->displayFrame());
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK_BOTTOM))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_desk_bottom.png")->displayFrame());
            
            for(int i = 0;i < 3;i++)
            {
                ((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK)->getChildByTag(i))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_crashtable.png")->displayFrame());
            
                //((CCSprite*)this->getChildByTag(GAME_UI_TAG_SLOT_DISH_0 + i))->setDisplayFrame(CCSprite::create("blank_trans.png")->displayFrame());
            }
    
            
            
			break;
        case DyStageType_Korean:
            tycoonManager->m_sprCashierDesk->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_counter.png")->displayFrame());
			
            tycoonManager->m_sprCashierDesk->setPosition(ccp(372, 468-2));
            //CCLog("%f, %f", tycoonManager->m_sprCashierDesk->getPosition().x, tycoonManager->m_sprCashierDesk->getPosition().y);
            
			tycoonManager->m_sprCashierDesk->setVisible(false);
			tycoonManager->m_urlCounter = "shop_ui_korean_counter.png";
			
			for(unsigned int i = 0;i < curtains->count();i++)
			{
				CCSprite* sprCurtainPiese = (CCSprite*)curtains->objectAtIndex(i);
				sprCurtainPiese->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_curtain.png")->displayFrame());
			}
            
			curtainOnMenu->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_curtain_piese.png")->displayFrame());
            
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_GROUND))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_ground.png")->displayFrame());
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_desk_main.png")->displayFrame());
			((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK_BOTTOM))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_desk_bottom.png")->displayFrame());
            
            for(int i = 0;i < 3;i++)
                ((CCSprite*)this->getChildByTag(GAME_UI_TAG_DESK)->getChildByTag(i))->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_crashtable.png")->displayFrame());
			break;
	}
	
    if(m_layerGameBoard->m_board->m_isHard)
    {
        ((CCSprite*)this->getChildByTag(GAME_UI_TAG_BOARD))->setDisplayFrame(CCSprite::create("image_game_ui_board2.png")->displayFrame());
    }
    else if(stageManager->m_curShopType == DyStageType_Korean)
    {
        ((CCSprite*)this->getChildByTag(GAME_UI_TAG_BOARD))->setDisplayFrame(CCSprite::create("image_game_ui_board3.png")->displayFrame());
        //((CCSprite*)this->getChildByTag(GAME_UI_TAG_BOARD))->setColor(ccc3(0,0,0));
    }
    
    m_layerGameBoard->m_board->m_boardSprite = (CCSprite*)this->getChildByTag(GAME_UI_TAG_BOARD);
    tutorialManager->m_layer = this;
    tutorialManager->m_boardSprite = (CCSprite*)this->getChildByTag(GAME_UI_TAG_BOARD);
    
    
    if(tycoonManager->m_trouble_table_cnt > 0)
    {
        getChildByTag(GAME_UI_TAG_GROUND)->getChildByTag(tycoonManager->m_trouble_table_idx[0])->setVisible(true);
        getChildByTag(GAME_UI_TAG_DESK)->getChildByTag(tycoonManager->m_trouble_table_idx[0])->setVisible(true);
    }
    
    if(tycoonManager->m_trouble_table_cnt > 1)
    {
        getChildByTag(GAME_UI_TAG_GROUND)->getChildByTag(tycoonManager->m_trouble_table_idx[1])->setVisible(true);
        getChildByTag(GAME_UI_TAG_DESK)->getChildByTag(tycoonManager->m_trouble_table_idx[1])->setVisible(true);
    }
  
    
    tycoonManager->m_last_candy = atoi(stageManager->m_shopinfo->getValue("last_candy", stageManager->m_curShopType));
    tycoonManager->m_last_msg = atoi(stageManager->m_shopinfo->getValue("last_msg", stageManager->m_curShopType));
    tycoonManager->m_last_halo = atoi(stageManager->m_shopinfo->getValue("last_halo", stageManager->m_curShopType));
    tycoonManager->m_last_cross = atoi(stageManager->m_shopinfo->getValue("last_cross", stageManager->m_curShopType));
    tycoonManager->m_last_fire = atoi(stageManager->m_shopinfo->getValue("last_fire", stageManager->m_curShopType));
    
	reorderChild(getChildByTag(GAME_UI_TAG_DISP_HEART_BAR), ZORDER_GAME_ON_CURTAIN);

	tycoonManager->m_progHeart = new CCProgressTimer();
	CCSprite* sprHeartGage = (CCSprite*)getChildByTag(GAME_UI_TAG_DISP_HEART_PROGRESS);
	sprHeartGage->removeFromParentAndCleanup(false);
	tycoonManager->m_progHeart->initWithSprite(sprHeartGage);
	tycoonManager->m_progHeart->setType(kCCProgressTimerTypeBar);
	tycoonManager->m_progHeart->setBarChangeRate(ccp(1, 0));
    tycoonManager->m_progHeart->setMidpoint(ccp(0, 0));    
	tycoonManager->m_progHeart->setAnchorPoint(ccp(0.0, 0.5));
	tycoonManager->m_progHeart->setPosition(sprHeartGage->getPosition());
	addChild(tycoonManager->m_progHeart, ZORDER_GAME_ON_CURTAIN);
	tycoonManager->m_progHeart->release();
	
	reorderChild(getChildByTag(GAME_UI_TAG_DISP_HEART_ICON), ZORDER_GAME_ON_CURTAIN);
	//
	// progress bar - gold
	reorderChild(getChildByTag(GAME_UI_TAG_DISP_GOLD_BAR), ZORDER_GAME_ON_CURTAIN);
	
	tycoonManager->m_progGold = new CCProgressTimer();
	CCSprite* sprGoldGage = (CCSprite*)getChildByTag(GAME_UI_TAG_DISP_GOLD_PROGRESS);
	sprGoldGage->removeFromParentAndCleanup(false);
	tycoonManager->m_progGold->initWithSprite(sprGoldGage);
	tycoonManager->m_progGold->setType(kCCProgressTimerTypeBar);
	tycoonManager->m_progGold->setBarChangeRate(ccp(1, 0));
    tycoonManager->m_progGold->setMidpoint(ccp(0, 0));    
	tycoonManager->m_progGold->setAnchorPoint(ccp(0.0, 0.5));
	tycoonManager->m_progGold->setPosition(sprGoldGage->getPosition());
	addChild(tycoonManager->m_progGold, ZORDER_GAME_ON_CURTAIN);
	tycoonManager->m_progGold->release();
	
	reorderChild(getChildByTag(GAME_UI_TAG_DISP_GOLD_ICON), ZORDER_GAME_ON_CURTAIN);
	reorderChild(getChildByTag(GAME_UI_TAG_DISP_GOLD_LABEL_SHADOW0), ZORDER_GAME_ON_CURTAIN);
	reorderChild(getChildByTag(GAME_UI_TAG_DISP_GOLD_LABEL_SHADOW1), ZORDER_GAME_ON_CURTAIN);
	reorderChild(getChildByTag(GAME_UI_TAG_DISP_GOLD_LABEL_SHADOW2), ZORDER_GAME_ON_CURTAIN);
	reorderChild(getChildByTag(GAME_UI_TAG_DISP_GOLD_LABEL_SHADOW3), ZORDER_GAME_ON_CURTAIN);
	reorderChild(getChildByTag(GAME_UI_TAG_DISP_GOLD_LABEL), ZORDER_GAME_ON_CURTAIN);

	CCLabelTTF* labelGold = (CCLabelTTF *)getChildByTag(GAME_UI_TAG_DISP_GOLD_LABEL);
	labelGold->setColor(ccWHITE);

	sprintf(lib->m_strTemp, "%d", tycoonManager->m_current_gold);
	labelGold->setString(lib->m_strTemp);

	for(int i = 0;i < 4;i++)
	{
		CCLabelTTF* labelGoldShadow = (CCLabelTTF *)getChildByTag(GAME_UI_TAG_DISP_GOLD_LABEL_SHADOW0+i);
		labelGoldShadow->setColor(ccBLACK);
		labelGoldShadow->setString(lib->m_strTemp);
	}

	//
	// progress bar = timer
	reorderChild(getChildByTag(GAME_UI_TAG_DISP_TIME_BAR), ZORDER_GAME_ON_CURTAIN);
	
	getChildByTag(GAME_UI_TAG_DISP_TIME_CLOSEMESSAGE)->setVisible(false);
	tycoonManager->m_progTime = new CCProgressTimer();
	CCSprite* sprTimeGage = (CCSprite*)getChildByTag(GAME_UI_TAG_DISP_TIME_PROGRESS);
	sprTimeGage->removeFromParentAndCleanup(false);
	tycoonManager->m_progTime->initWithSprite(sprTimeGage);
	tycoonManager->m_progTime->setType(kCCProgressTimerTypeBar);
	tycoonManager->m_progTime->setBarChangeRate(ccp(1, 0));
    tycoonManager->m_progTime->setMidpoint(ccp(0, 0));    
	tycoonManager->m_progTime->setAnchorPoint(ccp(0.0, 0.5));
	tycoonManager->m_progTime->setPosition(sprTimeGage->getPosition());
	tycoonManager->m_progTime->setPercentage(100);
	addChild(tycoonManager->m_progTime, ZORDER_GAME_ON_CURTAIN);
	tycoonManager->m_progTime->release();
	
	reorderChild(getChildByTag(GAME_UI_TAG_DISP_TIME_ICON), ZORDER_GAME_ON_CURTAIN);
	reorderChild(getChildByTag(GAME_UI_TAG_DISP_TIME_CLOSEMESSAGE), ZORDER_GAME_ON_CURTAIN);

	reorderChild(getChildByTag(GAME_UI_TAG_MENU_PAUSE), ZORDER_GAME_MENU);
	reorderChild(getChildByTag(GAME_UI_TAG_MENU_PAUSEMENU), ZORDER_GAME_MENU);
    reorderChild(getChildByTag(GAME_UI_TAG_MENU_PAUSEMENU2), ZORDER_GAME_MENU);

	reorderChild(curtainOnMenu, ZORDER_GAME_MENU+1);
	reorderChild(getChildByTag(GAME_UI_TAG_MENU_OVER_STAGE), ZORDER_GAME_MENU+2);

	reorderChild(getChildByTag(GAME_UI_TAG_SLOT_BALLOON_0), ZORDER_GAME_BALLON);
	reorderChild(getChildByTag(GAME_UI_TAG_SLOT_BALLOON_1), ZORDER_GAME_BALLON);
	reorderChild(getChildByTag(GAME_UI_TAG_SLOT_BALLOON_2), ZORDER_GAME_BALLON);

	reorderChild(getChildByTag(GAME_UI_TAG_SLOT_PRICE_0), ZORDER_GAME_PRICE);
	reorderChild(getChildByTag(GAME_UI_TAG_SLOT_PRICE_1), ZORDER_GAME_PRICE);
	reorderChild(getChildByTag(GAME_UI_TAG_SLOT_PRICE_2), ZORDER_GAME_PRICE);

	for(int i = 0;i < 3;i++)
	{
		CCSprite* sprDon = (CCSprite*)getChildByTag(GAME_UI_TAG_SLOT_PRICE_0+i);
		tycoonManager->m_labelDon[i] = CCLabelTTF::create("1000", FONT_NAME_DEFAULT, 18.f);
		addChild(tycoonManager->m_labelDon[i], ZORDER_GAME_PRICE+1);
		tycoonManager->m_labelDon[i]->setPosition(ccp(sprDon->getPosition().x, sprDon->getPosition().y-2));
		tycoonManager->m_labelDon[i]->setVisible(false);
		
		tycoonManager->m_sprDonIcon[i] = CCSprite::createWithSpriteFrameName("game_ui_price_icon.png");
		addChild(tycoonManager->m_sprDonIcon[i], ZORDER_GAME_PRICE+1);
		tycoonManager->m_sprDonIcon[i]->setPosition(ccp(sprDon->getPosition().x - sprDon->getContentSize().width/2, sprDon->getPosition().y));
		tycoonManager->m_sprDonIcon[i]->setVisible(false);
	}

	
	for(int i = 0;i < 3;i++)
	{
		CCLayer* layerCharacter = (CCLayer*)getChildByTag(GAME_UI_TAG_SLOT_TOUCH_CHARACTER_0 + i);
		CCPoint tgtPoint = ccp(layerCharacter->getPosition().x+layerCharacter->getContentSize().width-25, layerCharacter->getPosition().y+layerCharacter->getContentSize().height+25);

		tycoonManager->m_indicatorCharacter[i] = YtAniObject::loadAniObject(tycoonManager, "effect_indicator.ccbi");
		tycoonManager->m_indicatorCharacter[i]->setPosition(tgtPoint);
		addChild(tycoonManager->m_indicatorCharacter[i], ZORDER_GAME_GAME_INDICATOR);

		tycoonManager->m_indicatorCharacter[i]->setVisible(false);
	}

	for(int i = 0;i < 3;i++)
	{
		CCLayer* layerPay = (CCLayer*)getChildByTag(GAME_UI_TAG_SLOT_PAY_0 + i);
		CCPoint tgtPoint = ccp(layerPay->getPosition().x+layerPay->getContentSize().width-25, layerPay->getPosition().y+layerPay->getContentSize().height-25);

		tycoonManager->m_indicatorDish[i] = YtAniObject::loadAniObject(tycoonManager, "effect_indicator.ccbi");
		tycoonManager->m_indicatorDish[i]->setPosition(tgtPoint);
		addChild(tycoonManager->m_indicatorDish[i], ZORDER_GAME_GAME_INDICATOR);
		
		tycoonManager->m_indicatorDish[i]->setVisible(false);
	}

	m_layerPause = CCLayerColor::create(ccc4(0, 0, 0, 140), lib->m_sizeWin.width, lib->m_sizeWin.height);
	addChild(m_layerPause, ZORDER_GAME_PAUSELAYER);
	reorderChild(layerCurtain, ZORDER_GAME_CURTAIN);
	
    
    
	m_layerPause->setVisible(false);
    
	setTouchEnabled(true);

	tycoonManager->m_openDoggy = YtAniObject::loadAniObject(this, "node_animal_doggy.ccbi");
	DyCostumeManager::sharedCostumeManager()->setCostume(tycoonManager->m_openDoggy, &DyDataManager::sharedDataManager()->m_public.m_costume_status, &DyDataManager::sharedDataManager()->m_custume_function);
	tycoonManager->m_openDoggy->setPosition(ccp(m_layerGameBoard->getPosition().x + m_layerGameBoard->getContentSize().width/2, m_layerGameBoard->getPosition().y + m_layerGameBoard->getContentSize().height/2));
	tycoonManager->m_openDoggy->m_actionManager->setDelegate(this);
	addChild(tycoonManager->m_openDoggy, ZORDER_GAME_BASE_LEVEL2);
	tycoonManager->m_openDoggy->setVisible(false);

	tycoonManager->m_questbar_show = false;
	tycoonManager->m_questbar = YtAniObject::loadAniObject(this, "effect_quest.ccbi");
	tycoonManager->m_questbar->setPosition(ccp(lib->m_sizeWin.width, lib->m_sizeWin.height));
	addChild(tycoonManager->m_questbar, ZORDER_GAME_MENU);
	tycoonManager->m_questbar->m_delegate = this;
	tycoonManager->m_menuQuestPopup = (CCMenu*)((CCLayer*)tycoonManager->m_questbar->getChildByTag(1000))->getChildByTag(1001);
	tycoonManager->m_menuQuestPopup->setEnabled(false);
    
    if(tycoonManager->m_quest && tycoonManager->m_quest->m_questtype == DyQuestType_Ignore)
        tycoonManager->m_questbar->setVisible(false);
    
    
    DyRankManager* rankManager = DyRankManager::sharedRankManager();
    rankManager->initCheckPoint(tycoonManager->m_isCrazyMode ? DyRankType_Crazy : DyRankType_Stage);
    
    tycoonManager->m_currentTarget = rankManager->getCurrrentTarget(rankManager->currentCheckRankType(), tycoonManager->m_crazy_point);
    
    
    tycoonManager->m_changeOrder = false;
    
	CCLabelTTF* labelQ = (CCLabelTTF*)tycoonManager->m_questbar->getChildByTag(1000)->getChildByTag(1200);
	labelQ->setColor(ccc3(255, 240, 0));
    
    CCSprite* sprFlag = (CCSprite*)tycoonManager->m_questbar->getChildByTag(1000)->getChildByTag(1201);
    
    if(tycoonManager->m_isCrazyMode)
    {
        sprFlag->setVisible(true);
        labelQ->setVisible(false);
    }
    else
    {
        sprFlag->setVisible(false);
        labelQ->setVisible(true);
    }
    
	CCLayer* layerQuestContent = (CCLayer*)tycoonManager->m_questbar->getChildByTag(1000)->getChildByTag(1100);

    if(tycoonManager->m_isCrazyMode)
    {
        tycoonManager->setRankPopup();
    }
    else
    {
        sprintf(lib->m_strTemp, stageManager->m_curShopInfo->getValue("quest_pannel", stageManager->m_curLevel), tycoonManager->m_quest->m_countComplete);
        buildQuest(layerQuestContent, lib->m_strTemp);
        
    }
	layerQuestContent->setVisible(false);
        
    
    //
    // 사용가능한 아이템갯수 셋팅
    for(unsigned int i = 0;i < itemManager->m_arrItems->count();i++)
    {
        DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(i);
        
        item->m_cntAvailable = item->m_cntHas;
        
        //
        // 시간 늘이기만 3개
        if(item->m_itemtype == DyItemType_TimeIncrease)
        {
            if(item->m_cntAvailable > 3)
                item->m_cntAvailable = 3;
        }
    }
    
    
    CCSprite* sprGold = (CCSprite*)tycoonManager->m_layer->getChildByTag(GAME_UI_TAG_DISP_GOLD_ICON);
    CCSprite* sprGoldGlow = CCSprite::createWithSpriteFrameName("game_ui_gage_gold_glow.png");
    sprGold->addChild(sprGoldGlow);
    sprGoldGlow->setTag(GAME_UI_TAG_DISP_GOLD_GLOW);
    sprGoldGlow->setOpacity(0);
    sprGoldGlow->setPosition(ccp(sprGold->getContentSize().width/2, sprGold->getContentSize().height/2));
    
    CCSprite* sprHeart = (CCSprite*)tycoonManager->m_layer->getChildByTag(GAME_UI_TAG_DISP_HEART_ICON);
    
    CCSprite* sprHeartEff = CCSprite::createWithSpriteFrameName("game_ui_gage_heart.png");
    sprHeart->addChild(sprHeartEff);
    sprHeartEff->setTag(GAME_UI_TAG_DISP_HEART_EFF);
    sprHeartEff->setOpacity(0);
    sprHeartEff->setPosition(ccp(sprHeart->getContentSize().width/2, sprHeart->getContentSize().height/2));
    
    CCSprite* sprHeartGlow = CCSprite::createWithSpriteFrameName("game_ui_gage_heart_glow.png");
    sprHeart->addChild(sprHeartGlow);
    sprHeartGlow->setTag(GAME_UI_TAG_DISP_HEART_GLOW);
    sprHeartGlow->setOpacity(0);
    sprHeartGlow->setPosition(sprHeartEff->getPosition());
    
    sprHeart->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("game_ui_gage_heart_disable.png"));
	
    CCSprite* sprTime = (CCSprite*)tycoonManager->m_layer->getChildByTag(GAME_UI_TAG_DISP_TIME_ICON);
    CCSprite* sprTimeGlow = CCSprite::createWithSpriteFrameName("game_ui_gage_time_glow.png");
    sprTime->addChild(sprTimeGlow);
    sprTimeGlow->setTag(GAME_UI_TAG_DISP_TIME_GLOW);
    sprTimeGlow->setOpacity(0);
    sprTimeGlow->setPosition(ccp(sprTime->getContentSize().width/2, sprTime->getContentSize().height/2));
    
    
    if(tycoonManager->m_isCrazyMode)
    {
        CCSprite* sprPause = (CCSprite*)getChildByTag(GAME_UI_TAG_MENU_OVER_STAGE);
        sprPause->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("crazy_pannel_top_crazymode.png"));
        getChildByTag(GAME_UI_TAG_DISP_LEVEL)->setVisible(false);
        getChildByTag(GAME_UI_TAG_DISP_GOAL)->setVisible(false);
        
        getChildByTag(GAME_UI_TAG_DISP_GOLD_LABEL)->setVisible(false);
        getChildByTag(GAME_UI_TAG_DISP_GOLD_BAR)->setVisible(false);
                
        getChildByTag(GAME_UI_TAG_DISP_GOLD_LABEL_SHADOW0)->setVisible(false);
        getChildByTag(GAME_UI_TAG_DISP_GOLD_LABEL_SHADOW1)->setVisible(false);
        getChildByTag(GAME_UI_TAG_DISP_GOLD_LABEL_SHADOW2)->setVisible(false);
        getChildByTag(GAME_UI_TAG_DISP_GOLD_LABEL_SHADOW3)->setVisible(false);

        getChildByTag(GAME_UI_TAG_DISP_HEART_ICON)->setVisible(false);
        getChildByTag(GAME_UI_TAG_DISP_HEART_BAR)->setVisible(false);
      
        getChildByTag(GAME_UI_TAG_DISP_TIME_ICON)->setVisible(false);
        
        getChildByTag(GAME_UI_TAG_DISP_TIME_CLOSEMESSAGE)->setVisible(false);
        getChildByTag(GAME_UI_TAG_DISP_TIME_BAR)->setVisible(false);
        
        
        tycoonManager->m_progTime->setVisible(false);
        tycoonManager->m_progHeart->setVisible(false);
        tycoonManager->m_progGold->setVisible(false);
        
        
        CCSprite* sprPanScore = CCSprite::createWithSpriteFrameName("crazy_pannel_top_crazymode_score_bottom.png");
        sprPanScore->setAnchorPoint(ccp(0.5, 0));
        addChild(sprPanScore, ZORDER_GAME_ON_CURTAIN);
        sprPanScore->setPosition(ccp(245, 585));
        
        tycoonManager->m_labelCrazyScore = CCLabelBMFont::create("0", "font_digit.fnt");
        tycoonManager->m_labelCrazyScore->setPosition(ccp(150, 10));
        tycoonManager->m_labelCrazyScore->setAnchorPoint(ccp(1.0, 0.5));
        tycoonManager->m_labelCrazyScore->setColor(ccc3(245,0,8));
        sprPanScore->addChild(tycoonManager->m_labelCrazyScore);

        
        tycoonManager->m_sprPanHeart = CCSprite::createWithSpriteFrameName("crazy_pannel_top_crazymode_life_bottom.png");
        addChild(tycoonManager->m_sprPanHeart, ZORDER_GAME_ON_CURTAIN);
        tycoonManager->m_sprPanHeart->setAnchorPoint(ccp(0.5, 0));
        tycoonManager->m_sprPanHeart->setPosition(ccp(405, 585));
        
        tycoonManager->m_crazyRemainHeart = 3;
        for(int i = 0;i < tycoonManager->m_crazyRemainHeart;i++)
        {
            tycoonManager->m_sprCrazyHeart[i] = CCSprite::createWithSpriteFrameName("crazy_pannel_top_crazymode_life_on.png");
            tycoonManager->m_sprPanHeart->addChild(tycoonManager->m_sprCrazyHeart[i]);
            tycoonManager->m_sprCrazyHeart[i]->setPosition(ccp(23 + i*39, 18));
        }
        
        CCSprite* sprPanTime = CCSprite::createWithSpriteFrameName("crazy_pannel_top_crazymode_time_bottom.png");
        addChild(sprPanTime, ZORDER_GAME_ON_CURTAIN);
        sprPanTime->setAnchorPoint(ccp(0.5, 0));
        sprPanTime->setPosition(ccp(555, 585));
        
        tycoonManager->m_labelCrazyTimeM = CCLabelBMFont::create("00", "font_digit.fnt");
        tycoonManager->m_labelCrazyTimeM->setPosition(ccp(55, 10));
        tycoonManager->m_labelCrazyTimeM->setAnchorPoint(ccp(1.0, 0.5));
        tycoonManager->m_labelCrazyTimeM->setColor(ccc3(91,255,10));
        sprPanTime->addChild(tycoonManager->m_labelCrazyTimeM);

        
        tycoonManager->m_labelCrazyTimeS = CCLabelBMFont::create("00", "font_digit.fnt");
        tycoonManager->m_labelCrazyTimeS->setPosition(ccp(70, 10));
        tycoonManager->m_labelCrazyTimeS->setAnchorPoint(ccp(0.0, 0.5));
        tycoonManager->m_labelCrazyTimeS->setColor(ccc3(91,255,10));
        sprPanTime->addChild(tycoonManager->m_labelCrazyTimeS);
        
        
        CCSprite* sprPanGold = CCSprite::createWithSpriteFrameName("crazy_pannel_top_crazymode_mygold_bottom.png");
        addChild(sprPanGold, ZORDER_GAME_ON_CURTAIN);
        sprPanGold->setAnchorPoint(ccp(0.5, 0));
        sprPanGold->setPosition(ccp(700, 585-2));
        
        tycoonManager->m_labelCrazyGold = CCLabelTTF::create("0", FONT_NAME_DEFAULT, 25);
        tycoonManager->m_labelCrazyGold->setAnchorPoint(ccp(1,0));
        tycoonManager->m_labelCrazyGold->setPosition(ccp(130, 3));
        tycoonManager->m_labelCrazyGold->setColor(ccc3(255,255,11));
        sprPanGold->addChild(tycoonManager->m_labelCrazyGold);
        
        CCSprite* sprGoldIcon = (CCSprite*)getChildByTag(GAME_UI_TAG_DISP_GOLD_ICON);
        
        sprGoldIcon->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("crazy_pannel_top_crazymode_mygold.png"));
        reorderChild(sprGoldIcon, ZORDER_GAME_ON_CURTAIN);
        sprGoldIcon->setPosition(ccp(700-45, 585-2+20));
        
        CCSprite* sprPanDelly = CCSprite::createWithSpriteFrameName("crazy_pannel_top_crazymode_mydelly_bottom.png");
        addChild(sprPanDelly, ZORDER_GAME_ON_CURTAIN);
        sprPanDelly->setAnchorPoint(ccp(0.5, 0));
        sprPanDelly->setPosition(ccp(835, 585-2));
        
        tycoonManager->m_labelCrazyDelly = CCLabelTTF::create("0", FONT_NAME_DEFAULT, 25);
        tycoonManager->m_labelCrazyDelly->setAnchorPoint(ccp(1,0));
        tycoonManager->m_labelCrazyDelly->setPosition(ccp(100, 3));
        tycoonManager->m_labelCrazyDelly->setColor(ccc3(58,252,224));
        sprPanDelly->addChild(tycoonManager->m_labelCrazyDelly);
        
        
        CCSprite* sprDellyIcon = CCSprite::createWithSpriteFrameName("crazy_pannel_top_crazymode_mydelly.png");
        addChild(sprDellyIcon, ZORDER_GAME_ON_CURTAIN, GAME_UI_TAG_DISP_DELLY_ICON);
        sprDellyIcon->setPosition(ccp(835-33, 585-2+20));
        
        CCSprite* sprDellyGlow = CCSprite::createWithSpriteFrameName("game_ui_gage_gold_glow.png");
        sprDellyIcon->addChild(sprDellyGlow);
        sprDellyGlow->setTag(GAME_UI_TAG_DISP_DELLY_GLOW);
        sprDellyGlow->setOpacity(0);
        sprDellyGlow->setPosition(ccp(sprDellyGlow->getContentSize().width/2, sprDellyGlow->getContentSize().height/2));
        
    }
    
    for(int i = 0;i < 3;i++)
    {
        CCMenuItemImage* menuItem = (CCMenuItemImage *)tycoonManager->m_menuItem->getChildByTag(GAME_UI_TAG_MENUITEM_SLOT0+i);
        
        tycoonManager->m_dtItemSlot[i] = 30;
        tycoonManager->m_enableSlotItem[i] = true;
        
        tycoonManager->m_sprProgItemSlotBg[i] = CCSprite::createWithSpriteFrameName("game_ui_btn_bomb.png");
        addChild(tycoonManager->m_sprProgItemSlotBg[i], ZORDER_GAME_BASE_LEVEL2);
        tycoonManager->m_sprProgItemSlotBg[i]->setPosition(menuItem->getPosition());
        tycoonManager->m_sprProgItemSlotBg[i]->setVisible(false);
        
        tycoonManager->m_progItemSlot[i] = CCProgressTimer::create(CCSprite::createWithSpriteFrameName("game_ui_btn_bomb.png"));
        
//        tycoonManager->m_progItemSlot[i]->setType(kCCProgressTimerTypeBar);
//        tycoonManager->m_progItemSlot[i]->setBarChangeRate(ccp(0, 1));
//        tycoonManager->m_progItemSlot[i]->setMidpoint(ccp(0.5, 0));
        tycoonManager->m_progItemSlot[i]->setType(kCCProgressTimerTypeRadial);

        addChild(tycoonManager->m_progItemSlot[i], ZORDER_GAME_BASE_LEVEL2);
        tycoonManager->m_progItemSlot[i]->setPosition(menuItem->getPosition());
        
        tycoonManager->m_progItemSlot[i]->setVisible(false);
        
        tycoonManager->m_labelItemSlot[i] = CCLabelTTF::create("10", FONT_NAME_DEFAULT, 18);
        tycoonManager->m_labelItemSlot[i]->setColor(ccc3(204, 204, 204));
        addChild(tycoonManager->m_labelItemSlot[i], ZORDER_GAME_BASE_LEVEL2);
        
        tycoonManager->m_labelItemSlot[i]->setPosition(ccp(menuItem->getPosition().x, menuItem->getPosition().y-14));
        
        tycoonManager->m_labelItemSlot[i]->setVisible(false);
    }
    
    curtainDrop(true, true);
    
    buildDebugCostume();
    
    CCLayer* layerStaff = (CCLayer*)getChildByTag(GAME_UI_TAG_STAFF_SELECT);
    
    
    if(stageManager->m_curShopType < DyStageType_Pizza)
    {
        layerStaff->setVisible(false);
        dataManager->m_private.m_current_staff = 0;
    }
    reorderChild(layerStaff, ZORDER_GAME_ON_CURTAIN);
}


CCLabelTTF* DyLayerGame::buildQuest(CCLayer* layer, std::string message)
{
    CCLabelTTF* labelContent = (CCLabelTTF*)layer->getChildByTag(10);
    
    if(labelContent == NULL)
    {
        labelContent = CCLabelTTF::create(message.c_str(), FONT_NAME_EXP, message.size() > 32 ? 16.f : 18.f);
        labelContent->setTag(10);
        layer->addChild(labelContent);
    }
    else
        labelContent->setString(message.c_str());
    
    labelContent->setColor(ccWHITE);
    labelContent->setPosition(ccp(10, 0));
    
    
    return labelContent;
}

void DyLayerGame::buildDebugCostume()
{
    DyLib* lib = DyLib::sharedLib();
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(g_isDebug)
    {
        CCLabelTTF* labelDebug = (CCLabelTTF*)getChildByTag(GAME_UI_TAG_CURTAIN)->getChildByTag(0);
        
        sprintf(lib->m_strTemp, "I:%d H:%d D:%d T:%d C:%d", tycoonManager->m_functionGame.m_ice,
                tycoonManager->m_functionGame.m_heart,
                tycoonManager->m_functionGame.m_decay,
                tycoonManager->m_functionGame.m_tip,
                tycoonManager->m_functionGame.m_cooling);
        
        labelDebug->setString(lib->m_strTemp);
        labelDebug->setVisible(true);
    }

}


void DyLayerGame::hideQuestPopup(float dt)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(!tycoonManager->m_isCrazyMode && tycoonManager->m_quest && tycoonManager->m_quest->m_questtype == DyQuestType_Ignore)
         return;
    
	if(tycoonManager->m_questbar_show)
	{
		tycoonManager->m_questbar->m_actionManager->runAnimations("hide");
		tycoonManager->m_questbar_show = false;

		CCLayer* layerQuestContent = (CCLayer*)tycoonManager->m_questbar->getChildByTag(1000)->getChildByTag(1100);
		layerQuestContent->setVisible(false);
	}

	unschedule(schedule_selector(DyLayerGame::hideQuestPopup));
}



void DyLayerGame::showQuestPopup(bool show, bool reverse)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

    if(!tycoonManager->m_isCrazyMode && tycoonManager->m_quest && tycoonManager->m_quest->m_questtype == DyQuestType_Ignore)
        return;

	if(show)
	{
        YtLib::sharedLib()->playEffect(YtLibSndEff_CURTAIN_SPREAD);
        
		tycoonManager->m_questbar->m_actionManager->runAnimations("show");
		tycoonManager->m_questbar_show = true;

        if(reverse)
            schedule(schedule_selector(DyLayerGame::hideQuestPopup), 2.0f);

		CCLayer* layerQuestContent = (CCLayer*)tycoonManager->m_questbar->getChildByTag(1000)->getChildByTag(1100);
		layerQuestContent->setVisible(true);
        
        
        //m_layerGameBoard->m_board->printBlockInfo();
        
        
	}
	else
	{
        YtLib::sharedLib()->playEffect(YtLibSndEff_CURTAIN_FOLD);
        
		tycoonManager->m_questbar->m_actionManager->runAnimations("hide");
		tycoonManager->m_questbar_show = false;
        
        if(reverse)
            unschedule(schedule_selector(DyLayerGame::hideQuestPopup));
	
		CCLayer* layerQuestContent = (CCLayer*)tycoonManager->m_questbar->getChildByTag(1000)->getChildByTag(1100);
		layerQuestContent->setVisible(false);
	}
    
}


void DyLayerGame::onClicked()
{
	if(!m_isTop)
		return;

	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

	if(tycoonManager->m_questbar_show)
	{
		showQuestPopup(false, true);
	}
	else
	{
		showQuestPopup(true, true);
	}
}


void DyLayerGame::onEnter()
{
	YtLayer::onEnter();
}


void DyLayerGame::endAnimationGoldEffect(CCNode* node)
{
	node->removeFromParentAndCleanup(true);

	this->removeChildByTag(GAME_UI_TAG_GOAL_BALLON, true);
	this->removeChildByTag(GAME_UI_TAG_GOAL_STAR, true);
}


void DyLayerGame::startGoldEffect(DyGameStatus gamestaus)
{
	if(m_gamestatus == gamestaus)
		return;

    YtLib::sharedLib()->playEffect(YtLibSndEff_Goal);
    
	m_gamestatus = gamestaus;

	CCSprite* sprGoldBar = (CCSprite*)getChildByTag(GAME_UI_TAG_DISP_GOLD_BAR);

	CCPoint posision = ccp(sprGoldBar->getPosition().x + sprGoldBar->getContentSize().width/2-20, sprGoldBar->getPosition().y);

	CCSprite* sprEffect = CCSprite::createWithSpriteFrameName("game_ui_pannel_alarm01.png");
	sprEffect->setAnchorPoint(ccp(0.5f, 0.5f));
	sprEffect->setPosition(posision);
	addChild(sprEffect, ZORDER_GAME_ON_CURTAIN, GAME_UI_TAG_GOAL_BALLON);
	sprEffect->runAction(CCRepeatForever::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_goal_pannel")));

	CCSprite* sprGoal;
	
	if(m_gamestatus == DyGameStatus_Perfect)
	{
		CCSprite* sprStar = CCSprite::createWithSpriteFrameName("threematch_ui_ani_stars01.png");
		sprStar->setAnchorPoint(ccp(0.5f, 0.5f));
		sprStar->setPosition(posision);
		addChild(sprStar, ZORDER_GAME_ON_CURTAIN, GAME_UI_TAG_GOAL_STAR);

		sprStar->runAction(CCSequence::create(
			CCextAnimatePacker::getInstance()->getSequence("threematch_ui_goal_star"),
			NULL));

		sprGoal = CCSprite::createWithSpriteFrameName("game_ui_text_perfect.png");
	}
	else
	{
		sprEffect->setScale(0.9f);
		sprGoal = CCSprite::createWithSpriteFrameName("game_ui_text_goal.png");
	}

	sprGoal->setPosition(posision);
	addChild(sprGoal, ZORDER_GAME_ON_CURTAIN);
	sprGoal->setScale(0.9f);
	sprGoal->runAction(CCSequence::create(
		CCScaleTo::create(0.25f, 1.0f, 1.0f),
		CCScaleTo::create(0.25f, 0.9f, 0.9f),
		CCScaleTo::create(0.25f, 1.0f, 1.0f),
		CCScaleTo::create(0.25f, 0.9f, 0.9f),
		CCCallFuncN::create(this, callfuncN_selector(DyLayerGame::endAnimationGoldEffect)), 
		NULL));
}


void DyLayerGame::completedAnimationSequenceNamed(const char *name)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    
	if(strcmp(name, "start") == 0)
	{
		tycoonManager->m_openDoggy->setVisible(false);
		tycoonManager->startGame();
        
        
        if(!tutorialManager->m_pauseBoard)
            schedule(schedule_selector(DyLayerGame::displayGrill), 2.0f);
	}
	//else if(strcmp(name, "open_fadeout") == 0)
	//{
	//	//m_open->removeFromParentAndCleanup(true);
	//	//YtAniObject::unloadAniObject(m_open);
	//}
	//else if(strcmp(name, "fever_start") == 0)
	//{
	//	tycoonManager->m_feverstart->setVisible(false);
	//}
}


SEL_MenuHandler DyLayerGame::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onItemSlotClicked", DyLayerGame::onItemSlotClicked);

    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPauseClicked", DyLayerGame::onPauseClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPlayClicked", DyLayerGame::onPlayClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onHomeClicked", DyLayerGame::onHomeClicked);

	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onOptionClicked", DyLayerGame::onOptionClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onReplayClicked", DyLayerGame::onReplayClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onListClicked", DyLayerGame::onListClicked);


	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onShopClicked", DyLayerGame::onShopClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onMyroomClicked", DyLayerGame::onMyroomClicked);

    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onStartClicked", DyLayerGame::onStartClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCostumeClicked", DyLayerGame::onCostumeClicked);

    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onSelectStaff", DyLayerGame::onSelectStaff);
    
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onStaffClicked", DyLayerGame::onStaffClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPromoteClicked", DyLayerGame::onPromoteClicked);
    
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onMachineClicked", DyLayerGame::onMachineClicked);
    return NULL;    
}


SEL_CCControlHandler DyLayerGame::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}


bool DyLayerGame::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerGame::curtainDrop(bool drop, bool direct)
{
    CCLayer* _layerCurtain = (CCLayer*)getChildByTag(GAME_UI_TAG_CURTAIN);
    
    CCLayer* layerStaff = (CCLayer*)getChildByTag(GAME_UI_TAG_STAFF_SELECT);
    
    if(drop)
    {
        if(direct)
        {
             _layerCurtain->setPosition(ccp(0, 331));
            layerStaff->setPosition(ccp(0, 100));
        }
        else
        {
            _layerCurtain->runAction(CCSequence::create(
                                                        CCMoveTo::create(0.25f, ccp(0, 291)),
                                                        NULL));

            layerStaff->runAction(CCSequence::create(CCMoveTo::create(0.25f, ccp(0, 100)), NULL));
        }
        
        
    }
    else
    {
        _layerCurtain->runAction(CCSequence::create(
                                                    CCMoveTo::create(0.25f, ccp(0, 913)),
                                                    NULL));
        
        layerStaff->runAction(CCSequence::create(CCMoveTo::create(0.25f, ccp(0, 0)), NULL));
    }
}


void DyLayerGame::onPauseClicked(CCObject * pSender)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	
    if(tycoonManager->m_checkLastPang)
        return;

    if(tycoonManager->m_gameSetting)
    {
        reorderChild(m_layerPause, ZORDER_GAME_PAUSELAYER);
    }
    else if(!tycoonManager->m_gameStarted)
        return;
    
	unschedule(schedule_selector(DyLayerGame::hideQuestPopup));
	
    if(pSender)
    {
        YtLib::sharedLib()->playEffect(YtLibSndEff_CURTAIN_SPREAD);
        m_actionManager->runAnimations("pause");
    }
    else
        m_actionManager->runAnimations("pause_by_phone");
	this->pauseSchedulerAndActions();
	m_layerGameBoard->pauseSchedulerAndActions();
	m_layerGameBoard->setTouchEnabled(false);
	tycoonManager->pauseGame();

	tycoonManager->m_menuItem->setEnabled(false);
	tycoonManager->m_menuPause->setEnabled(false);
	setKeypadEnabled(true);
    
    tycoonManager->m_menuPauseMenu->setEnabled(true);
    tycoonManager->m_menuPauseMenu2->setEnabled(true);
    
	m_layerPause->setVisible(true);
    
	CCSprite* sprCurtainPiece = (CCSprite*)getChildByTag(GAME_UI_TAG_MENU_OVER_CURTAIN);
	CCLayerColor* layerCurtain = CCLayerColor::create(ccc4(0, 0, 0, 140), sprCurtainPiece->getContentSize().width,
		sprCurtainPiece->getContentSize().height);
	sprCurtainPiece->addChild(layerCurtain);

	YtLib::sharedLib()->stopBGM();

	m_isGamePause = true;
    
    calltime(0);
    m_labelTimeHour->unschedule(schedule_selector(DyLayerGame::calltime));
	m_labelTimeHour->schedule(schedule_selector(DyLayerGame::calltime), 1.0f);
    
    
    
    curtainDrop(true, pSender == NULL);
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(dataManager->m_display_ad)
        YtPlatform_ShowAd(true);
    
    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    
    tutorialManager->showTutorial(false);
}


void DyLayerGame::onExit()
{
	YtLayer::onExit();
    YtPlatform_ShowAd(false);
}


void DyLayerGame::calltime(float dt)
{
	YtLib* lib = YtLib::sharedLib();

	int hour, minute, second;
	YtLib::getCurrentTime(&hour, &minute, &second);

	sprintf(lib->m_strTemp, "%02d", hour);
	g_layerGame->m_labelTimeHour->setString(lib->m_strTemp);
	
	sprintf(lib->m_strTemp, "%02d", minute);
	g_layerGame->m_labelTimeMinute->setString(lib->m_strTemp);
}


void DyLayerGame::onPlayClicked(CCObject * pSender)
{
	//YtLib* lib = YtLib::sharedLib();
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	//DyStageManager* stageManager = DyStageManager::sharedStageManager();
	//DyBlockManager* blockManager = DyBlockManager::sharedBlockManager();

	//setBlocks();

	m_actionManager->runAnimations("resume");
	this->resumeSchedulerAndActions();
	m_layerGameBoard->resumeSchedulerAndActions();
	m_layerGameBoard->setTouchEnabled(true);

	tycoonManager->resumeGame();

	tycoonManager->m_menuItem->setEnabled(true);
	tycoonManager->m_menuPause->setEnabled(true);
	tycoonManager->m_menuPauseMenu->setEnabled(false);
    tycoonManager->m_menuPauseMenu2->setEnabled(false);
    
    setKeypadEnabled(true);
    
    
	

	CCSprite* sprCurtainPiece = (CCSprite*)getChildByTag(GAME_UI_TAG_MENU_OVER_CURTAIN);

//	CCLayerColor* layerCurtain = CCLayerColor::create(ccc4(0, 0, 0, 180), sprCurtainPiece->getContentSize().width,
//		sprCurtainPiece->getContentSize().height);
	sprCurtainPiece->removeAllChildrenWithCleanup(true);

    if(tycoonManager->m_gameStarted)
    {
        m_layerPause->setVisible(false);
        YtLib::sharedLib()->playBGM(tycoonManager->m_bgmIdx);
    }
    else
    {
        reorderChild(m_layerPause, ZORDER_GAME_SETTING);
    }
    
	m_isGamePause = false;
	m_labelTimeHour->unschedule(schedule_selector(DyLayerGame::calltime));

	hideQuestPopup(2.0f);
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_CURTAIN_FOLD);
    
    
    curtainDrop(false, false);
    
    YtPlatform_ShowAd(false);
    
    
    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    
    tutorialManager->showTutorial(true);
    
    
    if(DyStaffManager::sharedStaffManager()->isHire())
    {
        CCLayer* layerCashier = (CCLayer*)getChildByTag(GAME_UI_TAG_CASHIER);
        layerCashier->removeAllChildrenWithCleanup(false);
        if(tycoonManager->m_aniCashier)
            YtAniObject::unloadAniObject(tycoonManager->m_aniCashier);
        tycoonManager->m_aniCashier = DyStaffManager::sharedStaffManager()->getStaffAniSprite(this);
        layerCashier->addChild(tycoonManager->m_aniCashier);
    }

}


void DyLayerGame::onPopupClicked(cocos2d::CCObject * pSender)
{
	//DyLib* lib = DyLib::sharedLib();
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);

	if(pPopup->m_active_tag == 1)
	{
        YtLib::sharedLib()->stopBGM();
		YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
		DyLib::sharedLib()->setStatus(DyLib_AppStatus_Tip, 0);
	}
	else if(pPopup->m_active_tag == 11)
	{
        YtLib::sharedLib()->stopBGM();
		YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
		DyLib::sharedLib()->setStatus(DyLib_AppStatus_MainMenu, 0);
		
		m_labelTimeHour->unschedule(schedule_selector(DyLayerGame::calltime));
	}
    else if(pPopup->m_active_tag == 21)
	{
        YtLib::sharedLib()->stopBGM();
		YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        DyStageManager* stageManager = DyStageManager::sharedStageManager();
        DyLib::sharedLib()->setStatus(DyLib_AppStatus_Stage, 10+stageManager->m_curShopType);
		
		m_labelTimeHour->unschedule(schedule_selector(DyLayerGame::calltime));
	}
	else
	{
		YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
	}
}


void DyLayerGame::onListClicked(CCObject* pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerGame::onPopupClicked));
	popup->setText(STRING_question_golist);
	popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 10);
	popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 21);
	popup->addButton(DyButtonType_Close, "", 12);
    
	YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);

}

void DyLayerGame::onReplayClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
	//DyLib* lib = DyLib::sharedLib();

	DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerGame::onPopupClicked));
	popup->setText(STRING_question_replay);
	popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
	popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 1);
	popup->addButton(DyButtonType_Close, "", 2);

	YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerGame::onOptionClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	DyLayerOption* layerOption = DyLayerOption::create(this, false);
	YtLayerManager::sharedLayerManager()->pushLayer(layerOption, true, true);
}


void DyLayerGame::onMyroomClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Myroom, 1, stageManager->m_curShopType);
	YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
}



void DyLayerGame::onShopClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
	
	DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Normal, 3, stageManager->m_curShopType);
	YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
}


void DyLayerGame::onHomeClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
	//DyLib* lib = DyLib::sharedLib();

	DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerGame::onPopupClicked));
	popup->setText(STRING_question_gohome);
	popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 10);
	popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 11);
	popup->addButton(DyButtonType_Close, "", 12);

	YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}


void DyLayerGame::onStartClicked(CCObject* pSender)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    if(!tycoonManager->m_gameSetting)
        return;
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    DyLib* lib = DyLib::sharedLib();
    
   
    if(!tycoonManager->m_isCrazyMode)
    {
        DyStageManager* stageManager = DyStageManager::sharedStageManager();
        
        CCArray* quest_item = stageManager->m_curShopInfo->getArray("quest_item", stageManager->m_curLevel);
        
        if(strcmp(((CCString*)quest_item->objectAtIndex(0))->getCString(), "NULL") != 0)
        {
            DyItemManager* itemManager = DyItemManager::sharedItemManager();
            
            for(int i = 0;i < quest_item->count();i++)
            {
                const char* item_tag = ((CCString*)quest_item->objectAtIndex(i))->getCString();
                
                bool has = false;
                for(int j = 0;j < DYITEM_SLOT_CNT;j++)
                {
                    
                    if(itemManager->m_itemslot[j])
                    {
                        if(itemManager->m_itemslot[j]->m_tag.compare(item_tag) == 0)
                        {
                            has = true;
                        }
                    }
                }
                
                if(!has)
                {
                    DyPopup *popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
                    
                    sprintf(lib->m_strTemp, "%s 아이템을 슬롯에 장착해야 퀘스트를 깰 수 있어요.", itemManager->getItemByTag(item_tag)->m_name.c_str());
                    popup->setText(lib->m_strTemp);
                    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
                    
                    return;
                }
            }
        }
    }
    
//    DyItemManager* itemManager = DyItemManager::sharedItemManager();
//    
//	for(unsigned int i = 0;i < DYITEM_SLOT_CNT;i++)
//	{
//		DyItem* item = itemManager->m_itemslot[i];
//        
//        CCMenuItemImage* menuItem = (CCMenuItemImage *)tycoonManager->m_menuItem->getChildByTag(GAME_UI_TAG_MENUITEM_SLOT0+i);
//        
//        menuItem->setVisible(item);
//	}
    
    
    getChildByTag(GAME_UI_TAG_STAFF_SELECT)->setVisible(false);
    
    tycoonManager->startGameReal();
    itemSlotSetting();
}


void DyLayerGame::onCostumeClicked(CCObject* pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;

    
    if(!DyTycoonManager::sharedTycoonManager()->m_gameSetting)
        return;

    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Myroom, 0, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);

}




void DyLayerGame::buildPromoteButton()
{
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    CCMenuItemImage* menuItem = (CCMenuItemImage*)getChildByTag(GAME_UI_TAG_MENU_START)->getChildByTag(1);
    
    CCSprite* sprNormal = (CCSprite*)menuItem->getNormalImage();
    sprNormal->removeAllChildrenWithCleanup(true);
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    if(staffManager->getGrade(dataManager->m_private.m_current_staff) == staffManager->m_staff[dataManager->m_private.m_current_staff]->count()-1)
    {
        menuItem->setVisible(false);
    }
    else
        menuItem->setVisible(true);
    
    sprNormal->stopAllActions();
    
    if(staffManager->getGrade(dataManager->m_private.m_current_staff) >= 0 &&
       staffManager->getGrade(dataManager->m_private.m_current_staff) < staffManager->m_staff[dataManager->m_private.m_current_staff]->count()-1 &&
       staffManager->getRemainCountCurrent() == 0)
    {
        CCSprite* sprNow = CCSprite::createWithSpriteFrameName("menu_ui_sticker_now.png");
        sprNormal->addChild(sprNow);
        sprNow->setPosition(ccp(120, 0));
        sprNormal->setPosition(ccp(sprNormal->getContentSize().width/2, sprNormal->getContentSize().height/2));
        
        sprNormal->stopAllActions();
        sprNormal->setAnchorPoint(ccp(0.5,0.5));
        CCActionInterval* seq = (CCActionInterval*)(CCSequence::create(
                                                                       CCScaleTo::create(0.1f, 1.3f),
                                                                       CCScaleTo::create(0.1f, 1.0f),
                                                                       CCDelayTime::create(1.f),
                                                                       NULL));
        
        sprNormal->runAction(CCRepeatForever::create(seq));
    }
    
    if(staffManager->getGrade(dataManager->m_private.m_current_staff) == -1 || DyTycoonManager::sharedTycoonManager()->m_isCrazyMode)
    {
        menuItem->setVisible(false);
    }
}



void DyLayerGame::setCashier(bool resume)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    //
	// cashier
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    if(tycoonManager->m_quest && tycoonManager->m_quest->m_questNotStaff)
    {
        staffManager->setIdle(true, "이번 스테이지에서는 휴가중이라 고용할 수 없어요.");
    }
    
    else if(stageManager->m_curShopType == DyStageType_Snack && DyStageManager::sharedStageManager()->m_curLevel < 2)
    {
        staffManager->setIdle(true, "아직 고용할 수 없어요.");
    }
    else
        staffManager->setIdle(false, "");
    
    
    CCLayer* layerStaff = (CCLayer*)getChildByTag(GAME_UI_TAG_STAFF_SELECT);
    
 
    
    CCSprite* sprStaff0_Glow = (CCSprite*)layerStaff->getChildByTag(0);
    CCSprite* sprStaff0 = (CCSprite*)sprStaff0_Glow->getChildByTag(0);
    
    CCSprite* sprStaff1_Glow = (CCSprite*)layerStaff->getChildByTag(1);
    CCSprite* sprStaff1 = (CCSprite*)sprStaff1_Glow->getChildByTag(0);
    
    CCSprite* sprCheck0 = (CCSprite*)layerStaff->getChildByTag(10);
    CCSprite* sprCheck1 = (CCSprite*)layerStaff->getChildByTag(11);
    
    
    if(dataManager->m_public.m_staff_grade >= 3)
    {
        sprStaff0_Glow->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("staff_ui_manager_cursor_glow.png"));
        sprStaff0->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("staff_ui_manager_cursor.png"));
        sprStaff0->setPosition(ccp(41+2, 38-3));
    }
    
    
    if(dataManager->m_private.m_current_staff == 0)
    {
        sprStaff0_Glow->setColor(ccGRAY);
        sprStaff0->setColor(ccGRAY);
   
        sprCheck0->setVisible(true);
        sprCheck1->setVisible(false);
    }
    else
    {
        sprStaff0_Glow->setColor(ccWHITE);
        sprStaff0->setColor(ccWHITE);
        
        sprCheck0->setVisible(false);
        sprCheck1->setVisible(true);
    }
    
    
    if(dataManager->m_private.m_staff_ext_grade[0] == -1)
    {
        sprStaff1_Glow->setColor(ccBLACK);
        sprStaff1->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("staff_ui_icon_lock.png"));
        
    }
    else
    {
        //sprStaff1_Glow->setColor(ccWHITE);
        sprStaff1->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("staff_ui_candy_cursor.png"));
        
        if(dataManager->m_private.m_current_staff == 0)
        {
            sprStaff1_Glow->setColor(ccWHITE);
            sprStaff1->setColor(ccWHITE);
        }
        else
        {
            sprStaff1_Glow->setColor(ccGRAY);
            sprStaff1->setColor(ccGRAY);
        }
    }

    staffManager->setHire(staffManager->hasStaff() && staffManager->getPay() <= moneyManager->getGold());
    
    buildCashierButton();
    buildPromoteButton();
    
    if(staffManager->getGrade(dataManager->m_private.m_current_staff) == -1 || DyTycoonManager::sharedTycoonManager()->m_isCrazyMode)
    {
        layerStaff->setVisible(false);
    }
    
    if(DyStaffManager::sharedStaffManager()->isHire())
    {

        CCLayer* layerCashier = (CCLayer*)getChildByTag(GAME_UI_TAG_CASHIER);
        layerCashier->removeAllChildrenWithCleanup(false);
        if(tycoonManager->m_aniCashier)
            YtAniObject::unloadAniObject(tycoonManager->m_aniCashier);
        tycoonManager->m_aniCashier = DyStaffManager::sharedStaffManager()->getStaffAniSprite(this);
        layerCashier->addChild(tycoonManager->m_aniCashier);
    }
}


void DyLayerGame::buildCashierButton()
{
    DyLib* lib = DyLib::sharedLib();
    
    CCLayer* layer = (CCLayer*)getChildByTag(GAME_UI_TAG_SETTING_STAFF);
    CCLabelTTF* lablePay = (CCLabelTTF*)layer->getChildByTag(0);
    CCSprite* sprCheckbox = (CCSprite*)layer->getChildByTag(1);
    
    CCSprite* sprChecker = (CCSprite*)layer->getChildByTag(2);
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    
    CCMenuItemImage* menuItem = (CCMenuItemImage*)getChildByTag(GAME_UI_TAG_MENU_START)->getChildByTag(0);
    
    CCSprite* sprNormal = (CCSprite*)menuItem->getNormalImage();
    
    sprNormal->removeAllChildrenWithCleanup(true);
    
    if(staffManager->getGrade(dataManager->m_private.m_current_staff) == -1 || DyTycoonManager::sharedTycoonManager()->m_isCrazyMode)
    {
        layer->setVisible(false);
        menuItem->setVisible(false);
    }
    else
    {
        if(staffManager->isIdle() || staffManager->getPay() > moneyManager->getGold())
        {
            sprCheckbox->setDisplayFrame(CCSprite::createWithSpriteFrameName("menu_ui_startmenu_checkbox_lock.png")->displayFrame());
        }
        else
        {
            sprCheckbox->setDisplayFrame(CCSprite::createWithSpriteFrameName("menu_ui_startmenu_checkbox.png")->displayFrame());
        }
        
        
        menuItem->setNormalSpriteFrame(staffManager->getSelSprite()->displayFrame());
        
        
        if(staffManager->isHire())
        {
            sprintf(lib->m_strTemp, "-%dG", staffManager->getPay());
            lablePay->setString(lib->m_strTemp);
            lablePay->setVisible(true);
            sprChecker->setVisible(true);

        }
        else
        {
            lablePay->setVisible(false);
            sprChecker->setVisible(false);
        }
    }
}


void DyLayerGame::onSelectStaff(CCObject * pSender)
{
    CCMenuItemImage *itemImage = dynamic_cast<CCMenuItemImage*>(pSender);
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    
    if(itemImage->getTag() > 0 && dataManager->m_private.m_staff_ext_grade[itemImage->getTag()-1] == -1)
    {
        DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Myroom, 2, 0);
        YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
        return;
    }
    
    
    
    dataManager->m_private.m_current_staff = itemImage->getTag();
    
    setCashier(false);
}


void DyLayerGame::onStaffClicked(CCObject * pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    if(!DyTycoonManager::sharedTycoonManager()->m_gameSetting)
        return;


    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(staffManager->hasStaff())
    {
        if(!staffManager->isHire() && staffManager->getPay() > moneyManager->getGold())
        {
            DyMoneyManager::sharedMoneyManager()->alramNotEnoughGold(true, true);
        }
        else
        {
            if(staffManager->isIdle())
            {
                DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
                
                popup->setText(staffManager->getReason().c_str());
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
                
            }
            else
            {
                staffManager->setHire(!staffManager->isHire());
                buildCashierButton();
            }
        }
    }
    else
    {
		DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
        
        popup->setText(staffManager->m_staff[dataManager->m_private.m_current_staff]->getValue("condition_explain", 0));
		popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
        
		YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
}


void DyLayerGame::onPromoteClicked(CCObject * pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    if(!DyTycoonManager::sharedTycoonManager()->m_gameSetting)
        return;
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(staffManager->hasStaff())
    {
        DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Myroom, 2, dataManager->m_private.m_current_staff+1);
        YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
    }
    else
    {
		DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
        
		popup->setText(staffManager->m_staff[dataManager->m_private.m_current_staff]->getValue("condition_explain", 0));
		popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
        
		YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }

}



void DyLayerGame::onMachineClicked(CCObject * pSender)
{
    DyMachineManager* machineManager = DyMachineManager::sharedMachineManager();
    
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    if(!DyTycoonManager::sharedTycoonManager()->m_gameSetting)
        return;


    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    
    if(machineManager->getMachineLevel() == MC_MACHINE_MAX_LEVEL)
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
        
		popup->setText("업그레이드를 모두 완료했어요.");
		popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
        
		YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);

    }
    else
    {
        DyLayerGameStartMachine* layerGameStartMachine = DyLayerGameStartMachine::create(this);
        YtLayerManager::sharedLayerManager()->pushLayer(layerGameStartMachine, true, true);
    }
}


void DyLayerGame::onItemSlotClicked(CCObject * pSender)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

    if(tycoonManager->m_lockInput)
        return;
    
    if(tycoonManager->m_gameSetting)
    {
        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
        DyLayerGameStartItem* layerGameStartItem = DyLayerGameStartItem::create(this);
        YtLayerManager::sharedLayerManager()->pushLayer(layerGameStartItem, true, true);
        return;
    }

    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    DyTutorial* tutorial = tutorialManager->getCurrentTutorial();
    
	CCMenuItemImage* pMenu = dynamic_cast<CCMenuItemImage*>(pSender);
	int indexSlot = pMenu->getTag()-GAME_UI_TAG_MENUITEM_SLOT0;

    if(!tycoonManager->m_enableSlotItem[indexSlot])
        return;

	
    DyItemManager* itemManager = DyItemManager::sharedItemManager();
	DyItem* item = itemManager->m_itemslot[indexSlot];

    if(tutorial)
    {
        if(tutorial->m_command_type != DyTutorialCommandType_UseItem)
            return;
        if(item->m_itemtype != tutorial->m_paramInt0)
            return;
    }

	if(item->m_itemtype == DyItemType_Bomb)
	{
		if(m_layerGameBoard->m_board->m_isBombSelectMode)
		{
			m_layerGameBoard->m_board->bombUnReady();
			return;
		}
	}
	m_layerGameBoard->m_board->bombEnable(true);
	m_layerGameBoard->m_board->bombUnReady();

	if(m_layerGameBoard->m_board->m_blockSelected)
	{	
		m_layerGameBoard->m_board->m_blockSelected->setAction(m_layerGameBoard->m_board->m_blockSelected->m_posCur.x, m_layerGameBoard->m_board->m_blockSelected->m_posCur.y, TmBlockActionCmd_Unselect);
		m_layerGameBoard->m_board->m_sprCursor->setVisible(false);
		m_layerGameBoard->m_board->m_blockSelected = NULL;	
	}

	if(item->m_cntAvailable > 0)
	{
		YtLib::sharedLib()->playEffect(YtLibSndEff_CLICK);
		switch(item->m_itemtype)
		{
			case DyItemType_Hint:
				if(m_layerGameBoard->m_board->m_blockHighlight == NULL)
				{
					TmCommand* command = new TmCommand(TmCommandType_Hint, m_layerGameBoard->m_board, NULL, NULL, 0, 0);
					m_layerGameBoard->m_board->m_commandManager->addLast(command);
					command->release();
				}
				break;
			case DyItemType_Refresh:
				{
					TmCommand* command = new TmCommand(TmCommandType_Refresh, m_layerGameBoard->m_board, NULL, NULL, 0, 0);
					m_layerGameBoard->m_board->m_commandManager->addLast(command);
					command->release();
				}
				break;
			case DyItemType_TimeIncrease:
				{
					DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddTime, NULL, NULL, NULL, 0, 0, 0);
					tycoonManager->m_commandManager->addLast(commandTycoon);
					commandTycoon->release();
				}
				break;
			case DyItemType_Candy:
				{
					DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_Candy, NULL, NULL, NULL, 0, 0, 0);
					tycoonManager->m_commandManager->addLast(commandTycoon);
					commandTycoon->release();
				}
				break;
			case DyItemType_Bomb:
				{
					TmCommand* command = new TmCommand(TmCommandType_BombReady, m_layerGameBoard->m_board, NULL, NULL, 0, 0);
					m_layerGameBoard->m_board->m_commandManager->addLast(command);
					command->release();
				}
				break;
            default:
                break;
		}
	}
}


void DyLayerGame::pause()
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	//m_layerPause->setVisible(false);
	tycoonManager->m_menuPauseMenu->setEnabled(false);
    tycoonManager->m_menuPauseMenu2->setEnabled(false);
    setKeypadEnabled(false);
    
    if(tycoonManager->m_pauseCheckOpportunity)
        pauseByGaest();
    
    YtPlatform_ShowAd(false);
    
    if(tycoonManager->m_gameSetting)
    {
        ((CCMenu*)getChildByTag(GAME_UI_TAG_MENU_START))->setEnabled(false);
        tycoonManager->m_menuItem->setEnabled(false);
        
        
        CCMenu* menuStaff = (CCMenu*)getChildByTag(GAME_UI_TAG_STAFF_SELECT)->getChildByTag(100);
        menuStaff->setEnabled(false);
        
    }
}


void DyLayerGame::pauseByGaest()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	
    this->pauseSchedulerAndActions();
    m_layerGameBoard->pauseSchedulerAndActions();
    m_layerGameBoard->setTouchEnabled(false);
    tycoonManager->pauseGame();
    
    tycoonManager->m_menuItem->setEnabled(false);
    tycoonManager->m_menuPause->setEnabled(false);
    setKeypadEnabled(true);
    
    m_pausedByGaest = true;
}


void DyLayerGame::resumeByGaest()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	
    this->resumeSchedulerAndActions();
    m_layerGameBoard->resumeSchedulerAndActions();
    m_layerGameBoard->setTouchEnabled(true);
    
    tycoonManager->resumeGame();
    
    tycoonManager->m_menuItem->setEnabled(true);
    tycoonManager->m_menuPause->setEnabled(true);
    
    m_pausedByGaest = false;
}



void DyLayerGame::itemSlotSetting()
{
    YtLib* lib = YtLib::sharedLib();
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
	
    for(unsigned int i = 0;i < DYITEM_SLOT_CNT;i++)
	{
		DyItem* item = itemManager->m_itemslot[i];
        
        CCMenuItemImage* menuItem = (CCMenuItemImage *)tycoonManager->m_menuItem->getChildByTag(GAME_UI_TAG_MENUITEM_SLOT0+i);
        
        getChildByTag(GAME_UI_TAG_DISP_SLOT0_COUNT+i)->setVisible(false);
        
        if(menuItem->getChildByTag(GAME_UI_TAG_ITEM_BUTTON))
            menuItem->removeChildByTag(GAME_UI_TAG_ITEM_BUTTON, true);
        if(menuItem->getChildByTag(GAME_UI_TAG_ITEM_REMOVE))
            menuItem->removeChildByTag(GAME_UI_TAG_ITEM_REMOVE, true);
        
		if(item)
		{
            menuItem->setVisible(true);
			sprintf(lib->m_strTemp, "%d", item->m_cntAvailable);
			item->m_lblAvailable = (CCLabelTTF *)getChildByTag(GAME_UI_TAG_DISP_SLOT0_COUNT+i);
			item->m_lblAvailable->setString(lib->m_strTemp);
            
			item->m_lblAvailable->setVisible(true);
            
			sprintf(lib->m_strTemp, "game_ui_btn_%s.png", item->m_urlIconTag.c_str());
			CCSprite* sprItem = CCSprite::createWithSpriteFrameName(lib->m_strTemp);
			sprintf(lib->m_strTemp, "game_ui_btn_%s_sel.png", item->m_urlIconTag.c_str());
			CCSprite* sprItemSel = CCSprite::createWithSpriteFrameName(lib->m_strTemp);
			sprintf(lib->m_strTemp, "game_ui_btn_%s_disable.png", item->m_urlIconTag.c_str());
			CCSprite* sprItemDisable = CCSprite::createWithSpriteFrameName(lib->m_strTemp);
			menuItem->setNormalSpriteFrame(sprItem->displayFrame());
			menuItem->setSelectedSpriteFrame(sprItemSel->displayFrame());
			menuItem->setDisabledSpriteFrame(sprItemDisable->displayFrame());
            
			if(item->m_cntAvailable == 0)
				menuItem->setEnabled(false);
			else
				menuItem->setEnabled(true);
            
            sprintf(lib->m_strTemp, "game_ui_btn_%s_back.png", item->m_urlIconTag.c_str());
            tycoonManager->m_sprProgItemSlotBg[i]->setDisplayFrame(CCSprite::createWithSpriteFrameName(lib->m_strTemp)->displayFrame());
            tycoonManager->m_sprProgItemSlotBg[i]->setVisible(true);
            
            sprintf(lib->m_strTemp, "game_ui_btn_%s.png", item->m_urlIconTag.c_str());
            tycoonManager->m_progItemSlot[i]->setSprite(CCSprite::createWithSpriteFrameName(lib->m_strTemp));
            tycoonManager->m_progItemSlot[i]->setVisible(true);
            
            
            sprintf(lib->m_strTemp, "%d", item->m_cntAvailable);
            tycoonManager->m_labelItemSlot[i]->setString(lib->m_strTemp);
            tycoonManager->m_labelItemSlot[i]->setVisible(true);
            
            tycoonManager->m_velItemSlot[i] = item->m_cooltime*(100-tycoonManager->m_functionGame.m_cooling)/100.f;
            
            if(tycoonManager->m_dtItemSlot[i]*100.f/tycoonManager->m_velItemSlot[i] < 100)
            {
                tycoonManager->slotItemEnable(i, false);
            }
            else
            {
                tycoonManager->slotItemEnable(i, true);
            }
            
            if(tycoonManager->m_gameSetting)
            {
                CCSprite* sprOutline = CCSprite::createWithSpriteFrameName("game_ui_item_line.png");
                menuItem->addChild(sprOutline, 0, GAME_UI_TAG_ITEM_BUTTON);
                sprOutline->setPosition(ccp(35, 35));
                
                CCSprite* sprRemove = CCSprite::createWithSpriteFrameName("menu_ui_item_slot_remove.png");
                menuItem->addChild(sprRemove, 0, GAME_UI_TAG_ITEM_REMOVE);
                sprRemove->setPosition(ccp(55, 55));
                menuItem->setEnabled(true);
            }
		}
        else
        {
            if(tycoonManager->m_gameSetting)
            {
                menuItem->setVisible(true);
                menuItem->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("game_ui_item_line.png")->displayFrame());
                menuItem->setSelectedSpriteFrame(CCSprite::createWithSpriteFrameName("game_ui_item_line.png")->displayFrame());
                menuItem->setDisabledSpriteFrame(CCSprite::createWithSpriteFrameName("game_ui_item_line.png")->displayFrame());
            }
            else
                menuItem->setVisible(false);
        }
	}
}


void DyLayerGame::resume()
{
	//YtLib* lib = YtLib::sharedLib();
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

	//DyItemManager* itemManager = DyItemManager::sharedItemManager();
	
	setBlocks();
    
    DyCostumeManager::sharedCostumeManager()->setCostume(tycoonManager->m_openDoggy, &DyDataManager::sharedDataManager()->m_public.m_costume_status, &DyDataManager::sharedDataManager()->m_custume_function);
	

    itemSlotSetting();
    
    
	if(m_isGamePause)
	{
		//m_layerPause->setVisible(true);
		tycoonManager->m_menuPauseMenu->setEnabled(true);
        tycoonManager->m_menuPauseMenu2->setEnabled(true);
        setKeypadEnabled(true);
        
        DyDataManager* dataManager = DyDataManager::sharedDataManager();
        
        if(dataManager->m_display_ad)
            YtPlatform_ShowAd(true);
	}

	if(tycoonManager->m_gameStartForce)
	{	
		tycoonManager->startGameReady();
        tycoonManager->m_gameStartForce = false;
	}
    
    if(m_pausedByGaest)
    {
        resumeByGaest();
    }
    
    
    if(tycoonManager->m_gameSetting)
    {
        DyStageManager* stageManager = DyStageManager::sharedStageManager();
        
        ((CCMenu*)getChildByTag(GAME_UI_TAG_MENU_START))->setEnabled(true);
        tycoonManager->m_menuItem->setEnabled(true);
        setCashier(true);
        DyMachineManager::sharedMachineManager()->setupMachineUI(this, stageManager->m_curShopType);
        DyMachineManager::sharedMachineManager()->setMachineReady(true, (CCLayer*)getChildByTag(GAME_UI_TAG_SETTING_MACHINE), (CCMenuItemImage*)getChildByTag(GAME_UI_TAG_MENU_START)->getChildByTag(2));

        
        CCMenu* menuStaff = (CCMenu*)getChildByTag(GAME_UI_TAG_STAFF_SELECT)->getChildByTag(100);
        menuStaff->setEnabled(true);
        
    }
    
    if(DyStaffManager::sharedStaffManager()->isHire())
    {
        //DyDataManager* dataManager = DyDataManager::sharedDataManager();
        CCLayer* layerCashier = (CCLayer*)getChildByTag(GAME_UI_TAG_CASHIER);
        layerCashier->removeAllChildrenWithCleanup(false);
        if(tycoonManager->m_aniCashier)
            YtAniObject::unloadAniObject(tycoonManager->m_aniCashier);
        tycoonManager->m_aniCashier = DyStaffManager::sharedStaffManager()->getStaffAniSprite(this);
        layerCashier->addChild(tycoonManager->m_aniCashier);
    }
    
    buildDebugCostume();
}


void DyLayerGame::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerGame::show()
{
}


DySceneGame::DySceneGame()
{
}


DySceneGame::~DySceneGame()
{
}


void DySceneGame::pauseByPhone()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

    if(!m_layerGame->m_isGamePause && !tycoonManager->m_pauseCheckOpportunity)
    {
        m_layerGame->onPauseClicked(NULL);
    }
}


void DySceneGame::resumeByPhone()
{
    
}


void DySceneGame::didSceneChanged()
{
	m_layerGame = (DyLayerGame*)YtLayerManager::sharedLayerManager()->getTopLayer();
   
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

	DyLayerGameStart* layerGameStart = DyLayerGameStart::create(this);
	YtLayerManager::sharedLayerManager()->pushLayer(layerGameStart, true, true);
 
                
//	DyLayerGameResult* layerGameResult = DyLayerGameResult::create(this);
//    layerGameResult->m_curStageState = DyStageState_Fail;
//    layerGameResult->m_failWithQuest = true;
//	YtLayerManager::sharedLayerManager()->pushLayer(layerGameResult, true);

	tycoonManager->m_menuItem->setEnabled(false);
	tycoonManager->m_menuPause->setEnabled(false);
}
