/*
DyStage.h
Doggy chef
@initialize at 121008

@history
121008	yoonsr	initialize
*/


#ifndef __DYSTAGE_H__
#define __DYSTAGE_H__

#include "DyConfig.h"

#include "YtObject.h"
#include "DyCharacter.h"

#include "YtLoader.h"

#include "YtCommand.h"
#include "DyData.h"
#include "DyRecipe.h"

class DyStageManager : public CCObject
{
public:
	YtObjectGroup* m_shopinfo;
	YtObjectGroup* m_curShopInfo;

	DyStageType m_curShopType;
	int m_curLevel;
	int m_cntLevel;
    int m_cntBonusLevel;

	int m_cntShop;

	DyStageManager();
	~DyStageManager();
    
    void setupShopUI(CCLayer* layer);
    
    DyStageType setShop(DyStageType shopTyp);
	void setLevel(int level);
    
	void calculateTotalPoint();
	void nextStageOpen();
	void stageSave();
	
    bool isShopOpen(int shop);
	bool isStageClear(int shop);
	int getCurrentLevel(int shop);
	int getCountLevel(int shop);
	int getCurrentShop();
	std::string getShopName(int shop);

	void buyedShop(int shop);

    bool isLastLevel();
    bool isNextLevelOpen();
    
    bool isCrazyOpenLeastOne();
    
    DyStageState getStageState(int shop, int stage);
    void setStageState(int shop, int stage, DyStageState state);
    
    void getCurrentStage(DyDataPublic* data, int* input_shop, int* input_stage);
    void setCurrentStage(DyDataPublic* data, int input_shop, int input_stage);
    
	static DyStageManager* sharedStageManager();
};



#endif  // __DYSTAGE_H__