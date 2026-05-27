/*
DyCustomer.h
Doggy chef
@initialize at 121008

@history
121008	yoonsr	initialize
*/


#ifndef __DYCUSTOMER_H__
#define __DYCUSTOMER_H__

#include "DyConfig.h"

#include "YtObject.h"
#include "DyCharacter.h"


typedef enum tag_DyCustomerType
{
	DyCustomerType_Normal = 0,
    DyCustomerType_Gaest,
	DyCustomerType_Delivery
}DyCustomerType;



class DyCustomer : public CCObject
{
private:
    CCLayer* m_layerCharacter;
    
public:
    DyCustomerType m_customerType;
    float m_dtDangling;
    
    int m_idCharacter;
	
	DyCustomer(CCLayer* layer, int characterID, int slot, DyCustomerType customerType);
	~DyCustomer();
	
	DyCharacter *m_character;
	CCArray *m_arrOrderRecipe;
	CCArray* m_arrTotMateiralStatus;
	CCArray* m_arrSellRecipe;

    
	CCRect m_rectTouch;
    
	int m_slotidx;
	int m_moveCnt;
    
	bool m_hasSide;
    bool m_isSideByCashier;
	
	int m_payTip;
	int m_pay;
	float m_payMux;
	int m_payFeverMux;

	float m_deltaPay;
	bool m_startBlinkPay;

	void endAnimationDish(CCNode* node);
	void updateStatus(bool order);
	void updateTotalMaterial(bool update);
	void addPay(int plus, float mux);
	void updateRecipe();
    int getOrderMaterialCnt();
};


class DyCustomerManager : public CCObject
{
private:

    
public:
    
    //
    // 손님을 생성할 수 있는가?
    bool m_isCreatableCustomer;
    
    bool m_isOccupied[MAX_SLOT];
    int m_customer_term;
    
    void pauseCustomers();
    void resumeCustomers();
    
    int m_cntCustomerAccu;
    int m_cntCustomerSuccess;
    
	int m_cntCustomer;
	DyCustomer* m_arrCustomer[MAX_SLOT];
	CCArray* m_arrCustomerOrder;
    
	bool m_isHasGuestCustomer;
    bool m_isEnteredGuestCustomer;
    bool m_isCreateOpportunityGeast;
    
	float m_deltaCreateCustomerTime;
    
 	DyCustomerManager();
	~DyCustomerManager();
    
    void initCustomers();
   
    void updateCustomer(float dt);
    DyCustomer* createCustomer(bool isGuest);
    
    void outCustomers();
    
    int getSlotAvailable(bool isGuest);
    
	static DyCustomerManager* sharedCustomerManager();
};





#endif  // __DYCUSTOMER_H__