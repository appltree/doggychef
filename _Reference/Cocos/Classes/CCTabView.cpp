//
//  CCTabView.m
//  testxdoceione
//
//  Created by Michael YUE on 12/03/12.
//  Copyright (c) 2012 private. All rights reserved.
//
//http://www.cocos2d-x.org/boards/18/topics/9283
//
//CCTabView * myTab = new CCTabView(CCRectMake(50, 20, 380, 300))
//myTab->autorelease();
//CCLayerColor * mylay = CCLayerColor::layerWithColorWidthHeight(ccc4(255, 0, 0,120),200,100);  
//CCLabelTTF * lbl = CCLabelTTF::labelWithString("tab1","Thonburi",18); 
//lbl->setPosition(ccp(50,50));  
//mylay->addChild(lbl);
//
//CCLayerColor * mylay1 = CCLayerColor::layerWithColorWidthHeight(ccc4(0, 255, 0,120),200,100);
//CCLabelTTF * lbl1 = CCLabelTTF::labelWithString("tab2","Thonburi",18);
//lbl1->setPosition(ccp(50,50));  
//mylay1->addChild(lbl1);
//
//CCLayerColor * mylay2 = CCLayerColor::layerWithColorWidthHeight(ccc4(0,50, 220,120),200,100);  
//CCLabelTTF * lbl2 = CCLabelTTF::labelWithString("tab3","Thonburi",18);  
//lbl2->setPosition(ccp(50,50));  
//mylay2->addChild(lbl2);
//
//
//myTab->addTab("tab1", mylay);  
//myTab->addTab("tab2", mylay1);  
//myTab->addTab("tab3", mylay2);   
//this->addChild(myTab);


#include "CCTabView.h"
#include <math.h>
using namespace std;

CCTabView::CCTabView(CCRect rect)
{
	m_init = false;
	m_contentOffsetX = 0;
	m_contentOffsetY = 0;

    this->setContentSize(rect.size);
    this->setPosition(rect.origin);
    
    m_preSelectedTab = NULL;
    m_delegate = NULL;
	m_tabMenuArray = new CCArray();
    m_tabViewArray = new CCArray();
	m_tabSprNormalArray = new CCArray();
	m_tabSprActiveArray = new CCArray();
    m_tabSprSelectArray = new CCArray();
    // pre-load tab images
    //CCTextureCache::sharedTextureCache()->addImage(TAB_NORMAL);
    //CCTextureCache::sharedTextureCache()->addImage(TAB_LINE);
    //CCTextureCache::sharedTextureCache()->addImage(TAB_ACTIVE);
    //CCTexture2D * edgeTexture = CCTextureCache::sharedTextureCache()->addImage(TAB_EDGE);
    //_edgeSize = edgeTexture->getContentSize();
    
    // create sub view container
    m_tabView = CCLayerColor::create(ccc4(255, 255, 255, 0), rect.size.width, rect.size.height);
	m_tabView->setPosition(CCPointZero);
    this->addChild(m_tabView, 100);
	
}

CCTabView::~CCTabView()
{
	m_tabMenuArray->release();
	m_tabViewArray->release();
	m_tabSprNormalArray->release();
	m_tabSprActiveArray->release();
	m_tabSprSelectArray->release();
}


void CCTabView::setContentBg(CCLayer* layer)
{
	m_contentBgLayer = layer;
	this->addChild(m_contentBgLayer, 6);
}


void CCTabView::addTab(CCSprite* sprNormal, CCSprite* sprSelect, CCSprite* sprActive, CCLayer* tabView)
{
	m_tabSprNormalArray->addObject(sprNormal);
	m_tabSprSelectArray->addObject(sprSelect);
	m_tabSprActiveArray->addObject(sprActive);
	m_tabViewArray->addObject(tabView);
	m_tabView->addChild(tabView); //for memory leak scroll view
}


void CCTabView::goTab(int index, bool byTabButton)
{
	CCMenu *_pMenu = (CCMenu *)this->getChildByTag(index);
	CCMenuItemImage * pMenu = (CCMenuItemImage *)_pMenu->getChildByTag(index);

	CCSprite* sprHighlight = (CCSprite*)m_tabSprActiveArray->objectAtIndex(index);

	pMenu->setSelectedSpriteFrame(sprHighlight->displayFrame());
	pMenu->setNormalSpriteFrame(sprHighlight->displayFrame());
    pMenu->selected(); 

    CCLayer * subView;
    
    bool redo = pMenu == m_preSelectedTab;
    
    if(pMenu != m_preSelectedTab)
    {
        // unselected previous one0
        if( m_preSelectedTab != NULL)
        {
			int preTabIndex = m_preSelectedTab->getTag();
			CCSprite* sprSelect = (CCSprite*)m_tabSprSelectArray->objectAtIndex(preTabIndex);
			CCSprite* sprNormal = (CCSprite*)m_tabSprNormalArray->objectAtIndex(preTabIndex);
			m_preSelectedTab->setSelectedSpriteFrame(sprSelect->displayFrame());
			m_preSelectedTab->setNormalSpriteFrame(sprNormal->displayFrame());
            m_preSelectedTab->unselected();
        }
        m_preSelectedTab = pMenu;
        // showing related tab view
        // remove previous tab view
        m_tabView->removeAllChildrenWithCleanup(true);
		subView = (CCLayer *)m_tabViewArray->objectAtIndex(index);
		//float yPos = 0;//this->getContentSize().height - m_tabSize.height - subView->getContentSize().height;
		//CCLog("subView->getContentSize().height = %f", subView->getContentSize().height);
        subView->setPosition(ccp(m_contentOffsetX,m_contentOffsetY));
        m_tabView->addChild(subView);

    }
    
    
    if(m_delegate)
        m_delegate->tabDidChange(this, index, subView, !byTabButton, redo);
}
// tab clicked events
void CCTabView::tabClicked(CCObject * obj)
{
    CCMenuItemImage * pMenu = dynamic_cast<CCMenuItemImage*>(obj);
	int tabIndex = pMenu->getTag();
	goTab(tabIndex, true);
}


void CCTabView::initTabData()
{
	if(m_init)
		return;
	m_init = true;

    int tabSprCount = m_tabSprNormalArray->count();
    int tabViewCount = m_tabViewArray->count();
    if ( tabSprCount <= 0 || tabViewCount <= 0 ) return;
    
    int tabCount = min(tabSprCount,tabViewCount);
   

	CCSprite* sprHighlight = (CCSprite*)m_tabSprActiveArray->objectAtIndex(0);
	CCSize tabSize = sprHighlight->getContentSize();

    CCPoint tabStartPos = ccp(m_leftMargin, this->getContentSize().height-tabSize.height/2 + m_upMargin);

    CCPoint lastPos = tabStartPos;
    for ( int i = 0 ; i < tabCount ; i++ )
    {
		CCSprite* sprNormal = (CCSprite*)m_tabSprNormalArray->objectAtIndex(i);
		CCSprite* sprHighlight = (CCSprite*)m_tabSprActiveArray->objectAtIndex(i);
		CCSprite* sprSelect = (CCSprite*)m_tabSprSelectArray->objectAtIndex(i);

		tabSize = sprHighlight->getContentSize();

        // set tab background
        lastPos = ccpAdd(lastPos, ccp((tabSize.width/2),0));
        CCMenuItemImage * oneTab = CCMenuItemImage::create();
		
		oneTab->initWithNormalSprite(sprNormal, sprSelect, sprNormal, this, menu_selector(CCTabView::tabClicked));
        oneTab->setPosition(lastPos);
        oneTab->setTag(i);
        
		lastPos = ccpAdd(lastPos, ccp((tabSize.width/2+m_midPad),0));

        if(i == m_selectedTab) 
        {
			oneTab->setSelectedSpriteFrame(sprHighlight->displayFrame());
			oneTab->setNormalSpriteFrame(sprHighlight->displayFrame());

            oneTab->selected();
			
            m_preSelectedTab = oneTab;
            
            m_tabView->removeAllChildrenWithCleanup(true);
			CCLayer * subView = (CCLayer *)m_tabViewArray->objectAtIndex(i);			
            subView->setPosition(ccp(m_contentOffsetX,m_contentOffsetY));
            m_tabView->addChild(subView);

			if(m_delegate)
				m_delegate->tabDidChange(this, i, subView, true, false);

        }
        
        CCMenu * pMenu = CCMenu::create(oneTab, NULL);
        pMenu->setPosition(CCPointZero);
        pMenu->setTag(i);
        this->addChild(pMenu, 5);
		m_tabMenuArray->addObject(pMenu);
        // set tab title
		//CCString * titleName = (CCString*)_tabNameArray->objectAtIndex(i);
		//CCLabelTTF * ttl = CCLabelTTF::create(titleName->getCString(),_titleFontName.c_str(),_titleFontSize);
  //      ttl->setColor(_titleFontColor);
  //      ttl->setPosition(lastPos);
  //      this->addChild(ttl);
    }
    
    // draw edges
    // left top 
    //CCTexture2D * angleTexture = CCTextureCache::sharedTextureCache()->textureForKey(TAB_EDGE);
    //CCSprite * leftTopSprite = CCSprite::createWithTexture(angleTexture, CCRectMake(0,0,9,7));
    //float yPos = this->getContentSize().height - _tabSize.height;
    //leftTopSprite->setPosition(ccp(0.0,yPos));
    //this->addChild(leftTopSprite);
    
    // between left to and first tab
    //CCTexture2D * line = CCTextureCache::sharedTextureCache()->textureForKey(TAB_LINE);
    //int lineCount = _leftMargin;
    //for ( int i = 0 ; i < lineCount ; i++ )
    //{
    //    CCSprite * oneline = CCSprite::createWithTexture(line);
    //    oneline->setPosition(ccp(4 + i, leftTopSprite->getPosition().y + 21.0));
    //    this->addChild(oneline);
    //}
    
    // last tab up to right top
    //lineCount = this->getContentSize().width - (lastPos.x+_tabSize.width/2);
    //CCPoint startPos = ccp(lastPos.x+_tabSize.width/2,leftTopSprite->getPosition().y + 21.0);
    //int j = 0;
    //for ( j = 0 ; j < lineCount ; j++ )
    //{
    //    CCSprite * oneline = CCSprite::createWithTexture(line);
    //    oneline->setPosition(ccpAdd(startPos, ccp(j,0)));
    //    this->addChild(oneline);
    //}
    
    // right top 
    //CCSprite * rightTopSprite = CCSprite::createWithTexture(angleTexture, CCRectMake(10,0,9,7));
    //rightTopSprite->setPosition(ccp(startPos.x+j,yPos));
    //this->addChild(rightTopSprite);
    
    // from right top to right bottom
    //lineCount = this->getContentSize().height - _tabSize.height - 9;
    //startPos = ccp(startPos.x+j,yPos-3);
    //for ( j = 0 ; j < lineCount ; j++ )
    //{
    //    CCSprite * oneline = CCSprite::createWithTexture(angleTexture, CCRectMake(10,7,9,1));
    //    oneline->setPosition(ccpAdd(startPos, ccp(0,-j)));
    //    this->addChild(oneline);
    //}
    //
    //// bottom right 
    //CCSprite * rightBottomSprite = CCSprite::createWithTexture(angleTexture, CCRectMake(10,8,9,7));
    //rightBottomSprite->setPosition(ccp(startPos.x,startPos.y-j));
    //this->addChild(rightBottomSprite);
    //
    //// from right bottom to left bottom
    //lineCount = this->getContentSize().width-9;
    //startPos = ccp(startPos.x-5,startPos.y-j-1);
    //for ( j = 0 ; j < lineCount ; j++ )
    //{
    //    CCSprite * oneline = CCSprite::createWithTexture(angleTexture, CCRectMake(10,8,1,9));
    //    oneline->setPosition(ccpAdd(startPos, ccp(-j,0)));
    //    this->addChild(oneline);
    //}
    //
    //// left bottom 
    //CCSprite * leftBottomSprite = CCSprite::createWithTexture(angleTexture, CCRectMake(0,8,9,7));
    //leftBottomSprite->setPosition(ccp(0,rightBottomSprite->getPosition().y));
    //this->addChild(leftBottomSprite);
    //
    //// from left bottom to left top
    //lineCount = this->getContentSize().height - _tabSize.height-9;
    //startPos = ccpAdd(ccp(0,0),ccp(0,leftBottomSprite->getPosition().y));
    //for ( j = 0 ; j < lineCount ; j++ )
    //{
    //    CCSprite * oneline = CCSprite::createWithTexture(angleTexture, CCRectMake(0,7,9,1));
    //    oneline->setPosition(ccpAdd(startPos, ccp(0,j)));
    //    this->addChild(oneline);
    //}
}

void CCTabView::onEnter()
{
    // to do something here
    // .....
    this->initTabData();
    
    CCLayer::onEnter();
}

void CCTabView::onExit()
{
    // to do something here
    // ......
    //CCTextureCache::sharedTextureCache()->removeTextureForKey(TAB_NORMAL);
    //CCTextureCache::sharedTextureCache()->removeTextureForKey(TAB_ACTIVE);
    //CCTextureCache::sharedTextureCache()->removeTextureForKey(TAB_LINE);
    //CCTextureCache::sharedTextureCache()->removeTextureForKey(TAB_EDGE);
    
    CCLayer::onExit();
}


/////Menu - Events
void CCTabView::registerWithTouchDispatcher()
{
	CCTouchDispatcher* pDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();

	pDispatcher->addTargetedDelegate(this, kCCMenuHandlerPriority-1, false);
}

bool CCTabView::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
	//--CCPoint touchLocation = touch->locationInView(touch->view());
	return true;
}

void CCTabView::ccTouchEnded(CCTouch *touch, CCEvent* event)
{
	
}

void CCTabView::ccTouchCancelled(CCTouch *touch, CCEvent* event)
{
	
}

void CCTabView::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
	
}

void CCTabView::setEnable(bool value)
{
	for(unsigned int i = 0;i < m_tabMenuArray->count();i++)
	{
		CCMenu* menu = (CCMenu*)m_tabMenuArray->objectAtIndex(i);
		menu->setEnabled(value);
	}
}