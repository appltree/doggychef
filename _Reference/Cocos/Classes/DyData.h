/*
DyData.h
Doggy chef
@initialize at 121224

@history
121224	yoonsr	initialize
*/

#ifndef __DYDATA_H__
#define __DYDATA_H__

#include "YtLib.h"
#include "DyConfig.h"
#include "platform/platform.h"
#include "DyItem.h"
#include "DyStaff.h"
#include "CCDate.h"
#include "SInt.h"

#define MAX_CHARNUM     ((unsigned char)200)
#define MAX_INTNUM      ((int)2147483647)
#define MAX_SCHARNUM    ((signed char)125)

#define DYDATA_LOCALDATA_VER        1


#define DYDATA_PRIVATEDATA_VER      5//버전 3부터 스테이지점수 나눠서 저장함

#define DYDATA_PUBLICDATA_VER       3
#define DYDATA_GUESTDAT_VER         0
#define DYDATA_INVITE_VER           0
#define DYDATA_NOTICE_DELETE_VER           0
#define DYDATA_NOTICE_CHECK_VER           0

#define PRESENT_CHECK_DAY               10

typedef enum tag_DyStageType
{
	DyStageType_Snack = 0,
	DyStageType_Hamburger,
	DyStageType_Coffee,
    DyStageType_China,
    DyStageType_Pizza,
    DyStageType_Korean,
}DyStageType;


typedef enum tag_DyStageState
{
	DyStageState_Lock,
	DyStageState_Fail,
    DyStageState_Fail_Pass,
	DyStageState_Goal,
	DyStageState_Goal_Plus,
	DyStageState_Perfect
}DyStageState;

typedef struct tag_DyStageRecord
{
	//DyStageState m_state;
	unsigned int m_point;
}DyStageRecord;


typedef enum tag_DyAwardsCheckType
{
	DyAwardsCheckType_Recipe,
	DyAwardsCheckType_Costume,
	DyAwardsCheckType_Shop,
	DyAwardsCheckType_Character,
	DyAwardsCheckType_Rank,
	DyAwardsCheckType_Gold,
}DyAwardsCheckType;

typedef unsigned char DyAwardsDataSub;



typedef struct tag_DyAwardsDataMain
{
	DyAwardsDataSub m_award_recipe[3];
	DyAwardsDataSub m_award_costume[3];
	//DyAwardsDataSub m_award_rank[4];
	DyAwardsDataSub m_award_gold[3];
	DyAwardsDataSub m_award_shop[3];
	DyAwardsDataSub m_award_service[DYDATA_CUSTOMER_DEFAULT_CNT];
}DyAwardsDataMain;


typedef struct tag_DyCostumeFunction
{
    signed char m_ice;
    signed char m_heart;
    signed char m_decay;
    signed char m_tip;
    signed char m_cooling;
}DyCostumeFunction;


//
// 바꾸면 안된다.
#define COSTUME_DEFAULT_CNT_SET     13
#define COSTUME_DEFAULT_CNT_CLOTH   14
#define COSTUME_DEFAULT_CNT_APRON   11
#define COSTUME_DEFAULT_CNT_HAT     17
#define COSTUME_DEFAULT_CNT_SCARF   12

//
// 코스튬 추가하면 아래를 증가 시킨다.
#define COSTUME_ADDED_CNT_SET     8
#define COSTUME_ADDED_CNT_CLOTH   8
#define COSTUME_ADDED_CNT_APRON   7
#define COSTUME_ADDED_CNT_HAT     8
#define COSTUME_ADDED_CNT_SCARF   6



typedef struct tag_DyCostumeStatus
{
	signed char m_indexSet;     //[0-12]
	signed char m_indexCloth;   //[0-13]
	signed char m_indexApron;   //[0-10]
	signed char m_indexHat;     //[0-16]
	signed char m_indexScarf;   //[0-11]
}DyCostumeStatus;



#pragma pack(push, 1)


typedef struct tag_DyDataLocal_00
{
    unsigned char m_version_00;
    unsigned char m_option_00;
	
    unsigned char buffer_00[498];
}DyDataLocal_00;

typedef struct tag_DyDataLocal
{
    unsigned char m_version;
    unsigned char m_option;
    
    bool m_isPromised;
    
    bool m_dataSaved;
    
    bool m_isViewDeliveryTutorial;
	
    bool m_viewPuddleStory;
    bool m_viewPuddleTutorial;
    
    bool m_isViewGrilTutorial;
    
    unsigned char buffer[492];
}DyDataLocal;


typedef struct tag_DyDataPrivate_00
{
    unsigned char m_version_00;
    
    unsigned int m_delly_00;
	unsigned int m_gold_00;
	unsigned char m_cntRunRoulette_00;
	unsigned char m_cntGuest_00;
    unsigned char m_cntInvite_00;
    unsigned char m_cntFreeRoullete_00;
    unsigned char m_cntPride_00;
    
    double m_excuteDate_00;
    signed char m_presentday_00;
    
    signed char m_staff_next_grade_remain_00;//[0~30]
    
    unsigned short m_itemslot_00;
    DyItemData m_item_00[DyItemType_Count];
    
    DyAwardsDataMain m_awards_00;
    
    unsigned short m_sellcount_00[DYDATA_CNT_RECIPE];
    unsigned char m_recipe_00[DYDATA_CNT_RECIPE/2];
    
    unsigned char m_costume_00[DYDATA_MAX_COSTUMEITEM/4];
    
    unsigned char m_character_00[8];
    signed char m_character_eat_00[8];
    
    unsigned char m_shop_status_00[SHOP_COUNT_DEFAULT];
    unsigned int m_shop_record_00[SHOP_COUNT_DEFAULT][30];
    
    bool m_rewardedReview_00;
    unsigned char m_cntCheckReview_00;
    
    unsigned char m_inviteDay_00;
    unsigned char m_cntInviteToday_00;
    
    unsigned char buffer_00[445];
    
}DyDataPrivate_00;


typedef struct tag_DyDataPrivate_02
{
    unsigned char m_version_02;
    
    unsigned char m_delly_02[4];
	unsigned char m_gold_02[4];
    
	unsigned char m_cntRunRoulette_02;
	unsigned char m_cntGuest_02;
    unsigned char m_cntInvite_02;
    unsigned char m_cntFreeRoullete_02;
    unsigned char m_cntPride_02;
    
    double m_excuteDate_02;
    signed char m_presentday_02;
    
    signed char m_staff_next_grade_remain_02;//[0~30]

    unsigned short m_itemslot_02;
    DyItemData m_item_02[DyItemType_Count];
    
    DyAwardsDataMain m_awards_02;
    
    unsigned short m_sellcount_02[DYDATA_CNT_RECIPE];
    unsigned char m_recipe_02[DYDATA_CNT_RECIPE/2];
    
    unsigned char m_costume_02[DYDATA_MAX_COSTUMEITEM/4];
    
    unsigned char m_character_02[DYDATA_CUSTOMER_DEFAULT_CNT];
    signed char m_character_eat_02[DYDATA_CUSTOMER_DEFAULT_CNT];
    
    unsigned char m_shop_status_02[SHOP_COUNT_DEFAULT];
    unsigned int m_shop_record_02[SHOP_COUNT_DEFAULT][30];
    
    bool m_rewardedReview_02;
    unsigned char m_cntCheckReview_02;
    
    unsigned char m_inviteDay_02;
    unsigned char m_cntInviteToday_02;
    
    unsigned char m_machineGrade_02[SHOP_COUNT_DEFAULT];
    
    bool m_viewNewTutorial_02;
    
    unsigned char m_cntRoulletteDay_02;
    unsigned char m_cntRoulletteToday_02;

    unsigned char m_costume_ex_02[32/4];
    
    unsigned char m_shop_status_added_02[3];
    unsigned int m_shop_record_added_02[3][30];

    unsigned short m_sellcount_added_02[12];//m_sellcount의 2개와 11개 사용 = 중국집은 총 13개
    
    /*!!! 1.1.02 버그로 인해서 초기화 한번 해줘야한다.초기화 시점은 중국집 열렸을 때가 적당할 것 같다.*/
    unsigned char m_recipe_added_02[12/2];
    
    unsigned char m_machineGrade_added_02[3];
    
    unsigned char m_character_added_02[6];
    signed char m_character_eat_added_02[6];

    DyAwardsDataSub m_award_recipe_added_02[3];
	DyAwardsDataSub m_award_costume_added_02[3];
	DyAwardsDataSub m_award_shop_added_02[3];
	DyAwardsDataSub m_award_service_added_02[6];
    
    unsigned char buffer_02[8];
    
}DyDataPrivate_02;



typedef struct tag_DyDataPrivate
{
    //
    // 고정자리
    unsigned char m_version;
    
    unsigned char m_delly[4];
	unsigned char m_gold[4];
    
	unsigned char m_cntRunRoulette;
	unsigned char m_cntGuest;
    unsigned char m_cntInvite;
    unsigned char m_cntFreeRoullete;
    unsigned char m_cntPride;
    
    double m_excuteDate;
    signed char m_presentday;
    
    signed char m_staff_next_grade_remain;//[0~30]
    
    unsigned short m_itemslot;
    DyItemData m_item[DyItemType_Count];
    //고정자리
    //
    
    
    bool m_rewardedReview;
    unsigned char m_cntCheckReview;
 
    unsigned char m_inviteDay;
    unsigned char m_cntInviteToday;
    
    bool m_viewNewTutorial;
    
    unsigned char m_cntRoulletteDay;
    unsigned char m_cntRoulletteToday;
    
    //
    // 캐릭터
    unsigned char m_character[DYDATA_CUSTOMER_DEFAULT_CNT+6];
    signed char m_character_eat[DYDATA_CUSTOMER_DEFAULT_CNT+6];
    unsigned char buffer_characer[40];
    
    
    char m_run_cnt;
    char m_event_type;
    double m_check_event_date;
    signed char m_current_staff;
    signed char m_staff_ext_grade[4];
    signed char m_staff_ext_next_grade_remain[4];//[0~30]
    
    unsigned char buffer_parameter[41];
    
    //
    // 레시피
    unsigned short m_sellcount[DYDATA_CNT_RECIPE+12+30];
    unsigned char m_recipe[(DYDATA_CNT_RECIPE+12+30)/2];
    unsigned char buffer_recipe[100];
    
    //
    // 코스튬
    unsigned char m_costume[(DYDATA_MAX_COSTUMEITEM+DYDATA_MAX_ADD_COSTUMEITEM_EX)/4];
    unsigned char buffer_costume[92];
    //
    // 샵
    unsigned char m_shop_status[SHOP_COUNT_DEFAULT+3];
    unsigned char m_machineGrade[SHOP_COUNT_DEFAULT+3];
    unsigned char buffer_shop[100];
    
    //
    // 업적
    DyAwardsDataSub m_award_recipe[3+3];
	DyAwardsDataSub m_award_costume[3+3];
	DyAwardsDataSub m_award_shop[3+3];
	DyAwardsDataSub m_award_service[DYDATA_CUSTOMER_DEFAULT_CNT+6];
    DyAwardsDataSub m_award_gold[3];
    //DyAwardsDataSub m_award_rank[4];
	unsigned char buffer_award[100];
    
    //unsigned char m_statusStage[6][16];
    //unsigned char buffer[57];
    
    unsigned int m_stage_status[6*6];
    unsigned char buffer[9];
}DyDataPrivate;


typedef struct tag_DyDataPublic_00
{
    unsigned char m_version_00;
    unsigned short m_staff_00;
    DyCostumeStatus m_costume_status_00;
    DyAwardsDataMain m_awards_00;
    DyItemData m_item_00[DyItemType_Count];
    unsigned char m_stage_status_00[SHOP_COUNT_DEFAULT*30/2];
    unsigned char m_shop_status_00[SHOP_COUNT_DEFAULT];
    unsigned int m_stageRecord_00;
    unsigned char buffer_00[15];
}DyDataPublic_00;


typedef struct tag_DyDataPublic
{
    unsigned char m_version;
    signed char m_staff_grade;//[0~5]
    
    unsigned int m_stageRecord;
    
    DyCostumeStatus m_costume_status;
    
    unsigned int m_stage_status[SHOP_COUNT_DEFAULT*3];
    
    DyCostumeStatus m_costume_status_ex; // 임시변수임
    
    unsigned int m_stage_status_added[3*3];
    
    int m_max_shop;
    int m_max_stage;
    
    unsigned char buffer[4];//41
    
}DyDataPublic;


#pragma pack(pop)


class DyDataManager : public CCObject
{
private:
    void patchCostume(DyDataPublic* dataPublic);
    
public:
    char* m_base64Temp;
    
	DyDataLocal m_local;
    
    DyDataPrivate m_private;
    DyDataPublic m_public;
    
    CCArray* m_arrShopPoint_forMigration;
    
    bool m_is_need_privatedata_migration;
    bool m_is_need_checkevent;
    
    //bool m_is_need_reset;
	
    std::string m_reset_last_date;
	std::string m_reset_private_data;
	std::string m_reset_public_data;
    
    //bool m_isGuestLogin;
    
    unsigned int m_gum;
    unsigned int m_rewardedGum;
    
    unsigned int m_max_gum;
    unsigned int m_rechargeable_gum;
    
    CCDate m_heart_regen_starts_at;
    int m_heart_regen_interval;//(초)
    
    unsigned int m_crazyBestRecord;
    unsigned int m_crazyRecord;
    
    bool m_limitSanta_Open;
    
    int m_cntRoulletPerDay;
    int m_cntPovabilityHero;
    
    bool m_message_blocked;
    
    CCDate m_time_server;
    CCDate m_time_diff;
    
    CCDate getCurrentDate();
    
    CCDate m_time_refresh;
    std::string getRefreshRemainDate();
    
    int m_message_interval;
    
    int m_invitation_interval;
    
    DyCostumeFunction m_custume_function;
    
    int m_min_version;
    int m_current_version;
    
    std::string m_main_url;
    std::string m_notice;
    
    int m_display_ad;
    int m_present_award;
    
    int m_gum_send_award;
    
    bool m_delly_event;
    
    std::string m_link;
    
    std::string m_nickname;
    std::string m_user_id;
    std::string m_profile_image_url;
    
    std::string m_platform;
    
    std::string m_greeting;
    
	DyDataManager();
	~DyDataManager();

	static DyDataManager* sharedDataManager();

    void data_private_migration_2_4(DyDataPrivate* dataPrivate);

    void saveScore(std::string data);
    
	void dataReset_private(DyDataPrivate* dataPrivate);
    void dataReset_public(DyDataPublic* dataPublic, bool isMine);
    void dataReset_local(DyDataLocal* dataLocal);
   
    void reset();
    
	void dataSave_local();
    std::string dataSave_private();
    std::string dataSave_public();
    void dataSave();
    
	bool dataLoad_local();
    bool dataLoad_private(DyDataPrivate* dataPrivate, std::string data);
    bool dataLoad_public(DyDataPublic* dataPublic, std::string data, bool isMine);
    
    //void dataSave_public_stage(DyDataPublic* dataPublic);

	void onPopupClicked(cocos2d::CCObject * pSender);
	void checkAwards(DyAwardsCheckType checktype, bool isSaveData);
    void initRewardData(int type, int index);
    
    int checkPresent();
    void checkEvent();
    
    
    bool recipe_isAvailable(int index);
    bool recipe_isSawInShop(int index);
    bool recipe_isSawInGame(int index);
    
    void recipe_setAvailable(int index, bool value);
    void recipe_setSawInShop(int index, bool value);
    void recipe_setSawInGame(int index, bool value);
    
  
    bool costume_get_hasitem(int index);
    bool costume_get_saw(int index);
    
    void costume_set_hasitem(int index, bool value);
    void costume_set_saw(int index, bool value);
    
    int item_slot_get_itemtype(int index);
    void item_slot_set_itemtype(int index, int itemtype);
    
    
    void shop_set_viewending(int shop, int isView);
    bool shop_get_viewending(int shop);
    
    void shop_set_crazyopen(int shop, int isOpen);
    bool shop_get_crazyopen(int shop);
        

    void shop_set_record(int shop, int stage, int point);
    int shop_get_record(int shop, int stage);
    
  
    void character_set_see(int character, bool see);
    bool character_get_see(int charcter);
    
    void character_set_heart(int character, int heart);
    int character_get_heart(int character);
    
    
    int character_get_eat(int character);
    void character_eat_inc(int character);
    void character_eat_dec(int character);
    
    void addRoullette(int count);

    //
    // 0 recipe
    // 1 costume
    // 2 gold
    // 3 shop
    // 4 service
    void awards_set_complete(int type, int index, bool complete);
    bool awards_get_complete(int type, int index);
    
    void awards_set_isSawAward(int type, int index, bool isSaw);
    bool awards_get_isSawAward(int type, int index);
    
    void awards_set_complete_count(int type, int index, int count);
    int awards_get_complete_count(int type, int index);
    
    void awards_set_reward_count(int type, int index, int count);
    int awards_get_reward_count(int type, int index);
    
    
    
    DyCostumeStatus getCostumeStatus(DyCostumeStatus* status, DyCostumeStatus* status_ex);
    void setCostumeStatus(DyCostumeStatus costumeStatus, DyCostumeStatus* status, DyCostumeStatus* status_ex);
    
};


#endif  // __DYDATA_H__