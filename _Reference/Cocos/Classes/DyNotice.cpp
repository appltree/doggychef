/*
DyNotice.cpp
Doggy chef
@initialize at 130922

@history
130922	yoonsr	initialize
*/

#include "DyNotice.h"
#include "DyData.h"
#include "DyLib.h"

#include "DyLoader.h"
#include "YtNetwork.h"

#include "DyCostume.h"
#include "DyFriend.h"
#include "json.h"
#include "DyGuest.h"

#include "pugixml.hpp"


#define UNIT_DATA_SIZE_NOTICE_ID      (36)
#define UNIT_DATA_SIZE_NOTICE_ATTR    (4)

char g_notice_temp[256];


using namespace std;


DyNoticeManager* g_sharedNoticeManager;


DyNotice::DyNotice()
{
}



DyNotice::~DyNotice()
{
}



DyNoticeManager::DyNoticeManager()
{
    m_index = 0;
    m_loaded = false;
	m_arrNotices = new CCArray();
    
    m_arrDeleteNoticeID = new CCArray();
    m_arrAttrNotice = new CCArray();
}

DyNoticeManager::~DyNoticeManager()
{
    m_arrNotices->release();
    m_arrDeleteNoticeID->release();
    m_arrAttrNotice->release();
}


void DyNoticeManager::reset()
{
    m_loaded = false;
}



int DyNoticeManager::getCount()
{
    return m_arrNotices->count();
}


bool DyNoticeManager::isDeleted(std::string tag)
{
    for(int i = 0;i < m_arrDeleteNoticeID->count();i++)
    {
        CCString* str = (CCString*)m_arrDeleteNoticeID->objectAtIndex(i);
        if(tag.compare(str->getCString()) == 0)
        {
            return true;
        }
    }
    return false;
}

bool DyNoticeManager::canTodayShow(std::string tag)
{
    CCDate today;
    for(int i = 0;i < m_arrAttrNotice->count();i++)
    {
        DyNoticeAttr* attr = (DyNoticeAttr*)m_arrAttrNotice->objectAtIndex(i);
        if(tag.compare(attr->m_notice_id) == 0)
        {
            if(today.mday() == attr->m_showDay)
                return false;
        }
    }
    return true;
}



void DyNoticeManager::changeCanTodayShow(std::string tag, bool showTodayShow)
{
    CCDate today;
    
    bool has = false;
    
    for(int i = 0;i < m_arrAttrNotice->count();i++)
    {
        DyNoticeAttr* attr = (DyNoticeAttr*)m_arrAttrNotice->objectAtIndex(i);
        if(tag.compare(attr->m_notice_id) == 0)
        {
            if(showTodayShow)
            {
                m_arrAttrNotice->removeObject(attr);
            }
            else
            {
                attr->m_showDay = today.mday();
            }
            has = true;
        }
    }
    
    
    if(!has && !showTodayShow)
    {
        DyNoticeAttr* attr = new DyNoticeAttr();
        attr->m_notice_id = tag;
        attr->m_showDay = today.mday();
        
        m_arrAttrNotice->addObject(attr);
        attr->release();
    }
    
    int offset = 0;
    char* buffer = (char*)malloc((UNIT_DATA_SIZE_NOTICE_ID+UNIT_DATA_SIZE_NOTICE_ATTR)*m_arrAttrNotice->count()+1);
    
    unsigned char version = DYDATA_NOTICE_CHECK_VER;
    memcpy(buffer+offset, &version, 1);offset += 1;
    
    memset(buffer, 0, (UNIT_DATA_SIZE_NOTICE_ID+UNIT_DATA_SIZE_NOTICE_ATTR)*m_arrAttrNotice->count());
    
    for(int i = 0;i < m_arrAttrNotice->count();i++)
    {
        DyNoticeAttr* attr = (DyNoticeAttr*)m_arrAttrNotice->objectAtIndex(i);
        
       
        strcpy(g_notice_temp, attr->m_notice_id.c_str());
        
        memcpy(buffer+offset, g_notice_temp, strlen(g_notice_temp));offset += UNIT_DATA_SIZE_NOTICE_ID;
 
        memcpy(buffer+offset, &attr->m_showDay, UNIT_DATA_SIZE_NOTICE_ATTR);offset += UNIT_DATA_SIZE_NOTICE_ATTR;
    }
    
    YtLib::fileWrite(DATA_FILE_NOTICE_CHECK, buffer, offset);
    
    free(buffer);

    
}


bool DyNoticeManager::loadNotices(std::string data)
{
    m_loaded = true;
    
    m_arrNotices->removeAllObjects();
    m_arrDeleteNoticeID->removeAllObjects();
    m_arrAttrNotice->removeAllObjects();
    
    if(YtLib::fileExist(DATA_FILE_NOTICE_DELETE))
	{
        int size = YtLib::fileGetSize(DATA_FILE_NOTICE_DELETE);
        
        char* buffer = (char*)malloc(size);
        
        YtLib::fileRead(DATA_FILE_NOTICE_DELETE, (char *)buffer, size);
        
        int offset = 0;
        
        unsigned char version;
        memcpy(&version, buffer + offset, 1);offset += 1;
        
        for(int i = 0;i < size/UNIT_DATA_SIZE_NOTICE_ID;i++)
        {
            memcpy(g_notice_temp, buffer + offset, UNIT_DATA_SIZE_NOTICE_ID);offset += UNIT_DATA_SIZE_NOTICE_ID;
            
            CCString* str = new CCString(g_notice_temp);
            m_arrDeleteNoticeID->addObject(str);
            str->release();
        }
    }
    
    if(YtLib::fileExist(DATA_FILE_NOTICE_CHECK))
	{
        int size = YtLib::fileGetSize(DATA_FILE_NOTICE_CHECK);
        
        char* buffer = (char*)malloc(size);
        
        YtLib::fileRead(DATA_FILE_NOTICE_CHECK, (char *)buffer, size);
        
        int offset = 0;
        
        unsigned char version;
        memcpy(&version, buffer + offset, 1);offset += 1;
        
        for(int i = 0;i < size/(UNIT_DATA_SIZE_NOTICE_ID+UNIT_DATA_SIZE_NOTICE_ATTR);i++)
        {
            memcpy(g_notice_temp, buffer + offset, UNIT_DATA_SIZE_NOTICE_ID);offset += (UNIT_DATA_SIZE_NOTICE_ID);
            
            DyNoticeAttr* attr = new DyNoticeAttr();
            attr->m_notice_id = g_notice_temp;
            
            memcpy(&attr->m_showDay, buffer + offset, (UNIT_DATA_SIZE_NOTICE_ATTR));offset += (UNIT_DATA_SIZE_NOTICE_ATTR);
            
            m_arrAttrNotice->addObject(attr);
            attr->release();
        }
    }
    
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    
    reader.parse(data, root);
    
    Json::Value notices = root.get("notices", ""); // 게임내 리더보드 목록
    
    for(int index = 0; index < notices.size(); ++index )
    {
        Json::Value notice_id = notices[index].get("notice_id", "");
        Json::Value notice_type = notices[index].get("notice_type", "");
        Json::Value notice_msg = notices[index].get("notice_msg", "");
        Json::Value info = notices[index].get("info", "");
        Json::Value title = notices[index].get("title", "");
   
        
        if(isDeleted(notice_id.asString()))
           continue;

        DyNotice* _notice = new DyNotice();

        _notice->m_notice_id = notice_id.asString();
        _notice->m_notice_type = atoi(notice_type.asString().c_str());
        _notice->m_notice_msg = notice_msg.asString();
        _notice->m_notice_info = info.asString();

        _notice->m_title = title.asString();

        _notice->m_canSawToday = canTodayShow(_notice->m_notice_id);
        
        m_arrNotices->addObject(_notice);
        _notice->release();
    }

        
        
//  	pugi::xml_document doc;
//    
//  	doc.load_buffer_inplace((void*)data.c_str(), data.length());
//    
//	pugi::xml_node root = doc.child("notice_list");
//    
//    pugi::xml_node notices = root.child("notices");
//    
//	for (pugi::xml_node notice = notices.first_child(); notice; notice = notice.next_sibling())
//	{
//        pugi::xml_node notice_id = notice.child("notice_id");
//        pugi::xml_node type = notice.child("type");
//        pugi::xml_node notice_msg = notice.child("notice_msg");
//        pugi::xml_node info = notice.child("info");
//        pugi::xml_node title = notice.child("title");
//        
//        
//        if(isDeleted(notice_id.text().as_string()))
//           continue;
//           
//        DyNotice* _notice = new DyNotice();
//        
//        _notice->m_notice_id = notice_id.text().as_string();
//        _notice->m_notice_type = type.text().as_int();
//        _notice->m_notice_msg = notice_msg.text().as_string();
//        _notice->m_notice_info = info.text().as_string();
//        
//        memset(g_notice_temp, 0, 256);
//        base64_decode((char *)title.text().as_string(), (unsigned char *)g_notice_temp, 256);
//        _notice->m_title = g_notice_temp;
//        
//        _notice->m_canSawToday = canTodayShow(notice_id.text().as_string());
//        
//        m_arrNotices->addObject(_notice);
//        _notice->release();
//	}
    
    
    

    return true;
}


void DyNoticeManager::deleteNotice(DyNotice* notice)
{
    CCString* str = new CCString(notice->m_notice_id.c_str());
    m_arrDeleteNoticeID->addObject(notice);
    str->release();
    
    m_arrNotices->removeObject(notice);
  
    int offset = 0;
    char* buffer = (char*)malloc((UNIT_DATA_SIZE_NOTICE_ID)*m_arrDeleteNoticeID->count()+1);
    
    unsigned char version = DYDATA_NOTICE_DELETE_VER;
    memcpy(buffer+offset, &version, 1);offset += 1;
    
    memset(buffer, 0, (UNIT_DATA_SIZE_NOTICE_ID)*m_arrDeleteNoticeID->count());
    
    for(int i = 0;i < m_arrDeleteNoticeID->count();i++)
    {
        DyNotice* notice = (DyNotice*)m_arrDeleteNoticeID->objectAtIndex(i);
        
        strcpy(g_notice_temp, notice->m_notice_id.c_str());
        
        memcpy(buffer+offset, g_notice_temp, strlen(g_notice_temp));offset += UNIT_DATA_SIZE_NOTICE_ID;
    }
    
    YtLib::fileWrite(DATA_FILE_NOTICE_DELETE, buffer, offset);

    free(buffer);
}



DyNotice* DyNoticeManager::getCurrentNotice()
{
    for(int i = 0;i < m_arrNotices->count();i++)
    {
        DyNotice* _notice = (DyNotice*)m_arrNotices->objectAtIndex(i);
        
        if(_notice->m_canSawToday)
            return _notice;
    }
    
    return NULL;
}



DyNoticeManager* DyNoticeManager::sharedNoticeManager()
{
	if(g_sharedNoticeManager == NULL)
	{
		g_sharedNoticeManager = new DyNoticeManager();
	}
	return g_sharedNoticeManager;
}



