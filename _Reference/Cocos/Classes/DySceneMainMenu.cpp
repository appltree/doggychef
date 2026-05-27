/*
DySceneMainMenu.cpp
Doggy chef
@initialize at 120928

@history
120928	yoonsr	initialize
*/

#include "DySceneMainMenu.h"
#include "DyLib.h"
#include "DySceneGame.h"
#include "DyStage.h"
#include "DyLayerOption.h"
#include "DyLayerHelp.h"
#include "DyLayerShop.h"
#include "DyLayerPostbox.h"
#include "DyData.h"
#include "DyLayerRoullette.h"
#include "DyLayerInvite.h"
#include "DyLayerPride.h"
#include "DyGift.h"
#include "DyNotice.h"
#include "DyLayerNotice.h"
#include "DyTycoon.h"
#include "DyNetwork.h"
#include "DyToast.h"
#include "DyLayerEvent.h"
#include "json.h"


void DyLayerMainMenu_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str);



void DyLayerMainMenu::onCheatClicked(CCObject * pSender)
{
    if(g_isDebug)
        DyLib::sharedLib()->setStatus(DyLib_AppStatus_Cheat, 0);
}


DyLayerMainMenu::DyLayerMainMenu()
{
	m_doggy = NULL;
    m_tag = DYLAYERMAINMENU_TAG;
    m_layerRankGlobal = NULL;
}

DyLayerMainMenu::~DyLayerMainMenu()
{
	DyMoneyManager::sharedMoneyManager()->removeMoneyLayer(m_layerMoney);

	m_layerMoney->release();

	if(m_doggy)
		YtAniObject::unloadAniObject(m_doggy);
}



void DyLayerMainMenu::callfunc_nickname_success(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    layerMainMenu->unschedule(schedule_selector(DyLayerMainMenu::callfunc_nickname_success));
    
    std::string callFuncString = lib->m_buffer;
    
    if(callFuncString.length() == 0)//입력취소
    {
        //CCLog("callFuncString.length() == 0");
        
    }
    else//입력완료
    {
        //CCLog("callFuncString.length() == %s", callFuncString.c_str());
        
        DyPopup* popup = lib->show_network_alaram(true);
        popup->setText(STRING_TITLE_UPDATE_DATA);
        
        DYNETWORK_ADD_PROC(DyNetworkCmdType_NicknameRegist, callFuncString, NULL, DyLayerMainMenu_NetCallFunc);
    }
}


void DyLayerMainMenu_nickname_callfunc(void* value, int code)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    if(value != NULL)
    {
        lib->m_buffer = (std::string)(char*)value;
        
        layerMainMenu->schedule(schedule_selector(DyLayerMainMenu::callfunc_nickname_success), 0.05f);
    }
}


void DyLayerMainMenu_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str)
{
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    //CCLog("%s", (char*)value_str);
    
    if(value_int == 1)
    {
        return;
    }
    switch(cmdType)
    {
        case DyNetworkCmdtype_TotalScoreGet:
        {
            DyLib::sharedLib()->show_network_alaram(false);
            
            DyRankManager* rankManager = DyRankManager::sharedRankManager();
            
            rankManager->loadGlobalRank((char*)value_str);
            
            layerMainMenu->m_layerRankGlobal->loadGlobalRankList(DyRankType_Stage);
            
            ((CCMenuItemImage*)layerMainMenu->m_menuRank->getChildByTag(0))->unselected();
            ((CCMenuItemImage*)layerMainMenu->m_menuRank->getChildByTag(1))->selected();
            
            layerMainMenu->m_layerRankGlobal->setPosition(RANK_POS_SHOW);
            layerMainMenu->m_layerRank->setPosition(RANK_POS_HIDE);
        }
            break;
        case DyNetworkCmdType_NicknameExist:
        {
            if(value_int == 0)
            {
                DYNETWORK_ADD_PROC(DyNetworkCmdtype_TotalScoreGet, "story", NULL, DyLayerMainMenu_NetCallFunc);
            }
            else
            {
                DyLib::sharedLib()->show_network_alaram(false);
                DyDataManager* dataManager = DyDataManager::sharedDataManager();
                YtPlatform_ShowNicknameView(DyLayerMainMenu_nickname_callfunc, dataManager->m_nickname.c_str(), 0);

            }
        }
            break;
            
        case DyNetworkCmdType_NicknameRegist:
        {
            if(value_int == 0)
            {
                DYNETWORK_ADD_PROC(DyNetworkCmdtype_TotalScoreGet, "story", NULL, DyLayerMainMenu_NetCallFunc);
            }
            else
            {
                //등록 실패 하여 다시 입력 유도
                DyLib::sharedLib()->show_network_alaram(false);
                DyDataManager* dataManager = DyDataManager::sharedDataManager();
                YtPlatform_ShowNicknameView(DyLayerMainMenu_nickname_callfunc, dataManager->m_nickname.c_str(), 1);

//                DyLib::sharedLib()->show_network_alaram(false);
//                DyDataManager* dataManager = DyDataManager::sharedDataManager();
//                YtPlatform_ShowNicknameView(DyLayerMainMenu_nickname_callfunc, dataManager->m_nickname.c_str());
                
            }
        }
            break;


        default:
            break;
    }
}



void DyLayerMainMenu::onRankTabClicked(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    DyLib* lib = DyLib::sharedLib();
    
    
    CCMenuItemImage *pMenuItem = dynamic_cast<CCMenuItemImage*>(obj);
    switch (pMenuItem->getTag()) {
        case 0:
            
            ((CCMenuItemImage*)m_menuRank->getChildByTag(0))->selected();
            ((CCMenuItemImage*)m_menuRank->getChildByTag(1))->unselected();
 
            m_layerRank->setPosition(RANK_POS_SHOW);
            m_layerRankGlobal->setPosition(RANK_POS_HIDE);
            
            break;
        case 1:
            
            
            DyPopup* popup = lib->show_network_alaram(true);
            popup->setText(STRING_TITLE_UPDATE_DATA);
            
            DYNETWORK_ADD_PROC(DyNetworkCmdType_NicknameExist, "", NULL, DyLayerMainMenu_NetCallFunc);

            break;
    }
}


void DyLayerMainMenu::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
	YtLib* lib = YtLib::sharedLib();

	m_menu = (CCMenu*)getChildByTag(MAINMENU_UI_TAG_MENU);
    
    CCMenuItemImage* menuitem_post = (CCMenuItemImage*)m_menu->getChildByTag(MAINMENU_UI_TAG_MENUITEM_POSTBOX);
    
    m_sprGiftNew = CCSprite::createWithSpriteFrameName("menu_ui_new_mark.png");
    menuitem_post->addChild(m_sprGiftNew);
    m_sprGiftNew->setPosition(ccp(20, 100));

    m_sprGiftNew->setVisible(DyGiftManager::sharedGiftManager()->getGiftCount() > 0);
    
    
    CCMenuItemImage* menuitem_free = (CCMenuItemImage*)m_menu->getChildByTag(MAINMENU_UI_TAG_MENUITEM_ROULLETTE);
    m_sprRoulletteFree = CCSprite::createWithSpriteFrameName("menu_ui_free_mark.png");
    menuitem_free->addChild(m_sprRoulletteFree);
    m_sprRoulletteFree->setPosition(ccp(10, 80));
    m_sprRoulletteFree->setVisible(dataManager->m_private.m_cntFreeRoullete > 0);

	m_layerMoney = new DyLayerMoney(DyLayerMoneyType_Normal, 0);
	m_layerMoney->setPosition(ccp((lib->m_sizeWin.width-m_layerMoney->getContentSize().width)/2, lib->m_sizeWin.height - 45+10));
	addChild(m_layerMoney);
	DyMoneyManager::sharedMoneyManager()->addMoneyLayer(m_layerMoney);

	m_layerRank = DyLayerRank::create(DyLayerRankType_Friend);
	m_layerRank->setPosition(RANK_POS_SHOW);
	this->getChildByTag(MAINMENU_UI_TAG_RANKLAYER)->addChild(m_layerRank);
    
    m_layerRankGlobal = DyLayerRank::create(DyLayerRankType_Global);
    m_layerRankGlobal->setPosition(RANK_POS_HIDE);
    this->getChildByTag(MAINMENU_UI_TAG_RANKLAYER)->addChild(m_layerRankGlobal);
    
    m_menuRank = CCMenu::create();
    m_menuRank->setPosition(ccp(0, 0));
    CCMenuItemImage* menuitem_friend = CCMenuItemImage::create();
    menuitem_friend->initWithNormalSprite(CCSprite::createWithSpriteFrameName("menu2_ui_button_friend_off.png"),
                                          CCSprite::createWithSpriteFrameName("menu2_ui_button_friend_on.png"),
                                          NULL, this, menu_selector(DyLayerMainMenu::onRankTabClicked));
    
    menuitem_friend->setTag(0);
    m_menuRank->addChild(menuitem_friend);
    addChild(m_menuRank);
    
    CCMenuItemImage* menuitem_global = CCMenuItemImage::create();
    menuitem_global->initWithNormalSprite(CCSprite::createWithSpriteFrameName("menu2_ui_button_all_off.png"),
                                          CCSprite::createWithSpriteFrameName("menu2_ui_button_all_on.png"),
                                          NULL, this, menu_selector(DyLayerMainMenu::onRankTabClicked));
    
    
    menuitem_global->setTag(1);
    m_menuRank->addChild(menuitem_global);
    
    menuitem_friend->setAnchorPoint(ccp(1, 0));
    menuitem_friend->setPosition(ccp(-25+250-8, 374+160));
    
    menuitem_global->setAnchorPoint(ccp(0, 0));
    menuitem_global->setPosition(ccp(-25+10+250-8, 374+160));
    
    menuitem_friend->getSelectedImage()->setPosition(ccp(-18, 0));
    menuitem_global->getNormalImage()->setPosition(ccp(8,0));
    
    menuitem_friend->getNormalImage()->setPosition(ccp(-15, 0));
    menuitem_global->getSelectedImage()->setPosition(ccp(-28, 0));
    
    menuitem_friend->selected();
    
//    {
//        menuitem_friend->setAnchorPoint(ccp(0.5, 0));
//        //menuitem_friend->setPosition(ccp(-25, 374));
//        menuitem_friend->setPosition(ccp(-25+250-8, 374+160));
//        menuitem_friend->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("menu2_ui_button_friend_on.png")->displayFrame());
//    }
    
    
	setKeypadEnabled(true);

	CCSprite* sprPlay[4];
	sprPlay[0] = (CCSprite*)this->getChildByTag(MAINMENU_UI_TAG_PLAY_P);
	sprPlay[1] = (CCSprite*)this->getChildByTag(MAINMENU_UI_TAG_PLAY_L);
	sprPlay[2] = (CCSprite*)this->getChildByTag(MAINMENU_UI_TAG_PLAY_A);
	sprPlay[3] = (CCSprite*)this->getChildByTag(MAINMENU_UI_TAG_PLAY_Y);

	float delay = 0;

	for(int i = 0;i < 4;i++)
	{
		CCPoint orgP = sprPlay[i]->getPosition();
		CCPoint tgtP = ccp(orgP.x, orgP.y + 30);

		sprPlay[i]->runAction(CCRepeatForever::create((CCActionInterval*)CCSequence::create(
			CCDelayTime::create(1.0f + delay), 
			CCMoveTo::create(0.2f, tgtP),
			CCMoveTo::create(0.2f, orgP),
			CCDelayTime::create(.2f), 

			CCDelayTime::create(3.0f - delay), 
			NULL)));
  		sprPlay[i]->runAction(CCRepeatForever::create((CCActionInterval*)CCSequence::create(
			CCDelayTime::create(1.0f + delay), 
			CCRotateTo::create(0.2f, -8),
			CCRotateTo::create(0.2f, 3),
			CCRotateTo::create(0.2f, 0),

			CCDelayTime::create(3.0f - delay),
			NULL)));   
  		sprPlay[i]->runAction(CCRepeatForever::create((CCActionInterval*)CCSequence::create(
			CCDelayTime::create(1.0f + delay), 
			CCScaleTo::create(0.2f, 1.1f),
			CCScaleTo::create(0.2f, 0.95f),
			CCScaleTo::create(0.2f, 1.0f),

			CCDelayTime::create(3.0f - delay),
			NULL))); 

		delay += 0.2f;
	}

	m_doggy = YtAniObject::loadAniObject(this, "node_animal_doggy.ccbi");
	m_doggy->setPosition(ccp(490, 150));
	m_doggy->m_actionManager->runAnimations("title");
	DyCostumeManager::sharedCostumeManager()->setCostume(m_doggy, &DyDataManager::sharedDataManager()->m_public.m_costume_status, &DyDataManager::sharedDataManager()->m_custume_function);
	addChild(m_doggy);
    
    setKeypadEnabled(true);
    this->scheduleUpdate();
}


SEL_MenuHandler DyLayerMainMenu::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onGameStartClicked", DyLayerMainMenu::onGameStartClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onAnimationTestClicked", DyLayerMainMenu::onAnimationTestClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCheatClicked", DyLayerMainMenu::onCheatClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onMyRoomClicked", DyLayerMainMenu::onMyRoomClicked);

	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onShopClicked", DyLayerMainMenu::onShopClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPostboxClicked", DyLayerMainMenu::onPostboxClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onHelpClicked", DyLayerMainMenu::onHelpClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onOptionClicked", DyLayerMainMenu::onOptionClicked);
    
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onEventClicked", DyLayerMainMenu::onEventClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onFriendClicked", DyLayerMainMenu::onFriendClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onDoggyClicked", DyLayerMainMenu::onDoggyClicked);
    return NULL;    
}


SEL_CCControlHandler DyLayerMainMenu::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}


bool DyLayerMainMenu::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode)
{
    return false;
}


void DyLayerMainMenu::callfunc_update_me_success2(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    layerMainMenu->unschedule(schedule_selector(DyLayerMainMenu::callfunc_update_me_success2));
    
    lib->show_network_alaram(false);
    
    checkEvent();
}


void DyLayerMainMenu::callfunc_update_me_fail2(float dt)
{
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    layerMainMenu->unschedule(schedule_selector(DyLayerMainMenu::callfunc_update_me_fail2));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
}


void DyLayerMainMenu_callfunc_update_me2(int value, int code)
{
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    if(value == 0)
    {
        layerMainMenu->schedule(schedule_selector(DyLayerMainMenu::callfunc_update_me_success2), 0.05f);
    }
    else
    {
        layerMainMenu->schedule(schedule_selector(DyLayerMainMenu::callfunc_update_me_fail2), 0.05f);
    }
}


void DyLayerMainMenu::checkPresent()
{
    DyLib* lib = DyLib::sharedLib();
//    if(DyDataManager::sharedDataManager()->m_isGuestLogin)
//        return;
    
    lib->data_backup();
    
    //
    // 이벤트 체크 데이터를 서버에 저장하기 위해 미리 체크한다.
    DyDataManager::sharedDataManager()->checkEvent();
    
    int presentday = DyDataManager::sharedDataManager()->checkPresent();
    
    
    if(presentday > 0)
    {
        YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_COMPLETE);
        
        DyPopup* popup = DyPopup::create(DyPopupType_Present, this, callfuncO_selector(DyLayerMainMenu::onPresentPopupClicked));
        popup->setDay(presentday-1);
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
    else
        lib->network_kakao_update_me(0, DyLayerMainMenu_callfunc_update_me2);
}


void DyLayerMainMenu::onEnter()
{
    YtLayer::onEnter();

    YtLib::sharedLib()->playBGM(YtLibSndBGM_Title);
	
    DyNotice* notice = DyNoticeManager::sharedNoticeManager()->getCurrentNotice();
    
    if(notice)
    {
        DyLayerNotice* layerNotice = DyLayerNotice::create(this, notice);
        YtLayerManager::sharedLayerManager()->pushLayer(layerNotice, true, true);
        m_check_init = true;
    }
    else
    {
        checkPresent();
        m_check_init = false;
    }
}


void DyLayerMainMenu::onGameStartClicked(CCObject * pSender)
{
    //
    // 이중으로 눌러지는 버그 수정
    CCMenuItemImage * pMenu = dynamic_cast<CCMenuItemImage*>(pSender);
    pMenu->setEnabled(false);
    m_menu->setEnabled(false);
    m_menuRank->setEnabled(false);
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_EAT);

	DyStageManager* stageManager = DyStageManager::sharedStageManager();

	DyLib::sharedLib()->setStatus(DyLib_AppStatus_Stage, stageManager->getCurrentShop());
}


void DyLayerMainMenu::onAnimationTestClicked(CCObject * pSender) 
{
//	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
//
//	DyLib::sharedLib()->setStatus(DyLib_AppStatus_AnimationTest, 0);
}



void DyLayerMainMenu::onMyRoomClicked(cocos2d::CCObject * pSender)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Myroom, 0, 0);
	YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
}


void DyLayerMainMenu::onShopClicked(cocos2d::CCObject * pSender)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Normal, 0, 0);
	YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
}


void DyLayerMainMenu::callfunc_load_game_messages_success(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    layerMainMenu->unschedule(schedule_selector(DyLayerMainMenu::callfunc_load_game_messages_success));
    
    DyGiftManager* giftManager = DyGiftManager::sharedGiftManager();
    giftManager->loadGifts(lib->m_buffer);
    
    lib->network_get_present_list(httpresponse_selector(DyLayerMainMenu::onHttpRequestCompleted));
}


void DyLayerMainMenu::callfunc_load_game_messages_fail(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    layerMainMenu->unschedule(schedule_selector(DyLayerMainMenu::callfunc_load_game_messages_fail));
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerMainMenu::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerMainMenu_callfunc_load_game_messages(void* value, int code)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    if(value != NULL)
    {
        lib->m_buffer = (std::string)(char*)value;
        
        layerMainMenu->schedule(schedule_selector(DyLayerMainMenu::callfunc_load_game_messages_success), 0.05f);
    }
    else
    {
        layerMainMenu->schedule(schedule_selector(DyLayerMainMenu::callfunc_load_game_messages_fail), 0.05f);
    }
}


void DyLayerMainMenu::onPostboxClicked(cocos2d::CCObject * pSender)
{
    DyLib* lib = DyLib::sharedLib();
    
	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
//    if(DyDataManager::sharedDataManager()->m_isGuestLogin)
//    {
//        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
//        popup->setText(STRING_GUEST);
//        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
//        
//        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
//        return;
//    }
    
    lib->network_kakao_load_game_messages(DyLayerMainMenu_callfunc_load_game_messages);
}


void DyLayerMainMenu::onHelpClicked(cocos2d::CCObject * pSender)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

    //YtPlatform_ShowIndicator(true);
	DyLayerHelp* layerHelp = DyLayerHelp::create(this);
	YtLayerManager::sharedLayerManager()->pushLayer(layerHelp, true, true);
    //YtPlatform_ShowIndicator(false);
}


void DyLayerMainMenu::onOptionClicked(cocos2d::CCObject * pSender)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    DyLayerOption* layerOption = DyLayerOption::create(this, true);
    YtLayerManager::sharedLayerManager()->pushLayer(layerOption, true, true);
    
//    DyRankManager* rankManager = DyRankManager::sharedRankManager();
//    rankManager->initCheckPoint(DyRankType_Stage);
//    rankManager->setCheckPoint(true);
//    DyLib::sharedLib()->setStatus(DyLib_AppStatus_Rankup, 0);
    
    
//    DyLayerPride* layerPride = DyLayerPride::create(this, DyRankType_Stage, "TEST");
//    YtLayerManager::sharedLayerManager()->pushLayer(layerPride, true);
    
//	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("recipe_boonsik.plist");
//    DyStageManager* stageManager = DyStageManager::sharedStageManager();
//    stageManager->m_curShopType = (DyStageType)0;
//    
//    DyRecipeManager* recipeManager = DyRecipeManager::sharedRecipeManager();
//    
//    recipeManager->m_process_recipe_tag = "cheesekimbob";
//    
//    DyPopup* popup = DyPopup::create(DyPopupType_NewRecipe, this, callfuncO_selector(DyRecipeManager::onPopupClicked));
//	popup->setRecipe(recipeManager->m_process_recipe_tag);
//    
//    recipeManager->m_callFunc = DyLayerMainMenu_startGameReal;
//    //popup->setRecipe("hotcheeseddukbokki");
//	popup->addButton(DyButtonType_Blue, STRING_POPUP_SHOP, 1);
//	popup->addButton(DyButtonType_Red, STRING_POPUP_BUY, 2);
//    popup->addButton(DyButtonType_Close, "", 0);
//    
//	YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
    //sprintf(lib->m_strTemp, "%s %s%s%s", item->m_name.c_str(), lib->m_strTemp2, STRING_count, STRING_present_receive);
    
//    DyPopup* popup = DyPopup::create(DyPopuptype_RandomBox, this, callfuncO_selector(DyRecipeManager::onPopupClicked));
//	popup->setText("새로고침 20개가 지급 되었어요.");
//    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 10);
//    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerMainMenu::onEventClicked(cocos2d::CCObject * pSender)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    lib->network_get_param(httpresponse_selector(DyLayerMainMenu::onHttpRequestCompleted));
    
    
	//DyLayerRoullette* layerRoullette = DyLayerRoullette::create(this, true);
    
	//YtLayerManager::sharedLayerManager()->pushLayer(layerRoullette, true, true);
}


void DyLayerMainMenu::onFriendClicked(cocos2d::CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
//    if(DyDataManager::sharedDataManager()->m_isGuestLogin)
//    {
//        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
//        popup->setText(STRING_GUEST);
//        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
//        
//        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
//        return;
//    }

	DyLayerInvite* layerInvite = DyLayerInvite::create(this);
	YtLayerManager::sharedLayerManager()->pushLayer(layerInvite, true, true);
}


void DyLayerMainMenu::onDoggyClicked(cocos2d::CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Myroom, 0, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
}


void DyLayerMainMenu::onPopupClicked(cocos2d::CCObject * pSender)
{
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);

    if(pPopup->m_active_tag == 1000)
    {
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);

        DyLib::sharedLib()->data_restore();
        checkPresent();
    }
	else if(pPopup->m_active_tag == 3000)
    {
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        YtLib::app_end();// only android
    }
    else if(pPopup->m_active_tag == 10)
    {
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        checkEvent();
    }
    else
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerMainMenu::keyBackClicked()
{
    if(DyLib::sharedLib()->m_showLoadingPopup)
        return;

    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerMainMenu::onPopupClicked), CCSizeMake(460, 430));
    popup->setPicture(CCSprite::create("images_quit_image.png"), ccp(0, 40));
    popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
    popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 3000);
    popup->addButton(DyButtonType_Close, "", 0);
    
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerMainMenu::pause()
{
	setKeypadEnabled(false);
	m_menu->setTouchEnabled(false);
    m_menuRank->setTouchEnabled(false);

	m_layerMoney->pause();
	m_layerRank->pause();
    if(m_layerRankGlobal)
        m_layerRankGlobal->pause();
    setKeypadEnabled(false);
}


void DyLayerMainMenu::callNoticeCheck(float dt)
{
    this->unschedule(schedule_selector(DyLayerMainMenu::callNoticeCheck));
    if(m_check_init)
    {
        DyNotice* notice = DyNoticeManager::sharedNoticeManager()->getCurrentNotice();
        if(notice)
        {
            DyLayerNotice* layerNotice = DyLayerNotice::create(this, notice);
            YtLayerManager::sharedLayerManager()->pushLayer(layerNotice, true, true);
        }
        else
        {
            checkPresent();
            m_check_init = false;
        }
    }
}


void DyLayerMainMenu::resume()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
	setKeypadEnabled(true);
	m_menu->setTouchEnabled(true);
    m_menuRank->setTouchEnabled(true);

	m_layerMoney->resume();
	m_layerRank->resume();
    if(m_layerRankGlobal)
        m_layerRankGlobal->resume();

	DyCostumeManager::sharedCostumeManager()->setCostume(m_doggy, &DyDataManager::sharedDataManager()->m_public.m_costume_status, &DyDataManager::sharedDataManager()->m_custume_function);
    
    m_sprGiftNew->setVisible(DyGiftManager::sharedGiftManager()->getGiftCount() > 0);
    m_sprRoulletteFree->setVisible(dataManager->m_private.m_cntFreeRoullete > 0);
    
    setKeypadEnabled(true);
    
    schedule(schedule_selector(DyLayerMainMenu::callNoticeCheck), 0.1f);
}


void DyLayerMainMenu::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerMainMenu::show()
{
}


void DyLayerMainMenu::checkEvent()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(dataManager->m_is_need_checkevent)
    {
        DyLayerEvent* layerEvent = DyLayerEvent::create(this, dataManager->m_private.m_event_type == 0 ? DyEventStatus_Delly : DyEventStatus_Hero);
        YtLayerManager::sharedLayerManager()->pushLayer(layerEvent, true, true);
        dataManager->m_is_need_checkevent = false;
    }
}

void DyLayerMainMenu::callfunc_update_me_success(float dt)
{
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    layerMainMenu->unschedule(schedule_selector(DyLayerMainMenu::callfunc_update_me_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);

    layerMainMenu->m_popup_process->addButton(DyButtonType_Red, STRING_POPUP_OK, 10);
    YtLayerManager::sharedLayerManager()->pushLayer(layerMainMenu->m_popup_process, true, true);
    layerMainMenu->m_popup_process->release();
}


void DyLayerMainMenu::callfunc_update_me_fail(float dt)
{
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    layerMainMenu->unschedule(schedule_selector(DyLayerMainMenu::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    layerMainMenu->m_popup_process->release();

    DyPopup* popup = DyPopup::create(DyPopupType_Text, layerMainMenu, callfuncO_selector(DyLayerMainMenu::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerMainMenu_callfunc_update_me(int value, int code)
{
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    if(value == 0)
    {
        layerMainMenu->schedule(schedule_selector(DyLayerMainMenu::callfunc_update_me_success), 0.05f);
    }
    else
    {
         layerMainMenu->schedule(schedule_selector(DyLayerMainMenu::callfunc_update_me_fail), 0.05f);
    }
}


void DyLayerMainMenu::onPresentPopupClicked(cocos2d::CCObject * pSender)
{
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyLib* lib = DyLib::sharedLib();
    
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    
    const char* type = lib->m_parameters->getValue("value_string", pPopup->m_curday);
    int count = atoi(lib->m_parameters->getValue("value_integer", pPopup->m_curday));
    
    if(strcmp(type, "gold") == 0)
    {
        m_popup_process = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerMainMenu::onPopupClicked));
        
        lib->setMoneyStyle(lib->m_strTemp2, count*dataManager->m_present_award);
        sprintf(lib->m_strTemp, "%s%s%s", lib->m_strTemp2, STRING_gold, STRING_present_receive);
        m_popup_process->setText(lib->m_strTemp);
        DyMoneyManager::sharedMoneyManager()->addGold(count*dataManager->m_present_award, true);
        
    }
    else if(strcmp(type, "delly") == 0)
    {
        m_popup_process = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerMainMenu::onPopupClicked));
        
        lib->setMoneyStyle(lib->m_strTemp2, count*dataManager->m_present_award);
        sprintf(lib->m_strTemp, "%s%s%s", lib->m_strTemp2, STRING_delly, STRING_present_receive);
        m_popup_process->setText(lib->m_strTemp);
        DyMoneyManager::sharedMoneyManager()->addDelly(count*dataManager->m_present_award, true);
    }
    else if(strcmp(type, "roullette") == 0)
    {
        m_popup_process = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerMainMenu::onPopupClicked));
        
        lib->setMoneyStyle(lib->m_strTemp2, count*dataManager->m_present_award);
        sprintf(lib->m_strTemp, "룰렛 %s개가 지급 되었어요.", lib->m_strTemp2);
        m_popup_process->setText(lib->m_strTemp);
        dataManager->addRoullette(count*dataManager->m_present_award);
    }
    else if(strcmp(type, "gum") == 0)
    {
        m_popup_process = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerMainMenu::onPopupClicked));
        
        lib->setMoneyStyle(lib->m_strTemp2, count*dataManager->m_present_award);
        sprintf(lib->m_strTemp, "껌 %s개가 지급 되었어요.", lib->m_strTemp2);
        m_popup_process->setText(lib->m_strTemp);
        
        DyMoneyManager::sharedMoneyManager()->addGum(count*dataManager->m_present_award, true);
        DyDataManager::sharedDataManager()->m_rewardedGum += count*dataManager->m_present_award;
        
        DyMoneyManager::sharedMoneyManager()->addDelly(count*dataManager->m_present_award, true);
    }
    else if(strcmp(type, "random") == 0)
    {
        m_popup_process = DyPopup::create(DyPopuptype_RandomBox, this, callfuncO_selector(DyLayerMainMenu::onPopupClicked));

        DyItem* item = DyItemManager::sharedItemManager()->getItem((DyItemType)(rand()%DyItemType_Count));
        
        int item_count = 1;
        
        int random_num = rand()%5;
        
        switch(random_num)
        {
            case 0:
                item_count = 1;
                break;
            case 1:
                item_count = 3;
                break;
            case 2:
                item_count = 5;
                break;
            case 3:
                item_count = 10;
                break;
            case 4:
                item_count = 30;
                break;
        }
        DyItemManager::sharedItemManager()->addItem(item->m_itemtype, item_count*dataManager->m_present_award);
        
        lib->setMoneyStyle(lib->m_strTemp2, item_count*dataManager->m_present_award);
        sprintf(lib->m_strTemp, "%s %s%s%s", item->m_name.c_str(), lib->m_strTemp2, STRING_count, STRING_present_receive);
        m_popup_process->setText(lib->m_strTemp);
    }
    
    lib->network_kakao_update_me(0, DyLayerMainMenu_callfunc_update_me);
    m_popup_process->retain();
}


void DyLayerMainMenu::openPostBox()
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


void DyLayerMainMenu::onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response)
{
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    DyLib* lib = DyLib::sharedLib();
    
    if(lib->network_checkError(response, true) == false)
    {
        if(lib->network_getTag(response).compare(NETWORK_SERVICE_GET_PRESENT_LIST) == 0)
        {
            lib->show_network_alaram(false);
            lib->m_main_server_error = true;
            layerMainMenu->openPostBox();
        }
        
        if(lib->network_getTag(response).compare(NETWORK_SERVICE_GET_PARAM) == 0)
        {
            DyLayerRoullette* layerRoullette = DyLayerRoullette::create(this, true);
            YtLayerManager::sharedLayerManager()->pushLayer(layerRoullette, true, true);
        }

        return;
    }
    
    if(lib->network_getTag(response).compare(NETWORK_SERVICE_GET_PRESENT_LIST) == 0)
    {
        lib->show_network_alaram(false);
        
        lib->network_get_present_list_ret(sender, response);
        
        layerMainMenu->openPostBox();
    }
    
    if(lib->network_getTag(response).compare(NETWORK_SERVICE_GET_PARAM) == 0)
    {
        lib->network_get_param_ret(sender, response);
        
        DyLayerRoullette* layerRoullette = DyLayerRoullette::create(this, true);
        YtLayerManager::sharedLayerManager()->pushLayer(layerRoullette, true, true);
    }
}


void DyLayerMainMenu::update(float dt)
{
    DyNetworkManager* networkManager = DyNetworkManager::sharedNetworkManager();

    networkManager->update(dt);
}


DySceneMainMenu::DySceneMainMenu()
{
}


DySceneMainMenu::~DySceneMainMenu()
{
}


void DySceneMainMenu::didSceneChanged()
{
    static bool isEnter = false;
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(!isEnter && dataManager->m_greeting.length() > 0)
    {
        DyLib* lib = DyLib::sharedLib();
        
        DyToast* toast = DyToast::create(DyToastType_Text, this, callfuncO_selector(DyDataManager::onPopupClicked));
        
        sprintf(lib->m_strTemp, dataManager->m_greeting.c_str(), dataManager->m_nickname.c_str());
        toast->setText(lib->m_strTemp);
        toast->showToast();
        isEnter = true;
    }

}
