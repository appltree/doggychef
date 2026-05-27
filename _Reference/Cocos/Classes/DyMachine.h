/*
DyMachine.h
Doggy chef
@initialize at 131205

@history
131205	yoonsr	initialize
*/


#ifndef __DYMACHINE_H__
#define __DYMACHINE_H__

#include "DyConfig.h"
#include "YtObject.h"
#include "DyStage.h"

#define MC_MACHINE_MAX_LEVEL        3

class DyMachineManager : public CCObject
{
private:
	CCSprite* m_sprMachineGlow;
    CCSprite* m_sprMachineButton[3];
	CCSprite* m_sprMachine;
    
    int m_curMachineLevel;
    DyStageType m_curShop;
    
    CCLayer* m_layer;
    
    int m_grade;
    
public:
	DyMachineManager();
	~DyMachineManager();
    
    void setupMachineUI(CCLayer* layer, DyStageType shop);
    void setMachineReady(bool ready, CCLayer* layer, CCMenuItemImage* itemImage);
    
    void setMachineImage(CCSprite* sprMachine, bool isInUpgradeView);
    
    void sideReadyUI();
    
    void machineSelect(bool select);
    
    void rampUI(int grade);
    
    bool isSelectMachine(CCPoint position);
    
    int getSidePrice();
    int getMachineLevel();
    
    std::string getCurrentMachineName();
    int getNextUpgradePrice();
    bool canUpgrade();
    void upgradeMachine();
    int getMachineLevel(DyStageType shop);
    
    CCSprite* createSide();
    CCSprite* createSideGlow();
    
    float getFullTime();
    
    
    
	static DyMachineManager* sharedMachineManager();
};



#endif  // __DYMACHINE_H__