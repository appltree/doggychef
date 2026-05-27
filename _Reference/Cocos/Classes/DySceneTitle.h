/*
DySceneTitle.h
Doggy chef
@initialize at 120928

@history
120928	yoonsr	initialize
*/


#ifndef __DYSCENETITLE_H__
#define __DYSCENETITLE_H__

#define TITLE_UI_TAG_BG             2000
//#define TITLE_UI_TAG_TITLE      3000
//#define TITLE_UI_TAG_TOUCH          3000
//#define TITLE_UI_TAG_TOUCH_FOOT     3001

#define TITLE_UI_TAG_MENU           1000
#define TITLE_UI_TAG_MENUITEM_LOGIN           1001

#define TITLE_UI_TAG_MENUITEM_LOGIN_GUEST           1002
#define TITLE_UI_DECO_LAYER                         3000

#include "YtScene.h"
#include "YtLayer.h"

#include "DyPopup.h"

#define DYLAYERTITLE_TAG            "DyLayerTitle"

typedef enum tag_KakaoAPIStatus
{
    KakaoAPIStatus_None,
    KakaoAPIStatus_IsLogin,
    KakaoAPIStatus_Login,
    KakaoAPIStatus_LoginStarted,
    
    KakaoAPIStatus_LoadGameInfo,
    KakaoAPIStatus_LoadGameMe,
}KakaoAPIStatus;


class DyLayerTitle
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
, public CCBAnimationManagerDelegate

{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerTitle, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

    CCMenu* m_menu;
    
    bool m_isDataReset;
    
    bool m_isPromised;
    
	DyLayerTitle();
	virtual ~DyLayerTitle();
	virtual void ccTouchesBegan(CCSet* touches, CCEvent* event);

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

	virtual void onEnter();
	virtual void onExit();
    
	virtual void completedAnimationSequenceNamed(const char *name);
    
    void setLoginUI();
    
    void checkAppStatus();
    void goNext();
    void goMainMenu();
    
    void onLoginClicked(cocos2d::CCObject * pSender);
    void onGuestLoginClicked(cocos2d::CCObject * pSender);
    
    CCArray* m_arrFriend;
    
    KakaoAPIStatus m_callFuncStep;
    int m_callFuncValue;
    std::string callFuncString;
    
    void net_IsLogin();
    void net_Login(bool isGeust);
 
    //void net_LoadGameMe();
    
    void onPopupClicked(cocos2d::CCObject * pSender);


    virtual void update(float dt);
    
    virtual void keyBackClicked();
    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);
    
    
    void startBlink(CCNode* node);

    
};


class DyLayerTitleLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerTitleLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerTitle);
};

class DySceneTitle : public YtScene
{
public:
    
	DySceneTitle();
	~DySceneTitle();

	virtual void didSceneChanged();
//	void cbCloseLoadingPopup(float dt);
//
//    void onPopupClicked(cocos2d::CCObject * pSender);
//    
    
    
    void resumeByPhone();
    void pauseByPhone();

};


#endif  // __DYSCENETITLE_H__