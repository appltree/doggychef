/*
DyLayerPresentDelly.cpp
Doggy chef
@initialize at 121231

@history
121231	yoonsr	initialize
*/


#include "DyLayerPresentDelly.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyFriend.h"
#include "DyItem.h"
#include "DyLayerShop.h"
#include "DyPopup.h"
#include "DyLayerPresent.h"
#include "DyNetwork.h"

DyLayerPresentDelly::DyLayerPresentDelly()
{
    m_tag = DYLAYERPRESENTDELLY_TAG;
    for(int i = 0;i < 6;i++)
		m_menuDelly[i] = NULL;
}


DyLayerPresentDelly::~DyLayerPresentDelly()
{
}


void DyLayerPresentDelly::onEnter()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyLib* lib = DyLib::sharedLib();
    
	CCLayer::onEnter();

	if(m_isInit)
		return;
	m_isInit = true;
    
    CCLabelTTF* labelTo = (CCLabelTTF*)getChildByTag(PRESENTDELLY_UI_TAG_TO);
    if(dataManager->m_delly_event)
        sprintf(lib->m_strTemp, "\"%s\"님께 선물도 하고 1+1으로 델리도 받으세요~", m_friend->m_nickname.c_str());
    else
        sprintf(lib->m_strTemp, "\"%s\"님께 보낼 선물을 선택하세요.", m_friend->m_nickname.c_str());
    labelTo->setString(lib->m_strTemp);

    
    setKeypadEnabled(true);
}


void DyLayerPresentDelly::onExit()
{
	CCLayer::onExit();
}


void DyLayerPresentDelly::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
    DyLib* lib = DyLib::sharedLib();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();

    m_menu = (CCMenu*)getChildByTag(PRESENTDELLY_UI_TAG_MENU);
  
    
	DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    
	for(int i = 0;i < 6;i++)
	{
		DyDellyInfo* delly = (DyDellyInfo*)moneyManager->m_arrDellyInfos->objectAtIndex(i);
        
		CCLayer* layerGroup = (CCLayer*)getChildByTag(PRESENTDELLY_UI_TAG_GROUP_0 + 1000*i);
        
		CCLabelTTF* labelDellyCount = (CCLabelTTF*)layerGroup->getChildByTag(PRESENTDELLY_UI_TAG_SUB_DELLY_COUNT + PRESENTDELLY_UI_TAG_GROUP_0 + 1000*i);
		labelDellyCount->setColor(ccWHITE);
		sprintf(lib->m_strTemp, "%d", delly->m_delly_cnt);
		labelDellyCount->setString(lib->m_strTemp);
        
		CCLabelTTF* labelBonusCount = (CCLabelTTF*)layerGroup->getChildByTag(PRESENTDELLY_UI_TAG_SUB_BONUS_COUNT + PRESENTDELLY_UI_TAG_GROUP_0 + 1000*i);
		labelBonusCount->setColor(ccc3(255,143,12));
		sprintf(lib->m_strTemp, "Bonus\n+%d", delly->m_bonus);
		labelBonusCount->setString(lib->m_strTemp);
        
		m_menuDelly[i] = (CCMenu*)layerGroup->getChildByTag(PRESENTDELLY_UI_TAG_SUB_MENU + PRESENTDELLY_UI_TAG_GROUP_0 + 1000*i);
        
		CCSprite* sprWon = (CCSprite*)layerGroup->getChildByTag(PRESENTDELLY_UI_TAG_SUB_WON + PRESENTDELLY_UI_TAG_GROUP_0 + 1000*i);
		CCLabelTTF* labelPrice = (CCLabelTTF*)layerGroup->getChildByTag(PRESENTDELLY_UI_TAG_SUB_PRICE + PRESENTDELLY_UI_TAG_GROUP_0 + 1000*i);
		labelPrice->setColor(ccc3(101,58,39));
        
#ifdef ANDROID
		YtLib::setMoneyStyle(lib->m_strTemp, delly->m_price_won);
		labelPrice->setString(lib->m_strTemp);
#else
        sprWon->setDisplayFrame(CCSprite::createWithSpriteFrameName("menushop_ui_delly_dollar.png")->displayFrame());
        labelPrice->setString(delly->m_price_dollar.c_str());
#endif
        
		YtLib::arrangeNodesH(ccp(0, labelPrice->getPosition().y), ccp(0.5, 0.5), 5, sprWon, labelPrice, NULL);
		sprWon->setPosition(ccp(sprWon->getPosition().x, sprWon->getPosition().y -2));
        
        
        if(dataManager->m_delly_event)
        {
            CCSprite* sprHeartShadow = CCSprite::createWithSpriteFrameName("main_ui_gift_heart_shadow.png");
            layerGroup->addChild(sprHeartShadow);
            
            
            CCSprite* sprHeart = CCSprite::createWithSpriteFrameName("main_ui_gift_heart01.png");
            layerGroup->addChild(sprHeart);
            
            sprHeart->setPosition(ccp(100, 70));
            sprHeartShadow->setPosition(ccp(100, 58));
            
            labelBonusCount->setPosition(ccp(65, 80));
            
            lib->setupHeartAni(sprHeart, sprHeartShadow);
        }
        
        this->reorderChild(layerGroup, 10 - i);
        
	}


}


SEL_MenuHandler DyLayerPresentDelly::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName)
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCloseClicked", DyLayerPresentDelly::onCloseClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onBuyClicked", DyLayerPresentDelly::onBuyClicked);
    return NULL;    
}

SEL_CCControlHandler DyLayerPresentDelly::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}

bool DyLayerPresentDelly::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerPresentDelly::onCloseClicked(cocos2d::CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLOSE);
	YtLayerManager::sharedLayerManager()->popLayer();
}


void DyLayerPresentDelly::keyBackClicked()
{
    setKeypadEnabled(false);
    onCloseClicked(NULL);
}




void DyLayerPresentDelly::callfunc_update_me_success(float dt)
{
    DyLayerPresentDelly *layerPresent = (DyLayerPresentDelly*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPRESENTDELLY_TAG);
    
    unschedule(schedule_selector(DyLayerPresentDelly::callfunc_update_me_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->network_send_present(layerPresent->m_friend, DyGiftTypeDelly, layerPresent->m_process_delly->m_delly_cnt, httpresponse_selector(DyLayerPresentDelly::onHttpRequestCompleted));
}


void DyLayerPresentDelly::callfunc_update_me_fail(float dt)
{
    unschedule(schedule_selector(DyLayerPresentDelly::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* _popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerPresentDelly::onPopupClicked));
    _popup->setText(STRING_SENDPRESENT_FAILURE);
    _popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(_popup, true, true);
}


void DyLayerPresentDelly_callfunc_update_me(int value, int code)
{
    DyLayerPresentDelly *layerPresent = (DyLayerPresentDelly*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPRESENTDELLY_TAG);
    
    if(value == 0)
    {
        layerPresent->schedule(schedule_selector(DyLayerPresentDelly::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layerPresent->schedule(schedule_selector(DyLayerPresentDelly::callfunc_update_me_fail), 0.05f);
    }
}


void DyLayerPresentDelly::callfunc_iab_success(float dt)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyLib* lib = DyLib::sharedLib();
    
    unschedule(schedule_selector(DyLayerPresentDelly::callfunc_iab_success));
    
    if(dataManager->m_delly_event)
    {
        DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
        
        moneyManager->addDelly(m_process_delly->m_delly_cnt, true);
        
        lib->network_kakao_update_me(0, DyLayerPresentDelly_callfunc_update_me);
    }
    else
    {
        lib->network_send_present(m_friend, DyGiftTypeDelly, m_process_delly->m_delly_cnt, httpresponse_selector(DyLayerPresentDelly::onHttpRequestCompleted));
    }
}


void DyLayerPresentDelly::callfunc_iab_fail(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    this->unschedule(schedule_selector(DyLayerPresentDelly::callfunc_iab_fail));
    
    lib->show_network_alaram(false);
    
    //lib->data_restore();
    
    DyPopup* _popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerPresentDelly::onPopupClicked));
    _popup->setText(STRING_SENDPRESENT_FAILURE);
    _popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(_popup, true, true);
}



void DyLayerPresentDelly_callfunc_iab(int value, int code)
{
    DyLayerPresentDelly *layerPresent = (DyLayerPresentDelly*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPRESENTDELLY_TAG);

    
    if(value == 0)
    {
        layerPresent->schedule(schedule_selector(DyLayerPresentDelly::callfunc_iab_success), 0.05f);
    }
    else
    {
        layerPresent->schedule(schedule_selector(DyLayerPresentDelly::callfunc_iab_fail), 0.05f);
    }
}



void DyLayerPresentDelly::onPopupClicked(cocos2d::CCObject * pSender)
{
//    DyLib* lib = DyLib::sharedLib();
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    
//	if(pPopup->m_active_tag == 1)
//	{
//        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
//        
//        lib->network_iab(DyLayerPresentDelly_callfunc_iab, m_process_delly->m_product_id.c_str());
//	}
//    else
    if(pPopup->m_active_tag == 100)
    {
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        YtLayerManager::sharedLayerManager()->popLayer();
    }
    else
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}



void DyLayerPresentDelly::onBuyClicked(cocos2d::CCObject * pSender)
{
    DyLib* lib = DyLib::sharedLib();
    
    CCMenuItemImage *pMenu = dynamic_cast<CCMenuItemImage*>(pSender);
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    int tag = pMenu->getTag() - PRESENTDELLY_UI_TAG_SUB_MENUITEM_BUY;
	tag = tag/1000 - 1;
    
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
	m_process_delly = (DyDellyInfo*)moneyManager->m_arrDellyInfos->objectAtIndex(tag);
    
//    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerPresentDelly::onPopupClicked));
//    
//#ifdef ANDROID
//    YtLib::setMoneyStyle(lib->m_strTemp3, m_process_delly->m_price_won);
//    sprintf(lib->m_strTemp, "%d델리를 %s님에게 카카오톡으로 선물하시겠어요? 현금 %s원이 결제됩니다.", m_process_delly->m_delly_cnt, m_friend->m_nickname.c_str(), lib->m_strTemp3);
//#else
//    sprintf(lib->m_strTemp, "%d델리를 %s님에게 카카오톡으로 선물하시겠어요? 현금 $%s이 결제됩니다.", m_process_delly->m_delly_cnt, m_friend->m_nickname.c_str(), m_process_delly->m_price_dollar.c_str());
//#endif
//    popup->setText(lib->m_strTemp);
//    
//    popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
//    popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 1);
//    popup->addButton(DyButtonType_Close, "", 0);
//    
//    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
    lib->network_iab(DyLayerPresentDelly_callfunc_iab, m_process_delly->m_present_id.c_str());
}


void DyLayerPresentDelly::pause()
{
    setKeypadEnabled(false);
	m_menu->setEnabled(false);
    
    for(int i = 0;i < 6;i++)
		if(m_menuDelly[i])
			m_menuDelly[i]->setEnabled(false);

}


void DyLayerPresentDelly::resume()
{
    setKeypadEnabled(true);
	m_menu->setEnabled(true);
    
    for(int i = 0;i < 6;i++)
		if(m_menuDelly[i])
			m_menuDelly[i]->setEnabled(true);
    setKeypadEnabled(true);
}


void DyLayerPresentDelly::destory()
{
	//DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
	//moneyManager->removeGumLayer(getChildByTag(SHOP_UI_TAG_GUM_BG));
	YtLayer::destory();
}


void DyLayerPresentDelly::hide()
{
	YtLib* lib = YtLib::sharedLib();

	runAction(CCSequence::create(

		CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)), 
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2)),

		CCCallFunc::create(this, callfunc_selector(DyLayerPresentDelly::destory)),
		NULL));
}


void DyLayerPresentDelly::show()
{
	YtLib* lib = YtLib::sharedLib();

	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));

	runAction(CCSequence::create(

		CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
		NULL));
}


void DyLayerPresentDelly_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerPresentDelly *layerPresent = (DyLayerPresentDelly*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPRESENTDELLY_TAG);
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    switch(cmdType)
    {
        case DyNetworkCmdType_SendMessage:
        if(value_int == 0)
        {
            DyPopup* popup = DyPopup::create(DyPopupType_Text, layerPresent, callfuncO_selector(DyLayerPresentDelly::onPopupClicked));
            
            if(dataManager->m_delly_event)
            {
                sprintf(lib->m_strTemp, "%s님에게 선물하기 성공하였습니다.(%d델리도 받으셨습니다.)", layerPresent->m_friend->m_nickname.c_str(), layerPresent->m_process_delly->m_delly_cnt);
            }
            else
                sprintf(lib->m_strTemp, "%s%s", layerPresent->m_friend->m_nickname.c_str(), STRING_SENDPRESENT_SUCCESS);
            
            popup->setText(lib->m_strTemp);
            popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 100);
            YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        }
            break;
        default:
            break;
    }
}

//
//TODO : 에러일 경우를 체크해야함
void DyLayerPresentDelly::onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response)
{
    DyLib* lib = DyLib::sharedLib();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyLayerPresentDelly *layerPresent = (DyLayerPresentDelly*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPRESENTDELLY_TAG);

    if(lib->network_checkError(response, false) == false)
        return;
    
    if(lib->network_getTag(response).compare(NETWORK_SERVICE_SEND_PRESENT) == 0)
    {
        if(lib->network_send_present_ret(sender, response))
        {
            std::string game_message = dataManager->m_nickname;
            
            sprintf(lib->m_strTemp, "님이 %d델리를 보냈어요.", layerPresent->m_process_delly->m_delly_cnt);
            game_message += lib->m_strTemp;
            
            DYNETWORK_ADD_PROC(DyNetworkCmdType_SendMessage, game_message, layerPresent->m_friend, DyLayerPresentDelly_NetCallFunc);
            
//            DyNetworkCommand* commandNetwork = new DyNetworkCommand(DyNetworkCmdType_SendMessage, game_message, layerPresent->m_friend, DyLayerPresentDelly_NetCallFunc);
//            DyNetworkManager::sharedNetworkManager()->addLast(commandNetwork);
//            commandNetwork->release();
            //YtPlatform_NetKakao_SendMessage(DyLayerPresentDelly_callfunc_send_present, layerPresent->m_friend->m_user_id.c_str(), game_message.c_str(), "");
        }
        else
        {
            lib->show_network_alaram(false);
            
            DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
            popup->setText(STRING_SENDPRESENT_FAILURE);
            popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
            YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        }
    }    
}





DyLayerPresentDelly* DyLayerPresentDelly::create(CCNode* owner, DyFriend* _friend)
{
	DyLayerPresentDelly* layerPresentDelly = (DyLayerPresentDelly*)YtLayerManager::create("DyLayerPresentDelly", DyLayerPresentDellyLoader::loader(), "layer_present_delly.ccbi", owner);
    
    layerPresentDelly->m_friend = _friend;
    
	return layerPresentDelly;
}