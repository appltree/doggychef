/*
DyLayerGameStartItem.cpp
Doggy chef
@initialize at 121128

@history
121128	yoonsr	initialize
*/


#include "DyConfig.h"
#include "DyLayerGameStartItem.h"
#include "DyLib.h"
#include "DyTycoon.h"
#include "DyStage.h"
#include "DyLayerShop.h"
#include "CCextAnimatePacker.h"
#include "DyPopup.h"
#include "DyTutorial.h"

DyLayerGameStartItem::DyLayerGameStartItem()
{
	memset(m_sprSlotItem, 0, sizeof(CCSprite*)*DYITEM_SLOT_CNT);
	memset(m_sprInvenItem, 0, sizeof(CCSprite*)*DyItemType_Count);
}

DyLayerGameStartItem::~DyLayerGameStartItem()
{	
}


void DyLayerGameStartItem::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
	DyLib* lib = DyLib::sharedLib();

	//DyItemManager* itemManager = DyItemManager::sharedItemManager();

	m_menu = ((CCMenu*)getChildByTag(GAMESTARTITEM_UI_TAG_MENU));
	
	for(int i = 0;i < DYITEM_SLOT_CNT;i++)
		m_menuItemSlot[i] = (CCMenuItem*)m_menu->getChildByTag(GAMESTARTITEM_UI_TAG_MENUITEM_USE_SLOT0 + i);
		
	for(int i = 0;i < DyItemType_Count;i++)
		m_menuItemInven[i] = (CCMenuItem*)m_menu->getChildByTag(GAMESTARTITEM_UI_TAG_MENUITEM_INVEN_SLOT0 + i);


	CCScale9Sprite* sprBox = (CCScale9Sprite*)getChildByTag(GAMESTARTITEM_UI_TAG_DESCRIPTION_BOX);

	CCLabelTTF* labelTip = CCLabelTTF::create(STRING_item_slot_explain, FONT_NAME_EXP, 25.f);
	labelTip->setColor(ccWHITE);
	labelTip->setPosition(ccp(sprBox->getContentSize().width/2, sprBox->getContentSize().height/2));
	labelTip->setTag(GAMESTARTITEM_UI_TAG_DESCRIPTION_TEXT);
	sprBox->addChild(labelTip);
	updateItemStatus();

	CCMenuItemImage* menuitemDone = (CCMenuItemImage*)m_menu->getChildByTag(GAMESTARTITEM_UI_TAG_MENUITEM_DONE);
	DyLib::setMenuItemText_kor(menuitemDone, DyButtonType_Red, STRING_POPUP_DONE);

	this->reorderChild(this->getChildByTag(GAMESTARTITEM_UI_TAG_ITEM_DISABLE_LOCK_0), 100);
	this->reorderChild(this->getChildByTag(GAMESTARTITEM_UI_TAG_ITEM_DISABLE_LOCK_1), 100);

	DyItemManager* itemManager = DyItemManager::sharedItemManager();
    
    //
    // 캔디 제약
	if(itemManager->isAvailable(itemManager->getItem(DyItemType_Candy)))
	{
		getChildByTag(GAMESTARTITEM_UI_TAG_ITEM_DISABLE_BG_0)->setVisible(false);
		getChildByTag(GAMESTARTITEM_UI_TAG_ITEM_DISABLE_LOCK_0)->setVisible(false);
	}
	else
	{
		sprintf(lib->m_strTemp, "%d", itemManager->getItem(DyItemType_Candy)->m_stage);

		CCLabelTTF* labelCount =  CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 15.f);
		labelCount->setColor(ccc3(97,90,61));
		getChildByTag(GAMESTARTITEM_UI_TAG_ITEM_DISABLE_LOCK_0)->addChild(labelCount);
		labelCount->setPosition(ccp(12, 9));
	}

    //
    // 폭탄 제약
	if(itemManager->isAvailable(itemManager->getItem(DyItemType_Bomb)))
	{
		getChildByTag(GAMESTARTITEM_UI_TAG_ITEM_DISABLE_BG_1)->setVisible(false);
		getChildByTag(GAMESTARTITEM_UI_TAG_ITEM_DISABLE_LOCK_1)->setVisible(false);
	}
	else
	{
		sprintf(lib->m_strTemp, "%d", itemManager->getItem(DyItemType_Bomb)->m_stage);

		CCLabelTTF* labelCount =  CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 15.f);
		labelCount->setColor(ccc3(97,90,61));
		getChildByTag(GAMESTARTITEM_UI_TAG_ITEM_DISABLE_LOCK_1)->addChild(labelCount);
		labelCount->setPosition(ccp(12, 9));
	}
}


SEL_MenuHandler DyLayerGameStartItem::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onDoneClicked", DyLayerGameStartItem::onDoneClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onShopClicked", DyLayerGameStartItem::onShopClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onSlotClicked", DyLayerGameStartItem::onSlotClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onInvenClicked", DyLayerGameStartItem::onInvenClicked);
    return NULL;    
}


SEL_CCControlHandler DyLayerGameStartItem::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}

bool DyLayerGameStartItem::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerGameStartItem::onSlotClicked(CCObject* pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    
    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
    if(tutorial && tutorial->m_command_type == DyTutorialCommandType_TouchQuestDetail)
    {
        return;
    }

    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
	DyItemManager* itemManager = DyItemManager::sharedItemManager();

	CCMenuItemImage* pMenu = dynamic_cast<CCMenuItemImage*>(pSender);

	int indexItem = pMenu->getTag()-GAMESTARTITEM_UI_TAG_MENUITEM_USE_SLOT0;

	if(itemManager->m_itemslot[indexItem])
	{
		itemManager->m_itemslot[indexItem] = NULL;
	}

	updateItemStatus();
}


void DyLayerGameStartItem::onInvenClicked(CCObject* pSender)
{
    if(DyTycoonManager::sharedTycoonManager()->m_lockInput)
        return;
    
    
    DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
    
    if(tutorial && tutorial->m_command_type == DyTutorialCommandType_TouchQuestDetail)
    {
        return;
    }
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
	DyItemManager* itemManager = DyItemManager::sharedItemManager();

	CCMenuItemImage* pMenu = dynamic_cast<CCMenuItemImage*>(pSender);

	int indexItem = pMenu->getTag()-GAMESTARTITEM_UI_TAG_MENUITEM_INVEN_SLOT0;
	DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(indexItem);
	
	for(int i = 0;i < DyItemType_Count;i++)
	{
		m_menuItemInven[i]->unselected();
	}

	m_menuItemInven[indexItem]->selected();

	CCScale9Sprite* sprBox = (CCScale9Sprite*)getChildByTag(GAMESTARTITEM_UI_TAG_DESCRIPTION_BOX);
	sprBox->removeChildByTag(GAMESTARTITEM_UI_TAG_DESCRIPTION_TEXT, true);
	sprBox->removeChildByTag(GAMESTARTITEM_UI_TAG_DESCRIPTION_TEXT+1, true);
	
	CCLabelTTF* labelName = CCLabelTTF::create(item->m_name.c_str(), FONT_NAME_EXP, 25.f);
	labelName->setColor(ccc3(255, 204, 1));
	//labelTip->setPosition(ccp(sprBox->getContentSize().width/2, sprBox->getContentSize().height/2));
	labelName->setTag(GAMESTARTITEM_UI_TAG_DESCRIPTION_TEXT);
	sprBox->addChild(labelName);
	float sizeWidthName = labelName->getContentSize().width;
    
	CCLabelTTF* labelExp = CCLabelTTF::create(item->m_explain_kor.c_str(), FONT_NAME_EXP, 25.f);
	labelExp->setColor(ccWHITE);
	//labelTip->setPosition(ccp(sprBox->getContentSize().width/2, sprBox->getContentSize().height/2));
	labelExp->setTag(GAMESTARTITEM_UI_TAG_DESCRIPTION_TEXT+1);
	sprBox->addChild(labelExp);
	float sizeWidthExp = labelExp->getContentSize().width;
	float sizeWidthGap = 10.f;
	CCPoint posExpCenter = ccp(sprBox->getContentSize().width/2, sprBox->getContentSize().height/2);

	labelName->setPosition(ccp(posExpCenter.x - (sizeWidthName+sizeWidthGap+sizeWidthExp)/2, posExpCenter.y));
	labelName->setAnchorPoint(ccp(0, 0.5));
	labelExp->setPosition(ccp(posExpCenter.x - (sizeWidthName+sizeWidthGap+sizeWidthExp)/2 + sizeWidthName+sizeWidthGap, posExpCenter.y));
	labelExp->setAnchorPoint(ccp(0, 0.5));

	if(itemManager->isAvailable(item))
	{
		for(int i = 0;i < DYITEM_SLOT_CNT;i++)
		{
			if(itemManager->m_itemslot[i] == item)
			{
				return;
			}
		}

		for(int i = 0;i < DYITEM_SLOT_CNT;i++)
		{
			if(itemManager->m_itemslot[i] == NULL)
			{
				itemManager->m_itemslot[i] = item;
				break;
			}
		}
	}
	else
	{
		DyLib* lib = DyLib::sharedLib();

		sprintf(lib->m_strTemp, STRING_item_stage_notuse, item->m_name.c_str(), item->m_stage);

		DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);

		popup->setText(lib->m_strTemp);
		popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);

		YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
	}

	updateItemStatus();
}


void DyLayerGameStartItem::onDoneClicked(CCObject* pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
	DyItemManager* itemManager = DyItemManager::sharedItemManager();
	itemManager->dataSave();
	YtLayerManager::sharedLayerManager()->popLayer();
}


void DyLayerGameStartItem::onShopClicked(CCObject* pSender)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
	DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Normal, 4, 0);
	YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
}


void DyLayerGameStartItem::updateItemStatus()
{
	//YtLib* lib = YtLib::sharedLib();
	DyItemManager* itemManager = DyItemManager::sharedItemManager();

    //
    // 상단 슬롯
	for(int i = 0;i < DYITEM_SLOT_CNT;i++)
	{
		if(m_sprSlotItem[i])
		{
			m_sprSlotItem[i]->removeFromParentAndCleanup(true);
			m_sprSlotItem[i] = NULL;
		}

		DyItem* item = itemManager->m_itemslot[i];

		if(item)
		{
			m_sprSlotItem[i] = itemManager->createItemIcon(item, true, true);
			m_sprSlotItem[i]->setPosition(m_menuItemSlot[i]->getPosition());
			addChild(m_sprSlotItem[i]); 

			CCSprite* sprRemoveMark = CCSprite::createWithSpriteFrameName("menu_ui_item_slot_remove.png");
			sprRemoveMark->setPosition(ccp(55, 55));
			m_sprSlotItem[i]->addChild(sprRemoveMark);
		}
	}
	
    //
    // 하단 인벤
	for(int i = 0;i < DyItemType_Count;i++)
	{
		bool inSlot = false;
		DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(i);

		if(m_sprInvenItem[i])
		{
			m_sprInvenItem[i]->removeFromParentAndCleanup(true);
			m_sprInvenItem[i] = NULL;
		}

		for(int j = 0;j < DYITEM_SLOT_CNT;j++)
		{
			if(item == itemManager->m_itemslot[j])
			{
				inSlot = true;
			}
		}

		if(itemManager->isAvailable(item))
		{
			m_sprInvenItem[i] = itemManager->createItemIcon(item, !inSlot, false);
		}
		else
			m_sprInvenItem[i] = itemManager->createItemIcon(item, false, false);
		
		m_sprInvenItem[i]->setPosition(m_menuItemInven[i]->getPosition());
		addChild(m_sprInvenItem[i]);
	}
}



void DyLayerGameStartItem::pause()
{
	m_menu->setEnabled(false);
}


void DyLayerGameStartItem::resume()
{
	m_menu->setEnabled(true);
	updateItemStatus();
}


void DyLayerGameStartItem::hide()
{
	YtLib* lib = YtLib::sharedLib();
    
	runAction(CCSequence::create(
                                 
                                 CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)),
                                 CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
                                 CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height)),
                                 
                                 CCCallFunc::create(this, callfunc_selector(DyLayerGameStartItem::destory)),
                                 NULL));

}


void DyLayerGameStartItem::show()
{
    YtLib* lib = YtLib::sharedLib();
    
  	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));
    
	runAction(CCSequence::create(
                                 
                                 CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
                                 CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
                                 CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
                                 NULL));

}


DyLayerGameStartItem* DyLayerGameStartItem::create(CCNode* owner)
{
	return (DyLayerGameStartItem*)YtLayerManager::create("DyLayerGameStartItem", DyLayerGameStartItemLoader::loader(), "layer_game_start_item.ccbi", owner);
}

