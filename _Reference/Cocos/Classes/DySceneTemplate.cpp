/*
DySceneTemplate.cpp
Doggy chef
@initialize at 121120

@history
121120	yoonsr	initialize
*/

#include "DyConfig.h"
#include "DySceneTemplate.h"
#include "DyLib.h"

#include "YtAniObject.h"

DyLayerTemplate::DyLayerTemplate()
{
}

DyLayerTemplate::~DyLayerTemplate()
{	
}


void DyLayerTemplate::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
	//DyLib *lib = DyLib::sharedLib();
	setTouchEnabled(true);
}


SEL_MenuHandler DyLayerTemplate::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	//CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onBackClicked", DyLayerAnimationTest::onBackClicked);
	//CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onSelectAnimalClicked", DyLayerAnimationTest::onSelectAnimalClicked);
	//CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onOrderClicked", DyLayerAnimationTest::onOrderClicked);
	//CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onWalkinClicked", DyLayerAnimationTest::onWalkinClicked);
	//CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onWalkoutClicked", DyLayerAnimationTest::onWalkoutClicked);
	//CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onEatClicked", DyLayerAnimationTest::onEatClicked);
	//CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onLikeClicked", DyLayerAnimationTest::onLikeClicked);
	//CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onAngryClicked", DyLayerAnimationTest::onAngryClicked);

    return NULL;    
}

SEL_CCControlHandler DyLayerTemplate::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}

bool DyLayerTemplate::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerTemplate::onBackClicked(CCObject * pSender)
{
}


void DyLayerTemplate::onSelectAnimalClicked(CCObject * pSender)
{
}


void DyLayerTemplate::onOrderClicked(CCObject * pSender)
{
}


void DyLayerTemplate::onWalkinClicked(CCObject * pSender)
{
}


void DyLayerTemplate::onWalkoutClicked(CCObject * pSender)
{
}


void DyLayerTemplate::onEatClicked(CCObject * pSender)
{
}


void DyLayerTemplate::onLikeClicked(CCObject * pSender)
{
}


void DyLayerTemplate::onAngryClicked(CCObject * pSender)
{
}



void DyLayerTemplate::pause()
{
}


void DyLayerTemplate::resume()
{
}


void DyLayerTemplate::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerTemplate::show()
{
}


DySceneTemplate::DySceneTemplate()
{
}

DySceneTemplate::~DySceneTemplate()
{
}
