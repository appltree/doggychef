/*
DyLayerPride.cpp
Doggy chef
@initialize at 130831

@history
130831	yoonsr	initialize
*/

#include "DyConfig.h"
#include "DyLayerPride.h"
#include "DyLib.h"
#include "YtSpriteEx.h"
#include "DyData.h"
#include "DyTycoon.h"

#include "DyLayerStageRank.h"


DyLayerPride::DyLayerPride()
{
    m_list = NULL;
}

DyLayerPride::~DyLayerPride()
{
    if(m_list)
        m_list->release();
}


void DyLayerPride::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
	m_menu = (CCMenu*)getChildByTag(PRIDE_UI_TAG_MENU);
    
    refresh();
}


void DyLayerPride::refresh()
{
    DyLib* lib = DyLib::sharedLib();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    CCLabelTTF* label = (CCLabelTTF*)getChildByTag(PRIDE_UI_TAG_COUNT);
    
    sprintf(lib->m_strTemp, "(%d/10)", dataManager->m_private.m_cntPride);
    
    label->setString(lib->m_strTemp);
    
}

void DyLayerPride::onEnter()
{
    CCLayer::onEnter();
    
	if(m_isInit)
		return;
	m_isInit = true;
    CCSize sizeScrollView = CCSizeMake(500, 250);
    CCRect rectScroll = CCRectMake(-sizeScrollView.width/2, -sizeScrollView.height/2 + 33, sizeScrollView.width, sizeScrollView.height);
    
    m_list = new DyLayerRank_list(rectScroll);
    m_list->setRankLayer(DyLayerRankListType_Pride, m_rankType, NULL);
    m_list->m_pridePoint = m_pridePoint;
	addChild(m_list);
    
    setKeypadEnabled(true);
}


SEL_MenuHandler DyLayerPride::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCloseClicked", DyLayerPride::onCloseClicked);
    return NULL;    
}

SEL_CCControlHandler DyLayerPride::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}

bool DyLayerPride::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerPride::onCloseClicked(cocos2d::CCObject * pSender)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLOSE);

	YtLayerManager::sharedLayerManager()->popLayer();
}



void DyLayerPride::keyBackClicked()
{
    setKeypadEnabled(false);
    onCloseClicked(NULL);
}



void DyLayerPride::pause()
{
    m_menu->setEnabled(false);
    m_list->pause();
    setKeypadEnabled(false);

}


void DyLayerPride::resume()
{
    m_menu->setEnabled(true);
    m_list->resume();
    setKeypadEnabled(true);
    refresh();
}


void DyLayerPride::hide()
{
	YtLib* lib = YtLib::sharedLib();

	runAction(CCSequence::create(

		CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)), 
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height)),

		CCCallFunc::create(this, callfunc_selector(DyLayerPride::destory)),
		NULL));
}


void DyLayerPride::show()
{
	YtLib* lib = YtLib::sharedLib();

	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));

	runAction(CCSequence::create(

		CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
		NULL));
}


DyLayerPride* DyLayerPride::create(CCNode* owner, DyRankType rankType, int pridePoint)
{
	DyLayerPride* layerPride = (DyLayerPride*)YtLayerManager::create("DyLayerPride", DyLayerPrideLoader::loader(), "layer_pride.ccbi", owner);

    layerPride->m_pridePoint = pridePoint;
    layerPride->m_rankType = rankType;
    
	return layerPride;
}