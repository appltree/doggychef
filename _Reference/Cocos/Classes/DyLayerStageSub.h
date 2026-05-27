/*
DyLayerStageSub.h
Doggy chef
@initialize at 121120

@history
121120	yoonsr	initialize
*/


#ifndef __DYLAYERSTAGESUB_H__
#define __DYLAYERSTAGESUB_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "SlidingMenu.h"
#include "DyMoney.h"
#include "DyTycoon.h"
#include "DyFriend.h"
#include "DyLayerRank_list.h"


#define DYLAYERSTAGESUB_TAG                     "DyLayerStageSub"

#define STAGESUB_UI_TAG_MENU				900
#define STAGESUB_UI_TAG_MENUITEM_LEFT		902
#define STAGESUB_UI_TAG_MENUITEM_RIGHT		903
#define STAGESUB_UI_TAG_MENUITEM_CRAZY		904


#define STAGESUB_UI_TAG_DECO_LEFT		1300
#define STAGESUB_UI_TAG_DECO_RIGHT		1301

#define STAGESUB_UI_TAG_DELLY           2000
#define STAGESUB_UI_TAG_DELLY_TEXT      2001



class DyLayerStageSub
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
, public SlidingMenuGridDelegate
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerStageSub, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

    int m_totalStar;
    
	CCMenu* m_menuMain;
	SlidingMenuGrid* m_menuGrid;
    
	CCArray* m_arrPageCursorBg;
	CCSprite* m_sprPageCursor;

	DyLayerMoney* m_layerMoney;
    
    CCArray* m_arrStageRank;
    
    CCArray* m_arrPictureFrame;
    
    CCArray* m_arrRankMenu;
    
	int m_lastStage;
	void onCloseClicked(CCObject * pSender);
	void onRightClicked(CCObject * pSender);
	void onLeftClicked(CCObject * pSender);
    void onCrazyClicked(CCObject * pSender);
    
	DyLayerStageSub();
	virtual ~DyLayerStageSub();

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    virtual void keyBackClicked();

	CCMenuItemSprite* createStageButton(int stage);
	void stageClicked(CCObject* obj);
    void rankClicked(CCObject* obj);
    
    
	virtual void pageDidChange(SlidingMenuGrid* menugrid, int page);
	virtual void pageMoving(SlidingMenuGrid* menugrid);

    void buildCrazyButton();
    
    void onPopupClicked(cocos2d::CCObject * pSender);
   
    void startGame(bool isCrazyMode);
    void setupRank(int level);
    void sortRank(int level);
    
    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);

    void alarmOpenCrazy(float dt);

    
    DyStageRank* getFirstRank(CCArray* array);
 	static DyLayerStageSub* create(CCNode* owner);
};


class DyLayerStageSubLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerStageSubLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerStageSub);
};


#endif  // __DYLAYERSTAGESUB_H__