/*
YtPlatform_ios.c
YongTrim Lib
@initialize at 121220

@history
121220	yoonsr	initialize
*/

#if 0
#include "YtPlatform.h"

extern int AppController_GetBatteryLevel();
extern void AppController_ShowIndicator(bool show);

extern void AppController_NetKakao_IsLogin(YtPlatform_CallFunc callFunc);
extern void AppController_NetKakao_Login(bool isGuest, YtPlatform_CallFunc callFunc);
extern void AppController_NetKakao_Logout(YtPlatform_CallFunc callFunc);
extern void AppController_NetKakao_Unregist(YtPlatform_CallFunc callFunc);
extern void AppController_NetKakao_DeleteMe(YtPlatform_CallFunc callFunc);

extern void AppController_NetKakao_GetUserInfo(YtPlatform_CallFuncO callFunc);
extern void AppController_NetKakao_GetFriends(YtPlatform_CallFuncO callFunc);
extern void AppController_NetKakao_SendMessage(YtPlatform_CallFuncO callFunc, const char* receiverID, const char* message, const char *executeURLString);
extern void AppController_NetKakao_LoadGameInfo(YtPlatform_CallFuncO callFunc);
extern void AppController_NetKakao_LoadGameMe(YtPlatform_CallFuncO callFunc);
extern void AppController_NetKakao_SendData(YtPlatform_CallFunc callFunc, int type, const char* data);
extern void AppController_NetKakao_LoadGameFriends(YtPlatform_CallFuncO callFunc);
extern void AppController_NetKakao_LoadLeaderBoards(YtPlatform_CallFuncO callFunc, int type);
extern void AppController_CopyToClipBoard(const char* user_id);
extern void AppController_SendEmail(const char* user_id, const char* app_ver);
extern void AppController_NetKakao_SendGameMessage(YtPlatform_CallFuncO callFunc, const char* data);
extern void AppController_NetKakao_LoadGameMessages(YtPlatform_CallFuncO callFunc);
extern void AppController_NetKakao_AcceptMessage(YtPlatform_CallFunc callFunc, const char* messageid);
extern void AppController_NetKakao_AcceptAllMessages(YtPlatform_CallFunc callFunc);
extern void AppController_NetKakao_Invite(YtPlatform_CallFuncO callFunc, const char* user_id, const char* message);
extern void AppController_NetKakao_MessageBlock(YtPlatform_CallFunc callFunc, bool messageBlocked);
extern void AppController_GoWeb(const char* strUrl);
extern void AppController_GoWebView(const char* strUrl);
extern void AppController_IAP(YtPlatform_CallFunc callFunc, const char* strProduct);
extern void AppController_ShowAd(bool show);
extern void AppController_Push(bool on);
extern void AppController_CheckPush();
extern void AppController_NetKakao_SendPride(YtPlatform_CallFunc callFunc, const char* receiverID, const char* sender_nick, const char* score, const char* shop_name, const char* stage, const char *executeURLString);
extern void AppController_ShowNicknameView(YtPlatform_CallFuncO callFunc, const char* dafaultName, int retry);

void YtPlatform_InitInApp()
{
    
}

//int g_platform_notProcessDelly;

int YtPlatform_GetBatteryLevel()
{
    //UIDevice *myDevice = [UIDevice currentDevice];
    
	return AppController_GetBatteryLevel();
}

//char* YtPlatform_GetUDID()
//{
//	//return "WINDOW_UDID";
//	//return "WINDOW_UDID_FRIEND0";
//	return (char* )"WINDOW_UDID_FRIEND1";
//	//return "WINDOW_UDID_FRIEND2";
//	//return "WINDOW_UDID_FRIEND3";
//	//return "WINDOW_UDID_FRIEND4";
//	//return "WINDOW_UDID_FRIEND5";
//	//return "WINDOW_UDID_FRIEND6";
//	//return "WINDOW_UDID_FRIEND7";
//}
//
//char* YtPlatform_GetPhonenumber()
//{
//	//return "+8201012345670";
//	//return "+8201012345671";
//	return (char* )"+8201012345672";
//	//return "+8201012345673";
//	//return "+8201012345674";
//	//return "+8201012345675";
//	//return "+8201012345676";
//	//return "+8201012345677";
//	//return "+8201012345678";
//	//return "+8201012345679";
//    
//}
//

//char* YtPlatform_GetContactList()
//{
//	//return (char* )"ZORO\t01022223333\nPPIYONG\t01033334444\nDONGDONG\t01044445555";
//	return (char* )"ZORO\t01022223333\nPPIYONG\t01033334444\nDONGDONG\t01044445555\nDONGDONG0\t01044445552\nDONGDONG1\t01044445558\nDONGDONG2\t01044445510\nDONGDONG3\t01044445555\nDONGDONG4\t01044445111\nDONGDONG5\t01044441111\nDONGDONG6\t01044445123";
//	//return (char* )"ZORO\t01022223333";
//}



void YtPlatform_NetKakao_IsLogin(YtPlatform_CallFunc callFunc)
{
    AppController_NetKakao_IsLogin(callFunc);
}


void YtPlatform_NetKakao_Login(int isGuest, YtPlatform_CallFunc callFunc, YtPlatform_CallFunc callFuncStart)
{
    AppController_NetKakao_Login(isGuest, callFunc);
}


void YtPlatform_NetKakao_Logout(YtPlatform_CallFunc callFunc)
{
    AppController_NetKakao_Logout(callFunc);
}


void YtPlatform_NetKakao_Unregist(YtPlatform_CallFunc callFunc)
{
    AppController_NetKakao_Unregist(callFunc);
}


void YtPlatform_NetKakao_DeleteMe(YtPlatform_CallFunc callFunc)
{
    AppController_NetKakao_DeleteMe(callFunc);
}



void YtPlatform_NetKakao_GetUserInfo(YtPlatform_CallFuncO callFunc)
{
    AppController_NetKakao_GetUserInfo(callFunc);
}

void YtPlatform_NetKakao_GetFriends(YtPlatform_CallFuncO callFunc)
{
    AppController_NetKakao_GetFriends(callFunc);
}


void YtPlatform_NetKakao_SendMessage(YtPlatform_CallFuncO callFunc, const char* receiverID, const char* message, const char *executeURLString)
{
    AppController_NetKakao_SendMessage(callFunc, receiverID, message, executeURLString);
}


void YtPlatform_NetKakao_SendPride(YtPlatform_CallFunc callFunc, const char* receiverID, const char* sender_nick, const char* score, const char* shop_name, const char* stage, const char *executeURLString)
{
    AppController_NetKakao_SendPride(callFunc, receiverID, sender_nick, score, shop_name, stage, executeURLString);
}


void YtPlatform_NetKakao_LoadGameInfo(YtPlatform_CallFuncO callFunc)
{
    AppController_NetKakao_LoadGameInfo(callFunc);
}



void YtPlatform_NetKakao_LoadGameMe(YtPlatform_CallFuncO callFunc)
{
    AppController_NetKakao_LoadGameMe(callFunc);
}


void YtPlatform_NetKakao_LoadGameFriends(YtPlatform_CallFuncO callFunc)
{
    AppController_NetKakao_LoadGameFriends(callFunc);
}


void YtPlatform_NetKakao_LoadLeaderBoard(YtPlatform_CallFuncO callFunc, int type)
{
    AppController_NetKakao_LoadLeaderBoards(callFunc, type);
}


void YtPlatform_ShowIndicator(int show)
{
   AppController_ShowIndicator(show);
}


void YtPlatform_NetKakao_SendData(YtPlatform_CallFunc callFunc, NetKakaSendDataType type, const char* data)
{
    AppController_NetKakao_SendData(callFunc, type, data);
}




void YtPlatform_NetKakao_SendGameMessage(YtPlatform_CallFuncO callFunc, const char* data)
{
    AppController_NetKakao_SendGameMessage(callFunc, data);
}


void YtPlatform_NetKakao_AcceptMessage(YtPlatform_CallFunc callFunc, const char* messageid)
{
    AppController_NetKakao_AcceptMessage(callFunc, messageid);
}


void YtPlatform_NetKakao_AcceptAllMessages(YtPlatform_CallFunc callFunc)
{
    AppController_NetKakao_AcceptAllMessages(callFunc);
}



void YtPlatform_NetKakao_LoadGameMessages(YtPlatform_CallFuncO callFunc)
{
    AppController_NetKakao_LoadGameMessages(callFunc);
}


void YtPlatform_NetKakao_Invite(YtPlatform_CallFuncO callFunc, const char* user_id, const char* message)
{
    AppController_NetKakao_Invite(callFunc, user_id, message);
}


void YtPlatform_NetKakao_MessageBlock(YtPlatform_CallFunc callFunc, int messageBlocked)
{
    AppController_NetKakao_MessageBlock(callFunc, messageBlocked);
}


void YtPlatform_CopyToClipBoard(const char* user_id)
{
    AppController_CopyToClipBoard(user_id);
    //http://stackoverflow.com/questions/1479468/copy-text-to-clipboard-with-iphone-sdk
    //http://stackoverflow.com/questions/238284/how-to-copy-text-programatically-in-my-android-app
}


void YtPlatform_SendEmail(const char* user_id, const char* app_ver)
{
    AppController_SendEmail(user_id, app_ver);
}

void YtPlatform_GoWeb(const char* strUrl)
{
    AppController_GoWeb(strUrl);
}

void YtPlatform_GoWebView(const char* strUrl)
{
    AppController_GoWebView(strUrl);
}

void YtPlatform_IAB(YtPlatform_CallFunc callFunc, const char* product_id, const char* payload)
{
    AppController_IAP(callFunc, product_id);
}


void YtPlatform_contumeProdcut()
{
    
}

void YtPlatform_PushOnOff(YtPlatform_CallFunc callFunc, int onoff)
{
    AppController_Push(onoff);

    callFunc(0, 0);
}

void YtPlatform_ShowAd(int show)
{
    AppController_ShowAd(show);
}

void YtPlatform_RunBackground(int value)
{
    
}

void YtPlatform_CreateShort()
{
    
}


void YtPlatform_CheckPush()
{
    AppController_CheckPush();
}


void YtPlatform_ShowNicknameView(YtPlatform_CallFuncO callFunc, const char* dafaultName, int retry)
{
    AppController_ShowNicknameView(callFunc, dafaultName, retry);
}

#endif


