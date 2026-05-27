/*
DySceneStory.h
Doggy chef
@initialize at 130201

@history
130201	yoonsr	initialize
*/


#include "DyConfig.h"
#include "DySceneStory.h"
#include "DyLib.h"
#include "DyData.h"

DyLayerStory::DyLayerStory()
{

}

DyLayerStory::~DyLayerStory()
{	
}

void DyLayerStory::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
	setTouchEnabled(true);

	this->setTag(100);
}


void DyLayerStory::onEnter()
{
	CCLayer::onEnter();
	if(m_isInit)
		return;

	m_isInit = true;

	m_toggle = false;
    
	switch(m_storytype)
	{
		case DyStoryType_Intro:
            
        case DyStoryType_Option:
        {
            CCDate current;
            m_dateStart = current;
            
			m_storystatus = DyStoryStatus_Story_0;
			m_menuskipcount = 0;
            getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(false);
			schedule(schedule_selector(DyLayerStory::cbUpdate), 0.1f);
			YtLib::sharedLib()->playBGM(YtLibSndBGM_Story);
			m_actionManager->runAnimations("story_0");
        }
            break;
        case DyStoryType_Puddle:
            
        {
            CCDate current;
            m_dateStart = current;
            
			m_storystatus = DyStoryStatus_Story_0;
			m_menuskipcount = 0;
            getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(false);
			schedule(schedule_selector(DyLayerStory::cbUpdate), 0.1f);
			YtLib::sharedLib()->playBGM(YtLibSndBGM_Puddle);
            
			m_actionManager->runAnimations("story_0");
        }
			break;
            
        case DyStoryType_Ending:
            
        {
            CCDate current;
            m_dateStart = current;
            
			m_storystatus = DyStoryStatus_Story_0;
			m_menuskipcount = 0;
            getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(false);
			schedule(schedule_selector(DyLayerStory::cbUpdate), 0.1f);
			YtLib::sharedLib()->playBGM(YtLibSndBGM_Title);
            
			m_actionManager->runAnimations("story_0");
        }
			break;
		case DyStoryType_Hamburger:
			m_storystatus = DyStoryStatus_Story_0;
            YtLib::sharedLib()->playBGM(YtLibSndBGM_Title);
			m_actionManager->runAnimations("default");
			break;
		case DyStoryType_Coffee:
			m_storystatus = DyStoryStatus_Story_0;
            YtLib::sharedLib()->playBGM(YtLibSndBGM_Title);
			m_actionManager->runAnimations("default");
			break;
		case DyStoryType_China:
			m_storystatus = DyStoryStatus_Story_0;
            YtLib::sharedLib()->playBGM(YtLibSndBGM_Title);
			m_actionManager->runAnimations("default");
			break;
		case DyStoryType_Pizza:
			m_storystatus = DyStoryStatus_Story_0;
            YtLib::sharedLib()->playBGM(YtLibSndBGM_Title);
			m_actionManager->runAnimations("default");
			break;
        case DyStoryType_Korean:
			m_storystatus = DyStoryStatus_Story_0;
            YtLib::sharedLib()->playBGM(YtLibSndBGM_Title);
			m_actionManager->runAnimations("default");
			break;

	}
}


void DyLayerStory::completedAnimationSequenceNamed(const char *name)
{
	switch(m_storytype)
	{
		case DyStoryType_Intro:
        case DyStoryType_Option:
			if(strcmp(name, "story_0") == 0)
			{
                getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(true);
				m_storystatus = DyStoryStatus_Story_0_end;
			}
			if(strcmp(name, "story_1") == 0)
			{
                getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(true);
				m_storystatus = DyStoryStatus_Story_1_end;
			}
			if(strcmp(name, "story_2") == 0)
			{
				getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(true);
				m_storystatus = DyStoryStatus_end;
			}
			break;
        case DyStoryType_Puddle:
            if(strcmp(name, "story_0") == 0)
			{
                getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(true);
				m_storystatus = DyStoryStatus_Story_0_end;
			}
			if(strcmp(name, "story_1") == 0)
			{
				getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(true);
				m_storystatus = DyStoryStatus_end;
			}
            if(strcmp(name, "story_2_end") == 0)
			{
                DyLib::sharedLib()->setStatus(DyLib_AppStatus_Stage, 4+10);
			}
            
            break;
        case DyStoryType_Ending:
            if(strcmp(name, "story_0") == 0)
			{
                getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(true);
				m_storystatus = DyStoryStatus_Story_0_end;
			}
			if(strcmp(name, "story_1") == 0)
			{
				getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(true);
				m_storystatus = DyStoryStatus_end;
			}
            if(strcmp(name, "story_2_end") == 0)
			{
                DyLib::sharedLib()->setStatus(DyLib_AppStatus_MainMenu, 0);
			}
            break;

		case DyStoryType_Hamburger:
			if(strcmp(name, "default") == 0)
			{
				m_storystatus = DyStoryStatus_end;
			}
			break;
		case DyStoryType_Coffee:
			if(strcmp(name, "default") == 0)
			{
				m_storystatus = DyStoryStatus_end;
			}
			break;
		case DyStoryType_China:
			if(strcmp(name, "default") == 0)
			{
				m_storystatus = DyStoryStatus_end;
			}
			break;
		case DyStoryType_Pizza:
			if(strcmp(name, "default") == 0)
			{
				m_storystatus = DyStoryStatus_end;
			}
			break;
		case DyStoryType_Korean:
			if(strcmp(name, "default") == 0)
			{
				m_storystatus = DyStoryStatus_end;
			}
			break;

	}
}


void DyLayerStory::cbUpdate(float dt)
{
    
	CCSprite* sprite0 = (CCSprite*)this->getChildByTag(1000);
	CCSprite* sprite1 = (CCSprite*)this->getChildByTag(1100);
	CCSprite* sprite2 = (CCSprite*)this->getChildByTag(1101);
	
    if(m_storytype == DyStoryType_Intro || m_storytype == DyStoryType_Option)
    {
        if(m_toggle)
        {
            sprite0->setDisplayFrame(CCSprite::create("story_1-3_1.png")->displayFrame());
            sprite1->setDisplayFrame(CCSprite::create("story_2-2-1.png")->displayFrame());
            sprite2->setDisplayFrame(CCSprite::create("story_2-3-1.png")->displayFrame());
        }
        else
        {
            sprite0->setDisplayFrame(CCSprite::create("story_1-3_2.png")->displayFrame());
            sprite1->setDisplayFrame(CCSprite::create("story_2-2-2.png")->displayFrame());
            sprite2->setDisplayFrame(CCSprite::create("story_2-3-2.png")->displayFrame());
        }
    }
    else if(m_storytype == DyStoryType_Puddle)
    {
        if(m_toggle)
        {
            sprite0->setDisplayFrame(CCSprite::create("story_7_3-1-2.png")->displayFrame());
            sprite1->setDisplayFrame(CCSprite::create("story_7_3-3-1.png")->displayFrame());
        }
        else
        {
            sprite0->setDisplayFrame(CCSprite::create("story_7_3-1.png")->displayFrame());
            sprite1->setDisplayFrame(CCSprite::create("story_7_3-3-2.png")->displayFrame());
        }
    }
    
    
    
    
    CCMenuItemImage* itemImage = (CCMenuItemImage*)getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP);
	m_menuskipcount++;
	if(m_menuskipcount > 20)
	{
		m_menuskipcount = 0;
	}

	if(m_menuskipcount < 10)
	{
		itemImage->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("story_ui_next01.png")->displayFrame());
	}
	else
	{
		itemImage->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("story_ui_next02.png")->displayFrame());
	}

	m_toggle = !m_toggle;
}


SEL_MenuHandler DyLayerStory::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onSkipClicked", DyLayerStory::onSkipClicked);
	return NULL;    
}


void DyLayerStory::onSkipClicked(CCObject * pSender)
{   
    switch(m_storystatus)
    {
        case DyStoryStatus_Story_0:
        {
            CCDate current;
            CCDate diffDate = current - m_dateStart;
         
            if(diffDate.time() > 1)
            {
            
                getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(true);
                m_storystatus = DyStoryStatus_Story_0_end;
                m_actionManager->runAnimations("story_0_end");
            }
        }
            break;
        case DyStoryStatus_Story_0_end:
            m_actionManager->runAnimations("story_1");
            m_storystatus = DyStoryStatus_Story_1;
            getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(false);
            break;
        case DyStoryStatus_Story_1:
            
            getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(true);
            if(m_storytype == DyStoryType_Puddle)
                m_storystatus = DyStoryStatus_end;
            else if(m_storytype == DyStoryType_Ending)
                m_storystatus = DyStoryStatus_end;
            else
                m_storystatus = DyStoryStatus_Story_1_end;

            m_actionManager->runAnimations("story_1_end");
            
            
            break;
        case DyStoryStatus_Story_1_end:
            
            m_actionManager->runAnimations("story_2");
            m_storystatus = DyStoryStatus_Story_2;
            getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(false);
            
            
            break;
        case DyStoryStatus_Story_2:
            getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(true);
            m_storystatus = DyStoryStatus_end;
            m_actionManager->runAnimations("story_2_end");
            break;

        case DyStoryStatus_end:
        {
            if(m_storytype == DyStoryType_Option)
            {
                YtLib::sharedLib()->stopBGM();
                
                setTouchEnabled(false);
                DyLib::sharedLib()->setStatus(DyLib_AppStatus_MainMenu, 0);
            }
            else if(m_storytype == DyStoryType_Puddle)
            {
                getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(false);
                
                setTouchEnabled(false);
                m_actionManager->runAnimations("story_2_end");
                
            }
            else if(m_storytype == DyStoryType_Ending)
            {
                getChildByTag(STORY_UI_TAG_MENU)->getChildByTag(STORY_UI_TAG_MENU_SKIP)->setVisible(false);
                
                setTouchEnabled(false);
                m_actionManager->runAnimations("story_2_end");
                
            }
            else
            {
                YtLib::sharedLib()->stopBGM();
                
                CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("menushop_ui.plist");
                
                setTouchEnabled(false);
                DyLib::sharedLib()->setStatus(DyLib_AppStatus_Title, 0);
                
                DyDataManager* dataManager = DyDataManager::sharedDataManager();
                YtOption_set_viewintro(dataManager->m_local.m_option, true);
                dataManager->dataSave();
            }
            
            
            break;
        }
        default:
            break;
    }
}


SEL_CCControlHandler DyLayerStory::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}

bool DyLayerStory::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerStory::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
	CCSetIterator it;
	CCTouch* touch;

	for(it = touches->begin();it != touches->end();it++) 
	{
		touch = (CCTouch*)(*it);
		if(!touch)
			break;

		switch(m_storytype)
		{
			case DyStoryType_Intro:
            case DyStoryType_Option:
				switch(m_storystatus)
				{
                    case DyStoryStatus_Story_0:
                        onSkipClicked(NULL);
                     	break;
					case DyStoryStatus_Story_0_end:
                        onSkipClicked(NULL);
                     	break;
                    case DyStoryStatus_Story_1:
                        onSkipClicked(NULL);
                     	break;
					case DyStoryStatus_Story_1_end:
                        onSkipClicked(NULL);
                     	break;
                    case DyStoryStatus_Story_2:
                        onSkipClicked(NULL);
                     	break;
					case DyStoryStatus_end:
						onSkipClicked(NULL);
						break;
                    default:
                        break;
				}
				break;
			case DyStoryType_Puddle:
                switch(m_storystatus)
                {
                    case DyStoryStatus_Story_0:
                        onSkipClicked(NULL);
                        break;
                    case DyStoryStatus_Story_0_end:
                        onSkipClicked(NULL);
                        break;
                    case DyStoryStatus_Story_1:
                        onSkipClicked(NULL);
                        break;
                    case DyStoryStatus_end:
                        onSkipClicked(NULL);
                    default:
                        break;
                }
				break;
			case DyStoryType_Ending:
                switch(m_storystatus)
                {
                    case DyStoryStatus_Story_0:
                        onSkipClicked(NULL);
                        break;
                    case DyStoryStatus_Story_0_end:
                        onSkipClicked(NULL);
                        break;
                    case DyStoryStatus_Story_1:
                        onSkipClicked(NULL);
                        break;
                    case DyStoryStatus_end:
                        onSkipClicked(NULL);
                    default:
                        break;
                }
				break;


			case DyStoryType_Hamburger:
				if(m_storystatus == DyStoryStatus_end)
				{
                    setTouchEnabled(false);
					DyLib::sharedLib()->setStatus(DyLib_AppStatus_Stage, 1);
				}
				break;
			case DyStoryType_Coffee:
				if(m_storystatus == DyStoryStatus_end)
				{
                    setTouchEnabled(false);
					DyLib::sharedLib()->setStatus(DyLib_AppStatus_Stage, 2);
				}
				break;
			case DyStoryType_China:
				if(m_storystatus == DyStoryStatus_end)
				{
                    setTouchEnabled(false);
					DyLib::sharedLib()->setStatus(DyLib_AppStatus_Stage, 3);
				}
				break;
			case DyStoryType_Pizza:
				if(m_storystatus == DyStoryStatus_end)
				{
                    setTouchEnabled(false);
					DyLib::sharedLib()->setStatus(DyLib_AppStatus_Stage, 4);
				}
				break;
			case DyStoryType_Korean:
				if(m_storystatus == DyStoryStatus_end)
				{
                    setTouchEnabled(false);
					DyLib::sharedLib()->setStatus(DyLib_AppStatus_Stage, 5);
				}
				break;

		}
		return;
	}
}


void DyLayerStory::pause()
{
}


void DyLayerStory::resume()
{
}


void DyLayerStory::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerStory::show()
{
}


DySceneStory::DySceneStory()
{
}

DySceneStory::~DySceneStory()
{
}


void DySceneStory::didSceneChanged()
{
	DyLayerStory* layer = (DyLayerStory*)getChildByTag(100);
	layer->m_actionManager->setDelegate(layer);
}
