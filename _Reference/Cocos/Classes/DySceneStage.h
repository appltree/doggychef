/*
DySceneStage.h
Doggy chef
@initialize at 121120

@history
121120	yoonsr	initialize
*/


#ifndef __DYSCENESTAGE_H__
#define __DYSCENESTAGE_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "YtAniObject.h"
#include "DyStage.h"
#include "DyMoney.h"

#define DYLAYERSTAGE_TAG                            "DyLayerStage"
#define STAGE_UI_TAG_BACKGROUND						999
#define STAGE_UI_TAG_MENU							1000
#define STAGE_UI_TAG_MENUITEM_LEFT					1001
#define STAGE_UI_TAG_MENUITEM_RIGHT					1002

#define STAGS_UI_TAG_CURSOR_MAIN					4000
#define STAGS_UI_TAG_CURSOR_TITLEBG					4001
#define STAGS_UI_TAG_CURSOR_TITLELABEL				4002


#define COMINGSOON_COUNT			0

class DyMenuItemShop : public CCMenuItemSprite
{
public:
	CCRect m_rectClip;

	virtual void visit();

	static DyMenuItemShop * create(CCNode *normalSprite, CCNode *selectedSprite, CCNode *disabledSprite, CCObject *target, SEL_MenuHandler selector);
};


class DyLayerStage
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerStage, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	YtAniObject *m_doggy;

	CCMenu* m_menuShop;
	CCMenu* m_menu;
    CCArray* m_arrMenuShopOpen;
		
	float m_menuShopGap;
	float m_posShopMenuY;

	CCArray* m_arrPageCursorBg;
	CCSprite* m_sprPageCursor;


	int m_cntShop;
    DyStageType m_selectShop;
	DyStageType m_nSelectedShop;
    DyStageType m_nProcessShop;
    
	DyLayerMoney* m_layerMoney;

	void onBackClicked(CCObject * pSender);
	void onLeftClicked(CCObject * pSender);
	void onRightClicked(CCObject * pSender);
	void onShopClicked(CCObject * pSender);
    void onStageShopClicked(CCObject* obj);
    void onDoggyClicked(CCObject* obj);
    
    void onOpenByGold(CCObject* obj);
    void onOpenByDelly(CCObject* obj);
    void onDirectOpenByDelly(CCObject* obj);
    
	DyLayerStage();
	virtual ~DyLayerStage();

	void setShopButtonSprite(DyMenuItemShop* menuitem, int shop);
	DyMenuItemShop* createShopButton(DyStageType shop);
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

	virtual void keyBackClicked();
	
	void gotoShopCursor(DyStageType shop);
	void goShop(DyStageType shopType);

	void endAnimationShop(CCNode* node);
    void onPopupClicked(cocos2d::CCObject * pSender);
    
    virtual void update(float dt);

    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);

	void callback_pushSubStage(float dt);
};


class DyLayerStageLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerStageLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerStage);
};

class DySceneStage : public YtScene
{
public:
	virtual void didSceneChanged();
	DySceneStage();
	~DySceneStage();
};


#endif  // __DYSCENESTAGE_H__