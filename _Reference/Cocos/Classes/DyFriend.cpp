/*
DyFriend.cpp
Doggy chef
@initialize at 121231

@history
121231	yoonsr	initialize
*/

#include "DyFriend.h"
#include "DyMoney.h"
#include "DyData.h"
#include "DyLib.h"

#include "DyLoader.h"
#include "YtNetwork.h"

#include "DyCostume.h"
#include "DyStage.h"

#include "json.h"

using namespace std;

DyFriendManager* g_sharedFriendManager;

#define INVITE_UNIT_SIZE    (32+8)
DyFriend::DyFriend()
{
    m_sprPicture = NULL;
    m_isDummy = false;
    
    m_arrShopPoint = new CCArray();
    for(int i = 0;i < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;i++)
    {
        m_arrShopPoint->addObject(CCArray::create());
    }
    
}


DyFriend::~DyFriend()
{
	if(m_sprPicture)
    {
        m_sprPicture->stopDownload();
		m_sprPicture->release();
    }
    m_arrShopPoint->release();
}

DyFriendInvite::DyFriendInvite()
{
    memset(m_user_id, 0, 32);
    m_time_last_message_send_at = 0;
}


//int DyFriendManager_split(const string &pattern, const string &subject, list<string> &list)  
//{  
//    string::size_type pattern_length = pattern.length();  
//	string::size_type beginpos = 0;  
//	string::size_type endpos = subject.find(pattern);  
//	
//	while (endpos != string::npos)  
//	{  
//		list.push_back(subject.substr(beginpos, endpos-beginpos));
//		beginpos = endpos + pattern_length;  
//		endpos = subject.find(pattern, beginpos);  
//	}  
//	list.push_back(subject.substr(beginpos));
//	return 0;  
//}  


DyFriendManager::DyFriendManager()
{
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("roullette.plist");
	
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    DyLib* lib = DyLib::sharedLib();
	m_arrFriends = new CCArray();
    m_arrDummy = new CCArray();
    
    std::string name[3] =
    {
        "초코",
        "순심이",
        "백구",
    };
    
    std::string picture[3] =
    {
        "dummy_picture_0.png",
        "dummy_picture_1.png",
        "dummy_picture_2.png",
    };
    
    for(int i = 0;i < 3;i++)
    {
        DyFriend* _friend = new DyFriend();
        
        _friend->m_friendType = DyFriendType_Gamer;
        
        _friend->m_nickname = name[i];
        
        sprintf(lib->m_strTemp, "dummy_id_%d", i);
        _friend->m_user_id = lib->m_strTemp;
        
        _friend->m_message_blocked = true;
        _friend->m_isDummy = true;
        
        _friend->m_profile_image_url = "";
        _friend->m_sprPicture = YtAsyncSprite::create(picture[i].c_str(), "", picture[i].c_str(), false, CCSizeMake(110, 110));
        _friend->m_sprPicture->retain();

        dataManager->dataReset_public(&_friend->m_public, false);
        _friend->m_public.m_costume_status.m_indexApron = 0;

        switch(i)
        {
            case 0:
                _friend->m_public.m_costume_status.m_indexCloth = 2;
                _friend->m_public.m_costume_status.m_indexHat = 4;
                _friend->m_public.m_costume_status.m_indexScarf = 2;
                _friend->m_public.m_staff_grade = 0;
                
                stageManager->setCurrentStage(&_friend->m_public, 0, 16);
                break;
            case 1:
                _friend->m_public.m_costume_status.m_indexSet = 5;
                stageManager->setCurrentStage(&_friend->m_public, 1, 12);

                _friend->m_public.m_staff_grade = 2;
                break;
            case 2:
                _friend->m_public.m_costume_status.m_indexSet = 7;
                stageManager->setCurrentStage(&_friend->m_public, 2, 21);
                _friend->m_public.m_staff_grade = 4;
                break;
        }
        
        _friend->m_time_last_message_sent_at = CCDate(0);
        
        m_arrDummy->addObject(_friend);
        _friend->release();
    }
    
    m_arrInvited = new CCArray();
    
}

DyFriendManager::~DyFriendManager()
{
	m_arrFriends->release();
    m_arrDummy->release();
    m_arrInvited->release();
}


CCSprite* DyFriendManager::getPicture(DyFriend* friendInfo, CCSize size)
{
    CCSprite* sprRet;
    if(friendInfo->m_sprPicture == NULL)
    {
        std::string strUrl;
        if(friendInfo->m_profile_image_url.length() > 0)
        {
            int posFirst = friendInfo->m_profile_image_url.rfind("/");
            int posSecond = friendInfo->m_profile_image_url.rfind("/", posFirst-1);
            strUrl = friendInfo->m_profile_image_url.substr(posSecond);
            strUrl.replace(posFirst-posSecond, 1, "_");
            strUrl.replace(0, 1, "_");
        }
        else
            strUrl = "";
        
        friendInfo->m_sprPicture = YtAsyncSprite::create("main_ui_user_picture_default.png", friendInfo->m_profile_image_url.c_str(), strUrl.c_str(), false, size);
        friendInfo->m_sprPicture->retain();
        sprRet = friendInfo->m_sprPicture;
    }
    else
    {
        sprRet = CCSprite::createWithSpriteFrame(friendInfo->m_sprPicture->displayFrame());
    }
    
    if(sprRet)
        sprRet->setScale(size.width/friendInfo->m_sprPicture->getContentSize().width);
    
    return sprRet;
}


DyFriend* DyFriendManager::getFriend(std::string id)
{
	for(unsigned int i = 0;i < m_arrFriends->count();i++)
	{
		DyFriend* _friend = (DyFriend*)m_arrFriends->objectAtIndex(i);

		if(_friend->m_user_id.compare(id) == 0)
		{
			return _friend;
		}
	}
	return NULL;
}



void DyFriendManager::initFriendsTag()
{
    for(unsigned int i = 0;i < m_arrFriends->count();i++)
	{
		DyFriend* _friend = (DyFriend*)m_arrFriends->objectAtIndex(i);
        
		_friend->m_tmpTag = 0;
	}
}


DyFriend* DyFriendManager::getI()
{
    for(unsigned int i = 0;i < m_arrFriends->count();i++)
	{
		DyFriend* _friend = (DyFriend*)m_arrFriends->objectAtIndex(i);
        
		if(_friend->m_friendType == DyFriendType_Mine)
		{
			return _friend;
		}
	}
	return NULL;
}


void DyFriendManager::updateMine()
{
    DyFriend* mine = getI();
    
    if(mine == NULL)
        return;
    
    mine->m_public = DyDataManager::sharedDataManager()->m_public;
}


void DyFriendManager::saveFriendsScore(int shop, std::string data)
{
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    DyLib* lib = DyLib::sharedLib();
   
    
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    
    reader.parse(data, root);
    root = root.get("scores", "");
    
    int stage_cnt =  stageManager->getCountLevel(shop);
    
    DyStageType prevShop = stageManager->setShop((DyStageType)shop);
    
    
    for(int i = 0;i < m_arrFriends->count();i++)
    {
        DyFriend* _friend = (DyFriend*)m_arrFriends->objectAtIndex(i);
        
        if(_friend->m_friendType == DyFriendType_Gamer && !_friend->m_isDummy)
        {
            CCArray* arrShop = (CCArray*)_friend->m_arrShopPoint->objectAtIndex(shop);
            arrShop->removeAllObjects();
            
            Json::Value shop_scores = root[_friend->m_user_id];
            
            if(!shop_scores.empty())
            {
                for(int stage = 0;stage < shop_scores.size();stage++)
                {
                    sprintf(lib->m_strTemp, "stage_%d", stage);
                    
                    arrShop->addObject(CCInteger::create(atoi(shop_scores[lib->m_strTemp].asString().c_str())));
                }
            }
            else
            {
                for(int stage = 0;stage < stage_cnt;stage++)
                {
                    int goal_gold = atoi(stageManager->m_curShopInfo->getValue("goal_gold", stage));
                    int perfect_gold = atoi(stageManager->m_curShopInfo->getValue("perfect_gold", stage));
                    
                    DyStageState friend_stage;// = stageManager->getStageState(&_friend->m_public, shop, stage);
                    
                    //DyStageState DyStageManager::getStageState(DyDataPublic* dataPublic, int shop, int stage)
                    if(stage < 30)
                    {
                        //
                        // stage 에 대한 범위 체크 필요

                        unsigned int tgt;

                        if(shop < SHOP_COUNT_DEFAULT)
                        {
                            tgt = _friend->m_public.m_stage_status[shop*3 + stage/10];
                        }
                        else
                        {
                            tgt = _friend->m_public.m_stage_status_added[(shop-SHOP_COUNT_DEFAULT)*3 + stage/10];
                        }
                        
                        int offset = stage%10;
                        
                        friend_stage = (DyStageState)((tgt>>(offset*3))&0b111);
                    }
                    else
                        friend_stage = DyStageState_Lock;
                    
                    
                    int score;
                    
                    switch(friend_stage)
                    {
                        case DyStageState_Goal:
                            score = goal_gold;
                            break;
                        case DyStageState_Goal_Plus:
                            score = goal_gold + (perfect_gold-goal_gold)/2;
                            break;
                        case DyStageState_Perfect:
                            score = perfect_gold;
                            break;
                        default:
                            score = 0;
                            break;
                    }
                    
                    arrShop->addObject(CCInteger::create(score));
                }
            }
        }
        else if(_friend->m_friendType == DyFriendType_Mine)
        {
            CCArray* arrShop = (CCArray*)_friend->m_arrShopPoint->objectAtIndex(shop);
            arrShop->removeAllObjects();
            
            for(int stage = 0;stage < stage_cnt;stage++)
            {
                int score = DyDataManager::sharedDataManager()->shop_get_record(shop, stage);
                arrShop->addObject(CCInteger::create(score));
            }
        }
    }
    
    stageManager->setShop(prevShop);

}


void DyFriendManager::loadFriends(std::string data)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    m_arrFriends->removeAllObjects();
    
    m_arrFriends->addObjectsFromArray(m_arrDummy);
    
    loadInvitedFriend();
    
    if(data.length() == 0)
    {
        DyFriend* _friend = new DyFriend();
        _friend->m_friendType = DyFriendType_Mine;
        _friend->m_user_id = dataManager->m_user_id;
        _friend->m_nickname = DyLib::getCorrectNickname(dataManager->m_nickname);
        
        _friend->m_profile_image_url = dataManager->m_profile_image_url;
        _friend->m_sprPicture = NULL;
        
        memcpy(&_friend->m_public, &dataManager->m_public, sizeof(DyDataPublic));
        
        m_arrFriends->addObject(_friend);
        _friend->release();
   
        return;
    }
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    
    reader.parse(data, root);
    
    Json::Value friends_count = root.get("friends_count", 0);
    
    Json::Value app_friends = root.get("app_friends", ""); // 게임내 리더보드 목록
    
    for(int index = 0; index < app_friends.size(); ++index)
    {
        Json::Value nickname = app_friends[index].get("nickname", "default");
        Json::Value profile_image_url = app_friends[index].get("profile_image_url", "");
        Json::Value user_id = app_friends[index].get("user_id", "default");
        Json::Value public_data = app_friends[index].get("public_data", "default");
        
        if(dataManager->m_user_id.compare(user_id.asString()) == 0)
        {
            continue;
        }
        
        DyFriend* _friend = new DyFriend();
        _friend->m_friendType = DyFriendType_Gamer;
        _friend->m_user_id = user_id.asString();
        _friend->m_nickname = DyLib::getCorrectNickname(nickname.asString());
 
        
        if(app_friends[index].get("last_message_sent_at", 0).isNull())
            _friend->m_time_last_message_sent_at = CCDate(0);
        else
            _friend->m_time_last_message_sent_at = CCDate(app_friends[index].get("last_message_sent_at", 0).asDouble());
        
        
        _friend->m_message_blocked = app_friends[index].get("message_blocked", 0).asInt();
        
        _friend->m_profile_image_url = profile_image_url.asString();
       _friend->m_sprPicture = NULL;
        
        dataManager->dataLoad_public(&_friend->m_public, public_data.asString(), false);
        m_arrFriends->addObject(_friend);
        _friend->release();
    }
    
    

    Json::Value friends = root.get("friends", ""); // 게임내 리더보드 목록
    
    for( int index = 0; index < friends.size(); ++index )
    {
        Json::Value nickname = friends[index].get("nickname", "default");
        Json::Value profile_image_url = friends[index].get("profile_image_url", "");
        Json::Value user_id = friends[index].get("user_id", "default");
        
        if(dataManager->m_user_id.compare(user_id.asString()) == 0)
            continue;
        
        if(getFriend(user_id.asString().c_str()))
            continue;
      
        DyFriend* _friend = new DyFriend();
        
        _friend->m_friendType = DyFriendType_Normal;
        _friend->m_user_id = user_id.asString();

        _friend->m_nickname = DyLib::getCorrectNickname(nickname.asString());
        
        if(friends[index].get("last_message_sent_at", 0).isNull())
            _friend->m_time_last_message_sent_at = CCDate(0);
        else
            _friend->m_time_last_message_sent_at = CCDate(friends[index].get("last_message_sent_at", 0).asDouble());
        
        if(_friend->m_time_last_message_sent_at.time() == 0)
        {
            _friend->m_time_last_message_sent_at = CCDate(getTimeInvitedFriend(_friend->m_user_id));
        }
  
        _friend->m_profile_image_url = profile_image_url.asString();
        
        _friend->m_message_blocked = app_friends[index].get("message_blocked", 0).asInt();
        
        _friend->m_sprPicture = NULL;
        m_arrFriends->addObject(_friend);
        _friend->release();
    }
    
    
    if(this->getI() == NULL)
    {
        DyFriend* _friend = new DyFriend();
        _friend->m_friendType = DyFriendType_Mine;
        _friend->m_user_id = dataManager->m_user_id;
        _friend->m_nickname = DyLib::getCorrectNickname(dataManager->m_nickname);
        
        _friend->m_profile_image_url = dataManager->m_profile_image_url;
        _friend->m_sprPicture = NULL;
        
        memcpy(&_friend->m_public, &dataManager->m_public, sizeof(DyDataPublic));
        
        m_arrFriends->addObject(_friend);
        _friend->release();
    }

}


void DyFriendManager::loadInvitedFriend()
{
    m_arrInvited->removeAllObjects();
    
    if(YtLib::fileExist(DATA_FILE_INVITE))
	{
        int size = YtLib::fileGetSize(DATA_FILE_INVITE);
        
        char* buffer = (char*)malloc(size);
        
        YtLib::fileRead(DATA_FILE_INVITE, (char *)buffer, size);
        
        int offset = 0;
        
        unsigned char version;
        memcpy(&version, buffer + offset, 1);offset += 1;
        
        for(int i = 0;i < size/INVITE_UNIT_SIZE;i++)
        {
            DyFriendInvite* friendInvite = new DyFriendInvite();
            
            memcpy(friendInvite->m_user_id, buffer + offset, 32);offset += 32;
            memcpy(&friendInvite->m_time_last_message_send_at, buffer + offset, 8);offset += 8;
            
            DyLib* lib = DyLib::sharedLib();
            
            CCDate(friendInvite->m_time_last_message_send_at).to_str(lib->m_strTemp, 128);
            
            m_arrInvited->addObject(friendInvite);
            friendInvite->release();
        }
        free(buffer);
	}
}


void DyFriendManager::saveInviteFirend()
{
    int offset = 0;
    char* buffer = (char*)malloc((INVITE_UNIT_SIZE)*m_arrInvited->count()+1);
    
    unsigned char version = DYDATA_INVITE_VER;
    memcpy(buffer+offset, &version, 1);offset += 1;
    
    for(int i = 0;i < m_arrInvited->count();i++)
    {
        DyFriendInvite* friendInvite = (DyFriendInvite*)m_arrInvited->objectAtIndex(i);
        
        memcpy(buffer+offset, friendInvite->m_user_id, 32);offset += 32;
        memcpy(buffer+offset, &friendInvite->m_time_last_message_send_at, 8);offset += 8;
    }
    
    YtLib::fileWrite(DATA_FILE_INVITE, buffer, offset);
    
    free(buffer);
}


void DyFriendManager::addInvitedFriend(std::string user_id, float datetime)
{
    bool isHas = false;
    for(unsigned int i = 0;i < m_arrInvited->count();i++)
	{
		DyFriendInvite* friendInvite  = (DyFriendInvite*)m_arrInvited->objectAtIndex(i);
        
		if(user_id.compare(friendInvite->m_user_id) == 0)
		{
            friendInvite->m_time_last_message_send_at = datetime;
            isHas = true;
            break;
        }
    }
    
    if(!isHas)
    {
        DyFriendInvite* friendInvite = new DyFriendInvite();
       
        strcpy(friendInvite->m_user_id, user_id.c_str());
        friendInvite->m_time_last_message_send_at = datetime;
        
        m_arrInvited->addObject(friendInvite);
        friendInvite->release();
    }
    saveInviteFirend();
}


double DyFriendManager::getTimeInvitedFriend(std::string user_id)
{
    for(unsigned int i = 0;i < m_arrInvited->count();i++)
	{
		DyFriendInvite* friendInvite  = (DyFriendInvite*)m_arrInvited->objectAtIndex(i);
        
		if(user_id.compare(friendInvite->m_user_id) == 0)
		{
            return friendInvite->m_time_last_message_send_at;
        }
    }
    return 0;
}



DyFriendManager* DyFriendManager::sharedFriendManager()
{
	if(g_sharedFriendManager == NULL)
	{
		g_sharedFriendManager = new DyFriendManager();
	}
	return g_sharedFriendManager;
}





