/*
DyLayerShopDelly.h
Doggy chef
@initialize at 121207

@history
121207	yoonsr	initialize
*/


#ifndef __DYLAYERSHOPDELLY_H__
#define __DYLAYERSHOPDELLY_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "CCTabView.h"
#include "DyCostume.h"

#include "DyItem.h"
#include "DyMoney.h"
#include "DyRecipe.h"

#include "YtAniObject.h"



#define SHOPDELLY_UI_TAG_GROUP_0						1000
#define SHOPDELLY_UI_TAG_GROUP_1						2000
#define SHOPDELLY_UI_TAG_GROUP_2						3000
#define SHOPDELLY_UI_TAG_GROUP_3						4000
#define SHOPDELLY_UI_TAG_GROUP_4						5000


#define SHOPDELLY_UI_TAG_SUB_DELLY_COUNT					1
#define SHOPDELLY_UI_TAG_SUB_BONUS_COUNT					2

#define SHOPDELLY_UI_TAG_SUB_MENU						20
#define SHOPDELLY_UI_TAG_SUB_MENUITEM_BUY				21

#define SHOPDELLY_UI_TAG_SUB_WON							30
#define SHOPDELLY_UI_TAG_SUB_PRICE						31


class DyLayerShopDelly
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:

	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerShopDelly, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    
    virtual void onEnter();
	virtual void onExit();
	
	void onBuyClicked(cocos2d::CCObject * pSender);
	CCMenu* m_menu[6];
	DyLayerShopDelly();
	virtual ~DyLayerShopDelly();
};


class DyLayerShopDellyLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerShopDellyLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerShopDelly);
};



#endif  // __DYLAYERSHOPDELLY_H__