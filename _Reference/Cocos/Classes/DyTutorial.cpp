/*
DyTutorial.cpp
Doggy chef
@initialize at 121008

@history
121008	yoonsr	initialize
*/

#include "DyTutorial.h"
#include "DyTycoon.h"
#include "DyLib.h"

#include "DyLayerGameStart.h"

#include "CCextAnimatePacker.h"
#include "DySceneGame.h"

/*
 
tutorial 0(판매)
    기본 판매/ 사이드 판매
 
tutorial 1(목표매출달성과 점수)
    골/퍼펙트, 캔디/MSG,  (하트 시스템, 시간바, 클로즈타임, 라스트팡) 
 
tutorial 2(블럭 및 아이템 사용)
    퀘스트, 슬롯아이템, 
  
 
tutorial 9(펫설명)
    푸들직원채용
tutorial 10
    푸들 선택

 tutorial 20(특수아이템)
    사탕아이템/폭탄아이템
*/

DyTutorialManager* g_sharedTutorialManager;



DyTutorial::DyTutorial(DyTutorialType type, DyTutorialCommandType commandType, void* param0, void* param1, int paramInt0, YtLibSndEff sndEffect)
{
    m_messageType = 0;
    m_sndEffect = sndEffect;
    m_tutorial_status = DyTutorialStatus_Ready;
    
    m_time_excute = 0;
    m_tutorial_type = type;
    m_command_type = commandType;
    m_param0 = param0;
    m_paramInt0 = paramInt0;
    
    m_value_internal = 0;
    
    m_rectBox = CCRect(-500, -500, 1, 1);
    m_posDoggy = ccp(0, 0);
    m_posIndicator = CCPointMake(-500, -1);
    
    m_arrNodes = new CCArray();
    
    switch(m_tutorial_type)
    {
        case DyTutorialType_Message:
            m_message = (std::string)(char*)param0;
            break;
        default:
            break;
    }
    m_menu_ok = NULL;
    m_directArrow = -1;
}


DyTutorial::~DyTutorial()
{
    for(int i = 0;i < m_arrNodes->count();i++)
    {
        CCNode* node = (CCNode*)m_arrNodes->objectAtIndex(i);
        node->removeFromParentAndCleanup(true);
    }
    m_arrNodes->release();
}



void DyTutorial::onOKClicked(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    m_tutorial_status = DyTutorialStatus_Terminate;
    m_menu_ok = NULL;
}


void DyTutorial::onSkipClicked(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    m_menu_ok = NULL;
    
    m_tutorial_status = DyTutorialStatus_Terminate;
    
    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
    
    while(true)
    {
        DyTutorial* tutorial = (DyTutorial*)tutorialManager->m_poolTutorials->objectAtIndex(1);
        
        if(tutorial->m_tutorial_type == DyTutorialType_End || tutorial->m_command_type == DyTutorialCommandType_Must)
        {
            break;
        }
        else
        {
            tutorialManager->removeTutorial(tutorial);
        }
    }
}



void DyTutorial::initBox(CCRect rectBox)
{
    CCLayer* layer = DyTutorialManager::sharedTutorialManager()->getMainLayer();
    CCPoint posStart = DyTutorialManager::sharedTutorialManager()->getCenterPos();
    
    CCScale9Sprite* sprBoxExplain = CCScale9Sprite::createWithSpriteFrame(CCSprite::createWithSpriteFrameName("menu2_ui_tutorial_box.png")->displayFrame());
    
    sprBoxExplain->setPosition(ccp(rectBox.origin.x + rectBox.size.width/2 + posStart.x, rectBox.origin.y + rectBox.size.height/2 + posStart.y));
    sprBoxExplain->setAnchorPoint(ccp(0.5,0.5));
    sprBoxExplain->setContentSize(CCSizeMake(rectBox.size.width, rectBox.size.height));
    
    layer->addChild(sprBoxExplain, ZORDER_GAME_TUTORIAL);
    m_arrNodes->addObject(sprBoxExplain);
    
    sprBoxExplain->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCFadeIn::create(.3f), CCFadeOut::create(.3f), NULL))));
    
}


void DyTutorial::initMessageBox()
{
    if(m_sndEffect < YtLibSndEff_CNT)
        YtLib::sharedLib()->playEffect(m_sndEffect);
    
    CCLayer* layer = DyTutorialManager::sharedTutorialManager()->getMainLayer();
    
    CCPoint posStart = DyTutorialManager::sharedTutorialManager()->getCenterPos();
    
    CCScale9Sprite* sprBox = CCScale9Sprite::createWithSpriteFrame(CCSprite::createWithSpriteFrameName("menu2_ui_tutorial_popup.png")->displayFrame());
    
	sprBox->setPosition(ccp(posStart.x + m_posDoggy.x + 600, posStart.y + m_posDoggy.y + 150));
	sprBox->setAnchorPoint(ccp(0.5,0.5));
    sprBox->setContentSize(CCSizeMake(360, 160));
    
    layer->addChild(sprBox, ZORDER_GAME_TUTORIAL);
    m_arrNodes->addObject(sprBox);
    
    CCLabelTTF* labelMessage = CCLabelTTF::create(m_message.c_str(), FONT_NAME_EXP, 25.f, CCSizeMake(300,500), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
	labelMessage->setColor(ccWHITE);
	labelMessage->setAnchorPoint(ccp(0.5, 0.5));
    
    CCPoint posMessage = sprBox->getPosition();
    
    if(m_messageType == 2 || m_messageType == 3)
        labelMessage->setPosition(ccp(posMessage.x, posMessage.y+30));
	else
        labelMessage->setPosition(posMessage);
	
    layer->addChild(labelMessage, ZORDER_GAME_TUTORIAL);
    
    
    if(m_messageType == 2)
    {
        CCSprite* sprKimchi = CCSprite::createWithSpriteFrameName("menu2_ui_tuto_sam_kimchi.png");
        sprKimchi->setPosition(ccp(posMessage.x - 50, posMessage.y-30));
        layer->addChild(sprKimchi, ZORDER_GAME_TUTORIAL);
        m_arrNodes->addObject(sprKimchi);
        
        CCSprite* sprVegetable = CCSprite::createWithSpriteFrameName("menu2_ui_tuto_vegetable.png");
        sprVegetable->setPosition(ccp(posMessage.x + 50, posMessage.y-30));
        layer->addChild(sprVegetable, ZORDER_GAME_TUTORIAL);
        m_arrNodes->addObject(sprVegetable);
    }
    
    
    if(m_messageType == 3)
    {
        CCSprite* sprBeef = CCSprite::createWithSpriteFrameName("menu2_ui_tuto_beef.png");
        sprBeef->setPosition(ccp(posMessage.x - 50, posMessage.y-30));
        layer->addChild(sprBeef, ZORDER_GAME_TUTORIAL);
        m_arrNodes->addObject(sprBeef);
        
        CCSprite* sprGrill = CCSprite::createWithSpriteFrameName("menu2_ui_tuto_gril.png");
        sprGrill->setPosition(ccp(posMessage.x + 50, posMessage.y-30));
        layer->addChild(sprGrill, ZORDER_GAME_TUTORIAL);
        m_arrNodes->addObject(sprGrill);
    }
    

    m_arrNodes->addObject(labelMessage);
    
    m_menu_ok = new CCMenu();
    
    CCMenuItemImage * oneTab = CCMenuItemImage::create();
    oneTab->initWithNormalSprite(CCSprite::createWithSpriteFrameName("menu2_ui_tutorial_ok.png"), CCSprite::createWithSpriteFrameName("menu2_ui_tutorial_ok_enable.png"), NULL, this, menu_selector(DyTutorial::onOKClicked));
    oneTab->setPosition(ccp(posStart.x + m_posDoggy.x + 760, posStart.y + m_posDoggy.y + 70));
    oneTab->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCScaleTo::create(.2f, 1.1), CCScaleTo::create(.2f, 1.0), NULL))));
  
    
    if(m_command_type == DyTutorialCommandType_None)
    {
        CCMenuItemImage * twoTab = CCMenuItemImage::create();
        twoTab->initWithNormalSprite(CCSprite::createWithSpriteFrameName("menushop_ui_skip_tuto.png"), NULL, NULL, this, menu_selector(DyTutorial::onSkipClicked));
        twoTab->setPosition(ccp(posStart.x + m_posDoggy.x + 490, posStart.y + m_posDoggy.y + 55));
        
        m_menu_ok = CCMenu::create(oneTab, twoTab, NULL);
    }
    else
        m_menu_ok = CCMenu::create(oneTab, NULL);
    
    
    m_menu_ok->setPosition(CCPointZero);
    layer->addChild(m_menu_ok, ZORDER_GAME_TUTORIAL);
    
    m_arrNodes->addObject(m_menu_ok);
    
    CCSprite* sprDoggy = CCSprite::createWithSpriteFrameName("menu2_ui_tutorial_doggy_01.png");
    sprDoggy->setPosition(ccp(posStart.x + m_posDoggy.x + 870, posStart.y + m_posDoggy.y + 150));
    
    sprDoggy->runAction(CCRepeatForever::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_tutorial_doggy")));
    
    layer->addChild(sprDoggy, ZORDER_GAME_TUTORIAL);
    
    m_arrNodes->addObject(sprDoggy);
    
    initBox(m_rectBox);
    
  
    if(m_directArrow >= 0)
    {
        CCPoint posSource = ccp(m_posArrow.x + posStart.x, m_posArrow.y + posStart.y);
        CCPoint posTarget;
        
        CCSprite* sprIndicator = CCSprite::createWithSpriteFrameName("menu2_ui_tutorial_arrow.png");
        
        if(m_directArrow == 0)
        {
            posTarget = ccp(posSource.x, posSource.y+30);
        }
        else
        {
            posTarget = ccp(posSource.x, posSource.y-30);
            sprIndicator->setRotation(180);
        }
        sprIndicator->setPosition(posSource);
        
        sprIndicator->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(
                                                                                               CCMoveTo::create(.3f, posTarget),
                                                                                               CCMoveTo::create(.1f, posSource),
                                                                                               CCDelayTime::create(.5f),
                                                                                               NULL))));
        
        
        layer->addChild(sprIndicator, ZORDER_GAME_TUTORIAL);
        m_arrNodes->addObject(sprIndicator);
    }
    
    
    if(m_messageType == 1)
    {
        DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
        
        float blockWidth = 79;
        float blockHeight = 79;
        
        tutorialManager->m_sprGrill = CCSprite::createWithSpriteFrameName("game_ui_korean_grill.png");
        tutorialManager->m_boardSprite->addChild(tutorialManager->m_sprGrill);
        
        tutorialManager->m_sprGrill->setPosition(ccp(blockWidth/2 + blockWidth*3 + 28, blockHeight/2 + blockHeight*1 + 21));
        
        
        tutorialManager->m_sprGrill->setOpacity(60);
        
        CCActionInterval* seq = (CCActionInterval*)(CCSequence::create(CCFadeTo::create(1.f, 120), CCFadeTo::create(1.f, 60), NULL));
        tutorialManager->m_sprGrill->runAction(CCRepeatForever::create(seq));
        
    }
    
}


void DyTutorial::pause()
{
    if(m_menu_ok)
        m_menu_ok->setTouchEnabled(false);
}


void DyTutorial::resume()
{
    
    if(m_menu_ok)
        m_menu_ok->setTouchEnabled(true);
}


void DyTutorial::update(float dt)
{
    switch(m_tutorial_type)
    {
        case DyTutorialType_Message:
            processMessage();
            break;
            
        case DyTutorialType_ReadyCommand:
            processCommand();
        default:
            break;
    }

}



void DyTutorial::processCommand()
{
    DyLib* lib = DyLib::sharedLib();
    
    CCLayer* layer = DyTutorialManager::sharedTutorialManager()->getMainLayer();
    CCPoint posStart = DyTutorialManager::sharedTutorialManager()->getCenterPos();
    
    switch(m_command_type)
    {
        case DyTutorialCommandType_ThreeMatch:
            if(m_tutorial_status == DyTutorialStatus_Ready)
            {
                m_tutorial_status = DyTutorialStatus_Process;
                
                initBox(CCRectMake(665-245/2, 370-245/2, 245, 245));
                
                
                CCPoint position = ccp(665+15, 370+15);
                
                CCPoint posSource = ccp(position.x+18, position.y+20);
                CCPoint posTarget = ccp(position.x, position.y);
                CCPoint posTarget2 = ccp(position.x-60, position.y);
                CCSprite* sprIndicator = CCSprite::createWithSpriteFrameName("game_ui_hand.png");
                
                sprIndicator->setPosition(posSource);
                
                sprIndicator->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(
                    CCMoveTo::create(.1f, posTarget),
                    CCMoveTo::create(.5f, posTarget2),
                    CCMoveTo::create(.1f, posSource),
                    CCDelayTime::create(1.f),
                    NULL))));
                
                sprIndicator->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(
                                                                                                       CCScaleTo::create(.1f, 1.3f),
                                                                                                       CCDelayTime::create(.5f),
                                                                                                       CCScaleTo::create(.1f, 1.0f),
                                                                                                       CCDelayTime::create(1.f),
                                                                                                       NULL))));
                

                
                
                layer->addChild(sprIndicator, ZORDER_GAME_TUTORIAL);
                m_arrNodes->addObject(sprIndicator);
            }
            break;
        case DyTutorialCommandType_RemoveCurrentBlock:
            if(m_tutorial_status == DyTutorialStatus_Ready)
            {
                m_tutorial_status = DyTutorialStatus_Process;
                
                TmBlock* currentBlock = DyTutorialManager::sharedTutorialManager()->m_currentBlock;
                
                CCPoint position = DyTycoonManager::sharedTycoonManager()->m_board->getGlobalPosition(currentBlock);
                
                CCPoint posSource = ccp(position.x+40, position.y+50);
                CCPoint posTarget = ccp(position.x+40, position.y+20);
                CCSprite* sprIndicator = CCSprite::createWithSpriteFrameName("game_ui_hand.png");
                
                sprIndicator->setPosition(posSource);
                sprIndicator->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(
                                                                                                       CCMoveTo::create(.1f, posTarget),
                                                                                                       CCMoveTo::create(.1f, posSource),
                                                                                                       CCMoveTo::create(.1f, posTarget),
                                                                                                       CCMoveTo::create(.1f, posSource),
                                                                                                       CCDelayTime::create(1.f),
                                                                                                       NULL))));
                
                
                
                layer->addChild(sprIndicator, ZORDER_GAME_TUTORIAL);
                m_arrNodes->addObject(sprIndicator);
            }

            break;
        case DyTutorialCommandType_UseItem:
            if(m_tutorial_status == DyTutorialStatus_Ready)
            {
                DyItemManager* itemManager = DyItemManager::sharedItemManager();
                
                m_tutorial_status = DyTutorialStatus_Process;
                
                CCSprite* sprIndicator = lib->createIndicator(ccp(150 + itemManager->getSlotIndex((DyItemType)m_paramInt0)*72+40, 60));
                
                layer->addChild(sprIndicator, ZORDER_GAME_TUTORIAL);
                m_arrNodes->addObject(sprIndicator);
            }

        case DyTutorialCommandType_GetMoney:
            if(m_tutorial_status == DyTutorialStatus_Ready)
            {
                m_tutorial_status = DyTutorialStatus_Process;
                
                CCSprite* prIndicator = lib->createIndicator(ccp(140+100, 220+100));
                
                layer->addChild(prIndicator, ZORDER_GAME_TUTORIAL);
                m_arrNodes->addObject(prIndicator);
            }
            break;
        case DyTutorialCommandType_SideMenuClick:
            if(m_tutorial_status == DyTutorialStatus_Ready)
            {
                m_tutorial_status = DyTutorialStatus_Process;
                
                CCSprite* sprIndicator = lib->createIndicator(ccp(100, 100));
                
                layer->addChild(sprIndicator, ZORDER_GAME_TUTORIAL);
                m_arrNodes->addObject(sprIndicator);
            }

            break;
        case DyTutorialCommandType_Sell:
            if(m_tutorial_status == DyTutorialStatus_Ready)
            {
                m_tutorial_status = DyTutorialStatus_Process;
            }
            else if(m_tutorial_status == DyTutorialStatus_Process)
            {
                if(m_posIndicator.x < 0)
                    m_time_excute = 0;
                
                if(m_time_excute > 10.f && m_value_internal == 0)
                {
                    CCSprite* sprIndicator = lib->createIndicator(m_posIndicator);
                    
                    layer->addChild(sprIndicator, ZORDER_GAME_TUTORIAL);
                    m_arrNodes->addObject(sprIndicator);
                    m_value_internal = 1;
                }
            }

            break;
        case DyTutorialCommandType_TouchQuestDetail:
            if(m_tutorial_status == DyTutorialStatus_Ready)
            {
                m_tutorial_status = DyTutorialStatus_Process;
            }
            else if(m_tutorial_status == DyTutorialStatus_Process)
            {
                if(m_posIndicator.x < 0)
                    m_time_excute = 0;
                
                if(m_time_excute > 0 && m_value_internal == 0)
                {
                    m_sprIndicator = lib->createIndicator(ccp(m_posIndicator.x + posStart.x, m_posIndicator.y + posStart.y));
                    
                    layer->addChild(m_sprIndicator, ZORDER_GAME_TUTORIAL);
                    m_arrNodes->addObject(m_sprIndicator);
                    m_value_internal = 1;
                }
            }

            break;
            
        case DyTutorialCommandType_DeliveryGetPhone:
        case DyTutorialCommandType_DeliveryCall:
            if(m_tutorial_status == DyTutorialStatus_Ready)
            {
                m_tutorial_status = DyTutorialStatus_Process;
                
                CCSprite* prIndicator = lib->createIndicator(ccp(140+100, 220+100));
                
                layer->addChild(prIndicator, ZORDER_GAME_TUTORIAL);
                m_arrNodes->addObject(prIndicator);
            }
            break;

        default:
            break;
    }
}


void DyTutorial::processMessage()
{
    //CCLayer* layer = DyTycoonManager::sharedTycoonManager()->m_layer;
    
    switch(m_command_type)
    {
        case DyTutorialCommandType_None:
        case DyTutorialCommandType_Must:
            if(m_tutorial_status == DyTutorialStatus_Ready)
            {
                m_tutorial_status = DyTutorialStatus_Process;
                
                initMessageBox();
            }

            break;
        case DyTutorialCommandType_GetMoney:
            break;
        default:
            break;
    }

}


//
// direct 0:up, 1:right, 2:down, 3:left
void DyTutorial::addArrow(CCPoint position, int direct)
{
    m_posArrow = position;
    m_directArrow = direct;

}


DyTutorialManager::DyTutorialManager()
{
    m_poolTutorials = new CCArray();
    
    clearTutorial();
}


DyTutorialManager::~DyTutorialManager()
{
    m_poolTutorials->release();
}

void DyTutorialManager::clearTutorial()
{
    m_doingSpecial = false;
    m_pauseBoard = false;
    m_sprGrill = NULL;
    m_poolTutorials->removeAllObjects();
}


void DyTutorialManager::update(float dt)
{
    DyTutorial* tutorial = getCurrentTutorial();
    
    if(tutorial == NULL)
        return;
    
   tutorial->m_time_excute += dt;
    
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(tutorial->m_tutorial_status == DyTutorialStatus_Terminate)
    {
        removeTutorial(tutorial);
        return;
    }
    
    switch(tutorial->m_tutorial_type)
    {
        case DyTutorialType_Start:
            tycoonManager->m_lockInput = true;
            removeTutorial(tutorial);
            break;
        case DyTutorialType_BlockFallStart:
            m_pauseBoard = false;
            removeTutorial(tutorial);
            m_layer->schedule(schedule_selector(DyLayerGame::displayGrill), 4.0f);
            dataManager->m_local.m_isViewGrilTutorial = true;
            dataManager->dataSave_local();
            if(m_sprGrill)
                m_sprGrill->removeFromParentAndCleanup(true);
            break;
        case DyTutorialType_End:
            tycoonManager->m_lockInput = false;
            removeTutorial(tutorial);
            if(m_index == 6)
            {
                DyDataManager::sharedDataManager()->m_local.m_isViewDeliveryTutorial = true;
                
                DyDataManager::sharedDataManager()->dataSave_local();
            }
            
            if(m_index == 5 || m_index == 4 || m_index == 7)
            {
                tycoonManager->highlightStartButton();
            }
            
            break;
        case DyTutorialType_Message:
            tycoonManager->m_lockInput = true;
            tutorial->update(dt);
            break;

        case DyTutorialType_ReadyCommand:
            tycoonManager->m_lockInput = false;
            tutorial->update(dt);
            break;
        case DyTutorialType_Delay:
            tycoonManager->m_lockInput = true;
            if(tutorial->m_time_excute > (float)tutorial->m_paramInt0)
            {
                removeTutorial(tutorial);
            }
            break;
        case DyTutorialType_Hold:
            if(tutorial->m_time_excute > (float)tutorial->m_paramInt0)
            {
                removeTutorial(tutorial);
            }
            break;
        default:
            tycoonManager->m_lockInput = true;
            break;
    }
}


void DyTutorialManager::addLast(DyTutorial* tutorial)
{
    m_poolTutorials->addObject(tutorial);
}


DyTutorial* DyTutorialManager::getCurrentTutorial()
{
    if(m_poolTutorials->count() == 0)
        return NULL;
    
    return (DyTutorial*)m_poolTutorials->objectAtIndex(0);
}


void DyTutorialManager::removeTutorial(DyTutorial* tutorial)
{
    m_poolTutorials->removeObject(tutorial);
}


void DyTutorialManager::buildTutorial_0()
{
    DyTutorial* tutorial = new DyTutorial(DyTutorialType_BlockSettingDefault, DyTutorialCommandType_None, NULL, NULL, 0, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_OrderRecipe, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Start, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"안녕하세요. 저는 도기셰프예요. 지금부터 저와 함께 가게운영을 배워볼까요?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"우선, 기본적인 음식 판매에 대한 것부터 배워볼까요?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"손님이 주문을 하면 주문한 음식과 재료가 보여요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(225, 250, 100, 100);
    tutorial->addArrow(ccp(225+50,250+100+50), 2);
    
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"해당 재료 블럭을 3개이상 가로나 세로로 맞추면 손님에게 서빙돼요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(543, 248, 245, 245);
    tutorial->addArrow(ccp(543+122,248+245+50), 2);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_ThreeMatch, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 1, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"아주 잘 하셨어요.", NULL, 0, YtLibSndEff_Combo_High);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"손님이 두고간 돈을 터치하면 판매가 완료돼요", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(140, 220, 100, 100);
    tutorial->addArrow(ccp(140+50,220+100+50), 2);
    
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_GetMoney, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"아주 잘 하셨어요.", NULL, 0, YtLibSndEff_Combo_High);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"자, 그럼 이 손님에게 음식을 판매해 보세요~", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_Sell, NULL, NULL, 0, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"아주 훌륭해요.", NULL, 0, YtLibSndEff_Combo_High);
    addLast(tutorial);
    tutorial->release();
    
    
    
    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 1, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이번엔 손님이 음식을 기다리는 동안 재빨리 사이드메뉴를 끼워팔아 추가수익을 얻어 볼까요?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(0, 0, 120, 120);
    tutorial->addArrow(ccp(0+60,0+120+50), 2);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"기계의 램프에 빨간불이 3개 모두 들어오면 사이드메뉴를 서빙할 수 있어요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(0, 0, 80, 40);
    tutorial->addArrow(ccp(0+40,0+40+50), 2);
    addLast(tutorial);
    tutorial->release();
        
    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_SideMenuClick, NULL, NULL, 0, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_SideMoveToCustomer, NULL, NULL, 0, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"아주 잘 했어요. 판매를 완료하면 추가 수익을 얻을 수 있어요!", NULL, 0, YtLibSndEff_Combo_High);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_Sell, NULL, NULL, 0, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"아주 훌륭해요.", NULL, 0, YtLibSndEff_Combo_High);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이제 기본 판매에 대한 방법을 익혔어요. 이번 스테이지를 클리어 해보세요!", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();

    
    tutorial = new DyTutorial(DyTutorialType_End, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
}


//  골/퍼펙트,
void DyTutorialManager::buildTutorial_1()
{
    YtLib* lib = YtLib::sharedLib();
    
    //
    // 골/퍼펙트
    DyTutorial* tutorial = new DyTutorial(DyTutorialType_BlockSettingDefault, DyTutorialCommandType_None, NULL, NULL, 0, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_OrderRecipe, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Start, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이번에는 목표매출 달성과 점수에 대해 알아볼까요?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"일단, 이 손님에게 음식을 판매해 보세요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_Sell, NULL, NULL, 0, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_OrderRecipe, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"우와~이제 잘하시네요.", NULL, 0, YtLibSndEff_Combo_High);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"방금 판매한 금액만큼 왼쪽 상단에 골드게이지바가 차지요?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(160, lib->m_sizeWin.height - 50, 180, 48);
    tutorial->addArrow(ccp(160+90,lib->m_sizeWin.height - 50-50), 0);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"맨 왼쪽 상단에 보시면 GOAL이 있는데, 이 값이 이번 스테이지의 목표매출이예요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(0, lib->m_sizeWin.height - 120, 150, 124);
    tutorial->addArrow(ccp(0+75,lib->m_sizeWin.height - 120-50), 0);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이번 스테이지는 200G네요. 200G를 넘으면 목표매출이 달성된 거예요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(160, lib->m_sizeWin.height - 50, 180, 48);
    tutorial->addArrow(ccp(160+90,lib->m_sizeWin.height - 50-50), 0);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"골드게이지 바를 가득 채우면 퍼펙트가 되는데, 퍼펙트를 많이 달성하면 여러가지 좋은 일이 있답니다.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(160, lib->m_sizeWin.height - 50, 180, 48);
    tutorial->addArrow(ccp(160+90,lib->m_sizeWin.height - 50-50), 0);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"200G를 넘지 못하면 이번 스테이지는 실패하게 돼요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"실패하면 이번 스테이지에서 번 골드를 받을 수 없으니 주의하세요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"실패하지 않도록 목표매출을 열심히 달성해 보세요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_End, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    

    
//    //
//    // 하트 시스템
//    tutorial = new DyTutorial(DyTutorialType_Hold, DyTutorialCommandType_None, NULL, NULL, 3, YtLibSndEff_CNT);
//    addLast(tutorial);
//    tutorial->release();
//    
//    tutorial = new DyTutorial(DyTutorialType_Start, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
//    addLast(tutorial);
//    tutorial->release();
//    
//    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이번에는 하트게이지에 대해 알아볼까요?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
//    addLast(tutorial);
//    tutorial->release();
//
//    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"상단에 하트 게이지바가 보이죠?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
//    tutorial->m_rectBox = CCRectMake(380, lib->m_sizeWin.height - 50, 170, 50);
//    addLast(tutorial);
//    tutorial->release();
//
//    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"조금 전에 음식을 팔아서 손님하트수 만큼 게이지가 올라갔네요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
//    tutorial->m_rectBox = CCRectMake(380, lib->m_sizeWin.height - 50, 170, 50);
//    addLast(tutorial);
//    tutorial->release();
//
//    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"모든 손님은 최대 다섯개의 하트를 가지고 있어요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
//    addLast(tutorial);
//    tutorial->release();
//
//    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"손님 하트수는 시간이 지날 수록 줄어들어요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
//    addLast(tutorial);
//    tutorial->release();
//    
//    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"줄어들기 전에 빨리 판매를 해야 이 하트게이지를 많이 채울 수 있어요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
//    tutorial->m_rectBox = CCRectMake(380, lib->m_sizeWin.height - 50, 170, 50);
//    addLast(tutorial);
//    tutorial->release();
//
//    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이 하트게이지를 모두 채우면 추가 점수를 받을 수 있답니다.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
//    tutorial->m_rectBox = CCRectMake(380, lib->m_sizeWin.height - 50, 170, 50);
//    addLast(tutorial);
//    tutorial->release();
//    
//    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"하트게이지의 설명이 끝났어요. 이번 스테이지도 꼭 성공시켜 보세요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
//    addLast(tutorial);
//    tutorial->release();
//
//    
//    tutorial = new DyTutorial(DyTutorialType_End, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
//    addLast(tutorial);
//    tutorial->release();
}


void DyTutorialManager::buildTutorial_2()
{
    DyTutorial* tutorial = new DyTutorial(DyTutorialType_Start, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    
//    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 1, YtLibSndEff_CNT);
//    addLast(tutorial);
//    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이번에는 시작전 팝업의 기능을 살펴볼까요? 게임에 필요한 여러 기능을 세팅할 수 있는곳이예요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이번 스테이지의 퀘스트는 이곳에 표시돼요. 물음표를 누르면 자세한 설명을 볼 수 있답니다.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(250, 280, 445, 160);
    tutorial->addArrow(ccp(260+415/2,290+150+50), 2);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_TouchQuestDetail, NULL, NULL, 0, YtLibSndEff_CNT);
    tutorial->m_posIndicator = ccp(250+445, 280+160);
    addLast(tutorial);
    tutorial->release();

    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"개스트는 영업종료 후 등장해 내가 가진 가장 비싼 메뉴를 주문하기 때문에 높은 점수 달성에 아주 유용하답니다.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(250+150, 120, 150, 180);
    tutorial->addArrow(ccp(250+75+150,120+180+50), 2);
    tutorial->m_posDoggy = ccp(0, 300);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"친구들과 서로 주고받아 서로 플레이를 도울 수 있어요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(250+150, 120, 150, 180);
    tutorial->addArrow(ccp(250+75+150,120+180+50), 2);
    tutorial->m_posDoggy = ccp(0, 300);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이제 시작 버튼을 누르면 3스테이지를 시작할 수 있어요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(250+110, 25, 220, 100);
    tutorial->addArrow(ccp(250+110+220/2,25+100+50), 2);
    tutorial->m_posDoggy = ccp(0, 300);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"퀘스트 클리어 하는것 잊지마시고 이번 스테이지에서도 행운을 빌어요!", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->addArrow(ccp(260+415/2,290+150+50), 2);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_End, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    

}


void DyTutorialManager::buildTutorial_5()
{
    DyTutorial* tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"여기는 가게를 세팅하는 곳이에요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_posDoggy = ccp(0, 300);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"직원은 6스테이지의 퀘스트를 클리어하면 고용할 수 있는데요,", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(235, 395, 150, 180);
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(235+150/2,395-50), 0);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이곳에서 고용여부 체크, 승진시키기가 가능하답니다.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(235, 395, 150, 180);
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(235+150/2,395-50), 0);
    addLast(tutorial);
    tutorial->release();


    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"사이드 메뉴를 업그레이드해서 조리속도와 가격을 높일 수 있어요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(65, 0, 100, 100);
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(65+50,100+50), 2);
    addLast(tutorial);
    tutorial->release();
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이 아이콘을 터치하면 도기의 옷을 바로 갈아 입힐 수 있어요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(725, 170, 100, 100);
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(725+50,170+100+50), 2);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이곳에 내가 사용할 아이템을 장착할 수 있어요. 지금은 3가지 밖에 사용할 수 없으니 3개 모두 장착되어있네요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(110, 0, 280, 80);
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(110+140, 80+50), 2);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_End, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();


}



void DyTutorialManager::buildTutorial_3()
{
    DyLib* lib = DyLib::sharedLib();
    DyTutorial* tutorial = new DyTutorial(DyTutorialType_BlockSettingDefault, DyTutorialCommandType_None, NULL, NULL, 0, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_OrderRecipe, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Start, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이번엔 블럭 아이템들을 알아볼께요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 1, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    // 사탕 등장
    tutorial = new DyTutorial(DyTutorialType_AddSpecialBlock, DyTutorialCommandType_None, NULL, NULL, 0, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    // 손님2,3등장
    tutorial = new DyTutorial(DyTutorialType_Customer, DyTutorialCommandType_None, NULL, NULL, 6, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Customer, DyTutorialCommandType_None, NULL, NULL, 4, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 1, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    


    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"캔디를 더블탭 하면 가장 기분이 좋지 않은 손님에게 자동으로 먹여서 하트를 채워줘요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 1, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();


    // 사탕 터치(더블탭에니)
    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_RemoveCurrentBlock, NULL, NULL, 0, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"잘하셨어요. 캔디는 손님에게 3번 연속 만족스런 서빙을 하면 등장한답니다.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"끊기지 않고 잘 플레이 해서 캔디 아이템을 얻으면 유용하게 쓸 수 있겠죠?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    
    //
    // MSG등장
    tutorial = new DyTutorial(DyTutorialType_AddSpecialBlock, DyTutorialCommandType_None, NULL, NULL, 1, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 1, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    


    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이번엔 MSG 블럭을 사용해 볼까요?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 1, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    //
    // MSG 터치(더블탭애니)
    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_RemoveCurrentBlock, NULL, NULL, 0, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"쨘~ 보셨죠? MSG를 사용하면 손님이 음식값을 많이 지불한답니다.(1.5배)", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"음식 맛을 돋궈주나봐요. 몸에도 좋을지는 확실치 않지만.....", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"MSG블럭은 판매콤보 5번을 성공하면 등장한답니다. 비싼 음식을 주문했을때 사용하면 더 효과적이겠죠?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    
    /*
    ""
    "현재 사용 가능한 슬롯 아이템은
    "(차례대로 네모로 표시)돋보기, 새로고침, 시계 아이템이랍니다."
    
     
     
     "돋보기는 매칭 가능한 퍼즐을 찾아주는 아이템이예요 사용해볼까요?"
    (손가락이 돋보기에 생김)
    
    "새로고침은 퍼즐판 배열이 맘에안들경우 바꿀 수 있답니다. 사용해 볼까요?"
    (손가락이 새로고침에 생김)
    
    "시계는 스테이지 시간을 5초 늘려주어 손님이 더 올수있는 시간을 준답니다 사용해 볼까요?"
    (손가락이 시계에 생김)
    
    */
    
    DyItemManager* itemManager = DyItemManager::sharedItemManager();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"이번 스테이지의 퀘스트는 슬롯 아이템 3가지를 모두 사용하는 건데요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(120, 1, 240, 80);
    tutorial->addArrow(ccp(120+120,1+80+50), 2);
    addLast(tutorial);
    tutorial->release();

    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"힌트", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(135 + itemManager->getSlotIndex(DyItemType_Hint)*72, 1, 80, 80);
    tutorial->addArrow(ccp(tutorial->m_rectBox.origin.x+40, 1+80+50), 2);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"새로고침", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(135 + itemManager->getSlotIndex(DyItemType_Refresh)*72, 1, 80, 80);
    tutorial->addArrow(ccp(tutorial->m_rectBox.origin.x+40, 1+80+50), 2);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"시계 아이템이랍니다.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(135 + itemManager->getSlotIndex(DyItemType_TimeIncrease)*72, 1, 80, 80);
    tutorial->addArrow(ccp(tutorial->m_rectBox.origin.x+40, 1+80+50), 2);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"힌트는 매칭 가능한 퍼즐을 찾아주는 아이템이예요 사용해볼까요?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(135 + itemManager->getSlotIndex(DyItemType_Hint)*72, 1, 80, 80);
    tutorial->addArrow(ccp(tutorial->m_rectBox.origin.x+40, 1+80+50), 2);
    addLast(tutorial);
    tutorial->release();

    
    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_UseItem, NULL, NULL, DyItemType_Hint, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();


    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"새로고침은 퍼즐판 배열이 맘에안들경우 바꿀 수 있답니다. 사용해 볼까요?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(135 + itemManager->getSlotIndex(DyItemType_Refresh)*72, 1, 80, 80);
    tutorial->addArrow(ccp(tutorial->m_rectBox.origin.x+40, 1+80+50), 2);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_UseItem, NULL, NULL, DyItemType_Refresh, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    
    sprintf(lib->m_strTemp, "시계는 스테이지 시간을 %d초 늘려주어 손님이 더 올수있는 시간을 준답니다 사용해 볼까요?", (int)ITEM_ADD_TIME);
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)lib->m_strTemp, NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(135 + itemManager->getSlotIndex(DyItemType_TimeIncrease)*72, 1, 80, 80);
    tutorial->addArrow(ccp(tutorial->m_rectBox.origin.x+40, 1+80+50), 2);
    addLast(tutorial);
    tutorial->release();
    
    
    
    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_UseItem, NULL, NULL, DyItemType_TimeIncrease, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"시계아이템은 한 스테이지에 세번까지만 사용가능하며 가게를 닫은 후엔 사용할 수 없으니 유의하세요~", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
                              
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"슬롯 아이템은 골드로 충전할 수 있지만, 지나친 사용시 이윤을 남기지못하니 현명한 운영이 필요하겠죠?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"업적달성시 보상으로 주어지기도 하니 잘 챙기세요~", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"이제 슬롯 아이템의 사용법을 아셨죠?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
 
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"이번 스테이지의 퀘스트는 성공했으니 골을 달성하여 스테이지를 클리어 하는 일만 남았네요~", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_End, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
}



void DyTutorialManager::buildTutorial_4()
{
    //CCLog("DyTutorialManager::buildTutorial_4()");
    
    DyTutorial* tutorial = new DyTutorial(DyTutorialType_Start, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    
//    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 1, YtLibSndEff_CNT);
//    addLast(tutorial);
//    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"이제부터 푸들 캐셔가 계산을 도와줄 거에요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(235, 395, 150, 180);
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(235+150/2,395-50), 0);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"여러번 고용하면 능력을 업그레이드 할 수 있답니다.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(235, 395, 150, 180);
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(235+150/2,395-50), 0);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"승진시키기 버튼을 누르거나, 마이룸>직원 탭에서 관리가 가능해요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(235, 395-0, 140, 80);
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(235+150/2,395-50-0), 0);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_End, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
}


void DyTutorialManager::buildTutorial_6()
{
    //DyLib* lib = DyLib::sharedLib();
    DyTutorial* tutorial = new DyTutorial(DyTutorialType_Start, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"점장님,이번 스테이지부터는 배달 주문을 받아야 해요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();

    
    tutorial = new DyTutorial(DyTutorialType_OrderRecipeByDelivery, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"배달 주문 전화가 왔어요. 터치해서 전화를 받아요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_DeliveryGetPhone, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"이제 주문한 음식을 만들어야 하는데요,다른 주문보다 우선으로 조리되니 주의하세요.", NULL, 0, YtLibSndEff_Combo_High);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_DeliveryMake, NULL, NULL, 0, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"조리가 완료되면 철가방을 터치하여 배달부를 불러요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();

    
    tutorial = new DyTutorial(DyTutorialType_ReadyCommand, DyTutorialCommandType_DeliveryCall, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"잘했어요! 이 방법으로 배달 주문을 소화하면 된답니다.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"이번 스테이지에서는 배달을 2회 소화해야 퀘스트가 완료되니 잊지말고 전화를 받으세요.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    addLast(tutorial);
    tutorial->release();

    
    tutorial = new DyTutorial(DyTutorialType_Delay, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
    


    tutorial = new DyTutorial(DyTutorialType_End, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
}





void DyTutorialManager::buildTutorial_7()
{
    DyTutorial* tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"점장님! 저와 결혼을 약속한 푸들이 여동생을 데려왔어요!", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_posDoggy = ccp(0, 300);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"그래서 이제부턴 캐셔를 선택해서 고용할 수 있답니다.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_posDoggy = ccp(0, 300);
    addLast(tutorial);
    tutorial->release();

    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"새로운 캐셔에게 어떤 능력이 있는지는 마이룸-직원에서 확인할 수 있어요", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(235-30, 395, 150+60, 180+70);
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(235+150/2,395-50), 0);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"새로 온 캐셔와 함께 가게문을 열어 볼까요?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_rectBox = CCRectMake(235-30, 395, 150+60, 180+70);
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(235+150/2,395-50), 0);
    addLast(tutorial);
    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_End, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
}

void DyTutorialManager::buildTutorial_8()
{
    m_pauseBoard = true;
    
    DyTutorial* tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"점장님! 도기예요! 오랜만이죠?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_posDoggy = ccp(0, 300);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"이번 스테이지부터는 새로운 조리법이 추가돼요!", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_posDoggy = ccp(0, 300);
    addLast(tutorial);
    tutorial->release();
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"달궈진 그릴에 고기가 올라오면 구워진답니다.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_messageType = 1;
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(670, 240), 2);
    addLast(tutorial);
    tutorial->release();

    
    
    //    tutorial->m_rectBox = CCRectMake(235-30, 395, 150+60, 180+70);
    //    tutorial->m_posDoggy = ccp(0, 300);
    //    tutorial->addArrow(ccp(235+150/2,395-50), 0);
    
    
    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"구운고기는 쌈 메뉴에 필요한 재료랍니다~", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_messageType = 2;
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(670, 240), 2);
    addLast(tutorial);
    tutorial->release();

    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_Must, (void*)"이제부터 고기재료를 두가지 방법으로 조리할 수 있어요!", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
    tutorial->m_messageType = 3;
    tutorial->m_posDoggy = ccp(0, 300);
    tutorial->addArrow(ccp(670, 240), 2);
    addLast(tutorial);
    tutorial->release();


    tutorial = new DyTutorial(DyTutorialType_BlockFallStart, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();

    
//    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"그래서 이제부턴 캐셔를 선택해서 고용할 수 있답니다.", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
//    tutorial->m_posDoggy = ccp(0, 300);
//    addLast(tutorial);
//    tutorial->release();
//    
//    
//    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"새로운 캐셔에게 어떤 능력이 있는지는 마이룸-직원에서 확인할 수 있어요", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
//    tutorial->m_rectBox = CCRectMake(235-30, 395, 150+60, 180+70);
//    tutorial->m_posDoggy = ccp(0, 300);
//    tutorial->addArrow(ccp(235+150/2,395-50), 0);
//    addLast(tutorial);
//    tutorial->release();
//    
//    
//    tutorial = new DyTutorial(DyTutorialType_Message, DyTutorialCommandType_None, (void*)"새로 온 캐셔와 함께 가게문을 열어 볼까요?", NULL, 0, YtLibSndEff_AWARD_COMPLETE);
//    tutorial->m_rectBox = CCRectMake(235-30, 395, 150+60, 180+70);
//    tutorial->m_posDoggy = ccp(0, 300);
//    tutorial->addArrow(ccp(235+150/2,395-50), 0);
//    addLast(tutorial);
//    tutorial->release();
    
    tutorial = new DyTutorial(DyTutorialType_End, DyTutorialCommandType_None, NULL, NULL, 2, YtLibSndEff_CNT);
    addLast(tutorial);
    tutorial->release();
}




void DyTutorialManager::buildTutorial(int index)
{
    extern int g_tutorial_cursorX;
    g_tutorial_cursorX = 0;

    m_index = index;
    switch(index)
    {
        case 0:
            buildTutorial_0();
            break;
        case 1:
            buildTutorial_1();
            break;
        case 2:
            buildTutorial_2();
            break;
        case 3:
            buildTutorial_3();
            break;
        case 4:
            buildTutorial_4();
            break;
        case 5:
            buildTutorial_5();
            break;
        case 6:
            buildTutorial_6();
            break;
        case 7:
            buildTutorial_7();
            break;
        case 8:
            buildTutorial_8();
            break;


    }
}



CCLayer* DyTutorialManager::getMainLayer()
{
    switch(m_index)
    {
        case 2:
        //case 4:
            return (CCLayer*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMESTART_TAG);
    }
    
    return DyTycoonManager::sharedTycoonManager()->m_layer;
}

CCPoint DyTutorialManager::getCenterPos()
{
    DyLib* lib = DyLib::sharedLib();
    
    switch(m_index)
    {
        case 2:
        //case 4:
            return ccp(-lib->m_sizeWin.width/2, -lib->m_sizeWin.height/2);
    }
    
    return ccp(0, 0);

}


void DyTutorialManager::showTutorial(bool show)
{
    DyTutorial* tutorial = getCurrentTutorial();
    
    if(tutorial)
    {
        for(int i = 0;i < tutorial->m_arrNodes->count();i++)
        {
            CCNode* node = (CCNode*)tutorial->m_arrNodes->objectAtIndex(i);
            node->setVisible(show);
        }
    }
}


DyTutorialManager* DyTutorialManager::sharedTutorialManager()
{
	if(g_sharedTutorialManager == NULL)
	{
		g_sharedTutorialManager = new DyTutorialManager();
	}
	return g_sharedTutorialManager;
}


