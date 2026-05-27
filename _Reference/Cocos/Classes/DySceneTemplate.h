/*
DySceneStage.h
Doggy chef
@initialize at 121120

@history
121120	yoonsr	initialize
*/


#ifndef __DYSCENETEMPLATE_H__
#define __DYSCENETEMPLATE_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "YtAniObject.h"


class DyLayerTemplate
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerTemplate, create);
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

	DyLayerTemplate();
	virtual ~DyLayerTemplate();

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
};


class DyLayerTemplateLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerTemplateLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerTemplate);
};

class DySceneTemplate : public YtScene
{
public:
	DySceneTemplate();
	~DySceneTemplate();
};


#endif  // __DYSCENETEMPLATE_H__