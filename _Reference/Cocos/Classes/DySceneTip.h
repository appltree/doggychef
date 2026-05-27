/*
DySceneTip.h
Doggy chef
@initialize at 130427

@history
130427	yoonsr	initialize
*/


#ifndef __DYSCENETIP_H__
#define __DYSCENETIP_H__

#include "YtScene.h"
#include "YtLayer.h"

#define TIP_TAG_IMAGE       1000
#define TIP_TAG_CONTENT     1001
#define TIP_TAG_TAPTHESCREEN    2000

class DyLayerTip
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener

{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerTip, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	DyLayerTip();
	virtual ~DyLayerTip();

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

	virtual void onEnter();
	virtual void onExit();
    
    void nextStep();
    
    void nextStep_Touch();
    
    
    virtual void ccTouchesBegan(CCSet* touches, CCEvent* event);

    
    CCSprite* m_sprTabScreen;
};


class DyLayerTipLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerTipLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerTip);
};

class DySceneTip : public YtScene
{
public:
	DySceneTip();
	~DySceneTip();

	virtual void didSceneChanged();
};


#endif  // __DYSCENETIP_H__