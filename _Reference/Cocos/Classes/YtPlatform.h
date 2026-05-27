/*
YtPlatform.h
YongTrim Lib
@initialize at 121220

@history
121220	yoonsr	initialize
*/


#ifndef __YTPLATFORM_H__
#define __YTPLATFORM_H__

#ifdef __cplusplus
extern "C" {
#endif
    
extern void DyMoneyManager_VerifyPrice(const char* data, const char* signature);
void YtPlatform_InitInApp();

void YtLib_updateBatteryStatus(int level);
int YtPlatform_GetBatteryLevel();
void YtPlatform_contumeProdcut();

    
typedef void (*YtPlatform_CallFunc)(int value, int code);
typedef void (*YtPlatform_CallFuncO)(void* value, int code);

void YtPlatform_NetKakao_IsLogin(YtPlatform_CallFunc callFunc);
void YtPlatform_NetKakao_Login(int isGuest, YtPlatform_CallFunc callFunc, YtPlatform_CallFunc callFuncStart);
void YtPlatform_NetKakao_Logout(YtPlatform_CallFunc callFunc);
void YtPlatform_NetKakao_Unregist(YtPlatform_CallFunc callFunc);
void YtPlatform_NetKakao_DeleteMe(YtPlatform_CallFunc callFunc);
void YtPlatform_NetKakao_GetUserInfo(YtPlatform_CallFuncO callFunc);
void YtPlatform_NetKakao_GetFriends(YtPlatform_CallFuncO callFunc);
void YtPlatform_NetKakao_SendMessage(YtPlatform_CallFuncO callFunc, const char* receiverID, const char* message, const char *executeURLString);
void YtPlatform_NetKakao_LoadGameFriends(YtPlatform_CallFuncO callFunc);
void YtPlatform_NetKakao_LoadLeaderBoard(YtPlatform_CallFuncO callFunc, int type);
void YtPlatform_NetKakao_SendPride(YtPlatform_CallFunc callFunc, const char* receiverID, const char* sender_nick, const char* score, const char* shop_name, const char* stage, const char *executeURLString);

    
void YtPlatform_NetKakao_LoadGameInfo(YtPlatform_CallFuncO callFunc);
void YtPlatform_NetKakao_LoadGameMe(YtPlatform_CallFuncO callFunc);
    
    
void YtPlatform_CopyToClipBoard(const char* user_id);
void YtPlatform_SendEmail(const char* user_id, const char* app_ver);
    
typedef enum tag_NetKakaSendDataType
{
    NetKakaSendDataType_updateMe = 0,
    NetKakaSendDataType_useHeart,
    NetKakaSendDataType_updateResult,
}NetKakaSendDataType;
    
void YtPlatform_NetKakao_SendData(YtPlatform_CallFunc callFunc, NetKakaSendDataType type, const char* data);
void YtPlatform_NetKakao_SendGameMessage(YtPlatform_CallFuncO callFunc, const char* data);
void YtPlatform_NetKakao_LoadGameMessages(YtPlatform_CallFuncO callFunc);
void YtPlatform_NetKakao_AcceptMessage(YtPlatform_CallFunc callFunc, const char* messageid);
void YtPlatform_NetKakao_AcceptAllMessages(YtPlatform_CallFunc callFunc);
void YtPlatform_NetKakao_Invite(YtPlatform_CallFuncO callFunc, const char* user_id, const char* message);
void YtPlatform_NetKakao_MessageBlock(YtPlatform_CallFunc callFunc, int messageBlocked);

void YtPlatform_ShowIndicator(int show);
    
void YtPlatform_GoWeb(const char* strUrl);
void YtPlatform_GoWebView(const char* strUrl);
void YtPlatform_IAB(YtPlatform_CallFunc callFunc, const char* product_id, const char* payload);
    
void YtPlatform_PushOnOff(YtPlatform_CallFunc callFunc, int onoff);

void YtPlatform_ShowAd(int show);
    
void YtPlatform_RunBackground(int value);

void YtPlatform_CreateShort();
    
void YtPlatform_CheckPush();
    
void YtPlatform_ShowNicknameView(YtPlatform_CallFuncO callFunc, const char* dafaultName, int retry);
    

    
#ifdef __cplusplus
}
#endif
#endif // __YTPLATFORM_H__
