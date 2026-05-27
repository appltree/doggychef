/*
DyNotice.h
Doggy chef
@initialize at 130922

@history
130922	yoonsr	initialize
*/


#ifndef __DYNOTICE_H__
#define __DYNOTICE_H__

#include "DyConfig.h"

#include "YtObject.h"
#include "DyCharacter.h"
#include "YtLoader.h"
#include "YtCommand.h"
#include "DyItem.h"
#include "DyData.h"

class DyNotice : public CCObject
{
public:
    std::string m_notice_id;
    int m_notice_type;
    std::string m_notice_msg;
    std::string m_notice_info;
    std::string m_title;
    
    bool m_canSawToday;       // 한번 보여짐(메인메뉴에서)
    
	DyNotice();
	~DyNotice();
};

class DyNoticeAttr : public CCObject
{
public:
    std::string m_notice_id;
    int m_showDay;
};

class DyNoticeManager : public CCObject
{
public:
    
    int m_index;
    bool m_loaded;
	CCArray* m_arrNotices;
    CCArray* m_arrDeleteNoticeID;
    CCArray* m_arrAttrNotice;
	DyNoticeManager();
	~DyNoticeManager();
    
    void reset();
    int getCount();
    
    void deleteNotice(DyNotice* notice);
    
    bool loadNotices(std::string data);
    bool isDeleted(std::string tag);
    bool canTodayShow(std::string tag);
    
    void changeCanTodayShow(std::string tag, bool showTodayShow);
    
    DyNotice* getCurrentNotice();
    
	static DyNoticeManager* sharedNoticeManager();
};



#endif  // __DYNOTICE_H__