/*
DyLayerShop.h
Doggy chef
@initialize at 121207

@history
121207	yoonsr	initialize
*/


#ifndef __DYLAYERSHOP_H__
#define __DYLAYERSHOP_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "CCTabView.h"
#include "DyCostume.h"

#include "DyItem.h"
#include "DyMoney.h"
#include "DyRecipe.h"

#include "YtAniObject.h"

#define DYLAYERSHOP_TAG     "DyLayerShop"
typedef enum tag_DyLayerShopType
{
	DyLayerShopType_Normal,
	DyLayerShopType_Myroom,
}DyLayerShopType;


#define SHOP_UI_TAG_MENU						1000
#define SHOP_UI_TAG_MENUITEM_GOSHOP				1001

#define SHOP_UI_TAG_CONTENT						1200

#define SHOP_UI_TAG_TITLE						1300

class DyLayerShopTab : public CCTabView
{
public:
	DyLayerShopTab(DyLayerShopType shopType, CCRect rect = CCRectZero);
};


#define DYLAYERSHOP_MAX_TAB		6
class DyLayerShop
: public YtLayer
, public DyLayerMoneyDelegate
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
, public CCTabViewDelegate
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerShop, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	DyLayerShop();
	virtual ~DyLayerShop();

	void onCloseClicked(cocos2d::CCObject * pSender);
	void onShopClicked(cocos2d::CCObject * pSender);

	DyLayerShopType m_shopType;

	int m_callShop;
    int m_callShop_param;

	int m_startTab;
	int m_startSubTab;
	CCMenu* m_menu;
	DyLayerShopTab* m_tabView;
	int m_tabIndex;
	YtLayer *m_layerContent[DYLAYERSHOP_MAX_TAB];

	DyLayerMoney* m_layerMoney;

    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);
    
    
    //void showDelly(float dt);
    
    void onPopupClicked(cocos2d::CCObject * pSender);

    
	virtual void destory();
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    virtual void keyBackClicked();

    virtual void onEnter();
	virtual void onExit();

	virtual void tabDidChange(CCTabView* tabView, int index, CCLayer* contentLayer, bool init, bool redo);
	virtual void buttonClicked(DyLayerMoney* moneyView, int index);
	static DyLayerShop* create(CCNode* owner, DyLayerShopType shopType, int startTab, int startSubTab);

};


class DyLayerShopLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerShopLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerShop);
};

#endif  // __DYLAYERSHOP_H__