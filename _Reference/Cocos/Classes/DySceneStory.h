/*
DySceneStory.h
Doggy chef
@initialize at 130201

@history
130201	yoonsr	initialize
*/


#ifndef __DYSCENESTORY_H__
#define __DYSCENESTORY_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "YtAniObject.h"
#include "CCDate.h"

#define STORY_UI_TAG_MENU				2000
#define STORY_UI_TAG_MENU_SKIP			2001

// 순서 바뀌면 안됨
typedef enum tag_DyStoryType
{
	DyStoryType_Intro,
	DyStoryType_Hamburger,
	DyStoryType_Coffee,
    DyStoryType_China,
    DyStoryType_Pizza,
    DyStoryType_Korean,
    DyStoryType_Ending,
    DyStoryType_Option,
    DyStoryType_Puddle,
}DyStoryType;

typedef enum tag_DyStoryStatus
{
	DyStoryStatus_Story_0,
	DyStoryStatus_Story_0_end,
	DyStoryStatus_Story_1,
	DyStoryStatus_Story_1_end,
	DyStoryStatus_Story_2,
    
	DyStoryStatus_end,
}DyStoryStatus;

class DyLayerStory
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
, public CCBAnimationManagerDelegate
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerStory, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	virtual void ccTouchesBegan(CCSet* touches, CCEvent* event);

    CCDate m_dateStart;
	bool m_toggle;
	int m_menuskipcount;
	DyStoryStatus m_storystatus;
	DyStoryType m_storytype;

	void onSkipClicked(CCObject * pSender);

	DyLayerStory();
	virtual ~DyLayerStory();

	virtual void onEnter();
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

	void cbUpdate(float dt);
	virtual void completedAnimationSequenceNamed(const char *name);
};


class DyLayerStoryLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerStoryLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerStory);
};

class DySceneStory : public YtScene
{
public:
	virtual void didSceneChanged();

	DySceneStory();
	~DySceneStory();
};


#endif  // __DYSCENESTORY_H__