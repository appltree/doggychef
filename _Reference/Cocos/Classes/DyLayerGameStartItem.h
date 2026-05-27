/*
DyLayerGameStartItem.h
Doggy chef
@initialize at 121128

@history
121128	yoonsr	initialize
*/


#ifndef __DYLAYERGAMESTARTITEM_H__
#define __DYLAYERGAMESTARTITEM_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "YtAniObject.h"

#include "DyItem.h"
#include "DyMoney.h"

#define GAMESTARTITEM_UI_TAG_MENU						1000
#define GAMESTARTITEM_UI_TAG_MENUITEM_USE_SLOT0			1010
#define GAMESTARTITEM_UI_TAG_MENUITEM_USE_SLOT1			1011
#define GAMESTARTITEM_UI_TAG_MENUITEM_USE_SLOT2			1012

#define GAMESTARTITEM_UI_TAG_MENUITEM_INVEN_SLOT0		1020
#define GAMESTARTITEM_UI_TAG_MENUITEM_INVEN_SLOT1		1021
#define GAMESTARTITEM_UI_TAG_MENUITEM_INVEN_SLOT2		1022
#define GAMESTARTITEM_UI_TAG_MENUITEM_INVEN_SLOT3		1023
#define GAMESTARTITEM_UI_TAG_MENUITEM_INVEN_SLOT4		1024

#define GAMESTARTITEM_UI_TAG_MENUITEM_DONE				1030



#define GAMESTARTITEM_UI_TAG_DESCRIPTION_BOX		2010
#define GAMESTARTITEM_UI_TAG_DESCRIPTION_TEXT		2011

#define GAMESTARTITEM_UI_TAG_ITEM_DISABLE_BG_0		3000
#define GAMESTARTITEM_UI_TAG_ITEM_DISABLE_BG_1		3001

#define GAMESTARTITEM_UI_TAG_ITEM_DISABLE_LOCK_0	3100
#define GAMESTARTITEM_UI_TAG_ITEM_DISABLE_LOCK_1	3101


class DyLayerGameStartItem
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerGameStartItem, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	CCMenu* m_menu;
	CCMenuItem* m_menuItemSlot[DYITEM_SLOT_CNT];
	CCMenuItem* m_menuItemInven[DyItemType_Count];

	CCSprite* m_sprSlotItem[DYITEM_SLOT_CNT];
	CCSprite* m_sprInvenItem[DyItemType_Count];

	DyLayerGameStartItem();
	virtual ~DyLayerGameStartItem();

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
	
	void onDoneClicked(CCObject* pSender);
	void onShopClicked(CCObject* pSender);

	void onSlotClicked(CCObject* pSender);
	void onInvenClicked(CCObject* pSender);

	static DyLayerGameStartItem* create(CCNode* owner);

	void updateItemStatus();
};


class DyLayerGameStartItemLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerGameStartItemLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerGameStartItem);
};


#endif  // __DYLAYERGAMESTARTITEM_H__