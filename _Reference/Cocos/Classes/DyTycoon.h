/*
DyTycoon.h
Doggy chef
@initialize at 121008

@history
121008	yoonsr	initialize
*/


#ifndef __DYTYCOON_H__
#define __DYTYCOON_H__

#include "DyConfig.h"

#include "YtObject.h"
#include "DyCharacter.h"

#include "YtLoader.h"

#include "YtCommand.h"
#include "DyLayerGameBoard.h"
#include "DyItem.h"
#include "DyData.h"
#include "DyRecipe.h"
#include "DyLayerGameResult.h"
#include "DyRank.h"

//typedef enum tag_DyTycoonMode
//{
//    DyTycoonMode_Story,
//    DyTycoonMode_Crazy,
//}DyTycoonMode;

typedef enum tag_DyTycoonStatus
{
	DyTycoonStatus_Idle = 0,
	DyTycoonStatus_Do,
	DyTycoonStatus_Paused,
}DyTycoonStatus;

typedef enum tag_DyQuestType
{
	DyQuestType_All,    // 하나도 빠짐없이 성공해야한다.
	DyQuestType_Number, // 숫자만큼 달성하면된다.
    DyQuestType_NumberLimit, // 0이되면 실패다
    DyQuestType_Ignore, // 아무것도 아니니 무시해도 좋다.
}DyQuestType;

typedef enum tag_DyQuestStatus
{
	DyQuestStatus_Progress,
	DyQuestStatus_Sucess,
	DyQuestStatus_Failure,
}DyQuestStatus;


class DyQuest : public CCObject
{
public:
	std::string m_questMainTag;
	std::string m_questSubTag;
	DyQuestType m_questtype;
	DyQuestStatus m_queststatus;
    bool m_questNotStaff;

	int m_countQuest;			//DyQuestType_All then 10000
	int m_countComplete;		//DyQuestType_All then ignore
};


class DyTycoonManager : public CCObject
{
public:
    bool m_isCrazyMode;
    bool m_failWithQuest;

    int m_customerSlot[3];
    
    //
    // 스테이지 값
    YtLibSndBGM m_bgmIdx;
    
	int m_indexRecipe;
    
    int m_nottimesnd;
    
	CCLayer* m_layer;
	DyBoard* m_board;
	DyTycoonStatus m_status;
	bool m_gameStarted;
	bool m_gameStartForce;
    bool m_gameSetting;

	YtAniObject *m_indicatorDish[3];
	YtAniObject *m_indicatorCharacter[3];

	YtAniObject *m_openDoggy;
	
	bool m_questbar_show;
	YtAniObject *m_questbar;

	CCSprite* m_sprDonIcon[3];
	CCLabelTTF* m_labelDon[3];
		
	CCLabelBMFont* m_labelPrice[3];

	int m_max_block_count;
    float m_block_weight;

	bool m_isReplay;
    
    bool m_checkLastOpportunity;
    bool m_pauseCheckOpportunity;
	
    //
    // Crazy mode
    int m_recipe_add_cnt;
    int m_recipe_addable_cnt;
    
    int m_crazy_time_add;
    int m_crazy_time;
    int m_crazy_status;
    int m_crazyRemainHeart;
    int m_crazyHeartVel;
    CCLabelBMFont* m_labelCrazyScore;
    
    CCSprite* m_sprPanHeart;
    CCSprite* m_sprCrazyHeart[3];
    CCLabelBMFont* m_labelCrazyTimeM;
    CCLabelBMFont* m_labelCrazyTimeS;
    CCLabelTTF* m_labelCrazyGold;
    CCLabelTTF* m_labelCrazyDelly;
    void addCrazyHeart(int count);
    void addCrazyPoint(int point, int type);
    int m_crazy_point_time;
    int m_crazy_point_removeblock;
    int m_crazy_point_gold;
    int m_crazy_point_combo;
    int m_crazy_point;
    
    int m_crazy_createGold;
    int m_crazy_createDelly;
    int m_crazy_createHeart;
    
    int m_crazy_bonusGold;
    int m_crazy_bonusDelly;
    
    float m_velItemSlot[3];
    float m_dtItemSlot[3];
    
    bool m_enableSlotItem[3];
    CCSprite* m_sprProgItemSlotBg[3];
    CCProgressTimer* m_progItemSlot[3];
    CCLabelTTF* m_labelItemSlot[3];
    
    void slotItemUse(DyItemType itemType);
    void slotItemEnable(int indexSlot, bool isEnable);
	//
	// Quest
    bool m_questItemBasic[3];// 슬롯아이템모두 사용 퀘스트를 위한 저장소
	DyQuest* m_quest;
	void questProcess(const char* tag, const char* subTag, int count);
	bool showQuestMessage(bool isAllCheck);
   
    //
    // 점수
	int m_stage_point;
	int m_bouns_heart_point;
    int m_combo_point;
    int m_star_point;

    bool m_newscore;
    
    //
    // 골드
    int m_current_gold;
	int m_sold_gold;
	int m_tip_gold;
	int m_cashier_gold;
    int m_bonustime_gold;
	bool m_questProcessComplete;
    bool m_heartComplete;
    
	//
	// gold
	CCProgressTimer *m_progGold;
	int m_goal_gold;
	int m_perfect_gold;
	int m_questbonus_gold;

	//
	// processing gold
	int m_earnGold;
	
	//
	// heart
	CCProgressTimer *m_progHeart;
	int m_bonus_heart_cnt;
	int m_bouns_heart;
	int m_current_heart;
    
    
    int m_trouble_table_cnt;
    int m_trouble_table_idx[2];
    
    int m_heart_minus;
    int m_fix_block_cnt;
    
    int m_double_probability;
    
    int m_association[3];
    //int m_material_probability[DYRECIPE_MATERIAL_MAX_CNT];
    
	int m_combo_heart;
    int m_customer_happy_cnt;

	//
	// side dish
	float m_dtSide;
    float m_dtSideReady;
    
	//float m_timeFullSide;
	bool m_isCompleteSide;
	bool m_isSelectedSide;
	//int m_priceSide;
    
    int m_last_candy;
    int m_last_msg;
    int m_last_halo;
    int m_last_cross;
    int m_last_fire;
    

	YtAniObject* m_aniCashier;
	CCSprite* m_sprCashierDesk; //분식점만 캐셔없을때 뒷배경이 있다.

	//
	// timer
	CCProgressTimer *m_progTime;
	float m_time_limit;
	float m_dtOpenTime;
	bool m_isStartWarning;
	void updateTimer(float dt);
	bool m_closeTime;

    bool m_checkLastPang;
    bool m_checkLastPangEvent;

    bool m_lockInput;
    
    DyCostumeFunction m_functionGame;
    
    

	YtCommandManager* m_commandManager;
	
    int m_crazy_create_gold_snack;
    int m_crazy_create_gold_hamburger;
    int m_crazy_create_gold_coffee;
    int m_crazy_create_start_gold;
    
    int m_crazy_create_delly_snack_start;
    int m_crazy_create_delly_snack_interval;
    
    int m_crazy_create_delly_hamburger_start;
    int m_crazy_create_delly_hamburger_interval;
    
    int m_crazy_create_delly_coffee_start;
    int m_crazy_create_delly_coffee_interval;
    
    int m_crazy_create_delly_first_pro;
    int m_crazy_create_delly_second_pro;
    int m_crazy_create_delly_etc_pro;
    
    int m_crazy_create_gold_china;
    int m_crazy_create_delly_china_start;
    int m_crazy_create_delly_china_interval;

    
	void updateIndicator();

    //
	// command
	CCMenu* m_menuItem;
	CCMenu* m_menuPause;
	CCMenu* m_menuPauseMenu;
	CCMenu* m_menuPauseMenu2;
	CCMenu* m_menuQuestPopup;
	
	//
	// 
	std::string m_urlCounter;
    
    std::string m_urlBag;
    std::string m_urlBag_disable;
    

	DyTycoonManager();
	~DyTycoonManager();

    void updateSide(float dt);
    
    
    bool m_changeOrder;
    DyRank* m_currentTarget;
    float m_dtCrazyOrder;
    void updateCrazyOrder(float dt);
    
    void initGame();
	void startGameReady();
    void startGameReal();
    
    void highlightStartButton();
    
    void startGameReal_Real(float dt);
    
    
	void sellRecipe(DyCustomer* customer, bool byChshier);
	void update(float dt);
	void startGame();
	void endGame();
	void pauseGame();
	void resumeGame();
	void ccTouchesBegan(CCTouch* touch);
    
    void clearBlocksBeforeEnd();

	void setLayerAndBoard(CCLayer* layer, DyBoard* board);
	void setOrderRecipe(DyCustomer* customer);

	void addSide(DyCustomer* customer, bool byManager);
	void addMaterial(int id, CCPoint position);
	void addSpeicialMaterial(int idCustomer, TmBlockType blockType, CCPoint position);
	void addTime();
	void setFever(bool isFever);

	void endAnimationAdd(CCNode* node);
	void endAnimationSpecialAdd(CCNode* node);
	void endAnimationGold(CCNode* node);
	void endAnimationCashier(CCNode* node);
	void endAnimationSide(CCNode* node);	
	void endAnimationTime(CCNode* node);
	void endAnimationTimeOut(CCNode* node);
	void addHeart(int cntHeart, bool isAnimate);
    
    void endAnimationGoldCrazy(CCNode* node);
    void endAnimationDellyCrazy(CCNode* node);
    void endAnimationHeartCrazy(CCNode* node);

	void destoryGameObject();
	void addHeartToAllCustomer();
    void addHeart(int index);

	void cbCandyItemRecover(float dt);
    
    void startSideServe(CCNode* node);

    void soundfood00(float dt);
    void soundfood01(float dt);
    void soundfood02(float dt);
    void soundfood03(float dt);
    void soundfood04(float dt);
    void soundfood05(float dt);
    void soundfood06(float dt);
    void soundfood07(float dt);
    void soundfood08(float dt);
    void soundfood09(float dt);
    void soundfood10(float dt);
    
    void soundfood11(float dt);
    void soundfood12(float dt);
    void soundfood13(float dt);
    
    void checkGameEnd();
    
    void checkLastPangEnd(float dt);
    void startLastBonusEvent();
    void endAnimationLastBonusEvent(CCNode* node);
    
    //
    // 0:touch, 1:by cashier, 2:by bonus
    void earnGold(CCPoint posGold, int customer_pay, int customer_tip, int cashier_pay, int type);
   
    void displayEarnGold(float delay, CCPoint pos, int mainGold, int subGold);
    CCLabelTTF* getDisplayEarnGoldLabel(const char* text, float fontSize, CCPoint pos, float delay);

    void sideServeToCustomer(DyCustomer* customer, bool byManager);
    void earnGoldCrazy(CCPoint posGold, int gold);
    void earnDellyCrazy(CCPoint posGold, int delly);
  
    void earnHeartCrazy(CCPoint posHeart);
    
    void endEffect(CCNode* node);
    
    
    void setRankPopup();
    void setRankPopupOver(DyRank* rankTarget);
    
    
    
    void onPopupClicked(cocos2d::CCObject * pSender);
    
	static DyTycoonManager* sharedTycoonManager();
};


#endif  // __DYTYCOON_H__