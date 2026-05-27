/*
YtSpriteEx.cpp
YongTrim Lib.
@initialize at 120401

@history
120401	yoonsr	initialize
*/

#include "YtSpriteEx.h"
#include "YtLib.h"
#include "YtNetwork.h"

#include "platform/CCFileUtils.h"

//// XXX HACK: optimization
//#define SET_DIRTY_RECURSIVELY() {									\
//					if (m_bUsesBatchNode && ! m_bRecursiveDirty) {	\
//						m_bDirty = m_bRecursiveDirty = true;				\
//						if ( m_bHasChildren)							\
//							setDirtyRecursively(true);			\
//						}											\
//					}
//
//YtMaskSprite::~YtMaskSprite()
//{
//	m_pMaskSpriteFrame->release();
//}
//
//
//void YtMaskSprite::drawTexture()
//{
//#define kQuadSize sizeof(m_sQuad.bl)
//	long offset = (long)&m_sQuad;
//
//	// vertex
//	int diff = offsetof(ccV3F_C4B_T2F, vertices);
//	glVertexPointer(3, GL_FLOAT, kQuadSize, (void*)(offset + diff));
//
//	// color
//	diff = offsetof( ccV3F_C4B_T2F, colors);
//	glColorPointer(4, GL_UNSIGNED_BYTE, kQuadSize, (void*)(offset + diff));
	//
//	// tex coords
//	diff = offsetof( ccV3F_C4B_T2F, texCoords);
//	glTexCoordPointer(2, GL_FLOAT, kQuadSize, (void*)(offset + diff));
 //
//	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//}
 //
	//
//void YtMaskSprite::draw()
//{
	//CCTexture2D	*maskTexture = m_pMaskSpriteFrame->getTexture();
//
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//glBindTexture(GL_TEXTURE_2D, maskTexture->getName());
	//drawTexture();
//
	//glBlendFunc(GL_DST_COLOR, GL_ONE);
//    glBindTexture(GL_TEXTURE_2D, m_pobTexture->getName());
//    drawTexture();
//
//	glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
//
//}
//
//
//YtMaskSprite* YtMaskSprite::spriteWithSpriteFrame(CCSpriteFrame *pSpriteFrame, CCSpriteFrame *pMaskSpriteFrame)
//{
//	YtMaskSprite *pobSprite = new YtMaskSprite();
//	pobSprite->m_pMaskSpriteFrame = pMaskSpriteFrame;
//	pobSprite->m_pMaskSpriteFrame->retain();
//
//	if (pobSprite && pobSprite->initWithSpriteFrame(pSpriteFrame))
//	{
//		pobSprite->autorelease();
//		return pobSprite;
//	}
//	CC_SAFE_DELETE(pobSprite);
//	return NULL;
//}


YtBlankSprite* YtBlankSprite::spriteWithSizeAndColor(CCSize size, ccColor3B color)
{
	//YTLib* lib = YTLib::sharedYTLib();
	YtBlankSprite *pobSprite = new YtBlankSprite();
	if (pobSprite && pobSprite->initWithFile("blank.png"))
	{
		pobSprite->autorelease();
		
		pobSprite->setTextureRect(CCRectMake(0, 0, size.width, size.height));
		pobSprite->setColor(color);
		return pobSprite;
	}
	CC_SAFE_DELETE(pobSprite);

	return NULL;
}


YtBoxSprite* YtBoxSprite::spriteWithSize(CCSize size, const char *strFile)
{
    const char *pszPath = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(strFile);
    CCDictionary *dict = CCDictionary::createWithContentsOfFileThreadSafe(pszPath);
	
	const char *texturePath = dict->valueForKey("textureFileName")->getCString();
	CCRect frameLeftTop = CCRectFromString(dict->valueForKey("lefttop")->getCString());
	CCRect frameTop = CCRectFromString(dict->valueForKey("top")->getCString());
	CCRect frameRightTop = CCRectFromString(dict->valueForKey("righttop")->getCString());
	CCRect frameLeft = CCRectFromString(dict->valueForKey("left")->getCString());
	CCRect frameCenter = CCRectFromString(dict->valueForKey("center")->getCString());
	CCRect frameRight = CCRectFromString(dict->valueForKey("right")->getCString());
	CCRect frameLeftBottom = CCRectFromString(dict->valueForKey("leftbottom")->getCString());
	CCRect frameBottom = CCRectFromString(dict->valueForKey("bottom")->getCString());
	CCRect frameRightBottom = CCRectFromString(dict->valueForKey("rightbottom")->getCString());

	YtBoxSprite *pobSprite = new YtBoxSprite();
	pobSprite->autorelease();

	CCSpriteBatchNode *batchNode = CCSpriteBatchNode::create(texturePath);

	for(int y = 0;y < 2;y++)
	{
		for(int x = -size.width/2+frameLeftTop.size.width;x < size.width/2 - frameRightTop.size.width;)
		{
			CCRect rect = frameTop;

			if(x + rect.size.width > size.width/2 - frameRightTop.size.width)
				rect.size.width = size.width/2 - frameRightTop.size.width - x;

			CCSprite* top = CCSprite::createWithTexture(batchNode->getTexture(), rect);

			if(y == 0)
			{
				top->setAnchorPoint(ccp(0.f, 1.0f));
				top->setPosition(ccp(x, size.height/2));
			}
			else
			{
				top->setAnchorPoint(ccp(0.f, 0.0f));
				top->setPosition(ccp(x, -size.height/2));
			}
			batchNode->addChild(top);
			x += rect.size.width;
		}
	}

	for(int x = 0;x < 2;x++)
	{
		for(int y = -size.height/2+frameLeftBottom.size.height;y < size.height/2 - frameLeftTop.size.height;)
		{
			CCRect rect = frameLeft;

			if(y + rect.size.height > size.height/2 - frameLeftTop.size.height)
				rect.size.height = size.height/2 - frameLeftTop.size.height - y;

			CCSprite* left = CCSprite::createWithTexture(batchNode->getTexture(), rect);

			if(x == 0)
			{
				left->setAnchorPoint(ccp(0.f, 0.0f));
				left->setPosition(ccp(-size.width/2, y));
			}
			else
			{
				left->setAnchorPoint(ccp(1.f, 0.0f));
				left->setPosition(ccp(size.width/2, y));
			}
			batchNode->addChild(left);
			y += rect.size.height;
		}
	}

	for(int y = -size.height/2+frameLeftBottom.size.height;y < size.height/2 - frameLeftTop.size.height;)
	{
		CCRect rect = frameCenter;

		if(y + rect.size.height > size.height/2 - frameLeftTop.size.height)
			rect.size.height = size.height/2 - frameLeftTop.size.height - y;

		for(int x = -size.width/2+frameLeftTop.size.width;x < size.width/2 - frameRightTop.size.width;)
		{
			if(x + rect.size.width > size.width/2 - frameRightTop.size.width)
				rect.size.width = size.width/2 - frameRightTop.size.width - x;

			CCSprite* center = CCSprite::createWithTexture(batchNode->getTexture(), rect);

			center->setAnchorPoint(ccp(0.f, 0.0f));
			center->setPosition(ccp(x, y));

			batchNode->addChild(center);

			x += rect.size.width;
		}

		y += rect.size.height;
	}

    CCSprite* lefttop = CCSprite::createWithTexture(batchNode->getTexture(), frameLeftTop);
	lefttop->setAnchorPoint(ccp(0.f, 1.0f));
	lefttop->setPosition(ccp(-size.width/2, size.height/2));

    CCSprite* righttop = CCSprite::createWithTexture(batchNode->getTexture(), frameRightTop);
	righttop->setAnchorPoint(ccp(1.f, 1.0f));
	righttop->setPosition(ccp(size.width/2, size.height/2));

    CCSprite* leftbottom = CCSprite::createWithTexture(batchNode->getTexture(), frameLeftBottom);
	leftbottom->setAnchorPoint(ccp(0.f, 0.f));
	leftbottom->setPosition(ccp(-size.width/2, -size.height/2));

    CCSprite* rightbottom = CCSprite::createWithTexture(batchNode->getTexture(), frameRightBottom);
	rightbottom->setAnchorPoint(ccp(1.f, 0.f));
	rightbottom->setPosition(ccp(size.width/2, -size.height/2));

	batchNode->addChild(lefttop);
    batchNode->addChild(righttop);
	batchNode->addChild(leftbottom);
	batchNode->addChild(rightbottom);
	
	pobSprite->addChild(batchNode);

	return pobSprite;
}



void YtPatternSprite::visit()
{
	
	// quick return if not visible
	if (!isVisible())
    {
		return;
    }

	kmGLPushMatrix();
	
//	glPushMatrix();
	
    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->beforeDraw();
        this->transformAncestors();
    }

	this->transform();

	//CCPoint screenPos = this->convertToWorldSpace(this->getParent()->getPosition());
    
    
    CCPoint originPos = this->getPosition();
    if(cocos2d::CCEGLView::sharedOpenGLView()->isRetinaEnabled())
    {
        CCSize winSize = CCDirector::sharedDirector()->getWinSizeInPixels();
        originPos.x += (winSize.width/4)/YtLib::sharedLib()->m_scale;
        originPos.y += (winSize.height/4)/YtLib::sharedLib()->m_scale;
    }

    
	CCPoint screenPos = this->convertToWorldSpace(originPos);


	glEnable(GL_SCISSOR_TEST);
    float s = this->getScale();
	
	screenPos.x -= m_size.width*s*YtLib::sharedLib()->m_scale/2;
	screenPos.y -= m_size.height*s*YtLib::sharedLib()->m_scale/2;
    
    

    glScissor(screenPos.x*s, screenPos.y*s, m_size.width*s*YtLib::sharedLib()->m_scale, m_size.height*s*YtLib::sharedLib()->m_scale);

    //this->beforeDraw();

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



YtPatternSprite* YtPatternSprite::spriteWithSize(CCSize size, const char* fileImage)
{	
	YtPatternSprite *pobSprite = new YtPatternSprite();
	pobSprite->autorelease();
	pobSprite->initWithColor(ccc4(0, 0, 0, 0), size.width, size.height);

	pobSprite->m_size = size;
	CCSpriteBatchNode *batchNode = CCSpriteBatchNode::create(fileImage, kDefaultSpriteBatchCapacity);

	CCSprite* piece = CCSprite::createWithTexture(batchNode->getTexture());

	CCSize contentSize = piece->getContentSize();

	for(int x = -size.width/2;x < size.width/2;x+=contentSize.width)
	{
		CCSprite* _piece = CCSprite::createWithTexture(batchNode->getTexture());
		_piece->setAnchorPoint(ccp(0,0.5));
		_piece->setPosition(ccp(x, 0));
		batchNode->addChild(_piece);
	}	
	pobSprite->addChild(batchNode);

	return pobSprite;
}


void YtCropSprite::visit(void)
{
 	// quick return if not visible
	if (!isVisible())
    {
		return;
    }
    
	kmGLPushMatrix();
    
    
    // TODO: This scrollview should respect parents' positions
    //CCPoint screenPos = this->convertToWorldSpace(this->getParent()->getPosition());
    
    if(m_isCrop)
    {
        glEnable(GL_SCISSOR_TEST);
        float s = this->getScale();
        float scaleGlobal = YtLib::sharedLib()->m_scale;
        
        float insect = -5;
        
        CCPoint screenPos = this->getParent()->convertToWorldSpace(getPosition());
        
        CCSize sizeCrop = this->getContentSize();
        
        sizeCrop.width -= insect;
        sizeCrop.height -= insect;
        
        
        sizeCrop.width *= (scaleGlobal*s);
        sizeCrop.height *= (scaleGlobal*s);
        
        
        glScissor((GLint)screenPos.x-sizeCrop.width/2, (GLint)screenPos.y-sizeCrop.height/2, (GLsizei)(sizeCrop.width), (GLsizei)(sizeCrop.height));
    }
    
    //	glPushMatrix();
	
    if (m_pGrid && m_pGrid->isActive())
    {
        m_pGrid->beforeDraw();
        this->transformAncestors();
    }
    
	this->transform();
    //this->beforeDraw();
    
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
            
            if(m_isCrop)
            {
                if(child->getTag() == YTSPRITEEX_NOTCROPNODE)
                    glDisable(GL_SCISSOR_TEST);
                
                else
                    glEnable(GL_SCISSOR_TEST);
            }
			child->visit();
		}
        
	}
    else
    {
		this->draw();
    }
    
    if(m_isCrop)
        glDisable(GL_SCISSOR_TEST);
    //this->afterDraw();
	if ( m_pGrid && m_pGrid->isActive())
    {
		m_pGrid->afterDraw(this);
    }
    
	kmGLPopMatrix();
}


YtCropSprite* YtCropSprite::create(const char *pszFileName)
{
	YtCropSprite *pobSprite = new YtCropSprite();
    pobSprite->m_isCrop = false;
	if (pobSprite && pobSprite->initWithFile(pszFileName))
	{
		pobSprite->autorelease();
		return pobSprite;
	}
	CC_SAFE_DELETE(pobSprite);
	return NULL;
}








YtAsyncSprite* YtAsyncSprite::create(const char *pszDefaultSpriteFrameName, const char *strUrl, const char* strTag, bool saveFile, CCSize size)
{
    YtLib* lib = YtLib::sharedLib();
    
	YtAsyncSprite *pobSprite = new YtAsyncSprite();

    saveFile = true;
    
    if(pszDefaultSpriteFrameName == NULL)
        pobSprite->initWithFile("blank_trans.png");
    else
        pobSprite->initWithSpriteFrameName(pszDefaultSpriteFrameName);
    
    pobSprite->m_url = strUrl;
    pobSprite->m_tag = strTag;
    pobSprite->m_size = size;
    pobSprite->m_save = saveFile;
    pobSprite->m_stop = false;
    
    CCSprite* sprLocal = NULL;
    if(saveFile && pobSprite->m_tag.length() > 0)
    {
        sprLocal = lib->createSpriteFromFileSystem(strTag);
  
        if(sprLocal)
        {
            pobSprite->setDisplayFrame(sprLocal->displayFrame());
        }
    }
    
    if(sprLocal == NULL && pobSprite->m_url.length() > 0)
    {
        pobSprite->downloadPicture(httpresponse_selector(YtAsyncSprite::onHttpRequestCompleted));
    }
    
    pobSprite->autorelease();
    return pobSprite;
}


void YtAsyncSprite::downloadPicture(SEL_HttpResponse pSelector)
{
  	std::string bufParam = "";
	
    //CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(m_url.c_str(), bufParam.c_str(), pSelector);
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequestPutGet(m_url.c_str(), pSelector, true);
    
    
	if(request)
	{
		//request->setTag(m_tag.c_str());
        request->setUserData(this);
        this->retain();
 	}
}

void YtAsyncSprite::stopDownload()
{
    m_stop = true;
}


void YtAsyncSprite::onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response)
{
    YtLib* lib = YtLib::sharedLib();
 
    YtAsyncSprite* sprite = (YtAsyncSprite*)response->getHttpRequest()->getUserData();

    if(sprite->m_stop)
    {
        sprite->release();
        return;
    }
    
    if(!response || !response->isSucceed())
    {
        return;
    }

    sprite->release();
	std::vector<char>* buffer = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompletedBinary(sender, response);

    int buffer_size = buffer->size();
    unsigned char* buffer_tgt = (unsigned char*)malloc(buffer_size);

    for(int i = 0;i < buffer_size;i++)
    {
        buffer_tgt[i] = (*buffer)[i];
    }
    
    YtLib::fileWrite(sprite->m_tag.c_str(), (char *)buffer_tgt, buffer_size);
    free(buffer_tgt);
    
    CCSprite* sprPicture = lib->createSpriteFromFileSystem(sprite->m_tag.c_str());
    
    sprite->setDisplayFrame(sprPicture->displayFrame());
    sprite->setScale(sprite->m_size.width/sprPicture->getContentSize().width);
}




//
//void YTClipSprite::visit(void)
//{
//	CCPoint point = CCNode::convertToWorldSpace(m_rectClip.origin);
//
//	glEnable(GL_SCISSOR_TEST);
//	glScissor((GLint)point.x, (GLint)point.y, (GLsizei)m_rectClip.size.width, (GLsizei)m_rectClip.size.height);
//	CCSprite::visit();
//	glDisable(GL_SCISSOR_TEST);
//}
//
//
//void YTClipSprite::setClip(CCRect rect)
//{
//	m_rectClip = rect;
//}
//
//
//CCRect YTClipSprite::getClip()
//{
//	return m_rectClip; 
//}
//
//
//YTClipSprite* YTClipSprite::spriteWithSpriteFrame(CCSpriteFrame *pSpriteFrame)
//{
//	YTClipSprite *pobSprite = new YTClipSprite();
//
//	if (pobSprite && pobSprite->initWithSpriteFrame(pSpriteFrame))
//	{
//		pobSprite->autorelease();
//		return pobSprite;
//	}
//	CC_SAFE_DELETE(pobSprite);
//	return NULL;
//}
//
//
//
////
//// YTClipTo
////
//YTClipTo* YTClipTo::actionWithDuration(ccTime duration, const CCRect& rect)
//{
//	YTClipTo *pClipTo = new YTClipTo();
//	pClipTo->initWithDuration(duration, rect);
//	pClipTo->autorelease();
//
//	return pClipTo;
//}
//
//bool YTClipTo::initWithDuration(ccTime duration, const CCRect& rect)
//{
//	if (CCActionInterval::initWithDuration(duration))
//	{
//		m_endRect = rect;
//		return true;
//	}
//
//	return false;
//}
//
////CCObject* CCMoveTo::copyWithZone(CCZone *pZone)
////{
////	CCZone* pNewZone = NULL;
////	CCMoveTo* pCopy = NULL;
////	if(pZone && pZone->m_pCopyObject) 
////	{
////		//in case of being called at sub class
////		pCopy = (CCMoveTo*)(pZone->m_pCopyObject);
////	}
////	else
////	{
////		pCopy = new CCMoveTo();
////		pZone = pNewZone = new CCZone(pCopy);
////	}
////
////	CCActionInterval::copyWithZone(pZone);
////
////	pCopy->initWithDuration(m_fDuration, m_endPosition);
////
////	CC_SAFE_DELETE(pNewZone);
////	return pCopy;
////}
//
//
//void YTClipTo::startWithTarget(CCNode *pTarget)
//{
//	CCActionInterval::startWithTarget(pTarget);
//	m_startRect = ((YTClipSprite*)pTarget)->getClip();
//	m_deltaRect.origin.x = m_endRect.origin.x - m_startRect.origin.x;
//	m_deltaRect.origin.y = m_endRect.origin.y - m_startRect.origin.y;
//	m_deltaRect.size.width = m_endRect.size.width - m_startRect.size.width;
//	m_deltaRect.size.height = m_endRect.size.height - m_startRect.size.height;
//}
//
//
//void YTClipTo::update(ccTime time)
//{
//	if (m_pTarget)
//	{
//		((YTClipSprite*)m_pTarget)->setClip(CCRectMake(m_startRect.origin.x + m_deltaRect.origin.x * time, 
//				m_startRect.origin.y + m_deltaRect.origin.y * time, m_startRect.size.width + m_deltaRect.size.width * time,
//				m_startRect.size.height + m_deltaRect.size.height * time));
//	}
//}

