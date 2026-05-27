/*
DyLayerStageSub.cpp
Doggy chef
@initialize at 121120

@history
121120	yoonsr	initialize
*/

#include "DyConfig.h"
#include "DyLayerStageSub.h"
#include "DyLib.h"
#include "DyStage.h"
#include "DyMoney.h"
#include "DyLayerShop.h"
#include "DyPopup.h"
#include "DyFriend.h"
#include "DyLayerStageRank.h"


DyLayerStageSub::DyLayerStageSub()
{
    m_tag = DYLAYERSTAGESUB_TAG;
    
    m_arrPageCursorBg = new CCArray();
    m_arrStageRank = new CCArray();
    
    m_arrPictureFrame = new CCArray();
   
    m_arrRankMenu = new CCArray();
}

DyLayerStageSub::~DyLayerStageSub()
{
    for(int i = 0;i <  m_arrRankMenu->count();i++)
    {
        CCMenu* menu = (CCMenu*)m_arrRankMenu->objectAtIndex(i);
        menu->setEnabled(false);
    }

	DyMoneyManager::sharedMoneyManager()->removeMoneyLayer(m_layerMoney);

	m_layerMoney->release();
    m_arrPageCursorBg->release();
    m_arrStageRank->release();
    
    m_arrRankMenu->release();
    
    
    m_arrPictureFrame->release();
}



//DyStageRank* DyLayerStageSub::getMyRank(CCArray* array)
//{
//   for(unsigned int i = 0;i < array->count();i++)
//	{
//        DyStageRank* rank = (DyStageRank*)array->objectAtIndex(i);
//        
//        if(rank->m_friend->m_friendType == DyFriendType_Mine)
//            return rank;
//	}
//    return NULL;
//}

DyStageRank* DyLayerStageSub::getFirstRank(CCArray* array)
{
    if(array->count() == 0)
        return NULL;
    return (DyStageRank*)array->objectAtIndex(0);
}

CCMenuItemSprite* DyLayerStageSub::createStageButton(int stage)
{
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
    //DyShopRecord* shopRecord = stageManager->getShopRecord(stageManager->m_curShopType);
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
	CCSprite* sprNormal;
	CCSprite* sprSelect;
	DyLib* lib = DyLib::sharedLib();

    DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
    
    
    if(stage >= stageManager->m_cntLevel)
    {
        if(stageManager->getStageState(stageManager->m_curShopType, stage) == DyStageState_Lock)
        {
            sprNormal = CCSprite::createWithSpriteFrameName("menu_ui_btn_stage_disable_bonus.png");
            sprSelect = CCSprite::createWithSpriteFrameName("menu_ui_btn_stage_disable_bonus.png");
        }
        else
        {
            sprNormal = CCSprite::createWithSpriteFrameName("menu_ui_btn_stage_bonus.png");
            sprSelect = CCSprite::createWithSpriteFrameName("menu_ui_btn_stage_bonus.png");
        }
    }
    else
    {
        if(stageManager->getStageState(stageManager->m_curShopType, stage) == DyStageState_Lock)
        {
            sprNormal = CCSprite::createWithSpriteFrameName("menu_ui_btn_stage_disable.png");
            sprSelect = CCSprite::createWithSpriteFrameName("menu_ui_btn_stage_disable.png");
        }
        else
        {
            sprNormal = CCSprite::createWithSpriteFrameName("menu_ui_btn_stage.png");
            sprSelect = CCSprite::createWithSpriteFrameName("menu_ui_btn_stage.png");
        }
    }
    
    
    
	for(int i = 0;i < 2;i++)
	{
        
		CCSprite* sprTaget;
		if(i == 0)
			sprTaget = sprNormal;
		else
		{
            sprTaget = sprSelect;
            if(stageManager->getStageState(stageManager->m_curShopType, stage) != DyStageState_Lock)
            {
                sprTaget = sprSelect;
                CCSprite* sprSel = CCSprite::createWithSpriteFrameName("menu_ui_stage_sel.png");
                sprTaget->addChild(sprSel);
                sprSel->setPosition(ccp(sprTaget->getContentSize().width/2, sprTaget->getContentSize().height/2));
            }
		}
        
        CCSprite* sprPin = CCSprite::createWithSpriteFrameName("menu_ui_stage_pin.png");
        sprTaget->addChild(sprPin);
        sprPin->setPosition(ccp(sprTaget->getContentSize().width/2-5, sprTaget->getContentSize().height-5));
        
        
        if(stageManager->m_curShopType == DyStageType_Snack && stage < 2)
        {
            CCSprite* sprTutorialMark = CCSprite::createWithSpriteFrameName("menu_ui_tutorial_mark.png");
            sprTaget->addChild(sprTutorialMark);
            sprTutorialMark->setPosition(ccp(sprTaget->getContentSize().width-15, sprTaget->getContentSize().height-8));
        }
        else
        {
            //
            //랭킹표시
            CCSprite* sprBtnRank = NULL;
            DyStageRank* friendRank = getFirstRank((CCArray*)m_arrStageRank->objectAtIndex(stage));

            if(friendRank)
            {
                sprBtnRank = CCSprite::createWithSpriteFrameName("menu2_ui_friends_frame.png");
            
                m_arrPictureFrame->addObject(sprBtnRank);
                
                CCSprite* sprPicture = friendManager->getPicture(friendRank->m_friend, CCSizeMake(39.f, 39.f));
                
                if(sprPicture)
                {
                    sprBtnRank->addChild(sprPicture);
                    sprPicture->setPosition(ccp(sprBtnRank->getContentSize().width/2-4, sprBtnRank->getContentSize().height/2+4));
                }
            }

            if(sprBtnRank)
            {
                CCMenuItemImage* menuitemRank = CCMenuItemImage::create();
                
                menuitemRank->initWithNormalSprite(sprBtnRank, NULL, NULL, this, menu_selector(DyLayerStageSub::rankClicked));
                
                menuitemRank->setTag(stage);
                
                CCMenu* menu = CCMenu::create(menuitemRank, NULL);
                menu->setPosition(ccp(sprTaget->getContentSize().width-13, sprTaget->getContentSize().height-15));
                
                sprTaget->addChild(menu);
                
                m_arrRankMenu->addObject(menu);
            }
        }
        
		if(stageManager->getStageState(stageManager->m_curShopType, stage) == DyStageState_Lock)
		{
			CCSprite* sprLock = CCSprite::createWithSpriteFrameName("menu_ui_stage_lock.png");
			sprTaget->addChild(sprLock);
			sprLock->setPosition(ccp(sprTaget->getContentSize().width/2, sprTaget->getContentSize().height/2-5));
		}
		else
		{
            CCSprite* sprNumberBg;
            
            if(stage >= stageManager->m_cntLevel)
                sprNumberBg = CCSprite::createWithSpriteFrameName("menu_ui_stagenumber_bg_bonus.png");
            else
                sprNumberBg = CCSprite::createWithSpriteFrameName("menu_ui_stagenumber_bg.png");
                
            sprTaget->addChild(sprNumberBg);
            sprNumberBg->setPosition(ccp(sprTaget->getContentSize().width/2, sprTaget->getContentSize().height/2+23));
            
            sprintf(lib->m_strTemp, "%02d", stage+1);
			CCLabelTTF* labelStage = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 27.f);
			sprTaget->addChild(labelStage);
			labelStage->setColor(ccc3(255, 255, 255));

			labelStage->setPosition(ccp(sprNumberBg->getPosition().x, sprNumberBg->getPosition().y-2));

            
            
            lib->setMoneyStyle(lib->m_strTemp, dataManager->shop_get_record(stageManager->m_curShopType, stage));
            CCLabelTTF* labelPoint = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 24.f);
		    sprTaget->addChild(labelPoint);
            labelPoint->setColor(ccc3(245, 95, 96));
            labelPoint->setPosition(ccp(sprTaget->getContentSize().width/2, sprTaget->getContentSize().height/2-5));
        	
            
			float scaleStar = 0.72f;

			
			CCSprite* sprStar0 = CCSprite::createWithSpriteFrameName("menu_ui_star_normal.png");
			sprTaget->addChild(sprStar0);
			sprStar0->setPosition(ccp(sprTaget->getContentSize().width/2-34, 20));
			sprStar0->setScale(scaleStar);
			
			CCSprite* sprStar1 = CCSprite::createWithSpriteFrameName("menu_ui_star_normal.png");
			sprTaget->addChild(sprStar1);
			sprStar1->setPosition(ccp(sprTaget->getContentSize().width/2, 20));
			sprStar1->setScale(scaleStar);

			CCSprite* sprStar2 = CCSprite::createWithSpriteFrameName("menu_ui_star_normal.png");
			sprTaget->addChild(sprStar2);
			sprStar2->setPosition(ccp(sprTaget->getContentSize().width/2+34, 20));
			sprStar2->setScale(scaleStar);


			if(stageManager->getStageState(stageManager->m_curShopType, stage) >= DyStageState_Fail)
			{
				if(m_lastStage < stage)
					m_lastStage = stage;
			}

			switch(stageManager->getStageState(stageManager->m_curShopType, stage))
			{
				case DyStageState_Fail:
					break;
				case DyStageState_Goal:
					{
						CCSprite* sprStar_high_0 = CCSprite::createWithSpriteFrameName("menu_ui_star_highlight.png");
						sprTaget->addChild(sprStar_high_0);
						sprStar_high_0->setPosition(sprStar0->getPosition());
						sprStar_high_0->setScale(scaleStar);
						if(i == 0 && stage < stageManager->m_cntLevel)
                            m_totalStar++;
					}
					break;
				case DyStageState_Goal_Plus:
					{
						CCSprite* sprStar_high_0 = CCSprite::createWithSpriteFrameName("menu_ui_star_highlight.png");
						sprTaget->addChild(sprStar_high_0);
						sprStar_high_0->setPosition(sprStar0->getPosition());
						sprStar_high_0->setScale(scaleStar);
						CCSprite* sprStar_high_1 = CCSprite::createWithSpriteFrameName("menu_ui_star_highlight.png");
						sprTaget->addChild(sprStar_high_1);
						sprStar_high_1->setPosition(sprStar1->getPosition());
						sprStar_high_1->setScale(scaleStar);
						if(i == 0 && stage < stageManager->m_cntLevel)
							m_totalStar += 2;
					}
					break;
				case DyStageState_Perfect:
					{
						CCSprite* sprStar_high_0 = CCSprite::createWithSpriteFrameName("menu_ui_star_highlight.png");
						sprTaget->addChild(sprStar_high_0);
						sprStar_high_0->setPosition(sprStar0->getPosition());
						sprStar_high_0->setScale(scaleStar);

						CCSprite* sprStar_high_1 = CCSprite::createWithSpriteFrameName("menu_ui_star_highlight.png");
						sprTaget->addChild(sprStar_high_1);
						sprStar_high_1->setPosition(sprStar1->getPosition());
						sprStar_high_1->setScale(scaleStar);

						CCSprite* sprStar_high_2 = CCSprite::createWithSpriteFrameName("menu_ui_star_highlight.png");
						sprTaget->addChild(sprStar_high_2);
						sprStar_high_2->setPosition(sprStar2->getPosition());
						sprStar_high_2->setScale(scaleStar);

						if(i == 0 && stage < stageManager->m_cntLevel)
							m_totalStar += 3;
					}
					break;
                default:
                    break;
			}
		}
	}

	CCMenuItemSprite* menuSprite = CCMenuItemSprite::create(sprNormal, sprSelect, this, menu_selector(DyLayerStageSub::stageClicked));
	menuSprite->setTag(stage);
	return menuSprite;
}



void DyLayerStageSub::sortRank(int level)
{
    CCArray* arrRank = (CCArray*)m_arrStageRank->objectAtIndex(level);
    
	for(unsigned int i = 0;i < arrRank->count();i++)
	{
		for(unsigned int j = i+1;j < arrRank->count();j++)
		{
			DyStageRank* rank_I = (DyStageRank*)arrRank->objectAtIndex(i);
			DyStageRank* rank_J = (DyStageRank*)arrRank->objectAtIndex(j);
            
			if(rank_I->m_score < rank_J->m_score)
			{
				arrRank->exchangeObjectAtIndex(i, j);
			}
		}
	}
    
	int ranking = 1;
	int prevPoint = -1;
	int prevRank = ranking;
	for(unsigned int i = 0;i < arrRank->count();i++)
	{
		DyStageRank* rank = (DyStageRank*)arrRank->objectAtIndex(i);
        
		if(prevPoint == rank->m_score)
		{
			rank->m_order = prevRank;
		}
		else
		{
			rank->m_order = ranking;
		}
        
		prevRank = rank->m_order;
		prevPoint = rank->m_score;
		ranking++;
	}
}


void DyLayerStageSub::setupRank(int level)
{
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
  
    CCArray* arrRank = CCArray::create();
    //
    // 스테이지별 랭킹
    DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
    for(int i = 0;i < friendManager->m_arrFriends->count();i++)
    {
        DyFriend* _friend = (DyFriend*)friendManager->m_arrFriends->objectAtIndex(i);
        
        if(_friend->m_friendType != DyFriendType_Normal && !_friend->m_isDummy)
        {
            CCArray* arrScore = (CCArray*)_friend->m_arrShopPoint->objectAtIndex(stageManager->m_curShopType);
            int score = ((CCInteger*)arrScore->objectAtIndex(level))->getValue();
            if(score > 0)
            {
                DyStageRank* stageRank = new DyStageRank();
                stageRank->m_friend = _friend;
                stageRank->m_score = score;
                arrRank->addObject(stageRank);
                stageRank->release();
            }
        }
    }
    
    m_arrStageRank->addObject(arrRank);
}


//void DyLayerStageSub::markLast()
//{
//    CCArray* arrFriendCheckLast = CCArray::create();
//    
//    for(int i = m_arrStageRank->count()-1;i >= 0;i--)
//    {
//        CCArray* arrRank = (CCArray*)m_arrStageRank->objectAtIndex(i);
//        
//        for(int j = 0;j < arrRank->count();j++)
//        {
//            DyStageRank* stageRank = (DyStageRank*)arrRank->objectAtIndex(j);
//            
//            if(arrFriendCheckLast->indexOfObject(stageRank->m_friend) == CC_INVALID_INDEX)
//            {
//                stageRank->m_isLast = true;
//                arrFriendCheckLast->addObject(stageRank->m_friend);
//            }
//        }
//    }
//}

void DyLayerStageSub::alarmOpenCrazy(float dt)
{
    unschedule(schedule_selector(DyLayerStageSub::alarmOpenCrazy));
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerStageSub::onPopupClicked));
    popup->setText(STRING_CRAZY_OPEN_ALARM);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OPEN, 30);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_MSG);
    
}


void DyLayerStageSub::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyStageManager* stageManager = DyStageManager::sharedStageManager();

	setTouchEnabled(true);
	
	//DyShopRecord* shopRecord = stageManager->getShopRecord(stageManager->m_curShopType);

    m_totalStar = 0;
	m_lastStage = 0;
    
	CCArray* arrMenuItem = CCArray::create();
    
    int countLevel = stageManager->getCountLevel(stageManager->m_curShopType);
    
	for(int i = 0;i < countLevel;i++)
	{
        setupRank(i);
        sortRank(i);
		arrMenuItem->addObject(createStageButton(i));
	}
    //markLast();
    
    int crazy_mode_max_cnt = atoi(lib->m_parameters->getValue("value_integer", DyLibParam_Crazymode_Starcnt));
    
    if(dataManager->shop_get_crazyopen(stageManager->m_curShopType) == false && m_totalStar >= crazy_mode_max_cnt)
    {
        schedule(schedule_selector(DyLayerStageSub::alarmOpenCrazy), 0.2f);
    }
        
	m_menuGrid = SlidingMenuGrid::menuWithArray(arrMenuItem, 5, 3, CCPointMake(-300, 480), CCPointMake(150, 110), false);
	m_menuGrid->setClipOut(CCRectMake(-370, -115, 750, 340), this);
	
	m_menuGrid->setDelegate(this);
	addChild(m_menuGrid);
	
	for(int i = 0;i < m_menuGrid->getPageCount();i++)
	{
        CCSprite* sprPageCursorBg = CCSprite::createWithSpriteFrameName("menu_ui_page_cursor_default.png");
        m_arrPageCursorBg->addObject(sprPageCursorBg);
        
        sprPageCursorBg->setAnchorPoint(ccp(0, 0.5));
		sprPageCursorBg->setPosition(ccp(- (50*m_menuGrid->getPageCount())/2 + i*50, -250-10));
		addChild(sprPageCursorBg, 100);
	}

	m_sprPageCursor = CCSprite::createWithSpriteFrameName("menu_ui_page_cursor_current.png");
	m_sprPageCursor->setAnchorPoint(ccp(0, 0.5));
    
	m_sprPageCursor->setPosition(((CCSprite*)m_arrPageCursorBg->objectAtIndex(0))->getPosition());
	addChild(m_sprPageCursor, 100);
    
	m_menuMain = (CCMenu*)getChildByTag(STAGESUB_UI_TAG_MENU);
	m_menuMain->getChildByTag(STAGESUB_UI_TAG_MENUITEM_LEFT)->setVisible(false);

	CCSprite* sprLeftDeco = (CCSprite*)this->getChildByTag(STAGESUB_UI_TAG_DECO_LEFT);
	CCSprite* sprRightDeco = (CCSprite*)this->getChildByTag(STAGESUB_UI_TAG_DECO_RIGHT);

	switch(stageManager->m_curShopType)
	{
		case DyStageType_Hamburger://hamburger
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("shop_ui_hamburger.plist");
			sprLeftDeco->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_hamburger_stagesel_deco_left.png")->displayFrame());
			sprRightDeco->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_hamburger_stagesel_deco_right.png")->displayFrame());
			sprRightDeco->setPosition(ccp(sprRightDeco->getPosition().x, sprRightDeco->getPosition().y - 2));
			break;
		case DyStageType_Coffee://coffee
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("shop_ui_coffee.plist");
			sprLeftDeco->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_coffee_stagesel_deco_left.png")->displayFrame());
			sprLeftDeco->setPosition(ccp(sprLeftDeco->getPosition().x + 30, sprLeftDeco->getPosition().y-10));
			sprRightDeco->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_coffee_stagesel_deco_right.png")->displayFrame());
			sprRightDeco->setPosition(ccp(sprRightDeco->getPosition().x, sprRightDeco->getPosition().y + 15));
			break;
            
		case DyStageType_China://coffee
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("shop_ui_china.plist");
			sprLeftDeco->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_stagesel_deco_left.png")->displayFrame());
			sprLeftDeco->setPosition(ccp(sprLeftDeco->getPosition().x-30, sprLeftDeco->getPosition().y-20));
			sprRightDeco->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_stagesel_deco_right.png")->displayFrame());
			sprRightDeco->setPosition(ccp(sprRightDeco->getPosition().x+10, sprRightDeco->getPosition().y));
			break;
		case DyStageType_Pizza://coffee
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("shop_ui_pizza.plist");
			sprLeftDeco->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_stagesel_deco_left.png")->displayFrame());
			sprLeftDeco->setPosition(ccp(sprLeftDeco->getPosition().x-30, sprLeftDeco->getPosition().y-50));
			sprRightDeco->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_stagesel_deco_right.png")->displayFrame());
            
			sprRightDeco->setPosition(ccp(sprRightDeco->getPosition().x-10, sprRightDeco->getPosition().y+50));
			break;
        case DyStageType_Korean://coffee
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("shop_ui_korean.plist");
			sprLeftDeco->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_stagesel_deco_left.png")->displayFrame());
			sprLeftDeco->setPosition(ccp(sprLeftDeco->getPosition().x-50, sprLeftDeco->getPosition().y-50));
			sprRightDeco->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_stagesel_deco_right.png")->displayFrame());
            
			sprRightDeco->setPosition(ccp(sprRightDeco->getPosition().x+30, sprRightDeco->getPosition().y+50-30));
			break;
        default:
            break;
	}

	m_layerMoney = new DyLayerMoney(DyLayerMoneyType_Normal, 1);

	m_layerMoney->setPosition(ccp(-410.f, 240.f));
	addChild(m_layerMoney);
	DyMoneyManager::sharedMoneyManager()->addMoneyLayer(m_layerMoney);

    //
    // 마지막 레벨로 이동
	if(m_lastStage/15 != 0)
	{
		m_menuGrid->gotoPage(m_lastStage/15, false);
	}
    
    //
    // 크레이지 모드 강제 시작
    //shopRecord->m_totalStar = 50;
    
    buildCrazyButton();
    
    setKeypadEnabled(true);
}


void DyLayerStageSub::buildCrazyButton()
{
	DyLib* lib = DyLib::sharedLib();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    //DyShopRecord* shopRecord = stageManager->getShopRecord(stageManager->m_curShopType);
    
    CCMenuItemImage* itemImageCrazy = (CCMenuItemImage*)m_menuMain->getChildByTag(STAGESUB_UI_TAG_MENUITEM_CRAZY);
    CCSprite* sprNormal = (CCSprite*)itemImageCrazy->getNormalImage();
    sprNormal->removeAllChildrenWithCleanup(true);
    
    CCSprite* sprDelly = (CCSprite*)getChildByTag(STAGESUB_UI_TAG_DELLY);
    
    if(dataManager->shop_get_crazyopen(stageManager->m_curShopType) == false)
    {
        CCSprite* sprCrazyLock = CCSprite::createWithSpriteFrameName("menu_ui_crazy_mode_lock.png");
        sprCrazyLock->setPosition(ccp(100, 70));
        sprNormal->addChild(sprCrazyLock);
        sprDelly->setVisible(true);
        
        CCLabelTTF* labelDelly = (CCLabelTTF*)sprDelly->getChildByTag(STAGESUB_UI_TAG_DELLY_TEXT);
        sprintf(lib->m_strTemp, "%d", CRAZY_OPEN_DELLY);
        labelDelly->setString(lib->m_strTemp);
    }
    else
    {
        sprDelly->setVisible(false);
    }
    
    int crazy_mode_max_cnt = atoi(lib->m_parameters->getValue("value_integer", DyLibParam_Crazymode_Starcnt));
    sprintf(lib->m_strTemp, "/%d", crazy_mode_max_cnt);
    CCLabelTTF* labelMaxCnt = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 30);
    labelMaxCnt->setAnchorPoint(ccp(0, 0.5));
    labelMaxCnt->setPosition(ccp(120, 24));
    labelMaxCnt->setColor(ccWHITE);
    sprNormal->addChild(labelMaxCnt);
    
    sprintf(lib->m_strTemp, "%d", m_totalStar);
    CCLabelTTF* labelCurCnt = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 30);
    labelCurCnt->setAnchorPoint(ccp(1, 0.5));
    labelCurCnt->setPosition(ccp(115, 24));
    labelCurCnt->setColor(ccBLACK);
    sprNormal->addChild(labelCurCnt);
}



SEL_MenuHandler DyLayerStageSub::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName)
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCloseClicked", DyLayerStageSub::onCloseClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onLeftClicked", DyLayerStageSub::onLeftClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onRightClicked", DyLayerStageSub::onRightClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCrazyClicked", DyLayerStageSub::onCrazyClicked);
    return NULL;
}


SEL_CCControlHandler DyLayerStageSub::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}

bool DyLayerStageSub::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerStageSub::onCloseClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLOSE);
	YtLayerManager::sharedLayerManager()->popLayer();
}

void DyLayerStageSub::keyBackClicked()
{
    setKeypadEnabled(false);
    onCloseClicked(NULL);
}



void DyLayerStageSub::onRightClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	m_menuGrid->gotoPage(m_menuGrid->getCurrentPage()+1, true);
}


void DyLayerStageSub::onLeftClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	m_menuGrid->gotoPage(m_menuGrid->getCurrentPage()-1, true);
}




void DyLayerStageSub::callfunc_update_me_success(float dt)
{
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    
    unschedule(schedule_selector(DyLayerStageSub::callfunc_update_me_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_MSG);
    
    moneyManager->applyDelly();
    
    buildCrazyButton();
}


void DyLayerStageSub::callfunc_update_me_fail(float dt)
{
    unschedule(schedule_selector(DyLayerStageSub::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    lib->data_restore();
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShop::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerStageSub_callfunc_update_me(int value, int code)
{
    DyLayerStageSub *layerStageSub = (DyLayerStageSub*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERSTAGESUB_TAG);
    
    if(value == 0)
    {
        layerStageSub->schedule(schedule_selector(DyLayerStageSub::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layerStageSub->schedule(schedule_selector(DyLayerStageSub::callfunc_update_me_fail), 0.05f);
    }
}


void DyLayerStageSub::onPopupClicked(cocos2d::CCObject * pSender)
{
	DyLib* lib = DyLib::sharedLib();
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
    
    if(pPopup->m_active_tag == 20)
    {
        return;//by ranking
    }
	if(pPopup->m_active_tag == 11)
	{
        DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
        
        if(moneyManager->getDelly() < CRAZY_OPEN_DELLY)
        {
            DyMoneyManager::sharedMoneyManager()->alramNotEnoughDelly(true, true);
        }
        else
        {
            lib->data_backup();
            
            dataManager->shop_set_crazyopen(stageManager->m_curShopType, true);
            
            moneyManager->addDelly(-CRAZY_OPEN_DELLY, false);
            
            DyLib::sharedLib()->network_kakao_update_me(0, DyLayerStageSub_callfunc_update_me);
            
        }
    }
    else if(pPopup->m_active_tag == 30)
	{
        lib->data_backup();
        
        dataManager->shop_set_crazyopen(stageManager->m_curShopType, true);
        
        DyLib::sharedLib()->network_kakao_update_me(0, DyLayerStageSub_callfunc_update_me);
    }

    
    dataManager->dataSave();
}



void DyLayerStageSub::onCrazyClicked(CCObject * pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    //DyShopRecord* shopRecord = stageManager->getShopRecord(stageManager->m_curShopType);
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(dataManager->shop_get_crazyopen(stageManager->m_curShopType) == false)
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerStageSub::onPopupClicked));
        
        popup->setText(STRING_NOT_ENOUGH_STAR);
        popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 10);
        popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 11);
        popup->addButton(DyButtonType_Close, "", 10);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
    else
    {
        DyStageManager::sharedStageManager()->setLevel(stageManager->getCurrentLevel(stageManager->m_curShopType));
        
        startGame(true);
    }
}



void DyLayerStageSub::pause()
{
	m_menuMain->setEnabled(false);
	m_menuGrid->setEnabled(false);
	m_layerMoney->pause();
    
    for(int i = 0;i <  m_arrRankMenu->count();i++)
    {
        CCMenu* menu = (CCMenu*)m_arrRankMenu->objectAtIndex(i);
        menu->setEnabled(false);
    }
    setKeypadEnabled(false);
}


void DyLayerStageSub::resume()
{
	m_menuMain->setEnabled(true);
	m_menuGrid->setEnabled(true);
	m_layerMoney->resume();
    
    for(int i = 0;i <  m_arrRankMenu->count();i++)
    {
        CCMenu* menu = (CCMenu*)m_arrRankMenu->objectAtIndex(i);
        menu->setEnabled(true);
    }
    setKeypadEnabled(true);
}


void DyLayerStageSub::hide()
{
	YtLib* lib = YtLib::sharedLib();

	runAction(CCSequence::create(

		CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)), 
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height)),

		CCCallFunc::create(this, callfunc_selector(DyLayerStageSub::destory)),
		NULL));

   //YtLayerManager::sharedLayerManager()->destoryTopLayer();
}


void DyLayerStageSub::show()
{
	YtLib* lib = YtLib::sharedLib();

	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));

	runAction(CCSequence::create(

		CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
		NULL));
}



void DyLayerStageSub::startGame(bool isCrazyMode)
{   
    YtLib::sharedLib()->stopBGM();
	
	m_menuMain->setEnabled(false);
	m_menuGrid->setEnabled(false);
    
    DyTycoonManager::sharedTycoonManager()->m_isCrazyMode = isCrazyMode;
    
	DyLib::sharedLib()->setStatus(DyLib_AppStatus_Tip, 0);
}



void DyLayerStageSub::rankClicked(CCObject* obj)
{
    CCMenuItemSprite* menuSprite = dynamic_cast<CCMenuItemSprite*>(obj);
    
    int stage = menuSprite->getTag();

    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    DyLayerStageRank* DyLayerStageRank = DyLayerStageRank::create(this, stage, (CCArray*)m_arrStageRank->objectAtIndex(stage));
  	YtLayerManager::sharedLayerManager()->pushLayer(DyLayerStageRank, true, true);
}


void DyLayerStageSub::stageClicked(CCObject* obj)
{
    CCMenuItemSprite* menuSprite = dynamic_cast<CCMenuItemSprite*>(obj);
	//DyShopRecord* shopRecord = DyStageManager::sharedStageManager()->getShopRecord(DyStageManager::sharedStageManager()->m_curShopType);
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    
	if(stageManager->getStageState(stageManager->m_curShopType, menuSprite->getTag()) == DyStageState_Lock)
		return;

    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    DyStageManager::sharedStageManager()->setLevel(menuSprite->getTag());

    startGame(false);
}

void DyLayerStageSub::pageMoving(SlidingMenuGrid* menugrid)
{
	m_menuMain->getChildByTag(STAGESUB_UI_TAG_MENUITEM_LEFT)->setVisible(false);
	m_menuMain->getChildByTag(STAGESUB_UI_TAG_MENUITEM_RIGHT)->setVisible(false);
}


void DyLayerStageSub::pageDidChange(SlidingMenuGrid* menugrid, int page)
{
    m_sprPageCursor->setPosition(((CCSprite*)m_arrPageCursorBg->objectAtIndex(page))->getPosition());

	if(page > 0)
		m_menuMain->getChildByTag(STAGESUB_UI_TAG_MENUITEM_LEFT)->setVisible(true);
	else
		m_menuMain->getChildByTag(STAGESUB_UI_TAG_MENUITEM_LEFT)->setVisible(false);

	if(page < menugrid->getPageCount()-1)
		m_menuMain->getChildByTag(STAGESUB_UI_TAG_MENUITEM_RIGHT)->setVisible(true);
	else
		m_menuMain->getChildByTag(STAGESUB_UI_TAG_MENUITEM_RIGHT)->setVisible(false);
}


DyLayerStageSub* DyLayerStageSub::create(CCNode* owner)
{
	return (DyLayerStageSub*)YtLayerManager::create("DyLayerStageSub", DyLayerStageSubLoader::loader(), "layer_stage_sub.ccbi", owner);
}