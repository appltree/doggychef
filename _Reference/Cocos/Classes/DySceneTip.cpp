/*
DySceneTip.cpp
Doggy chef
@initialize at 130427

@history
130427	yoonsr	initialize
*/

#include "DyConfig.h"
#include "DySceneTip.h"
#include "DyLib.h"
#include "DyPopup.h"


DyLayerTip::DyLayerTip()
{
}

DyLayerTip::~DyLayerTip()
{
}


void DyLayerTip::onEnter()
{
    YtLayer::onEnter();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(dataManager->m_display_ad)
        YtPlatform_ShowAd(true);
}


void DyLayerTip::onExit()
{
	YtLayer::onExit();
    YtPlatform_ShowAd(false);
}

void DyLayerTip::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
    DyLib* lib = DyLib::sharedLib();
    
    CCSprite* sprPicture = (CCSprite*)getChildByTag(TIP_TAG_IMAGE);
    
    CCLabelTTF* labelContent = (CCLabelTTF*)getChildByTag(TIP_TAG_CONTENT);
    
    int index = rand()%TIP_CNT;
    
    static std::string strMessage[TIP_CNT] =
    {
        STRING_tip_01,
        STRING_tip_02,
        STRING_tip_03,
        STRING_tip_04,
        STRING_tip_05,
        STRING_tip_06,
        STRING_tip_07,
        STRING_tip_08,
        STRING_tip_09,
        STRING_tip_10,
        STRING_tip_11,
        STRING_tip_12,
        STRING_tip_13,
        STRING_tip_14,
        STRING_tip_15,
        STRING_tip_16,
        STRING_tip_17,
        STRING_tip_18,
        STRING_tip_19,
        STRING_tip_20,
        STRING_tip_21,
        STRING_tip_22,
        STRING_tip_23,
    };

    
    sprintf(lib->m_strTemp, "tip_tip%02d.png", index+1);
    sprPicture->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));
    labelContent->setString(strMessage[index].c_str());
    
    this->schedule(schedule_selector(DyLayerTip::nextStep), 3.3f);
    
    m_sprTabScreen = (CCSprite*)getChildByTag(TIP_TAG_TAPTHESCREEN);
    m_sprTabScreen->setVisible(false);
    
    setTouchEnabled(false);
	
}


void DyLayerTip::nextStep()
{
    this->unschedule(schedule_selector(DyLayerTip::nextStep));
    //DyLib::sharedLib()->setStatus(DyLib_AppStatus_Game, 0);
    
    YtLayerManager::sharedLayerManager()->popLayer();
   
    m_sprTabScreen->setVisible(true);
    
    setTouchEnabled(true);
	
    this->schedule(schedule_selector(DyLayerTip::nextStep_Touch), .5f);
}


void DyLayerTip::nextStep_Touch()
{
    m_sprTabScreen->stopAllActions();
    m_sprTabScreen->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCBlink::create(1.0, 1), NULL))));
    
    this->unschedule(schedule_selector(DyLayerTip::nextStep_Touch));
}

SEL_MenuHandler DyLayerTip::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName)
{
    return NULL;    
}

SEL_CCControlHandler DyLayerTip::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}

bool DyLayerTip::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}

void DyLayerTip::pause()
{
}


void DyLayerTip::resume()
{
}


void DyLayerTip::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerTip::show()
{
}



void DyLayerTip::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
	CCSetIterator it;
	CCTouch* touch;
    
	for(it = touches->begin();it != touches->end();it++)
	{
		touch = (CCTouch*)(*it);
		if(!touch)
			break;
        
        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_EAT);
        
        setTouchEnabled(false);
        
		DyLib::sharedLib()->setStatus(DyLib_AppStatus_Game, 0);
		return;
	}
}


DySceneTip::DySceneTip()
{
}

DySceneTip::~DySceneTip()
{
}

void DySceneTip::didSceneChanged()
{
    YtLib* lib = YtLib::sharedLib();
    
    DyPopup* popup = DyPopup::create(DyPopupType_LoadingCycle);
    
    
    //popup->setText("IS_REGIST....");
    YtLayerManager::sharedLayerManager()->pushLayer(popup, false, true);
    popup->setPosition(ccp(lib->m_sizeWin.width/2, 200));
}