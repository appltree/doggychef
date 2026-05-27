/*
DyLayerPresentDelly.h
Doggy chef
@initialize at 130731

@history
130731	yoonsr	initialize
*/


#ifndef __DYLAYERPresentDelly_H__
#define __DYLAYERPresentDelly_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "CCTabView.h"
#include "DyCostume.h"

#include "DyItem.h"
#include "DyMoney.h"

#include "DyFriend.h"

#define DYLAYERPRESENTDELLY_TAG                             "DyLayerPresentDelly"

#define PRESENTDELLY_UI_TAG_MENU                            10000

#define PRESENTDELLY_UI_TAG_GROUP_0                         1000
#define PRESENTDELLY_UI_TAG_GROUP_1                         2000
#define PRESENTDELLY_UI_TAG_GROUP_2                         3000
#define PRESENTDELLY_UI_TAG_GROUP_3                         4000
#define PRESENTDELLY_UI_TAG_GROUP_4                         5000
#define PRESENTDELLY_UI_TAG_SUB_DELLY_COUNT					1
#define PRESENTDELLY_UI_TAG_SUB_BONUS_COUNT					2
#define PRESENTDELLY_UI_TAG_SUB_MENU						20
#define PRESENTDELLY_UI_TAG_SUB_MENUITEM_BUY				21
#define PRESENTDELLY_UI_TAG_SUB_WON							30
#define PRESENTDELLY_UI_TAG_SUB_PRICE						31
#define PRESENTDELLY_UI_TAG_TO                              7000

class DyLayerPresent;

class DyLayerPresentDelly
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerPresentDelly, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	DyLayerPresentDelly();
	virtual ~DyLayerPresentDelly();

	void onCloseClicked(cocos2d::CCObject * pSender);
    void onBuyClicked(cocos2d::CCObject * pSender);
    
    //DyLayerPresent* m_layerPresent;
    DyDellyInfo* m_process_delly;
    
	CCMenu* m_menu;
    
    CCMenu* m_menuDelly[6];
	
    DyFriend* m_friend;
    
	virtual void destory();
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

    virtual void onEnter();
	virtual void onExit();
    
//    void callfunc_send_present_success(float dt);
//    void callfunc_send_present_fail(float dt);

    void callfunc_iab_success(float dt);
    void callfunc_iab_fail(float dt);
    
    void onPopupClicked(cocos2d::CCObject * pSender);
    
    virtual void keyBackClicked();
    
    void onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response);

	static DyLayerPresentDelly* create(CCNode* owner, DyFriend* _friend);
    
    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);

    

};


class DyLayerPresentDellyLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerPresentDellyLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerPresentDelly);
};

#endif  // __DYLAYERPresentDelly_H__