/*
YtLib.h
YongTrim Lib
@initialize at 120907

@history
130313  yoonsr  사운드 정리
120907	yoonsr	initialize
*/


#ifndef __YTLIB_H__
#define __YTLIB_H__


#include "cocos2d.h"
#include "cocos-ext.h"
#include "YtPlatform.h"

using namespace cocos2d;
using namespace cocos2d::extension;

#ifdef WIN32
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#define YYASSERT assert
#else
#define YYASSERT
#endif

typedef enum tag_YtLibSndEff
{
    YtLibSndEff_BLOCK_00,
    YtLibSndEff_BLOCK_01,
    YtLibSndEff_BLOCK_02,
    YtLibSndEff_BLOCK_03,
    YtLibSndEff_BLOCK_04,
    YtLibSndEff_BLOCK_05,
    YtLibSndEff_BLOCK_06,
    YtLibSndEff_BLOCK_07,
    YtLibSndEff_BLOCK_08,
    YtLibSndEff_BLOCK_09,
    YtLibSndEff_BLOCK_10,
    YtLibSndEff_BLOCK_11,
    YtLibSndEff_BLOCK_12,
    YtLibSndEff_BLOCK_13,
    YtLibSndEff_BLOCK_14,
    YtLibSndEff_BLOCK_15,
    
    YtLibSndEff_BLOCK_16,
    YtLibSndEff_BLOCK_17,
    
    YtLibSndEff_BLOCK_18,
    
    YtLibSndEff_BLOCK_BOILED,
    
    
	YtLibSndEff_APPEAR_SPECIAL_BLOCK,       // 특수 블럭 출현
    YtLibSndEff_BLOCK_CLOSS,                // 크로스 아이템
    YtLibSndEff_BLOCK_DECAY,                // 썩는 아이템 생성
    YtLibSndEff_BLOCK_DECAY_DESTROY,        // 썩는 아이템 소멸
    
    YtLibSndEff_FOOD_MSG,                   // MSG
    YtLibSndEff_CANDY,                      // 캔디
    YtLibSndEff_HINT,                 // 힌트
    YtLibSndEff_ADDTIME,                 // 시간더하기 
    YtLibSndEff_REFRESH,                 // 새로고침
    
    YtLibSndEff_Quest_Step_Success,       // 미션 단계별 성공
    YtLibSndEff_Quest_Success,            // 미션 완전 성공
    YtLibSndEff_Quest_Failure,            // 미션 실패
    
    YtLibSndEff_BUTTON_CLOSE,               // 닫기 버튼
    YtLibSndEff_BUTTON_EAT,               // 개소리 버튼
    YtLibSndEff_CURTAIN_SPREAD,               // 커튼 펼치기
    YtLibSndEff_CURTAIN_FOLD,                 // 커튼 접기
    
    YtLibSndEff_WOW,                            // 환호성
    YtLibSndEff_FireFlower,                     // 불꽃
    YtLibSndEff_Cry,                            // 우는 소리 
    
    YtLibSndEff_ANGRY2,                      // 화내는소리2
    YtLibSndEff_Combo_Normal,               // 5콤보이상
    YtLibSndEff_Combo_High,               // 10콤보이상
    
    
    YtLibSndEff_STAR_PANG0,// 팡1
	YtLibSndEff_STAR_PANG1,// 팡2
	YtLibSndEff_STAR_PANG2,// 팡3
    
	YtLibSndEff_BLOCK_MOVE,                 // 움직이는 소리
	YtLibSndEff_BLOCK_MOVEBACK,             // 움직이는 소리 돌아오는 소리
    YtLibSndEff_BLOCK_FIRE,                 // 불 소리
	YtLibSndEff_CHARACTER_ENTER,            // 캐릭터 들어 오는 소리
	YtLibSndEff_MONEY_REGISTER,             // 유저가 돈받는 소리
	YtLibSndEff_MONEY_REGISTER_BYCHASIER,   // 캐셔가 돈받는 소리
   	YtLibSndEff_SIDE_READY,                 // 사이드 완료
    YtLibSndEff_FOOD_COMPLETE,              // 음식 완성
	
    YtLibSndEff_NOTTIME,                    // 시간 촉박
	YtLibSndEff_TIMEOUT,                    // CLOSE 이벤트
    YtLibSndEff_CLOSE,                      // 문닫는 소리
	
    YtLibSndEff_EAT,                        // 먹는소리1
	YtLibSndEff_EAT2,                       // 먹는소리2
	YtLibSndEff_ANGRY,                      // 화내는소리
    
    YtLibSndEff_BLOCK_ICE_BREAK,            // 얼음 깨지는소리
    YtLibSndEff_THROWBOMB,                  // 폭탄 날라가는 소리
	YtLibSndEff_THROWBOMB_ANDBOMB,          // 폭탄 날라간다음에 터지는 소리
    YtLibSndEff_BLOCK_HALO,                 // 후광 아이템 터지는 소리

    YtLibSndEff_CLICK,                      // 게임중 아이템버튼이나 사이드 눌렀을 때
	YtLibSndEff_DOGGY_SHOW,                 // 게임 시작할때 도기 뿅 나오는 소리
	YtLibSndEff_BUTTON_CLICK,               // UI 기본 버튼 눌렀을때
    YtLibSndEff_GIVE_SIDE,                  // 싸이드 메뉴 캐릭터에 도착
    YtLibSndEff_POINT_CALC,//점수계산
    YtLibSndEff_GUM_DROP,//뼈다구 붙었을때
    YtLibSndEff_Goal,//골 일때
    YtLibSndEff_BLOCK_PANG,//블럭 터졌을 때
    YtLibSndEff_GUM_FALL,// 껌떨어지는 소리
    
    YtLibSndEff_BONUS_TIME,//보너스타임
	YtLibSndEff_AWARD_COMPLETE,// awards 달성
    
    YtLibSndEff_AWARD_DOJANG,// 도장받을 때
    YtLibSndEff_AWARD_SHOWPICTURE,//사진 출현할때
    
    YtLibSndEff_ROULLETTE_RUN,//룰렛 돌리기
    YtLibSndEff_ROULLETTE_STOP,//룰렛 멈추기
    
    YtLibSndEff_DELIVERY_BIKE,      //배달
    YtLibSndEff_DELIVERY_BAG,       //철가방
    YtLibSndEff_DELIVERY_CALL,      //전화소리
    
    YtLibSndEff_DELIVERY_ACCEPT,      //전화받는소리
    
    YtLibSndEff_BLOCK_FIX_BREAK,      //노끈 풀리는 소리
    
    
    YtLibSndEff_CNT
}YtLibSndEff;

typedef enum tag_YtLibSndBGM
{
    YtLibSndBGM_Game_0,
    YtLibSndBGM_Game_1,
    YtLibSndBGM_Game_2,
    YtLibSndBGM_Game_3,
    YtLibSndBGM_Game_4,
    YtLibSndBGM_Title,
    YtLibSndBGM_Story,
    YtLibSndBGM_Intro,
    YtLibSndBGM_Puddle,
	YYEff_BGM_Cnt,
}YtLibSndBGM;


//typedef struct tag_YtOption
//{
//	bool m_bgm;
//	bool m_effect;
//    bool m_push;
//    bool m_viewIntro;
//    
//}YtOption;


#define YtOption_set_bgm(tgt, arg)      (tgt) = ((tgt)&0b11111110 | ((arg)&0b1))
#define YtOption_get_bgm(tgt)           (((tgt))&0b1)

#define YtOption_set_effect(tgt, arg)   (tgt) = ((tgt)&0b11111101 | (((arg)&0b1)<<1))
#define YtOption_get_effect(tgt)        (((tgt>>1))&0b1)


#define YtOption_set_push(tgt, arg)   (tgt) = ((tgt)&0b11111011 | (((arg)&0b1)<<2))
#define YtOption_get_push(tgt)        (((tgt>>2))&0b1)

#define YtOption_set_viewintro(tgt, arg)   (tgt) = ((tgt)&0b11110111 | (((arg)&0b1)<<3))
#define YtOption_get_viewintro(tgt)        (((tgt>>3))&0b1)

typedef void (*YtLib_BatteryCallFunc)(int level);

class YtLibSndCmd : public CCObject
{
public:
	YtLibSndEff m_sndeff;
	float m_time;
};

class YtLib : public CCObject
{
public:
	YtLib();
	~YtLib();
	char m_strTemp[512];
	char m_strTemp2[256];
	char m_strTemp3[256];
	char m_strTemp4[256];

	//unsigned char m_option;

	void setWinSize(CCSize size);
	CCSize m_sizeWin;
	float m_scale;
	CCSize m_gapWin;

	int m_currentBGM;
	void playBGM(YtLibSndBGM sndbgm);
	void stopBGM();
	void pauseBGM();
	void resumeBGM();

	void stopEffect(unsigned int id);
	int playEffect(YtLibSndEff sndeff);
	void preloadEffect();
    
    CCArray* m_arrSound;
	void update(float dt);

	static YtLib* sharedLib();

	YtLib_BatteryCallFunc m_batteryCallFunc;
	void setBatteryCallFunc(YtLib_BatteryCallFunc callFunc);
    
    CCSprite* createSpriteFromFileSystem(const char* strFileName);
    
    CCSprite* createSpriteFromData(const char* data, int size, CCImage::EImageFormat format, const char* tag);
    
	static void getWritableFilePath(const char* filename, char* filepath);
	static bool fileExist(const char* filename);
	static int fileRead(const char* filename, char* buffer, int size);
	static int fileWrite(const char* filename, char* buffer, int size);
	static int fileGetSize(const char* filename);
    static void fileDelete(const char* filename);
	static CCMenuItemSprite* createButton(const char *normalImage, const char *selectedImage, CCObject* target, SEL_MenuHandler selector);

	static void setMoneyStyle(char* strTarget, long long money);

	static int arrangeNodesH(CCPoint position, CCPoint anchorPoint, float gapWidth, CCNode* node, ...);

	static void getCurrentTime(int* hour, int* minute, int* second);
    static int getCurrentDay();
	static long long getCurrentSecond();
    
    static std::string getMultilineText(const char* strValue);
    
    static void app_end();
    
    void playTitleBGM(float dt);

    
    //static float getCurrentMilliSecond();
};

extern int g_YtLib_currentBatteryLevel;


int base64_encode(char *text, int numBytes, char **encodedText);
int base64_decode(char *text, unsigned char *dst, int numBytes );

#endif  // __YTLIB_H__