/*
YtLayerLog.cpp
YongTrim Lib
@initialize at 121002

@history
121002	yoonsr	initialize
*/


#include "YtLayerLog.h"

YtLayerLog::YtLayerLog(CCSize size, const char* strMessage)
//{)
{
	m_size = size;
	m_sizeContent = CCSizeMake(size.width, m_size.height);

	initWithColor(ccc4(0, 0, 0, 0), m_size.width, m_size.height);
	setTouchEnabled(true);

    
	m_content = CCLabelTTF::create(strMessage, "godom.ttf", 20, CCSizeMake(m_size.width, 0), kCCTextAlignmentLeft, kCCVerticalTextAlignmentTop);

    
    CCLayerColor* layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), m_content->getContentSize().width, m_content->getContentSize().height);
    
    m_content->setAnchorPoint(ccp(0,0));
    m_content->setPosition(ccp(0,0));
    layerContent->addChild(m_content);
    
	m_scrollView = CCScrollView::create(m_size, layerContent);
    
    //m_tabScrollView[i]->setDelegate(this);
    
    m_scrollView->setContentSize(layerContent->getContentSize());
    
    
    m_scrollView->setDirection(kCCScrollViewDirectionVertical);
    m_scrollView->setContentOffset(m_scrollView->minContainerOffset());
    //m_scrollView->setContentOffset(ccp(0,0));
    m_scrollView->setGlobalScale(YtLib::sharedLib()->m_scale);
   
	this->addChild(m_scrollView);
}


YtLayerLog::~YtLayerLog()
{
}

//void YtLayerLog::addLog(const char* strMessage)
//{
//	m_content->setString(strMessage);
//}
//

void YtLayerLog::pause()
{
}


void YtLayerLog::resume()
{
}


void YtLayerLog::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void YtLayerLog::show()
{
}
