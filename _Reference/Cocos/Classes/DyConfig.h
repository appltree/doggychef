/*
DyConfig.cpp
Doggy chef
@initialize at 120907

@history
120907	yoonsr	initialize
*/

#ifndef __DYCONFIG_H__
#define __DYCONFIG_H__

//
// 레시피 갯수는 84개를 넘지말아야한다.

//
// 반드시 수정 확인해야함.
#define SHOP_COUNT_ADDED                        (1+1+1)
#define DYDATA_CUSTOMER_ADDED_CNT               (2+2+2)
#define DYDATA_MAX_ADD_COSTUMEITEM_EX           64  //(산타3, 강시5, 한복4, 아줌마5, 큐피드5, 교복4, 피자5, 장금이4)

//
//


//
// 수정하면 안됨
#define SHOP_COUNT_DEFAULT                      3
#define DYDATA_CUSTOMER_DEFAULT_CNT             8



#define COMBO_TIME                              1800.f          //(ms)

#define CONFIG_ADMIN
#define KAKAO_CLIENT_ID                         "89657319379831729"
#define KAKAO_SECRET_KEY                        "f70f32a3d375bd86918048d8155ecd5f797cb438ebba5a52de6b38210b72996c"
#define KAKAO_SERVICE_GAMEINFO                  "https://gameapi.kakao.com/server_v1/game.json"

#define IOS_DOWNLOAD_URL                        "http://itunes.apple.com/app/id714160985"
#define ANDROID_DOWNLOAD_URL                    "market://details?id=com.yongtrim.doggychef"

#define PROMISE_1_URL                           "http://blog.naver.com/yongtrim2013/50179504890"
#define PROMISE_2_URL                           "http://blog.naver.com/yongtrim2013/50179504982"

#define BLOG_URL                                "http://blog.naver.com/yongtrim2013"
#define BOARD_URL                               "http://yongtrim.cafe24.com/dc_admin/cms/m/bbs/board.php?bo_table=regist_friend"
//
// 버전
extern int CONFIG_VER;                          //(4자리)1800      1.8.00GK            G(google), A(apple),           K(Korea)
#define STRING_VER                              "현재 버전 : Ver %d.%d.%02d"


//
// 네트웍 관리
//#define NETWORK_TEST
#define NETWORK_MAIN_URL_TEST					"http://localhost:8080/doggychef_server/"

#define NETWORK_SUB_URL                         "http://yongtrim.cafe24.com/dc/"
#define NETWORK_SERVICE_PRICE_VERIFY            "price_verify.php"
#define NETWORK_SERVICE_SCORE_MIGRATION         "score_migration.php"
#define NETWORK_SERVICE_SCORE_GET               "score_get.php"
#define NETWORK_SERVICE_SCORE_REGIST            "score_regist.php"
#define NETWORK_SERVICE_SCORE_FRIENDS_GET       "score_friends_get.php"
#define NETWORK_SERVICE_USER_REGIST             "user_regist.php"
#define NETWORK_SERVICE_GET_NOTICE_LIST         "notices_get.php"
#define NETWORK_SERVICE_GET_PARAM               "parameters_get.php"
#define NETWORK_SERVICE_UNREGIST                "user_unregist.php"
#define NETWORK_SERVICE_GET_PRESENT_LIST        "present_list_get.php"
#define NETWORK_SERVICE_SEND_PRESENT            "present_regist.php"


#define NETWORK_SERVICE_PRESENT_CHECK           "present_get.php"

#define NETWORK_SERVICE_SCORE_TOTAL_GET         "score_total_get.php"

#define NETWORK_SERVICE_NICKNAME_EXIST          "nickname_exist.php"
#define NETWORK_SERVICE_NICKNAME_REGIST         "nickname_regist.php"
#define NETWORK_SERVICE_SCORE_USER_GET          "score_user_get.php"



#define NETWORK_SERVICE_PRESENT_CHECK_TAG           "present_check"
#define NETWORK_SERVICE_PRESENT_CHECK_ALL_TAG       "present_check_all"


#define ADMIN_ID                                "doggychef"

#define FONT_NAME_DEFAULT                       "anjaeliane.ttf"
#define FONT_NAME_EXP                           "godom.ttf"

#define DATA_FILE_DATA                          "local_data.dat"
#define DATA_FILE_GUEST                         "local_data2.dat"
#define DATA_FILE_NOTICE_DELETE                 "local_data3.dat"
#define DATA_FILE_NOTICE_CHECK                  "local_data4.dat"
#define DATA_FILE_INVITE                        "local_data5.dat"

#define QUESTTAG_GUEST                          "GUEST"
#define QUESTTAG_COMBO                          "COMBO"
#define QUESTTAG_BLOCK                          "BLOCK"
#define QUESTTAG_FOOD                           "FOOD"
#define QUESTTAG_REMOVE                         "REMOVE"
#define QUESTTAG_ITEM                           "ITEM"
#define QUESTTAG_ETC                            "ETC"
#define QUESTTAG_REMAIN                         "REMAIN"

#define GEAST_OPPORTUNIITY_DELLY                3

#define MAX_GAEST                               99
//
// 팁 갯수
#define TIP_CNT                                 23


//#define DYDATA_MAXLEVEL                         30
#define DYDATA_MAX_COSTUMEITEM                  64
#define DYDATA_CNT_RECIPE                       42

#define MAX_SLOT                                3

#define PAY_WANING_TIME                         8.f
#define PAY_WAIT_TIME                           10.f

#define MSG_ADD_MUX                             1.5f
#define ITEM_ADD_TIME                           10.f
#define TIME_WARNING                            10.f

#define COMBO_HEART_CANDY                       3
#define COMBO_HEART_MSG                         5

#define ANDGRY_HEARTLIFE                        0.2
#define LIKE_HEARTLIFE                          0.4

#define STAR_POINT1                             100
#define STAR_POINT2                             200
#define STAR_POINT3                             300

#define STORY_INCOMING_GOLD_GOAL                50
#define STORY_INCOMING_GOLD_GOAL_PLUS           70
#define STORY_INCOMING_GOLD_PERFECT             100

#define COMBO_POINT_1                           10
#define COMBO_POINT_5                           50
#define COMBO_POINT_10                          100

#define ZORDER_GAME_BASE_LEVEL0                 0
#define ZORDER_GAME_BASE_LEVEL1                 10
#define ZORDER_GAME_BASE_LEVEL2                 20
#define ZORDER_GAME_CHARACTER                   100
#define ZORDER_GAME_BALLON                      120
#define ZORDER_GAME_PRICE                       130
#define ZORDER_GAME_MOVEBLOCKTOCUSTOMER         200
#define ZORDER_GAME_EVENT                       400
#define ZORDER_GAME_GAME_INDICATOR              500
#define ZORDER_GAME_MOVE_OBJECT_UNDER           550
#define ZORDER_GAME_MACHINE                     555

#define ZORDER_GAME_SETTING                     560        // 게임 DIM
#define ZORDER_GAME_SETTING_ON                  570        // 게임 DIM
#define ZORDER_GAME_SETTING_ON2                  580        // 게임 DIM
#define ZORDER_GAME_CURTAIN                     600
#define ZORDER_GAME_ON_CURTAIN                  700
#define ZORDER_GAME_MOVE_OBJECT                 800
#define ZORDER_GAME_PAUSELAYER                  900         // 게임중 보통 DIM
#define ZORDER_GAME_MENU                        1000
#define ZORDER_GAME_MENU_DECO                   1010
//#define ZORDER_GAME_SETTING                     2000        // 게임 DIM
//#define ZORDER_GAME_SETTING_ON                  2001        // 게임 DIM

#define ZORDER_GAME_TUTORIAL                    3000



#define CHARACTER_INTERFVAL_AWARD_CNT           40    //40 최대 저장수가 127이므로 이 값은 변함없어야 한다.

#define SHOP_INTERFVAL_AWARD_CNT                10

#define MAX_CRAZY_STEP                          10
#define CRAZY_TIME_ADD                          100
#define CRAZY_REMOVEBLOCK_ADD                   100

#define CRAZY_OPEN_DELLY                        20

#define STRING_GUEST                            "게스트 사용자는 이용하실 수 없습니다."
#define STRING_CRAZY_OPEN_ALARM                 "축하합니다. 별을 모두 모아서 크레이지 모드를 오픈할 수 있어요."
#define STRING_REWARD_ALARM                     "점장님~ 평점 안주실 꺼에요? 별다섯개만 주세요. ㅠㅠ \\ 보상으로 5델리 드릴께요~"
#define STRING_REWARD_RESULT                    "감사합니다. 점장님~ \\ 여기 5델리 받아가세요. \\ 제 전 재산이에요."
#define STRING_GAEST_OPPORTUNITY                "개스트를 바로 불러 이번 스테이지를 마저 클리어 하시겠어요?"
#define STRING_DANGOL_ALRAM                     "단골이 되어야 사진을 얻을 수 있어요."
#define STRING_HIRE_STAFF_SUCCESS               "축하합니다. 이제부터 푸들을 고용할 수 있어요."
//#define STRING_QUIT_MESSAGE                     "정말로 도기셰프와 13마리의 배고픈 동생들을 두고 나가실 거에요?ㅜㅜ"
#define STRING_PURCHASE_FAILURE                 "구매 실패하였습니다."
#define STRING_LOGOUT                           "로그아웃 하시겠습니까?"
#define STRING_LOGOUT_RESULT                    "로그아웃되어 타이틀로 이동합니다."
#define STRING_LEAVE                            "게임을 탈퇴하실 경우 모든 게임데이타, 결제하신 모든 상품들과 구매내역이 삭제되며, 탈퇴후에는 복구가 불가능합니다. 정말 탈퇴하시겠습니까?"
#define STRING_LEAVE_RESULT                     "회원 탈퇴가 완료되었습니다. 타이틀로 돌아갑니다."
#define STRING_APPID_COPY_RESULT                "카카오회원번호가 복사되었습니다. ID를 \"붙여넣기\"할 수 있어요."
#define STRING_TITLE_LOGIN_FAILURE              "로그인 실패하였습니다. 다시 시도해 주세요."

#define STRING_TITLE_LOGIN_GETUSER              "유저정보를 가져오는 중"
//#define STRING_TITLE_LOGIN_GETUSER_FAILURE      "유저정보 가져오기 실패하였습니다. 다시 시도합니다."
//#define STRING_TITLE_LOGIN_GETFRIENDS           "친구들 정보를 가져오는 중"
//#define STRING_TITLE_LOGIN_GETFRIENDS_FAILURE   "친구들 정보 가져오기 실패하였습니다. 다시 시도합니다."
//#define STRING_TITLE_LOGIN_GETSERVER            "서버 정보를 가져오는 중"
#define STRING_TITLE_UPDATE_DATA                "서버와 통신중이에요."
#define STRING_TITLE_UPDATE_DATA_FAILURE        "서버와 통신 실패하였습니다. 다시 시도해 주세요.(계속 실패시, 종료후 재실행해주세요ㅠㅠ)"
#define STRING_TITLE_UPDATE_DATA_FAILURE_END    "서버와 통신 실패하였습니다. 게임을 종료합니다. 다시 시도해 주세요."


//#define STRING_STAGE_CANTNOTHIRE                "이번 스테이지에서는 고용할 수 없습니다."
#define STRING_STAGE_CANTNOTINVITE              "이번 스테이지에서는 초대할 수 없습니다."
//#define STRING_INVITE_GUEST                     "바로 초대하려면 1델리가 소모됩니다. 초대하시겠습니까?\\ (개스트는 가장 비싼 메뉴를 주문해요.)"
#define STRING_INVITE_GUEST                     "바로 초대하려면 1델리가 소모됩니다. \\ (개스트는 가장 비싼 메뉴를 주문해요.)"
#define STRING_INVITE_GUEST2                    "메인화면에서 친구한테 개스트 보내달라고 조르실 수 있어요."
#define STRING_DELLY_UNIT                       "10델리 단위로 교환할 수 있어요. 델리 사러갈까요?"
#define STRING_ITEM_UNIT                        "10개 단위로 살 수 있어요. 골드 교환하러 갈까요?"
#define STRING_NOT_ENOUGH_STAR                  "별이 모자랍니다. 즉시 오픈시 20델리가 소모됩니다. 오픈하시겠습니까?"
#define STRING_NOW_PRMOTE                       "지금 바로 승진 할 수 있어요"
#define STRING_MORE                             "번 더"
#define STRING_HIRE                             "고용하세요."
#define STRING_QUEST_PROMOTE                    "%d델리가 필요합니다. 승진시킬까요?"
#define STRING_MORE_PROMOTE                     "좀 더 진급시켜야하는데..."
#define STRING_EXPLAIN_PROMOTE                  "푸들을 승진시키면 더 많은 일을 도와줘요."
#define STRING_PRIDE                            "게임결과를 \"%s\"님에게 카카오톡으로 자랑하시겠습니까?"
#define STRING_GAMERESULT_PRIDE_CRAZY           "%s 크레이지모드 %d달성"
#define STRING_GAMERESULT_PRIDE_STORY           "%s %d스테이지 %d달성(총점:%d점)"
#define STRING_GAMERESULT_PRIDE_SUCCESS         "메시지 전송 성공했습니다.(자랑하기보상 %d/10)"
#define STRING_SENDMESSAGE_SUCCESS              "메시지 전송 성공했습니다."
#define STRING_SENDMESSAGE_FAILURE              "메시지 전송 실패했습니다."
#define STRING_SENDPRESENT_SUCCESS              "님에게 선물하기 성공했습니다."
#define STRING_SENDPRESENT_FAILURE              "선물하기 실패했습니다."
#define STRING_TIME                             "시간"
#define STRING_SELL_GOLD                        "판매금액"
#define STRING_REMOVE_BLOCK                     "방해블럭제거"
#define STRING_COSTUME_GOLD_LOCK                "%s골드 이상 보유해야 이 옷을 입을 수 있어요."
#define STRING_COSTUME_CRAZY_LOCK               "크레이지모드를 열어야 이 옷을 입을 수 있어요."
#define STRING_COSTUME_ROULETTE_LOCK            "룰렛에서 %s을(를) 얻어야 이 옷을 입을 수 있어요."
#define STRING_GAMERESULT_DELLY                 "다음으로 바로 이동하시려면 %d델리 소모됩니다. 이동하시겠습니까?"
#define STRING_ROULLETTE_DELLY                  "%d델리를 획득하였습니다."
#define STRING_ROULLETTE_GOLD                   "%d골드를 획득하였습니다."
#define STRING_ROULLETTE_GUM                    "껌 %d개를 획득하였습니다."
#define STRING_ROULLETTE_ITEM                   "%s %s개를 획득하였습니다."
#define STRING_ROULLETTE_HAT                    "\"%s\"를(을) 획득하였습니다. 마이룸에 가서 확인해 보세요."
#define STRING_ROULLETTE_GUEST                  "개스트 이용권 %d장을 획득하였습니다."
#define STRING_RECIPE_SET                       "%s 레시피"
#define STRING_RECIPE_SET_DESC                  "시크릿 레시피를 포함한 전체 레시피를 저렴한 가격으로 한꺼번에 구매할 수 있어요!"
#define STRING_RECIPE_SET_BUY_SUCCESS           "구매 완료 하였습니다. 감사합니다."
//#define STRING_RECIPE_SET_ALREADY_BUYED         "이미 구매한 레시피 입니다"
#define STRING_CONFIRM_RECIPESET_BUY            "%s의 모든 레시피를 %d델리에 모두 구매하시겠습니까?"
#define STRING_UNDER_CONSTRUCT                  "준비중입니다. 조금만 기다려주세요."
#define STRING_item_slot_explain                "아이템을 3개까지 장착할 수 있어요."
#define STRING_question_exit                    "정말 배고픈 동생들을 두고 게임에서 나가실꺼예요?"
#define STRING_exchange_question                "%d델리를 %s골드로 교환하시겠어요?"
#define STRING_exchange_result                  "감사합니다."
#define STRING_exchange_result2                 "현재 %s골드를 보유하고 있어요."
#define STRING_item_question                    "%s %d개를 구매하시겠어요?"
#define STRING_item_gold_price                  "%s골드가 소모돼요."
#define STRING_item_buy_result                  "%s %d개를 구매했어요."
#define STRING_item_buy_result2                 "현재 %s %d개를 보유하고 있어요."
#define STRING_notenough_delly                  "델리가 부족합니다.\\ 구매하러갈까요?"
#define STRING_notenough_gold                   "골드가 부족합니다.\\ 구매하러갈까요?"
#define STRING_question_gohome                  "스테이지를 종료하고 홈으로 이동할까요?"
#define STRING_question_golist                  "스테이지를 종료하고 스테이지 리스트로 이동할까요?"
#define STRING_question_replay                  "다시 시작할까요?"
#define STRING_gum                              "껌"
#define STRING_delly                            "델리"
#define STRING_gold                             "골드"
#define STRING_to_you                           "님에게"
#define STRING_question_senditem                "%s님에게 %s %d개를 선물로 보내시겠어요?"
#define STRING_senditem_success                 "선물보내기를 성공하였습니다."
#define STRING_senditem_faiilure                "선물보내기를 실패하였습니다.\\ 다시 시도해주세요."
#define STRING_invite_friend                    "친구를 초대해주세요. 껌을 드려요."
#define STRING_me                               "나"
#define STRING_if_invite_friend                 "친구를 초대하면"
#define STRING_give_gum                         "을 1개 드려요."
#define STRING_buy_question_bygold              "%s(을)를 구매하시겠어요?\\ %s골드가 소모됩니다."
#define STRING_buy_question_bydelly             "%s(을)를 구매하시겠어요?\\ %s델리가 소모됩니다."
#define STRING_buy_question_bygolddelly         "%s(을)를 구매하시겠어요? %s델리와 %s골드가 소모됩니다."
#define STRING_buy_question_bycash              "%s(을)를 구매하시겠어요?\\ 현금 %s원이 결제됩니다."
#define STRING_buy_question_bycash_dollar       "%s(을)를 구매하시겠어요?\\ 현금 $%s이 결제됩니다."
#define STRING_buy_allcostume                   "현재 셋팅되어 있는 코스튬"
#define STRING_result_buy_success               "구매해주셔서 고맙습니다."
#define STRING_result_buy_fail                  "구매를 실패 했습니다. \\ 다시 시도해주세요."
#define STRING_result_buy_cancel                "구매를 취소하셨습니다."
//#define STRING_network_error                    "네트워크 연결을 실패했습니다. WIFI 상태를 확인한 후 다시 게임을 실행해주세요."
#define STRING_not_have_costume                 "아무것도 없습니다. SHOP에서 구매해보세요. 코스튬에 따라 능력치가 달라져요."
#define STRING_secret_recipe                    "%s 레시피를 모두 모아야 시크릿 레시피를 구매할 수 있어요."
#define STRING_shop_recipe                      "%s(을)를 오픈해야 이 레시피를 구매할 수 있어요."
#define STRING_not_have_recipe                  "%s를 오픈해야 구매할 수 있어요."
#define STRING_shop_costume                     "%s(을)를 오픈해야 이 옷을 입을 수 있어요."
#define STRING_recipe_buy_warning               "레시피를 구매하면 더 많은 돈을 벌 수 있어요."
#define STRING_shop_shop                        "%s(을)를 완료해야 골드로 다음 가게를 열 수 있어요."
#define STRING_sell_count                       "판매 갯수"
#define STRING_item_stage_notuse                "%s(은)는 분식점 %d스테이지를 클리어한 후 사용하실 수 있습니다."
#define STRING_item_stage_use                   "%s를 사용하실 수 있습니다. %s을 장착해 보세요."
#define STRING_clear_all_stage                  "축하합니다! %s의 모든 스테이지를 클리어하셨습니다. %s(을)를 오픈할 수 있어요!"
#define STRING_clear_all_shop                   "와~ 축하합니다! 모든 가게를 클리어하셨습니다."// 다음가게를 준비중이니 기대해 주세요~~"
#define STRING_recipe_buy_warning2              "손님들이 찾는 음식이 있어요. 샵에 가서 레시피를 구매하세요."
#define STRING_awards_shop_explain              "스테이지 퍼펙트를 10번 완료하면 보상을 받을 수 있어요."
#define STRING_awards_character_explain         "단골손님을 잘 관리해야 팁을 많이 받을 수 있어요."
#define STRING_awards_recipe_0                  "%s의 레시피를 모두 보유"
#define STRING_awards_recipe_1                  "%s의 커스튬(%s)을 모두 보유"
#define STRING_awards_rank_thropy_0             "1주일 연속 1위"
#define STRING_awards_rank_thropy_1             "10회 연속 1위"
#define STRING_awards_rank_thropy_2             "급성장 1위(하루안에 친구 10명을 추월해서 1위)"
#define STRING_awards_rank_thropy_3             "단기간 급하락(하루안에 친구 10명에게 추월당함)"
#define STRING_awards_rank_stamp_0              "10,000골드 보유"
#define STRING_awards_rank_stamp_1              "100,000골드 보유"
#define STRING_awards_rank_stamp_2              "1,000,000골드 보유"
#define STRING_awards_word_complete_notyet      "해야 얻을 수 있어요."
#define STRING_awards_word_complete             "했어요."
#define STRING_awards_complete_service_first	"축하합니다. %s손님이 단골이 되었어요. 마이룸>업적에서 확인해보세요."
#define STRING_awards_complete_service          "축하합니다. %s손님 도장 받을 수 있어요. 마이룸>업적에서 확인해보세요."
#define STRING_awards_complete_rank             "축하합니다. %s 했어요. 마이룸>업적에서 확인해보세요."
#define STRING_awards_complete_shop             "축하합니다. %s 도장 하나 받을 수 있어요. 마이룸>업적에서 확인해보세요."
#define STRING_awards_complete_recipe           "축하합니다. %s 레시피 마스터 우표를 받을 수 있어요. 마이룸>업적에서 확인해보세요."
#define STRING_awards_complete_costume          "축하합니다. %s 코스튬 마스터 카드를 받을 수 있어요. 마이룸>업적에서 확인해보세요."
#define STRING_game_result_earn                 "매출"
#define STRING_game_result_tip                  "팁"
#define STRING_game_result_questbonus           "퀘스트 보너스"
#define STRING_game_result_lastbonus            "라스트 보너스"
#define STRING_game_result_cashier              "직원 팁"
#define STRING_game_result_totalgold            "총 수익"
#define STRING_game_result_heartbonus           "보너스"
#define STRING_game_result_quest_failed         "실패"
#define STRING_s_recipe                         "의 레시피"
#define STRING_rewarded                         "를 받았어요."
#define STRING_count                            "개"

#define STRING_tip_01                           "적절한 퍼즐 아이템과 슬롯 아이템의 사용으로 게임의 효율성을 높일수 있어요!"
#define STRING_tip_02                           "때와 장소에 맞는 복장은 장사에 도움이 되지요!"
#define STRING_tip_03                           "레시피를 다양하게 구매하면 매상을 더 올릴 수 있어요!"
#define STRING_tip_04                           "사이드 메뉴를 끼워 팔아 추가 이익을 얻어요!"
#define STRING_tip_05                           "블럭 아이템을 더블 탭 하면 가장 효율적인 곳에 사용돼요!"
#define STRING_tip_06                           "손님의 남은 하트 수가 많은 상태에서 음식이 나오면 추가수익을 얻을 수 있어요!"
#define STRING_tip_07                           "스테이지별 최고 점수를 달성해서 스테이지별 기록도 남기고 랭킹 점수도 올려보세요."
#define STRING_tip_08                           "유리한 스테이지에 계속 도전해서 골드도 모으고 높은 랭킹도 노려보아요~!"
#define STRING_tip_09                           "판매 연속 콤보를 3번 달성하면 선택한 손님의 기분을 가득 채워주는 캔디 아이템이 등장해요!"
#define STRING_tip_10                           "판매 연속 콤보를 5번 달성하면 선택한 음식의 판매가격을 1.5배 올려주는 MSG 아이템이 등장해요!"
#define STRING_tip_11                           "다양한 업적을 달성해서 아이템을 보상받으세요!"
#define STRING_tip_12                           "가게별 레시피를 모두 모으면 실속있는 시크릿 레시피를 살 수 있어요!"
#define STRING_tip_13                           "친구끼리 아이템을 선물해서 도와가며 가게를 운영해요!"
#define STRING_tip_14                           "매일 출석체크를 하면 다양한 선물을 드려요!"
#define STRING_tip_15                           "친구를 초대하면 다양한 선물을 받을 수 있어요!"
#define STRING_tip_16                           "슬롯 아이템 중 시계 아이템은 한 스테이지당 3번 까지만 사용할 수 있어요!"
#define STRING_tip_17                           "각 스테이지별 판매 목표매상과 퀘스트를 둘다 달성해야만 클리어 할 수 있어요!"
#define STRING_tip_18                           "스테이지모드의 결과창에서 얻은 별을 모아 크레이지 모드를 열수있어요!"
#define STRING_tip_19                           "상위 레시피를 구입하면 크레이지모드에서 더 좋은 점수를 낼 수 있어요!"
#define STRING_tip_20                           "친구들에게 개스트를 보내어 서로 골 달성을 도와요!"
#define STRING_tip_21                           "푸들캐셔를 승진시키면 더 많은 일을 도와줘요!"
#define STRING_tip_22                           "룰렛에서 레어 아이템을 얻을수 있어요!"
#define STRING_tip_23                           "시작 전 세팅메뉴에서 사이드메뉴를 업그레이드 하면 더 많은 돈을 벌 수 있어요."

#define STRING_present_explain                  "매일 매일 출석 해서\\ 다양한 선물을 받으세요~"
//#define STRING_present_explain_event            "오픈 기념 2배 보상!\\ 매일 매일 챙겨가세요~"

#define STRING_present_receive                  "가 지급 되었어요."
#define STRING_random_box                       "랜덤상자"


#define STRING_POPUP_OK                         "확인"
#define STRING_POPUP_YES                        "예"
#define STRING_POPUP_NO                         "아니오"
#define STRING_POPUP_LATER                      "나중에"
#define STRING_POPUP_SHOP                       "사러가기"
#define STRING_POPUP_BUY                        "바로구매"
#define STRING_POPUP_DONE                       "확인"
#define STRING_POPUP_OPEN                       "오픈하기"
#define STRING_POPUP_INVITE                     "친구초대"
#define STRING_POPUP_UPDATE                     "업데이트"
#define STRING_POPUP_EXIT                       "게임종료"
#define STRING_POPUP_DIRECTOPEN                 "바로열기"


#endif  // __DYCONFIG_H__