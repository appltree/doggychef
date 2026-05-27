/*
DyLayerInvite.cpp
Doggy chef
@initialize at 130104

@history
130104	yoonsr	initialize
*/


#include "DyLayerInvite.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyFriend.h"
#include "DyItem.h"
#include "DyLayerShop.h"
#include "DyLayerRankDetail.h"
#include "DyPopup.h"
#include "DyNetwork.h"

#define PRESENT_SET_30          "set_04"
#define PRESENT_SET_40          "set_16"

DyLayerInvite::DyLayerInvite()
{
    m_tag = DYLAYERINVITE_TAG;
    
	m_arrPictureFrame = new CCArray();
    m_arrNormalFriend = new CCArray();
}


DyLayerInvite::~DyLayerInvite()
{
    m_arrPictureFrame->release();
    m_arrNormalFriend->release();
}


CCLayer* DyLayerInvite::createListLayer(float width)
{
	DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
	m_sizeItem = CCSizeMake(width, 70);
	m_itemCount = 0;
	for(unsigned int i = 0;i < friendManager->m_arrFriends->count();i++)
	{
		DyFriend* _friend = (DyFriend*)friendManager->m_arrFriends->objectAtIndex(i);

		if(_friend->m_friendType == DyFriendType_Normal)
		{
			m_itemCount++;
		}
	}

	CCLayerColor* layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), 
		m_sizeItem.width, m_sizeItem.height*m_itemCount);

	m_menuInvite = CCMenu::create();
    m_menuInvite->setPosition(CCPointZero);
    layerContent->addChild(m_menuInvite);
    
    
	int index = 0;
    int sendcount = 0;
	for(unsigned int i = 0;i < friendManager->m_arrFriends->count();i++)
	{
		DyFriend* _friend = (DyFriend*)friendManager->m_arrFriends->objectAtIndex(i);

    	if(_friend->m_friendType == DyFriendType_Normal)
		{
            m_arrNormalFriend->addObject(_friend);
            
            CCSprite* sprPictureFrame = CCSprite::createWithSpriteFrameName("main_ui_rank_picture_frame.png");
            sprPictureFrame->setPosition(ccp(40, m_sizeItem.height*m_itemCount - (index+1)*m_sizeItem.height + m_sizeItem.height/2));
			layerContent->addChild(sprPictureFrame);
            
            m_arrPictureFrame->addObject(sprPictureFrame);
            
			CCLabelTTF* labelName = CCLabelTTF::create(_friend->m_nickname.c_str(), FONT_NAME_EXP, 20.f, CCSizeMake(190, 23), kCCTextAlignmentLeft);
			labelName->setPosition(ccp(40+35, m_sizeItem.height*m_itemCount - (index+1)*m_sizeItem.height + m_sizeItem.height/2+15));
			labelName->setColor(ccc3(255,221,36));
			labelName->setAnchorPoint(ccp(0, 0.5));
			layerContent->addChild(labelName);
            
            
            CCSprite* sprGum = CCSprite::createWithSpriteFrameName("menushop_ui_gum.png");
            sprGum->setPosition(ccp(40+35, m_sizeItem.height*m_itemCount - (index+1)*m_sizeItem.height + m_sizeItem.height/2-15));
			sprGum->setAnchorPoint(ccp(0, 0.5));
            sprGum->setScale(0.8);
			layerContent->addChild(sprGum);


            CCLabelTTF* labelGumPlus = CCLabelTTF::create("+1 받기", FONT_NAME_EXP, 20.f, CCSizeMake(190, 20), kCCTextAlignmentLeft);
			labelGumPlus->setPosition(ccp(40+35+30, m_sizeItem.height*m_itemCount - (index+1)*m_sizeItem.height + m_sizeItem.height/2-15));
			labelGumPlus->setAnchorPoint(ccp(0, 0.5));
            labelGumPlus->setColor(ccWHITE);
			layerContent->addChild(labelGumPlus);

			CCMenuItemImage* menuitem = CCMenuItemImage::create();
			menuitem->initWithNormalSprite(
				CCSprite::createWithSpriteFrameName("menu_ui_button_invite_normal.png"),
				CCSprite::createWithSpriteFrameName("menu_ui_button_invite_touch.png"),
				CCSprite::createWithSpriteFrameName("menu_ui_button_invite_unuse.png"), this, menu_selector(DyLayerInvite::onInviteClicked));

			menuitem->setTag(index);
			menuitem->setPosition(ccp(m_sizeItem.width - 80, m_sizeItem.height*m_itemCount - (index+1)*m_sizeItem.height + m_sizeItem.height/2));

			m_menuInvite->addChild(menuitem);
            
            CCDate dataCurrent = dataManager->getCurrentDate();
            CCDate dataDiff = dataCurrent - _friend->m_time_last_message_sent_at;
            
            if(_friend->m_message_blocked || (_friend->m_time_last_message_sent_at.time() > 0 && dataManager->m_invitation_interval > dataDiff.time()))
            {
                menuitem->setEnabled(false);
            }
            
            if(_friend->m_time_last_message_sent_at.time() > 0 && dataManager->m_invitation_interval > dataDiff.time())
            {
                sendcount++;
            }

			if(index < m_itemCount - 1)
			{
				CCSprite* sprLine = CCSprite::createWithSpriteFrameName("main_ui_ranking_line.png");
				sprLine->setPosition(ccp(m_sizeItem.width/2, m_sizeItem.height*m_itemCount - (index+1)*m_sizeItem.height));
				layerContent->addChild(sprLine);
			}
			index++;
		}
	}

//    if(sendcount > dataManager->m_private.m_cntInvite)
//    {
//        dataManager->m_private.m_cntInvite = sendcount;
//    }
    
    
  	return layerContent;
}



void DyLayerInvite::loadFriends()
{
    
    //	CCScale9Sprite *background = CCScale9Sprite::createWithSpriteFrame(CCSprite::createWithSpriteFrameName("menu_ui_popup.png")->displayFrame());
    //	background->setContentSize(this->getContentSize());
    //	addChild(background);
    //
    //	m_menu = CCMenu::create();
    //    m_menu->setPosition(CCPointZero);
    //    addChild(m_menu);
    //
    //	CCMenuItemImage* closeMenu = CCMenuItemImage::create();
    //	closeMenu->initWithNormalSprite(CCSprite::createWithSpriteFrameName("menu_ui_close.png"),
    //		CCSprite::createWithSpriteFrameName("menu_ui_close_sel.png"),
    //		CCSprite::createWithSpriteFrameName("menu_ui_close.png"), this, menu_selector(DyLayerInvite::onCloseClicked));
    //	closeMenu->setPosition(ccp(this->getContentSize().width/2-20, this->getContentSize().height/2-20));
    //	m_menu->addChild(closeMenu);
    
	CCSize sizeScrollView = CCSizeMake(400, 297);
    
	CCLayer* layerContent = createListLayer(sizeScrollView.width);
    
	m_layerContents = CCScrollView::create(sizeScrollView, layerContent);
	m_layerContents->setDelegate(this);
    
	m_layerContents->setContentSize(layerContent->getContentSize());
	m_layerContents->setDirection(kCCScrollViewDirectionVertical);
	m_layerContents->setContentOffset(m_layerContents->minContainerOffset());
	
    
	m_layerContents->setGlobalScale(YtLib::sharedLib()->m_scale);
	m_layerContents->setPosition(ccp(-sizeScrollView.width/2, -sizeScrollView.height/2));
    
	if(sizeScrollView.height > layerContent->getContentSize().height)
	{
		m_layerContents->setTouchEnabled(false);
	}
	
    getChildByTag(INVITE_UI_TAG_CONTENTLAYER)->addChild(m_layerContents);
    
	resume();
    
    setKeypadEnabled(true);
    
    getChildByTag(INVITE_UI_TAG_WAIT)->setVisible(false);
}

void DyLayerInvite::onEnter()
{
	CCLayer::onEnter();

	if(m_isInit)
		return;
	m_isInit = true;
}


void DyLayerInvite::onExit()
{
	CCLayer::onExit();
}

void DyLayerInvite::callfunc_update_me_success(float dt)
{
    unschedule(schedule_selector(DyLayerInvite::callfunc_update_me_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    
    switch(m_present)
    {
        case 0:
            {
                DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerInvite::onPopupClicked));
                sprintf(lib->m_strTemp, "%s님께 초대장 보내기 성공하여 껌하나 받으셨습니다", m_processFriend->m_nickname.c_str());
                
                popup->setText(lib->m_strTemp);
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
                
            }
            break;
        case 1:
            {
                DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerInvite::onPopupClicked));
                popup->setText("축하합니다. 무료룰렛이용권 1장 받으셨습니다.");
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);

            }
            break;
        case 2:
            {
                DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerInvite::onPopupClicked));
                popup->setText("축하합니다. 10,000골드 받으셨습니다.");
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
                
            }

            break;
        case 3:
            {
                DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerInvite::onPopupClicked));
                popup->setText("축하합니다. 10델리 받으셨습니다.");
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
            }
            break;
        case 4:
            {
                DyCostume* costume = DyCostumeManager::sharedCostumeManager()->getCostume(PRESENT_SET_30);
                
                DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerInvite::onPopupClicked));
                
                sprintf(lib->m_strTemp, "축하합니다. %s 받으셨습니다.", costume->m_name.c_str());
                popup->setText(lib->m_strTemp);
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
            }
            break;
        case 5:
            {
                DyCostume* costume = DyCostumeManager::sharedCostumeManager()->getCostume(PRESENT_SET_40);
                
                DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerInvite::onPopupClicked));
                
                sprintf(lib->m_strTemp, "축하합니다. %s 받으셨습니다.", costume->m_name.c_str());
                popup->setText(lib->m_strTemp);
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
            }
            break;
        case 6:
            {
                DyCostume* costume = DyCostumeManager::sharedCostumeManager()->getCostume(PRESENT_SET_30);
                
                DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerInvite::onPopupClicked));
                
                if(costume->m_price_delly)
                {
                    sprintf(lib->m_strTemp, "축하합니다. %s 보유하셔서 해당 %d델리 받으셨습니다.", costume->m_name.c_str(), costume->m_price_delly);
                    popup->setText(lib->m_strTemp);
                }
                else if(costume->m_price_gold)
                {
                    sprintf(lib->m_strTemp, "축하합니다. %s 보유하셔서 해당 %d골드 받으셨습니다.", costume->m_name.c_str(), costume->m_price_gold);
                    popup->setText(lib->m_strTemp);
                }
                
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
            }
            break;
        case 7:
            {
                DyCostume* costume = DyCostumeManager::sharedCostumeManager()->getCostume(PRESENT_SET_40);
                
                DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerInvite::onPopupClicked));
                
                //if(costume->m_price_delly)
                {
                    sprintf(lib->m_strTemp, "축하합니다. %s 보유하셔서 해당 %d델리 받으셨습니다.", costume->m_name.c_str(), 40);
                    popup->setText(lib->m_strTemp);
                }
//                else if(costume->m_price_gold)
//                {
//                    sprintf(lib->m_strTemp, "축하합니다. %s 보유하셔서 해당 %d골드 받으셨습니다.", costume->m_name.c_str(), costume->m_price_gold);
//                    popup->setText(lib->m_strTemp);
//                }
                
                popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
                YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
                
            }
            break;
    }
}



void DyLayerInvite::callfunc_update_me_fail(float dt)
{
    unschedule(schedule_selector(DyLayerInvite::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerInvite::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}


void DyLayerInvite_callfunc_update_me(int value, int code)
{
    DyLayerInvite *layerInvite = (DyLayerInvite*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERINVITE_TAG);
    
    if(value == 0)
    {
        layerInvite->schedule(schedule_selector(DyLayerInvite::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layerInvite->schedule(schedule_selector(DyLayerInvite::callfunc_update_me_fail), 0.05f);
    }
}


void DyLayerInvite_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str)
{
    DyLayerInvite *layerInvite = (DyLayerInvite*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERINVITE_TAG);
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    DyLib* lib = DyLib::sharedLib();
    
    switch(cmdType)
    {
        case DyNetworkCmdType_Invite:
            if(value_int == 0)
            {
                layerInvite->m_menuitemProcess->setEnabled(false);
                layerInvite->m_processFriend->m_time_last_message_sent_at = dataManager->getCurrentDate();
                layerInvite->m_present = 0;
                dataManager->m_private.m_cntInvite++;
                
                CCDate dateCurrent = dataManager->getCurrentDate();
                
                if(dataManager->m_private.m_inviteDay != dateCurrent.mday())
                {
                    dataManager->m_private.m_inviteDay = dateCurrent.mday();
                    dataManager->m_private.m_cntInviteToday = 0;
                }
                
                dataManager->m_private.m_cntInviteToday++;
                
                if(dataManager->m_private.m_cntInvite > 40)
                    dataManager->m_private.m_cntInvite = 0;
                
                moneyManager->addGum(1, true);
                
                if(dataManager->m_private.m_cntInvite == 5)
                {
                    layerInvite->m_present = 1;
                    dataManager->m_private.m_cntFreeRoullete += 1;
                    if(dataManager->m_private.m_cntFreeRoullete >= MAX_CHARNUM)
                        dataManager->m_private.m_cntFreeRoullete = MAX_CHARNUM;
                }
                else if(dataManager->m_private.m_cntInvite == 10)
                {
                    layerInvite->m_present = 2;
                    
                    moneyManager->addGold(10000, true);
                    
                }
                else if(dataManager->m_private.m_cntInvite == 20)
                {
                    layerInvite->m_present = 3;
                    
                    moneyManager->addDelly(10, true);
                }
                else if(dataManager->m_private.m_cntInvite == 30)
                {
                    DyCostume* costume = DyCostumeManager::sharedCostumeManager()->getCostume(PRESENT_SET_30);
                    
                    if(!DyCostumeManager::sharedCostumeManager()->hasCostume(PRESENT_SET_30))
                    {
                        layerInvite->m_present = 4;
                        DyCostumeManager::sharedCostumeManager()->buyedCostume(costume);
                    }
                    else
                    {
                        layerInvite->m_present = 6;
                        if(costume->m_price_gold > 0)
                            moneyManager->addGold(costume->m_price_gold, true);
                        else if(costume->m_price_delly > 0)
                            moneyManager->addDelly(costume->m_price_delly, true);
                    }
                }
                else if(dataManager->m_private.m_cntInvite == 40)
                {
                    DyCostume* costume = DyCostumeManager::sharedCostumeManager()->getCostume(PRESENT_SET_40);
                    
                    if(!DyCostumeManager::sharedCostumeManager()->hasCostume(PRESENT_SET_40))
                    {
                        layerInvite->m_present = 5;
                        DyCostumeManager::sharedCostumeManager()->buyedCostume(costume);
                    }
                    else
                    {
                        layerInvite->m_present = 7;
                        moneyManager->addDelly(40, true);
//                        if(costume->m_price_gold > 0)
//                            moneyManager->addGold(costume->m_price_gold, true);
//                        else if(costume->m_price_delly > 0)
//                            moneyManager->addDelly(costume->m_price_delly, true);
                    }
                    
                    dataManager->m_private.m_cntInvite = 0;
                }
                
                layerInvite->refresh();
                
                lib->network_kakao_update_me(1, DyLayerInvite_callfunc_update_me);
                
                layerInvite->m_processFriend->m_time_last_message_sent_at = dataManager->getCurrentDate();
                DyFriendManager::sharedFriendManager()->addInvitedFriend(layerInvite->m_processFriend->m_user_id, dataManager->getCurrentDate().time());
            }
            else if(value_int == 0)
            {
                
            }
            break;
        default:
            break;
    }
}


void DyLayerInvite::onPopupClicked(cocos2d::CCObject * pSender)
{
    DyLib* lib = DyLib::sharedLib();
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    
	if(pPopup->m_active_tag == 1)
	{
        
        DYNETWORK_ADD_PROC(DyNetworkCmdType_Invite, "", m_processFriend, DyLayerInvite_NetCallFunc);
//        DyNetworkCommand* commandNetwork = new DyNetworkCommand(DyNetworkCmdType_Invite, "", m_processFriend, DyLayerInvite_NetCallFunc);
//        DyNetworkManager::sharedNetworkManager()->addLast(commandNetwork);
//        commandNetwork->release();
        
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        lib->show_network_alaram(true);
        
	}
    else
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}



void DyLayerInvite::onInviteClicked(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	
    DyLib* lib = DyLib::sharedLib();
//    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    CCMenuItemImage * pMenuItem = dynamic_cast<CCMenuItemImage*>(obj);
	int index = pMenuItem->getTag();

//    CCDate dateCurrent = dataManager->getCurrentDate();
//    if(dataManager->m_private.m_inviteDay == dateCurrent.mday() && dataManager->m_private.m_cntInviteToday == 20)
//    {
//        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
//        popup->setText("1일 초대 제한 횟수 20회를 초과하였습니다.");
//        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
//        
//        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
//        return;
//    }
    
//    if(dataManager->m_private.m_cntInvite == 40)
//    {
//        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
//        popup->setText("40명이상 초대하실 수 없습니다.");
//        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
//        
//        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
//        return;
//    }
    
    
    m_processFriend = (DyFriend*)m_arrNormalFriend->objectAtIndex(index);
    m_menuitemProcess = pMenuItem;
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerInvite::onPopupClicked));
    
    sprintf(lib->m_strTemp, "\"%s\"님에게 초대장을 카카오톡으로 보내시겠어요?", m_processFriend->m_nickname.c_str());
    
    popup->setText(lib->m_strTemp);
    
    popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
    popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 1);
    popup->addButton(DyButtonType_Close, "", 0);
    
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerInvite::onCloseClicked(cocos2d::CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLOSE);
	YtLayerManager::sharedLayerManager()->popLayer();
}


void DyLayerInvite::keyBackClicked()
{
    if(DyLib::sharedLib()->m_showLoadingPopup)
        return;

    setKeypadEnabled(false);
    onCloseClicked(NULL);
}


void DyLayerInvite::refresh()
{
    DyLib* lib = DyLib::sharedLib();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    CCLabelTTF* labelInviteCnt = (CCLabelTTF*)getChildByTag(INVITE_UI_TAG_REMAIN);
    
    sprintf(lib->m_strTemp, "%d", dataManager->m_private.m_cntInvite);
    labelInviteCnt->setString(lib->m_strTemp);
}


void DyLayerInvite::scrollViewDidScroll(CCScrollView* view)
{
    //DyLib* lib = DyLib::sharedLib();
    
    CCPoint posOffset = view->getContentOffset();
    CCSize sizeView = view->getViewSize();
    
    DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
    
    for(int offsetY = posOffset.y;offsetY < posOffset.y + sizeView.height + m_sizeItem.height;offsetY += m_sizeItem.height)
    {
        float offsetStart = m_sizeItem.height*m_itemCount - sizeView.height;
        
        int index = (offsetStart + offsetY)/m_sizeItem.height;
        
        if(index >= 0 && index < m_arrPictureFrame->count())
        {
            CCSprite* sprPictureFrame = (CCSprite*)this->m_arrPictureFrame->objectAtIndex(index);
            
            if(sprPictureFrame->getChildrenCount() == 0)
            {
                DyFriend* _friend = (DyFriend*)m_arrNormalFriend->objectAtIndex(index);
                
                CCSprite* sprPicture = friendManager->getPicture(_friend, CCSizeMake(48, 48));
                
                if(sprPicture)
                {
                    sprPictureFrame->addChild(sprPicture);
                    sprPicture->setPosition(ccp(sprPictureFrame->getContentSize().width/2, sprPictureFrame->getContentSize().height/2));
                }
            }
        }
    }
}


void DyLayerInvite::scrollViewDidZoom(CCScrollView* view)
{
}


void DyLayerInvite::scrollViewTouchContent(CCScrollView* view, CCPoint position)
{
}


void DyLayerInvite::pause()
{
    
    m_menuBoard->setEnabled(false);
	m_menu->setEnabled(false);
	m_menuInvite->setEnabled(false);
    setKeypadEnabled(false);
    
}


void DyLayerInvite::resume()
{
    m_menuBoard->setEnabled(true);
	m_menu->setEnabled(true);
	m_menuInvite->setEnabled(true);
    
    refresh();
    setKeypadEnabled(true);
}

void DyLayerInvite::showBoardButton()
{
    m_menuBoard->runAction(CCSequence::create(CCMoveTo::create(0.1f, ccp(-411, -223)),
                                              CCMoveTo::create(0.1f, ccp(0, 0)), NULL));
    
    
}


void DyLayerInvite::hideBoardButton()
{
    m_menuBoard->runAction(CCSequence::create(CCMoveTo::create(0.1f, ccp(-230, 0)), NULL));
}


void DyLayerInvite::hide()
{
	YtLib* lib = YtLib::sharedLib();
    
	runAction(CCSequence::create(
                                 CCCallFunc::create(this, callfunc_selector(DyLayerInvite::hideBoardButton)),
                                 CCDelayTime::create(0.1),
                                 CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)),
                                 CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
                                 CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2)),
                                 
                                 CCCallFunc::create(this, callfunc_selector(DyLayerInvite::destory)),
                                 NULL));
}


void DyLayerInvite::show()
{
	YtLib* lib = YtLib::sharedLib();
    
	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));
    
	runAction(CCSequence::create(
                                 
                                 CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
                                 CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
                                 CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
                                 CCCallFunc::create(this, callfunc_selector(DyLayerInvite::showBoardButton)),
                                 CCCallFunc::create(this, callfunc_selector(DyLayerInvite::loadFriends)),
                                 NULL));
}


void DyLayerInvite::onBoardClicked(CCObject * obj)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    std::string url = BOARD_URL;
    url.append("&user_id=");
    url.append(dataManager->m_user_id);
    
	YtPlatform_GoWebView(url.c_str());
}


void DyLayerInvite::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyLib* lib = DyLib::sharedLib();
    
    m_menu = (CCMenu*)getChildByTag(INVITE_UI_TAG_MENU);
    m_menuBoard = (CCMenu*)getChildByTag(INVITE_UI_TAG_MENU_BOARD);
    
    
    sprintf(lib->m_strTemp, "이미 초대한 친구는 %d일 후 다시 초대할 수 있습니다.", dataManager->m_invitation_interval/(60*60*24));
    CCLabelTTF* labelNotice = (CCLabelTTF*)getChildByTag(INVITE_UI_TAG_NOTICE);
    labelNotice->setString(lib->m_strTemp);
    
    refresh();
    
    m_menuBoard->setPosition(ccp(-230, 0));
    
    if(dataManager->m_private.m_cntInvite > 40)
        dataManager->m_private.m_cntInvite = 0;

    
}


SEL_MenuHandler DyLayerInvite::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName)
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCloseClicked", DyLayerInvite::onCloseClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onBoardClicked", DyLayerInvite::onBoardClicked);
    return NULL;
}

SEL_CCControlHandler DyLayerInvite::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName)
{
	return NULL;
}

bool DyLayerInvite::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode)
{
    return false;
}




DyLayerInvite* DyLayerInvite::create(CCNode* owner)
{
	DyLayerInvite* layerInvite = (DyLayerInvite*)YtLayerManager::create("DyLayerInvite", DyLayerInviteLoader::loader(), "layer_invite.ccbi", owner);
	return layerInvite;
}