/*
DyLayerOption.cpp
Doggy chef
@initialize at 121021

@history
121021	yoonsr	initialize
*/

#include "DyConfig.h"
#include "DyLayerOption.h"
#include "DyLib.h"
#include "YtSpriteEx.h"
#include "DyData.h"
#include "DyTycoon.h"
#include "DyLayerCredits.h"
#include "DySceneStory.h"
#include "DyPopup.h"
#include "DyNetwork.h"


DyLayerOption::DyLayerOption()
{
    m_tag = DYLAYEROPTION_TAG;
}

DyLayerOption::~DyLayerOption()
{
}


void DyLayerOption::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
    DyLib* lib = DyLib::sharedLib();
    
	DyDataManager* dataManager = DyDataManager::sharedDataManager();

	m_menu = (CCMenu*)getChildByTag(OPTION_UI_TAG_MENU);
    m_menuBlog = (CCMenu*)getChildByTag(OPTION_UI_TAG_MENU_BLOG);
    m_menuBoard = (CCMenu*)getChildByTag(OPTION_UI_TAG_MENU_BOARD);
    
    
	for(int i = 0;i < 3;i++)
	{
//		((CCLabelTTF*)getChildByTag(OPTION_UI_TAG_BGM_TEXT+10*i))->setColor(ccc3(255,221,35));
//		((CCLabelTTF*)getChildByTag(OPTION_UI_TAG_BGM_TEXT+10*i))->setFontName(FONT_NAME_DEFAULT);
//
//		((CCLabelTTF*)getChildByTag(OPTION_UI_TAG_BGM_TEXT_SHADOW+10*i))->setColor(ccBLACK);
//		((CCLabelTTF*)getChildByTag(OPTION_UI_TAG_BGM_TEXT_SHADOW+10*i))->setFontName(FONT_NAME_DEFAULT);

		CCSprite* sprChecker = (CCSprite*)getChildByTag(OPTION_UI_TAG_BGM_CHECKER+10*i);
		
		switch(i)
		{
			case 0:
				sprChecker->setVisible(YtOption_get_bgm(dataManager->m_local.m_option));
				break;
			case 1:
				sprChecker->setVisible(YtOption_get_effect(dataManager->m_local.m_option));
				break;
			case 2:
				sprChecker->setVisible(YtOption_get_push(dataManager->m_local.m_option));
				break;
		}

		CCSprite* sprButton = YtBlankSprite::spriteWithSizeAndColor(CCSizeMake(70, 55), ccRED);
		sprButton->setOpacity(0);
		CCSprite* sprButtonSel = YtBlankSprite::spriteWithSizeAndColor(CCSizeMake(70, 55), ccRED);
		sprButtonSel->setOpacity(0);

		CCMenuItemImage* menuitem = CCMenuItemImage::create();
		menuitem->initWithNormalSprite(
			sprButton,
			sprButtonSel, 
			NULL, this, menu_selector(DyLayerOption::onOptionClicked));

		menuitem->setTag(i);
		menuitem->setPosition(ccp(sprChecker->getPosition().x-5, sprChecker->getPosition().y-10));

		m_menu->addChild(menuitem);
	}

    CCLabelTTF* labelMyVer = (CCLabelTTF*)getChildByTag(OPTION_UI_TAG_MY_VER_TEXT);
    
    int ver_high = CONFIG_VER/1000;
    int ver_mid = (CONFIG_VER - ver_high*1000)/100;
    int ver_low = CONFIG_VER%100;
    
    sprintf(lib->m_strTemp, STRING_VER, ver_high, ver_mid, ver_low);
    labelMyVer->setString(lib->m_strTemp);
    
    
    CCLabelTTF* labelAppID = (CCLabelTTF*)getChildByTag(OPTION_UI_TAG_APP_ID);
    sprintf(lib->m_strTemp, "%s", dataManager->m_user_id.c_str());
    labelAppID->setString(lib->m_strTemp);
    
    setKeypadEnabled(true);
    
    m_menuBlog->setPosition(ccp(230, 0));
    
    m_menuBoard->setPosition(ccp(-230, 0));
    
    
    m_menu->getChildByTag(OPTION_UI_TAG_MENU_ITEM_RESET)->setVisible(false);
    getChildByTag(OPTION_UI_TAG_RESET_TEXT)->setVisible(false);
}


SEL_MenuHandler DyLayerOption::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCloseClicked", DyLayerOption::onCloseClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onLeaveClicked", DyLayerOption::onLeaveClicked);

	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onSendmailClicked", DyLayerOption::onSendmailClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onLogoutClicked", DyLayerOption::onLogoutClicked);

	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onStoryClicked", DyLayerOption::onStoryClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCreditsClicked", DyLayerOption::onCreditsClicked);
    
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCopyIDClicked", DyLayerOption::onCopyIDClicked);
    
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onBlogClicked", DyLayerOption::onBlogClicked);
    
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onBoardClicked", DyLayerOption::onBoardClicked);
    //CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onResetClicked", DyLayerOption::onResetClicked);
    return NULL;
}

SEL_CCControlHandler DyLayerOption::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}

bool DyLayerOption::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerOption::onCopyIDClicked(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    YtPlatform_CopyToClipBoard(DyDataManager::sharedDataManager()->m_user_id.c_str());
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
    popup->setText(STRING_APPID_COPY_RESULT);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);

}


void DyLayerOption::onCloseClicked(cocos2d::CCObject * pSender)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLOSE);

	if(!m_callByMainMenu)
	{
		YtLib::sharedLib()->pauseBGM();
	}

	YtLayerManager::sharedLayerManager()->popLayer();
}


void DyLayerOption::keyBackClicked()
{
    if(DyLib::sharedLib()->m_showLoadingPopup)
        return;

    setKeypadEnabled(false);
    onCloseClicked(NULL);
}



void DyLayerOption::callfunc_push_onoff_success(float dt)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    this->unschedule(schedule_selector(DyLayerOption::callfunc_push_onoff_success));
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    CCSprite* sprChecker = (CCSprite*)getChildByTag(OPTION_UI_TAG_BGM_CHECKER+10*2);
	
    //lib->m_option = dataManager->m_local.m_option;
    
    
    sprChecker->setVisible(YtOption_get_push(dataManager->m_local.m_option));

}



void DyLayerOption::callfunc_push_onoff_fail(float dt)
{
    this->unschedule(schedule_selector(DyLayerOption::callfunc_push_onoff_fail));
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
    popup->setText(STRING_SENDPRESENT_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerOption_callfunc_push_onoff(int value, int code)
{
    DyLayerOption *layerOption = (DyLayerOption*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYEROPTION_TAG);
    
    if(value == 0)
    {
        layerOption->schedule(schedule_selector(DyLayerOption::callfunc_push_onoff_success), 0.05f);
    }
    else
    {
        layerOption->schedule(schedule_selector(DyLayerOption::callfunc_push_onoff_fail), 0.05f);
    }
}


void DyLayerOption::onOptionClicked(CCObject * obj)
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	DyLib* lib = DyLib::sharedLib();
	CCMenuItemImage * pMenu = dynamic_cast<CCMenuItemImage*>(obj);

	int index = pMenu->getTag();
	
	CCSprite* sprChecker = (CCSprite*)getChildByTag(OPTION_UI_TAG_BGM_CHECKER+10*index);
		
	switch(index)
	{
		case 0:
			YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

            YtOption_set_bgm(dataManager->m_local.m_option, !YtOption_get_bgm(dataManager->m_local.m_option));
			//lib->m_option = dataManager->m_local.m_option;

			sprChecker->setVisible(YtOption_get_bgm(dataManager->m_local.m_option));

			if(YtOption_get_bgm(dataManager->m_local.m_option))
			{
				if(m_callByMainMenu)
				{
					YtLib::sharedLib()->playBGM(YtLibSndBGM_Title);
				}
				else
				{
                    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
                    
					YtLib::sharedLib()->playBGM(tycoonManager->m_bgmIdx);
                    YtLib::sharedLib()->stopBGM();// reset bgm
				}
			}
			else
			{
				YtLib::sharedLib()->stopBGM();
			}

			break;
		case 1:
            
			YtOption_set_effect(dataManager->m_local.m_option, !YtOption_get_effect(dataManager->m_local.m_option));
			//lib->m_option = dataManager->m_local.m_option;
			sprChecker->setVisible(YtOption_get_effect(dataManager->m_local.m_option));
			YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
			break;
		case 2:
			YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

            YtOption_set_push(dataManager->m_local.m_option, !YtOption_get_push(dataManager->m_local.m_option));

//            if(DyDataManager::sharedDataManager()->m_isGuestLogin)
//                callfunc_push_onoff_success(0);
//            else
            {
                lib->network_push_onoff(DyLayerOption_callfunc_push_onoff, YtOption_get_push(dataManager->m_local.m_option) ? 1:0);
            }
			break;
//		case 3:
//			YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
//			dataManager->m_local.m_option.m_vibration = !dataManager->m_local.m_option.m_vibration;
//			lib->m_option = dataManager->m_local.m_option;
//			sprChecker->setVisible(dataManager->m_local.m_option.m_vibration);
//			break;
	}

	dataManager->dataSave();
}



void DyLayerOption::callfunc_logout_success(float dt)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    unschedule(schedule_selector(DyLayerOption::callfunc_logout_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    dataManager->reset();
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerOption::onPopupClicked));
    popup->setText(STRING_LOGOUT_RESULT);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 2000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}


void DyLayerOption::callfunc_logout_fail(float dt)
{
    unschedule(schedule_selector(DyLayerOption::callfunc_logout_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerOption::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}


void DyLayerOption_callfunc_logout(int value, int code)
{
    DyLayerOption *layerOption = (DyLayerOption*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYEROPTION_TAG);
    
    if(value == 0)
    {
        layerOption->schedule(schedule_selector(DyLayerOption::callfunc_logout_success), 0.05f);
    }
    else
    {
        layerOption->schedule(schedule_selector(DyLayerOption::callfunc_logout_fail), 0.05f);
    }
}


void DyLayerOption::callfunc_unregist_success(float dt)
{
    unschedule(schedule_selector(DyLayerOption::callfunc_unregist_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    
    lib->network_unregist(httpresponse_selector(DyLayerOption::onHttpRequestCompleted));
}


void DyLayerOption::callfunc_unregist_fail(float dt)
{
    unschedule(schedule_selector(DyLayerOption::callfunc_unregist_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerOption::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}


void DyLayerOption_callfunc_upregist(int value, int code)
{
    DyLayerOption *layerOption = (DyLayerOption*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYEROPTION_TAG);
    
    if(value == 0)
    {
        layerOption->schedule(schedule_selector(DyLayerOption::callfunc_unregist_success), 0.05f);
    }
    else
    {
        layerOption->schedule(schedule_selector(DyLayerOption::callfunc_unregist_fail), 0.05f);
    }
}



void DyLayerOption::callfunc_delete_me_success(float dt)
{
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyLayerOption *layerOption = (DyLayerOption*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYEROPTION_TAG);
    
    layerOption->unschedule(schedule_selector(DyLayerOption::callfunc_delete_me_success));
    
    YtPlatform_NetKakao_Unregist(DyLayerOption_callfunc_upregist);
}



void DyLayerOption::callfunc_delete_me_fail(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerOption *layerOption = (DyLayerOption*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYEROPTION_TAG);
    
    layerOption->unschedule(schedule_selector(DyLayerOption::callfunc_delete_me_fail));
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLib::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}



void DyLayerOption_callfunc_delete_me(int value, int code)
{
    DyLayerOption *layerOption = (DyLayerOption*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYEROPTION_TAG);
    
    if(value == 0)
    {
        layerOption->schedule(schedule_selector(DyLayerOption::callfunc_delete_me_success), 0.05f);
    }
    else
    {
        layerOption->schedule(schedule_selector(DyLayerOption::callfunc_delete_me_fail), 0.05f);
    }
}





//void DyLayerOption::callfunc_update_me_success(float dt)
//{
//    unschedule(schedule_selector(DyLayerOption::callfunc_update_me_success));
//    
////    DyLib* lib = DyLib::sharedLib();
//    
//    //lib->show_network_alaram(false);
//    
//    //YtLayerManager::sharedLayerManager()->popLayer();
//    
////    DyPopup* popup = lib->show_network_alaram(true);
////    popup->setText(STRING_TITLE_UPDATE_DATA);
//    
//    
//    YtPlatform_NetKakao_DeleteMe(DyLayerOption_callfunc_delete_me);
//    
//    
//}
//
//
//void DyLayerOption::callfunc_update_me_fail(float dt)
//{
//    unschedule(schedule_selector(DyLayerOption::callfunc_update_me_fail));
//    
//    DyLib* lib = DyLib::sharedLib();
//    
//    lib->show_network_alaram(false);
//    
//    lib->data_restore();
//    
//    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerOption::onPopupClicked));
//    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
//    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
//    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
//}
//
//
//void DyLayerOption_callfunc_update_me(int value, int code)
//{
//    DyLayerOption *layerOption = (DyLayerOption*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYEROPTION_TAG);
//    
//    if(value == 0)
//    {
//        layerOption->schedule(schedule_selector(DyLayerOption::callfunc_update_me_success), 0.05f);
//    }
//    else
//    {
//        layerOption->schedule(schedule_selector(DyLayerOption::callfunc_update_me_fail), 0.05f);
//    }
//}


//void DyLayerOption::callfunc_update_me_success(float dt)
//{
//    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
//    
//    this->unschedule(schedule_selector(DyLayerOption::callfunc_update_me_success));
//  
//    DyLib* lib = DyLib::sharedLib();
//    
//    //dataManager->m_is_need_reset = false;
//    
//    lib->network_set_user_info(httpresponse_selector(DyLayerOption::onHttpRequestCompleted));
//    
//}
//
//
//void DyLayerOption::callfunc_update_me_fail(float dt)
//{
//    this->unschedule(schedule_selector(DyLayerOption::callfunc_update_me_fail));
//    DyLib* lib = DyLib::sharedLib();
//    
//    lib->show_network_alaram(false);
//    
//    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerOption::onPopupClicked));
//    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
//    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
//    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
//}
//
//
//void DyLayerOption_callfunc_update_me(int value, int code)
//{
//    DyLayerOption *layerOption = (DyLayerOption*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYEROPTION_TAG);
//    
//    if(value == 0)
//    {
//        layerOption->schedule(schedule_selector(DyLayerOption::callfunc_update_me_success), 0.05f);
//    }
//    else
//    {
//        layerOption->schedule(schedule_selector(DyLayerOption::callfunc_update_me_fail), 0.05f);
//    }
//}



void DyLayerOption_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str)
{
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    //DyLib* lib = DyLib::sharedLib();
    //DyLayerOption *layerOption = (DyLayerOption*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYEROPTION_TAG);
    
    if(value_int != 0)
        return;
    
    switch(cmdType)
    {
        case DyNetworkCmdType_ScoreMigration:
        {
            YtPlatform_NetKakao_DeleteMe(DyLayerOption_callfunc_delete_me);
            
        }
            break;
        default:
            break;
    }
}


void DyLayerOption::onPopupClicked(cocos2d::CCObject * pSender)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyLib* lib = DyLib::sharedLib();
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
    
    if(pPopup->m_active_tag == 10)
	{
        DyPopup* popup = lib->show_network_alaram(true);
        popup->setText(STRING_TITLE_UPDATE_DATA);
        
        dataManager->reset();
        
        DYNETWORK_ADD_PROC(DyNetworkCmdType_ScoreMigration, "", NULL, DyLayerOption_NetCallFunc);
        
    }
    
    if(pPopup->m_active_tag == 11)
	{
        DyPopup* popup = lib->show_network_alaram(true);
        popup->setText(STRING_TITLE_UPDATE_DATA);
       
        YtPlatform_NetKakao_Logout(DyLayerOption_callfunc_logout);
    }
    
//    if(pPopup->m_active_tag == 30)
//	{
//        dataManager->dataLoad_private(&dataManager->m_private, dataManager->m_reset_private_data);
//        dataManager->dataLoad_public(&dataManager->m_public, dataManager->m_reset_public_data, true);
//        
//        
//        lib->network_kakao_update_me(0, DyLayerOption_callfunc_update_me);
//        
//        
//        
//    }
    
    if(pPopup->m_active_tag == 40)
	{
        YtLib::app_end();
    }

    if(pPopup->m_active_tag == 20)
    {
        DyLib::sharedLib()->setStatus(DyLib_AppStatus_Story, DyStoryType_Option);
    }
    
    if(pPopup->m_active_tag == 2000)
	{
        dataManager->m_local.m_isPromised = false;
        dataManager->dataSave();
        
        DyLib::sharedLib()->setStatus(DyLib_AppStatus_Title, 0);
    }
    
    if(pPopup->m_active_tag == 2001)
	{
        DyLib::sharedLib()->setStatus(DyLib_AppStatus_Title, 0);
    }
}


void DyLayerOption::onLeaveClicked(CCObject * obj)
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

    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerOption::onPopupClicked), CCSizeMake(440, 350));
	popup->setText(STRING_LEAVE);
	popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
	popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 10);
	popup->addButton(DyButtonType_Close, "", 2);
    
	YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerOption::onSendmailClicked(CCObject * obj)
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

    
    int ver_high = CONFIG_VER/1000;
    int ver_mid = (CONFIG_VER - ver_high*1000)/100;
    int ver_low = CONFIG_VER%100;
    
    sprintf(lib->m_strTemp, "Ver %d.%d.%02d", ver_high, ver_mid, ver_low);
    
    YtPlatform_SendEmail(DyDataManager::sharedDataManager()->m_user_id.c_str(), lib->m_strTemp);
    
}


void DyLayerOption::onLogoutClicked(CCObject * obj)
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

    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerOption::onPopupClicked));
	popup->setText(STRING_LOGOUT);
	popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
	popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 11);
	popup->addButton(DyButtonType_Close, "", 2);
    
	YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerOption::onStoryClicked(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    
    if(m_callByMainMenu)
    {
        DyLib::sharedLib()->setStatus(DyLib_AppStatus_Story, DyStoryType_Option);
    }
    else
    {
        
        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerOption::onPopupClicked));
        popup->setText("지금 스토리를 보시려면 게임에서 나가야합니다. 정말 보시겠어요?");
        popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
        popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 20);
        popup->addButton(DyButtonType_Close, "", 0);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        
    }

    
    
    
}


void DyLayerOption::onCreditsClicked(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	DyLayerCredits* layerCredits =  DyLayerCredits::create(this, false);
	YtLayerManager::sharedLayerManager()->pushLayer(layerCredits, true, true);
}

void DyLayerOption::onBlogClicked(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	YtPlatform_GoWeb(BLOG_URL);
}



void DyLayerOption::onBoardClicked(CCObject * obj)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    std::string url = BOARD_URL;
    url.append("&user_id=");
    url.append(dataManager->m_user_id);
    
	YtPlatform_GoWebView(url.c_str());
}


//void DyLayerOption::onResetClicked(CCObject * obj)
//{
//    DyLib* lib = DyLib::sharedLib();
//    DyDataManager* dataManager = DyDataManager::sharedDataManager();
//    
//    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
//    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerOption::onPopupClicked));
//    
//    sprintf(lib->m_strTemp, "%s 시점으로 복구하시겠습니까?", dataManager->m_reset_last_date.c_str());
//    popup->setText(lib->m_strTemp);
//    popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
//    popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 30);
//    popup->addButton(DyButtonType_Close, "", 0);
//    
//    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
//}

void DyLayerOption::pause()
{
    m_menu->setEnabled(false);
    m_menuBlog->setEnabled(false);
    
    m_menuBoard->setEnabled(false);
    setKeypadEnabled(false);
}


void DyLayerOption::resume()
{
    m_menu->setEnabled(true);
    m_menuBlog->setEnabled(true);
    
    m_menuBoard->setEnabled(true);
    
    setKeypadEnabled(true);
}


void DyLayerOption::showBlogButton()
{
    //menuBlog->setPosition(ccp(230, 0));
	m_menuBlog->runAction(CCSequence::create(CCMoveTo::create(0.1f, ccp(-10, 0)),
                                           CCMoveTo::create(0.1f, ccp(0, 0)), NULL));
    
    m_menuBoard->runAction(CCSequence::create(CCMoveTo::create(0.1f, ccp(10, 0)),
                                             CCMoveTo::create(0.1f, ccp(0, 0)), NULL));


}


void DyLayerOption::hideBlogButton()
{
    m_menuBlog->runAction(CCSequence::create(CCMoveTo::create(0.1f, ccp(230, 0)), NULL));
    m_menuBoard->runAction(CCSequence::create(CCMoveTo::create(0.1f, ccp(-230, 0)), NULL));
}


void DyLayerOption::hide()
{
	YtLib* lib = YtLib::sharedLib();

	runAction(CCSequence::create(
        CCCallFunc::create(this, callfunc_selector(DyLayerOption::hideBlogButton)),
        CCDelayTime::create(0.1),
		CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)), 
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height)),

		CCCallFunc::create(this, callfunc_selector(DyLayerOption::destory)),
		NULL));
}


void DyLayerOption::show()
{
	YtLib* lib = YtLib::sharedLib();

	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));

	runAction(CCSequence::create(

		CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
        CCCallFunc::create(this, callfunc_selector(DyLayerOption::showBlogButton)),
		NULL));
}




void DyLayerOption::onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response)
{
    DyLib* lib = DyLib::sharedLib();
//    DyDataManager* dataManager = DyDataManager::sharedDataManager();
//    DyLayerOption *layerOption = (DyLayerOption*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYEROPTION_TAG);
    
    std::string tag = lib->network_getTag(response);
    
    if(lib->network_getTag(response).compare(NETWORK_SERVICE_UNREGIST) == 0)
    {
        lib->network_unregist_ret(sender, response);
        lib->show_network_alaram(false);
        
        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerOption::onPopupClicked));
        popup->setText(STRING_LEAVE_RESULT);
        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 2001);
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        return;
    }
    
//    if(lib->network_getTag(response).compare(NETWORK_SERVICE_SET_USER_INFO) == 0)
//    {
//        lib->show_network_alaram(false);
//        
//        layerOption->m_menu->getChildByTag(OPTION_UI_TAG_MENU_ITEM_RESET)->setVisible(dataManager->m_is_need_reset);
//        layerOption->getChildByTag(OPTION_UI_TAG_RESET_TEXT)->setVisible(dataManager->m_is_need_reset);
//        
//        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerOption::onPopupClicked));
//        
//        popup->setText("강제종료됩니다. 게임을 다시 실행해주세요.");
//        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 40);
//        popup->addButton(DyButtonType_Close, "", 0);
//        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
//
//        
//        return;
//    }
}





DyLayerOption* DyLayerOption::create(CCNode* owner, bool callByMainMenu)
{
	DyLayerOption* layerOption = (DyLayerOption*)YtLayerManager::create("DyLayerOption", DyLayerOptionLoader::loader(), "layer_option.ccbi", owner);

	layerOption->m_callByMainMenu = callByMainMenu;
	return layerOption;
}