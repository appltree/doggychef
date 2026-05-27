/*
DyLayerCredits.cpp
Doggy chef
@initialize at 130802

@history
130802	yoonsr	initialize
*/

#include "DyConfig.h"
#include "DyLayerCredits.h"
#include "DyLib.h"
#include "YtSpriteEx.h"
#include "DyData.h"
#include "DyTycoon.h"

DyLayerCredits::DyLayerCredits()
{
}

DyLayerCredits::~DyLayerCredits()
{
}


void DyLayerCredits::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
	m_menu = (CCMenu*)getChildByTag(CREDIT_UI_TAG_MENU);
    setKeypadEnabled(true);

}


SEL_MenuHandler DyLayerCredits::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCloseClicked", DyLayerCredits::onCloseClicked);
    return NULL;    
}

SEL_CCControlHandler DyLayerCredits::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}

bool DyLayerCredits::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerCredits::onCloseClicked(cocos2d::CCObject * pSender)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLOSE);

	YtLayerManager::sharedLayerManager()->popLayer();
}

void DyLayerCredits::keyBackClicked()
{
    setKeypadEnabled(false);
    onCloseClicked(NULL);
}


void DyLayerCredits::pause()
{
    setKeypadEnabled(false);
}


void DyLayerCredits::resume()
{
    setKeypadEnabled(true);
}


void DyLayerCredits::hide()
{
	YtLib* lib = YtLib::sharedLib();

	runAction(CCSequence::create(

		CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)), 
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height)),

		CCCallFunc::create(this, callfunc_selector(DyLayerCredits::destory)),
		NULL));
}


void DyLayerCredits::show()
{
	YtLib* lib = YtLib::sharedLib();

	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));

	runAction(CCSequence::create(

		CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
		NULL));
}


DyLayerCredits* DyLayerCredits::create(CCNode* owner, bool callByMainMenu)
{
	DyLayerCredits* layerCredits = (DyLayerCredits*)YtLayerManager::create("DyLayerCredit", DyLayerCreditsLoader::loader(), "layer_credit.ccbi", owner);

	return layerCredits;
}