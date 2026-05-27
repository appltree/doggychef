/*
DyLayerGameStart.cpp
Doggy chef
@initialize at 121128

@history
121128	yoonsr	initialize
*/


#include "DyConfig.h"
#include "DyLayerGameStart.h"
#include "DyLib.h"
#include "DyTycoon.h"
#include "DyStage.h"
#include "DyLayerShop.h"
#include "CCextAnimatePacker.h"
#include "DyPopup.h"
#include "DyStaff.h"
#include "DyFriend.h"
#include "DyGuest.h"
#include "DyLayerInvite.h"
#include "DyTutorial.h"
#include "DyLayerPostbox.h"
#include "DyNotice.h"
#include "DyCustomer.h"

DyLayerGameStart::DyLayerGameStart()
{
    m_tag = DYLAYERGAMESTART_TAG;
    m_doggy = NULL;
	memset(m_sprSlotItem, 0, sizeof(CCSprite*)*DYITEM_SLOT_CNT);
	memset(m_sprInvenItem, 0, sizeof(CCSprite*)*DyItemType_Count);
    m_clickStart = false;
    m_goQuestDetailForTutorial = false;
}

DyLayerGameStart::~DyLayerGameStart()
{	
	DyMoneyManager::sharedMoneyManager()->removeMoneyLayer(m_layerMoney);

	m_layerMoney->release();
    
	if(m_doggy)
		YtAniObject::unloadAniObject(m_doggy);
}


void DyLayerGameStart::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
	DyLib* lib = DyLib::sharedLib();
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
	if(stageManager->getStageState(stageManager->m_curShopType, stageManager->m_curLevel) >= DyStageState_Goal)
	{
		tycoonManager->m_isReplay = true;
	}
	else
	{
		tycoonManager->m_isReplay = false;
	}
    
    ((CCLabelTTF*)getChildByTag(GAMESTART_UI_TAG_STAGE))->setFontName(FONT_NAME_DEFAULT);
    ((CCLabelTTF*)getChildByTag(GAMESTART_UI_TAG_STAGEVALUE))->setFontName(FONT_NAME_DEFAULT);
    ((CCLabelTTF*)getChildByTag(GAMESTART_UI_TAG_TIME))->setFontName(FONT_NAME_DEFAULT);
    ((CCLabelTTF*)getChildByTag(GAMESTART_UI_TAG_HEART))->setFontName(FONT_NAME_DEFAULT);
    ((CCLabelTTF*)getChildByTag(GAMESTART_UI_TAG_GOALVALUE))->setFontName(FONT_NAME_DEFAULT);
    
    ((CCLabelTTF*)getChildByTag(GAMESTART_UI_TAG_QUESTBONUS_PLUS))->setFontName(FONT_NAME_DEFAULT);
    ((CCLabelTTF*)getChildByTag(GAMESTART_UI_TAG_QUESTVALUE))->setFontName(FONT_NAME_DEFAULT);
    ((CCLabelTTF*)getChildByTag(GAMESTART_UI_TAG_QUESTVALUE_G))->setFontName(FONT_NAME_DEFAULT);
    
	m_menu = ((CCMenu*)getChildByTag(GAMESTART_UI_TAG_MENU));

	//
	// stage
	CCLabelTTF* labelStage = (CCLabelTTF *)getChildByTag(GAMESTART_UI_TAG_STAGEVALUE);
	sprintf(lib->m_strTemp, "%02d", stageManager->m_curLevel+1);
	labelStage->setString(lib->m_strTemp);
	
    
    //
    // time
	CCLabelTTF* labelTime = (CCLabelTTF *)getChildByTag(GAMESTART_UI_TAG_TIME);
    sprintf(lib->m_strTemp, "%d:%02d", (int)tycoonManager->m_time_limit/60, (int)tycoonManager->m_time_limit%60);
	labelTime->setString(lib->m_strTemp);

    //
    // heart
    CCLabelTTF* labelHeart = (CCLabelTTF *)getChildByTag(GAMESTART_UI_TAG_HEART);
    sprintf(lib->m_strTemp, "%d", tycoonManager->m_bonus_heart_cnt);
	labelHeart->setString(lib->m_strTemp);

    
	//
	// goal
 	CCLabelTTF* labelGoal = (CCLabelTTF *)getChildByTag(GAMESTART_UI_TAG_GOALVALUE);
	lib->setMoneyStyle(lib->m_strTemp, tycoonManager->m_goal_gold);
	labelGoal->setString(lib->m_strTemp);
    
	
	//
	// gum
	m_layerMoney = new DyLayerMoney(DyLayerMoneyType_OnlyGum, 1);

	//m_layerMoney->setPosition(ccp(-100, 195));
    
    if(tycoonManager->m_isCrazyMode)
        m_layerMoney->setPosition(ccp(-100, 90));
    else
        m_layerMoney->setPosition(ccp(-100, 180));
    
	addChild(m_layerMoney);
	DyMoneyManager::sharedMoneyManager()->addMoneyLayer(m_layerMoney);

	//
	// start button
    CCMenuItemImage* buttonStart = (CCMenuItemImage*)m_menu->getChildByTag(GAMESTART_UI_TAG_MENUITEM_START);
    buttonStart->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("menu_ui_startmenu_btn_start_gum.png")->displayFrame());
    buttonStart->setSelectedSpriteFrame(CCSprite::createWithSpriteFrameName("menu_ui_startmenu_btn_start_gum_sel.png")->displayFrame());

    if(tycoonManager->m_questbonus_gold == 0)
    {
        getChildByTag(GAMESTART_UI_TAG_QUESTBONUS_PLUS)->setVisible(false);
        getChildByTag(GAMESTART_UI_TAG_QUESTVALUE)->setVisible(false);
        getChildByTag(GAMESTART_UI_TAG_QUESTVALUE_G)->setVisible(false);
    }

	CCNode* nodeQuestContent = getChildByTag(GAMESTART_UI_TAG_QUESTCONTENT);

	CCLabelTTF* labelStageMsg = CCLabelTTF::create(stageManager->m_curShopInfo->getValue("stage_msg", stageManager->m_curLevel), FONT_NAME_EXP, 23.f,
		CCSizeMake(nodeQuestContent->getContentSize().width - 25, 70), kCCTextAlignmentLeft);
	labelStageMsg->setColor(ccWHITE);
	labelStageMsg->setAnchorPoint(ccp(0.5, 0.5));
	labelStageMsg->setPosition(ccp((nodeQuestContent->getContentSize().width)/2, 60));
	nodeQuestContent->addChild(labelStageMsg);

    std::string message = stageManager->m_curShopInfo->getValue("quest_msg", stageManager->m_curLevel);
    
	CCLabelTTF* labelQuestMsg = CCLabelTTF::create(message.c_str(), FONT_NAME_EXP, message.size() > 45 ? 25.f : 30.f);
	labelQuestMsg->setColor(ccc3(255,204,1));
	labelQuestMsg->setAnchorPoint(ccp(0.5, 0.5));
	labelQuestMsg->setPosition(ccp(nodeQuestContent->getContentSize().width/2, nodeQuestContent->getContentSize().height/2-40));
	nodeQuestContent->addChild(labelQuestMsg);
    
    
    if(tycoonManager->m_quest && tycoonManager->m_quest->m_questtype != DyQuestType_Ignore)
    {
        labelQuestMsg->runAction(CCSequence::create(
                                            CCScaleTo::create(0.1f, 1.4f),
                                            CCScaleTo::create(0.1f, 1.2f),
                                            CCScaleTo::create(0.1f, 1.4f),
                                            CCScaleTo::create(0.1f, 1.2f),
                                            CCScaleTo::create(0.1f, 1.4f),
                                            CCScaleTo::create(0.1f, 1.0f),
                                            NULL));
        
        if(tycoonManager->m_isReplay)
        {
            if(strcmp(tycoonManager->m_quest->m_questMainTag.c_str(), QUESTTAG_GUEST) == 0 && strcmp(tycoonManager->m_quest->m_questSubTag.c_str(), "delivery") != 0)
                labelQuestMsg->setString(stageManager->m_curShopInfo->getValue("quest_pannel", stageManager->m_curLevel));
        }
    }
    
    setGuest(false);
    
    
    m_layerItem = (CCLayer*)getChildByTag(GAMESTART_UI_TAG_ITEMLAYER);
    
    m_menuItem =  ((CCMenu*)m_layerItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_MENU));
    
    for(int i = 0;i < DYITEM_SLOT_CNT;i++)
		m_menuItemSlot[i] = (CCMenuItem*)m_menuItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_MENUITEM_USE_SLOT0 + i);
    
	for(int i = 0;i < DyItemType_Count;i++)
		m_menuItemInven[i] = (CCMenuItem*)m_menuItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_MENUITEM_INVEN_SLOT0 + i);
    
    
	CCScale9Sprite* sprBox = (CCScale9Sprite*)m_layerItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_DESCRIPTION_BOX);
    
	CCLabelTTF* labelTip = CCLabelTTF::create(STRING_item_slot_explain, FONT_NAME_EXP, 21.f);
	labelTip->setColor(ccWHITE);
	labelTip->setPosition(ccp(sprBox->getContentSize().width/2, sprBox->getContentSize().height/2));
	labelTip->setTag(GAMESTART_UI_TAG_ITEMLAYER_DESCRIPTION_TEXT);
	sprBox->addChild(labelTip);
	updateItemStatus();
    
	
	m_layerItem->reorderChild(m_layerItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_ITEM_DISABLE_LOCK_0), 100);
	m_layerItem->reorderChild(m_layerItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_ITEM_DISABLE_LOCK_1), 100);
    
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
    
    //
    // 캔디 제약
	if(itemManager->isAvailable(itemManager->getItem(DyItemType_Candy)))
	{
		m_layerItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_ITEM_DISABLE_BG_0)->setVisible(false);
		m_layerItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_ITEM_DISABLE_LOCK_0)->setVisible(false);
	}
	else
	{
		sprintf(lib->m_strTemp, "%d", itemManager->getItem(DyItemType_Candy)->m_stage);
        
		CCLabelTTF* labelCount =  CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 15.f);
		labelCount->setColor(ccc3(97,90,61));
		m_layerItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_ITEM_DISABLE_LOCK_0)->addChild(labelCount);
		labelCount->setPosition(ccp(12, 9));
	}
    
    
    //
    // 폭탄 제약
	if(itemManager->isAvailable(itemManager->getItem(DyItemType_Bomb)))
	{
		m_layerItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_ITEM_DISABLE_BG_1)->setVisible(false);
		m_layerItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_ITEM_DISABLE_LOCK_1)->setVisible(false);
	}
	else
	{
		sprintf(lib->m_strTemp, "%d", itemManager->getItem(DyItemType_Bomb)->m_stage);
        
		CCLabelTTF* labelCount =  CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 15.f);
		labelCount->setColor(ccc3(97,90,61));
		m_layerItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_ITEM_DISABLE_LOCK_1)->addChild(labelCount);
		labelCount->setPosition(ccp(12, 9));
	}

    
//    if(tycoonManager->m_isCrazyMode)
//    {
//        m_doggy = YtAniObject::loadAniObject(this, "node_animal_doggy.ccbi");
//        m_doggy->setPosition(ccp(-375, 30));
//        m_doggy->setScaleX(-0.6f);
//        m_doggy->setScaleY(0.6f);
//        
//        addChild(m_doggy);
//    }

    //updateCostume();
    
    setKeypadEnabled(true);
    
    DyRecipeManager::sharedRecipeManager()->clearGameCheck();
    
    m_sprIndicatorStart = NULL;
    if(!tycoonManager->m_isCrazyMode && stageManager->m_curShopType == DyStageType_Snack && stageManager->m_curLevel == 0 && stageManager->getStageState(0, 0) <= DyStageState_Fail)
    {
        m_sprIndicatorStart = lib->createIndicator(ccp(30+30, -139+30-30-30-20));
        addChild(m_sprIndicatorStart, ZORDER_GAME_TUTORIAL);
    }
    
    
    CCMenuItemImage* menuitem_post = (CCMenuItemImage*)m_menu->getChildByTag(GAMESTART_UI_TAG_MENUITEM_POSTBOX);
    
    m_sprGiftNew = CCSprite::createWithSpriteFrameName("menu_ui_new_mark.png");
    menuitem_post->addChild(m_sprGiftNew);
    m_sprGiftNew->setPosition(ccp(20, 125));
    
    m_sprGiftNew->setVisible(DyGiftManager::sharedGiftManager()->getGiftCount() > 0);

    
}


void DyLayerGameStart::setGuest(bool resume)
{
    if(m_clickStart)
        return;
    
//    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
//    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
//    //
//	// cashier
//    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
//    
//    if(tycoonManager->m_quest && tycoonManager->m_quest->m_questSubTag.compare("GOLD") == 0)
//    {
//        staffManager->setIdle(true, "이번 스테이지에서는 휴가중이라 고용할 수 없어요.");
//    }
//    else if(stageManager->m_curShopType == DyStageType_Snack && DyStageManager::sharedStageManager()->m_curLevel < 2)
//    {
//        staffManager->setIdle(true, "아직 고용할 수 없어요.");
//    }
//    else
//        staffManager->setIdle(false, "");
//    
//    
//    
//    if(!resume)
//        staffManager->setHire(staffManager->hasStaff() && staffManager->getPay() <= moneyManager->getGold());
    
    //
    //quest
    DyGuestManager* guestManager = DyGuestManager::sharedGuestManager();
    
    if(stageManager->m_curShopType == DyStageType_Snack && DyStageManager::sharedStageManager()->m_curLevel < 2)
    {
        guestManager->setIdle(true);
    }
    else
        guestManager->setIdle(false);
    
    //guestManager->setIdle(false);
    if(!resume)
        guestManager->setInvite(!guestManager->isIdle() && dataManager->m_private.m_cntGuest > 0);
    
    //buildCashierButton((CCMenuItemImage*)m_menu->getChildByTag(GAMESTART_UI_TAG_MENUITEM_CASHIER));
    buildGuestButton((CCMenuItemImage*)m_menu->getChildByTag(GAMESTART_UI_TAG_MENUITEM_GUEST));
    
    //buildPromoteButton((CCMenuItemImage*)m_menu->getChildByTag(GAMESTART_UI_TAG_MENUITEM_PROMTE));
    buildInviteButton((CCMenuItemImage*)m_menu->getChildByTag(GAMESTART_UI_TAG_MENUITEM_INVITE));
    
    
    std::string quest_explain = stageManager->m_curShopInfo->getValue("quest_explain", stageManager->m_curLevel);
    
    if(quest_explain.compare("NULL") == 0)
    {
        this->getChildByTag(GAMESTART_UI_TAG_GUEST_INFO)->setVisible(false);
    }
}


SEL_MenuHandler DyLayerGameStart::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName)
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onBackClicked", DyLayerGameStart::onBackClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onStartClicked", DyLayerGameStart::onStartClicked);
	//CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onItemClicked", DyLayerGameStart::onItemClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onShopClicked", DyLayerGameStart::onShopClicked);
	
    //CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCashierClicked", DyLayerGameStart::onCashierClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onGuestClicked", DyLayerGameStart::onGuestClicked);
	//CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPromoteClicked", DyLayerGameStart::onPromoteClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onInviteClicked", DyLayerGameStart::onInviteClicked);

	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onSlotClicked", DyLayerGameStart::onSlotClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onInvenClicked", DyLayerGameStart::onInvenClicked);

    
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCostumeClicked", DyLayerGameStart::onCostumeClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCostumeDetailClicked", DyLayerGameStart::onCostumeDetailClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onQuestDetailClicked", DyLayerGameStart::onQuestDetailClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPostboxClicked", DyLayerGameStart::onPostboxClicked);

    return NULL;    
}


SEL_CCControlHandler DyLayerGameStart::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}


bool DyLayerGameStart::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerGameStart::onBackClicked(CCObject* pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
	DyStageManager* stageManager = DyStageManager::sharedStageManager();

	m_menu->setEnabled(false);

	DyLib::sharedLib()->setStatus(DyLib_AppStatus_Stage, 10+stageManager->m_curShopType);
}


void DyLayerGameStart::keyBackClicked()
{
    if(DyLib::sharedLib()->m_showLoadingPopup)
        return;
    
    if(m_menu->isEnabled() == false)
        return;
    
    setKeypadEnabled(false);
    onBackClicked(NULL);
}


void DyLayerGameStart::startGame()
{	
	YtLayerManager::sharedLayerManager()->popLayer();
}


void DyLayerGameStart_callFuncRecipe(int value, int code)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    tycoonManager->startGameReady();
}


void DyLayerGameStart::destory()
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);

	if(DyCharacterManager::sharedCharacterManager()->checkNewCustomer(DyLayerGameStart_callFuncRecipe) == false)
	{
		if(DyRecipeManager::sharedRecipeManager()->checkNewRecipe(DyLayerGameStart_callFuncRecipe) == false)
			tycoonManager->startGameReady();
	}
    
    
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    if(staffManager->isHire())
	{
        tycoonManager->m_aniCashier->m_actionManager->runAnimations("default");
        
		tycoonManager->m_aniCashier->setVisible(true);
        
		tycoonManager->m_sprCashierDesk->setVisible(true);

        
        staffManager->hire();
	}
}


void DyLayerGameStart::onPopupClicked(cocos2d::CCObject * pSender)
{
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);

    if(pPopup->m_active_tag == 1)
	{
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
//        if(DyDataManager::sharedDataManager()->m_isGuestLogin)
//        {
//            DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
//            popup->setText(STRING_GUEST);
//            popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
//            
//            YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
//        }
//        else
        {
            DyLayerInvite* layerInvite = DyLayerInvite::create(this);
            YtLayerManager::sharedLayerManager()->pushLayer(layerInvite, true, true);
        }
	}

	else if(pPopup->m_active_tag == 2)
	{
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
		DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Normal, 1, 0);
		YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
	}
    else if(pPopup->m_active_tag == 10)
    {
        DyGuestManager* guestManager = DyGuestManager::sharedGuestManager();
        DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
        DyDataManager* dataManager = DyDataManager::sharedDataManager();
        
        if(moneyManager->getDelly() <= 0)
        {
            YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
            DyMoneyManager::sharedMoneyManager()->alramNotEnoughDelly(true, true);
        }
        else
        {
            guestManager->addGuest(1, DyGuestType_Buyed, NULL);
            moneyManager->addDelly(-1, true);
            dataManager->dataSave();
            YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        }
    }
    else if(pPopup->m_active_tag == 11)
    {
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
        popup->setText(STRING_INVITE_GUEST2);
        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
    else
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);

}


void DyLayerGameStart::startGumEffect()
{
    CCMenuItemImage* buttonStart = (CCMenuItemImage*)m_menu->getChildByTag(GAMESTART_UI_TAG_MENUITEM_START);
    CCPoint posTarget = buttonStart->getPosition();
    
	CCSprite* sprGumEff = CCSprite::createWithSpriteFrameName("game_ui_gum_eff_0.png");
	sprGumEff->setPosition(ccp(posTarget.x-152, posTarget.y-5));
	sprGumEff->runAction(CCSequence::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_gum"), CCFadeOut::create(.1f), NULL));

	addChild(sprGumEff);
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_GUM_DROP);
}




void DyLayerGameStart::callfunc_load_game_messages_success(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerGameStart *layerGameStart = (DyLayerGameStart*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMESTART_TAG);
    
    layerGameStart->unschedule(schedule_selector(DyLayerGameStart::callfunc_load_game_messages_success));
    
    DyGiftManager* giftManager = DyGiftManager::sharedGiftManager();
    giftManager->loadGifts(lib->m_buffer);
    
    lib->network_get_present_list(httpresponse_selector(DyLayerGameStart::onHttpRequestCompleted));
}


void DyLayerGameStart::callfunc_load_game_messages_fail(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerGameStart *layerGameStart = (DyLayerGameStart*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMESTART_TAG);
    
    layerGameStart->unschedule(schedule_selector(DyLayerGameStart::callfunc_load_game_messages_fail));
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerGameStart::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerGameStart_callfunc_load_game_messages(void* value, int code)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerGameStart *layerGameStart = (DyLayerGameStart*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMESTART_TAG);
    
    if(value != NULL)
    {
        lib->m_buffer = (std::string)(char*)value;
        
        layerGameStart->schedule(schedule_selector(DyLayerGameStart::callfunc_load_game_messages_success), 0.05f);
    }
    else
    {
        layerGameStart->schedule(schedule_selector(DyLayerGameStart::callfunc_load_game_messages_fail), 0.05f);
    }
}


void DyLayerGameStart::openPostBox()
{
    DyGiftManager* giftManager = DyGiftManager::sharedGiftManager();
    DyNoticeManager* noticeManager = DyNoticeManager::sharedNoticeManager();
    
    if(giftManager->getGiftCount() + noticeManager->getCount() > 0)
    {
        DyLayerPostbox* layerPostbox = DyLayerPostbox::create(this);
        YtLayerManager::sharedLayerManager()->pushLayer(layerPostbox, true, true);
    }
    else
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
        popup->setText("받은 메시지가 없습니다.");
        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
}


void DyLayerGameStart::onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response)
{
    DyLayerGameStart *layerGameStart = (DyLayerGameStart*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMESTART_TAG);
    
    DyLib* lib = DyLib::sharedLib();
    
    if(lib->network_checkError(response, true) == false)
    {
        if(lib->network_getTag(response).compare(NETWORK_SERVICE_GET_PRESENT_LIST) == 0)
        {
            lib->show_network_alaram(false);
            lib->m_main_server_error = true;
            layerGameStart->openPostBox();
        }
        return;
    }
    
    if(lib->network_getTag(response).compare(NETWORK_SERVICE_GET_PRESENT_LIST) == 0)
    {
        lib->show_network_alaram(false);
        
        lib->network_get_present_list_ret(sender, response);
        
        layerGameStart->openPostBox();
        
    }
}



void DyLayerGameStart::onPostboxClicked(CCObject* pSender)
{
    DyLib* lib = DyLib::sharedLib();
    
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
    if(tutorial && tutorial->m_command_type == DyTutorialCommandType_TouchQuestDetail)
    {
        return;
    }
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	
//    DyLayerPostbox* layerPostbox = DyLayerPostbox::create(this);
//    YtLayerManager::sharedLayerManager()->pushLayer(layerPostbox, true, true);
    lib->network_kakao_load_game_messages(DyLayerGameStart_callfunc_load_game_messages);
}




void DyLayerGameStart::onShopClicked(CCObject* pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
    if(tutorial && tutorial->m_command_type == DyTutorialCommandType_TouchQuestDetail)
    {
        return;
    }
    
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Normal, 4, 0);
	YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
}



void DyLayerGameStart::onCostumeClicked(CCObject* pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    if(tycoonManager->m_isCrazyMode)
    {
        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
        DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Myroom, 0, 0);
        YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
    }
}


void DyLayerGameStart::onCostumeDetailClicked(CCObject* pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    if(tycoonManager->m_isCrazyMode)
    {
        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
        
        DyPopup* popup = DyPopup::create(DyPopupType_Costume2, NULL, NULL);
        
        DyCostumeStatus costumeStatus = DyDataManager::sharedDataManager()->m_public.m_costume_status;
        
        popup->setCostumeInfo(&costumeStatus, &tycoonManager->m_functionGame);
        popup->addButton(DyButtonType_Close, "", 0);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
   }
}


void DyLayerGameStart::onQuestDetailClicked(CCObject* pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
    if(tutorial && tutorial->m_command_type == DyTutorialCommandType_TouchQuestDetail)
    {
        tutorial->m_sprIndicator->setVisible(false);
        m_goQuestDetailForTutorial = true;
    }
    
    DyStageManager* stageManager = DyStageManager::sharedStageManager();

    std::string quest_explain = stageManager->m_curShopInfo->getValue("quest_explain", stageManager->m_curLevel);
    std::string quest_explain_icon = stageManager->m_curShopInfo->getValue("quest_explain_icon", stageManager->m_curLevel);
    
    std::string quest_explain2 = stageManager->m_curShopInfo->getValue("quest_explain2", stageManager->m_curLevel);
    
    if(quest_explain.compare("NULL") == 0)
    {
        return;
    }
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    
    DyPopup* popup;
    
    if(quest_explain2.compare("NULL") == 0)
    {
        popup = DyPopup::create(DyPopupType_Quest, NULL, NULL);
        popup->setQuestInfo(quest_explain, quest_explain_icon);
    }
    else
    {
        popup = DyPopup::create(DyPopupType_Quest2, NULL, NULL);
        popup->setQuestInfo2(quest_explain, quest_explain2, quest_explain_icon);
    }
    
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerGameStart::startGumAni()
{
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();

    m_menu->setEnabled(false);
    moneyManager->addGum(-1, true);
    moneyManager->dataSave();
    
    CCSprite* sprGum = CCSprite::createWithSpriteFrameName("menushop_ui_gum.png");
    sprGum->setPosition(ccp(m_layerMoney->getPosition().x + 120, m_layerMoney->getPosition().y + 10));
    addChild(sprGum);
    
    CCCallFunc* callfun = CCCallFunc::create(this, callfunc_selector(DyLayerGameStart::startGame));
    
    CCMenuItemImage* buttonStart = (CCMenuItemImage*)m_menu->getChildByTag(GAMESTART_UI_TAG_MENUITEM_START);
    CCPoint posTarget = buttonStart->getPosition();
    
    sprGum->runAction(CCSequence::create(
                                         CCMoveTo::create(0.6f, ccp(posTarget.x-152, posTarget.y-5)),
                                         CCScaleTo::create(0.1f, 1.4f),
                                         CCScaleTo::create(0.1f, 1.2f),
                                         CCDelayTime::create(0.5f),
                                         callfun,
                                         NULL));
    
    sprGum->runAction(CCSequence::create(
                                         CCRotateBy::create(0.6f, 360),
                                         CCCallFunc::create(this, callfunc_selector(DyLayerGameStart::startGumEffect)),
                                         NULL));
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_GUM_FALL);
    
    if(m_sprIndicatorStart)
        m_sprIndicatorStart->setVisible(false);
}






void DyLayerGameStart::callfunc_update_me_success(float dt)
{
    DyLayerGameStart *layerGameStart = (DyLayerGameStart*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMESTART_TAG);
    
    unschedule(schedule_selector(DyLayerGameStart::callfunc_update_me_success));
    
    DyLib* lib = DyLib::sharedLib();

    lib->show_network_alaram(false);

    layerGameStart->startGumAni();
    
    //YtLayerManager::sharedLayerManager()->popLayer();
}



void DyLayerGameStart::callfunc_update_me_fail(float dt)
{
    DyLayerGameStart *layerGameStart = (DyLayerGameStart*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMESTART_TAG);
    
    unschedule(schedule_selector(DyLayerGameStart::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
    
    layerGameStart->m_clickStart = false;
}


void DyLayerGameStart_callfunc_update_me(int value, int code)
{
    DyLayerGameStart *layerGameStart = (DyLayerGameStart*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMESTART_TAG);
    
    if(value == 0)
    {
        layerGameStart->schedule(schedule_selector(DyLayerGameStart::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layerGameStart->schedule(schedule_selector(DyLayerGameStart::callfunc_update_me_fail), 0.05f);
    }
}




void DyLayerGameStart::callfunc_use_heart_success(float dt)
{
    this->unschedule(schedule_selector(DyLayerGameStart::callfunc_use_heart_success));
    
    
    
    DyLib::sharedLib()->network_kakao_update_me(0, DyLayerGameStart_callfunc_update_me);
}



void DyLayerGameStart::callfunc_use_heart_fail(float dt)
{
    DyLayerGameStart *layerGameStart = (DyLayerGameStart*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMESTART_TAG);
    
    this->unschedule(schedule_selector(DyLayerGameStart::callfunc_use_heart_fail));
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
    
    layerGameStart->m_clickStart = false;
}



void DyLayerGameStart_callfunc_use_heart(int value, int code)
{
    DyLayerGameStart *layerGameStart = (DyLayerGameStart*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMESTART_TAG);
    
    if(value == 0)
    {
        layerGameStart->schedule(schedule_selector(DyLayerGameStart::callfunc_use_heart_success), 0.05f);
    }
    else
    {
        layerGameStart->schedule(schedule_selector(DyLayerGameStart::callfunc_use_heart_fail), 0.05f);
    }
}


void DyLayerGameStart::onStartClicked(CCObject* pSender)
{
    if(m_clickStart)
        return;
    
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
    if(tutorial && tutorial->m_command_type == DyTutorialCommandType_TouchQuestDetail)
    {
        return;
    }
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
	DyLib* lib = DyLib::sharedLib();
    
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
    if(moneyManager->getGum() == 0)
    {
        DyPopup* popup = DyPopup::create(DyPopupType_GumWarning, this, callfuncO_selector(DyLayerGameStart::onPopupClicked));

        popup->addButton(DyButtonType_Blue, STRING_POPUP_INVITE, 1);
        popup->addButton(DyButtonType_Red, STRING_POPUP_BUY, 2);
        popup->addButton(DyButtonType_Close, "", 0);

        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
    else
    {
        if(!tycoonManager->m_isCrazyMode)
        {
            DyDataManager* dataManager = DyDataManager::sharedDataManager();
            DyGuestManager* guestManager = DyGuestManager::sharedGuestManager();
    
            customerManager->m_isHasGuestCustomer = DyGuestManager::sharedGuestManager()->isInvite();
            
            if(customerManager->m_isHasGuestCustomer)
            {
                if(dataManager->m_private.m_cntGuest == 0)
                {
                    guestManager->addGuest(1, DyGuestType_Buyed, NULL);
                    moneyManager->addDelly(-1, true);
                }
                DyGuestManager::sharedGuestManager()->useGuest();
            }
        }
        
        m_clickStart = true;
        lib->network_kakao_use_heart(DyLayerGameStart_callfunc_use_heart);
    }
}


void DyLayerGameStart::onGuestClicked(CCObject* pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
    if(tutorial && tutorial->m_command_type == DyTutorialCommandType_TouchQuestDetail)
    {
        return;
    }
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    DyGuestManager* guestManager = DyGuestManager::sharedGuestManager();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(guestManager->isIdle())
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
        
        popup->setText(STRING_STAGE_CANTNOTINVITE);
        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
    else
    {
        DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
        if(dataManager->m_private.m_cntGuest == 0 && !guestManager->isInvite() && moneyManager->getDelly() <= 0)
        {
            DyMoneyManager::sharedMoneyManager()->alramNotEnoughDelly(true, true);
            return;
        }
       
        guestManager->setInvite(!guestManager->isInvite());
        buildGuestButton((CCMenuItemImage*)m_menu->getChildByTag(GAMESTART_UI_TAG_MENUITEM_GUEST));
        
        if(dataManager->m_private.m_cntGuest == 0 && guestManager->isInvite())
        {
            DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL, CCSizeMake(440, 350));
            popup->setText(STRING_INVITE_GUEST);
            popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
            YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        }
    }
}


void DyLayerGameStart::onInviteClicked(CCObject* pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    

    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerGameStart::onPopupClicked), CCSizeMake(440, 350));
	
    popup->setText(STRING_INVITE_GUEST);
    popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 11);
    popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 10);
    popup->addButton(DyButtonType_Close, "", 11);
    
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerGameStart::buildGuestButton(CCMenuItemImage* menuItem)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyGuestManager* guestManager = DyGuestManager::sharedGuestManager();
    
    CCSprite* sprNormal = (CCSprite*)menuItem->getNormalImage();
    
    sprNormal->removeAllChildrenWithCleanup(true);
    
    
    CCSprite* sprLockCashier = CCSprite::createWithSpriteFrameName("menu_ui_startmenu_guest.png");
    sprNormal->addChild(sprLockCashier);
    sprLockCashier->setPosition(ccp(87, 40));
    
    
    CCSprite* sprSlotNumber = CCSprite::createWithSpriteFrameName("menu_ui_startmenu_slot_number.png");
    sprNormal->addChild(sprSlotNumber);
    sprSlotNumber->setPosition(ccp(134, 78));
    
    sprintf(lib->m_strTemp, "%d", dataManager->m_private.m_cntGuest);
    
    CCLabelTTF* labelNumber;
    
    if(dataManager->m_private.m_cntGuest < 10)
        labelNumber = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 30.f);
    else if(dataManager->m_private.m_cntGuest < 100)
        labelNumber = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 25.f);
    else
        labelNumber = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 20.f);
        
    
    sprSlotNumber->addChild(labelNumber);
    labelNumber->setPosition((ccp(20, 17)));
    
    
    CCSprite* sprCheckBox;
    
    if(guestManager->isIdle())
    {
        sprCheckBox = CCSprite::createWithSpriteFrameName("menu_ui_startmenu_checkbox_lock.png");
    }
    else
        sprCheckBox = CCSprite::createWithSpriteFrameName("menu_ui_startmenu_checkbox.png");

    sprNormal->addChild(sprCheckBox);
    sprCheckBox->setPosition(ccp(20, 64));
 
    
    if(dataManager->m_private.m_cntGuest == 0)
    {
        labelNumber->setColor(ccc3(245,0,8));
        
        //
        // -델리 표시
        CCSprite* sprDellyBg = CCSprite::createWithSpriteFrameName("menu2_ui_spend_delly.png");
        sprDellyBg->setPosition(ccp(135, 5));
        sprNormal->addChild(sprDellyBg);
        
        CCLabelTTF* labelDelly = CCLabelTTF::create("1", FONT_NAME_DEFAULT, 22);
        labelDelly->setColor(ccBLACK);
        labelDelly->setPosition(ccp(31,13));
        sprDellyBg->addChild(labelDelly);

    }
    else
    {
        labelNumber->setColor(ccc3(251,195,10));
        
        DyGuestManager::sharedGuestManager()->setupGuest();
        
        CCSprite* sprPictureFrame = DyGuestManager::sharedGuestManager()->getCurrentGuestPicture();
        
        if(sprPictureFrame)
        {
            sprPictureFrame->setPosition(ccp(115, 20));
            sprPictureFrame->setScale(0.8);
            sprNormal->addChild(sprPictureFrame);
        }
    }
    
    if(guestManager->isInvite())
    {
        CCSprite* sprCheck = CCSprite::createWithSpriteFrameName("menu_ui_startmenu_cashier_checker.png");
        sprNormal->addChild(sprCheck);
        sprCheck->setPosition(ccp(24, 75));
    }
}


void DyLayerGameStart::buildInviteButton(CCMenuItemImage* menuItem)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(dataManager->m_private.m_cntGuest == 0)
    {
        menuItem->setVisible(true);
    }
    else
    {
        menuItem->setVisible(false);
    }

}


void DyLayerGameStart::pause()
{
	m_menu->setEnabled(false);
	m_layerMoney->pause();
    
	m_menuItem->setEnabled(false);
    setKeypadEnabled(false);
    
}


void DyLayerGameStart::resume()
{
	m_menu->setEnabled(true);
	m_layerMoney->resume();
    
    setGuest(true);
    
    m_menuItem->setEnabled(true);
	updateItemStatus();

    //updateCostume();
    
    setKeypadEnabled(true);
    
    if(m_goQuestDetailForTutorial)
    {
        DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
        DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
        m_goQuestDetailForTutorial = false;
    }
    
    m_sprGiftNew->setVisible(DyGiftManager::sharedGiftManager()->getGiftCount() > 0);
    
}


void DyLayerGameStart::hide()
{
	YtLib* lib = YtLib::sharedLib();

	runAction(CCSequence::create(

		CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)), 
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height)),

		CCCallFunc::create(this, callfunc_selector(DyLayerGameStart::destory)),
		NULL));
}


void DyLayerGameStart::show()
{
	YtLib* lib = YtLib::sharedLib();

    YtLib::sharedLib()->playEffect(YtLibSndEff_CURTAIN_SPREAD);
    
	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));

	runAction(CCSequence::create(

		CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
		NULL));
}


DyLayerGameStart* DyLayerGameStart::create(CCNode* owner)
{
    DyLib* lib = DyLib::sharedLib();
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    
    DyLayerGameStart* layerGameStart = (DyLayerGameStart*)YtLayerManager::create("DyLayerGameStart", DyLayerGameStartLoader::loader(), "layer_game_start.ccbi", owner);
    
    if(tycoonManager->m_isCrazyMode)
    {
        DyRecipeManager* recipeManager = DyRecipeManager::sharedRecipeManager();
        
        layerGameStart->m_actionManager->runAnimations("crazy");
        
        CCLabelTTF* labelRecipe = (CCLabelTTF *)layerGameStart->getChildByTag(GAMESTART_UI_TAG_RECIPE_CNT);
        labelRecipe->setFontName(FONT_NAME_DEFAULT);
        
        sprintf(lib->m_strTemp, "%d/%d", recipeManager->getRecipeHasCnt(DyStageManager::sharedStageManager()->m_curShopType), recipeManager->getRecipeTotalCnt(DyStageManager::sharedStageManager()->m_curShopType));
        labelRecipe->setString(lib->m_strTemp);
        
        
        DyCharacterManager* characterManager = DyCharacterManager::sharedCharacterManager();
        
        CCLabelTTF* labelCustomer = (CCLabelTTF *)layerGameStart->getChildByTag(GAMESTART_UI_TAG_CUSTOMER_CNT);
        labelCustomer->setFontName(FONT_NAME_DEFAULT);
        
        sprintf(lib->m_strTemp, "%d/%d", characterManager->getCharacterDangolCnt(DyStageManager::sharedStageManager()->m_curShopType, true), characterManager->getCharacterTotalCnt(DyStageManager::sharedStageManager()->m_curShopType));
        labelCustomer->setString(lib->m_strTemp);
    }
    else
    {
        layerGameStart->m_actionManager->runAnimations("story");
        
        //
        // quest
        CCLabelTTF* labelBonus = (CCLabelTTF *)layerGameStart->getChildByTag(GAMESTART_UI_TAG_QUESTVALUE);
        lib->setMoneyStyle(lib->m_strTemp, tycoonManager->m_questbonus_gold);
        labelBonus->setString(lib->m_strTemp);
        
        CCLabelTTF* labelGoldMark = (CCLabelTTF*)layerGameStart->getChildByTag(GAMESTART_UI_TAG_QUESTVALUE_G);
        
        labelGoldMark->setPosition(ccp(labelBonus->getPosition().x + labelBonus->getContentSize().width + 5,
                                       labelBonus->getPosition().y-3));
    }
    
	return layerGameStart;
}




void DyLayerGameStart::onSlotClicked(CCObject* pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    
    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
    if(tutorial && tutorial->m_command_type == DyTutorialCommandType_TouchQuestDetail)
    {
        return;
    }
    

    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
    
	CCMenuItemImage* pMenu = dynamic_cast<CCMenuItemImage*>(pSender);
    
	int indexItem = pMenu->getTag()-GAMESTART_UI_TAG_ITEMLAYER_MENUITEM_USE_SLOT0;
    
	if(itemManager->m_itemslot[indexItem])
	{
		itemManager->m_itemslot[indexItem] = NULL;
	}
    
	updateItemStatus();
}


void DyLayerGameStart::onInvenClicked(CCObject* pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    
    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
    if(tutorial && tutorial->m_command_type == DyTutorialCommandType_TouchQuestDetail)
    {
        return;
    }
    

    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
    
	CCMenuItemImage* pMenu = dynamic_cast<CCMenuItemImage*>(pSender);
    
	int indexItem = pMenu->getTag()-GAMESTART_UI_TAG_ITEMLAYER_MENUITEM_INVEN_SLOT0;
	DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(indexItem);
	
	for(int i = 0;i < DyItemType_Count;i++)
	{
		m_menuItemInven[i]->unselected();
	}
    
	m_menuItemInven[indexItem]->selected();
    
	CCScale9Sprite* sprBox = (CCScale9Sprite*)m_layerItem->getChildByTag(GAMESTART_UI_TAG_ITEMLAYER_DESCRIPTION_BOX);
	sprBox->removeChildByTag(GAMESTART_UI_TAG_ITEMLAYER_DESCRIPTION_TEXT, true);
	sprBox->removeChildByTag(GAMESTART_UI_TAG_ITEMLAYER_DESCRIPTION_TEXT+1, true);
	
	CCLabelTTF* labelName = CCLabelTTF::create(item->m_name.c_str(), FONT_NAME_EXP, 21.f);
	labelName->setColor(ccc3(255, 204, 1));
	//labelTip->setPosition(ccp(sprBox->getContentSize().width/2, sprBox->getContentSize().height/2));
	labelName->setTag(GAMESTART_UI_TAG_ITEMLAYER_DESCRIPTION_TEXT);
	sprBox->addChild(labelName);
	float sizeWidthName = labelName->getContentSize().width;
    
	CCLabelTTF* labelExp = CCLabelTTF::create(item->m_explain_kor.c_str(), FONT_NAME_EXP, 21.f);
	labelExp->setColor(ccWHITE);
	//labelTip->setPosition(ccp(sprBox->getContentSize().width/2, sprBox->getContentSize().height/2));
	labelExp->setTag(GAMESTART_UI_TAG_ITEMLAYER_DESCRIPTION_TEXT+1);
	sprBox->addChild(labelExp);
	float sizeWidthExp = labelExp->getContentSize().width;
	float sizeWidthGap = 10.f;
    
	CCPoint posExpCenter = ccp(sprBox->getContentSize().width/2, sprBox->getContentSize().height/2);
    
	labelName->setPosition(ccp(posExpCenter.x - (sizeWidthName+sizeWidthGap+sizeWidthExp)/2, posExpCenter.y));
	labelName->setAnchorPoint(ccp(0, 0.5));
	labelExp->setPosition(ccp(posExpCenter.x - (sizeWidthName+sizeWidthGap+sizeWidthExp)/2 + sizeWidthName+sizeWidthGap, posExpCenter.y));
	labelExp->setAnchorPoint(ccp(0, 0.5));
    
	if(itemManager->isAvailable(item))
	{
		for(int i = 0;i < DYITEM_SLOT_CNT;i++)
		{
			if(itemManager->m_itemslot[i] == item)
			{
				return;
			}
		}
        
		for(int i = 0;i < DYITEM_SLOT_CNT;i++)
		{
			if(itemManager->m_itemslot[i] == NULL)
			{
				itemManager->m_itemslot[i] = item;
				break;
			}
		}
	}
	else
	{
		DyLib* lib = DyLib::sharedLib();
        
		sprintf(lib->m_strTemp, STRING_item_stage_notuse, item->m_name.c_str(), item->m_stage);
        
		DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
        
		popup->setText(lib->m_strTemp);
		popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
        
		YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
	}
    
	updateItemStatus();
}


void DyLayerGameStart::updateItemStatus()
{
	//YtLib* lib = YtLib::sharedLib();
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
    
    //
    // 상단 슬롯
	for(int i = 0;i < DYITEM_SLOT_CNT;i++)
	{
		if(m_sprSlotItem[i])
		{
			m_sprSlotItem[i]->removeFromParentAndCleanup(true);
			m_sprSlotItem[i] = NULL;
		}
        
		DyItem* item = itemManager->m_itemslot[i];
        
		if(item)
		{
			m_sprSlotItem[i] = itemManager->createItemIcon(item, true, true);
			m_sprSlotItem[i]->setPosition(ccp(m_menuItemSlot[i]->getPosition().x, m_menuItemSlot[i]->getPosition().y-15));
			m_layerItem->addChild(m_sprSlotItem[i]);
            
			CCSprite* sprRemoveMark = CCSprite::createWithSpriteFrameName("menu_ui_item_slot_remove.png");
			sprRemoveMark->setPosition(ccp(55, 55));
			m_sprSlotItem[i]->addChild(sprRemoveMark);
		}
	}
	
    //
    // 하단 인벤
	for(int i = 0;i < DyItemType_Count;i++)
	{
		bool inSlot = false;
		DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(i);
        
		if(m_sprInvenItem[i])
		{
			m_sprInvenItem[i]->removeFromParentAndCleanup(true);
			m_sprInvenItem[i] = NULL;
		}
        
		for(int j = 0;j < DYITEM_SLOT_CNT;j++)
		{
			if(item == itemManager->m_itemslot[j])
			{
				inSlot = true;
			}
		}
        
		if(itemManager->isAvailable(item))
		{
			m_sprInvenItem[i] = itemManager->createItemIcon(item, !inSlot, false);
		}
		else
			m_sprInvenItem[i] = itemManager->createItemIcon(item, false, false);
		
		m_sprInvenItem[i]->setPosition(ccp(m_menuItemInven[i]->getPosition().x, m_menuItemInven[i]->getPosition().y-10));
		m_layerItem->addChild(m_sprInvenItem[i]);
	}
}

