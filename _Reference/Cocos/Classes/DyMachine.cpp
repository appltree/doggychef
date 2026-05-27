/*
DyMachine.cpp
Doggy chef
@initialize at 131205

@history
131205	yoonsr	initialize
*/

#include "DyMachine.h"
#include "DySceneGame.h"
#include "DyData.h"
#include "DyLib.h"

DyMachineManager* g_sharedMachineManager;


DyMachineManager::DyMachineManager()
{
}


DyMachineManager::~DyMachineManager()
{
}



void DyMachineManager::setMachineImage(CCSprite* sprMachine, bool isInUpgradeView)
{
    switch(m_curShop)
	{
        case DyStageType_Snack:
            if(m_curMachineLevel <= 1)
                sprMachine->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_machine.png")->displayFrame());
            else
                sprMachine->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_machine2.png")->displayFrame());
            break;
        case DyStageType_Hamburger:
            if(m_curMachineLevel <= 1)
                sprMachine->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_hamburger_machine.png")->displayFrame());
            else
                sprMachine->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_hamburger_machine2.png")->displayFrame());
            break;
            
        case DyStageType_Coffee:
            if(m_curMachineLevel <= 1)
                sprMachine->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_coffee_machine.png")->displayFrame());
            else
                sprMachine->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_coffee_machine2.png")->displayFrame());
            break;
        case DyStageType_China:
            if(m_curMachineLevel <= 1)
                sprMachine->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_machine.png")->displayFrame());
            else
                sprMachine->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_machine2.png")->displayFrame());
            
            if(isInUpgradeView)
                sprMachine->setScale(0.8);
            break;
        case DyStageType_Pizza:
            if(m_curMachineLevel <= 1)
                sprMachine->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_machine.png")->displayFrame());
            else
                sprMachine->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_machine2.png")->displayFrame());
            
            if(isInUpgradeView)
                sprMachine->setScale(0.9);
            break;
        case DyStageType_Korean:
            if(m_curMachineLevel <= 1)
                sprMachine->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_machine.png")->displayFrame());
            else
                sprMachine->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_machine2.png")->displayFrame());
            
            if(isInUpgradeView)
                sprMachine->setScale(0.8);
            break;


	}
}


CCSprite* DyMachineManager::createSide()
{
    switch(m_curShop)
	{
        case DyStageType_Snack:
            if(m_curMachineLevel <= 1)
                return CCSprite::createWithSpriteFrameName("shop_ui_side.png");
            else
                return CCSprite::createWithSpriteFrameName("shop_ui_side2.png");
            break;
        case DyStageType_Hamburger:
            if(m_curMachineLevel <= 1)
                return CCSprite::createWithSpriteFrameName("shop_ui_hamburger_side.png");
            else
                return CCSprite::createWithSpriteFrameName("shop_ui_hamburger_side2.png");
            break;
            
        case DyStageType_Coffee:
            if(m_curMachineLevel <= 1)
                return CCSprite::createWithSpriteFrameName("shop_ui_coffee_side.png");
            else
                return CCSprite::createWithSpriteFrameName("shop_ui_coffee_side2.png");
            break;
            
        case DyStageType_China:
            if(m_curMachineLevel <= 1)
                return CCSprite::createWithSpriteFrameName("shop_ui_china_side.png");
            else
                return CCSprite::createWithSpriteFrameName("shop_ui_china_side2.png");
            break;
        case DyStageType_Pizza:
            if(m_curMachineLevel <= 1)
                return CCSprite::createWithSpriteFrameName("shop_ui_pizza_side.png");
            else
                return CCSprite::createWithSpriteFrameName("shop_ui_pizza_side2.png");
            break;
        case DyStageType_Korean:
            if(m_curMachineLevel <= 1)
                return CCSprite::createWithSpriteFrameName("shop_ui_korean_side.png");
            else
                return CCSprite::createWithSpriteFrameName("shop_ui_korean_side2.png");
            break;


	}
}


CCSprite* DyMachineManager::createSideGlow()
{
    switch(m_curShop)
	{
        case DyStageType_Snack:
            if(m_curMachineLevel <= 1)
                return CCSprite::createWithSpriteFrameName("shop_ui_side_glow.png");
            else
                return CCSprite::createWithSpriteFrameName("shop_ui_side2_glow.png");
        case DyStageType_Hamburger:
            if(m_curMachineLevel <= 1)
                return CCSprite::createWithSpriteFrameName("shop_ui_hamburger_side_glow.png");
            else
                return CCSprite::createWithSpriteFrameName("shop_ui_hamburger_side2_glow.png");
        case DyStageType_Coffee:
            return CCSprite::createWithSpriteFrameName("shop_ui_coffee_side_glow.png");
        case DyStageType_China:
            if(m_curMachineLevel <= 1)
                return CCSprite::createWithSpriteFrameName("shop_ui_china_side_glow.png");
            else
                return CCSprite::createWithSpriteFrameName("shop_ui_china_side2_glow.png");
        case DyStageType_Pizza:
            return CCSprite::createWithSpriteFrameName("shop_ui_pizza_side_glow.png");
        case DyStageType_Korean:
            if(m_curMachineLevel <= 1)
                return CCSprite::createWithSpriteFrameName("shop_ui_korean_side_glow.png");
            else
                return CCSprite::createWithSpriteFrameName("shop_ui_korean_side2_glow.png");
	}
}



void DyMachineManager::setupMachineUI(CCLayer* layer, DyStageType shop)
{
    m_curShop = shop;
    m_curMachineLevel = getMachineLevel(shop);
    
    m_sprMachineGlow = (CCSprite*)layer->getChildByTag(GAME_UI_TAG_MACHINE);
	m_sprMachineButton[0] = (CCSprite*)m_sprMachineGlow->getChildByTag(0);
	m_sprMachineButton[1] = (CCSprite*)m_sprMachineGlow->getChildByTag(1);
	m_sprMachineButton[2] = (CCSprite*)m_sprMachineGlow->getChildByTag(2);
    m_sprMachine = (CCSprite*)m_sprMachineGlow->getChildByTag(10);
    
    layer->reorderChild(m_sprMachineGlow, ZORDER_GAME_MACHINE);
	
    rampUI(0);
    machineSelect(false);
    setMachineImage(m_sprMachine, false);
    
    
    switch(shop)
	{
        case DyStageType_Snack:
            
            if(m_curMachineLevel <= 1)
                m_sprMachineGlow->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_machine_glow.png")->displayFrame());
            else
                m_sprMachineGlow->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_machine2_glow.png")->displayFrame());
            
            m_sprMachineGlow->setPosition(ccp(56, 64));
            m_sprMachineButton[0]->setPosition(ccp(46, 24));
            m_sprMachineButton[1]->setPosition(ccp(68, 24));
            m_sprMachineButton[2]->setPosition(ccp(90, 24));
            break;
        case DyStageType_Hamburger:
            m_sprMachineGlow->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_hamburger_machine_glow.png")->displayFrame());
            
            m_sprMachineGlow->setPosition(ccp(42, 64));
            m_sprMachineButton[0]->setPosition(ccp(77, 21));
            m_sprMachineButton[1]->setPosition(ccp(99, 21));
            m_sprMachineButton[2]->setPosition(ccp(121, 21));
            break;
            
        case DyStageType_Coffee:
            m_sprMachineGlow->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_coffee_machine_glow.png")->displayFrame());
            
            m_sprMachineGlow->setPosition(ccp(47, 64));
            m_sprMachineButton[0]->setPosition(ccp(40, 33));
            m_sprMachineButton[1]->setPosition(ccp(62, 33));
            m_sprMachineButton[2]->setPosition(ccp(84, 33));

            break;
            
        case DyStageType_China:
            if(m_curMachineLevel <= 1)
            {
                m_sprMachineGlow->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_machine_glow.png")->displayFrame());
                m_sprMachineButton[0]->setPosition(ccp(65, 56));
                m_sprMachineButton[1]->setPosition(ccp(87, 56));
                m_sprMachineButton[2]->setPosition(ccp(109, 56));

            }
            else
            {
                m_sprMachineGlow->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_china_machine2_glow.png")->displayFrame());
                m_sprMachineButton[0]->setPosition(ccp(49, 56));
                m_sprMachineButton[1]->setPosition(ccp(71, 56));
                m_sprMachineButton[2]->setPosition(ccp(93, 56));
            }

            m_sprMachineGlow->setPosition(ccp(61, 45));
            m_sprMachine->setPosition(ccp(5,4));
            
            
            break;
        case DyStageType_Pizza:
            if(m_curMachineLevel <= 1)
            {
                m_sprMachineGlow->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_machine_glow.png")->displayFrame());
                m_sprMachineButton[0]->setPosition(ccp(65-30, 56-9));
                m_sprMachineButton[1]->setPosition(ccp(87-30, 56-9));
                m_sprMachineButton[2]->setPosition(ccp(109-30, 56-9));
                
            }
            else
            {
                m_sprMachineGlow->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_pizza_machine_glow.png")->displayFrame());
                m_sprMachineButton[0]->setPosition(ccp(65-30+1, 56-9+1));
                m_sprMachineButton[1]->setPosition(ccp(87-30+1, 56-9+1));
                m_sprMachineButton[2]->setPosition(ccp(109-30+1, 56-9+1));
            }
            
            m_sprMachineGlow->setPosition(ccp(61, 45));
            m_sprMachine->setPosition(ccp(5,4));
            
            
            break;
            
        case DyStageType_Korean:
            if(m_curMachineLevel <= 1)
            {
                m_sprMachineGlow->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_machine_glow.png")->displayFrame());
                m_sprMachineButton[0]->setPosition(ccp(65-34+12, 22-8));
                m_sprMachineButton[1]->setPosition(ccp(87-34+12, 22-8));
                m_sprMachineButton[2]->setPosition(ccp(109-34+12, 22-8));
                
            }
            else
            {
                m_sprMachineGlow->setDisplayFrame(CCSprite::createWithSpriteFrameName("shop_ui_korean_machine2_glow.png")->displayFrame());
                m_sprMachineButton[0]->setPosition(ccp(65-34+12, 22-8));
                m_sprMachineButton[1]->setPosition(ccp(87-34+12, 22-8));
                m_sprMachineButton[2]->setPosition(ccp(109-34+12, 22-8));
            }
            
            m_sprMachineGlow->setPosition(ccp(61+10-10-2, 45+15+3+10-1));
            //m_sprMachine->setPosition(ccp(5,4));
            
            
            break;


	}
    
    
    m_layer = layer;
}


void DyMachineManager::setMachineReady(bool ready, CCLayer* layer, CCMenuItemImage* itemImage)
{
    if(ready)
    {
        m_layer->reorderChild(m_sprMachineGlow, ZORDER_GAME_SETTING_ON);
        m_sprMachineGlow->setColor(ccc3(255, 255, 255));
        m_sprMachineGlow->setOpacity(255);
        
        for(int i = 0;i <= MC_MACHINE_MAX_LEVEL;i++)
        {
            CCSprite* sprMark = (CCSprite*)layer->getChildByTag(i);
            if(i <= m_curMachineLevel)
            {
                sprMark->setDisplayFrame(CCSprite::createWithSpriteFrameName("game_ui_machine_good.png")->displayFrame());
            }
            else
                sprMark->setDisplayFrame(CCSprite::createWithSpriteFrameName("game_ui_machine_bad.png")->displayFrame());
                
        }
        
        if(m_curMachineLevel == MC_MACHINE_MAX_LEVEL)
        {
            itemImage->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("game_ui_machine_ok_btn.png")->displayFrame());
            itemImage->setSelectedSpriteFrame(CCSprite::createWithSpriteFrameName("game_ui_machine_ok_btn.png")->displayFrame());
        }
        
    }
    else
    {
        m_layer->reorderChild(m_sprMachineGlow, ZORDER_GAME_MACHINE);
        machineSelect(false);
    }
}



int DyMachineManager::getSidePrice()
{
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    CCArray* arrayPrice = stageManager->m_shopinfo->getArray("side_price", m_curShop);
    
    return atoi(((CCString*)arrayPrice->objectAtIndex(m_curMachineLevel))->getCString());
}



int DyMachineManager::getMachineLevel()
{
    return m_curMachineLevel;
}


float DyMachineManager::getFullTime()
{
    switch(m_curMachineLevel)
    {
        case 0:
            return 11.f;
        case 1:
            return 10.f;
        case 2:
            return 9.f;
        case 3:
            return 8.f;
    }
    return 10.f;
}


std::string DyMachineManager::getCurrentMachineName()
{
    DyLib* lib = DyLib::sharedLib();
    
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    sprintf(lib->m_strTemp, "%s Lv.%d", stageManager->m_shopinfo->getValue("side_name", m_curShop), m_curMachineLevel+1);
    
    return lib->m_strTemp;
}


int DyMachineManager::getNextUpgradePrice()
{
    if(m_curMachineLevel >= MC_MACHINE_MAX_LEVEL)
        return 0;
    
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    CCArray* arrayPrice = stageManager->m_shopinfo->getArray("side_upgrade", m_curShop);
    
    return atoi(((CCString*)arrayPrice->objectAtIndex(m_curMachineLevel+1))->getCString());
}


bool DyMachineManager::canUpgrade()
{
    return m_curMachineLevel < MC_MACHINE_MAX_LEVEL;
}

int DyMachineManager::getMachineLevel(DyStageType shop)
{
//    if(shop < SHOP_COUNT_DEFAULT)
//        return DyDataManager::sharedDataManager()->m_private.m_machineGrade[shop];
//    
//    return DyDataManager::sharedDataManager()->m_private.m_machineGrade_added[shop-SHOP_COUNT_DEFAULT];
    
    return DyDataManager::sharedDataManager()->m_private.m_machineGrade[shop];
}


void DyMachineManager::upgradeMachine()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
//    if(m_curShop < SHOP_COUNT_DEFAULT)
//        dataManager->m_private.m_machineGrade[m_curShop]++;
//    else
//        dataManager->m_private.m_machineGrade_added[m_curShop-SHOP_COUNT_DEFAULT]++;
    
    dataManager->m_private.m_machineGrade[m_curShop]++;
    
    m_curMachineLevel = getMachineLevel(m_curShop);
}

void DyMachineManager::sideReadyUI()
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_SIDE_READY);

    CCActionInterval* action = CCScaleTo::create(.12f, 1.2f);
    CCActionInterval* actionReverse = CCScaleTo::create(.05f, 1.0f);
    m_sprMachineGlow->runAction(CCSequence::create(action, actionReverse, NULL));

    m_sprMachineGlow->setColor(ccc3(255, 255, 255));
    m_sprMachineGlow->setOpacity(255);

}


void DyMachineManager::rampUI(int grade)
{
    m_grade = grade;
    
	if(grade == 3)
	{
 		m_sprMachineButton[0]->setVisible(true);
		m_sprMachineButton[1]->setVisible(true);
		m_sprMachineButton[2]->setVisible(true);
	}
	else if(grade == 2)
	{
  		m_sprMachineButton[0]->setVisible(true);
		m_sprMachineButton[1]->setVisible(true);
		m_sprMachineButton[2]->setVisible(false);
	}
	else if(grade == 1)
	{
        m_sprMachineButton[0]->setVisible(true);
		m_sprMachineButton[1]->setVisible(false);
		m_sprMachineButton[2]->setVisible(false);
	}
	else
	{
     	m_sprMachineButton[0]->setVisible(false);
		m_sprMachineButton[1]->setVisible(false);
		m_sprMachineButton[2]->setVisible(false);
        m_sprMachineGlow->setOpacity(0);
	}
//    if(m_grade == 3)
//    {
//        m_sprMachineGlow->setColor(ccc3(255, 255, 255));
//        m_sprMachineGlow->setOpacity(255);
//        
//    }
//    else
//        m_sprMachineGlow->setOpacity(0);

}


bool DyMachineManager::isSelectMachine(CCPoint position)
{
    CCSize size = m_sprMachineGlow->getContentSize();
	CCPoint point = m_sprMachineGlow->getPosition();
	CCRect rect = CCRectMake(point.x-size.width/2, point.y-size.height/2, size.width, size.height);
    
	if(rect.containsPoint(position))
	{
        machineSelect(true);
		return true;
	}
    return false;

}


void DyMachineManager::machineSelect(bool select)
{
    if(select)
    {
        YtLib::sharedLib()->playEffect(YtLibSndEff_CLICK);
        m_sprMachineGlow->setColor(ccc3(255, 255, 0));
        m_sprMachineGlow->setOpacity(255);
    }
    else
    {
        if(m_grade == 3)
        {
            m_sprMachineGlow->setColor(ccc3(255, 255, 255));
            m_sprMachineGlow->setOpacity(255);

        }
        else
            m_sprMachineGlow->setOpacity(0);
    }
}


DyMachineManager* DyMachineManager::sharedMachineManager()
{
	if(g_sharedMachineManager == NULL)
	{
		g_sharedMachineManager = new DyMachineManager();
	}
	return g_sharedMachineManager;
}

