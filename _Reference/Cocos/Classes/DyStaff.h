/*
DyStaff.h
Doggy chef
@initialize at 130516

@history
130516	yoonsr	initialize
*/

#ifndef __DYSTAFF_H__
#define __DYSTAFF_H__

#include "YtLib.h"
#include "DyLoader.h"
#include "DyConfig.h"
#include "YtAniObject.h"


//typedef struct tag_DyStaffData
//{
//    //
//    // -1:고용못함
//    // 0~2:캐셔
//    // 3~5:매니져
//    signed char m_grade;//[0~5]
//    signed char m_next_grade_remain;//[0~30]
//}DyStaffData;
//

#define STAFF_CNT           2


class DyStaffManager : public CCObject
{
public:
    
    std::string m_reason;
    
    YtObjectGroup* m_staff_list;
    
    YtObjectGroup* m_staff[STAFF_CNT];
    
    bool m_isHire;
    
    bool m_isIdle;
    
    bool hasStaff();
    void setHire(bool hire);
    void setIdle(bool idle, std::string strReason);
    bool isIdle();
    
    bool isHire();
    
    int getPay();
    
    void hire();
    
    float getProfitShare();
   
    
    void hireSuccess();
    
	DyStaffManager();
	~DyStaffManager();

	static DyStaffManager* sharedStaffManager();

    
	cc_timeval m_timeWork;

    std::string getReason();
	void dataSave();
    
    bool processCount(float dt);
    bool processSideServe(float dt);
    bool processCandy();
    
    CCSprite* getStaffIcon(int no, int grade);
    CCSprite* getImage(int no);
    
    CCSprite* getImageWithGrade(int no, int grade);
    
    
    bool isAvailable(int no);
    bool isBuyable(int no);
    
    int getGrade(int no);
    int getGradeCurrent();
    
    
    int getRemainCount(int no);
    int getRemainCountCurrent();
    void setCurrentStaff(int no);
    
    void promote(int no);

    
    CCSprite* getSelSprite();
    
    YtAniObject* getStaffAniSprite(CCObject* owner);//, int no, int grade);
    
    
};


#endif  // __DYSTAFF_H__