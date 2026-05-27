/*
DyLayerShopStaff.h
Doggy chef
@initialize at 130516

@history
130516	yoonsr	initialize
*/


#ifndef __DYLAYERSHOPSTAFF_H__
#define __DYLAYERSHOPSTAFF_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "CCTabView.h"
#include "DyCostume.h"

#include "DyItem.h"
#include "DyMoney.h"
#include "DyRecipe.h"


//#define SHOPSTAFF_UI_TAG_PREVIEW_LAYER              2000
//#define SHOPSTAFF_UI_TAG_NAME                       2001
//#define SHOPSTAFF_UI_TAG_PICTURE                    2002
//#define SHOPSTAFF_UI_TAG_PICTURE_DESC               2003

#define SHOPSTAFF_UI_TAG_LAYER  3000

class DyLayerShopStaff
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
, public CCScrollViewDelegate
{
public:

	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerShopStaff, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
	DyLayerShopStaff();
	virtual ~DyLayerShopStaff();

    virtual void onEnter();
	virtual void onExit();
    
    
    void setPicture();
    void setPannel(int index);
    void resetStaffLayer();
    
    void onPromotionClicked(CCObject * obj);
    void onPopupClicked(cocos2d::CCObject * pSender);
    void promote();
    
    int m_curStaff;
    int m_selectedIndex;
    
    CCMenu* m_menu;
    CCMenuItemImage* m_menuItem[6];
    CCSprite* m_sprDelly[6];
    CCLabelTTF* m_labelDelly[6];
    CCSprite* m_sprPannel[6];
    
	CCScrollView* m_scrollView;
    
    CCLayer* m_layerContent;
    
    virtual void scrollViewDidScroll(CCScrollView* view);
	virtual void scrollViewDidZoom(CCScrollView* view);
	virtual void scrollViewTouchContent(CCScrollView* view, CCPoint position);

    
    void refresh(int idxSelect);
    void refresh();
};


class DyLayerShopStaffLoader : public CCLayerLoader
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerShopStaffLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerShopStaff);
};

#endif  // __DYLAYERSHOPSTAFF_H__