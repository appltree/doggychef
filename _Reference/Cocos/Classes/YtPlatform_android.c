/*
YtPlatform_android.cpp
YongTrim Lib
@initialize at 121220

@history
121220	yoonsr	initialize
*/

#include "YtPlatform.h"

#if 1
#ifdef ANDROID

#include <jni.h>
#include <sys/time.h>
#include <time.h>
#include <android/log.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define GET_CLASS jNativesCls = (*env)->FindClass(env, "com/yongtrim/lib/YtNative");
#define FREE_CLASS (*env)->DeleteLocalRef(env, jNativesCls);

//
//#define TRUE	1
//#define FALSE	0
//
//extern YYNetwork_plat g_network_plat;
//

JavaVM *g_VM;
jclass jNativesCls;
//
char g_app_version[80];
char g_app_param[80];

char g_android_buffer[512];
char g_android_buffer2[512];

char* g_android_contactList;

//int g_platform_notProcessDelly;
//
void Java_com_yongtrim_lib_YtNative_initialize(JNIEnv * env, jclass cls, jobjectArray jargv)
{
	(*env)->GetJavaVM(env, &g_VM);

	GET_CLASS
    
	// Extract char ** args from Java array
	jsize clen = (*env)->GetArrayLength(env, jargv);
	
	char* args[(int)clen];

	int i;
	jstring jrow;

	for(i = 0;i < clen; i++)
	{
		jrow = (jstring)(*env)->GetObjectArrayElement(env, jargv, i);
	    const char *row = (*env)->GetStringUTFChars(env, jrow, 0);
		args[i] = (char*)malloc(strlen(row) + 1);
		strcpy(args[i], row);
	    (*env)->ReleaseStringUTFChars(env, jrow, row);
	}

	strcpy(g_app_version, args[0]);
	strcpy(g_app_param, args[1]);
    
    FREE_CLASS
}

void YtPlatform_InitInApp()
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "initializeInAppBilling", "(Ljava/lang/String;)V");
    
    jstring jstrKey = (*env)->NewStringUTF(env, "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAioNBGp+0Bur2eVkc51+r0LrIrbIsmDp/05BeDRM6U118wEvAawVHKXNt4V2jx8QhIknbk3YJaGDyHI3o89Yp4F0aCIDV+qxqBAk1mO0RpPysCinXZ1cNrgXLXzwKlzxFe+LKiJ9nXXLhuLp34U0/wuxuqJrDUq16+gljjLP3WXj9AmdsnL4uMPZXqRGpURgHxIxd2L2nVGpK2uQyi9nX0kMD+4hIwPfpTBcxlJsVZoPWdNp6k5uo45p62pXwIrV6ssLzm2fyLkcDRjSCx9AVDXEHS9f7Bh2Zvc47u3+986VfplwrlrseIGrVsXfW3EcEJGaE/OEv8CSKqzdgZUWP0wIDAQAB");
    
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstrKey);
    
    (*env)->DeleteLocalRef(env, jstrKey);
    
    FREE_CLASS
}

void Java_com_yongtrim_lib_YtNative_verifyPrice(JNIEnv * env, jclass cls, jbyteArray bytearrayData, jbyteArray bytearraySignature)
{
	size_t lenData = (*env)->GetArrayLength(env, bytearrayData);
	jbyte *nativeBytesData = (*env)->GetByteArrayElements(env, bytearrayData, 0);
    
	memcpy(g_android_buffer, nativeBytesData, lenData);
	g_android_buffer[lenData] = 0;
	
	(*env)->ReleaseByteArrayElements(env, bytearrayData, nativeBytesData, JNI_ABORT);
   
    
    size_t lenSignature = (*env)->GetArrayLength(env, bytearraySignature);
	jbyte *nativeBytesSignature = (*env)->GetByteArrayElements(env, bytearraySignature, 0);
    
	memcpy(g_android_buffer2, nativeBytesSignature, lenSignature);
	g_android_buffer2[lenSignature] = 0;
	
	(*env)->ReleaseByteArrayElements(env, bytearraySignature, nativeBytesSignature, JNI_ABORT);
    
    
    DyMoneyManager_VerifyPrice(g_android_buffer, g_android_buffer2);
}


void YtPlatform_contumeProdcut()
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "contumeProdcut", "(I)V");
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    
    FREE_CLASS
}



void Java_com_yongtrim_lib_YtNative_updateBatteryStatus(JNIEnv* env, jobject thiz, jint level)
{
	YtLib_updateBatteryStatus(level);
}

int YtPlatform_GetBatteryLevel()
{
	return 100;
}


void Java_com_yongtrim_lib_YtNative_setUDID(JNIEnv * env, jclass cls, jbyteArray bytearray)
{
	size_t len = (*env)->GetArrayLength(env, bytearray); 
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0); 

	memcpy(g_android_buffer, nativeBytes, len);
	g_android_buffer[len] = 0;
	
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT); 
}

void Java_com_yongtrim_lib_YtNative_setPhonenumber(JNIEnv * env, jclass cls, jbyteArray bytearray)
{
	size_t len = (*env)->GetArrayLength(env, bytearray); 
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0); 

	memcpy(g_android_buffer, nativeBytes, len);
	g_android_buffer[len] = 0;
	
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT); 
}


void Java_com_yongtrim_lib_YtNative_setContactList(JNIEnv * env, jclass cls, jbyteArray bytearray)
{
	size_t len = (*env)->GetArrayLength(env, bytearray); 
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0); 

	if(g_android_contactList)
		free(g_android_contactList);
	g_android_contactList = malloc(len+1);
	memcpy(g_android_contactList, nativeBytes, len);
	g_android_contactList[len] = 0;

	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT); 
}


YtPlatform_CallFunc callFuncIsLogin;

void Java_com_yongtrim_lib_YtNative_NetKakaoIsLoginRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
	//YtLib_updateBatteryStatus(level);
    callFuncIsLogin(value, code);
}


void YtPlatform_NetKakao_IsLogin(YtPlatform_CallFunc callFunc)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoIsLogin", "(I)V");
    
    callFuncIsLogin = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    FREE_CLASS
}


YtPlatform_CallFunc callFuncLogout;

void Java_com_yongtrim_lib_YtNative_NetKakaoLogoutRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
	//YtLib_updateBatteryStatus(level);
    callFuncLogout(value, code);
}

void YtPlatform_NetKakao_Logout(YtPlatform_CallFunc callFunc)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoLogout", "(I)V");
    
    callFuncLogout = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    FREE_CLASS
}




YtPlatform_CallFunc callFuncUnregist;

void Java_com_yongtrim_lib_YtNative_NetKakaoUnregistRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
	//YtLib_updateBatteryStatus(level);
    callFuncUnregist(value, code);
}



void YtPlatform_NetKakao_Unregist(YtPlatform_CallFunc callFunc)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoUnregist", "(I)V");
    
    callFuncUnregist = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    FREE_CLASS
}



YtPlatform_CallFunc callFuncDeleteme;

void Java_com_yongtrim_lib_YtNative_NetKakaoDeletemeRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
	//YtLib_updateBatteryStatus(level);
    callFuncDeleteme(value, code);
}



void YtPlatform_NetKakao_DeleteMe(YtPlatform_CallFunc callFunc)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoDeleteme", "(I)V");
    
    callFuncDeleteme = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    FREE_CLASS
}



YtPlatform_CallFunc callFuncLogin;

YtPlatform_CallFunc callFuncLoginStarted;
void Java_com_yongtrim_lib_YtNative_NetKakaoLoginRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    if(callFuncLogin)
        callFuncLogin(value, code);
    callFuncLogin = NULL;
}

void Java_com_yongtrim_lib_YtNative_NetKakaoLoginStartedRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    callFuncLoginStarted(value, code);
}


void YtPlatform_NetKakao_Login(int isGuest, YtPlatform_CallFunc callFunc, YtPlatform_CallFunc callFuncStart)
{
    if(isGuest)
    {
        callFunc(2, 0);
        return;
    }
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoLogin", "(I)V");
    
    callFuncLogin = callFunc;
    callFuncLoginStarted = callFuncStart;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    FREE_CLASS
}


YtPlatform_CallFuncO callFuncGetUserInfo;

void Java_com_yongtrim_lib_YtNative_NetKakaoGetUserInfoRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
	size_t len = (*env)->GetArrayLength(env, bytearray);
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0);
    
	char* buffer = malloc(len+1);
	memcpy(buffer, nativeBytes, len);
	buffer[len] = 0;
    
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT);
    
    callFuncGetUserInfo(buffer, code);
    free(buffer);
}



void YtPlatform_NetKakao_GetUserInfo(YtPlatform_CallFuncO callFunc)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoGetUserInfo", "(I)V");
    
    callFuncGetUserInfo = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    FREE_CLASS

}




YtPlatform_CallFuncO callFuncGetFriends;

void Java_com_yongtrim_lib_YtNative_NetKakaoGetFriendsRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
	size_t len = (*env)->GetArrayLength(env, bytearray);
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0);
    
	char* buffer = malloc(len+1);
	memcpy(buffer, nativeBytes, len);
	buffer[len] = 0;
    
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT);
    
    callFuncGetFriends(buffer, code);
    free(buffer);
}

void YtPlatform_NetKakao_GetFriends(YtPlatform_CallFuncO callFunc)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoGetFriends", "(I)V");
    
    callFuncGetFriends = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    FREE_CLASS
}




YtPlatform_CallFuncO callFuncSendMessage;

void Java_com_yongtrim_lib_YtNative_NetKakaoSendMessageRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
	size_t len = (*env)->GetArrayLength(env, bytearray);
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0);
    
	char* buffer = malloc(len+1);
	memcpy(buffer, nativeBytes, len);
	buffer[len] = 0;
    
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT);
    
    callFuncSendMessage(buffer, code);
    free(buffer);
}


void YtPlatform_NetKakao_SendMessage(YtPlatform_CallFuncO callFunc, const char* receiverID, const char* message, const char *executeURLString)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoSendMessage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    
    callFuncSendMessage = callFunc;
    
    jstring jstrReceiverID = (*env)->NewStringUTF(env, receiverID);
    jstring jstrMessage = (*env)->NewStringUTF(env, message);
    jstring jstrExecuteURLString = (*env)->NewStringUTF(env, executeURLString);
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstrReceiverID, jstrMessage, jstrExecuteURLString);
    
    (*env)->DeleteLocalRef(env, jstrReceiverID);
    (*env)->DeleteLocalRef(env, jstrMessage);
    (*env)->DeleteLocalRef(env, jstrExecuteURLString);
    
    FREE_CLASS
}



YtPlatform_CallFunc callFuncSendPride;

void Java_com_yongtrim_lib_YtNative_NetKakaoSendPrideRet(JNIEnv * env, jclass cls, jint value, jint code)
{
	callFuncSendPride(value, code);
}


void YtPlatform_NetKakao_SendPride(YtPlatform_CallFunc callFunc, const char* receiverID, const char* sender_nick, const char* score, const char* shop_name, const char* stage, const char *executeURLString)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoSendPride", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    
    callFuncSendPride = callFunc;
    
    jstring jstrReceiverID = (*env)->NewStringUTF(env, receiverID);
    jstring jstrSender_nick = (*env)->NewStringUTF(env, sender_nick);
    jstring jstrScore = (*env)->NewStringUTF(env, score);
    jstring jstrShop_name = (*env)->NewStringUTF(env, shop_name);
    jstring jstrStage = (*env)->NewStringUTF(env, stage);
    jstring jstrExecuteURLString = (*env)->NewStringUTF(env, executeURLString);
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstrReceiverID, jstrSender_nick, jstrScore, jstrShop_name, jstrStage, jstrExecuteURLString);
    
    (*env)->DeleteLocalRef(env, jstrReceiverID);
    (*env)->DeleteLocalRef(env, jstrSender_nick);
    (*env)->DeleteLocalRef(env, jstrScore);
    (*env)->DeleteLocalRef(env, jstrStage);
    (*env)->DeleteLocalRef(env, jstrSender_nick);
    (*env)->DeleteLocalRef(env, jstrExecuteURLString);
    FREE_CLASS
}





jmethodID g_jShowIndicator;

void YtPlatform_ShowIndicator(int enable)
{
	JNIEnv *env;
    
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	if(!g_jShowIndicator)
		g_jShowIndicator = (*env)->GetStaticMethodID(env, jNativesCls, "ShowIndicator", "(I)V");
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, g_jShowIndicator, enable);
    FREE_CLASS
}



YtPlatform_CallFuncO callFuncLoadGameInfo;

void Java_com_yongtrim_lib_YtNative_NetKakaoLoadGameInfoRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
	size_t len = (*env)->GetArrayLength(env, bytearray);
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0);
    
	char* buffer = malloc(len+1);
	memcpy(buffer, nativeBytes, len);
	buffer[len] = 0;
    
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT);
    
    callFuncLoadGameInfo(buffer, code);
    free(buffer);
}


void YtPlatform_NetKakao_LoadGameInfo(YtPlatform_CallFuncO callFunc)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoLoadGameInfo", "(I)V");
    
    callFuncLoadGameInfo = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    FREE_CLASS
}


YtPlatform_CallFuncO callFuncLoadGameMe;

void Java_com_yongtrim_lib_YtNative_NetKakaoLoadGameMeRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
	size_t len = (*env)->GetArrayLength(env, bytearray);
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0);
    
	char* buffer = malloc(len+1);
	memcpy(buffer, nativeBytes, len);
	buffer[len] = 0;
    
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT);
    
    callFuncLoadGameMe(buffer, code);
    free(buffer);
}


void YtPlatform_NetKakao_LoadGameMe(YtPlatform_CallFuncO callFunc)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoLoadGameMe", "(I)V");
    
    callFuncLoadGameMe = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    FREE_CLASS
}


YtPlatform_CallFuncO callFuncLoadGameFriends;

void Java_com_yongtrim_lib_YtNative_NetKakaoLoadGameFriendsRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
	size_t len = (*env)->GetArrayLength(env, bytearray);
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0);
    
	char* buffer = malloc(len+1);
	memcpy(buffer, nativeBytes, len);
	buffer[len] = 0;
    
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT);
    
    callFuncLoadGameFriends(buffer, code);
    free(buffer);
}


void YtPlatform_NetKakao_LoadGameFriends(YtPlatform_CallFuncO callFunc)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoLoadGameFriends", "(I)V");
    
    callFuncLoadGameFriends = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    FREE_CLASS
}



YtPlatform_CallFuncO callFuncLoadLeaderBoard;

void Java_com_yongtrim_lib_YtNative_NetKakaoLoadLeaderBoardRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
	size_t len = (*env)->GetArrayLength(env, bytearray);
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0);
    
	char* buffer = malloc(len+1);
	memcpy(buffer, nativeBytes, len);
	buffer[len] = 0;
    
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT);
    
    callFuncLoadLeaderBoard(buffer, code);
    free(buffer);
}



void YtPlatform_NetKakao_LoadLeaderBoard(YtPlatform_CallFuncO callFunc, int type)
{
	JNIEnv *env;
    
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoLoadLeaderBoard", "(I)V");
    callFuncLoadLeaderBoard = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, type);
    FREE_CLASS
}


YtPlatform_CallFunc callFuncSendData;

void Java_com_yongtrim_lib_YtNative_NetKakaoSendDataRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
	//YtLib_updateBatteryStatus(level);
    callFuncSendData(value, code);
}



void YtPlatform_NetKakao_SendData(YtPlatform_CallFunc callFunc, NetKakaSendDataType type, const char* data)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoSendData", "(Ljava/lang/String;I)V");
    
    callFuncSendData = callFunc;
    
    jstring jstrData = (*env)->NewStringUTF(env, data);
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstrData, type);
    
    (*env)->DeleteLocalRef(env, jstrData);
    FREE_CLASS
    
}



YtPlatform_CallFuncO callFuncSendGameMessage;

void Java_com_yongtrim_lib_YtNative_NetKakaoSendGameMessageRet(JNIEnv* env, jobject thiz, jbyteArray bytearray, jint code)
{
    size_t len = (*env)->GetArrayLength(env, bytearray);
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0);
    
	char* buffer = malloc(len+1);
	memcpy(buffer, nativeBytes, len);
	buffer[len] = 0;
    
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT);
    
    callFuncSendGameMessage(buffer, code);
    free(buffer);
}


void YtPlatform_NetKakao_SendGameMessage(YtPlatform_CallFuncO callFunc, const char* data)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoSendGameMessage", "(Ljava/lang/String;)V");
    
    callFuncSendGameMessage = callFunc;
    
    jstring jstrData = (*env)->NewStringUTF(env, data);
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstrData);
    
    (*env)->DeleteLocalRef(env, jstrData);
    FREE_CLASS
}


YtPlatform_CallFunc callFuncAcceptMessage;

void Java_com_yongtrim_lib_YtNative_NetKakaoAcceptMessageRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    callFuncAcceptMessage(value, code);
}



void YtPlatform_NetKakao_AcceptMessage(YtPlatform_CallFunc callFunc, const char* messageid)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoAcceptMessage", "(Ljava/lang/String;)V");
    
    callFuncAcceptMessage = callFunc;
    
    jstring jstrData = (*env)->NewStringUTF(env, messageid);
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstrData);
    
    (*env)->DeleteLocalRef(env, jstrData);
    FREE_CLASS
}



YtPlatform_CallFunc callFuncAcceptAllMessages;

void Java_com_yongtrim_lib_YtNative_NetKakaoAcceptAllMessagesRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    callFuncAcceptAllMessages(value, code);
}


void YtPlatform_NetKakao_AcceptAllMessages(YtPlatform_CallFunc callFunc)
{
	JNIEnv *env;
    
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoAcceptAllMessages", "(I)V");
    callFuncAcceptAllMessages = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    FREE_CLASS
}





YtPlatform_CallFuncO callFuncLoadGameMessages;

void Java_com_yongtrim_lib_YtNative_NetKakaoLoadGameMessagesRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
	size_t len = (*env)->GetArrayLength(env, bytearray);
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0);
    
	char* buffer = malloc(len+1);
	memcpy(buffer, nativeBytes, len);
	buffer[len] = 0;
    
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT);
    
    callFuncLoadGameMessages(buffer, code);
    free(buffer);
}



void YtPlatform_NetKakao_LoadGameMessages(YtPlatform_CallFuncO callFunc)
{
	JNIEnv *env;
    
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoLoadGameMessages", "(I)V");
    callFuncLoadGameMessages = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
    FREE_CLASS
}





YtPlatform_CallFuncO callFuncInvite;

void Java_com_yongtrim_lib_YtNative_NetKakaoInviteRet(JNIEnv* env, jobject thiz, jbyteArray bytearray, jint code)
{
    size_t len = (*env)->GetArrayLength(env, bytearray);
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0);
    
	char* buffer = malloc(len+1);
	memcpy(buffer, nativeBytes, len);
	buffer[len] = 0;
    
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT);
    
    callFuncInvite(buffer, code);
    free(buffer);
}





void YtPlatform_NetKakao_Invite(YtPlatform_CallFuncO callFunc, const char* user_id, const char* message)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoInvite", "(Ljava/lang/String;Ljava/lang/String;)V");
    
    callFuncInvite = callFunc;
    
    jstring jstrUserID = (*env)->NewStringUTF(env, user_id);
    jstring jstrMessage = (*env)->NewStringUTF(env, message);
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstrUserID, jstrMessage);
    
    (*env)->DeleteLocalRef(env, jstrUserID);
    (*env)->DeleteLocalRef(env, jstrMessage);
    FREE_CLASS
}



YtPlatform_CallFunc callFuncMessageBlock;

void Java_com_yongtrim_lib_YtNative_NetKakaoMessageBlockRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    callFuncMessageBlock(value, code);
}


void YtPlatform_NetKakao_MessageBlock(YtPlatform_CallFunc callFunc, int messageBlocked)
{
	JNIEnv *env;
    
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "NetKakaoMessageBlock", "(I)V");
    callFuncMessageBlock = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, messageBlocked);
    FREE_CLASS
}




void YtPlatform_CopyToClipBoard(const char* user_id)
{
	JNIEnv *env;

	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS;

    jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "CopyToClipBoard", "(Ljava/lang/String;)V");
    
    jstring jstr = (*env)->NewStringUTF(env, user_id);
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstr);
    (*env)->DeleteLocalRef(env,jstr);
    FREE_CLASS
    
    //http://stackoverflow.com/questions/1479468/copy-text-to-clipboard-with-iphone-sdk
    //http://stackoverflow.com/questions/238284/how-to-copy-text-programatically-in-my-android-app
}


void YtPlatform_SendEmail(const char* user_id, const char* app_ver)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "SendEmail", "(Ljava/lang/String;Ljava/lang/String;)V");
    
    jstring jstrUser_id = (*env)->NewStringUTF(env, user_id);
    jstring jstrApp_ver = (*env)->NewStringUTF(env, app_ver);
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstrUser_id, jstrApp_ver);
    
    (*env)->DeleteLocalRef(env, jstrUser_id);
    (*env)->DeleteLocalRef(env, jstrApp_ver);
    FREE_CLASS
}




void YtPlatform_GoWeb(const char* strUrl)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "GoWeb", "(Ljava/lang/String;)V");
    
    jstring jstrUrl = (*env)->NewStringUTF(env, strUrl);
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstrUrl);
    
    (*env)->DeleteLocalRef(env, jstrUrl);
    
    FREE_CLASS
}


void YtPlatform_GoWebView(const char* strUrl)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "GoWebView", "(Ljava/lang/String;)V");
    
    jstring jstrUrl = (*env)->NewStringUTF(env, strUrl);
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstrUrl);
    
    (*env)->DeleteLocalRef(env, jstrUrl);
    FREE_CLASS
}


YtPlatform_CallFunc g_callFuncIAB;


void Java_com_yongtrim_lib_YtNative_IABRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
//    if(callFuncIAB == NULL)
//    {
//        g_platform_notProcessDelly += code;
//    }
//    else
//    {
        if(value == 0)
            return;
        g_callFuncIAB(value, code);
//    }
}



void YtPlatform_IAB(YtPlatform_CallFunc callFunc, const char* product_id, const char* payload)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "IAB", "(Ljava/lang/String;Ljava/lang/String;)V");
    
    g_callFuncIAB = callFunc;
    
    jstring jstrUrl = (*env)->NewStringUTF(env, product_id);
    jstring jstrPayload = (*env)->NewStringUTF(env, payload);
    
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstrUrl, jstrPayload);
    
    (*env)->DeleteLocalRef(env, jstrUrl);
    (*env)->DeleteLocalRef(env, jstrPayload);
    FREE_CLASS
}







YtPlatform_CallFunc callFuncPushOnOff;

void Java_com_yongtrim_lib_YtNative_PushOnOffRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    callFuncPushOnOff(value, code);
}


void YtPlatform_PushOnOff(YtPlatform_CallFunc callFunc, int onoff)
{
	JNIEnv *env;
    
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "PushOnOff", "(I)V");
    callFuncPushOnOff = callFunc;
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, onoff);
    FREE_CLASS
}


jmethodID g_jShowAd;

void YtPlatform_ShowAd(int show)
{
	JNIEnv *env;
    
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	if(!g_jShowAd)
		g_jShowAd = (*env)->GetStaticMethodID(env, jNativesCls, "ShowAd", "(I)V");
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, g_jShowAd, show);
    FREE_CLASS
}



jmethodID g_jCheckPush;


void YtPlatform_CheckPush()
{
    JNIEnv *env;
    
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	if(!g_jCheckPush)
		g_jCheckPush = (*env)->GetStaticMethodID(env, jNativesCls, "CheckPush", "(I)V");
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, g_jCheckPush, 0);
    FREE_CLASS

}


jmethodID g_jRunBackground;

void YtPlatform_RunBackground(int value)
{
    JNIEnv *env;
    
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	if(!g_jRunBackground)
		g_jRunBackground = (*env)->GetStaticMethodID(env, jNativesCls, "RunBackground", "(I)V");
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, g_jRunBackground, value);
    FREE_CLASS
}


void YtPlatform_CreateShort()
{
    
    JNIEnv *env;
    
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID createShort = (*env)->GetStaticMethodID(env, jNativesCls, "CreateShort", "(I)V");
    
    (*env)->CallStaticVoidMethod(env, jNativesCls, createShort, 0);
    FREE_CLASS
}



YtPlatform_CallFuncO callFuncShowNicknameView;

void Java_com_yongtrim_lib_YtNative_ShowNicknameViewRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
	size_t len = (*env)->GetArrayLength(env, bytearray);
	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0);
    
	char* buffer = malloc(len+1);
	memcpy(buffer, nativeBytes, len);
	buffer[len] = 0;
    
	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT);
    
    callFuncShowNicknameView(buffer, code);
    free(buffer);
}


void YtPlatform_ShowNicknameView(YtPlatform_CallFuncO callFunc, const char* dafaultName, int retry)
{
    JNIEnv *env;
	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
    GET_CLASS
    
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "ShowNicknameView", "(Ljava/lang/String;I)V");
    
    callFuncShowNicknameView = callFunc;
    
    jstring jstr = (*env)->NewStringUTF(env, dafaultName);
    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstr, retry);
    (*env)->DeleteLocalRef(env,jstr);
    FREE_CLASS
}


//
//
//jmethodID g_mSendStr;
//void YYPlatform_PrintMessage(char* message)
//{
//	JNIEnv *env;
//
//	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
//
//	if(!g_mSendStr)
//		g_mSendStr = (*env)->GetStaticMethodID(env, jNativesCls, "OnMessage", "(Ljava/lang/String;)V");
//	
//    jstring jstr = (*env)->NewStringUTF(env, message);
//    (*env)->CallStaticVoidMethod(env, jNativesCls, g_mSendStr, jstr);
//    (*env)->DeleteLocalRef(env,jstr);
//}
//
//
//void YYPlatform_Printf(char* format, ...)
//{
//	va_list argptr;
//	static char string[1024];
//
//	va_start(argptr, format);
//	vsprintf(string, format,argptr);
//	va_end(argptr);
//
//	YYPlatform_PrintMessage(string);
//}
//
//
//PLATBOOL YYPlatform_Network_Connect(YYNetwork_ConnectCompleteCallFunc cbConnectComplete)
//{
//	JNIEnv *env;
//
//	g_network_plat.m_cbConnectComplete = cbConnectComplete;
//	
//	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
//
//	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls,
//		"SocketConnect",
//		"(Ljava/lang/String;I)V");
//
//	if(mid)
//	{
//		jstring jstr = (*env)->NewStringUTF(env, g_network_plat.m_strUrl);
//		(*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstr, g_network_plat.m_port);
//		(*env)->DeleteLocalRef(env,jstr);
//	}
//	return TRUE;
//}
//
//
//void Java_com_yongyong_yylib_YYNative_SocketConnectResult(JNIEnv* env, jobject thiz, jint success)
//{
//	if(g_network_plat.m_cbConnectComplete)
//		g_network_plat.m_cbConnectComplete(success);
//}
//
//
//void Java_com_yongyong_yylib_YYNative_SocketSendDataResult(JNIEnv* env, jobject thiz, jint success)
//{
//	if(g_network_plat.m_cbWriteComplete)
//		g_network_plat.m_cbWriteComplete(success);
//}
//
//
//PLATBOOL YYPlatform_Network_SendData(char *pData, int nSize, YYNetwork_WriteCompleteCallFunc cbWriteComplete)
//{
//	JNIEnv *env;
//
//	g_network_plat.m_cbWriteComplete = cbWriteComplete;
//
//	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
//
//	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "SocketWrite", "([BI)V");
//
//	jbyteArray jArray = (*env)->NewByteArray(env, nSize);
//	(*env)->SetByteArrayRegion(env, jArray, 0, nSize, (jbyte *)pData);
//
//	if(mid)
//	{
//	    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jArray, nSize);
//	}
//
//	(*env)->DeleteLocalRef(env,jArray);
//
//	return TRUE;
//}
//
//
//PLATBOOL YYPlatform_Network_ReadyRead(YYNetwork_ReadCompleteCallFunc cbReadComplete)
//{
//	g_network_plat.m_cbReadComplete = cbReadComplete;
//	return TRUE;
//}
//
//
//void Java_com_yongyong_yylib_YYNative_SocketSetReadData(JNIEnv *env, jobject thiz, jbyteArray bytearray)
//{
//	size_t len = (*env)->GetArrayLength(env, bytearray); 
//	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0); 
//	
//	memset(g_network_plat.m_buffer, 0, YYPLATFORM_NETWORK_BUF_SIZE);
//	if(nativeBytes)
//		memcpy(g_network_plat.m_buffer, nativeBytes, len);
//
//	YYNetwork_ReceiveMessage(YYNetwork_msg_success);
//	//if(g_network_plat.m_cbReadComplete)
//	//	g_network_plat.m_cbReadComplete(g_network_plat.m_buffer, len, YYNetwork_msg_success);
//
//	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT); 
//}
//
//
//PLATBOOL YYPlatform_Network_DisConnect()
//{
//	JNIEnv *env;
//
//	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
//
//	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "SocketClose", "(I)V");
//
//	if(mid)
//	{
//	    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
//	}
//
//	g_network_plat.m_cbConnectComplete = NULL;
//	g_network_plat.m_cbReadComplete = NULL;
//	g_network_plat.m_cbWriteComplete = NULL;
//
//	return TRUE;
//}
//
//
//void Java_com_yongyong_yylib_YYNative_InAppPurchaseResult(JNIEnv* env, jobject thiz, jint success)
//{
//	g_network_plat.m_cb_purchasecomplete(success);
//}
//
//
//PLATBOOL YYPlatform_InAppPurchase(char* pid, int price, YYNetwork_PurchaseCompleteCallFunc cb_purchasecomplete)
//{
//	JNIEnv *env;
//
//	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
//
//	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "InAppPurchase", "(Ljava/lang/String;I)V");
//
//	if(mid)
//	{
//		jstring jstrText = (*env)->NewStringUTF(env, pid);
//	    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, jstrText, price);
//		(*env)->DeleteLocalRef(env, jstrText);
//	}
//
//	g_network_plat.m_cb_purchasecomplete = cb_purchasecomplete;
//
//	//cb_purchasecomplete(YYNetwork_purchasemsg_success);
//	//cb_purchasecomplete(PLATLIB_PURCHASEMSG_FAILURE);
//
//	return TRUE;
//}
//
//
//char g_phonenumber[16];
//void Java_com_yongyong_yylib_YYNative_LibGetPhoneNumberSetReternValue(JNIEnv * env, jclass cls, jbyteArray bytearray)
//{
//	size_t len = (*env)->GetArrayLength(env, bytearray); 
//	jbyte *nativeBytes = (*env)->GetByteArrayElements(env, bytearray, 0); 
//
//	memcpy(g_phonenumber, nativeBytes, len);
//	g_phonenumber[len] = 0;
//	
//	(*env)->ReleaseByteArrayElements(env, bytearray, nativeBytes, JNI_ABORT); 
//}
//
//
//PLATBOOL YYPlatform_GetPhoneNumber(char *strPhoneNumber)
//{
//	JNIEnv *env;
//
//	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
//
//	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "LibGetPhoneNumber", "(I)V");
//    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
//	
//	strcpy(strPhoneNumber, g_phonenumber);
//
//	return TRUE;
//}
//
//
//jmethodID g_jVibrate;
//
//void YYPlatform_Vibrate(int millisecond)
//{
//	JNIEnv *env;
//
//	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
//
//	if(!g_jVibrate)
//		g_jVibrate = (*env)->GetStaticMethodID(env, jNativesCls, "Vibration", "(I)V");
//
//    (*env)->CallStaticVoidMethod(env, jNativesCls, g_jVibrate, millisecond);
//}
//
////
//void Java_com_yongyong_yylib_YYNative_LibCheckServerEnd(JNIEnv * env, jclass cls, jint argInt)
//{
//	extern void SRTitleLayer_CheckLoading(int hasKakao);
//	SRTitleLayer_CheckLoading(argInt);
//}
//
//
//void YYPlatform_CheckServer(int param)
//{
//	JNIEnv *env;
//	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
//
//	jmethodID g_jCheckServer = (*env)->GetStaticMethodID(env, jNativesCls, "CheckServer", "(I)V");
//
//    (*env)->CallStaticVoidMethod(env, jNativesCls, g_jCheckServer, param);
//}
//
//
//void YYPlatform_SetTextHeight(int param)
//{
//	JNIEnv *env;
//	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
//
//	jmethodID g_jSetTextHeight = (*env)->GetStaticMethodID(env, jNativesCls, "SetTextHeight", "(I)V");
//
//    (*env)->CallStaticVoidMethod(env, jNativesCls, g_jSetTextHeight, param);
//}
//
//
//void YYPlatform_GoProductView(int param)
//{
//	JNIEnv *env;
//	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
//
//	jmethodID g_jGoProductView = (*env)->GetStaticMethodID(env, jNativesCls, "GoProductView", "(I)V");
//
//    (*env)->CallStaticVoidMethod(env, jNativesCls, g_jGoProductView, param);
//}
//
//
//void YYPlatform_GoKakao(int param)
//{
//	JNIEnv *env;
//	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
//
//	jmethodID g_jGoKakao = (*env)->GetStaticMethodID(env, jNativesCls, "GoKakao", "(I)V");
//
//    (*env)->CallStaticVoidMethod(env, jNativesCls, g_jGoKakao, param);
//}
//
//void Java_com_yongyong_yylib_YYNative_DownloadProgress(JNIEnv* env, jobject thiz, jint arg)
//{
//	YYLib_DownloadProgress(arg);
//}
//
//
//void Java_com_yongyong_yylib_YYNative_DownloadResult(JNIEnv* env, jobject thiz, jint arg)
//{
//	//0 failure, 1 success
//	YYLib_DownloadResult(arg);
//}
//
//
//PLATBOOL YYPlatform_Download_start()
//{
//	JNIEnv *env;
//
//	(*g_VM)->AttachCurrentThread(g_VM, (void *)&env, NULL);
//
//	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls, "DownloadStart", "(I)V");
//    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, 0);
//	
//	return TRUE;
//}
#endif
#endif
