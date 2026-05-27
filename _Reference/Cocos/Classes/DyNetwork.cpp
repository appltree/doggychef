/*
DyNetwork.cpp
Doggy chef
@initialize at 131010

@history
131010	yoonsr	initialize
*/

#include "DyNetwork.h"
#include "DyPopup.h"
#include "YtPlatform.h"
#include "YtNetwork.h"
#include "json.h"
#include "DyStage.h"
#include <algorithm>


DyNetworkManager* g_sharedDyNetworkMananger;


DyNetworkCommand::DyNetworkCommand(DyNetworkCmdType type, std::string param, void* param_ptr, DyNetwork_CallFunc callFunc)
{
    m_status = DyNetworkCmdStatus_Ready;
	m_commandtype = type;
    m_param = param;
    m_param_ptr = param_ptr;
    m_callFunc = callFunc;
    
    m_retInt = 0;
    m_retString = "";
    
    if(m_commandtype == DyNetworkCmdType_SendLinkMessage)
    {
        CCArray* arrParams = (CCArray*)m_param_ptr;
        arrParams->retain();
    }
}


DyNetworkCommand::~DyNetworkCommand()
{
    if(m_commandtype == DyNetworkCmdType_SendLinkMessage)
    {
        CCArray* arrParams = (CCArray*)m_param_ptr;
        arrParams->release();
    }

}


void DyNetworkCommand_value_str(void* value, int code)
{
    DyNetworkCommand* command = (DyNetworkCommand*)g_sharedDyNetworkMananger->getCommand(0);
    
    switch(command->getCmdType())
    {
        case DyNetworkCmdType_LoadGameInfo:
            command->m_callFunc(DyNetworkCmdType_LoadGameInfo, code, value);
            break;
        case DyNetworkCmdType_LoadMe:
            command->m_callFunc(DyNetworkCmdType_LoadMe, code, value);
            break;
        default:
            break;
    }
    command->setStatus(DyNetworkCmdStatus_End);
}


//void DyNetworkCommand_value_int(int value)
//{
//    DyNetworkCommand* command = (DyNetworkCommand*)g_sharedDyNetworkMananger->m_poolCommands->objectAtIndex(0);
//    
//    switch(command->m_commandtype)
//    {
//        case DyNetworkCmdType_UpdateMe:
//            break;
//        default:
//            break;
//    }
//    
//    command->m_status = DyNetworkCmdStatus_End;
//    
//}


DyNetworkCmdStatus DyNetworkCommand::getStatus()
{
    return m_status;
}


void DyNetworkCommand::setStatus(DyNetworkCmdStatus status)
{
    m_status = status;
}


DyNetworkCmdType DyNetworkCommand::getCmdType()
{
    return m_commandtype;
}


void DyNetworkCommand::setCmdType(DyNetworkCmdType type)
{
    m_commandtype = type;
}


std::string DyNetworkCommand::getMessage(std::string message, int code)
{
    switch(code)
    {
        case 0:
            return "성공";//SUCCESS (성공)
        case 8:
            return "Guest Login상태에서는 지원하지 않는 기능입니다.";//NOT_SUPPORTED_IN_GUEST_MODE (Guest Login상태에서는 지원하지 않는 기능입니다. )
        case 6:
            return STRING_TITLE_UPDATE_DATA_FAILURE;
        case 10:
            return "계정 연결에는 성공하였으나 해당 사용자의 카카오계정은 아직 이메일 인증을 받지 않았습니다.";//SUCCESS_NOT_VERIFIED (계정 연결에는 성공하였으나 해당 사용자의 카카오계정은 아직 이메일 인증을 받지 않았습니다. )
        case -9798:
            return "인증서버 또는 API 서버 점검중입니다";//UNDER_MAINTENANCE ( 인증서버 또는 API 서버 점검중입니다 )
        case -1000:
            return "access_token이 만료 혹은 유효하지 않은 상태";//KAServerErrorNotAuthorized access_token이 만료 혹은 유효하지 않은 상태
        case -500:
            return message;//ERROR (에러가 발생한 경우 일반적으로 리턴되는 코드값이며, 이 경우 message를 반드시 확인하시는 것이 좋습니다)
        case -451:
            return "해당 앱의 이용가능 연령에 미달하는 사용자입니다";//LOWER_AGE_LIMIT (해당 앱의 이용가능 연령에 미달하는 사용자입니다)
        case -400:
            return "refresh_token이 만료 혹은 유효하지 않은 상태";//KAServerErrorInvaildGrant refresh_token이 만료 혹은 유효하지 않은 상태
        case -200:
            return "올바르지 않은 푸시 토큰입니다";//INVALID_PUSH_TOKEN (올바르지 않은 푸시 토큰입니다)
        case -100:
            return "권한이 없습니다. 권한이 부여되지 않은 정보를 요청한 경우";//INSUFFICIENT_PERMISSION (권한이 없습니다. 권한이 부여되지 않은 정보를 요청한 경우)
        case -32:
        {
            char temp[128];
            
            DyDataManager* dataManager = DyDataManager::sharedDataManager();
            
            sprintf(temp, "초대 메세지를 보낼 수 없는 사용자로 1일 %d회 이상 시도하셨습니다.", dataManager->m_private.m_cntInviteToday);
           
            return temp;
        }
        case -31:
            return "초대메시지를 보낼 수 없는 사용자입니다. 초대메시지는 같은 수신자에게는 30일에 1번만 보낼 수 있습니다."; //EXCEED_INVITE_CHAT_LIMIT
        case -17:
            return "메시지 수신을 차단한 사용자입니다.";// (앱을 설치하지 않은 친구의 초대 메시지 차단) //INVITE_MESSAGE_BLOCKED
        case -16:
            return "메시지 수신을 차단한 사용자입니다.";// (앱을 설치한 친구의 게임 메시지 차단) //MESSAGE_BLOCK_USER
        case -15:
            return "채팅방 정보를 찾을 수 없습니다.";// (채팅플러스를 통해 특정 채팅방으로의 메시지 전송을 요청한 경우)"
        case -14:
            return "메시지 수신을 차단한 사용자입니다.";//UNSUPPORTED_DEVICE 메시지 수신을 차단한 사용자입니다. (앱을 설치한 친구의 게임 메시지 차단)
        case -13:
            return "등록되지 않은 사용자입니다.";//UNREGISTERD_USER (등록되지 않은 사용자입니다. user_id가 없거나 user_id 값이 access token에 기록되어 있는 아이디값과 일치하지 않습니다. Parameter값을 잘못 전달해주거나 user_id값이 임의로 변경된 경우 등)
        case -12:
            return "해당 request에 필요한 parameter가 누락되었거나 parameter 형식이 지정된 것과 다른 경우 등";//
        case -11:
            return "탈퇴한 사용자입니다.";//DEACTIVATED_USER (탈퇴한 사용자입니다.)
        case -10:
            return "해당 사용자의 카카오 계정 정보를 찾을 수 없습니다";//
    }
    
    return "알 수 없는 에러";
}


void DyNetworkCommand::callfunc(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    lib->show_network_alaram(false);
    
   
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layer->unschedule(schedule_selector(DyNetworkCommand::callfunc));
    
    DyNetworkCommand* command = (DyNetworkCommand*)g_sharedDyNetworkMananger->getCommand(0);
    
    bool isError = false;
    
    switch(command->m_commandtype)
    {
        case DyNetworkCmdType_UpdateMe:
        case DyNetworkCmdType_SendLinkMessage:
        case DyNetworkCmdType_Invite:
            command->m_callFunc(command->m_commandtype, command->m_retInt, NULL);
            if(command->m_retInt > 0)
            {
                isError = true;
            }
            break;
        
        case DyNetworkCmdType_SendMessage:
        case DyNetworkCmdType_SendPresent:
            if(command->m_retString.length() > 0)
            {
                isError = true;
            }
            else
                command->m_callFunc(command->m_commandtype, command->m_retInt, (void*)command->m_retString.c_str());
            break;
        default:
            break;
    }
    
    if(isError)
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
        
        if(command->m_retCode != 0)
        {
            popup->setText(getMessage(command->m_retString, command->m_retCode).c_str());
        }
        else
            popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
        
        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
    
    command->m_status = DyNetworkCmdStatus_End;
}


void DyNetworkCommand_value_int_callback(int value, int code)
{
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getBaseLayer();

    DyNetworkCommand* command = (DyNetworkCommand*)g_sharedDyNetworkMananger->getCommand(0);
    
    command->m_retInt = value;
    command->m_retCode = code;
    
    layer->schedule(schedule_selector(DyNetworkCommand::callfunc), 0.05f);
}


void DyNetworkCommand_value_str_callback(void* value, int code)
{
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    DyNetworkCommand* command = (DyNetworkCommand*)g_sharedDyNetworkMananger->getCommand(0);
    
    command->m_retString = (char*)value;
    command->m_retCode = code;
    
    layer->schedule(schedule_selector(DyNetworkCommand::callfunc), 0.05f);
}


void DyNetworkCommand::process()
{
    DyLib* lib = DyLib::sharedLib();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    switch(m_commandtype)
    {
        case DyNetworkCmdType_LoadGameInfo:
            net_LoadGameInfo();
            break;
        case DyNetworkCmdType_LoadMe:
            net_LoadGameMe();
            break;
        case DyNetworkCmdType_UpdateMe:
            lib->network_kakao_update_me(0, DyNetworkCommand_value_int_callback);
            break;
            
        case DyNetworkCmdType_Invite:
            {
                DyFriend* _friend = (DyFriend*)m_param_ptr;
                //sprintf(lib->m_strTemp, "%s님의 가게는 대박 났대요! 맛있는 음식과 귀요미 동물손님들이 기다리고 있어요. 빨리 오픈하러 가요~", dataManager->m_nickname.c_str());
                YtPlatform_NetKakao_SendPride(DyNetworkCommand_value_int_callback, _friend->m_user_id.c_str(), dataManager->m_nickname.c_str(),  "", "", "", "");
                //lib->network_kakao_invite(DyNetworkCommand_value_str_callback, _friend->m_user_id.c_str(), lib->m_strTemp);
            }
            break;
        case DyNetworkCmdType_SendMessage:
            {
                DyFriend* _friend = (DyFriend*)m_param_ptr;
            
                YtPlatform_NetKakao_SendMessage(DyNetworkCommand_value_str_callback, _friend->m_user_id.c_str(), m_param.c_str(), "");
            }
            break;
            
        case DyNetworkCmdType_SendLinkMessage:
            {
                DyDataManager* dataManager = DyDataManager::sharedDataManager();
            
                CCArray* arrParams = (CCArray*)m_param_ptr;
                DyFriend* friendProcess = (DyFriend*)arrParams->objectAtIndex(0);
                CCString* score = (CCString*)arrParams->objectAtIndex(1);
                CCString* shop_name = (CCString*)arrParams->objectAtIndex(2);
                CCString* stage = (CCString*)arrParams->objectAtIndex(3);
            
                YtPlatform_NetKakao_SendPride(DyNetworkCommand_value_int_callback, friendProcess->m_user_id.c_str(), dataManager->m_nickname.c_str(),  score->getCString(), shop_name->getCString(), stage->getCString(), "");
            
                break;
            }
        case DyNetworkCmdType_SendPresent:
            {
                DyFriend* _friend = (DyFriend*)m_param_ptr;
                
                if(m_param.compare("gum") == 0)
                {
                    lib->network_kakao_send_present(DyNetworkCommand_value_str_callback, _friend, DyGiftTypeGum, 1);
                }
                else
                    lib->network_kakao_send_present(DyNetworkCommand_value_str_callback, _friend, DyGiftTypeGaest, 1);
            }
            break;
        case DyNetworkCmdType_PriceVerify:
            {
                char* _signature = (char*)m_param_ptr;
                
                std::string bufURL = NETWORK_SUB_URL;
                bufURL += NETWORK_SERVICE_PRICE_VERIFY;
                
                std::string bufParam = "version=";
                sprintf(lib->m_strTemp, "%d", CONFIG_VER);
                bufParam += lib->m_strTemp;
                bufParam += "&platform=";
                bufParam += dataManager->m_platform;

                bufParam += "&user_id=";
                bufParam += dataManager->m_user_id;

                bufParam += "&purchase_data=";
                bufParam += m_param;
   
                bufParam += "&signature=";
                bufParam += _signature;
                
                bufParam += "&payload=";
                bufParam += lib->m_payload;
                
                CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), httpresponse_selector(DyNetworkCommand::onHttpRequestCompleted));
                
                if(request)
                {
                    request->setTag(NETWORK_SERVICE_PRICE_VERIFY);
                }
            }
            break;
        case DyNetworkCmdType_ScoreMigration:
            {
                std::string bufURL = NETWORK_SUB_URL;
                bufURL += NETWORK_SERVICE_SCORE_MIGRATION;
                
                std::string bufParam = "version=";
                sprintf(lib->m_strTemp, "%d", CONFIG_VER);
                bufParam += lib->m_strTemp;
                bufParam += "&platform=";
                bufParam += dataManager->m_platform;
                
                bufParam += "&user_id=";
                bufParam += dataManager->m_user_id;
                
                Json::Value scores;
                
                
                for(int shop = 0;shop < dataManager->m_arrShopPoint_forMigration->count();shop++)
                {
                    Json::Value stage_scores;
                    CCArray* arrStage = (CCArray*)dataManager->m_arrShopPoint_forMigration->objectAtIndex(shop);
                    
                    for(int stage = 0;stage < arrStage->count();stage++)
                    {
                        sprintf(lib->m_strTemp, "stage_%d", stage);
                        
                        stage_scores[lib->m_strTemp] = ((CCInteger*)arrStage->objectAtIndex(stage))->getValue();
                    }
                    
                    if(shop == 0)
                    {
                        scores["shop_boonsik"] = stage_scores;
                    }
                    else if(shop == 1)
                    {
                        scores["shop_hamburger"] = stage_scores;
                    }
                    else if(shop == 2)
                    {
                        scores["shop_coffee"] = stage_scores;
                    }
                    else if(shop == 3)
                    {
                        scores["shop_china"] = stage_scores;
                    }
                    
                }
                Json::StyledWriter writer;
                std::string data = writer.write(scores);
                
                std::replace(data.begin(), data.end(), '\n', ' ');
                std::replace(data.begin(), data.end(), '"', '#');
                data.erase(remove_if(data.begin(), data.end(), isspace), data.end());
                
                bufParam += "&scores=";
                bufParam += data;
                
                    //CCLog("bufPararm=%s", bufParam.c_str());
            
                
                
                CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), httpresponse_selector(DyNetworkCommand::onHttpRequestCompleted));
                
                if(request)
                {
                    request->setTag(NETWORK_SERVICE_SCORE_MIGRATION);
                }
            }
            break;
        case DyNetworkCmdType_ScoreGet:
            {
                DyStageManager* stageManager = DyStageManager::sharedStageManager();
                
                std::string bufURL = NETWORK_SUB_URL;
                bufURL += NETWORK_SERVICE_SCORE_GET;
                
                std::string bufParam = "version=";
                sprintf(lib->m_strTemp, "%d", CONFIG_VER);
                bufParam += lib->m_strTemp;
                bufParam += "&platform=";
                bufParam += dataManager->m_platform;
                
                bufParam += "&user_id=";
                bufParam += dataManager->m_user_id;
                
                bufParam += "&boonsik=";
                int count = stageManager->getCountLevel(0);
        
                sprintf(lib->m_strTemp, "%d", count);
                bufParam += lib->m_strTemp;
                
                bufParam += "&hamburger=";
                count = stageManager->getCountLevel(1);
                sprintf(lib->m_strTemp, "%d", count);
                bufParam += lib->m_strTemp;

                bufParam += "&coffee=";
                count = stageManager->getCountLevel(2);
                sprintf(lib->m_strTemp, "%d", count);
                bufParam += lib->m_strTemp;

                bufParam += "&china=";
                count = stageManager->getCountLevel(3);
                sprintf(lib->m_strTemp, "%d", count);
                bufParam += lib->m_strTemp;

#if SHOP_COUNT_ADDED > 1
                bufParam += "&pizza=";
                count = stageManager->getCountLevel(4);
                sprintf(lib->m_strTemp, "%d", count);
                bufParam += lib->m_strTemp;
#endif
       
#if SHOP_COUNT_ADDED > 2
                bufParam += "&korean=";
                count = stageManager->getCountLevel(5);
                sprintf(lib->m_strTemp, "%d", count);
                bufParam += lib->m_strTemp;
#endif

                CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), httpresponse_selector(DyNetworkCommand::onHttpRequestCompleted));
                
                if(request)
                {
                    request->setTag(NETWORK_SERVICE_SCORE_GET);
                }
            }
            break;
        case DyNetworkCmdtype_UserRegist:
        {
            std::string bufURL = NETWORK_SUB_URL;
            bufURL += NETWORK_SERVICE_USER_REGIST;
            
            std::string bufParam = "version=";
            sprintf(lib->m_strTemp, "%d", CONFIG_VER);
            bufParam += lib->m_strTemp;
            bufParam += "&platform=";
            bufParam += dataManager->m_platform;
            
            bufParam += "&user_id=";
            bufParam += dataManager->m_user_id;
            
            CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), httpresponse_selector(DyNetworkCommand::onHttpRequestCompleted));
            
            if(request)
            {
                request->setTag(NETWORK_SERVICE_USER_REGIST);
            }
        }
            break;

        case DyNetworkCmdType_ScoreFriendsGet:
        {
            DyFriendManager* friendManager = DyFriendManager::sharedFriendManager();
            
            Json::Value root;   // will contains the root value after parsing.
            Json::Reader reader;
            
            reader.parse(m_param, root);
            
            int _shop = root["shopType"].asInt();
            int _stageCnt = root["stageCnt"].asInt();
            
            Json::Value friends;
            
            int count = 0;
            for(int i = 0;i < friendManager->m_arrFriends->count();i++)
            {
                DyFriend* _friend = (DyFriend*)friendManager->m_arrFriends->objectAtIndex(i);
                
                if(_friend->m_friendType == DyFriendType_Gamer && !_friend->m_isDummy)
                {
                    friends[count++] = _friend->m_user_id;
                }
            }
            
            Json::StyledWriter writer;
            std::string strFriends = writer.write(friends);
            
            
            std::string bufURL = NETWORK_SUB_URL;
            bufURL += NETWORK_SERVICE_SCORE_FRIENDS_GET;
            
            std::string bufParam = "version=";
            sprintf(lib->m_strTemp, "%d", CONFIG_VER);
            bufParam += lib->m_strTemp;
            bufParam += "&platform=";
            bufParam += dataManager->m_platform;
            
            bufParam += "&user_id=";
            bufParam += dataManager->m_user_id;
            
            bufParam += "&shop=";
            sprintf(lib->m_strTemp, "%d", _shop);
            bufParam += lib->m_strTemp;

            bufParam += "&stage_cnt=";
            sprintf(lib->m_strTemp, "%d", _stageCnt);
            bufParam += lib->m_strTemp;
            
            bufParam += "&friends=";
            bufParam += strFriends;
            
            //CCLog("bufPararm=%s", bufParam.c_str());
            
            CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), httpresponse_selector(DyNetworkCommand::onHttpRequestCompleted));
            
            if(request)
            {
                request->setTag(NETWORK_SERVICE_SCORE_FRIENDS_GET);
            }
        }
            break;
            
        case DyNetworkCmdType_ScoreRegist:
        {
            Json::Value root;   // will contains the root value after parsing.
            Json::Reader reader;
            
            reader.parse(m_param, root);
            
            int _shop = root["shop"].asInt();
            int _stage = root["stage"].asInt();
            int _score = root["score"].asInt();
            
            std::string bufURL = NETWORK_SUB_URL;
            bufURL += NETWORK_SERVICE_SCORE_REGIST;
            
            std::string bufParam = "version=";
            sprintf(lib->m_strTemp, "%d", CONFIG_VER);
            bufParam += lib->m_strTemp;
            bufParam += "&platform=";
            bufParam += dataManager->m_platform;
            
            bufParam += "&user_id=";
            bufParam += dataManager->m_user_id;
            
        
            bufParam += "&shop=";
            sprintf(lib->m_strTemp, "%d", _shop);
            bufParam += lib->m_strTemp;
            
            bufParam += "&stage=";
            sprintf(lib->m_strTemp, "%d", _stage);
            bufParam += lib->m_strTemp;
            
            bufParam += "&score=";
            sprintf(lib->m_strTemp, "%d", _score);
            bufParam += lib->m_strTemp;
            
            
            int max_shop = 0;
            int max_level = 0;
            
            stageManager->getCurrentStage(&dataManager->m_public, &max_shop, &max_level);
            
            bufParam += "&cur_shop=";
            sprintf(lib->m_strTemp, "%d", max_shop);
            bufParam += lib->m_strTemp;
            
            bufParam += "&cur_stage=";
            sprintf(lib->m_strTemp, "%d", max_level);
            bufParam += lib->m_strTemp;
            
            
            bufParam += "&stage_record=";
            sprintf(lib->m_strTemp, "%d", dataManager->m_public.m_stageRecord);
            bufParam += lib->m_strTemp;
            
            
            bufParam += "&crazy_record=";
            sprintf(lib->m_strTemp, "%d", dataManager->m_crazyRecord);
            bufParam += lib->m_strTemp;
            
            bufParam += "&crazy_record_best=";
            sprintf(lib->m_strTemp, "%d", dataManager->m_crazyBestRecord);
            bufParam += lib->m_strTemp;
            
            
            std::string public_data = dataManager->dataSave_public();
            std::string private_data = dataManager->dataSave_private();
            
            bufParam += "&public_data=";
            bufParam += public_data;
            
            
            bufParam += "&private_data=";
            bufParam += private_data;

            
            bufParam += "&gold=";
            sprintf(lib->m_strTemp, "%d", SInt(dataManager->m_private.m_gold).get());
            bufParam += lib->m_strTemp;

            bufParam += "&delly=";
            sprintf(lib->m_strTemp, "%d", SInt(dataManager->m_private.m_delly).get());
            bufParam += lib->m_strTemp;
  
            bufParam += "&gum=";
            sprintf(lib->m_strTemp, "%d", dataManager->m_gum);
            bufParam += lib->m_strTemp;

            
            //CCLog("bufURL = %s, bufParam = %s", bufURL.c_str(), bufParam.c_str());

            CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), httpresponse_selector(DyNetworkCommand::onHttpRequestCompleted));
            
            if(request)
            {
                request->setTag(NETWORK_SERVICE_SCORE_REGIST);
            }
        }
            break;
        case DyNetworkCmdtype_TotalScoreGet:
        {
            std::string bufURL = NETWORK_SUB_URL;
            bufURL += NETWORK_SERVICE_SCORE_TOTAL_GET;
            
            std::string bufParam = "version=";
            sprintf(lib->m_strTemp, "%d", CONFIG_VER);
            bufParam += lib->m_strTemp;
            bufParam += "&platform=";
            bufParam += dataManager->m_platform;
            
            bufParam += "&user_id=";
            bufParam += dataManager->m_user_id;
 
            bufParam += "&score_type=";
            bufParam += m_param;
            
            CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), httpresponse_selector(DyNetworkCommand::onHttpRequestCompleted));
            
            if(request)
            {
                request->setTag(NETWORK_SERVICE_SCORE_TOTAL_GET);
            }
        }
            break;
        case DyNetworkCmdType_NicknameExist:
        {
            std::string bufURL = NETWORK_SUB_URL;
            bufURL += NETWORK_SERVICE_NICKNAME_EXIST;
            
            std::string bufParam = "version=";
            sprintf(lib->m_strTemp, "%d", CONFIG_VER);
            bufParam += lib->m_strTemp;
            bufParam += "&platform=";
            bufParam += dataManager->m_platform;
            
            bufParam += "&user_id=";
            bufParam += dataManager->m_user_id;
            
            CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), httpresponse_selector(DyNetworkCommand::onHttpRequestCompleted));
            
            if(request)
            {
                request->setTag(NETWORK_SERVICE_NICKNAME_EXIST);
            }
        }
            break;
        case DyNetworkCmdType_NicknameRegist:
        {
            std::string bufURL = NETWORK_SUB_URL;
            bufURL += NETWORK_SERVICE_NICKNAME_REGIST;
            
            std::string bufParam = "version=";
            sprintf(lib->m_strTemp, "%d", CONFIG_VER);
            bufParam += lib->m_strTemp;
            bufParam += "&platform=";
            bufParam += dataManager->m_platform;
            
            bufParam += "&user_id=";
            bufParam += dataManager->m_user_id;
            
            
            bufParam += "&nickname=";
            bufParam += m_param;

            
            CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), httpresponse_selector(DyNetworkCommand::onHttpRequestCompleted));
            
            if(request)
            {
                request->setTag(NETWORK_SERVICE_NICKNAME_REGIST);
            }
        }
            break;
        case DyNetworkCmdType_UserScoreGet:
        {
            std::string bufURL = NETWORK_SUB_URL;
            bufURL += NETWORK_SERVICE_SCORE_USER_GET;
            
            std::string bufParam = "version=";
            sprintf(lib->m_strTemp, "%d", CONFIG_VER);
            bufParam += lib->m_strTemp;
            bufParam += "&platform=";
            bufParam += dataManager->m_platform;
            
            bufParam += "&user_id=";
            bufParam += dataManager->m_user_id;
            
            
            Json::Value root;   // will contains the root value after parsing.
            Json::Reader reader;
            
            reader.parse(m_param, root);
            
            std::string user_id = root["user_id"].asString();
            std::string score_type = root["score_type"].asString();

            
            bufParam += "&user_id=";
            bufParam += user_id;
            
            bufParam += "&score_type=";
            bufParam += score_type;
            
            CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), httpresponse_selector(DyNetworkCommand::onHttpRequestCompleted));
            
            if(request)
            {
                request->setTag(NETWORK_SERVICE_SCORE_USER_GET);
            }
        }

            break;

        default:
            break;

    }
    m_status = DyNetworkCmdStatus_Process;
}



void DyNetworkCommand::net_LoadGameInfo()
{
    DyPopup* popup = DyLib::sharedLib()->show_network_alaram(true);
    popup->setText(STRING_TITLE_LOGIN_GETUSER);
    YtPlatform_NetKakao_LoadGameInfo(DyNetworkCommand_value_str);
}



void DyNetworkCommand::net_LoadGameMe()
{
    YtPlatform_NetKakao_LoadGameMe(DyNetworkCommand_value_str);
}


void DyNetworkCommand::onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response)
{
    //YtLib* lib = YtLib::sharedLib();
    DyNetworkCommand* command = (DyNetworkCommand*)g_sharedDyNetworkMananger->getCommand(0);
    
    if(!response || !response->isSucceed())
    {
        command->m_callFunc(command->m_commandtype, 1, NULL);
        command->m_status = DyNetworkCmdStatus_End;
        
        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
        popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        return;
    }
    
    std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    //CCLog("strData = %s", strData.c_str());
    
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    
    reader.parse(strData, root);
    
    Json::Value result_code = root.get("result_code", 0);
    
    command->m_callFunc(command->m_commandtype, result_code.asInt(), (void*)strData.c_str());
    command->m_status = DyNetworkCmdStatus_End;
}


DyNetworkManager::DyNetworkManager()
{
    m_poolCommands = new CCArray();
}


DyNetworkManager::~DyNetworkManager()
{
    m_poolCommands->release();
}


void DyNetworkManager::update(float dt)
{
	if(m_poolCommands->count() == 0)
		return;
    
    DyNetworkCommand* command = (DyNetworkCommand*)m_poolCommands->objectAtIndex(0);
    
    DyNetworkCmdStatus status = command->getStatus();
    
    switch(status)
    {
        case DyNetworkCmdStatus_Ready:
            command->process();
            break;
        case DyNetworkCmdStatus_Process:
            break;
        case DyNetworkCmdStatus_End:
            m_poolCommands->removeObjectAtIndex(0);
            break;
    }
}


void DyNetworkManager::addLast(DyNetworkCommand* command)
{
	m_poolCommands->addObject(command);
}


DyNetworkCommand* DyNetworkManager::getCommand(int index)
{
    return (DyNetworkCommand*)m_poolCommands->objectAtIndex(index);
}

DyNetworkManager* DyNetworkManager::sharedNetworkManager()
{
	if(g_sharedDyNetworkMananger == NULL)
	{
		g_sharedDyNetworkMananger = new DyNetworkManager();
	}
	return g_sharedDyNetworkMananger;
}


