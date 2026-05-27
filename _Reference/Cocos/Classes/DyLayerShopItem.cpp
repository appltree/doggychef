/*
DyLayerShopItem.cpp
Doggy chef
@initialize at 121207

@history
121207	yoonsr	initialize
*/


#include "DyLayerShopItem.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyBlockInfo.h"
#include "DyPopup.h"


#define CHANGE_UNIT          10

DyLayerShopItem::DyLayerShopItem()
{
	for(int i = 0;i < DyItemType_Count;i++)
		m_menu[i] = NULL;
}

DyLayerShopItem::~DyLayerShopItem()
{
}

void DyLayerShopItem::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader) 
{

}

void DyLayerShopItem::onEnter()
{
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    
	CCLayer::onEnter();
	if(m_isInit)
		return;
	m_isInit = true;
	//YtLib* lib = YtLib::sharedLib();

	for(int i = 0;i < DyItemType_Count;i++)
	{
		DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(i);

		CCLayer* layerGroup = (CCLayer*)this->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*i);

		m_menu[i] = (CCMenu*)layerGroup->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*i + SHOPITEM_UI_TAG_SUB_MENU);

		CCLabelTTF* labelName = (CCLabelTTF*)layerGroup->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*i + SHOPITEM_UI_TAG_SUB_NAME);
		labelName->setColor(ccc3(255, 221, 36));
		labelName->setString(item->m_name.c_str());

		CCLabelTTF* labelExplain = (CCLabelTTF*)layerGroup->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*i + SHOPITEM_UI_TAG_SUB_EXPLAIN);
		labelExplain->setColor(ccWHITE);
		labelExplain->setString(item->m_explain_kor.c_str());

		CCMenuItemImage* menuitem_plus = (CCMenuItemImage*)m_menu[i]->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*i + SHOPITEM_UI_TAG_SUB_MENUITEM_PLUS);

		menuitem_plus->m_pfnPress = menu_selector(DyLayerShopItem::onPlusPressed);
		menuitem_plus->m_pfnReleased = menu_selector(DyLayerShopItem::onPlusReleased);
	
		CCMenuItemImage* menuitem_minus = (CCMenuItemImage*)m_menu[i]->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*i + SHOPITEM_UI_TAG_SUB_MENUITEM_MINUS);

		menuitem_minus->m_pfnPress = menu_selector(DyLayerShopItem::onMinusPressed);
		menuitem_minus->m_pfnReleased = menu_selector(DyLayerShopItem::onMinusReleased);
        
        if(item->m_price_gold*CHANGE_UNIT < moneyManager->getGold())
            m_count[i] = CHANGE_UNIT;
        else
            m_count[i] = 0;
        
	}
   
	resume();
}


void DyLayerShopItem::onExit()
{
	CCLayer::onExit();
}


void DyLayerShopItem::cbPlusMinusReal(float dt)
{
	YtLib* lib = YtLib::sharedLib();

	DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();

	DyItemManager* itemManager = DyItemManager::sharedItemManager();

	DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(m_itemBuyProcess);

	if(m_isPlusCb)
		m_count[m_itemBuyProcess] += CHANGE_UNIT;
	else
		m_count[m_itemBuyProcess] -= CHANGE_UNIT;

	if(m_count[m_itemBuyProcess]*item->m_price_gold > moneyManager->getGold())
		m_count[m_itemBuyProcess] -= CHANGE_UNIT;

	if(m_count[m_itemBuyProcess] < 0)
		m_count[m_itemBuyProcess] = 0;

	CCLayer* layerGroup = (CCLayer*)this->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*m_itemBuyProcess);
	CCLabelTTF* labelCount = (CCLabelTTF*)layerGroup->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*m_itemBuyProcess + SHOPITEM_UI_TAG_SUB_COUNT);
	sprintf(lib->m_strTemp, "%d", m_count[m_itemBuyProcess]);
	labelCount->setString(lib->m_strTemp);

	CCLabelTTF* labelPrice = (CCLabelTTF*)layerGroup->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*m_itemBuyProcess + SHOPITEM_UI_TAG_SUB_PRICE);
	YtLib::setMoneyStyle(lib->m_strTemp, item->m_price_gold*m_count[m_itemBuyProcess]);
	labelPrice->setString(lib->m_strTemp);
}


void DyLayerShopItem::cbPlusMinus(float dt)
{
	unschedule(schedule_selector(DyLayerShopItem::cbPlusMinus));
	schedule(schedule_selector(DyLayerShopItem::cbPlusMinusReal), .1f);
}

void DyLayerShopItem::onPlusPressed(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	CCMenuItemImage *menuitem = dynamic_cast<CCMenuItemImage*>(obj);
	m_itemBuyProcess = (menuitem->getTag()-SHOPITEM_UI_TAG_GROUP_HINT)/1000;

	m_isPlusCb = true;
	schedule(schedule_selector(DyLayerShopItem::cbPlusMinus), .6f);
}
	
void DyLayerShopItem::onPlusReleased(CCObject * obj)
{
	unschedule(schedule_selector(DyLayerShopItem::cbPlusMinus));
	unschedule(schedule_selector(DyLayerShopItem::cbPlusMinusReal));
}

void DyLayerShopItem::onMinusPressed(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	CCMenuItemImage *menuitem = dynamic_cast<CCMenuItemImage*>(obj);
	m_itemBuyProcess = (menuitem->getTag()-SHOPITEM_UI_TAG_GROUP_HINT)/1000;

	m_isPlusCb = false;
	schedule(schedule_selector(DyLayerShopItem::cbPlusMinus), .6f);
}
	
void DyLayerShopItem::onMinusReleased(CCObject * obj)
{
	unschedule(schedule_selector(DyLayerShopItem::cbPlusMinus));
	unschedule(schedule_selector(DyLayerShopItem::cbPlusMinusReal));
}


void DyLayerShopItem::onPlusClicked(CCObject * obj)
{
	YtLib* lib = YtLib::sharedLib();

	DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
	CCMenuItemImage *menuitem = dynamic_cast<CCMenuItemImage*>(obj);
	DyItemManager* itemManager = DyItemManager::sharedItemManager();

	int index = (menuitem->getTag()-SHOPITEM_UI_TAG_GROUP_HINT)/1000;

	DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(index);

//	m_count[index]++;
//
//	if(m_count[index]*item->m_price_gold > moneyManager->getGold())
//	{
//		m_count[index]--;
//		if(m_count[index] == 0)
//			moneyManager->alramNotEnoughGold(true);
//	}
//
    
    m_count[index] += CHANGE_UNIT;
    if(m_count[index]*item->m_price_gold > moneyManager->getGold())
	{
        m_count[index] -= CHANGE_UNIT;
        
        if(moneyManager->getGold() < item->m_price_gold)
        {
            DyMoneyManager::sharedMoneyManager()->alramNotEnoughGold(true, true);
        }
        else if((moneyManager->getGold()/item->m_price_gold)%10 != 0)
        {
//            DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyMoneyManager::onPopupClicked));
//            popup->setText(STRING_ITEM_UNIT);
//            
//            popup->addButton(DyButtonType_Blue, "NO", 30);
//            popup->addButton(DyButtonType_Red, "YES", 31);
//            popup->addButton(DyButtonType_Close, "", 30);
//            
//            YtLayerManager::sharedLayerManager()->pushLayer(popup, true);
            moneyManager->alramNotEnoughGold(true, true);
        }
        return;
	}

    
	CCLayer* layerGroup = (CCLayer*)this->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*index);
	CCLabelTTF* labelCount = (CCLabelTTF*)layerGroup->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*index + SHOPITEM_UI_TAG_SUB_COUNT);
	sprintf(lib->m_strTemp, "%d", m_count[index]);
	labelCount->setString(lib->m_strTemp);

	CCLabelTTF* labelPrice = (CCLabelTTF*)layerGroup->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*index + SHOPITEM_UI_TAG_SUB_PRICE);
	YtLib::setMoneyStyle(lib->m_strTemp, item->m_price_gold*m_count[index]);
	labelPrice->setString(lib->m_strTemp);
}


void DyLayerShopItem::onMinusClicked(CCObject * obj)
{
	YtLib* lib = YtLib::sharedLib();

	CCMenuItemImage *menuitem = dynamic_cast<CCMenuItemImage*>(obj);
	DyItemManager* itemManager = DyItemManager::sharedItemManager();

	int index = (menuitem->getTag()-SHOPITEM_UI_TAG_GROUP_HINT)/1000;

	DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(index);

	m_count[index] -= CHANGE_UNIT;

	if(m_count[index] < 0)
		m_count[index] = 0;

	CCLayer* layerGroup = (CCLayer*)this->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*index);
	CCLabelTTF* labelCount = (CCLabelTTF*)layerGroup->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*index + SHOPITEM_UI_TAG_SUB_COUNT);
	sprintf(lib->m_strTemp, "%d", m_count[index]);
	labelCount->setString(lib->m_strTemp);

	CCLabelTTF* labelPrice = (CCLabelTTF*)layerGroup->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*index + SHOPITEM_UI_TAG_SUB_PRICE);
	YtLib::setMoneyStyle(lib->m_strTemp, item->m_price_gold*m_count[index]);
	labelPrice->setString(lib->m_strTemp);
}



void DyLayerShopItem::onPopupClicked(cocos2d::CCObject * pSender)
{	DyLib* lib = DyLib::sharedLib();
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);

	DyPopup* popup = NULL;

	if(pPopup->m_active_tag == 1)
	{
        YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_MSG);
        
		DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
		DyItemManager* itemManager = DyItemManager::sharedItemManager();

		DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(m_itemBuyProcess);

		popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShopItem::onPopupClicked));

        
        if(item->m_cntHas + m_count[m_itemBuyProcess] > DYITEM_MAX_CNT)
        {
            m_count[m_itemBuyProcess] = DYITEM_MAX_CNT - item->m_cntHas;
        }
        
		moneyManager->addGold(-m_count[m_itemBuyProcess]*item->m_price_gold, true);
		moneyManager->dataSave();

		itemManager->addItem(item->m_itemtype, m_count[m_itemBuyProcess]);

		sprintf(lib->m_strTemp3, STRING_item_buy_result, item->m_name.c_str(), m_count[m_itemBuyProcess]);

		sprintf(lib->m_strTemp4, STRING_item_buy_result2, item->m_name.c_str(), item->m_cntHas);

		sprintf(lib->m_strTemp, "%s\n%s", lib->m_strTemp3, lib->m_strTemp4);

		popup->setText(lib->m_strTemp);
		popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 10);

		itemManager->dataSave();
	}
	else if(pPopup->m_active_tag == 10) // exchange ok
	{
	}
	
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);

	if(popup)
		YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerShopItem::onBuyClicked(CCObject * obj)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	CCMenuItemImage *menuitem = dynamic_cast<CCMenuItemImage*>(obj);
	DyLib* lib = DyLib::sharedLib();
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
	int index = (menuitem->getTag()-SHOPITEM_UI_TAG_GROUP_HINT)/1000;
	DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(index);

    if(item->m_cntHas == DYITEM_MAX_CNT)
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
        popup->setText("100개 이상 보유하실 수 없습니다.");
        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);

    }
	else if(m_count[index] > 0)
	{
		DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShopItem::onPopupClicked));
	
		sprintf(lib->m_strTemp, STRING_item_question, item->m_name.c_str(), m_count[index]);
		YtLib::setMoneyStyle(lib->m_strTemp3, m_count[index]*item->m_price_gold);
		sprintf(lib->m_strTemp2, STRING_item_gold_price, lib->m_strTemp3);
		sprintf(lib->m_strTemp3, "%s\n%s", lib->m_strTemp, lib->m_strTemp2);

		popup->setText(lib->m_strTemp3);
		popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
		popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 1);
		popup->addButton(DyButtonType_Close, "", 2);

		YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);

		m_itemBuyProcess = index;
	}
}


SEL_MenuHandler DyLayerShopItem::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPlusClicked", DyLayerShopItem::onPlusClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onMinusClicked", DyLayerShopItem::onMinusClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onBuyClicked", DyLayerShopItem::onBuyClicked);

    return NULL;    
}

SEL_CCControlHandler DyLayerShopItem::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}

bool DyLayerShopItem::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}

void DyLayerShopItem::pause()
{
	if(m_menu[0])
	{
		for(int i = 0;i < DyItemType_Count;i++)
		{
			m_menu[i]->setEnabled(false);
		}
	}
}


void DyLayerShopItem::resume()
{  
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
	DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();

	YtLib* lib = YtLib::sharedLib();

	if(m_menu[0])
	{
		for(int i = 0;i < DyItemType_Count;i++)
		{
			m_menu[i]->setEnabled(true);

			DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(i);

			CCLayer* layerGroup = (CCLayer*)this->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*i);

            if(m_count[i]*item->m_price_gold > moneyManager->getGold())
            {
                if(item->m_price_gold*CHANGE_UNIT < moneyManager->getGold())
                    m_count[i] = CHANGE_UNIT;
                else
                    m_count[i] = 0;
            }

			CCLabelTTF* labelCount = (CCLabelTTF*)layerGroup->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*i + SHOPITEM_UI_TAG_SUB_COUNT);
			sprintf(lib->m_strTemp, "%d", m_count[i]);
			labelCount->setString(lib->m_strTemp);
			labelCount->setColor(ccBLACK);

			CCLabelTTF* labelPrice = (CCLabelTTF*)layerGroup->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*i + SHOPITEM_UI_TAG_SUB_PRICE);
			YtLib::setMoneyStyle(lib->m_strTemp, item->m_price_gold*m_count[i]);
			labelPrice->setString(lib->m_strTemp);
			labelPrice->setColor(ccBLACK);

			CCLabelTTF* labelRemainCount = (CCLabelTTF*)layerGroup->getChildByTag(SHOPITEM_UI_TAG_GROUP_HINT + 1000*i + SHOPITEM_UI_TAG_SUB_REMAIN_COUNT);
			labelRemainCount->setFontName(FONT_NAME_DEFAULT);
			labelRemainCount->setColor(ccc3(204, 204, 204));
			sprintf(lib->m_strTemp, "%d", item->m_cntHas);
			labelRemainCount->setString(lib->m_strTemp);
		}
	}
}


void DyLayerShopItem::hide()
{
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}

void DyLayerShopItem::show()
{
}
