/*
DyRecipe.cpp
Doggy chef
@initialize at 121008

@history
121008	yoonsr	initialize
*/

#include "DyRecipe.h"
#include "DyStage.h"
#include "DyBlockInfo.h"

#include "DyLoader.h"
#include "DyLib.h"
#include "DyLayerShop.h"
#include "DyTycoon.h"
#include "DyPopup.h"

DyRecipeManager* g_sharedRecipeManager;


DyMaterial::DyMaterial()
{
	m_labelCount = NULL;
}


DyMaterial::~DyMaterial()
{
}


DyOrderRecipe::DyOrderRecipe(DyRecipeInfo* info)
{
	m_info = info;
	m_arrMateiralStatus = new CCArray();

    
	for(unsigned int i = 0;i < info->m_arrMaterial->count();i++)
	{
		DyMaterial* materialOrgin = (DyMaterial*)info->m_arrMaterial->objectAtIndex(i);
		DyMaterial* material = new DyMaterial();
		
        material->m_count = materialOrgin->m_count;
		material->m_tag = materialOrgin->m_tag;
			
		m_arrMateiralStatus->addObject(material);

		material->release();
	}
    
    //야채쌈(기본): 야채 -돼지고기/소고기-마늘/파/고추 총 3단계의 재로가 들어감
    //김치쌈: 김치-돼지고기/소고기-마늘/파/고추
    if(info->m_tag.compare("sam_vegetable") == 0 || info->m_tag.compare("sam_kimchi") == 0)
    {
        DyMaterial* material = new DyMaterial();
		material->m_count = 3 + rand()%3;
		material->m_tag = (rand()%2 == 0 ? "r_pork" : "r_beef");
		m_arrMateiralStatus->addObject(material);
		material->release();

        material = new DyMaterial();
		material->m_count = 3 + rand()%3;
        
        int what = rand()%3;
        if(what == 0)
            material->m_tag = "garlic";
        else if(what == 1)
            material->m_tag = "pa";
        else
            material->m_tag = "pepper";
        
		m_arrMateiralStatus->addObject(material);
		material->release();
    }
}



DyOrderRecipe::~DyOrderRecipe()
{
	m_arrMateiralStatus->release();
}


DyRecipeInfo::DyRecipeInfo()
{
	m_arrMaterial = new CCArray();
}


DyRecipeInfo::~DyRecipeInfo()
{
	m_arrMaterial->release();
}


void DyRecipeManager::loadRecipe()
{
	YtLib* lib = YtLib::sharedLib();
    
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyDataManager* dataManager = DyDataManager::sharedDataManager();

    m_dicRecipes->removeAllObjects();
    
    for(int i = 0;i < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;i++)
	{
		const char* data_url = stageManager->m_shopinfo->getValue("resource_recipe", i);
        
		sprintf(lib->m_strTemp, "%s.txt", data_url);
        
		YtObjectGroup* recipeInfo = DyLoader::sharedLoader()->loadObjects(lib->m_strTemp);
        
		for(int j = 0;j < recipeInfo->count();j++)
		{
			DyRecipeInfo* recipe = new DyRecipeInfo();
            
			recipe->m_id = atoi(recipeInfo->getValue("id", j));
			recipe->m_idShop = atoi(stageManager->m_shopinfo->getValue("id", i));
			recipe->m_selling_price = atoi(recipeInfo->getValue("selling_price", j));
			recipe->m_purchase_gold = atoi(recipeInfo->getValue("purchase_gold", j));
            recipe->m_purchase_delly = atoi(recipeInfo->getValue("purchase_delly", j));
			recipe->m_tag = recipeInfo->getValue("tag", j);
			recipe->m_name = recipeInfo->getValue("name_kor", j);
			recipe->m_urlResultImage = recipeInfo->getValue("resultimage", j);
			recipe->m_explain = recipeInfo->getValue("explain_kor", j);
            
            recipe->m_crazy_mux = atoi(recipeInfo->getValue("crazy_mux", j));
            recipe->m_heart_plus = atoi(recipeInfo->getValue("heart_plus", j));
            
			recipe->m_isAvailable = dataManager->recipe_isAvailable(recipe->m_id);
			recipe->m_isSawInShop = dataManager->recipe_isSawInShop(recipe->m_id);
			recipe->m_isSawInGame = dataManager->recipe_isSawInGame(recipe->m_id);
            
			recipe->m_mainMaterial = recipeInfo->getValue("main_material", j);
            
			const char* attr = recipeInfo->getValue("attribute", j);
			
			if(strcmp(attr, "default") == 0)
			{
                
                dataManager->recipe_setAvailable(recipe->m_id, true);
				dataManager->recipe_setSawInShop(recipe->m_id, true);
				dataManager->recipe_setSawInGame(recipe->m_id, true);

                
                
				recipe->m_attr = DyRecipeAttr_Default;
				recipe->m_isAvailable = true;
				recipe->m_isSawInShop = true;
				recipe->m_isSawInGame = true;
			}
			else if(strcmp(attr, "secret") == 0)
			{
				recipe->m_attr = DyRecipeAttr_Secret;
			}
			else
			{
				recipe->m_attr = DyRecipeAttr_Additional;
            }
            
            recipe->m_showStage = atoi(recipeInfo->getValue("show_stage", j));
            
			CCArray* arrayMaterial = recipeInfo->getArray("material", j);
            
			for(unsigned int k = 0;k < arrayMaterial->count();k+=2)
			{
				DyMaterial* material = new DyMaterial();
                
				material->m_tag = ((CCString*)arrayMaterial->objectAtIndex(k))->getCString();
				material->m_count = ((CCString*)arrayMaterial->objectAtIndex(k+1))->intValue();
				material->m_unAvailableCount = 0;
				//material->m_id = blockManager->getIdByTag(material->m_tag.c_str());
				recipe->m_arrMaterial->addObject(material);
				material->release();
			}
            
			CCArray* arrayAnchoPoint = recipeInfo->getArray("anchorpoint", j);
			recipe->m_anchorPoint.x = ((CCString*)arrayAnchoPoint->objectAtIndex(0))->floatValue();
			recipe->m_anchorPoint.y = ((CCString*)arrayAnchoPoint->objectAtIndex(1))->floatValue();
            
			const char* hasDish = recipeInfo->getValue("dish", j);
			
			if(strcmp(hasDish, "yes") == 0)
				recipe->m_hasDish = true;
			else
				recipe->m_hasDish = false;
            
			recipe->m_scale = atof(recipeInfo->getValue("scale", j));
            
			m_dicRecipes->setObject(recipe, recipe->m_id);
			recipe->release();
		}
	}
    m_removeCakeType = -1;
    
	dataManager->dataSave();
}


DyRecipeManager::DyRecipeManager()
{
    m_callFunc = NULL;
    
	m_dicRecipes = new CCDictionary();

	m_arrAvailableRecipes = new CCArray();
	m_arrWantedRecipe = new CCArray();
    
    loadRecipe();
}


bool DyRecipeManager::isAllbuy(int shopID)
{
	CCArray* allkeys = m_dicRecipes->allKeys();

	bool isAllBuy = true;
	for(unsigned int i = 0;i < allkeys->count();i++)
	{
		DyRecipeInfo* recipe = (DyRecipeInfo*)m_dicRecipes->objectForKey(((CCInteger*)allkeys->objectAtIndex(i))->getValue());
		if(recipe->m_idShop == shopID)
		{
			if(recipe->m_isAvailable == false)
			{
				isAllBuy = false;
				break;
			}
		}
	}

	return isAllBuy;
}


void DyRecipeManager::buyedAllRecipe(int shop)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    CCArray* allkeys = m_dicRecipes->allKeys();
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_MSG);

    
    for(unsigned int i = 0;i < allkeys->count();i++)
	{
		DyRecipeInfo* recipe = (DyRecipeInfo*)m_dicRecipes->objectForKey(((CCInteger*)allkeys->objectAtIndex(i))->getValue());
		if(recipe->m_idShop == shop)
		{
            recipe->m_isAvailable = true;
            dataManager->recipe_setAvailable(recipe->m_id, true);
            dataManager->recipe_setSawInGame(recipe->m_id, true);
            
            dataManager->checkAwards(DyAwardsCheckType_Recipe, false);
		}
	}
}



int DyRecipeManager::getRecipeTotalCnt(int shopID)
{
   	CCArray* allkeys = m_dicRecipes->allKeys();
    
	int cnt = 0;
	for(unsigned int i = 0;i < allkeys->count();i++)
	{
		DyRecipeInfo* recipe = (DyRecipeInfo*)m_dicRecipes->objectForKey(((CCInteger*)allkeys->objectAtIndex(i))->getValue());
		if(recipe->m_idShop == shopID)
		{
			cnt++;
		}
	}
    
	return cnt;
 
}


int DyRecipeManager::getRecipeHasCnt(int shopID)
{
    CCArray* allkeys = m_dicRecipes->allKeys();
    
	int cnt = 0;
    for(unsigned int i = 0;i < allkeys->count();i++)
	{
		DyRecipeInfo* recipe = (DyRecipeInfo*)m_dicRecipes->objectForKey(((CCInteger*)allkeys->objectAtIndex(i))->getValue());
        if(recipe->m_idShop == shopID)
		{
			if(recipe->m_isAvailable)
			{
				cnt++;
			}
		}
	}
	return cnt;
}


int DyRecipeManager::getDefaultCnt(int shopID)
{
    CCArray* allkeys = m_dicRecipes->allKeys();
    
	int cnt = 0;
    for(unsigned int i = 0;i < allkeys->count();i++)
	{
		DyRecipeInfo* recipe = (DyRecipeInfo*)m_dicRecipes->objectForKey(((CCInteger*)allkeys->objectAtIndex(i))->getValue());
        if(recipe->m_idShop == shopID)
		{
			if(recipe->m_attr == DyRecipeAttr_Default)
			{
				cnt++;
			}
		}
	}
	return cnt;

}



void DyRecipeManager::removeRecipe(CCArray* array, std::string tag)
{
    for(unsigned int i = 0;i < array->count();i++)
    {
        DyRecipeInfo* recipe = (DyRecipeInfo*)array->objectAtIndex(i);
        
        if(recipe->m_tag.compare(tag) == 0)
        {
            array->removeObject(recipe);
            break;
        }
    }
}


bool DyRecipeManager::hasRecipe(CCArray* array, std::string tag)
{
    for(unsigned int i = 0;i < array->count();i++)
    {
        DyRecipeInfo* recipe = (DyRecipeInfo*)array->objectAtIndex(i);
        
        if(recipe->m_tag.compare(tag) == 0)
        {
            return true;
        }
    }
    return false;
}



bool DyRecipeManager::isAvailableRecipe(std::string tag)
{
    for(int i = 0;i < m_arrAvailableRecipes->count();i++)
    {
        DyRecipeInfo* recipe = (DyRecipeInfo*)m_arrAvailableRecipes->objectAtIndex(i);
        
        if(recipe->m_tag.compare(tag) == 0)
        {
            return true;
        }
    }
    return false;
}



void DyRecipeManager::removeCakeOrMuffine(CCArray* array, bool isCake)
{
    if(isCake)
    {
        removeRecipe(array, "cake");
        removeRecipe(array, "cake_cream");
        removeRecipe(array, "cake_choco_straw_cream");
        removeRecipe(array, "cake_tira_choco");
    }
    else
    {
        removeRecipe(array, "muff");
        removeRecipe(array, "muff_choco");
        removeRecipe(array, "muff_choco_straw_cream");
    }

}

void DyRecipeManager::reorderingAvailable(int shopID)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    bool isRemoveCakeOrMuffine = false;
    bool isRemoveCake = false;
    
    bool isRemovePotato = false;  // 햄버거가게에서 치킨이 나오면 포테이토를 뺀다.
    
    bool isRemoveSam = false; // 한식당 크레이지 모드 일경우 쌈을 없애야한다.
    bool isRemoveBob = false; // 한식당에서 다른싸이드메뉴가 없으면 bob을 시키지 않는다.
    
    
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
	CCArray* allkeys = m_dicRecipes->allKeys();

	m_arrAvailableRecipes->removeAllObjects();
        
        
	for(unsigned int i = 0;i < allkeys->count();i++)
	{
		DyRecipeInfo* recipe = (DyRecipeInfo*)m_dicRecipes->objectForKey(((CCInteger*)allkeys->objectAtIndex(i))->getValue());
        if(recipe->m_idShop == shopID && recipe->m_isAvailable)
        {
            if(tycoonManager->m_isCrazyMode || recipe->m_showStage <= stageManager->m_curLevel)
            {
                m_arrAvailableRecipes->addObject(recipe);
            }
        }
	}
    
    m_arrWantedRecipe->removeAllObjects();
    for(unsigned int i = 0;i < allkeys->count();i++)
	{
		DyRecipeInfo* recipe = (DyRecipeInfo*)m_dicRecipes->objectForKey(((CCInteger*)allkeys->objectAtIndex(i))->getValue());
        if(recipe->m_idShop == shopID)
        {
            bool add = false;
            
            if(recipe->m_attr == DyRecipeAttr_Secret && recipe->m_isAvailable == false)
                continue;

            if(tycoonManager->m_isCrazyMode)
            {
                if(recipe->m_isAvailable)
                     add = true;
            }
            else
            {
                if(recipe->m_showStage <= stageManager->m_curLevel)
                    add = true;
            }
            
            if(add)
            {
                m_arrWantedRecipe->addObject(recipe);
                
                if(strstr(recipe->m_tag.c_str(), "chicken") > 0)
                     isRemovePotato = true;
                if(strstr(recipe->m_tag.c_str(), "straw") > 0)
                {
                    isRemoveCakeOrMuffine = true;
                    
                    const char* strNewRecipe = stageManager->m_curShopInfo->getValue("new_recipe", stageManager->m_curLevel);
                    
                    if(strcmp(strNewRecipe, "cake_choco_straw_cream") == 0)
                        m_removeCakeType = 1;
                    else if(strcmp(strNewRecipe, "muff_choco_straw_cream") == 0)
                        m_removeCakeType = 0;
                }
                
                if(tycoonManager->m_isCrazyMode && stageManager->m_curShopType == DyStageType_Korean)
                    isRemoveSam = true;
                
                
                if(stageManager->m_curShopType == DyStageType_Korean && tycoonManager->m_association[1] == 0 && tycoonManager->m_association[2] == 0)
                    isRemoveBob = true;
            }
        }
	}
    
    if(isRemoveBob)
    {
        removeRecipe(m_arrAvailableRecipes, "bob");
        removeRecipe(m_arrWantedRecipe, "bob");
    }
    
    
    if(isRemovePotato)
    {
        removeRecipe(m_arrAvailableRecipes, "french_fried");
        removeRecipe(m_arrWantedRecipe, "french_fried");
    }
    
    if(isRemoveSam)
    {
        removeRecipe(m_arrAvailableRecipes, "sam_vegetable");
        removeRecipe(m_arrWantedRecipe, "sam_vegetable");
        
        removeRecipe(m_arrAvailableRecipes, "sam_kimchi");
        removeRecipe(m_arrWantedRecipe, "sam_kimchi");
    }
    
    if(isRemoveCakeOrMuffine)
    {
        int cakeCnt = 0;
        
        if(hasRecipe(m_arrAvailableRecipes, "cake"))cakeCnt++;
        if(hasRecipe(m_arrAvailableRecipes, "cake_cream"))cakeCnt++;
        if(hasRecipe(m_arrAvailableRecipes, "cake_choco_straw_cream"))cakeCnt++;
        if(hasRecipe(m_arrAvailableRecipes, "cake_tira_choco"))cakeCnt++;
        
        int muffCnt = 0;
        if(hasRecipe(m_arrAvailableRecipes, "muff"))muffCnt++;
        if(hasRecipe(m_arrAvailableRecipes, "muff_choco"))muffCnt++;
        if(hasRecipe(m_arrAvailableRecipes, "muff_choco_straw_cream"))muffCnt++;
        
        if(m_removeCakeType == -1)
        {
            if(cakeCnt == 0)
                isRemoveCake = false;
            else if(muffCnt == 0)
                isRemoveCake = true;
            else
                isRemoveCake = rand()%2 == 1 ? true: false;
            
            if(isRemoveCake)
                m_removeCakeType = 0;
            else
                m_removeCakeType = 1;
        }
        else if(m_removeCakeType == 0)
        {
            isRemoveCake = true;
        }
        else
        {
            isRemoveCake = false;
        }
        
        removeCakeOrMuffine(m_arrAvailableRecipes, isRemoveCake);
        removeCakeOrMuffine(m_arrWantedRecipe, isRemoveCake);
    }
}


DyRecipeManager::~DyRecipeManager()
{
	m_dicRecipes->release();
	m_arrAvailableRecipes->release();
	m_arrWantedRecipe->release();
}


DyRecipeManager* DyRecipeManager::sharedRecipeManager()
{
	if(g_sharedRecipeManager == NULL)
	{
		g_sharedRecipeManager = new DyRecipeManager();
	}
	return g_sharedRecipeManager;
}

void DyRecipeManager::onPopupClicked(cocos2d::CCObject * pSender)
{
    DyRecipeManager* recipeManager = DyRecipeManager::sharedRecipeManager();
	//DyLib* lib = DyLib::sharedLib();
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);

	if(pPopup->m_active_tag == 10)
	{
		YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        if(recipeManager->m_callFunc)
            recipeManager->m_callFunc(0, 0);
	}
	else if(pPopup->m_active_tag == 0)//닫기
	{
		DyPopup* popupWarning  = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyRecipeManager::onPopupClicked));

		popupWarning->setText(STRING_recipe_buy_warning);
		popupWarning->addButton(DyButtonType_Red, STRING_POPUP_OK, 10);		

		YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
		YtLayerManager::sharedLayerManager()->pushLayer(popupWarning, true, true);
	}
	else if(pPopup->m_active_tag == 1)//사러가기
	{
		DyStageManager* stageManager = DyStageManager::sharedStageManager();
		DyLayerShop* layerShop = DyLayerShop::create(YtLayerManager::sharedLayerManager()->getTopLayer(), DyLayerShopType_Normal, 3, stageManager->m_curShopType);
		
		YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
		YtLayerManager::sharedLayerManager()->pushLayer(layerShop, true, true);

		tycoonManager->m_gameStartForce = true;
	}
	else if(pPopup->m_active_tag == 2)//바로구매
	{
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
        DyRecipeInfo* recipe = DyRecipeManager::sharedRecipeManager()->getRecipe(recipeManager->m_process_recipe_tag);
        DyMoneyManager::sharedMoneyManager()->questBuy(DyBuyItemType_Recipe, recipe, recipe->m_purchase_gold > 0 ? DyBuyMethod_Gold : DyBuyMethod_Delly);
        
        tycoonManager->m_gameStartForce = true;
	}
}


void DyRecipeManager::alramNewRecipe(std::string tag, bool alreadHas)
{
	DyPopup* popup = DyPopup::create(DyPopupType_NewRecipe, this, callfuncO_selector(DyRecipeManager::onPopupClicked));
	popup->setRecipe(tag, alreadHas);
    
    if(alreadHas)
    {
        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 10);
    }
    else
    {
        popup->addButton(DyButtonType_Blue, STRING_POPUP_SHOP, 1);
        popup->addButton(DyButtonType_Red, STRING_POPUP_BUY, 2);
        popup->addButton(DyButtonType_Close, "", 0);
    }
    
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_MSG);
    
    m_process_recipe_tag = tag;
}



void DyRecipeManager::clearGameCheck()
{
    CCArray* allkeys = m_dicRecipes->allKeys();
    for(unsigned int i = 0;i < allkeys->count();i++)
    {
        DyRecipeInfo* recipe = (DyRecipeInfo*)m_dicRecipes->objectForKey(((CCInteger*)allkeys->objectAtIndex(i))->getValue());
        
        recipe->m_isCheckedInGame = false;
    }

}


bool DyRecipeManager::checkNewRecipe(YtPlatform_CallFunc callFunc)
{
	//DyLib* lib = DyLib::sharedLib();

	m_callFunc = callFunc;
    
	//bool hasNewRecipe = false;
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();

    if(!tycoonManager->m_isCrazyMode)
    {
        CCArray* allkeys = m_dicRecipes->allKeys();
        for(unsigned int i = 0;i < allkeys->count();i++)
        {
            DyRecipeInfo* recipe = (DyRecipeInfo*)m_dicRecipes->objectForKey(((CCInteger*)allkeys->objectAtIndex(i))->getValue());

            if(recipe->m_attr != DyRecipeAttr_Default && recipe->m_isCheckedInGame == false && recipe->m_idShop == stageManager->m_curShopType && recipe->m_showStage == stageManager->m_curLevel && recipe->m_attr != DyRecipeAttr_Secret)
            {
                recipe->m_isCheckedInGame = true;
                if(recipe->m_isAvailable)
                {
                    alramNewRecipe(recipe->m_tag, true);
                }
                else
                {
                    recipe->m_isSawInGame = true;
                    dataManager->recipe_setSawInGame(recipe->m_id, true);
                    dataManager->dataSave();
                    
                    alramNewRecipe(recipe->m_tag, false);
                }
                return true;
            }
        }
        
        
        
        bool isAllReciepeHaveForSecret = true;
        
        for(unsigned int i = 0;i < allkeys->count();i++)
        {
            DyRecipeInfo* recipe = (DyRecipeInfo*)m_dicRecipes->objectForKey(((CCInteger*)allkeys->objectAtIndex(i))->getValue());
            
            if(recipe->m_attr != DyRecipeAttr_Default && recipe->m_idShop == stageManager->m_curShopType)
            {
                if(recipe->m_attr == DyRecipeAttr_Secret)
                {
                    if(recipe->m_isAvailable || isAllReciepeHaveForSecret == false)
                    {
                        continue;
                    }
                }
                else if(!recipe->m_isAvailable)
                    isAllReciepeHaveForSecret = false;
                
                if(recipe->m_attr == DyRecipeAttr_Secret && recipe->m_isCheckedInGame == false && recipe->m_showStage <= stageManager->m_curLevel)
                {
                    recipe->m_isCheckedInGame = true;
                    recipe->m_isSawInGame = true;
                    dataManager->recipe_setSawInGame(recipe->m_id, true);
                    dataManager->dataSave();
                    
                    alramNewRecipe(recipe->m_tag, false);
                    return true;
                }
            }
        }
    }
    
    return false;
}


void DyRecipeManager::buyedRecipe(DyRecipeInfo* recipe)
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();

    YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_MSG);

    
	recipe->m_isAvailable = true;
	dataManager->recipe_setAvailable(recipe->m_id, true);
	dataManager->recipe_setSawInGame(recipe->m_id, true);

	dataManager->checkAwards(DyAwardsCheckType_Recipe, false);
}


void DyRecipeManager::addSellingCount(DyRecipeInfo* recipe)
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
//    if(recipe->m_id < DYDATA_CNT_RECIPE)
//        dataManager->m_private.m_sellcount[recipe->m_id]++;
//    else
//        dataManager->m_private.m_sellcount_added[recipe->m_id - DYDATA_CNT_RECIPE]++;
    
    dataManager->m_private.m_sellcount[recipe->m_id]++;
}


int DyRecipeManager::getSellingCount(DyRecipeInfo* recipe)
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
//    if(recipe->m_id < DYDATA_CNT_RECIPE)
//        return dataManager->m_private.m_sellcount[recipe->m_id];
//    else
//        return dataManager->m_private.m_sellcount_added[recipe->m_id - DYDATA_CNT_RECIPE];
    
    return dataManager->m_private.m_sellcount[recipe->m_id];
}


DyRecipeInfo* DyRecipeManager::getRecipe(std::string tag)
{
	CCArray* allkeys = m_dicRecipes->allKeys();

	for(unsigned int i = 0;i < allkeys->count();i++)
	{
		DyRecipeInfo* recipe = (DyRecipeInfo*)m_dicRecipes->objectForKey(((CCInteger*)allkeys->objectAtIndex(i))->getValue());

		if(recipe->m_tag.compare(tag) == 0)
			return recipe;
	}

	return NULL;
}


void DyRecipeManager::loadRecipeResource(int id_shop)
{
	DyLib* lib = DyLib::sharedLib();

	DyBlockManager* blockManager = DyBlockManager::sharedBlockManager();
	DyStageManager* stageManager = DyStageManager::sharedStageManager();

	const char* resource_block_url = stageManager->m_shopinfo->getValue("resource_block", id_shop);
	sprintf(lib->m_strTemp, "%s.plist", resource_block_url);
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(lib->m_strTemp);

	const char* resource_recipe_url = stageManager->m_shopinfo->getValue("resource_recipe", id_shop);
	sprintf(lib->m_strTemp, "%s.plist", resource_recipe_url);
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(lib->m_strTemp);

	blockManager->reloadBlockInfo(resource_block_url);
}
