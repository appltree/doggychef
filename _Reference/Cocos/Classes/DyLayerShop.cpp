/*
DyLayerShop.cpp
Doggy chef
@initialize at 121207

@history
121207	yoonsr	initialize
*/


#include "DyLayerShop.h"
#include "DyLayerShopAwards.h"
#include "DyLayerShopCostume.h"
#include "DyLayerShopDelly.h"
#include "DyLayerShopExchange.h"
#include "DyLayerShopGum.h"
#include "DyLayerShopItem.h"
#include "DyLayerShopRecipe.h"
#include "DyLayerShopStaffList.h"



#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyBlockInfo.h"
#include "DyPopup.h"


DyLayerShopTab::DyLayerShopTab(DyLayerShopType shopType, CCRect rect):CCTabView(rect)
{
	switch(shopType)
	{
		case DyLayerShopType_Normal:
			m_leftMargin = 40.0;
			m_midPad = 1;//5.0;
			break;
		case DyLayerShopType_Myroom:
//			m_leftMargin = 150.0;
//			m_midPad = 100;//5.0;
            
            m_leftMargin = 70.0;
			m_midPad = 75;//5.0;
			break;
	}
	m_upMargin = 43.0;
	m_selectedTab = 0;
}


DyLayerShop::DyLayerShop()
{
	m_tag = DYLAYERSHOP_TAG;

	m_callShop = -1;
	m_startTab = 0;
	m_startSubTab = 0;

	for(int i = 0;i < DYLAYERSHOP_MAX_TAB;i++)
	{
		m_layerContent[i] = NULL;
	}
    
    m_callShop_param = 0;
}


DyLayerShop::~DyLayerShop()
{
	DyMoneyManager::sharedMoneyManager()->removeMoneyLayer(m_layerMoney);

	m_layerMoney->release();
}


void DyLayerShop::onEnter()
{
	CCLayer::onEnter();

	if(m_isInit)
		return;
	m_isInit = true;
	
	CCLayerColor* layer = (CCLayerColor*)getChildByTag(SHOP_UI_TAG_CONTENT);
	CCSize size = layer->getContentSize();

	m_menu = (CCMenu*)this->getChildByTag(SHOP_UI_TAG_MENU);
	switch(m_shopType)
	{
		case DyLayerShopType_Normal:
			{
				m_menu->getChildByTag(SHOP_UI_TAG_MENUITEM_GOSHOP)->setVisible(false);

				m_tabView = new DyLayerShopTab(DyLayerShopType_Normal, CCRectMake(0, 0, size.width, size.height));
				m_tabView->m_selectedTab = m_startTab;

				m_layerContent[0] = (YtLayer *)YtLayerManager::create("DyLayerShopDelly", DyLayerShopDellyLoader::loader(), "layer_shop_delly.ccbi", m_tabView);
				m_layerContent[1] = (YtLayer *)YtLayerManager::create("DyLayerShopGum", DyLayerShopGumLoader::loader(), "layer_shop_gum.ccbi", m_tabView);
				m_layerContent[2] = (YtLayer *)YtLayerManager::create("DyLayerShopCostume", DyLayerShopCostumeLoader::loader(), "layer_shop_costume.ccbi", m_tabView);
				((DyLayerShopCostume*)m_layerContent[2])->m_shopType = DyLayerShopType_Normal;

				m_layerContent[3] = (YtLayer *)YtLayerManager::create("DyLayerShopRecipe", DyLayerShopRecipeLoader::loader(), "layer_shop_recipe.ccbi", m_tabView);
				((DyLayerShopRecipe*)m_layerContent[3])->m_shopType = DyLayerShopType_Normal;

				m_layerContent[4] = (YtLayer *)YtLayerManager::create("DyLayerShopItem", DyLayerShopItemLoader::loader(), "layer_shop_item.ccbi", m_tabView);
				m_layerContent[5] = (YtLayer *)YtLayerManager::create("DyLayerShopExchange", DyLayerShopExchangeLoader::loader(), "layer_shop_exchange.ccbi", m_tabView);


				switch(m_startTab)
				{
					case 2:
						((DyLayerShopCostume*)m_layerContent[2])->m_startTab = m_startSubTab;
						break;
					case 3:
						((DyLayerShopRecipe*)m_layerContent[3])->m_startTab = m_startSubTab;
						break;
				}


				const char* tab_resource_name[DYLAYERSHOP_MAX_TAB*3] =
				{
					"menushop_ui_tab_delly.png",
					"menushop_ui_tab_delly_sel.png",
					"menushop_ui_tab_delly_highlight.png",

					"menushop_ui_tab_gum.png",
					"menushop_ui_tab_gum_sel.png",
					"menushop_ui_tab_gum_highlight.png",

					"menushop_ui_tab_costume.png",
					"menushop_ui_tab_costume_sel.png",
					"menushop_ui_tab_costume_highlight.png",

					"menushop_ui_tab_recipe.png",
					"menushop_ui_tab_recipe_sel.png",
					"menushop_ui_tab_recipe_highlight.png",

					"menushop_ui_tab_item.png",
					"menushop_ui_tab_item_sel.png",
					"menushop_ui_tab_item_highlight.png",

					"menushop_ui_tab_exchange.png",
					"menushop_ui_tab_exchange_sel.png",
					"menushop_ui_tab_exchange_highlight.png",
				};

				for(int i = 0;i < DYLAYERSHOP_MAX_TAB;i++)
				{
					m_tabView->addTab(CCSprite::createWithSpriteFrameName(tab_resource_name[i*3]),
						CCSprite::createWithSpriteFrameName(tab_resource_name[i*3+1]),
						CCSprite::createWithSpriteFrameName(tab_resource_name[i*3+2]), 
						m_layerContent[i]);
				}
			}

			break;
		case DyLayerShopType_Myroom:
			{
				CCSprite* sprTitle = (CCSprite*)getChildByTag(SHOP_UI_TAG_TITLE);
				sprTitle->setDisplayFrame(CCSprite::createWithSpriteFrameName("menushop_ui_myroom_title.png")->displayFrame());

				m_tabView = new DyLayerShopTab(DyLayerShopType_Myroom, CCRectMake(0, 0, size.width, size.height));
				m_tabView->m_selectedTab = m_startTab;

				DyLayerShopCostume* layerShopCostume;
				DyLayerShopRecipe* layerShopRecipe;
                DyLayerShopStaffList* layerShopStaffList;
               DyLayerShopAwards* layerShopAwards;

				layerShopCostume = (DyLayerShopCostume*)YtLayerManager::create("DyLayerShopCostume", DyLayerShopCostumeLoader::loader(), "layer_shop_costume.ccbi", m_tabView);
				layerShopCostume->m_shopType = DyLayerShopType_Myroom;
				layerShopCostume->m_actionManager->runAnimations("myroom");

				layerShopRecipe = (DyLayerShopRecipe*)YtLayerManager::create("DyLayerShopRecipe", DyLayerShopRecipeLoader::loader(), "layer_shop_recipe.ccbi", m_tabView);
				layerShopRecipe->m_shopType = DyLayerShopType_Myroom;

                layerShopStaffList = (DyLayerShopStaffList*)YtLayerManager::create("DyLayerShopStaff", DyLayerShopStaffListLoader::loader(), "layer_shop_staff.ccbi", m_tabView);
                
                layerShopAwards = (DyLayerShopAwards*)YtLayerManager::create("DyLayerShopAwards", DyLayerShopAwardsLoader::loader(), "layer_shop_awards.ccbi", m_tabView);

				m_layerContent[0] = (YtLayer *)layerShopCostume;
				m_layerContent[1] = (YtLayer *)layerShopRecipe;
                m_layerContent[2] = (YtLayer *)layerShopStaffList;
				m_layerContent[3] = (YtLayer *)layerShopAwards;

				switch(m_startTab)
				{
					case 0:
						layerShopCostume->m_startTab = m_startSubTab;
						break;
					case 1:
						layerShopRecipe->m_startTab = m_startSubTab;
						break;
                    case 2:
                        layerShopStaffList->m_startIndex = m_startSubTab;
                        break;
					case 3:
						layerShopAwards->m_startTab = m_startSubTab;
                        
                        ((DyLayerShopCostume*)m_layerContent[2])->m_startTab = m_startSubTab;
						
                        
						break;
				}

				const char* tab_resource_name[3*4] =
				{
					"menushop_ui_tab_costume.png",
					"menushop_ui_tab_costume_sel.png",
					"menushop_ui_tab_costume_highlight.png",

					"menushop_ui_tab_recipe.png",
					"menushop_ui_tab_recipe_sel.png",
					"menushop_ui_tab_recipe_highlight.png",

                    "menushop_ui_tab_staff.png",
					"menushop_ui_tab_staff_sel.png",
					"menushop_ui_tab_staff_highlight.png",

                    
					"menushop_ui_tab_award.png",
					"menushop_ui_tab_award_sel.png",
					"menushop_ui_tab_award_highlight.png",
				};

				for(int i = 0;i < 4;i++)
				{
					m_tabView->addTab(CCSprite::createWithSpriteFrameName(tab_resource_name[i*3]),
						CCSprite::createWithSpriteFrameName(tab_resource_name[i*3+1]),
						CCSprite::createWithSpriteFrameName(tab_resource_name[i*3+2]), 
						m_layerContent[i]);
				}

			}
			break;
	}

	m_tabIndex = m_tabView->m_selectedTab;
	m_tabView->m_delegate = this;
	layer->addChild(m_tabView);
	m_tabView->release();

	m_layerMoney = new DyLayerMoney(DyLayerMoneyType_Normal, 0);
	m_layerMoney->m_delegate = this;

	m_layerMoney->setPosition(ccp(-410.f, 240.f));
	addChild(m_layerMoney);
	DyMoneyManager::sharedMoneyManager()->addMoneyLayer(m_layerMoney);

	resume();
    
    setKeypadEnabled(true);
    
    
//    extern int g_platform_notProcessDelly;
//    if(g_platform_notProcessDelly > 0)
//    {
//        schedule(schedule_selector(DyLayerShop::showDelly), 0.1f);
//    }
}



//void DyLayerShop::showDelly(float dt)
//{
//    extern int g_platform_notProcessDelly;
//    
//    unschedule(schedule_selector(DyLayerShop::showDelly));
//    
//    DyLib* lib = DyLib::sharedLib();
//    
//    DyMoneyManager::sharedMoneyManager()->addDelly(g_platform_notProcessDelly, true);
//    
//    DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
//    
//    sprintf(lib->m_strTemp, "%d델리 구매한것이 지금반영되었습니다.", g_platform_notProcessDelly);
//    popup->setText(lib->m_strTemp);
//    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
//    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
//    
//    g_platform_notProcessDelly = 0;
//}
//

void DyLayerShop::onExit()
{
	CCLayer::onExit();
}


void DyLayerShop::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
}


SEL_MenuHandler DyLayerShop::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCloseClicked", DyLayerShop::onCloseClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onShopClicked", DyLayerShop::onShopClicked);
    return NULL;    
}

SEL_CCControlHandler DyLayerShop::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}

bool DyLayerShop::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}




void DyLayerShop::callfunc_update_me_success(float dt)
{
    unschedule(schedule_selector(DyLayerShop::callfunc_update_me_success));

    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    YtLayerManager::sharedLayerManager()->popLayer();
}


void DyLayerShop::onPopupClicked(cocos2d::CCObject * pSender)
{
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    
    if(pPopup->m_active_tag == 1000)
    {
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
    }
}



void DyLayerShop::callfunc_update_me_fail(float dt)
{
    unschedule(schedule_selector(DyLayerShop::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShop::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}


void DyLayerShop_callfunc_update_me(int value, int code)
{
    DyLayerShop *layerShop = (DyLayerShop*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERSHOP_TAG);
    
    if(value == 0)
    {
        layerShop->schedule(schedule_selector(DyLayerShop::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layerShop->schedule(schedule_selector(DyLayerShop::callfunc_update_me_fail), 0.05f);
    }
}



void DyLayerShop::onCloseClicked(cocos2d::CCObject * pSender)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLOSE);
    
    
    lib->network_kakao_update_me(0, DyLayerShop_callfunc_update_me);
    
}


void DyLayerShop::keyBackClicked()
{
    if(DyLib::sharedLib()->m_showLoadingPopup)
        return;

    setKeypadEnabled(false);
    onCloseClicked(NULL);
}


void DyLayerShop::buttonClicked(DyLayerMoney* moneyView, int index)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	if(m_shopType == DyLayerShopType_Myroom)
	{
		switch(index)
		{
			case 0:
                m_callShop = 1;
                YtLayerManager::sharedLayerManager()->popLayer();
				break;
			case 1:
                m_callShop = 0;
                YtLayerManager::sharedLayerManager()->popLayer();
				break;
			case 2:
                m_callShop = 5;
                YtLayerManager::sharedLayerManager()->popLayer();
				break;
		}
	}
	else
	{
		switch(index)
		{
			case 0:
				m_tabView->goTab(1, false);
				break;
			case 1:
				m_tabView->goTab(0, false);
				break;
			case 2:
				m_tabView->goTab(5, false);
				break;
		}
	}
}

void DyLayerShop::tabDidChange(CCTabView* tabView, int index, CCLayer* contentLayer, bool init,  bool redo)
{
    if(!init && m_shopType == DyLayerShopType_Myroom && index == 2)
    {
        DyLayerShopStaffList* layerShopStaffList = (DyLayerShopStaffList*)m_layerContent[2];
        layerShopStaffList->showList();
    }
    else if(redo)
        return;
    
    if(!init)
        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
  	m_tabIndex = index;

	if(m_shopType == DyLayerShopType_Myroom)
	{
		CCMenuItemImage* pMenuShop = (CCMenuItemImage*)m_menu->getChildByTag(SHOP_UI_TAG_MENUITEM_GOSHOP);
		switch(index)
		{
			case 0:
			case 1:
				pMenuShop->setVisible(true);
				//pMenuShop->setNormalSpriteFrame(CCSprite::createWithSpriteFrameName("menushop_ui_myroom_more.png")->displayFrame());
				//pMenuShop->setSelectedSpriteFrame(CCSprite::createWithSpriteFrameName("menushop_ui_myroom_more_sel.png")->displayFrame());
				break;
			case 2:
            case 3:
				pMenuShop->setVisible(false);
				break;
		}
	}
}


void DyLayerShop::onShopClicked(cocos2d::CCObject * pSender)
{
	switch(m_tabIndex)
	{
	case 0:
		{
            YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
			m_callShop = 2;//코스튬
			YtLayerManager::sharedLayerManager()->popLayer();

		}
		break;
	case 1:
		{
            
            DyLayerShopRecipe* layerShopRecipe = (DyLayerShopRecipe*)m_layerContent[1];
            
            YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
			m_callShop = 3;//레시피
            m_callShop_param = layerShopRecipe->m_tabIndex;
			YtLayerManager::sharedLayerManager()->popLayer();
		}
		break;
	}
}

void DyLayerShop::pause()
{
	m_tabView->setEnable(false);
	m_menu->setEnabled(false);
	//for(int i = 0;i < DYLAYERSHOP_MAX_TAB;i++)
	{
		if(m_layerContent[m_tabIndex])
			m_layerContent[m_tabIndex]->pause();
	}
	m_layerMoney->pause();
    setKeypadEnabled(false);
}


void DyLayerShop::resume()
{
	//YtLib* lib = YtLib::sharedLib();
	//DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();

	m_tabView->setEnable(true);
	m_menu->setEnabled(true);
	//for(int i = 0;i < DYLAYERSHOP_MAX_TAB;i++)
	{
		if(m_layerContent[m_tabIndex])
			m_layerContent[m_tabIndex]->resume();
	}

	m_layerMoney->resume();
    
    setKeypadEnabled(true);
}


void DyLayerShop::destory()
{
	//int callShop = m_callShop;

	YtLayer::destory();

	if(m_callShop >= 0 && m_callShop < 10)
	{
      	DyLayerShop* layerShop = DyLayerShop::create(this, DyLayerShopType_Normal, m_callShop, m_callShop_param);
		YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
	}
}


void DyLayerShop::hide()
{
	YtLib* lib = YtLib::sharedLib();

	runAction(CCSequence::create(

		CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)), 
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height)),

		CCCallFunc::create(this, callfunc_selector(DyLayerShop::destory)),
		NULL));
}


void DyLayerShop::show()
{
	YtLib* lib = YtLib::sharedLib();

	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));

	runAction(CCSequence::create(

		CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
		NULL));
}

DyLayerShop* DyLayerShop::create(CCNode* owner, DyLayerShopType shopType, int startTab, int startSubTab)
{
	DyLayerShop* layerShop = (DyLayerShop*)YtLayerManager::create(DYLAYERSHOP_TAG, DyLayerShopLoader::loader(), "layer_shop.ccbi", owner);

	layerShop->m_shopType = shopType;
	layerShop->m_startTab = startTab;
	layerShop->m_startSubTab = startSubTab;
	return layerShop;
}