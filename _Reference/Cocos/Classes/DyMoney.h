/*
DyMoney.h
Doggy chef
@initialize at 121127

@history
121127	yoonsr	initialize
*/

#ifndef __DYMONEY_H__
#define __DYMONEY_H__

#include "YtLib.h"
#include "DyConfig.h"
#include "YtLayer.h"
#include "CCDate.h"

#include "DyPopup.h"

typedef enum tag_DyBuyMethod
{
	DyBuyMethod_Cash,
	DyBuyMethod_Delly,
	DyBuyMethod_Gold,
    DyBuyMethod_GoldAndDelly,
}DyBuyMethod;


typedef enum tag_DyBuyItemType
{
	DyBuyItemType_Delly,
	DyBuyItemType_Costume,
	DyBuyItemType_Gum,
	DyBuyItemType_CostumeArray,
	DyBuyItemType_Recipe,
	DyBuyItemType_Shop,

}DyBuyItemType;

class DyGumInfo : public CCObject
{
public:
	int m_id;
	int m_gum_cnt;
	int m_price_delly;
	int m_bonus;
};

class DyDellyInfo : public CCObject
{
public:
	int m_id;
	int m_delly_cnt;
	int m_price_won;
    std::string m_price_dollar;
	int m_bonus;
    std::string m_product_id;
    std::string m_present_id;
};


class DyLayerMoney;

class DyMoneyManager : public CCObject
{
public:
	int m_exchange_rate;
	CCArray* m_arrDellyInfos;
	CCArray* m_arrGumInfos;

	DyMoneyManager();
	~DyMoneyManager();

    bool m_isDoCheckTimeForGum;
    
	static DyMoneyManager* sharedMoneyManager();

	void dataSave();

	void addDelly(int value, bool apply);
	void addGold(int value, bool apply);
	void addGum(int value, bool apply);

    void applyDelly();
    void applyGold();
    void applyGum();
    
    DyPopup* m_popup_process;
    
	int getGold();
	int getDelly();
	unsigned int getGum();

	CCArray* m_arrMoneyLayer;

	void updateMoneyLayer(bool isGumTime, int remainTime);
	void addMoneyLayer(DyLayerMoney* layer);
	void removeMoneyLayer(DyLayerMoney* layer);

	void updateGum(float dt);


	void* m_processItem;
	DyBuyItemType m_processItemType;
	DyBuyMethod m_method;

	void questBuy(DyBuyItemType itemType, void* info, DyBuyMethod method);
	void onPopupClicked(cocos2d::CCObject * pSender);

	void* alramNotEnoughGold(bool showDirect, bool needPause);
	void* alramNotEnoughDelly(bool showDirect, bool needPause);
    
    
    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);

    
    void callfunc_iab_success(float dt);
    void callfunc_iab_fail(float dt);

    
};


typedef enum tag_DyLayerMoneyType
{
	DyLayerMoneyType_Normal,
	DyLayerMoneyType_OnlyGum,
}DyLayerMoneyType;
class DyLayerMoneyDelegate 
{
public:
	virtual void buttonClicked(DyLayerMoney* moneyView, int index){}
};


class DyLayerMoney : public YtLayer
{
public:
	CCMenu* m_menu;
	DyLayerMoneyType m_layerType;
    int m_gum_bg_color;

	DyLayerMoneyDelegate* m_delegate;

	CCSprite* m_sprGum[5];

	CCLabelTTF* m_labelGumStatus;
	CCLabelTTF* m_labelDellyStatus;
	CCLabelTTF* m_labelGoldStatus;

    virtual void onEnter();
	virtual void onExit();
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    
	void updateStatus(bool isGumTime, int remainTime);
	void buttonClicked(CCObject * obj);
	DyLayerMoney(DyLayerMoneyType layerType, int gum_bg_color);
	virtual ~DyLayerMoney();
};



#endif  // __DYMONEY_H__