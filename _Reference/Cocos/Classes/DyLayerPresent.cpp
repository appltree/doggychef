/*
DyLayerPresent.h
Doggy chef
@initialize at 130731

@history
130731	yoonsr	initialize
*/


#include "DyLayerPresent.h"
#include "DyLayerPresentDelly.h"
#include "DyLib.h"
#include "DyPopup.h"
#include "DyNetwork.h"

DyLayerPresent::DyLayerPresent()
{
    m_tag = DYLAYERPRESENT_TAG;
    m_sendGum = false;
}


DyLayerPresent::~DyLayerPresent()
{
}

void DyLayerPresent::onEnter()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyLib* lib = DyLib::sharedLib();
    
	CCLayer::onEnter();
    if(m_isInit)
		return;
	m_isInit = true;
    
    if(m_friend->m_sprPicture)
    {
        CCSprite* sprPictureAngle = (CCSprite*)this->getChildByTag(PRESENT_UI_TAG_PICTURE);
        CCSprite* sprPicture = CCSprite::createWithSpriteFrame(m_friend->m_sprPicture->displayFrame());
        sprPicture->setScale(48.f/sprPicture->getContentSize().width);
        
        sprPictureAngle->addChild(sprPicture);
        sprPicture->setPosition(ccp(sprPictureAngle->getContentSize().width/2, sprPictureAngle->getContentSize().height/2));
    }
    
    CCLabelTTF* labelName = (CCLabelTTF*)getChildByTag(PRESENT_UI_TAG_NAME);
    sprintf(lib->m_strTemp, "\"%s\"님께", m_friend->m_nickname.c_str());
    labelName->setString(lib->m_strTemp);
    
    m_friend->m_time_last_message_sent_at.time_to_str(lib->m_strTemp, 512);
    
    chagneStatus(DyPresentStatus_Present);
    
    schedule(schedule_selector(DyLayerPresent::updateRemainTime), 1.0f);
    
    setKeypadEnabled(true);
    
    
    if(dataManager->m_gum_send_award > 0)
    {
        CCLabelTTF* labelMessage = (CCLabelTTF*)getChildByTag(PRESENT_UI_TAG_MESSAGE);
        labelMessage->setString("껌을 보내면 껌 한 개 받을 수 있어요.");
        
        
    }
    
    if(dataManager->m_delly_event > 0)
    {
        getChildByTag(PRESENT_UI_TAG_DELLY_DECO_SHADOW)->setVisible(true);
        getChildByTag(PRESENT_UI_TAG_DELLY_DECO_HEART)->setVisible(true);
        
        lib->setupHeartAni((CCSprite*)getChildByTag(PRESENT_UI_TAG_DELLY_DECO_HEART), (CCSprite*)getChildByTag(PRESENT_UI_TAG_DELLY_DECO_SHADOW));
    }
}


void DyLayerPresent::updateRemainTime(float dt)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyLib* lib = DyLib::sharedLib();
    
    if(m_friend->m_time_last_message_sent_at.time() > 0)
    {
        CCDate dataCurrent = dataManager->getCurrentDate();
        CCDate dataDiff = dataCurrent - m_friend->m_time_last_message_sent_at;
        
        if(m_status == DyPresentStatus_Present && dataManager->m_message_interval > dataDiff.time())
        {
            ((CCMenuItem*)m_menu->getChildByTag(PRESENT_UI_TAG_MENUITEM_GUM))->setEnabled(false);
            ((CCMenuItem*)m_menu->getChildByTag(PRESENT_UI_TAG_MENUITEM_GAEST))->setEnabled(false);
            //((CCMenuItem*)m_menu->getChildByTag(PRESENT_UI_TAG_MENUITEM_DELLY))->setEnabled(false);
            
            CCDate dataInterval(dataManager->m_message_interval);
            
            CCDate dataDiff2 = dataInterval - dataDiff;
            
//            int hour = dataDiff2.time()/(60*60);// dataDiff2.hour();
//            int min = (dataDiff2.time()-hour*60*60)/(60) ;//dataDiff2.min();
//            int sec = dataDiff2.time()-hour*60*60-min*60;  //dataDiff2.sec();
            
            int hour = dataDiff2.getHour();
            int min = dataDiff2.getMin();
            int sec = dataDiff2.getSec();


            if(hour > 0)//dataDiff2.time() > 60*60)
                sprintf(lib->m_strTemp, "%02d시간 %02d분 %02d초 남음", hour, min, sec);
            else
                sprintf(lib->m_strTemp, "%02d분 %02d초 남음", min, sec);
            
            m_labelRemain->setString(lib->m_strTemp);
            m_labelRemain->setVisible(true);
            return;
        }
    }
    
    
    ((CCMenuItem*)m_menu->getChildByTag(PRESENT_UI_TAG_MENUITEM_GUM))->setEnabled(true);
    ((CCMenuItem*)m_menu->getChildByTag(PRESENT_UI_TAG_MENUITEM_GAEST))->setEnabled(true);
    
    ((CCMenuItem*)m_menu->getChildByTag(PRESENT_UI_TAG_MENUITEM_DELLY))->setEnabled(true);
    
    m_labelRemain->setVisible(false);
}


void DyLayerPresent::onExit()
{
	CCLayer::onExit();
}


void DyLayerPresent::onCloseClicked(cocos2d::CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLOSE);
	YtLayerManager::sharedLayerManager()->popLayer();
}



void DyLayerPresent::keyBackClicked()
{
    setKeypadEnabled(false);
    onCloseClicked(NULL);
}



void DyLayerPresent::onPresentClicked(CCObject * obj)
{
    if(m_status == DyPresentStatus_Present)
        return;

    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    chagneStatus(DyPresentStatus_Present);
}

void DyLayerPresent::onAskClicked(CCObject * obj)
{
    if(m_status == DyPresentStatus_Ask)
        return;

    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    chagneStatus(DyPresentStatus_Ask);
}





void DyLayerPresent::callfunc_send_present_success(float dt)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    this->unschedule(schedule_selector(DyLayerPresent::callfunc_send_present_success));
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    m_friend->m_time_last_message_sent_at = dataManager->getCurrentDate();
    updateRemainTime(0);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
    sprintf(lib->m_strTemp, "%s%s", m_friend->m_nickname.c_str(), STRING_SENDPRESENT_SUCCESS);
    popup->setText(lib->m_strTemp);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}



void DyLayerPresent::callfunc_send_present_fail(float dt)
{
    this->unschedule(schedule_selector(DyLayerPresent::callfunc_send_present_fail));
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
    
    popup->setText(lib->m_net_error_msg.c_str());
    
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerPresent_callfunc_send_present(void* value)
{
    DyLib* lib = DyLib::sharedLib();
    const char* messageRet = (const char*)value;
    
    DyLayerPresent *layerPresent = (DyLayerPresent*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPRESENT_TAG);
    
    if(strlen(messageRet) == 0)
    {
        layerPresent->schedule(schedule_selector(DyLayerPresent::callfunc_send_present_success), 0.05f);
    }
    else
    {
        lib->m_net_error_msg = (char*)value;
        
        layerPresent->schedule(schedule_selector(DyLayerPresent::callfunc_send_present_fail), 0.05f);
    }
}




void DyLayerPresent::callfunc_update_me_success(float dt)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyLayerPresent *layerPresent = (DyLayerPresent*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPRESENT_TAG);

    unschedule(schedule_selector(DyLayerPresent::callfunc_update_me_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
    sprintf(lib->m_strTemp, "%s님에게 선물하기 성공했습니다. 껌 %d개도 받았어요.", layerPresent->m_friend->m_nickname.c_str(), dataManager->m_gum_send_award);
    popup->setText(lib->m_strTemp);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerPresent::callfunc_update_me_fail(float dt)
{
    unschedule(schedule_selector(DyLayerPresent::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerPresent::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerPresent_callfunc_update_me(int value, int code)
{
    DyLayerPresent *layerPresent = (DyLayerPresent*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPRESENT_TAG);
    
    if(value == 0)
    {
        layerPresent->schedule(schedule_selector(DyLayerPresent::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layerPresent->schedule(schedule_selector(DyLayerPresent::callfunc_update_me_fail), 0.05f);
    }
}



void DyLayerPresent_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyLayerPresent *layerPresent = (DyLayerPresent*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPRESENT_TAG);
    DyLib* lib = DyLib::sharedLib();
    
    switch(cmdType)
    {
        case DyNetworkCmdType_SendMessage:
        if(value_int == 0)
        {
            DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
            popup->setText(STRING_SENDMESSAGE_SUCCESS);
            popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
            YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        }
            break;
        case DyNetworkCmdType_SendPresent:
        if(value_int == 0)
        {
            layerPresent->m_friend->m_time_last_message_sent_at = dataManager->getCurrentDate();
            layerPresent->updateRemainTime(0);
            
            
            if(dataManager->m_gum_send_award > 0)
            {
                DyMoneyManager::sharedMoneyManager()->addGum(dataManager->m_gum_send_award, true);
                
                lib->network_kakao_update_me(dataManager->m_gum_send_award, DyLayerPresent_callfunc_update_me);
            }
            else
            {
                DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
                sprintf(lib->m_strTemp, "%s%s", layerPresent->m_friend->m_nickname.c_str(), STRING_SENDPRESENT_SUCCESS);
                popup->setText(lib->m_strTemp);
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
            }
        }
            break;
        default:
            break;
    }
}


void DyLayerPresent::askPresent(DyGiftType gifttype)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    std::string message = dataManager->m_nickname + "님이 ";
    
    switch(gifttype)
    {
        case DyGiftTypeDelly:
            message += "델리를 조르셨어요.";
            break;
        case DyGiftTypeGaest:
            message += "개스트를 조르셨어요.";
            break;
        case DyGiftTypeGum:
            message += "껌을 조르셨어요.";
            break;
        default:
            break;
    }
    
    //YtPlatform_NetKakao_SendMessage(DyLayerPresent_CallFunc_SendMessageAsk, m_friend->m_user_id.c_str(), message.c_str(), "");
    
    DYNETWORK_ADD_PROC(DyNetworkCmdType_SendMessage, message, m_friend, DyLayerPresent_NetCallFunc);
//    DyNetworkCommand* commandNetwork = new DyNetworkCommand(DyNetworkCmdType_SendMessage, message, m_friend, DyLayerPresent_NetCallFunc);
//    DyNetworkManager::sharedNetworkManager()->addLast(commandNetwork);
//    commandNetwork->release();
}


void DyLayerPresent::onPopupClicked(cocos2d::CCObject * pSender)
{
    //DyLib* lib = DyLib::sharedLib();
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    
	if(pPopup->m_active_tag == 11)
	{
        askPresent(DyGiftTypeGum);
	}
	else if(pPopup->m_active_tag == 12)
	{
        askPresent(DyGiftTypeGaest);
	}
	else if(pPopup->m_active_tag == 13)
	{
        askPresent(DyGiftTypeDelly);
	}
    
  	else if(pPopup->m_active_tag == 21)
	{
        DYNETWORK_ADD_PROC(DyNetworkCmdType_SendPresent, "gum", m_friend, DyLayerPresent_NetCallFunc);
//        DyNetworkCommand* commandNetwork = new DyNetworkCommand(DyNetworkCmdType_SendPresent, "gum", m_friend, DyLayerPresent_NetCallFunc);
//        DyNetworkManager::sharedNetworkManager()->addLast(commandNetwork);
//        commandNetwork->release();
        m_sendGum = true;
 	}
    else if(pPopup->m_active_tag == 22)
	{
        DYNETWORK_ADD_PROC(DyNetworkCmdType_SendPresent, "gaest", m_friend, DyLayerPresent_NetCallFunc);
//        DyNetworkCommand* commandNetwork = new DyNetworkCommand(DyNetworkCmdType_SendPresent, "gaest", m_friend, DyLayerPresent_NetCallFunc);
//        DyNetworkManager::sharedNetworkManager()->addLast(commandNetwork);
//        commandNetwork->release();
        m_sendGum = false;
 	}
    
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}



void DyLayerPresent::onGumClicked(CCObject * obj)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    if(m_status == DyPresentStatus_Present)
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerPresent::onPopupClicked), CCSizeMake(440, 350));
        
        sprintf(lib->m_strTemp, "%s님에게 껌을 선물하시겠어요? \\ (카카오톡으로 메시지가 전달됩니다.)", m_friend->m_nickname.c_str());
        popup->setText(lib->m_strTemp);
        
        popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
        popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 21);
        popup->addButton(DyButtonType_Close, "", 0);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
    else
    {   
        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerPresent::onPopupClicked));
        
        sprintf(lib->m_strTemp, "%s님에게 껌을 카카오톡으로 조르시겠어요?", m_friend->m_nickname.c_str());
        popup->setText(lib->m_strTemp);
        
        popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
        popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 11);
        popup->addButton(DyButtonType_Close, "", 0);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
}


void DyLayerPresent::onGaestClicked(CCObject * obj)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    if(m_status == DyPresentStatus_Present)
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerPresent::onPopupClicked), CCSizeMake(440, 350));
        
        sprintf(lib->m_strTemp, "%s님에게 개스트를 보내시겠어요? \\ (카카오톡으로 메시지가 전달됩니다.)", m_friend->m_nickname.c_str());
        popup->setText(lib->m_strTemp);
        
        popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
        popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 22);
        popup->addButton(DyButtonType_Close, "", 0);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
    else
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerPresent::onPopupClicked));
        
        sprintf(lib->m_strTemp, "%s님에게 개스트를 카카오톡으로 조르시겠어요?", m_friend->m_nickname.c_str());
        popup->setText(lib->m_strTemp);
        
        popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
        popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 12);
        popup->addButton(DyButtonType_Close, "", 0);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
}


void DyLayerPresent::onDellyClicked(CCObject * obj)
{
    DyLib* lib = DyLib::sharedLib();

    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    

    if(m_status == DyPresentStatus_Present)
    {
        if(lib->m_main_server_error)
        {
            DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
            popup->setText("일시적으로 델리 보내기를 사용하실 수 없습니다. 게임을 다시 시작해 주세요.");
            popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
            YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        }
        else
        {
            DyLayerPresentDelly* layerPresentDelly = DyLayerPresentDelly::create(this, m_friend);
            YtLayerManager::sharedLayerManager()->pushLayer(layerPresentDelly, true, true);
        }
        //
    }
    else
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerPresent::onPopupClicked));
        
        sprintf(lib->m_strTemp, "%s님에게 델리를 조르시겠어요?", m_friend->m_nickname.c_str());
        popup->setText(lib->m_strTemp);
        
        popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
        popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 13);
        popup->addButton(DyButtonType_Close, "", 0);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
}


void DyLayerPresent::pause()
{
    m_menu->setEnabled(false);
    setKeypadEnabled(false);
}


void DyLayerPresent::resume()
{
    m_menu->setEnabled(true);
    setKeypadEnabled(true);
}

void DyLayerPresent::hide()
{
    YtLib* lib = YtLib::sharedLib();
    
	runAction(CCSequence::create(
                                 
                                 CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)),
                                 CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
                                 CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2)),
                                 
                                 CCCallFunc::create(this, callfunc_selector(DyLayerPresent::destory)),
                                 NULL));

    
    
}


void DyLayerPresent::show()
{
	YtLib* lib = YtLib::sharedLib();
    
	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));
    
	runAction(CCSequence::create(
                                 
                                 CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
                                 CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
                                 CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
                                 NULL));
}


void DyLayerPresent::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader)
{
    m_menu = (CCMenu*)getChildByTag(PRESENT_UI_TAG_MENU);
    
    m_labelRemain = (CCLabelTTF*)getChildByTag(PRESENT_UI_TAG_TIME_REMAIN);
}





SEL_MenuHandler DyLayerPresent::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName)
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCloseClicked", DyLayerPresent::onCloseClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPresentClicked", DyLayerPresent::onPresentClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onAskClicked", DyLayerPresent::onAskClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onGumClicked", DyLayerPresent::onGumClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onGaestClicked", DyLayerPresent::onGaestClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onDellyClicked", DyLayerPresent::onDellyClicked);
    
    
    return NULL;
}

SEL_CCControlHandler DyLayerPresent::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName)
{
	return NULL;
}

bool DyLayerPresent::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode)
{
    return false;
}


void DyLayerPresent::chagneStatus(DyPresentStatus status)
{
    m_status = status;
    
    CCMenuItemImage* menuitemPresent = (CCMenuItemImage*)m_menu->getChildByTag(PRESENT_UI_TAG_MENUITEM_PRESENT);
    CCMenuItemImage* menuitemAsk = (CCMenuItemImage*)m_menu->getChildByTag(PRESENT_UI_TAG_MENUITEM_ASK);
    
    switch(status)
    {
        case DyPresentStatus_Present:
            menuitemPresent->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("main_ui_button_present_on.png")->displayFrame());
            menuitemAsk->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("main_ui_button_ask_off.png")->displayFrame());
            getChildByTag(PRESENT_UI_TAG_TIME_REMAIN)->setVisible(true);
            
            break;
        case DyPresentStatus_Ask:
            menuitemPresent->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("main_ui_button_present_off.png")->displayFrame());
            menuitemAsk->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("main_ui_button_ask_on.png")->displayFrame());
            
            getChildByTag(PRESENT_UI_TAG_TIME_REMAIN)->setVisible(false);
            
            break;
    }
    
    updateRemainTime(0);
}



DyLayerPresent* DyLayerPresent::create(CCNode* owner, DyFriend* _friend)
{
	DyLayerPresent* layerPresent = (DyLayerPresent*)YtLayerManager::create("DyLayerPresent", DyLayerPresentLoader::loader(), "layer_present.ccbi", owner);
    layerPresent->m_friend = _friend;
    
#ifndef ANDROID
    layerPresent->m_actionManager->runAnimations("iphone");
#endif
    
	return layerPresent;
}