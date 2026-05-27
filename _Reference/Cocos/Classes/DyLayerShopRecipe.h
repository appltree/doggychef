/*
DyLayerShopRecipe.h
Doggy chef
@initialize at 121207

@history
121207	yoonsr	initialize
*/


#ifndef __DYLAYERSHOPRECIPE_H__
#define __DYLAYERSHOPRECIPE_H__

#include "DyLayerShop.h"

#include "YtScene.h"
#include "YtLayer.h"

#include "CCTabView.h"
#include "DyCostume.h"

#include "DyItem.h"
#include "DyMoney.h"
#include "DyRecipe.h"

#include "YtAniObject.h"


#define SHOPRECIPE_UI_TAG_BG_TOP					1000
#define SHOPRECIPE_UI_TAG_BG_BOTTOM					2000
//#define SHOPRECIPE_UI_TAG_SCROLL_BG					2001
//#define SHOPRECIPE_UI_TAG_SCROLL_CURSOR				2002

class DyLayerShopRecipeTab : public CCTabView
{
public:
	DyLayerShopRecipeTab(CCRect rect = CCRectZero);
};

class DyLayerShopRecipe
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
, public CCScrollViewDelegate
, public CCTabViewDelegate
{
public:

	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerShopRecipe, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
	
    bool m_isSuspend;   // 팝업이 떠서 레이어가 뒤로 갔는가?
    
	DyLayerShopType m_shopType;

	CCArray* m_arrRecipe[SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED];
	CCArray* m_arrShown[SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED];

	CCArray* m_arrLayer[SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED];
    
    CCMenuItemSprite *m_menuitemAllBuy[SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED];
    
    bool m_initShopLayer[SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED];
    
    int m_cnt_allrecipe[SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED];

	CCScrollView* m_tabScrollView[SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED];

	CCSize m_scrollViewSize;

	CCLayer* m_contentLayer;
	int m_startTab;
	int m_tabIndex;
	int checkRecipe(int id_shop, bool first);
	void reloadRecipeResource(int id_shop);

	void setRecipeLayer(CCLayer* layer, DyRecipeInfo* recipe, bool isShown, int shop, int index, bool replace);
    void setAllRecipeLayer(CCLayer* layer, int shop);
    
	CCLayerColor* createRecipeLayer(int id_shop, int layerHight);
    void setRecipeLayer(int id_shop, CCLayerColor* layerContent);
    
	DyLayerShopRecipe();
	virtual ~DyLayerShopRecipe();

	virtual void scrollViewDidScroll(CCScrollView* view);
	virtual void scrollViewDidZoom(CCScrollView* view);

	virtual void tabDidChange(CCTabView* tabView, int index, CCLayer* contentLayer, bool init, bool redo);

    virtual void onEnter();
	virtual void onExit();

	void onBuyClicked(CCObject * obj);

	void refresh(bool enable);
    
    void onPopupClicked(cocos2d::CCObject * pSender);
    
    CCLayerColor* m_layerContent[SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED];
};


class DyLayerShopRecipeLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerShopRecipeLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerShopRecipe);
};

#endif  // __DYLAYERSHOPRECIPE_H__