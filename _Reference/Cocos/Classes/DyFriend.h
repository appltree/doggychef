/*
DyFriend.h
Doggy chef
@initialize at 121231

@history
121231	yoonsr	initialize
*/


#ifndef __DYFRIEND_H__
#define __DYFRIEND_H__

#include "DyConfig.h"

#include "YtObject.h"
#include "DyCharacter.h"
#include "YtLoader.h"
#include "YtCommand.h"
#include "DyItem.h"
#include "DyData.h"
#include "YtSpriteEx.h"
typedef enum tag_DyFriendType
{
	DyFriendType_Mine,
	DyFriendType_Gamer,
	DyFriendType_Normal,
    
	DyFriendType_Total,
}DyFriendType;

class DyFriend : public CCObject
{
public:
	DyFriendType m_friendType;
    bool m_isDummy;

	std::string m_user_id;
	std::string m_nickname;
    std::string m_profile_image_url;
    
    DyDataPublic m_public;

    DyCostumeFunction m_costume_function;
    
  	YtAsyncSprite *m_sprPicture;
  
    int m_tmpTag;
    
    CCDate m_time_last_message_sent_at;
    
    bool m_message_blocked;
    
    
    CCArray* m_arrShopPoint;
    
	DyFriend();
	~DyFriend();
};


class DyFriendInvite : public CCObject
{
public:
    char m_user_id[32];
    double m_time_last_message_send_at;
    
    DyFriendInvite();
};


class DyFriendManager : public CCObject
{
public:
	CCArray* m_arrFriends;
    
    CCArray* m_arrDummy;

	DyFriendManager();
	~DyFriendManager();

	DyFriend* getFriend(std::string id);
    DyFriend* getI();
    
    CCSprite* getPicture(DyFriend* friendInfo, CCSize size);
  
    void loadFriends(std::string data);
    
    void updateMine();
    
    void initFriendsTag();
    
    void saveFriendsScore(int shop, std::string data);
    
    
    CCArray* m_arrInvited;
    
    void loadInvitedFriend();
    void saveInviteFirend();
    void addInvitedFriend(std::string user_id, float datetime);
    double getTimeInvitedFriend(std::string user_id);
    
    
	static DyFriendManager* sharedFriendManager();
};


#endif  // __DYFRIEND_H__