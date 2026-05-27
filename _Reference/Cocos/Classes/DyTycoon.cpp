/*
DyTycoon.cpp
Doggy chef
@initialize at 121008

@history
130309  yoonsr  quest 추가(ITEM, ETC)
121008	yoonsr	initialize
 
 
QUEST

1. 손님 몇명 만족
GUEST,ALL,X
GUEST,동물,X

2. 콤보%회달성 1회 달성
COMBO,5,X
COMBO,10,X
COMBO,20,X
COMBO,30,X

3. 불아이템사용 1회 이상(콤보를 10번 달성해 불 아이템을 사용해 보세요)
BLOCK,FIRE,X
BLOCK,CANDY,X
BLOCK,MSG,X
BLOCK,HALO,X
BLOCK,CROSS,X

4. 서브메뉴 팔기 1회 이상(손님에게 서브메뉴를 팔아요(1회), 10회, 모든 손님)
FOOD,SIDE,X
FOOD,ddukbokki,X

FOOD,kimbob,X <-- 기본 재료면 기본 재료 모두 포함(치즈김밥)
 
 
5. 상해버린 음식깨기
REMOVE,DECAY,20
REMOVE,ICE,20

 
 
6. 아이템
ITEM,BASIC,1
ITEM,candy,1
ITEM,bomb,1
 
7. 빠짐없이 돈받기
ETC,GOLD,0

 
 
1. Matching(3단매치 확률)
- 얼음 나올 확률(100%)		↑
- 썩는정도				↑
- 나오는 블럭수(단무지)		↑


2.Order(주문을 완성할 확률)
- 이중주문확률				↑
- 한주문의 재료수 평균		↑


3.Recipe 주문을 완성했을때 들어오는 돈의 평균
- 보유 레시피(gold)		↓


4.Time
- 시간					↓


5. Goal
- 목표골드					↑


6. Perfect
- 퍼펙트 골드(퀘스트 골드 + 알파 + 목표 골드)					↑


7. Animal
- 출현동물 heart 평균 수		↓


8. Quest
- 퀘스트 난이도				↑


[공식]
#스테이지 goal 난이도(s_goal) = Matching + Order + Recipe + Time + Goal + Animal
#스테이지 perfect 난이도(s_perfect) = s_goal + Quest + perfect

[원칙]
- 아이템을 하나도 사용 안했을 때 겨우 goal 할 수 있도록
- 살 수 있는 레시피가 있다는 가정하에 goal 할 수 있도록 반영
- 전 스테이지를 3~5번 해야 이번 스테이지 goal를 획득 가능
- 시크릿은 goal에서 무시

*/


#include "DyTycoon.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "DyCustomer.h"
#include "DyBlockInfo.h"

#include "DyLoader.h"
#include "TmBlock.h"
#include "DyTycoonCommand.h"
#include "DySceneGame.h"

#include "DyItem.h"


#include "CCextAnimatePacker.h"
#include "DyLib.h"
#include "DyMoney.h"
#include "DyFriend.h"
#include "DyTutorial.h"
#include "DyGuest.h"
#include "DyRank.h"
#include "DyPopup.h"
#include "DyCostume.h"
#include "DyMachine.h"
//#define APPLY_MATERIAL_CNT


DyTycoonManager* g_sharedTycoonManager;

DyTycoonManager::DyTycoonManager()
{
	m_commandManager = new YtCommandManager();
	m_quest = NULL;
	
    
    for(int i = 0;i < MAX_SLOT;i++)
	{
		m_indicatorCharacter[i] = NULL;
		m_indicatorDish[i] = NULL;
	}

	m_openDoggy = NULL;
	m_questbar = NULL;

	m_isReplay = false;
    m_gameStartForce = false;
    
    m_checkLastOpportunity = false;
    m_pauseCheckOpportunity = false;
    
    m_crazy_create_gold_snack = 150;
    m_crazy_create_gold_hamburger = 200;
    m_crazy_create_gold_coffee = 300;
    m_crazy_create_gold_china = 350;
    
    m_crazy_create_start_gold = 5000;
    
    m_crazy_create_delly_snack_start = 240;
    m_crazy_create_delly_snack_interval = 60;
  
    m_crazy_create_delly_hamburger_start = 210;
    m_crazy_create_delly_hamburger_interval = 60;
    
    m_crazy_create_delly_coffee_start = 180;
    m_crazy_create_delly_coffee_interval = 60;
    
    m_crazy_create_delly_china_start = 180;
    m_crazy_create_delly_china_interval = 60;

    
    m_crazy_create_delly_first_pro = 10;
    m_crazy_create_delly_second_pro = 20;
    m_crazy_create_delly_etc_pro = 30;
}


DyTycoonManager::~DyTycoonManager()
{
	destoryGameObject();

	m_commandManager->release();
	
	if(m_quest)
    {
		m_quest->release();
    }
}


void DyTycoonManager::setLayerAndBoard(CCLayer* layer, DyBoard* board)
{
	m_layer = layer;
	m_board = board;
}




//
// 주문 레시피 세팅
void DyTycoonManager::setOrderRecipe(DyCustomer* customer)
{
	DyRecipeManager* recipeManager = DyRecipeManager::sharedRecipeManager();
	DyStageManager* stageManager = DyStageManager::sharedStageManager();

    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
	customer->m_arrOrderRecipe->removeAllObjects();
    
    if(customer->m_customerType == DyCustomerType_Gaest)
    {
        int sell_price = 0;
        DyRecipeInfo* recipe_max;
        
        for(int i = 0;i < recipeManager->m_arrAvailableRecipes->count();i++)
        {
            DyRecipeInfo* recipe = (DyRecipeInfo*)recipeManager->m_arrAvailableRecipes->objectAtIndex(i);
            
            if(sell_price < recipe->m_selling_price)
            {
                sell_price = recipe->m_selling_price;
                recipe_max = recipe;
            }
        }
        
        DyOrderRecipe* orderRecipe = new DyOrderRecipe(recipe_max);
        customer->m_arrOrderRecipe->addObject(orderRecipe);
        orderRecipe->release();
        
        if(m_isCrazyMode)
            customer->m_pay += orderRecipe->m_info->m_selling_price * ((float)orderRecipe->m_info->m_crazy_mux/100.f);
        else
            customer->m_pay += orderRecipe->m_info->m_selling_price;
    }
    else
    {

        //
        // 레시피 갯수 결정
        int creatable_cnt = 1;

        int probability_cur = rand()%100;
        int probability_one = m_association[0];
        int probability_two = m_association[1];

        if(probability_cur < probability_one)
            creatable_cnt = 1;
        else if(probability_cur < probability_one+probability_two)
            creatable_cnt = 2;
        else
            creatable_cnt = 3;

        //
        // 조합 재료 갯수 결정
#ifdef APPLY_MATERIAL_CNT
        int material_cnt = 1;
        int probailityTotal[DYRECIPE_MATERIAL_MAX_CNT];
        for(int i = 0;i < DYRECIPE_MATERIAL_MAX_CNT;i++)
            probailityTotal[i] = 0;
        for(int i = 0;i < DYRECIPE_MATERIAL_MAX_CNT;i++)
        {
            for(int j = 0;j <= i;j++)
            {
                probailityTotal[i] += m_material_probability[j];
            }
        }
        probability_cur = rand()%100;
        for(int i = 0;i < DYRECIPE_MATERIAL_MAX_CNT;i++)
        {
            if(probability_cur <= probailityTotal[i])
            {
                material_cnt = i+1;
                break;
            }
        }
#else
        int material_cnt = m_max_block_count;
        
#endif
    
        CCArray* recipes = stageManager->m_curShopInfo->getArray("order_recipe", stageManager->m_curLevel);
     
        CCArray* repice_frequence = stageManager->m_curShopInfo->getArray("recipe_frequency", stageManager->m_curLevel);
        
        DyRecipeInfo* recipe = NULL;
        
        
        const char* strRecipeFrequency = NULL;
        
        if(!m_isCrazyMode && strcmp(((CCString*)repice_frequence->objectAtIndex(0))->getCString(), "NULL") != 0)
        {
            int count = repice_frequence->count();
            int index = (rand()%(count-1)) + 1;
            
            int frequence = atoi(((CCString*)repice_frequence->objectAtIndex(0))->getCString());
            
            strRecipeFrequency = ((CCString*)repice_frequence->objectAtIndex(index))->getCString();
            
            if(recipeManager->m_removeCakeType == 0)// 케이크를 제거
            {
                if(strcmp(strRecipeFrequency, "cake") > 0)
                {
                    if(count > 2)
                    {
                        if(index == 1) index = 2;
                        else if(index == 2) index = 1;
                        
                        strRecipeFrequency = ((CCString*)repice_frequence->objectAtIndex(index))->getCString();
                    }
                }
            }
            if(recipeManager->m_removeCakeType == 1)// 머핀 제거
            {
                if(strcmp(strRecipeFrequency, "muff") > 0)
                {
                    if(count > 2)
                    {
                        if(index == 1) index = 2;
                        else if(index == 2) index = 1;
                        
                        strRecipeFrequency = ((CCString*)repice_frequence->objectAtIndex(index))->getCString();
                    }
                }
 
            }
            if(m_indexRecipe%frequence == frequence-1)
            {
                
                recipe = recipeManager->getRecipe(strRecipeFrequency);
                
                if(material_cnt < recipe->m_arrMaterial->count())
                    material_cnt = recipe->m_arrMaterial->count();
            }
            //CCLog("strRecipeFrequency = %s , %d, %d", strRecipeFrequency, m_indexRecipe, frequence);
        }
        
        
        int try_cnt = 0;
        while(customer->m_arrOrderRecipe->count() < (unsigned int)creatable_cnt)
        {
            if(try_cnt++ > 100)
                break;
            
            bool success = true;
            
            //
            // 레시피 선택
            if(recipe == NULL)
            {
                int recipe_prob = rand()%100;
                int recipe_prob_cur = 0;
                int recipe_index;
                for(recipe_index = 0;recipe_index < recipes->count()/2;recipe_index++)
                {
                    recipe_prob_cur += atoi(((CCString*)recipes->objectAtIndex(recipe_index*2+1))->getCString());
                    if(recipe_prob_cur > recipe_prob)
                    {
                        break;
                    }
                }
                
                CCString* strRecipe = (CCString*)recipes->objectAtIndex(recipe_index*2);
                if(strRecipe->compare("ALL") == 0 || m_isCrazyMode)
                {
                    while(recipe == NULL)
                    {
                        int index = rand()%recipeManager->m_arrWantedRecipe->count();
                        recipe = (DyRecipeInfo*)recipeManager->m_arrWantedRecipe->objectAtIndex(index);
                        
                        for(int recipe_check_idx = 0;recipe_check_idx < recipes->count()/2;recipe_check_idx++)
                        {
                            CCString* _strRecipe = (CCString*)recipes->objectAtIndex(recipe_check_idx*2);
                            
                            if(_strRecipe->compare(recipe->m_tag.c_str()) == 0)
                            {
                                recipe = NULL;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    recipe = recipeManager->getRecipe(strRecipe->getCString());
                }
                
                
                if(tutorial && (tutorial->m_tutorial_type == DyTutorialType_OrderRecipe || tutorial->m_tutorial_type == DyTutorialType_OrderRecipeByDelivery))
                {
                    recipe = (DyRecipeInfo*)recipeManager->m_arrWantedRecipe->objectAtIndex(tutorial->m_paramInt0);
                    DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
                }
                if(strRecipeFrequency)
                {
                    int count = repice_frequence->count();
                    
                    for(int i = 1;i < count;i++)
                    {
                        const char* _strRecipeFrequency = ((CCString*)repice_frequence->objectAtIndex(i))->getCString();
                        
                        if(strcmp(_strRecipeFrequency, recipe->m_tag.c_str()) == 0)
                        {
                            success = false;
                        }
                    }
                }   
            }
            
            
            //

            //
            // 메인 레시피랑 같으면 다시 생성
            if(success)
            {
                for(unsigned int i = 0;i < customer->m_arrOrderRecipe->count();i++)
                {
                    DyOrderRecipe* orderRecipe = (DyOrderRecipe*)customer->m_arrOrderRecipe->objectAtIndex(i);

                    if(orderRecipe->m_info->m_mainMaterial.compare(recipe->m_mainMaterial) == 0)
                    {
                        success = false;
                    }
                    
                }

            }
            //
            // 레시피 생성 성공
            if(success)
            {
                DyOrderRecipe* orderRecipe = new DyOrderRecipe(recipe);

                
                CCLog("주문음식 : %s", orderRecipe->m_info->m_name.c_str());
                
                
                customer->m_arrOrderRecipe->addObject(orderRecipe);
                orderRecipe->release();
                
                int material_cur_cnt = customer->getOrderMaterialCnt();
                // 원하는 재료 갯수 초과
                if(material_cur_cnt > material_cnt || material_cur_cnt > m_max_block_count)
                {
                    customer->m_arrOrderRecipe->removeObject(orderRecipe);
                }
                else
                {
                    if(m_isCrazyMode)
                        customer->m_pay += orderRecipe->m_info->m_selling_price * ((float)orderRecipe->m_info->m_crazy_mux/100.f);
                    else
                        customer->m_pay += orderRecipe->m_info->m_selling_price;
                    
                }
                
                
            }
            recipe = NULL;
        }
        
        
        if(customer->m_arrOrderRecipe->count() == 0)
        {
            setOrderRecipe(customer);
            return;
        }
        
        if(customer->m_arrOrderRecipe->count() == 1)
        {
            DyOrderRecipe* orderRecipe = (DyOrderRecipe*)customer->m_arrOrderRecipe->objectAtIndex(0);
            
            if(orderRecipe->m_info->m_tag.compare("bob") == 0)
            {
                setOrderRecipe(customer);
                return;

            }
        }

    }
    
    
	customer->addPay(0, 1);
	customer->updateTotalMaterial(false);
	//customer->m_character->setOrders(customer->m_arrOrderRecipe);
    m_indexRecipe++;
}



void DyTycoonManager::updateIndicator()
{
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
	for(int i = 0;i < 3;i++)
	{
		m_indicatorDish[i]->setVisible(false);
		m_indicatorCharacter[i]->setVisible(false);
	}

	for(unsigned int i = 0;i < MAX_SLOT;i++)
	{
		DyCustomer* customer = customerManager->m_arrCustomer[i];
		if(customer == NULL)
			continue;

		if(customer->m_character->m_status != DyCharacterStatus_Order)
			continue;

		if(m_board->m_blockSelected && m_board->m_blockSelected->m_blocktype == TmBlockType_Candy)
		{
            if(customer->m_customerType != DyCustomerType_Delivery)
                m_indicatorCharacter[customer->m_slotidx]->setVisible(true);
		}
		
		if(m_board->m_blockSelected && m_board->m_blockSelected->m_blocktype == TmBlockType_MSG)
		{
			m_indicatorDish[customer->m_slotidx]->setVisible(true);
		}

		if(m_isSelectedSide && !customer->m_hasSide)
		{
			m_indicatorDish[customer->m_slotidx]->setVisible(true);
		}	
	}
}


				
//
// 0:touch, 1:by cashier, 2:by bonus, 
void DyTycoonManager::earnGold(CCPoint posGold, int customer_pay, int customer_tip, int cashier_pay, int type)
{
    int gold = customer_pay + customer_tip - cashier_pay;
    
    m_earnGold++;
    
    CCInteger* pay = CCInteger::create(gold);
    
	pay->retain();
	CCSprite* sprGold = CCSprite::createWithSpriteFrameName("menu_ui_gold.png");
	sprGold->setPosition(posGold);
	m_layer->addChild(sprGold, ZORDER_GAME_MOVE_OBJECT);
	sprGold->setUserData(pay);
    
    
    CCPoint posTargetGoldBar = m_layer->getChildByTag(GAME_UI_TAG_DISP_GOLD_ICON)->getPosition();
    
    if(m_isCrazyMode)
        posTargetGoldBar = ccp(245, 585);
    
    switch(type)
    {
        case 0: // by touch
            {
                YtLib::sharedLib()->playEffect(YtLibSndEff_MONEY_REGISTER);
                
                float distance = ccpDistance(sprGold->getPosition(), posTargetGoldBar);
                CCActionInterval* moveto = CCMoveTo::create(0.001f*distance, posTargetGoldBar);
                
                CCCallFunc* callfun = CCCallFuncN::create(m_layer, callfuncN_selector(DyTycoonManager::endAnimationGold));
                
                sprGold->runAction(CCSequence::create(moveto, callfun, NULL));
                sprGold->runAction(CCSequence::create(CCFadeTo::create(0.001f*distance, 0), NULL));
                
                displayEarnGold(0, posGold, customer_pay, customer_tip);
            }
            break;
        case 1: // by cashier
            {
                CCPoint posCashier = m_layer->getChildByTag(GAME_UI_TAG_CASHIER)->getPosition();
            
                CCPoint posTargetCashier = ccp(posCashier.x-10, posCashier.y+50);
                float distance_cashier = ccpDistance(sprGold->getPosition(), posTargetCashier);
                CCActionInterval* moveto_cashier = CCMoveTo::create(0.0007f*distance_cashier, posTargetCashier);
                CCCallFunc* callfun_cashier = CCCallFuncN::create(m_layer, callfuncN_selector(DyTycoonManager::endAnimationCashier));
                
                float distance_bar = ccpDistance(posTargetCashier, posTargetGoldBar);
                CCActionInterval* moveto_bar = CCMoveTo::create(0.001f*distance_bar, posTargetGoldBar);
                //CCActionInterval* move_ease_in_bar = CCEaseIn::create(moveto_bar, 0.3f);
                CCCallFunc* callfun_bar = CCCallFuncN::create(m_layer, callfuncN_selector(DyTycoonManager::endAnimationGold));
                
                CCFiniteTimeAction *seq = CCSequence::create(moveto_cashier, callfun_cashier, CCDelayTime::create(0.3f), moveto_bar, callfun_bar, NULL);
                sprGold->runAction(seq);
                sprGold->runAction(CCSequence::create(CCDelayTime::create(0.0007f*distance_cashier + 0.3f), CCFadeTo::create(0.001f*distance_bar, 0), NULL));
                sprGold->runAction(CCSequence::create(CCScaleTo::create(0.0007f*distance_cashier + 0.3f, 0.3f), CCScaleTo::create(0.001f*distance_bar, 1.0f), NULL));
                
                displayEarnGold(0.5, ccp(posTargetCashier.x-50, posTargetCashier.y), customer_pay, customer_tip);
            }
            break;
        case 2:
        {
            float distance = ccpDistance(sprGold->getPosition(), posTargetGoldBar);
            CCActionInterval* moveto = CCMoveTo::create(0.001f*distance, posTargetGoldBar);
            
            CCCallFunc* callfun = CCCallFuncN::create(m_layer, callfuncN_selector(DyTycoonManager::endAnimationGold));
            
            sprGold->runAction(CCSequence::create(moveto, callfun, NULL));
            sprGold->runAction(CCSequence::create(CCFadeTo::create(0.001f*distance, 0), NULL));
            
            displayEarnGold(0, posGold, customer_pay, 0);
            break;
        }
            
    }
}


void DyTycoonManager::endAnimationGoldCrazy(CCNode* node)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
	CCInteger* pay = (CCInteger*)node->getUserData();
    
	DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddGoldCrazyReal, NULL, NULL, NULL, pay->getValue(), 0, 0);
	tycoonManager->m_commandManager->addLast(commandTycoon);
	commandTycoon->release();
    
	pay->release();
    
	node->removeFromParentAndCleanup(true);
    
    //
    // 골드가 붙는 애니메이션
    CCSprite* sprGold = (CCSprite*)tycoonManager->m_layer->getChildByTag(GAME_UI_TAG_DISP_GOLD_ICON);
    CCSprite* sprGlow = (CCSprite*)sprGold->getChildByTag(GAME_UI_TAG_DISP_GOLD_GLOW);
    
    sprGlow->setOpacity(0);
    sprGold->setScale(1.0f);
    sprGold->runAction(CCSequence::create(
                                          CCScaleTo::create(0.4f, 1.2f),
                                          CCScaleTo::create(0.4f, 1.0f),
                                          NULL
                                          ));
    sprGlow->runAction(CCSequence::create(
                                          CCFadeIn::create(0.4f),
                                          CCFadeOut::create(0.4f),
                                          NULL
                                          ));
}


void DyTycoonManager::endAnimationDellyCrazy(CCNode* node)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
	CCInteger* pay = (CCInteger*)node->getUserData();
    
	DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddDellyCrazyReal, NULL, NULL, NULL, pay->getValue(), 0, 0);
	tycoonManager->m_commandManager->addLast(commandTycoon);
	commandTycoon->release();
    
	pay->release();
    
	node->removeFromParentAndCleanup(true);
    
    //
    // 골드가 붙는 애니메이션
    CCSprite* sprGold = (CCSprite*)tycoonManager->m_layer->getChildByTag(GAME_UI_TAG_DISP_DELLY_ICON);
    CCSprite* sprGlow = (CCSprite*)sprGold->getChildByTag(GAME_UI_TAG_DISP_DELLY_GLOW);
    
    sprGlow->setOpacity(0);
    sprGold->setScale(1.0f);
    sprGold->runAction(CCSequence::create(
                                          CCScaleTo::create(0.4f, 1.2f),
                                          CCScaleTo::create(0.4f, 1.0f),
                                          NULL
                                          ));
    sprGlow->runAction(CCSequence::create(
                                          CCFadeIn::create(0.4f),
                                          CCFadeOut::create(0.4f),
                                          NULL
                                          ));
}



void DyTycoonManager::earnGoldCrazy(CCPoint posGold, int gold)
{
    CCInteger* pay = CCInteger::create(gold);
	pay->retain();
	CCSprite* sprGold = CCSprite::createWithSpriteFrameName("menu_ui_gold.png");
	sprGold->setPosition(posGold);
	m_layer->addChild(sprGold, ZORDER_GAME_MOVE_OBJECT);
	sprGold->setUserData(pay);
    
    CCPoint posTargetGoldBar = m_layer->getChildByTag(GAME_UI_TAG_DISP_GOLD_ICON)->getPosition();
    
    float distance = ccpDistance(sprGold->getPosition(), posTargetGoldBar);
    CCActionInterval* moveto = CCMoveTo::create(0.001f*distance, posTargetGoldBar);
    
    CCCallFunc* callfun = CCCallFuncN::create(m_layer, callfuncN_selector(DyTycoonManager::endAnimationGoldCrazy));
    
    sprGold->runAction(CCSequence::create(moveto, callfun, NULL));
    sprGold->runAction(CCSequence::create(CCFadeTo::create(0.001f*distance, 0), NULL));
    
    displayEarnGold(0, posGold, gold, 0);
}



void DyTycoonManager::earnDellyCrazy(CCPoint posGold, int delly)
{
    CCInteger* pay = CCInteger::create(delly);
	pay->retain();
	CCSprite* sprGold = CCSprite::createWithSpriteFrameName("crazy_pannel_top_crazymode_mydelly.png");
	sprGold->setPosition(posGold);
	m_layer->addChild(sprGold, ZORDER_GAME_MOVE_OBJECT);
	sprGold->setUserData(pay);
    
    CCPoint posTargetGoldBar = m_layer->getChildByTag(GAME_UI_TAG_DISP_DELLY_ICON)->getPosition();
    
    float distance = ccpDistance(sprGold->getPosition(), posTargetGoldBar);
    CCActionInterval* moveto = CCMoveTo::create(0.001f*distance, posTargetGoldBar);
    
    CCCallFunc* callfun = CCCallFuncN::create(m_layer, callfuncN_selector(DyTycoonManager::endAnimationDellyCrazy));
    
    sprGold->runAction(CCSequence::create(moveto, callfun, NULL));
    sprGold->runAction(CCSequence::create(CCFadeTo::create(0.001f*distance, 0), NULL));

}


void DyTycoonManager::endAnimationHeartCrazy(CCNode* node)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    
	DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddHeartCrazyReal, NULL, NULL, NULL, 0, 0, 0);
	tycoonManager->m_commandManager->addLast(commandTycoon);
	commandTycoon->release();
    
	node->removeFromParentAndCleanup(true);
    
//    //
//    // 골드가 붙는 애니메이션
//    CCSprite* sprGold = (CCSprite*)tycoonManager->m_layer->getChildByTag(GAME_UI_TAG_DISP_DELLY_ICON);
//    CCSprite* sprGlow = (CCSprite*)sprGold->getChildByTag(GAME_UI_TAG_DISP_DELLY_GLOW);
//    
//    sprGlow->setOpacity(0);
//    sprGold->setScale(1.0f);
//    sprGold->runAction(CCSequence::create(
//                                          CCScaleTo::create(0.4f, 1.2f),
//                                          CCScaleTo::create(0.4f, 1.0f),
//                                          NULL
//                                          ));
//    sprGlow->runAction(CCSequence::create(
//                                          CCFadeIn::create(0.4f),
//                                          CCFadeOut::create(0.4f),
//                                          NULL
//                                          ));
}


void DyTycoonManager::earnHeartCrazy(CCPoint posHeart)
{
   CCSprite* sprGold = CCSprite::createWithSpriteFrameName("crazy_pannel_top_crazymode_life_on.png");
	sprGold->setPosition(posHeart);
	m_layer->addChild(sprGold, ZORDER_GAME_MOVE_OBJECT);
	
    CCPoint posTargetGoldBar = ccp(m_sprPanHeart->getPosition().x + m_sprCrazyHeart[m_crazyRemainHeart]->getPosition().x,
                                   m_sprPanHeart->getPosition().y + m_sprCrazyHeart[m_crazyRemainHeart]->getPosition().y);
    
    float distance = ccpDistance(sprGold->getPosition(), posTargetGoldBar);
    CCActionInterval* moveto = CCMoveTo::create(0.001f*distance, posTargetGoldBar);
    
    CCCallFunc* callfun = CCCallFuncN::create(m_layer, callfuncN_selector(DyTycoonManager::endAnimationHeartCrazy));
    
    sprGold->runAction(CCSequence::create(moveto, callfun, NULL));
    sprGold->runAction(CCSequence::create(CCFadeTo::create(0.001f*distance, 0), NULL));
}




void DyTycoonManager::endEffect(CCNode* node)
{
	node->removeFromParentAndCleanup(true);
}




CCLabelTTF* DyTycoonManager::getDisplayEarnGoldLabel(const char* text, float fontSize, CCPoint pos, float delay)
{
	CCLabelTTF* labelDon = CCLabelTTF::create(text, FONT_NAME_DEFAULT, fontSize);
  	labelDon->setColor(ccWHITE);
	labelDon->setPosition(pos);
    labelDon->setOpacity(0);
    
	labelDon->runAction(CCSequence::create(CCDelayTime::create(delay), CCFadeIn::create(.01f), CCMoveTo::create(1.3f, ccp(pos.x, pos.y + 80)), NULL));
	labelDon->runAction(CCSequence::create(CCDelayTime::create(1.f+delay), CCFadeOut::create(.3f), CCCallFuncN::create(this, callfuncN_selector(DyTycoonManager::endEffect)),
                                           NULL));
    
    
    for(int i = 0;i < 4;i++)
    {
       	CCLabelTTF* labelDonBack = CCLabelTTF::create(text, FONT_NAME_DEFAULT, fontSize);
        labelDonBack->setColor(ccBLACK);
        
        switch(i)
        {
            case 0:
                labelDonBack->setPosition(ccp(labelDonBack->getContentSize().width/2-1, labelDonBack->getContentSize().height/2-1));
                break;
            case 1:
                labelDonBack->setPosition(ccp(labelDonBack->getContentSize().width/2-1, labelDonBack->getContentSize().height/2+1));
                break;
            case 2:
                labelDonBack->setPosition(ccp(labelDonBack->getContentSize().width/2+1, labelDonBack->getContentSize().height/2-1));
                break;
            case 3:
                labelDonBack->setPosition(ccp(labelDonBack->getContentSize().width/2+1, labelDonBack->getContentSize().height/2+1));
                break;
                
        }
        
        labelDonBack->setOpacity(0);
        
        labelDonBack->runAction(CCSequence::create(CCDelayTime::create(delay), CCFadeTo::create(.01f, 102), CCDelayTime::create(.8f), CCFadeTo::create(.3f, 0), CCCallFuncN::create(this, callfuncN_selector(DyTycoonManager::endEffect)),
                                                   NULL));
        
        labelDon->addChild(labelDonBack, -1);
 
    }
    return labelDon;
}


void DyTycoonManager::displayEarnGold(float delay, CCPoint pos, int mainGold, int subGold)
{
    YtLib* lib = YtLib::sharedLib();
	
    sprintf(lib->m_strTemp, "%d", mainGold);
	CCLabelTTF* labelDon = getDisplayEarnGoldLabel(lib->m_strTemp, 25.f, pos, delay);
    if(subGold == 0)
        labelDon->setAnchorPoint(ccp(0.5, 0.5));
    else
        labelDon->setAnchorPoint(ccp(1.0, 0.5));
	m_layer->addChild(labelDon, ZORDER_GAME_GAME_INDICATOR);
    
	if(subGold > 0)
	{
		sprintf(lib->m_strTemp, "+%d", subGold);
		CCLabelTTF* labelTip = getDisplayEarnGoldLabel(lib->m_strTemp, 20.f, pos, delay);
		labelTip->setAnchorPoint(ccp(0, 0.5));
		m_layer->addChild(labelTip, ZORDER_GAME_GAME_INDICATOR);
	}
}


void DyTycoonManager::sellRecipe(DyCustomer* customer, bool byChshier)
{
	//CCLayer* layerDon = (CCLayer*)m_layer->getChildByTag(GAME_UI_TAG_SLOT_PAY_0 + customer->m_slotidx);

//	DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
	int price = customer->m_pay*customer->m_payMux;

	int cashier = 0;
	if(byChshier)
		cashier = (int)(price*staffManager->getProfitShare());

    //
    // 계산
	m_sold_gold += price;
	m_tip_gold += customer->m_payTip;
	m_cashier_gold += cashier;

    
	customer->m_character->m_layerTable->stopAllActions();
	customer->m_character->m_layerTable->setVisible(false);
	
    
    earnGold(ccp(customer->m_character->m_layerTable->getPosition().x + customer->m_character->m_layerTable->getContentSize().width/2,
                customer->m_character->m_layerTable->getPosition().y + customer->m_character->m_layerTable->getContentSize().height/2), price, customer->m_payTip, cashier, byChshier?1:0);
    
	//CCLog("-----------------------------------------------------");
	//CCLog("price = %d, customer->m_payTip = %d, cashier = %d", price, customer->m_payTip, cashier);
	//CCLog("m_sold_gold = %d, m_tip_gold = %d, m_cashier_gold = %d", m_sold_gold, m_tip_gold, m_cashier_gold);

	for(unsigned int i = 0;i < customer->m_arrSellRecipe->count();i++)
	{
		DyRecipeInfo* recipe = (DyRecipeInfo*)customer->m_arrSellRecipe->objectAtIndex(i);
		DyRecipeManager::sharedRecipeManager()->addSellingCount(recipe);

		questProcess(QUESTTAG_FOOD, recipe->m_tag.c_str(), 1);
	}


	if(customer->m_hasSide)
    {
		questProcess(QUESTTAG_FOOD, "SIDE", 1);
    }
    else
    {
        questProcess(QUESTTAG_FOOD, "SIDE", 0);
    }

	customer->m_pay = 0;
	customer->m_character->m_layerTable->removeAllChildrenWithCleanup(true);
    customer->m_character->m_sprPhone = NULL;
}


void DyTycoonManager::onPopupClicked(cocos2d::CCObject * pSender)
{
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
    DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
    
	if(pPopup->m_active_tag == 1)
	{
        DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
        
        if(moneyManager->getDelly() < GEAST_OPPORTUNIITY_DELLY)
        {
            DyMoneyManager::sharedMoneyManager()->alramNotEnoughDelly(true, true);
            m_checkLastOpportunity = false;
        }
        else
        {
            customerManager->m_isHasGuestCustomer = true;
            customerManager->m_isEnteredGuestCustomer = false;
            customerManager->m_isCreateOpportunityGeast = true;
            moneyManager->addDelly(-GEAST_OPPORTUNIITY_DELLY, true);
            m_pauseCheckOpportunity = false;
        }
	}
    else
    {
        m_pauseCheckOpportunity = false;
    }
}


void DyTycoonManager::checkGameEnd()
{
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
    DyLib* lib = DyLib::sharedLib();
    
    if(m_isCrazyMode)
    {
        if(customerManager->m_cntCustomer == 0 && m_earnGold <= 0 && m_crazyRemainHeart == 0 && m_checkLastPang == false)
        {
            m_checkLastPang = true;
            m_lockInput = true;
            m_layer->unschedule(schedule_selector(DyTycoonManager::checkLastPangEnd));
            m_layer->schedule(schedule_selector(DyTycoonManager::checkLastPangEnd), .1f);
        }
    }
    else
    {
        bool isNotNeedGuestEnter = false;
        
        if(customerManager->m_isHasGuestCustomer)
        {
             if(customerManager->m_isEnteredGuestCustomer)
                 isNotNeedGuestEnter = true;
        }
        else
            isNotNeedGuestEnter = true;
        
        if(m_closeTime && customerManager->m_cntCustomer == 0 && m_earnGold <= 0 && isNotNeedGuestEnter)
        {
            //
            // 마지막 개스트
            if(m_checkLastOpportunity == false && (!m_failWithQuest && m_current_gold < m_goal_gold))
            {
                
                m_checkLastOpportunity = true;
                m_pauseCheckOpportunity = true;
                
                DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyTycoonManager::onPopupClicked), CCSizeMake(440, 350));
                
                popup->setPicture(CCSprite::createWithSpriteFrameName("menu_ui_gaest_buy.png"), ccp(0, 70));
                popup->setText(STRING_GAEST_OPPORTUNITY);
                popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
                DyPopupButton* buttonYes = popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 1);
                popup->addButton(DyButtonType_Close, "", 0);
                
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
                
                CCSprite* sprDellyBg = CCSprite::createWithSpriteFrameName("menu2_ui_spend_delly.png");
                sprDellyBg->setPosition(ccp(140, 10));
                buttonYes->m_menuItem->addChild(sprDellyBg);
                
                sprintf(lib->m_strTemp, "%d", GEAST_OPPORTUNIITY_DELLY);
                CCLabelTTF* labelDelly = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 22);
                labelDelly->setColor(ccBLACK);
                labelDelly->setPosition(ccp(31,13));
                sprDellyBg->addChild(labelDelly);
                
                YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_MSG);
                return;
            }
            else if(m_pauseCheckOpportunity == false && m_checkLastPang == false)
            {
                m_checkLastPang = true;
                m_lockInput = true;
                m_layer->unschedule(schedule_selector(DyTycoonManager::checkLastPangEnd));
                m_layer->schedule(schedule_selector(DyTycoonManager::checkLastPangEnd), .1f);
                return;
            }
            
            //미션이 성공인경우 돈이 추가되면서 다시 checkGameEnd()을 호출하게 된다.
            showQuestMessage(true);
        }
    }
}


void DyTycoonManager::updateSide(float dt)
{
    DyMachineManager* machineManager = DyMachineManager::sharedMachineManager();
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
    float timeFullSide = machineManager->getFullTime();
    
	m_dtSide += dt;
    
    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    DyTutorial* tutorial = tutorialManager->getCurrentTutorial();
    
    if(tutorial && tutorial->m_command_type == DyTutorialCommandType_SideMenuClick)
    {
        m_dtSide = timeFullSide+10;
    }
 
  	if(m_dtSide > timeFullSide)
	{
		if(m_isCompleteSide == false)
		{
            machineManager->sideReadyUI();            
            m_dtSideReady = 0;
		}
        
		m_isCompleteSide = true;
		m_dtSide = timeFullSide;
        
	}
        
	if(m_dtSide*100/timeFullSide > 99)
	{
        machineManager->rampUI(3);
	}
	else if(m_dtSide*100/timeFullSide >= 66)
	{
        machineManager->rampUI(2);
	}
	else if(m_dtSide*100/timeFullSide >= 33)
	{
        machineManager->rampUI(1);
	}
	else
	{
        machineManager->rampUI(0);
	}
    
    if(m_isCompleteSide)
    {
        m_dtSideReady += dt;
        
        if(DyStaffManager::sharedStaffManager()->processSideServe(m_dtSideReady))
        {
            for(int i = MAX_SLOT-1;i >= 0;i--)
            {
                DyCustomer* customer = customerManager->m_arrCustomer[i];
                
                if(customer == NULL)
                    continue;
                
                if(customer->m_character->m_status == DyCharacterStatus_Order && !customer->m_hasSide && !m_isSelectedSide)
                {
                    m_aniCashier->m_actionManager->runAnimations("serve");
                    
                    m_dtSideReady = 0;
                    m_aniCashier->runAction(CCSequence::create(CCDelayTime::create(0.5f), CCCallFuncN::create(this, callfuncN_selector(DyTycoonManager::startSideServe)), NULL));
                    m_aniCashier->setUserData(customer);
                    break;
                }
            }
        }
    }
}


void DyTycoonManager::startSideServe(CCNode* node)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    tycoonManager->sideServeToCustomer((DyCustomer*)node->getUserData(), true);
}


void DyTycoonManager::update(float dt)
{
    float dt_org = dt;
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
    
    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
	DyTutorial* tutorial = tutorialManager->getCurrentTutorial();
    if(tutorial && tutorial->m_tutorial_type == DyTutorialType_Message)
        dt = 0;
    if(tutorial && tutorial->m_tutorial_type == DyTutorialType_ReadyCommand)
        dt = 0;
	
    if(tutorial && (tutorialManager->m_doingSpecial || stageManager->m_curLevel == 2 || stageManager->m_curLevel == 6))
    {
        tutorialManager->update(dt_org);
        return;
    }
    
    if(m_status != DyTycoonStatus_Do)
		return;

	if(!m_gameStarted)
		return;
    
//    if(m_board->m_endClear)
//        return;
//
	updateTimer(dt);
	m_commandManager->update(dt);
	updateIndicator();
    customerManager->updateCustomer(dt);
    updateSide(dt);
    
    tutorialManager->update(dt_org);
    
    checkGameEnd();
    
    updateCrazyOrder(dt);
    
    
    if(tutorial && tutorial->m_tutorial_type == DyTutorialType_AddSpecialBlock)
    {
        if(tutorial->m_paramInt0 == 0)
        {
            tutorialManager->m_currentBlock = m_board->m_tblBlock[2 + 5*m_board->m_cntCol];
            tutorialManager->m_currentBlock->setBlockType(TmBlockType_Candy);
        }
        else if(tutorial->m_paramInt0 == 1)
        {
            tutorialManager->m_currentBlock = m_board->m_tblBlock[4 + 5*m_board->m_cntCol];
            tutorialManager->m_currentBlock->setBlockType(TmBlockType_MSG);
        }

        DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
    }

    

    
    //CCLog("m_layer->count = %d", m_layer->getChildrenCount());
}


void DyTycoonManager::updateCrazyOrder(float dt)
{
    if(!m_isCrazyMode)
        return;
    
    m_dtCrazyOrder += dt;
    
    
    if(m_dtCrazyOrder > 2.0f)
    {
        DyRankManager* rankManager = DyRankManager::sharedRankManager();
       
        
        DyRank* currentTarget = rankManager->getCurrrentTarget(rankManager->currentCheckRankType(), m_crazy_point);
  
        
        if(m_currentTarget != currentTarget)
        {
            setRankPopupOver(m_currentTarget);
            
            if(currentTarget == NULL)
                YtLib::sharedLib()->playEffect(YtLibSndEff_Quest_Success);
            else
                YtLib::sharedLib()->playEffect(YtLibSndEff_Quest_Step_Success);
            ((DyLayerGame*)m_layer)->showQuestPopup(true, true);
           
            m_currentTarget = currentTarget;
            m_changeOrder = true;
            
        }
        else if(m_changeOrder)
        {
            setRankPopup();
            m_changeOrder = false;
        }
        
        m_dtCrazyOrder = 0;
    }
}



void DyTycoonManager::endAnimationLastBonusEvent(CCNode* node)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
	node->removeFromParentAndCleanup(true);
    
    tycoonManager->m_layer->schedule(schedule_selector(DyTycoonManager::checkLastPangEnd), .1f);
}



void DyTycoonManager::startLastBonusEvent()
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
	
	CCSprite* sprNoMore = CCSprite::createWithSpriteFrameName("threematch_ui_lastbonus.png");
	CCSprite* sprNoMoreGlow = CCSprite::createWithSpriteFrameName("threematch_ui_lastbonus_glow.png");
    
	sprNoMore->setPosition(ccp(670, 300));
	sprNoMoreGlow->setPosition(ccp(670, 300));
    
	tycoonManager->m_layer->addChild(sprNoMore, ZORDER_GAME_EVENT);
	tycoonManager->m_layer->addChild(sprNoMoreGlow, ZORDER_GAME_EVENT);
    
	sprNoMoreGlow->setOpacity(0);
	sprNoMoreGlow->runAction(CCSequence::create(
                                                CCFadeIn::create(.5f),
                                                CCFadeOut::create(.5f),
                                                CCCallFuncN::create(this, callfuncN_selector(DyBoard::endAnimationEffect)),
                                                NULL));
    
	sprNoMore->runAction(CCSequence::create(
                                            CCDelayTime::create(1.f),
                                            CCCallFuncN::create(this, callfuncN_selector(DyTycoonManager::endAnimationLastBonusEvent)),
                                            NULL));
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BONUS_TIME);
}



void DyTycoonManager::checkLastPangEnd(float dt)
{
    DyTycoonManager* tycoonManger = DyTycoonManager::sharedTycoonManager();
    tycoonManger->m_layer->unschedule(schedule_selector(DyTycoonManager::checkLastPangEnd));
    if(tycoonManger->m_board->isAllBlockUserMovable())
    {
        if(!tycoonManger->m_checkLastPangEvent)
        {
            //
            // start event
            if(tycoonManger->m_board->pangSpecialBlock(false))
            {
                tycoonManger->m_board->m_cntCombo = 0;
                tycoonManger->startLastBonusEvent();
            }
            else
                tycoonManger->clearBlocksBeforeEnd();
            tycoonManger->m_checkLastPangEvent = true;
        }
        else
        {
            if(tycoonManger->m_board->pangSpecialBlock(true))
            {
                tycoonManger->m_layer->schedule(schedule_selector(DyTycoonManager::checkLastPangEnd), .1f);
            }
            else
                tycoonManger->clearBlocksBeforeEnd();
        }
    }
    else
    {
        tycoonManger->m_layer->schedule(schedule_selector(DyTycoonManager::checkLastPangEnd), .1f);
    }
}


void DyTycoonManager::endAnimationGold(CCNode* node)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
		
	CCInteger* pay = (CCInteger*)node->getUserData();

	DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddGoldReal, NULL, NULL, NULL, pay->getValue(), 0, 0);
	tycoonManager->m_commandManager->addLast(commandTycoon);
	commandTycoon->release();

	pay->release();

	node->removeFromParentAndCleanup(true);
    
    if(!tycoonManager->m_isCrazyMode)
    {
        //
        // 골드가 붙는 애니메이션
        CCSprite* sprGold = (CCSprite*)tycoonManager->m_layer->getChildByTag(GAME_UI_TAG_DISP_GOLD_ICON);
        CCSprite* sprGlow = (CCSprite*)sprGold->getChildByTag(GAME_UI_TAG_DISP_GOLD_GLOW);
        
        sprGlow->setOpacity(0);
        sprGold->setScale(1.0f);
        sprGold->runAction(CCSequence::create(
                                                 CCScaleTo::create(0.4f, 1.2f),
                                                 CCScaleTo::create(0.4f, 1.0f),
                                                 NULL
                                                 ));
        sprGlow->runAction(CCSequence::create(
                                              CCFadeIn::create(0.4f),
                                              CCFadeOut::create(0.4f),
                                              NULL
                                              ));
    }
}


void DyTycoonManager::endAnimationCashier(CCNode* node)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_MONEY_REGISTER_BYCHASIER);
}



void DyTycoonManager::sideServeToCustomer(DyCustomer* customer, bool byManager)
{
    DyMachineManager* machineManager = DyMachineManager::sharedMachineManager();
    
    DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddSideToCustomer, customer, NULL, NULL, byManager ? 1 : 0, 0, 0);
    m_commandManager->addLast(commandTycoon);
    commandTycoon->release();
    
    m_dtSide = 0.f;
    m_isCompleteSide = false;
    m_isSelectedSide = false;
    
    machineManager->machineSelect(false);

}

void DyTycoonManager::ccTouchesBegan(CCTouch* touch)
{
	//YtLib* lib = YtLib::sharedLib();
    DyMachineManager* machineManager = DyMachineManager::sharedMachineManager();
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
	if(m_status != DyTycoonStatus_Do)
		return;

    if(m_lockInput)
        return;

	CCPoint touchLocation = touch->getLocation();
	CCPoint nodePosition = m_layer->convertToNodeSpace(touchLocation);

    
    if(!m_isSelectedSide && m_isCompleteSide && machineManager->isSelectMachine(nodePosition))
    {
        m_board->disableSelectBlock(NULL);
		m_isSelectedSide = true;
        
        DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
        DyTutorial* tutorial = tutorialManager->getCurrentTutorial();
        
        if(tutorial && tutorial->m_command_type == DyTutorialCommandType_SideMenuClick)
        {
            DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
        }
        return;

    }
    
    CCSize size;
    CCPoint point;
    CCRect rect;
    
	for(int i = MAX_SLOT-1;i >= 0;i--)
	{
		DyCustomer* customer = customerManager->m_arrCustomer[i];

		if(customer == NULL)
			continue;

		//CCLayer* layerDon = (CCLayer*)m_layer->getChildByTag(GAME_UI_TAG_SLOT_PAY_0 + customer->m_slotidx);

		size = customer->m_character->m_layerTable->getContentSize();
		point = customer->m_character->m_layerTable->getPosition();
		rect = CCRectMake(point.x, point.y, size.width, size.height);

		if(rect.containsPoint(nodePosition))
		{
			if(customer->m_character->m_status >= DyCharacterStatus_Walkout && customer->m_pay > 0)			
			{
                DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
                DyTutorial* tutorial = tutorialManager->getCurrentTutorial();
                
                if(tutorial && tutorial->m_command_type == DyTutorialCommandType_GetMoney)
                {
                    DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
                }
                if(tutorial && tutorial->m_command_type == DyTutorialCommandType_Sell)
                {
                    DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
                }
				sellRecipe(customer, false);
				return;
			}
			else if(customer->m_character->m_status == DyCharacterStatus_Order && m_board->m_blockSelected && m_board->m_blockSelected->m_blocktype == TmBlockType_MSG)
			{
				CCPoint curPos = m_board->getGlobalPosition(m_board->m_blockSelected);
                

				TmCommand* command = new TmCommand(TmCommandType_DestoryBlock, m_board, m_board->m_blockSelected, NULL, 0, 0);
				m_board->m_commandManager->addLast(command);
				command->release();
                
              	DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddSpecialBlockToCustomer, &curPos, NULL, NULL, TmBlockType_MSG, customer->m_idCharacter, 0);
				m_commandManager->addLast(commandTycoon);
				commandTycoon->release();

				m_board->destoryBlock("MSG", curPos);
                
                DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
                
                if(tutorial && (tutorial->m_command_type == DyTutorialCommandType_RemoveCurrentBlock))
                {
                    if(m_board->m_blockSelected == DyTutorialManager::sharedTutorialManager()->m_currentBlock)
                    {
                        DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
                    }
                }

				return;
			}
			else if(customer->m_character->m_status == DyCharacterStatus_Order && !customer->m_hasSide && m_isSelectedSide)
			{
                sideServeToCustomer(customer, false);
				return;
			}
            else if(customer->m_customerType == DyCustomerType_Delivery && customer->m_character->m_status == DyCharacterStatus_Walkin)
            {
                DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
                
                if(tutorial && tutorial->m_command_type == DyTutorialCommandType_DeliveryGetPhone)
                {
                    DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
                }

                
                DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, customer->m_character, NULL, NULL, DyCharacterStatus_Order, customer->m_idCharacter<<2 | customer->m_slotidx, 0);
                m_commandManager->addLast(commandTycoon);
                commandTycoon->release();
            }
            else if(customer->m_customerType == DyCustomerType_Delivery && customer->m_character->m_status == DyCharacterStatus_Like)
            {
                YtLib::sharedLib()->playEffect(YtLibSndEff_DELIVERY_BAG);
                
                DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
                
                if(tutorial && tutorial->m_command_type == DyTutorialCommandType_DeliveryCall)
                {
                    DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
                }
                
                
                DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, customer->m_character, NULL, NULL, DyCharacterStatus_Eat, customer->m_idCharacter<<2 | customer->m_slotidx, 0);
                m_commandManager->addLast(commandTycoon);
                commandTycoon->release();
            }
		}

		if(customer->m_rectTouch.containsPoint(nodePosition))
		{	
			if(customer->m_customerType != DyCustomerType_Delivery && customer->m_character->m_status == DyCharacterStatus_Order && m_board->m_blockSelected && m_board->m_blockSelected->m_blocktype == TmBlockType_Candy)
			{
                //DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
                
				CCPoint curPos = m_board->getGlobalPosition(m_board->m_blockSelected);
                
				TmCommand* command = new TmCommand(TmCommandType_DestoryBlock, m_board, m_board->m_blockSelected, NULL, 0, 0);
				m_board->m_commandManager->addLast(command);
				command->release();
                
				DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddSpecialBlockToCustomer, &curPos, NULL, NULL, TmBlockType_Candy, customer->m_idCharacter, 0);
				m_commandManager->addLast(commandTycoon);
				commandTycoon->release();

				m_board->destoryBlock("CANDY", curPos);
                
                DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
                
                if(tutorial && (tutorial->m_command_type == DyTutorialCommandType_RemoveCurrentBlock))
                {
                    if(m_board->m_blockSelected == DyTutorialManager::sharedTutorialManager()->m_currentBlock)
                    {
                        DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
                    }
                }
				return;
			}
		}
	}
    
    if(m_isSelectedSide)// && m_board->m_blockSelected)
	{
		m_isSelectedSide = false;
        machineManager->machineSelect(false);
	}
}



void DyTycoonManager::endAnimationTimeOut(CCNode* node)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_CLOSE);
	
	CCSprite* sprClock = (CCSprite*)m_layer->getChildByTag(GAME_UI_TAG_DISP_TIME_ICON);
    CCSprite* sprGlow = (CCSprite*)sprClock->getChildByTag(GAME_UI_TAG_DISP_TIME_GLOW);
    sprGlow->setOpacity(0);
    
    sprClock->stopAllActions();
    sprGlow->stopAllActions();
	sprClock->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("game_ui_gage_time_close.png"));
	m_progTime->stopAllActions();
	m_closeTime = true;
}


void DyTycoonManager::updateTimer(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
    if(m_isCrazyMode)
    {
        m_dtOpenTime += dt;
        
        sprintf(lib->m_strTemp, "%02d", ((int)m_dtOpenTime)/60);
        m_labelCrazyTimeM->setString(lib->m_strTemp);
        
        sprintf(lib->m_strTemp, "%02d", ((int)m_dtOpenTime)%60);
        m_labelCrazyTimeS->setString(lib->m_strTemp);
        
        if(m_dtOpenTime > m_crazy_time)
        {
            m_crazy_status++;
            
            if(m_crazy_status < MAX_CRAZY_STEP)
            {
                m_board->m_ice_probability = atoi(lib->m_crazy->getValue("ice_probability", m_crazy_status));
                m_board->m_decay_idle_duration = atoi(lib->m_crazy->getValue("decay_idle_duration", m_crazy_status));
                m_board->m_decay_duration = atoi(lib->m_crazy->getValue("decay_duration", m_crazy_status));
                
                
                customerManager->m_customer_term = atoi(lib->m_crazy->getValue("customer_term", m_crazy_status));
                m_crazy_time = atoi(lib->m_crazy->getValue("time", m_crazy_status));
                m_crazyHeartVel = atoi(lib->m_crazy->getValue("heart_vel", m_crazy_status));
            }
            else
            {
                int heart_vel = atoi(lib->m_parameters->getValue("value_integer", DyLibParam_Crazy_heart_vel));
                
                m_crazy_time += atoi(lib->m_parameters->getValue("value_integer", DyLibParam_Crazy_time_interval));
                
                
                m_crazyHeartVel = (int)((float)m_crazyHeartVel*(float)heart_vel/100.0f);
            }
            
            addCrazyPoint(m_crazy_status*m_crazy_time_add, 0);
        }
        
        DyStageManager* stageManager = DyStageManager::sharedStageManager();
        
        bool createDelly = false;
        switch(stageManager->m_curShopType)
        {
            case DyStageType_Snack:
                if(m_dtOpenTime > m_crazy_create_delly_snack_start + m_crazy_createDelly*m_crazy_create_delly_snack_interval)
                {
                    createDelly = true;
                }
                break;
            case DyStageType_Hamburger:
                if(m_dtOpenTime > m_crazy_create_delly_hamburger_start + m_crazy_createDelly*m_crazy_create_delly_hamburger_interval)
                {
                    createDelly = true;
                }
                break;
            case DyStageType_Coffee:
                if(m_dtOpenTime > m_crazy_create_delly_coffee_start + m_crazy_createDelly*m_crazy_create_delly_coffee_interval)
                {
                    createDelly = true;
                }
                break;
            case DyStageType_China:
            case DyStageType_Pizza:
            case DyStageType_Korean:
                if(m_dtOpenTime > m_crazy_create_delly_china_start + m_crazy_createDelly*m_crazy_create_delly_china_interval)
                {
                    createDelly = true;
                }
                break;
    

        }
        
        if(createDelly)
        {
            if((m_crazy_createDelly == 0 && rand()%100 < m_crazy_create_delly_first_pro) || (m_crazy_createDelly == 1 && rand()%100 < m_crazy_create_delly_second_pro) || (m_crazy_createDelly > 1 && rand()%100 < m_crazy_create_delly_etc_pro))
            {
                if(!m_checkLastPang)
                {
                    TmCommand* command = new TmCommand(TmCommandType_AddSpeicalAttr, m_board, NULL, NULL, TmBlockAttr_Delly, 1);
                    m_board->m_commandManager->addLast(command);
                    command->release();
                }
            }
            m_crazy_createDelly++;
        }
    }
    else
    {
        m_dtOpenTime -= dt;
        
        if(m_dtOpenTime >= 0)
            m_progTime->setPercentage(100-(100*(m_time_limit-m_dtOpenTime)/m_time_limit));
        else if(m_layer->getChildByTag(GAME_UI_TAG_DISP_TIME_CLOSEMESSAGE)->isVisible() == false)
        {
            CCSprite* sprTimeOut = (CCSprite* )m_layer->getChildByTag(GAME_UI_TAG_DISP_TIME_CLOSEMESSAGE);
            sprTimeOut->setVisible(true);
            CCPoint posTarget = sprTimeOut->getPosition();
            
            sprTimeOut->setPosition(ccp(670, 300));
            sprTimeOut->setOpacity(0);
            
            sprTimeOut->runAction(CCSequence::create(
                                                     CCScaleTo::create(0.3f, 1.1f),
                                                     CCScaleTo::create(0.1f, 1.0f),
                                                     CCRotateTo::create(0.05f, 30.f),
                                                     CCRotateTo::create(0.1f, -30.f),
                                                     CCRotateTo::create(0.1f, 30.f),
                                                     CCRotateTo::create(0.1f, -30.f),
                                                     CCRotateTo::create(0.1f, 30.f),
                                                     CCRotateTo::create(0.05f, 0.f),
                                                     CCScaleTo::create(0.2f, 0.5f),
                                                     CCCallFuncN::create(this, callfuncN_selector(DyTycoonManager::endAnimationTimeOut)),
                                                     NULL
                                                     ));
            
            sprTimeOut->runAction(CCSequence::create(
                                                     CCFadeIn::create(0.3f),
                                                     CCDelayTime::create(0.1f+0.05f+0.1f+0.1f+0.1f+0.1f+0.05f),
                                                     CCMoveTo::create(0.2f, posTarget),
                                                     NULL
                                                     ));
            
            YtLib::sharedLib()->stopEffect(m_nottimesnd);
            m_nottimesnd = -1;
            YtLib::sharedLib()->playEffect(YtLibSndEff_TIMEOUT);
        }
        
        //
        // 시간 워닝 시작
        if(!m_isStartWarning && m_dtOpenTime < TIME_WARNING)
        {
            m_nottimesnd = YtLib::sharedLib()->playEffect(YtLibSndEff_NOTTIME);
            CCActionInterval* action2 = CCTintBy::create(0.5, 0, -255, -255);
            CCActionInterval* action2Back = action2->reverse();
            
            CCActionInterval* seq = (CCActionInterval*)(CCSequence::create(action2, action2Back, NULL));
            
            m_progTime->runAction(CCRepeatForever::create(seq));
            m_isStartWarning = true;
            
            
            //
            // 골드가 붙는 애니메이션
            CCSprite* sprTime = (CCSprite*)m_layer->getChildByTag(GAME_UI_TAG_DISP_TIME_ICON);
            CCSprite* sprGlow = (CCSprite*)sprTime->getChildByTag(GAME_UI_TAG_DISP_TIME_GLOW);
            
            sprGlow->setOpacity(0);
            sprGlow->setColor(ccRED);
            
            sprTime->setScale(1.0f);
            sprTime->runAction(CCRepeatForever::create((CCActionInterval*)CCSequence::create(
                                                                                             CCScaleTo::create(0.2f, 1.5),
                                                                                             CCScaleTo::create(0.2f, 1.0f),
                                                                                             NULL
                                                                                             )));
            sprGlow->runAction(CCRepeatForever::create((CCActionInterval*)CCSequence::create(
                                                                                             CCFadeTo::create(0.2f, 128),
                                                                                             CCFadeTo::create(0.2f, 0),
                                                                                             NULL
                                                                                             )));
        }
    }
    
    
    for(int i = 0;i < 3;i++)
    {
        m_dtItemSlot[i] += dt;
        
        DyItem* item = DyItemManager::sharedItemManager()->m_itemslot[i];
        
        if(item == NULL)
            continue;
        
        if(item->m_cntAvailable == 0)
        {
            m_dtItemSlot[i] = 0;
        }
        
        if(m_isCrazyMode && item->m_itemtype == DyItemType_TimeIncrease)
            m_dtItemSlot[i] = 0;
        
        m_progItemSlot[i]->setPercentage(m_dtItemSlot[i]*100.f/m_velItemSlot[i]);
        
        if(m_dtItemSlot[i]*100.f/m_velItemSlot[i] >= 100)
        {
            slotItemEnable(i, true);
        }
        else
        {
            slotItemEnable(i, false);
        }
    }

}


//
// type 0   time
// type 1   gold
// type 2   removeblock
// type 3   combo
void DyTycoonManager::addCrazyPoint(int point, int type)
{
    if(!m_isCrazyMode)
        return;
    
    switch(type)
    {
        case 0:
            m_crazy_point_time += point;
            break;
        case 1:
            m_crazy_point_gold += point;
            break;
        case 2:
            m_crazy_point_removeblock += point;
            break;
        case 3:
            m_crazy_point_combo += point;

            break;
    }
    m_crazy_point += point;
    
    DyLib* lib = DyLib::sharedLib();
    sprintf(lib->m_strTemp, "%d", m_crazy_point);
    m_labelCrazyScore->setString(lib->m_strTemp);
    
    int createGold = 0;
    
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    switch(stageManager->m_curShopType)
    {
        case DyStageType_Snack:
            createGold = m_crazy_create_gold_snack;
            break;
        case DyStageType_Hamburger:
            createGold = m_crazy_create_gold_hamburger;
            break;
        case DyStageType_Coffee:
            createGold = m_crazy_create_gold_coffee;
            break;
        case DyStageType_China:
        case DyStageType_Pizza:
        case DyStageType_Korean:
            createGold = m_crazy_create_gold_china;
            break;

    }
    
    if(m_crazy_point > m_crazy_createGold*m_crazy_create_start_gold)
    {
        m_crazy_createGold++;
        if(m_crazy_createGold > 1)
        {
            if(!m_checkLastPang)
            {
                TmCommand* command = new TmCommand(TmCommandType_AddSpeicalAttr, m_board, NULL, NULL, TmBlockAttr_Gold, (m_crazy_createGold-1)*createGold);
                m_board->m_commandManager->addLast(command);
                command->release();
            }
        }
    }
}



void DyTycoonManager::addCrazyHeart(int count)
{
    if(!m_isCrazyMode)
        return;
    
    if(m_crazyRemainHeart == 0)
        return;
 
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
    m_crazyRemainHeart += count;
    
    if(m_crazyRemainHeart > 3)
        m_crazyRemainHeart = 3;
    
    if(m_crazyRemainHeart == 0)
    {
        customerManager->outCustomers();
    }
    
    for(int i = 0;i < 3;i++)
    {
        if(i < m_crazyRemainHeart)
            m_sprCrazyHeart[i]->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("crazy_pannel_top_crazymode_life_on.png"));
        else
            m_sprCrazyHeart[i]->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("crazy_pannel_top_crazymode_life_slot.png"));
    }
}


void DyTycoonManager::initGame()
{
    DyCostumeManager* costumeManager = DyCostumeManager::sharedCostumeManager();
    
    m_combo_heart = 0;
    m_customer_happy_cnt = 0;
	
	m_closeTime = false;
    m_checkLastPang = false;
    m_checkLastPangEvent = false;
    
    m_failWithQuest = false;
    
	m_dtSide = 0.f;
	
    m_isCompleteSide = false;
	m_isSelectedSide = false;
    
    
    m_checkLastOpportunity = false;
    m_pauseCheckOpportunity = false;
    
    m_indexRecipe = 0;
    
	m_earnGold = 0;
    
    m_nottimesnd = -1;
    
    m_newscore = false;
    
	//m_arrCustomerOrder->removeAllObjects();
    
    DyCustomerManager::sharedCustomerManager()->initCustomers();
 
    for(int i = 0;i < 3;i++)
        m_questItemBasic[i] = false;
    
    for(int i = 0;i < 3;i++)
        m_customerSlot[i] = -1;
    
    
    costumeManager->setCostumeGameFunction(&m_functionGame);
}


void DyTycoonManager::startGame()
{
    //DyGuestManager* guestManager = DyGuestManager::sharedGuestManager();
    
	m_status = DyTycoonStatus_Do;
	m_gameStarted = true;


}


void DyTycoonManager::destoryGameObject()
{
	for(int i = 0;i < 3;i++)
	{
		if(m_indicatorCharacter[i])
			YtAniObject::unloadAniObject(m_indicatorCharacter[i]);
		if(m_indicatorDish[i])
			YtAniObject::unloadAniObject(m_indicatorDish[i]);
		m_indicatorCharacter[i] = NULL;
		m_indicatorDish[i] = NULL;
	}

	//if(m_feverstart)
	//	YtAniObject::unloadAniObject(m_feverstart);
	//m_feverstart = NULL;
	if(m_openDoggy)
		YtAniObject::unloadAniObject(m_openDoggy);	
	m_openDoggy = NULL;
    
    
    
    
	//if(m_questpannel)
	//	YtAniObject::unloadAniObject(m_questpannel);	
	//m_questpannel = NULL;
    
	if(m_questbar)
		YtAniObject::unloadAniObject(m_questbar);
	m_questbar = NULL;
}

void DyTycoonManager_callFuncRecipe(int value, int code)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    tycoonManager->startGameReady();
}


void DyTycoonManager::startGameReal_Real(float dt)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    tycoonManager->m_layer->unschedule(schedule_selector(DyTycoonManager::startGameReal_Real));

    
	//tycoonManager->m_openDoggy->setVisible(true);
	tycoonManager->m_openDoggy->m_actionManager->runAnimations("start");
	
	tycoonManager->m_board->m_layerMain->scheduleUpdate();
	YtLib::sharedLib()->playBGM(tycoonManager->m_bgmIdx);
    
	tycoonManager->m_menuItem->setEnabled(true);
	tycoonManager->m_menuPause->setEnabled(true);
	tycoonManager->m_menuQuestPopup->setEnabled(true);
    tycoonManager->m_layer->setKeypadEnabled(true);
    
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_DOGGY_SHOW);
    

}


void DyTycoonManager::startGameReal()
{
    
    if(m_isCrazyMode || m_quest == NULL || m_quest->m_questtype == DyQuestType_Ignore)
    {
        startGameReal_Real(0);
    }
    else
    {
        ((DyLayerGame*)m_layer)->showQuestPopup(true, true);
        m_layer->schedule(schedule_selector(DyTycoonManager::startGameReal_Real), 1.0f);
    }
    
    
    m_gameSetting = false;
    
    CCSprite* sprGlow = (CCSprite*)m_openDoggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_OPEN_GLOW);
    sprGlow->setVisible(false);
    
    m_layer->reorderChild(((DyLayerGame*)m_layer)->m_layerPause, ZORDER_GAME_PAUSELAYER);
	((DyLayerGame*)m_layer)->m_layerPause->setVisible(false);
    
    m_layer->reorderChild(m_openDoggy, ZORDER_GAME_BASE_LEVEL2);
    m_layer->reorderChild(m_menuItem, ZORDER_GAME_BASE_LEVEL2);
    
    for(unsigned int i = 0;i < DYITEM_SLOT_CNT;i++)
	{
		CCLabelTTF* labelRemain = (CCLabelTTF *)m_layer->getChildByTag(GAME_UI_TAG_DISP_SLOT0_COUNT+i);
		m_layer->reorderChild(labelRemain, ZORDER_GAME_BASE_LEVEL2);
	}

    
    
    ((DyLayerGame*)m_layer)->getChildByTag(GAME_UI_TAG_SETTING_STAFF)->setVisible(false);
    ((DyLayerGame*)m_layer)->getChildByTag(GAME_UI_TAG_MENU_START)->setVisible(false);
    ((DyLayerGame*)m_layer)->getChildByTag(GAME_UI_TAG_SETTING_MACHINE)->setVisible(false);
    DyMachineManager::sharedMachineManager()->setMachineReady(false, NULL, NULL);
    
    
    
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    if(staffManager->isHire())
	{
        m_aniCashier->m_actionManager->runAnimations("default");
        
		m_aniCashier->setVisible(true);
        
		m_sprCashierDesk->setVisible(true);
		m_sprCashierDesk->setDisplayFrame(CCSprite::createWithSpriteFrameName(m_urlCounter.c_str())->displayFrame());
        
        staffManager->hire();
	}
    else
    {
        m_aniCashier->setVisible(false);
    }
}


void DyTycoonManager::highlightStartButton()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    float speed = .2f;
    
    CCSprite* sprGlow = (CCSprite*)tycoonManager->m_openDoggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_OPEN_GLOW);
    CCActionInterval* seq = (CCActionInterval*)(CCSequence::create(CCFadeIn::create(speed), CCFadeOut::create(speed), NULL));
    sprGlow->runAction(CCRepeatForever::create(seq));
	sprGlow->setVisible(true);
    
    CCSprite* sprLeft = (CCSprite*)sprGlow->getChildByTag(0);
    CCPoint posLeft = sprLeft->getPosition();
    seq = (CCActionInterval*)(CCSequence::create(CCMoveTo::create(speed, ccp(posLeft.x + 15, posLeft.y)), CCMoveTo::create(speed, ccp(posLeft.x, posLeft.y)), NULL));
    sprLeft->runAction(CCRepeatForever::create(seq));
    sprLeft->setVisible(true);
    
    CCSprite* sprRight = (CCSprite*)sprGlow->getChildByTag(1);
    CCPoint posRight = sprRight->getPosition();
    seq = (CCActionInterval*)(CCSequence::create(CCMoveTo::create(speed, ccp(posRight.x - 15, posRight.y)), CCMoveTo::create(speed, ccp(posRight.x, posRight.y)), NULL));
    sprRight->runAction(CCRepeatForever::create(seq));
    sprRight->setVisible(true);
}


void DyTycoonManager::startGameReady()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(m_gameStarted)
        return;

    if(DyRecipeManager::sharedRecipeManager()->checkNewRecipe(DyTycoonManager_callFuncRecipe))
    {
        return;
    }
    
    tycoonManager->initGame();
    ((DyLayerGame*)tycoonManager->m_layer)->buildDebugCostume();
    
    
    tycoonManager->m_gameStartForce = false;
    tycoonManager->m_gameSetting = true;
    
    ((DyLayerGame*)tycoonManager->m_layer)->curtainDrop(false, false);
    ((DyLayerGame*)tycoonManager->m_layer)->itemSlotSetting();
    YtLib::sharedLib()->playEffect(YtLibSndEff_CURTAIN_FOLD);
    
    tycoonManager->m_openDoggy->setVisible(true);
	tycoonManager->m_openDoggy->m_actionManager->runAnimations("ready");
    
    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    
    
    tycoonManager->m_menuItem->setEnabled(true);
	
    tycoonManager->m_layer->reorderChild(((DyLayerGame*)tycoonManager->m_layer)->m_layerPause, ZORDER_GAME_SETTING);
	((DyLayerGame*)tycoonManager->m_layer)->m_layerPause->setVisible(true);
    
    tycoonManager->m_layer->reorderChild(tycoonManager->m_openDoggy, ZORDER_GAME_SETTING_ON);
    tycoonManager->m_layer->reorderChild(tycoonManager->m_menuItem, ZORDER_GAME_SETTING_ON);
    for(unsigned int i = 0;i < DYITEM_SLOT_CNT;i++)
	{
		CCLabelTTF* labelRemain = (CCLabelTTF *)tycoonManager->m_layer->getChildByTag(GAME_UI_TAG_DISP_SLOT0_COUNT+i);
		tycoonManager->m_layer->reorderChild(labelRemain, ZORDER_GAME_SETTING_ON);
	}

//    승진 테스트
//    DyDataManager* dataManager = DyDataManager::sharedDataManager();
////
////    dataManager->m_public.m_staff.m_grade = 0;
//
//    //if(dataManager->m_private.m_staff_next_grade_remain < 10)
//        dataManager->m_private.m_staff_next_grade_remain = 0;

    DyMachineManager::sharedMachineManager()->setMachineReady(true, (CCLayer*)((DyLayerGame*)m_layer)->getChildByTag(GAME_UI_TAG_SETTING_MACHINE), (CCMenuItemImage*)((DyLayerGame*)m_layer)->getChildByTag(GAME_UI_TAG_MENU_START)->getChildByTag(2));
    tycoonManager->m_layer->reorderChild(((DyLayerGame*)m_layer)->getChildByTag(GAME_UI_TAG_SETTING_MACHINE), ZORDER_GAME_SETTING_ON2);
    tycoonManager->m_layer->reorderChild(((DyLayerGame*)m_layer)->getChildByTag(GAME_UI_TAG_MENU_START), ZORDER_GAME_SETTING_ON2);
    tycoonManager->m_layer->reorderChild(((DyLayerGame*)m_layer)->getChildByTag(GAME_UI_TAG_SETTING_STAFF), ZORDER_GAME_SETTING_ON2);
    
    ((DyLayerGame*)tycoonManager->m_layer)->setCashier(false);
    
    
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    tutorialManager->m_doingSpecial = false;
    
    bool hasTutorial = false;
    
    if(!tycoonManager->m_isCrazyMode)
    {
        if(stageManager->m_curShopType == DyStageType_Snack)
        {
            DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
            
            if(stageManager->m_curLevel == 2 && stageManager->getStageState(0, 2) <= DyStageState_Fail)
            {
                hasTutorial = true;
                tutorialManager->clearTutorial();
                tutorialManager->buildTutorial(5);
                dataManager->m_private.m_viewNewTutorial = true;
            }
            else if(stageManager->m_curLevel == 6 && stageManager->getStageState(0, 6) <= DyStageState_Fail && !staffManager->isIdle())
            {
                hasTutorial = true;
                tutorialManager->clearTutorial();
                tutorialManager->buildTutorial(4);
            }
        }
        
        int max_shop = 0;
        int max_level = 0;
        
        stageManager->getCurrentStage(&dataManager->m_public, &max_shop, &max_level);
        
        if(!hasTutorial && (max_shop > 0 || max_level > 6) && !dataManager->m_private.m_viewNewTutorial)
        {
            tutorialManager->m_doingSpecial = true;
            tutorialManager->clearTutorial();
            tutorialManager->buildTutorial(5);
            dataManager->m_private.m_viewNewTutorial = true;
            hasTutorial = true;
        }
        else if(!dataManager->m_local.m_viewPuddleTutorial && stageManager->m_curShopType == DyStageType_Pizza)
        {
            tutorialManager->m_doingSpecial = true;
            
            tutorialManager->clearTutorial();
            tutorialManager->buildTutorial(7);
            dataManager->m_local.m_viewPuddleTutorial = true;
            dataManager->dataSave();
            hasTutorial = true;
        }
    }
    
    if(!hasTutorial)
    {
        tycoonManager->highlightStartButton();
    }
    else
    {
        CCSprite* sprGlow = (CCSprite*)tycoonManager->m_openDoggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_OPEN_GLOW);
        CCSprite* sprLeft = (CCSprite*)sprGlow->getChildByTag(0);
        CCSprite* sprRight = (CCSprite*)sprGlow->getChildByTag(1);
        
        sprGlow->setVisible(false);
        sprLeft->setVisible(false);
        sprRight->setVisible(false);
    }

    
    tycoonManager->m_menuQuestPopup->setEnabled(true);
    tycoonManager->m_menuPause->setEnabled(true);
}



void DyTycoonManager::clearBlocksBeforeEnd()
{
    m_board->endAndClearBlocks();
    m_layer->schedule(schedule_selector(DyTycoonManager::endGame), 1.f);
}


void DyTycoonManager::endGame()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    tycoonManager->m_layer->unschedule(schedule_selector(DyTycoonManager::endGame));
    
    if(tycoonManager->m_status != DyTycoonStatus_Do)
        return;
    
    tycoonManager->m_status = DyTycoonStatus_Idle;
    
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
	tycoonManager->m_board->pause();
	tycoonManager->m_menuItem->setEnabled(false);
	tycoonManager->m_menuPause->setEnabled(false);
	tycoonManager->m_menuQuestPopup->setEnabled(false);
    tycoonManager->m_layer->setKeypadEnabled(false);


	YtLib::sharedLib()->stopBGM();

    //
    // 자랑하기를 위한 것
    DyFriendManager::sharedFriendManager()->initFriendsTag();
    
    
    if(tycoonManager->m_isCrazyMode)
    {
        DyLayerGameResult* layerGameResult = DyLayerGameResult::create(tycoonManager->m_layer);
        YtLayerManager::sharedLayerManager()->pushLayer(layerGameResult, true, true);
        
        if(dataManager->m_crazyRecord < tycoonManager->m_crazy_point)
        {
            dataManager->m_crazyRecord = tycoonManager->m_crazy_point;
            tycoonManager->m_newscore = true;
        }
        
        if(dataManager->m_crazyBestRecord < tycoonManager->m_crazy_point)
        {
            dataManager->m_crazyBestRecord = tycoonManager->m_crazy_point;
        }
        
        moneyManager->addGold(tycoonManager->m_crazy_bonusGold, true);
        moneyManager->addDelly(tycoonManager->m_crazy_bonusDelly, true);
    }
    else
    {
        //DyStageRecord* stageRecord = stageManager->getStageRecord(stageManager->m_curShopType, stageManager->m_curLevel);
        
        DyStageState curStageState;
        
        //
        // caculate point
        if(tycoonManager->m_current_gold < tycoonManager->m_goal_gold)
        {
            curStageState = DyStageState_Fail;
        }
        else if(tycoonManager->m_current_gold < tycoonManager->m_perfect_gold)
        {
            curStageState = DyStageState_Goal;
            
            if(tycoonManager->m_current_gold >= tycoonManager->m_goal_gold + (tycoonManager->m_perfect_gold-tycoonManager->m_goal_gold)/2)
            {
                curStageState = DyStageState_Goal_Plus;
            }
        }
        else
        {
            curStageState = DyStageState_Perfect;
        }
        
        
        //
        // show result popup
        DyLayerGameResult* layerGameResult = DyLayerGameResult::create(tycoonManager->m_layer);
        
        
        if(curStageState >= DyStageState_Goal)
        {
            if(tycoonManager->m_questProcessComplete == false && tycoonManager->m_quest->m_questtype != DyQuestType_Ignore)
            {
                curStageState = DyStageState_Fail;
                tycoonManager->m_failWithQuest = true;
            }
        }
        layerGameResult->m_curStageState = curStageState;
        
        //
        // 실패가 아님
        if(curStageState >= DyStageState_Goal)
        {
            DyStaffManager::sharedStaffManager()->hireSuccess();
            
            if(curStageState == DyStageState_Goal)
                moneyManager->addGold((tycoonManager->m_current_gold*STORY_INCOMING_GOLD_GOAL)/100.f, true);
            else if(curStageState == DyStageState_Goal_Plus)
                moneyManager->addGold((tycoonManager->m_current_gold*STORY_INCOMING_GOLD_GOAL_PLUS)/100.f, true);
            else if(curStageState == DyStageState_Perfect)
                moneyManager->addGold((tycoonManager->m_current_gold*STORY_INCOMING_GOLD_PERFECT)/100.f, true);
            
            //
            // 하트 보너스
            if(tycoonManager->m_current_heart >= tycoonManager->m_bonus_heart_cnt)
                tycoonManager->m_bouns_heart_point = tycoonManager->m_bouns_heart;
            else
                tycoonManager->m_bouns_heart_point = 0;
            
            //
            // 콤보 보너스
            
            //
            // 스타 보너스
            switch(curStageState)
            {
                case DyStageState_Goal:
                    tycoonManager->m_star_point = STAR_POINT1;
                    break;
                case DyStageState_Goal_Plus:
                    tycoonManager->m_star_point = STAR_POINT1+STAR_POINT2;
                    break;
                case DyStageState_Perfect:
                    tycoonManager->m_star_point = STAR_POINT1+STAR_POINT2+STAR_POINT3;
                    break;
                default:
                    tycoonManager->m_star_point = 0;
                    break;
            }
            
            tycoonManager->m_stage_point = tycoonManager->m_current_gold + tycoonManager->m_bouns_heart_point + tycoonManager->m_star_point + tycoonManager->m_combo_point;
            
            if(stageManager->getStageState(stageManager->m_curShopType, stageManager->m_curLevel) < curStageState)
                stageManager->setStageState(stageManager->m_curShopType, stageManager->m_curLevel, curStageState);
            
            if(dataManager->shop_get_record(stageManager->m_curShopType, stageManager->m_curLevel) < tycoonManager->m_stage_point)
            {
                dataManager->shop_set_record(stageManager->m_curShopType, stageManager->m_curLevel, tycoonManager->m_stage_point);
                tycoonManager->m_newscore = true;
            }
            
            stageManager->calculateTotalPoint();
        }
        
        YtLayerManager::sharedLayerManager()->pushLayer(layerGameResult, true, true);
        
        
        //
        // 다음스테이지 셋팅
        
        if(curStageState > DyStageState_Fail)
        {   
            if(stageManager->isLastLevel())
            {
                if(dataManager->shop_get_viewending(stageManager->m_curShopType))
                {
                    layerGameResult->m_menu->getChildByTag(GAMERESULT_UI_TAG_MENUITEM_NEXT)->setVisible(false);
                }
                else
                {
                    layerGameResult->m_menu->getChildByTag(GAMERESULT_UI_TAG_MENUITEM_MENU)->setVisible(false);
                    layerGameResult->m_menu->getChildByTag(GAMERESULT_UI_TAG_MENUITEM_REPLAY)->setVisible(false);
                    layerGameResult->m_menu->getChildByTag(GAMERESULT_UI_TAG_MENUITEM_NEXT)->setPosition(layerGameResult->m_menu->getChildByTag(GAMERESULT_UI_TAG_MENUITEM_REPLAY)->getPosition());
                }
            }
            else
            {
                stageManager->nextStageOpen();
            }
        }
        
        DyDataManager::sharedDataManager()->checkAwards(DyAwardsCheckType_Shop, false);
    }
    
    //DyRankManager::sharedRankManager()->setMyRecord();
	stageManager->stageSave();
}


void DyTycoonManager::addHeart(int cntHeart, bool isAnimate)
{
	m_current_heart += cntHeart;

    //CCLog("누적하트 = %d", m_current_heart);
	float percent = (float)m_current_heart/(float)m_bonus_heart_cnt;
	if(m_current_heart >=  m_bonus_heart_cnt)
	{
		percent = 1.0f;
     
        if(!m_heartComplete)
        {
            CCSprite* sprHeart = (CCSprite*)m_layer->getChildByTag(GAME_UI_TAG_DISP_HEART_ICON);
            sprHeart->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("game_ui_gage_heart.png"));
            m_heartComplete = true;
            YtLib::sharedLib()->playEffect(YtLibSndEff_CANDY);
        }
	}

	if(isAnimate)
	{
		if(m_progHeart->getPercentage() != 100)
		{
			CCProgressTo *progressTo = CCProgressTo::create(0.2f, percent*100);
			m_progHeart->runAction(progressTo);
		}
	}
	else
		m_progHeart->setPercentage(percent*100);
}


void DyTycoonManager::pauseGame()	
{
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
	m_status = DyTycoonStatus_Paused;

    
    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
    if(tutorial)
        tutorial->pause();
    
    customerManager->pauseCustomers();
    
    CCObject* pObj = NULL;
    CCArray* pChildren = m_layer->getChildren();
    CCARRAY_FOREACH(pChildren, pObj)
    {
        CCNode* pChild = (CCNode*)pObj;
		if(pChild->getTag() < GAME_UI_TAG_MENU_ITEM)
			pChild->pauseSchedulerAndActions();
    }

	m_board->pause();
    
    if(m_nottimesnd >= 0)
    {
        YtLib::sharedLib()->stopEffect(m_nottimesnd);
        m_nottimesnd = -1;
    }
}


void DyTycoonManager::resumeGame()
{
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
	m_status = DyTycoonStatus_Do;
    
    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
    if(tutorial)
        tutorial->resume();

    customerManager->resumeCustomers();
    

    CCObject* pObj = NULL;
    CCArray* pChildren = m_layer->getChildren();
    CCARRAY_FOREACH(pChildren, pObj)
    {
        CCNode* pChild = (CCNode*)pObj;
		if(pChild->getTag() < GAME_UI_TAG_MENU_ITEM)
			pChild->resumeSchedulerAndActions();
    }

	m_board->resume();
    
    if(m_nottimesnd >= 0)
        m_nottimesnd = YtLib::sharedLib()->playEffect(YtLibSndEff_NOTTIME);
}


void DyTycoonManager::endAnimationSide(CCNode* node)
{
	DyCustomer* customer = (DyCustomer*)node->getUserData();
	
    customer->m_character->m_comming--;
    
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	DyTycoonCommand* command = new DyTycoonCommand(DyTycoonCommandType_AddSideToCustomerReal, customer, NULL, NULL, 0, 0, 0);
	tycoonManager->m_commandManager->addLast(command);
	command->release();

	node->removeFromParentAndCleanup(true);
}


void DyTycoonManager::endAnimationTime(CCNode* node)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
	DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(DyItemType_TimeIncrease);

	if(tycoonManager->m_dtOpenTime > 0)
	{
		tycoonManager->m_dtOpenTime += ITEM_ADD_TIME;
		if(tycoonManager->m_dtOpenTime > tycoonManager->m_time_limit)
			tycoonManager->m_dtOpenTime = tycoonManager->m_time_limit;

		if(itemManager->useItem(item->m_itemtype, 1) == 0)
		{
			CCMenuItemImage* menuItem = (CCMenuItemImage *)tycoonManager->m_menuItem->getChildByTag(GAME_UI_TAG_MENUITEM_SLOT0+itemManager->getSlotIndex(item->m_itemtype));
			menuItem->setEnabled(false);
		}
        
        tycoonManager->questProcess(QUESTTAG_ITEM, "timeincrease", 1);
        tycoonManager->slotItemUse(DyItemType_TimeIncrease);
        
		itemManager->dataSave();

		tycoonManager->m_progTime->setPercentage(100-(100*(tycoonManager->m_time_limit-tycoonManager->m_dtOpenTime)/tycoonManager->m_time_limit));

		CCMenuItemImage* ctrlBtnTime = (CCMenuItemImage*)tycoonManager->m_menuItem->getChildByTag(
			itemManager->getSlotIndex(DyItemType_TimeIncrease)+GAME_UI_TAG_MENUITEM_SLOT0);
		ctrlBtnTime->setEnabled(true);

		if(tycoonManager->m_isStartWarning && tycoonManager->m_dtOpenTime > TIME_WARNING)
		{   
			tycoonManager->m_progTime->stopAllActions();

			tycoonManager->m_progTime->setColor(ccWHITE);
			tycoonManager->m_isStartWarning = false;
            YtLib::sharedLib()->stopEffect(tycoonManager->m_nottimesnd);
            tycoonManager->m_nottimesnd = -1;
  		}
        
        YtLib::sharedLib()->playEffect(YtLibSndEff_ADDTIME);
	}
	node->removeFromParentAndCleanup(true);
}


void DyTycoonManager::addTime()
{
	DyItemManager* itemManager = DyItemManager::sharedItemManager();

	if(m_dtOpenTime > 0)
	{
		CCMenuItemImage* ctrlBtnTime = (CCMenuItemImage*)m_menuItem->getChildByTag(itemManager->getSlotIndex(DyItemType_TimeIncrease)+GAME_UI_TAG_MENUITEM_SLOT0);
		CCNode* sprTarget = (CCNode*)m_layer->getChildByTag(GAME_UI_TAG_DISP_TIME_ICON);

		CCSprite* sprite = CCSprite::createWithSpriteFrameName("game_ui_item_time.png");
		sprite->setPosition(ctrlBtnTime->getPosition());

		CCPoint posMid = ccp(m_board->m_layerMain->getPosition().x + m_board->m_layerMain->getContentSize().width/2, m_board->m_layerMain->getPosition().y + m_board->m_layerMain->getContentSize().height/2);
		
		CCCallFunc* callfun = CCCallFuncN::create(m_layer, callfuncN_selector(DyTycoonManager::endAnimationTime));
		
		CCActionInterval* moveto1 = CCMoveTo::create(0.2f, posMid);
		CCActionInterval* moveto2 = CCMoveTo::create(0.3f, sprTarget->getPosition());
		CCActionInterval* rotate = CCRotateBy::create(0.5f, 360.f);

		CCActionInterval* scaleto1 = CCScaleTo::create(0.2f, 5.f);
		CCActionInterval* scaleto2 = CCScaleTo::create(0.3f, 1.0f);

		CCFiniteTimeAction *seq = CCSequence::create(moveto1, rotate, moveto2, callfun, NULL);
		sprite->runAction(seq);

		sprite->runAction(CCSequence::create(scaleto1, CCDelayTime::create(0.5f), scaleto2, NULL));
		sprite->runAction(CCSequence::create(CCFadeTo::create(0.2f, 153), CCDelayTime::create(0.5f), CCFadeTo::create(0.3f, 255), NULL));

		m_layer->addChild(sprite, ZORDER_GAME_MOVE_OBJECT);

		ctrlBtnTime->setEnabled(false);
	}
}


void DyTycoonManager::cbCandyItemRecover(float dt)
{
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

	DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(DyItemType_Candy);
	
	CCMenuItemImage* menuItem = (CCMenuItemImage *)tycoonManager->m_menuItem->getChildByTag(GAME_UI_TAG_MENUITEM_SLOT0+itemManager->getSlotIndex(item->m_itemtype));
		
	if(item->m_cntAvailable == 0)
	{
		menuItem->setEnabled(false);
	}
	else
		menuItem->setEnabled(true);
	tycoonManager->m_layer->unschedule(schedule_selector(DyTycoonManager::cbCandyItemRecover));
}


void DyTycoonManager::addHeartToAllCustomer()
{
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	DyItemManager* itemManager = DyItemManager::sharedItemManager();

	CCMenuItemImage* ctrlBtnCandy = (CCMenuItemImage*)m_menuItem->getChildByTag(itemManager->getSlotIndex(DyItemType_Candy)+GAME_UI_TAG_MENUITEM_SLOT0);

	CCPoint curPos = ctrlBtnCandy->getPosition();

	bool isUsed = false;
	for(int i = MAX_SLOT-1;i >= 0;i--)
	{
		DyCustomer* customer = customerManager->m_arrCustomer[i];

		if(customer && customer->m_customerType != DyCustomerType_Delivery && customer->m_character->m_status == DyCharacterStatus_Order)
		{
            DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddSpecialBlockToCustomer, &curPos, NULL, NULL, TmBlockType_Candy, customer->m_idCharacter, 0);
			m_commandManager->addLast(commandTycoon);
			commandTycoon->release();
			isUsed = true;
		}
	}

	if(isUsed)
	{
		DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(DyItemType_Candy);

		itemManager->useItem(item->m_itemtype, 1);

		itemManager->dataSave();

		ctrlBtnCandy->setEnabled(false);
		tycoonManager->m_layer->schedule(schedule_selector(DyTycoonManager::cbCandyItemRecover), 1.0f);
        tycoonManager->questProcess(QUESTTAG_ITEM, "candy", 1);
        tycoonManager->questProcess(QUESTTAG_REMAIN, "candy", 1);
        
        tycoonManager->slotItemUse(DyItemType_Candy);
    }
    
    
}





void DyTycoonManager::addHeart(int index)
{
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
//	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
//	DyItemManager* itemManager = DyItemManager::sharedItemManager();
    
	//CCMenuItemImage* ctrlBtnCandy = (CCMenuItemImage*)m_menuItem->getChildByTag(itemManager->getSlotIndex(DyItemType_Candy)+GAME_UI_TAG_MENUITEM_SLOT0);
    
	CCPoint curPos = ccp(308, 511);//ctrlBtnCandy->getPosition();
    
	//bool isUsed = false;
	//for(int i = MAX_SLOT-1;i >= 0;i--)
	{
		DyCustomer* customer = customerManager->m_arrCustomer[index];
        
		if(customer && customer->m_customerType != DyCustomerType_Delivery && customer->m_character->m_status == DyCharacterStatus_Order)
		{
            DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddSpecialBlockToCustomer, &curPos, NULL, NULL, TmBlockType_Candy, customer->m_idCharacter, 0);
			m_commandManager->addLast(commandTycoon);
			commandTycoon->release();
//			isUsed = true;
		}
	}
    
//	if(isUsed)
//	{
//		DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(DyItemType_Candy);
//        
//		itemManager->useItem(item->m_itemtype, 1);
//        
//		itemManager->dataSave();
//        
//		ctrlBtnCandy->setEnabled(false);
//		tycoonManager->m_layer->schedule(schedule_selector(DyTycoonManager::cbCandyItemRecover), 1.0f);
//        tycoonManager->questProcess(QUESTTAG_ITEM, "candy", 1);
//        tycoonManager->questProcess(QUESTTAG_REMAIN, "candy", 1);
//        
//        tycoonManager->slotItemUse(DyItemType_Candy);
//    }
    
    
}





void DyTycoonManager::addSide(DyCustomer* customer, bool byManager)
{
    DyMachineManager* machineManager = DyMachineManager::sharedMachineManager();
    
	CCPoint tgtLayerPoint = m_layer->getChildByTag(GAME_UI_TAG_SLOT_DISH_0+customer->m_slotidx)->getPosition();
	CCSize tgtLayerSize = m_layer->getChildByTag(GAME_UI_TAG_SLOT_DISH_0+customer->m_slotidx)->getContentSize();
	CCPoint tgtPoint = ccp(tgtLayerPoint.x - tgtLayerSize.width/2 + 17-13, tgtLayerPoint.y - tgtLayerSize.height/2);

	CCSprite* sprite = machineManager->createSide();
	sprite->setPosition(ccp(50, 50));
    
    float speed = 1.0f;
    if(byManager)
    {
        CCSprite* sprManagerHand = CCSprite::createWithSpriteFrameName("game_ui_staff_hand.png");
        sprite->addChild(sprManagerHand);
        sprManagerHand->setPosition(ccp(30,10));
        speed = 2.0f;
        
        CCSprite* sprGlow = machineManager->createSideGlow();
        sprite->addChild(sprGlow, -1);
        sprGlow->setPosition(ccp(sprite->getContentSize().width/2, sprite->getContentSize().height/2));
        
        customer->m_isSideByCashier = true;
    }
    
	float distance = ccpDistance(sprite->getPosition(), tgtPoint);
    
    customer->m_character->m_comming++;
	CCCallFunc* callfun = CCCallFuncN::create(m_layer, callfuncN_selector(DyTycoonManager::endAnimationSide));
	
    CCActionInterval* move_ease_in = CCEaseIn::create(CCMoveTo::create(0.001f*distance*speed, tgtPoint), 0.2f);
	
    CCFiniteTimeAction *seq = CCSequence::create(move_ease_in, callfun, NULL);
	sprite->runAction(seq);
	sprite->setUserData(customer);
	sprite->setAnchorPoint(ccp(0.5, 0.0));

	m_layer->addChild(sprite, ZORDER_GAME_MOVE_OBJECT_UNDER);
}


void DyTycoonManager::addSpeicialMaterial(int idCustomer, TmBlockType blockType, CCPoint position)
{
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
	bool apply = false;
	CCPoint tgtPoint;
	CCPoint srcPoint = position;//m_layer->convertToNodeSpace(position);
	DyCustomer* customer;

	for(unsigned int i = 0;i < MAX_SLOT;i++)
	{
		customer = customerManager->m_arrCustomer[i];
		
		if(customer == NULL)
			continue;

		if(customer->m_character->m_status != DyCharacterStatus_Order)
			continue;
        
        //
        // 화나 가기 직전은 스킵
        if(customer->m_character->m_heartLife < 0.02)
            continue;
        
		if(customer->m_idCharacter != idCustomer)
			continue;
		
		switch(blockType)
		{
			case TmBlockType_Candy:
				{
                    if(customer->m_customerType == DyCustomerType_Delivery)
                        continue;
					CCLayer* layerCharacter = (CCLayer*)m_layer->getChildByTag(GAME_UI_TAG_SLOT_TOUCH_CHARACTER_0+customer->m_slotidx);
					tgtPoint = ccp(layerCharacter->getPosition().x+layerCharacter->getContentSize().width/2, layerCharacter->getPosition().y+layerCharacter->getContentSize().height/2);
				}
				break;
			case TmBlockType_MSG:
				{
					CCSprite* sprBallon = (CCSprite*)m_layer->getChildByTag(GAME_UI_TAG_SLOT_BALLOON_0+customer->m_slotidx);
					tgtPoint = sprBallon->getPosition();
					tgtPoint = ccp(tgtPoint.x, tgtPoint.y);
				}
				break;
            default:
                break;
		}
		apply = true;
		break;
	}

	if(apply)
	{
		//DyBlockManager* blockManager = DyBlockManager::sharedBlockManager();
		CCSprite* sprite = NULL;
		switch(blockType)
		{
			case TmBlockType_Candy:
				sprite = CCSprite::createWithSpriteFrameName("threematch_ui_candy.png");
				break;
			case TmBlockType_MSG:
				sprite = CCSprite::createWithSpriteFrameName("threematch_ui_msg.png");
				break;
            default:
                break;
		}					

		m_layer->addChild(sprite, ZORDER_GAME_MOVEBLOCKTOCUSTOMER);
		
		sprite->setPosition(srcPoint);
        
        customer->m_character->m_comming++;

		float distance = ccpDistance(srcPoint, tgtPoint);
		CCCallFunc* callfun = CCCallFuncN::create(m_layer, callfuncN_selector(DyTycoonManager::endAnimationSpecialAdd));
		CCActionInterval* moveto = CCMoveTo::create(0.001f*distance, tgtPoint);
		//CCActionInterval* move_ease_in = CCEaseOut::create(moveto, 0.f);
		CCActionInterval* move_ease_in = CCEaseIn::create(moveto, 0.2f);
		CCFiniteTimeAction *seq = CCSequence::create(move_ease_in, callfun, NULL);
		sprite->runAction(seq);
		sprite->runAction(CCSequence::create(CCScaleTo::create(0.001f*distance, .6f, .6f), NULL));
		sprite->runAction(CCSequence::create(CCFadeTo::create(0.001f*distance, 0), NULL));

		CCArray* array = new CCArray();
		array->addObject(CCInteger::create(blockType));
		array->addObject(customer);

		sprite->setUserData(array);
	}
}



void DyTycoonManager::slotItemUse(DyItemType itemType)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyItemManager* itemManager = DyItemManager::sharedItemManager();
    m_dtItemSlot[itemManager->getSlotIndex(itemType)] = 0;
   
    m_progItemSlot[itemManager->getSlotIndex(itemType)]->setPercentage(0);
    m_progItemSlot[itemManager->getSlotIndex(itemType)]->setVisible(true);
    m_sprProgItemSlotBg[itemManager->getSlotIndex(itemType)]->setVisible(true);
    
    DyItem* item = itemManager->getItem(itemType);
    
    sprintf(lib->m_strTemp, "%d", item->m_cntAvailable);
    m_labelItemSlot[itemManager->getSlotIndex(itemType)]->setString(lib->m_strTemp);
    m_labelItemSlot[itemManager->getSlotIndex(itemType)]->setVisible(true);
    
    
    //CCMenuItemImage* menuItem = (CCMenuItemImage *)m_menuItem->getChildByTag(GAME_UI_TAG_MENUITEM_SLOT0+itemManager->getSlotIndex(itemType));
    //menuItem->setEnabled(false);
    
    m_enableSlotItem[itemManager->getSlotIndex(itemType)] = false;
    
    
}


void DyTycoonManager::slotItemEnable(int indexSlot, bool isEnable)
{
    //CCMenuItemImage* menuItem = (CCMenuItemImage *)m_menuItem->getChildByTag(GAME_UI_TAG_MENUITEM_SLOT0+indexSlot);
    m_enableSlotItem[indexSlot] = isEnable;
    
    if(isEnable)
    {
        m_progItemSlot[indexSlot]->setVisible(false);
        m_sprProgItemSlotBg[indexSlot]->setVisible(false);
        m_labelItemSlot[indexSlot]->setVisible(false);
    }
}


void DyTycoonManager::addMaterial(int id, CCPoint position)
{
	DyBlockManager* blockManager = DyBlockManager::sharedBlockManager();
    DyCustomerManager* custoerManager = DyCustomerManager::sharedCustomerManager();
    
//	YtLib* lib = YtLib::sharedLib();
	std::string tag = blockManager->getTagByID(id);
    
	bool added = false;
	CCPoint tgtPoint;
	CCPoint srcPoint = position;//m_layer->convertToNodeSpace(position);
	DyCustomer* customer;
	DyRecipeManager* recipeManager = DyRecipeManager::sharedRecipeManager();
    
	for(unsigned int i = 0;i < custoerManager->m_arrCustomerOrder->count();i++)
	{
		customer = (DyCustomer*)custoerManager->m_arrCustomerOrder->objectAtIndex(i);

		if(customer->m_character->m_status != DyCharacterStatus_Order)
			continue;

		for(unsigned j = 0;j < customer->m_arrTotMateiralStatus->count();j++)
		{
			DyMaterial* materialTot = (DyMaterial*)customer->m_arrTotMateiralStatus->objectAtIndex(j);

			if(materialTot->m_tag.compare(tag) == 0 && materialTot->m_count - materialTot->m_unAvailableCount > 0)
			{
				CCSprite* sprBallon = (CCSprite*)m_layer->getChildByTag(GAME_UI_TAG_SLOT_BALLOON_0+customer->m_slotidx);
				tgtPoint = sprBallon->getPosition();

				bool processed = false;
				for(unsigned int k = 0;k < customer->m_arrOrderRecipe->count();k++)
				{
					DyOrderRecipe* orderRecipe = (DyOrderRecipe*)customer->m_arrOrderRecipe->objectAtIndex(k);

					if(recipeManager->m_arrAvailableRecipes->containsObject(orderRecipe->m_info) == true)
					{
						for(unsigned l = 0;l < orderRecipe->m_arrMateiralStatus->count();l++)
						{
							DyMaterial* material = (DyMaterial*)orderRecipe->m_arrMateiralStatus->objectAtIndex(l);

							if(material->m_tag.compare(tag) == 0 && material->m_count > 0)
							{
								added = true;
                                customer->m_character->m_comming++;
                                
								material->m_count--;
								materialTot->m_count--;				
								customer->m_moveCnt++;

								processed = true;
								break;
							}
						}
					}
					if(processed)
						break;
				}

				break;
			}
		}

		if(added)
			break;
	}

	if(added)
	{
        
		DyBlockManager* blockManager = DyBlockManager::sharedBlockManager();

        blockManager->changeStatus();

        DyBlockInfo* blockInfo = blockManager->getBlockInfo(id);
		CCSprite* sprite = CCSprite::createWithSpriteFrameName(blockManager->getImageName(id));

		m_layer->addChild(sprite, ZORDER_GAME_MOVEBLOCKTOCUSTOMER);
		
		sprite->setPosition(srcPoint);

		float distance = ccpDistance(srcPoint, tgtPoint);
		CCCallFunc* callfun = CCCallFuncN::create(m_layer, callfuncN_selector(DyTycoonManager::endAnimationAdd));
		CCActionInterval* moveto = CCMoveTo::create(0.001f*distance, tgtPoint);
		CCActionInterval* move_ease_in = CCEaseIn::create(moveto, 0.2f);

		CCFiniteTimeAction *seq = CCSequence::create(move_ease_in, callfun, NULL);
		sprite->runAction(seq);
		sprite->runAction(CCSequence::create(CCScaleTo::create(0.001f*distance, .6f, .6f), NULL));
		sprite->runAction(CCSequence::create(CCFadeTo::create(0.001f*distance, 0), NULL));

		sprite->setUserData(customer);

        float delay = 0.1f;
        
        switch(blockInfo->m_sound_id)
        {
            case 0:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood00));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood00), delay);
                break;
            case 1:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood01));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood01), delay);
                break;
            case 2:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood02));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood02), delay);
                break;
            case 3:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood03));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood03), delay);
                break;
            case 4:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood04));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood04), delay);
                break;
            case 5:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood05));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood05), delay);
                break;
            case 6:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood06));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood06), delay);
                break;
            case 7:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood07));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood07), delay);
                break;
            case 8:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood08));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood08), delay);
                break;
            case 9:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood09));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood09), delay);
                break;
            case 10:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood10));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood10), delay);
                break;
            case 11:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood11));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood11), delay);
                break;

            case 12:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood12));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood12), delay);
                break;

            case 13:
                m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood13));
                m_layer->schedule(schedule_selector(DyTycoonManager::soundfood13), delay);
                break;

        }
	}
}


void DyTycoonManager::soundfood00(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_00);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood00));
    
}

void DyTycoonManager::soundfood01(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_01);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood01));
}

void DyTycoonManager::soundfood02(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_02);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood02));
}

void DyTycoonManager::soundfood03(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_03);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood03));
}

void DyTycoonManager::soundfood04(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_04);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood04));
}

void DyTycoonManager::soundfood05(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_05);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood05));
}

void DyTycoonManager::soundfood06(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_06);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood06));
}

void DyTycoonManager::soundfood07(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_07);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood07));
}

void DyTycoonManager::soundfood08(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_08);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood08));
}

void DyTycoonManager::soundfood09(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_09);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood09));
}

void DyTycoonManager::soundfood10(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_10);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood10));
}


void DyTycoonManager::soundfood11(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_11);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood11));
}


void DyTycoonManager::soundfood12(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_12);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood12));
}

void DyTycoonManager::soundfood13(float dt)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BLOCK_13);
    DyTycoonManager::sharedTycoonManager()->m_layer->unschedule(schedule_selector(DyTycoonManager::soundfood13));
}


void DyTycoonManager::endAnimationAdd(CCNode* node)
{
	DyCustomer* customer = (DyCustomer*)node->getUserData();

	if(customer->m_character->m_status < DyCharacterStatus_Walkout)
	{
		customer->m_moveCnt--;
        customer->m_character->m_comming--;
		if(customer->m_moveCnt == 0)
		{
			DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
			DyTycoonCommand* command = new DyTycoonCommand(DyTycoonCommandType_AddBlockToCustomerReal, customer, NULL, NULL, 0, 0, 0);
			tycoonManager->m_commandManager->addLast(command);
			command->release();
		
		}
	}
	node->removeFromParentAndCleanup(true);
}


void DyTycoonManager::endAnimationSpecialAdd(CCNode* node)
{
	CCArray* array = (CCArray*)node->getUserData();
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	CCInteger* blocktype = (CCInteger*)array->objectAtIndex(0);
	DyCustomer* customer = (DyCustomer*)array->objectAtIndex(1);

    
    customer->m_character->m_comming--;

	switch(blocktype->getValue())
	{
		case TmBlockType_Candy:
            if(customer->m_character->m_status < DyCharacterStatus_Walkout)
			{
				CCLayer* layerCharacter = (CCLayer*)tycoonManager->m_layer->getChildByTag(GAME_UI_TAG_SLOT_TOUCH_CHARACTER_0+customer->m_slotidx);
				CCPoint tgtPoint = ccp(layerCharacter->getPosition().x+layerCharacter->getContentSize().width/2, layerCharacter->getPosition().y+layerCharacter->getContentSize().height/2);
				
				if(customer->m_character->m_heartAni != NULL)
                {
                    customer->m_character->m_heartAni->removeFromParentAndCleanup(true);
					YtAniObject::unloadAniObject(customer->m_character->m_heartAni);
                }

				customer->m_character->m_heartAni = YtAniObject::loadAniObject(tycoonManager, "effect_explodingheart.ccbi");
				customer->m_character->m_heartAni->setPosition(tgtPoint);
				tycoonManager->m_layer->addChild(customer->m_character->m_heartAni, ZORDER_GAME_MOVE_OBJECT_UNDER);
			
				YtLib::sharedLib()->playEffect(YtLibSndEff_CANDY);
				
               DyTycoonCommand* command = new DyTycoonCommand(DyTycoonCommandType_AddSpecialBlockToCustomerReal, customer, NULL, NULL, TmBlockType_Candy, 0, 0);
				tycoonManager->m_commandManager->addLast(command);
				command->release();
			}
			break;
		case TmBlockType_MSG:
            if(customer->m_character->m_status < DyCharacterStatus_Walkout)
			{
				CCSprite* sprBalloon = (CCSprite*)tycoonManager->m_layer->getChildByTag(GAME_UI_TAG_SLOT_BALLOON_0+customer->m_slotidx);
				CCPoint tgtPoint = sprBalloon->getPosition();//ccp(layerPay->getPosition().x+layerPay->getContentSize().width/2, layerPay->getPosition().y+layerPay->getContentSize().height/2);

				CCSprite* sprMsg = CCSprite::createWithSpriteFrameName("threematch_ui_effect_msg00.png");
				sprMsg->setPosition(tgtPoint);
				sprMsg->setScale(0.5f);
				CCActionInterval* actionScale = CCScaleTo::create(0.6f, 1.0f, 1.0f);

				CCActionInterval* fadein = CCFadeIn::create(.3f);
				CCActionInterval* fadeout = CCFadeOut::create(.3f);

                
				sprMsg->runAction(CCSequence::create(actionScale, NULL));
				sprMsg->runAction(CCSequence::create(fadein, CCDelayTime::create(0.4f), fadeout, CCCallFuncN::create(this, callfuncN_selector(DyTycoonManager::endEffect)), NULL));
				sprMsg->runAction(CCRepeatForever::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_msg")));

				tycoonManager->m_layer->addChild(sprMsg, ZORDER_GAME_MOVE_OBJECT_UNDER);

				YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_MSG);
    
            
				DyTycoonCommand* command = new DyTycoonCommand(DyTycoonCommandType_AddSpecialBlockToCustomerReal, customer, NULL, NULL, TmBlockType_MSG, 0, 0);
				tycoonManager->m_commandManager->addLast(command);
				command->release();
			}
			break;
	}

	array->release();
	node->removeFromParentAndCleanup(true);
}


void DyTycoonManager::setRankPopupOver(DyRank* rankTarget)
{
    if(rankTarget == NULL)
        return;
    
    
    DyLib* lib = DyLib::sharedLib();

    DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
    
	CCLayer* layerQuestContent = (CCLayer*)m_questbar->getChildByTag(1000)->getChildByTag(1100);
    layerQuestContent->removeAllChildrenWithCleanup(true);

  
    sprintf(lib->m_strTemp, "%d위", rankTarget->m_order);
    CCLabelTTF* labelNext = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 20.f);
    labelNext->setColor(ccWHITE);
    labelNext->setAnchorPoint(ccp(0, 0.5));
    labelNext->setPosition(ccp(-90, 0));
    
    layerQuestContent->addChild(labelNext);
    
    CCSprite* sprPictureFrame = CCSprite::createWithSpriteFrameName("main_ui_rank_picture_frame.png");
    sprPictureFrame->setScale(0.8);
    CCSprite* sprPicture = friendManager->getPicture(rankTarget->m_friend, sprPictureFrame->getContentSize());
    if(sprPicture)
    {
        sprPictureFrame->addChild(sprPicture);
        sprPicture->setPosition(ccp(sprPictureFrame->getContentSize().width/2, sprPictureFrame->getContentSize().height/2));
    }
    sprPictureFrame->setAnchorPoint(ccp(0, 0.5));
    sprPictureFrame->setPosition(ccp(labelNext->getPosition().x + labelNext->getContentSize().width + 5, 2));
    
    layerQuestContent->addChild(sprPictureFrame);
    
    CCLabelTTF* labelOrder = CCLabelTTF::create("추월!", FONT_NAME_EXP, 20.f);
    labelOrder->setColor(ccWHITE);
    labelOrder->setAnchorPoint(ccp(0, 0.5));
    labelOrder->setPosition(ccp(sprPictureFrame->getPosition().x + sprPictureFrame->getContentSize().width, 0));
    layerQuestContent->addChild(labelOrder);
}


void DyTycoonManager::setRankPopup()
{
    DyLib* lib = DyLib::sharedLib();
    DyRankManager* rankManager = DyRankManager::sharedRankManager();
    DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
    
	CCLayer* layerQuestContent = (CCLayer*)m_questbar->getChildByTag(1000)->getChildByTag(1100);
    layerQuestContent->removeAllChildrenWithCleanup(true);
    
    
    DyRank* rankFront = rankManager->getCurrrentTarget(rankManager->currentCheckRankType(), m_crazy_point);
    
    if(rankFront)
    {
        CCLabelTTF* labelNext = CCLabelTTF::create("다음목표", FONT_NAME_EXP, 18.f);
        labelNext->setColor(ccWHITE);
        labelNext->setAnchorPoint(ccp(0, 0.5));
        labelNext->setPosition(ccp(-90, 0));
        
        layerQuestContent->addChild(labelNext);
        
        CCSprite* sprPictureFrame = CCSprite::createWithSpriteFrameName("main_ui_rank_picture_frame.png");
        sprPictureFrame->setScale(0.8);
        CCSprite* sprPicture = friendManager->getPicture(rankFront->m_friend, sprPictureFrame->getContentSize());
        if(sprPicture)
        {
            sprPictureFrame->addChild(sprPicture);
            sprPicture->setPosition(ccp(sprPictureFrame->getContentSize().width/2, sprPictureFrame->getContentSize().height/2));
        }
        sprPictureFrame->setAnchorPoint(ccp(0, 0.5));
        sprPictureFrame->setPosition(ccp(labelNext->getPosition().x + labelNext->getContentSize().width + 5, 2));
        
        layerQuestContent->addChild(sprPictureFrame);
        
        sprintf(lib->m_strTemp, "%d위", rankFront->m_order);
        CCLabelTTF* labelOrder = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 18.f);
        labelOrder->setColor(ccWHITE);
        labelOrder->setAnchorPoint(ccp(0, 0.5));
        labelOrder->setPosition(ccp(sprPictureFrame->getPosition().x + sprPictureFrame->getContentSize().width, 10));
        layerQuestContent->addChild(labelOrder);
        
        lib->setMoneyStyle(lib->m_strTemp2, rankFront->m_point);
        sprintf(lib->m_strTemp, "%s점", lib->m_strTemp2);
        CCLabelTTF* labelPoint = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 18.f);
        labelPoint->setColor(ccWHITE);
        labelPoint->setAnchorPoint(ccp(0, 0.5));
        labelPoint->setPosition(ccp(sprPictureFrame->getPosition().x + sprPictureFrame->getContentSize().width, -10));
        
        layerQuestContent->addChild(labelPoint);
    }
    else
    {
        CCLabelTTF* labelContent = CCLabelTTF::create("현재 1위중", FONT_NAME_EXP, 25.f);
        labelContent->setColor(ccWHITE);
        labelContent->setAnchorPoint(ccp(0.5, 0.5));
        labelContent->setPosition(ccp(0, 0));
        layerQuestContent->addChild(labelContent);
    }
}



bool DyTycoonManager::showQuestMessage(bool isAllCheck)
{
    if(m_quest->m_questtype == DyQuestType_Ignore)
        return false;
   
	YtLib* lib = YtLib::sharedLib();
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	CCLayer* layerQuestContent = (CCLayer*)m_questbar->getChildByTag(1000)->getChildByTag(1100);
	
	if(m_quest->m_questtype == DyQuestType_All)
	{
		if(isAllCheck)
		{
			if(m_quest->m_queststatus == DyQuestStatus_Progress)
				m_quest->m_queststatus = DyQuestStatus_Sucess;
			else
				return false;
		}

		sprintf(lib->m_strTemp, stageManager->m_curShopInfo->getValue("quest_pannel", stageManager->m_curLevel), 0);
	}
	else if(m_quest->m_questtype == DyQuestType_NumberLimit)
	{
		if(isAllCheck)
		{
			if(m_quest->m_queststatus == DyQuestStatus_Progress)
				m_quest->m_queststatus = DyQuestStatus_Sucess;
			else
				return false;
		}
        
		sprintf(lib->m_strTemp, stageManager->m_curShopInfo->getValue("quest_pannel", stageManager->m_curLevel), m_quest->m_countComplete);
	}
	else
	{
		if(isAllCheck)
			return false;

		sprintf(lib->m_strTemp, stageManager->m_curShopInfo->getValue("quest_pannel", stageManager->m_curLevel), m_quest->m_countComplete);
	}
    layerQuestContent->removeChildByTag(1, true);
    layerQuestContent->removeChildByTag(2, true);
    layerQuestContent->removeChildByTag(3, true);
    
	CCLabelTTF* labelContent = ((DyLayerGame*)m_layer)->buildQuest(layerQuestContent, lib->m_strTemp);
    layerQuestContent->setVisible(false);
    layerQuestContent->setPosition(ccp(50, -30)); // 왜 위치를 옮겨야하는지 잘 모르겠다.

	switch(m_quest->m_queststatus)
	{
		case DyQuestStatus_Sucess:
			{
				CCSprite* sprComplete = CCSprite::createWithSpriteFrameName("game_ui_qeust_complete0.png");
                sprComplete->setTag(1);
				layerQuestContent->addChild(sprComplete);

				CCSprite* sprCompleteGlow = CCSprite::createWithSpriteFrameName("game_ui_qeust_complete1.png");
                sprCompleteGlow->setTag(2);
				layerQuestContent->addChild(sprCompleteGlow);

				sprCompleteGlow->runAction(CCSequence::create(
					CCFadeOut::create(0.5), CCFadeIn::create(0.5),
					CCFadeOut::create(0.5), CCFadeIn::create(0.5),
					CCFadeOut::create(0.5), NULL));

				sprComplete->runAction(CCSequence::create(
					CCDelayTime::create(2.5),
					CCMoveTo::create(0.01f, ccp(0, -10)),
					CCScaleTo::create(0.01f, 0.5f),
					NULL));

				labelContent->runAction(CCSequence::create(
					CCFadeOut::create(0.01f),
					CCDelayTime::create(2.5),
					CCMoveTo::create(0.01f, ccp(0, 10)),
					CCScaleTo::create(0.01f, 0.8f),
					CCFadeIn::create(0.01f),
					NULL));
			}
			break;
		case DyQuestStatus_Failure:
			{
                YtLib::sharedLib()->playEffect(YtLibSndEff_Quest_Failure);
				CCSprite* sprFailed = CCSprite::createWithSpriteFrameName("game_ui_qeust_failed.png");
                sprFailed->setTag(2);
				layerQuestContent->addChild(sprFailed);

				sprFailed->runAction(CCSequence::create(
					CCDelayTime::create(2.5),
					CCMoveTo::create(0.01f, ccp(0, -10)),
					CCScaleTo::create(0.01f, 0.5f),
					NULL));

				labelContent->runAction(CCSequence::create(
					CCFadeOut::create(0.01f),
					CCDelayTime::create(2.5),
					CCMoveTo::create(0.01f, ccp(0, 10)),
					CCScaleTo::create(0.01f, 0.8f),
					CCFadeIn::create(0.01f),
					NULL));
			}
			break;
        default:
            break;
	}

	if(!isAllCheck)
		((DyLayerGame*)m_layer)->showQuestPopup(true, true);

    if(m_quest->m_queststatus == DyQuestStatus_Progress)
        YtLib::sharedLib()->playEffect(YtLibSndEff_Quest_Step_Success);
    
	if(m_questProcessComplete == false && m_quest->m_queststatus == DyQuestStatus_Sucess)
	{
        if(isAllCheck)
            ((DyLayerGame*)m_layer)->showQuestPopup(true, true);
        
        YtLib::sharedLib()->playEffect(YtLibSndEff_Quest_Success);
		m_questProcessComplete = true;
        
        CCSprite* sprGoldIcon = (CCSprite*)m_layer->getChildByTag(GAME_UI_TAG_DISP_GOLD_ICON);
        
        earnGold(ccp(sprGoldIcon->getPosition().x, sprGoldIcon->getPosition().y - 50), m_questbonus_gold, 0, 0, 2);
        
		return true;
	}
	return false;
}


void DyTycoonManager::questProcess(const char* tag, const char* subTag, int count)
{
	if(m_quest && m_quest->m_questMainTag.compare(tag) == 0)
	{
		switch(m_quest->m_queststatus)
		{
			case DyQuestStatus_Progress:
            {
                bool bCheck = false;
                if(m_quest->m_questSubTag.compare("ALL") == 0 || m_quest->m_questSubTag.compare(subTag) == 0)
                    bCheck = true;
                
                if(strcmp(tag, QUESTTAG_FOOD) == 0)
                {
                    if(strstr(subTag, m_quest->m_questSubTag.c_str()))
                        bCheck = true;
                    else
                        bCheck = false;
                }
                
                
                if(strcmp(tag, QUESTTAG_ITEM) == 0)
                {
                    bCheck = false;
                    
                    if(strcmp(m_quest->m_questSubTag.c_str(), "BASIC") == 0)
                    {
                        if(strcmp(subTag, "hint") == 0)
                        {
                            if(m_questItemBasic[0] == false)
                            {
                                bCheck = true;
                            }
                            m_questItemBasic[0] = true;
                        }
                        if(strcmp(subTag, "refresh") == 0)
                        {
                            if(m_questItemBasic[1] == false)
                            {
                                bCheck = true;
                            }
                            m_questItemBasic[1] = true;
                        }
                        if(strcmp(subTag, "timeincrease") == 0)
                        {
                            if(m_questItemBasic[2] == false)
                            {
                                bCheck = true;
                            }
                            m_questItemBasic[2] = true;
                        }
                    }
                    else if(strcmp(m_quest->m_questSubTag.c_str(), "NOTUSE") == 0)
                    {
                        bCheck = true;
                        count = 0;
                    }
                    else if(m_quest->m_questSubTag.compare(subTag) == 0)
                    {
                        bCheck = true;   
                    }
                }
                
                bool questFailDirect = false;
				if(bCheck)
				{
					if(count == 0)
					{
						if(m_quest->m_questtype == DyQuestType_All)
						{
                            questFailDirect = true;
						}
					}
					else
					{
						if(m_quest->m_questtype == DyQuestType_Number)
						{
							m_quest->m_countComplete += count;

							if(m_quest->m_countComplete == m_quest->m_countQuest)
                            {
								m_quest->m_queststatus = DyQuestStatus_Sucess;
                            }

							showQuestMessage(false);
						}
                        
                        if(m_quest->m_questtype == DyQuestType_NumberLimit)
						{
							m_quest->m_countComplete -= count;
                            
							if(m_quest->m_countComplete <= 0)
                            {
//                                if(m_quest->m_questSubTag.compare("candy") == 0)
//                                    
//                                else
                                questFailDirect = true;
                            }
                            else
                                showQuestMessage(false);
						}

					}
				}
                
                if(questFailDirect)
                {
                    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
                    
                    tycoonManager->m_quest->m_queststatus = DyQuestStatus_Failure;
                    tycoonManager->showQuestMessage(false);
                    
                    //DyCustomerManager::sharedCustomerManager()->outCustomers();
                    
                    tycoonManager->clearBlocksBeforeEnd();
                    if(tycoonManager->m_nottimesnd >= 0)
                    {
                        YtLib::sharedLib()->stopEffect(tycoonManager->m_nottimesnd);
                        tycoonManager->m_nottimesnd = -1;
                    }

                    
                    m_failWithQuest = true;
                }
            }
				break;
			case DyQuestStatus_Sucess:
				break;
			case DyQuestStatus_Failure:
				break;
		}
	}
}


DyTycoonManager* DyTycoonManager::sharedTycoonManager()
{
	if(g_sharedTycoonManager == NULL)
	{
		g_sharedTycoonManager = new DyTycoonManager();
	}
	return g_sharedTycoonManager;
}
