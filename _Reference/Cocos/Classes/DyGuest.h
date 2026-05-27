/*
DyFriend.h
Doggy chef
@initialize at 121231

@history
121231	yoonsr	initialize
*/


#ifndef __DYGUEST_H__
#define __DYGUEST_H__

#include "DyConfig.h"

#include "YtObject.h"
#include "DyCharacter.h"
#include "YtLoader.h"
#include "YtCommand.h"
#include "DyItem.h"
#include "DyData.h"
#include "DyFriend.h"


typedef enum tag_DyGuestType
{
    DyGuestType_Default,
    DyGuestType_Friend,
    DyGuestType_Admin,
    DyGuestType_Buyed,
    DyGuestType_Gacha,
    DyGuestType_Unkown,
}DyGuestType;


class DyGuest : public CCObject
{
public:
    DyGuestType m_guesttype;
    
	char m_sender_id[32];
    
	DyGuest();
	~DyGuest();
};


class DyGuestManager : public CCObject
{
public:
    
    DyGuestManager();
    ~DyGuestManager();

    CCArray* m_arrGuest;
    
    DyGuest* m_curGuest;
    
    DyFriend* m_processFriend;
    CCSprite* m_sprPictureFrame;
    
    bool m_isIdle;
    bool m_isInvite;
    
    void setIdle(bool idle);
    bool isIdle();
    
    void setInvite(bool invite);
    bool isInvite();
    
    void addGuest(int count, DyGuestType type, const char* user_id);
    void useGuest();
    
    std::string getCurrentGuestName();
    CCSprite* getCurrentGuestPicture();
    
    void loadGuest();
    void saveGuest();
    
    void setupGuest();
    
    //void onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response);

    
    static DyGuestManager* sharedGuestManager();
};


#endif  // __DYFRIEND_H__