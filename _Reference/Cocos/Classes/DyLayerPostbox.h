/*
DyLayerPostbox.h
Doggy chef
@initialize at 121231

@history
121231	yoonsr	initialize
*/


#ifndef __DYLAYERPOSTBOX_H__
#define __DYLAYERPOSTBOX_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "CCTabView.h"
#include "DyCostume.h"

#include "DyItem.h"
#include "DyMoney.h"
#include "DyGift.h"

#define DYLAYERPOSTBOX_TAG                      "DyLayerPostbox"

#define POSTBOX_UI_TAG_MENU						1000
#define POSTBOX_UI_TAG_MENUITEM_SENDGIFT		1001
#define POSTBOX_UI_TAG_MENUITEM_ACCEPT_ALL		1002

//#define POSTBOX_UI_TAG_LABEL_NOTPRESENT         2000
class DyLayerPostbox
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
, public CCScrollViewDelegate
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerPostbox, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	DyLayerPostbox();
	virtual ~DyLayerPostbox();

	void onCloseClicked(cocos2d::CCObject * pSender);
	
    void onAcceptallClicked(cocos2d::CCObject * pSender);
	void onAcceptClicked(CCObject * obj);
    
	void onViewClicked(cocos2d::CCObject * pSender);
	void onDeleteClicked(CCObject * obj);
    
    CCMenu* m_menu;
	CCMenu* m_menuRecieve;

    CCSize m_sizeItem;
    int m_itemCount;
    
	DyLayerMoney* m_layerMoney;
	CCScrollView* m_layerContents;
	CCLayer* createListLayer(float width);

    CCArray* m_arrPictureFrame;
    CCArray* m_arrNormalFriend;
    
    DyGift* m_processGift;
    
    void refresh();
    
    
	virtual void destory();
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

    
    void callfunc_load_accept_message_success(float dt);
    void callfunc_load_accept_message_fail(float dt);
    
    
    
    
    void callfunc_load_accept_allmessages_success(float dt);
    void callfunc_load_accept_allmessages_fail(float dt);

    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);

    void onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response);
	static DyLayerPostbox* create(CCNode* owner);

};


class DyLayerPostboxLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerPostboxLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerPostbox);
};

#endif  // __DYLAYERPOSTBOX_H__