/*
DyLayerShopCostume.cpp
Doggy chef
@initialize at 121207

@history
121207	yoonsr	initialize
*/


#include "DyLayerShop.h"
#include "DyLayerShopCostume.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyBlockInfo.h"
#include "DyPopup.h"

#include "DyTycoon.h"

DyLayerShopCostumeTab::DyLayerShopCostumeTab(CCRect rect):CCTabView(rect)
{
    m_leftMargin = 60.0;
	m_upMargin = 0;
    m_midPad = 5.0;
	m_selectedTab = 0;
	m_contentOffsetX = 325;
	m_contentOffsetY = 20;

	m_thick = 50;
}


DyLayerShopCostume::DyLayerShopCostume()
{
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("costume.plist");
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("doggy.plist");
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("doggy2.plist");

	m_startTab = 0;
	for(int i = 0;i < 5;i ++)
	{
		m_arrDefaultBox[i] = new CCArray();
		m_arrCostumes[i] = new CCArray();
		m_arrLock[i] = new CCArray();
		m_tabScrollView[i] = NULL;
		m_selectIndex[i] = -1;
	}
	m_tabView = NULL;
	m_doggy = NULL;
	m_arrCostume = NULL;
	m_labelNotiShop = NULL;
}

DyLayerShopCostume::~DyLayerShopCostume()
{
	m_contentLayer->release();

	for(int i = 0;i < 5;i++)
	{
		m_arrDefaultBox[i]->release();
		m_arrCostumes[i]->release();
		m_arrLock[i]->release();
	}
	if(m_doggy)
		YtAniObject::unloadAniObject(m_doggy);

	if(m_arrCostume)
		m_arrCostume->release();
}


void DyLayerShopCostume::enableBoxButton(int tab, int index, bool enable)
{
	if(index < 0)
		return;

	DyCostume* costume = (DyCostume*)m_arrCostumes[tab]->objectAtIndex(index);

	m_tabScrollView[tab]->getContent()->getChildByTag((int)tab*10000+index*100+1)->setVisible(enable);
	if(m_shopType == DyLayerShopType_Normal)
		m_tabScrollView[tab]->getContent()->getChildByTag((int)tab*10000+index*100+2)->setVisible(enable);

	if(costume->m_has)
		m_tabScrollView[tab]->getContent()->getChildByTag((int)tab*10000+index*100+2)->setVisible(false);

	if(enable)
		m_selectIndex[tab] = index;
	else if(enable == false && m_selectIndex[tab] == index)
		m_selectIndex[tab] = -1;
}


void DyLayerShopCostume::scrollViewTouchContent(CCScrollView* view, CCPoint position)
{
	DyLib* lib = DyLib::sharedLib();
	DyStageManager* stageManager = DyStageManager::sharedStageManager();

	int select_index = -1;
	for(unsigned int i = 0;i < m_arrDefaultBox[m_currentType]->count();i++)
	{
		CCScale9Sprite *background = (CCScale9Sprite *)m_arrDefaultBox[m_currentType]->objectAtIndex(i);

		if(background->boundingBox().containsPoint(position))
		{
			select_index = i;
		}
	}

	if(select_index >= 0)
	{	
		if(((CCInteger*)m_arrLock[m_currentType]->objectAtIndex(select_index))->getValue() == 1)
		{
			DyCostume* costume = (DyCostume*)m_arrCostumes[m_currentType]->objectAtIndex(select_index);

            if(costume->m_condition_type == DyCostumeLockType_NoneOrShop)
            {
                sprintf(lib->m_strTemp, STRING_shop_costume,
                    stageManager->m_shopinfo->getValue("name_kor", costume->m_condition_value));

                DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);

                popup->setText(lib->m_strTemp);
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);

                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
            }
            if(costume->m_condition_type == DyCostumeLockType_Gold)
            {
                lib->setMoneyStyle(lib->m_strTemp2, costume->m_condition_value);
                sprintf(lib->m_strTemp, STRING_COSTUME_GOLD_LOCK, lib->m_strTemp2);
                
                DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
                
                popup->setText(lib->m_strTemp);
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
            }
            if(costume->m_condition_type == DyCostumeLockType_Crazy)
            {
                DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
                
                popup->setText(STRING_COSTUME_CRAZY_LOCK);
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
            }
            if(costume->m_condition_type == DyCostumeLockType_Roulette)
            {
                DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
                
                sprintf(lib->m_strTemp, STRING_COSTUME_ROULETTE_LOCK, DyCostumeManager::sharedCostumeManager()->getCostume(costume->m_condition_string)->m_name.c_str());
                
                popup->setText(lib->m_strTemp);
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
            }

			return;
		}

		for(unsigned int i = 0;i < m_arrDefaultBox[m_currentType]->count();i++)
		{
			view->getContent()->getChildByTag((int)m_currentType*10000+i*100+1)->setVisible(false);
			view->getContent()->getChildByTag((int)m_currentType*10000+i*100+2)->setVisible(false);
		}

		if(m_selectIndex[m_currentType] == select_index)
		{
			DyCostume* costume = (DyCostume*)m_arrCostumes[m_currentType]->objectAtIndex(m_selectIndex[m_currentType]);
			DyLib* lib = DyLib::sharedLib();

			if(m_currentType > 0)
			{
				if(m_costumeStatus.m_indexSet > 0)
				{
					sprintf(lib->m_strTemp, "set_%02d", m_costumeStatus.m_indexSet);
					DyCostume* costumeSet = DyCostumeManager::sharedCostumeManager()->getCostume(lib->m_strTemp);
					std::list<std::string>::iterator it = costumeSet->m_arrInfo.begin();
					for(unsigned int i = 0;i < costumeSet->m_arrInfo.size();i++)
					{
						std::string subTag = (*it);
						DyCostume* subCostume = DyCostumeManager::sharedCostumeManager()->getCostume(subTag);

						if(costume->m_cosumeType == subCostume->m_cosumeType && costume->m_id == subCostume->m_id)
						{
							enableBoxButton(0, m_selectIndex[0], false);
							break;
						}

						it++;
					}
				}
			}
			else
			{
				std::list<std::string>::iterator it = costume->m_arrInfo.begin();
				for(unsigned int j = 0;j < costume->m_arrInfo.size();j++)
				{
					std::string subTag = (*it);
					DyCostume* subCostume = DyCostumeManager::sharedCostumeManager()->getCostume(subTag);

					enableBoxButton(subCostume->m_cosumeType, m_selectIndex[subCostume->m_cosumeType], false);
						
					it++;
				}
			}
			enableBoxButton(m_currentType, select_index, false);
		}
		else
		{
			enableBoxButton(m_currentType, select_index, true);

			//
			// set then sub clear
			if(m_currentType == 0)
			{
				enableBoxButton(1, m_selectIndex[1], false);
				enableBoxButton(2, m_selectIndex[2], false);
				enableBoxButton(3, m_selectIndex[3], false);
				enableBoxButton(4, m_selectIndex[4], false);
			}
		}

		DyCostumeManager::sharedCostumeManager()->clearCostumeStatus(&m_costumeStatus);

		for(int i = 0;i < 5;i++)
		{
			if(m_selectIndex[i] >= 0)
			{
				DyCostume* costume = (DyCostume*)m_arrCostumes[i]->objectAtIndex(m_selectIndex[i]);

				DyCostumeManager::sharedCostumeManager()->setCostumeStatus(&m_costumeStatus, costume->m_tag);
			}
		}

		DyCostumeManager::sharedCostumeManager()->setCostume(m_doggy, &m_costumeStatus, &m_costumeFunction);

		reloadCostumeStatus();
	}
}


CCLayerColor* DyLayerShopCostume::createCostumeLayer(DyCosumeType costumeType, int layerHight)
{
	YtLib* lib = YtLib::sharedLib();
	DyCostumeManager* costumeManger = DyCostumeManager::sharedCostumeManager();
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	DyStageManager* stageManager = DyStageManager::sharedStageManager();

    m_cntCostume[costumeType] = 0;
	for(unsigned int i = 0;i < costumeManger->m_arrCostumes->count();i++)
	{
		DyCostume* costume = (DyCostume*)costumeManger->m_arrCostumes->objectAtIndex(i);
		bool isViewable = false;

		if(m_shopType == DyLayerShopType_Normal)
		{
			isViewable = costume->m_cosumeType == costumeType;
            if(costume->m_condition_type == DyCostumeLockType_LimitSanta)
            {
                isViewable = dataManager->m_limitSanta_Open && costume->m_cosumeType == costumeType;
            }
            if(costume->m_condition_type == DyCostumeLockType_LimitCupid)
            {
                isViewable = false;
            }
            

		}
		else
		{
			isViewable = costume->m_cosumeType == costumeType && costume->m_has;
		}

		if(isViewable)
		{
			m_cntCostume[costumeType]++;
		}
	}

    //CCLog("costumeType = %d, cnt_costume = %d", costumeType, m_cntCostume[costumeType]);
    
	CCLayerColor* layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), ((m_cntCostume[costumeType]+1)/2)*220 + (110*2-220), layerHight);

	int count = 0;
	
	for(unsigned int i = 0;i < costumeManger->m_arrCostumes->count();i++)
	{
		DyCostume* costume = (DyCostume*)costumeManger->m_arrCostumes->objectAtIndex(i);

		bool isViewable = false;

		if(m_shopType == DyLayerShopType_Normal)
		{
			isViewable = costume->m_cosumeType == costumeType;
            if(costume->m_condition_type == DyCostumeLockType_LimitSanta)
            {
                isViewable = dataManager->m_limitSanta_Open && costume->m_cosumeType == costumeType;
            }
            if(costume->m_condition_type == DyCostumeLockType_LimitCupid)
            {
                isViewable = false;
            }

		}
		else
		{
			isViewable = costume->m_cosumeType == costumeType && costume->m_has;
		}
        
		if(isViewable)
		{
			CCPoint posCenter = ccp(110+((count)/2)*220, 10+layerHight/2+88 - 194*(count%2));
            
			m_arrCostumes[costumeType]->addObject(costume);
			bool isLock = false;
			if(costume->m_condition_type == DyCostumeLockType_NoneOrShop && stageManager->isShopOpen(costume->m_condition_value) == false)
			{
				isLock = true;
			}

            if(costume->m_condition_type == DyCostumeLockType_Gold && !dataManager->awards_get_complete(2, 1))
            {
                isLock = true;
            }
            
            if(costume->m_condition_type == DyCostumeLockType_Crazy && !DyStageManager::sharedStageManager()->isCrazyOpenLeastOne())
            {
                isLock = true;
            }
            if(costume->m_condition_type == DyCostumeLockType_Roulette && !DyCostumeManager::sharedCostumeManager()->hasCostume(costume->m_condition_string))
            {
                isLock = true;
            }
            
			m_arrLock[costumeType]->addObject(CCInteger::create(isLock));
			
			CCSprite* sprBg = CCSprite::createWithSpriteFrameName("menushop_ui_box_00.png");
			CCScale9Sprite *background = CCScale9Sprite::createWithSpriteFrame(sprBg->displayFrame());
			background->setContentSize(CCSizeMake(187, 177));
			background->setPosition(posCenter);
			layerContent->addChild(background);
			m_arrDefaultBox[costumeType]->addObject(background);

			CCSprite* sprBgEnable = CCSprite::createWithSpriteFrameName("menushop_ui_box_01.png");
			CCScale9Sprite *backgroundEnable = CCScale9Sprite::createWithSpriteFrame(sprBgEnable->displayFrame());
			backgroundEnable->setContentSize(CCSizeMake(187, 177));
			backgroundEnable->setPosition(posCenter);
			layerContent->addChild(backgroundEnable);
			backgroundEnable->setVisible(false);
			backgroundEnable->setTag((int)costumeType*10000+count*100+1);

			CCSprite* sprRibon = CCSprite::createWithSpriteFrameName("menushop_ui_costume_title.png");
			sprRibon->setPosition(ccp(posCenter.x-10, posCenter.y+65+4));
			layerContent->addChild(sprRibon);

			CCLabelTTF* labelName;
			//if(isLock)
			//	labelName = CCLabelTTF::create("?", FONT_NAME_EXP, 20.f);
			//else
				labelName = CCLabelTTF::create(costume->m_name.c_str(), FONT_NAME_EXP, 20.f);

			labelName->setColor(ccc3(57, 27, 6));
			labelName->setAnchorPoint(ccp(0, 0.5));
			labelName->setPosition(ccp(posCenter.x-90, posCenter.y+67+4));
			layerContent->addChild(labelName);

			if(isLock)
			{
				CCSprite* sprBox = CCSprite::createWithSpriteFrameName("menushop_ui_costume_box_disable.png");
				sprBox->setPosition(ccp(posCenter.x, posCenter.y-5+4));
				layerContent->addChild(sprBox);

				CCSprite* sprLock = CCSprite::createWithSpriteFrameName("menushop_ui_lock.png");
				sprLock->setPosition(ccp(posCenter.x, posCenter.y));
				sprLock->setScale(0.7f);
				layerContent->addChild(sprLock);

                if(costume->m_condition_type == DyCostumeLockType_NoneOrShop && costume->m_condition_value > 0)
                {
                    CCSprite* sprLockIcon = CCSprite::createWithSpriteFrameName(stageManager->m_shopinfo->getValue("lock_icon", costume->m_condition_value));
                    sprLockIcon->setPosition(ccp(posCenter.x, posCenter.y-14));
                    sprLockIcon->setScale(0.8f);
                    layerContent->addChild(sprLockIcon);
                }
                else if(costume->m_condition_type == DyCostumeLockType_Gold)
                {
                    CCSprite* sprLockIcon = CCSprite::createWithSpriteFrameName("menushop_ui_lock_gold.png");
                    sprLockIcon->setPosition(ccp(posCenter.x, posCenter.y-14));
                    sprLockIcon->setScale(0.8f);
                    layerContent->addChild(sprLockIcon);
                }
                else if(costume->m_condition_type == DyCostumeLockType_Crazy)
                {
                    CCSprite* sprLockIcon = CCSprite::createWithSpriteFrameName("menushop_ui_lock_crazy.png");
                    sprLockIcon->setPosition(ccp(posCenter.x, posCenter.y-14));
                    sprLockIcon->setScale(0.8f);
                    layerContent->addChild(sprLockIcon);
                }
                else if(costume->m_condition_type == DyCostumeLockType_Roulette)
                {
                    CCSprite* sprLockIcon = CCSprite::createWithSpriteFrameName("menushop_ui_lock_roulette.png");
                    sprLockIcon->setPosition(ccp(posCenter.x, posCenter.y-14));
                    sprLockIcon->setScale(0.8f);
                    layerContent->addChild(sprLockIcon);
                }

			}
			else
			{
				CCSprite* sprBox = CCSprite::createWithSpriteFrameName("menushop_ui_costume_box_enable.png");
				sprBox->setPosition(ccp(posCenter.x, posCenter.y-5+4));
				layerContent->addChild(sprBox);

				CCSprite* sprPic = CCSprite::createWithSpriteFrameName(costume->m_resultImage.c_str());
				sprPic->setPosition(ccp(posCenter.x, posCenter.y));
				layerContent->addChild(sprPic);

				if(m_shopType == DyLayerShopType_Normal)
				{
                    if(costume->m_price_gold > 0)
                    {
                        YtLib::setMoneyStyle(lib->m_strTemp, costume->m_price_gold);
                        CCLabelTTF* labelPurchasePrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 20.f);
                        labelPurchasePrice->setColor(ccBLACK);
                        layerContent->addChild(labelPurchasePrice);

                        CCSprite* sprPurchasePriceMark = CCSprite::createWithSpriteFrameName("menu_ui_gold.png");
                        sprPurchasePriceMark->setScale(0.6f);
                        layerContent->addChild(sprPurchasePriceMark);

                        sprPurchasePriceMark->setPosition(ccp(sprPurchasePriceMark->getPosition().x, posCenter.y-68));
                        labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, posCenter.y-68));
                        YtLib::arrangeNodesH(ccp(posCenter.x, posCenter.y-68), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
                    }
                    
                    if(costume->m_price_delly > 0)
                    {
                        YtLib::setMoneyStyle(lib->m_strTemp, costume->m_price_delly);
                        CCLabelTTF* labelPurchasePrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 20.f);
                        labelPurchasePrice->setColor(ccBLACK);
                        layerContent->addChild(labelPurchasePrice);
                        
                        CCSprite* sprPurchasePriceMark = CCSprite::createWithSpriteFrameName("menu_ui_delly.png");
                        sprPurchasePriceMark->setScale(0.6f);
                        layerContent->addChild(sprPurchasePriceMark);
                        
                        sprPurchasePriceMark->setPosition(ccp(sprPurchasePriceMark->getPosition().x, posCenter.y-68));
                        labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, posCenter.y-68));
                        YtLib::arrangeNodesH(ccp(posCenter.x, posCenter.y-68), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
                    }
                    
                    
                    if(costume->m_condition_type == DyCostumeLockType_LimitInvite)
                    {
                        CCLabelTTF* labelLimit = CCLabelTTF::create("-초대 한정-", FONT_NAME_EXP, 23.f);
                        labelLimit->setColor(ccc3(171, 30, 13));
                        labelLimit->setPosition(ccp(posCenter.x, posCenter.y-71));
                        layerContent->addChild(labelLimit);
                        
                    }
                    
                    
				}
			}

 			CCMenuItemSprite *menuitemBuy = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("menushop_ui_btn_buy.png"),
				CCSprite::createWithSpriteFrameName("menushop_ui_btn_buy_sel.png"),
				CCSprite::createWithSpriteFrameName("menushop_ui_btn_buy.png"),
				this, menu_selector(DyLayerShopCostume::onBuyClicked));

			menuitemBuy->setPosition(ccp(posCenter.x-3, posCenter.y-35));
			menuitemBuy->setTag(i);

			CCMenu *pMenu = CCMenu::create(menuitemBuy, NULL);
			pMenu->setPosition(CCPointZero);
			pMenu->setTag((int)costumeType*10000+count*100+2);
			pMenu->setVisible(false);
			layerContent->addChild(pMenu);

			//
			CCSprite* sprCheck = CCSprite::createWithSpriteFrameName("menushop_ui_select.png");
			sprCheck->setPosition(ccp(posCenter.x-65, posCenter.y-60));
			sprCheck->setTag((int)costumeType*10000+count*100+3);

			layerContent->addChild(sprCheck	);

			if(!isLock)
			{
                if(m_shopType == DyLayerShopType_Normal && costume->m_condition_type == DyCostumeLockType_LimitSanta )
                {
                    DyDataManager::sharedDataManager()->costume_set_saw(costume->m_no, true);
                    
                    CCSprite* sprLimitMark = CCSprite::createWithSpriteFrameName("menushop_limit.png");
                    sprLimitMark->setPosition(ccp(posCenter.x+70, posCenter.y+35));
                    layerContent->addChild(sprLimitMark);
                }
                else if(m_shopType == DyLayerShopType_Normal && costume->m_condition_type == DyCostumeLockType_LimitCupid )
                {
                    DyDataManager::sharedDataManager()->costume_set_saw(costume->m_no, true);
                    
                    CCSprite* sprLimitMark = CCSprite::createWithSpriteFrameName("menushop_limit.png");
                    sprLimitMark->setPosition(ccp(posCenter.x+70, posCenter.y+35));
                    layerContent->addChild(sprLimitMark);
                }

                else if(!DyDataManager::sharedDataManager()->costume_get_saw(costume->m_no))
                {
                    DyDataManager::sharedDataManager()->costume_set_saw(costume->m_no, true);

                    CCSprite* sprNew = CCSprite::createWithSpriteFrameName("menu_ui_new_mark.png");
                    sprNew->setPosition(ccp(posCenter.x+70, posCenter.y+35));
                    layerContent->addChild(sprNew);
                }
			}
			count++;
		}
	}

	DyDataManager::sharedDataManager()->dataSave();
	return layerContent;
}


void DyLayerShopCostume::reloadCostumeStatus()
{
    DyLib* lib = DyLib::sharedLib();
    
	DyCostumeManager* costumeManger = DyCostumeManager::sharedCostumeManager();
	//DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
	for(int costumeType = 0;costumeType < 5;costumeType++)
	{
		m_selectIndex[costumeType] = -1;

		for(unsigned int i = 0;i < m_arrCostumes[costumeType]->count();i++)
		{
			DyCostume* costume = (DyCostume*) m_arrCostumes[costumeType]->objectAtIndex(i);

			CCSprite* sprCheck = (CCSprite*)m_tabScrollView[costume->m_cosumeType]->getContent()->getChildByTag((int)costume->m_cosumeType*10000+i*100+3);
			CCNode* pMenu = (CCSprite*)m_tabScrollView[costume->m_cosumeType]->getContent()->getChildByTag((int)costume->m_cosumeType*10000+i*100+2);
			CCSprite* backgroundEnable = (CCSprite*)m_tabScrollView[costume->m_cosumeType]->getContent()->getChildByTag((int)costume->m_cosumeType*10000+i*100+1);

			backgroundEnable->setVisible(false);
			sprCheck->setVisible(false);
			pMenu->setVisible(false);

			if(m_shopType == DyLayerShopType_Normal && costume->m_has)
			{
				sprCheck->setVisible(true);
			}
			
            
			switch(costume->m_cosumeType)
			{
				case DyCosumeType_Set:
					if(m_costumeStatus.m_indexSet == costume->m_id)
					{
						backgroundEnable->setVisible(true);
						m_selectIndex[DyCosumeType_Set] = i;

						if(m_shopType == DyLayerShopType_Normal)
						{
							if(costume->m_has)
							{
								DyDataManager::sharedDataManager()->m_custume_function = m_costumeFunction;
                                
								DyDataManager::sharedDataManager()->m_public.m_costume_status.m_indexSet = m_costumeStatus.m_indexSet;
								DyDataManager::sharedDataManager()->m_public.m_costume_status.m_indexCloth = 0;
								DyDataManager::sharedDataManager()->m_public.m_costume_status.m_indexHat = 0;
								DyDataManager::sharedDataManager()->m_public.m_costume_status.m_indexScarf = 0;
								DyDataManager::sharedDataManager()->m_public.m_costume_status.m_indexApron = 0;
							}
							else
								pMenu->setVisible(true);
						}
					}
					break;
				case DyCosumeType_Cloth:
					if(m_costumeStatus.m_indexCloth == costume->m_id)
					{
						backgroundEnable->setVisible(true);
						m_selectIndex[DyCosumeType_Cloth] = i;
						if(m_shopType == DyLayerShopType_Normal)
						{
							if(costume->m_has)
								DyDataManager::sharedDataManager()->m_public.m_costume_status.m_indexCloth = m_costumeStatus.m_indexCloth;
							else
								pMenu->setVisible(true);
						}
					}
					break;
				case DyCosumeType_Hat:
					if(m_costumeStatus.m_indexHat == costume->m_id)
					{
						backgroundEnable->setVisible(true);
						m_selectIndex[DyCosumeType_Hat] = i;

						if(m_shopType == DyLayerShopType_Normal)
						{
							if(costume->m_has)
								DyDataManager::sharedDataManager()->m_public.m_costume_status.m_indexHat = m_costumeStatus.m_indexHat;
							else
								pMenu->setVisible(true);
						}
					}
					break;
				case DyCosumeType_Scarf:
					if(m_costumeStatus.m_indexScarf == costume->m_id)
					{
						backgroundEnable->setVisible(true);
						m_selectIndex[DyCosumeType_Scarf] = i;

						if(m_shopType == DyLayerShopType_Normal)
						{
							if(costume->m_has)
								DyDataManager::sharedDataManager()->m_public.m_costume_status.m_indexScarf = m_costumeStatus.m_indexScarf;
							else
								pMenu->setVisible(true);
						}
					}
					break;
				case DyCosumeType_Apron:
					if(m_costumeStatus.m_indexApron == costume->m_id)
					{
						backgroundEnable->setVisible(true);
						m_selectIndex[DyCosumeType_Apron] = i;

						if(m_shopType == DyLayerShopType_Normal)
						{
							if(costume->m_has)
								DyDataManager::sharedDataManager()->m_public.m_costume_status.m_indexApron = m_costumeStatus.m_indexApron;
							else
								pMenu->setVisible(true);
						}
					}
					break;
			}
            
            
            
            if(costume->m_condition_type == DyCostumeLockType_LimitInvite)
                pMenu->setVisible(false);
		}
	}

	//m_labelFunction->setString(DyCostumeManager::sharedCostumeManager()->getFunctionExplain(m_costumeStatus.m_function).c_str());

	if(m_arrCostumes[m_currentType]->count() == 0)
		m_labelNotiShop->setVisible(true);
	else
		m_labelNotiShop->setVisible(false);

	if(m_shopType == DyLayerShopType_Myroom)
	{
		DyDataManager::sharedDataManager()->m_public.m_costume_status = m_costumeStatus;
        costumeManger->setCostumeGameFunction(&DyTycoonManager::sharedTycoonManager()->m_functionGame);
	}
    else
    {
        DyCostumeManager* costumeManager = DyCostumeManager::sharedCostumeManager();
        
        DyCostumeStatus costumeTemp = m_costumeStatus;
        DyCostumeFunction custume_function_temp;
        
        sprintf(lib->m_strTemp, "hat_%02d", costumeTemp.m_indexHat);
        if(!costumeManager->hasCostume(lib->m_strTemp))
            costumeTemp.m_indexHat = 0;
        
        sprintf(lib->m_strTemp, "cloth_%02d", costumeTemp.m_indexCloth);
        if(!costumeManager->hasCostume(lib->m_strTemp))
            costumeTemp.m_indexCloth = 0;
   
        sprintf(lib->m_strTemp, "scarf_%02d", costumeTemp.m_indexScarf);
        if(!costumeManager->hasCostume(lib->m_strTemp))
            costumeTemp.m_indexScarf = 0;

        sprintf(lib->m_strTemp, "apron_%02d", costumeTemp.m_indexApron);
        if(!costumeManager->hasCostume(lib->m_strTemp))
            costumeTemp.m_indexApron = 0;
        
        costumeTemp.m_indexSet = 0;
        
        costumeManager->setCostume(NULL, &costumeTemp, &custume_function_temp);
        
        DyDataManager::sharedDataManager()->m_public.m_costume_status = costumeTemp;
        DyDataManager::sharedDataManager()->m_custume_function = custume_function_temp;
        
    }
    
	DyDataManager::sharedDataManager()->dataSave();
    
    CCLabelTTF* labelDecay = (CCLabelTTF*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_TEXT_DECAY);
    CCLabelTTF* labelIce = (CCLabelTTF*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_TEXT_ICE);
    CCLabelTTF* labelHeart = (CCLabelTTF*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_TEXT_HEART);
    CCLabelTTF* labelGold = (CCLabelTTF*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_TEXT_GOLD);
    CCLabelTTF* labelCooling = (CCLabelTTF*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_TEXT_COOLING);
    
    labelDecay->setFontName(FONT_NAME_DEFAULT);
    labelIce->setFontName(FONT_NAME_DEFAULT);
    labelHeart->setFontName(FONT_NAME_DEFAULT);
    labelGold->setFontName(FONT_NAME_DEFAULT);
    labelCooling->setFontName(FONT_NAME_DEFAULT);
    
    
    if(m_costumeFunction.m_decay > 0)
        sprintf(lib->m_strTemp, "-%d%%", m_costumeFunction.m_decay);
    else
        sprintf(lib->m_strTemp, "0%%");
    labelDecay->setString(lib->m_strTemp);
    
    if(m_costumeFunction.m_ice > 0)
        sprintf(lib->m_strTemp, "-%d%%", m_costumeFunction.m_ice);
    else
        sprintf(lib->m_strTemp, "0%%");
    labelIce->setString(lib->m_strTemp);
    
    if(m_costumeFunction.m_heart > 0)
        sprintf(lib->m_strTemp, "+%d%%", m_costumeFunction.m_heart);
    else
        sprintf(lib->m_strTemp, "0%%");
    labelHeart->setString(lib->m_strTemp);
    
    if(m_costumeFunction.m_tip > 0)
        sprintf(lib->m_strTemp, "+%d%%", m_costumeFunction.m_tip);
    else
        sprintf(lib->m_strTemp, "0%%");
    labelGold->setString(lib->m_strTemp);
    
    if(m_costumeFunction.m_cooling > 0)
        sprintf(lib->m_strTemp, "-%d%%", m_costumeFunction.m_cooling);
    else
        sprintf(lib->m_strTemp, "0%%");
    labelCooling->setString(lib->m_strTemp);
    

    CCSprite* sprTop = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_TOP);
    CCSprite* sprScarf = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SCARF);
    CCSprite* sprCloth = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_CLOTH);
    CCSprite* sprApron = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_APRON);
    
    CCSprite* sprTop_shop = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SHOP_TOP);
    CCSprite* sprScarf_shop = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SHOP_SCARF);
    CCSprite* sprCloth_shop = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SHOP_CLOTH);
    CCSprite* sprApron_shop = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SHOP_APRON);
 
    sprTop_shop->setVisible(false);
    sprScarf_shop->setVisible(false);
    sprCloth_shop->setVisible(false);
    sprApron_shop->setVisible(false);
    
    
    CCSprite* sprTop_buyed = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SHOP_TOP_CHECKBUY);
    CCSprite* sprScarf_buyed = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SHOP_SCARF_CHECKBUY);
    CCSprite* sprCloth_buyed = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SHOP_CLOTH_CHECKBUY);
    CCSprite* sprApron_buyed = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SHOP_APRON_CHECKBUY);
    
    sprTop_buyed->setVisible(false);
    sprScarf_buyed->setVisible(false);
    sprCloth_buyed->setVisible(false);
    sprApron_buyed->setVisible(false);

    if(m_costumeStatus.m_indexHat > 0)
    {
        sprintf(lib->m_strTemp, "hat_%02d", m_costumeStatus.m_indexHat);
		DyCostume* costume = costumeManger->getCostume(lib->m_strTemp);
		setShopMark(sprTop_shop, costume->m_function_shop);
        
        sprTop->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_icon_slot_top_on.png"));
        
        if(m_shopType == DyLayerShopType_Normal && costume->m_has)
            sprTop_buyed->setVisible(true);
    }
    else
        sprTop->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_icon_slot_top.png"));
    
    if(m_costumeStatus.m_indexCloth > 0)
    {
        sprintf(lib->m_strTemp, "cloth_%02d", m_costumeStatus.m_indexCloth);
		DyCostume* costume = costumeManger->getCostume(lib->m_strTemp);
		setShopMark(sprCloth_shop, costume->m_function_shop);
        
        sprCloth->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_icon_slot_cloth_on.png"));
        
        if(m_shopType == DyLayerShopType_Normal && costume->m_has)
            sprCloth_buyed->setVisible(true);

    }
    else
        sprCloth->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_icon_slot_cloth.png"));
    
    if(m_costumeStatus.m_indexScarf > 0)
    {
        sprintf(lib->m_strTemp, "scarf_%02d", m_costumeStatus.m_indexScarf);
		DyCostume* costume = costumeManger->getCostume(lib->m_strTemp);
		setShopMark(sprScarf_shop, costume->m_function_shop);
        
        sprScarf->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_icon_slot_scarf_on.png"));
        
        if(m_shopType == DyLayerShopType_Normal && costume->m_has)
            sprScarf_buyed->setVisible(true);

    }
    else
        sprScarf->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_icon_slot_scarf.png"));
    
    if(m_costumeStatus.m_indexApron > 0)
    {
        sprintf(lib->m_strTemp, "apron_%02d", m_costumeStatus.m_indexApron);
		DyCostume* costume = costumeManger->getCostume(lib->m_strTemp);
		setShopMark(sprApron_shop, costume->m_function_shop);
        
        sprApron->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_icon_slot_apron_on.png"));
        
        if(m_shopType == DyLayerShopType_Normal && costume->m_has)
            sprApron_buyed->setVisible(true);

    }
    else
        sprApron->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_icon_slot_apron.png"));
    
    costumeManger->setCostumeGameFunction(&DyTycoonManager::sharedTycoonManager()->m_functionGame);
}



void DyLayerShopCostume::setShopMark(CCSprite* sprMark, int shop)
{
    if(shop < 0)
    {
        sprMark->setVisible(false);
    }
    else
    {
        sprMark->setVisible(true);
        switch(shop)
        {
            case 0:
                sprMark->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_shop_icon_snack.png"));
                sprMark->setScale(1.0f);
                
                break;
            case 1:
                sprMark->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_shop_icon_hamberger.png"));
                sprMark->setScale(1.0f);
                
                break;
            case 2:
                sprMark->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_shop_icon_coffee.png"));
                sprMark->setScale(1.0f);
                
                break;
            case 3:
                sprMark->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_shop_icon_china.png"));
                sprMark->setScale(0.85f);
                break;
            case 4:
                sprMark->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_shop_icon_pizza.png"));
                sprMark->setScale(0.85f);
                break;
                
            case 5:
                sprMark->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("menu2_ui_shop_icon_korean.png"));
                sprMark->setScale(0.85f);
                break;


        }
    }
}


void DyLayerShopCostume::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
    //DyLib* lib = DyLib::sharedLib();
    
	m_contentLayer = (CCLayer* )getChildByTag(SHOPCOSTUME_UI_TAG_BG_BOTTOM);
	m_contentLayer->removeFromParentAndCleanup(false);
	m_contentLayer->retain();

	CCLabelTTF* labelUserName = (CCLabelTTF*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_USERNAME);
	labelUserName->setColor(ccBLACK);
    //labelUserName->setString(((CCString*)lib->m_kakaoUserInfo->objectForKey("nickname"))->getCString());
    labelUserName->setString(DyDataManager::sharedDataManager()->m_nickname.c_str());
   
    m_menu = (CCMenu*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_MENU);
}


void DyLayerShopCostume::onEnter()
{
	CCLayer::onEnter();
	if(m_isInit)
		return;
	m_isInit = true;
	//YtLib* lib = YtLib::sharedLib();

	m_costumeStatus = DyDataManager::sharedDataManager()->m_public.m_costume_status;
	m_costumeStatusFirst = m_costumeStatus;
    m_costumeFunctionFirst = m_costumeFunction;

	m_scrollViewSize = CCSizeMake(880, 465);

	m_tabView = new DyLayerShopCostumeTab(CCRectMake(0, 0, m_scrollViewSize.width, m_scrollViewSize.height));
	m_tabView->m_delegate = this;
	m_tabView->m_selectedTab = m_startTab;

	const char* tab_resource_name[5*3] =
	{
		"menushop_ui_costume_tab_set.png",
		"menushop_ui_costume_tab_set_sel.png",
		"menushop_ui_costume_tab_set_highlight.png",

		"menushop_ui_costume_tab_cloth.png",
		"menushop_ui_costume_tab_cloth_sel.png",
		"menushop_ui_costume_tab_cloth_highlight.png",

		"menushop_ui_costume_tab_hat.png",
		"menushop_ui_costume_tab_hat_sel.png",
		"menushop_ui_costume_tab_hat_highlight.png",

		"menushop_ui_costume_tab_scarf.png",
		"menushop_ui_costume_tab_scarf_sel.png",
		"menushop_ui_costume_tab_scarf_highlight.png",

		"menushop_ui_costume_tab_apron.png",
		"menushop_ui_costume_tab_apron_sel.png",
		"menushop_ui_costume_tab_apron_highlight.png",
	};

	for(int i = 0;i < 5;i++)
	{
		float height = m_scrollViewSize.height - m_tabView->m_thick - 55+25;

		CCLayer* layerContent = createCostumeLayer((DyCosumeType)i, height);

		m_tabScrollView[i] = CCScrollView::create(CCSizeMake(m_scrollViewSize.width - m_tabView->m_contentOffsetX, height), layerContent);
		m_tabScrollView[i]->setDelegate(this);

		m_tabScrollView[i]->setContentSize(layerContent->getContentSize());
        
        
		m_tabScrollView[i]->setDirection(kCCScrollViewDirectionHorizontal);
		//layerTab->setContentOffset(layerTab->minContainerOffset());
		m_tabScrollView[i]->setContentOffset(ccp(0,0));
		m_tabScrollView[i]->setGlobalScale(YtLib::sharedLib()->m_scale);

		m_tabView->addTab(CCSprite::createWithSpriteFrameName(tab_resource_name[i*3]),
			CCSprite::createWithSpriteFrameName(tab_resource_name[i*3+1]),
			CCSprite::createWithSpriteFrameName(tab_resource_name[i*3+2]), 
			m_tabScrollView[i]);
	}

	addChild(m_tabView);
	m_tabView->release();

	//m_contentLayer->setParent(NULL);
	m_tabView->setContentBg(m_contentLayer);

	m_doggy = YtAniObject::loadAniObject(this, "node_animal_doggy.ccbi");
	m_doggy->setPosition(ccp(135+30+15+8, 135+10));
	m_doggy->setScale(0.8f);

	CCSprite* sprShadow = CCSprite::createWithSpriteFrameName("doggy_shadow.png");
	sprShadow->setPosition(ccp(m_doggy->getPosition().x, m_doggy->getPosition().y + 5));
	addChild(sprShadow);

	addChild(m_doggy);
	DyCostumeManager::sharedCostumeManager()->setCostume(m_doggy, &m_costumeStatus, &m_costumeFunction);

//	m_labelFunction = CCLabelTTF::create("", FONT_NAME_EXP, 18.f, CCSizeMake(210,50), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
//
//	m_labelFunction->setPosition(ccp(135, 105));
//	m_labelFunction->setColor(ccc3(100, 57, 38));
//
//	addChild(m_labelFunction);

	m_labelNotiShop = CCLabelTTF::create(STRING_not_have_costume, FONT_NAME_EXP, 18.f, CCSizeMake(400,100), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
	m_labelNotiShop->setPosition(ccp(570, 220));
	m_labelNotiShop->setColor(ccc3(100, 57, 38));
	m_labelNotiShop->setVisible(false);

	addChild(m_labelNotiShop);

	reloadCostumeStatus();
}


void DyLayerShopCostume::onExit()
{
	CCLayer::onExit();
}


void DyLayerShopCostume::scrollViewDidScroll(CCScrollView* view)
{
//	CCSprite* sprScrollBg = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SCROLL_BG);
//	CCSprite* sprScrollCursor = (CCSprite*)m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SCROLL_CURSOR);

//	CCPoint posScrollBg = ccp(sprScrollBg->getPosition().x - sprScrollBg->getContentSize().width*sprScrollBg->getScaleX()/2, sprScrollBg->getPosition().y);
//	CCPoint position = view->getContentOffset();
//	CCSize size = view->getContentSize();
//
//	float insect = 10;
//	float posX = posScrollBg.x + sprScrollBg->getContentSize().width*sprScrollBg->getScaleX() * (-position.x/(size.width-view->getViewSize().width-insect*2));
//
//	if(posX < posScrollBg.x+insect)
//		posX = posScrollBg.x+insect;
//
//	if(posX > posScrollBg.x + sprScrollBg->getContentSize().width*sprScrollBg->getScaleX()-insect)
//		posX = posScrollBg.x + sprScrollBg->getContentSize().width*sprScrollBg->getScaleX()-insect;
//
//	sprScrollCursor->setPosition(ccp(posX, posScrollBg.y));
}


void DyLayerShopCostume::scrollViewDidZoom(CCScrollView* view)
{
}


// tab clicked events
void DyLayerShopCostume::onBuyClicked(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	DyCostume* costume = (DyCostume*)m_arrCostumes[m_currentType]->objectAtIndex(m_selectIndex[m_currentType]);
    
	DyMoneyManager::sharedMoneyManager()->questBuy(DyBuyItemType_Costume, costume, costume->m_price_gold > 0 ? DyBuyMethod_Gold : DyBuyMethod_Delly);
    
}


void DyLayerShopCostume::tabDidChange(CCTabView* tabView, int index, CCLayer* contentLayer, bool init, bool redo)
{
    if(redo)
        return;
    
    if(!init)
        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	CCScrollView* view = (CCScrollView*)contentLayer;

	scrollViewDidScroll(view);

	m_currentType = (DyCosumeType)index;

//	if(m_scrollViewSize.width > m_tabScrollView[m_currentType]->getContentSize().width)
//	{	
//		m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SCROLL_BG)->setVisible(false);
//		m_contentLayer->getChildByTag(SHOPCOSTUME_UI_TAG_SCROLL_CURSOR)->setVisible(false);
//	}

	if(m_labelNotiShop)
	{
		if(m_arrCostumes[m_currentType]->count() == 0)
			m_labelNotiShop->setVisible(true);
		else
			m_labelNotiShop->setVisible(false);
	}
}

void DyLayerShopCostume::onFunctionClicked(cocos2d::CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

    DyPopup* popup = DyPopup::create(DyPopupType_Costume, NULL, NULL);
    
    popup->setCostumeInfo(&m_costumeStatus, &m_costumeFunction);
                          
    popup->addButton(DyButtonType_Close, "", 0);
    
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);

}


void DyLayerShopCostume::onResetClicked(cocos2d::CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	m_costumeStatus = m_costumeStatusFirst;
    m_costumeFunction = m_costumeFunctionFirst;

	DyCostumeManager::sharedCostumeManager()->setCostume(m_doggy, &m_costumeStatus, &m_costumeFunction);
	reloadCostumeStatus();
}


void DyLayerShopCostume::onAllBuyClicked(cocos2d::CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

    
	if(m_arrCostume)
		m_arrCostume->release();

	m_arrCostume = new CCArray();

	DyLib* lib = DyLib::sharedLib();
	DyCostumeStatus costumeStatus = m_costumeStatus;
	int count_gold = 0;
    int cound_delly = 0;
    
    
    bool hasSet = false;
    
    if(costumeStatus.m_indexSet > 0)
	{
        hasSet = true;
        sprintf(lib->m_strTemp, "set_%02d", m_costumeStatus.m_indexSet);
		DyCostume* costumeSet = DyCostumeManager::sharedCostumeManager()->getCostume(lib->m_strTemp);
		std::list<std::string>::iterator it = costumeSet->m_arrInfo.begin();
		for(unsigned int i = 0;i < costumeSet->m_arrInfo.size();i++)
		{
			std::string subTag = (*it);
			DyCostume* subCostume = DyCostumeManager::sharedCostumeManager()->getCostume(subTag);
            
            if(subCostume->m_has)
            {
                hasSet = false;
            }
            it++;
		}
	}
    
	if(hasSet)
	{
     	sprintf(lib->m_strTemp, "set_%02d", m_costumeStatus.m_indexSet);
		DyCostume* costumeSet = DyCostumeManager::sharedCostumeManager()->getCostume(lib->m_strTemp);
		std::list<std::string>::iterator it = costumeSet->m_arrInfo.begin();
		for(unsigned int i = 0;i < costumeSet->m_arrInfo.size();i++)
		{
			std::string subTag = (*it);
			DyCostume* subCostume = DyCostumeManager::sharedCostumeManager()->getCostume(subTag);

            switch(subCostume->m_cosumeType)
			{
				case DyCosumeType_Cloth:
					costumeStatus.m_indexCloth = 0;
					break;
				case DyCosumeType_Hat:
					costumeStatus.m_indexHat = 0;
					break;
				case DyCosumeType_Scarf:
					costumeStatus.m_indexScarf = 0;
					break;
				case DyCosumeType_Apron:	
					costumeStatus.m_indexApron = 0;
					break;
                default:
                    break;
			}
			it++;
		}
		if(!costumeSet->m_has)
		{
			count_gold += costumeSet->m_price_gold;
            cound_delly += costumeSet->m_price_delly;
			m_arrCostume->addObject(costumeSet);
		}
	}

	if(costumeStatus.m_indexCloth > 0)
	{
		sprintf(lib->m_strTemp, "cloth_%02d", m_costumeStatus.m_indexCloth);
		DyCostume* costume = DyCostumeManager::sharedCostumeManager()->getCostume(lib->m_strTemp);
		if(!costume->m_has)
		{
			count_gold += costume->m_price_gold;
            cound_delly += costume->m_price_delly;
			m_arrCostume->addObject(costume);
		}
	}
	if(costumeStatus.m_indexHat > 0)
	{
		sprintf(lib->m_strTemp, "hat_%02d", m_costumeStatus.m_indexHat);
		DyCostume* costume = DyCostumeManager::sharedCostumeManager()->getCostume(lib->m_strTemp);
		if(!costume->m_has)
		{
			count_gold += costume->m_price_gold;
            cound_delly += costume->m_price_delly;
			m_arrCostume->addObject(costume);
		}
	}
	if(costumeStatus.m_indexScarf > 0)
	{
		sprintf(lib->m_strTemp, "scarf_%02d", m_costumeStatus.m_indexScarf);
		DyCostume* costume = DyCostumeManager::sharedCostumeManager()->getCostume(lib->m_strTemp);
		if(!costume->m_has)
		{
			count_gold += costume->m_price_gold;
            cound_delly += costume->m_price_delly;
			m_arrCostume->addObject(costume);
		}
	}
	if(costumeStatus.m_indexApron > 0)
	{
		sprintf(lib->m_strTemp, "apron_%02d", m_costumeStatus.m_indexApron);
		DyCostume* costume = DyCostumeManager::sharedCostumeManager()->getCostume(lib->m_strTemp);
		if(!costume->m_has)
		{
			count_gold += costume->m_price_gold;
            cound_delly += costume->m_price_delly;
			m_arrCostume->addObject(costume);
		}
	}

	if(count_gold > 0 || cound_delly > 0)
	{
		DyMoneyManager::sharedMoneyManager()->questBuy(DyBuyItemType_CostumeArray, m_arrCostume, DyBuyMethod_GoldAndDelly);
	}
}


SEL_MenuHandler DyLayerShopCostume::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onResetClicked", DyLayerShopCostume::onResetClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onAllBuyClicked", DyLayerShopCostume::onAllBuyClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onFunctionClicked", DyLayerShopCostume::onFunctionClicked);
    return NULL;    
}

SEL_CCControlHandler DyLayerShopCostume::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}

bool DyLayerShopCostume::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}

void DyLayerShopCostume::pause()
{
	for(int i = 0;i < 5;i++)
	{
		if(m_tabScrollView[i])
        {
			m_tabScrollView[i]->setTouchEnabled(false);
            
            if(m_selectIndex[i] >= 0)
            {
                CCMenuItemSprite* pMenu = (CCMenuItemSprite*)m_tabScrollView[i]->getContent()->getChildByTag((int)i*10000+m_selectIndex[i]*100+2);
                pMenu->setVisible(false);
            }
        }
	}
	if(m_tabView)
		m_tabView->setEnable(false);

	m_menu->setEnabled(false);
}


void DyLayerShopCostume::resume()
{
    for(int i = 0;i < 5;i++)
	{
		if(m_tabScrollView[i])
		{
			m_tabScrollView[i]->setTouchEnabled(true);
			if(m_cntCostume[i] <= 4)
			{	
				m_tabScrollView[i]->setScrollEnable(false);
			}
			else
				m_tabScrollView[i]->setScrollEnable(true);
		}
	}
	if(m_tabView)
		m_tabView->setEnable(true);

	m_menu->setEnabled(true);

	if(m_tabScrollView[0])
	{
		int count[5] = {0, };
		DyCostumeManager* costumeManger = DyCostumeManager::sharedCostumeManager();
        DyDataManager* dataManager = DyDataManager::sharedDataManager();
        

		for(unsigned int i = 0;i < costumeManger->m_arrCostumes->count();i++)
		{
			DyCostume* costume = (DyCostume*)costumeManger->m_arrCostumes->objectAtIndex(i);

			if(m_shopType == DyLayerShopType_Normal)
			{
                if(costume->m_condition_type == DyCostumeLockType_LimitSanta && !dataManager->m_limitSanta_Open)
                {
                    continue;
                }
                if(costume->m_condition_type == DyCostumeLockType_LimitCupid)
                {
                    continue;
                }
                
				CCSprite* sprCheck = (CCSprite*)m_tabScrollView[costume->m_cosumeType]->getContent()->getChildByTag((int)costume->m_cosumeType*10000+count[costume->m_cosumeType]*100+3);
				CCNode* nodeBuyMenu = (CCSprite*)m_tabScrollView[costume->m_cosumeType]->getContent()->getChildByTag((int)costume->m_cosumeType*10000+count[costume->m_cosumeType]*100+2);
			
				if(costume->m_has)
				{
					nodeBuyMenu->setVisible(false);
					sprCheck->setVisible(true);
				}
				else
				{
					sprCheck->setVisible(false);
				}
                
				count[costume->m_cosumeType]++;
			}
		}
		reloadCostumeStatus();
	}
}


void DyLayerShopCostume::hide()
{
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}

void DyLayerShopCostume::show()
{
    
}

