/*
DyGift.h
Doggy chef
@initialize at 121231

@history
121231	yoonsr	initialize
*/


#ifndef __DYGIFT_H__
#define __DYGIFT_H__

#include "DyConfig.h"

#include "YtObject.h"
#include "DyCharacter.h"
#include "YtLoader.h"
#include "YtCommand.h"
#include "DyItem.h"
#include "DyData.h"

typedef enum tag_DyGiftType
{
	DyGiftTypeDelly,
	DyGiftTypeGaest,
	DyGiftTypeGum,
    DyGiftTypeRoullette,
    DyGiftTypeGold,
    DyGiftTypeCostumeSet,
    
}DyGiftType;


class DyFriend;

class DyGift : public CCObject
{
public:
    
    bool m_is_main;
    
	DyFriend* m_sender;
    
    DyGiftType m_gifttype;
	int m_count;
	bool m_isAccepted;

    std::string m_message;
    std::string m_gift_id;
    bool m_all;
    
    CCDate m_time_send_at;
    
    
	DyGift();
	~DyGift();
};

class DyGiftManager : public CCObject
{
public:
	CCArray* m_arrReceivedGifts;

	DyGiftManager();
	~DyGiftManager();

    
    void loadGifts(std::string data);
    bool loadGifts_ByMain(std::string data);

	//void acceptByIndex(int index);
    
    void accept(DyGift* gift, bool acceptAndRemove);

    
    void accept_all();
    
    int getGiftCount();
    DyGift* getGift(int index);
    
	static DyGiftManager* sharedGiftManager();
};



#endif  // __DYGIFT_H__