/*
DyLib.cpp
Doggy chef
@initialize at 120907

@history
120907	yoonsr	initialize
 
ver 1.4.00(140503)
- 약관 동의 방식 변경
- 한식집 추가
 
ver 1.3.01(140403_0)
 - 피자집 크레이지 모드 밸런스 값

ver 1.3.00(140402_0)
- 피자집 추가

ver 1.2.14(140307_0)
- 전체랭킹
- 교복코스튬 추가
- 과금 안정화
 
 
ver 1.2.13(140211_0)
 - 찬스150적용
 - 똑딱똑딱 버그 수정
 - 발랜타인데이 타이틀 변경
 - 중국집 보너스 스테이지 10개 
 - 큐피드 코스튬 추가
 - 안드로이드 과금 안정화
 - 단축아이콘 두개 생기는 버그 수정
 
ver 1.2.12(140205_0)
 - 광고제거
 - 커스튬선물 한자리 버그 수정

ver 1.2.11(140204_1)
 - 초대 버그 수정

 ver 1.2.10(140204_0)
 - 초대하기 로딩 일찍뜨기/코스튬 선물하기
 - 40명 후 재초대 버그 수정
 
ver 1.2.09(140201_0)
 - 커피숍 39 스테이지 퀘스트 숫자 잘못된 것 수정
 - unregist migration
 - 선물관련 migration

ver 1.2.08(140131_0)
 - 공지사항 오늘 하루 보지 않기 누르면 닫기
 - 공지사항/파라미터 마이그레이션
 - 데이터 복구 기능 제거
 - 찬스 이벤트
 - 이모 여기요 셋트 추가
 - 초대 이벤트 변경
 
ver 1.2.07(140125_0)
 - 룰렛 선물 값 줄임
 - 커피숍 보너스 스테이지 10개추가
 
ver 1.2.06(140118_0)
- 스테이지별 점수 랭킹 기능 추가
- 친추게시판 바로가기를 친구초대팝업에도 추가
- 산타코스튬 판매 제거
 
ver 1.2.05(140112_0)
 - 델리선물하기 기능 추가(안드로이드)
 - 룰렛 모자 겹쳐서 나올경우 골드로 보상
 - 친구추가 게시판 추가
 
ver 1.2.04(140107_0)
 - 친구가 보낸 개스트 버그 수정
 - 출석체크 10일로 변경

ver 1.2.03(140105_0)
- 한복 코스튬 추가
- 타이틀 설분위기로 교체
- 업적(레시피, 코스튬) 부드럽게 움직이도록 수정
- 과금.푸쉬 안정성 높임
 
ver 1.2.02(131231_1)
- 코스튬 상세팝업 죽는 것 수정

ver 1.2.01(131231_1)
- 업적 들어갈때 강제종료 버그 수정

ver 1.2.00(131231_0)
- 중국집 추가!

ver 1.1.05(131221_0)
 - 긴급패치 - 산타 코스튬 관련 버그 수정
 
ver 1.1.04(131213_0)
- 긴급패치 - 레시피저장관련 문제 발견함
 
 
ver 1.1.02(131213_0)
- 중국집 내부적으로 추가 되어 있음
 
 
ver 1.1.01(131210_0)
- 초기화 되는 현상 방어코드 삽입
- 아이템 데이터가 private/public 꼬여있었던 것 수정
- 타이틀 트리장식 배경음 교체
- 산타 코스튬 추가 및 호환
 
ver 1.1.00(131206_0)
- 크레이지 모드에서 개스트 소모되는 버그 수정
- 안드로이드 단축아이콘 1개생기도록
- 햄버거 시크릿 레시피 골드로 구매해지는 버그 수정
- 햄버거가게 레시피를 모두 모아야 시크릿 레시피를 구매할 수 있어요.. 짤림
- 퀘스트아이템—>퀘스트음식(퀘스트팁 수정필요함)
- 햄버거 블럭 일부 변경
- 기계 업그레이드
- 가게 셋팅 추가
- 룰렛 하루 5번제한(슈퍼맨 이벤트 확률제어)

ver 1.0.16(131127_1)
- 안드로이드 단축아이콘 1개생기도록(생김)
 
ver 1.0.15(131127_0)
- 레시피에 포함되어 있더라도, 필요블락일 경우에만 가중치 부여하도록 수정
- 더미블럭 제거
- 애니메이션 테스트 창 보이는 버그 수정
- 공지사항 타이틀 글자버퍼 초기화 안했던 버그 수정. 
- 탈퇴/복귀기능 추가
- 앱스토어 링크 소스에 하드코딩
- 껌 받고 보내기
- 과금 안정화
- 껌 가득참 알림(안드로이드)
- 초대메시지 40명 제한 풀음
- 자랑하기 이미지 포함한 메시지 보내기
- 초기화 되는 버그 의심 부분 수정
- KAKAO_SDK_Ver1.2.5b 교체
- 게스트 로그인 제거
- 로그아웃할때 로컬 파일 삭제
- 이미지 넣어서 초대메시지
- 튜토리얼 4스테이지 보강
 
 
ver 1.0.14(131115_0) <--APPLE 검수
- 분식점 난이도 조절(단무지)
 
 
ver 1.0.13(131112_0) 
- 푸쉬수신시 소리 안나게 함(안드로이드)
- 이메일에 마켓 표시
- 공지사항 iOS/Android 구분
- 라스트팡 때는 모든 불럭이 같도록
- 밸런스값 조절(분식점, 햄버거)
- 운영자의 골드 선물 받을 수 있도록 수정
- 크레이지모드 원격제어
- 5콤보 퀘스트일때 5콤보마다가 아니라 5콤보간격으로 처리
- 샵 선택에서 껌 슬롯 색깔 교체

- 단무지 제거
- 블럭 가독성 높임
- 하트 속도 약간 느리게
- 오뎅국물 터치 로직 견고하게 개선
- 5콤보 10콤보 퀘스트 버그 수정
- 재료 슬롯 이미지 변경
- 개스트 델리 체크 로직
- 플랫폼별 선물 보낼수있는 기능 추가
 

ver 1.0.12 (131110_0)
- 다음 샵 열기 UI 변경
- 퀘스트 실패 요인 알리기
- 분식점 12스테이지 떡뽁이 4명중에 하나 나오도록 수정
 
ver 1.0.11 (131109_1)
- 업적 껌 보상 서버 업데이트 처리 완료 

ver 1.0.10 (131109_0)
- 룰렛에서 껌 얻었을 때 버그 수정
 
ver 1.0.09 (131108_0) 카카오 런칭 버전
 버전체크를 android 1.0.08로 처리 iOS는 1.0.07처리
- 팁 변경
- 푸들관련 튜토리얼 추가
- 튜토리얼 화살표 추가
- 결제과정 동선 줄임
- 상태업데이트(댓글, 출석체크 실패시)
- 룰렛 무료 있을 때 표시
- 룰렛 효과음 추가
- 단골 퀘스트 명칭 변경( 이미 깬 스테이지)
- 튜토리얼 스킵 기능 추가
- 안드로이드 바로가기 적용
- 시크릿 레시피 구매유도
- 골드/델리 음수로 되는 현상 방어
- 메인에서 스타트 버튼 눌렀을 때 다른 버튼 눌러지는 것 방지
- 종료메시지 도기쉐프 수정
- 현재 버전 체크 루틴은 삭제하고 최소버전만 체크
 
ver 1.0.08 (131029_1)
- 스테이지 게임시작 버튼
- 게스트모드 게임중에 껌모자랄 경우 친구초대 방지
- 게임중 스토리모기 경고창
- 커피숖 타리미스 초코레시피로 추가
 
ver 1.0.07 (131029_0)<----APPLE 검수
- 초대하기 메시지 수정
- 종료하기 메시지 수정
- 스테이지1 시작버튼 인디게이터
- 토스트 디자인 수정
- 옵션에 블로그가기
- 껌관련 버그 수정
- 레시피 바로구매 관련 버그 수정
- 자랑하기 부분버그 수정
- 룰렛, 랜덤박스 아이템 갯수 조절
 
ver 1.0.05 (131028_0)
- 튜토리얼 zorder문제
- 공지사항 오늘 안보는 기능 추가
 
ver 1.0.04 (131021_0)
- 네트웍 끊겼을 때 처리
- iOS검수준비

ver 1.0.03 (131020_0)
- 이벤트 출첵 이미지 교체
- 똑딱나왔을때 퀘스트 실패시 버그 수정
- 일부 친구 닉네임 안보이는 버튼
- 클로즈, 뒤로, 코인추가 버튼 영역 크게
- 디버깅로그 제거
- 튜토리얼 추가

ver 1.0.02 (131017_0)
- 카카오검수 통과

ver 0.8.61 (131014_0)
- 각종 네트웍 에러 처리
- 델리 선물하기 일단 제거
- 게스트 로그인 처리
- 초대하기 갯수 체크
- iOS 가격을 달러로 변경
- 게임중 정지화면 교체

ver 0.8.60 (131010_2)
- iOS푸쉬
- iOS7 status bar 처리
- 타이틀화면에서도 취소키로 종료가능하도록 수정
- 더미 친구 추가
- 타이틀 수정
- 도움말 일부 수정
- 초대하기 이벤트 수정

ver 0.8.59 (131007_2)
- iPad 광고 크기 조절
- 아이템 구매에서 골드 모자를 때 생기는 버그 수정
- 커피숍 데이터 변경
- 선물하기 쿨링타임 3시간으로 변경

ver 0.8.58 (131007_1)
- 카울리 광고

ver 0.8.57 (131005_1)
- 도움말 수정
- 개스트 선물 받다가 죽는 부분 수정
- 퀘스트 실패시에는 개스트 찬스 안나오도록 수정
- 이전 샵 클리어 후 샵 열기 UI수정
- 크레이지 오픈 알림 추가

ver 0.8.56 (131004_1)
- 초대하기 40명까지로 변경
- 댓글 보상 5델리로 상향조정
- 디폴트 레시피는 뉴레시피 안뜨도록 수정
- 개스트 찬스 부분 오류 수정
- 점수 결과창 획득골드 표시 수정
- 퀘스트 설명 변경

ver 0.8.55 (131004_0)
- 스테이지 실패시 개스트 사용유도
- 댓글유도(첫번째 퍼펙트, 5번골이상마다, 달때까지, 3델리보상)
- 결과창 획득골드 효과 수정
- 순위 변동 이펙트
- 밸런스 데이터 변경
- 크레이지 모드 실시간 순위 체크
- 게임 pause - 커튼 내리기
- 게임중 백그라운드모드(전화올때등등..)로 갈때 pause 처리

ver 0.8.54 (131002_1)
- 알림문구 일부 변경
- 레시피 보유시 판매금액 표시안함
- 스테이지 실패시 다음판 3델리
- 개스트 가장 비싼것 시키는 로직 변경
- 커피숍 밸런싱 데이터 변경

ver 0.8.53 (131002_0)
- 이미 구매한 레시피가 출현했을 때 알림
- 캐셔 등장 타임 조절
- 크레이지모드에서 미보유 레시피는 나오는 현상 제거

ver 0.8.52 (131001_0)
- sdk v1.2.5로 교체
- 아이템관련 알림창 수정
- 레시피 바로구매
- 햄버거 밸런스 데이터 수정
- 전체구매 할인딱지
- 전체구매했을 때 해당 버튼 disable 처리
- 스테이지모드 게임 시작전 퀘스트알림

ver 0.8.51 (130930_0)
- iOS과금 처리
- 블럭 떨어지는 것 개선
- MSG/사탕처리 개선
- 슬롯아이템 사용 퀘스트 전에 장착 확인
- 팝업버튼, 아이템이름 한글화

ver 0.8.50 (130929_0)
- 설정/도움말버튼
- 커스튬 상세정보
- 도움말
- 퀘스트설명창

ver 0.8.49 (130928_0)
- 코스튬 관련버그 수정(샵에서 입힐때 산것만 입힘)
- 레티나 버그 수정
- 썸네일 처리 방식 변경
- 가끔 캔디아이템 처리 안되는 버그

ver 0.8.48 (130926_0)
- 블럭움직임 개선
- 손님 나갈때 싱크

ver 0.8.47 (130923_0)
- 유저 정보 팝업 교체
- block_weight 밖으로 빼냄
- private data 2000bytes로 증가

ver 0.8.46 (130922_1)
- 공지사항

ver 0.8.45 (130922_0)
- 레시피 주문당 재료 갯수 한도 제거(기존은 6개로 했는데, 치킨이 6개여서 주문할 수 없었음)
- cheat 화면 개선
*/


bool g_isDebug = false;



//#define RESET_NETWORK_DATA
//
// 버전
#ifdef ANDROID
// 1206 20140119    스테이지별 점수 랭킹 보기
// 1205 20140112    델리선물하기 기능 추가(안드로이드)/룰렛 모자 겹쳐서 나올경우 골드로 보상/친구추가 게시판 추가
// 1204 20140107    출석체크상향/개스트관련버그수정
// 1203 20140105    설빔추가
// 1202 20131231    커스튬설명, 업적에서 뻗는 것 수정하여 업데이트
// 1200 20131231    중국집 업데이트
// 1105 20131221    커스튬 관련 버그 ㅅ정
// 1104 20131213    릴리즈
// 1102 20131213    릴리즈
// 1100 20131206    릴리즈
// 1016 20131127    릴리즈
// 1015 20131127    릴리즈
// 1014 20131115    릴리즈
// 1011 20131109    껌관련 보상 버그 수정
// 1010 20131109    룰렛 버그 수정
// 1009 20131108    카카오 런칭
// 1008 20131107    카카오 사전런칭
int CONFIG_VER = 1400;                 //(4자리)1800      1.8.00GK            G(google), A(apple),           K(Korea)
#else
// 1206 20140119    스테이지별 점수 랭킹 보기
// 1204 20140107    출석체크상향/개스트관련버그수정
// 1203 20140105    검수
// 1202 20131231    검수
// 1101 20131210    검수
// 1015 20131127    검수
// 1014 20131115    검수
// 1013 20131112    검수(자체리젝)
// 1009 20131108    카카오 런칭 전 apple 검수
// 1007 20131029    apple 2차 검수
// 1004 20131021    apple 검수
int CONFIG_VER = 1400;                 //(4자리)1800      1.8.00GK            G(google), A(apple),           K(Korea)
#endif


#include "DyConfig.h"
#include "DyLib.h"


#include "DySceneGame.h"
#include "DySceneTitle.h"
#include "DySceneMainMenu.h"
#include "DySceneAnimationTest.h"
#include "DySceneStage.h"
#include "DySceneCheat.h"
#include "DySceneStory.h"
#include "DySceneTip.h"


#include "DyLoader.h"
#include "DyFriend.h"

#include "YtNetwork.h"
#include "DyRecipe.h"

#include "pugixml.hpp"
#include "DyPopup.h"
#include "DySceneIntro.h"
#include "DyCostume.h"
#include "DyRank.h"

#include "json.h"

#include "DyTycoon.h"
#include "DySceneStory.h"
#include "DyGift.h"
#include "DyGuest.h"
#include "DyNotice.h"
//DyGiftTypeDelly,
//DyGiftTypeGaest,
//DyGiftTypeGum,
//DyGiftTypeRoullette,



extern YtLib *g_sharedLib;

void DyLib::loadDefaultResources()
{
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("menu_ui.plist");
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("main_ui.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("game_ui.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("menu2_ui.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("staff_ui.plist");
}

DyLib::DyLib()
{
    m_showLoadingPopup = false;

    m_parameters = DyLoader::sharedLoader()->loadObjects("parameters.txt");
	m_parameters->retain();
    
    m_crazy = DyLoader::sharedLoader()->loadObjects("crazy.txt");
    m_crazy->retain();
    
    m_main_server_error = false;
}


DyLib::~DyLib()
{
    m_parameters->release();
    m_crazy->release();
}


void DyLib::callfunc_load_game_messages_success(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer *layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layer->unschedule(schedule_selector(DyLib::callfunc_load_game_messages_success));
    
    DyGiftManager* giftManager = DyGiftManager::sharedGiftManager();
    giftManager->loadGifts(lib->m_buffer);
    
    
    
    lib->network_get_present_list(httpresponse_selector(DyLib::onHttpRequestCompleted));
}


void DyLib::callfunc_load_game_messages_fail(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer *layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layer->unschedule(schedule_selector(DyLib::callfunc_load_game_messages_fail));
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLib::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLib_callfunc_load_game_messages(void* value, int code)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer *layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    if(value != NULL)
    {
        lib->m_buffer = (std::string)(char*)value;
        
        layer->schedule(schedule_selector(DyLib::callfunc_load_game_messages_success), 0.05f);
    }
    else
    {
        layer->schedule(schedule_selector(DyLib::callfunc_load_game_messages_fail), 0.05f);
    }
}


void DyLib::callfunc_load_leader_board_success_crazy(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer *layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layer->unschedule(schedule_selector(DyLib::callfunc_load_leader_board_success_crazy));
    
    DyRankManager::sharedRankManager()->loadLeaderBoard(DyRankType_Crazy, lib->m_buffer);
    
    lib->network_kakao_load_game_messages(DyLib_callfunc_load_game_messages);
}


void DyLib::callfunc_load_leader_board_fail_crazy(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer *layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layer->unschedule(schedule_selector(DyLib::callfunc_load_leader_board_fail_crazy));
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLib::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLib_callfunc_load_leader_boards_crazy(void* value, int code)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer *layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    if(value != NULL)
    {
        lib->m_buffer = (std::string)(char*)value;
        
        layer->schedule(schedule_selector(DyLib::callfunc_load_leader_board_success_crazy), 0.05f);
    }
    else
    {
        layer->schedule(schedule_selector(DyLib::callfunc_load_leader_board_fail_crazy), 0.05f);
    }
}


void DyLib::callfunc_load_leader_board_success(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer *layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layer->unschedule(schedule_selector(DyLib::callfunc_load_leader_board_success));
    
    DyRankManager::sharedRankManager()->loadLeaderBoard(DyRankType_Stage, lib->m_buffer);
    
    lib->network_kakao_load_leader_boards(1, DyLib_callfunc_load_leader_boards_crazy);
}


void DyLib::callfunc_load_leader_board_fail(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer *layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layer->unschedule(schedule_selector(DyLib::callfunc_load_leader_board_fail));
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLib::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}



void DyLib_callfunc_load_leader_boards(void* value, int code)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer *layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    if(value != NULL)
    {
        lib->m_buffer = (std::string)(char*)value;
        
        layer->schedule(schedule_selector(DyLib::callfunc_load_leader_board_success), 0.05f);
    }
    else
    {
        layer->schedule(schedule_selector(DyLib::callfunc_load_leader_board_fail), 0.05f);
    }
}


void DyLib::callfunc_load_game_friend_success(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer *layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layer->unschedule(schedule_selector(DyLib::callfunc_load_game_friend_success));
    
    DyFriendManager::sharedFriendManager()->loadFriends(lib->m_buffer);
    
    lib->network_kakao_load_leader_boards(0, DyLib_callfunc_load_leader_boards);
}


void DyLib::callfunc_load_game_friend_fail(float dt)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer *layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    layer->unschedule(schedule_selector(DyLib::callfunc_load_game_friend_fail));
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLib::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}


void DyLib_callfunc_load_game_firend(void* value, int code)
{
    DyLib* lib = DyLib::sharedLib();
    
    YtLayer *layer = YtLayerManager::sharedLayerManager()->getBaseLayer();
    
    if(value != NULL)
    {
        lib->m_buffer = (std::string)(char*)value;
        
        
        layer->schedule(schedule_selector(DyLib::callfunc_load_game_friend_success), 0.05f);
    }
    else
    {
        layer->schedule(schedule_selector(DyLib::callfunc_load_game_friend_fail), 0.05f);
    }
}


void DyLib::setStatus(DyLib_AppStatus status, int param)
{
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
	m_status_param = param;
    
    //YtPlatform_ShowAd(false);

	switch(status)
	{
        case DyLib_AppStatus_Intro:
            {
                DySceneIntro* pIntro = new DySceneIntro();
                
                YtSceneMananger::changeScene(pIntro, DyLayerIntroLoader::loader(), "layer_intro.ccbi", "DyLayerIntro");
                pIntro->release();
            }
			break;

		case DyLib_AppStatus_Title:
			{
				DySceneTitle* pTitle = new DySceneTitle();
				
				YtSceneMananger::changeScene(pTitle, DyLayerTitleLoader::loader(), "layer_title.ccbi", "DyLayerTitle");
				pTitle->release();
			}
			break;
		case DyLib_AppStatus_Story:
			{
				DySceneStory* pStory = new DySceneStory();
				DyLayerStory* layer;
				switch(param)
				{
                    case DyStoryType_Intro:
                    case DyStoryType_Option:
						YtSceneMananger::changeScene(pStory, DyLayerStoryLoader::loader(), "layer_story.ccbi", "DyLayerStory");
						layer = (DyLayerStory*)pStory->getChildByTag(100);
						layer->m_storytype = (DyStoryType)param;
						break;
					case DyStoryType_Hamburger:
						YtSceneMananger::changeScene(pStory, DyLayerStoryLoader::loader(), "layer_story_hamburger.ccbi", "DyLayerStory");
						layer = (DyLayerStory*)pStory->getChildByTag(100);
						layer->m_storytype = DyStoryType_Hamburger;
						break;
					case DyStoryType_Coffee:
						YtSceneMananger::changeScene(pStory, DyLayerStoryLoader::loader(), "layer_story_coffee.ccbi", "DyLayerStory");
						layer = (DyLayerStory*)pStory->getChildByTag(100);
						layer->m_storytype = DyStoryType_Coffee;
						break;
                    case DyStoryType_China:
						YtSceneMananger::changeScene(pStory, DyLayerStoryLoader::loader(), "layer_story_china.ccbi", "DyLayerStory");
						layer = (DyLayerStory*)pStory->getChildByTag(100);
						layer->m_storytype = DyStoryType_China;
						break;
                    case DyStoryType_Pizza:
                        YtSceneMananger::changeScene(pStory, DyLayerStoryLoader::loader(), "layer_story_pizza.ccbi", "DyLayerStory");
						layer = (DyLayerStory*)pStory->getChildByTag(100);
						layer->m_storytype = DyStoryType_Pizza;
                        break;
                    case DyStoryType_Korean:
                        YtSceneMananger::changeScene(pStory, DyLayerStoryLoader::loader(), "layer_story_korean.ccbi", "DyLayerStory");
						layer = (DyLayerStory*)pStory->getChildByTag(100);
						layer->m_storytype = DyStoryType_Korean;
                        break;
                    case DyStoryType_Puddle:
						YtSceneMananger::changeScene(pStory, DyLayerStoryLoader::loader(), "layer_story_puddle.ccbi", "DyLayerStory");
						layer = (DyLayerStory*)pStory->getChildByTag(100);
						layer->m_storytype = (DyStoryType)param;
                        break;
                    case DyStoryType_Ending:
						YtSceneMananger::changeScene(pStory, DyLayerStoryLoader::loader(), "layer_story_end.ccbi", "DyLayerStory");
						layer = (DyLayerStory*)pStory->getChildByTag(100);
						layer->m_storytype = (DyStoryType)param;
                        break;
				}
				
				pStory->release();
			}
			break;
		case DyLib_AppStatus_MainMenu:
			{
//                if(DyDataManager::sharedDataManager()->m_isGuestLogin)
//                {
//                    DyFriendManager::sharedFriendManager()->loadFriends("");
//                    
//                    DyRankManager::sharedRankManager()->loadLeaderBoard(DyRankType_Stage, "");
//                    
//                    DyRankManager::sharedRankManager()->loadLeaderBoard(DyRankType_Crazy, "");
//                    
//                    
//                    DySceneMainMenu* pMainMenu = new DySceneMainMenu();
//                    YtSceneMananger::changeScene(pMainMenu, DyLayerMainMenuLoader::loader(), "layer_mainmenu.ccbi", "DyLayerMainMenu");
//                    pMainMenu->release();
//                }
//                else
                    network_kakao_load_game_friends(DyLib_callfunc_load_game_firend);
			}
			break;
		case DyLib_AppStatus_Game:
			{
				DySceneGame* pGame = new DySceneGame();
				YtSceneMananger::changeScene(pGame, DyLayerGameLoader::loader(), "layer_game.ccbi", "DyLayerGame");
				pGame->release();
			}
			break;
		case DyLib_AppStatus_AnimationTest:
			{
				DySceneAnimationTest* pAnimationTest = new DySceneAnimationTest();
				YtSceneMananger::changeScene(pAnimationTest, DyLayerAnimationTestLoader::loader(), "layer_animationtest.ccbi", "DyLayerAnimationTest");
				pAnimationTest->release();
			}
			break;
		case DyLib_AppStatus_Cheat:
			{
				DySceneCheat* pCheat = new DySceneCheat();
				YtSceneMananger::changeScene(pCheat, DyLayerCheatLoader::loader(), "layer_cheat.ccbi", "DyLayerCheat");
				pCheat->release();
			}
			break;
		case DyLib_AppStatus_Stage:
			{
				DySceneStage* pStage = new DySceneStage();
				YtSceneMananger::changeScene(pStage, DyLayerStageLoader::loader(), "layer_stage.ccbi", "DyLayerStage");
				pStage->release();
			}
			break;
        case DyLib_AppStatus_Tip:
            {
                DySceneTip* pTip = new DySceneTip();
                YtSceneMananger::changeScene(pTip, DyLayerTipLoader::loader(), "layer_tip.ccbi", "DyLayerTip");
                pTip->release();
            }
            break;
	}
}

void DyLib::setNormalButton(CCMenuItemImage* menuitem, const char* strTagRes)
{
    CCSprite* sprNormal = (CCSprite*)menuitem->getNormalImage();
    sprNormal->setAnchorPoint(ccp(0.5, 0.5));
    sprNormal->setPosition(ccp(sprNormal->getContentSize().width/2, sprNormal->getContentSize().height/2));
    
    float scaleBase = sprNormal->getScale();
    
    if(strTagRes)
    {
        CCSprite* sprText = CCSprite::createWithSpriteFrameName(strTagRes);
        sprText->setPosition(ccp(sprNormal->getContentSize().width/2, sprNormal->getContentSize().height/2));
        
        sprNormal->addChild(sprText);
    }
    
    
    CCSprite* sprSelected = (CCSprite*)menuitem->getSelectedImage();
    sprSelected->setAnchorPoint(ccp(0.5, 0.5));
    sprSelected->setPosition(ccp(sprSelected->getContentSize().width/2, sprSelected->getContentSize().height/2));
    sprSelected->setScale(0.9*scaleBase);
    
    if(strTagRes)
    {
        CCSprite* sprText = CCSprite::createWithSpriteFrameName(strTagRes);
        sprText->setPosition(ccp(sprSelected->getContentSize().width/2, sprSelected->getContentSize().height/2));
        sprSelected->addChild(sprText);
        
    }
}


void DyLib::setMenuItemText(CCMenuItemImage* menuitem, DyButtonType buttontype, const char* text, float normalTextSize, float selectTextSize)
{
	CCSprite* sprNormal = NULL;
	CCSprite* sprSelected = NULL;
    
	if(buttontype == DyButtonType_Blue)
	{
		sprNormal = CCSprite::createWithSpriteFrameName("menu_ui_btn_custom_blue.png");
		sprSelected = CCSprite::createWithSpriteFrameName("menu_ui_btn_custom_blue_sel.png");
	}
	else if(buttontype == DyButtonType_Red)
	{
		sprNormal = CCSprite::createWithSpriteFrameName("menu_ui_btn_custom_red.png");
		sprSelected = CCSprite::createWithSpriteFrameName("menu_ui_btn_custom_red_sel.png");
	}
	else if(buttontype == DyButtonType_Close)
	{
		sprNormal = CCSprite::createWithSpriteFrameName("menu_ui_close.png");
		sprSelected = CCSprite::createWithSpriteFrameName("menu_ui_close_sel.png");
	}
    else
        return;
    
	if(text && strlen(text) > 0)
	{
		CCLabelTTF* labelNormal = CCLabelTTF::create(text, FONT_NAME_DEFAULT, normalTextSize);
		labelNormal->setColor(ccWHITE);
		labelNormal->setPosition(ccp(sprNormal->getContentSize().width/2, sprNormal->getContentSize().height/2));
		sprNormal->addChild(labelNormal);
        
		CCLabelTTF* labelSelected = CCLabelTTF::create(text, FONT_NAME_DEFAULT, selectTextSize);
		labelSelected->setColor(ccc3(255,204,1));
		labelSelected->setPosition(ccp(sprSelected->getContentSize().width/2, sprSelected->getContentSize().height/2));
		sprSelected->addChild(labelSelected);
	}
    
	menuitem->setSelectedImage(sprSelected);
	menuitem->setNormalImage(sprNormal);
}


void DyLib::setMenuItemText(CCMenuItemImage* menuitem, DyButtonType buttontype, const char* text)
{
	setMenuItemText(menuitem, buttontype, text, 35, 40);
}



void DyLib::setMenuItemText_kor(CCMenuItemImage* menuitem, DyButtonType buttontype, const char* text, float normalTextSize, float selectTextSize)
{
	CCSprite* sprNormal = NULL;
	CCSprite* sprSelected = NULL;
    CCSprite* sprDisable = NULL;
    
	if(buttontype == DyButtonType_Blue)
	{
		sprNormal = CCSprite::createWithSpriteFrameName("menu_ui_btn_custom_blue.png");
		sprSelected = CCSprite::createWithSpriteFrameName("menu_ui_btn_custom_blue_sel.png");
        sprDisable = CCSprite::createWithSpriteFrameName("menu_ui_btn_custom_blue_sel.png");

	}
	else if(buttontype == DyButtonType_Red)
	{
		sprNormal = CCSprite::createWithSpriteFrameName("menu_ui_btn_custom_red.png");
		sprSelected = CCSprite::createWithSpriteFrameName("menu_ui_btn_custom_red_sel.png");
        sprDisable = CCSprite::createWithSpriteFrameName("menu_ui_btn_custom_red_sel.png");

	}
	else if(buttontype == DyButtonType_Close)
	{
		sprNormal = CCSprite::createWithSpriteFrameName("menu_ui_close.png");
		sprSelected = CCSprite::createWithSpriteFrameName("menu_ui_close_sel.png");
        sprDisable = CCSprite::createWithSpriteFrameName("menu_ui_close_sel.png");
	}
    else
        return;
    
	if(text && strlen(text) > 0)
	{
		CCLabelTTF* labelNormal = CCLabelTTF::create(text, FONT_NAME_EXP, normalTextSize);
		labelNormal->setColor(ccWHITE);
		labelNormal->setPosition(ccp(sprNormal->getContentSize().width/2, sprNormal->getContentSize().height/2));
		sprNormal->addChild(labelNormal);
        
		CCLabelTTF* labelSelected = CCLabelTTF::create(text, FONT_NAME_EXP, selectTextSize);
		labelSelected->setColor(ccc3(255,204,1));
		labelSelected->setPosition(ccp(sprSelected->getContentSize().width/2, sprSelected->getContentSize().height/2));
		sprSelected->addChild(labelSelected);
        
        
        CCLabelTTF* labelDisable = CCLabelTTF::create(text, FONT_NAME_EXP, selectTextSize);
		labelDisable->setColor(ccc3(48,48,48));
		labelDisable->setPosition(ccp(sprSelected->getContentSize().width/2, sprSelected->getContentSize().height/2));
		sprDisable->addChild(labelDisable);

	}
    
	menuitem->setSelectedImage(sprSelected);
	menuitem->setNormalImage(sprNormal);
    menuitem->setDisabledImage(sprDisable);
}


void DyLib::setMenuItemText_kor(CCMenuItemImage* menuitem, const char* text, float normalTextSize, float selectTextSize)
{
	CCSprite* sprNormal = (CCSprite*)menuitem->getNormalImage();
	CCSprite* sprSelected = (CCSprite*)menuitem->getSelectedImage();
    
	if(text && strlen(text) > 0)
	{
		CCLabelTTF* labelNormal = CCLabelTTF::create(text, FONT_NAME_EXP, normalTextSize);
		labelNormal->setColor(ccWHITE);
		labelNormal->setPosition(ccp(sprNormal->getContentSize().width/2, sprNormal->getContentSize().height/2));
		sprNormal->addChild(labelNormal);
        
		CCLabelTTF* labelSelected = CCLabelTTF::create(text, FONT_NAME_EXP, selectTextSize);
		labelSelected->setColor(ccc3(255,204,1));
		labelSelected->setPosition(ccp(sprSelected->getContentSize().width/2, sprSelected->getContentSize().height/2));
		sprSelected->addChild(labelSelected);
	}
}

void DyLib::setMenuItemText_kor(CCMenuItemImage* menuitem, const char* text)
{
	setMenuItemText_kor(menuitem, text, 35, 40);
}





void DyLib::setMenuItemText_kor(CCMenuItemImage* menuitem, DyButtonType buttontype, const char* text)
{
	setMenuItemText_kor(menuitem, buttontype, text, 30, 35);
}


DyLib* DyLib::sharedLib()
{
	if(g_sharedLib == NULL)
	{
		g_sharedLib = new DyLib();
	}
	return (DyLib*)g_sharedLib;
}


DyPopup* DyLib::show_network_alaram(bool show)
{
    DyPopup* popup = NULL;
    YtLayer* layer = YtLayerManager::sharedLayerManager()->getTopLayer();
    
    m_showLoadingPopup = show;
    
    if(show)
    {
        if(layer->m_tag.compare("DyPopup") == 0)
            return (DyPopup*)layer;
        
        popup = DyPopup::create(DyPopupType_LoadingCycle);
        YtLayerManager::sharedLayerManager()->pushLayer(popup, false, true);
    
        popup->setPosition(ccp(m_sizeWin.width/2, 110));
        
        popup->setText(STRING_TITLE_UPDATE_DATA);
        
    }
    else
    {
        if(layer->m_tag.compare("DyPopup") == 0)
        {
            YtLayerManager::sharedLayerManager()->popLayer();
        }
    }
    return popup;
}


std::string string_to_hex(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();
    
    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

std::string hex_to_string(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();
    //if (len & 1) throw std::invalid_argument("odd length");
    
    std::string output;
    output.reserve(len / 2);
    for (size_t i = 0; i < len; i += 2)
    {
        char a = input[i];
        const char* p = std::lower_bound(lut, lut + 16, a);
        //if (*p != a) throw std::invalid_argument("not a hex digit");
        
        char b = input[i + 1];
        const char* q = std::lower_bound(lut, lut + 16, b);
        //if (*q != b) throw std::invalid_argument("not a hex digit");
        
        output.push_back(((p - lut) << 4) | (q - lut));
    }
    return output;
}


void DyLib::onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response)
{
    DyLib* lib = DyLib::sharedLib();
    
    if(lib->network_checkError(response, true) == false)
    {
        if(lib->network_getTag(response).compare(NETWORK_SERVICE_GET_PRESENT_LIST) == 0 ||
           lib->network_getTag(response).compare(NETWORK_SERVICE_GET_NOTICE_LIST) == 0)
        {
            lib->m_main_server_error = true;
            
            lib->show_network_alaram(false);
            DySceneMainMenu* pMainMenu = new DySceneMainMenu();
            YtSceneMananger::changeScene(pMainMenu, DyLayerMainMenuLoader::loader(), "layer_mainmenu.ccbi", "DyLayerMainMenu");
            pMainMenu->release();
        }
        
        if(lib->network_getTag(response).compare(NETWORK_SERVICE_GET_PARAM) == 0)
        {
            lib->use_heart();
        }
        return;
    }
    
    if(lib->network_getTag(response).compare(NETWORK_SERVICE_GET_PRESENT_LIST) == 0)
    {
        lib->network_get_present_list_ret(sender, response);
        
        if(!DyNoticeManager::sharedNoticeManager()->m_loaded)
        {
            lib->network_get_notice_list(httpresponse_selector(DyLib::onHttpRequestCompleted));
        }
        else
        {
            lib->show_network_alaram(false);
            
            DySceneMainMenu* pMainMenu = new DySceneMainMenu();
            YtSceneMananger::changeScene(pMainMenu, DyLayerMainMenuLoader::loader(), "layer_mainmenu.ccbi", "DyLayerMainMenu");
            pMainMenu->release();
        }
    }
    
    if(lib->network_getTag(response).compare(NETWORK_SERVICE_GET_NOTICE_LIST) == 0)
    {
        lib->network_get_notice_list_ret(sender, response);
        
        lib->show_network_alaram(false);
        DySceneMainMenu* pMainMenu = new DySceneMainMenu();
        YtSceneMananger::changeScene(pMainMenu, DyLayerMainMenuLoader::loader(), "layer_mainmenu.ccbi", "DyLayerMainMenu");
        pMainMenu->release();
    }
    
//    if(lib->network_getTag(response).compare(NETWORK_SERVICE_GET_USER_INFO) == 0)
//    {
//        lib->network_get_user_info_ret(sender, response);
//        
//        lib->show_network_alaram(false);
//        DySceneMainMenu* pMainMenu = new DySceneMainMenu();
//        YtSceneMananger::changeScene(pMainMenu, DyLayerMainMenuLoader::loader(), "layer_mainmenu.ccbi", "DyLayerMainMenu");
//        pMainMenu->release();
//    }

    
    if(lib->network_getTag(response).compare(NETWORK_SERVICE_GET_PARAM) == 0)
    {
        lib->network_get_param_ret(sender, response);
        
        lib->use_heart();
    }

}



void DyLib::network_error()
{
    YtLayerManager::sharedLayerManager()->popLayer();
    
//    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLib::onPopupClicked));
//    popup->setText(STRING_network_error);
//    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1);
//    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
}


bool DyLib::network_checkError(CCHttpResponse *response, bool ignore)
{
   if(!response || !response->isSucceed())
    {
        if(!ignore)
            network_error();
        return false;
    }
	return true;
}


std::string DyLib::network_getTag(CCHttpResponse *response)
{
	return response->getHttpRequest()->getTag();
}



void DyLib::network_kakao_admin_gameinfo_set(SEL_HttpResponse pSelector)
{
    std::string bufURL = KAKAO_SERVICE_GAMEINFO;
    
  	bufURL += "?client_id=";
	bufURL += KAKAO_CLIENT_ID;
	bufURL += "&secret_key=";
	bufURL += KAKAO_SECRET_KEY;
    
	bufURL += "&max_heart=";        //default 10
    sprintf(m_strTemp, "%d", 999);
	bufURL += m_strTemp;
	
	bufURL += "&rechargeable_heart=";   //default 5
    sprintf(m_strTemp, "%d", 5);
	bufURL += m_strTemp;
    
	bufURL += "&heart_regen_interval="; // default 300
    sprintf(m_strTemp, "%d", 8*60);
	bufURL += m_strTemp;
    
	bufURL += "&game_message_interval="; // default 300
    sprintf(m_strTemp, "%d", 6*60);
	bufURL += m_strTemp;
    
	bufURL += "&score_reset_wday="; // default 1
    sprintf(m_strTemp, "%d", 6);
	bufURL += m_strTemp;

	bufURL += "&score_reset_hour="; // default 0
    sprintf(m_strTemp, "%d", 23);
	bufURL += m_strTemp;

	bufURL += "&min_version_for_ios="; // default 1.0
	bufURL += "1.0";

    bufURL += "&min_version_for_android="; // default 1.0
	bufURL += "1.0";

    bufURL += "&current_version_for_ios="; // default 1.0
	bufURL += "1.0";

    bufURL += "&current_version_for_android="; // default 1.0
	bufURL += "1.0";

    bufURL += "&notice="; // default ""
	bufURL += "";

    bufURL += "&message_limit="; // default 100
    sprintf(m_strTemp, "%d", 100);
	bufURL += m_strTemp;

    bufURL += "&message_expire_day="; // default 30
    sprintf(m_strTemp, "%d", 30);
	bufURL += m_strTemp;
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequestPutGet(bufURL.c_str(), pSelector, false);
    
	if(request)
	{
		request->setTag(D_NETWORK_TAG_KAKA_GAMEINFO_SET);
 	}

}


void DyLib::network_kakao_admin_gameinfo_set_ret(CCHttpClient *sender, CCHttpResponse *response)
{
	std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);

    //CCLog("network_kakao_admin_gameinfo_set_ret : %s", strData.c_str());
}



void DyLib::network_kakao_admin_gameinfo_get(SEL_HttpResponse pSelector)
{
    std::string bufURL = KAKAO_SERVICE_GAMEINFO;
    
  	bufURL += "?client_id=";
	bufURL += KAKAO_CLIENT_ID;
	bufURL += "&secret_key=";
	bufURL += KAKAO_SECRET_KEY;
    
	
	CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequestPutGet(bufURL.c_str(), pSelector, true);
    
	if(request)
	{
		request->setTag(D_NETWORK_TAG_KAKA_GAMEINFO_GET);
 	}
}


void DyLib::network_kakao_admin_gameinfo_get_ret(CCHttpClient *sender, CCHttpResponse *response)
{
    std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    //CCLog("network_kakao_admin_gameinfo_get_ret : %s", strData.c_str());
    
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    
    reader.parse(strData, root);

    Json::Value name = root.get("name", "도기셰프");
    
    Json::Value max_heart = root.get("max_heart", 100); // 최대 하트 수
    Json::Value rechargeable_heart = root.get("rechargeable_heart", 5); // 충전 가능 하트수
    Json::Value heart_regen_interval = root.get("heart_regen_interval", 8*60); // 하트 충전 주기(초)
    Json::Value game_message_interval = root.get("game_message_interval", 8*60); // 게임 메세지 전송 주기(초)
    Json::Value invitation_interval = root.get("invitation_interval", 5*60); // 초대 메세지 전송 주기(초)
    Json::Value score_reset_wday = root.get("score_reset_wday", 6); // 점수 리셋 요일 (일:0 월:1, ...)
    Json::Value score_reset_hour = root.get("score_reset_hour", 11); // 점수 리셋 시간 (24h)
    Json::Value next_score_reset_time = root.get("next_score_reset_time", 23); // 다음 점수 리셋 시간(score_reset_wday, hour로 계산)
    Json::Value min_version_for_ios = root.get("min_version_for_ios", "1.0"); // 지원 하는 최소 버전(ios)
    Json::Value min_version_for_android = root.get("min_version_for_android", "1.0"); // 지원 하는 최소 버전(android)
    Json::Value current_version_for_ios = root.get("current_version_for_ios", "1.0"); // 지원 하는 최소 버전(ios)
    Json::Value current_version_for_android = root.get("current_version_for_android", "1.0"); // 지원 하는 최소 버전(android)
    Json::Value notice = root.get("notice", ""); // 공지사항
    Json::Value leaderboards = root.get("leaderboards", ""); // 게임내 리더보드 목록
    
    
    //CCLog("name = %s", name.asCString());
    //CCLog("max_heart = %d", max_heart.asInt());
    //CCLog("rechargeable_heart = %d", rechargeable_heart.asInt());
    //CCLog("heart_regen_interval = %d", heart_regen_interval.asInt());
    //CCLog("game_message_interval = %d", game_message_interval.asInt());
    //CCLog("invitation_interval = %d", invitation_interval.asInt());
    //CCLog("score_reset_wday = %d", score_reset_wday.asInt());
    //CCLog("score_reset_hour = %d", score_reset_hour.asInt());
    //CCLog("next_score_reset_time = %s", next_score_reset_time.asCString());
    //CCLog("min_version_for_ios = %s", min_version_for_ios.asCString());
    //CCLog("min_version_for_android = %s", min_version_for_android.asCString());
    //CCLog("current_version_for_ios = %s", current_version_for_ios.asCString());
    //CCLog("current_version_for_android = %s", current_version_for_android.asCString());
    //CCLog("notice = %s", notice.asCString());
    
//    for( int index = 0; index < leaderboards.size(); ++index )
//    {
//        Json::Value key = leaderboards[index].get("key", "default");        // 리더보드 이름
//        Json::Value name = leaderboards[index].get("name", "default");      // 리더보드 KEY
//        //CCLog("key = %s", key.asCString());
//        //CCLog("name = %s", name.asCString());
//    }
    
    
    
   
//    RW-"max_heart":10,
//    RW-"rechargeable_heart":5,
//    RW-"heart_regen_interval":300,
//    RW-"game_message_interval":300,
 
//    RW-"score_reset_wday":1,
//    RW-"score_reset_hour"0,
//    
//    RW-"min_version_for_ios":"1.0",
//    RW-"min_version_for_android":"1.0",
//    RW-"current_version_for_ios":"1.0",
//    RW-"current_version_for_android":"1.0",
//    
//    RW-"notice":"",
//    RW-"message_limit":100,
//    RW-"message_expire_day":30,
    
//    R-"invitation_interval":2678400,//31일
//    R-"name":"도기셰프",
//    R-"next_score_reset_time":"2013-08-19T00:00:00+09:00",
//    R-"leaderboards":[{"name":"crazymode","key":"crazymode"},{"name":"default","key":"default"}]}

}



bool DyLib::network_proc_game_me(std::string& data)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
  
    Json::Value root;
    Json::Reader reader;
    
    reader.parse(data, root);
   
    Json::Value nickname = root.get("nickname", "도기셰프");
    dataManager->m_nickname = DyLib::getCorrectNickname(nickname.asString());
    
    
    Json::Value user_id = root.get("user_id", "도기셰프");
    dataManager->m_user_id = user_id.asString();

    
    Json::Value profile_image_url = root.get("profile_image_url", "");
    dataManager->m_profile_image_url = profile_image_url.asString();
    
    
    Json::Value heart = root.get("heart", 5);
    dataManager->m_gum = heart.asInt();
    
    Json::Value max_heart = root.get("max_heart", 999);
    dataManager->m_max_gum = max_heart.asInt();
    
    
    Json::Value message_blocked = root.get("message_blocked", 0);
    dataManager->m_message_blocked = message_blocked.asInt();
    
    Json::Value server_time = root.get("server_time", 0);
    dataManager->m_time_server = CCDate(server_time.asDouble());
  
    Json::Value heart_regen_starts_at = root.get("heart_regen_starts_at", 0);
    dataManager->m_heart_regen_starts_at = CCDate(heart_regen_starts_at.asDouble());
  
    CCDate dateNow;
    dataManager->m_time_diff = dataManager->m_time_server - dateNow;
    
    
    DyMoneyManager::sharedMoneyManager()->m_isDoCheckTimeForGum = false;
    if(dataManager->m_gum < dataManager->m_rechargeable_gum)
    {
        DyMoneyManager::sharedMoneyManager()->m_isDoCheckTimeForGum = true;
    }
    
    dataManager->m_crazyRecord = 0;
    dataManager->m_crazyBestRecord = 0;
    
    Json::Value scores = root.get("scores", 0);
    
    for( int index = 0; index < scores.size(); ++index )
    {
        Json::Value leaderboard_key = scores[index].get("leaderboard_key", "default");
        
        if(leaderboard_key.asString().compare("crazymode") == 0)
        {
            Json::Value season_score = scores[index].get("season_score", 0);
            dataManager->m_crazyRecord = season_score.asInt();
            
            Json::Value best_score = scores[index].get("best_score", 0);
            dataManager->m_crazyBestRecord = best_score.asInt();
        }
    }
    
    //
    // 제일 마지막 처리해야함
    Json::Value private_data = root.get("private_data", "");
    Json::Value public_data = root.get("public_data", "");
    
    if(dataManager->m_local.m_dataSaved && (private_data.asString().length() == 0 || public_data.asString().length() == 0))
    {
        return false;
    }
    
//#ifdef RESET_NETWORK_DATA
//    dataManager->dataLoad_private("");
//#else
    dataManager->dataLoad_private(&dataManager->m_private, private_data.asString());
//#endif
    
    
//#ifdef RESET_NETWORK_DATA
//    dataManager->dataLoad_public(&dataManager->m_public, "", true);
//#else
    dataManager->dataLoad_public(&dataManager->m_public, public_data.asString(), true);
//#endif
    
    DyGuestManager::sharedGuestManager()->loadGuest();
    
    return true;
}


void DyLib::network_proc_game_info(std::string& data)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    //CCLog("KakaoAPISTatus_LoadGameInfo : %s", data.c_str());
    
    Json::Value root;
    Json::Reader reader;
    
    reader.parse(data, root);
    
    Json::Value max_heart = root.get("max_heart", 999);
    dataManager->m_max_gum = max_heart.asInt();

    Json::Value rechargeable_heart = root.get("rechargeable_heart", 5);
    dataManager->m_rechargeable_gum = rechargeable_heart.asInt();
    
    Json::Value heart_regen_interval = root.get("heart_regen_interval", 480);
    dataManager->m_heart_regen_interval = heart_regen_interval.asInt();
    
    Json::Value server_time = root.get("next_score_reset_timestamp", 0);
    dataManager->m_time_refresh = CCDate(server_time.asDouble());
    
    dataManager->m_time_refresh.to_str(m_strTemp, 512);
    //CCLog("dataManager->m_time_refresh = %s", m_strTemp);

    dataManager->m_message_interval = root.get("game_message_interval", 0).asInt();
    dataManager->m_invitation_interval = root.get("invitation_interval", 0).asInt();

#ifdef ANDROID
    dataManager->m_min_version = atoi(root.get("min_version_for_android", "0").asString().c_str());
    dataManager->m_current_version = atoi(root.get("current_version_for_android", "0").asString().c_str());
#else
    dataManager->m_min_version = atoi(root.get("min_version_for_ios", "0").asString().c_str());
    dataManager->m_current_version = atoi(root.get("current_version_for_ios", "0").asString().c_str());
#endif

    
    Json::Value notice;
    Json::Reader reader_notice;
    
    reader_notice.parse(root.get("notice", "0").asString(), notice);
    
    
#ifdef NETWORK_TEST
    dataManager->m_main_url = NETWORK_MAIN_URL_TEST;
#else

    dataManager->m_main_url = notice.get("url", "0").asString();
#endif
    
    dataManager->m_notice = notice.get("notice", "0").asString();
    
    dataManager->m_display_ad = notice.get("ad", 0).asInt();
    
    dataManager->m_present_award = notice.get("p", 1).asInt();
    
    dataManager->m_gum_send_award = notice.get("gum", 0).asInt();
    
    dataManager->m_delly_event = notice.get("d_e", 0).asInt();
    
#ifdef ANDROID
    dataManager->m_link = ANDROID_DOWNLOAD_URL;
#else
    dataManager->m_link = IOS_DOWNLOAD_URL;
#endif
    
    dataManager->m_greeting = notice.get("g", "").asString();
    
    dataManager->m_limitSanta_Open = notice.get("limit", 0).asInt();
}

void DyLib::data_backup()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    m_backupPrivate = dataManager->m_private;
    m_backupPublic = dataManager->m_public;
    m_backupGum = dataManager->m_gum;
    m_backupNotProcessedRewaredGum = dataManager->m_rewardedGum;
    
}


void DyLib::data_restore()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    dataManager->m_private = m_backupPrivate;
    dataManager->m_public = m_backupPublic;
    dataManager->m_gum = m_backupGum;
    dataManager->m_rewardedGum = m_backupNotProcessedRewaredGum;
}


void DyLib::use_heart()
{
    DyPopup* popup = show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
    
    Json::Value root;
    
    root["heart_count"] = 1;
    
    Json::StyledWriter writer;
    
    std::string data = writer.write(root);
    
    YtPlatform_NetKakao_SendData(m_callFuncForUpdateMe, NetKakaSendDataType_useHeart, data.c_str());

}

void DyLib::network_kakao_use_heart(YtPlatform_CallFunc callFunc)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    DyLib* lib = DyLib::sharedLib();
    
//    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
//    if(dataManager->m_isGuestLogin)
//    {
//        callFunc(0, 0);
//        return;
//    }
    
    lib->m_callFuncForUpdateMe = callFunc;
    
    if(tycoonManager->m_isCrazyMode)
        lib->network_get_param(httpresponse_selector(DyLib::onHttpRequestCompleted));
    else
        use_heart();
}




void DyLib::network_kakao_update_result(bool isCrazy, YtPlatform_CallFunc callFunc)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
//    if(dataManager->m_isGuestLogin)
//    {
//        callFunc(0, 0);
//        return;
//    }
    
    DyPopup* popup = show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
    
    
    std::string public_data = DyDataManager::sharedDataManager()->dataSave_public();
    std::string private_data = DyDataManager::sharedDataManager()->dataSave_private();
    
    Json::Value root;
    
    root["public_data"] = public_data;
    root["private_data"] = private_data;
    root["exp"] = 0;
    if(isCrazy)
    {
        root["leaderboard_key"] = "crazymode";
        root["score"] = dataManager->m_crazyRecord;
    }
    else
    {
        root["leaderboard_key"] = "default";
        root["score"] = dataManager->m_public.m_stageRecord;
    }
    
    Json::StyledWriter writer;
    
    std::string data = writer.write(root);
    
    YtPlatform_NetKakao_SendData(callFunc, NetKakaSendDataType_updateResult, data.c_str());
}


void DyLib_load_game_me_callfunc(void* value, int code)
{
    DyLib* lib = DyLib::sharedLib();
    
    std::string callFuncString = (char*)value;
    
    if(callFuncString.length() == 0)
    {
        lib->m_callFuncForUpdateMe(1, 0);
    }
    else
    {
        DyDataManager* dataManager = DyDataManager::sharedDataManager();
        
        int current_gum = dataManager->m_gum;
        {
            Json::Value root;
            Json::Reader reader;
            
            reader.parse(callFuncString, root);
            
            Json::Value heart = root.get("heart", 5);
            current_gum = heart.asInt();
        }
        
        {
            std::string public_data = DyDataManager::sharedDataManager()->dataSave_public();
            std::string private_data = DyDataManager::sharedDataManager()->dataSave_private();
            
            Json::Value root;
            
            root["public_data"] = public_data;
            root["private_data"] = private_data;
            root["heart"] = lib->m_heartForUpdateMe;
            root["current_heart"] = current_gum;//dataManager->m_gum-(gum_buyed+dataManager->m_rechargedGum);
            
            Json::StyledWriter writer;
            std::string data = writer.write(root);
            YtPlatform_NetKakao_SendData(lib->m_callFuncForUpdateMe, NetKakaSendDataType_updateMe, data.c_str());
        }
    }
}


DyPopup* DyLib::network_kakao_update_me(int gum_buyed, YtPlatform_CallFunc callFunc)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    dataManager->m_local.m_dataSaved = true;
    dataManager->dataSave_local();
    
//    if(dataManager->m_isGuestLogin)
//    {
//        callFunc(0, 0);
//        return NULL;
//    }
    DyPopup* popup = show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
    popup->setVisible(true);
    
    if(gum_buyed + dataManager->m_rewardedGum != 0)
    {
        m_heartForUpdateMe = gum_buyed + dataManager->m_rewardedGum;
        dataManager->m_rewardedGum = 0;
        m_callFuncForUpdateMe = callFunc;
        YtPlatform_NetKakao_LoadGameMe(DyLib_load_game_me_callfunc);
        return popup;
    }
    
    std::string public_data = DyDataManager::sharedDataManager()->dataSave_public();
    std::string private_data = DyDataManager::sharedDataManager()->dataSave_private();
    
    Json::Value root;
    
    root["public_data"] = public_data;
    root["private_data"] = private_data;
//    if(gum_buyed != 0)
//    {
//        root["heart"] = (gum_buyed+dataManager->m_rechargedGum);
//        root["current_heart"] = dataManager->m_gum-(gum_buyed+dataManager->m_rechargedGum);
//        dataManager->m_rechargedGum = 0;
//    }
    
    Json::StyledWriter writer;
    std::string data = writer.write(root);
    YtPlatform_NetKakao_SendData(callFunc, NetKakaSendDataType_updateMe, data.c_str());
    
    
    return popup;
}


void DyLib::network_kakao_load_game_friends(YtPlatform_CallFuncO callFunc)
{
    DyPopup* popup = show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
    
    YtPlatform_NetKakao_LoadGameFriends(callFunc);
}


void DyLib::network_kakao_load_leader_boards(int type, YtPlatform_CallFuncO callFunc)
{
    DyPopup* popup = show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
 
    YtPlatform_NetKakao_LoadLeaderBoard(callFunc, type);
}


void DyLib::network_send_present(DyFriend* _friend, DyGiftType type, int count, SEL_HttpResponse pSelector)
{
    std::string game_message = "님이 ";

    sprintf(m_strTemp, "%d델리를 보냈어요.", count);
    game_message += m_strTemp;
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL = NETWORK_SUB_URL;
	bufURL += NETWORK_SERVICE_SEND_PRESENT;

  	std::string bufParam = "version=";
    sprintf(m_strTemp, "%d", CONFIG_VER);
    bufParam += m_strTemp;
    bufParam += "&platform=";
	bufParam += dataManager->m_platform;
	
    
    bufParam += "&receiver_id=";
	bufParam += _friend->m_user_id;
	bufParam += "&sender_id=";
    bufParam += dataManager->m_user_id;
	bufParam += "&message=";
    
    bufParam += game_message;
    
    bufParam += "&type=";
    sprintf(m_strTemp, "%d", type);
    bufParam += m_strTemp;
   
    bufParam += "&count=";
    sprintf(m_strTemp, "%d", count);
    bufParam += m_strTemp;
    
    //CCLog("bufURL = %s, bufParam = %s", bufURL.c_str(), bufParam.c_str());

    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);

    
	if(request)
	{
		request->setTag(NETWORK_SERVICE_SEND_PRESENT);
 	}
}


bool DyLib::network_send_present_ret(CCHttpClient *sender, CCHttpResponse *response)
{
	//pugi::xml_document doc;

	std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
//    CCLog("network_send_present_ret() %s", strData.c_str());
    
//	doc.load_buffer_inplace((void*)strData.c_str(), strData.length());

//	pugi::xml_node root = doc.child("send_present");
//
//	pugi::xml_node result_type = root.child("result_type");
//
//    
//    //if(strcmp(result_type.text().as_string(), D_NETWORK_RET_TYPE_SUCCESS) == 0)
//	{
//		return true;
//    }
//	return false;
    return true;
}



void DyLib::network_get_present_list(SEL_HttpResponse pSelector)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL = NETWORK_SUB_URL;
	bufURL += NETWORK_SERVICE_GET_PRESENT_LIST;
    
  	std::string bufParam = "version=";
    sprintf(m_strTemp, "%d", CONFIG_VER);
    bufParam += m_strTemp;
    bufParam += "&platform=";
	bufParam += dataManager->m_platform;
    
    bufParam += "&receiver_id=";
	bufParam += dataManager->m_user_id;

    //CCLog("bufURL = %s, bufParam = %s", bufURL.c_str(), bufParam.c_str());
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
    
	if(request)
	{
		request->setTag(NETWORK_SERVICE_GET_PRESENT_LIST);
 	}
}


bool DyLib::network_get_present_list_ret(CCHttpClient *sender, CCHttpResponse *response)
{
 	std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    DyGiftManager* giftManager = DyGiftManager::sharedGiftManager();
    
    return giftManager->loadGifts_ByMain(strData);
}



void DyLib::network_get_param(SEL_HttpResponse pSelector)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL = NETWORK_SUB_URL;
	bufURL += NETWORK_SERVICE_GET_PARAM;
    
  	std::string bufParam = "version=";
    sprintf(m_strTemp, "%d", CONFIG_VER);
    bufParam += m_strTemp;
    bufParam += "&platform=";
	bufParam += dataManager->m_platform;
   
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
    
	if(request)
	{
		request->setTag(NETWORK_SERVICE_GET_PARAM);
 	}
}


bool DyLib::network_get_param_ret(CCHttpClient *sender, CCHttpResponse *response)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    
    reader.parse(strData, root);
    
    Json::Value params = root.get("params", ""); // 게임내 리더보드 목록
    
    for(int index = 0; index < params.size(); ++index )
    {
        Json::Value param_key = params[index].get("param_key", "");
        Json::Value param_value = params[index].get("param_value", "");
        
        if(param_key.asString().compare("crazy_create_gold_snack") == 0)
            tycoonManager->m_crazy_create_gold_snack = atoi(param_value.asString().c_str());
        
        else if(param_key.asString().compare("crazy_create_gold_hamburger") == 0)
            tycoonManager->m_crazy_create_gold_hamburger = atoi(param_value.asString().c_str());
        
        else if(param_key.asString().compare("crazy_create_gold_coffee") == 0)
            tycoonManager->m_crazy_create_gold_coffee = atoi(param_value.asString().c_str());
        
        else if(param_key.asString().compare("crazy_create_start_gold") == 0)
            tycoonManager->m_crazy_create_start_gold = atoi(param_value.asString().c_str());
        
        else if(param_key.asString().compare("crazy_create_delly_snack_start") == 0)
            tycoonManager->m_crazy_create_delly_snack_start = atoi(param_value.asString().c_str());
        
        else if(param_key.asString().compare("crazy_create_delly_snack_interval") == 0)
            tycoonManager->m_crazy_create_delly_snack_interval = atoi(param_value.asString().c_str());
        
        else if(param_key.asString().compare("crazy_create_delly_hamburger_start") == 0)
            tycoonManager->m_crazy_create_delly_hamburger_start = atoi(param_value.asString().c_str());
        
        else if(param_key.asString().compare("crazy_create_delly_hamburger_interval") == 0)
            tycoonManager->m_crazy_create_delly_hamburger_interval = atoi(param_value.asString().c_str());
        
        else if(param_key.asString().compare("crazy_create_delly_coffee_start") == 0)
            tycoonManager->m_crazy_create_delly_coffee_start = atoi(param_value.asString().c_str());
        
        else if(param_key.asString().compare("crazy_create_delly_coffee_interval") == 0)
            tycoonManager->m_crazy_create_delly_coffee_interval = atoi(param_value.asString().c_str());
    
        else if(param_key.asString().compare("crazy_create_delly_etc_pro") == 0)
            tycoonManager->m_crazy_create_delly_etc_pro = atoi(param_value.asString().c_str());
        
        else if(param_key.asString().compare("crazy_create_delly_second_pro") == 0)
            tycoonManager->m_crazy_create_delly_second_pro = atoi(param_value.asString().c_str());

        else if(param_key.asString().compare("crazy_create_delly_first_pro") == 0)
            tycoonManager->m_crazy_create_delly_first_pro= atoi(param_value.asString().c_str());

        else if(param_key.asString().compare("hero_probability") == 0)
            dataManager->m_cntPovabilityHero = atoi(param_value.asString().c_str());

        else if(param_key.asString().compare("roullette_per_day") == 0)
            dataManager->m_cntRoulletPerDay = atoi(param_value.asString().c_str());

        else if(param_key.asString().compare("crazy_create_gold_china") == 0)
            tycoonManager->m_crazy_create_gold_china = atoi(param_value.asString().c_str());
    
        else if(param_key.asString().compare("crazy_create_delly_china_start") == 0)
            tycoonManager->m_crazy_create_delly_china_start = atoi(param_value.asString().c_str());
    
        else if(param_key.asString().compare("crazy_create_delly_china_interval") == 0)
            tycoonManager->m_crazy_create_delly_china_interval = atoi(param_value.asString().c_str());
    }


    
    
//    CCLog("tycoonManager->m_crazy_create_gold_snack = %d", tycoonManager->m_crazy_create_gold_snack);
//    CCLog("tycoonManager->m_crazy_create_gold_hamburger = %d", tycoonManager->m_crazy_create_gold_hamburger);
//    CCLog("tycoonManager->m_crazy_create_gold_coffee = %d", tycoonManager->m_crazy_create_gold_coffee);
//    CCLog("tycoonManager->m_crazy_create_start_gold = %d", tycoonManager->m_crazy_create_start_gold);
//    CCLog("tycoonManager->m_crazy_create_delly_snack_start = %d", tycoonManager->m_crazy_create_delly_snack_start);
//    CCLog("tycoonManager->m_crazy_create_delly_snack_interval = %d", tycoonManager->m_crazy_create_delly_snack_interval);
//    CCLog("tycoonManager->m_crazy_create_delly_hamburger_start = %d", tycoonManager->m_crazy_create_delly_hamburger_start);
//    CCLog("tycoonManager->m_crazy_create_delly_hamburger_interval = %d", tycoonManager->m_crazy_create_delly_hamburger_interval);
//    CCLog("tycoonManager->m_crazy_create_delly_coffee_start = %d", tycoonManager->m_crazy_create_delly_coffee_start);
//    CCLog("tycoonManager->m_crazy_create_delly_coffee_interval = %d", tycoonManager->m_crazy_create_delly_coffee_interval);
//    CCLog("tycoonManager->m_crazy_create_delly_first_pro = %d", tycoonManager->m_crazy_create_delly_first_pro);
//    CCLog("tycoonManager->m_crazy_create_delly_second_pro = %d", tycoonManager->m_crazy_create_delly_second_pro);
//    CCLog("tycoonManager->m_crazy_create_delly_etc_pro = %d", tycoonManager->m_crazy_create_delly_etc_pro);
//    CCLog("tycoonManager->m_crazy_create_gold_china = %d", tycoonManager->m_crazy_create_gold_china);
//    CCLog("tycoonManager->m_crazy_create_delly_china_start = %d", tycoonManager->m_crazy_create_delly_china_start);
//    CCLog("tycoonManager->m_crazy_create_delly_china_interval = %d", tycoonManager->m_crazy_create_delly_china_interval);
//    CCLog("dataManager->m_cntPovabilityHero = %d", dataManager->m_cntPovabilityHero);
//    CCLog("dataManager->m_cntRoulletPerDay = %d", dataManager->m_cntRoulletPerDay);
  
    
    
//    tycoonManager->m_crazy_create_gold_snack = 400;
//    tycoonManager->m_crazy_create_gold_hamburger = 500;
//    tycoonManager->m_crazy_create_gold_coffee = 600;
//    tycoonManager->m_crazy_create_start_gold = atoi(crazy_create_start_gold.text().as_string());
//    
//    tycoonManager->m_crazy_create_delly_snack_start = atoi(crazy_create_delly_snack_start.text().as_string());
//    tycoonManager->m_crazy_create_delly_snack_interval = atoi(crazy_create_delly_snack_interval.text().as_string());
//    
//    tycoonManager->m_crazy_create_delly_hamburger_start = atoi(crazy_create_delly_hamburger_start.text().as_string());
//    tycoonManager->m_crazy_create_delly_hamburger_interval = atoi(crazy_create_delly_hamburger_interval.text().as_string());
//    
//    tycoonManager->m_crazy_create_delly_coffee_start = atoi(crazy_create_delly_coffee_start.text().as_string());
//    tycoonManager->m_crazy_create_delly_coffee_interval = atoi(crazy_create_delly_coffee_interval.text().as_string());
//    
//    tycoonManager->m_crazy_create_delly_first_pro = atoi(crazy_create_delly_first_pro.text().as_string());
//    tycoonManager->m_crazy_create_delly_second_pro = atoi(crazy_create_delly_second_pro.text().as_string());
//    tycoonManager->m_crazy_create_delly_etc_pro = atoi(crazy_create_delly_etc_pro.text().as_string());

    
    return true;
}



void DyLib::network_get_notice_list(SEL_HttpResponse pSelector)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL = NETWORK_SUB_URL;
	bufURL += NETWORK_SERVICE_GET_NOTICE_LIST;
    
  	std::string bufParam = "version=";
    sprintf(m_strTemp, "%d", CONFIG_VER);
    bufParam += m_strTemp;
    bufParam += "&platform=";
	bufParam += dataManager->m_platform;
    
    bufParam += "&user_id=";
	bufParam += dataManager->m_user_id;
    
    //CCLog("bufURL = %s, bufParam = %s", bufURL.c_str(), bufParam.c_str());
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
    
	if(request)
	{
		request->setTag(NETWORK_SERVICE_GET_NOTICE_LIST);
 	}
}


bool DyLib::network_get_notice_list_ret(CCHttpClient *sender, CCHttpResponse *response)
{
 	std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
   
    DyNoticeManager* noticeManager = DyNoticeManager::sharedNoticeManager();
    
    return noticeManager->loadNotices(strData);
}


//void DyLib::network_get_user_info(SEL_HttpResponse pSelector)
//{
//    DyDataManager* dataManager = DyDataManager::sharedDataManager();
// 	std::string bufURL = dataManager->m_main_url;
//	bufURL += NETWORK_SERVICE_GET_USER_INFO;
//    
//  	std::string bufParam = "version=";
//    sprintf(m_strTemp, "%d", CONFIG_VER);
//    bufParam += m_strTemp;
//    bufParam += "&platform=";
//	bufParam += dataManager->m_platform;
//    
//    bufParam += "&user_id=";
//	//bufParam += "88216888227442624";//dataManager->m_user_id;
//    bufParam += dataManager->m_user_id;
//    
//    
//    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
//    
//	if(request)
//	{
//		request->setTag(NETWORK_SERVICE_GET_USER_INFO);
// 	}
//
//}


//bool DyLib::network_get_user_info_ret(CCHttpClient *sender, CCHttpResponse *response)
//{
//    DyDataManager* dataManager = DyDataManager::sharedDataManager();
// 	
//    std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
//    
//    //CCLog("strData = %s", strData.c_str());
//    
//    pugi::xml_document doc;
//    
//  	doc.load_buffer_inplace((void*)strData.c_str(), strData.length());
//    
//	pugi::xml_node root = doc.child("user_info");
//    
//    pugi::xml_node is_reset = root.child("is_reset");
//    pugi::xml_node last_date = root.child("last_date");
//    
//    pugi::xml_node private_data = root.child("private_data");
//    pugi::xml_node public_data = root.child("public_data");
//    
//    if(strcmp(is_reset.text().as_string(), "true") == 0)
//    {
//        dataManager->m_is_need_reset = true;
//        dataManager->m_reset_last_date = last_date.text().as_string();
//        dataManager->m_reset_private_data = private_data.text().as_string();
//        dataManager->m_reset_public_data = public_data.text().as_string();
//    }
//    else
//    {
//        dataManager->m_is_need_reset = false;
//    }
//    
//    return true;
//}



void DyLib::network_unregist(SEL_HttpResponse pSelector)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL = NETWORK_SUB_URL;
	bufURL += NETWORK_SERVICE_UNREGIST;
    
  	std::string bufParam = "version=";
    sprintf(m_strTemp, "%d", CONFIG_VER);
    bufParam += m_strTemp;
    bufParam += "&platform=";
	bufParam += dataManager->m_platform;
    
    bufParam += "&user_id=";
	bufParam += dataManager->m_user_id;
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
    
	if(request)
	{
		request->setTag(NETWORK_SERVICE_UNREGIST);
 	}

}


bool DyLib::network_unregist_ret(CCHttpClient *sender, CCHttpResponse *response)
{
    //std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    return true;
}
//
//void DyLib::network_set_user_info(SEL_HttpResponse pSelector)
//{
//    DyDataManager* dataManager = DyDataManager::sharedDataManager();
// 	std::string bufURL = dataManager->m_main_url;
//	bufURL += NETWORK_SERVICE_SET_USER_INFO;
//    
//  	std::string bufParam = "version=";
//    sprintf(m_strTemp, "%d", CONFIG_VER);
//    bufParam += m_strTemp;
//    bufParam += "&platform=";
//	bufParam += dataManager->m_platform;
//    
//    bufParam += "&user_id=";
//	bufParam += dataManager->m_user_id;
//    
//    if(dataManager->m_is_need_reset)
//        bufParam += "&reset_flag=1";
//    else
//        bufParam += "&reset_flag=0";
//    
//    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
//    
//	if(request)
//	{
//		request->setTag(NETWORK_SERVICE_SET_USER_INFO);
// 	}
//
//}


//bool DyLib::network_set_user_info_ret(CCHttpClient *sender, CCHttpResponse *response)
//{
//    return true;
//}
//

void DyLib::network_present_check(DyGift* gift, SEL_HttpResponse pSelector)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL = NETWORK_SUB_URL;
	bufURL += NETWORK_SERVICE_PRESENT_CHECK;
    
  	std::string bufParam = "version=";
    sprintf(m_strTemp, "%d", CONFIG_VER);
    bufParam += m_strTemp;
    bufParam += "&platform=";
	bufParam += dataManager->m_platform;
    
    bufParam += "&receiver_id=";
	bufParam += dataManager->m_user_id;
    bufParam += "&gift_id=";
	bufParam += gift->m_gift_id;
    
    bufParam += "&gift_type=";
    if(gift->m_all)
        bufParam += "all";
    else
        bufParam += "single";
    
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
    
	if(request)
	{
		request->setTag(NETWORK_SERVICE_PRESENT_CHECK_TAG);
 	}
}


bool DyLib::network_present_check_ret(CCHttpClient *sender, CCHttpResponse *response)
{
    std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    //CCLog("network_present_check_ret() %s", strData.c_str());
    
    return true;
}



void DyLib::network_present_check_all(SEL_HttpResponse pSelector)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
 	std::string bufURL =NETWORK_SUB_URL;
	bufURL += NETWORK_SERVICE_PRESENT_CHECK;
    
  	std::string bufParam = "version=";
    sprintf(m_strTemp, "%d", CONFIG_VER);
    bufParam += m_strTemp;
    bufParam += "&platform=";
	bufParam += dataManager->m_platform;
    
    bufParam += "&receiver_id=";
	bufParam += dataManager->m_user_id;
    
    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
    
	if(request)
	{
		request->setTag(NETWORK_SERVICE_PRESENT_CHECK_ALL_TAG);
 	}
}


bool DyLib::network_present_check_all_ret(CCHttpClient *sender, CCHttpResponse *response)
{
    std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
    
    //CCLog("network_present_check_all_ret() %s", strData.c_str());
    
    return true;
}



//void DyLib::network_update_result(SEL_HttpResponse pSelector)
//{
//
//    DyDataManager* dataManager = DyDataManager::sharedDataManager();
//    DyStageManager* stageManager = DyStageManager::sharedStageManager();
//    
// 	std::string bufURL = dataManager->m_main_url;
//	bufURL += NETWORK_SERVICE_UPDATE_RESULT;
//    
//  	std::string bufParam = "version=";
//    sprintf(m_strTemp, "%d", CONFIG_VER);
//    bufParam += m_strTemp;
//    bufParam += "&platform=";
//	bufParam += dataManager->m_platform;
//    
//    
//    bufParam += "&user_id=";
//	bufParam += dataManager->m_user_id;
//
//    
//    int max_shop = 0;
//    int max_level = 0;
//    
//    stageManager->getMaxShopStage(&dataManager->m_public, &max_shop, &max_level);
//    
//    
//    bufParam += "&cur_shop=";
//    sprintf(m_strTemp, "%d", max_shop);
//	bufParam += m_strTemp;
//
//    bufParam += "&cur_stage=";
//    sprintf(m_strTemp, "%d", max_level);
//	bufParam += m_strTemp;
//    
//    
//    bufParam += "&stage_record=";
//    sprintf(m_strTemp, "%d", dataManager->m_public.m_stageRecord);
//	bufParam += m_strTemp;
//
//
//    bufParam += "&crazy_record=";
//    sprintf(m_strTemp, "%d", dataManager->m_crazyRecord);
//	bufParam += m_strTemp;
//
//    bufParam += "&crazy_record_best=";
//    sprintf(m_strTemp, "%d", dataManager->m_crazyBestRecord);
//	bufParam += m_strTemp;
//    
//    
//    std::string public_data = DyDataManager::sharedDataManager()->dataSave_public();
//    std::string private_data = DyDataManager::sharedDataManager()->dataSave_private();
//    
//    bufParam += "&public_data=";
//    bufParam += public_data;
//
//    
//    bufParam += "&private_data=";
//    bufParam += private_data;
//    
//    //CCLog("bufURL = %s, bufParam = %s", bufURL.c_str(), bufParam.c_str());
//    
//    CCHttpRequest* request = YtNetworkMananger::sharedNetworkManager()->httpRequest(bufURL.c_str(), bufParam.c_str(), pSelector);
//    
//	if(request)
//	{
//		request->setTag(NETWORK_SERVICE_UPDATE_RESULT);
// 	}
// 
//}


//bool DyLib::network_update_result_ret(CCHttpClient *sender, CCHttpResponse *response)
//{
//    std::string strData = YtNetworkMananger::sharedNetworkManager()->onHttpRequestCompleted(sender, response);
//    
//    //CCLog("network_present_check_all_ret() %s", strData.c_str());
//    
//    return true;
//}


void DyLib::network_kakao_send_present(YtPlatform_CallFuncO callfunc, DyFriend* _friend, DyGiftType type, int count)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyPopup* popup = show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
    
    char parameter[20] = {0, };
    
    std::string talk_message = dataManager->m_nickname + "님이 ";
    
    std::string game_message = "님이 ";
    
    Json::Value root;

    switch(type)
    {
        case DyGiftTypeDelly:
            root["heart"] = 0;
            
            sprintf(m_strTemp, "%d델리를 보냈어요.", count);
            talk_message += m_strTemp;
            game_message += m_strTemp;
            
            break;

        case DyGiftTypeGaest:
            root["heart"] = 0;
            
            talk_message += "개스트를 보냈어요.";
            game_message += "개스트를 보냈어요.";
            
            break;
            
        case DyGiftTypeGum:
            root["heart"] = count;
            talk_message += "껌을 보냈어요.";
            game_message += "껌을 보냈어요.";
            
            break;
        default:
            break;
    }
    
    memcpy(parameter, &type, 4);
    memcpy(parameter+4, &count, 4);
    
    base64_encode(parameter, 20, &dataManager->m_base64Temp);
    root["data"] = dataManager->m_base64Temp;
    
    
    root["talk_message"] = talk_message;
    root["game_message"] = game_message;
    
    
    root["user_id"] = _friend->m_user_id;
    
    Json::StyledWriter writer;
    
    std::string data = writer.write(root);
    
    YtPlatform_NetKakao_SendGameMessage(callfunc, data.c_str());
}



void DyLib::network_kakao_load_game_messages(YtPlatform_CallFuncO callFunc)
{
    DyPopup* popup = show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
    
    YtPlatform_NetKakao_LoadGameMessages(callFunc);
}


void DyLib::network_kakao_accept_message(YtPlatform_CallFunc callFunc, const char* message_id)
{
    DyPopup* popup = show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
    
    YtPlatform_NetKakao_AcceptMessage(callFunc, message_id);
 
}


void DyLib::network_kakao_accept_allmessages(YtPlatform_CallFunc callFunc)
{
    DyPopup* popup = show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
    
    YtPlatform_NetKakao_AcceptAllMessages(callFunc);
}



void DyLib::network_kakao_invite(YtPlatform_CallFuncO callFunc, const char* user_id, const char* message)
{
    DyPopup* popup = show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
    
    YtPlatform_NetKakao_Invite(callFunc, user_id, message);
}


void DyLib::network_kakao_message_block(YtPlatform_CallFunc callFunc, bool messageBlock)
{
    DyPopup* popup = show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
    
    YtPlatform_NetKakao_MessageBlock(callFunc, messageBlock);
}


void DyLib::network_iab(YtPlatform_CallFunc callFunc, const char* product_id)
{
    DyPopup* popup = show_network_alaram(true);
    
#ifdef ANROID
    popup->setVisible(false);
#else
    popup->setText("서버와 통신중");
#endif
    sprintf(m_strTemp, "%s%lld", product_id, this->getCurrentSecond());
    
    m_payload = m_strTemp;
    //, const char* payload
    m_callFuncIAB = callFunc;
    YtPlatform_IAB(callFunc, product_id, m_payload.c_str());
}



void DyLib::network_push_onoff(YtPlatform_CallFunc callFunc, bool onoff)
{
    DyPopup* popup = show_network_alaram(true);
    popup->setText(STRING_TITLE_UPDATE_DATA);
    
    YtPlatform_PushOnOff(callFunc, onoff);
}


void DyLib::onPopupClicked(cocos2d::CCObject * pSender)
{	
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


CCSprite* DyLib::createIndicator(CCPoint position)
{
    CCPoint posSource = ccp(position.x+18, position.y+20);
    CCPoint posTarget = ccp(position.x, position.y);
    CCSprite* sprIndicator = CCSprite::createWithSpriteFrameName("game_ui_hand.png");
    
    sprIndicator->setPosition(posSource);
    
    sprIndicator->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCMoveTo::create(.2f, posTarget), CCMoveTo::create(.2f, posSource), NULL))));
    
    return sprIndicator;
}

CCDictionary* DyLib::createFromString(const char* strData)
{
    CCDictionary* dicRet = CCDictionary::create();
    
    int offset = 0;
    int sizeData = strlen(strData);
    char* cursor = (char*)strData;
	
    char* wordStart = cursor;
	char tockenKey[256] = {0,};
	char tockenValue[256] = {0,};
	
    bool isKey = true;
    //
    // /r/n 0d 0a
    // /t 09
	while(offset < sizeData)
	{   
        //
        // 단어의 끝
		if(*cursor == 0x09)
		{
            if(isKey)
            {
                memcpy(tockenKey, wordStart, cursor-wordStart);
                tockenKey[cursor-wordStart] = 0;
                
                isKey = false;
			}
            else
            {
                
                memcpy(tockenValue, wordStart, cursor-wordStart);
                tockenValue[cursor-wordStart] = 0;
                
                std::string key = tockenKey;
                std::string value = tockenValue;
                
                dicRet->setObject(CCString::create(value), key);
                
                isKey = true;
            }
            wordStart = cursor+1;
          
		}
        else if(*cursor == 0x0a)
        {
            
        }
        
        cursor++;
        offset++;
	}

    return dicRet;
}


std::string DyLib::getCorrectNickname(std::string real_nickname)
{
//    std::wstring nickname = string2wstring(real_nickname);
//    if(nickname.size() > 20)
//    {
//        return wstring2string(nickname.substr(0, 20));
//    }
    return real_nickname;
    //return "가나다라마바사아자차카파하";
}

void DyLib::setupHeartAni(CCSprite* sprHeart, CCSprite* sprHeartShadow)
{
    CCArray* animFrames = CCArray::create();
    
   
    const char* strHeartFrame[4] = {
        "main_ui_gift_heart01.png",
        "main_ui_gift_heart02.png",
        "main_ui_gift_heart03.png",
        "main_ui_gift_heart02.png",
    };
    
    for(int i = 0; i < 4; i++)
    {
        animFrames->addObject(CCSprite::createWithSpriteFrameName(strHeartFrame[i])->displayFrame());
    }
    
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.1f);
    sprHeart->runAction(CCRepeat::create(CCAnimate::create(animation), 1000));
    
    
    CCPoint pointCur = sprHeart->getPosition();
    
    sprHeart->runAction(CCRepeat::create(CCSequence::create(CCMoveTo::create(0.5, ccp(pointCur.x, pointCur.y+10)), CCMoveTo::create(0.5, ccp(pointCur.x, pointCur.y)), NULL), 1000));
    
    
    sprHeartShadow->setScale(0.9f);
    //sprHeartShadow->runAction(CCRepeat::create(CCSequence::create(CCScaleTo::create(0.5, 1.0f), CCScaleTo::create(0.5, 0.9f), NULL), 1000));
    sprHeartShadow->runAction(CCRepeat::create(CCSequence::create(CCFadeTo::create(0.5, 100), CCFadeTo::create(0.5, 255), NULL), 1000));
    
    
//    m_sprPhone->runAction(CCSequence::create(CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::hangOn)), CCAnimate::create(animation), CCDelayTime::create(2.f), CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::hangOn)), CCAnimate::create(animation), CCDelayTime::create(2.f), CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::hangOn)), CCAnimate::create(animation), NULL));
}



