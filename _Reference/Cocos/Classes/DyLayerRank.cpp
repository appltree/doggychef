/*
DyLayerRank.cpp
Doggy chef
@initialize at 130104

@history
130104	yoonsr	initialize
*/


#include "DyLayerRank.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyFriend.h"
#include "DyItem.h"
#include "DyLayerShop.h"
#include "DyPopup.h"
#include "DyRank.h"
#include "DySceneMainMenu.h"
#include "DyNetwork.h"

DyLayerRank::DyLayerRank()
{
	this->initWithColor(ccc4(255, 0, 0, 0), 450, 410);
    m_layer_rank_list_story = NULL;
    m_layer_rank_list_crazy = NULL;
}


DyLayerRank::~DyLayerRank()
{
    if(m_layer_rank_list_story)
        m_layer_rank_list_story->release();
    if(m_layer_rank_list_crazy)
        m_layer_rank_list_crazy->release();
}



void DyLayerRank::displayModeType()
{
    DyLib* lib = DyLib::sharedLib();
    
    bool isStage = m_status_friend == 0;

    if(isStage)
        m_layer_rank_list_story->setRankLayer(DyLayerRankListType_Friend, DyRankType_Stage, NULL);
    else
        m_layer_rank_list_crazy->setRankLayer(DyLayerRankListType_Friend, DyRankType_Crazy, NULL);

    lib->show_network_alaram(false);
    
    CCMenuItemImage* menuitem_stage = (CCMenuItemImage*)m_menu->getChildByTag(0);
    CCMenuItemImage* menuitem_crazy = (CCMenuItemImage*)m_menu->getChildByTag(1);
    
    if(isStage)
    {
        menuitem_stage->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("menu2_ui_button_stage_on.png")->displayFrame());
        menuitem_crazy->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("menu2_ui_button_crazy_off.png")->displayFrame());
        m_labelRemainTime->setVisible(false);
    }
    else
    {
        menuitem_stage->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("menu2_ui_button_stage_off.png")->displayFrame());
        menuitem_crazy->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("menu2_ui_button_crazy_on.png")->displayFrame());
        m_labelRemainTime->setVisible(true);
    }
    
    if(m_layer_rank_list_story)
        m_layer_rank_list_story->setVisible(isStage);
    if(m_layer_rank_list_crazy)
        m_layer_rank_list_crazy->setVisible(!isStage);
}


void DyLayerRank::loadGlobalRankList(DyRankType rankType)
{
    m_status_friend = rankType;
    
    if(m_status_friend == DyRankType_Stage)
        m_layer_rank_list_story->setRankGlobalLayer(DyRankType_Stage);
    else
        m_layer_rank_list_crazy->setRankGlobalLayer(DyRankType_Crazy);
    
    
    CCMenuItemImage* menuitem_stage = (CCMenuItemImage*)m_menu->getChildByTag(0);
    CCMenuItemImage* menuitem_crazy = (CCMenuItemImage*)m_menu->getChildByTag(1);
    
    m_labelRemainTime->setVisible(false);
    
    if(m_status_friend == DyRankType_Stage)
    {
        menuitem_stage->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("menu2_ui_button_stage_on.png")->displayFrame());
        menuitem_crazy->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("menu2_ui_button_crazy_off.png")->displayFrame());
        
    }
    else
    {
        menuitem_stage->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("menu2_ui_button_stage_off.png")->displayFrame());
        menuitem_crazy->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("menu2_ui_button_crazy_on.png")->displayFrame());
    }
    
    if(m_layer_rank_list_story)
        m_layer_rank_list_story->setVisible(m_status_friend == DyRankType_Stage);
    if(m_layer_rank_list_crazy)
        m_layer_rank_list_crazy->setVisible(m_status_friend == DyRankType_Crazy);
}


void DyLayerRank::callfunc_load_leader_board_success(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerMainMenu* layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layerMainMenu->unschedule(schedule_selector(DyLayerRank::callfunc_load_leader_board_success));
    
    DyRankManager::sharedRankManager()->loadLeaderBoard(layerMainMenu->m_layerRank->m_tmpType, lib->m_buffer);
    
    layerMainMenu->m_layerRank->m_status_friend = layerMainMenu->m_layerRank->m_tmpType;
    layerMainMenu->m_layerRank->displayModeType();
}


void DyLayerRank::callfunc_load_leader_board_fail(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerMainMenu* layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layerMainMenu->unschedule(schedule_selector(DyLayerRank::callfunc_load_leader_board_fail));
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLib::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}



void DyLayerRank_callfunc_load_leader_boards(void* value, int code)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyLayerMainMenu* layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    if(value != NULL)
    {
        lib->m_buffer = (std::string)(char*)value;
        
        layerMainMenu->schedule(schedule_selector(DyLayerRank::callfunc_load_leader_board_success), 0.05f);
    }
    else
    {
        layerMainMenu->schedule(schedule_selector(DyLayerRank::callfunc_load_leader_board_fail), 0.05f);
    }
}



void DyLayerRank_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str)
{
    //DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
    DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    DyLib::sharedLib()->show_network_alaram(false);
    
    if(value_int != 0)
    {
        return;
    }
    switch(cmdType)
    {
        case DyNetworkCmdtype_TotalScoreGet:
        {
            DyRankManager* rankManager = DyRankManager::sharedRankManager();
            
            DyRankType type = rankManager->loadGlobalRank((char*)value_str);
            
            layerMainMenu->m_layerRankGlobal->loadGlobalRankList(type);
            
        }
            break;
        default:
            break;
    }
}


void DyLayerRank::setModeType(DyRankType type)
{
    DyLib* lib = DyLib::sharedLib();
   
    if(m_layerRankType == DyLayerRankType_Global)
    {
        DyPopup* popup = lib->show_network_alaram(true);
        popup->setText(STRING_TITLE_UPDATE_DATA);
        
        DYNETWORK_ADD_PROC(DyNetworkCmdtype_TotalScoreGet, type == DyRankType_Stage ? "story" : "crazy", NULL, DyLayerRank_NetCallFunc);
    }
    else
    {
        if(type == DyRankType_Invalid)
        {
            m_status_friend = DyRankType_Stage;
            displayModeType();
        }
        else
        {
            m_tmpType = type;
            lib->network_kakao_load_leader_boards(type, DyLayerRank_callfunc_load_leader_boards);
        }
    }
}


void DyLayerRank::cbCrazyModeRefresh(float dt)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    m_labelRemainTime->setString(dataManager->getRefreshRemainDate().c_str());

}

void DyLayerRank::onTabClicked(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    CCMenuItemImage *pMenuItem = dynamic_cast<CCMenuItemImage*>(obj);
    int tag = pMenuItem->getTag();

    switch(tag)
    {
        case 0:
            setModeType(DyRankType_Stage);
            break;
        case 1:
            setModeType(DyRankType_Crazy);
            break;
    }
}


void DyLayerRank::onEnter()
{
	CCLayer::onEnter();

	if(m_isInit)
		return;
	m_isInit = true;
    
	CCSprite* sprBg = CCSprite::create("images_ranking_bg.png");

	addChild(sprBg);
	
	CCSize sizeScrollView = CCSizeMake(395, 285);
    
    CCRect rectScroll = CCRectMake(-sizeScrollView.width/2, -sizeScrollView.height/2-10-25-3+10+3-5, sizeScrollView.width, sizeScrollView.height);
    
    
    m_layer_rank_list_story = new DyLayerRank_list(rectScroll);
    addChild(m_layer_rank_list_story);
    
    m_layer_rank_list_crazy = new DyLayerRank_list(rectScroll);
    
    addChild(m_layer_rank_list_crazy);
    m_layer_rank_list_crazy->setVisible(false);

    CCSprite* sprBgFront = CCSprite::create("images_ranking_front.png");
    addChild(sprBgFront);
    sprBgFront->setPosition(ccp(0, 140));
    
        
	m_menu = CCMenu::create();
    m_menu->setPosition(CCPointZero);
    
    CCMenuItemImage* menuitem_stage = CCMenuItemImage::create();
    menuitem_stage->initWithNormalSprite(CCSprite::createWithSpriteFrameName("menu2_ui_button_stage_on.png"),
                                   NULL,
                                   NULL, this, menu_selector(DyLayerRank::onTabClicked));
    menuitem_stage->setAnchorPoint(ccp(0, 0));
    
    menuitem_stage->setTag(0);
    menuitem_stage->setPosition(ccp(-185, 122));
    m_menu->addChild(menuitem_stage);
  
    
    CCMenuItemImage* menuitem_crazy = CCMenuItemImage::create();
    menuitem_crazy->initWithNormalSprite(CCSprite::createWithSpriteFrameName("menu2_ui_button_crazy_on.png"),
                                   NULL,
                                   NULL, this, menu_selector(DyLayerRank::onTabClicked));
    menuitem_crazy->setAnchorPoint(ccp(1, 0));
    
    menuitem_crazy->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("menu2_ui_button_crazy_off.png")->displayFrame());
    menuitem_crazy->setTag(1);
    menuitem_crazy->setPosition(ccp(185, 122));
    m_menu->addChild(menuitem_crazy);
    
    addChild(m_menu);
    
    m_labelRemainTime = CCLabelTTF::create("", FONT_NAME_EXP, 15);
    m_labelRemainTime->setAnchorPoint(ccp(1,0));
    m_labelRemainTime->setPosition(ccp(170, 160));
    m_labelRemainTime->setColor(ccc3(109, 65, 36));
    addChild(m_labelRemainTime);
    
    m_status_friend = DyRankType_Invalid;
    
    
    if(m_layerRankType == DyLayerRankType_Friend)
        setModeType(m_status_friend);
 
    
	resume();
    
    cbCrazyModeRefresh(0);
    schedule(schedule_selector(DyLayerRank::cbCrazyModeRefresh), 1.0f);
}


void DyLayerRank::onExit()
{
	CCLayer::onExit();
}

void DyLayerRank::pause()
{
    m_menu->setEnabled(false);
    
    if(m_layer_rank_list_story)
        m_layer_rank_list_story->pause();
    if(m_layer_rank_list_crazy)
        m_layer_rank_list_crazy->pause();
}


void DyLayerRank::resume()
{
    m_menu->setEnabled(true);
    
    if(m_layer_rank_list_story)
        m_layer_rank_list_story->resume();
    if(m_layer_rank_list_crazy)
        m_layer_rank_list_crazy->resume();
}


void DyLayerRank::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerRank::show()
{
}


DyLayerRank* DyLayerRank::create(DyLayerRankType type)
{
	DyLayerRank* layerRank = new DyLayerRank();
    layerRank->m_layerRankType = type;
    
	layerRank->autorelease();
	return layerRank;
}
