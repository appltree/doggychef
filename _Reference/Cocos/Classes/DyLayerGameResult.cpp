/*
DyLayerGameResult.cpp
Doggy chef
@initialize at 121125

@history
121125	yoonsr	initialize
*/


#include "DyConfig.h"
#include "DyLayerGameResult.h"
#include "DyLib.h"
#include "DyStage.h"
#include "DyTycoon.h"
#include "DyMoney.h"
#include "DyLayerShop.h"

#include "CCextAnimatePacker.h"

#include "DyPopup.h"

#include "YtLib.h"

#include "DyLayerPride.h"

#include "DyToast.h"
#include "DySceneGame.h"

#include "DyNetwork.h"
#include "json.h"

DyLayerGameResult* g_layerGameResult;



DyLayerGameResultRankup::DyLayerGameResultRankup()
{
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("fever_time.plist");
    CCextAnimatePacker::getInstance()->loadAnimations("threematch_ui_effect.xml");
    m_isPaused = false;
}

DyLayerGameResultRankup::~DyLayerGameResultRankup()
{
}


void DyLayerGameResultRankup::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader)
{
    DyLib* lib = DyLib::sharedLib();
    m_layerFire = (CCLayer*)getChildByTag(RANKUP_UI_TAG_FIREWORKS_LAYER);
    
    
    CCSprite* sprRainbow = CCSprite::createWithSpriteFrameName("fever_time_bg.png");
    m_layerFire->addChild(sprRainbow);
    
    sprRainbow->setPosition(ccp(0, 0));
    
    sprRainbow->setScale(lib->m_sizeWin.width/sprRainbow->getContentSize().width);
    
    sprRainbow->setOpacity(0);
    CCActionInterval* seq = (CCActionInterval*)(CCSequence::create(CCFadeIn::create(.8f), CCFadeOut::create(.8f), NULL));
    sprRainbow->runAction(CCRepeatForever::create(seq));
    
    DyRankManager* rankManager = DyRankManager::sharedRankManager();
    DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
    
    for(int i = 0;i < 2;i++)
    {
        CCSprite* sprBg;
        DyRank* rank = rankManager->getMyRank(rankManager->currentCheckRankType());
        if(i == 0)
        {
            sprBg = (CCSprite*)getChildByTag(RANKUP_UI_TAG_USER_LAYER);
            rank = rankManager->getMyRank(rankManager->currentCheckRankType());
        }
        else
        {
            sprBg = (CCSprite*)getChildByTag(RANKUP_UI_TAG_TARGET_LAYER);
            rank = rankManager->getNextFriendRank(rankManager->currentCheckRankType());
            if(rank == NULL)
                continue;
        }
        
        //
        // 방향키
        CCSprite* sprUserArrow = (CCSprite*)sprBg->getChildByTag(RANKUP_UI_TAG_ARROW);
        CCPoint posUserArrow = sprUserArrow->getPosition();
        CCSprite* sprUserArrowReal = CCSprite::createWithSpriteFrame(sprUserArrow->displayFrame());
        sprBg->addChild(sprUserArrowReal);
        sprUserArrowReal->setPosition(posUserArrow);
        sprUserArrow->removeFromParentAndCleanup(true);
        sprUserArrowReal->setOpacity(0);
        if(i == 0)
            sprUserArrowReal->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCMoveTo::create(.3f, posUserArrow), CCMoveTo::create(.3f, ccp(posUserArrow.x, posUserArrow.y + 20)), NULL))));
        else
            sprUserArrowReal->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCMoveTo::create(.3f, posUserArrow), CCMoveTo::create(.3f, ccp(posUserArrow.x, posUserArrow.y - 20)), NULL))));
        sprUserArrowReal->runAction(CCSequence::create(CCDelayTime::create(0.5f), CCFadeIn::create(0.1f), NULL));
        
        //
        // 순위
        CCLabelTTF* labelOrder = (CCLabelTTF*)sprBg->getChildByTag(RANKUP_UI_TAG_ORDER);
        sprintf(lib->m_strTemp, "%d", rank->m_order);
        labelOrder->setString(lib->m_strTemp);
        
        //
        // 사진
        CCSprite* sprPictureFrame = (CCSprite*)sprBg->getChildByTag(RANKUP_UI_TAG_PICTUREFRAME);
        CCSprite* sprPicture = friendManager->getPicture(rank->m_friend, CCSizeMake(70, 70));
        if(sprPicture)
        {
            sprPictureFrame->addChild(sprPicture);
            sprPicture->setPosition(ccp(sprPictureFrame->getContentSize().width/2, sprPictureFrame->getContentSize().height/2));
        }
        
        //
        // 이름
        CCLabelTTF* labelName = (CCLabelTTF*)sprBg->getChildByTag(RANKUP_UI_TAG_NAME);
        labelName->setString(rank->m_friend->m_nickname.c_str());
        
        //
        // 점수
        CCLabelTTF* labelPoint = (CCLabelTTF*)sprBg->getChildByTag(RANKUP_UI_TAG_POINT);
        lib->setMoneyStyle(lib->m_strTemp, rank->m_point);
        labelPoint->setString(lib->m_strTemp);
    }
    
    schedule(schedule_selector(DyLayerGameResultRankup::cbTouchEnable), 1.f);
    
    setStartFireWorks();
    
    if(DyTycoonManager::sharedTycoonManager()->m_isCrazyMode)
        YtLib::sharedLib()->playEffect(YtLibSndEff_WOW);
}

void DyLayerGameResultRankup::cbTouchEnable(float dt)
{
    unschedule(schedule_selector(DyLayerGameResultRankup::cbTouchEnable));
    setTouchEnabled(true);
}


void DyLayerGameResultRankup::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
	CCSetIterator it;
	CCTouch* touch;
    
	for(it = touches->begin();it != touches->end();it++)
	{
		touch = (CCTouch*)(*it);
		if(!touch)
			break;
        
        if(m_isPaused)
            return;
   
        this->removeFromParentAndCleanup(true);
        
        g_layerGameResult->m_isRankup = false;
        g_layerGameResult->resume();
		return;
	}
}


void DyLayerGameResultRankup::endAnimationFire(CCNode* node)
{
	YtLib* lib = YtLib::sharedLib();
    
	CCSprite* sprEffect = CCSprite::createWithSpriteFrameName("fever_time_ani00.png");
	m_layerFire->addChild(sprEffect);
	
	int insectX = 50;
	int insectY = 50;
	int x = rand()%(int)(lib->m_sizeWin.width-insectX*2);
	int y = rand()%(int)(lib->m_sizeWin.height-insectY*2);
    
	x -= lib->m_sizeWin.width/2;
	y -= lib->m_sizeWin.height/2;
    
	sprEffect->setPosition(ccp(insectX + x, insectY + y));
	sprEffect->setScale(((float)(400 - rand()%200))/100.f);
    
	float delay = (rand()%10)*0.1f;
	sprEffect->setOpacity(0);
    
	CCCallFunc* callfun_effect = CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResultRankup::endAnimationFire));
    
	sprEffect->runAction(CCSequence::create(CCDelayTime::create(delay), CCFadeIn::create(.05f),
                                            CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_fever"), callfun_effect,
                                            NULL));
    YtLib::sharedLib()->playEffect(YtLibSndEff_FireFlower);
    
	if(node)
		node->removeFromParentAndCleanup(true);
}


void DyLayerGameResultRankup::setStartFireWorks()
{
	for(int i = 0;i < 30;i++)
	{
		endAnimationFire(NULL);
	}
}



SEL_MenuHandler DyLayerGameResultRankup::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName)
{
    return NULL;
}

SEL_CCControlHandler DyLayerGameResultRankup::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName)
{
	return NULL;
}

bool DyLayerGameResultRankup::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode)
{
    return false;
}


void DyLayerGameResultRankup::pause()
{
    m_isPaused = true;
}


void DyLayerGameResultRankup::resume()
{
    m_isPaused = false;
}


void DyLayerGameResultRankup::hide()
{
    //	YtLib* lib = YtLib::sharedLib();
    //
    //	runAction(CCSequence::create(
    //
    //		CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)),
    //		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
    //		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height)),
    //
    //		CCCallFunc::create(this, callfunc_selector(DyLayerOption::destory)),
    //		NULL));
}


void DyLayerGameResultRankup::show()
{
}


DyLayerGameResultRankup* DyLayerGameResultRankup::create(CCNode* owner)
{
	DyLayerGameResultRankup* layerGameResultRankup = (DyLayerGameResultRankup*)YtLayerManager::create("DyLayerRankup", DyLayerGameResultRankupLoader::loader(), "layer_rankup.ccbi", owner);
	return layerGameResultRankup;
}





DyLayerGameResult::DyLayerGameResult()
{
    m_tag = DYLAYERGAMERESULT;
	m_doggy = NULL;
	m_cntStar = 0;
    m_layerPointView = CCLayer::create();
    m_layerPointView->retain();
    g_layerGameResult = this;
    m_addGold = 0;
    m_isRankup = false;
}


DyLayerGameResult::~DyLayerGameResult()
{
    m_layerPointView->release();
    
	if(m_doggy)
		YtAniObject::unloadAniObject(m_doggy);
    
    g_layerGameResult = NULL;
}


//
// type : 0 Gold
// 1 : 하트 보너스
// 2 : 콤보
// 3 : 퀘스트
// 4 : 그냥 더하기
CCLayer* DyLayerGameResult::createLine(std::string tag, int value, int type, bool box)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    DyLib* lib = DyLib::sharedLib();
    CCLayer* layer = CCLayer::create();
    
    if(box)
    {
        CCSprite* sprBg = CCSprite::createWithSpriteFrameName("menu_ui_box_pattern.png");
        CCScale9Sprite *background = CCScale9Sprite::createWithSpriteFrame(sprBg->displayFrame());
        background->setContentSize(CCSizeMake(360, 45));
        layer->addChild(background);
    }

    CCLabelTTF* labelTag = CCLabelTTF::create(tag.c_str(), FONT_NAME_EXP, 30.f);
    labelTag->setAnchorPoint(ccp(0, 0.5));
    labelTag->setPosition(ccp(-170, 0));
    labelTag->setColor(ccWHITE);
    layer->addChild(labelTag);
    
    if(type == 1)
    {
        CCSprite* sprTag = CCSprite::createWithSpriteFrameName("game_ui_gage_heart.png");
        sprTag->setAnchorPoint(ccp(0, 0.5));
        sprTag->setPosition(ccp(-170, 0));
        layer->addChild(sprTag);
        labelTag->setPosition(ccp(-170+40, 0));
        
    }
    else if(type == 2)
    {
        CCSprite* sprTag = CCSprite::createWithSpriteFrameName("menu_ui_result_combo.png");
        sprTag->setAnchorPoint(ccp(0, 0.5));
        sprTag->setPosition(ccp(-170, 0));
        layer->addChild(sprTag);
    }
   
    CCLabelTTF* labelValue = CCLabelTTF::create("", FONT_NAME_DEFAULT, 30.f);
    labelValue->setColor(ccc3(255,204,1));

    labelValue->setAnchorPoint(ccp(1, 0.5));
    labelValue->setPosition(ccp(140, 0));
    layer->addChild(labelValue);
    
    if(type == 3 && tycoonManager->m_questProcessComplete == false)
    {
        labelValue->setFontName(FONT_NAME_EXP);
        
        if(tycoonManager->m_quest->m_questtype == DyQuestType_Ignore)
        {
            labelValue->setString("-");
            labelValue->setColor(ccWHITE);
        }
        else
        {
            labelValue->setString(STRING_game_result_quest_failed);
            labelValue->setColor(ccRED);
        }
        labelValue->setAnchorPoint(ccp(1, 0.5));
        labelValue->setPosition(ccp(170, 0));
    }
    else if(type == 3 || type == 0)
    {
        lib->setMoneyStyle(lib->m_strTemp, value);
        labelValue->setString(lib->m_strTemp);
        
        CCLabelTTF* labelTagG = CCLabelTTF::create("G", FONT_NAME_DEFAULT, 30.f);
        labelTagG->setColor(ccWHITE);
        
        labelTagG->setAnchorPoint(ccp(1, 0.5));
        labelTagG->setPosition(ccp(170, 0));
        layer->addChild(labelTagG);
    }
    else
    {
        lib->setMoneyStyle(lib->m_strTemp, value);
        sprintf(lib->m_strTemp2, "+%s", lib->m_strTemp);
        labelValue->setString(lib->m_strTemp2);
    }
    return layer;
}


void DyLayerGameResult::callfunc_update_result_success(float dt)
{
    this->unschedule(schedule_selector(DyLayerGameResult::callfunc_update_result_success));
    DyLib* lib = DyLib::sharedLib();
    
    
    //if(lib->m_main_server_error)
    {
        lib->show_network_alaram(false);
    
        if(DyTycoonManager::sharedTycoonManager()->m_isCrazyMode)
        {
            g_layerGameResult->startAnimationCrazy(NULL);
        }
        else
        {
            g_layerGameResult->startAni();
        }
    }
//    else
//    {
//        DyDataManager::sharedDataManager()->dataSave();
//        lib->network_update_result(httpresponse_selector(DyLayerGameResult::onHttpRequestCompleted));
//    }
}


void DyLayerGameResult::callfunc_update_result_fail(float dt)
{
    this->unschedule(schedule_selector(DyLayerGameResult::callfunc_update_result_fail));
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerGameResult::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 200);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerGameResult_callfunc_update_result(int value, int code)
{
    DyLayerGameResult *layerGameResult = (DyLayerGameResult*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMERESULT);
    
    if(value == 0)
    {
        layerGameResult->schedule(schedule_selector(DyLayerGameResult::callfunc_update_result_success), 0.05f);
    }
    else
    {
        layerGameResult->schedule(schedule_selector(DyLayerGameResult::callfunc_update_result_fail), 0.05f);
    }
}




void DyLayerGameResult_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str)
{
    DyLib* lib = DyLib::sharedLib();
    
    if(value_int != 0)
        return;
    
    switch(cmdType)
    {
        case DyNetworkCmdType_ScoreRegist:
        {
            lib->network_kakao_update_result(DyTycoonManager::sharedTycoonManager()->m_isCrazyMode, DyLayerGameResult_callfunc_update_result);
        }
        break;
        default:
            break;
    }
}


void DyLayerGameResult_callfunc_update_me3(int value, int code);

void DyLayerGameResult::result_update(CCNode* node)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(!tycoonManager->m_isCrazyMode)
    {
        setResult();
    }
   
    if(m_curStageState == DyStageState_Fail)
    {
        
        DyLib::sharedLib()->network_kakao_update_me(0, DyLayerGameResult_callfunc_update_me3);
    }
    else
    {
        DyLib::sharedLib()->show_network_alaram(true);

        
        DyStageManager* stageManager = DyStageManager::sharedStageManager();
        
        Json::Value score;
        
        score["shop"] = stageManager->m_curShopType;
        score["stage"] = stageManager->m_curLevel;
        score["score"] = dataManager->shop_get_record(stageManager->m_curShopType, stageManager->m_curLevel);
        
        Json::StyledWriter writer;
        std::string data = writer.write(score);
        
        DYNETWORK_ADD_PROC(DyNetworkCmdType_ScoreRegist, data, NULL, DyLayerGameResult_NetCallFunc);
    }
}


void DyLayerGameResult::startAnimationCrazy(CCNode* node)
{
    float delayLine = 0.2f;
    
    DyLib* lib = DyLib::sharedLib();
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    m_layerPointView->removeAllChildrenWithCleanup(true);
    CCLayer* layerLine[4] = {0, };
    
    sprintf(lib->m_strTemp2, "%02d", ((int)tycoonManager->m_dtOpenTime)/60);
    sprintf(lib->m_strTemp3, "%02d", ((int)tycoonManager->m_dtOpenTime)%60);
    sprintf(lib->m_strTemp, "%s(%s:%s)", STRING_TIME, lib->m_strTemp2, lib->m_strTemp3);
    
    
    layerLine[0] = createLine(lib->m_strTemp, tycoonManager->m_crazy_point_time, 4, false);
    layerLine[1] = createLine(STRING_SELL_GOLD, tycoonManager->m_crazy_point_gold, 4, false);
    layerLine[2] = createLine(STRING_REMOVE_BLOCK, tycoonManager->m_crazy_point_removeblock, 4, false);
    layerLine[3] = createLine("", tycoonManager->m_crazy_point_combo, 2, false);
    
    int offsetY = 200;
    
    for(int i = 0;i < 4;i++)
    {
        layerLine[i]->setVisible(false);
        
        
        layerLine[i]->setPosition(ccp(-20, offsetY));
        m_layerPointView->addChild(layerLine[i]);
        
        layerLine[i]->runAction(CCSequence::create(
                                                   CCDelayTime::create(delayLine*i),
                                                   CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationLine)),
                                                   CCMoveTo::create(delayLine, ccp(0, offsetY)),
                                                   NULL));
        if(i < 3)
            offsetY -= 40;
    }
    
    CCSprite* sprLine = CCSprite::createWithSpriteFrameName("menu_ui_result_line.png");
    sprLine->setVisible(false);
    offsetY -= 30;
    sprLine->setPosition(ccp(0, offsetY));
    m_layerPointView->addChild(sprLine);
    sprLine->runAction(CCSequence::create(
                                          CCDelayTime::create(delayLine*4),
                                          CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationLine)),
                                          NULL));
    CCSprite* sprTotalBg = CCSprite::createWithSpriteFrameName("crazy_result_score_pannel.png");
    sprTotalBg->setVisible(false);
    offsetY -= 80;
    sprTotalBg->setPosition(ccp(0, offsetY));
    m_layerPointView->addChild(sprTotalBg);
    sprTotalBg->runAction(CCSequence::create(
                                             CCDelayTime::create(delayLine*4),
                                             CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationLine)),
                                             NULL));
    
    m_labelTotalPoint = CCLabelBMFont::create(lib->m_strTemp,  "font_total.fnt");
	m_labelTotalPoint->setPosition(ccp(0, offsetY - 5));
	m_layerPointView->addChild(m_labelTotalPoint);
    
    lib->setMoneyStyle(lib->m_strTemp, tycoonManager->m_current_gold);
    m_labelTotalPoint->setString(lib->m_strTemp);
    
    m_labelTotalPoint->setVisible(false);
    m_labelTotalPoint->runAction(CCSequence::create(
                                                    CCDelayTime::create(delayLine*4),
                                                    CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationLine)),
                                                    CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::startCrazyScroeAni)),
                                                    NULL));
}


void DyLayerGameResult::startCrazyScroeAni(CCNode* node)
{
    animationTotalStart(0);
}


void DyLayerGameResult::endAnimationFirstView(CCNode* node)
{
    float delayLine = 0.2f;
    
    DyLib* lib = DyLib::sharedLib();
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    m_layerPointView->removeAllChildrenWithCleanup(true);
    CCLayer* layerLine[2] = {0, };
    
    layerLine[0] = createLine(STRING_game_result_totalgold, tycoonManager->m_current_gold, 0, false);
    //layerLine[1] = createLine(STRING_game_result_heartbonus, tycoonManager->m_bouns_heart_point, 1, false);
    layerLine[1] = createLine("", tycoonManager->m_combo_point, 2, false);
    
    layerLine[0]->setPosition(ccp(0, -140));
    m_layerPointView->addChild(layerLine[0]);
    
    int offsetY = 120;
    
    layerLine[0]->runAction(CCSequence::create(
                                               CCMoveTo::create(delayLine, ccp(0, offsetY)),
                                               NULL));

    for(int i = 1;i < 2;i++)
    {
        layerLine[i]->setVisible(false);
        
        offsetY -= 43;
        
        layerLine[i]->setPosition(ccp(-20, offsetY));
        m_layerPointView->addChild(layerLine[i]);
        
        layerLine[i]->runAction(CCSequence::create(
                                                   CCDelayTime::create(delayLine*i),
                                                   CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationLine)),
                                                   CCMoveTo::create(delayLine, ccp(0, offsetY)),
                                                   NULL));
    }
    offsetY -= 43;
    
    
    CCSprite* sprLine = CCSprite::createWithSpriteFrameName("menu_ui_result_line.png");
    sprLine->setVisible(false);
    offsetY -= 35;
    sprLine->setPosition(ccp(0, offsetY));
    m_layerPointView->addChild(sprLine);
    sprLine->runAction(CCSequence::create(
                                          CCDelayTime::create(delayLine*3),
                                          CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationLine)),
                                          NULL));
    CCSprite* sprTotalBg = CCSprite::createWithSpriteFrameName("menu_ui_result_total_point.png");
    sprTotalBg->setVisible(false);
    offsetY -= 76;
    sprTotalBg->setPosition(ccp(0, offsetY));
    m_layerPointView->addChild(sprTotalBg);
    sprTotalBg->runAction(CCSequence::create(
                                          CCDelayTime::create(delayLine*3),
                                          CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationLine)),
                                          NULL));
    
    
    
    CCSprite* sprIncomingGoldPanel = (CCSprite*)getChildByTag(GAMERESULT_UI_TAG_GOLD_BG);
    sprIncomingGoldPanel->setPosition(ccp(-150, -176));
    sprIncomingGoldPanel->setOpacity(0);
    
    sprIncomingGoldPanel->runAction(CCSequence::create(CCDelayTime::create(delayLine*3), CCMoveTo::create(0.2f, ccp(0, -176)), NULL));
    sprIncomingGoldPanel->runAction(CCSequence::create(CCDelayTime::create(delayLine*3), CCFadeIn::create(0.1f), NULL));

    
    CCLabelTTF* labelGold = ((CCLabelTTF*)getChildByTag(GAMERESULT_UI_TAG_GOLD_BG)->getChildByTag(GAMERESULT_UI_TAG_GOLD_TEXT));
    labelGold->setOpacity(0);
    labelGold->runAction(CCSequence::create(CCDelayTime::create(delayLine*3), CCFadeIn::create(0.1f), NULL));

    
    
    
    m_labelTotalPoint = CCLabelBMFont::create(lib->m_strTemp,  "font_total.fnt");
	m_labelTotalPoint->setPosition(ccp(0, offsetY - 5));
	m_layerPointView->addChild(m_labelTotalPoint);
    
    lib->setMoneyStyle(lib->m_strTemp, tycoonManager->m_current_gold);
    m_labelTotalPoint->setString(lib->m_strTemp);
    
    m_labelTotalPoint->setVisible(false);
    m_labelTotalPoint->runAction(CCSequence::create(
                                             CCDelayTime::create(delayLine*3),
                                             CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationLine)),
                                             //CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::startAddStar)),
                                             NULL));

    switch(m_curStageState)
	{
		case DyStageState_Goal:
			runAction(CCSequence::create(
                                         CCDelayTime::create(0.4*3.f),
                                         CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::startAddStar_0)),
                                         CCDelayTime::create(2.5f),
                                         CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::startDoggyAniGoal)),
                                         
                                         NULL));
            
            
			break;
		case DyStageState_Goal_Plus:
			runAction(CCSequence::create(
                                         CCDelayTime::create(0.4*3.f),
                                         CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::startAddStar_0)),
                                         CCDelayTime::create(0.3f),
                                         CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::startAddStar_1)),
                                         CCDelayTime::create(2.5f),
                                         CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::startDoggyAniGoal)),
                                         NULL));
            
			break;
		case DyStageState_Perfect:
			runAction(CCSequence::create(
                                         CCDelayTime::create(0.4*3.f),
                                         CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::startAddStar_0)),
                                         CCDelayTime::create(0.3f),
                                         CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::startAddStar_1)),
                                         CCDelayTime::create(0.3f),
                                         CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::startAddStar_2)),
                                         CCDelayTime::create(2.5f),
                                         CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::startDoggyAniPerfect)),
                                         NULL));
			break;
        default:
            break;
	}
    
    m_cntStar = 0;
}


void DyLayerGameResult::startIncomingGold()
{
    YtLib* lib = YtLib::sharedLib();
    
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    int incomingGold = 0;
        
    if(m_curStageState == DyStageState_Goal)
        incomingGold = (tycoonManager->m_current_gold*STORY_INCOMING_GOLD_GOAL)/100.f;
    else if(m_curStageState == DyStageState_Goal_Plus)
        incomingGold = (tycoonManager->m_current_gold*STORY_INCOMING_GOLD_GOAL_PLUS)/100.f;
    else if(m_curStageState == DyStageState_Perfect)
        incomingGold = (tycoonManager->m_current_gold*STORY_INCOMING_GOLD_PERFECT)/100.f;
    
    lib->setMoneyStyle(lib->m_strTemp, incomingGold);
    ((CCLabelTTF*)getChildByTag(GAMERESULT_UI_TAG_GOLD_BG)->getChildByTag(GAMERESULT_UI_TAG_GOLD_TEXT))->setString(lib->m_strTemp);
  
    CCSprite* sprIncomingGoldPanel = (CCSprite*)getChildByTag(GAMERESULT_UI_TAG_GOLD_BG);
    sprIncomingGoldPanel->setPosition(ccp(0, -176));
    
    sprIncomingGoldPanel->runAction(CCSequence::create(CCDelayTime::create(0.15f), CCMoveTo::create(0.2f, ccp(-100, -176)), NULL));
    
    
    
    
    CCMenuItemImage* menuitemPride = (CCMenuItemImage*)m_menu->getChildByTag(GAMERESULT_UI_TAG_PRIDE);
    menuitemPride->setPosition(ccp(150, -176));
    
    menuitemPride->setOpacity(0);

    menuitemPride->runAction(CCSequence::create(CCDelayTime::create(0.2f), CCMoveTo::create(0.2f, ccp(124, -176)), NULL));
    menuitemPride->runAction(CCSequence::create(CCDelayTime::create(0.2f), CCFadeIn::create(0.1f), NULL));
}


void DyLayerGameResult::startAddStar(int index)
{
    CCPoint posTargte = ccp(0, -96);
    
	CCSprite* sprBg = (CCSprite*)getChildByTag(GAMERESULT_UI_TAG_STAR0+index);
	CCSprite* sprStar = CCSprite::createWithSpriteFrameName("menu_ui_star_highlight.png");
    
	sprStar->setPosition(sprBg->getPosition());
    
	sprStar->runAction(CCSequence::create(
                                          CCMoveTo::create(0.4f, posTargte),
                                          CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAddStar)),
                                          NULL));
	addChild(sprStar);
    
	CCSprite* sprEffect = CCSprite::createWithSpriteFrameName("threematch_ui_effect_bang00.png");
	sprEffect->setAnchorPoint(ccp(0.5f, 0.5f));
	sprEffect->setPosition(posTargte);
	addChild(sprEffect);
    
	sprEffect->setOpacity(0);
	sprEffect->runAction(CCSequence::create(
                                            CCDelayTime::create(0.4f),
                                            CCFadeIn::create(.05f),
                                            CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_bang"),
                                            CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAddStarEffect)), 
                                            NULL));
    
    sprBg->setOpacity(0);
	sprBg->runAction(CCSequence::create(CCDelayTime::create(0.3f),
                                        CCFadeIn::create(.4f), NULL));
}


void DyLayerGameResult::startAddStar_0()
{
	startAddStar(0);
}


void DyLayerGameResult::startAddStar_1()
{
	startAddStar(1);
}


void DyLayerGameResult::startAddStar_2()
{
	startAddStar(2);
}


void DyLayerGameResult::animationTotalStart(int startPoint)
{
    m_curTotal = startPoint;
    YtLib::sharedLib()->playEffect(YtLibSndEff_POINT_CALC);
    schedule(schedule_selector(DyLayerGameResult::cbUpdate), 0.1f);
}


void DyLayerGameResult::cbUpdate(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(tycoonManager->m_isCrazyMode)
    {
        m_curTotal += (tycoonManager->m_crazy_point)/12;
        
        if(m_curTotal > tycoonManager->m_crazy_point)
        {
            m_curTotal = tycoonManager->m_crazy_point;
            m_menu->setVisible(true);
            showNewScore();
            checkRankup();
            unschedule(schedule_selector(DyLayerGameResult::cbUpdate));
            
            m_labelTotalPoint->runAction(CCSequence::create(CCScaleTo::create(0.08f, 1.3f), CCScaleTo::create(0.05f, 1.f), NULL));

        }
    }
    else
    {
        m_curTotal += (tycoonManager->m_bouns_heart_point+tycoonManager->m_combo_point)/12;
        
        if(m_curTotal > tycoonManager->m_stage_point)
        {
            m_curTotal = tycoonManager->m_stage_point;
            m_menu->setVisible(true);
            showNewScore();
            checkRankup();
            checkReview();
            unschedule(schedule_selector(DyLayerGameResult::cbUpdate));
            m_labelTotalPoint->runAction(CCSequence::create(CCScaleTo::create(0.08f, 1.3f), CCScaleTo::create(0.05f, 1.f), NULL));
        }
    }
    
    lib->setMoneyStyle(lib->m_strTemp, m_curTotal);
    m_labelTotalPoint->setString(lib->m_strTemp);
}



void DyLayerGameResult::endAddStar(CCNode* node)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    YtLib* lib = YtLib::sharedLib();
  
    int incGold = 0;
    if(m_cntStar == 0)
    {
        incGold = ((tycoonManager->m_current_gold*STORY_INCOMING_GOLD_GOAL)/100.f);

        m_addGold += incGold;
        if(m_curStageState == DyStageState_Goal)
            m_addGold = (tycoonManager->m_current_gold*STORY_INCOMING_GOLD_GOAL)/100.f;
    }
    else if(m_cntStar == 1)
    {
        incGold = (tycoonManager->m_current_gold*(STORY_INCOMING_GOLD_GOAL_PLUS - STORY_INCOMING_GOLD_GOAL))/100.f;
        m_addGold += incGold;
        
        if(m_curStageState == DyStageState_Goal_Plus)
            m_addGold = (tycoonManager->m_current_gold*STORY_INCOMING_GOLD_GOAL_PLUS)/100.f;
        
    }
    else
    {
        incGold = (tycoonManager->m_current_gold*(STORY_INCOMING_GOLD_PERFECT - STORY_INCOMING_GOLD_GOAL_PLUS))/100.f;
        m_addGold += incGold;
        if(m_curStageState == DyStageState_Perfect)
            m_addGold = (tycoonManager->m_current_gold*STORY_INCOMING_GOLD_PERFECT)/100.f;
    }
    
    lib->setMoneyStyle(lib->m_strTemp2, incGold);
    sprintf(lib->m_strTemp, "+%s", lib->m_strTemp2);
    
    CCLabelTTF* labelPlusGold = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 28);
    //labelPlusGold->setColor(ccc3(227,180,0));
    labelPlusGold->setColor(ccc3(255,224,0));
    
    labelPlusGold->setAnchorPoint(ccp(1, 0.5));
    labelPlusGold->setPosition(ccp(105, -150));
    addChild(labelPlusGold);
    
    labelPlusGold->runAction(CCSequence::create(CCDelayTime::create(0.25), CCFadeOut::create(0.1), NULL));
    labelPlusGold->runAction(CCSequence::create(CCMoveTo::create(0.35, ccp(105, -130)), NULL));
    
    
    lib->setMoneyStyle(lib->m_strTemp, m_addGold);
    ((CCLabelTTF*)getChildByTag(GAMERESULT_UI_TAG_GOLD_BG)->getChildByTag(GAMERESULT_UI_TAG_GOLD_TEXT))->setString(lib->m_strTemp);
  
    
    YtLib::sharedLib()->playEffect((YtLibSndEff)((int)YtLibSndEff_STAR_PANG0+m_cntStar));
	
    node->removeFromParentAndCleanup(true);
  
    int cur_point = tycoonManager->m_current_gold;
    
    switch(m_curStageState)
    {
        case DyStageState_Goal:
            cur_point += STAR_POINT1;
            animationTotalStart(cur_point);
            //getChildByTag(GAMERESULT_UI_TAG_STAR0)->setVisible(true);
            break;
        case DyStageState_Goal_Plus:
            switch(m_cntStar)
            {
                case 0:
                    cur_point += STAR_POINT1;
                    break;
                default:
                    cur_point += (STAR_POINT1 + STAR_POINT2);
                    animationTotalStart(cur_point);
                    //getChildByTag(GAMERESULT_UI_TAG_STAR0)->setVisible(true);
                    //getChildByTag(GAMERESULT_UI_TAG_STAR1)->setVisible(true);
                    break;
            }
            
            break;
        case DyStageState_Perfect:
            switch(m_cntStar)
            {
                case 0:
                    cur_point += STAR_POINT1;
                    break;
                case 1:
                    cur_point += (STAR_POINT1 + STAR_POINT2);
                    break;
                default:
                    cur_point += (STAR_POINT1 + STAR_POINT2 + STAR_POINT3);
                    animationTotalStart(cur_point);
                    
                    //getChildByTag(GAMERESULT_UI_TAG_STAR0)->setVisible(true);
                    //getChildByTag(GAMERESULT_UI_TAG_STAR1)->setVisible(true);
                    //getChildByTag(GAMERESULT_UI_TAG_STAR2)->setVisible(true);
                    break;
            }break;
        default:
            animationTotalStart(cur_point);
            break;
    }

    
    lib->setMoneyStyle(lib->m_strTemp, cur_point);
    m_labelTotalPoint->setString(lib->m_strTemp);
    
	m_cntStar++;
}



void DyLayerGameResult::endAddStarEffect(CCNode* node)
{
	node->removeFromParentAndCleanup(true);
}


void DyLayerGameResult::setPointView()
{
    //DyLib* lib = DyLib::sharedLib();
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    float delayLine = 0.2f;
    
    m_layerPointView->removeAllChildrenWithCleanup(true);
    
    CCLayer* layerLine[6] = {0, };
    
    layerLine[0] = createLine(STRING_game_result_earn, tycoonManager->m_sold_gold, 0, false);
    layerLine[1] = createLine(STRING_game_result_tip, tycoonManager->m_tip_gold, 0, false);
    layerLine[2] = createLine(STRING_game_result_questbonus, tycoonManager->m_questbonus_gold, 3, false);
    layerLine[3] = createLine(STRING_game_result_lastbonus, tycoonManager->m_bonustime_gold, 0, false);
    layerLine[4] = createLine(STRING_game_result_cashier, -tycoonManager->m_cashier_gold, 0, false);
    layerLine[5] = createLine(STRING_game_result_totalgold, tycoonManager->m_current_gold, 0, true);
 
    CCSprite* sprLine = CCSprite::createWithSpriteFrameName("menu_ui_result_line.png");
    sprLine->setVisible(false);
    
    int offsetY = 150;
    for(int i = 0;i < 6;i++)
    {
        layerLine[i]->setVisible(false);
        if(i == 5)
        {
            offsetY -= 25;
        
            sprLine->setPosition(ccp(0, offsetY));
             m_layerPointView->addChild(sprLine);
            
            
            sprLine->runAction(CCSequence::create(
                                                  CCDelayTime::create(delayLine*i),
                                                  CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationTotalLine)),
                                                  NULL));
            
            offsetY -= 40;
        }
        else
            offsetY -= 45;
        
        layerLine[i]->setPosition(ccp(-20, offsetY));
        m_layerPointView->addChild(layerLine[i]);
        
        
        layerLine[i]->runAction(CCSequence::create(
                                                   CCDelayTime::create(delayLine*i),
                                                   CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationLine)),
                                                   CCMoveTo::create(delayLine, ccp(0, offsetY)), 
                                                   NULL));
    }
    
    m_layerPointView->runAction(CCSequence::create(
                                               CCDelayTime::create(delayLine*6+1.5),
                                               CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationFirstView)),
                                                   NULL));
}


void DyLayerGameResult::endAnimationTotalLine(CCNode* node)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_MONEY_REGISTER_BYCHASIER);
}


void DyLayerGameResult::endAnimationLine(CCNode* node)
{
    node->setVisible(true);
    //YtLib::sharedLib()->playEffect(YtLibSndEff_GIVE_SIDE);
}


void DyLayerGameResult::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
	DyLib* lib = DyLib::sharedLib();

	DyStageManager* stageManager = DyStageManager::sharedStageManager();

	m_menu = ((CCMenu*)getChildByTag(GAMERESULT_UI_TAG_MENU));

	//
	// stage
	((CCLabelTTF*)getChildByTag(GAMERESULT_UI_TAG_STAGENAME))->setFontName(FONT_NAME_DEFAULT);
	((CCLabelTTF*)getChildByTag(GAMERESULT_UI_TAG_STAGENAME))->setColor(ccc3(255,153,3));
	CCLabelTTF* labelStage = (CCLabelTTF *)getChildByTag(GAMERESULT_UI_TAG_STAGEVALUE);
	labelStage->setFontName(FONT_NAME_DEFAULT);
	sprintf(lib->m_strTemp, "%02d", stageManager->m_curLevel+1);
	labelStage->setString(lib->m_strTemp);

    addChild(m_layerPointView);
    
	m_doggy = YtAniObject::loadAniObject(this, "node_animal_doggy.ccbi");
	m_doggy->setPosition(ccp(266+100, -324));
	addChild(m_doggy);
	DyCostumeManager::sharedCostumeManager()->setCostume(m_doggy, &DyDataManager::sharedDataManager()->m_public.m_costume_status, &DyDataManager::sharedDataManager()->m_custume_function);
    
    
    CCSprite* sprTextBox = (CCSprite*)getChildByTag(GAMERESULT_UI_TAG_NEWSCOREBOX);
    reorderChild(sprTextBox, 1000);
    sprTextBox->setVisible(false);
    
    m_menu->setVisible(false);
    
    ((CCLabelTTF*)getChildByTag(GAMERESULT_UI_TAG_GOLD_BG)->getChildByTag(GAMERESULT_UI_TAG_GOLD_TEXT))->setFontName(FONT_NAME_DEFAULT);
	((CCLabelTTF*)getChildByTag(GAMERESULT_UI_TAG_DELLY_BG)->getChildByTag(GAMERESULT_UI_TAG_DELLY_TEXT))->setFontName(FONT_NAME_DEFAULT);
	
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(tycoonManager->m_isCrazyMode)
    {
        //sprintf(lib->m_strTemp, "%d", tycoonManager->m_crazy_bonusGold);
        lib->setMoneyStyle(lib->m_strTemp, tycoonManager->m_crazy_bonusGold);
        ((CCLabelTTF*)getChildByTag(GAMERESULT_UI_TAG_GOLD_BG)->getChildByTag(GAMERESULT_UI_TAG_GOLD_TEXT))->setString(lib->m_strTemp);
        
        
        sprintf(lib->m_strTemp, "%d", tycoonManager->m_crazy_bonusDelly);
        ((CCLabelTTF*)getChildByTag(GAMERESULT_UI_TAG_DELLY_BG)->getChildByTag(GAMERESULT_UI_TAG_DELLY_TEXT))->setString(lib->m_strTemp);
    }
    
    getChildByTag(GAMERESULT_UI_TAG_NEEDDELLY)->setVisible(false);
    
    setKeypadEnabled(true);
}


SEL_MenuHandler DyLayerGameResult::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onShopClicked", DyLayerGameResult::onShopClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onMenuClicked", DyLayerGameResult::onMenuClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onReplayClicked", DyLayerGameResult::onReplayClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onNextClicked", DyLayerGameResult::onNextClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onFacebookClicked", DyLayerGameResult::onFacebookClicked);

    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPrideClicked", DyLayerGameResult::onPrideClicked);
    return NULL;
}


SEL_CCControlHandler DyLayerGameResult::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}

bool DyLayerGameResult::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}

void DyLayerGameResult::onShopClicked(CCObject* pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Normal, 0, 0);
	YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
}



void DyLayerGameResult::cbReward(float dt)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    dataManager->m_private.m_rewardedReview = true;

        
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    
    unschedule(schedule_selector(DyLayerGameResult::cbReward));
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerGameResult::onPopupClicked));

    popup->setText(STRING_REWARD_RESULT);
    
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 300);
    
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
    moneyManager->addDelly(5, true);
}




void DyLayerGameResult::callfunc_update_me_success(float dt)
{
    unschedule(schedule_selector(DyLayerGameResult::callfunc_update_me_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    //YtLayerManager::sharedLayerManager()->popLayer();
}



void DyLayerGameResult::callfunc_update_me_fail(float dt)
{
    unschedule(schedule_selector(DyLayerGameResult::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
//    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShop::onPopupClicked));
//    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
//    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
//    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerGameResult_callfunc_update_me(int value, int code)
{
    DyLayerGameResult *layerGameResult = (DyLayerGameResult*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMERESULT);
    
    if(value == 0)
    {
        layerGameResult->schedule(schedule_selector(DyLayerGameResult::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layerGameResult->schedule(schedule_selector(DyLayerGameResult::callfunc_update_me_fail), 0.05f);
    }
}




void DyLayerGameResult::callfunc_update_me_success2(float dt)
{
    DyLayerGameResult *layerGameResult = (DyLayerGameResult*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMERESULT);
    
    unschedule(schedule_selector(DyLayerGameResult::callfunc_update_me_success2));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    layerGameResult->endProcess();
}



void DyLayerGameResult::callfunc_update_me_fail2(float dt)
{
    unschedule(schedule_selector(DyLayerGameResult::callfunc_update_me_fail2));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
}


void DyLayerGameResult_callfunc_update_me2(int value, int code)
{
    DyLayerGameResult *layerGameResult = (DyLayerGameResult*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMERESULT);
    
    if(value == 0)
    {
        layerGameResult->schedule(schedule_selector(DyLayerGameResult::callfunc_update_me_success2), 0.05f);
    }
    else
    {
        layerGameResult->schedule(schedule_selector(DyLayerGameResult::callfunc_update_me_fail2), 0.05f);
    }
}


void DyLayerGameResult::callfunc_update_me_success3(float dt)
{
    DyLayerGameResult *layerGameResult = (DyLayerGameResult*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMERESULT);
    
    unschedule(schedule_selector(DyLayerGameResult::callfunc_update_me_success3));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(tycoonManager->m_isCrazyMode)
    {
        layerGameResult->startAnimationCrazy(NULL);
    }
    else
    {
        layerGameResult->startAni();
    }
}



void DyLayerGameResult::callfunc_update_me_fail3(float dt)
{
    unschedule(schedule_selector(DyLayerGameResult::callfunc_update_me_fail3));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
}


void DyLayerGameResult_callfunc_update_me3(int value, int code)
{
    DyLayerGameResult *layerGameResult = (DyLayerGameResult*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERGAMERESULT);
    
    if(value == 0)
    {
        layerGameResult->schedule(schedule_selector(DyLayerGameResult::callfunc_update_me_success3), 0.05f);
    }
    else
    {
        layerGameResult->schedule(schedule_selector(DyLayerGameResult::callfunc_update_me_fail3), 0.05f);
    }
}

void DyLayerGameResult::onPopupClicked(cocos2d::CCObject * pSender)
{
	//DyLib* lib = DyLib::sharedLib();
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
	DyStageManager* stageManager = DyStageManager::sharedStageManager();

	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
	if(pPopup->m_active_tag == 10)
	{
        dataManager->shop_set_viewending(stageManager->m_curShopType, true);
 		DyLib::sharedLib()->setStatus(DyLib_AppStatus_Stage, stageManager->m_curShopType+1);
	}
	if(pPopup->m_active_tag == 20/* || pPopup->m_active_tag == 30*/)
	{
        dataManager->shop_set_viewending(stageManager->m_curShopType, true);
        
 		DyLib::sharedLib()->setStatus(DyLib_AppStatus_Story, stageManager->m_curShopType+1);
	}
    if(pPopup->m_active_tag == 31)
    {
        DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
        
        if(moneyManager->getDelly() < m_stage_pass_delly)
        {
            DyMoneyManager::sharedMoneyManager()->alramNotEnoughDelly(true, true);
        }
        else
        {
            stageManager->nextStageOpen();
            moneyManager->addDelly(-m_stage_pass_delly, true);
            m_menu->setEnabled(false);
            m_closeandcommand = 2;
            
            if(stageManager->getStageState(stageManager->m_curShopType, stageManager->m_curLevel) == DyStageState_Fail)
            {
                stageManager->setStageState(stageManager->m_curShopType, stageManager->m_curLevel, DyStageState_Fail_Pass);
            }
            
            DyLib::sharedLib()->network_kakao_update_me(0, DyLayerGameResult_callfunc_update_me2);
            
        }
    }
    if(pPopup->m_active_tag == 100)
    {
        YtPlatform_GoWeb(dataManager->m_link.c_str());
        
        schedule(schedule_selector(DyLayerGameResult::cbReward), .5f);
    }
    if(pPopup->m_active_tag == 200)
    {
        result_update(NULL);
    }
    if(pPopup->m_active_tag == 300)
    {
        DyLib::sharedLib()->network_kakao_update_me(0, DyLayerGameResult_callfunc_update_me);
    }
}


void DyLayerGameResult::destory()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);

	switch(m_closeandcommand)
	{
	case 0://go menu
		{
			DyStageManager* stageManager = DyStageManager::sharedStageManager();
			m_menu->setEnabled(false);
			//DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
			//tycoonManager->m_hasBonusGum = false;
			DyLib::sharedLib()->setStatus(DyLib_AppStatus_Stage, 10+stageManager->m_curShopType);
		}
		break;
	case 1://replay
		{
			m_menu->setEnabled(false);
			//DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
			//tycoonManager->m_hasBonusGum = false;
			DyLib::sharedLib()->setStatus(DyLib_AppStatus_Tip, 0);
		}
		break;
	case 2://next
		{
			m_menu->setEnabled(false);
			DyStageManager* stageManager = DyStageManager::sharedStageManager();
		
            if(stageManager->m_curLevel != stageManager->m_cntLevel - 1 && stageManager->m_curLevel + 1 < stageManager->m_cntLevel + stageManager->m_cntBonusLevel)
			{
				stageManager->setLevel(stageManager->m_curLevel + 1);
				DyLib::sharedLib()->setStatus(DyLib_AppStatus_Tip, 0);
			}
			else
			{
				DyLib* lib = DyLib::sharedLib();
                
                /*
                if(stageManager->m_cntBonusLevel > 0 && stageManager->m_curLevel >= stageManager->m_cntLevel + stageManager->m_cntBonusLevel-1)
                {
                    DyPopup *popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerGameResult::onPopupClicked), CCSizeMake(440, 350));
					popup->setText("보너스 스테이지까지 모드 클리어 하셨어요.^^");
					popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 30);
                    
					YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
                }
				else
                */
                if(stageManager->m_curShopType == (DyStageType)(SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED-1))
				{
					DyPopup *popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerGameResult::onPopupClicked));
                    
					popup->setText(STRING_clear_all_shop);
					
                    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 20);

					YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
				}
				else
				{
					DyPopup *popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerGameResult::onPopupClicked), CCSizeMake(440, 350));
                    
                    if(stageManager->m_cntBonusLevel > 0)
                    {
                        sprintf(lib->m_strTemp, "축하합니다! 이제 %s(을)를 오픈할 수 있어요! %s의 모든 스테이지를 클리어하셨습니다. 보너스 스테이지도 클리어 해보세요~",
                                stageManager->getShopName(stageManager->m_curShopType+1).c_str(),
                                stageManager->getShopName(stageManager->m_curShopType).c_str());

                    }
                    else
                        sprintf(lib->m_strTemp, STRING_clear_all_stage,
                                stageManager->getShopName(stageManager->m_curShopType).c_str(),
                                stageManager->getShopName(stageManager->m_curShopType+1).c_str());

					popup->setText(lib->m_strTemp);
					popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 20);

					YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
                }
                YtLib::sharedLib()->playEffect(YtLibSndEff_WOW);
			}
		}
		break;
	}
}


void DyLayerGameResult::onMenuClicked(CCObject* pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    m_menu->setEnabled(false);
	m_closeandcommand = 0;
	endProcess();
    
}

void DyLayerGameResult::keyBackClicked()
{
    if(m_menu->isVisible())
    {
        setKeypadEnabled(false);
        onMenuClicked(NULL);
    }
}


void DyLayerGameResult::onReplayClicked(CCObject* pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
     m_menu->setEnabled(false);
	m_closeandcommand = 1;
	endProcess();
    

}





//void DyLayerGameResult::onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response)
//{
//    DyLib* lib = DyLib::sharedLib();
//    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
//    
//    lib->show_network_alaram(false);
//    
//    if(lib->network_checkError(response, true) == false)
//    {
//    }
//    else if(lib->network_getTag(response).compare(NETWORK_SERVICE_UPDATE_RESULT) == 0)
//    {
//        lib->network_update_result_ret(sender, response);
//    }
//    
//    if(DyTycoonManager::sharedTycoonManager()->m_isCrazyMode)
//    {
//        g_layerGameResult->startAnimationCrazy(NULL);
//    }
//    else
//    {
//        g_layerGameResult->startAni();
//    }
//}


void DyLayerGameResult::endProcess()
{
    YtLayerManager::sharedLayerManager()->popLayer();
}


void DyLayerGameResult::onNextClicked(CCObject* pSender)
{
    DyLib* lib = DyLib::sharedLib();
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    
    if(m_curStageState == DyStageState_Fail)
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerGameResult::onPopupClicked));
        
        sprintf(lib->m_strTemp, STRING_GAMERESULT_DELLY, m_stage_pass_delly);
        popup->setText(lib->m_strTemp);
        
        popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 30);
        popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 31);
        popup->addButton(DyButtonType_Close, "", 30);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
    else
    {
        m_menu->setEnabled(false);
        m_closeandcommand = 2;
        endProcess();
    }
}


void DyLayerGameResult::onFacebookClicked(CCObject* pSender)
{
}


void DyLayerGameResult::onPrideClicked(CCObject* pSender)
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

    //
    // 어플 실행하자마자 크레이지모드 플레이하면, 크레이지 리더보드가 생성안된상태이기때문에 무조건 스테이지모드로 리스트를 보여줘야한다.
    //DyLayerPride* layerPride = DyLayerPride::create(this, tycoonManager->m_isCrazyMode ? DyRankType_Crazy : DyRankType_Stage, lib->m_strTemp);
    DyLayerPride* layerPride = DyLayerPride::create(this, DyRankType_Stage, m_curTotal);
    YtLayerManager::sharedLayerManager()->pushLayer(layerPride, true, true);
}


void DyLayerGameResult::attachStar(int index)
{
	CCSprite* sprBg = (CCSprite*)getChildByTag(GAMERESULT_UI_TAG_STAR_BG0+index);
	CCSprite* sprStar = CCSprite::createWithSpriteFrameName("menu_ui_star_highlight.png");

	sprStar->setPosition(sprBg->getPosition());
    sprStar->setTag(GAMERESULT_UI_TAG_STAR0+index);

	addChild(sprStar);
}


void DyLayerGameResult::startDoggyAniGoal()
{
    startIncomingGold();
	m_doggy->m_actionManager->runAnimations("goal");
    YtLib::sharedLib()->playEffect(YtLibSndEff_WOW);
    //m_menu->setVisible(true);
    //showNewScore();
}


void DyLayerGameResult::startDoggyAniPerfect()
{
    startIncomingGold();
	setStartFireWorks();
	m_doggy->m_actionManager->runAnimations("perfect");
    YtLib::sharedLib()->playEffect(YtLibSndEff_WOW);
    //m_menu->setVisible(true);
    //showNewScore();
}


void DyLayerGameResult::endAnimationFailed(CCNode* node)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_DOJANG);
}


void DyLayerGameResult::drawFailStamp(CCPoint position, const char* strRes, float delay)
{
    CCSprite* sprResultMark = CCSprite::createWithSpriteFrameName(strRes);
    addChild(sprResultMark);
    sprResultMark->setOpacity(0);
    sprResultMark->setScale(1.2);
    sprResultMark->setPosition(position);
    sprResultMark->runAction(CCSequence::create(CCDelayTime::create(delay), CCFadeIn::create(0.01), CCScaleTo::create(0.05, 1.0f), CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationFailed)), NULL));
}


void DyLayerGameResult::showQuestPopup(float dt)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

    
    
    unschedule(schedule_selector(DyLayerGameResult::showQuestPopup));
    ((DyLayerGame*)tycoonManager->m_layer)->showQuestPopup(true, false);
    
 
    tycoonManager->m_questbar->removeFromParentAndCleanup(false);
    addChild(tycoonManager->m_questbar, ZORDER_GAME_TUTORIAL);
    
    CCPoint pos = tycoonManager->m_questbar->getPosition();
    tycoonManager->m_questbar->setPosition(ccp(pos.x - 480, pos.y - 320));
    
    CCPoint posSource = ccp(370, 230);
    CCPoint posTarget;
    
    CCSprite* sprIndicator = CCSprite::createWithSpriteFrameName("menu2_ui_tutorial_arrow.png");
    
    posTarget = ccp(posSource.x, posSource.y+15);
    
    sprIndicator->setPosition(posSource);
    
    sprIndicator->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(
                                                                                           CCMoveTo::create(.15f, posTarget),
                                                                                           CCMoveTo::create(.05f, posSource),
                                                                                           CCDelayTime::create(.25f),
                                                                                           NULL))));
    
    
    addChild(sprIndicator, ZORDER_GAME_TUTORIAL);
    
    
}


void DyLayerGameResult::startFailEffect()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

    if(tycoonManager->m_failWithQuest)
    {
        if(tycoonManager->m_current_gold < tycoonManager->m_goal_gold)
        {
            drawFailStamp(ccp(8,-2+60), "menu_ui_failed_quest_stamp.png", 0.1);
            drawFailStamp(ccp(8,-2-40), "menu_ui_failed_goal_stamp.png", 0.6);
            schedule(schedule_selector(DyLayerGameResult::showQuestPopup), .6f);
        }
        else
        {
            drawFailStamp(ccp(8,-2), "menu_ui_failed_quest_stamp.png", 0.1);
            schedule(schedule_selector(DyLayerGameResult::showQuestPopup), .1f);
        }
    }
    else
    {
        if(tycoonManager->m_questProcessComplete == false && tycoonManager->m_quest->m_questtype != DyQuestType_Ignore)
        {
            drawFailStamp(ccp(8,-2+60), "menu_ui_failed_goal_stamp.png", 0.1);
            drawFailStamp(ccp(8,-2-40), "menu_ui_failed_quest_stamp.png", 0.6);
            schedule(schedule_selector(DyLayerGameResult::showQuestPopup), .6f);
        }
        else
            drawFailStamp(ccp(8,-2), "menu_ui_failed_goal_stamp.png", 0.1);
    }
}


void DyLayerGameResult::setResult()
{
    if(m_curStageState != DyStageState_Fail)
    {
        DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
        
        if(strcmp(tycoonManager->m_quest->m_questMainTag.c_str(), QUESTTAG_GUEST) == 0)
        {
            DyDataManager* dataManager = DyDataManager::sharedDataManager();
           
            DyLib* lib = DyLib::sharedLib();
            
            DyToast* toast = DyToast::create(DyToastType_Text, this, callfuncO_selector(DyDataManager::onPopupClicked));
            
            
            DyCharacterInfo* characterInfo = DyCharacterManager::sharedCharacterManager()->getCharacterInfo(tycoonManager->m_quest->m_questSubTag.c_str());
            
            if(characterInfo && !DyCharacterManager::sharedCharacterManager()->isDangol(characterInfo->m_id))
            {
                sprintf(lib->m_strTemp, STRING_awards_complete_service_first, characterInfo->m_name.c_str());
                toast->setText(lib->m_strTemp);
                toast->showToast();
            
                DyCharacterManager::sharedCharacterManager()->setDangol(characterInfo->m_id);
                
                if(strcmp(tycoonManager->m_quest->m_questSubTag.c_str(), "poodle") == 0)
                {
                    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
                    if(dataManager->m_public.m_staff_grade == -1)
                    {
                        dataManager->m_public.m_staff_grade = 0;
                        dataManager->m_private.m_staff_next_grade_remain = atoi(staffManager->m_staff[dataManager->m_private.m_current_staff]->getValue("conditon", dataManager->m_public.m_staff_grade+1));
                        
                        
                        DyToast* toast = DyToast::create(DyToastType_Text, NULL, NULL);
                        toast->setText(STRING_HIRE_STAFF_SUCCESS);
                        toast->showToast();
                        
                    }
                }
            }
        }
    }
}



void DyLayerGameResult::checkRankup()
{
    DyRankManager* rankManager = DyRankManager::sharedRankManager();
    
    if(rankManager->setCheckPoint())
    {
        this->pause();
    
        m_layerGameResultRankup = DyLayerGameResultRankup::create(this);
        addChild(m_layerGameResultRankup);
        m_isRankup = true;
    }
}



void DyLayerGameResult::showPopupReview(float dt)
{
    unschedule(schedule_selector(DyLayerGameResult::showPopupReview));
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerGameResult::onPopupClicked), CCSizeMake(440, 350));
    
    popup->setText(STRING_REWARD_ALARM);
    
    popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
    popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 100);
    popup->addButton(DyButtonType_Close, "", 0);
    
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerGameResult::checkReview()
{
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    if(stageManager->m_curShopType == DyStageType_Snack && DyStageManager::sharedStageManager()->m_curLevel < 2)
    {
        return;
    }

    bool needReview = false;
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    if(dataManager->m_private.m_rewardedReview == false)
    {
        if(dataManager->m_private.m_cntCheckReview == 0)
        {
            if(m_curStageState == DyStageState_Perfect)
            {
                dataManager->m_private.m_cntCheckReview++;
                needReview = true;
            }
        }
        else
        {
            if(m_curStageState >= DyStageState_Goal)
            {
                dataManager->m_private.m_cntCheckReview++;
                
                if(dataManager->m_private.m_cntCheckReview > MAX_CHARNUM)
                    return;
                if(dataManager->m_private.m_cntCheckReview%5 == 0)
                {
                    needReview = true;
                }
            }
        }
    }
    
    if(needReview)
    {
        schedule(schedule_selector(DyLayerGameResult::showPopupReview), m_isRankup ? 1.f:0.01f);
    }
}


void DyLayerGameResult::showNewScore()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(tycoonManager->m_newscore == false)
        return;
    
    float delayFade = 3.f;
    
    CCSprite* sprTextBox = (CCSprite*)getChildByTag(GAMERESULT_UI_TAG_NEWSCOREBOX);
    CCPoint position = ccp(sprTextBox->getContentSize().width/2,
                           sprTextBox->getContentSize().height/2);
    sprTextBox->setVisible(true);

    sprTextBox->runAction(CCRepeatForever::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_newscore_textbox")));
    sprTextBox->runAction(CCSequence::create(CCDelayTime::create(delayFade), CCFadeOut::create(.5f), NULL));
    
    CCSprite* sprStar = CCSprite::createWithSpriteFrameName("threematch_ui_newscore_star0.png");
    sprStar->runAction(CCRepeatForever::create(CCextAnimatePacker::getInstance()->getSequence("threematch_ui_newscore_star")));
    sprStar->runAction(CCSequence::create(CCDelayTime::create(delayFade), CCFadeOut::create(.5f), NULL));
    sprStar->setPosition(position);
    
    sprTextBox->addChild(sprStar);
    
    CCSprite* sprText = CCSprite::createWithSpriteFrameName("threematch_ui_newscore_text.png");
    sprText->setPosition(position);
    
    sprTextBox->addChild(sprText);

    
    sprText->runAction(CCSequence::create(CCDelayTime::create(delayFade), CCFadeOut::create(.5f), NULL));
    CCActionInterval* seq = (CCActionInterval*)(CCSequence::create(CCDelayTime::create(0.3), CCScaleTo::create(0.2, 1.2), CCScaleTo::create(0.1, 1.0f), NULL));
    sprText->runAction(CCRepeatForever::create(seq));
}


void DyLayerGameResult::pause()
{
    if(m_isRankup)
    {
        m_layerGameResultRankup->pause();
    }
    else
    {
        m_menu->setEnabled(false);
        setKeypadEnabled(false);
    }
}


void DyLayerGameResult::resume()
{
    if(m_isRankup)
    {
        m_layerGameResultRankup->resume();
    }
    else
    {
        m_menu->setEnabled(true);
        setKeypadEnabled(true);
    
        DyCostumeManager::sharedCostumeManager()->setCostume(m_doggy, &DyDataManager::sharedDataManager()->m_public.m_costume_status, &DyDataManager::sharedDataManager()->m_custume_function);
    }
}


void DyLayerGameResult::hide()
{
	YtLib* lib = YtLib::sharedLib();

	getChildByTag(GAMERESULT_UI_TAG_EFFECT)->setVisible(false);

	runAction(CCSequence::create(

		CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)), 
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height)),

		CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::destory)),
		NULL));
}


void DyLayerGameResult::startAni()
{
	DyStageManager* stageManager = DyStageManager::sharedStageManager();

    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(m_curStageState == DyStageState_Fail)
    {
        m_menu->setVisible(true);
       
        if((stageManager->isLastLevel() && dataManager->shop_get_viewending(stageManager->m_curShopType)) ||
           stageManager->isNextLevelOpen())
        {
            getChildByTag(GAMERESULT_UI_TAG_NEEDDELLY)->setVisible(false);
            m_menu->getChildByTag(GAMERESULT_UI_TAG_MENUITEM_NEXT)->setVisible(false);
        }
        else
        {
            DyLib* lib = DyLib::sharedLib();
            m_stage_pass_delly = (YtLibSndBGM)atoi(lib->m_parameters->getValue("value_integer", DyLibParam_Stage_pass_dell));
            CCLabelTTF* labelDelly = (CCLabelTTF*)getChildByTag(GAMERESULT_UI_TAG_NEEDDELLY)->getChildByTag(GAMERESULT_UI_TAG_NEEDDELLY_TEXT);
            getChildByTag(GAMERESULT_UI_TAG_NEEDDELLY)->setVisible(true);
            
            sprintf(lib->m_strTemp, "%d", m_stage_pass_delly);
            labelDelly->setString(lib->m_strTemp);
        }
        
        runAction(CCSequence::create(
                                     CCDelayTime::create(0.1f),
                                     CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::startFailEffect)),
                                     NULL));
    }
    else
    {
        runAction(CCSequence::create(
                                     CCDelayTime::create(0.1f),
                                     CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::setPointView)),
                                     NULL));
    }
    
    
	switch(m_curStageState)
	{
		case DyStageState_Fail:
			
			break;
		case DyStageState_Goal:
			stageManager->nextStageOpen();
			break;
		case DyStageState_Goal_Plus:
			stageManager->nextStageOpen();
			break;
		case DyStageState_Perfect:
			stageManager->nextStageOpen();
			break;
        default:
            break;
	}
}


void DyLayerGameResult::show()
{
 	YtLib* lib = YtLib::sharedLib();

    YtLib::sharedLib()->playEffect(YtLibSndEff_CURTAIN_SPREAD);
    
	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));

    
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(tycoonManager->m_isCrazyMode)
    {
        m_actionManager->runAnimations("crazy");
        //m_doggy->m_actionManager->runAnimations("map_hello");
        m_doggy->setVisible(false);
        
        runAction(CCSequence::create(
                                     
                                     CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
                                     CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
                                     CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
                                     CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::result_update)),
                                     NULL));
        //m_menu->setVisible(true);
        //m_menu->getChildByTag(GAMERESULT_UI_TAG_MENUITEM_NEXT)->setVisible(false);
    }
    else
    {
        switch(m_curStageState)
        {
            case DyStageState_Fail:
                m_actionManager->runAnimations("fail");
                m_doggy->m_actionManager->runAnimations("fail");
                YtLib::sharedLib()->playEffect(YtLibSndEff_Cry);
                
                break;
            case DyStageState_Goal:
                m_actionManager->runAnimations("goal");
                m_doggy->m_actionManager->runAnimations("map_hello");
                
                break;
            case DyStageState_Goal_Plus:
                m_actionManager->runAnimations("goal_plus");
                m_doggy->m_actionManager->runAnimations("map_hello");
                
                break;
            case DyStageState_Perfect:
                m_actionManager->runAnimations("perfect");
                m_doggy->m_actionManager->runAnimations("map_hello");
                break;
            default:
                break;
        }
        
        switch(m_curStageState)
        {
            case DyStageState_Goal:
                attachStar(0);
                break;
            case DyStageState_Goal_Plus:
                attachStar(0);
                attachStar(1);
                break;
            case DyStageState_Perfect:
                attachStar(0);
                attachStar(1);
                attachStar(2);
                break;
            default:
                break;
        }
        
        runAction(CCSequence::create(
                                     
                                     CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
                                     CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
                                     CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
                                     CCCallFunc::create(this, callfunc_selector(DyLayerGameResult::result_update)),
                                     NULL));

    }
}


DyLayerGameResult* DyLayerGameResult::create(CCNode* owner)
{
	return (DyLayerGameResult*)YtLayerManager::create("DyLayerGameResult", DyLayerGameResultLoader::loader(), "layer_game_result.ccbi", owner);
}


void DyLayerGameResult::endAnimationFire(CCNode* node)
{
	YtLib* lib = YtLib::sharedLib();

	CCSprite* sprEffect = CCSprite::createWithSpriteFrameName("fever_time_ani00.png");
	m_layerFire->addChild(sprEffect);
	
	int insectX = 50;
	int insectY = 50;
	int x = rand()%(int)(lib->m_sizeWin.width-insectX*2);
	int y = rand()%(int)(lib->m_sizeWin.height-insectY*2);

	x -= lib->m_sizeWin.width/2;
	y -= lib->m_sizeWin.height/2;

	sprEffect->setPosition(ccp(insectX + x, insectY + y));
	sprEffect->setScale(((float)(400 - rand()%200))/100.f);

	float delay = (rand()%10)*0.1f;
	sprEffect->setOpacity(0);

	CCCallFunc* callfun_effect = CCCallFuncN::create(this, callfuncN_selector(DyLayerGameResult::endAnimationFire));
		
	sprEffect->runAction(CCSequence::create(CCDelayTime::create(delay), CCFadeIn::create(.05f), 
		CCextAnimatePacker::getInstance()->getSequence("threematch_ui_effect_fever"), callfun_effect, 
		NULL));
    YtLib::sharedLib()->playEffect(YtLibSndEff_FireFlower);

	if(node)
		node->removeFromParentAndCleanup(true);
}


void DyLayerGameResult::setStartFireWorks()
{
	m_layerFire = (CCLayer*)this->getChildByTag(GAMERESULT_UI_TAG_EFFECT);

	for(int i = 0;i < 30;i++)
	{
		endAnimationFire(NULL);
	}
}
