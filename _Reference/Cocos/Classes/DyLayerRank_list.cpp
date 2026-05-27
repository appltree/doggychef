/*
DyLayerRank_list.cpp
Doggy chef
@initialize at 130710

@history
130710	yoonsr	initialize
*/


#include "DyLayerRank_list.h"
#include "DyFriend.h"
#include "DyLib.h"
#include "DyCostume.h"

#include "DyLayerRankDetail.h"

#include "DyLayerInvite.h"
#include "DyLayerPresent.h"
#include "DyPopup.h"
#include "DyRank.h"
#include "DyData.h"
#include "DyNetwork.h"
#include "DyStage.h"
#include "DyTycoon.h"
#include "json.h"


void DyLayerRank_list_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str);

DyLayerRank_list* g_layerRank_list;


DyLayerRank_list::DyLayerRank_list(CCRect rect)
{
    m_arrPictureFrame = new CCArray();
    m_arrAppFriend = new CCArray();
    m_arrButtons = new CCArray();
    m_arrRankInList = new CCArray();
    
    m_rect = rect;

    m_menuList = NULL;
    m_scrollView = NULL;
    
    m_doggy = NULL;
    m_isVisible = true;
    m_arrRank = NULL;
}


void DyLayerRank_list::setRankLayer(DyLayerRankListType listType, DyRankType rankType, CCArray* arrRank)
{
    removeAllChildrenWithCleanup(true);
    m_arrAppFriend->removeAllObjects();
    m_arrPictureFrame->removeAllObjects();
    m_arrButtons->removeAllObjects();
    m_arrRankInList->removeAllObjects();
    
    m_listType = listType;
    m_rankType = rankType;
    
    
    DyLib* lib = DyLib::sharedLib();
    
    m_arrRank = arrRank;
    CCArray* rankarray;
    if(arrRank)
        rankarray = arrRank;
    else
        rankarray = DyRankManager::sharedRankManager()->getArray(m_rankType);
    
    m_sizeItem = CCSizeMake(m_rect.size.width, 65);
    
    int top_gap;
    
    m_itemCountForHeight = 0;
    if(m_listType == DyLayerRankListType_SubStage)
    {
        top_gap = 0;
        m_itemCountForHeight = m_itemCount = rankarray->count();
    }
    else if(m_listType == DyLayerRankListType_Pride)
    {
        top_gap = -4;
        m_itemCount = rankarray->count();
        
        for(int i = 0;i < rankarray->count();i++)
        {
            DyFriend* _friend = getUser(i);
            if(_friend->m_isDummy)
                continue;
            
            if(_friend->m_friendType == DyFriendType_Mine)
                continue;

            m_itemCountForHeight++;
        }
    }
    else
    {
        top_gap = -4;
        m_itemCountForHeight = m_itemCount = rankarray->count() + 1;
    }
    
    CCLayerColor* layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), m_sizeItem.width, m_sizeItem.height*(m_itemCountForHeight)-top_gap);
    
    m_menuList = CCMenu::create();
    m_menuList->setPosition(CCPointZero);
    
    int index = 0;
    for(int i = 0;i < m_itemCount;i++)
    {
        float posY = top_gap + m_sizeItem.height*m_itemCountForHeight - (index+1)*m_sizeItem.height + m_sizeItem.height/2;
        
        if(i < (int)rankarray->count())
        {
            DyFriend* _friend = getUser(i);
            int order;
            long long point;
            
            DyStageRank* rankStage;
            DyRank* rank;
            
            if(m_listType == DyLayerRankListType_SubStage)
            {
                rankStage = (DyStageRank*)rankarray->objectAtIndex(i);
                order = rankStage->m_order;
                point = rankStage->m_score;
            
            }
            else
            {
                rank = (DyRank*)rankarray->objectAtIndex(i);
                order = rank->m_order;
                point = rank->m_point;
            }            
            
            if(_friend->m_friendType == DyFriendType_Mine)
            {
                //m_myIndex = i;
                if(m_listType == DyLayerRankListType_Pride)
                    continue;
                
                CCScale9Sprite *background = CCScale9Sprite::createWithSpriteFrame(CCSprite::createWithSpriteFrameName("main_ui_rank_cursor.png")->displayFrame());
                background->setContentSize(CCSizeMake(m_sizeItem.width-15, m_sizeItem.height-4));
                background->setPosition(ccp(m_sizeItem.width/2, posY));
                
                layerContent->addChild(background);
            }
            
            if(m_listType == DyLayerRankListType_Pride && _friend->m_isDummy)
            {
                continue;
            }

            
            m_arrAppFriend->addObject(_friend);
            
            if(m_listType == DyLayerRankListType_SubStage)
                m_arrRankInList->addObject(rankStage);
            else
                m_arrRankInList->addObject(rank);

            if(m_listType == DyLayerRankListType_Friend || m_listType == DyLayerRankListType_SubStage)
            {
                CCSprite* sprNumberBg;
                
                sprintf(lib->m_strTemp, "%d", order);
                
                CCLabelTTF* labelNumber = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 25.f);
                if(order == 1)
                {
                    sprNumberBg = CCSprite::createWithSpriteFrameName("main_ui_rankmark_0.png");
                    labelNumber->setColor(ccc3(57,27,5));
                    labelNumber->setPosition(ccp(35, posY+2));
                }
                else if(order == 2)
                {
                    sprNumberBg = CCSprite::createWithSpriteFrameName("main_ui_rankmark_1.png");
                    labelNumber->setColor(ccc3(57,27,5));
                    labelNumber->setPosition(ccp(35, posY+2));
                }
                else if(order == 3)
                {
                    sprNumberBg = CCSprite::createWithSpriteFrameName("main_ui_rankmark_2.png");
                    labelNumber->setColor(ccc3(57,27,5));
                    labelNumber->setPosition(ccp(35, posY+2));
                }
                else
                {
                    sprNumberBg = CCSprite::createWithSpriteFrameName("main_ui_rank_number_bg.png");
                    labelNumber->setColor(ccWHITE);
                    labelNumber->setPosition(ccp(35, posY));
                }
                
                if(order > 99)
                {
                    labelNumber->setFontSize(18.f);
                }
                
                sprNumberBg->setPosition(ccp(35, posY));
                layerContent->addChild(sprNumberBg);
                layerContent->addChild(labelNumber);
            }
            
            
            //
            // 사진
            int offset_picture_x;
            
            if(m_listType == DyLayerRankListType_Friend || m_listType == DyLayerRankListType_SubStage)
            {
                offset_picture_x = 35+55;
            }
            else
            {
                offset_picture_x = 35;
            }
            
            
            CCSprite* sprPictureFrame = CCSprite::createWithSpriteFrameName("main_ui_rank_picture_frame.png");
            sprPictureFrame->setPosition(ccp(offset_picture_x, posY));
            layerContent->addChild(sprPictureFrame);
            m_arrPictureFrame->addObject(sprPictureFrame);
            
            //
            // 코스튬
            int offset_costume_x;
            
            if(m_listType == DyLayerRankListType_Friend || m_listType == DyLayerRankListType_SubStage)
            {
                offset_costume_x = 35+110;
            }
            else
            {
                offset_costume_x = 35+50;
            }

            YtAniObject* doggy = YtAniObject::loadAniObject(this, "node_animal_doggy.ccbi");
            doggy->setPosition(ccp(offset_costume_x, posY -m_sizeItem.height/2 + 3/* + sizeItem.height/2*/));
            doggy->setScale(0.22f);
            layerContent->addChild(doggy);
            if(_friend->m_friendType == DyFriendType_Mine)
            {
                m_doggy = doggy;
                DyCostumeManager::sharedCostumeManager()->setCostume(m_doggy, &DyDataManager::sharedDataManager()->m_public.m_costume_status, &DyDataManager::sharedDataManager()->m_custume_function);
            }
            else
            {
                DyCostumeManager::sharedCostumeManager()->setCostume(doggy, &_friend->m_public.m_costume_status, &_friend->m_costume_function);
            }
            
            CCLabelTTF* labelName;
            
            if(m_listType == DyLayerRankListType_Friend || m_listType == DyLayerRankListType_SubStage)
            {
                labelName = CCLabelTTF::create(_friend->m_nickname.c_str(), FONT_NAME_EXP, 22.f, CCSizeMake(150, 26), kCCTextAlignmentLeft);
                labelName->setPosition(ccp(35+150, posY + 15));
                labelName->setColor(ccc3(57,27,5));
            }
            else // DyLayerRankListType_Pride
            {
                labelName = CCLabelTTF::create(_friend->m_nickname.c_str(), FONT_NAME_EXP, 28.f, CCSizeMake(180, 35), kCCTextAlignmentLeft);
                labelName->setPosition(ccp(35+80, posY));
                labelName->setColor(ccc3(255,221,36));
                
                
//                if(_friend->m_isDummy)
//                {
//                    sprPictureFrame->setVisible(false);
//                    labelName->setVisible(false);
//                }
            }
   
            labelName->setAnchorPoint(ccp(0, 0.5));
            layerContent->addChild(labelName);
            
           
            if(m_listType == DyLayerRankListType_SubStage)
            {
//                int count = point - DyStageState_Fail_Pass;
//                
//                for(int i = 0;i < count;i++)
//                {
//                    CCSprite* sprStar = CCSprite::createWithSpriteFrameName("menu_ui_star_highlight.png");
//                    sprStar->setPosition(ccp(35+150+20 +i*45, posY-14));
//                    layerContent->addChild(sprStar);
//                    
//                }
                
                YtLib::setMoneyStyle(lib->m_strTemp, point);
                
                CCLabelTTF* labelPoint = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 28.f);
                labelPoint->setPosition(ccp(m_sizeItem.width - 80, posY - 13));
                labelPoint->setColor(ccc3(246,96,97));
                labelPoint->setAnchorPoint(ccp(1, 0.5));
                layerContent->addChild(labelPoint);

                
            }
            else if(m_listType == DyLayerRankListType_Friend)
            {
                YtLib::setMoneyStyle(lib->m_strTemp, point);
                
                CCLabelTTF* labelPoint = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 28.f);
                labelPoint->setPosition(ccp(m_sizeItem.width - 80, posY - 13));
                labelPoint->setColor(ccc3(246,96,97));
                labelPoint->setAnchorPoint(ccp(1, 0.5));
                layerContent->addChild(labelPoint);
            }
            
            
            if(m_listType == DyLayerRankListType_Friend || m_listType == DyLayerRankListType_SubStage)
            {
                CCMenuItemImage* menuitem = CCMenuItemImage::create();
                
                
                if(_friend->m_friendType == DyFriendType_Mine)
                {
                    
                    if(DyDataManager::sharedDataManager()->m_message_blocked)
                    {
                        menuitem->initWithNormalSprite(
                                                       CCSprite::createWithSpriteFrameName("main_ui_btn_present_off.png"),
                                                       CCSprite::createWithSpriteFrameName("main_ui_btn_present_off_sel.png"),
                                                       CCSprite::createWithSpriteFrameName("main_ui_btn_present_off.png"), this, menu_selector(DyLayerRank_list::onMessageBlockClicked));
                    }
                    else
                    {
                        menuitem->initWithNormalSprite(
                                                       CCSprite::createWithSpriteFrameName("main_ui_btn_present_on.png"),
                                                       CCSprite::createWithSpriteFrameName("main_ui_btn_present_on_sel.png"),
                                                       CCSprite::createWithSpriteFrameName("main_ui_btn_present_on.png"), this, menu_selector(DyLayerRank_list::onMessageBlockClicked));

                    }
                    
                    
                    m_menuitemMine = menuitem;
                }
                else
                {
                    
                    
                    if(_friend->m_message_blocked)
                    {
                        menuitem->initWithNormalSprite(
                                                       CCSprite::createWithSpriteFrameName("main_ui_btn_present_off.png"),
                                                       CCSprite::createWithSpriteFrameName("main_ui_btn_present_off_sel.png"),
                                                       CCSprite::createWithSpriteFrameName("main_ui_btn_present_off.png"), this, menu_selector(DyLayerRank_list::onPresentClicked));
                    }
                    else
                    {
                        if(DyDataManager::sharedDataManager()->m_gum_send_award == 0)
                        {
                            menuitem->initWithNormalSprite(
                                                       CCSprite::createWithSpriteFrameName("main_ui_btn_present_send.png"),
                                                       CCSprite::createWithSpriteFrameName("main_ui_btn_present_send_sel.png"),
                                                       CCSprite::createWithSpriteFrameName("main_ui_btn_present_send.png"), this, menu_selector(DyLayerRank_list::onPresentClicked));
                        }
                        else
                        {
                            menuitem->initWithNormalSprite(
                                                           CCSprite::createWithSpriteFrameName("main_ui_btn_present_plus.png"),
                                                           CCSprite::createWithSpriteFrameName("main_ui_btn_present_plus_sel.png"),
                                                           CCSprite::createWithSpriteFrameName("main_ui_btn_present_plus.png"), this, menu_selector(DyLayerRank_list::onPresentClicked));
                        }
                    }
                }
                
                menuitem->setTag(i);
                menuitem->setPosition(ccp(m_sizeItem.width - 40, posY));
                m_menuList->addChild(menuitem);
            }
            else
            {
                CCMenuItemImage* menuitem = CCMenuItemImage::create();
                menuitem->initWithNormalSprite(
                                               CCSprite::createWithSpriteFrameName("menu2_ui_button_pride.png"),
                                               CCSprite::createWithSpriteFrameName("menu2_ui_button_pride_touch.png"),
                                               CCSprite::createWithSpriteFrameName("menu2_ui_button_pride_disable.png"), this, menu_selector(DyLayerRank_list::onPrideClicked));
                
                menuitem->setTag(i);
                menuitem->setPosition(ccp(m_sizeItem.width - 100, posY));
                m_menuList->addChild(menuitem);
                
                if(_friend->m_message_blocked || _friend->m_tmpTag == 1)
                {
                    menuitem->setEnabled(false);
                }
                
                if(_friend->m_isDummy)
                    menuitem->setVisible(false);
                
            }
            
            CCSprite* sprButton = YtBlankSprite::spriteWithSizeAndColor(CCSizeMake(m_sizeItem.width-50, m_sizeItem.height), ccRED);
            sprButton->setOpacity(0);
            sprButton->setPosition(ccp((m_sizeItem.width-100)/2 - 25, posY));
            //sprButton->setTag(i);
            layerContent->addChild(sprButton);
            m_arrButtons->addObject(sprButton);
            
        }
        else
        {
            CCSprite* sprPlus = CCSprite::createWithSpriteFrameName("main_ui_rank_more.png");
            sprPlus->setPosition(ccp(35, posY));
            layerContent->addChild(sprPlus);
            
            CCLabelTTF* labelInvite = CCLabelTTF::create(STRING_invite_friend, FONT_NAME_EXP, 20.f,
                                                         CCSizeMake(m_sizeItem.width - 180, m_sizeItem.height-10), kCCTextAlignmentLeft);
            labelInvite->setColor(ccc3(129,83,48));
            labelInvite->setAnchorPoint(ccp(0, 0.5));
            labelInvite->setPosition(ccp(35+30, posY));
            layerContent->addChild(labelInvite);
            
            CCSprite* sprFriendMark = CCSprite::createWithSpriteFrameName("main_ui_icon_friend.png");
            sprFriendMark->setPosition(ccp(m_sizeItem.width - 10, posY));
            sprFriendMark->setAnchorPoint(ccp(1, 0.5));
            layerContent->addChild(sprFriendMark);
            
            CCSprite* sprButton = YtBlankSprite::spriteWithSizeAndColor(m_sizeItem, ccRED);
            sprButton->setOpacity(0);
            sprButton->setPosition(ccp(m_sizeItem.width/2, posY));
            //sprButton->setTag(i);
            layerContent->addChild(sprButton);
            m_arrButtons->addObject(sprButton);
        }
        
        if(index < m_itemCountForHeight - 1)
        {
            CCSprite* sprLine = CCSprite::createWithSpriteFrameName("main_ui_ranking_line.png");
            sprLine->setPosition(ccp(m_sizeItem.width/2-5, posY - m_sizeItem.height/2));
            layerContent->addChild(sprLine);
            
            if(m_listType == DyLayerRankListType_Pride)
                sprLine->setScaleX(1.3);
        }
        index++;
    }
    
    layerContent->addChild(m_menuList);
    
    m_scrollView = CCScrollView::create(m_rect.size, layerContent);
	m_scrollView->setDelegate(this);
    
	m_scrollView->setContentSize(layerContent->getContentSize());
	m_scrollView->setDirection(kCCScrollViewDirectionVertical);
	m_scrollView->setContentOffset(m_scrollView->minContainerOffset());
	m_scrollView->setGlobalScale(YtLib::sharedLib()->m_scale);
	m_scrollView->setPosition(m_rect.origin);
    
	if(m_rect.size.height > layerContent->getContentSize().height)
	{
		m_scrollView->setScrollEnable(false);
		m_isScrollable = false;
	}
	else
		m_isScrollable = true;
    
    addChild(m_scrollView);
}



void DyLayerRank_list::setRankGlobalLayer(DyRankType rankType)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyRankManager* rankManager = DyRankManager::sharedRankManager();
    
    removeAllChildrenWithCleanup(true);
    m_arrAppFriend->removeAllObjects();
    m_arrPictureFrame->removeAllObjects();
    m_arrButtons->removeAllObjects();
    m_arrRankInList->removeAllObjects();
    
    m_listType = DyLayerRankListType_Global;
    m_rankType = rankType;
    
    DyLib* lib = DyLib::sharedLib();
    
    if(rankType == DyRankType_Stage)
    {
        m_arrRank = rankManager->m_arrTotalStory;
    }
    else
        m_arrRank = rankManager->m_arrTotalCrazy;
    
    CCArray* rankarray = m_arrRank;
    
    m_sizeItem = CCSizeMake(m_rect.size.width, 65);
    
    int top_gap;
    
    m_itemCountForHeight = 0;
    
    top_gap = -4;
    m_itemCountForHeight = m_itemCount = rankarray->count();
    
    CCLayerColor* layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), m_sizeItem.width, m_sizeItem.height*(m_itemCountForHeight)-top_gap);
    
    m_menuList = CCMenu::create();
    m_menuList->setPosition(CCPointZero);
    
    int index = 0;
    for(int i = 0;i < m_itemCount;i++)
    {
        float posY = top_gap + m_sizeItem.height*m_itemCountForHeight - (index+1)*m_sizeItem.height + m_sizeItem.height/2;
        
        //if(i < (int)rankarray->count())
        {
            int order;
            
            long long point;
            DyRankGlobal* rank;
            
            rank = (DyRankGlobal*)rankarray->objectAtIndex(i);
            point = rank->m_point;
            order = rank->m_order;
            
            m_arrRankInList->addObject(rank);
            
            if(rank->m_user_id.compare(dataManager->m_user_id) == 0)
            {
                CCScale9Sprite *background = CCScale9Sprite::createWithSpriteFrame(CCSprite::createWithSpriteFrameName("main_ui_rank_cursor.png")->displayFrame());
                background->setContentSize(CCSizeMake(m_sizeItem.width-15, m_sizeItem.height-4));
                background->setPosition(ccp(m_sizeItem.width/2, posY));
                
                layerContent->addChild(background);
            }

            
            
            CCSprite* sprNumberBg;
            
            if(order < 100)
                sprintf(lib->m_strTemp, "%d", order);
            else
                sprintf(lib->m_strTemp, "99+");
            
            CCLabelTTF* labelNumber = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 25.f);
            if(order == 1)
            {
                sprNumberBg = CCSprite::createWithSpriteFrameName("main_ui_rankmark_0.png");
                labelNumber->setColor(ccc3(57,27,5));
                labelNumber->setPosition(ccp(35, posY+2));
            }
            else if(order == 2)
            {
                sprNumberBg = CCSprite::createWithSpriteFrameName("main_ui_rankmark_1.png");
                labelNumber->setColor(ccc3(57,27,5));
                labelNumber->setPosition(ccp(35, posY+2));
            }
            else if(order == 3)
            {
                sprNumberBg = CCSprite::createWithSpriteFrameName("main_ui_rankmark_2.png");
                labelNumber->setColor(ccc3(57,27,5));
                labelNumber->setPosition(ccp(35, posY+2));
            }
            else
            {
                sprNumberBg = CCSprite::createWithSpriteFrameName("main_ui_rank_number_bg.png");
                labelNumber->setColor(ccWHITE);
                labelNumber->setPosition(ccp(35, posY));
            }
            
            if(order > 99)
            {
                labelNumber->setFontSize(18.f);
            }
            
            sprNumberBg->setPosition(ccp(35, posY));
            layerContent->addChild(sprNumberBg);
            layerContent->addChild(labelNumber);
            
            
            CCSprite* sprPictureFrame = CCSprite::createWithSpriteFrameName("main_ui_rank_picture_frame.png");
            sprPictureFrame->setPosition(ccp(35+55, posY));
            sprPictureFrame->setVisible(false);
            layerContent->addChild(sprPictureFrame);
            m_arrPictureFrame->addObject(sprPictureFrame);
            
            //
            // 코스튬
            int offset_costume_x;
            offset_costume_x = 35+55;
            
            YtAniObject* doggy = YtAniObject::loadAniObject(this, "node_animal_doggy.ccbi");
            doggy->setPosition(ccp(offset_costume_x, posY -m_sizeItem.height/2 + 3/* + sizeItem.height/2*/));
            doggy->setScale(0.22f);
            layerContent->addChild(doggy);
            
            if(rank->m_public.m_version == DYDATA_PUBLICDATA_VER && rank->m_public.m_stageRecord == rank->m_stage_record)
            {
            }
            else
            {
                DyCostumeManager::setDefaultCostumeData(&rank->m_public.m_costume_status, &rank->m_public.m_costume_status_ex);
            }
            DyCostumeManager::sharedCostumeManager()->setCostume(doggy, &rank->m_public.m_costume_status, &rank->m_costume_function);

            
            CCLabelTTF* labelName;
            
            labelName = CCLabelTTF::create(rank->m_nickname.c_str(), FONT_NAME_EXP, 22.f, CCSizeMake(250, 26), kCCTextAlignmentLeft);
            labelName->setPosition(ccp(35+100, posY + 15));
            labelName->setColor(ccc3(57,27,5));
            
            labelName->setAnchorPoint(ccp(0, 0.5));
            layerContent->addChild(labelName);
            
            
            YtLib::setMoneyStyle(lib->m_strTemp, point);
                
            CCLabelTTF* labelPoint = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 28.f);
            labelPoint->setPosition(ccp(m_sizeItem.width - 80 + 50, posY - 13));
            labelPoint->setColor(ccc3(246,96,97));
            labelPoint->setAnchorPoint(ccp(1, 0.5));
            layerContent->addChild(labelPoint);
            
            
            CCSprite* sprButton = YtBlankSprite::spriteWithSizeAndColor(CCSizeMake(m_sizeItem.width-50, m_sizeItem.height), ccRED);
            sprButton->setOpacity(0);
            sprButton->setPosition(ccp((m_sizeItem.width-100)/2 - 25, posY));
            //sprButton->setTag(i);
            layerContent->addChild(sprButton);
            m_arrButtons->addObject(sprButton);
            
        }
        
        if(index < m_itemCountForHeight - 1)
        {
            CCSprite* sprLine = CCSprite::createWithSpriteFrameName("main_ui_ranking_line.png");
            sprLine->setPosition(ccp(m_sizeItem.width/2-5, posY - m_sizeItem.height/2));
            layerContent->addChild(sprLine);
            
            if(m_listType == DyLayerRankListType_Pride)
                sprLine->setScaleX(1.3);
        }
        index++;
    }
    
    layerContent->addChild(m_menuList);
    
    m_scrollView = CCScrollView::create(m_rect.size, layerContent);
	m_scrollView->setDelegate(this);
    
	m_scrollView->setContentSize(layerContent->getContentSize());
	m_scrollView->setDirection(kCCScrollViewDirectionVertical);
	m_scrollView->setContentOffset(m_scrollView->minContainerOffset());
	m_scrollView->setGlobalScale(YtLib::sharedLib()->m_scale);
	m_scrollView->setPosition(m_rect.origin);
    
	if(m_rect.size.height > layerContent->getContentSize().height)
	{
		m_scrollView->setScrollEnable(false);
		m_isScrollable = false;
	}
	else
		m_isScrollable = true;
    
    addChild(m_scrollView);
}


DyLayerRank_list::~DyLayerRank_list()
{
    m_arrButtons->release();
    
    m_arrPictureFrame->release();
    m_arrAppFriend->release();
}


void DyLayerRank_list::scrollViewDidScroll(CCScrollView* view)
{
    CCPoint posOffset = view->getContentOffset();
    CCSize sizeView = view->getViewSize();
    
    DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
    
    for(int offsetY = posOffset.y;offsetY < posOffset.y + sizeView.height + m_sizeItem.height;offsetY += m_sizeItem.height)
    {
        float offsetStart = m_sizeItem.height*m_itemCountForHeight - sizeView.height;
        
        int index = (offsetStart + offsetY)/m_sizeItem.height;
        
        if(index >= 0 && index < m_arrPictureFrame->count() && index < m_arrAppFriend->count())
        {
            CCSprite* sprPictureFrame = (CCSprite*)this->m_arrPictureFrame->objectAtIndex(index);
            
            if(sprPictureFrame->getChildrenCount() == 0)
            {
                DyFriend* _friend = (DyFriend*)m_arrAppFriend->objectAtIndex(index);
                
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


void DyLayerRank_list::scrollViewDidZoom(CCScrollView* view)
{
}




void DyLayerRank_list::setVisible(bool value)
{
    cocos2d::CCNode::setVisible(value);
 
    m_isVisible = value;
    
    if(m_menuList)
        m_menuList->setEnabled(value);

    if(m_scrollView)
    {
        m_scrollView->setTouchEnabled(value);
        
    }
}


void DyLayerRank_list::scrollViewTouchContentGlobal(CCScrollView* view, CCPoint position)
{
    m_select_index = -1;
    
    
    for(unsigned int i = 0;i < m_arrButtons->count();i++)
	{
		CCSprite *button = (CCSprite *)m_arrButtons->objectAtIndex(i);
        
		if(button->boundingBox().containsPoint(position))
		{
            //sprSelect = button;
			m_select_index = i;
		}
	}
    
    if(m_select_index < 0)
        return;
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    DyRankGlobal* rank = (DyRankGlobal*)m_arrRankInList->objectAtIndex(m_select_index);
    
    CCNode* nodeTarget = (CCNode*)m_arrPictureFrame->objectAtIndex(m_select_index);
    CCPoint _position = nodeTarget->getPosition();
    
    _position = nodeTarget->getParent()->convertToWorldSpace(_position);
    _position = YtLayerManager::sharedLayerManager()->getTopLayer()->convertToNodeSpace(_position);
    
    
    DyLayerRankDetail *layerRankDetail = DyLayerRankDetail::create(_position, m_rankType, rank);
    YtLayerManager::sharedLayerManager()->pushLayer(layerRankDetail, false, true);
 
//    DyLib* lib = DyLib::sharedLib();
//    
//    sprintf(lib->m_strTemp, "{\"user_id\":\"%s\",\"score_type\":\"%s\"}", rank->m_user_id.c_str(), m_rankType == DyRankType_Stage ? "story" : "crazy");
//    
//    DYNETWORK_ADD_PROC(DyNetworkCmdType_UserScoreGet, lib->m_strTemp, NULL, DyLayerRank_list_NetCallFunc);
}


void DyLayerRank_list::showDetailView(int global_order)
{
    unsigned int score;
    unsigned int scoreBest;

    DyFriend* _friend;
    if(m_listType == DyLayerRankListType_SubStage)
    {
        DyStageRank* rank = (DyStageRank*)m_arrRankInList->objectAtIndex(m_select_index);
        _friend = rank->m_friend;
    }
    else
    {
        DyRank* rank = (DyRank*)m_arrRankInList->objectAtIndex(m_select_index);
        _friend = rank->m_friend;
        score = rank->m_point;
        scoreBest = rank->m_pointBest;
    }
    
    CCNode* nodeTarget = (CCNode*)m_arrPictureFrame->objectAtIndex(m_select_index);
    
    CCPoint _position = nodeTarget->getPosition();
    
    if(m_listType == DyLayerRankListType_Friend)
    {
        //
        // 메인랭킹
        _position = nodeTarget->getParent()->convertToWorldSpace(_position);
        _position = YtLayerManager::sharedLayerManager()->getTopLayer()->convertToNodeSpace(_position);
    }
    else
    {
        //
        // 자랑하기
        // 스테이지별 랭킹
        YtLayerManager* layerManager = YtLayerManager::sharedLayerManager();
        YtLayer* layerBg = layerManager->getLayerByFromTop(layerManager->getLayerCount() - 3);
        
        _position = nodeTarget->getParent()->convertToWorldSpace(_position);
        _position = layerBg->convertToNodeSpace(_position);
    }
    
    DyLayerRankDetail *layerRankDetail = DyLayerRankDetail::create(_position, _friend, m_rankType, score, scoreBest, global_order);
    YtLayerManager::sharedLayerManager()->pushLayer(layerRankDetail, false, true);

}


void DyLayerRank_list::scrollViewTouchContent(CCScrollView* view, CCPoint position)
{
    DyLib* lib = DyLib::sharedLib();
    
    if(m_listType == DyLayerRankListType_Global)
    {
        scrollViewTouchContentGlobal(view, position);
        return;
    }

    m_select_index = -1;
    
    for(unsigned int i = 0;i < m_arrButtons->count();i++)
	{
		CCSprite *button = (CCSprite *)m_arrButtons->objectAtIndex(i);
        
		if(button->boundingBox().containsPoint(position))
		{
            //sprSelect = button;
			m_select_index = i;
		}
	}
    
    if(m_select_index < 0)
        return;
    
    if(m_listType == DyLayerRankListType_Friend && m_select_index == m_arrButtons->count()-1)
    {
        onInvitefirendClicked(NULL);
        return;
    }
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    DyFriend* _friend;
    if(m_listType == DyLayerRankListType_SubStage)
    {
        DyStageRank* rank = (DyStageRank*)m_arrRankInList->objectAtIndex(m_select_index);
        _friend = rank->m_friend;
    }
    else
    {
        DyRank* rank = (DyRank*)m_arrRankInList->objectAtIndex(m_select_index);
        _friend = rank->m_friend;
    }

    sprintf(lib->m_strTemp, "{\"user_id\":\"%s\",\"score_type\":\"%s\"}", _friend->m_user_id.c_str(), m_rankType == DyRankType_Crazy ? "crazy" : "story");
    
    g_layerRank_list = this;
    DyPopup* popup = lib->show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
    
    DYNETWORK_ADD_PROC(DyNetworkCmdType_UserScoreGet, lib->m_strTemp, NULL, DyLayerRank_list_NetCallFunc);
}


void DyLayerRank_list::pause()
{
    if(m_scrollView)
        m_scrollView->setTouchEnabled(false);
    
    if(m_menuList)
        m_menuList->setEnabled(false);
}

void DyLayerRank_list::resume()
{
    if(m_scrollView)
        m_scrollView->setTouchEnabled(m_isVisible);

    if(m_menuList)
        m_menuList->setEnabled(m_isVisible);
    
    if(m_doggy)
        DyCostumeManager::sharedCostumeManager()->setCostume(m_doggy, &DyDataManager::sharedDataManager()->m_public.m_costume_status, &DyDataManager::sharedDataManager()->m_custume_function);
    
}

DyFriend* DyLayerRank_list::getUser(int index)
{
    CCArray* rankarray;
    if(m_arrRank)
        rankarray = m_arrRank;
    else
        rankarray = DyRankManager::sharedRankManager()->getArray(m_rankType);
    
    if(m_listType == DyLayerRankListType_SubStage)
    {
        DyStageRank* rank = (DyStageRank*)rankarray->objectAtIndex(index);
        return rank->m_friend;
    }
    else
    {
        DyRank* rank = (DyRank*)rankarray->objectAtIndex(index);
        return rank->m_friend;
    }
}


//void DyLayerRank_list_CallFunc_SendMessage(void* value)
//{
////    const char* messageRet = (const char*)value;
////    if(strlen(messageRet) == 0)
////        CCLog("send ok.....");
//}


void DyLayerRank_list::onPresentClicked(CCObject * obj)
{   
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    CCMenuItemImage *menuitem = dynamic_cast<CCMenuItemImage*>(obj);
    
    int index = menuitem->getTag();
    
    DyFriend* _friend = getUser(index);
    
    if(_friend->m_message_blocked)
        return;
    
    DyLayerPresent* layerPresent = DyLayerPresent::create(this, _friend);
	YtLayerManager::sharedLayerManager()->pushLayer(layerPresent, true, true);
}





void DyLayerRank_list::callfunc_message_block_success(float dt)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layer->unschedule(schedule_selector(DyLayerRank_list::callfunc_message_block_success));
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    
    if(dataManager->m_message_blocked)// off성공
    {
        dataManager->m_message_blocked = false;
        
        g_layerRank_list->m_menuitemMine->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("main_ui_btn_present_on.png")->displayFrame());
        g_layerRank_list->m_menuitemMine->setSelectedSpriteFrame(CCSprite::createWithSpriteFrameName("main_ui_btn_present_on_sel.png")->displayFrame());
    }
    else // on 성공
    {
        dataManager->m_message_blocked = true;
        g_layerRank_list->m_menuitemMine->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("main_ui_btn_present_off.png")->displayFrame());
        g_layerRank_list->m_menuitemMine->setSelectedSpriteFrame(CCSprite::createWithSpriteFrameName("main_ui_btn_present_off_sel.png")->displayFrame());

    }
}



void DyLayerRank_list::callfunc_message_block_fail(float dt)
{
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layer->unschedule(schedule_selector(DyLayerRank_list::callfunc_message_block_fail));
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
    popup->setText(STRING_SENDPRESENT_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerRank_list_callfunc_message_block(int value, int code)
{
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    if(value == 0)
    {
        layer->schedule(schedule_selector(DyLayerRank_list::callfunc_message_block_success), 0.05f);
    }
    else
    {
        layer->schedule(schedule_selector(DyLayerRank_list::callfunc_message_block_fail), 0.05f);
    }
}




void DyLayerRank_list::onMessageBlockClicked(CCObject * obj)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
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
    
    g_layerRank_list = this;
    
    if(dataManager->m_message_blocked)
    {
        DyLib::sharedLib()->network_kakao_message_block(DyLayerRank_list_callfunc_message_block, false);
    }
    else
    {
        DyPopup* popup = NULL;
        popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerRank_list::onPopupClicked));
        
        popup->setText("이 설정을 끄게되면 친구로 부터 선물을 받을 수 없어요.");
        
        popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
        popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 2);
        popup->addButton(DyButtonType_Close, "", 0);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
}


void DyLayerRank_list::onInvitefirendClicked(cocos2d::CCObject * pSender)
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


void DyLayerRank_list::onPrideClicked(CCObject * obj)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    CCMenuItemImage *menuitem = dynamic_cast<CCMenuItemImage*>(obj);

    int index = menuitem->getTag();

    DyFriend* _friend = getUser(index);

    
    
    DyPopup* popup = NULL;
	popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerRank_list::onPopupClicked));
    
    sprintf(lib->m_strTemp, STRING_PRIDE, _friend->m_nickname.c_str());
    
	popup->setText(lib->m_strTemp);
    
	popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
	popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 1);
	popup->addButton(DyButtonType_Close, "", 0);
    
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
    m_friendProcess = _friend;
    m_indexProcess = index;
}


void DyLayerRank_list::callfunc_update_me_success(float dt)
{
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    layer->unschedule(schedule_selector(DyLayerRank_list::callfunc_update_me_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    if(dataManager->m_private.m_cntPride > 0)
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
        
        sprintf(lib->m_strTemp, STRING_GAMERESULT_PRIDE_SUCCESS, dataManager->m_private.m_cntPride);
        popup->setText(lib->m_strTemp);
        
        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
    else
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
        popup->setText("축하합니다. 무료룰렛이용권 1장 받으셨습니다.");
        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
}



void DyLayerRank_list::callfunc_update_me_fail(float dt)
{
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layer->unschedule(schedule_selector(DyLayerRank_list::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerRank_list::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
    
}


void DyLayerRank_list_callfunc_update_me(int value, int code)
{
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    if(value == 0)
    {
        layer->schedule(schedule_selector(DyLayerRank_list::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layer->schedule(schedule_selector(DyLayerRank_list::callfunc_update_me_fail), 0.05f);
    }
}



void DyLayerRank_list_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str)
{
    switch(cmdType)
    {
        case DyNetworkCmdType_SendLinkMessage:
            if(value_int == 0)
            {
                DyDataManager* dataManager = DyDataManager::sharedDataManager();
                
                DyLib* lib = DyLib::sharedLib();
                
                g_layerRank_list->m_friendProcess->m_tmpTag = 1;
                ((CCMenuItemImage*)g_layerRank_list->m_menuList->getChildByTag(g_layerRank_list->m_indexProcess))->setEnabled(false);
                
                dataManager->m_private.m_cntPride++;
                
                if(dataManager->m_private.m_cntPride >= 10)
                {
                    dataManager->m_private.m_cntPride = 0;
                    dataManager->m_private.m_cntFreeRoullete++;
                    
                    if(dataManager->m_private.m_cntFreeRoullete > MAX_CHARNUM)
                        dataManager->m_private.m_cntFreeRoullete = MAX_CHARNUM;
                }
                
                lib->network_kakao_update_me(0, DyLayerRank_list_callfunc_update_me);
            }
            break;
        case DyNetworkCmdType_UserScoreGet:
            DyLib::sharedLib()->show_network_alaram(false);
            
            if(value_int == 0)
            {
                Json::Value root;   // will contains the root value after parsing.
                Json::Reader reader;
                
                reader.parse((char*)value_str, root);
                
                //CCLog("%s", (char*)value_str);
                
                Json::Value userinfo = root["result"]["userinfo"];
                

                if(userinfo.isNull())
                    g_layerRank_list->showDetailView(-1);
                else
                {
                    Json::Value order = userinfo.get("order", 0);

                    g_layerRank_list->showDetailView(atoi(order.asString().c_str()));
                }
                
            }
            
            break;
        default:
            break;
    }
}

void DyLayerRank_list::onPopupClicked(cocos2d::CCObject * pSender)
{
    DyLib* lib = DyLib::sharedLib();
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
    
    //
    // 자랑하기
	if(pPopup->m_active_tag == 1)
    {
        //DyDataManager* dataManager = DyDataManager::sharedDataManager();
        
        DyPopup* popup = lib->show_network_alaram(true);
        popup->setText(STRING_TITLE_UPDATE_DATA);
        
        
        DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
        
        DyStageManager* stageManager = DyStageManager::sharedStageManager();
        
        std::string score = "";
        std::string shop_name = "";
        std::string stage = "";
        
        if(tycoonManager->m_isCrazyMode)
        {
            sprintf(lib->m_strTemp, "%d", m_pridePoint);
            score = lib->m_strTemp;
        }
        else
        {
            shop_name = stageManager->getShopName(stageManager->m_curShopType);
            sprintf(lib->m_strTemp, "%d", stageManager->m_curLevel+1);
            stage = lib->m_strTemp;
            
            sprintf(lib->m_strTemp, "%d", m_pridePoint);
            score = lib->m_strTemp;
        }
        
        CCArray* arrParam = CCArray::create();
        
        arrParam->addObject(m_friendProcess);
        arrParam->addObject(CCString::create(score));
        arrParam->addObject(CCString::create(shop_name));
        arrParam->addObject(CCString::create(stage));
        
        
        g_layerRank_list = this;
        DYNETWORK_ADD_PROC(DyNetworkCmdType_SendLinkMessage, "", arrParam, DyLayerRank_list_NetCallFunc);
        
//        DyNetworkCommand* commandNetwork = new DyNetworkCommand(DyNetworkCmdType_SendLinkMessage, "", arrParam, DyLayerRank_list_NetCallFunc);
//        DyNetworkManager::sharedNetworkManager()->addLast(commandNetwork);
//        commandNetwork->release();
        
    }
    if(pPopup->m_active_tag == 2)
    {
        DyPopup* popup = lib->show_network_alaram(true);
        popup->setText(STRING_TITLE_UPDATE_DATA);
        
        DyLib::sharedLib()->network_kakao_message_block(DyLayerRank_list_callfunc_message_block, true);
    }

    
}
