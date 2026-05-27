/*
DyCustomer.cpp
Doggy chef
@initialize at 121008

@history
121008	yoonsr	initialize
*/

#include "DyCustomer.h"

#include "DyLoader.h"

#include "DyRecipe.h"
#include "DyCustomer.h"
#include "DyStage.h"
#include "DyBlockInfo.h"
#include "DySceneGame.h"
#include "DyTycoon.h"
#include "DyTycoonCommand.h"
#include "CCextAnimatePacker.h"
#include "DyMachine.h"
#include "DyTutorial.h"

extern bool g_isDebug;

DyCustomerManager* g_sharedCustomerManager;



DyCustomer::DyCustomer(CCLayer* layer, int characterID, int slot, DyCustomerType customerType)
{
	static int id_gen = 0;
	m_idCharacter = id_gen++;
    
	m_layerCharacter = layer;
	m_moveCnt = 0;
	m_customerType = customerType;
    
	m_arrSellRecipe = new CCArray();
	m_arrOrderRecipe = new CCArray();
	m_arrTotMateiralStatus = new CCArray();
	m_pay = 0;
	m_payFeverMux = 0;
	m_payMux = 1;
	m_payTip = 0;

	m_deltaPay = 0;
	m_startBlinkPay = false;
	m_slotidx = slot;
	
	m_hasSide = false;
    m_isSideByCashier = false;
    
    m_dtDangling = 0.f;
    
    m_character = new DyCharacter(characterID, this, m_layerCharacter);
}


DyCustomer::~DyCustomer()
{
	m_arrSellRecipe->release();
	m_character->release();
	m_arrOrderRecipe->release();
	m_arrTotMateiralStatus->release();
}


void DyCustomer::addPay(int plus, float mux)
{
	if(m_character == NULL || m_character->m_status >= DyCharacterStatus_Walkout || m_character->m_status <= DyCharacterStatus_Walkin)
		return;
	
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

	m_pay += plus;
	if(mux < 0)
		m_payMux /= fabs(mux);
	else
		m_payMux *= mux;

	//
	// defense code
	if(m_payMux == 0)
		m_payMux = 1;

	CCSprite* sprBg = (CCSprite*)m_layerCharacter->getChildByTag(GAME_UI_TAG_SLOT_PRICE_0+m_slotidx);
	YtLib* lib = YtLib::sharedLib();
	if(m_payMux > 1)
	{
		sprintf(lib->m_strTemp, "%dx%.1f", m_pay, m_payMux);
		tycoonManager->m_labelDon[m_slotidx]->setColor(ccYELLOW);
	}
	else
	{
		sprintf(lib->m_strTemp, "%d", m_pay);
		tycoonManager->m_labelDon[m_slotidx]->setColor(ccWHITE);
	}
	tycoonManager->m_labelDon[m_slotidx]->setString(lib->m_strTemp);

	float scaleX = (tycoonManager->m_labelDon[m_slotidx]->getContentSize().width+20)/sprBg->getContentSize().width;

	sprBg->setScaleX(scaleX);
	tycoonManager->m_sprDonIcon[m_slotidx]->setPosition(ccp(-15+tycoonManager->m_labelDon[m_slotidx]->getPosition().x - tycoonManager->m_labelDon[m_slotidx]->getContentSize().width/2,
		tycoonManager->m_labelDon[m_slotidx]->getPosition().y));



	//CCActionInterval* action = CCScaleTo::create(.12f, 1.2f);
	//CCActionInterval* actionReverse = CCScaleTo::create(.05f, 1.0f);
	//label->stopAllActions();
	//if(tycoonManager->m_isFever || (isFever && mux > 0))
	//	label->runAction(CCRepeat::create(CCSequence::create(action, actionReverse, NULL), 1000));
	//else if(isFever && mux < 0)
	//{
	//}
	//else
	//	label->runAction(CCSequence::create(action, actionReverse, NULL));
}


//
// 완성 음식 처리
void DyCustomer::updateStatus(bool order)
{
    DyMachineManager* machineManager = DyMachineManager::sharedMachineManager();
    
	if(order && m_character->m_status != DyCharacterStatus_Order)
		return;

	YtLib* lib = YtLib::sharedLib();

	static CCPoint posFood[1+2+3] =
	{
		ccp(0+57, 0+36), 
		ccp(-30+57, 0+36), ccp(30+57, 0+36), 
		ccp(0+57, 20+36), ccp(-20+57, -15+36), ccp(20+57, -15+36),
	};

	float scaleFood[3] = {0.8f, 0.6f, 0.6f};

	int orderRecipeCnt = m_arrOrderRecipe->count();
	int posStart = 0;
	if(orderRecipeCnt == 1)
		posStart = 0;
	else if(orderRecipeCnt == 2)
		posStart = 1;
	else if(orderRecipeCnt == 3)
		posStart = 3;

	//DyBlockManager* blockManager = DyBlockManager::sharedBlockManager();
	
	CCSprite* sprBallon = (CCSprite*)m_layerCharacter->getChildByTag(GAME_UI_TAG_SLOT_BALLOON_0+m_slotidx);
	sprBallon->removeAllChildrenWithCleanup(true);
	CCSprite* sprDish = (CCSprite*)m_layerCharacter->getChildByTag(GAME_UI_TAG_SLOT_DISH_0+m_slotidx);
	sprDish->removeAllChildrenWithCleanup(true);
    
	if(order)
	{
		DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

		sprBallon->setVisible(true);
		tycoonManager->m_labelDon[m_slotidx]->setVisible(true);
		tycoonManager->m_sprDonIcon[m_slotidx]->setVisible(true);
		m_layerCharacter->getChildByTag(GAME_UI_TAG_SLOT_PRICE_0+m_slotidx)->setVisible(true);
		sprDish->setVisible(false);
	}

	
	for(unsigned k = 0;k < m_arrOrderRecipe->count();k++)
	{
		DyOrderRecipe* orderRecipe = (DyOrderRecipe*)m_arrOrderRecipe->objectAtIndex(k);
		int remainMaterial = 0;
		for(unsigned l = 0;l < orderRecipe->m_arrMateiralStatus->count();l++)
		{
			DyMaterial* material = (DyMaterial*)orderRecipe->m_arrMateiralStatus->objectAtIndex(l);
			remainMaterial += material->m_count;
		}
        
        CCSprite* sprRecipe;
		CCSprite* sprRecipe_Dish;

        if(orderRecipe->m_info->m_tag.compare("sam_vegetable") == 0 || orderRecipe->m_info->m_tag.compare("sam_vegetable") == 0)
        {
            DyMaterial* material = (DyMaterial*)orderRecipe->m_arrMateiralStatus->objectAtIndex(0);
			
            if(material->m_tag.compare("vegetable") == 0)
            {
                sprRecipe = CCSprite::createWithSpriteFrameName("recipe_korean_sam_vegetable_complete_base.png");
                sprRecipe_Dish = CCSprite::createWithSpriteFrameName("recipe_korean_sam_vegetable_complete_base.png");
            }
            else
            {
                sprRecipe = CCSprite::createWithSpriteFrameName("recipe_korean_sam_kimchi_complete_base.png");
                sprRecipe_Dish = CCSprite::createWithSpriteFrameName("recipe_korean_sam_kimchi_complete_base.png");
            }
            
            material = (DyMaterial*)orderRecipe->m_arrMateiralStatus->objectAtIndex(1);
			
            if(material->m_tag.compare("r_pork") == 0)
            {
                CCSprite* sprMaterial = CCSprite::createWithSpriteFrameName("recipe_korean_complete_pork.png");
                sprMaterial->setPosition(ccp(55, 45));
                sprRecipe->addChild(sprMaterial);
                
                sprMaterial = CCSprite::createWithSpriteFrameName("recipe_korean_complete_pork.png");
                sprMaterial->setPosition(ccp(55, 45));
                sprRecipe_Dish->addChild(sprMaterial);
            }
            else
            {
                CCSprite* sprMaterial = CCSprite::createWithSpriteFrameName("recipe_korean_complete_beef.png");
                sprMaterial->setPosition(ccp(55, 45));
                sprRecipe->addChild(sprMaterial);
                
                sprMaterial = CCSprite::createWithSpriteFrameName("recipe_korean_complete_beef.png");
                sprMaterial->setPosition(ccp(55, 45));
                sprRecipe_Dish->addChild(sprMaterial);
            }

            
            material = (DyMaterial*)orderRecipe->m_arrMateiralStatus->objectAtIndex(2);
            if(material->m_tag.compare("garlic") == 0)
            {
                CCSprite* sprMaterial = CCSprite::createWithSpriteFrameName("recipe_korean_complete_galic.png");
                sprMaterial->setPosition(ccp(55, 45));
                sprRecipe->addChild(sprMaterial);
                
                sprMaterial = CCSprite::createWithSpriteFrameName("recipe_korean_complete_galic.png");
                sprMaterial->setPosition(ccp(55, 45));
                sprRecipe_Dish->addChild(sprMaterial);
            }
            else if(material->m_tag.compare("pa") == 0)
            {
                CCSprite* sprMaterial = CCSprite::createWithSpriteFrameName("recipe_korean_complete_pa.png");
                sprMaterial->setPosition(ccp(55, 45));
                sprRecipe->addChild(sprMaterial);
                
                sprMaterial = CCSprite::createWithSpriteFrameName("recipe_korean_complete_pa.png");
                sprMaterial->setPosition(ccp(55, 45));
                sprRecipe_Dish->addChild(sprMaterial);
            }
            else
            {
                CCSprite* sprMaterial = CCSprite::createWithSpriteFrameName("recipe_korean_complete_pepper.png");
                sprMaterial->setPosition(ccp(55, 45));
                sprRecipe->addChild(sprMaterial);
                
                sprMaterial = CCSprite::createWithSpriteFrameName("recipe_korean_complete_pepper.png");
                sprMaterial->setPosition(ccp(55, 45));
                sprRecipe_Dish->addChild(sprMaterial);
            }
        }
        else
        {
            sprRecipe = CCSprite::createWithSpriteFrameName(orderRecipe->m_info->m_urlResultImage.c_str());
            sprRecipe_Dish = CCSprite::createWithSpriteFrameName(orderRecipe->m_info->m_urlResultImage.c_str());
        }
        
        
		sprBallon->addChild(sprRecipe);
		sprDish->addChild(sprRecipe_Dish);
		
		//CCLog("sprRecipe->getContentSize().width = %f", sprBallon->getContentSize().width);
		sprRecipe->setAnchorPoint(ccp(orderRecipe->m_info->m_anchorPoint.x, orderRecipe->m_info->m_anchorPoint.y));
		sprRecipe->setScale(scaleFood[orderRecipeCnt-1]);
		sprRecipe->setPosition(ccp(posFood[posStart+k].x+30, posFood[posStart+k].y+50));

		sprRecipe_Dish->setAnchorPoint(ccp(0.5, 0.5));
		sprRecipe_Dish->setScale(scaleFood[orderRecipeCnt-1]);
		sprRecipe_Dish->setPosition(ccp(posFood[posStart+k].x, posFood[posStart+k].y+10));

		if(remainMaterial > 0)
			sprRecipe->setOpacity(175);
	}

	if(m_hasSide)
	{
		CCSprite* spriteSide = machineManager->createSide();
		CCSprite* spriteSide_Dish = machineManager->createSide();
		sprBallon->addChild(spriteSide);
		sprDish->addChild(spriteSide_Dish);
		spriteSide->setAnchorPoint(ccp(0.5, 0.0));
		spriteSide->setPosition(ccp(35, 32));
		spriteSide_Dish->setAnchorPoint(ccp(0.5, 0.0));
		spriteSide_Dish->setPosition(ccp(7, 5));
        
        if(this->m_isSideByCashier)
        {
            CCSprite* sprSideGlow = machineManager->createSideGlow();
            spriteSide->addChild(sprSideGlow, -1);
            sprSideGlow->setPosition(ccp(spriteSide->getContentSize().width/2, spriteSide->getContentSize().height/2));
            
            CCSprite* sprSideGlow_Dish = machineManager->createSideGlow();
            spriteSide_Dish->addChild(sprSideGlow_Dish, -1);
            sprSideGlow_Dish->setPosition(ccp(spriteSide_Dish->getContentSize().width/2, spriteSide_Dish->getContentSize().height/2));
        }
	}

	//
	// 상태 업데이트
	int remainCnt = 0;
	for(unsigned i = 0;i < m_arrTotMateiralStatus->count();i++)
	{	
		DyMaterial* materialTot = (DyMaterial*)m_arrTotMateiralStatus->objectAtIndex(i);
		sprintf(lib->m_strTemp, "%d", materialTot->m_count);
		materialTot->m_labelCount->setString(lib->m_strTemp);
		remainCnt += materialTot->m_count;
	}

	//
	// 모든 레시피 완성하여 판매완료
	if(remainCnt == 0 && m_character->m_status == DyCharacterStatus_Order)
	{
        DyStageManager* stageManager = DyStageManager::sharedStageManager();
        
        //m_character->m_foodReady = true;
        
		YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_COMPLETE);

		sprBallon->setVisible(false);
		CCLayer* layerBlockInfo = (CCLayer*)m_layerCharacter->getChildByTag(GAME_UI_TAG_SLOT_BLOCK_0 + m_slotidx);
		layerBlockInfo->setVisible(false);

		CCSprite* sprEffect = CCSprite::createWithSpriteFrameName("threematch_ui_effect_balloonbang00.png");
		CCCallFunc* callfun_effect = CCCallFuncN::create(m_layerCharacter, callfuncN_selector(DyCustomer::endAnimationDish));
		sprEffect->runAction(CCSequence::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_ballonbang"), callfun_effect, NULL));
		sprEffect->setUserData(this);

		m_layerCharacter->addChild(sprEffect, ZORDER_GAME_MOVE_OBJECT_UNDER);
		sprEffect->setPosition(sprBallon->getPosition());

		sprDish->setVisible(true);
        
        
		sprDish->setOpacity(0);
        
        if(stageManager->m_curShopType != DyStageType_Pizza)
            sprDish->runAction(CCSequence::create(/*CCDelayTime::create(0.1f), */CCFadeIn::create(.5f), NULL));

		CCArray *children = sprDish->getChildren();
		for(unsigned int i = 0;i < children->count();i++)
		{
			CCSprite* sprChild = (CCSprite*)children->objectAtIndex(i);
			sprChild->setOpacity(0);
			sprChild->runAction(CCSequence::create(/*CCDelayTime::create(0.1f), */CCFadeIn::create(.5f), NULL));
		}
	}

	updateTotalMaterial(true);
}



int DyCustomer::getOrderMaterialCnt()
{
	 CCArray* totMatireal = new CCArray();
    
	//
	//
	for(unsigned int i = 0;i < m_arrOrderRecipe->count();i++)
	{
		DyOrderRecipe* orderRecipe = (DyOrderRecipe*)m_arrOrderRecipe->objectAtIndex(i);
        
		for(unsigned int j = 0;j < orderRecipe->m_arrMateiralStatus->count();j++)
		{
			DyMaterial* material = (DyMaterial*)orderRecipe->m_arrMateiralStatus->objectAtIndex(j);
            
			bool isHas = false;
            
			for(unsigned int k = 0;k < totMatireal->count();k++)
			{
				DyMaterial* materialTot = (DyMaterial*)totMatireal->objectAtIndex(k);
                
				if(materialTot->m_tag.compare(material->m_tag) == 0)
				{
					isHas = true;
					break;
				}
			}
            
            
			if(isHas == false)
			{
				DyMaterial* materialTot = new DyMaterial();
				
                materialTot->m_unAvailableCount = 0;
				materialTot->m_count = material->m_count;
				materialTot->m_tag = material->m_tag;
                
				totMatireal->addObject(materialTot);
				materialTot->release();
			}
		}
	}
    
    int count = totMatireal->count();
    totMatireal->release();
    
    return count;
}



void DyCustomer::updateTotalMaterial(bool update)
{
	DyBlockManager* blockManager = DyBlockManager::sharedBlockManager();
	//DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	DyRecipeManager* recipeManager = DyRecipeManager::sharedRecipeManager();

	YtLib* lib = YtLib::sharedLib();

	CCArray* totMatireal;
	CCArray *totMaterialStatusNew = NULL;

	if(update)
	{
		totMaterialStatusNew = new CCArray();
		totMatireal = totMaterialStatusNew;
	}
	else
	{
		m_arrTotMateiralStatus->removeAllObjects();
		totMatireal = m_arrTotMateiralStatus;
	}
	//
	//
	for(unsigned int i = 0;i < m_arrOrderRecipe->count();i++)
	{
		DyOrderRecipe* orderRecipe = (DyOrderRecipe*)m_arrOrderRecipe->objectAtIndex(i);

		for(unsigned int j = 0;j < orderRecipe->m_arrMateiralStatus->count();j++)
		{
			DyMaterial* material = (DyMaterial*)orderRecipe->m_arrMateiralStatus->objectAtIndex(j);

			bool isHas = false;

			for(unsigned int k = 0;k < totMatireal->count();k++)
			{
				DyMaterial* materialTot = (DyMaterial*)totMatireal->objectAtIndex(k);

				if(materialTot->m_tag.compare(material->m_tag) == 0)
				{
					isHas = true;
					if(recipeManager->m_arrAvailableRecipes->containsObject(orderRecipe->m_info) == false)
						materialTot->m_unAvailableCount += material->m_count;
					materialTot->m_count += material->m_count;
					break;
				}
			}


			if(isHas == false)
			{
				DyMaterial* materialTot = new DyMaterial();
				//materialTot->m_id = blockManager->getIdByTag(material->m_tag.c_str());
				materialTot->m_unAvailableCount = 0;
				if(recipeManager->m_arrAvailableRecipes->containsObject(orderRecipe->m_info) == false)
					materialTot->m_unAvailableCount += material->m_count;

				materialTot->m_count = material->m_count;
				materialTot->m_tag = material->m_tag;

				totMatireal->addObject(materialTot);
				materialTot->release();
			}
		}
	}
	
	if(totMaterialStatusNew != NULL)
	{
		for(unsigned int i = 0;i < totMaterialStatusNew->count();i++)
		{
			DyMaterial* materialTotNew = (DyMaterial*)totMaterialStatusNew->objectAtIndex(i);
			DyMaterial* materialTot = (DyMaterial*)m_arrTotMateiralStatus->objectAtIndex(i);

			materialTot->m_unAvailableCount = materialTotNew->m_unAvailableCount;
		}
		totMaterialStatusNew->release();
	}
	
	CCLayer* layerBlockInfo  = (CCLayer*)m_layerCharacter->getChildByTag(GAME_UI_TAG_SLOT_BLOCK_0 + m_slotidx);
	layerBlockInfo->removeAllChildrenWithCleanup(true);
	static CCPoint type4posarr[4] = {
		ccp(-30, 30+8), ccp(30, 30+8), ccp(-30, -30+8), ccp(30, -30+8)
	};

	static CCPoint type9posarr[9] = {
		ccp(-38+3, 40), ccp(0+3, 40), ccp(38+3, 40),
		ccp(-38+3, 0), ccp(0+3, 0), ccp(38+3, 0),
		ccp(-38+3, -40), ccp(0+3, -40), ccp(38+3, -40),
	};

	for(unsigned int i = 0;i < m_arrTotMateiralStatus->count();i++)
	{	
		CCSprite* bg = CCSprite::createWithSpriteFrameName("game_ui_block_socket.png");

		CCPoint position;
		float scaleBox;
		float scaleMaterial;
		float scaleNumber;

		if(m_arrTotMateiralStatus->count() <= 4)
		{
			position = type4posarr[i];
			scaleBox = 1.0f;
			scaleMaterial = 0.6f;
			scaleNumber = 1.0f;
		}
		else
		{
			position = type9posarr[i];
			scaleBox = 0.75f;
			scaleMaterial = 0.45f;
			scaleNumber = .8f;
		}
		bg->setPosition(position);
		bg->setScale(scaleBox);
		layerBlockInfo->addChild(bg);

		DyMaterial* materialTot = (DyMaterial*)m_arrTotMateiralStatus->objectAtIndex(i);
		CCSprite* sprMaterial = CCSprite::createWithSpriteFrameName(blockManager->getImageNameByTag(materialTot->m_tag.c_str()));
		sprMaterial->setScale(scaleMaterial);
		sprMaterial->setPosition(position);
		layerBlockInfo->addChild(sprMaterial);

		if(materialTot->m_count == materialTot->m_unAvailableCount && materialTot->m_unAvailableCount > 0)
		{
			CCSprite* blind = CCSprite::createWithSpriteFrameName("game_ui_block_socket.png");
			blind->setPosition(position);
			blind->setScale(scaleBox);
			layerBlockInfo->addChild(blind);
		}

		CCSprite* sprNumberBg = CCSprite::createWithSpriteFrameName("game_ui_block_count_bg.png");
		sprNumberBg->setScale(scaleNumber);
		sprNumberBg->setPosition(ccp(position.x + bg->getContentSize().width*scaleBox/2-7*scaleNumber,
			position.y + bg->getContentSize().height*scaleBox/2-7*scaleNumber));
		layerBlockInfo->addChild(sprNumberBg);

		sprintf(lib->m_strTemp, "%d", materialTot->m_count);
		materialTot->m_labelCount = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 20*scaleNumber);
		materialTot->m_labelCount->setColor(ccYELLOW);
		materialTot->m_labelCount->setPosition(sprNumberBg->getPosition());

		layerBlockInfo->addChild(materialTot->m_labelCount);
	}
}


void DyCustomer::updateRecipe()
{
	updateTotalMaterial(true);
}


void DyCustomer::endAnimationDish(CCNode* node)
{
	DyCustomer* customer = (DyCustomer*)node->getUserData();

	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, customer->m_character, NULL, NULL, DyCharacterStatus_Like, customer->m_idCharacter<<2 | customer->m_slotidx, 0);
	tycoonManager->m_commandManager->addLast(commandTycoon);
	commandTycoon->release();
	
	node->setVisible(false);
    node->removeFromParentAndCleanup(true);
}


DyCustomerManager::DyCustomerManager()
{
    m_arrCustomerOrder = new CCArray();
	
    for(int i = 0;i < MAX_SLOT;i++)
	{
		m_isOccupied[i] = false;
		m_arrCustomer[i] = NULL;
	}
}


DyCustomerManager::~DyCustomerManager()
{
    m_arrCustomerOrder->release();
}



void DyCustomerManager::initCustomers()
{
    m_isCreatableCustomer = true;
    
	m_isEnteredGuestCustomer = false;
    m_isCreateOpportunityGeast = false;
    
	m_cntCustomer = 0;
    m_cntCustomerAccu = 0;
    m_cntCustomerSuccess = 0;
    
    for(int i = 0;i < MAX_SLOT;i++)
	{
		m_isOccupied[i] = false;
		m_arrCustomer[i] = NULL;
	}
	
	m_deltaCreateCustomerTime = 0;
	
	for(int i = 0;i < MAX_SLOT;i++)
		m_isOccupied[i] = false;
    


}


void DyCustomerManager::outCustomers()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    for(unsigned int i = 0;i < m_arrCustomerOrder->count();i++)
    {
        DyCustomer* customer = (DyCustomer*)m_arrCustomerOrder->objectAtIndex(i);
        
        if(customer->m_character->m_status < DyCharacterStatus_Walkout)
        {
            DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, customer->m_character, NULL, NULL, DyCharacterStatus_Walkout, customer->m_idCharacter<<2 | customer->m_slotidx, 0);
            tycoonManager->m_commandManager->addLast(commandTycoon);
            commandTycoon->release();

        }
        
        //customer->m_pay = 0;
    }
    
    m_isCreatableCustomer = false;
}


void DyCustomerManager::pauseCustomers()
{
    for(unsigned int i = 0;i < m_arrCustomerOrder->count();i++)
	{
		DyCustomer* customer = (DyCustomer*)m_arrCustomerOrder->objectAtIndex(i);
		customer->m_character->m_animation->pauseAnimation();
	}

}


void DyCustomerManager::resumeCustomers()
{
    for(unsigned int i = 0;i < m_arrCustomerOrder->count();i++)
	{
		DyCustomer* customer = (DyCustomer*)m_arrCustomerOrder->objectAtIndex(i);
		customer->m_character->m_animation->resumeAnimation();
		customer->updateRecipe();
	}

}


void DyCustomerManager::updateCustomer(float dt)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
	for(unsigned int i = 0;i < MAX_SLOT;)
	{
		DyCustomer* customer = m_arrCustomer[i];
		if(customer == NULL)
		{
			i++;
			continue;
		}
        
        if(customer->m_customerType == DyCustomerType_Delivery && customer->m_character->m_status == DyCharacterStatus_Like)
        {
            customer->m_deltaPay += dt;
        
            if(customer->m_deltaPay > PAY_WANING_TIME && customer->m_startBlinkPay == false)
            {
                CCActionInterval* blink = CCBlink::create(1.f, 5);
                customer->m_character->m_layerTable->runAction(CCRepeat::create(CCSequence::create(blink, NULL), 1000));
                
                customer->m_startBlinkPay = true;
            }
            else if(customer->m_deltaPay > PAY_WAIT_TIME)
            {
                customer->m_character->m_layerTable->stopAllActions();
                customer->m_character->m_layerTable->setVisible(false);
                customer->m_character->m_layerTable->removeAllChildrenWithCleanup(true);
                customer->m_character->m_sprPhone = NULL;
                
                DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, customer->m_character, NULL, NULL, DyCharacterStatus_Walkout, customer->m_idCharacter<<2 | customer->m_slotidx, 0);
                tycoonManager->m_commandManager->addLast(commandTycoon);
                commandTycoon->release();
            }
            
            i++;
        }
		else if(customer->m_character->m_status == DyCharacterStatus_Destroy)
		{
			if(customer->m_pay > 0)
			{
				//CCLayer* layerDon = (CCLayer*)tycoonManager->m_layer->getChildByTag(GAME_UI_TAG_SLOT_PAY_0 + customer->m_slotidx);
                
				customer->m_deltaPay += dt;
                
                DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
                DyTutorial* tutorial = tutorialManager->getCurrentTutorial();
                
                if(tutorial && tutorial->m_command_type == DyTutorialCommandType_GetMoney)
                {
                    customer->m_deltaPay = 0;
                }
                
                else if(tutorial && tutorial->m_command_type == DyTutorialCommandType_Sell)
                {
                    customer->m_deltaPay = 0;
                    tutorial->m_posIndicator = ccp(customer->m_character->m_layerTable->getPosition().x + 100, customer->m_character->m_layerTable->getPosition().y + 100);
                    
                }
                else if(tutorial && tutorial->m_tutorial_type == DyTutorialType_Delay)
                {
                    customer->m_deltaPay = 0;
                }
                else if(tutorial && tutorial->m_tutorial_type == DyTutorialType_Message)
                {
                    customer->m_deltaPay = 0;
                }
                else
                {
                    if(DyStaffManager::sharedStaffManager()->processCount(customer->m_deltaPay))
                    {
                        tycoonManager->m_aniCashier->m_actionManager->runAnimations("work");
                        
                        tycoonManager->sellRecipe(customer, true);
                    }
                    
                    else if(customer->m_deltaPay > PAY_WANING_TIME && customer->m_startBlinkPay == false)
                    {
                        CCActionInterval* blink = CCBlink::create(1.f, 5);
                        customer->m_character->m_layerTable->runAction(CCRepeat::create(CCSequence::create(blink, NULL), 1000));
                        
                        customer->m_startBlinkPay = true;
                    }
                    else if(customer->m_deltaPay > PAY_WAIT_TIME)
                    {
                        //
                        // 돈을 못받음
                        tycoonManager->questProcess(QUESTTAG_ETC, "GOLD", 0);
                        
                        customer->m_pay = 0;
                        customer->m_character->m_layerTable->stopAllActions();
                        customer->m_character->m_layerTable->setVisible(false);
                        customer->m_character->m_layerTable->removeAllChildrenWithCleanup(true);
                        customer->m_character->m_sprPhone = NULL;
                    }
                }
                
				i++;
			}
			else
			{
				m_isOccupied[customer->m_slotidx] = false;
				m_arrCustomer[customer->m_slotidx] = NULL;
                
				m_cntCustomer--;
                
				m_arrCustomerOrder->removeObject(customer);
                
                //checkGameEnd();
            }
		}
		else
		{
            if(customer->m_character->m_status == DyCharacterStatus_Order)
			{
				customer->m_character->m_dtHeart += dt;
				if(customer->m_character->m_dtHeart > 0.2f)
				{
					customer->m_character->m_dtHeart = 0;
					customer->m_character->updateHeart();
				}
			}
            else if(customer->m_character->m_status == DyCharacterStatus_None)
            {
                customer->m_dtDangling += dt;
                
                if(customer->m_dtDangling > 2.0f)
                {
                    //CCMessageBox("이거 뜨면 알려줘", "알림");
                    
                    customer->m_character->m_status = DyCharacterStatus_Destroy;
                    customer->m_pay = 0;
                }
            }
            
			i++;
		}
	}
    
    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
    if(tutorial && tutorial->m_tutorial_type == DyTutorialType_Customer)
    {
        DyCustomer* customer = createCustomer(false);
        customer->m_character->m_heartLife = tutorial->m_paramInt0*0.1f;
        customer->m_character->updateHeart();
        
        DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
    }
    
    if(tycoonManager->m_closeTime == true)
	{
		if(m_cntCustomer == 0 && m_isHasGuestCustomer && !m_isEnteredGuestCustomer)
		{
			m_isEnteredGuestCustomer = true;
            createCustomer(true);
		}
	}
    else
    {
        m_deltaCreateCustomerTime += dt;
        if(m_cntCustomer == 0 || (m_deltaCreateCustomerTime > m_customer_term && m_cntCustomer < MAX_SLOT))
        {
            m_deltaCreateCustomerTime = 0;
            createCustomer(false);
        }
    }
}

int DyCustomerManager::getSlotAvailable(bool isGuest)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(tycoonManager->m_trouble_table_cnt > 0)
    {
        m_isOccupied[tycoonManager->m_trouble_table_idx[0]] = true;
    }
    if(tycoonManager->m_trouble_table_cnt > 1)
    {
        m_isOccupied[tycoonManager->m_trouble_table_idx[1]] = true;
    }
    
    if(m_isOccupied[0] && m_isOccupied[1] && m_isOccupied[2])
        return -1;
    
    int slot;
    if(isGuest)
    {
        slot = 1;
        
        if(tycoonManager->m_trouble_table_cnt > 0)
        {
            slot = rand()%MAX_SLOT;
            while(m_isOccupied[slot] == true)
            {
                slot = rand()%MAX_SLOT;
            }
        }
    }
    else
    {
        slot = rand()%MAX_SLOT;
        while(m_isOccupied[slot] == true)
        {
            slot = rand()%MAX_SLOT;
        }
    }
    return slot;
}


DyCustomer* DyCustomerManager::createCustomer(bool isGuest)
{
    //DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    DyCustomerType customerType = DyCustomerType_Normal;
    
    if(tycoonManager->m_isCrazyMode && tycoonManager->m_crazyRemainHeart == 0)
    {
        return NULL;
    }
    
    if(!m_isCreatableCustomer)
        return NULL;
    
    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
	
    if(tutorialManager->m_pauseBoard)
    {
        return NULL;
    }

    
    //
    // 위치와 캐릭터 선택
	DyCharacterManager* characterManager = DyCharacterManager::sharedCharacterManager();
    //DyCharacterInfo* characterInfo;
    int characterID;
    
    int slot;
    
    if(isGuest)
    {
        //characterInfo = (DyCharacterInfo*)characterManager->m_arrCharaterInfo->objectAtIndex(7);
        slot = getSlotAvailable(true);
        if(slot < 0)
            return NULL;
        
        customerType = DyCustomerType_Gaest;
        characterID = -1;
    }
    else
    {
        //
        // 캐릭터 선택
        DyStageManager* stageManager = DyStageManager::sharedStageManager();
        
        CCArray* animal = stageManager->m_curShopInfo->getArray("order_animal", stageManager->m_curLevel);
        
        if(animal->count() > 2)
        {
            CCString* _strAnimal = (CCString*)animal->objectAtIndex(2);
            
            int probability = atoi(((CCString*)animal->objectAtIndex(3))->getCString());
            
            int percent = rand()%100;
            
            if(percent < probability)
                characterID = characterManager->getChracterID(_strAnimal->getCString());
            else
                characterID = rand()%characterManager->m_cntAvailable;
        }
        else
        {
            characterID = rand()%characterManager->m_cntAvailable;
        }
        
        //
        // 슬롯 선택
        slot = getSlotAvailable(false);
        
        if(slot < 0)
            return NULL;

        customerType = DyCustomerType_Normal;
        
        
        if(!tycoonManager->m_isCrazyMode)
        {
            int delivery_frequence = atoi(stageManager->m_curShopInfo->getValue("delivery_frequence", stageManager->m_curLevel));
            
            if(delivery_frequence == 0)
            {
                characterID = -1;
                customerType = DyCustomerType_Delivery;
            }
            else if(delivery_frequence > 0 && m_cntCustomerAccu > 0)
            {
                if(m_cntCustomerAccu%delivery_frequence == 0)
                {
                    characterID = -1;
                    customerType = DyCustomerType_Delivery;

                }
            }
        }
        
//        characterID = -1;
//        customerType = DyCustomerType_Delivery;

    }
    

    
   DyTutorial* tutorial = tutorialManager->getCurrentTutorial();
    
    if(tutorial && tutorial->m_tutorial_type == DyTutorialType_OrderRecipe)
    {
        slot = 1;
    }
    if(tutorial && tutorial->m_tutorial_type == DyTutorialType_OrderRecipeByDelivery)
    {
        slot = 1;
        characterID = -1;
        customerType = DyCustomerType_Delivery;
        m_cntCustomerAccu = -1;
    }
    else if(tutorial && tutorialManager->m_index == 6)
    {
        return NULL;
    }

    DyCustomer* customer = new DyCustomer(tycoonManager->m_layer, characterID, slot, customerType);
    
    //
    // 배달 음식은 우선순위가 높다
    if(customerType == DyCustomerType_Delivery)
    {
        bool isAdded = false;
        for(int i = m_arrCustomerOrder->count() - 1;i >= 0;i--)
        {
            DyCustomer* _customer = (DyCustomer*)m_arrCustomerOrder->objectAtIndex(i);
            if(_customer->m_customerType == DyCustomerType_Delivery)
            {
                m_arrCustomerOrder->insertObject(customer, i+1);
                isAdded = true;
                break;
            }
        }
        if(!isAdded)
        {
            m_arrCustomerOrder->insertObject(customer, 0);
        }
    }
    else
        m_arrCustomerOrder->addObject(customer);
    
    customer->release();
    
    m_isOccupied[slot] = true;
    m_arrCustomer[slot] = customer;
	
    m_cntCustomer++;
    m_cntCustomerAccu++;
    
    if(g_isDebug)
    {
        CCLog("손님 갯수 = %d", m_cntCustomerAccu);
    }
	tycoonManager->setOrderRecipe(customer);
    
    DyBlockManager::sharedBlockManager()->changeStatus();
    
    return customer;
}



DyCustomerManager* DyCustomerManager::sharedCustomerManager()
{
	if(g_sharedCustomerManager == NULL)
	{
		g_sharedCustomerManager = new DyCustomerManager();
	}
	return g_sharedCustomerManager;
}