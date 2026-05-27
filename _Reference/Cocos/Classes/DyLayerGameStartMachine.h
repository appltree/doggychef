/*
DyLayerGameStartMachine.h
Doggy chef
@initialize at 131205

@history
131205	yoonsr	initialize
*/


#ifndef __DYLAYERGAMESTARTMACHINE_H__
#define __DYLAYERGAMESTARTMACHINE_H__

#define GAMESTARTMACHINE_MENU               1000
#define GAMESTARTMACHINE_MENUITEM_UPGRADE   1001

#define GAMESTARTMACHINE_MACHINENAME        2000
#define GAMESTARTMACHINE_MACHINEIMAGE       3000

#define GAMESTARTMACHINE_PRICE              4000
#define GAMESTARTMACHINE_GOLDMARK           4001

#define GAMESTARTMACHINE_GRADE_0            5000
#define GAMESTARTMACHINE_GRADE_1            5001
#define GAMESTARTMACHINE_GRADE_2            5002
#define GAMESTARTMACHINE_GRADE_3            5003


#define DYLAYERGAMESTARTMACHINE_TAG                     "DyLayerGameStartMachine"


#include "YtScene.h"
#include "YtLayer.h"

class DyLayerGameStartMachine
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerGameStartMachine, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

    CCMenu* m_menu;
    
	DyLayerGameStartMachine();
	virtual ~DyLayerGameStartMachine();

    void setupUI();
    
	void onCloseClicked(cocos2d::CCObject * pSender);
    void onUpgradeClicked(cocos2d::CCObject * pSender);
    
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    virtual void keyBackClicked();
    virtual void onEnter();
    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);

    
    static DyLayerGameStartMachine* create(CCNode* owner);
};


class DyLayerGameStartMachineLoader : public CCLayerLoader
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerGameStartMachineLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerGameStartMachine);
};

#endif  // __DYLAYERGAMESTARTMACHINE_H__