/*
DySceneMainMenu.h
Doggy chef
@initialize at 120928

@history
120928	yoonsr	initialize
*/


#ifndef __DYSCENEMAINMENU_H__
#define __DYSCENEMAINMENU_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "DyMoney.h"
#include "YtAniObject.h"
#include "DyLayerRank.h"
#include "DyPopup.h"


#define RANK_POS_SHOW   ccp(217, 355)
#define RANK_POS_HIDE   ccp(-500, 355)


#define DYLAYERMAINMENU_TAG                         "DyLayerMainMenu"

#define MAINMENU_UI_TAG_MENU						1000
#define MAINMENU_UI_TAG_MENUITEM_POSTBOX			1001
#define MAINMENU_UI_TAG_MENUITEM_SHOP				1002
#define MAINMENU_UI_TAG_MENUITEM_MYROOM				1003
#define MAINMENU_UI_TAG_MENUITEM_ROULLETTE          1004
#define MAINMENU_UI_TAG_MENUITEM_FRIENDS            1005

#define MAINMENU_UI_TAG_RANKLAYER					3000

#define MAINMENU_UI_TAG_PLAY_P						4000
#define MAINMENU_UI_TAG_PLAY_L						4001
#define MAINMENU_UI_TAG_PLAY_A						4002
#define MAINMENU_UI_TAG_PLAY_Y						4003


class DyLayerMainMenu
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerMainMenu, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	DyLayerMainMenu();
	virtual ~DyLayerMainMenu();
    
    bool m_check_init;
    DyPopup *m_popup_process;
    
    CCMenu* m_menu;
    CCMenu* m_menuRank;
	
    CCSprite* m_sprGiftNew;
    CCSprite* m_sprRoulletteFree;
    
	DyLayerMoney* m_layerMoney;
	DyLayerRank* m_layerRank;
    DyLayerRank* m_layerRankGlobal;
    
	YtAniObject *m_doggy;

	void onGameStartClicked(cocos2d::CCObject* pSender);
	void onAnimationTestClicked(cocos2d::CCObject * pSender);
	void onCheatClicked(cocos2d::CCObject * pSender);
	void onMyRoomClicked(cocos2d::CCObject * pSender);
	void onShopClicked(cocos2d::CCObject * pSender);
	void onPostboxClicked(cocos2d::CCObject * pSender);
	void onHelpClicked(cocos2d::CCObject * pSender);
	void onOptionClicked(cocos2d::CCObject * pSender);

	void onPopupClicked(cocos2d::CCObject * pSender);
    void onEventClicked(cocos2d::CCObject * pSender);
    void onFriendClicked(cocos2d::CCObject * pSender);
    void onDoggyClicked(cocos2d::CCObject * pSender);
	
    void checkPresent();

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
	virtual void keyBackClicked();

    virtual void update(float dt);

    void callNoticeCheck(float dt);
    
	virtual void onEnter();
    void onPresentPopupClicked(cocos2d::CCObject * pSender);
    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);
    
    void callfunc_update_me_success2(float dt);
    void callfunc_update_me_fail2(float dt);
    
    void callfunc_load_game_messages_success(float dt);
    void callfunc_load_game_messages_fail(float dt);
    
    void callfunc_nickname_success(float dt);
    
    
    void onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response);
    void openPostBox();
    
    void checkEvent();
    void onRankTabClicked(CCObject * obj);

    
};


class DyLayerMainMenuLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerMainMenuLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerMainMenu);													    
};


class DySceneMainMenu : public YtScene
{
public:
	DySceneMainMenu();
	~DySceneMainMenu();
	
	virtual void didSceneChanged();
};


#endif  // __DYSCENEMAINMENU_H__