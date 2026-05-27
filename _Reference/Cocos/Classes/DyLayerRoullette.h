/*
DyLayerRoullette.h
Doggy chef
@initialize at 130608

@history
130608	yoonsr	initialize
*/


#ifndef __DYLAYERROULLETTE_H__
#define __DYLAYERROULLETTE_H__

#include "YtScene.h"
#include "YtLayer.h"

#define DYLAYERROULLETTE_TAG                        "DyLayerRoullette"

#define ROULLETTE_UI_TAG_MENU						1000

#define ROULLETTE_UI_TAG_MENUITEM_PLAY              1001
#define ROULLETTE_UI_TAG_MENUITEM_STOP              1002
#define ROULLETTE_UI_TAG_MENUITEM_CLOSE             1003

#define ROULLETTE_UI_TAG_CIRCLE                     2000
#define ROULLETTE_UI_TAG_FRAME                      2100

#define ROULLETTE_UI_TAG_YELLOW0                    3000

#define ROULLETTE_UI_TAG_BLUE0                      3100


#define ROULLETTE_UI_TAG_NOTI                       4000
#define ROULLETTE_UI_TAG_NOTI_CNT                   4001
class DyLayerRoullette
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerRoullette, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	DyLayerRoullette();
	virtual ~DyLayerRoullette();

	void onCloseClicked(cocos2d::CCObject * pSender);
	void onPlayClicked(CCObject * obj);
	void onStopClicked(CCObject * obj);
    
    int m_runsnd;
    CCMenu* m_menu;
    CCSprite* m_sprCircle;
    CCSprite* m_sprFrame;
    
    CCSprite* m_sprLEDYellow[8];
    CCSprite* m_sprLEDBlue[8];
    
    float m_angle;
    float m_vel;
    
    float m_dtRun;
    float m_dtRunTime;
    bool m_runRoulette;
    
    float m_dtBlink;
    
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    
    virtual void keyBackClicked();

    
    virtual void update(float dt);
    
    void getPresent(int index);
    
    void refresh();
    
    bool m_yellow;
    void blinkLED();
	static DyLayerRoullette* create(CCNode* owner, bool callByMainMenu);
    
    
    void callfunc_update_me_success(float dt);
    void onPopupClicked(cocos2d::CCObject * pSender);
    void callfunc_update_me_fail(float dt);

};


class DyLayerRoulletteLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerRoulletteLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerRoullette);
};

#endif  // __DYLAYERROULLETTE_H__