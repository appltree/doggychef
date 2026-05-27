/*
DyMoney.cpp
Doggy chef
@initialize at 121127

@history
121127	yoonsr	initialize
*/


#include "DyMoney.h"

#include "DyLoader.h"
#include "DyData.h"
#include "DyLayerShop.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "DyNetwork.h"

DyMoneyManager* g_sharedMoneyManager;


DyMoneyManager::DyMoneyManager()
{
	YtObjectGroup* dellyInfo = DyLoader::sharedLoader()->loadObjects("delly.txt");

	m_arrDellyInfos = new CCArray();
	
	for(int i = 0;i < dellyInfo->count();i++)
	{
		DyDellyInfo* delly = new DyDellyInfo();

		delly->m_id = atoi(dellyInfo->getValue("id", i));
		delly->m_delly_cnt = atoi(dellyInfo->getValue("delly", i));
		delly->m_price_won = atoi(dellyInfo->getValue("price_won", i));
		delly->m_price_dollar = dellyInfo->getValue("price_dollar", i);
		delly->m_bonus = atoi(dellyInfo->getValue("bonus", i));

#ifdef ANDROID
        delly->m_product_id = dellyInfo->getValue("andorid_id", i);
        delly->m_present_id = dellyInfo->getValue("android_present_id", i);
#else
        delly->m_product_id = dellyInfo->getValue("ios_id", i);
#endif
        
		m_arrDellyInfos->addObject(delly);
		delly->release();
	}

	m_exchange_rate = 1000;

	YtObjectGroup* gumInfo = DyLoader::sharedLoader()->loadObjects("gum.txt");

	m_arrGumInfos = new CCArray();
	
	for(int i = 0;i < gumInfo->count();i++)
	{
		DyGumInfo* gum = new DyGumInfo();

		gum->m_id = atoi(gumInfo->getValue("id", i));
		gum->m_gum_cnt = atoi(gumInfo->getValue("gum", i));
		gum->m_price_delly = atoi(gumInfo->getValue("price_delly", i));
		gum->m_bonus = atoi(gumInfo->getValue("bonus", i));

		m_arrGumInfos->addObject(gum);
		gum->release();
	}

	m_arrMoneyLayer = new CCArray();

	CCDirector::sharedDirector()->getScheduler()->scheduleSelector(
                schedule_selector(DyMoneyManager::updateGum), this, 1.0f, false, kCCRepeatForever, 0);
    
    m_isDoCheckTimeForGum = false;

}


DyMoneyManager::~DyMoneyManager()
{
	m_arrDellyInfos->release();
	m_arrGumInfos->release();

	m_arrMoneyLayer->release();
}

void DyMoneyManager::updateMoneyLayer(bool isGumTime, int remainTime)
{
	for(unsigned int i = 0;i < m_arrMoneyLayer->count();i++)
	{
		DyLayerMoney* layer = (DyLayerMoney*)m_arrMoneyLayer->objectAtIndex(i);
		layer->updateStatus(isGumTime, remainTime);
	}
}

void DyMoneyManager::dataSave()
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	dataManager->dataSave();
}



void DyMoneyManager::applyDelly()
{
    updateMoneyLayer(false, 0);
}


void DyMoneyManager::applyGold()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
    updateMoneyLayer(false, 0);
    
	dataManager->checkAwards(DyAwardsCheckType_Gold, false);

}


void DyMoneyManager::applyGum()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
    if(dataManager->m_gum < dataManager->m_rechargeable_gum)
	{
		if(m_isDoCheckTimeForGum == false)
        {
			dataManager->m_heart_regen_starts_at = dataManager->getCurrentDate();
        }
		m_isDoCheckTimeForGum = true;
	}
	else
	{
        m_isDoCheckTimeForGum = false;
	}
	
	updateMoneyLayer(false, 0);
}


void DyMoneyManager::addDelly(int value, bool apply)
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
    SInt delly = SInt(dataManager->m_private.m_delly);
    
    delly.add(value);
    
    if(delly.get() >= MAX_INTNUM)
        delly.set(MAX_INTNUM);
    
    delly.store(dataManager->m_private.m_delly);
    
    if(apply)
        applyDelly();
}


void DyMoneyManager::addGold(int value, bool apply)
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    SInt gold = SInt(dataManager->m_private.m_gold);
    
    gold.add(value);
    
    if(gold.get() >= MAX_INTNUM)
        gold.set(MAX_INTNUM);
    
    gold.store(dataManager->m_private.m_gold);
    
    if(apply)
        applyGold();
}


void DyMoneyManager::addGum(int value, bool apply)
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	dataManager->m_gum += value;
    
    
    if(dataManager->m_gum > dataManager->m_max_gum)
        dataManager->m_gum = dataManager->m_max_gum;
        
    if(apply)
        applyGum();

}

int DyMoneyManager::getGold()
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	return SInt(dataManager->m_private.m_gold).get();
}


int DyMoneyManager::getDelly()
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	return SInt(dataManager->m_private.m_delly).get();
}


unsigned int DyMoneyManager::getGum()
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	return dataManager->m_gum;
}


void DyMoneyManager::updateGum(float dt)
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();

    if(m_isDoCheckTimeForGum)
	{
        CCDate dateCurrent = dataManager->getCurrentDate();
        
        CCDate diffDate = dateCurrent - dataManager->m_heart_regen_starts_at;

		if(diffDate.time() > dataManager->m_heart_regen_interval)
		{
			m_isDoCheckTimeForGum = false;
            int addedGum = diffDate.time()/dataManager->m_heart_regen_interval;
            CCDate remainDate = diffDate - CCDate(addedGum*dataManager->m_heart_regen_interval);
            
            int currentGum = getGum();
            
            if(addedGum+currentGum > dataManager->m_rechargeable_gum)
            {
                //
                // 처리 순서 중요함
                //dataManager->m_rechargedGum += (dataManager->m_rechargeable_gum-currentGum);
                addGum(dataManager->m_rechargeable_gum-currentGum, true);
            }
            else
            {
                //dataManager->m_rechargedGum += addedGum;
                
                addGum(addedGum, true);
                
                dataManager->m_heart_regen_starts_at = dataManager->getCurrentDate()-remainDate;
            }
		}
		else
			updateMoneyLayer(true, dataManager->m_heart_regen_interval - diffDate.time());
	}
}


void DyMoneyManager::removeMoneyLayer(DyLayerMoney* layer)
{
	m_arrMoneyLayer->removeObject(layer);
}

void DyMoneyManager::addMoneyLayer(DyLayerMoney* layer)
{
	m_arrMoneyLayer->addObject(layer);
}


void* DyMoneyManager::alramNotEnoughGold(bool showDirect, bool needPause)
{
	//DyLib* lib = DyLib::sharedLib();

	DyPopup* popup = NULL;
	popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyMoneyManager::onPopupClicked));
	popup->setText(STRING_notenough_gold);

	popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 30);
	popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 31);
	popup->addButton(DyButtonType_Close, "", 30);

	if(showDirect)
		YtLayerManager::sharedLayerManager()->pushLayer(popup, true, needPause);

	return popup;
}


void* DyMoneyManager::alramNotEnoughDelly(bool showDirect, bool needPause)
{
	//DyLib* lib = DyLib::sharedLib();

	DyPopup* popup = NULL;
	popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyMoneyManager::onPopupClicked));
	popup->setText(STRING_notenough_delly);

	popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 20);
	popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 21);
	popup->addButton(DyButtonType_Close, "", 20);

	if(showDirect)
		YtLayerManager::sharedLayerManager()->pushLayer(popup, true, needPause);

	return popup;
}



void DyMoneyManager::callfunc_update_me_success(float dt)
{
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getTopLayer();
    
    layer->unschedule(schedule_selector(DyMoneyManager::callfunc_update_me_success));
    
    lib->show_network_alaram(false);
   
    switch(moneyManager->m_processItemType)
    {
        case DyBuyItemType_Delly:
            moneyManager->applyDelly();
            break;
        case DyBuyItemType_Gum:
            moneyManager->applyGum();
            break;
        default:
            break;
    }       
    
    
    YtLayerManager::sharedLayerManager()->pushLayer(moneyManager->m_popup_process, true, true);
}


void DyMoneyManager::callfunc_update_me_fail(float dt)
{
    //
    // 예외적으로 무조건 성공 처리
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    
    DyLib* lib = DyLib::sharedLib();
    
    moneyManager->m_popup_process->release();
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getTopLayer();
    
    layer->unschedule(schedule_selector(DyMoneyManager::callfunc_update_me_fail));
    lib->show_network_alaram(false);
    
    lib->data_restore();
    
    switch(moneyManager->m_processItemType)
    {
        case DyBuyItemType_Delly:
            break;
        case DyBuyItemType_Gum:
        case DyBuyItemType_Recipe:
        {
            DyPopup* _popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyMoneyManager::onPopupClicked));
            _popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
            _popup->addButton(DyButtonType_Red, "OK", 1000);
            YtLayerManager::sharedLayerManager()->pushLayer(_popup, true, true);
        }
            break;
        default:
            break;
    }
}


void DyMoneyManager_callfunc_update_me(int value, int code)
{
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getTopLayer();
    
    if(value == 0)
    {
        layer->schedule(schedule_selector(DyMoneyManager::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layer->schedule(schedule_selector(DyMoneyManager::callfunc_update_me_fail), 0.05f);
    }
}


void DyMoneyManager::callfunc_iab_success(float dt)
{
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getTopLayer();
    
    layer->unschedule(schedule_selector(DyMoneyManager::callfunc_iab_success));
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_MSG);
    
    
    
    //lib->show_network_alaram(false);
    
    DyDellyInfo* delly = (DyDellyInfo*)moneyManager->m_processItem;

    moneyManager->addDelly(delly->m_delly_cnt, true);
    
    lib->data_backup();
    lib->network_kakao_update_me(0, DyMoneyManager_callfunc_update_me);
}


void DyMoneyManager::callfunc_iab_fail(float dt)
{
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    
    DyLib* lib = DyLib::sharedLib();
    
    moneyManager->m_popup_process->release();
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getTopLayer();
    
    layer->unschedule(schedule_selector(DyMoneyManager::callfunc_iab_fail));
    lib->show_network_alaram(false);
    
    //lib->data_restore();
    
    DyPopup* _popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyMoneyManager::onPopupClicked));
    _popup->setText(STRING_PURCHASE_FAILURE);
    _popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(_popup, true, true);
}



void DyMoneyManager_callfunc_iab(int value, int code)
{
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getTopLayer();
    
    if(value == 0)
    {
        layer->schedule(schedule_selector(DyMoneyManager::callfunc_iab_success), 0.05f);
    }
    else
    {
        layer->schedule(schedule_selector(DyMoneyManager::callfunc_iab_fail), 0.05f);
    }
}



void DyMoneyManager::onPopupClicked(cocos2d::CCObject * pSender)
{	
	DyLib* lib = DyLib::sharedLib();
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);

    m_popup_process = NULL;

	if(pPopup->m_active_tag == 1)
	{
		m_popup_process = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyMoneyManager::onPopupClicked));
		m_popup_process->setText(STRING_result_buy_success);
		m_popup_process->addButton(DyButtonType_Red, STRING_POPUP_OK, 10);

		switch(m_processItemType)
		{
//			case DyBuyItemType_Delly:
//				{
//                    
//					DyDellyInfo* delly = (DyDellyInfo*)m_processItem;
//
//                    //DyLib::sharedLib()->data_backup();
//                    
//					//addDelly(delly->m_delly_cnt, false);
//                    
//                    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
//                    
//                    
//                    lib->network_iab(DyMoneyManager_callfunc_iab, delly->m_product_id.c_str(), "");
//                    
//                    
//                    
//                    m_popup_process->retain();
//
//                    return;
//                    //dataSave();
//				}
//				break;
                
			case DyBuyItemType_Costume:
				{
					DyCostume* costume = (DyCostume*)m_processItem;

                    if(costume->m_price_gold > 0)
                    {
                        if(getGold() < costume->m_price_gold)
                        {
                            m_popup_process = (DyPopup*)alramNotEnoughGold(false, true);
                        }
                        else
                        {
                            DyCostumeManager::sharedCostumeManager()->buyedCostume(costume);
                            addGold(-costume->m_price_gold, true);
                            dataSave();
                        }
                    }
                    if(costume->m_price_delly > 0)
                    {
                        if(getDelly() < costume->m_price_delly)
                        {
                            m_popup_process = (DyPopup*)alramNotEnoughDelly(false, true);
                        }
                        else
                        {
                            DyCostumeManager::sharedCostumeManager()->buyedCostume(costume);
                            addDelly(-costume->m_price_delly, true);
                            dataSave();
                        }
                    }
                }
				break;
			case DyBuyItemType_CostumeArray:
				{
					int gold = 0;
                    int delly = 0;
					CCArray* costumeArray = (CCArray*)m_processItem;
					for(unsigned int i = 0;i < costumeArray->count();i++)
					{
						DyCostume* costume = (DyCostume*)costumeArray->objectAtIndex(i);
						gold += costume->m_price_gold;
                        delly += costume->m_price_delly;
					}
                    if(getDelly() < delly)
					{
						m_popup_process = (DyPopup*)alramNotEnoughDelly(false, true);
					}
					else if(getGold() < gold)
					{
						m_popup_process = (DyPopup*)alramNotEnoughGold(false, true);
					}
					else
					{
						for(unsigned int i = 0;i < costumeArray->count();i++)
						{
							DyCostume* costume = (DyCostume*)costumeArray->objectAtIndex(i);
							DyCostumeManager::sharedCostumeManager()->buyedCostume(costume);
						}						
						addGold(-gold, true);
                        addGold(-delly, true);
						dataSave();
					}
				}
				break;
			case DyBuyItemType_Gum:
				{
					DyGumInfo* gum = (DyGumInfo*)m_processItem;
					
					if(getDelly() < gum->m_price_delly)
					{
						m_popup_process = (DyPopup*)alramNotEnoughDelly(false, true);
					}
					else
					{
                        DyLib::sharedLib()->data_backup();
                        
						addDelly(-gum->m_price_delly, false);
						addGum(gum->m_gum_cnt, false);
                        
                        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
                        
                        lib->network_kakao_update_me(gum->m_gum_cnt, DyMoneyManager_callfunc_update_me);
                        m_popup_process->retain();
						return;
					}
				}
				break;
			case DyBuyItemType_Recipe:
				{
					DyRecipeInfo* recipe = (DyRecipeInfo*)m_processItem;

                    if(recipe->m_purchase_gold > 0)
                    {
                        if(getGold() < recipe->m_purchase_gold)
                        {
                            m_popup_process = (DyPopup*)alramNotEnoughGold(false, true);
                        }
                        else
                        {
                            DyLib::sharedLib()->data_backup();
                            
                            DyRecipeManager::sharedRecipeManager()->buyedRecipe(recipe);
                            addGold(-recipe->m_purchase_gold, true);
                            
                            YtLayerManager::sharedLayerManager()->destoryTopLayer(false);
                            
                            DyPopup* loadingPopup = lib->network_kakao_update_me(0, DyMoneyManager_callfunc_update_me);
                            if(loadingPopup)
                                loadingPopup->m_resumeAndDestory = false;
                            
                            m_popup_process->retain();
                            return;
                        }
                    }
                    if(recipe->m_purchase_delly > 0)
                    {
                        if(getDelly() < recipe->m_purchase_delly)
                        {
                            m_popup_process = (DyPopup*)alramNotEnoughDelly(false, true);
                        }
                        else
                        {
                            DyLib::sharedLib()->data_backup();
                            
                            DyRecipeManager::sharedRecipeManager()->buyedRecipe(recipe);
                            addDelly(-recipe->m_purchase_delly, true);
                            
                            YtLayerManager::sharedLayerManager()->destoryTopLayer(false);
                            
                            DyPopup* loadingPopup = lib->network_kakao_update_me(0, DyMoneyManager_callfunc_update_me);
                            if(loadingPopup)
                                loadingPopup->m_resumeAndDestory = false;
                            
                            m_popup_process->retain();
                            return;
                        }
                    }
				}
				break;

			case DyBuyItemType_Shop:
				{
					DyStageManager* stageManager = DyStageManager::sharedStageManager();

					int shop = *(int *)m_processItem;
					int gold = atoi(stageManager->m_shopinfo->getValue("gold", shop));

					if(getGold() < gold)
					{
						m_popup_process = (DyPopup*)alramNotEnoughGold(false, true);
					}
					else
					{
                        DyLib::sharedLib()->data_backup();
                        
						stageManager->buyedShop(shop);
						addGold(-gold, true);
                        
                        YtLayerManager::sharedLayerManager()->destoryTopLayer(false);
                        
                        lib->network_kakao_update_me(0, DyMoneyManager_callfunc_update_me);
                        m_popup_process->retain();
                        return;
					}
				}
				break;
            default:
                break;
		}
		YtLayerManager::sharedLayerManager()->destoryTopLayer(false);
	}
	else if(pPopup->m_active_tag == 10) // success
	{
		YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
	}
	else if(pPopup->m_active_tag == 21) // delly not enough
	{
		YtLayerManager::sharedLayerManager()->destoryTopLayer(false);
		YtLayer* layer = (YtLayer*)YtLayerManager::sharedLayerManager()->getTopLayer();

        if(layer->m_tag.compare(DYLAYERSHOP_TAG) == 0)
        {
            layer->resume();
            
            DyLayerShop* layerShop = (DyLayerShop*)layer;
            if(layerShop->m_shopType == DyLayerShopType_Myroom)
            {
                //layerShop->m_callShop = 0;
                //YtLayerManager::sharedLayerManager()->popLayer();
                DyLayerShop* layerShop2 = DyLayerShop::create(layerShop, DyLayerShopType_Normal, 0, 0);
                YtLayerManager::sharedLayerManager()->pushLayer(layerShop2, true, true);
                
            }
            else
            {
                layerShop->m_tabView->goTab(0, false);
            }
        }
        else
        {
            DyLayerShop* layerShop2 = DyLayerShop::create(layer, DyLayerShopType_Normal, 0, 0);
            YtLayerManager::sharedLayerManager()->pushLayer(layerShop2, true, true);

        }
	}
	else if(pPopup->m_active_tag == 31) // gold not enough
	{
		YtLayerManager::sharedLayerManager()->destoryTopLayer(false);
		DyLayerShop* layerShop = (DyLayerShop*)YtLayerManager::sharedLayerManager()->getTopLayer();

		if(layerShop->m_tag.compare(DYLAYERSHOP_TAG) == 0)
		{
            layerShop->resume();
            
			if(layerShop->m_shopType == DyLayerShopType_Myroom)
			{
                //layerShop->m_callShop = 1;
                //YtLayerManager::sharedLayerManager()->popLayer();
                DyLayerShop* layerShop2 = DyLayerShop::create(layerShop, DyLayerShopType_Normal, 5, 0);
                YtLayerManager::sharedLayerManager()->pushLayer(layerShop2, true, false);
                
			}
			else
			{
				layerShop->m_tabView->goTab(5, false);
			}
		}
		else
		{
			DyLayerShop* layerShop2 = DyLayerShop::create(layerShop, DyLayerShopType_Normal, 5, 0);
			YtLayerManager::sharedLayerManager()->pushLayer(layerShop2, true, false);
		}
	}
	else
    {
		YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
    }

	if(m_popup_process)
		YtLayerManager::sharedLayerManager()->pushLayer(m_popup_process, true, false);
}


void DyMoneyManager::questBuy(DyBuyItemType itemType, void* info, DyBuyMethod method)
{
	m_processItem = info;
	m_processItemType = itemType;
	m_method = method;

	DyLib* lib = DyLib::sharedLib();

	DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyMoneyManager::onPopupClicked));
	
    int gold = 0;
    int delly = 0;
    
	switch(m_processItemType)
	{
		case DyBuyItemType_Delly:
			{
                m_popup_process = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyMoneyManager::onPopupClicked));
                m_popup_process->setText(STRING_result_buy_success);
                m_popup_process->addButton(DyButtonType_Red, STRING_POPUP_OK, 10);
                
                DyDellyInfo* delly = (DyDellyInfo*)m_processItem;
                
                lib->network_iab(DyMoneyManager_callfunc_iab, delly->m_product_id.c_str());
                
                m_popup_process->retain();
                
                return;
			}
			break;
		case DyBuyItemType_Costume:
			{
				DyCostume* costume = (DyCostume*)info;

				sprintf(lib->m_strTemp2, "%s", costume->m_name.c_str());
                
                if(costume->m_price_gold > 0)
                    YtLib::setMoneyStyle(lib->m_strTemp3, costume->m_price_gold);
                else
                    YtLib::setMoneyStyle(lib->m_strTemp3, costume->m_price_delly);
			}
			break;
		case DyBuyItemType_Gum:
			{
				DyGumInfo* gum = (DyGumInfo*)info;
				sprintf(lib->m_strTemp2, "%d%s", gum->m_gum_cnt, STRING_gum);
				YtLib::setMoneyStyle(lib->m_strTemp3, gum->m_price_delly);
			}
			break;
		case DyBuyItemType_CostumeArray:
			{
				CCArray* costumeArray = (CCArray*)info;
				for(unsigned int i = 0;i < costumeArray->count();i++)
				{
					DyCostume* costume = (DyCostume*)costumeArray->objectAtIndex(i);
					gold += costume->m_price_gold;
                    delly += costume->m_price_delly;
				}
				sprintf(lib->m_strTemp2, "%s", STRING_buy_allcostume);

				YtLib::setMoneyStyle(lib->m_strTemp3, delly);
                YtLib::setMoneyStyle(lib->m_strTemp4, gold);
			}
			break;
		case DyBuyItemType_Recipe:
			{
				DyRecipeInfo* recipe = (DyRecipeInfo*)info;

				sprintf(lib->m_strTemp2, "%s%s", recipe->m_name.c_str(), STRING_s_recipe);
                
                if(recipe->m_purchase_gold > 0)
                    YtLib::setMoneyStyle(lib->m_strTemp3, recipe->m_purchase_gold);
                else
                    YtLib::setMoneyStyle(lib->m_strTemp3, recipe->m_purchase_delly);
			}
			break;
		case DyBuyItemType_Shop:
			{
				DyStageManager* stageManager = DyStageManager::sharedStageManager();

				int shop = *(int *)info;
				sprintf(lib->m_strTemp2, "%s", stageManager->m_shopinfo->getValue("name_kor", shop));
                
				YtLib::setMoneyStyle(lib->m_strTemp3, atoi(stageManager->m_shopinfo->getValue("gold", shop)));
			}
			break;
	}

	switch(method)
	{
		case DyBuyMethod_Cash:
#ifdef ANDROID
			sprintf(lib->m_strTemp, STRING_buy_question_bycash, lib->m_strTemp2, lib->m_strTemp3);
#else
            sprintf(lib->m_strTemp, STRING_buy_question_bycash_dollar, lib->m_strTemp2, lib->m_strTemp3);
#endif
			break;
		case DyBuyMethod_Delly:
			sprintf(lib->m_strTemp, STRING_buy_question_bydelly, lib->m_strTemp2, lib->m_strTemp3);
			break;
		case DyBuyMethod_Gold:
			sprintf(lib->m_strTemp, STRING_buy_question_bygold, lib->m_strTemp2, lib->m_strTemp3);
			break;
		case DyBuyMethod_GoldAndDelly:
            if(delly > 0 && gold > 0)
                sprintf(lib->m_strTemp, STRING_buy_question_bygolddelly, lib->m_strTemp2, lib->m_strTemp3, lib->m_strTemp4);
            else if(delly > 0)
                sprintf(lib->m_strTemp, STRING_buy_question_bydelly, lib->m_strTemp2, lib->m_strTemp3);
            else if(gold > 0)
                sprintf(lib->m_strTemp, STRING_buy_question_bygold, lib->m_strTemp2, lib->m_strTemp4);
            break;

	}

	popup->setText(lib->m_strTemp);
	popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
	popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 1);
	popup->addButton(DyButtonType_Close, "", 2);

	YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}

void DyMoneyManager_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str)
{
    DyLib* lib = DyLib::sharedLib();
    
    //DyLayerMainMenu *layerMainMenu = (DyLayerMainMenu*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERMAINMENU_TAG);
    
    switch(cmdType)
    {
        case DyNetworkCmdType_PriceVerify:
            if(value_int == 0)
            {
                YtPlatform_contumeProdcut();
                lib->m_callFuncIAB(0, 0);
            }
            else
                lib->m_callFuncIAB(1, 0);
            break;
        default:
            break;
    }
}


void DyMoneyManager_VerifyPrice(const char* data, const char* signature)
{
    DYNETWORK_ADD_PROC(DyNetworkCmdType_PriceVerify, data, (void*)signature, DyMoneyManager_NetCallFunc);
}


DyMoneyManager* DyMoneyManager::sharedMoneyManager()
{
	if(g_sharedMoneyManager == NULL)
	{
		g_sharedMoneyManager = new DyMoneyManager();
	}
	return g_sharedMoneyManager;
}


DyLayerMoney::DyLayerMoney(DyLayerMoneyType layerType, int gum_bg_color)
{
	m_layerType = layerType;
	m_menu = NULL;
	m_delegate = NULL;
    m_gum_bg_color = gum_bg_color;

	initWithColor(ccc4(255, 0, 0, 0), 630, 26);

	float fBoxWidth[3] = {223, 142, 187};
	CCScale9Sprite* sprBg[3];

	CCMenuItemImage *oneItem[3];
	CCLabelTTF *label[3];
	float curX = 0;

	for(int i = 0;i < 3;i++)
	{
		curX += fBoxWidth[i]/2;

		sprBg[i] = CCScale9Sprite::createWithSpriteFrame(CCSprite::createWithSpriteFrameName("menu_ui_number_bg.png")->displayFrame());
		sprBg[i]->setContentSize(CCSizeMake(fBoxWidth[i], 26));
		sprBg[i]->setAnchorPoint(ccp(0.5, 0.5));
		sprBg[i]->setPosition(ccp(curX, 13));
		addChild(sprBg[i]);

        oneItem[i] = CCMenuItemImage::create();
		CCSprite* sprNormal = CCSprite::createWithSpriteFrameName("menu_ui_btn_plus.png");
		CCSprite* sprSelect = CCSprite::createWithSpriteFrameName("menu_ui_btn_plus_sel.png");
		CCSprite* sprDisable = CCSprite::createWithSpriteFrameName("menu_ui_btn_plus.png");

		oneItem[i]->initWithNormalSprite(sprNormal, sprSelect, sprDisable, this, menu_selector(DyLayerMoney::buttonClicked));
        oneItem[i]->setTag(i);
		oneItem[i]->setPosition(ccp(sprBg[i]->getPosition().x + sprBg[i]->getContentSize().width/2 - 10, sprBg[i]->getPosition().y));

		curX += (fBoxWidth[i]/2 + 36.f);

		label[i] = CCLabelTTF::create("10:11", FONT_NAME_DEFAULT, 24.f);
		label[i]->setAnchorPoint(ccp(1, 0.5));
		label[i]->setPosition(ccp(sprBg[i]->getPosition().x + sprBg[i]->getContentSize().width/2 - 25, sprBg[i]->getPosition().y-2));
		//label[i]->setColor(ccBLACK);
		label[i]->setColor(ccc3(52, 23, 14));

		addChild(label[i]);
		
		switch(i)
		{
			case 0:
				break;
			case 1:
				if(m_layerType == DyLayerMoneyType_Normal)
				{
					CCSprite* sprDelly = CCSprite::createWithSpriteFrameName("menu_ui_delly_outline.png");
					sprDelly->setScale(0.8f);
					sprDelly->setPosition(ccp(sprBg[i]->getPosition().x - sprBg[i]->getContentSize().width/2, sprBg[i]->getPosition().y));
					addChild(sprDelly);

				}
				break;
			case 2:
				if(m_layerType == DyLayerMoneyType_Normal)
				{
					CCSprite* sprGold = CCSprite::createWithSpriteFrameName("menu_ui_gold_line.png");
					sprGold->setScale(0.8f);
					sprGold->setPosition(ccp(sprBg[i]->getPosition().x - sprBg[i]->getContentSize().width/2, sprBg[i]->getPosition().y));
					addChild(sprGold);
				}
				break;
		}
	}

	m_labelGumStatus = label[0];
	m_labelDellyStatus = label[1];
	m_labelGoldStatus = label[2];

	m_menu = CCMenu::create(oneItem[0], oneItem[1], oneItem[2], NULL);
	m_menu->setPosition(CCPointZero);
	addChild(m_menu);

	for(int i = 0;i < DyDataManager::sharedDataManager()->m_rechargeable_gum;i++)
	{
		CCSprite* sprGumBg;

		if(m_gum_bg_color == 1)
			sprGumBg = CCSprite::createWithSpriteFrameName("menushop_ui_gum_bg2.png");// 녹색
		else
			sprGumBg = CCSprite::createWithSpriteFrameName("menushop_ui_gum_bg.png");//갈색

		sprGumBg->setPosition(ccp(10+25*i, 12));
		sprGumBg->setAnchorPoint(ccp(0.5, 0.5));
		addChild(sprGumBg);

		m_sprGum[i] = CCSprite::createWithSpriteFrameName("menushop_ui_gum.png");

		m_sprGum[i]->setPosition(ccp(10+25*i, 12));
		m_sprGum[i]->setAnchorPoint(ccp(0.5, 0.5));
		m_sprGum[i]->setVisible(false);
		addChild(m_sprGum[i]);
	}

	if(m_layerType == DyLayerMoneyType_OnlyGum)
	{
		for(int i = 1;i < 3;i++)
		{
			sprBg[i]->setVisible(false);
			label[i]->setVisible(false);
			oneItem[i]->setVisible(false);
		}
	}
	updateStatus(false, 0);

}

DyLayerMoney::~DyLayerMoney()
{
}

void DyLayerMoney::onEnter()
{
	CCLayer::onEnter();
	if(m_isInit)
		return;

	m_isInit = true;
}


void DyLayerMoney::updateStatus(bool isGumTime, int remainTime)
{
	YtLib* lib = YtLib::sharedLib();

	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();

	if(isGumTime)
	{
		sprintf(lib->m_strTemp, "%02d:%02d", remainTime/60, remainTime%60);
		m_labelGumStatus->setString(lib->m_strTemp);
	}
	else
	{
		if(moneyManager->m_isDoCheckTimeForGum)
		{
            CCDate dateCurrent = dataManager->getCurrentDate();
            
            CCDate diffDate = dateCurrent - dataManager->m_heart_regen_starts_at;
          
			int _remainTime = dataManager->m_heart_regen_interval - diffDate.time();
			sprintf(lib->m_strTemp, "%02d:%02d", _remainTime/60, _remainTime%60);
		}
		else
		{
			if(dataManager->m_gum == dataManager->m_rechargeable_gum)
				sprintf(lib->m_strTemp, "MAX");
			else
				sprintf(lib->m_strTemp, "+%d", dataManager->m_gum - dataManager->m_rechargeable_gum);
		}

		m_labelGumStatus->setString(lib->m_strTemp);
		for(int i = 0;i < dataManager->m_rechargeable_gum;i++)
		{
			if(i < dataManager->m_gum)
				m_sprGum[i]->setVisible(true);
			else
				m_sprGum[i]->setVisible(false);
		}

		YtLib::setMoneyStyle(lib->m_strTemp, moneyManager->getGold());
		m_labelGoldStatus->setString(lib->m_strTemp);

		sprintf(lib->m_strTemp, "%d", moneyManager->getDelly());
		m_labelDellyStatus->setString(lib->m_strTemp);
	}
}


void DyLayerMoney::buttonClicked(CCObject * obj)
{
	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

    CCMenuItemImage * pMenu = dynamic_cast<CCMenuItemImage*>(obj);
	int buttonIndex = pMenu->getTag();

	if(m_delegate)
		m_delegate->buttonClicked(this, buttonIndex);
	else
	{
		DyLayerShop* layerShop = NULL;
		switch(buttonIndex)
		{
			case 0:
				layerShop = DyLayerShop::create(this, DyLayerShopType_Normal, 1, 0);
				break;
			case 1:
				layerShop = DyLayerShop::create(this, DyLayerShopType_Normal, 0, 0);
				break;
			case 2:
				layerShop = DyLayerShop::create(this, DyLayerShopType_Normal, 5, 0);
				break;
		}
		YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);
	}
}



void DyLayerMoney::onExit()
{
	CCLayer::onExit();
}



void DyLayerMoney::pause()
{
	if(m_menu)
		m_menu->setEnabled(false);
}


void DyLayerMoney::resume()
{
	if(m_menu)
		m_menu->setEnabled(true);
}


void DyLayerMoney::hide()
{
}


void DyLayerMoney::show()
{
}
