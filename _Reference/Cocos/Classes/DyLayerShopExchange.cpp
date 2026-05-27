/*
DyLayerShopExchange.cpp
Doggy chef
@initialize at 121207

@history
121207	yoonsr	initialize
*/


#include "DyLayerShopExchange.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyBlockInfo.h"
#include "DyPopup.h"

#define CHANGE_UNIT          10

DyLayerShopExchange::DyLayerShopExchange()
{
	m_menu = NULL;
}

DyLayerShopExchange::~DyLayerShopExchange()
{
}

void DyLayerShopExchange::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader) 
{	
	YtLib* lib = YtLib::sharedLib();

	DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();

	m_menu = (CCMenu*)this->getChildByTag(SHOPEXCHANGE_UI_TAG_MENU);

	((CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_EXPLAIN_EXCHANGERATE))->setColor(ccc3(100,57,38));
	((CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_EXPLAIN_EXCHANGERATE_GOLD))->setColor(ccc3(100,57,38));
	((CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_EXPLAIN_EXCHANGERATE_DELLY))->setColor(ccc3(100,57,38));

	((CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_TEXT_DELLAY))->setColor(ccBLACK);
	((CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_TEXT_GOLD))->setColor(ccBLACK);

	CCLabelTTF* labelRate = (CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_EXPLAIN_EXCHANGERATE_GOLD);

	YtLib::setMoneyStyle(lib->m_strTemp, moneyManager->m_exchange_rate);
	labelRate->setString(lib->m_strTemp);

	CCMenuItemImage* menuitem_plus = (CCMenuItemImage*)m_menu->getChildByTag(SHOPEXCHANGE_UI_TAG_MENUITEM_PLUS);

	menuitem_plus->m_pfnPress = menu_selector(DyLayerShopExchange::onPlusPressed);
	menuitem_plus->m_pfnReleased = menu_selector(DyLayerShopExchange::onPlusReleased);
	
	CCMenuItemImage* menuitem_minus = (CCMenuItemImage*)m_menu->getChildByTag(SHOPEXCHANGE_UI_TAG_MENUITEM_MINUS);

	menuitem_minus->m_pfnPress = menu_selector(DyLayerShopExchange::onMinusPressed);
	menuitem_minus->m_pfnReleased = menu_selector(DyLayerShopExchange::onMinusReleased);
	
	setDelly(CHANGE_UNIT);
}


void DyLayerShopExchange::onEnter()
{
	CCLayer::onEnter();
	if(m_isInit)
		return;
	m_isInit = true;
}


void DyLayerShopExchange::onExit()
{
	CCLayer::onExit();
}


SEL_MenuHandler DyLayerShopExchange::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPlusClicked", DyLayerShopExchange::onPlusClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onMinusClicked", DyLayerShopExchange::onMinusClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onExchangeClicked", DyLayerShopExchange::onExchangeClicked);

    return NULL;    
}

SEL_CCControlHandler DyLayerShopExchange::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}

bool DyLayerShopExchange::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}

void DyLayerShopExchange::pause()
{
	if(m_menu)
		m_menu->setEnabled(false);
}


void DyLayerShopExchange::setDelly(int delly)
{
    YtLib* lib = YtLib::sharedLib();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    CCLabelTTF* labelDelly = (CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_TEXT_DELLAY);
    CCLabelTTF* labelGold = (CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_TEXT_GOLD);
    m_currentDelly = delly;
    if(moneyManager->getDelly() < delly)
        m_currentDelly = 0;
    m_currentGold = moneyManager->m_exchange_rate*m_currentDelly;
    
    sprintf(lib->m_strTemp, "%d", m_currentDelly);
    labelDelly->setString(lib->m_strTemp);
    
    YtLib::setMoneyStyle(lib->m_strTemp, m_currentGold);
    labelGold->setString(lib->m_strTemp);
}


void DyLayerShopExchange::resume()
{
	if(m_menu)
	{
        setDelly(m_currentDelly);
        
		m_menu->setEnabled(true);
	}
}


void DyLayerShopExchange::hide()
{
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerShopExchange::show()
{
}


void DyLayerShopExchange::cbPlusMinusReal(float dt)
{
	YtLib* lib = YtLib::sharedLib();
	DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
	CCLabelTTF* labelDelly = (CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_TEXT_DELLAY);
	CCLabelTTF* labelGold = (CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_TEXT_GOLD);

	if(m_isPlusCb)
		m_currentDelly += CHANGE_UNIT;
	else
		m_currentDelly -= CHANGE_UNIT;

    
    if(m_currentDelly > moneyManager->getDelly())
	{
        m_currentDelly -= CHANGE_UNIT;
    }
    

	//if(m_currentDelly >= moneyManager->getDelly())
	//	m_currentDelly = moneyManager->getDelly();
	if(m_currentDelly <= 0)
		m_currentDelly = 0;

	m_currentGold = moneyManager->m_exchange_rate*m_currentDelly;
	
	sprintf(lib->m_strTemp, "%d", m_currentDelly);
	labelDelly->setString(lib->m_strTemp);

	YtLib::setMoneyStyle(lib->m_strTemp, m_currentGold);
	labelGold->setString(lib->m_strTemp);
}


void DyLayerShopExchange::cbPlusMinus(float dt)
{
	unschedule(schedule_selector(DyLayerShopExchange::cbPlusMinus));
	schedule(schedule_selector(DyLayerShopExchange::cbPlusMinusReal), .1f);
}

void DyLayerShopExchange::onPlusPressed(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	m_isPlusCb = true;
	schedule(schedule_selector(DyLayerShopExchange::cbPlusMinus), .6f);
}
	
void DyLayerShopExchange::onPlusReleased(CCObject * obj)
{
	unschedule(schedule_selector(DyLayerShopExchange::cbPlusMinus));
	unschedule(schedule_selector(DyLayerShopExchange::cbPlusMinusReal));
}

void DyLayerShopExchange::onMinusPressed(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	m_isPlusCb = false;
	schedule(schedule_selector(DyLayerShopExchange::cbPlusMinus), .6f);
}
	
void DyLayerShopExchange::onMinusReleased(CCObject * obj)
{
	unschedule(schedule_selector(DyLayerShopExchange::cbPlusMinus));
	unschedule(schedule_selector(DyLayerShopExchange::cbPlusMinusReal));
}


void DyLayerShopExchange::onPlusClicked(CCObject * obj)
{
	YtLib* lib = YtLib::sharedLib();
	DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
	CCLabelTTF* labelDelly = (CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_TEXT_DELLAY);
	CCLabelTTF* labelGold = (CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_TEXT_GOLD);

	m_currentDelly += CHANGE_UNIT;

	if(m_currentDelly > moneyManager->getDelly())
	{
        m_currentDelly -= CHANGE_UNIT;
        
        if(moneyManager->getDelly() <= 0)
        {
            DyMoneyManager::sharedMoneyManager()->alramNotEnoughDelly(true, true);
        }
        else if(moneyManager->getDelly()%10 != 0)
        {
            DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyMoneyManager::onPopupClicked));
            popup->setText(STRING_DELLY_UNIT);
            
            popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 20);
            popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 21);
            popup->addButton(DyButtonType_Close, "", 20);
            
            YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        }
        return;
	}

	m_currentGold = moneyManager->m_exchange_rate*m_currentDelly;
	
	sprintf(lib->m_strTemp, "%d", m_currentDelly);
	labelDelly->setString(lib->m_strTemp);

	YtLib::setMoneyStyle(lib->m_strTemp, m_currentGold);
	labelGold->setString(lib->m_strTemp);
}


void DyLayerShopExchange::onMinusClicked(CCObject * obj)
{
	YtLib* lib = YtLib::sharedLib();
	DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
	CCLabelTTF* labelDelly = (CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_TEXT_DELLAY);
	CCLabelTTF* labelGold = (CCLabelTTF*)getChildByTag(SHOPEXCHANGE_UI_TAG_TEXT_GOLD);

	m_currentDelly -= CHANGE_UNIT;

	if(m_currentDelly <= 0)
		m_currentDelly = 0;
	
	m_currentGold = moneyManager->m_exchange_rate*m_currentDelly;
	
    sprintf(lib->m_strTemp, "%d", m_currentDelly);
	labelDelly->setString(lib->m_strTemp);
	YtLib::setMoneyStyle(lib->m_strTemp, m_currentGold);
	labelGold->setString(lib->m_strTemp);
}


void DyLayerShopExchange::onPopupClicked(cocos2d::CCObject * pSender)
{
	DyLib* lib = DyLib::sharedLib();
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);

	DyPopup* popup = NULL;

	if(pPopup->m_active_tag == 1)
	{
        YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_MSG);
        
        
		DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();

		popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShopExchange::onPopupClicked));

		moneyManager->addDelly(-m_currentDelly, true);
		moneyManager->addGold(m_currentGold, true);
		moneyManager->dataSave();

		//YtLib::setMoneyStyle(lib->m_strTemp2, m_currentGold);
		//sprintf(lib->m_strTemp3, lib->m_dicText->valueForKey("exchange_result")->getCString(), m_currentDelly, lib->m_strTemp2);

		YtLib::setMoneyStyle(lib->m_strTemp2, moneyManager->getGold());
		sprintf(lib->m_strTemp4, STRING_exchange_result2, lib->m_strTemp2);

		sprintf(lib->m_strTemp, "%s%s", STRING_exchange_result, lib->m_strTemp4);

		popup->setText(lib->m_strTemp);
		popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 10);
	}
	else if(pPopup->m_active_tag == 10) // exchange ok
	{
	}
	
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);

	if(popup)
		YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerShopExchange::onExchangeClicked(CCObject * obj)
{
	if(m_currentDelly == 0)
		return;
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	DyLib* lib = DyLib::sharedLib();
	//DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
	DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShopExchange::onPopupClicked));
	
	YtLib::setMoneyStyle(lib->m_strTemp2, m_currentGold);
	sprintf(lib->m_strTemp, STRING_exchange_question, m_currentDelly, lib->m_strTemp2);

	popup->setText(lib->m_strTemp);
	popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
	popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 1);
	popup->addButton(DyButtonType_Close, "", 2);

	YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


