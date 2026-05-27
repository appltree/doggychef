/*
DyStage.cpp
Doggy chef
@initialize at 121008

@history
121008	yoonsr	initialize
*/

#include "DyStage.h"
#include "DyLoader.h"
#include "YtLib.h"
#include "DyFriend.h"
#include "DyMachine.h"

DyStageManager* g_sharedStageManager;


DyStageManager::DyStageManager()
{
	m_curShopInfo = NULL;
	m_cntShop = SHOP_COUNT_DEFAULT + SHOP_COUNT_ADDED;

	m_shopinfo = DyLoader::sharedLoader()->loadObjects("shop.txt");
	m_shopinfo->retain();
    m_curShopType = (DyStageType)-1;
}


DyStageManager::~DyStageManager()
{
	m_shopinfo->release();

	if(m_curShopInfo)
	{
		m_curShopInfo->release();
		m_curShopInfo = NULL;
	}
}


void DyStageManager::setupShopUI(CCLayer* layer)
{
    DyMachineManager* machineManager = DyMachineManager::sharedMachineManager();
    machineManager->setupMachineUI(layer, m_curShopType);
    
}


DyStageType DyStageManager::setShop(DyStageType shopType)
{
    DyStageType prevStage = m_curShopType;
    
    if(shopType < 0)
        return prevStage;
    
    if(m_curShopType == shopType)
        return prevStage;
    
	m_curShopType = shopType;
	const char *data_level = m_shopinfo->getValue("data_level", m_curShopType);

	if(m_curShopInfo != NULL)
		m_curShopInfo->release();

	m_curShopInfo = DyLoader::sharedLoader()->loadObjects(data_level);
	m_curShopInfo->retain();

    m_cntBonusLevel = atoi(m_shopinfo->getValue("count_level_bonus", m_curShopType));
	m_cntLevel = atoi(m_shopinfo->getValue("count_level", m_curShopType));
    
    return prevStage;
}


void DyStageManager::setLevel(int level)
{
	m_curLevel = level;
}


bool DyStageManager::isShopOpen(int shop)
{
	return getStageState(shop, 0) > DyStageState_Lock;
}


bool DyStageManager::isStageClear(int shop)
{
	int cntLevel = atoi(m_shopinfo->getValue("count_level", shop));
	bool isAllClear = true;

	for(int i = 0;i < cntLevel;i++)
	{
		if(getStageState(shop, i) <= DyStageState_Fail)
		{
			isAllClear = false;
			break;
		}
	}
	return isAllClear;
}


void DyStageManager::buyedShop(int shop)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_MSG);
    setStageState(shop, 0, DyStageState_Fail);
}


int DyStageManager::getCountLevel(int shop)
{
	return atoi(m_shopinfo->getValue("count_level", shop)) + atoi(m_shopinfo->getValue("count_level_bonus", shop));
}



int DyStageManager::getCurrentShop()
{
	for(int i = SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED-1;i >= 0;i--)
	{
		if(isShopOpen(i))
			return i;
	}
	return SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;
}


std::string DyStageManager::getShopName(int shop)
{
	return m_shopinfo->getValue("name_kor", shop);
}


int DyStageManager::getCurrentLevel(int shop)
{
    int countLevel = getCountLevel(shop);
    
	//DyDataManager* dataManager = DyDataManager::sharedDataManager();
	for(int i = 0;i < countLevel;i++)
	{
		if(getStageState(shop, i) == DyStageState_Lock)
			return i-1;
        
        if(i == countLevel-1)
            return countLevel-1;
	}
	return -1;
}


void DyStageManager::nextStageOpen()
{
	if(!isLastLevel())
	{
		if(getStageState(m_curShopType, m_curLevel + 1) == DyStageState_Lock)
            setStageState(m_curShopType, m_curLevel + 1, DyStageState_Fail);
	}
}

bool DyStageManager::isLastLevel()
{
    return m_curLevel == m_cntBonusLevel + m_cntLevel - 1;
}


bool DyStageManager::isNextLevelOpen()
{
  	if(!isLastLevel())
	{
		if(getStageState(m_curShopType, m_curLevel + 1) == DyStageState_Lock)
			return false;
        else
            return true;
	}
    
    return false;
}


bool DyStageManager::isCrazyOpenLeastOne()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    bool ret = false;
    for(int i = 0;i < m_cntShop;i++)
	{
		if(dataManager->shop_get_crazyopen(i))
        {
            ret = true;
            break;
        }
    }
    return ret;
}


void DyStageManager::calculateTotalPoint()
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();

    unsigned int totalPoint = 0;
	for(int i = 0;i < m_cntShop;i++)
	{
        int countLevel = getCountLevel(i);
        
		for(int j = 0;j < countLevel;j++)
		{
			totalPoint += dataManager->shop_get_record(i, j);
		}
	}

	dataManager->m_public.m_stageRecord = totalPoint;
}



void DyStageManager::getCurrentStage(DyDataPublic* data, int* input_shop, int* input_stage)
{
    *input_shop = data->m_max_shop;
    *input_stage = data->m_max_stage;
}

void DyStageManager::setCurrentStage(DyDataPublic* data, int input_shop, int input_stage)
{
    data->m_max_shop = input_shop;
    data->m_max_stage = input_shop;
}


DyStageState DyStageManager::getStageState(int shop, int stage)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    unsigned int tgt = dataManager->m_private.m_stage_status[shop*6 + stage/10];
   
    int offset = stage%10;
    
    DyStageState state = (DyStageState)((tgt>>(offset*3))&0b111);
    
    return state;

}


void DyStageManager::setStageState(int shop, int stage, DyStageState state)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    unsigned int tgt = dataManager->m_private.m_stage_status[shop*6 + stage/10];
    
    int offset = stage%10;
    unsigned int tgt_tgt;
    
    switch(offset)
    {
        case 0:
            tgt_tgt = (tgt&0b11111111111111111111111111111000) | (state&0b111);
            break;
        case 1:
            tgt_tgt = (tgt&0b11111111111111111111111111000111) | ((state&0b111)<<3);
            break;
        case 2:
            tgt_tgt = (tgt&0b11111111111111111111111000111111) | ((state&0b111)<<6);
            break;
        case 3:
            tgt_tgt = (tgt&0b11111111111111111111000111111111) | ((state&0b111)<<9);
            break;
        case 4:
            tgt_tgt = (tgt&0b11111111111111111000111111111111) | ((state&0b111)<<12);
            break;
        case 5:
            tgt_tgt = (tgt&0b11111111111111000111111111111111) | ((state&0b111)<<15);
            break;
        case 6:
            tgt_tgt = (tgt&0b11111111111000111111111111111111) | ((state&0b111)<<18);
            break;
        case 7:
            tgt_tgt = (tgt&0b11111111000111111111111111111111) | ((state&0b111)<<21);
            break;
        case 8:
            tgt_tgt = (tgt&0b11111000111111111111111111111111) | ((state&0b111)<<24);
            break;
        case 9:
            tgt_tgt = (tgt&0b11000111111111111111111111111111) | ((state&0b111)<<27);
            break;
    }
    
    dataManager->m_private.m_stage_status[shop*6 + stage/10] = tgt_tgt;
    
    bool isFinded = false;
    
    for(int index = SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED-1;index >= 0;index--)
    {
        int cntLevel = this->getCountLevel(index);
        
        for(int level = cntLevel-1;level >= 0;level--)
        {
            DyStageState stage = getStageState(index, level);
            
            if(!isFinded && stage > DyStageState_Lock)
            {
                dataManager->m_public.m_max_shop = index;
                dataManager->m_public.m_max_stage = level;
                isFinded = true;
            }
        }
    }

    
    //
    // 하위 버전을 위해 저장
    if(shop <= 3 && stage < 30)
    {
        unsigned int tgt;
        
        if(shop < 3)
        {
            tgt = dataManager->m_public.m_stage_status[shop*3 + stage/10];
        }
        else
        {
            tgt = dataManager->m_public.m_stage_status_added[(shop-3)*3 + stage/10];
        }
        
        int offset = stage%10;
        unsigned int tgt_tgt;
        
        switch(offset)
        {
            case 0:
                tgt_tgt = (tgt&0b11111111111111111111111111111000) | (state&0b111);
                break;
            case 1:
                tgt_tgt = (tgt&0b11111111111111111111111111000111) | ((state&0b111)<<3);
                break;
            case 2:
                tgt_tgt = (tgt&0b11111111111111111111111000111111) | ((state&0b111)<<6);
                break;
            case 3:
                tgt_tgt = (tgt&0b11111111111111111111000111111111) | ((state&0b111)<<9);
                break;
            case 4:
                tgt_tgt = (tgt&0b11111111111111111000111111111111) | ((state&0b111)<<12);
                break;
            case 5:
                tgt_tgt = (tgt&0b11111111111111000111111111111111) | ((state&0b111)<<15);
                break;
            case 6:
                tgt_tgt = (tgt&0b11111111111000111111111111111111) | ((state&0b111)<<18);
                break;
            case 7:
                tgt_tgt = (tgt&0b11111111000111111111111111111111) | ((state&0b111)<<21);
                break;
            case 8:
                tgt_tgt = (tgt&0b11111000111111111111111111111111) | ((state&0b111)<<24);
                break;
            case 9:
                tgt_tgt = (tgt&0b11000111111111111111111111111111) | ((state&0b111)<<27);
                break;
        }
        
        if(shop < 3)
        {
            dataManager->m_public.m_stage_status[shop*3 + stage/10] = tgt_tgt;
        }
        else
        {
            dataManager->m_public.m_stage_status_added[(shop-3)*3 + stage/10] = tgt_tgt;
        }
    }
}


void DyStageManager::stageSave()
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	dataManager->dataSave();
    
    DyFriendManager::sharedFriendManager()->updateMine();
}



DyStageManager* DyStageManager::sharedStageManager()
{
	if(g_sharedStageManager == NULL)
	{
		g_sharedStageManager = new DyStageManager();
	}
	return g_sharedStageManager;
}


