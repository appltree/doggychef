/*
DyLayerPresent.h
Doggy chef
@initialize at 130731

@history
130731	yoonsr	initialize
*/


#ifndef __DYLAYERPresent_H__
#define __DYLAYERPresent_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "YtAniObject.h"
#include "DyFriend.h"
#include "DyGift.h"

#define DYLAYERPRESENT_TAG          "DyLayerPresent"

typedef enum tag_DyPresentStatus
{
    DyPresentStatus_Present,
    DyPresentStatus_Ask,
    
}DyPresentStatus;


#define PRESENT_UI_TAG_MENU         1000
#define PRESENT_UI_TAG_MENUITEM_PRESENT 1001    //(tab)
#define PRESENT_UI_TAG_MENUITEM_ASK     1002    //(tab)

#define PRESENT_UI_TAG_MENUITEM_GUM     1003
#define PRESENT_UI_TAG_MENUITEM_GAEST   1004
#define PRESENT_UI_TAG_MENUITEM_DELLY   1005

#define PRESENT_UI_TAG_PICTURE          2000
#define PRESENT_UI_TAG_NAME             2001


#define PRESENT_UI_TAG_TIME_REMAIN      3000

#define PRESENT_UI_TAG_MESSAGE          4000


#define PRESENT_UI_TAG_DELLY_DECO_SHADOW       5000
#define PRESENT_UI_TAG_DELLY_DECO_HEART       5001


class DyLayerPresent : public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerPresent, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);
    
	void onCloseClicked(CCObject * obj);

    void onPresentClicked(CCObject * obj);
    void onAskClicked(CCObject * obj);
    void onGumClicked(CCObject * obj);
    void onGaestClicked(CCObject * obj);
    void onDellyClicked(CCObject * obj);
    
    
    virtual void keyBackClicked();

    DyPresentStatus m_status;
    
    DyFriend* m_friend;
    
    CCMenu* m_menu;
    
    CCLabelTTF* m_labelRemain;
    
    bool m_sendGum;
    
    void chagneStatus(DyPresentStatus status);
    
	DyLayerPresent();
	virtual ~DyLayerPresent();

    virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

    virtual void onEnter();
	virtual void onExit();
    
    void callfunc_send_present_success(float dt);
    void callfunc_send_present_fail(float dt);
    
    void askPresent(DyGiftType gifttype);
    
    void updateRemainTime(float dt);
    
    void onPopupClicked(cocos2d::CCObject * pSender);
    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);

	static DyLayerPresent* create(CCNode* owner, DyFriend* _friend);
};

class DyLayerPresentLoader : public CCLayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerPresentLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerPresent);
};


#endif  // __DYLAYERPresent_H__