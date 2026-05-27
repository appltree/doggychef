/*
DySceneStage.cpp
Doggy chef
@initialize at 121120

@history
121120	yoonsr	initialize
*/

#include "DyConfig.h"
#include "DySceneStage.h"
#include "DyLib.h"
#include "DyLayerStageSub.h"
#include "DyMoney.h"
#include "DyLayerShop.h"
#include "DyPopup.h"
#include "DyNetwork.h"
#include "json.h"
#include "DySceneStory.h"

void DyMenuItemShop::visit()
{
	if (!isVisible())
    {
		return;
    }

	kmGLPushMatrix();

    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->beforeDraw();
        this->transformAncestors();
    }

	this->transform();

	CCPoint screenPos = m_rectClip.origin;
	CCSize size = m_rectClip.size;

	glEnable(GL_SCISSOR_TEST);
	float s = this->getScale();
    
    
	glScissor(screenPos.x*s*YtLib::sharedLib()->m_scale + YtLib::sharedLib()->m_gapWin.width/2,
		screenPos.y*s*YtLib::sharedLib()->m_scale + YtLib::sharedLib()->m_gapWin.height/2,
		size.width*s*YtLib::sharedLib()->m_scale, size.height*s*YtLib::sharedLib()->m_scale);

	if(m_pChildren)
    {
		ccArray *arrayData = m_pChildren->data;
		unsigned int i=0;
		
		// draw children zOrder < 0
		for( ; i < arrayData->num; i++ )
        {
			CCNode *child =  (CCNode*)arrayData->arr[i];
			if ( child->getZOrder() < 0 )
            {
				child->visit();
			}
            else
            {
				break;
            }
		}
		
		// this draw
		this->draw();
		
		// draw children zOrder >= 0
		for( ; i < arrayData->num; i++ )
        {
			CCNode* child = (CCNode*)arrayData->arr[i];
			child->visit();
		}
        
	}
    else
    {
		this->draw();
    }

	glDisable(GL_SCISSOR_TEST);
   // this->afterDraw();
	if ( m_pGrid && m_pGrid->isActive())
    {
		m_pGrid->afterDraw(this);
    }

	kmGLPopMatrix();
}


DyMenuItemShop * DyMenuItemShop::create(CCNode *normalSprite, CCNode *selectedSprite, CCNode *disabledSprite, CCObject *target, SEL_MenuHandler selector)
{
    DyMenuItemShop *pRet = new DyMenuItemShop();
    pRet->initWithNormalSprite(normalSprite, selectedSprite, disabledSprite, target, selector); 
    pRet->autorelease();
    return pRet;
}


DyLayerStage::DyLayerStage()
{
	m_tag = DYLAYERSTAGE_TAG;

	m_nSelectedShop = DyStageType_Snack;
	m_doggy = NULL;
	m_arrPageCursorBg = new CCArray();
    m_arrMenuShopOpen = new CCArray();
}


DyLayerStage::~DyLayerStage()
{
	DyMoneyManager::sharedMoneyManager()->removeMoneyLayer(m_layerMoney);
	m_layerMoney->release();
	m_arrPageCursorBg->release();

	if(m_doggy)
		YtAniObject::unloadAniObject(m_doggy);
    
    m_arrMenuShopOpen->release();
}



void DyLayerStage::setShopButtonSprite(DyMenuItemShop* menuitem, int shop)
{
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyLib* lib = DyLib::sharedLib();
    
    //menuitem->setNormalImage(NULL);
    
	CCSprite* sprNormal;

	if(shop >= stageManager->m_cntShop)
	{
        CCLabelTTF* labelComing;
		CCLabelTTF* labelSoon;
        
        if(shop == 3)
        {
            sprNormal = CCSprite::create("image_shop_china_dim.png");
            labelComing = CCLabelTTF::create("12월중", FONT_NAME_EXP, 50.f);
            labelSoon = CCLabelTTF::create("오픈!", FONT_NAME_EXP, 50.f);
        }
        if(shop == 5)
        {
            sprNormal = CCSprite::create("image_shop_korean_dim.png");
            sprNormal->setColor(ccc3(25,25,25));
            labelComing = CCLabelTTF::create("4월중", FONT_NAME_EXP, 50.f);
            labelComing->setColor(ccWHITE);
            labelSoon = CCLabelTTF::create("오픈!", FONT_NAME_EXP, 50.f);
            labelSoon->setColor(ccWHITE);
        }

        else
        {
            sprNormal = CCSprite::create("image_shop_coming.png");
            labelComing = CCLabelTTF::create("Coming", FONT_NAME_DEFAULT, 50.f);
            labelSoon = CCLabelTTF::create("soon", FONT_NAME_DEFAULT, 50.f);
        }
		
        
        labelComing->setColor(ccWHITE);
		labelComing->setPosition(ccp(sprNormal->getContentSize().width/2, 150));
		sprNormal->addChild(labelComing);

		labelSoon->setColor(ccWHITE);
		labelSoon->setPosition(ccp(sprNormal->getContentSize().width/2, 100));
		sprNormal->addChild(labelSoon);
	}
	else
	{
		if(stageManager->isShopOpen(shop))
			sprNormal = CCSprite::create(stageManager->m_shopinfo->getValue("shop_icon", shop));
		else
		{
			if(stageManager->isStageClear(shop-1))
			{
				sprNormal = CCSprite::create(stageManager->m_shopinfo->getValue("shop_icon", shop));

				CCSprite* sprBlend = CCSprite::create(stageManager->m_shopinfo->getValue("shop_lock_icon", shop));

				sprNormal->addChild(sprBlend);
				
				sprBlend->setPosition(ccp(sprNormal->getContentSize().width/2, sprNormal->getContentSize().height/2));
				sprBlend->setOpacity(255*70/100);
                sprBlend->setColor(ccc3(40,40,40));
                
                CCMenu* menuShopOpen = CCMenu::create();
                menuShopOpen->setTag(shop);
                menuShopOpen->setPosition(ccp(sprNormal->getContentSize().width/2, 270));
                sprNormal->addChild(menuShopOpen, 5);
                
                
                CCMenuItemImage* menuitemGold = CCMenuItemImage::create();
                menuitemGold->initWithTarget(this, menu_selector(DyLayerStage::onOpenByGold));
                menuitemGold->setTag(shop);
                YtLib::setMoneyStyle(lib->m_strTemp, atoi(stageManager->m_shopinfo->getValue("gold", shop)));
                DyLib::setMenuItemText(menuitemGold, DyButtonType_Blue, lib->m_strTemp, 25, 30);
                menuitemGold->setPosition(ccp(0, -80));
                menuShopOpen->addChild(menuitemGold);
                
                CCSprite* sprGoldLock = CCSprite::createWithSpriteFrameName("menushop_ui_gold_lock.png");
                sprGoldLock->setPosition(ccp(-10, 30));
                menuitemGold->addChild(sprGoldLock);
            
                
                CCMenuItemImage* menuitemDelly = CCMenuItemImage::create();
                menuitemDelly->initWithTarget(this, menu_selector(DyLayerStage::onOpenByDelly));
                
                YtLib::setMoneyStyle(lib->m_strTemp, atoi(stageManager->m_shopinfo->getValue("delly_after", shop)));
                
                DyLib::setMenuItemText(menuitemDelly, DyButtonType_Red, lib->m_strTemp, 30, 35);
                menuitemDelly->setPosition(ccp(0, -200));
                menuShopOpen->addChild(menuitemDelly);
                menuitemDelly->setTag(shop);
                CCSprite* sprDellyLock = CCSprite::createWithSpriteFrameName("menushop_ui_delly_lock.png");
                sprDellyLock->setPosition(ccp(-10, 30));
                menuitemDelly->addChild(sprDellyLock);

                
                CCLabelTTF* labelOr = CCLabelTTF::create("또는", FONT_NAME_EXP, 30.f);
                labelOr->setColor(ccWHITE);
                labelOr->setPosition(ccp(sprNormal->getContentSize().width/2, 130));
                
                sprNormal->addChild(labelOr, 6);
                
                m_arrMenuShopOpen->addObject(menuShopOpen);
                
			}
			else
			{
                
				sprNormal = CCSprite::create(stageManager->m_shopinfo->getValue("shop_lock_icon", shop));
				sprintf(lib->m_strTemp, STRING_shop_shop, stageManager->m_shopinfo->getValue("name_kor", shop-1));
				
                CCLabelTTF* labelExplain = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 25.f, CCSizeMake(230,150), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
				labelExplain->setColor(ccWHITE);
				labelExplain->setPosition(ccp(sprNormal->getContentSize().width/2, 160));
                
				sprNormal->addChild(labelExplain);
				sprNormal->setColor(ccc3(51,51,51));
                
                CCMenu* menuShopOpen = CCMenu::create();
                menuShopOpen->setTag(shop);
                menuShopOpen->setPosition(ccp(sprNormal->getContentSize().width/2, 270));
                sprNormal->addChild(menuShopOpen, 5);

                CCMenuItemImage* menuitemDelly = CCMenuItemImage::create();
                menuitemDelly->initWithTarget(this, menu_selector(DyLayerStage::onDirectOpenByDelly));
                
                DyLib::setMenuItemText_kor(menuitemDelly, DyButtonType_Red, STRING_POPUP_DIRECTOPEN, 27, 30);
                menuitemDelly->setPosition(ccp(0, -210));
                menuShopOpen->addChild(menuitemDelly);
                menuitemDelly->setTag(shop);
                CCSprite* sprDellyLock = CCSprite::createWithSpriteFrameName("menushop_ui_delly_lock.png");
                sprDellyLock->setPosition(ccp(-10, 30));
                menuitemDelly->addChild(sprDellyLock);
                
                //
                // -델리 표시
                CCSprite* sprDellyBg = CCSprite::createWithSpriteFrameName("menu2_ui_spend_delly.png");
                sprDellyBg->setPosition(ccp(130, 0));
                menuitemDelly->addChild(sprDellyBg);
                
                YtLib::setMoneyStyle(lib->m_strTemp, atoi(stageManager->m_shopinfo->getValue("delly", shop)));
                
                CCLabelTTF* labelDelly = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 22);
                labelDelly->setColor(ccBLACK);
                labelDelly->setPosition(ccp(31,13));
                sprDellyBg->addChild(labelDelly);
                
			}
		}
	}
	menuitem->setNormalImage(sprNormal);
}


DyMenuItemShop* DyLayerStage::createShopButton(DyStageType shop)
{
	DyMenuItemShop* menuSprite = DyMenuItemShop::create(NULL, NULL, NULL, this, menu_selector(DyLayerStage::onStageShopClicked));
	setShopButtonSprite(menuSprite, shop);

	menuSprite->setTag(shop);
	m_posShopMenuY = -20;
    
    if(shop == DyStageType_China)
        m_posShopMenuY += 10;
    
	menuSprite->setPosition(ccp(shop*m_menuShopGap, m_posShopMenuY));
	menuSprite->m_rectClip.origin = getChildByTag(STAGS_UI_TAG_CURSOR_MAIN)->getPosition();
	menuSprite->m_rectClip.size = getChildByTag(STAGS_UI_TAG_CURSOR_MAIN)->getContentSize();
	menuSprite->m_rectClip.size.width -= 30;
	menuSprite->m_rectClip.size.height -= 30;
	menuSprite->m_rectClip.origin.x -= menuSprite->m_rectClip.size.width/2;
	menuSprite->m_rectClip.origin.y -= menuSprite->m_rectClip.size.height/2;
    
	menuSprite->setEnabled(false);
	return menuSprite;
}


void DyLayerStage::callback_pushSubStage(float dt)
{
	unschedule(schedule_selector(DyLayerStage::callback_pushSubStage));

	goShop((DyStageType)m_nSelectedShop);
}




void DyLayerStage::callfunc_update_me_success(float dt)
{
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    
    unschedule(schedule_selector(DyLayerStage::callfunc_update_me_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    moneyManager->applyDelly();
    
}


void DyLayerStage::callfunc_update_me_fail(float dt)
{
    unschedule(schedule_selector(DyLayerStage::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    lib->data_restore();
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShop::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerStage_callfunc_update_me(int value, int code)
{
    DyLayerStage *layerStage = (DyLayerStage*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERSTAGE_TAG);
    
    if(value == 0)
    {
        layerStage->schedule(schedule_selector(DyLayerStage::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layerStage->schedule(schedule_selector(DyLayerStage::callfunc_update_me_fail), 0.05f);
    }
}

void DyLayerStage::onPopupClicked(cocos2d::CCObject * pSender)
{
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
    
	if(pPopup->m_active_tag >= 11)
	{
        int shop;
        int delly;
        
        if(pPopup->m_active_tag >= 100)
        {
            shop = (pPopup->m_active_tag-100)/10;
            delly = atoi(stageManager->m_shopinfo->getValue("delly_after", shop));
        }
        else
        {
            shop = pPopup->m_active_tag/10;
            delly = atoi(stageManager->m_shopinfo->getValue("delly", shop));
        }
        
        DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
        
        if(moneyManager->getDelly() < delly)
        {
            DyMoneyManager::sharedMoneyManager()->alramNotEnoughDelly(true, true);
        }
        else
        {
            DyLib::sharedLib()->data_backup();
            
            stageManager->buyedShop(shop);
            moneyManager->addDelly(-delly, false);
            DyMenuItemShop* menuitem = (DyMenuItemShop*)m_menuShop->getChildByTag(shop);
            setShopButtonSprite(menuitem, shop);
            
            DyLib::sharedLib()->network_kakao_update_me(0, DyLayerStage_callfunc_update_me);
        }
    }
    
    
    dataManager->dataSave();
    DyDataManager::sharedDataManager()->checkAwards(DyAwardsCheckType_Shop, false);
}


void DyLayerStage::onOpenByGold(CCObject* obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    CCMenuItemImage* pMenu = dynamic_cast<CCMenuItemImage*>(obj);
	
    int shop = pMenu->getTag();
    
    DyMoneyManager::sharedMoneyManager()->questBuy(DyBuyItemType_Shop, (void*)shop, DyBuyMethod_Gold);
}


void DyLayerStage::onOpenByDelly(CCObject* obj)
{
    DyLib* lib = DyLib::sharedLib();
    DyStageManager* stageManager = DyStageManager::sharedStageManager();

    CCMenuItemImage* pMenu = dynamic_cast<CCMenuItemImage*>(obj);
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    int shop = pMenu->getTag();
    
    sprintf(lib->m_strTemp, "%s(을)를 구매하시겠어요? %s델리가 소모됩니다.", stageManager->m_shopinfo->getValue("name_kor", shop), stageManager->m_shopinfo->getValue("delly_after", shop));
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerStage::onPopupClicked));
    popup->setText(lib->m_strTemp);
    popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 10);
    popup->addButton(DyButtonType_Red, STRING_POPUP_YES, shop*10+100);
    popup->addButton(DyButtonType_Close, "", 10);
    
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);

}


void DyLayerStage::onDirectOpenByDelly(CCObject* obj)
{
    DyLib* lib = DyLib::sharedLib();
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    CCMenuItemImage* pMenu = dynamic_cast<CCMenuItemImage*>(obj);
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    int shop = pMenu->getTag();
    
    sprintf(lib->m_strTemp, "바로 오픈시 %d델리가 소모됩니다. 오픈하시겠습니까?", atoi(stageManager->m_shopinfo->getValue("delly", shop)));
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerStage::onPopupClicked));
    popup->setText(lib->m_strTemp);
    popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 10);
    popup->addButton(DyButtonType_Red, STRING_POPUP_YES, shop*10+1);
    popup->addButton(DyButtonType_Close, "", 10);
    
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerStage::onStageShopClicked(CCObject* obj)
{
	//DyLib *lib = DyLib::sharedLib();

	DyStageManager* stageManager = DyStageManager::sharedStageManager();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
	CCMenuItemSprite* menuSprite = dynamic_cast<CCMenuItemSprite*>(obj);
	DyStageType shop = (DyStageType)menuSprite->getTag();

	if(shop >= stageManager->m_cntShop)
	{
        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
        
		DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
		popup->setText("Coming soon~ ^^");
		popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);

		YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
	}
	else if(stageManager->isShopOpen(shop))
	{
        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
        
		m_menu->setEnabled(false);
		m_menuShop->setEnabled(false);
		m_layerMoney->pause();

        
        if(dataManager->m_local.m_viewPuddleStory == false && shop == DyStageType_Pizza)
        {
            dataManager->m_local.m_viewPuddleStory = true;
            
            dataManager->dataSave();
            DyLib::sharedLib()->setStatus(DyLib_AppStatus_Story, DyStoryType_Puddle);
        }
        else
        {
            m_doggy->m_actionManager->runAnimations("map_enter");
            CCSprite* sprCursorBg = (CCSprite*)getChildByTag(STAGS_UI_TAG_CURSOR_MAIN);
            sprCursorBg->setVisible(true);
            sprCursorBg->setDisplayFrame(CCSprite::create("image_shop_cursor_sel.png")->displayFrame());

            m_nSelectedShop = shop;

            
            callback_pushSubStage(0);
        }
	}
}


void DyLayerStage::update(float dt)
{
    DyNetworkManager::sharedNetworkManager()->update(dt);
}


void DyLayerStage::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader)
{
	DyLib *lib = DyLib::sharedLib();

	DyStageManager* stageManager = DyStageManager::sharedStageManager();

	this->setTag(100);
	setKeypadEnabled(true);

	CCArray* layerChildren = this->getChildren();

	for(unsigned int i = 0;i < layerChildren->count();i++)
	{
		CCNode* node = (CCNode*)layerChildren->objectAtIndex(i);

		if(node->getTag() == STAGE_UI_TAG_BACKGROUND)
			this->reorderChild(node, 1);
		else if(node->getTag() == STAGS_UI_TAG_CURSOR_MAIN)
			this->reorderChild(node, 2);
		else
			this->reorderChild(node, 10);
	}

	m_menuShop = CCMenu::create();
	m_menuShop->setPosition(getChildByTag(STAGS_UI_TAG_CURSOR_MAIN)->getPosition());
    addChild(m_menuShop, 5);
	m_menuShopGap = getChildByTag(STAGS_UI_TAG_CURSOR_MAIN)->getContentSize().width;

	m_cntShop = stageManager->m_cntShop + COMINGSOON_COUNT;

	for(int i = (int)DyStageType_Snack;i < m_cntShop;i++)
	{
		m_menuShop->addChild(createShopButton((DyStageType)i));
		CCSprite* sprPageCursorBg = CCSprite::createWithSpriteFrameName("menu_ui_page_cursor_default.png");
		m_arrPageCursorBg->addObject(sprPageCursorBg);
		sprPageCursorBg->setAnchorPoint(ccp(0, 0.5));
		sprPageCursorBg->setPosition(ccp(lib->m_sizeWin.width/2 - (50*m_cntShop)/2 + i*50, 50));
		addChild(sprPageCursorBg, 100);
	}

	m_sprPageCursor = CCSprite::createWithSpriteFrameName("menu_ui_page_cursor_current.png");
	m_sprPageCursor->setAnchorPoint(ccp(0, 0.5));

	m_sprPageCursor->setPosition(((CCSprite*)m_arrPageCursorBg->objectAtIndex(0))->getPosition());
	addChild(m_sprPageCursor, 100);

	CCMenuItemImage* menuitemShop = (CCMenuItemImage*)m_menuShop->getChildByTag(0);
	menuitemShop->setEnabled(true);

	CCLabelTTF* labelTitle = (CCLabelTTF*)getChildByTag(STAGS_UI_TAG_CURSOR_TITLELABEL);
	labelTitle->setFontName(FONT_NAME_EXP);
	labelTitle->setColor(ccc3(126, 72, 35));
	const char* shop_name = stageManager->m_shopinfo->getValue("name_kor", 0);
	labelTitle->setString(shop_name);

	m_menu = (CCMenu*)this->getChildByTag(STAGE_UI_TAG_MENU);
	m_menu->getChildByTag(STAGE_UI_TAG_MENUITEM_LEFT)->setVisible(false);

	m_layerMoney = new DyLayerMoney(DyLayerMoneyType_Normal, 1);

	m_layerMoney->setPosition(ccp((lib->m_sizeWin.width-m_layerMoney->getContentSize().width)/2+150, lib->m_sizeWin.height - 45));
	addChild(m_layerMoney,100);
	DyMoneyManager::sharedMoneyManager()->addMoneyLayer(m_layerMoney);

	m_doggy = YtAniObject::loadAniObject(this, "node_animal_doggy.ccbi");
	m_doggy->setPosition(ccp(730, 50));
	m_doggy->m_actionManager->runAnimations("map_hello");
	m_doggy->setScale(0.8f);
	DyCostumeManager::sharedCostumeManager()->setCostume(m_doggy, &DyDataManager::sharedDataManager()->m_public.m_costume_status, &DyDataManager::sharedDataManager()->m_custume_function);
	addChild(m_doggy, 100);

    YtLib::sharedLib()->playBGM(YtLibSndBGM_Title);
	//--YtLib::sharedLib()->playBGM(YtLibSndBGM_Menu);
    
    setKeypadEnabled(true);
    scheduleUpdate();
}


SEL_MenuHandler DyLayerStage::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onBackClicked", DyLayerStage::onBackClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onLeftClicked", DyLayerStage::onLeftClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onRightClicked", DyLayerStage::onRightClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onShopClicked", DyLayerStage::onShopClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onDoggyClicked", DyLayerStage::onDoggyClicked);

    return NULL;    
}


SEL_CCControlHandler DyLayerStage::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}

bool DyLayerStage::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerStage::onBackClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	DyLib::sharedLib()->setStatus(DyLib_AppStatus_MainMenu, 0);
}


void DyLayerStage::endAnimationShop(CCNode* node)
{
	DyStageManager* stageManager = DyStageManager::sharedStageManager();

	CCMenuItemImage* menuitemLeft = (CCMenuItemImage*)m_menu->getChildByTag(STAGE_UI_TAG_MENUITEM_LEFT);
	CCMenuItemImage* menuitemRight = (CCMenuItemImage*)m_menu->getChildByTag(STAGE_UI_TAG_MENUITEM_RIGHT);

	menuitemLeft->setEnabled(true);
	menuitemRight->setEnabled(true);

	menuitemLeft->setVisible(true);
	menuitemRight->setVisible(true);

	if(m_nSelectedShop == 0)
	{
		menuitemLeft->setVisible(false);
	}
	if(m_nSelectedShop == m_cntShop - 1)
	{
		menuitemRight->setVisible(false);
	}

    CCArray* pChildren = m_menuShop->getChildren();
	CCObject* pObj = NULL;
    CCARRAY_FOREACH(pChildren, pObj)
    {
        DyMenuItemShop* pChild = (DyMenuItemShop*)pObj;
		pChild->setEnabled(false);
    }

	CCMenuItemImage* menuitemShop = (CCMenuItemImage*)m_menuShop->getChildByTag(m_nSelectedShop);
	menuitemShop->setEnabled(true);

	m_sprPageCursor->setPosition(((CCSprite*)m_arrPageCursorBg->objectAtIndex(m_nSelectedShop))->getPosition());

	CCPoint position = getChildByTag(STAGS_UI_TAG_CURSOR_MAIN)->getPosition();
	CCPoint posTarget = ccp(position.x -m_nSelectedShop*m_menuShopGap, position.y);

	m_menuShop->setPosition(ccp(position.x -m_nSelectedShop*m_menuShopGap, position.y));

	if(stageManager->m_cntShop > m_nSelectedShop)
	{	
		getChildByTag(STAGS_UI_TAG_CURSOR_TITLEBG)->setVisible(true);
		getChildByTag(STAGS_UI_TAG_CURSOR_TITLELABEL)->setVisible(true);

		CCLabelTTF* labelTitle = (CCLabelTTF*)getChildByTag(STAGS_UI_TAG_CURSOR_TITLELABEL);
		labelTitle->setColor(ccc3(126, 72, 35));
		const char* shop_name = stageManager->m_shopinfo->getValue("name_kor", m_nSelectedShop);
		labelTitle->setString(shop_name);
	}
	else
	{
		getChildByTag(STAGS_UI_TAG_CURSOR_TITLEBG)->setVisible(false);
		getChildByTag(STAGS_UI_TAG_CURSOR_TITLELABEL)->setVisible(false);
	}
    
    
    for(int i = 0;i < m_arrMenuShopOpen->count();i++)
    {
        CCMenu* menuShopOpen = (CCMenu*)m_arrMenuShopOpen->objectAtIndex(i);
        if(menuShopOpen->getTag() == m_nSelectedShop)
            menuShopOpen->setEnabled(true);
        else
            menuShopOpen->setEnabled(false);
    }
}


void DyLayerStage::gotoShopCursor(DyStageType shop)
{
	CCMenuItemImage* menuitemLeft = (CCMenuItemImage*)m_menu->getChildByTag(STAGE_UI_TAG_MENUITEM_LEFT);
	CCMenuItemImage* menuitemRight = (CCMenuItemImage*)m_menu->getChildByTag(STAGE_UI_TAG_MENUITEM_RIGHT);

	if(shop < 0)
	{
		shop = DyStageType_Snack;
	}
	if(shop >= m_cntShop)
	{
		shop = (DyStageType)(m_cntShop - 1);
	}

	if(m_nSelectedShop != shop)
	{
		menuitemLeft->setEnabled(false);
		menuitemRight->setEnabled(false);

		for(int i = 0;i < m_cntShop;i++)
		{
			CCMenuItemImage* menuitemShop = (CCMenuItemImage*)m_menuShop->getChildByTag(i);
			menuitemShop->setEnabled(false);
		}

		m_nSelectedShop = shop;
		CCPoint position = getChildByTag(STAGS_UI_TAG_CURSOR_MAIN)->getPosition();

		CCPoint posTarget = ccp(position.x -shop*m_menuShopGap, position.y);

		m_menuShop->runAction(CCSequence::create(CCMoveTo::create(0.2f, posTarget),
			 CCCallFuncN::create(this, callfuncN_selector(DyLayerStage::endAnimationShop)),
			 NULL));
	}
}


void DyLayerStage::onLeftClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	gotoShopCursor((DyStageType)(m_nSelectedShop-1));
}


void DyLayerStage::onRightClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	gotoShopCursor((DyStageType)(m_nSelectedShop+1));
}

void DyLayerStage::onShopClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Normal, 0, 0);
	YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
}

void DyLayerStage::onDoggyClicked(CCObject* obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Myroom, 0, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
}


void DyLayerStage::keyBackClicked()
{
    setKeypadEnabled(false);
	onBackClicked(NULL);
}


void DyLayerStage::pause()
{
	m_menu->setEnabled(false);
	m_menuShop->setEnabled(false);
	m_layerMoney->pause();
    setKeypadEnabled(false);
    
    
    for(int i = 0;i < m_arrMenuShopOpen->count();i++)
    {
        CCMenu* menuShopOpen = (CCMenu*)m_arrMenuShopOpen->objectAtIndex(i);
        menuShopOpen->setEnabled(false);
    }
    
}


void DyLayerStage::resume()
{
	m_menu->setEnabled(true);
	m_menuShop->setEnabled(true);

	CCSprite* sprCursorBg = (CCSprite*)getChildByTag(STAGS_UI_TAG_CURSOR_MAIN);
	sprCursorBg->setDisplayFrame(CCSprite::create("image_shop_cursor.png")->displayFrame());
	m_layerMoney->resume();

	DyCostumeManager::sharedCostumeManager()->setCostume(m_doggy, &DyDataManager::sharedDataManager()->m_public.m_costume_status, &DyDataManager::sharedDataManager()->m_custume_function);

    CCArray* pChildren = m_menuShop->getChildren();
	CCObject* pObj = NULL;
	int shop = 0;
    CCARRAY_FOREACH(pChildren, pObj)
    {
        DyMenuItemShop* pChild = (DyMenuItemShop*)pObj;
		setShopButtonSprite(pChild, shop);
		shop++;
    }
	m_doggy->m_actionManager->runAnimations("map_hello");
    setKeypadEnabled(true);
    
    for(int i = 0;i < m_arrMenuShopOpen->count();i++)
    {
        CCMenu* menuShopOpen = (CCMenu*)m_arrMenuShopOpen->objectAtIndex(i);
        if(menuShopOpen->getTag() == m_nSelectedShop)
            menuShopOpen->setEnabled(true);
        else
            menuShopOpen->setEnabled(false);
    }
}


void DyLayerStage::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerStage::show()
{
}



void DyLayerStage_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str)
{
    DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
    DyLayerStage *layerStage = (DyLayerStage*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERSTAGE_TAG);
    
    DyLib::sharedLib()->show_network_alaram(false);
    
    if(value_int != 0)
    {
        return;
    }
    switch(cmdType)
    {
        case DyNetworkCmdType_ScoreFriendsGet:
        {
            
            friendManager->saveFriendsScore(layerStage->m_selectShop, (char*)value_str);
            
            DyStageManager::sharedStageManager()->setShop(layerStage->m_selectShop);
            
            DyLayerStageSub* layerStageSub = DyLayerStageSub::create(layerStage);
            YtLayerManager::sharedLayerManager()->pushLayer(layerStageSub, true, true);
            
        }
            break;
        default:
            break;
    }
    
}


void DyLayerStage::goShop(DyStageType shopType)
{
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    DyPopup* popup = DyLib::sharedLib()->show_network_alaram(true);
    popup->m_resumeAndDestory = false;
    
    
    m_selectShop = shopType;
    
    Json::Value parameter;
    Json::StyledWriter writer;
    
    parameter["shopType"] = shopType;
    parameter["stageCnt"] = stageManager->getCountLevel(shopType);
    
    
    DYNETWORK_ADD_PROC(DyNetworkCmdType_ScoreFriendsGet, writer.write(parameter), NULL, DyLayerStage_NetCallFunc);
}


DySceneStage::DySceneStage()
{
}

DySceneStage::~DySceneStage()
{
}

void DySceneStage::didSceneChanged()
{
	DyLib *lib = DyLib::sharedLib();
	DyLayerStage* layer = (DyLayerStage*)getChildByTag(100);

	if(lib->m_status_param >= 1)
	{
		if(lib->m_status_param >= 10)
			layer->m_nSelectedShop = (DyStageType)(lib->m_status_param - 10);
		else
			layer->m_nSelectedShop = (DyStageType)(lib->m_status_param);

		layer->endAnimationShop(NULL);

		if(lib->m_status_param >= 10)
		{
			layer->goShop((DyStageType)(lib->m_status_param-10));
		}
	}
}
