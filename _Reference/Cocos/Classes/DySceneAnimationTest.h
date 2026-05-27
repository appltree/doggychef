/*
DySceneAnimationTest.h
Doggy chef
@initialize at 120929

@history
120929	yoonsr	initialize
*/


#ifndef __DYSCENEANIMATIONTEST_H__
#define __DYSCENEANIMATIONTEST_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "YtAniObject.h"

#define ANIMATIONTEST_UI_TAG_MENU						1000
#define ANIMATIONTEST_UI_TAG_CLOSE						1001


class DyLayerAnimationTest
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	bool m_isDoggy;

	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerAnimationTest, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	void onBackClicked(CCObject * pSender);
	void onSelectAnimalClicked(CCObject * pSender);
	void onOrderClicked(CCObject * pSender);
	void onWalkinClicked(CCObject * pSender);
	void onWalkoutClicked(CCObject * pSender);
	void onEatClicked(CCObject * pSender);
	void onLikeClicked(CCObject * pSender);
	void onAngryClicked(CCObject * pSender);
	void loadAnimation(const char* tag);
	YtAniObject *m_animal;

	DyLayerAnimationTest();
	virtual ~DyLayerAnimationTest();

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

	CCMenu* m_menu;
};


class DyLayerAnimationTestLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerAnimationTestLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerAnimationTest);
};

class DySceneAnimationTest : public YtScene
{
public:
	DySceneAnimationTest();
	~DySceneAnimationTest();
};


#endif  // __DYSCENETITLE_H__