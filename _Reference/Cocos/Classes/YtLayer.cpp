/*
YtLayer.cpp
YongTrim Lib
@initialize at 120907

@history
120907	yoonsr	initialize
테스트
*/


#include "YtLayer.h"

#define ZORDER_POPUP		5000
#define ZORDER_TOAST		10000
YtLayerManager* g_sharedLayerManager;

void YtLayer::visit()
{
	if(m_isClip == false && m_isClipOut == false)
	{
		CCLayerColor::visit();
		return;
	}

	if (!isVisible())
    {
		return;
    }

	kmGLPushMatrix();

    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->beforeDraw();
        this->transformAncestors();
    }

	this->transform();

	glEnable(GL_SCISSOR_TEST);

	CCSize size = m_rectClip.size;	
	float s = this->getScale()*YtLib::sharedLib()->m_scale;	

    CCPoint originPos = m_rectClip.origin;
    if(cocos2d::CCEGLView::sharedOpenGLView()->isRetinaEnabled())
    {
        CCSize winSize = CCDirector::sharedDirector()->getWinSizeInPixels();
        originPos.x += (winSize.width/4)/YtLib::sharedLib()->m_scale;
        originPos.y += (winSize.height/4)/YtLib::sharedLib()->m_scale;
    }

    
	CCPoint screenPos;
	if(m_isClip)
		screenPos = convertToWorldSpace(originPos);
	if(m_isClipOut)
	{
		CCPoint thisPoint = m_clipBoard->convertToWorldSpace(ccp(0,0));
		screenPos = ccp(originPos.x*s + thisPoint.x, originPos.y*s + thisPoint.y);
	}

	glScissor(screenPos.x, screenPos.y, size.width*s, size.height*s);

	if(m_pChildren)
    {
		ccArray *arrayData = m_pChildren->data;
		unsigned int i=0;
		
		// draw children zOrder < 0
		for( ; i < arrayData->num; i++ )
        {
			CCNode *child =  (CCNode*)arrayData->arr[i];
			if ( child->getZOrder() < 0 )
            {
				child->visit();
			}
            else
            {
				break;
            }
		}
		
		// this draw
		this->draw();
		
		// draw children zOrder >= 0
		for( ; i < arrayData->num; i++ )
        {
			CCNode* child = (CCNode*)arrayData->arr[i];
			child->visit();
		}
        
	}
    else
    {
		this->draw();
    }

	glDisable(GL_SCISSOR_TEST);
   // this->afterDraw();
	if ( m_pGrid && m_pGrid->isActive())
    {
		m_pGrid->afterDraw(this);
    }

	kmGLPopMatrix();
}


YtLayer::YtLayer()
{
	m_isInit = false;
	m_isTop = false;
	m_isClip = false;
	m_isClipOut = false;
}


YtLayer::~YtLayer()
{
}


void YtLayer::destory()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void YtLayer::setClip(CCRect rect)
{
	m_isClip = true;
	m_rectClip = rect;
}


void YtLayer::setClipOut(CCRect rect, CCNode* clipBoard)
{
	m_isClipOut = true;
	m_rectClip = rect;
	m_clipBoard = clipBoard;
}


YtLayer* YtLayer::create()
{
    YtLayer *pRet = new YtLayer();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}


YtLayerInfo::YtLayerInfo(YtLayer* layer)
{
	m_layer = layer;
	m_hasDimEffect = false;
}


YtLayerInfo::~YtLayerInfo()
{
}


YtLayerManager::YtLayerManager()
{
	YtLib* lib = YtLib::sharedLib();
	m_arrLayerInfos = new CCArray();
	m_arrToastInfos = new CCArray();

	m_layerDim = CCLayerColor::create(ccc4(0, 0, 0, 140), lib->m_sizeWin.width, lib->m_sizeWin.height);
	m_layerDim->retain();
}


YtLayerManager::~YtLayerManager()
{
	m_arrToastInfos->release();
	m_arrLayerInfos->release();
	m_layerDim->release();
}



void YtLayerManager::popToast(YtLayer* layer)
{
	YtLayerInfo* layerInfoTaget = NULL;
	for(unsigned int i = 0;i < m_arrToastInfos->count();i++)
	{
		YtLayerInfo* layerInfo = (YtLayerInfo*)m_arrToastInfos->objectAtIndex(i);

		if(layerInfo->m_layer == layer)
		{
			layerInfoTaget = layerInfo;
			break;
		}
	}
	if(layerInfoTaget)
		layerInfoTaget->m_layer->hide();
}


void YtLayerManager::pushToast(YtLayer* layer)
{
	//YtLib* lib = YtLib::sharedLib();
	
	if(m_arrLayerInfos->count() == 0)
		return;

	YtLayerInfo* layerBaseInfo = (YtLayerInfo*)m_arrLayerInfos->objectAtIndex(0);

	if(m_arrToastInfos->count() == 0)
	{
		layerBaseInfo->m_layer->addChild(layer, ZORDER_TOAST);
	}
	else
	{
		YtLayerInfo* layerTopInfo = (YtLayerInfo*)m_arrToastInfos->lastObject();
		layerBaseInfo->m_layer->addChild(layer,  1 + layerTopInfo->m_layer->getZOrder());
	}	

	layer->show();

	YtLayerInfo* layerInfo = new YtLayerInfo(layer);
	m_arrToastInfos->addObject(layerInfo);
	layerInfo->release();	
}



void YtLayerManager::destoryToast(YtLayer* layer)
{
	//YtLayerInfo* layerBaseInfo = (YtLayerInfo*)m_arrLayerInfos->objectAtIndex(0);

	YtLayerInfo* layerInfoTaget = NULL;
	for(unsigned int i = 0;i < m_arrToastInfos->count();i++)
	{
		YtLayerInfo* layerInfo = (YtLayerInfo*)m_arrToastInfos->objectAtIndex(i);

		if(layerInfo->m_layer == layer)
		{
			layerInfoTaget = layerInfo;
			break;
		}
	}
	
	if(layerInfoTaget)
	{
		layerInfoTaget->m_layer->removeFromParentAndCleanup(true);
		m_arrToastInfos->removeObject(layerInfoTaget, true);
	}
}



void YtLayerManager::pushLayer(YtLayer* layer, bool hasDimEffect, bool needPause)
{
	YtLib* lib = YtLib::sharedLib();
	
	if(m_arrLayerInfos->count() > 0)
	{
		YtLayerInfo* layerBaseInfo = (YtLayerInfo*)m_arrLayerInfos->objectAtIndex(0);

		YtLayerInfo* layerTopInfo = (YtLayerInfo*)m_arrLayerInfos->lastObject();
        if(needPause)
            layerTopInfo->m_layer->pause();
		layerTopInfo->m_layer->m_isTop = false;
        layerTopInfo->m_storeTouch = layerTopInfo->m_layer->isTouchEnabled();
        layerTopInfo->m_layer->setTouchEnabled(false);
        
		if(hasDimEffect)
		{
			m_layerDim->setPosition(ccp(0, 0));
			m_layerDim->setVisible(true);
		}
		
		layer->setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height/2));

		if(m_arrLayerInfos->count() == 1)
		{
			if(hasDimEffect)
			{
				layerBaseInfo->m_layer->reorderChild(m_layerDim, ZORDER_POPUP);
			}
			layerBaseInfo->m_layer->addChild(layer, ZORDER_POPUP+1);
			
		}
		else
		{
			if(hasDimEffect)
				layerBaseInfo->m_layer->reorderChild(m_layerDim, m_arrLayerInfos->count() + layerTopInfo->m_layer->getZOrder());
			layerBaseInfo->m_layer->addChild(layer,  m_arrLayerInfos->count() + 1 + layerTopInfo->m_layer->getZOrder());
		}	

		layer->show();
	}
	else
	{
		if(m_layerDim->getParent())
			m_layerDim->removeFromParentAndCleanup(false);
		layer->addChild(m_layerDim);
		m_layerDim->setPosition(ccp(0, 0));
		m_layerDim->setVisible(false);
	}

	YtLayerInfo* layerInfo = new YtLayerInfo(layer);
	layerInfo->m_hasDimEffect = hasDimEffect;
	m_arrLayerInfos->addObject(layerInfo);
	layerInfo->release();	
}

void YtLayerManager::destoryTopLayer(bool needResume)
{
	YtLayerInfo* layerBaseInfo = (YtLayerInfo*)m_arrLayerInfos->objectAtIndex(0);
	YtLayerInfo* layerTopInfo = (YtLayerInfo*)m_arrLayerInfos->lastObject();

	layerTopInfo->m_layer->removeFromParentAndCleanup(true);

	//YtLayerManager::destory(layerInfo->m_layer);
	m_arrLayerInfos->removeLastObject();

	layerTopInfo = (YtLayerInfo*)m_arrLayerInfos->lastObject();
    if(needResume)
       layerTopInfo->m_layer->resume();
    layerTopInfo->m_layer->setTouchEnabled(layerTopInfo->m_storeTouch);
    
    
	layerTopInfo->m_layer->m_isTop = true;
	if(layerTopInfo->m_hasDimEffect)
	{
		layerBaseInfo->m_layer->reorderChild(m_layerDim, layerTopInfo->m_layer->getZOrder()-1);	
		m_layerDim->setVisible(true);
	}
	else
		m_layerDim->setVisible(false);
}


void YtLayerManager::popLayer()
{
	//YtLayerInfo* layerBaseInfo = (YtLayerInfo*)m_arrLayerInfos->objectAtIndex(0);
	YtLayerInfo* layerTopInfo = (YtLayerInfo*)m_arrLayerInfos->lastObject();
	
	//CCCallFunc* callfun = CCCallFunc::create(this, callfunc_selector(YtLayerManager::destoryLayer));

	layerTopInfo->m_layer->hide();
}


YtLayer* YtLayerManager::getTopLayer()
{
	YtLayerInfo* layerTopInfo = (YtLayerInfo*)m_arrLayerInfos->lastObject();
	return layerTopInfo->m_layer;
}


YtLayer* YtLayerManager::getBaseLayer()
{
    YtLayerInfo* layerTopInfo = (YtLayerInfo*)m_arrLayerInfos->objectAtIndex(0);
	return layerTopInfo->m_layer;

}


YtLayer* YtLayerManager::getLayerByTag(std::string tag)
{
    for(int i = m_arrLayerInfos->count()-1;i >= 0;i--)
    {
        YtLayerInfo* layerTopInfo = (YtLayerInfo*)m_arrLayerInfos->objectAtIndex(i);
        if(layerTopInfo->m_layer->m_tag.compare(tag) == 0)
        {
            return layerTopInfo->m_layer;
        }
    }
    return NULL;
}


YtLayer* YtLayerManager::getLayerByFromTop(int index)
{
   	YtLayerInfo* layerTopInfo = (YtLayerInfo*)m_arrLayerInfos->objectAtIndex(index);
	return layerTopInfo->m_layer;
}


int YtLayerManager::getLayerCount()
{
   	return m_arrLayerInfos->count();
}




void YtLayerManager::clearLayers()
{
	m_arrLayerInfos->removeAllObjects();
	m_arrToastInfos->removeAllObjects();
}


YtLayerManager* YtLayerManager::sharedLayerManager()
{
	if(g_sharedLayerManager == NULL)
	{
		g_sharedLayerManager = new YtLayerManager();
	}
	return g_sharedLayerManager;
}


YtLayer* YtLayerManager::create(const char* pClassName, CCNodeLoader* pCCNodeLoader, const char* pCCBFileName, CCObject* owner)
{
    CCNodeLoaderLibrary * ccNodeLoaderLibrary = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();
    ccNodeLoaderLibrary->registerCCNodeLoader(pClassName, pCCNodeLoader);
    cocos2d::extension::CCBReader * ccbReader = new cocos2d::extension::CCBReader(ccNodeLoaderLibrary);

	CCBAnimationManager* actionManager = NULL;
    YtLayer *node = (YtLayer*)ccbReader->readNodeGraphFromFile(pCCBFileName, owner, &actionManager);
    ccbReader->release();

	node->m_actionManager = actionManager;
	//node->m_actionManager->retain();

    node->didLayerCreated();
    
	return node;
}


void YtLayerManager::destory(YtLayer* layer)
{
	layer->m_actionManager->release();
}
