/*
DyLayerStageRank.h
Doggy chef
@initialize at 130707

@history
130707	yoonsr	initialize
*/


#ifndef __DYLAYERSTAGERANK_H__
#define __DYLAYERSTAGERANK_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "DyLayerRank_list.h"


#define STAGERANK_UI_LAYER_RACE             2000
#define STAGERANK_UI_TAG_STAGE              1100
#define STAGERANK_UI_TAG_STAGEVALUE			1101



class DyLayerStageRank
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerStageRank, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	DyLayerStageRank();
	virtual ~DyLayerStageRank();

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    virtual void keyBackClicked();

    
    virtual void onEnter();

    
    CCMenu* m_menu;
    DyLayerRank_list* m_list;
    
    int m_stage;
    CCArray* m_arrRank;
    
    void refresh();
    
    
    void onOkClicked(CCObject *obj);

	static DyLayerStageRank* create(CCNode* owner, int stage, CCArray* arrRank);
};


class DyLayerStageRankLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerStageRankLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerStageRank);
};

#endif  // __DYLAYERSTAGERANK_H__