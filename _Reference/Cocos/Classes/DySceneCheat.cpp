/*
DySceneCheat.h
Doggy chef
@initialize at 130128

@history
130128	yoonsr	initialize
*/

#include "DyConfig.h"
#include "DySceneCheat.h"
#include "DyLib.h"
#include "YtAniObject.h"
#include "DyCostume.h"
#include "DyData.h"
#include "DyStage.h"
#include "DyPopup.h"
#include "YtNetwork.h"
#include "DyGuest.h"

#include "pugixml.hpp"
#include "YtLayerLog.h"
#include "DyNetwork.h"

extern std::string g_history;


#define NETWORK_SERVICE_SEND_PRESENT_ALL_OLD       "send_present_all.jsp"
#define NETWORK_SERVICE_SEND_PRESENT_ALL       "present_all_regist.php"


#define NETWORK_SERVICE_REGIST_NOTICE_OLD           "regist_notice.jsp"
#define NETWORK_SERVICE_REGIST_NOTICE           "notices_regist.php"

//%s점장님을 위한 싼타코스튬 한정판매중! 어서 샵으로 ㄱㄱ~"
//%s점장님! 9시30분까지 룰렛10번 돌릴수있어요~
//%s점장님! 오늘은 룰렛15번 돌릴수있어요~
//%s점장님~ 새로 오픈한 중국집을 만나보세요~"

/*
 http://www.yongtrim.com:8080/doggychef/push_test.jsp?message=10시부터 새벽4시까지 껌을 무한리필 해드려요!&title=도기셰프&ticker=껌 무한 리필 이벤트!
 
 http://www.yongtrim.com:8080/doggychef/push.jsp?message=[이벤트]점장님~ 분식점 클리어 하시고 분식세트 받아가세요~&title=도기셰프&ticker=분식세트 받아가세요~
 http://www.yongtrim.com:8080/doggychef/push.jsp?message=[이벤트]크레이지모드 골드 대방출!! 아무 이유없어~ 토요일이니깐!!&title=도기셰프&ticker=골드 대방출
 
 http://www.yongtrim.com:8080/doggychef/push.jsp?message=업데이트하시고 5000골드 받아가세요. 10시까지에요.&title=도기셰프&ticker=업데이트기념
 
 http://www.yongtrim.com:8080/doggychef/push.jsp?message=분식점 클리어 이벤트 당첨자 발표! 두구두구두구&title=도기셰프&ticker=당첨자 발표
 
 
 http://www.yongtrim.com:8080/doggychef/push.jsp?message=크리스마스느낌 물신 나는 도기셰프를 만나시고, 5000골드 받아가세요~&title=도기셰프&ticker=당첨자 발표
 
 
 http://www.yongtrim.com:8080/doggychef/send_present.jsp?receiver_id=88171108243382705&sender_id=88276858327120528&message=test&type=1&count=89
 
 http://www.yongtrim.com:8080/doggychef/send_present.jsp?receiver_id=88171108243382705&sender_id=88276858327120528&message=test&type=0&count=100
 
 http://www.yongtrim.com:8080/doggychef/send_present.jsp?receiver_id=90949963944553024&sender_id=88375411098235377&message=test&type=0&count=500
 
 
 
 http://www.yongtrim.com:8080/doggychef/send_present.jsp?receiver_id=doggychef&sender_id=88137336413539265&message=reward&type=0&count=10
 
 
 ftp://yongtrim2@uws64-042.cafe24.com/www/dn/doggychef.apk
 
 88171108243382705(나)
 89100548714102832(노트)
 88276858327120528(명호)
 88375411098235377(혜경)
 90949963944553024 명호2
 */

/*
 DyGiftTypeDelly = 0
 DyGiftTypeGaest = 1,
 DyGiftTypeGum = 2,
 DyGiftTypeRoullette = 3,
 DyGiftTypeGold = 4,
 DyGiftTypeCostumeSet = 5,
 */

void DyLayerCheat::onPresentClicked(CCObject * pSender)
{
    m_menu->setEnabled(false);
    
    //network_send_present_cafe24(httpresponse_selector(DyLayerCheat::onHttpRequestCompleted));
    
    network_send_present_all_cafe24(httpresponse_selector(DyLayerCheat::onHttpRequestCompleted));
    
    //network_regist_notice_cafe24(httpresponse_selector(DyLayerCheat::onHttpRequestCompleted));
}



void DyLayerCheat::network_regist_notice_cafe24(SEL_HttpResponse pSelector)
{
    DyLib* lib = DyLib::sharedLib();
    
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL = NETWORK_SUB_URL;
	bufURL += NETWORK_SERVICE_REGIST_NOTICE;
    
  	std::string bufParam = "version=";
    sprintf(lib->m_strTemp, "%d", CONFIG_VER);
    bufParam += lib->m_strTemp;
    bufParam += "&notice_type=";
    bufParam += "0";
    bufParam += "&notice_id=";
	bufParam += "140307_00.jpg";
	bufParam += "&notice_msg=";
    bufParam += "http://yongtrim.cafe24.com/dc_notices/140307_00.jpg";
    
    bufParam += "&title=이제 부터 전체랭킹을 확인하실 수 있어요!";
    
	bufParam += "&info=";
    
    bufParam += "{\"width\":920,\"height\":600}";
    //bufParam += "{\"width\":920,\"height\":600,\"buttons\":[{\"x\":260,\"y\":320,\"w\":392,\"h\":144,\"url\":\"http://http://urymunge.dothome.co.kr/phonenumber/phonenumber.html\"}]}";
    //bufParam += "{\"width\":920,\"height\":600,\"buttons\":[{\"x\":260,\"y\":320,\"w\":392,\"h\":144,\"url\":\"http://yongtrim.cafe24.com/dc_admin/cms/m/bbs/board.php?bo_table=regist_friend\"}]}";
	
    CCLog("bufURL = %s, bufParam = %s", bufURL.c_str(), bufParam.c_str());
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
    
	if(request)
	{
		request->setTag(NETWORK_SERVICE_REGIST_NOTICE);
 	}
}


void DyLayerCheat::network_regist_notice(SEL_HttpResponse pSelector)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL = dataManager->m_main_url;
	bufURL += NETWORK_SERVICE_REGIST_NOTICE_OLD;
    
  	std::string bufParam = "version=";
    sprintf(lib->m_strTemp, "%d", CONFIG_VER);
    bufParam += lib->m_strTemp;
    //bufParam += "&platform=ios";
    
    //    bufParam += "&notice_id=";
    //	bufParam += "130922_00.jpg";
    //	bufParam += "&notice_type=";
    //    bufParam += "0";
    //	bufParam += "&notice_msg=";
    //    bufParam += "http://www.yongtrim.com/dc_tycoon_notice/notice_0000.jpg";
    
    bufParam += "&notice_type=";
    bufParam += "0";
    
    
    
    bufParam += "&notice_id=";
	bufParam += "140115_00.png";
	
	bufParam += "&notice_msg=";
    bufParam += "http://www.yongtrim.com/dc_tycoon_notice/140115_00.png";
    
    bufParam += "&title=";
    //
    // !표를 넣어줘야한다.
    //sprintf(lib->m_strTemp, "\"도기셰프\" 오픈 이벤트!!");
    //sprintf(lib->m_strTemp, "\"아이폰 및 아이패드 유저님들께\" 고하는 말씀!!");// 당분간 길이 유지해야한다.
    //sprintf(lib->m_strTemp, "\"분식점 클리어\" 선착순 이벤트를 진행중입니다.");
    //sprintf(lib->m_strTemp, "\"아이폰 및 아이패드 유저님들께\" 알려드려요~");
    //sprintf(lib->m_strTemp, "크레이지 모드 골드 대방출!! 아무이유없어~토요일이니깐!");
    //sprintf(lib->m_strTemp, "분식점 클리어 이벤트 당첨자 발표!!!");
    //sprintf(lib->m_strTemp, "친구 개스트 관련 버그 공지");
    sprintf(lib->m_strTemp, "델리선물하기 업데이트 기념 이벤트");
    base64_encode((char*)lib->m_strTemp, strlen(lib->m_strTemp), &dataManager->m_base64Temp);
    bufParam += dataManager->m_base64Temp;
    
	bufParam += "&info=";
    
    bufParam += "{\"width\":920,\"height\":600}";
    //bufParam += "{\"width\":920,\"height\":600,\"buttons\":[{\"x\":260,\"y\":320,\"w\":392,\"h\":144,\"url\":\"http://http://urymunge.dothome.co.kr/phonenumber/phonenumber.html\"}]}";
    //bufParam += "{\"width\":920,\"height\":600,\"buttons\":[{\"x\":260,\"y\":320,\"w\":392,\"h\":144,\"url\":\"http://yongtrim.cafe24.com/dc_admin/cms/m/bbs/board.php?bo_table=regist_friend\"}]}";
	
    CCLog("bufURL = %s, bufParam = %s", bufURL.c_str(), bufParam.c_str());
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
    
    
	if(request)
	{
		request->setTag(NETWORK_SERVICE_REGIST_NOTICE_OLD);
 	}
    
}


bool DyLayerCheat::network_regist_notice_ret(CCHttpClient *sender, CCHttpResponse *response)
{
    pugi::xml_document doc;
    
	std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    CCLog("DyLayerCheat::network_regist_notice_ret() %s", strData.c_str());
    
	doc.load_buffer_inplace((void*)strData.c_str(), strData.length());
    
	pugi::xml_node root = doc.child("regist_notice");
    
	pugi::xml_node result_type = root.child("result_type");
    
    m_menu->setEnabled(true);
    
    if(strcmp(result_type.text().as_string(), D_NETWORK_RET_TYPE_SUCCESS) == 0)
	{
		return true;
    }
	return false;
    
}



bool DyLayerCheat::network_regist_notice_cafe24_ret(CCHttpClient *sender, CCHttpResponse *response)
{
    //pugi::xml_document doc;
    
	std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    CCLog("DyLayerCheat::network_regist_notice_cafe24_ret() %s", strData.c_str());
    
    return true;
}


void DyLayerCheat::network_send_present_all(SEL_HttpResponse pSelector)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL = dataManager->m_main_url;
	bufURL += NETWORK_SERVICE_SEND_PRESENT_ALL_OLD;
    
  	std::string bufParam = "version=";
    sprintf(lib->m_strTemp, "%d", CONFIG_VER);
    bufParam += lib->m_strTemp;
    bufParam += "&platform=";
	bufParam += dataManager->m_platform;
	
    
    bufParam += "&type=";
	bufParam += "2";
	bufParam += "&count=";
    bufParam += "5";
	bufParam += "&message=";
    
    //std::string game_message = "크리스마스 업데이트 기념이에요. 껌 10개";//맨 마지막은 반드시 한글
    std::string game_message = "껌 5개 받아요";//맨 마지막은 반드시 한글
    
    base64_encode((char*)game_message.c_str(), strlen(game_message.c_str()), &dataManager->m_base64Temp);

    bufParam += dataManager->m_base64Temp;
    
    bufParam += "&start_date=2014-02-09 19:00:00";
    bufParam += "&end_date=2014-02-09 23:59:00";
    
    CCLog("bufURL = %s, bufParam = %s", bufURL.c_str(), bufParam.c_str());
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);

	if(request)
	{
		request->setTag(NETWORK_SERVICE_SEND_PRESENT_ALL_OLD);
 	}
}


bool DyLayerCheat::network_send_present_all_ret(CCHttpClient *sender, CCHttpResponse *response)
{
    pugi::xml_document doc;
    
	std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    CCLog("DyLayerCheat::network_send_present_all_ret() %s", strData.c_str());
    
	doc.load_buffer_inplace((void*)strData.c_str(), strData.length());
    
	pugi::xml_node root = doc.child("send_present_all");
    
	pugi::xml_node result_type = root.child("result_type");
    
    
    m_menu->setEnabled(true);
    
    
    if(strcmp(result_type.text().as_string(), D_NETWORK_RET_TYPE_SUCCESS) == 0)
	{
		return true;
    }
	return false;
    
}


void DyLayerCheat::network_send_present_all_cafe24(SEL_HttpResponse pSelector)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL = NETWORK_SUB_URL;
	bufURL += NETWORK_SERVICE_SEND_PRESENT_ALL;
    
  	std::string bufParam = "version=";
    sprintf(lib->m_strTemp, "%d", CONFIG_VER);
    bufParam += lib->m_strTemp;
    bufParam += "&platform=";
	bufParam += dataManager->m_platform;
	

    
    bufParam += "&type=";
	bufParam += "3";
	bufParam += "&count=";
    bufParam += "5";
	bufParam += "&message=";
    
    bufParam += "룰렛 5개 받으세요~~~";
    
    bufParam += "&start_date=2014-03-09 19:00:00";
    bufParam += "&end_date=2014-03-10 01:59:00";
    
    CCLog("bufURL = %s, bufParam = %s", bufURL.c_str(), bufParam.c_str());
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
    
	if(request)
	{
		request->setTag(NETWORK_SERVICE_SEND_PRESENT_ALL);
 	}
}


bool DyLayerCheat::network_send_present_all_cafe24_ret(CCHttpClient *sender, CCHttpResponse *response)
{
 	std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    //CCLog("DyLayerCheat::network_send_present_all_ret() %s", strData.c_str());
    m_menu->setEnabled(true);
    
    return true;
}



void DyLayerCheat::network_send_present_cafe24(SEL_HttpResponse pSelector)
{
    DyLib* lib = DyLib::sharedLib();
    
    //game_message += "개스트 30분 보냈습니다.";
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL = NETWORK_SUB_URL;
	bufURL += NETWORK_SERVICE_SEND_PRESENT;
    
  	std::string bufParam = "version=";
    sprintf(lib->m_strTemp, "%d", CONFIG_VER);
    bufParam += lib->m_strTemp;
    bufParam += "&platform=";
	bufParam += dataManager->m_platform;
	
    //
    //
    bufParam += "&receiver_id=";
	//bufParam += "88210796612605921";
    //bufParam += "88171108243382705";    //나
    //bufParam += "88276858327120528";  //명호
    //90949963944553024 명호2
    //bufParam += "88191823808792544";//성미
    //bufParam += "88078289466796688";//예현
    //bufParam += "88171601124178864"; //혜경룸메
    //88375411098235377(혜경)
    
    bufParam += "88812240486206912";
    
	bufParam += "&sender_id=";
    bufParam += ADMIN_ID;
	bufParam += "&message=";
    bufParam += "개스트 100마리 받으세요~";
    
    bufParam += "&type=";
    sprintf(lib->m_strTemp, "%d", 1);
    bufParam += lib->m_strTemp;
    
    bufParam += "&count=";
    sprintf(lib->m_strTemp, "%d", 100);
    bufParam += lib->m_strTemp;
    
    CCLog("bufURL = %s, bufParam = %s", bufURL.c_str(), bufParam.c_str());
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
    
	if(request)
	{
		request->setTag(NETWORK_SERVICE_SEND_PRESENT);
 	}
}



bool DyLayerCheat::network_send_present_cafe24_ret(CCHttpClient *sender, CCHttpResponse *response)
{
	//pugi::xml_document doc;
    
	std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    CCLog("DyLayerCheat::network_send_present_ret() %s", strData.c_str());

    m_menu->setEnabled(true);
    
    
    return true;
}



void DyLayerCheat::network_send_present(SEL_HttpResponse pSelector)
{
    DyLib* lib = DyLib::sharedLib();
    
    std::string game_message = "운영자가 ";
    
    //sprintf(lib->m_strTemp, "%d델리를 보냈어요.", 10);
    //game_message += "3델리 를 보냈습니다.";
    game_message += "12델리를 보냈습니다 죄송합니다";
    //game_message += "개스트 30분 보냈습니다.";
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL = dataManager->m_main_url;
	bufURL += "send_present.jsp";
    
  	std::string bufParam = "version=";
    sprintf(lib->m_strTemp, "%d", CONFIG_VER);
    bufParam += lib->m_strTemp;
    bufParam += "&platform=";
	bufParam += dataManager->m_platform;
	
    //
    //
    bufParam += "&receiver_id=";
	//bufParam += "88210796612605921";
    //bufParam += "88171108243382705";    //나
    //bufParam += "88276858327120528";  //명호
    //90949963944553024 명호2
    //bufParam += "88191823808792544";//성미
    //bufParam += "88078289466796688";//예현
    //bufParam += "88171601124178864"; //혜경룸메
    //88375411098235377(혜경)
    
    bufParam += "90949963944553024";
    
    
	bufParam += "&sender_id=";
    bufParam += ADMIN_ID;
	bufParam += "&message=";
    
    base64_encode((char*)game_message.c_str(), strlen(game_message.c_str()), &dataManager->m_base64Temp);
    
    bufParam += dataManager->m_base64Temp;
    
    bufParam += "&type=";
    sprintf(lib->m_strTemp, "%d", 0);
    bufParam += lib->m_strTemp;
    
    bufParam += "&count=";
    sprintf(lib->m_strTemp, "%d", 600);
    bufParam += lib->m_strTemp;
    
    CCLog("bufURL = %s, bufParam = %s", bufURL.c_str(), bufParam.c_str());
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
    
	if(request)
	{
		request->setTag("send_present.jsp");
 	}
}


bool DyLayerCheat::network_send_present_ret(CCHttpClient *sender, CCHttpResponse *response)
{
	pugi::xml_document doc;
    
	std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    
    CCLog("DyLayerCheat::network_send_present_ret() %s", strData.c_str());
    
	doc.load_buffer_inplace((void*)strData.c_str(), strData.length());
    
	pugi::xml_node root = doc.child("send_present");
    
	pugi::xml_node result_type = root.child("result_type");
    
    
    m_menu->setEnabled(true);
    
    
    if(strcmp(result_type.text().as_string(), D_NETWORK_RET_TYPE_SUCCESS) == 0)
	{
		return true;
    }
	return false;
}


DyLayerCheat::DyLayerCheat()
{
    m_tag = DYLAYERCHEAT_TAG;
}


DyLayerCheat::~DyLayerCheat()
{
}


void DyLayerCheat::reloadStatus()
{
	DyLib* lib = DyLib::sharedLib();
	
	DyStageManager* stageManager = DyStageManager::sharedStageManager();

	CCLabelTTF* labelSnack = (CCLabelTTF*)this->getChildByTag(CHEAT_UI_TAG_STATUS_SNACK);
	CCLabelTTF* labelHamburger = (CCLabelTTF*)this->getChildByTag(CHEAT_UI_TAG_STATUS_HAMBURGER);
	CCLabelTTF* labelCoffee = (CCLabelTTF*)this->getChildByTag(CHEAT_UI_TAG_STATUS_COFFEE);
    CCLabelTTF* labelChina = (CCLabelTTF*)this->getChildByTag(CHEAT_UI_TAG_STATUS_CHINA);
    CCLabelTTF* labelPizza = (CCLabelTTF*)this->getChildByTag(CHEAT_UI_TAG_STATUS_PIZZA);
    CCLabelTTF* labelKorean = (CCLabelTTF*)this->getChildByTag(CHEAT_UI_TAG_STATUS_KOREAN);

	sprintf(lib->m_strTemp, "%d", stageManager->getCurrentLevel(0)+1);
	labelSnack->setString(lib->m_strTemp);

	sprintf(lib->m_strTemp, "%d", stageManager->getCurrentLevel(1)+1);
	labelHamburger->setString(lib->m_strTemp);

	sprintf(lib->m_strTemp, "%d", stageManager->getCurrentLevel(2)+1);
	labelCoffee->setString(lib->m_strTemp);
    
	sprintf(lib->m_strTemp, "%d", stageManager->getCurrentLevel(3)+1);
	labelChina->setString(lib->m_strTemp);

	sprintf(lib->m_strTemp, "%d", stageManager->getCurrentLevel(4)+1);
	labelPizza->setString(lib->m_strTemp);

    sprintf(lib->m_strTemp, "%d", stageManager->getCurrentLevel(5)+1);
	labelKorean->setString(lib->m_strTemp);

}



void DyLayerCheat::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
	m_menu = (CCMenu*)getChildByTag(CHAET_UI_TAG_MENU);

	//DyLib *lib = DyLib::sharedLib();
	this->setTouchEnabled(true);

	CCMenuItemImage* menuItemClose = (CCMenuItemImage*)m_menu->getChildByTag(CHAET_UI_TAG_MENUITEM_CLOSE);

	CCLabelTTF* label = CCLabelTTF::create("back", FONT_NAME_DEFAULT, 25.f);
	label->setColor(ccWHITE);
	label->setPosition(ccp(menuItemClose->getContentSize().width/2, menuItemClose->getContentSize().height/2));
	menuItemClose->addChild(label);
    
    
//    YtLayerLog* log = new YtLayerLog(CCSizeMake(400,400), g_history.c_str());
//  
//    addChild(log);
//    log->release();
//    log->setPosition(ccp(500,100));
    
    
    this->scheduleUpdate();

	reloadStatus();
}


SEL_MenuHandler DyLayerCheat::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onBackClicked", DyLayerCheat::onBackClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPlusClicked", DyLayerCheat::onPlusClicked);
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onResetClicked", DyLayerCheat::onResetClicked);
    
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPresentClicked", DyLayerCheat::onPresentClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPresentAllClicked", DyLayerCheat::onPresentAllClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onStaffClicked", DyLayerCheat::onStaffClicked);
    return NULL;    
}

SEL_CCControlHandler DyLayerCheat::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;
}

bool DyLayerCheat::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerCheat::update(float dt)
{
    DyNetworkManager* networkManager = DyNetworkManager::sharedNetworkManager();
    
    networkManager->update(dt);
}



void DyLayerCheat::callfunc_update_result_success(float dt)
{
    this->unschedule(schedule_selector(DyLayerCheat::callfunc_update_result_success));
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyLib::sharedLib()->setStatus(DyLib_AppStatus_MainMenu, 0);
}


void DyLayerCheat::callfunc_update_result_fail(float dt)
{
    this->unschedule(schedule_selector(DyLayerCheat::callfunc_update_result_fail));
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerCheat_callfunc_update_result(int value)
{
    DyLayerCheat *layerCheat = (DyLayerCheat*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERCHEAT_TAG);
    
    if(value == 0)
    {
        layerCheat->schedule(schedule_selector(DyLayerCheat::callfunc_update_result_success), 0.05f);
    }
    else
    {
        layerCheat->schedule(schedule_selector(DyLayerCheat::callfunc_update_result_fail), 0.05f);
    }
}


void DyLayerCheat_NetCallFunc(DyNetworkCmdType cmdType, int value_int, void* value_str)
{
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    //DyLib* lib = DyLib::sharedLib();
    //DyLayerOption *layerOption = (DyLayerOption*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYEROPTION_TAG);
    
    if(value_int != 0)
        return;
    
    switch(cmdType)
    {
        case DyNetworkCmdType_ScoreMigration:
        {
            DyLib::sharedLib()->setStatus(DyLib_AppStatus_MainMenu, 0);
            
        }
            break;
        default:
            break;
    }
}



void DyLayerCheat::onBackClicked(CCObject * pSender)
{
    DyStageManager::sharedStageManager()->calculateTotalPoint();
    DyStageManager::sharedStageManager()->stageSave();
    
    //DyLib::sharedLib()->network_kakao_update_result(false, DyLayerCheat_callfunc_update_result, (void*)this);
    
    DyLib::sharedLib()->show_network_alaram(true);
    DYNETWORK_ADD_PROC(DyNetworkCmdType_ScoreMigration, "", NULL, DyLayerCheat_NetCallFunc);
    
}


void DyLayerCheat::onPlusClicked(CCObject * pSender)
{
	CCMenuItemImage * pMenu = dynamic_cast<CCMenuItemImage*>(pSender);
	int tag = pMenu->getTag();
	int shop = 0;
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyDataManager* dataManager = DyDataManager::sharedDataManager();

	switch(tag)
	{
		case CHEAT_UI_TAG_MENUITEM_PLUS_SNACK:
			shop = 0;
			break;
		case CHEAT_UI_TAG_MENUITEM_PLUS_HAMBURGER:
			shop = 1;
			break;
		case CHEAT_UI_TAG_MENUITEM_PLUS_COFFEE:
			shop = 2;
			break;
        case CHEAT_UI_TAG_MENUITEM_PLUS_CHINA:
			shop = 3;
			break;
        case CHEAT_UI_TAG_MENUITEM_PLUS_PIZZA:
            shop = 4;
            break;
        case CHEAT_UI_TAG_MENUITEM_PLUS_KOREAN:
            shop = 5;
            break;


	}
	int currentLevel = stageManager->getCurrentLevel(shop);

	if(currentLevel == -1)
		return;

    stageManager->setShop((DyStageType)shop);
    
    
	currentLevel++;
    int countLevel = stageManager->getCountLevel(shop);
	if(currentLevel >= countLevel)
	{
        
        int perfect_gold = atoi(stageManager->m_curShopInfo->getValue("perfect_gold", stageManager->getCountLevel(shop)-1));
        
        stageManager->setStageState(shop, stageManager->getCountLevel(shop)-1, DyStageState_Perfect);
        dataManager->shop_set_record(shop, stageManager->getCountLevel(shop)-1, perfect_gold);
		switch(shop)
		{
			case 0:
                stageManager->setStageState(1, 0, DyStageState_Fail);
        		break;
			case 1:
				stageManager->setStageState(2, 0, DyStageState_Fail);
				break;
			case 2:
                stageManager->setStageState(3, 0, DyStageState_Fail);
				break;
            case 3:
			    stageManager->setStageState(4, 0, DyStageState_Fail);
				break;
            case 4:
			    stageManager->setStageState(5, 0, DyStageState_Fail);
				break;
		}
		dataManager->checkAwards(DyAwardsCheckType_Shop, true);
		reloadStatus();
		return;
	}

    
    //int goal_gold = atoi(stageManager->m_curShopInfo->getValue("goal_gold", currentLevel-1));
    int perfect_gold = atoi(stageManager->m_curShopInfo->getValue("perfect_gold", currentLevel-1));
    
    stageManager->setStageState(shop, currentLevel-1, DyStageState_Perfect);
    dataManager->shop_set_record(shop, currentLevel-1, perfect_gold);

    stageManager->setStageState(shop, currentLevel, DyStageState_Fail);
    //dataManager->shop_set_record(shop, currentLevel, 0);

	dataManager->checkAwards(DyAwardsCheckType_Shop, true);
	reloadStatus();
}

void DyLayerCheat::onResetClicked(CCObject * pSender)
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	dataManager->reset();
	reloadStatus();
}


void DyLayerCheat::onPresentAllClicked(CCObject * pSender)
{
//    m_menu->setEnabled(false);
//    network_send_present_all(httpresponse_selector(DyLayerCheat::onHttpRequestCompleted));
}

void DyLayerCheat::onStaffClicked(CCObject * pSender)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    dataManager->m_public.m_staff_grade = 0;
    dataManager->m_private.m_staff_next_grade_remain = atoi(staffManager->m_staff[dataManager->m_private.m_current_staff]->getValue("conditon", dataManager->m_public.m_staff_grade+1));
}



void DyLayerCheat::pause()
{
    m_menu->setEnabled(false);
}


void DyLayerCheat::resume()
{
    m_menu->setEnabled(true);
}


void DyLayerCheat::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}



void DyLayerCheat::onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response)
{
    DyLib* lib = DyLib::sharedLib();
    DyLayerCheat *layerCheat = (DyLayerCheat*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERCHEAT_TAG);
    
    if(lib->network_checkError(response, false) == false)
        return;
    
    if(lib->network_getTag(response).compare("send_present.jsp") == 0)
    {
        layerCheat->network_send_present_ret(sender, response);
    }
    if(lib->network_getTag(response).compare(NETWORK_SERVICE_SEND_PRESENT) == 0)
    {
        layerCheat->network_send_present_cafe24_ret(sender, response);
    }

    if(lib->network_getTag(response).compare(NETWORK_SERVICE_SEND_PRESENT_ALL_OLD) == 0)
    {
        layerCheat->network_send_present_all_ret(sender, response);
    }
    if(lib->network_getTag(response).compare(NETWORK_SERVICE_SEND_PRESENT_ALL) == 0)
    {
        layerCheat->network_send_present_all_cafe24_ret(sender, response);
    }

    if(lib->network_getTag(response).compare(NETWORK_SERVICE_REGIST_NOTICE_OLD) == 0)
    {
        layerCheat->network_regist_notice_ret(sender, response);
    }
    if(lib->network_getTag(response).compare(NETWORK_SERVICE_REGIST_NOTICE) == 0)
    {
        layerCheat->network_regist_notice_cafe24_ret(sender, response);
    }

}


void DyLayerCheat::show()
{
}


DySceneCheat::DySceneCheat()
{
}

DySceneCheat::~DySceneCheat()
{
}
