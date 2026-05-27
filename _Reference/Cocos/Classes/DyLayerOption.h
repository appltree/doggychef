/*
DyLayerOption.h
Doggy chef
@initialize at 121021

@history
121021	yoonsr	initialize
*/


#ifndef __DYLAYEROPTION_H__
#define __DYLAYEROPTION_H__

#include "YtScene.h"
#include "YtLayer.h"

#define DYLAYEROPTION_TAG                       "DyLayerOption"

#define OPTION_UI_TAG_MENU						1000
#define OPTION_UI_TAG_MENU_ITEM_RESET           1002
#define OPTION_UI_TAG_MENU_BLOG                 1100

#define OPTION_UI_TAG_MENU_BOARD               1200

#define OPTION_UI_TAG_BGM_TEXT					2000
#define OPTION_UI_TAG_BGM_TEXT_SHADOW			2001

#define OPTION_UI_TAG_EFFECT_TEXT				2010
#define OPTION_UI_TAG_EFFECT_TEXT_SHADOW		2011

#define OPTION_UI_TAG_PUSH_TEXT					2020
#define OPTION_UI_TAG_PUSH_TEXT_SHADOW			2021

#define OPTION_UI_TAG_VIBRATION_TEXT			2030
#define OPTION_UI_TAG_VIBRATION_TEXT_SHADOW		2031

#define OPTION_UI_TAG_BGM_CHECKER				3000
#define OPTION_UI_TAG_EFFECT_CHECKER			3010
#define OPTION_UI_TAG_PUSH_CHECKER				3020
#define OPTION_UI_TAG_VIBRATION_CHECKER			3030

#define OPTION_UI_TAG_MY_VER_TEXT               2100
#define OPTION_UI_TAG_APP_ID                    2101
#define OPTION_UI_TAG_QA_TEXT					2102

#define OPTION_UI_TAG_RESET_TEXT                4000

class DyLayerOption
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerOption, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	bool m_callByMainMenu;

	DyLayerOption();
	virtual ~DyLayerOption();

	void onCloseClicked(cocos2d::CCObject * pSender);
	void onOptionClicked(CCObject * obj);
	void onLeaveClicked(CCObject * obj);
	void onSendmailClicked(CCObject * obj);
	void onLogoutClicked(CCObject * obj);
    void onCopyIDClicked(CCObject * obj);
    void onStoryClicked(CCObject * obj);
    void onCreditsClicked(CCObject * obj);
    void onBlogClicked(CCObject * obj);
    //void onResetClicked(CCObject * obj);
    
    void onBoardClicked(CCObject * obj);
    
    
	CCMenu* m_menu;
    CCMenu* m_menuBlog;
    
    CCMenu* m_menuBoard;
    
    
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    virtual void keyBackClicked();

    void onPopupClicked(cocos2d::CCObject * pSender);
    
	static DyLayerOption* create(CCNode* owner, bool callByMainMenu);
    
    
    void callfunc_logout_success(float dt);
    void callfunc_logout_fail(float dt);
    
    void callfunc_unregist_success(float dt);
    void callfunc_unregist_fail(float dt);
    
    void callfunc_push_onoff_success(float dt);
    void callfunc_push_onoff_fail(float dt);
    
    
    void callfunc_delete_me_success(float dt);
    void callfunc_delete_me_fail(float dt);
    
    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);


    void showBlogButton();
    void hideBlogButton();
    
    void onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response);

};


class DyLayerOptionLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerOptionLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerOption);
};

#endif  // __DYLAYEROPTION_H__