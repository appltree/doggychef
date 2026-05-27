//
//  CCTabView.h
//  testxdoceione
//
//  Created by Michael YUE on 12/03/12.
//  Copyright (c) 2012 private. All rights reserved.
//

#ifndef _CCTABVIEW_H_
#define _CCTABVIEW_H_


#include "YtLib.h"

USING_NS_CC;

using namespace cocos2d::extension;

class CCTabView;

class CCTabViewDelegate 
{
public:
	virtual void tabDidChange(CCTabView* tabView, int index, CCLayer* contentLayer, bool init, bool redo){}
};


class  CCTabView : public CCLayer
{
public:
    CCTabView(CCRect rect = CCRectZero);
    virtual ~CCTabView();
    
    //void addTab(const char * tabName, CCLayer * tabView);
	void addTab(CCSprite* sprNormal, CCSprite* sprSelect, CCSprite* sprActive, CCLayer* tabView);
	void setContentBg(CCLayer* layer);
    virtual void onEnter();
	virtual void onExit();
    
    // handle touch events
    virtual void registerWithTouchDispatcher();
	virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
	virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
	virtual void ccTouchCancelled(CCTouch *touch, CCEvent* event);
	virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
    
    // tab clicked events
    void tabClicked(CCObject * obj);
    void goTab(int index, bool byTabButton);
    // properties
    // middle padding position between tabs
    float m_midPad;

    // left margin for first tab
    float m_leftMargin;
	float m_upMargin;

    // tab header image size
	float m_thick;

	float m_contentOffsetX;
	float m_contentOffsetY;

	CCTabViewDelegate* m_delegate;
    // tab edge frame size
	//float m_leftInsect;
    //float m_topInsect;
    // tab title font size and color
    //CC_SYNTHESIZE(float,_titleFontSize,TitleFontSize);
    //CC_SYNTHESIZE(std::string,_titleFontName,TitleFontName);
    //CC_SYNTHESIZE(ccColor3B,_titleFontColor,TitleFontColor);
    
    // default selected tab
    //CC_SYNTHESIZE(int,_selectedTab,SelectedTab);
    
    // same tab title and related tab sub view
    //CC_SYNTHESIZE(CCMutableArray<CCString *> *,_tabNameArray,TabNameArray);
    //CC_SYNTHESIZE(CCMutableArray<CCLayer *> *,_tabViewArray,TabViewArray);
	//CC_SYNTHESIZE(CCArray *,_tabNameArray,TabNameArray);
    //CC_SYNTHESIZE(CCArray *,_tabViewArray,TabViewArray);
    
	int m_selectedTab;

	CCArray* m_tabMenuArray;
	CCArray* m_tabViewArray;
	CCArray* m_tabSprNormalArray;
	CCArray* m_tabSprSelectArray;
	CCArray* m_tabSprActiveArray;

	bool m_init;

	void setEnable(bool value);

private:
    void initTabData();
    //CCSize _edgeSize;
    
    CCMenuItemImage * m_preSelectedTab;
    CCLayerColor * m_tabView;
	CCLayer* m_contentBgLayer;
};

#endif