/*
DyData.cpp
Doggy chef
@initialize at 121224

@history
121224	yoonsr	initialize
*/

#include "YtLib.h"
#include "DyData.h"
#include "DyMoney.h"
#include "DyStage.h"
#include "DyLib.h"
#include "DyLayerShop.h"
#include "YtLayer.h"
#include "DyToast.h"
#include "CCDate.h"
#include "DyGuest.h"
#include "SInt.h"
#include "DyCostume.h"

#include "Rijndael.h"

#include "pugixml.hpp"
#include "json.h"


#define SECRET_KEY      "5234329812323269"

char g_tmpBuffer[1024];

DyDataManager* g_sharedDataManager;

DyDataManager::DyDataManager()
{
//    CCLog("sizeof(DyDataLocal) = %d", sizeof(DyDataLocal));
//    CCLog("sizeof(DyDataPublic) = %d", sizeof(DyDataPublic));
//    CCLog("sizeof(DyDataPrivate) = %d", sizeof(DyDataPrivate));

    //m_isGuestLogin = false;
    
    m_base64Temp = (char*)malloc(1600);
	dataLoad_local();
    
#ifdef ANDROID
    m_platform = "android";
#else
    m_platform = "ios";
#endif
    
    m_is_need_checkevent = false;
    m_rewardedGum = 0;
    //m_is_need_reset = false;
    m_is_need_privatedata_migration = false;
    
    m_cntRoulletPerDay = 5;
    m_cntPovabilityHero = 5;
    
    m_arrShopPoint_forMigration = new CCArray();
    for(int i = 0;i < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;i++)
    {
        m_arrShopPoint_forMigration->addObject(CCArray::create());
    }

    
   
//    const char *pPath = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("temp.xml");
//    pugi::xml_document doc;
//    doc.load_file(pPath);
//    pugi::xml_node root = doc.child("ROOT");
//    
//    for (pugi::xml_node row = root.first_child(); row; row = row.next_sibling())
//	{
//        int index = 0;
//        std::string no;
//        std::string user_id;
//        std::string cur_shop;
//        std::string cur_stage;
//        std::string stage_record;
//        std::string crazy_record;
//        std::string crazy_record_best;
//        std::string crazy_reg_date;
//        std::string crazy_last_date;
//        //std::string private_date;
//        //std::string public_data;
//
//        DyDataPublic _public;
//        DyDataPrivate _private;
//        
//        
//        for (pugi::xml_node field = row.first_child(); field; field = field.next_sibling())
//        {
//            switch(index)
//            {
//                case 0:
//                    no = field.text().as_string();
//                    break;
//                case 1:
//                    user_id = field.text().as_string();
//                    break;
//
//                case 2:
//                    cur_shop = field.text().as_string();
//                    break;
//
//                case 3:
//                    cur_stage = field.text().as_string();
//                    break;
//
//                case 4:
//                    stage_record = field.text().as_string();
//                    break;
//
//                case 5:
//                    crazy_record = field.text().as_string();
//                    break;
//
//                case 6:
//                    crazy_record_best = field.text().as_string();
//                    break;
//
//                case 7:
//                    crazy_reg_date = field.text().as_string();
//                    break;
//
//                case 8:
//                    crazy_last_date = field.text().as_string();
//                    break;
//
//                case 9:
//                    dataLoad_private(&_private, field.text().as_string());
//                    break;
//                case 10:
//                    dataLoad_public(&_public, field.text().as_string(), false);
//                    break;
//            }
//            
//            index++;
//        }
//        
//        int star_cnt[3] = {0, };
//        
//        DyStageManager* stageManager = DyStageManager::sharedStageManager();
//        
//        for(int shop = 0;shop < 3;shop++)
//        {
//            for(int stage = 0;stage < 30;stage++)
//            {
//                switch(stageManager->getStageState(&_public, shop, stage))
//                {
//                    case DyStageState_Fail:
//                        break;
//                    case DyStageState_Goal:
//                    {
//                        star_cnt[shop]++;
//                    }
//                        break;
//                    case DyStageState_Goal_Plus:
//                    {
//                        star_cnt[shop] += 2;
//                    }
//                        break;
//                    case DyStageState_Perfect:
//                    {
//                        star_cnt[shop] += 3;
//                    }
//                        break;
//                    default:
//                        break;
//                }
//            }
//        }
//        
//        
//        int cntRunRoulette = _private.m_cntRunRoulette;
//        int cntGuest = _private.m_cntGuest;
//        int cntInvite = _private.m_cntInvite;
//        
//        int delly = SInt(_private.m_delly).get();
//        int gold = SInt(_private.m_gold).get();
//
//        
//        
//        CCLog("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",no.c_str(),user_id.c_str(),cur_shop.c_str(),cur_stage.c_str(),stage_record.c_str(),crazy_record.c_str(),crazy_record_best.c_str(),crazy_reg_date.c_str(),crazy_last_date.c_str(),star_cnt[0],star_cnt[1],star_cnt[2], cntRunRoulette, cntGuest, cntInvite, delly, gold);
//        
//	}
}


DyDataManager::~DyDataManager()
{
    free(m_base64Temp);
	dataSave();
    m_arrShopPoint_forMigration->release();
}


void DyDataManager::dataSave_local()
{
    int size = sizeof(m_local);
    
    if(size%16 != 0)
        size += (16 - size%16);

    CRijndael oRijndael;
    oRijndael.MakeKey(SECRET_KEY, CRijndael::sm_chain0, 16, 16);
    oRijndael.Decrypt( (char *)&m_local, g_tmpBuffer, size, CRijndael::ECB);

    
    YtLib::fileWrite(DATA_FILE_DATA, g_tmpBuffer, size);
}





void DyDataManager::dataSave()
{
    dataSave_local();
}


CCDate DyDataManager::getCurrentDate()
{
    CCDate dateCurrent;
    
    return (dateCurrent + m_time_diff);
}


std::string DyDataManager::getRefreshRemainDate()
{
    DyLib* lib = DyLib::sharedLib();
    
    CCDate dataCurrent = getCurrentDate();
    CCDate dataDiff = m_time_refresh - dataCurrent;
 
    sprintf(lib->m_strTemp, "남은 시간 : %d일 %02d:%02d:%02d", dataDiff.getDay(), dataDiff.getHour(), dataDiff.getMin(), dataDiff.getSec());
    
    return lib->m_strTemp;
}


bool DyDataManager::dataLoad_local()
{	
	if(YtLib::fileExist(DATA_FILE_DATA))
	{
        int size = sizeof(m_local);
        
        if(size%16 != 0)
            size += (16 - size%16);
        
        YtLib::fileRead(DATA_FILE_DATA, (char *)g_tmpBuffer, size);
        
        CRijndael oRijndael;
        oRijndael.MakeKey(SECRET_KEY, CRijndael::sm_chain0, 16, 16);
        oRijndael.Encrypt(g_tmpBuffer, (char *)&m_local, size, CRijndael::ECB);
        
        if(m_local.m_version == 0)
        {
            DyDataLocal_00 dataLocal_00;
            memcpy(&dataLocal_00,  (unsigned char *)&m_local, sizeof(DyDataLocal_00));
            
            m_local.m_version = DYDATA_LOCALDATA_VER;
            
            m_local.m_isPromised = true;
        }
        
        return true;
	}
	else
	{
        YtPlatform_CreateShort();
        dataReset_local(&m_local);
        
        dataSave_local();
        
        return false;
	}
}



void DyDataManager::data_private_migration_2_4(DyDataPrivate* dataPrivate)
{
    DyDataPrivate_02 dataPrivate_02;
    memcpy(&dataPrivate_02, (unsigned char *)dataPrivate, sizeof(DyDataPrivate));
    
    memset(dataPrivate, 0, sizeof(DyDataPrivate));
    
    dataPrivate->m_version = DYDATA_PRIVATEDATA_VER;
    
    memcpy(dataPrivate->m_delly, dataPrivate_02.m_delly_02, 4);
    memcpy(dataPrivate->m_gold, dataPrivate_02.m_gold_02, 4);
    dataPrivate->m_cntRunRoulette = dataPrivate_02.m_cntRunRoulette_02;
    dataPrivate->m_cntGuest = dataPrivate_02.m_cntGuest_02;
    dataPrivate->m_cntInvite = dataPrivate_02.m_cntInvite_02;
    dataPrivate->m_cntFreeRoullete = dataPrivate_02.m_cntFreeRoullete_02;
    dataPrivate->m_cntPride = dataPrivate_02.m_cntPride_02;
    dataPrivate->m_excuteDate = dataPrivate_02.m_excuteDate_02;
    dataPrivate->m_presentday = dataPrivate_02.m_presentday_02;
    dataPrivate->m_staff_next_grade_remain = dataPrivate_02.m_staff_next_grade_remain_02;
    dataPrivate->m_itemslot = dataPrivate_02.m_itemslot_02;
    memcpy(dataPrivate->m_item, dataPrivate_02.m_item_02, sizeof(DyItemData)*DyItemType_Count);
    dataPrivate->m_rewardedReview = dataPrivate_02.m_rewardedReview_02;
    dataPrivate->m_cntCheckReview = dataPrivate_02.m_cntCheckReview_02;
    dataPrivate->m_inviteDay = dataPrivate_02.m_inviteDay_02;
    dataPrivate->m_cntInviteToday = dataPrivate_02.m_cntInviteToday_02;
    dataPrivate->m_viewNewTutorial = dataPrivate_02.m_viewNewTutorial_02;
    dataPrivate->m_cntRoulletteDay = dataPrivate_02.m_cntRoulletteDay_02;
    dataPrivate->m_cntRoulletteToday = dataPrivate_02.m_cntRoulletteToday_02;
    memcpy(dataPrivate->m_character, dataPrivate_02.m_character_02, DYDATA_CUSTOMER_DEFAULT_CNT);
    memcpy(dataPrivate->m_character+DYDATA_CUSTOMER_DEFAULT_CNT, dataPrivate_02.m_character_added_02, 6);
    
    memcpy(dataPrivate->m_character_eat, dataPrivate_02.m_character_eat_02, DYDATA_CUSTOMER_DEFAULT_CNT);
    memcpy(dataPrivate->m_character_eat+DYDATA_CUSTOMER_DEFAULT_CNT, dataPrivate_02.m_character_eat_added_02, 6);
    
    memcpy(dataPrivate->m_machineGrade, dataPrivate_02.m_machineGrade_02, SHOP_COUNT_DEFAULT);
    memcpy(dataPrivate->m_machineGrade+SHOP_COUNT_DEFAULT, dataPrivate_02.m_machineGrade_added_02, 3);
    
    memcpy(dataPrivate->m_sellcount, dataPrivate_02.m_sellcount_02, DYDATA_CNT_RECIPE*sizeof(unsigned short));
    memcpy(dataPrivate->m_sellcount+DYDATA_CNT_RECIPE, dataPrivate_02.m_sellcount_added_02, (12)*sizeof(unsigned short));
    
    memcpy(dataPrivate->m_recipe, dataPrivate_02.m_recipe_02, DYDATA_CNT_RECIPE/2);
    memcpy(dataPrivate->m_recipe+DYDATA_CNT_RECIPE/2, dataPrivate_02.m_recipe_added_02, (12/2));
    
    memcpy(dataPrivate->m_costume, dataPrivate_02.m_costume_02, DYDATA_MAX_COSTUMEITEM/4);
    memcpy(dataPrivate->m_costume+DYDATA_MAX_COSTUMEITEM/4, dataPrivate_02.m_costume_ex_02, (32/4));
    
    memcpy(dataPrivate->m_shop_status, dataPrivate_02.m_shop_status_02, SHOP_COUNT_DEFAULT);
    memcpy(dataPrivate->m_shop_status+SHOP_COUNT_DEFAULT, dataPrivate_02.m_shop_status_added_02, 3);
    
    memcpy(dataPrivate->m_award_recipe, dataPrivate_02.m_awards_02.m_award_recipe, 3);
    memcpy(dataPrivate->m_award_recipe+3, dataPrivate_02.m_award_recipe_added_02, 3);
    
    memcpy(dataPrivate->m_award_costume, dataPrivate_02.m_awards_02.m_award_costume, 3);
    memcpy(dataPrivate->m_award_costume+3, dataPrivate_02.m_award_costume_added_02, 3);
    
    memcpy(dataPrivate->m_award_shop, dataPrivate_02.m_awards_02.m_award_shop, 3);
    memcpy(dataPrivate->m_award_shop+3, dataPrivate_02.m_award_shop_added_02, 3);
    
    memcpy(dataPrivate->m_award_service, dataPrivate_02.m_awards_02.m_award_service, DYDATA_CUSTOMER_DEFAULT_CNT);
    memcpy(dataPrivate->m_award_service+DYDATA_CUSTOMER_DEFAULT_CNT, dataPrivate_02.m_award_service_added_02, 6);
    
    memcpy(dataPrivate->m_award_gold, dataPrivate_02.m_awards_02.m_award_gold, 3);
}


void DyDataManager::saveScore(std::string data)
{
    DyLib* lib = DyLib::sharedLib();
    
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    
    reader.parse(data, root);
    root = root.get("scores", "");
    
    for(int shop = 0;shop < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;shop++)
    {
        Json::Value shop_scores;
        switch(shop)
        {
            case 0:
                shop_scores = root["shop_boonsik"];
                break;
            case 1:
                shop_scores = root["shop_hamburger"];
                break;
            case 2:
                shop_scores = root["shop_coffee"];
                break;
            case 3:
                shop_scores = root["shop_china"];
                break;
            case 4:
                shop_scores = root["shop_pizza"];
                break;
            case 5:
                shop_scores = root["shop_korean"];
                break;


        }
        
        CCArray* arrShop = (CCArray*)m_arrShopPoint_forMigration->objectAtIndex(shop);
        arrShop->removeAllObjects();
        
        for(int stage = 0;stage < shop_scores.size();stage++)
        {
            sprintf(lib->m_strTemp, "stage_%d", stage);
            
            arrShop->addObject(CCInteger::create(atoi(shop_scores[lib->m_strTemp].asString().c_str())));
        }
    }
}



bool DyDataManager::dataLoad_private(DyDataPrivate* dataPrivate, std::string data)
{
    if(data.length() > 0)
    {
        base64_decode((char *)data.c_str(), (unsigned char *)dataPrivate, sizeof(DyDataPrivate));
        
        if(dataPrivate->m_version == 0)
        {
            DyDataPrivate_00 dataPrivate_00;
            memcpy(&dataPrivate_00,  (unsigned char *)dataPrivate, sizeof(DyDataPrivate));
            
            dataPrivate->m_version = 1;
            
            SInt gold = SInt(dataPrivate_00.m_gold_00);
            gold.store(dataPrivate->m_gold);
            
            SInt delly = SInt(dataPrivate_00.m_delly_00);
            delly.store(dataPrivate->m_delly);
        }
        
        if(dataPrivate->m_version == 1)
        {
            DyDataPrivate_02 dataPrivate_02;
            memcpy(&dataPrivate_02,  (unsigned char *)dataPrivate, sizeof(DyDataPrivate));
            
            
            dataPrivate_02.m_version_02 = 2;
            //
            // 1.1.05 버전은 산타 인덱스가 잘못 되어 있어서 아래와 같이 바꿔줘야한다.
            memcpy(dataPrivate_02.m_costume_ex_02, dataPrivate_02.m_costume_ex_02 + 16, 1);
            memset(dataPrivate_02.m_shop_status_added_02, 0, dataPrivate_02.buffer_02 + 8 - dataPrivate_02.m_shop_status_added_02);
            
            
            memcpy(dataPrivate,  &dataPrivate_02, sizeof(DyDataPrivate));
            
        }
        
        if(dataPrivate->m_version == 2)
        {
            DyDataPrivate_02 dataPrivate_02;
            memcpy(&dataPrivate_02,  (unsigned char *)dataPrivate, sizeof(DyDataPrivate));
            
            dataPrivate_02.m_version_02 = 3;
            
            m_is_need_privatedata_migration = true;
            
            for(int shop = 0;shop < SHOP_COUNT_DEFAULT;shop++)
            {
                CCArray* arrShop = (CCArray*)m_arrShopPoint_forMigration->objectAtIndex(shop);
                arrShop->removeAllObjects();
                
                for(int stage = 0;stage < 30;stage++)
                {
                    arrShop->addObject(CCInteger::create(dataPrivate_02.m_shop_record_02[shop][stage]));//unsigned int m_shop_record[SHOP_COUNT_DEFAULT][DYDATA_MAXLEVEL];
                }
            }
            
            for(int shop = 0;shop < 1;shop++)
            {
                CCArray* arrShop = (CCArray*)m_arrShopPoint_forMigration->objectAtIndex(SHOP_COUNT_DEFAULT+shop);
                arrShop->removeAllObjects();
                
                for(int stage = 0;stage < 30;stage++)
                {
                    arrShop->addObject(CCInteger::create(dataPrivate_02.m_shop_record_added_02[shop][stage]));
                }
            }
            
            memcpy(dataPrivate,  &dataPrivate_02, sizeof(DyDataPrivate));
        }
        
        if(dataPrivate->m_version == 3)
        {
            data_private_migration_2_4(dataPrivate);
        }
        
        if(dataPrivate->m_version == 4)
        {
            dataPrivate->m_version = DYDATA_PRIVATEDATA_VER;

            dataPrivate->m_current_staff = 0;
            dataPrivate->m_staff_ext_grade[0] = -1;
            dataPrivate->m_staff_ext_grade[1] = -1;
            dataPrivate->m_staff_ext_grade[2] = -1;
            dataPrivate->m_staff_ext_grade[3] = -1;
            dataPrivate->m_staff_ext_next_grade_remain[0] = 0;
            dataPrivate->m_staff_ext_next_grade_remain[1] = 0;
            dataPrivate->m_staff_ext_next_grade_remain[2] = 0;
            dataPrivate->m_staff_ext_next_grade_remain[3] = 0;

        }
        
        //
        // 델리 -로 가는 버그로 게임했던 유저를 위해 0으로 재 셋팅해준다.(131203)
        if(DyMoneyManager::sharedMoneyManager()->getDelly() < 0)
        {
            SInt delly = SInt(0);
            delly.store(dataPrivate->m_delly);
        }
        
        extern unsigned char g_sckey;
        g_sckey = 156;//ver 01

        DyItemManager* itemManager = DyItemManager::sharedItemManager();

        for(int i = 0;i < DYITEM_SLOT_CNT;i++)
		{
            int itemtype = item_slot_get_itemtype(i);
     		if(itemtype < DyItemType_Count)
                itemManager->m_itemslot[i] =  (DyItem*)itemManager->m_arrItems->objectAtIndex(itemtype);
		}
    
        for(int i = 0;i < DyItemType_Count;i++)
        {
            DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(i);
            item->m_cntHas = DyItemData_get_itemcount(dataPrivate->m_item[i]);
        }
  
        DyStaffManager::sharedStaffManager()->setCurrentStaff(dataPrivate->m_current_staff);
        
        return true;
    }
    else
    {
        dataReset_private(dataPrivate);
        return false;
    }
}



std::string DyDataManager::dataSave_private()
{
    DyItemManager* itemManager = DyItemManager::sharedItemManager();
    
	for(int i = 0;i < DYITEM_SLOT_CNT;i++)
	{
		if(itemManager->m_itemslot[i])
		{
            item_slot_set_itemtype(i, itemManager->m_itemslot[i]->m_itemtype);
		}
		else
            item_slot_set_itemtype(i, DyItemType_Count);
	}
    
    for(int i = 0;i < DyItemType_Count;i++)
	{
		DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(i);
        DyItemData_set_itemcount(m_private.m_item[i], item->m_cntHas);
	}
    
    base64_encode((char *)&m_private, sizeof(DyDataPrivate), &m_base64Temp);
    
    return std::string(m_base64Temp);
}

void DyDataManager::patchCostume(DyDataPublic* dataPublic)
{
    if(dataPublic->m_costume_status_ex.m_indexSet >= COSTUME_DEFAULT_CNT_SET + COSTUME_ADDED_CNT_SET)
    {
        //dataPublic->m_costume_status.m_indexSet = 0;
        dataPublic->m_costume_status_ex.m_indexSet = 0;
    }
    else if(dataPublic->m_costume_status_ex.m_indexSet >= COSTUME_DEFAULT_CNT_SET)
    {
        //dataPublic->m_costume_status_ex.m_indexSet = 0;
        dataPublic->m_costume_status.m_indexSet = dataPublic->m_costume_status_ex.m_indexSet;
    }
    
    
    if(dataPublic->m_costume_status_ex.m_indexScarf >= COSTUME_DEFAULT_CNT_SCARF + COSTUME_ADDED_CNT_SCARF)
    {
        //dataPublic->m_costume_status_ex.m_indexScarf = 0;
        dataPublic->m_costume_status.m_indexScarf = 0;
    }
    else if(dataPublic->m_costume_status_ex.m_indexScarf >= COSTUME_DEFAULT_CNT_SCARF)
    {
        //dataPublic->m_costume_status_ex.m_indexScarf = 0;
        dataPublic->m_costume_status.m_indexScarf = dataPublic->m_costume_status_ex.m_indexScarf;
    }
    
    if(dataPublic->m_costume_status_ex.m_indexHat >= COSTUME_DEFAULT_CNT_HAT + COSTUME_ADDED_CNT_HAT)
    {
        //dataPublic->m_costume_status_ex.m_indexHat = 0;
        dataPublic->m_costume_status.m_indexHat = 0;
    }
    else if(dataPublic->m_costume_status_ex.m_indexHat >= COSTUME_DEFAULT_CNT_HAT)
    {
        //dataPublic->m_costume_status_ex.m_indexHat = 0;
        dataPublic->m_costume_status.m_indexHat = dataPublic->m_costume_status_ex.m_indexHat;
    }
    
    if(dataPublic->m_costume_status_ex.m_indexApron >= COSTUME_DEFAULT_CNT_APRON + COSTUME_ADDED_CNT_APRON)
    {
        //dataPublic->m_costume_status_ex.m_indexApron = 0;
        dataPublic->m_costume_status.m_indexApron = 0;
    }
    else if(dataPublic->m_costume_status_ex.m_indexApron >= COSTUME_DEFAULT_CNT_APRON)
    {
        //dataPublic->m_costume_status_ex.m_indexApron = 0;
        dataPublic->m_costume_status.m_indexApron = dataPublic->m_costume_status_ex.m_indexApron;
    }
    
    if(dataPublic->m_costume_status_ex.m_indexCloth >= COSTUME_DEFAULT_CNT_CLOTH + COSTUME_ADDED_CNT_CLOTH)
    {
        //dataPublic->m_costume_status_ex.m_indexCloth = 0;
        dataPublic->m_costume_status.m_indexCloth = 0;
    }
    else if(dataPublic->m_costume_status_ex.m_indexCloth >= COSTUME_DEFAULT_CNT_CLOTH)
    {
        //dataPublic->m_costume_status_ex.m_indexCloth = 0;
        dataPublic->m_costume_status.m_indexCloth = dataPublic->m_costume_status_ex.m_indexCloth;
    }

}


std::string DyDataManager::dataSave_public()
{
    if(m_public.m_costume_status.m_indexSet >= COSTUME_DEFAULT_CNT_SET)
    {
        m_public.m_costume_status_ex.m_indexSet = m_public.m_costume_status.m_indexSet;
        m_public.m_costume_status.m_indexSet = 0;
    }
    else
        m_public.m_costume_status_ex.m_indexSet = 0;
    
    if(m_public.m_costume_status.m_indexApron >= COSTUME_DEFAULT_CNT_APRON)
    {
        m_public.m_costume_status_ex.m_indexApron = m_public.m_costume_status.m_indexApron;
        m_public.m_costume_status.m_indexApron = 0;
    }
    else
        m_public.m_costume_status_ex.m_indexApron = 0;

    
    if(m_public.m_costume_status.m_indexHat >= COSTUME_DEFAULT_CNT_HAT)
    {
        m_public.m_costume_status_ex.m_indexHat = m_public.m_costume_status.m_indexHat;
        m_public.m_costume_status.m_indexHat = 0;
    }
    else
        m_public.m_costume_status_ex.m_indexHat = 0;

    
    if(m_public.m_costume_status.m_indexCloth >= COSTUME_DEFAULT_CNT_CLOTH)
    {
        m_public.m_costume_status_ex.m_indexCloth = m_public.m_costume_status.m_indexCloth;
        m_public.m_costume_status.m_indexCloth = 0;
    }
    else
        m_public.m_costume_status_ex.m_indexCloth = 0;

    
    if(m_public.m_costume_status.m_indexScarf >= COSTUME_DEFAULT_CNT_SCARF)
    {
        m_public.m_costume_status_ex.m_indexScarf = m_public.m_costume_status.m_indexScarf;
        m_public.m_costume_status.m_indexScarf = 0;
    }
    else
        m_public.m_costume_status_ex.m_indexScarf = 0;

    
    base64_encode((char *)&m_public, sizeof(DyDataPublic), &m_base64Temp);
    
    
    patchCostume(&m_public);
    
    
    return std::string(m_base64Temp);
}


bool DyDataManager::dataLoad_public(DyDataPublic* dataPublic, std::string data, bool isMine)
{
   if(data.length() > 0)
    {
        base64_decode((char *)data.c_str(), (unsigned char *)dataPublic, sizeof(DyDataPublic));        
        
        if(dataPublic->m_version == 0)
        {
            DyDataPublic_00 dataPublic_00;
            memcpy(&dataPublic_00,  (unsigned char *)dataPublic, sizeof(DyDataPublic));
           
            dataPublic->m_version = DYDATA_PUBLICDATA_VER;
            dataPublic->m_staff_grade = -1;
            dataPublic->m_stageRecord = dataPublic_00.m_stageRecord_00;
            dataPublic->m_costume_status = dataPublic_00.m_costume_status_00;
            
            memset(dataPublic->m_stage_status, 0, sizeof(int)*SHOP_COUNT_DEFAULT*3);
        }
        
        
        //
        // ver 1이 잘못되어 ver2를 다시 처리함.
        if(dataPublic->m_version == 1 || dataPublic->m_version == 2)
        {
            dataPublic->m_version = DYDATA_PUBLICDATA_VER;
          
            if(isMine)
                memset(m_private.m_stage_status, 0, sizeof(int)*6*6);
            
            bool isFinded = false;
            
            for(int shop = 4-1;shop >= 0;shop--)
            {
                for(int level = 30-1;level >= 0;level--)
                {
                    //
                    // stage 에 대한 범위 체크 필요
                    unsigned int tgt;
                    
                    if(shop < 3)
                    {
                        tgt = dataPublic->m_stage_status[shop*3 + level/10];
                    }
                    else
                    {
                        tgt = dataPublic->m_stage_status_added[(shop-3)*3 + level/10];
                    }
                    
                    int offset = level%10;
                    
                    DyStageState stage = (DyStageState)((tgt>>(offset*3))&0b111);
                    
                    if(!isFinded && stage > DyStageState_Lock)
                    {
                        dataPublic->m_max_shop = shop;
                        dataPublic->m_max_stage = level;
                        isFinded = true;
                    }
                    if(isMine)
                    {
                        DyStageManager::sharedStageManager()->setStageState(shop, level, stage);
                    }

                }
            }
        }
        
        if(isMine)
        {
            //
            // 중국집 추가 처리
            DyStageManager* stageManager = DyStageManager::sharedStageManager();
            
            if(stageManager->getStageState(2, 29) > DyStageState_Fail)
            {
                shop_set_viewending(2, true);
                
                
                //
                // 커피숍 보너스 스테이지 처리
                int countLevel = stageManager->getCountLevel(2);
                
                if(countLevel > 30)
                {
                    if(stageManager->getStageState(2, 30) == DyStageState_Lock)
                    {
                        stageManager->setStageState(2, 30, DyStageState_Fail);
                    }
                }
            }
            
            
            //
            // 중국집 보너스 스테이지 처리
            if(stageManager->getStageState(3, 29) > DyStageState_Fail)
            {
                shop_set_viewending(3, true);
                
                int countLevel = stageManager->getCountLevel(3);
                
                if(countLevel > 30)
                {
                    if(stageManager->getStageState(3, 30) == DyStageState_Lock)
                    {
                        stageManager->setStageState(3, 30, DyStageState_Fail);
                    }
                }
            }
            
            
            
            //
            // 피자집 추가 처리
//            if(stageManager->getStageState(3, 29) > DyStageState_Fail)
//            {
//                shop_set_viewending(3, true);
//            }
      

            //한식집 추가 처리
            if(stageManager->getStageState(4, 29) > DyStageState_Fail)
            {
                shop_set_viewending(4, true);
            }
        }
        
        patchCostume(dataPublic);
        return true;
    }
    else
    {
        dataReset_public(dataPublic, isMine);
        return false;
    }
 
    return true;
}


void DyDataManager::reset()
{
    dataReset_private(&m_private);
    dataReset_local(&m_local);
    dataReset_public(&m_public, true);
    
    for(int shop = 0;shop < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;shop++)
    {
        CCArray* arrShop = (CCArray*)m_arrShopPoint_forMigration->objectAtIndex(shop);
        
        for(int stage = 0;stage < arrShop->count();stage++)
            arrShop->replaceObjectAtIndex(stage, CCInteger::create(0));
    }
    
	dataSave();
    
    YtLib::fileDelete(DATA_FILE_GUEST);
    YtLib::fileDelete(DATA_FILE_NOTICE_DELETE);
    YtLib::fileDelete(DATA_FILE_NOTICE_CHECK);
    YtLib::fileDelete(DATA_FILE_INVITE);
    
    DyRecipeManager::sharedRecipeManager()->loadRecipe();
    DyGuestManager::sharedGuestManager()->loadGuest();
    DyCostumeManager::sharedCostumeManager()->reloadCostume();
}


void DyDataManager::dataReset_private(DyDataPrivate* dataPrivate)
{
    DyItemManager* itemManager = DyItemManager::sharedItemManager();
    
    memset(dataPrivate, 0, sizeof(DyDataPrivate));
    
    dataPrivate->m_excuteDate = (getCurrentDate()).time();
    
    dataPrivate->m_presentday = -1;
    
    dataPrivate->m_cntGuest = 3;
    
    dataPrivate->m_version = DYDATA_PRIVATEDATA_VER;
    
    SInt gold = SInt(0);
    gold.store(dataPrivate->m_gold);
    
    SInt delly = SInt(0);
    delly.store(dataPrivate->m_delly);

    
    
    //
    // 급한대로 치마 기본제공
    costume_set_hasitem(13, true);
    costume_set_saw(13, true);
    
    item_slot_set_itemtype(0, DyItemType_Hint);
    item_slot_set_itemtype(1, DyItemType_Refresh);
    item_slot_set_itemtype(2, DyItemType_TimeIncrease);
    
    for(int i = 0;i < DYITEM_SLOT_CNT;i++)
    {
        itemManager->m_itemslot[i] = (DyItem*)itemManager->m_arrItems->objectAtIndex(item_slot_get_itemtype(i));
    }
    
    for(int i = 0;i < DyItemType_Count;i++)
    {
        DyItem* item = (DyItem*)itemManager->m_arrItems->objectAtIndex(i);
        
        DyItemData_set_itemcount(dataPrivate->m_item[i], item->m_default_count);
        item->m_cntHas = item->m_default_count;
        
//        if(i <= DyItemType_TimeIncrease)
//        {
//            DyItemData_set_itemview(dataPrivate->m_item[i], true);
//            //item->m_isView = true;
//        }
//        else
//        {
//            DyItemData_set_itemview(dataPrivate->m_item[i], false);
//            //item->m_isView = false;
//        }
    }
    
    DyCharacterManager::sharedCharacterManager()->setDangol(0);
    
    awards_set_complete(3,  0, true);
    awards_set_complete_count(3, 0, 1);
    awards_set_reward_count(3, 0, 1);

    dataPrivate->m_staff_next_grade_remain = 0;
    
    dataPrivate->m_current_staff = 0;
    dataPrivate->m_staff_ext_grade[0] = -1;
    dataPrivate->m_staff_ext_grade[1] = -1;
    dataPrivate->m_staff_ext_grade[2] = -1;
    dataPrivate->m_staff_ext_grade[3] = -1;
    
    dataPrivate->m_staff_ext_next_grade_remain[0] = 0;
    dataPrivate->m_staff_ext_next_grade_remain[1] = 0;
    dataPrivate->m_staff_ext_next_grade_remain[2] = 0;
    dataPrivate->m_staff_ext_next_grade_remain[3] = 0;
    
}


void DyDataManager::dataReset_public(DyDataPublic* dataPublic, bool isMine)
{
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
 
    memset(dataPublic, 0, sizeof(DyDataPublic));
    
    dataPublic->m_version = DYDATA_PUBLICDATA_VER;
    
    dataPublic->m_staff_grade = -1;
    
    if(isMine)
        stageManager->setStageState(0, 0, DyStageState_Fail);
    
    DyCostumeManager::setDefaultCostumeData(&dataPublic->m_costume_status, &dataPublic->m_costume_status_ex);
}


void DyDataManager::dataReset_local(DyDataLocal* dataLocal)
{
    memset(dataLocal, 0, sizeof(DyDataLocal));
    
    dataLocal->m_version = DYDATA_LOCALDATA_VER;
    
    YtOption_set_bgm(dataLocal->m_option, true);
    YtOption_set_effect(dataLocal->m_option, true);
    YtOption_set_push(dataLocal->m_option, true);
    
    dataLocal->m_isPromised = false;
    dataLocal->m_dataSaved = false;
}


void DyDataManager::onPopupClicked(cocos2d::CCObject * pSender)
{	
	DyToast *pToast = dynamic_cast<DyToast*>(pSender);

	switch(pToast->m_active_tag)
	{
		case DyAwardsCheckType_Recipe:
			break;
		case DyAwardsCheckType_Costume:
			break;
		case DyAwardsCheckType_Shop:
			break;
		case DyAwardsCheckType_Character:
			break;
		case DyAwardsCheckType_Rank:
			break;
		case DyAwardsCheckType_Gold:
			break;
	}
}

void DyDataManager::initRewardData(int type, int index)
{
    if(awards_get_complete(type, index) == false)
    {
        awards_set_isSawAward(type, index, false);
    }
}


void DyDataManager::checkAwards(DyAwardsCheckType checktype, bool isSaveData)
{
	DyLib* lib = DyLib::sharedLib();
	DyStageManager* stageManager = DyStageManager::sharedStageManager();

	switch(checktype)
	{
		case DyAwardsCheckType_Recipe:
			for(int i = 0;i < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;i++)
			{
                initRewardData(0, i);
                
				bool complete = DyRecipeManager::sharedRecipeManager()->isAllbuy(i);

				if(awards_get_complete(0, i) == false && complete)
				{
					awards_set_complete(0, i, true);
                    awards_set_complete_count(0, i, 1);
                    
					DyToast* toast = DyToast::create(DyToastType_Text, this, callfuncO_selector(DyDataManager::onPopupClicked));
					sprintf(lib->m_strTemp, STRING_awards_complete_recipe,
						stageManager->getShopName(i).c_str());
					toast->setText(lib->m_strTemp);
					toast->showToast();
				}
			}
			break;
		case DyAwardsCheckType_Costume:
			for(int i = 0;i < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;i++)
			{
                initRewardData(1, i);
                
				bool complete = false;
				switch(i)
				{
					case 0:
						complete = costume_get_hasitem(0);
						break;
					case 1:
						complete = costume_get_hasitem(4);
						break;
					case 2:
						complete = costume_get_hasitem(5);
						break;
                    case 3:
						complete = costume_get_hasitem(67);
						break;
                        
                    case 4:
						complete = costume_get_hasitem(90);
						break;
                    case 5:
						complete = costume_get_hasitem(95);
						break;


				}

				if(awards_get_complete(1, i) == false && complete)
				{
					awards_set_complete(1, i, true);
					awards_set_complete_count(1, i, 1);
                    
					DyToast* toast = DyToast::create(DyToastType_Text, this, callfuncO_selector(DyDataManager::onPopupClicked));

					sprintf(lib->m_strTemp, STRING_awards_complete_costume,
						stageManager->getShopName(i).c_str());
					toast->setText(lib->m_strTemp);
					toast->showToast();
				}
			}
			break;
		case DyAwardsCheckType_Shop:
			{
                
                DyStageType prevShop = stageManager->m_curShopType;
                
              
				for(int i = 0;i < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;i++)
				{
                    stageManager->setShop((DyStageType)i);
                    
                    initRewardData(3, i);
                
                    awards_set_complete(3, i, stageManager->isShopOpen(i));
                    
                    
                    if(awards_get_complete(3, i) && awards_get_complete_count(3, i) == 0)
                    {
                        awards_set_complete_count(3, i, 1);
                        awards_set_reward_count(3, i, 1);
                    }
                    
					//DyShopRecord* record = stageManager->getShopRecord(i);

					int perfect_count = 0;
					for(int j = 0;j < stageManager->m_cntLevel;j++)
					{
                     	if(stageManager->getStageState(i, j) == DyStageState_Perfect)
						{
							perfect_count++;
						}
					}

                    int complete_count = perfect_count/SHOP_INTERFVAL_AWARD_CNT;
                    if(complete_count > 3)
                        complete_count = 3;
                    
					if(awards_get_complete(3, i) && awards_get_complete_count(3, i) < 1 + complete_count)
					{
						awards_set_isSawAward(3, i, false);

						DyToast* toast = DyToast::create(DyToastType_Text, this, callfuncO_selector(DyDataManager::onPopupClicked));
						sprintf(lib->m_strTemp, STRING_awards_complete_shop, stageManager->getShopName(i).c_str());
						toast->setText(lib->m_strTemp);
						toast->showToast();
    
                        awards_set_complete_count(3, i, 1 + complete_count);
					}
				}
                stageManager->setShop(prevShop);

			}
			break;
		case DyAwardsCheckType_Character:
			for(int i = 0;i < DYDATA_CUSTOMER_DEFAULT_CNT+DYDATA_CUSTOMER_ADDED_CNT;i++)
			{
                initRewardData(4, i);
                
                bool isAchive = false;
                
                int heart_count = DyDataManager::sharedDataManager()->character_get_heart(i);
                
                DyCharacterInfo* characterInfo = DyCharacterManager::sharedCharacterManager()->getCharacterInfoByID(i);
                
                
                if(characterInfo == NULL)
                    continue;
                
                if(heart_count >= CHARACTER_INTERFVAL_AWARD_CNT*3)
                {
                    if(awards_get_complete_count(4, i) != 4)
                    {
                        awards_set_isSawAward(4, i, false);
                        isAchive = true;
                    }
                    awards_set_complete_count(4, i, 4);
                }
                else if(heart_count >= CHARACTER_INTERFVAL_AWARD_CNT*2)
                {
                    if(awards_get_complete_count(4, i) != 3)
                    {
                        awards_set_isSawAward(4, i, false);
                        isAchive = true;
                    }
                    awards_set_complete_count(4, i, 3);
                }
                else if(heart_count >= CHARACTER_INTERFVAL_AWARD_CNT)
                {
                    if(awards_get_complete_count(4, i) != 2)
                    {
                        awards_set_isSawAward(4, i, false);
                        isAchive = true;
                    }
                    awards_set_complete_count(4, i, 2);
                }
                
                if(awards_get_complete_count(4, i) > 0)
                    awards_set_complete(4, i, true);
                
				if(isAchive)
				{  
					DyToast* toast = DyToast::create(DyToastType_Text, this, callfuncO_selector(DyDataManager::onPopupClicked));
                    
                    sprintf(lib->m_strTemp, STRING_awards_complete_service, characterInfo->m_name.c_str());
					toast->setText(lib->m_strTemp);
					toast->showToast();
                    
				}
			}
			break;
		case DyAwardsCheckType_Rank:
			break;
		case DyAwardsCheckType_Gold:
			if(SInt(m_private.m_gold).get() >= 10000)
			{
                initRewardData(2, 0);
                
				if(awards_get_complete(2, 0) == false)
				{
					awards_set_complete(2, 0, true);
                    awards_set_complete_count(2, 0, 1);
                    
					DyToast* toast = DyToast::create(DyToastType_Text, this, callfuncO_selector(DyDataManager::onPopupClicked));
					sprintf(lib->m_strTemp, STRING_awards_complete_rank, STRING_awards_rank_stamp_0);
					toast->setText(lib->m_strTemp);
					toast->showToast();
				}
			}

			if(SInt(m_private.m_gold).get() >= 100000)
			{
                initRewardData(2, 1);
                
				if(awards_get_complete(2, 1) == false)
				{
					awards_set_complete(2, 1, true);
                    awards_set_complete_count(2, 1, 1);

					DyToast* toast = DyToast::create(DyToastType_Text, this, callfuncO_selector(DyDataManager::onPopupClicked));
					sprintf(lib->m_strTemp, STRING_awards_complete_rank, STRING_awards_rank_stamp_1);
					toast->setText(lib->m_strTemp);
					toast->showToast();
				}
			}

			if(SInt(m_private.m_gold).get() >= 1000000)
			{
                initRewardData(2, 2);
                
				if(awards_get_complete(2, 2) == false)
				{
					awards_set_complete(2, 2, true);
                    awards_set_complete_count(2, 2, 1);
                    
					DyToast* toast = DyToast::create(DyToastType_Text, this, callfuncO_selector(DyDataManager::onPopupClicked));
					sprintf(lib->m_strTemp, STRING_awards_complete_rank, STRING_awards_rank_stamp_2);
					toast->setText(lib->m_strTemp);
					toast->showToast();
				}
			}
			break;
	}

	if(isSaveData)
		dataSave();
}

void DyDataManager::addRoullette(int count)
{
    m_private.m_cntFreeRoullete += count;
    if(m_private.m_cntFreeRoullete >= MAX_CHARNUM)
        m_private.m_cntFreeRoullete = MAX_CHARNUM;
}

void DyDataManager::checkEvent()
{
    CCDate dateCurrent = (getCurrentDate());
    
    if(m_private.m_run_cnt == 0)
    {
        m_private.m_check_event_date = dateCurrent.time();
        m_private.m_event_type = 1;
    }
    
    CCDate dateBefore(m_private.m_check_event_date);
    
    CCDate dateDiff = dateCurrent - dateBefore;
    
    if(dateDiff.date() > 60*60*24*7*2)// 2주
    {
        m_private.m_run_cnt = 0;
        m_private.m_event_type = 1;
    }
    else if(dateDiff.date() > 60*60*24*7)
    {
        m_private.m_run_cnt = 0;
    }
    
    m_private.m_run_cnt++;
    
    if(m_private.m_run_cnt == 4)
    {
        
#ifdef ANDROID
        if(m_private.m_event_type == 1)
            m_private.m_event_type = 0;
        else
            m_private.m_event_type = 1;
        
        if(m_private.m_event_type == 1 && DyCostumeManager::sharedCostumeManager()->hasCostume("set_12"))
        {
            m_private.m_event_type = 0;
        }
           
        m_is_need_checkevent = true;
           
#else//iOS
        m_private.m_event_type = 1;
           
        if(DyCostumeManager::sharedCostumeManager()->hasCostume("set_12"))
        {
            m_is_need_checkevent = false;
        }
        else
           m_is_need_checkevent = true;

#endif
    }
    
//    m_private.m_event_type = 1;
//    
//    m_is_need_checkevent = true;
    
}

int DyDataManager::checkPresent()
{
    
    CCDate dateDiff;
    CCDate dateCurrent = (getCurrentDate());
    
    CCDate dateBefore(m_private.m_excuteDate);
        
    dateDiff = dateCurrent - dateBefore;
    
    int tm_day = dateDiff.date() / (60*60*24);
    
    bool hasPresent = false;
    
    if(m_private.m_presentday == -1)
    {
        m_private.m_presentday = 1;
        hasPresent = true;
    }
    else if(dateCurrent.mday() != dateBefore.mday() && tm_day < 2)
    {
        hasPresent = true;
        m_private.m_presentday++;
        if(m_private.m_presentday > PRESENT_CHECK_DAY)
        {
            m_private.m_presentday = 1;
        }
    }
    else if(tm_day >= 2)
    {
        m_private.m_presentday = 0;
    }
    
    m_private.m_excuteDate = dateCurrent.time();
  
    if(hasPresent)
        return m_private.m_presentday;
    else
        return 0;
}


bool DyDataManager::recipe_isAvailable(int index)
{
    unsigned char recipe;

    recipe = m_private.m_recipe[index/2];
    
    unsigned char tmp = recipe >> (4*(index%2));
    
    return (tmp&0b100) > 0;
}

bool DyDataManager::recipe_isSawInShop(int index)
{
    unsigned char recipe;

    recipe = m_private.m_recipe[index/2];
    
    unsigned char tmp = recipe >> (4*(index%2));
    
    return (tmp&0b10) > 0;
    
}


bool DyDataManager::recipe_isSawInGame(int index)
{
    unsigned char recipe;

    recipe = m_private.m_recipe[index/2];
    
    unsigned char tmp = recipe >> (4*(index%2));
    
    return (tmp&0b1) > 0;
}

void DyDataManager::recipe_setAvailable(int index, bool value)
{
    unsigned char recipe;

    recipe = m_private.m_recipe[index/2];
    
    int offset = index%2;
    
    unsigned char recipe_tgt;
    
    if(offset == 1)
    {
        recipe_tgt = (recipe&0b10111111) | ((value&0x1)<<6);
        
    }
    else
    {
        recipe_tgt = (recipe&0b11111011) | ((value&0x1)<<2);
    }

    m_private.m_recipe[index/2] = recipe_tgt;
}

void DyDataManager::recipe_setSawInShop(int index, bool value)
{
    unsigned char recipe;

    recipe = m_private.m_recipe[index/2];
    
    int offset = index%2;
    unsigned char recipe_tgt;
    
    if(offset == 1)
    {
        recipe_tgt = (recipe&0b11011111) | ((value&0x1)<<5);
        
    }
    else
    {
        recipe_tgt = (recipe&0b11111101) | ((value&0x1)<<1);
    }

    m_private.m_recipe[index/2] = recipe_tgt;
}


void DyDataManager::recipe_setSawInGame(int index, bool value)
{
    unsigned char recipe;
    
    recipe = m_private.m_recipe[index/2];
    
    int offset = index%2;
    
    unsigned char recipe_tgt;
    
    if(offset == 1)
    {
        recipe_tgt = (recipe&0b11101111) | ((value&0x1)<<4);
        
    }
    else
    {
        recipe_tgt = (recipe&0b11111110) | (value&0x1);
    }

    m_private.m_recipe[index/2] = recipe_tgt;
}


bool DyDataManager::costume_get_hasitem(int index)
{
    unsigned char costume;

    costume = m_private.m_costume[index/4];
    
    unsigned char tmp = costume >> (2*(index%4));
    
    return (tmp&0b10) > 0;
    
}


bool DyDataManager::costume_get_saw(int index)
{
    unsigned char costume;
    
    costume = m_private.m_costume[index/4];

    unsigned char tmp = costume >> (2*(index%4));
    
    return (tmp&0b01) > 0;
}

void DyDataManager::costume_set_hasitem(int index, bool value)
{
    unsigned char costume;
    
    costume = m_private.m_costume[index/4];
    
    int offset = index%4;
    
    unsigned char costume_result;
    
    if(offset == 3)
    {
        costume_result = (costume&0b01111111) | ((value&0b1)<<7);
        
    }
    else if(offset == 2)
    {
        costume_result = (costume&0b11011111) | ((value&0b1)<<5);
        
    }
    else if(offset == 1)
    {
        costume_result = (costume&0b11110111) | ((value&0b1)<<3);
        
    }
    else
    {
        costume_result = (costume&0b11111101) | ((value&0b1)<<1);
    }
    
    m_private.m_costume[index/4] = costume_result;
}


void DyDataManager::costume_set_saw(int index, bool value)
{
    unsigned char costume;
    
    costume = m_private.m_costume[index/4];
    
    int offset = index%4;
    
    unsigned char costume_result;
    
    
    if(offset == 3)
    {
        costume_result = (costume&0b10111111) | ((value&0b1)<<6);
        
    }
    else if(offset == 2)
    {
        costume_result = (costume&0b11101111) | ((value&0b1)<<4);
        
    }
    else if(offset == 1)
    {
        costume_result = (costume&0b11111011) | ((value&0b1)<<2);
        
    }
    else
    {
        costume_result = (costume&0b11111110) | (value&0b1);
    }

    m_private.m_costume[index/4] = costume_result;
    
}



int DyDataManager::item_slot_get_itemtype(int index)
{
    assert(index < 3);
    
    switch(index)
    {
        case 0:
            return m_private.m_itemslot&0b111;
        case 1:
            return (m_private.m_itemslot>>3)&0b111;
        case 2:
            return (m_private.m_itemslot>>6)&0b111;
    }
    return 0;
}


void DyDataManager::item_slot_set_itemtype(int index, int itemtype)
{
    assert(index < 3);
    
    switch(index)
    {
        case 0:
            m_private.m_itemslot = (m_private.m_itemslot&0b111111000) | (itemtype&0b111);
            break;
        case 1:
            m_private.m_itemslot = (m_private.m_itemslot&0b111000111) | ((itemtype&0b111)<<3);
            break;
        case 2:
            m_private.m_itemslot = (m_private.m_itemslot&0b000111111) | ((itemtype&0b111)<<6);
            break;
    }
}

#define DyShopRecord_set_viewending(tgt, arg)      (tgt) = ((tgt)&0b11111110 | ((arg)&0b1))
#define DyShopRecord_get_viewending(tgt)           (((tgt))&0b1)

#define DyShopRecord_set_crazyopen(tgt, arg)   (tgt) = ((tgt)&0b11111101 | (((arg)&0b1)<<1))
#define DyShopRecord_get_crazyopen(tgt)        (((tgt>>1))&0b1)


void DyDataManager::shop_set_viewending(int shop, int isView)
{
    DyShopRecord_set_viewending(m_private.m_shop_status[shop], isView);
}

bool DyDataManager::shop_get_viewending(int shop)
{
    return DyShopRecord_get_viewending(m_private.m_shop_status[shop]);
}

void DyDataManager::shop_set_crazyopen(int shop, int isOpen)
{
    DyShopRecord_set_crazyopen(m_private.m_shop_status[shop], isOpen);
}


bool DyDataManager::shop_get_crazyopen(int shop)
{
    return DyShopRecord_get_crazyopen(m_private.m_shop_status[shop]);
}


void DyDataManager::shop_set_record(int shop, int stage, int point)
{
    CCArray* arrShop = (CCArray*)m_arrShopPoint_forMigration->objectAtIndex(shop);
    arrShop->replaceObjectAtIndex(stage, CCInteger::create(point));
    
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    DyStageType prevShop = stageManager->setShop((DyStageType)shop);
    
    bool isFinded = false;
    
    for(int shop = SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED-1;shop >= 0;shop--)
    {
        int cntLevel = stageManager->getCountLevel(shop);
        
        for(int level = cntLevel-1;level >= 0;level--)
        {
            DyStageState stage = stageManager->getStageState(shop, level);
            
            if(!isFinded && stage > DyStageState_Lock)
            {
                m_public.m_max_shop = shop;
                m_public.m_max_stage = level;
                isFinded = true;
            }
        }
    }
    
    stageManager->setShop(prevShop);
    
}


int DyDataManager::shop_get_record(int shop, int stage)
{
    CCArray* arrShop = (CCArray*)m_arrShopPoint_forMigration->objectAtIndex(shop);
    if(arrShop->count() <= stage)
        return 0;
    
    return ((CCInteger*)arrShop->objectAtIndex(stage))->getValue();
}


void DyDataManager::character_set_see(int character, bool see)
{
    m_private.m_character[character] = (((m_private.m_character[character])&0b01111111) | (((see)&0b1)<<7));
}


bool DyDataManager::character_get_see(int charcter)
{
    return (((m_private.m_character[charcter])>>7)&0b1);
}


void DyDataManager::character_set_heart(int character, int heart)
{
    m_private.m_character[character] = (((m_private.m_character[character])&0b10000000) | ((heart)&0b01111111));
}

int DyDataManager::character_get_heart(int character)
{
    return ((m_private.m_character[character])&0b01111111);
    
}


int DyDataManager::character_get_eat(int character)
{
    return m_private.m_character_eat[character];
}


void DyDataManager::character_eat_inc(int character)
{
    m_private.m_character_eat[character]++;
}


void DyDataManager::character_eat_dec(int character)
{
    m_private.m_character_eat[character]--;
}




#define DyAwardsDataSub_set_complete(tgt, arg)   (tgt) = ((tgt)&0b01111111 | (((arg)&0b1)<<7))
#define DyAwardsDataSub_get_complete(tgt)        (((tgt)>>7)&0b1)

#define DyAwardsDataSub_set_isSawAward(tgt, arg)   (tgt) = ((tgt)&0b10111111 | (((arg)&0b1)<<6))
#define DyAwardsDataSub_get_isSawAward(tgt)        (((tgt)>>6)&0b1)


#define DyAwardsDataSub_set_complete_count(tgt, arg)   (tgt) = ((tgt)&0b11000111 | (((arg)&0b111)<<3))
#define DyAwardsDataSub_get_complete_count(tgt)        (((tgt)>>3)&0b111)

#define DyAwardsDataSub_set_reward_count(tgt, arg)   (tgt) = ((tgt)&0b11111000 | ((arg)&0b111))
#define DyAwardsDataSub_get_reward_count(tgt)        ((tgt)&0b111)




//
// 0 recipe
// 1 costume
// 2 gold
// 3 shop
// 4 service
void DyDataManager::awards_set_complete(int type, int index, bool complete)
{
    switch(type)
    {
        case 0:
            DyAwardsDataSub_set_complete(m_private.m_award_recipe[index], complete);
            break;
        case 1:
            DyAwardsDataSub_set_complete(m_private.m_award_costume[index], complete);
            break;
        case 2:
            DyAwardsDataSub_set_complete(m_private.m_award_gold[index], complete);
            break;
        case 3:
            DyAwardsDataSub_set_complete(m_private.m_award_shop[index], complete);
            break;
        case 4:
            DyAwardsDataSub_set_complete(m_private.m_award_service[index], complete);
            break;
    }
}


bool DyDataManager::awards_get_complete(int type, int index)
{
    switch(type)
    {
        case 0:
            return DyAwardsDataSub_get_complete(m_private.m_award_recipe[index]);
        case 1:
            return DyAwardsDataSub_get_complete(m_private.m_award_costume[index]);
        case 2:
            return DyAwardsDataSub_get_complete(m_private.m_award_gold[index]);
        case 3:
            return DyAwardsDataSub_get_complete(m_private.m_award_shop[index]);
        case 4:
            return DyAwardsDataSub_get_complete(m_private.m_award_service[index]);
    }
    return false;
}

void DyDataManager::awards_set_isSawAward(int type, int index, bool isSaw)
{
    switch(type)
    {
        case 0:
            DyAwardsDataSub_set_isSawAward(m_private.m_award_recipe[index], isSaw);
            break;
        case 1:
            DyAwardsDataSub_set_isSawAward(m_private.m_award_costume[index], isSaw);
            break;
        case 2:
            DyAwardsDataSub_set_isSawAward(m_private.m_award_gold[index], isSaw);
            break;
        case 3:
            DyAwardsDataSub_set_isSawAward(m_private.m_award_shop[index], isSaw);
            break;
        case 4:
            DyAwardsDataSub_set_isSawAward(m_private.m_award_service[index], isSaw);
            break;
    }
    
}

bool DyDataManager::awards_get_isSawAward(int type, int index)
{
    switch(type)
    {
        case 0:
            return DyAwardsDataSub_get_isSawAward(m_private.m_award_recipe[index]);
        case 1:
            return DyAwardsDataSub_get_isSawAward(m_private.m_award_costume[index]);
        case 2:
            return DyAwardsDataSub_get_isSawAward(m_private.m_award_gold[index]);
        case 3:
            return DyAwardsDataSub_get_isSawAward(m_private.m_award_shop[index]);
        case 4:
            return DyAwardsDataSub_get_isSawAward(m_private.m_award_service[index]);
    }
    return false;
}

void DyDataManager::awards_set_complete_count(int type, int index, int count)
{
    switch(type)
    {
        case 0:
            DyAwardsDataSub_set_complete_count(m_private.m_award_recipe[index], count);
            break;
        case 1:
            DyAwardsDataSub_set_complete_count(m_private.m_award_costume[index], count);
            break;
        case 2:
            DyAwardsDataSub_set_complete_count(m_private.m_award_gold[index], count);
            break;
        case 3:
            DyAwardsDataSub_set_complete_count(m_private.m_award_shop[index], count);
            break;
        case 4:
            DyAwardsDataSub_set_complete_count(m_private.m_award_service[index], count);
            break;
    }
    
}
int DyDataManager::awards_get_complete_count(int type, int index)
{
    switch(type)
    {
        case 0:
            return DyAwardsDataSub_get_complete_count(m_private.m_award_recipe[index]);
        case 1:
            return DyAwardsDataSub_get_complete_count(m_private.m_award_costume[index]);
        case 2:
            return DyAwardsDataSub_get_complete_count(m_private.m_award_gold[index]);
        case 3:
            return DyAwardsDataSub_get_complete_count(m_private.m_award_shop[index]);
        case 4:
            return DyAwardsDataSub_get_complete_count(m_private.m_award_service[index]);
    }
    return 0;
    
}

void DyDataManager::awards_set_reward_count(int type, int index, int count)
{
    switch(type)
    {
        case 0:
            DyAwardsDataSub_set_reward_count(m_private.m_award_recipe[index], count);
            break;
        case 1:
            DyAwardsDataSub_set_reward_count(m_private.m_award_costume[index], count);
            break;
        case 2:
            DyAwardsDataSub_set_reward_count(m_private.m_award_gold[index], count);
            break;
        case 3:
            DyAwardsDataSub_set_reward_count(m_private.m_award_shop[index], count);
            break;
        case 4:
            DyAwardsDataSub_set_reward_count(m_private.m_award_service[index], count);
            break;
    }
}


int DyDataManager::awards_get_reward_count(int type, int index)
{
    switch(type)
    {
        case 0:
            return DyAwardsDataSub_get_reward_count(m_private.m_award_recipe[index]);
        case 1:
            return DyAwardsDataSub_get_reward_count(m_private.m_award_costume[index]);
        case 2:
            return DyAwardsDataSub_get_reward_count(m_private.m_award_gold[index]);
        case 3:
            return DyAwardsDataSub_get_reward_count(m_private.m_award_shop[index]);
        case 4:
            return DyAwardsDataSub_get_reward_count(m_private.m_award_service[index]);
    }
    return 0;
}


DyDataManager* DyDataManager::sharedDataManager()
{
	if(g_sharedDataManager == NULL)
	{
		g_sharedDataManager = new DyDataManager();
	}
	return g_sharedDataManager;
}

