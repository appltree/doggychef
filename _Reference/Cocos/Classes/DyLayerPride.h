/*
DyLayerPride.h
Doggy chef
@initialize at 130831

@history
130831	yoonsr	initialize
*/


#ifndef __DYLAYERPride_H__
#define __DYLAYERPride_H__

#include "YtScene.h"
#include "YtLayer.h"

#include "DyLayerRank_list.h"


#define PRIDE_UI_TAG_MENU          1000

#define PRIDE_UI_TAG_COUNT          2001

class DyLayerPride
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerPride, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	DyLayerPride();
	virtual ~DyLayerPride();

	void onCloseClicked(cocos2d::CCObject * pSender);
	
	CCMenu* m_menu;

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

    virtual void onEnter();
    
    
    DyLayerRank_list* m_list;
    DyRankType m_rankType;
    
    int m_stage;
    CCArray* m_arrRank;
    void refresh();
    
    //std::string m_messagePride;
    int m_pridePoint;
    
    virtual void keyBackClicked();

    
	static DyLayerPride* create(CCNode* owner, DyRankType rankType, int pridePoint);
};


class DyLayerPrideLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerPrideLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerPride);
};

#endif  // __DYLAYERPride_H__