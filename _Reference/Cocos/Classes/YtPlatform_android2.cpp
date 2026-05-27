/*
YtPlatform_android.cpp
YongTrim Lib
@initialize at 121220

@history
121220	yoonsr	initialize
*/
#if 0

#include "YtPlatform.h"

#ifdef ANDROID

#include "platform/android/jni/JniHelper.h"


#define CLASS_NAME_JNI "com/yongtrim/lib/YtNative"

extern "C"
{

char g_app_version[80];
char g_app_param[80];

char g_android_buffer[512];
char g_android_buffer2[512];

char* g_android_contactList;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_initialize(JNIEnv * env, jclass cls, jobjectArray jargv)
{
    //    (*env)->GetJavaVM(env, &g_VM);
    //
    //    jNativesCls = (*env)->FindClass(env, "com/yongtrim/lib/YtNative");
    
    // Extract char ** args from Java array
    jsize clen = env->GetArrayLength(jargv);
    
    char* args[(int)clen];
    
    int i;
    jstring jrow;
    
    for(i = 0;i < clen; i++)
    {
        jrow = (jstring)env->GetObjectArrayElement(jargv, i);
        const char *row = env->GetStringUTFChars(jrow, 0);
        args[i] = (char*)malloc(strlen(row) + 1);
        strcpy(args[i], row);
        env->ReleaseStringUTFChars(jrow, row);
    }
    
    strcpy(g_app_version, args[0]);
    strcpy(g_app_param, args[1]);
}

void YtPlatform_InitInApp()
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "initializeInAppBilling", "(Ljava/lang/String;)V")) {
        
        jstring jstrKey = t.env->NewStringUTF("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAioNBGp+0Bur2eVkc51+r0LrIrbIsmDp/05BeDRM6U118wEvAawVHKXNt4V2jx8QhIknbk3YJaGDyHI3o89Yp4F0aCIDV+qxqBAk1mO0RpPysCinXZ1cNrgXLXzwKlzxFe+LKiJ9nXXLhuLp34U0/wuxuqJrDUq16+gljjLP3WXj9AmdsnL4uMPZXqRGpURgHxIxd2L2nVGpK2uQyi9nX0kMD+4hIwPfpTBcxlJsVZoPWdNp6k5uo45p62pXwIrV6ssLzm2fyLkcDRjSCx9AVDXEHS9f7Bh2Zvc47u3+986VfplwrlrseIGrVsXfW3EcEJGaE/OEv8CSKqzdgZUWP0wIDAQAB");
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstrKey);
        t.env->DeleteLocalRef(t.classID);
        
        t.env->DeleteLocalRef(jstrKey);
        
    }
}




JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_verifyPrice(JNIEnv * env, jclass cls, jbyteArray bytearrayData, jbyteArray bytearraySignature)
{
    size_t lenData = env->GetArrayLength(bytearrayData);
    jbyte *nativeBytesData = env->GetByteArrayElements(bytearrayData, 0);
    
    memcpy(g_android_buffer, nativeBytesData, lenData);
    g_android_buffer[lenData] = 0;
    
    env->ReleaseByteArrayElements(bytearrayData, nativeBytesData, JNI_ABORT);
    
    
    size_t lenSignature = env->GetArrayLength(bytearraySignature);
    jbyte *nativeBytesSignature = env->GetByteArrayElements(bytearraySignature, 0);
    
    memcpy(g_android_buffer2, nativeBytesSignature, lenSignature);
    g_android_buffer2[lenSignature] = 0;
    
    env->ReleaseByteArrayElements(bytearraySignature, nativeBytesSignature, JNI_ABORT);
    
    
    DyMoneyManager_VerifyPrice(g_android_buffer, g_android_buffer2);
}


void YtPlatform_contumeProdcut()
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "contumeProdcut", "(I)V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
    
    }
}



JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_updateBatteryStatus(JNIEnv* env, jobject thiz, jint level)
{
    YtLib_updateBatteryStatus(level);
}

int YtPlatform_GetBatteryLevel()
{
    return 100;
}


JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_setUDID(JNIEnv * env, jclass cls, jbyteArray bytearray)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    memcpy(g_android_buffer, nativeBytes, len);
    g_android_buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_setPhonenumber(JNIEnv * env, jclass cls, jbyteArray bytearray)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    memcpy(g_android_buffer, nativeBytes, len);
    g_android_buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
}


JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_setContactList(JNIEnv * env, jclass cls, jbyteArray bytearray)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    if(g_android_contactList)
        free(g_android_contactList);
    g_android_contactList = (char*)malloc(len+1);
    memcpy(g_android_contactList, nativeBytes, len);
    g_android_contactList[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
}


YtPlatform_CallFunc callFuncIsLogin;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoIsLoginRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    //YtLib_updateBatteryStatus(level);
    callFuncIsLogin(value, code);
}


void YtPlatform_NetKakao_IsLogin(YtPlatform_CallFunc callFunc)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoIsLogin", "(I)V")) {
        callFuncIsLogin = callFunc;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
    }
}


YtPlatform_CallFunc callFuncLogout;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoLogoutRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    //YtLib_updateBatteryStatus(level);
    callFuncLogout(value, code);
}

void YtPlatform_NetKakao_Logout(YtPlatform_CallFunc callFunc)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoLogout", "(I)V")) {
        callFuncLogout = callFunc;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
    }
}




YtPlatform_CallFunc callFuncUnregist;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoUnregistRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    //YtLib_updateBatteryStatus(level);
    callFuncUnregist(value, code);
}



void YtPlatform_NetKakao_Unregist(YtPlatform_CallFunc callFunc)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoUnregist", "(I)V")) {
        callFuncUnregist = callFunc;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
    }
}



YtPlatform_CallFunc callFuncDeleteme;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoDeletemeRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    //YtLib_updateBatteryStatus(level);
    callFuncDeleteme(value, code);
}



void YtPlatform_NetKakao_DeleteMe(YtPlatform_CallFunc callFunc)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoDeleteme", "(I)V")) {
        callFuncDeleteme = callFunc;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
    }
}



YtPlatform_CallFunc callFuncLogin;

YtPlatform_CallFunc callFuncLoginStarted;
JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoLoginRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    if(callFuncLogin)
        callFuncLogin(value, code);
    callFuncLogin = NULL;
}

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoLoginStartedRet(JNIEnv* env, jobject thiz, jint value, jint code)
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
    
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoLogin", "(I)V")) {
        callFuncLogin = callFunc;
        callFuncLoginStarted = callFuncStart;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
    }
}


YtPlatform_CallFuncO callFuncGetUserInfo;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoGetUserInfoRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    char* buffer = (char*)malloc(len+1);
    memcpy(buffer, nativeBytes, len);
    buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
    
    callFuncGetUserInfo(buffer, code);
    free(buffer);
}



void YtPlatform_NetKakao_GetUserInfo(YtPlatform_CallFuncO callFunc)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoGetUserInfo", "(I)V")) {
        callFuncGetUserInfo = callFunc;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
    }
    
}




YtPlatform_CallFuncO callFuncGetFriends;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoGetFriendsRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    char* buffer = (char*)malloc(len+1);
    memcpy(buffer, nativeBytes, len);
    buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
    
    callFuncGetFriends(buffer, code);
    free(buffer);
}

void YtPlatform_NetKakao_GetFriends(YtPlatform_CallFuncO callFunc)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoGetFriends", "(I)V")) {
        callFuncGetFriends = callFunc;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
    }
}




YtPlatform_CallFuncO callFuncSendMessage;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoSendMessageRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    char* buffer = (char*)malloc(len+1);
    memcpy(buffer, nativeBytes, len);
    buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
    
    callFuncSendMessage(buffer, code);
    free(buffer);
}


void YtPlatform_NetKakao_SendMessage(YtPlatform_CallFuncO callFunc, const char* receiverID, const char* message, const char *executeURLString)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoSendMessage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) {
        callFuncSendMessage = callFunc;
        
        jstring jstrReceiverID = t.env->NewStringUTF(receiverID);
        jstring jstrMessage = t.env->NewStringUTF(message);
        jstring jstrExecuteURLString = t.env->NewStringUTF(executeURLString);
   
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstrReceiverID, jstrMessage, jstrExecuteURLString);
        t.env->DeleteLocalRef(t.classID);
        
        t.env->DeleteLocalRef(jstrReceiverID);
        t.env->DeleteLocalRef(jstrMessage);
        t.env->DeleteLocalRef(jstrExecuteURLString);
        
    }
}



YtPlatform_CallFunc callFuncSendPride;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoSendPrideRet(JNIEnv * env, jclass cls, jint value, jint code)
{
    callFuncSendPride(value, code);
}


void YtPlatform_NetKakao_SendPride(YtPlatform_CallFunc callFunc, const char* receiverID, const char* sender_nick, const char* score, const char* shop_name, const char* stage, const char *executeURLString)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoSendPride", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) {
        callFuncSendPride = callFunc;
        
        jstring jstrReceiverID = t.env->NewStringUTF(receiverID);
        jstring jstrSender_nick = t.env->NewStringUTF(sender_nick);
        jstring jstrScore = t.env->NewStringUTF(score);
        jstring jstrShop_name = t.env->NewStringUTF(shop_name);
        jstring jstrStage = t.env->NewStringUTF(stage);
        jstring jstrExecuteURLString = t.env->NewStringUTF(executeURLString);
        
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID,  jstrReceiverID, jstrSender_nick, jstrScore, jstrShop_name, jstrStage, jstrExecuteURLString);
        t.env->DeleteLocalRef(t.classID);
        
        t.env->DeleteLocalRef(jstrReceiverID);
        t.env->DeleteLocalRef(jstrSender_nick);
        t.env->DeleteLocalRef(jstrScore);
        t.env->DeleteLocalRef(jstrStage);
        t.env->DeleteLocalRef(jstrSender_nick);
        t.env->DeleteLocalRef(jstrExecuteURLString);
        
    }
}



void YtPlatform_ShowIndicator(int enable)
{
    
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "ShowIndicator", "(I)V")) {
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, enable);
        t.env->DeleteLocalRef(t.classID);
        
    }
}



YtPlatform_CallFuncO callFuncLoadGameInfo;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoLoadGameInfoRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    char* buffer = (char*)malloc(len+1);
    memcpy(buffer, nativeBytes, len);
    buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
    
    callFuncLoadGameInfo(buffer, code);
    free(buffer);
}


void YtPlatform_NetKakao_LoadGameInfo(YtPlatform_CallFuncO callFunc)
{
    
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoLoadGameInfo", "(I)V")) {
        callFuncLoadGameInfo = callFunc;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
    }
}


YtPlatform_CallFuncO callFuncLoadGameMe;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoLoadGameMeRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    char* buffer = (char*)malloc(len+1);
    memcpy(buffer, nativeBytes, len);
    buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
    
    callFuncLoadGameMe(buffer, code);
    free(buffer);
}


void YtPlatform_NetKakao_LoadGameMe(YtPlatform_CallFuncO callFunc)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoLoadGameMe", "(I)V")) {
        callFuncLoadGameMe = callFunc;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
    }
}


YtPlatform_CallFuncO callFuncLoadGameFriends;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoLoadGameFriendsRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    char* buffer = (char*)malloc(len+1);
    memcpy(buffer, nativeBytes, len);
    buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
    
    callFuncLoadGameFriends(buffer, code);
    free(buffer);
}


void YtPlatform_NetKakao_LoadGameFriends(YtPlatform_CallFuncO callFunc)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoLoadGameFriends", "(I)V")) {
        callFuncLoadGameFriends = callFunc;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
    }
}



YtPlatform_CallFuncO callFuncLoadLeaderBoard;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoLoadLeaderBoardRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    char* buffer = (char*)malloc(len+1);
    memcpy(buffer, nativeBytes, len);
    buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
    
    callFuncLoadLeaderBoard(buffer, code);
    free(buffer);
}



void YtPlatform_NetKakao_LoadLeaderBoard(YtPlatform_CallFuncO callFunc, int type)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoLoadLeaderBoard", "(I)V")) {
        callFuncLoadLeaderBoard = callFunc;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, type);
        t.env->DeleteLocalRef(t.classID);
        
    }
}


YtPlatform_CallFunc callFuncSendData;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoSendDataRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    //YtLib_updateBatteryStatus(level);
    callFuncSendData(value, code);
}



void YtPlatform_NetKakao_SendData(YtPlatform_CallFunc callFunc, NetKakaSendDataType type, const char* data)
{
    
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoSendData", "(Ljava/lang/String;I)V")) {
        callFuncSendData = callFunc;
        
        jstring jstrData = t.env->NewStringUTF(data);
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstrData, type);
        t.env->DeleteLocalRef(t.classID);
        
        t.env->DeleteLocalRef(jstrData);
        
    }
    
}



YtPlatform_CallFuncO callFuncSendGameMessage;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoSendGameMessageRet(JNIEnv* env, jobject thiz, jbyteArray bytearray, jint code)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    char* buffer = (char*)malloc(len+1);
    memcpy(buffer, nativeBytes, len);
    buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
    
    callFuncSendGameMessage(buffer, code);
    free(buffer);
}


void YtPlatform_NetKakao_SendGameMessage(YtPlatform_CallFuncO callFunc, const char* data)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoSendGameMessage", "(Ljava/lang/String;)V")) {
        callFuncSendGameMessage = callFunc;
        
        jstring jstrData = t.env->NewStringUTF(data);
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstrData);
        t.env->DeleteLocalRef(t.classID);
        
        t.env->DeleteLocalRef(jstrData);
        
    }

}


YtPlatform_CallFunc callFuncAcceptMessage;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoAcceptMessageRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    callFuncAcceptMessage(value, code);
}



void YtPlatform_NetKakao_AcceptMessage(YtPlatform_CallFunc callFunc, const char* messageid)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoAcceptMessage", "(Ljava/lang/String;)V")) {
        callFuncAcceptMessage = callFunc;
        
        jstring jstrData = t.env->NewStringUTF(messageid);
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstrData);
        t.env->DeleteLocalRef(t.classID);
        
        t.env->DeleteLocalRef(jstrData);
        
    }
}



YtPlatform_CallFunc callFuncAcceptAllMessages;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoAcceptAllMessagesRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    callFuncAcceptAllMessages(value, code);
}


void YtPlatform_NetKakao_AcceptAllMessages(YtPlatform_CallFunc callFunc)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoAcceptAllMessages", "(I)V")) {
        callFuncAcceptAllMessages = callFunc;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
    }
}





YtPlatform_CallFuncO callFuncLoadGameMessages;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoLoadGameMessagesRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    char* buffer = (char*)malloc(len+1);
    memcpy(buffer, nativeBytes, len);
    buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
    
    callFuncLoadGameMessages(buffer, code);
    free(buffer);
}



void YtPlatform_NetKakao_LoadGameMessages(YtPlatform_CallFuncO callFunc)
{
    
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoLoadGameMessages", "(I)V")) {
        callFuncLoadGameMessages = callFunc;
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
    }
}





YtPlatform_CallFuncO callFuncInvite;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoInviteRet(JNIEnv* env, jobject thiz, jbyteArray bytearray, jint code)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    char* buffer = (char*)malloc(len+1);
    memcpy(buffer, nativeBytes, len);
    buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
    
    callFuncInvite(buffer, code);
    free(buffer);
}





void YtPlatform_NetKakao_Invite(YtPlatform_CallFuncO callFunc, const char* user_id, const char* message)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoInvite", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        callFuncInvite = callFunc;
        
        jstring jstrUserID = t.env->NewStringUTF(user_id);
        jstring jstrMessage = t.env->NewStringUTF(message);
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstrUserID, jstrMessage);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(jstrUserID);
        t.env->DeleteLocalRef(jstrMessage);
        
    }

}



YtPlatform_CallFunc callFuncMessageBlock;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_NetKakaoMessageBlockRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    callFuncMessageBlock(value, code);
}


void YtPlatform_NetKakao_MessageBlock(YtPlatform_CallFunc callFunc, int messageBlocked)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "NetKakaoMessageBlock", "(I)V")) {
        callFuncMessageBlock = callFunc;
        
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, messageBlocked);
        t.env->DeleteLocalRef(t.classID);

        
    }

}




void YtPlatform_CopyToClipBoard(const char* user_id)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "CopyToClipBoard", "(Ljava/lang/String;)V")) {
        
        jstring jstr = t.env->NewStringUTF(user_id);
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstr);
        t.env->DeleteLocalRef(t.classID);
        
        
        t.env->DeleteLocalRef(jstr);

    }
    
    
    
    //http://stackoverflow.com/questions/1479468/copy-text-to-clipboard-with-iphone-sdk
    //http://stackoverflow.com/questions/238284/how-to-copy-text-programatically-in-my-android-app
}


void YtPlatform_SendEmail(const char* user_id, const char* app_ver)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "SendEmail", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring jstrUser_id = t.env->NewStringUTF(user_id);
        jstring jstrApp_ver = t.env->NewStringUTF(app_ver);
        
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstrUser_id, jstrApp_ver);
        t.env->DeleteLocalRef(t.classID);
        
        t.env->DeleteLocalRef(jstrUser_id);
        t.env->DeleteLocalRef(jstrApp_ver);
        
        
    }
    
}




void YtPlatform_GoWeb(const char* strUrl)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "GoWeb", "(Ljava/lang/String;)V")) {
        jstring jstrUrl = t.env->NewStringUTF(strUrl);
        
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstrUrl);
        t.env->DeleteLocalRef(t.classID);
        
        t.env->DeleteLocalRef(jstrUrl);
        
        
    }
}


void YtPlatform_GoWebView(const char* strUrl)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "GoWebView", "(Ljava/lang/String;)V")) {
        jstring jstrUrl = t.env->NewStringUTF(strUrl);
        
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstrUrl);
        t.env->DeleteLocalRef(t.classID);
        
        t.env->DeleteLocalRef(jstrUrl);
        
        
    }
}


YtPlatform_CallFunc g_callFuncIAB;


JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_IABRet(JNIEnv* env, jobject thiz, jint value, jint code)
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
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "IAB", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        g_callFuncIAB = callFunc;
        
        jstring jstrUrl = t.env->NewStringUTF(product_id);
        jstring jstrPayload = t.env->NewStringUTF(payload);
        
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstrUrl, jstrPayload);
        t.env->DeleteLocalRef(t.classID);
        
        t.env->DeleteLocalRef(jstrUrl);
        t.env->DeleteLocalRef(jstrPayload);
        
        
    }
}







YtPlatform_CallFunc callFuncPushOnOff;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_PushOnOffRet(JNIEnv* env, jobject thiz, jint value, jint code)
{
    callFuncPushOnOff(value, code);
}


void YtPlatform_PushOnOff(YtPlatform_CallFunc callFunc, int onoff)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "PushOnOff", "(I)V")) {
        callFuncPushOnOff = callFunc;
        
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, onoff);
        t.env->DeleteLocalRef(t.classID);
        
        
        
    }
}



void YtPlatform_ShowAd(int show)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "ShowAd", "(I)V")) {
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, show);
        t.env->DeleteLocalRef(t.classID);
        
        
        
    }
    
}




void YtPlatform_CheckPush()
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "CheckPush", "(I)V")) {
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
        
        
    }
    
    
}



void YtPlatform_RunBackground(int value)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "RunBackground", "(I)V")) {
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, value);
        t.env->DeleteLocalRef(t.classID);
        
        
        
    }
}


void YtPlatform_CreateShort()
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "CreateShort", "(I)V")) {
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, 0);
        t.env->DeleteLocalRef(t.classID);
        
        
        
    }
    
}



YtPlatform_CallFuncO callFuncShowNicknameView;

JNIEXPORT void JNICALL Java_com_yongtrim_lib_YtNative_ShowNicknameViewRet(JNIEnv * env, jclass cls, jbyteArray bytearray, jint code)
{
    size_t len = env->GetArrayLength(bytearray);
    jbyte *nativeBytes = env->GetByteArrayElements(bytearray, 0);
    
    char* buffer = (char*)malloc(len+1);
    memcpy(buffer, nativeBytes, len);
    buffer[len] = 0;
    
    env->ReleaseByteArrayElements(bytearray, nativeBytes, JNI_ABORT);
    
    callFuncShowNicknameView(buffer, code);
    free(buffer);
}


void YtPlatform_ShowNicknameView(YtPlatform_CallFuncO callFunc, const char* dafaultName, int retry)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME_JNI, "ShowNicknameView", "(Ljava/lang/String;I)V")) {
        callFuncShowNicknameView = callFunc;
        
        jstring jstr = t.env->NewStringUTF(dafaultName);
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstr, retry);
        t.env->DeleteLocalRef(t.classID);
        
        t.env->DeleteLocalRef(jstr);
        
        
        
    }
    
}
}


#endif
#endif

