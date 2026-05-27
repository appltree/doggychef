/*
DyPopup.h
Doggy chef
@initialize at 120907

@history
120907	yoonsr	initialize
*/


#ifndef __DYPOPUP_H__
#define __DYPOPUP_H__

#include "DyLib.h"

#include "YtLayerLog.h"
#include "YtAniObject.h"
#include "YtSpriteEx.h"


typedef enum tag_DyPopupType
{
	DyPopupType_Text,
	DyPopupType_LoadingCycle,
 	DyPopupType_NewCustomer,
	DyPopupType_NewRecipe,
    DyPopupType_Present,
    DyPopupType_Costume,
    DyPopupType_Costume2,
    DyPopupType_Quest,
    DyPopupType_Quest2,
    DyPopupType_GumWarning,
    DyPopuptype_RandomBox,
}DyPopupType;



class DyPopupButton : public CCObject
{
public:
	std::string m_strName;
	DyButtonType m_buttontype;
	int m_tag;

	CCMenuItemImage* m_menuItem;
};


class DyPopupCostume
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:
	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyPopupCostume, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);
    
    DyCostumeStatus* m_costumeStatus;
    DyCostumeFunction* m_costumeFunction;
    
	DyPopupCostume();
	virtual ~DyPopupCostume();
    
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    
    
	static DyPopupCostume* create(CCNode* owner, DyCostumeStatus* costumeStatus, DyCostumeFunction* costumeFunction);
};


class DyPopupCostumeLoader : public CCLayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyPopupCostumeLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyPopupCostume);
};



class DyPopup : public YtLayer
{
private:
	//for loading cycle
	CCRenderTexture *m_rt;

	CCSprite* m_sprCircleWhite;
	CCSprite* m_sprCircleBlown;

	CCSprite* m_sprCircleWhiteRT;
	CCSprite* m_sprCircleBlownRT;
	CCSprite* m_sprCircleMaskRT;

	CCScale9Sprite* m_spr9TextBox;
	DyPopupType m_popuptype;
	YtAniObject* m_character;
	float m_dtCircle;
	bool m_isBlownDown;
	void cbCirclePopup(float dt);
    
    
    CCSprite* m_sprIndicator;
    
    CCSprite* m_sprFoot[PRESENT_CHECK_DAY];
    
    
    void endAnimationRewardFoot0(CCNode* node);
    void endAnimationRewardFoot1(CCNode* node);
public:
    CCObject*			m_pListener;
    SEL_CallFuncO		m_pfnSelector;

	CCArray* m_arrButtons;
	CCLabelTTF* m_content;
	CCSize m_size;
	DyPopup();
	~DyPopup();
    
    bool m_resumeAndDestory;
    
	void setCustomer(std::string tag);
	void setRecipe(std::string tag, bool alreadHas);
    void setDay(int day);
    
    void setCostumeInfo(DyCostumeStatus* costumeStatus, DyCostumeFunction* costumeFunction);
    void setQuestInfo(std::string text, std::string icon);
    void setQuestInfo2(std::string text, std::string text2, std::string icon);
    
    void setPicture(CCSprite* sprite, CCPoint pos);
    
	void setText(const char* text);
	DyPopupButton* addButton(DyButtonType type, const char* name, int tag);
	void arrangeButton();
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
	virtual void destory();

	virtual void onEnter();
	virtual void update(float dt);

    
    static DyPopup* create(DyPopupType type, CCObject *rec, SEL_CallFuncO selector, CCSize size);
	static DyPopup* create(DyPopupType type, CCObject *rec, SEL_CallFuncO selector);
	static DyPopup* create(DyPopupType type);

	int m_active_tag;

	void onButtonClicked(CCObject *obj);
	CCMenu *m_menu;
    
    int m_curday;
    
    virtual void keyBackClicked();

};



#endif  // __DYPOPUP_H__