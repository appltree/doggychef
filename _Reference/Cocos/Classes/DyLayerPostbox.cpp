/*
DyLayerPostbox.cpp
Doggy chef
@initialize at 121231

@history
121231	yoonsr	initialize
*/


#include "DyLayerPostbox.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyFriend.h"
#include "DyItem.h"
#include "DyLayerShop.h"
#include "DyPopup.h"
#include "DyGift.h"
#include "DyNotice.h"
#include "DyLayerNotice.h"


DyLayerPostbox::DyLayerPostbox()
{
    m_tag = DYLAYERPOSTBOX_TAG;
    
    m_arrPictureFrame = new CCArray();
    m_arrNormalFriend = new CCArray();
    m_layerContents = NULL;
}


DyLayerPostbox::~DyLayerPostbox()
{
	DyMoneyManager::sharedMoneyManager()->removeMoneyLayer(m_layerMoney);

	m_layerMoney->release();
    
    m_arrPictureFrame->release();
    m_arrNormalFriend->release();
}


void DyLayerPostbox::refresh()
{
    if(m_layerContents)
        m_layerContents->removeFromParentAndCleanup(true);
    
	CCSize sizeScrollView = CCSizeMake(820, 345+45);
    
	CCLayer* layerContent = createListLayer(sizeScrollView.width);
    
	m_layerContents = CCScrollView::create(sizeScrollView, layerContent);
	m_layerContents->setDelegate(this);
    
	m_layerContents->setContentSize(layerContent->getContentSize());
	m_layerContents->setDirection(kCCScrollViewDirectionVertical);
	m_layerContents->setContentOffset(m_layerContents->minContainerOffset());
	//layerContents->setContentOffset(ccp(0,0));
	m_layerContents->setGlobalScale(YtLib::sharedLib()->m_scale);
	m_layerContents->setPosition(ccp(-sizeScrollView.width/2, -sizeScrollView.height/2+12));
    
	if(sizeScrollView.height > layerContent->getContentSize().height)
	{
		m_layerContents->setTouchEnabled(false);
	}
	addChild(m_layerContents);
    //DyGiftManager* giftManager = DyGiftManager::sharedGiftManager();
    
    //getChildByTag(POSTBOX_UI_TAG_LABEL_NOTPRESENT)->setVisible(giftManager->getGiftCount() == 0);
}

CCLayer* DyLayerPostbox::createListLayer(float width)
{
	DyLib* lib = DyLib::sharedLib();
	
	DyGiftManager* giftManager = DyGiftManager::sharedGiftManager();
    DyNoticeManager* noticeManager = DyNoticeManager::sharedNoticeManager();
    
    m_sizeItem = CCSizeMake(820, 95);
	m_itemCount = giftManager->m_arrReceivedGifts->count() + noticeManager->getCount();

	CCLayerColor* layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), 
		m_sizeItem.width, m_sizeItem.height*m_itemCount);

	m_menuRecieve = CCMenu::create();
    m_menuRecieve->setPosition(CCPointZero);
    layerContent->addChild(m_menuRecieve);

    m_arrPictureFrame->removeAllObjects();
    m_arrNormalFriend->removeAllObjects();
    
	for(int i = 0;i < m_itemCount;i++)
	{
        int index = i;
        
        if(i < noticeManager->getCount())
        {
            DyNotice* notice = (DyNotice*)noticeManager->m_arrNotices->objectAtIndex(index);
            
            CCSprite* sprOperator = CCSprite::createWithSpriteFrameName("menu_ui_icon_operator.png");
            sprOperator->setPosition(ccp(30, m_sizeItem.height*m_itemCount - (i+1)*m_sizeItem.height + m_sizeItem.height/2));
            layerContent->addChild(sprOperator);
            
            CCLabelTTF* labelContent = CCLabelTTF::create(notice->m_title.c_str(), FONT_NAME_EXP, 25.f);
            labelContent->setPosition(ccp(70, m_sizeItem.height*m_itemCount - (i+1)*m_sizeItem.height + m_sizeItem.height/2));
            labelContent->setColor(ccc3(57,27,5));
            
            labelContent->setAnchorPoint(ccp(0, 0.5));
            
            layerContent->addChild(labelContent);
            
            
            CCMenuItemImage* menuitemView = CCMenuItemImage::create();
            
            menuitemView->initWithNormalSprite(
                                           CCSprite::createWithSpriteFrameName("menu_ui_btn_notice_see.png"),
                                           CCSprite::createWithSpriteFrameName("menu_ui_btn_notice_see_sel.png"),
                                           CCSprite::createWithSpriteFrameName("menu_ui_btn_notice_see.png"), this, menu_selector(DyLayerPostbox::onViewClicked));
            
            menuitemView->setTag(index);
            menuitemView->setPosition(ccp(m_sizeItem.width - 70+15-40-40, m_sizeItem.height*m_itemCount - (i+1)*m_sizeItem.height + m_sizeItem.height/2));
            
            m_menuRecieve->addChild(menuitemView);
            
            CCMenuItemImage* menuitemDelete = CCMenuItemImage::create();
            menuitemDelete->initWithNormalSprite(
                                               CCSprite::createWithSpriteFrameName("menu_ui_btn_delete.png"),
                                               CCSprite::createWithSpriteFrameName("menu_ui_btn_delete_sel.png"),
                                               CCSprite::createWithSpriteFrameName("menu_ui_btn_delete.png"), this, menu_selector(DyLayerPostbox::onDeleteClicked));
            
            
            menuitemDelete->setTag(index);
            menuitemDelete->setPosition(ccp(m_sizeItem.width - 70+15+40-40, m_sizeItem.height*m_itemCount - (i+1)*m_sizeItem.height + m_sizeItem.height/2));
            
            m_menuRecieve->addChild(menuitemDelete);

        }
        else
        {
            index = i - noticeManager->getCount();
            
            DyGift* gift = (DyGift*)giftManager->m_arrReceivedGifts->objectAtIndex(index);

            CCLabelTTF* labelName = NULL;
            
            if(gift->m_sender)
            {
                m_arrNormalFriend->addObject(gift->m_sender);
            
                CCSprite* sprPictureFrame = CCSprite::createWithSpriteFrameName("main_ui_rank_picture_frame.png");
                sprPictureFrame->setPosition(ccp(30, m_sizeItem.height*m_itemCount - (i+1)*m_sizeItem.height + m_sizeItem.height/2));
                layerContent->addChild(sprPictureFrame);
            
                m_arrPictureFrame->addObject(sprPictureFrame);
            
                labelName = CCLabelTTF::create(gift->m_sender->m_nickname.c_str(), FONT_NAME_EXP, 25.f);
                labelName->setPosition(ccp(0, m_sizeItem.height*m_itemCount - (i+1)*m_sizeItem.height + m_sizeItem.height/2));
                labelName->setColor(ccc3(255,109,54));
                layerContent->addChild(labelName);
            }
            else
            {
                CCSprite* sprOperator = CCSprite::createWithSpriteFrameName("menu_ui_icon_operator.png");
                sprOperator->setPosition(ccp(30, m_sizeItem.height*m_itemCount - (i+1)*m_sizeItem.height + m_sizeItem.height/2));
                layerContent->addChild(sprOperator);
            }
            
            CCMenuItemImage* menuitem = CCMenuItemImage::create();

            switch(gift->m_gifttype)
            {
                case DyGiftTypeCostumeSet:
                case DyGiftTypeRoullette:
                    menuitem->initWithNormalSprite(
                                                   CCSprite::createWithSpriteFrameName("main_ui_item_plus.png"),
                                                   CCSprite::createWithSpriteFrameName("main_ui_item_plus_sel.png"),
                                                   CCSprite::createWithSpriteFrameName("main_ui_item_plus.png"), this, menu_selector(DyLayerPostbox::onAcceptClicked));
                    
                    break;

                case DyGiftTypeDelly:
                    menuitem->initWithNormalSprite(
                        CCSprite::createWithSpriteFrameName("main_ui_delly_plus.png"),
                        CCSprite::createWithSpriteFrameName("main_ui_delly_plus_sel.png"), 
                        CCSprite::createWithSpriteFrameName("main_ui_delly_plus.png"), this, menu_selector(DyLayerPostbox::onAcceptClicked));

                    break;
                case DyGiftTypeGum:
                    menuitem->initWithNormalSprite(
                        CCSprite::createWithSpriteFrameName("main_ui_btn_gum_plus.png"),
                        CCSprite::createWithSpriteFrameName("main_ui_btn_gum_plus_sel.png"), 
                        CCSprite::createWithSpriteFrameName("main_ui_btn_gum_plus.png"), this, menu_selector(DyLayerPostbox::onAcceptClicked));
                    break;
                case DyGiftTypeGaest:
                    menuitem->initWithNormalSprite(
                        CCSprite::createWithSpriteFrameName("main_ui_btn_geast_plus.png"),
                        CCSprite::createWithSpriteFrameName("main_ui_btn_gaest_plus_sel.png"), 
                        CCSprite::createWithSpriteFrameName("main_ui_btn_geast_plus.png"), this, menu_selector(DyLayerPostbox::onAcceptClicked));
                    break;
                case DyGiftTypeGold:
                    menuitem->initWithNormalSprite(
                                                   CCSprite::createWithSpriteFrameName("main_ui_gold_plus.png"),
                                                   CCSprite::createWithSpriteFrameName("main_ui_gold_plus_sel.png"),
                                                   CCSprite::createWithSpriteFrameName("main_ui_gold_plus.png"), this, menu_selector(DyLayerPostbox::onAcceptClicked));
                    break;

                default:
                    break;
            }
            menuitem->setTag(index);
            menuitem->setPosition(ccp(m_sizeItem.width - 70+15, m_sizeItem.height*m_itemCount - (i+1)*m_sizeItem.height + m_sizeItem.height/2));
       
            if(gift->m_isAccepted)
                menuitem->setVisible(false);

            m_menuRecieve->addChild(menuitem);
            
            CCLabelTTF* labelContent = CCLabelTTF::create(gift->m_message.c_str(), FONT_NAME_EXP, 25.f);
            labelContent->setPosition(ccp(0, m_sizeItem.height*m_itemCount - (i+1)*m_sizeItem.height + m_sizeItem.height/2));
            labelContent->setColor(ccc3(57,27,5));
            
            layerContent->addChild(labelContent);

            if(labelName)
                YtLib::arrangeNodesH(ccp(70, labelContent->getPosition().y), ccp(0.0, 0.5), 5, labelName, labelContent, NULL);
            else
            {
                labelContent->setAnchorPoint(ccp(0, 0.5));
                labelContent->setPosition(ccp(70, labelContent->getPosition().y));
            }
            
            gift->m_time_send_at.to_str(lib->m_strTemp, 512);
            
            CCLabelTTF* labelDate = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 20.f);
            labelDate->setPosition(ccp(730, m_sizeItem.height*m_itemCount - (i+1)*m_sizeItem.height + m_sizeItem.height/2 - 35));
            labelDate->setColor(ccc3(57,27,5));
            labelDate->setAnchorPoint(ccp(1,0));
            
            layerContent->addChild(labelDate);
        }
        
        //
        // 구분선
		if(i < m_itemCount - 1)
		{
			CCSprite* sprLine = CCSprite::createWithSpriteFrameName("menushop_ui_horizontal_line_big.png");
			sprLine->setPosition(ccp(m_sizeItem.width/2, m_sizeItem.height*m_itemCount - (i+1)*m_sizeItem.height));
			layerContent->addChild(sprLine);
		}
	}

	return layerContent;
}


//void DyLayerPostbox::onSendClicked(CCObject* obj)
//{
//    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
//    
////    CCMenuItemImage * pMenuItem = dynamic_cast<CCMenuItemImage*>(obj);
////	int index = pMenuItem->getTag();
//    
////	DyGiftManager* giftManager = DyGiftManager::sharedGiftManager();
////	giftManager->accept(index);
////    
////	pMenuItem->setVisible(false);
//}
//

void DyLayerPostbox::onEnter()
{
	CCLayer::onEnter();

	if(m_isInit)
		return;
	m_isInit = true;
	
	m_menu = (CCMenu*)this->getChildByTag(POSTBOX_UI_TAG_MENU);
	
	CCMenuItemImage* menuitemAcceptAll = (CCMenuItemImage*)m_menu->getChildByTag(POSTBOX_UI_TAG_MENUITEM_ACCEPT_ALL);
	DyLib::setMenuItemText_kor(menuitemAcceptAll, DyButtonType_Red, "모두 받기", 25, 27);

	m_layerMoney = new DyLayerMoney(DyLayerMoneyType_Normal, 0);
	
	m_layerMoney->setPosition(ccp(-410.f+100, 240.f));
	addChild(m_layerMoney);
	DyMoneyManager::sharedMoneyManager()->addMoneyLayer(m_layerMoney);

    refresh();
	resume();
    
    setKeypadEnabled(true);
}


void DyLayerPostbox::onExit()
{
	CCLayer::onExit();
}


void DyLayerPostbox::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
}


SEL_MenuHandler DyLayerPostbox::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCloseClicked", DyLayerPostbox::onCloseClicked);
	//CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onSendgiftClicked", DyLayerPostbox::onSendgiftClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onAcceptallClicked", DyLayerPostbox::onAcceptallClicked);
    return NULL;    
}

SEL_CCControlHandler DyLayerPostbox::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}

bool DyLayerPostbox::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerPostbox::onCloseClicked(cocos2d::CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLOSE);
	YtLayerManager::sharedLayerManager()->popLayer();
}

//void DyLayerPostbox::onSendgiftClicked(cocos2d::CCObject * pSender)
//{
//    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
//    
//	DyLayerSendgift* layerSendgift = DyLayerSendgift::create();
//	YtLayerManager::sharedLayerManager()->pushLayer(layerSendgift, true);
//}




void DyLayerPostbox::onViewClicked(cocos2d::CCObject * pSender)
{
    //DyLib* lib = DyLib::sharedLib();
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    CCMenuItemImage * pMenuItem = dynamic_cast<CCMenuItemImage*>(pSender);
	int index = pMenuItem->getTag();
    
	DyNoticeManager* noticeManager = DyNoticeManager::sharedNoticeManager();
    
    DyNotice* notice = (DyNotice*)noticeManager->m_arrNotices->objectAtIndex(index);
                                                                             
    DyLayerNotice* layerNotice = DyLayerNotice::create(this, notice);
    YtLayerManager::sharedLayerManager()->pushLayer(layerNotice, true, true);
}


void DyLayerPostbox::onDeleteClicked(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    CCMenuItemImage * pMenuItem = dynamic_cast<CCMenuItemImage*>(obj);
	int index = pMenuItem->getTag();
    
	DyNoticeManager* noticeManager = DyNoticeManager::sharedNoticeManager();
    
    DyNotice* notice = (DyNotice*)noticeManager->m_arrNotices->objectAtIndex(index);
    
    
    noticeManager->deleteNotice(notice);
    
    refresh();
    
}


void DyLayerPostbox::callfunc_update_me_success(float dt)
{
    unschedule(schedule_selector(DyLayerPostbox::callfunc_update_me_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    //YtLayerManager::sharedLayerManager()->popLayer();
}



void DyLayerPostbox::callfunc_update_me_fail(float dt)
{
    unschedule(schedule_selector(DyLayerPostbox::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShop::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}


void DyLayerPostbox_callfunc_update_me(int value, int code)
{
    DyLayerPostbox *layerPostbox = (DyLayerPostbox*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPOSTBOX_TAG);
    
    if(value == 0)
    {
        layerPostbox->schedule(schedule_selector(DyLayerPostbox::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layerPostbox->schedule(schedule_selector(DyLayerPostbox::callfunc_update_me_fail), 0.05f);
    }
}




void DyLayerPostbox::callfunc_load_accept_message_success(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerPostbox *layerPostbox = (DyLayerPostbox*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPOSTBOX_TAG);
    
    layerPostbox->unschedule(schedule_selector(DyLayerPostbox::callfunc_load_accept_message_success));
    
    DyGiftManager* giftManager = DyGiftManager::sharedGiftManager();
    giftManager->accept(layerPostbox->m_processGift, true);
    
    layerPostbox->refresh();
    
    
    lib->network_kakao_update_me(0, DyLayerPostbox_callfunc_update_me);
    
//    DySceneMainMenu* pMainMenu = new DySceneMainMenu();
//    YtSceneMananger::changeScene(pMainMenu, DyLayerMainMenuLoader::loader(), "layer_mainmenu.ccbi", "DyLayerMainMenu");
//    pMainMenu->release();
}


void DyLayerPostbox::callfunc_load_accept_message_fail(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerPostbox *layerPostbox = (DyLayerPostbox*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPOSTBOX_TAG);
    
    layerPostbox->unschedule(schedule_selector(DyLayerPostbox::callfunc_load_accept_message_fail));
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLib::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}



void DyLayerPostbox_callfunc_accept_message(int value, int code)
{
    DyLayerPostbox *layerPostbox = (DyLayerPostbox*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPOSTBOX_TAG);
    
    if(value == 0)
    {
        layerPostbox->schedule(schedule_selector(DyLayerPostbox::callfunc_load_accept_message_success), 0.05f);
    }
    else
    {
        layerPostbox->schedule(schedule_selector(DyLayerPostbox::callfunc_load_accept_message_fail), 0.05f);
    }
}



void DyLayerPostbox::onAcceptClicked(CCObject * obj)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    CCMenuItemImage * pMenuItem = dynamic_cast<CCMenuItemImage*>(obj);
	int index = pMenuItem->getTag();
    
	DyGiftManager* giftManager = DyGiftManager::sharedGiftManager();
    
    m_processGift = giftManager->getGift(index);
    
    if(m_processGift->m_is_main)
    {
        DyPopup* popup = lib->show_network_alaram(true);
        popup->setText(STRING_TITLE_UPDATE_DATA);
        
        lib->network_present_check(m_processGift, httpresponse_selector(DyLayerPostbox::onHttpRequestCompleted));
    }
    else
        lib->network_kakao_accept_message(DyLayerPostbox_callfunc_accept_message, m_processGift->m_gift_id.c_str());

}


void DyLayerPostbox::callfunc_load_accept_allmessages_success(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerPostbox *layerPostbox = (DyLayerPostbox*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPOSTBOX_TAG);
    
    layerPostbox->unschedule(schedule_selector(DyLayerPostbox::callfunc_load_accept_allmessages_success));
    
    //lib->show_network_alaram(false);
    
    if(lib->m_main_server_error)
    {
        DyGiftManager* giftManager = DyGiftManager::sharedGiftManager();
        giftManager->accept_all();
        
        layerPostbox->refresh();
        
        lib->network_kakao_update_me(0, DyLayerPostbox_callfunc_update_me);
    }
    else
        lib->network_present_check_all(httpresponse_selector(DyLayerPostbox::onHttpRequestCompleted));
}

  

void DyLayerPostbox::callfunc_load_accept_allmessages_fail(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerPostbox *layerPostbox = (DyLayerPostbox*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPOSTBOX_TAG);
    
    layerPostbox->unschedule(schedule_selector(DyLayerPostbox::callfunc_load_accept_allmessages_fail));
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLib::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}



void DyLayerPostbox_callfunc_accept_allmessages(int value, int code)
{
    DyLayerPostbox *layerPostbox = (DyLayerPostbox*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPOSTBOX_TAG);
    
    if(value == 0)
    {
        layerPostbox->schedule(schedule_selector(DyLayerPostbox::callfunc_load_accept_allmessages_success), 0.05f);
    }
    else
    {
        layerPostbox->schedule(schedule_selector(DyLayerPostbox::callfunc_load_accept_allmessages_fail), 0.05f);
    }
}



void DyLayerPostbox::onAcceptallClicked(cocos2d::CCObject * pSender)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    
    lib->network_kakao_accept_allmessages(DyLayerPostbox_callfunc_accept_allmessages);
}


void DyLayerPostbox::scrollViewDidScroll(CCScrollView* view)
{
    //DyLib* lib = DyLib::sharedLib();
    
    CCPoint posOffset = view->getContentOffset();
    CCSize sizeView = view->getViewSize();
    
    DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
    
    for(int offsetY = posOffset.y;offsetY < posOffset.y + sizeView.height + m_sizeItem.height;offsetY += m_sizeItem.height)
    {
        float offsetStart = m_sizeItem.height*m_itemCount - sizeView.height;
        
        int index = (offsetStart + offsetY)/m_sizeItem.height;
        
        index -= DyNoticeManager::sharedNoticeManager()->getCount();
        
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

void DyLayerPostbox::scrollViewDidZoom(CCScrollView* view)
{
}


void DyLayerPostbox::scrollViewTouchContent(CCScrollView* view, CCPoint position)
{
}


void DyLayerPostbox::pause()
{
	m_menu->setEnabled(false);
	m_menuRecieve->setEnabled(false);
	m_layerMoney->pause();
	m_layerContents->setTouchEnabled(false);
    setKeypadEnabled(false);
}


void DyLayerPostbox::resume()
{
	m_menu->setEnabled(true);
	m_menuRecieve->setEnabled(true);
	m_layerMoney->resume();
	m_layerContents->setTouchEnabled(true);
    setKeypadEnabled(true);
}


void DyLayerPostbox::destory()
{
	//DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
	//moneyManager->removeGumLayer(getChildByTag(SHOP_UI_TAG_GUM_BG));
	YtLayer::destory();
}


void DyLayerPostbox::hide()
{
	YtLib* lib = YtLib::sharedLib();

	runAction(CCSequence::create(

		CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)), 
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2)),

		CCCallFunc::create(this, callfunc_selector(DyLayerPostbox::destory)),
		NULL));
}


void DyLayerPostbox::show()
{
	YtLib* lib = YtLib::sharedLib();

	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));

	runAction(CCSequence::create(

		CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
		NULL));
}

void DyLayerPostbox::keyBackClicked()
{
    if(DyLib::sharedLib()->m_showLoadingPopup)
        return;

    setKeypadEnabled(false);
    onCloseClicked(NULL);
}


void DyLayerPostbox::onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response)
{
    DyLib* lib = DyLib::sharedLib();
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyLayerPostbox *layerPostbox = (DyLayerPostbox*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERPOSTBOX_TAG);
    
//    if(lib->network_checkError(response, true) == false)
//        return;
    
    if(lib->network_getTag(response).compare(NETWORK_SERVICE_PRESENT_CHECK_TAG) == 0)
    {
        if(lib->network_checkError(response, true))
        {
            lib->network_present_check_ret(sender, response);
        
            layerPostbox->callfunc_load_accept_message_success(0);
        }
        else
            layerPostbox->callfunc_load_accept_message_fail(0);
    }
    
    else if(lib->network_getTag(response).compare(NETWORK_SERVICE_PRESENT_CHECK_ALL_TAG) == 0)
    {
        if(lib->network_checkError(response, true))
            lib->network_present_check_all_ret(sender, response);
        
        DyGiftManager* giftManager = DyGiftManager::sharedGiftManager();
        giftManager->accept_all();
        
        layerPostbox->refresh();
        
        lib->network_kakao_update_me(0, DyLayerPostbox_callfunc_update_me);
    }
}


DyLayerPostbox* DyLayerPostbox::create(CCNode* owner)
{
	DyLayerPostbox* layerPostbox = (DyLayerPostbox*)YtLayerManager::create("DyLayerPostbox", DyLayerPostboxLoader::loader(), "layer_postbox.ccbi", owner);
	return layerPostbox;
}