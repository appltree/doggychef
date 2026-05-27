/*
YtAniObject.cpp
YongTrim Lib
@initialize at 121002

@history
121002	yoonsr	initialize
*/


#include "YtAniObject.h"


YtAniObject::YtAniObject()
{
	m_isPaused = false;
	m_delegate = NULL;
}

YtAniObject::~YtAniObject()
{
}


void YtAniObject::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
}

SEL_MenuHandler YtAniObject::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onClicked", YtAniObject::onClicked);
    return NULL;    
}

SEL_CCControlHandler YtAniObject::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}

bool YtAniObject::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void YtAniObject::onClicked(cocos2d::CCObject * pSender)
{
	if(m_delegate)
		m_delegate->onClicked();
}


YtAniObject* YtAniObject::loadAniObject(CCObject* owner, const char* pCCBFileName)
{
    /* Create an autorelease CCNodeLoaderLibrary. */
    CCNodeLoaderLibrary * ccNodeLoaderLibrary = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();
    
    ccNodeLoaderLibrary->registerCCNodeLoader("YtAniObject", YtAniObjectLoader::loader());

    cocos2d::extension::CCBReader* ccbReader = new cocos2d::extension::CCBReader(ccNodeLoaderLibrary);
    ccbReader->autorelease();
    
	CCBAnimationManager* actionManager = NULL;
    YtAniObject* aniobject = (YtAniObject*)ccbReader->readNodeGraphFromFile(pCCBFileName, owner, &actionManager);
	aniobject->m_actionManager = actionManager;
	aniobject->m_actionManager->retain();

	return aniobject;
}

void YtAniObject::unloadAniObject(YtAniObject* aniObject)
{
	aniObject->m_actionManager->release();
}


void YtAniObject::pause()
{
}


void YtAniObject::resume()
{
}


void YtAniObject::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void YtAniObject::show()
{
}


void YtAniObject::pauseAnimation()
{
	m_isPaused = true;
	this->pauseSchedulerAndActions();
}

void YtAniObject::resumeAnimation()
{
	m_isPaused = false;
	this->resumeSchedulerAndActions();
}

bool YtAniObject::isRun()
{
	return !m_isPaused;
}