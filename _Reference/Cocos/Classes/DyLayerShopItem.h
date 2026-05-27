/*
DyLayerShopItem.h
Doggy chef
@initialize at 121207

@history
121207	yoonsr	initialize
*/


#ifndef __DYLAYERSHOPITEM_H__
#define __DYLAYERSHOPITEM_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "CCTabView.h"
#include "DyCostume.h"

#include "DyItem.h"
#include "DyMoney.h"
#include "DyRecipe.h"

#include "YtAniObject.h"



#define SHOPITEM_UI_TAG_GROUP_HINT						2000
#define SHOPITEM_UI_TAG_GROUP_REFRESH					3000
#define SHOPITEM_UI_TAG_GROUP_TIME						4000
#define SHOPITEM_UI_TAG_GROUP_CANDY						5000
#define SHOPITEM_UI_TAG_GROUP_BOMB						6000

#define SHOPITEM_UI_TAG_SUB_ICON						10
#define SHOPITEM_UI_TAG_SUB_NAME						11
#define SHOPITEM_UI_TAG_SUB_EXPLAIN						12
#define SHOPITEM_UI_TAG_SUB_REMAIN_COUNT				13
#define SHOPITEM_UI_TAG_SUB_MENU						20
#define SHOPITEM_UI_TAG_SUB_MENUITEM_MINUS				21
#define SHOPITEM_UI_TAG_SUB_MENUITEM_PLUS				22
#define SHOPITEM_UI_TAG_SUB_MENUITEM_BUY				23
#define SHOPITEM_UI_TAG_SUB_COUNT						30
#define SHOPITEM_UI_TAG_SUB_PRICE						31
#define SHOPITEM_UI_TAG_SUB_GOLD_ICON					32

class DyLayerShopItem
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:

	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerShopItem, create);
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
	
	void onPlusClicked(CCObject * obj);
	void onMinusClicked(CCObject * obj);

	void onBuyClicked(CCObject * obj);
	void onPopupClicked(cocos2d::CCObject * pSender);

	CCMenu* m_menu[DyItemType_Count];
	int m_count[DyItemType_Count];

	DyLayerShopItem();
	virtual ~DyLayerShopItem();

	int m_itemBuyProcess;

	void onPlusPressed(CCObject * obj);
	void onPlusReleased(CCObject * obj);

	void onMinusPressed(CCObject * obj);
	void onMinusReleased(CCObject * obj);


	bool m_isPlusCb;
	void cbPlusMinus(float dt);
	void cbPlusMinusReal(float dt);
};


class DyLayerShopItemLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerShopItemLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerShopItem);
};


#endif  // __DYLAYERSHOPITEM_H__