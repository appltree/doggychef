/*
DyLayerInvite.h
Doggy chef
@initialize at 130104

@history
130104	yoonsr	initialize
*/


#ifndef __DYLAYERINVITE_H__
#define __DYLAYERINVITE_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "YtAniObject.h"
#include "DyFriend.h"


#define DYLAYERINVITE_TAG           "DyLayerInvite"

#define INVITE_UI_TAG_MENU          1000
#define INVITE_UI_TAG_MENU_BOARD    1100
#define INVITE_UI_TAG_CONTENTLAYER  2000

#define INVITE_UI_TAG_REMAIN        3000
#define INVITE_UI_TAG_WAIT          3001
#define INVITE_UI_TAG_NOTICE        3002



class DyLayerInvite : public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
, public CCScrollViewDelegate
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerInvite, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

    
    int m_present;
    
	CCMenu* m_menuInvite;
	CCMenu* m_menu;
    
    CCMenu* m_menuBoard;
    

	void onCloseClicked(CCObject * obj);

	DyLayerInvite();
	virtual ~DyLayerInvite();

	CCScrollView* m_layerContents;

	CCLayer* createListLayer(float width);
    
    CCArray* m_arrPictureFrame;
    CCArray* m_arrNormalFriend;
   
    virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    virtual void keyBackClicked();

    virtual void onEnter();
	virtual void onExit();
	virtual void scrollViewDidScroll(CCScrollView* view);
	virtual void scrollViewDidZoom(CCScrollView* view);
	virtual void scrollViewTouchContent(CCScrollView* view, CCPoint position);

	void onInviteClicked(CCObject * obj);
    void onPopupClicked(cocos2d::CCObject * pSender);
    void onBoardClicked(CCObject * obj);
    
    CCSize m_sizeItem;
    int m_itemCount;
    
    DyFriend* m_processFriend;
    CCMenuItemImage* m_menuitemProcess;
    
    void callfunc_send_invite_success(float dt);
    void callfunc_send_invite_fail(float dt);

    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);

    void loadFriends();
    void refresh();
    
    //void onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response);

    
    void showBoardButton();
    void hideBoardButton();

    
	static DyLayerInvite* create(CCNode* owner);
};

class DyLayerInviteLoader : public CCLayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerInviteLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerInvite);
};


#endif  // __DYLAYERINVITE_H__