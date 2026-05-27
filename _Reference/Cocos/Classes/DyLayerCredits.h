/*
DyLayerCredits.h
Doggy chef
@initialize at 130802

@history
130828	yoonsr	initialize
*/


#ifndef __DYLAYERCREDITS_H__
#define __DYLAYERCREDITS_H__

#include "YtScene.h"
#include "YtLayer.h"

#define CREDIT_UI_TAG_MENU          1000


class DyLayerCredits
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerCredits, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	DyLayerCredits();
	virtual ~DyLayerCredits();

	void onCloseClicked(cocos2d::CCObject * pSender);
	
	CCMenu* m_menu;

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

    virtual void keyBackClicked();

	static DyLayerCredits* create(CCNode* owner, bool callByMainMenu);
};


class DyLayerCreditsLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerCreditsLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerCredits);
};

#endif  // __DYLAYERCREDITS_H__