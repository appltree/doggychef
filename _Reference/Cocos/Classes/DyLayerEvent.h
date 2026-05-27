/*
DyLayerEvent.h
Doggy chef
@initialize at 140130

@history
140130	yoonsr	initialize
*/


#ifndef __DYLAYEREVENT_H__
#define __DYLAYEREVENT_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "YtAniObject.h"
#include "DyFriend.h"
#include "DyGift.h"

#define DYLAYEREVENT_TAG          "DyLayerEvent"

#define EVENT_UI_TAG_MENU                 1000
#define EVENT_UI_TAG_MENUITEM_BUY         1001
#define EVENT_UI_TAG_MENUITEM_CLOSE       1002

typedef enum tag_DyEventStatus
{
    DyEventStatus_Delly,
    DyEventStatus_Hero,
    
}DyEventStatus;



class DyLayerEvent : public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerEvent, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);
    
	void onCloseClicked(CCObject * obj);

    void onBuyClicked(CCObject * obj);
    
    virtual void keyBackClicked();

    CCMenu* m_menu;
    
	DyLayerEvent();
	virtual ~DyLayerEvent();

    virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

    virtual void onEnter();
	virtual void onExit();
 
    void onPopupClicked(cocos2d::CCObject * pSender);

    DyEventStatus m_event_status;
    int m_prevDelly;
    
	static DyLayerEvent* create(CCNode* owner, DyEventStatus status);
};


class DyLayerEventLoader : public CCLayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerEventLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerEvent);
};


#endif  // __DYLAYERPresent_H__