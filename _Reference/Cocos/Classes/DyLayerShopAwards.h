/*
DyLayerShopAwards.h
Doggy chef
@initialize at 121207

@history
121207	yoonsr	initialize
*/


#ifndef __DYLAYERSHOPAWARDS_H__
#define __DYLAYERSHOPAWARDS_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "CCTabView.h"
#include "DyCostume.h"

#include "DyItem.h"
#include "DyMoney.h"
#include "DyRecipe.h"

#include "YtAniObject.h"


#define SHOPAWARDS_CNT       4

#define SHOPAWARDS_UI_TAG_BG_TOP					1000
#define SHOPAWARDS_UI_TAG_BG_BOTTOM					2000
#define SHOPAWARDS_UI_TAG_DOJANG                    3000

#define SHOPAWARDS_UI_TAG_SERVICE_SHADOW            4001
#define SHOPAWARDS_UI_TAG_SERVICE_NAME              4002
#define SHOPAWARDS_UI_TAG_SERVICE_DOJANG            4100

typedef enum tag_DyLayerShopAwardsType
{
	DyLayerShopAwardsType_Recipe,
	DyLayerShopAwardsType_Rank,
	DyLayerShopAwardsType_Shop,
	DyLayerShopAwardsType_Service,
//    DyLayerShopAwardsType_Social,
}DyLayerShopAwardsType;

class DyLayerShopAwardsTab : public CCTabView
{
public:
	DyLayerShopAwardsTab(CCRect rect = CCRectZero);
};


class DyLayerShopAwards
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
, public CCScrollViewDelegate
, public CCTabViewDelegate
{
public:

	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerShopAwards, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    
	int m_startTab;
	int m_tabIndex;

	CCMenu* m_menu[SHOPAWARDS_CNT];
    

	CCSize m_scrollViewSize;

	CCScrollView* m_tabScrollView[SHOPAWARDS_CNT];

    
    //
    //!DYDATA_CUSTOMER_DEFAULT_CNT+DYDATA_CUSTOMER_ADDED_CNT < (SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED)*2 큰값을 이용해야한다.
    //
    CCSprite* m_sprIndicator[SHOPAWARDS_CNT][DYDATA_CUSTOMER_DEFAULT_CNT+DYDATA_CUSTOMER_ADDED_CNT];
    CCPoint m_posIndicator[SHOPAWARDS_CNT][DYDATA_CUSTOMER_DEFAULT_CNT+DYDATA_CUSTOMER_ADDED_CNT];
    CCPoint m_posIndicatorGap;
    bool m_indicatorGapInc;
    
    CCSprite* m_sprPicture[SHOPAWARDS_CNT][DYDATA_CUSTOMER_DEFAULT_CNT+DYDATA_CUSTOMER_ADDED_CNT];
        
    DyLayerShopAwardsTab* m_tabView;

	YtAniObject *m_customer[DYDATA_CUSTOMER_DEFAULT_CNT+DYDATA_CUSTOMER_ADDED_CNT];
	CCLabelTTF* m_labelNotiShop;
	CCLabelTTF* m_labelNotiService;

	CCLayer* m_contentLayer;

	CCLayerColor* createAwardsLayer(DyLayerShopAwardsType type);
	DyLayerShopAwards();
	virtual ~DyLayerShopAwards();

	virtual void tabDidChange(CCTabView* tabView, int index, CCLayer* contentLayer, bool init, bool redo);

    virtual void onEnter();
	virtual void onExit();

    void updateAnimation();
    
	void onPictureClicked(cocos2d::CCObject * pSender);
    //void onSocialClicked(cocos2d::CCObject * pSender);
    
    
    void addIndicator(CCNode* node, CCPoint position, int mainIndex, int subIndex);
    void endAnimationReward(CCNode* node);
    void endAnimationRewardDojang(CCNode* node);
    
    bool m_isLockButton;
    
    char m_strReward[80];
    
    virtual void scrollViewDidScroll(CCScrollView* view);
	virtual void scrollViewDidZoom(CCScrollView* view);
	virtual void scrollViewTouchContent(CCScrollView* view, CCPoint position);
};


class DyLayerShopAwardsLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerShopAwardsLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerShopAwards);
};

#endif  // __DYLAYERSHOPAWARDS_H__