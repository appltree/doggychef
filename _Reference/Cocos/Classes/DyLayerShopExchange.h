/*
DyLayerShopExchange.h
Doggy chef
@initialize at 121207

@history
121207	yoonsr	initialize
*/


#ifndef __DYLAYERSHOPEXCHANGE_H__
#define __DYLAYERSHOPEXCHANGE_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "CCTabView.h"
#include "DyCostume.h"

#include "DyItem.h"
#include "DyMoney.h"
#include "DyRecipe.h"

#include "YtAniObject.h"


#define SHOPEXCHANGE_UI_TAG_MENU						1000
#define SHOPEXCHANGE_UI_TAG_MENUITEM_PLUS				1001
#define SHOPEXCHANGE_UI_TAG_MENUITEM_MINUS				1002
#define SHOPEXCHANGE_UI_TAG_MENUITEM_EXCHANGE			1003


#define SHOPEXCHANGE_UI_TAG_TEXT_DELLAY					1010
#define SHOPEXCHANGE_UI_TAG_TEXT_GOLD					1011

#define SHOPEXCHANGE_UI_TAG_EXPLAIN_EXCHANGERATE		1020
#define SHOPEXCHANGE_UI_TAG_EXPLAIN_EXCHANGERATE_GOLD	1021
#define SHOPEXCHANGE_UI_TAG_EXPLAIN_EXCHANGERATE_DELLY	1022

class DyLayerShopExchange
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:

	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerShopExchange, create);
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

	int m_currentDelly;
	int m_currentGold;

	
	CCMenu* m_menu;
	void onPlusPressed(CCObject * obj);
	void onPlusReleased(CCObject * obj);

	void onMinusPressed(CCObject * obj);
	void onMinusReleased(CCObject * obj);

	void onPlusClicked(CCObject * obj);
	void onMinusClicked(CCObject * obj);
	void onExchangeClicked(CCObject * obj);
	void onPopupClicked(cocos2d::CCObject * pSender);
	DyLayerShopExchange();
	virtual ~DyLayerShopExchange();

	bool m_isPlusCb;
	void cbPlusMinus(float dt);
	void cbPlusMinusReal(float dt);
    void setDelly(int delly);
};


class DyLayerShopExchangeLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerShopExchangeLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerShopExchange);
};



#endif  // __DYLAYERSHOPEXCHANGE_H__