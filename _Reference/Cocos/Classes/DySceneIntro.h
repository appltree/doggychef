/*
DySceneIntro.h
Monster showdown
@initialize at 130522

@history
130522	yoonsr	initialize
*/


#ifndef __DySCENEINTRO_H__
#define __DySCENEINTRO_H__

#include "YtScene.h"
#include "YtLayer.h"

class DyLayerIntro
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerIntro, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	DyLayerIntro();
	virtual ~DyLayerIntro();

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
	virtual void onEnter();
    
    int m_step;
    
    void ccTouchesBegan(CCSet* touches, CCEvent* event);
    void ccTouchesEnded(CCSet* touches, CCEvent* event);
    void ccTouchesMoved(CCSet* touches, CCEvent* event);
    
    void cbNext(float dt);
    void cbTouchEnable(float dt);
};

class DyLayerIntroLoader : public CCLayerLoader
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerIntroLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerIntro);
};


class DySceneIntro : public YtScene
{
public:
	DySceneIntro();
	~DySceneIntro();
	
	virtual void didSceneChanged();
};


#endif  // __DySCENEINTRO_H__