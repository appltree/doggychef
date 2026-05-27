/*
DyLayerPromise.h
Doggy chef
@initialize at 130801

@history
130801	yoonsr	initialize
*/


#ifndef __DYLAYERPromise_H__
#define __DYLAYERPromise_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "YtAniObject.h"

#define DYLAYERPROMISE_TAG_UI_MENU      1000
#define DYLAYERPROMISE_TAG_UI_MENUITEM  1001

#define DYLAYERPROMISE_TAG_UI_FRAME     2000

#define DYLAYERPROMISE_TAG_UI_LAYERRACE 3000

#define DYLAYERPROMISE_TAG_UI_CHECKER1  4000
#define DYLAYERPROMISE_TAG_UI_CHECKER2  4001


class DyLayerTitle;

class DyLayerPromise : public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerPromise, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);
    
    bool m_isCheck1;
    bool m_isCheck2;
    
	void onConfirmClicked(CCObject * obj);
    void onCheck1Clicked(CCObject * obj);
    void onCheck2Clicked(CCObject * obj);
    void onDetail1Clicked(CCObject * obj);
    void onDetail2Clicked(CCObject * obj);
  
	DyLayerPromise();
	virtual ~DyLayerPromise();

    virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

    virtual void onEnter();
	virtual void onExit();

    DyLayerTitle* m_layerTitle;
	static DyLayerPromise* create(CCNode* owner);
};

class DyLayerPromiseLoader : public CCLayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerPromiseLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerPromise);
};


#endif  // __DYLAYERPromise_H__