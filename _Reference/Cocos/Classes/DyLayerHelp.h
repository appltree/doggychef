/*
DyLayerHelp.h
Doggy chef
@initialize at 121022

@history
121022	yoonsr	initialize
*/


#ifndef __DYLAYERHELP_H__
#define __DYLAYERHELP_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "YtAniObject.h"

#define HELP_UI_TAG_CONTENT			2000

#define HELP_UI_TAG_MENU						1000
#define HELP_UI_TAG_CLOSE						1001
#define HELP_UI_TAG_LEFT						1100
#define HELP_UI_TAG_RIGHT						1101

#define HELP_PAGE_CNT				8


class DyLayerHelp
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerHelp, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	YtAniObject *m_content[HELP_PAGE_CNT];

	CCArray* m_arrPageCursorBg;
	CCSprite* m_sprPageCursor;

	YtLayer* m_layerContent;
	DyLayerHelp();
	virtual ~DyLayerHelp();

	bool m_touch;
	CCPoint m_posTouch;
	int m_currentPage;
	void onCloseClicked(cocos2d::CCObject * pSender);
	void onLeftClicked(cocos2d::CCObject * pSender);
	void onRightClicked(cocos2d::CCObject * pSender);
	void endAnimationPage(CCNode* node);
	void movePage(int page);
	CCMenu* m_menu;

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    virtual void keyBackClicked();

	virtual void ccTouchesBegan(CCSet* touches, CCEvent* event);
	virtual void ccTouchesEnded(CCSet* touches, CCEvent* event);
	//virtual void ccTouchesMoved(CCSet* touches, CCEvent* event);

	static DyLayerHelp* create(CCNode* owner);
};


class DyLayerHelpLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerHelpLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerHelp);
};

#endif  // __DYSCENETITLE_H__