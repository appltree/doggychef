/*
DySceneTitle.cpp
Doggy chef
@initialize at 120928

@history
120928	yoonsr	initialize
*/
#include "DyConfig.h"
#include "DySceneTitle.h"
#include "DyLib.h"
#include "DyData.h"
#include "DyFriend.h"
#include "DyCharacter.h"
#include "DyRecipe.h"
#include "DyToast.h"
#include "DyMoney.h"
#include "DyLayerPromise.h"

#include "platform/CCImage.h"
#include "textures/AWTextureEffects.h"

#include "pugixml.hpp"

#include "json.h"
#include "DyNetwork.h"
#include "DyNotice.h"
#include "DyStage.h"

DyLayerTitle::DyLayerTitle()
{
    m_tag = DYLAYERTITLE_TAG;
    
    m_arrFriend = NULL;
    
    m_callFuncStep = KakaoAPIStatus_None;
}

DyLayerTitle::~DyLayerTitle()
{
    if(m_arrFriend)
        m_arrFriend->release();
}


void DyLayerTitle::onEnter()
{
    YtLayer::onEnter();

	DyDataManager::sharedDataManager();
	DyFriendManager::sharedFriendManager();
}


void DyLayerTitle::onExit()
{
	YtLayer::onExit();
}

void DyLayerTitle::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
    DyLib* lib = DyLib::sharedLib();
    
    m_menu = (CCMenu*)getChildByTag(TITLE_UI_TAG_MENU);
    
    m_menu->setVisible(false);
    
    lib->setNormalButton((CCMenuItemImage*)m_menu->getChildByTag(TITLE_UI_TAG_MENUITEM_LOGIN), NULL);
    lib->setNormalButton((CCMenuItemImage*)m_menu->getChildByTag(TITLE_UI_TAG_MENUITEM_LOGIN_GUEST), NULL);
    
    
    scheduleUpdate();
    setKeypadEnabled(true);
    
    
    DyNoticeManager::sharedNoticeManager()->reset();
    
//    CCLayer* layerDeco = (CCLayer*)getChildByTag(TITLE_UI_DECO_LAYER);
//    for(int i = 0;i < 8;i++)
//    {
//        CCSprite* sprDama = (CCSprite*)layerDeco->getChildByTag(i)->getChildByTag(0);
//        sprDama->setOpacity(0);
//        
//        CCCallFunc* callfun = CCCallFuncN::create(this, callfuncN_selector(DyLayerTitle::startBlink));
//
//        
//        sprDama->runAction(CCSequence::create(CCDelayTime::create((rand()%10)*0.1f), callfun, NULL));
//    }
    
}

void DyLayerTitle::startBlink(CCNode* node)
{
    node->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCFadeIn::create(.3f), CCFadeOut::create(.3f), NULL))));
}


SEL_MenuHandler DyLayerTitle::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onLoginClicked", DyLayerTitle::onLoginClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onGuestLoginClicked", DyLayerTitle::onGuestLoginClicked);

    return NULL;    
}


SEL_CCControlHandler DyLayerTitle::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}


bool DyLayerTitle::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerTitle_CallFunc_KakaoIsLogin(int value, int code)
{
    DyLayerTitle* layerTitle = (DyLayerTitle*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERTITLE_TAG);
    layerTitle->m_callFuncStep = KakaoAPIStatus_IsLogin;
    layerTitle->m_callFuncValue = value;
}


void DyLayerTitle_CallFunc_KakaoLogin(int value, int code)
{
    DyLayerTitle* layerTitle = (DyLayerTitle*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERTITLE_TAG);
    
    layerTitle->m_callFuncStep = KakaoAPIStatus_Login;
    layerTitle->m_callFuncValue = value;
}


void DyLayerTitle_CallFunc_KakaoLoginStarted(int value, int code)
{
    DyLayerTitle* layerTitle = (DyLayerTitle*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERTITLE_TAG);
    
    layerTitle->m_callFuncStep = KakaoAPIStatus_LoginStarted;
    layerTitle->m_callFuncValue = value;

}


void DyLayerTitle_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyLayerTitle* layerTitle = (DyLayerTitle*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERTITLE_TAG);
    
    if(value_int != 0)
        return;
    
    switch(cmdType)
    {
        case DyNetworkCmdType_LoadGameInfo:
            layerTitle->m_callFuncStep = KakaoAPIStatus_LoadGameInfo;
            if(value_str == NULL)
                layerTitle->callFuncString = "";
            else
                layerTitle->callFuncString = (char*)value_str;
            break;
        case DyNetworkCmdType_LoadMe:
            layerTitle->m_callFuncStep = KakaoAPIStatus_LoadGameMe;
            if(value_str == NULL)
                layerTitle->callFuncString = "";
            else
                layerTitle->callFuncString = (char*)value_str;
            break;
        case DyNetworkCmdType_ScoreMigration:
        {
            //dataManager->data_private_migration_2_3(&dataManager->m_private);
            
            layerTitle->goNext();
            
            //DYNETWORK_ADD_PROC(DyNetworkCmdType_UpdateMe, "", NULL, DyLayerTitle_NetCallFunc);
        }
            
            break;
        case DyNetworkCmdType_ScoreGet:
        {
            dataManager->saveScore((char*)value_str);
            
            layerTitle->goNext();
        }
            
            break;

        case DyNetworkCmdtype_UserRegist:
            YtPlatform_CheckPush();
            layerTitle->checkAppStatus();
            
            break;
//        case DyNetworkCmdType_UpdateMe:
//            layerTitle->goNext();
//            break;
        default:
            break;
    }
    
}


void DyLayerTitle::update(float dt)
{
    DyNetworkManager* networkManager = DyNetworkManager::sharedNetworkManager();
    DyLayerTitle* layerTitle = (DyLayerTitle*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERTITLE_TAG);
    
    switch(m_callFuncStep)
    {
        case KakaoAPIStatus_None:
            break;
        case KakaoAPIStatus_IsLogin:
            // 로그인이 필요함
            if(m_callFuncValue == 1)
            {
                setLoginUI();
            }
            else
            {
                DYNETWORK_ADD_PROC(DyNetworkCmdType_LoadGameInfo, "", NULL, DyLayerTitle_NetCallFunc);
                
//                DyNetworkCommand* commandNetwork = new DyNetworkCommand(DyNetworkCmdType_LoadGameInfo, "", NULL, DyLayerTitle_NetCallFunc);
//                networkManager->addLast(commandNetwork);
//                commandNetwork->release();
            }
            break;
        case KakaoAPIStatus_Login:
            //
            // 로그인 성공
            if(m_callFuncValue == 0)
            {
                DYNETWORK_ADD_PROC(DyNetworkCmdType_LoadGameInfo, "", NULL, DyLayerTitle_NetCallFunc);
//                DyNetworkCommand* commandNetwork = new DyNetworkCommand(DyNetworkCmdType_LoadGameInfo, "", NULL, DyLayerTitle_NetCallFunc);
//                networkManager->addLast(commandNetwork);
//                commandNetwork->release();
            }
            else if(m_callFuncValue == 1)
            {
                //
                //로그인 실패
                DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerTitle::onPopupClicked));
                popup->setText(STRING_TITLE_LOGIN_FAILURE);
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
            }
//            else if(m_callFuncValue == 2)
//            {
//                DyDataManager* dataManager = DyDataManager::sharedDataManager();
//                
//                dataManager->m_nickname = "GUEST";
//                dataManager->m_isGuestLogin = true;
//                dataManager->reset();
//                
//                dataManager->m_gum = 5;
//                dataManager->m_rechargeable_gum = 5;
//                
//                dataManager->m_max_gum = 999;
//                if(dataManager->m_gum < dataManager->m_rechargeable_gum)
//                {
//                    DyMoneyManager::sharedMoneyManager()->m_isDoCheckTimeForGum = true;
//                }
//                
//                
//                dataManager->m_heart_regen_interval = 600;
//                
//                CCDate dateNow;
//                dataManager->m_time_diff = dateNow;
//                
//                dataManager->m_crazyRecord = 0;
//                dataManager->m_crazyBestRecord = 0;
//                
//                dataManager->m_message_blocked = true;
//                
//                dataManager->m_user_id = "GUEST LOGIN";
//                 
//                goNext();
//            }
            break;
        case KakaoAPIStatus_LoginStarted:
            m_menu->setVisible(false);
            
            break;
        case KakaoAPIStatus_LoadGameInfo:
            {
                if(callFuncString.length() == 0)
                {
                    DyLib::sharedLib()->show_network_alaram(false);

                    DyPopup* popup = DyPopup::create(DyPopupType_Text, layerTitle, callfuncO_selector(DyLayerTitle::onPopupClicked));
                    
                    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE_END);
                    popup->addButton(DyButtonType_Red, "OK", 4000);
                    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
                    
                }
                else
                {
                    
                    DyLib::sharedLib()->network_proc_game_info(callFuncString);
                    DYNETWORK_ADD_PROC(DyNetworkCmdType_LoadMe, "", NULL, DyLayerTitle_NetCallFunc);
//                    DyNetworkCommand* commandNetwork = new DyNetworkCommand(DyNetworkCmdType_LoadMe, "", NULL, DyLayerTitle_NetCallFunc);
//                    networkManager->addLast(commandNetwork);
//                    commandNetwork->release();
                }
            }
            break;
        case KakaoAPIStatus_LoadGameMe:
        {
            if(callFuncString.length() == 0)
            {
                DyLib::sharedLib()->show_network_alaram(false);

                DyPopup* popup = DyPopup::create(DyPopupType_Text, layerTitle, callfuncO_selector(DyLayerTitle::onPopupClicked));
                
                popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
                popup->addButton(DyButtonType_Red, "OK", 5000);
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);

            }
            else
            {
                if(DyLib::sharedLib()->network_proc_game_me(callFuncString))
                {
                    //layerTitle->checkAppStatus();
                    DYNETWORK_ADD_PROC(DyNetworkCmdtype_UserRegist, "", NULL, DyLayerTitle_NetCallFunc);
                }
                else
                {
                    DyLib::sharedLib()->show_network_alaram(false);
                    
                    DyPopup* popup = DyPopup::create(DyPopupType_Text, layerTitle, callfuncO_selector(DyLayerTitle::onPopupClicked));
                    
                    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
                    popup->addButton(DyButtonType_Red, "OK", 5000);
                    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
                }
                
            }
        }
            break;
        default:
            break;
    }
    m_callFuncStep = KakaoAPIStatus_None;
    networkManager->update(dt);
}


void DyLayerTitle::net_IsLogin()
{
    //CCLog("DyLayerTitle::net_IsLogin()");
    YtPlatform_NetKakao_IsLogin(DyLayerTitle_CallFunc_KakaoIsLogin);
}


void DyLayerTitle::net_Login(bool isGeust)
{
    
    //CCLog("DyLayerTitle::net_Login()");
    YtPlatform_NetKakao_Login(isGeust, DyLayerTitle_CallFunc_KakaoLogin, DyLayerTitle_CallFunc_KakaoLoginStarted);
}



void DyLayerTitle::onLoginClicked(cocos2d::CCObject * pSender)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    net_Login(false);
}


void DyLayerTitle::onGuestLoginClicked(cocos2d::CCObject * pSender)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    net_Login(true);
}


void DyLayerTitle::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
	CCSetIterator it;
	CCTouch* touch;

	for(it = touches->begin();it != touches->end();it++) 
	{
		touch = (CCTouch*)(*it);
		if(!touch)
			break;

        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_EAT);
		goMainMenu();
		return;
	}
}


void DyLayerTitle::setLoginUI()
{
    m_menu->setVisible(true);
    
#ifdef ANDROID
    m_menu->getChildByTag(TITLE_UI_TAG_MENUITEM_LOGIN_GUEST)->setVisible(false);
#endif
    

}


void DyLayerTitle::onPopupClicked(cocos2d::CCObject * pSender)
{
//    DyLayerTitle* layerTitle = (DyLayerTitle*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERTITLE_TAG);
    DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    
    
	if(pPopup->m_active_tag == 3000)
	{
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        YtLib::app_end();
 	}
	else if(pPopup->m_active_tag == 4000)
	{
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        //DyNetworkManager* networkManager = DyNetworkManager::sharedNetworkManager();

        DYNETWORK_ADD_PROC(DyNetworkCmdType_LoadGameInfo, "", NULL, DyLayerTitle_NetCallFunc);
//        DyNetworkCommand* commandNetwork = new DyNetworkCommand(DyNetworkCmdType_LoadGameInfo, "", NULL, DyLayerTitle_NetCallFunc);
//        networkManager->addLast(commandNetwork);
//        commandNetwork->release();
 	}
	else if(pPopup->m_active_tag == 5000)
	{
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        DYNETWORK_ADD_PROC(DyNetworkCmdType_LoadMe, "", NULL, DyLayerTitle_NetCallFunc);
        
//        DyNetworkManager* networkManager = DyNetworkManager::sharedNetworkManager();
//        
//        DyNetworkCommand* commandNetwork = new DyNetworkCommand(DyNetworkCmdType_LoadMe, "", NULL, DyLayerTitle_NetCallFunc);
//        networkManager->addLast(commandNetwork);
//        commandNetwork->release();
 	}

    else if(pPopup->m_active_tag == 300)
    {
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        YtLib::app_end();
    }
//    else if(pPopup->m_active_tag == 400)
//    {
//        layerTitle->m_isDataReset = true;
//        
//        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
//        
//        DyDataManager::sharedDataManager()->reset();
//        
//        DyNetworkManager* networkManager = DyNetworkManager::sharedNetworkManager();
//        
//        DyNetworkCommand* commandNetwork = new DyNetworkCommand(DyNetworkCmdType_UpdateMe, "", NULL, DyLayerTitle_NetCallFunc);
//        networkManager->addLast(commandNetwork);
//        commandNetwork->release();
//    }
    else if(pPopup->m_active_tag == 100)
    {
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        YtLib::app_end();
    }
    else if(pPopup->m_active_tag == 110)
    {
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        goNext();
    }
    else if(pPopup->m_active_tag == 101 || pPopup->m_active_tag == 111)
    {
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        YtPlatform_GoWeb(DyDataManager::sharedDataManager()->m_link.c_str());
        
        YtLib::app_end();
    }
    else
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerTitle::goMainMenu()
{
    YtPlatform_InitInApp();
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("menushop_ui.plist");
    DyLib::sharedLib()->setStatus(DyLib_AppStatus_MainMenu, 0);

}

void DyLayerTitle::goNext()
{
    goMainMenu();
}


void DyLayerTitle::checkAppStatus()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    m_menu->setVisible(false);
    
    if(dataManager->m_notice.length() > 0)
    {
        DyLib::sharedLib()->show_network_alaram(false);
        //
        // 반드시 다운로드
        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerTitle::onPopupClicked), CCSizeMake(500, 500));
        popup->setText(dataManager->m_notice.c_str());
        
        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 300);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        return;
    }
    
    // 버전
    if(CONFIG_VER < dataManager->m_min_version)
    {
        DyLib::sharedLib()->show_network_alaram(false);

        //
        // 반드시 다운로드
        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerTitle::onPopupClicked));
        popup->setText("업데이트를 하셔야 이용하실 수 있습니다. 업데이트로 이동하시겠어요?");
        
        popup->addButton(DyButtonType_Blue, STRING_POPUP_EXIT, 100);
        popup->addButton(DyButtonType_Red, STRING_POPUP_UPDATE, 101);
        popup->addButton(DyButtonType_Close, "", 100);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        
        return;
    }
    
    //
    // 스테이지 데이터 마이그레이션
    //    if(dataManager->m_private.m_version == 2)
//    {
//        
//        return;
//    }
    
    
    if(dataManager->m_is_need_privatedata_migration)
    {
        DYNETWORK_ADD_PROC(DyNetworkCmdType_ScoreMigration, "", NULL, DyLayerTitle_NetCallFunc);
        dataManager->m_is_need_privatedata_migration = false;
    }
    else // 스테이지 데이타를 가져와야한다.
    {
        DYNETWORK_ADD_PROC(DyNetworkCmdType_ScoreGet, "", NULL, DyLayerTitle_NetCallFunc);
    }
    
    //
}



void DyLayerTitle::keyBackClicked()
{
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerTitle::onPopupClicked), CCSizeMake(460, 430));
    popup->setPicture(CCSprite::create("images_quit_image.png"), ccp(0, 40));
    popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
    popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 3000);
    popup->addButton(DyButtonType_Close, "", 0);
    
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerTitle::completedAnimationSequenceNamed(const char *name)
{
    if(strcmp(name, "start_login") == 0 || strcmp(name, "start_normal") == 0)
        net_IsLogin();
}


void DyLayerTitle::pause()
{ 
    setKeypadEnabled(false);
}


void DyLayerTitle::resume()
{
    if(m_isPromised)
    {
        m_actionManager->runAnimations("start_login");
        
        m_isPromised = false;
        
        YtLib::sharedLib()->playBGM(YtLibSndBGM_Intro);
    }
    
    setKeypadEnabled(true);
}


void DyLayerTitle::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerTitle::show()
{
}


DySceneTitle::DySceneTitle()
{
}


DySceneTitle::~DySceneTitle()
{
}


void DySceneTitle::didSceneChanged()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
   DyLayerTitle* layerTitle = (DyLayerTitle*)YtLayerManager::sharedLayerManager()->getTopLayer();
    
    layerTitle->m_actionManager->setDelegate(layerTitle);
    
    layerTitle->m_isPromised = false;
    
    //
    // 약관동의
    if(dataManager->m_local.m_isPromised == false)
    {
        DyLayerPromise* layerPromise = DyLayerPromise::create(this);
        layerPromise->m_layerTitle = layerTitle;
        
        YtLayerManager::sharedLayerManager()->pushLayer(layerPromise, true, true);
        
        layerTitle->m_actionManager->runAnimations("promise");
        
    }
    else
    {
        YtLib::sharedLib()->playBGM(YtLibSndBGM_Intro);
    }

}


void DySceneTitle::resumeByPhone()
{
    
}


void DySceneTitle::pauseByPhone()
{
    
}