/*
DyLayerNotice.h
Doggy chef
@initialize at 130922

@history
130922	yoonsr	initialize
*/


#ifndef __DYLAYERNOTICE_H__
#define __DYLAYERNOTICE_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "DyNotice.h"

#define NOTICE_UI_TAG_LAYER                     900
#define NOTICE_UI_TAG_MENU						1000
#define NOTICE_UI_TAG_MENUITEM_CLOSE            1001
#define NOTICE_UI_TAG_MENUITEM_SHOW             1002
#define NOTICE_UI_TAG_CHECK                     2000


class DyLayerNotice
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerNotice, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	DyLayerNotice();
	virtual ~DyLayerNotice();

	void onCloseClicked(cocos2d::CCObject * pSender);
	void onShowClicked(cocos2d::CCObject * pSender);
	CCMenu* m_menu;
    DyNotice* m_notice;
    
    CCArray* m_arrUrl;
    
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    virtual void keyBackClicked();
    virtual void onEnter();
    
    void onButtonClicked(CCObject *obj);

    bool m_canShowToday;
    
    static DyLayerNotice* create(CCNode* owner, DyNotice* notice);
};


class DyLayerNoticeLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerNoticeLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerNotice);
};

#endif  // __DYLAYERNOTICE_H__