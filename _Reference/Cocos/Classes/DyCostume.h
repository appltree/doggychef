/*
DyCostume.h
Doggy chef
@initialize at 121208

@history
121208	yoonsr	initialize
*/

#ifndef __DYCOSTUME_H__
#define __DYCOSTUME_H__

#include "YtLib.h"
#include "DyConfig.h"

#include "YtAniObject.h"
#include "DyData.h"

#define COSTUME_UI_TAG_MAIN						1000
#define COSTUME_UI_TAG_CAFE						1100
#define COSTUME_UI_TAG_CAFE2					1101

#define COSTUME_UI_TAG_LEG_LEFT					1200
#define COSTUME_UI_TAG_LEG_RIGHT				1201
#define COSTUME_UI_TAG_LEG_OUT_RIGHT			1202
#define COSTUME_UI_TAG_ARM_LEFT					1300
#define COSTUME_UI_TAG_ARM_RIGHT				1301

#define COSTUME_UI_TAG_ARM_OUT_RIGHT			1302

#define COSTUME_UI_TAG_BODY						1400
#define COSTUME_UI_TAG_BODY_BACK				1401    // 큐피드 화살통

#define COSTUME_UI_TAG_APRON					1500
#define COSTUME_UI_TAG_SCARP					1600
#define COSTUME_UI_TAG_SCARP_BACK				1601    // 큐피드 날개

#define COSTUME_UI_TAG_FACE						1700
#define COSTUME_UI_TAG_HAT						1701
#define COSTUME_UI_TAG_TUL                      1702

#define COSTUME_UI_TAG_HAT_BACK                 1703    //19
#define COSTUME_UI_TAG_HAT_BACK2                 1704    //24장금이 

#define COSTUME_UI_TAG_MAP_FACE					1800
#define CPSTUME_UI_TAG_MAP_HAT					1801
#define CPSTUME_UI_TAG_MAP_TUL					1802

#define CPSTUME_UI_TAG_MAP_HAT_BACK				1803    //19
#define CPSTUME_UI_TAG_MAP_HAT_BACK2				1804    //24장금이


#define COSTUME_UI_TAG_OPEN_GLOW                2000


typedef enum tag_DyCosumeType
{
	DyCosumeType_Set = 0,
	DyCosumeType_Cloth,
	DyCosumeType_Hat,
	DyCosumeType_Scarf,
	DyCosumeType_Apron,	
}DyCosumeType;

typedef enum tag_DyCostumeLockType
{
    DyCostumeLockType_NoneOrShop,
    DyCostumeLockType_Gold,
    DyCostumeLockType_Crazy,
    DyCostumeLockType_Roulette,
    
    DyCostumeLockType_LimitSanta,
    DyCostumeLockType_LimitInvite,
    DyCostumeLockType_LimitCupid,
    
}DyCostumeLockType;

class DyCostume : public CCObject
{
public:
	int m_no;
	int m_id;
	std::string m_tag;
	std::string m_name;
	std::string m_resultImage;

	std::list<std::string> m_arrInfo;  

	DyCostumeFunction m_function;
	//std::string m_function_explain;
    
	DyCostumeLockType m_condition_type;
    int m_condition_value;
    std::string m_condition_string;
	int m_price_gold;
	int m_price_delly;
    
    int m_function_shop;
    
	bool m_has;

	DyCosumeType m_cosumeType;

	DyCostume();
	~DyCostume();
};


class DyCostumeManager : public CCObject
{
private:
	void _setCostume2(YtAniObject* doggy, std::string tag);
	void _setCostume(YtAniObject* doggy, DyCosumeType type, int index);
    
    void initFunction(DyCostumeFunction* function);
    void addFunction(DyCostumeFunction* functionTarget, DyCostumeFunction functionSource);

public:
	bool m_hasNeck;
	bool m_hasScarf06;

	DyCostumeManager();
	~DyCostumeManager();

	CCArray* m_arrCostumes;

	DyCostume* getCostume(std::string tag);

	bool buyedCostume(DyCostume* costume);

	void setCostume(YtAniObject* doggy, DyCostumeStatus* status, DyCostumeFunction* function);
	void clearCostumeStatus(DyCostumeStatus* status);
	void setCostumeStatus(DyCostumeStatus* status, std::string tag);

	//std::string getFunctionExplain(DyCostumeFunction function);

    void setCostumeGameFunction(DyCostumeFunction* function);
    
	static DyCostumeManager* sharedCostumeManager();

    void reloadCostume();
    
	void dataSave();
    
    void log(DyCostumeFunction function);
    
    
    static void setDefaultCostumeData(DyCostumeStatus* status, DyCostumeStatus* status_ex);
    
    
    bool hasCostume(std::string tag);
};


#endif  // __DYCOSTUME_H__