/*
YtAniObject.h
YongTrim Lib
@initialize at 121002

@history
121002	yoonsr	initialize
*/


#ifndef __YTANIOBJECT_H__
#define __YTANIOBJECT_H__

#include "YtLayer.h"

class YtAniObjectDelegate 
{
public:
	virtual void onClicked(){}
};



class YtAniObject : public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(YtAniObject, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	YtAniObject();
	virtual ~YtAniObject();

	CCBAnimationManager *m_actionManager;

	static YtAniObject* loadAniObject(CCObject* owner, const char* pCCBFileName);
	static void unloadAniObject(YtAniObject* aniObject);
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

	void pauseAnimation();
	void resumeAnimation();
	bool isRun();
	bool m_isPaused;

	YtAniObjectDelegate* m_delegate;

	void onClicked(cocos2d::CCObject * pSender);


};


class YtAniObjectLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(YtAniObjectLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(YtAniObject);
};

#endif  // __YTANIOBJECT_H__