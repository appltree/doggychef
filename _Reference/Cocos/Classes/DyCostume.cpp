/*
DyCostume.cpp
Doggy chef
@initialize at 121208

@history
121208	yoonsr	initialize
*/

#include "DyCostume.h"
#include "DyLoader.h"
#include "DyData.h"
#include "DyLib.h"
#include "DyTycoon.h"
#include "DyStage.h"

DyCostumeManager* g_sharedCostumeManager;


DyCostume::DyCostume()
{
	m_has = false;
}


DyCostume::~DyCostume()
{
}


void DyCostumeManager::reloadCostume()
{
    m_hasScarf06 = false;

    DyDataManager* dataManager = DyDataManager::sharedDataManager();

    YtObjectGroup* custumeInfo = DyLoader::sharedLoader()->loadObjects("costume.txt");

    m_arrCostumes->removeAllObjects();
    
    for(int i = 0;i < custumeInfo->count();i++)
	{
		DyCostume* costume = new DyCostume();
        
		costume->m_no = atoi(custumeInfo->getValue("no", i));
		costume->m_id = atoi(custumeInfo->getValue("id", i));
		costume->m_tag = custumeInfo->getValue("tag", i);
        
        costume->m_function.m_decay = atoi(custumeInfo->getValue("function_decay", i));
        costume->m_function.m_heart = atoi(custumeInfo->getValue("function_heart", i));
        costume->m_function.m_tip = atoi(custumeInfo->getValue("function_tip", i));
        costume->m_function.m_ice = atoi(custumeInfo->getValue("function_ice", i));
        costume->m_function.m_cooling = atoi(custumeInfo->getValue("function_cooling", i));
        
		const char* group_tag = custumeInfo->getValue("group_tag", i);
        
		if(strcmp(group_tag, "set") == 0)
		{
			costume->m_cosumeType = DyCosumeType_Set;
		}
		else if(strcmp(group_tag, "cloth") == 0)
		{
			costume->m_cosumeType = DyCosumeType_Cloth;
		}
		else if(strcmp(group_tag, "hat") == 0)
		{
			costume->m_cosumeType = DyCosumeType_Hat;
		}
		else if(strcmp(group_tag, "scarf") == 0)
		{
			costume->m_cosumeType = DyCosumeType_Scarf;
		}
		else if(strcmp(group_tag, "apron") == 0)
		{
			costume->m_cosumeType = DyCosumeType_Apron;
		}
		
		CCArray* arrayInfo = custumeInfo->getArray("info", i);
        
		for(unsigned int j = 0;j < arrayInfo->count();j++)
		{
			costume->m_arrInfo.push_back(((CCString*)arrayInfo->objectAtIndex(j))->getCString());
		}
        
		CCArray* arr_lock_condition = custumeInfo->getArray("lock_condition", i);
        
        std::string lock_condition = ((CCString*)arr_lock_condition->objectAtIndex(0))->getCString();
        
		if(lock_condition.compare("NULL") == 0)
		{
			costume->m_condition_type = DyCostumeLockType_NoneOrShop;
            costume->m_condition_value = 0;
            
			DyDataManager::sharedDataManager()->costume_set_saw(costume->m_no, true);
		}
        else if(lock_condition.compare("korean") == 0)
		{
			costume->m_condition_type = DyCostumeLockType_NoneOrShop;
            costume->m_condition_value = 5;
		}

        else if(lock_condition.compare("pizza") == 0)
		{
			costume->m_condition_type = DyCostumeLockType_NoneOrShop;
            costume->m_condition_value = 4;
		}
		else if(lock_condition.compare("china") == 0)
		{
			costume->m_condition_type = DyCostumeLockType_NoneOrShop;
            costume->m_condition_value = 3;
		}
        else if(lock_condition.compare("coffee") == 0)
		{
			costume->m_condition_type = DyCostumeLockType_NoneOrShop;
            costume->m_condition_value = 2;
		}
		else if(lock_condition.compare("hamburger") == 0)
		{
			costume->m_condition_type = DyCostumeLockType_NoneOrShop;
            costume->m_condition_value = 1;
		}
        else if(lock_condition.compare("gold") == 0)
		{
			costume->m_condition_type = DyCostumeLockType_Gold;
            costume->m_condition_value = atoi(((CCString*)arr_lock_condition->objectAtIndex(1))->getCString());
		}
        else if(lock_condition.compare("crazy") == 0)
        {
            costume->m_condition_type = DyCostumeLockType_Crazy;
            costume->m_condition_value = 0;
        }
        else if(lock_condition.compare("roulette") == 0)
        {
            costume->m_condition_type = DyCostumeLockType_Roulette;
            costume->m_condition_string = ((CCString*)arr_lock_condition->objectAtIndex(1))->getCString();
        }
        else if(lock_condition.compare("limit_santa") == 0)
        {
            costume->m_condition_type =  DyCostumeLockType_LimitSanta;
            costume->m_condition_value = 0;
        }
        else if(lock_condition.compare("limit_cupid") == 0)
        {
            costume->m_condition_type =  DyCostumeLockType_LimitCupid;
            costume->m_condition_value = 0;
        }

        else if(lock_condition.compare("limit_invite") == 0)
        {
            costume->m_condition_type =  DyCostumeLockType_LimitInvite;
            costume->m_condition_value = 0;
        }

        std::string function_shop = custumeInfo->getValue("function_shop", i);
        
        if(function_shop.compare("NULL") == 0)
        {
            costume->m_function_shop = -1;
        }
        else if(function_shop.compare("boonsik") == 0)
        {
            costume->m_function_shop = 0;
        }
        else if(function_shop.compare("hamburger") == 0)
        {
            costume->m_function_shop = 1;
        }
        else if(function_shop.compare("coffee") == 0)
        {
            costume->m_function_shop = 2;
        }
        else if(function_shop.compare("china") == 0)
        {
            costume->m_function_shop = 3;
        }
        else if(function_shop.compare("pizza") == 0)
        {
            costume->m_function_shop = 4;
        }
        else if(function_shop.compare("korean") == 0)
        {
            costume->m_function_shop = 5;
        }
        

		
		costume->m_resultImage = custumeInfo->getValue("resultimage", i);
        
		costume->m_name = custumeInfo->getValue("name_kor", i);
        
		costume->m_price_gold = atoi(custumeInfo->getValue("purchase_price_gold", i));
		costume->m_price_delly = atoi(custumeInfo->getValue("purchase_price_delly", i));
        
		if(dataManager->costume_get_hasitem(costume->m_no))
			costume->m_has = true;
		else
			costume->m_has = false;
        
		m_arrCostumes->addObject(costume);
		costume->release();
	}
    DyDataManager::sharedDataManager()->dataSave();
}

DyCostumeManager::DyCostumeManager()
{
	
	
    m_arrCostumes = new CCArray();
	


	reloadCostume();
}


DyCostumeManager::~DyCostumeManager()
{
	m_arrCostumes->release();
}



void DyCostumeManager::dataSave()
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	dataManager->dataSave();
}


DyCostume* DyCostumeManager::getCostume(std::string tag)
{
	for(unsigned int i = 0;i < m_arrCostumes->count();i++)
	{
		DyCostume* costume = (DyCostume*)m_arrCostumes->objectAtIndex(i);
		if(tag.compare(costume->m_tag) == 0)
			return costume;

	}
	return NULL;
}


bool DyCostumeManager::buyedCostume(DyCostume* costume)
{
	//DyLib* lib = DyLib::sharedLib();

    if(costume->m_has == true)
        return false;
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_FOOD_MSG);

	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	dataManager->costume_set_hasitem(costume->m_no, true);
	costume->m_has = true;

	if(costume->m_cosumeType == DyCosumeType_Set)
	{
		std::list<std::string>::iterator it = costume->m_arrInfo.begin();
		for(unsigned int i = 0;i < costume->m_arrInfo.size();i++)
		{
			std::string subTag = (*it);
			DyCostume* subCostume = getCostume(subTag);
			dataManager->costume_set_hasitem(subCostume->m_no, true);
			subCostume->m_has = true;
			
			it++;
		}
	}
	else
	{
		for(unsigned int i = 0;i < m_arrCostumes->count();i++)
		{
			DyCostume* costume = (DyCostume*)m_arrCostumes->objectAtIndex(i);

			if(costume->m_cosumeType == DyCosumeType_Set)
			{
				bool hasSet = true;
				std::list<std::string>::iterator it = costume->m_arrInfo.begin();
				for(unsigned int i = 0;i < costume->m_arrInfo.size();i++)
				{
					std::string subTag = (*it);
					DyCostume* subCostume = getCostume(subTag);

					if(!dataManager->costume_get_hasitem(subCostume->m_no))
					{
						hasSet = false;
						break;
					}

					it++;
				}
				if(hasSet)
				{
					dataManager->costume_set_hasitem(costume->m_no, true);
					costume->m_has = true;
				}
			}
		}
	}

	dataManager->checkAwards(DyAwardsCheckType_Costume, false);
    
    return true;
}



void DyCostumeManager::initFunction(DyCostumeFunction* function)
{
    function->m_decay = 0;
    function->m_heart = 0;
    function->m_ice = 0;
    function->m_tip = 0;
    function->m_cooling = 0;
}


void DyCostumeManager::addFunction(DyCostumeFunction* functionTarget, DyCostumeFunction functionSource)
{
    functionTarget->m_decay += functionSource.m_decay;
    functionTarget->m_heart += functionSource.m_heart;
    functionTarget->m_ice += functionSource.m_ice;
    functionTarget->m_tip += functionSource.m_tip;
    functionTarget->m_cooling += functionSource.m_cooling;
}


void DyCostumeManager::setCostume(YtAniObject* doggy, DyCostumeStatus* status, DyCostumeFunction* function)
{
	DyLib* lib = DyLib::sharedLib();

	_setCostume(doggy, DyCosumeType_Set, status->m_indexSet);

	int setCloth = 0;
	int setHat = 0;
	int setScarf = 0;
	int setApron = 0;

	if(status->m_indexSet > 0)
	{
		sprintf(lib->m_strTemp, "set_%02d", status->m_indexSet);
		DyCostume* costume = getCostume(lib->m_strTemp);
		std::list<std::string>::iterator it = costume->m_arrInfo.begin();
		for(unsigned int i = 0;i < costume->m_arrInfo.size();i++)
		{
			std::string subTag = (*it);
			DyCostume* subCostume = getCostume(subTag);

			switch(subCostume->m_cosumeType)
			{
				case DyCosumeType_Cloth:
					setCloth = subCostume->m_id;
					break;
				case DyCosumeType_Hat:
					setHat = subCostume->m_id;
					break;
				case DyCosumeType_Scarf:
					setScarf = subCostume->m_id;
					break;
				case DyCosumeType_Apron:
					setApron = subCostume->m_id;
					break;
                default:
                    break;
			}
			it++;
		}
	}

    
	if(status->m_indexCloth == 0 && setCloth > 0)
		status->m_indexCloth = setCloth;
	_setCostume(doggy, DyCosumeType_Cloth, status->m_indexCloth);
	if(status->m_indexHat == 0 && setHat > 0)
		status->m_indexHat = setHat;
	_setCostume(doggy, DyCosumeType_Hat, status->m_indexHat);

	if(status->m_indexScarf == 0 && setScarf > 0)
		status->m_indexScarf = setScarf;
	_setCostume(doggy, DyCosumeType_Scarf, status->m_indexScarf);

	if(status->m_indexApron == 0 && setApron > 0)
		status->m_indexApron = setApron;
	_setCostume(doggy, DyCosumeType_Apron, status->m_indexApron);


    //
    // 개별 옷으로 셋을 설정
    setCloth = 0;
    setHat = 0;
    setScarf = 0;
    setApron = 0;
    
	//
	// set check
	status->m_indexSet = 0;
	for(unsigned int i = 0;i < m_arrCostumes->count();i++)
	{
		DyCostume* costume = (DyCostume*)m_arrCostumes->objectAtIndex(i);

		if(costume->m_cosumeType == DyCosumeType_Set)
		{
			std::list<std::string>::iterator it = costume->m_arrInfo.begin();
			bool has = true;

			for(unsigned int j = 0;j < costume->m_arrInfo.size();j++)
			{
				std::string subTag = (*it);
				DyCostume* subCostume = getCostume(subTag);

				switch(subCostume->m_cosumeType)
				{
					case DyCosumeType_Cloth:
						if(subCostume->m_id != status->m_indexCloth)
                        {
                          	has = false;
                            //setCloth = status->m_indexCloth;
                        }
						
						break;
					case DyCosumeType_Hat:
						if(subCostume->m_id != status->m_indexHat)
                        {
                            has = false;
                            //setHat = status->m_indexHat;
                        }
                        
						break;
					case DyCosumeType_Scarf:
						if(subCostume->m_id != status->m_indexScarf)
                        {
                            has = false;
                            //setScarf = status->m_indexScarf;
                        }
                        
						break;
					case DyCosumeType_Apron:
						if(subCostume->m_id != status->m_indexApron)
                        {
                            has = false;
                            //setApron = status->m_indexApron;
                        }
						break;
                    default:
                        break;
				}
				it++;
			}

			if(has)
			{
				status->m_indexSet = costume->m_id;
                
                
                sprintf(lib->m_strTemp, "set_%02d", status->m_indexSet);
                DyCostume* costume = getCostume(lib->m_strTemp);
                std::list<std::string>::iterator it = costume->m_arrInfo.begin();
                for(unsigned int i = 0;i < costume->m_arrInfo.size();i++)
                {
                    std::string subTag = (*it);
                    DyCostume* subCostume = getCostume(subTag);
                    
                    switch(subCostume->m_cosumeType)
                    {
                        case DyCosumeType_Cloth:
                            setCloth = subCostume->m_id;
                            break;
                        case DyCosumeType_Hat:
                            setHat = subCostume->m_id;
                            break;
                        case DyCosumeType_Scarf:
                            setScarf = subCostume->m_id;
                            break;
                        case DyCosumeType_Apron:
                            setApron = subCostume->m_id;
                            break;
                        default:
                            break;
                    }
                    it++;
                }

                
                
				break;
			}
		}
	}
    
	// function
    initFunction(function);
  	if(status->m_indexSet > 0)
	{
		sprintf(lib->m_strTemp, "set_%02d", status->m_indexSet);
		DyCostume* costume = getCostume(lib->m_strTemp);
		addFunction(function, costume->m_function);
        
        if(status->m_indexCloth > 0 && setCloth == 0)
        {
            sprintf(lib->m_strTemp, "cloth_%02d", status->m_indexCloth);
            DyCostume* costume = getCostume(lib->m_strTemp);
            addFunction(function, costume->m_function);
        }
        
        if(status->m_indexApron > 0 && setApron == 0)
        {
            sprintf(lib->m_strTemp, "apron_%02d", status->m_indexApron);
            DyCostume* costume = getCostume(lib->m_strTemp);
            addFunction(function, costume->m_function);
        }
        
        if(status->m_indexHat > 0 && setHat == 0)
        {
            sprintf(lib->m_strTemp, "hat_%02d", status->m_indexHat);
            DyCostume* costume = getCostume(lib->m_strTemp);
            addFunction(function, costume->m_function);
        }
        
        if(status->m_indexScarf > 0 && setScarf == 0)
        {
            sprintf(lib->m_strTemp, "scarf_%02d", status->m_indexScarf);
            DyCostume* costume = getCostume(lib->m_strTemp);
            addFunction(function, costume->m_function);
        }

	}
    else
    {
        if(status->m_indexCloth > 0)
        {
            sprintf(lib->m_strTemp, "cloth_%02d", status->m_indexCloth);
            DyCostume* costume = getCostume(lib->m_strTemp);
            addFunction(function, costume->m_function);
        }
        
        if(status->m_indexApron > 0)
        {
            sprintf(lib->m_strTemp, "apron_%02d", status->m_indexApron);
            DyCostume* costume = getCostume(lib->m_strTemp);
            addFunction(function, costume->m_function);
        }

        if(status->m_indexHat > 0)
        {
            sprintf(lib->m_strTemp, "hat_%02d", status->m_indexHat);
            DyCostume* costume = getCostume(lib->m_strTemp);
            addFunction(function, costume->m_function);
        }

        if(status->m_indexScarf > 0)
        {
            sprintf(lib->m_strTemp, "scarf_%02d", status->m_indexScarf);
            DyCostume* costume = getCostume(lib->m_strTemp);
            addFunction(function, costume->m_function);
        }
    }
}


void DyCostumeManager::_setCostume2(YtAniObject* doggy, std::string tag)
{
	DyCostume* costume = getCostume(tag);

	_setCostume(doggy, costume->m_cosumeType, costume->m_id);
}



bool DyCostumeManager::hasCostume(std::string tag)
{
    DyCostume* costume = getCostume(tag);
    
    if(costume == NULL)
        return false;
    return costume->m_has;
}

void DyCostumeManager::clearCostumeStatus(DyCostumeStatus* status)
{
	status->m_indexSet = 0;
	status->m_indexCloth = 0;
	status->m_indexHat = 0;
	status->m_indexScarf = 0;
	status->m_indexApron = 0;
}


void DyCostumeManager::setCostumeStatus(DyCostumeStatus* status, std::string tag)
{
	//DyLib* lib = DyLib::sharedLib();
	DyCostume* costume = getCostume(tag);

    if(costume == NULL)
        return;
    
	switch(costume->m_cosumeType)
	{
		case DyCosumeType_Set:
			status->m_indexSet = costume->m_id;
			break;
		case DyCosumeType_Cloth:
			status->m_indexCloth = costume->m_id;
			break;
		case DyCosumeType_Hat:
			status->m_indexHat = costume->m_id;
			break;
		case DyCosumeType_Scarf:
			status->m_indexScarf = costume->m_id;
			break;
		case DyCosumeType_Apron:
			status->m_indexApron = costume->m_id;
			break;
	}
}


void DyCostumeManager::setCostumeGameFunction(DyCostumeFunction* function)
{
    //DyStageManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    DyStageManager* stageManager = DyStageManager::sharedStageManager();

    DyCostumeStatus status = DyDataManager::sharedDataManager()->m_public.m_costume_status;
    DyLib* lib = DyLib::sharedLib();
    
    int setCloth = 0;
	int setHat = 0;
	int setScarf = 0;
	int setApron = 0;
    
	if(status.m_indexSet > 0)
	{
		sprintf(lib->m_strTemp, "set_%02d", status.m_indexSet);
		DyCostume* costume = getCostume(lib->m_strTemp);
		std::list<std::string>::iterator it = costume->m_arrInfo.begin();
		for(unsigned int i = 0;i < costume->m_arrInfo.size();i++)
		{
			std::string subTag = (*it);
			DyCostume* subCostume = getCostume(subTag);
            
			switch(subCostume->m_cosumeType)
			{
				case DyCosumeType_Cloth:
					setCloth = subCostume->m_id;
					break;
				case DyCosumeType_Hat:
					setHat = subCostume->m_id;
					break;
				case DyCosumeType_Scarf:
					setScarf = subCostume->m_id;
					break;
				case DyCosumeType_Apron:
					setApron = subCostume->m_id;
					break;
                default:
                    break;
			}
			it++;
		}
	}

    // function
    initFunction(function);
    
  	if(status.m_indexSet > 0)
	{
		sprintf(lib->m_strTemp, "set_%02d", status.m_indexSet);
		DyCostume* costume = getCostume(lib->m_strTemp);
        
        if(costume->m_function_shop == -1 || costume->m_function_shop == stageManager->m_curShopType)
            addFunction(function, costume->m_function);
        
        if(status.m_indexCloth > 0 && status.m_indexCloth != setCloth)
        {
            sprintf(lib->m_strTemp, "cloth_%02d", status.m_indexCloth);
            DyCostume* costume = getCostume(lib->m_strTemp);
            if(costume->m_function_shop == -1 || costume->m_function_shop == stageManager->m_curShopType)
                addFunction(function, costume->m_function);
        }
        
        if(status.m_indexApron > 0 && status.m_indexApron != setApron)
        {
            sprintf(lib->m_strTemp, "apron_%02d", status.m_indexApron);
            DyCostume* costume = getCostume(lib->m_strTemp);
            
            if(costume->m_function_shop == -1 || costume->m_function_shop == stageManager->m_curShopType)
                addFunction(function, costume->m_function);
        }
        
        if(status.m_indexHat > 0 && status.m_indexHat != setHat)
        {
            sprintf(lib->m_strTemp, "hat_%02d", status.m_indexHat);
            DyCostume* costume = getCostume(lib->m_strTemp);
            if(costume->m_function_shop == -1 || costume->m_function_shop == stageManager->m_curShopType)
                addFunction(function, costume->m_function);
            
        }
        
        if(status.m_indexScarf > 0 && status.m_indexScarf != setScarf)
        {
            sprintf(lib->m_strTemp, "scarf_%02d", status.m_indexScarf);
            DyCostume* costume = getCostume(lib->m_strTemp);
            if(costume->m_function_shop == -1 || costume->m_function_shop == stageManager->m_curShopType)
                addFunction(function, costume->m_function);
        }
	}
    else
    {
        if(status.m_indexCloth > 0)
        {
            sprintf(lib->m_strTemp, "cloth_%02d", status.m_indexCloth);
            DyCostume* costume = getCostume(lib->m_strTemp);
            if(costume->m_function_shop == -1 || costume->m_function_shop == stageManager->m_curShopType)
                addFunction(function, costume->m_function);
        }
        
        if(status.m_indexApron > 0)
        {
            sprintf(lib->m_strTemp, "apron_%02d", status.m_indexApron);
            DyCostume* costume = getCostume(lib->m_strTemp);
            
            if(costume->m_function_shop == -1 || costume->m_function_shop == stageManager->m_curShopType)
                addFunction(function, costume->m_function);
        }
        
        if(status.m_indexHat > 0)
        {
            sprintf(lib->m_strTemp, "hat_%02d", status.m_indexHat);
            DyCostume* costume = getCostume(lib->m_strTemp);
            if(costume->m_function_shop == -1 || costume->m_function_shop == stageManager->m_curShopType)
                addFunction(function, costume->m_function);
            
        }
        
        if(status.m_indexScarf > 0)
        {
            sprintf(lib->m_strTemp, "scarf_%02d", status.m_indexScarf);
            DyCostume* costume = getCostume(lib->m_strTemp);
            if(costume->m_function_shop == -1 || costume->m_function_shop == stageManager->m_curShopType)
                addFunction(function, costume->m_function);
        }
    }
    
    //log(*function);
}


void DyCostumeManager::log(DyCostumeFunction function)
{
//    CCLog("------------------------------------------------------");
//    CCLog("function.m_decay = %d", function.m_decay);
//    CCLog("function.m_ice = %d", function.m_ice);
//    CCLog("function.m_tip = %d", function.m_tip);
//    CCLog("function.m_heart = %d", function.m_heart);
//    CCLog("function.m_cooling = %d", function.m_cooling);
}


void DyCostumeManager::_setCostume(YtAniObject* doggy, DyCosumeType type, int index)
{
	DyLib* lib = DyLib::sharedLib();
	//DyDataManager* dataManager = DyDataManager::sharedDataManager();

    if(doggy == NULL)
        return;
    
	switch(type)
	{
		case DyCosumeType_Set:
			if(index == 0)
			{
				_setCostume(doggy, DyCosumeType_Cloth, 0);
				_setCostume(doggy, DyCosumeType_Hat, 0);
				_setCostume(doggy, DyCosumeType_Scarf, 0);
				_setCostume(doggy, DyCosumeType_Apron, 0);
			}
			else
			{
				sprintf(lib->m_strTemp, "set_%02d", index);
				DyCostume* costume = getCostume(lib->m_strTemp);

				std::list<std::string>::iterator it = costume->m_arrInfo.begin();
				for(unsigned int i = 0;i < costume->m_arrInfo.size();i++)
				{
					std::string subTag = (*it);
					DyCostumeManager::sharedCostumeManager()->_setCostume2(doggy, subTag);
					it++;
				}
			}
			break;
		case DyCosumeType_Cloth:
			{
				CCSprite* sprBody = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_BODY);
				CCSprite* sprCafe = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_CAFE);

				CCSprite* sprArmLeft = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_ARM_LEFT);
				CCSprite* sprArmRight = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_ARM_RIGHT);
				CCSprite* sprArmRight_out = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_ARM_OUT_RIGHT);

				CCSprite* sprLegLeft = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_LEG_LEFT);
				CCSprite* sprLegRight = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_LEG_RIGHT);
				CCSprite* sprLegOutRight = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_LEG_OUT_RIGHT);

				if(index == 0)
				{
					sprBody->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("doggy_body.png"));
					sprArmLeft->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("doggy_arm_left.png"));
					sprArmRight->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("doggy_arm.png"));
					sprArmRight_out->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("doggy_arm.png"));
					sprLegLeft->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("doggy_leg_09.png"));
					sprLegRight->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("doggy_leg_09.png"));
					sprLegOutRight->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("doggy_leg_09.png"));
					sprCafe->setVisible(false);
					m_hasNeck = false;
				}
				else
				{
					sprintf(lib->m_strTemp, "cloth_%02d", index);
					DyCostume* costume = getCostume(lib->m_strTemp);

					sprintf(lib->m_strTemp, "doggy_body_%02d.png", index);
					sprBody->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));

					sprintf(lib->m_strTemp, "doggy_arm_left_%02d.png", index);
					sprArmLeft->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));
					
					sprintf(lib->m_strTemp, "doggy_arm_%02d.png", index);
					sprArmRight->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));
					sprArmRight_out->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));

					sprintf(lib->m_strTemp, "doggy_leg_%02d.png", index);
					sprLegLeft->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));
					sprLegRight->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));
					sprLegOutRight->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));

					std::list<std::string>::iterator it = costume->m_arrInfo.begin();
					it++;

					std::string neck = (*it);

					if(neck.compare("neck") == 0)
					{
						m_hasNeck = true;
					}	
					else
						m_hasNeck = false;

					if(costume->m_arrInfo.size() > 2)
					{
						it++;

						std::string urlCafe = (*it);
						sprCafe->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(urlCafe.c_str()));
						sprCafe->setVisible(true);
					}
					else
						sprCafe->setVisible(false);
				}
                
                doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_BODY_BACK)->setVisible(index == 18);
                

				if(m_hasScarf06)
				{
					_setCostume(doggy, DyCosumeType_Scarf, 6);
				}
			}
			break;
		case DyCosumeType_Hat:
			{
				CCSprite* sprFace = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_FACE);
				CCSprite* sprHat = (CCSprite*)sprFace->getChildByTag(COSTUME_UI_TAG_HAT);
                CCSprite* sprHatBack = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_HAT_BACK);

				CCSprite* sprFace_title = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_MAP_FACE);
				CCSprite* sprHat_title = (CCSprite*)sprFace_title->getChildByTag(CPSTUME_UI_TAG_MAP_HAT);
                CCSprite* sprHatBack_title = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(CPSTUME_UI_TAG_MAP_HAT_BACK);
                
                
                CCSprite* sprHatBack2 = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_HAT_BACK2);
                CCSprite* sprHatBack_title2 = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(CPSTUME_UI_TAG_MAP_HAT_BACK2);
                
                
                sprHatBack->setVisible(false);
                sprHatBack_title->setVisible(false);

                sprHatBack2->setVisible(false);
                sprHatBack_title2->setVisible(false);
                
                
                
				if(index == 0)
				{
					sprHat->setVisible(false);
					sprHat_title->setVisible(false);
                    
				}
				else
				{
					sprintf(lib->m_strTemp, "doggy_hat_%02d.png", index);
					sprHat->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));
					sprHat->setVisible(true);

					sprHat_title->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));
					sprHat_title->setVisible(true);
                    
                    if(index == 19)
                    {
                        sprHatBack->setVisible(true);
                        sprHatBack_title->setVisible(true);
 
                    }
                    
                    if(index == 24)
                    {
                        sprHatBack->setVisible(false);
                        sprHatBack_title->setVisible(false);
                        
                        sprHatBack2->setVisible(true);
                        sprHatBack_title2->setVisible(true);
                    }

				}
			}
			break;
		case DyCosumeType_Scarf:
			{
				CCSprite* sprScarp = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_SCARP);

                doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_FACE)->getChildByTag(COSTUME_UI_TAG_TUL)->setVisible(false);
                doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_MAP_FACE)->getChildByTag(CPSTUME_UI_TAG_MAP_TUL)->setVisible(false);

                doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_SCARP_BACK)->setVisible(false);

                
				if(index == 0)
				{
					sprScarp->setVisible(false);
					m_hasScarf06 = false;
				}
				else if(index == 6)
				{
					if(m_hasNeck)
						sprintf(lib->m_strTemp, "doggy_scarp_06_2.png");
					else
						sprintf(lib->m_strTemp, "doggy_scarp_06.png");
					sprScarp->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));
					sprScarp->setVisible(true);
					m_hasScarf06 = true;
				}
                else if(index == 12)
                {
                    doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_FACE)->getChildByTag(COSTUME_UI_TAG_TUL)->setVisible(true);
                    doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_MAP_FACE)->getChildByTag(CPSTUME_UI_TAG_MAP_TUL)->setVisible(true);
                    sprScarp->setVisible(false);
                    m_hasScarf06 = false;
                }
                else if(index == 15)
                {
                    doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_SCARP_BACK)->setVisible(true);
                    doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_SCARP)->setVisible(false);
                }
				else
				{
					sprintf(lib->m_strTemp, "doggy_scarp_%02d.png", index);
					sprScarp->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));
					sprScarp->setVisible(true);
					m_hasScarf06 = false;
				}

				CCSprite* sprCafe = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_CAFE2);

				sprintf(lib->m_strTemp, "scarf_%02d", index);
				DyCostume* costume = getCostume(lib->m_strTemp);

				if(costume && costume->m_arrInfo.size() > 1)
				{
					std::list<std::string>::iterator it = costume->m_arrInfo.begin();
					it++;

					std::string urlCafe = (*it);
					sprCafe->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(urlCafe.c_str()));
					sprCafe->setVisible(true);
				}
				else
					sprCafe->setVisible(false);
                
                


			}
			break;
		case DyCosumeType_Apron:
			{
				CCSprite* sprApron = (CCSprite*)doggy->getChildByTag(COSTUME_UI_TAG_MAIN)->getChildByTag(COSTUME_UI_TAG_APRON);

				if(index == 0)
				{
					sprApron->setVisible(false);
				}
				else
				{
					sprintf(lib->m_strTemp, "doggy_apron_%02d.png", index);
					sprApron->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(lib->m_strTemp));
					sprApron->setVisible(true);
				}
			}
			break;
	}
}


void DyCostumeManager::setDefaultCostumeData(DyCostumeStatus* status, DyCostumeStatus* status_ex)
{
    memset(status_ex, 0, sizeof(DyCostumeStatus));
    memset(status, 0, sizeof(DyCostumeStatus));
    status->m_indexApron = 2;
}


DyCostumeManager* DyCostumeManager::sharedCostumeManager()
{
	if(g_sharedCostumeManager == NULL)
	{
		g_sharedCostumeManager = new DyCostumeManager();
	}
	return g_sharedCostumeManager;
}

