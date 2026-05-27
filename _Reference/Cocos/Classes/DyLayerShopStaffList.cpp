/*
 DyLayerShopStaffList.h
 Doggy chef
 @initialize at 140328
 
 @history
 140328	yoonsr	initialize
 */


#include "DyConfig.h"
#include "DyLayerShopStaffList.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyBlockInfo.h"
#include "DyPopup.h"
#include "DyStaff.h"
#include "DyLayerShop.h"


DyLayerShopStaffList::DyLayerShopStaffList()
{
    m_menu = NULL;
    setTouchEnabled(true);
    //m_selectedIndex = -1;
    m_startIndex = 0;
}

DyLayerShopStaffList::~DyLayerShopStaffList()
{
}

void DyLayerShopStaffList::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader)
{
}

SEL_MenuHandler DyLayerShopStaffList::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName)
{
    return NULL;    
}

SEL_CCControlHandler DyLayerShopStaffList::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName)
{
	return NULL;
}

bool DyLayerShopStaffList::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode)
{
    return false;
}


void DyLayerShopStaffList::pause()
{
    if(m_layerShopStaff->isVisible())
    {
        m_layerShopStaff->pause();
    }
    else
    {
        if(m_menu)
            m_menu->setEnabled(false);
        m_scrollView->setTouchEnabled(false);
    }
}


void DyLayerShopStaffList::resume()
{
    if(m_layerShopStaff->isVisible())
    {
        m_layerShopStaff->resume();
    }
    else
    {
        if(m_menu)
            m_menu->setEnabled(true);
        
        m_scrollView->setTouchEnabled(true);
    }
}


void DyLayerShopStaffList::hide()
{
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}

void DyLayerShopStaffList::show()
{
    
}


void DyLayerShopStaffList::resetLayer()
{
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    m_layerContent->removeAllChildrenWithCleanup(true);

    m_menu = CCMenu::create();
    m_menu->setPosition(CCPointZero);
    
    for(int i = 0;i < STAFF_CNT;i++)
    {
        int offset_x = 500 + i*380;
        
        int offset_y = 200;
        
        m_sprStaffBg[i] = CCScale9Sprite::createWithSpriteFrame(CCSprite::createWithSpriteFrameName("menushop_ui_box_00.png")->displayFrame());
        m_sprStaffBg[i]->setContentSize(CCSizeMake(248, 368-10));
        m_sprStaffBg[i]->setAnchorPoint(ccp(0.5, 0.5));
        m_sprStaffBg[i]->setPosition(ccp(offset_x-294.0, offset_y+19+5));
        m_layerContent->addChild(m_sprStaffBg[i]);
        
        CCSprite* sprLabelBg2 = CCSprite::createWithSpriteFrameName("menushop_ui_recipe_title.png");
        sprLabelBg2->setPosition(ccp(offset_x-298.0, offset_y+194));
        m_layerContent->addChild(sprLabelBg2);
        
        CCLabelTTF* labelName = CCLabelTTF::create(staffManager->m_staff_list->getValue("name", i), FONT_NAME_EXP, 23);
        labelName->setPosition(ccp(offset_x-299.0, offset_y+199));
        labelName->setColor(ccc3(84,35,17));
        m_layerContent->addChild(labelName);
        
        CCLabelTTF* labelPictureDesc = CCLabelTTF::create(staffManager->isBuyable(i) ? staffManager->m_staff_list->getValue("explain", i) :  staffManager->m_staff_list->getValue("condition_explain", i), FONT_NAME_EXP, 18, CCSizeMake(240,100), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
        labelPictureDesc->setPosition(ccp(offset_x-297.0, offset_y-116+10));
        labelPictureDesc->setColor(ccc3(51,51,51));
        m_layerContent->addChild(labelPictureDesc);
        
        CCSprite* sprShadow = CCSprite::createWithSpriteFrameName("menu2_ui_cashier_shadow.png");
        sprShadow->setPosition(ccp(offset_x-299.0, offset_y-54+10));
        m_layerContent->addChild(sprShadow);
        
        CCSprite* sprPicture = staffManager->getImage(i);
        sprPicture->setPosition(ccp(offset_x-300.0, offset_y-54+10));
        sprPicture->setAnchorPoint(ccp(0.5, 0));
        m_layerContent->addChild(sprPicture);
        
        if(i < STAFF_CNT - 1)
        {
            CCSprite* sprLine = CCSprite::createWithSpriteFrameName("menushop_ui_vertical_line.png");
            sprLine->setPosition(ccp(offset_x-156.0+50, offset_y+19));
            m_layerContent->addChild(sprLine);
        }
        
        
//        if(m_selectedIndex == i)
//        {
//            CCSprite* sprCheck = CCSprite::createWithSpriteFrameName("menushop_ui_select.png");
//            sprCheck->setPosition(ccp(offset_x-300.0+100, offset_y+150));
//            m_layerContent->addChild(sprCheck);
//        }
        
        
        if(i > 0 && staffManager->isBuyable(i) && !staffManager->isAvailable(i))
        {
            DyLib* lib = DyLib::sharedLib();
            
            int buy_delly = atoi(staffManager->m_staff_list->getValue("buy_delly", i));
            
            int buy_gold = atoi(staffManager->m_staff_list->getValue("buy_gold", i));
            
            
            if(buy_gold > 0)
            {
                YtLib::setMoneyStyle(lib->m_strTemp, buy_gold);
                CCLabelTTF* labelPurchasePrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 20.f);
                labelPurchasePrice->setColor(ccBLACK);
                m_layerContent->addChild(labelPurchasePrice);
                
                CCSprite* sprPurchasePriceMark = CCSprite::createWithSpriteFrameName("menu_ui_gold.png");
                sprPurchasePriceMark->setScale(0.6f);
                m_layerContent->addChild(sprPurchasePriceMark);
                
                sprPurchasePriceMark->setPosition(ccp(sprPurchasePriceMark->getPosition().x, offset_y-150+10));
                labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, offset_y-150+10));
                YtLib::arrangeNodesH(ccp(offset_x-299.0, offset_y-150+10), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
            }
            
            if(buy_delly > 0)
            {
                YtLib::setMoneyStyle(lib->m_strTemp, buy_delly);
                CCLabelTTF* labelPurchasePrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 20.f);
                labelPurchasePrice->setColor(ccBLACK);
                m_layerContent->addChild(labelPurchasePrice);
                
                CCSprite* sprPurchasePriceMark = CCSprite::createWithSpriteFrameName("menu_ui_delly.png");
                sprPurchasePriceMark->setScale(0.6f);
                m_layerContent->addChild(sprPurchasePriceMark);
                
                sprPurchasePriceMark->setPosition(ccp(sprPurchasePriceMark->getPosition().x, offset_y-150));
                labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, offset_y-150));
                YtLib::arrangeNodesH(ccp(offset_x-299.0, offset_y-150), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
            }

            CCMenuItemSprite *menuitemBuy = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("menushop_ui_btn_buy.png"),
                                                                     CCSprite::createWithSpriteFrameName("menushop_ui_btn_buy_sel.png"),
                                                                     CCSprite::createWithSpriteFrameName("menushop_ui_btn_buy.png"),
                                                                     this, menu_selector(DyLayerShopStaffList::onBuyClicked));
            
			menuitemBuy->setPosition(ccp(offset_x-299.0, offset_y-185+10));
			menuitemBuy->setTag(i);
            
            m_menu->addChild(menuitemBuy);
        }
    }
    m_layerContent->addChild(m_menu);
}


void DyLayerShopStaffList::onBuyClicked(CCObject * obj)
{
    DyLib* lib = DyLib::sharedLib();
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    CCMenuItemSprite *menuitemBuy = dynamic_cast<CCMenuItemSprite*>(obj);
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    int buy_delly = atoi(staffManager->m_staff_list->getValue("buy_delly", menuitemBuy->getTag()));
    
    int buy_gold = atoi(staffManager->m_staff_list->getValue("buy_gold", menuitemBuy->getTag()));

    if(buy_delly > 0)
        sprintf(lib->m_strTemp, "%d델리가 필요합니다. 채용할까요?", buy_delly);
    else
        sprintf(lib->m_strTemp, "%d골드가 필요합니다. 채용할까요?", buy_gold);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShopStaffList::onPopupClicked));
    
    popup->setText(lib->m_strTemp);
    
    popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
    popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 1);
    popup->addButton(DyButtonType_Close, "", 2);
    
    popup->setTag(menuitemBuy->getTag());
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}



void DyLayerShopStaffList::onPopupClicked(cocos2d::CCObject * pSender)
{
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();

    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    int buy_delly = atoi(staffManager->m_staff_list->getValue("buy_delly", pPopup->getTag()));
    int buy_gold = atoi(staffManager->m_staff_list->getValue("buy_gold", pPopup->getTag()));

    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
    
    if(pPopup->m_active_tag == 1)
	{
        if(buy_delly > 0 && buy_delly > moneyManager->getDelly())
            DyMoneyManager::sharedMoneyManager()->alramNotEnoughDelly(true, true);
        else if(buy_gold > 0 && buy_gold > moneyManager->getGold())
            DyMoneyManager::sharedMoneyManager()->alramNotEnoughGold(true, true);
        else
        {
            scout(pPopup->getTag());
        }
 	}
}



void DyLayerShopStaffList::callfunc_update_me_success(float dt)
{
    unschedule(schedule_selector(DyLayerShopStaffList::callfunc_update_me_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
}

void DyLayerShopStaffList::callfunc_update_me_fail(float dt)
{
    unschedule(schedule_selector(DyLayerShopStaffList::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShopStaffList::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}


void DyLayerShopStaffList_callfunc_update_me(int value, int code)
{
    DyLayerShop *layerShop = (DyLayerShop*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERSHOP_TAG);
    
    if(value == 0)
    {
        layerShop->schedule(schedule_selector(DyLayerShopStaffList::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layerShop->schedule(schedule_selector(DyLayerShopStaffList::callfunc_update_me_fail), 0.05f);
    }
}



void DyLayerShopStaffList::scout(int index)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_SHOWPICTURE);
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    int buy_delly = atoi(staffManager->m_staff_list->getValue("buy_delly", index));
    int buy_gold = atoi(staffManager->m_staff_list->getValue("buy_gold", index));

    if(buy_delly)
        moneyManager->addDelly(-buy_delly, true);
    
    if(buy_gold)
        moneyManager->addGold(-buy_gold, true);
    
    dataManager->m_private.m_staff_ext_grade[index-1] = 0;
    dataManager->m_private.m_staff_ext_next_grade_remain[index-1] = atoi(staffManager->m_staff[index]->getValue("conditon", 1));
    
    resetLayer();
    
    dataManager->dataSave();
    
    DyLib::sharedLib()->network_kakao_update_me(0, DyLayerShopStaffList_callfunc_update_me);
    
    
}



void DyLayerShopStaffList::onEnter()
{
	CCLayer::onEnter();

//    DyDataManager* dataManager = DyDataManager::sharedDataManager();
//    
//    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
	if(m_isInit)
    {
		return;
    }
	m_isInit = true;
    
//    if(staffManager->isAvailable(dataManager->m_private.m_current_staff))
//    {
//        m_selectedIndex = dataManager->m_private.m_current_staff;
//    }
    
    m_layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), 830, 420);
    
    resetLayer();
    
    
    m_scrollView = CCScrollView::create(m_layerContent->getContentSize(), m_layerContent);
    m_scrollView->setDelegate(this);
    
    m_scrollView->setContentSize(m_layerContent->getContentSize());
    m_scrollView->setDirection(kCCScrollViewDirectionHorizontal);
    m_scrollView->setContentOffset(ccp(0,0));
    m_scrollView->setGlobalScale(YtLib::sharedLib()->m_scale);
    m_scrollView->setScrollEnable(false);
	 
    m_scrollView->setPosition(ccp(30, 30));
    
    addChild(m_scrollView);
    
    m_layerShopStaff = (DyLayerShopStaff*)YtLayerManager::create("DyLayerShopStaff", DyLayerShopStaffLoader::loader(), "layer_shop_staff.ccbi", this);
    m_layerShopStaff->m_curStaff = 0;
    addChild(m_layerShopStaff);
    m_layerShopStaff->setVisible(false);
    m_layerShopStaff->pause();
    
    
    if(m_startIndex > 0)
    {
        enterStaff(m_startIndex - 1);
    }
        
}


void DyLayerShopStaffList::onExit()
{
	CCLayer::onExit();
}


void DyLayerShopStaffList::scrollViewDidScroll(CCScrollView* view)
{
    
}

void DyLayerShopStaffList::scrollViewDidZoom(CCScrollView* view)
{
    
}


void DyLayerShopStaffList::scrollViewTouchContent(CCScrollView* view, CCPoint position)
{
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
	int select_index = -1;
    
    for(int i = 0;i < STAFF_CNT;i++)
    {
        if(m_sprStaffBg[i]->boundingBox().containsPoint(position))
            select_index = i;
    }

	if(select_index >= 0)
    {
        if(staffManager->isAvailable(select_index))
        {
            YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
            
            enterStaff(select_index);
            
        }
    }
}


void DyLayerShopStaffList::enterStaff(int index)
{
    pause();
    
    m_layerShopStaff->m_curStaff = index;
    //staffManager->setCurrentStaff(select_index);
    
    m_layerShopStaff->setVisible(true);
    m_layerShopStaff->resume();
    m_layerShopStaff->refresh();
}


void DyLayerShopStaffList::showList()
{
    m_layerShopStaff->setVisible(false);
    m_layerShopStaff->pause();
    resume();
    resetLayer();
}
