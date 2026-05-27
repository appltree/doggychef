/*
TmLayerMain.cpp
Three Match Lib
@initialize at 120907

@history
120907	yoonsr	initialize
*/

#include "TmLayerMain.h"
#include "YtLib.h"
#include "TmBoard.h"

TmLayerMain::TmLayerMain(CCSize size)
{
	m_size = size;
	initWithColor(ccc4(255, 0, 0, 0), m_size.width, m_size.height);
	setTouchEnabled(true);
}


void TmLayerMain::update(float dt)
{
	m_board->update(dt);
}

void TmLayerMain::visit()
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

    
    CCPoint originPos = this->getParent()->getPosition();
    
    if(cocos2d::CCEGLView::sharedOpenGLView()->isRetinaEnabled())
    {
        CCSize winSize = CCDirector::sharedDirector()->getWinSizeInPixels();
        originPos.x += (winSize.width/4)/YtLib::sharedLib()->m_scale;
        originPos.y += (winSize.height/4)/YtLib::sharedLib()->m_scale;
    }

	CCPoint screenPos = this->convertToWorldSpace(originPos);
    if(m_board->m_isHard)
        screenPos = ccpAdd(screenPos, ccp(-5, -5));
    
	glEnable(GL_SCISSOR_TEST);
    float s = this->getScale();
	
    glScissor(screenPos.x*s, screenPos.y*s, m_size.width*s*YtLib::sharedLib()->m_scale, m_size.height*s*YtLib::sharedLib()->m_scale+50);
    
    

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


