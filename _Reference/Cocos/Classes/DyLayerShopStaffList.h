/*
DyLayerShopStaffList.h
Doggy chef
@initialize at 140328

@history
140328	yoonsr	initialize
*/


#ifndef __DYLAYERSHOPSTAFFLIST_H__
#define __DYLAYERSHOPSTAFFLIST_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "CCTabView.h"
#include "DyCostume.h"

#include "DyItem.h"
#include "DyMoney.h"
#include "DyRecipe.h"
#include "DyStaff.h"
#include "DyLayerShopStaff.h"

#define SHOPSTAFF_UI_TAG_LAYER  3000

class DyLayerShopStaffList
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
, public CCScrollViewDelegate
{
public:

	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerShopStaffList, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
	DyLayerShopStaffList();
	virtual ~DyLayerShopStaffList();

    virtual void onEnter();
	virtual void onExit();
    
    CCLayer* m_layerContent;
    int m_startIndex;
    //int m_selectedIndex;
    
    DyLayerShopStaff* m_layerShopStaff;
    
    void resetLayer();
    
    void onPopupClicked(cocos2d::CCObject * pSender);
    
    void showList();
    
    CCMenu* m_menu;
    
    void scout(int index);

	CCScrollView* m_scrollView;
    
    void onBuyClicked(CCObject * obj);
    void enterStaff(int index);

    CCScale9Sprite* m_sprStaffBg[STAFF_CNT];
    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);

    
    virtual void scrollViewDidScroll(CCScrollView* view);
	virtual void scrollViewDidZoom(CCScrollView* view);
	virtual void scrollViewTouchContent(CCScrollView* view, CCPoint position);
};


class DyLayerShopStaffListLoader : public CCLayerLoader
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerShopStaffListLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerShopStaffList);
};

#endif  // __DYLAYERSHOPSTAFFLIST_H__