/*
DyFriend.cpp
Doggy chef
@initialize at 121231

@history
121231	yoonsr	initialize
*/

#include "DyRank.h"
#include "DyMoney.h"
#include "DyData.h"
#include "DyLib.h"

#include "DyLoader.h"
#include "YtNetwork.h"

#include "DyCostume.h"
#include "DyFriend.h"

#include "json.h"


using namespace std;

DyRankManager* g_sharedRankManager;

DyRank::DyRank()
{
}


DyRank::~DyRank()
{
}


DyRankManager::DyRankManager()
{
	m_arrRankStory = new CCArray();
    m_arrRankCrazy = new CCArray();
    
    
    m_arrTotalStory = new CCArray();
    m_arrTotalCrazy = new CCArray();
}


DyRankManager::~DyRankManager()
{
	m_arrRankStory->release();
    m_arrRankCrazy->release();
    
    m_arrTotalStory->release();
    m_arrTotalCrazy->release();
}


CCArray* DyRankManager::getArray(DyRankType type)
{
    switch(type)
    {
        case DyRankType_Stage:
            return m_arrRankStory;
        case DyRankType_Crazy:
            return m_arrRankCrazy;
        default:
            break;
            
    }
    return NULL;
}


CCArray* DyRankManager::sortRank(DyRankType type)
{
    CCArray* rankarray = getArray(type);
	
	for(unsigned int i = 0;i < rankarray->count();i++)
	{
		for(unsigned int j = i+1;j < rankarray->count();j++)
		{
			DyRank* rank_I = (DyRank*)rankarray->objectAtIndex(i);
			DyRank* rank_J = (DyRank*)rankarray->objectAtIndex(j);

            if(rank_I->m_point < rank_J->m_point)
            {
                rankarray->exchangeObjectAtIndex(i, j);
            }
        }
	}

	int ranking = 1;
	long long prevPoint = -1;
	int prevRank = ranking;
	for(unsigned int i = 0;i < rankarray->count();i++)
	{
		DyRank* rank = (DyRank*)rankarray->objectAtIndex(i);

        unsigned int tmpPoint = rank->m_point;
        
		if(prevPoint == tmpPoint)
		{
			rank->m_order = prevRank;
		}
		else
		{
			rank->m_order = ranking;
		}

		prevRank = rank->m_order;
		prevPoint = tmpPoint;

		ranking++;
	}
    
    return rankarray;
}


void DyRankManager::loadLeaderBoard(DyRankType type, std::string data)
{
    DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
    CCArray* arrayRank;
    
    arrayRank = getArray(type);
    arrayRank->removeAllObjects();
    
    
    if(data.length() > 0)
    {
        Json::Value root;   // will contains the root value after parsing.
        Json::Reader reader;
        
        reader.parse(data, root);
        
        Json::Value app_friends = root.get("app_friends", ""); // 게임내 리더보드 목록
        
        for(int index = 0; index < app_friends.size(); ++index)
        {
            Json::Value season_score = app_friends[index].get("season_score", 0);
            Json::Value best_score = app_friends[index].get("best_score", 0);
            
            Json::Value user_id = app_friends[index].get("user_id", "default");
            
            DyFriend *_friend = friendManager->getFriend(user_id.asString());
      
            if(_friend == NULL)
                continue;
            
            DyRank* rank = new DyRank();
            
            rank->m_friend = _friend;
      
            if(type == DyRankType_Stage)
                rank->m_point = rank->m_friend->m_public.m_stageRecord;
            else 
                rank->m_point = season_score.asInt();
            
            rank->m_pointBest = best_score.asInt();
            
            arrayRank->addObject(rank);
            rank->release();
        }
    }
    else
    {
        DyDataManager* dataManager = DyDataManager::sharedDataManager();
        
        DyFriend *_friend = friendManager->getI();
        
        DyRank* rank = new DyRank();
        
        rank->m_friend = _friend;
        
        if(type == DyRankType_Stage)
        {
            rank->m_pointBest = rank->m_point = dataManager->m_public.m_stageRecord;
        }
        else
        {
            rank->m_pointBest = rank->m_point = dataManager->m_crazyRecord;
        }
        arrayRank->addObject(rank);
        rank->release();

    }
    static int point_stage[3] =
    {
        15000, 160000, 650000,
    };
    
    
    static int point_crazy[3] =
    {
        8000, 20000, 45000,
    };
    
    for(int i = 0;i < friendManager->m_arrDummy->count();i++)
    {
        DyRank* rank = new DyRank();
        
        rank->m_friend = (DyFriend*)friendManager->m_arrDummy->objectAtIndex(i);
        
        if(type == DyRankType_Stage)
        {
            rank->m_point = point_stage[i];
            rank->m_pointBest = point_stage[i];
        }
        else
        {
            rank->m_point = point_crazy[i];
            rank->m_pointBest = point_crazy[i];
        }
        arrayRank->addObject(rank);
        rank->release();
    }

    sortRank(type);
}


DyRankType DyRankManager::loadGlobalRank(std::string data)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    
    reader.parse(data, root);
    
    Json::Value score_type = root["result"]["score_type"];
    Json::Value stage_record = root["result"]["record"];
    
    bool isStoryRecord = false;
    CCArray* arrTarget;
    if(strcmp(score_type.asCString(), "story") == 0)
    {
        isStoryRecord = true;
        arrTarget = m_arrTotalStory;
    }
    else
        arrTarget = m_arrTotalCrazy;
    
    arrTarget->removeAllObjects();
    
    bool hasMine = false;
    for(int index = 0; index < stage_record.size(); ++index)
    {
        Json::Value stage = stage_record[index];
     
        Json::Value order = stage.get("order", "");
        Json::Value nickname = stage.get("nickname", "");
        Json::Value stage_record = stage.get("stage_record", "");
        Json::Value crazy_record = stage.get("crazy_record", "");
        Json::Value picture_url = stage.get("picture_url", "");
        Json::Value user_id = stage.get("user_id", "");
        Json::Value public_data = stage.get("public_data", "");
        
        DyRankGlobal* rank = new DyRankGlobal();
        
        rank->m_order = atoi(order.asString().c_str());
        rank->m_nickname = nickname.asString();
        
        if(isStoryRecord)
        {
            rank->m_point = rank->m_stage_record = atoi(stage_record.asString().c_str());
            rank->m_crazy_record = atoi(crazy_record.asString().c_str());
        }
        else
        {
            rank->m_point = rank->m_crazy_record = atoi(crazy_record.asString().c_str());
            rank->m_stage_record = atoi(stage_record.asString().c_str());
        }
        
        rank->m_picture_url = picture_url.asString();
        rank->m_user_id = user_id.asString();
        
        
        if(dataManager->m_user_id.compare(rank->m_user_id) == 0)
            hasMine = true;
        if(rank->m_nickname.length() == 0)
        {
            rank->m_nickname = rank->m_user_id.substr(0, rank->m_user_id.length()-3);
            rank->m_nickname.append("XXX");
        }
        
        dataManager->dataLoad_public(&rank->m_public, public_data.asString(), false);
        
        arrTarget->addObject(rank);
        rank->release();
    }
    
    if(!hasMine)
    {
        Json::Value my_record = root["result"]["my_record"];
        
        DyRankGlobal* rank = new DyRankGlobal();
        
        rank->m_order = atoi(my_record.get("order", "").asString().c_str());
        rank->m_nickname = my_record.get("nickname", "").asString();
        
        if(isStoryRecord)
        {
            rank->m_point = rank->m_stage_record = atoi(my_record.get("stage_record", "").asString().c_str());
            rank->m_crazy_record = atoi(my_record.get("crazy_record", "").asString().c_str());
        }
        else
        {
            rank->m_point = rank->m_crazy_record = atoi(my_record.get("crazy_record", "").asString().c_str());
            rank->m_stage_record = atoi(my_record.get("stage_record", "").asString().c_str());
        }
        
        rank->m_user_id = dataManager->m_user_id;
        
        rank->m_public = dataManager->m_public;
        
        arrTarget->addObject(rank);
        rank->release();
    }
    
    
    if(isStoryRecord)
        return DyRankType_Stage;
    
    return DyRankType_Crazy;
}


DyRankType DyRankManager::currentCheckRankType()
{
    return m_curRankType;
}


void DyRankManager::initCheckPoint(DyRankType type)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    m_curRankType = type;
    
    DyRank* myrank = getMyRank(m_curRankType);
   
    if(myrank == NULL)
        return;
    
    if(m_curRankType == DyRankType_Stage)
        myrank->m_point = dataManager->m_public.m_stageRecord;
    else
        myrank->m_point = dataManager->m_crazyRecord;
   
    sortRank(m_curRankType);
    
    m_initMyOrder = myrank->m_order;
}


bool DyRankManager::setCheckPoint()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyRank* myrank = getMyRank(m_curRankType);
    
    if(myrank == NULL)
        return false;
    
    if(m_curRankType == DyRankType_Stage)
        myrank->m_point = dataManager->m_public.m_stageRecord;
    else
        myrank->m_point = dataManager->m_crazyRecord;
    
    sortRank(m_curRankType);
    
    return m_initMyOrder > myrank->m_order;
}



DyRank* DyRankManager::getCurrrentTarget(DyRankType type, int point)
{
    CCArray* rankarray = getArray(type);
	
	for(int i = rankarray->count() - 1;i >= 0;i--)
	{
        DyRank* rank = (DyRank*)rankarray->objectAtIndex(i);
        
        if(point < rank->m_point)
        {
            return rank;
        }
    }
    
    return NULL;
}


DyRank* DyRankManager::getMyRank(DyRankType type)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    CCArray* rankarray = getArray(type);
	
	for(unsigned int i = 0;i < rankarray->count();i++)
	{
        DyRank* rank_I = (DyRank*)rankarray->objectAtIndex(i);
        
        if(dataManager->m_user_id.compare(rank_I->m_friend->m_user_id) == 0)
        {
            return rank_I;
        }
    }
    
    return NULL;
}



//DyRank* DyRankManager::getRankByOrder(DyRankType type, int order)
//{
//    CCArray* rankarray = getArray(type);
//	
//	for(unsigned int i = 0;i < rankarray->count();i++)
//	{
//        DyRank* rank = (DyRank*)rankarray->objectAtIndex(i);
//        
//        if(rank->m_order == order)
//        {
//            return rank;
//        }
//    }
//    
//    return NULL;
//}



DyRank* DyRankManager::getPrevFriendRank(DyRankType type)
{
    DyRank* myrank = getMyRank(type);
    
    CCArray* rankarray = getArray(type);
	
	for(int i = rankarray->count() - 1;i >= 0;i--)
	{
        DyRank* rank = (DyRank*)rankarray->objectAtIndex(i);
        
        if(myrank->m_order > rank->m_order)
        {
            return rank;
        }
    }
    
    return NULL;
}


DyRank* DyRankManager::getNextFriendRank(DyRankType type)
{
    DyRank* myrank = getMyRank(type);
    
    CCArray* rankarray = getArray(type);
	
	for(int i = 0;i < rankarray->count();i++)
	{
        DyRank* rank = (DyRank*)rankarray->objectAtIndex(i);
        
        if(myrank->m_order < rank->m_order)
        {
            return rank;
        }
    }
    
    return NULL;
}



DyRankManager* DyRankManager::sharedRankManager()
{
	if(g_sharedRankManager == NULL)
	{
		g_sharedRankManager = new DyRankManager();
	}
	return g_sharedRankManager;
}




