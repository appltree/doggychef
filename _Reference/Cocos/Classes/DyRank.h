/*
DyFriend.h
Doggy chef
@initialize at 121231

@history
121231	yoonsr	initialize
*/


#ifndef __DYRANK_H__
#define __DYRANK_H__

#include "DyConfig.h"

#include "YtObject.h"
#include "DyCharacter.h"
#include "YtLoader.h"
#include "YtCommand.h"
#include "DyItem.h"
#include "DyData.h"

class DyFriend;

typedef enum tag_DyRankType
{
    DyRankType_Stage,
    DyRankType_Crazy,
    DyRankType_SubStage,
    DyRankType_Invalid,
    
}DyRankType;
class DyRank : public CCObject
{
public:
	int m_order;
    
	int m_point;
    int m_pointBest;
    
	DyFriend* m_friend;

	DyRank();
	~DyRank();
};


class DyRankGlobal : public CCObject
{
public:
    
	int m_order;
    std::string m_nickname;
    
    int m_point;
    
    int m_stage_record;
    int m_crazy_record;
    std::string m_picture_url;
    std::string m_user_id;
    DyDataPublic m_public;
    
    DyCostumeFunction m_costume_function;
    
};




class DyRankManager : public CCObject
{
public:
    
    CCArray* m_arrTotalStory;
    CCArray* m_arrTotalCrazy;
    
	CCArray* m_arrRankStory;
    CCArray* m_arrRankCrazy;
  
	DyRankManager();
	~DyRankManager();

    
	CCArray* sortRank(DyRankType type);
    CCArray* getArray(DyRankType type);
    
    void loadLeaderBoard(DyRankType type, std::string data);
    
    DyRankType m_curRankType;
    int m_initMyOrder;
    
    
    DyRankType currentCheckRankType();
    
    void initCheckPoint(DyRankType type);
    
    bool setCheckPoint();
    
    DyRank* getCurrrentTarget(DyRankType type, int point);
    
    DyRank* getMyRank(DyRankType type);
    DyRank* getPrevFriendRank(DyRankType type);
    DyRank* getNextFriendRank(DyRankType type);
    
    
    DyRankType loadGlobalRank(std::string data);
	static DyRankManager* sharedRankManager();
};


#endif  // __DYFRIEND_H__