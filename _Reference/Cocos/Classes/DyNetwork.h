/*
DyNetwork.h
Doggy chef
@initialize at 131010

@history
131215  yoonsr  정리
131010  yoonsr	initialize
*/


#ifndef __DYNETWORK_H__
#define __DYNETWORK_H__

#include "YtLib.h"
#include "YtCommand.h"

using namespace cocos2d;

USING_NS_CC_EXT;


#define DYNETWORK_ADD_PROC(type, param, param_ptr, callFunc)    {DyNetworkCommand* commandNetwork = new DyNetworkCommand(type, param, param_ptr, callFunc);\
DyNetworkManager::sharedNetworkManager()->addLast(commandNetwork);\
commandNetwork->release();}


typedef enum tag_DyNetworkCmdType
{
	DyNetworkCmdType_LoadGameInfo,
    DyNetworkCmdType_LoadMe,
    DyNetworkCmdType_UpdateMe,
    
    DyNetworkCmdType_Invite,
    DyNetworkCmdType_SendMessage,
    DyNetworkCmdType_SendPresent,
    DyNetworkCmdType_SendLinkMessage,
    
    DyNetworkCmdType_PriceVerify,
    DyNetworkCmdType_ScoreMigration,
    DyNetworkCmdType_ScoreGet,
    
    DyNetworkCmdType_ScoreRegist,
    DyNetworkCmdType_ScoreFriendsGet,
    
    DyNetworkCmdtype_UserRegist,
    
    DyNetworkCmdtype_TotalScoreGet,
    
    DyNetworkCmdType_NicknameExist,
    DyNetworkCmdType_NicknameRegist,
    
    DyNetworkCmdType_UserScoreGet
    
}DyNetworkCmdType;

typedef enum tag_DyNetworkCmdStatus
{
	DyNetworkCmdStatus_Ready = 0,
	DyNetworkCmdStatus_Process = 1,
	DyNetworkCmdStatus_End = 2,
}DyNetworkCmdStatus;


typedef void (*DyNetwork_CallFunc)(DyNetworkCmdType cmdType, int value_int, void* value_str);


class DyNetworkCommand : public CCObject
{
private:
    DyNetworkCmdStatus m_status;
	DyNetworkCmdType m_commandtype;
    std::string m_param;
    void* m_param_ptr;
public:
    DyNetwork_CallFunc m_callFunc;
    int m_retInt;
    std::string m_retString;
    int m_retCode;
    
    void process();
    
    void net_LoadGameInfo();
    void net_LoadGameMe();
    
    void callfunc(float dt);
   
    std::string getMessage(std::string message, int code);
    
    DyNetworkCmdStatus getStatus();
    void setStatus(DyNetworkCmdStatus status);
    
    DyNetworkCmdType getCmdType();
    void setCmdType(DyNetworkCmdType type);
    
    void onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response);

	DyNetworkCommand(DyNetworkCmdType type, std::string param, void* param_ptr, DyNetwork_CallFunc callFunc);
	~DyNetworkCommand();
};


class DyNetworkManager : public CCObject
{
private:
    
    CCArray* m_poolCommands;
    
public:
	DyNetworkManager();
	~DyNetworkManager();

    void update(float dt);
	void addLast(DyNetworkCommand* command);
	DyNetworkCommand* getCommand(int index);
    
	static DyNetworkManager* sharedNetworkManager();
};


#endif  // __DYNETWORK_H__