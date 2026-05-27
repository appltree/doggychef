/*
DySceneIntro.cpp
Monster showdown
@initialize at 130522

@history
130522	yoonsr	initialize
*/

#include "DySceneIntro.h"
#include "DyLib.h"
#include "DyData.h"
#include "DySceneStory.h"



DyLayerIntro::DyLayerIntro()
{
    m_step = 0;
}

DyLayerIntro::~DyLayerIntro()
{
}
	
void DyLayerIntro::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader)
{
    DyLib* lib = DyLib::sharedLib();
    
    CCLayerColor* layer = (CCLayerColor*)getChildByTag(1000);
    layer->initWithColor(ccc4(255,255,255,255), lib->m_sizeWin.width, lib->m_sizeWin.height);
    schedule(schedule_selector(DyLayerIntro::cbTouchEnable), 1.f);
}


SEL_MenuHandler DyLayerIntro::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName)
{
    return NULL;    
}

SEL_CCControlHandler DyLayerIntro::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}


bool DyLayerIntro::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode)
{
    return false;
}


void DyLayerIntro::onEnter()
{
    YtLayer::onEnter();
    schedule(schedule_selector(DyLayerIntro::cbNext), 3.f);
}


void DyLayerIntro::cbTouchEnable(float dt)
{
    unschedule(schedule_selector(DyLayerIntro::cbTouchEnable));
    setTouchEnabled(true);
}

void DyLayerIntro::cbNext(float dt)
{
    m_step++;
    if(m_step == 2)
    {
        unschedule(schedule_selector(DyLayerIntro::cbNext));
        
        DyDataManager* dataManager = DyDataManager::sharedDataManager();
        
        DyLib::sharedLib()->loadDefaultResources();
        
        if(YtOption_get_viewintro(dataManager->m_local.m_option) == false)
            DyLib::sharedLib()->setStatus(DyLib_AppStatus_Story, DyStoryType_Intro);
        else
            DyLib::sharedLib()->setStatus(DyLib_AppStatus_Title, 0);
    }
    else
    {
        schedule(schedule_selector(DyLayerIntro::cbTouchEnable), 1.f);
        m_actionManager->runAnimations("yongtrim");
    }
}


void DyLayerIntro::pause()
{
}


void DyLayerIntro::resume()
{
}


void DyLayerIntro::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerIntro::show()
{
}




void DyLayerIntro::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
    setTouchEnabled(false);
    cbNext(0);
}


void DyLayerIntro::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    
}


void DyLayerIntro::ccTouchesMoved(CCSet* touches, CCEvent* event)
{
    
}


DySceneIntro::DySceneIntro()
{
}


DySceneIntro::~DySceneIntro()
{
}


void DySceneIntro::didSceneChanged()
{
}
