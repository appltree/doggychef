/*
DyStaff.cpp
Doggy chef
@initialize at 130516

@history
130516	yoonsr	initialize
*/

#include "DyStaff.h"
#include "DyData.h"
#include "DyMoney.h"
#include "DyTycoon.h"
#include "DyStage.h"


DyStaffManager* g_sharedStaffManager;

DyStaffManager::DyStaffManager()
{
    m_staff_list = DyLoader::sharedLoader()->loadObjects("staff_list.txt");
    m_staff_list->retain();
    
    m_staff[0] = DyLoader::sharedLoader()->loadObjects("staff1.txt");
	m_staff[0]->retain();
    
    m_staff[1] = DyLoader::sharedLoader()->loadObjects("staff2.txt");
	m_staff[1]->retain();

    
    m_isHire = false;
    
    CCTime::gettimeofdayCocos2d(&m_timeWork, NULL);
}


DyStaffManager::~DyStaffManager()
{
    m_staff_list->release();
    
    for(int i = 0;i < STAFF_CNT;i++)
    {
        m_staff[i]->release();
    }
}


void DyStaffManager::dataSave()
{
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
	dataManager->dataSave();

}


bool DyStaffManager::hasStaff()
{
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
	if(getGradeCurrent() >= 0)
        return true;
    else
        return false;
}


void DyStaffManager::setHire(bool hire)
{
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(m_isIdle)
    {
        m_isHire = false;
        return;
    }
    
    if(getGradeCurrent() >= 0)
        m_isHire = hire;
    else
        m_isHire = false;
}


bool DyStaffManager::isHire()
{
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(getGradeCurrent() < 0)
        m_isHire = false;
        
    return m_isHire;
}


void DyStaffManager::setIdle(bool idle, std::string strReason)
{
    m_isIdle = idle;
    m_reason = strReason;
}


bool DyStaffManager::isIdle()
{
    return m_isIdle;
}


std::string DyStaffManager::getReason()
{
    return m_reason;
}


int DyStaffManager::getPay()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    return atoi(m_staff[dataManager->m_private.m_current_staff]->getValue("hire_pay", getGradeCurrent()));
}


void DyStaffManager::hire()
{
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    
    if(m_isHire == false)
        return;
    if(m_isIdle)
    {
        m_isHire = false;
        return;
    }
    
    moneyManager->addGold(-getPay(), true);
    moneyManager->dataSave();
}



float DyStaffManager::getProfitShare()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    return ((float)atoi(m_staff[dataManager->m_private.m_current_staff]->getValue("profit_share", getGradeCurrent())))/100.f;
}


bool DyStaffManager::processCount(float dt)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(!m_isHire)
        return false;
    
    if(tycoonManager->m_failWithQuest)
        return false;

    float term_count = ((float)atoi(m_staff[dataManager->m_private.m_current_staff]->getValue("term_count", getGradeCurrent())))/1;
    float term_work = ((float)atoi(m_staff[dataManager->m_private.m_current_staff]->getValue("term_work", getGradeCurrent())))/1;
    
    
	static cc_timeval _time;
	CCTime::gettimeofdayCocos2d(&_time, NULL);
	float diffSecond = CCTime::timersubCocos2d(&m_timeWork, &_time);
    
    //CCLog("diffSecond = %f  term_count = %f, term_work = %f", diffSecond, term_count, term_work);
    
    if(diffSecond >= term_work && dt > term_count/1000.f)
    {
        CCTime::gettimeofdayCocos2d(&m_timeWork, NULL);
        return true;
    }
    
    return false;
}


bool DyStaffManager::processCandy()
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if(!m_isHire)
        return false;
    
    if(tycoonManager->m_failWithQuest)
        return false;
    
    if(dataManager->m_private.m_current_staff != 1)
        return false;

    float candy_percent = ((float)atoi(m_staff[dataManager->m_private.m_current_staff]->getValue("candy_percent", getGradeCurrent())))/1;
    
    if(candy_percent > rand() % 100)
    {
        return true;
    }
    
    return false;
}


bool DyStaffManager::processSideServe(float dt)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(!m_isHire)
        return false;
    
    if(!(dataManager->m_private.m_current_staff == 0 && dataManager->m_public.m_staff_grade >= 3))
        return false;
    
    if(dataManager->m_private.m_current_staff != 0)
        return false;
    
    if(tycoonManager->m_failWithQuest)
        return false;

    float term_serve = ((float)atoi(m_staff[dataManager->m_private.m_current_staff]->getValue("term_serve", getGradeCurrent())))/1;
    float term_work = ((float)atoi(m_staff[dataManager->m_private.m_current_staff]->getValue("term_work", getGradeCurrent())))/1;

  	static cc_timeval _time;
	CCTime::gettimeofdayCocos2d(&_time, NULL);
	float diffSecond = CCTime::timersubCocos2d(&m_timeWork, &_time);
  
    if(diffSecond >= term_work && dt > term_serve/1000.f)
    {
        CCTime::gettimeofdayCocos2d(&m_timeWork, NULL);
        return true;
    }
    return false;
}

void DyStaffManager::hireSuccess()
{
    if(m_isHire)
    {
        DyDataManager* dataManager = DyDataManager::sharedDataManager();
        
        if(dataManager->m_private.m_current_staff == 0)
        {
            dataManager->m_private.m_staff_next_grade_remain--;
        
            if(dataManager->m_private.m_staff_next_grade_remain < 0)
                dataManager->m_private.m_staff_next_grade_remain = 0;
        }
        else
        {
            dataManager->m_private.m_staff_ext_next_grade_remain[dataManager->m_private.m_current_staff-1]--;
            
            if(dataManager->m_private.m_staff_ext_next_grade_remain[dataManager->m_private.m_current_staff-1] < 0)
                dataManager->m_private.m_staff_ext_next_grade_remain[dataManager->m_private.m_current_staff-1] = 0;
        }
        
        
        dataSave();
    }
}


YtAniObject* DyStaffManager::getStaffAniSprite(CCObject* owner)//, int no, int grade)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
    if(dataManager->m_private.m_current_staff == 0)
    {
        int grade = dataManager->m_public.m_staff_grade;
        if(grade < 3)
            return YtAniObject::loadAniObject(owner, "staff_cashier.ccbi");
        else
            return YtAniObject::loadAniObject(owner, "staff_manager.ccbi");
    }
    else
    {
        return YtAniObject::loadAniObject(owner, "staff_candy.ccbi");
    }
}


bool DyStaffManager::isAvailable(int no)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
    if(no == 0)
    {
        return dataManager->m_public.m_staff_grade >= 0;
    }
    return dataManager->m_private.m_staff_ext_grade[no-1] >= 0;
}


bool DyStaffManager::isBuyable(int no)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
    if(no == 0)
    {
        return dataManager->m_public.m_staff_grade >= 0;
    }
    else
    {
        std::string conditon = m_staff_list->getValue("conditon", no);
        
        if(conditon.compare("pizza") == 0)
        {
            DyStageManager* stageManager = DyStageManager::sharedStageManager();
            return stageManager->isShopOpen(DyStageType_Pizza);
        }
        
    }
    return false;
}


CCSprite* DyStaffManager::getImage(int no)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
    if(no == 0)
    {
        int grade = dataManager->m_public.m_staff_grade;
        if(grade == -1)
        {
            return CCSprite::createWithSpriteFrameName("staff_ui_cashier_lock.png");
        }
        else if(grade < 3)
            return CCSprite::createWithSpriteFrameName("staff_ui_cashier.png");
        else
            return CCSprite::createWithSpriteFrameName("staff_ui_manager.png");
    }
    else
    {
        int grade = dataManager->m_private.m_staff_ext_grade[no-1];
        
        if(grade == -1)
            return CCSprite::createWithSpriteFrameName(m_staff_list->getValue("image_glow", no));
        else
            return CCSprite::createWithSpriteFrameName(m_staff_list->getValue("image", no));
    }
    
    return NULL;
}


CCSprite* DyStaffManager::getImageWithGrade(int no, int grade)
{
    if(no == 0)
    {
        if(grade == -1)
        {
            return CCSprite::createWithSpriteFrameName("staff_ui_cashier_lock.png");
        }
        else if(grade < 3)
            return CCSprite::createWithSpriteFrameName("staff_ui_cashier.png");
        else
            return CCSprite::createWithSpriteFrameName("staff_ui_manager.png");
    }
    else
    {
        if(grade == -1)
            return CCSprite::createWithSpriteFrameName(m_staff_list->getValue("image_glow", no));
        else
            return CCSprite::createWithSpriteFrameName(m_staff_list->getValue("image", no));
    }
    
    return NULL;
}


int DyStaffManager::getGrade(int no)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
    if(no == 0)
    {
        return dataManager->m_public.m_staff_grade;
    }
    
    return dataManager->m_private.m_staff_ext_grade[no-1];

}

int DyStaffManager::getGradeCurrent()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
    if(dataManager->m_private.m_current_staff == 0)
    {
        return dataManager->m_public.m_staff_grade;
    }
    
    return dataManager->m_private.m_staff_ext_grade[dataManager->m_private.m_current_staff-1];

}

int DyStaffManager::getRemainCount(int no)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	if(no == 0)
        return dataManager->m_private.m_staff_next_grade_remain;
    
    return dataManager->m_private.m_staff_ext_next_grade_remain[no-1];
    
}

int DyStaffManager::getRemainCountCurrent()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	if(dataManager->m_private.m_current_staff == 0)
        return dataManager->m_private.m_staff_next_grade_remain;
    
    return dataManager->m_private.m_staff_ext_next_grade_remain[dataManager->m_private.m_current_staff-1];
    
}


void DyStaffManager::promote(int no)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    
    if(no == 0)
    {
        dataManager->m_public.m_staff_grade++;
        
        if(dataManager->m_public.m_staff_grade < 5)
            dataManager->m_private.m_staff_next_grade_remain = atoi(m_staff[no]->getValue("conditon", dataManager->m_public.m_staff_grade+1));
    }
    else
    {
        dataManager->m_private.m_staff_ext_grade[no-1]++;
        
        if(dataManager->m_private.m_staff_ext_grade[no-1] < m_staff[no]->count()-1)
        {
            dataManager->m_private.m_staff_ext_next_grade_remain[no-1] = atoi(m_staff[no]->getValue("conditon", dataManager->m_private.m_staff_ext_grade[no-1]+1));
        }
    }

}


void DyStaffManager::setCurrentStaff(int no)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
//    if(m_cur_staff)
//        m_cur_staff->release();
    
    dataManager->m_private.m_current_staff = no;
    
//    m_cur_staff = DyLoader::sharedLoader()->loadObjects(m_staff_list->getValue("link", no));
//	m_cur_staff->retain();
    m_isHire = false;
}

CCSprite* DyStaffManager::getStaffIcon(int no, int grade)
{
    CCSprite* sprStaff;
    
    if(grade == -1)
    {
        sprStaff = CCSprite::createWithSpriteFrameName("staff_ui_icon_lock.png");
    }
    else if(no == 0)
    {
        if(grade < 3)
        {
            sprStaff = CCSprite::createWithSpriteFrameName("staff_ui_cashier_icon.png");
        }
        else
        {
            sprStaff = CCSprite::createWithSpriteFrameName("staff_ui_manager_icon.png");
        }
    }
    else if(no == 1)
    {
        sprStaff = CCSprite::createWithSpriteFrameName("staff_ui_candy_icon.png");
    }
    
    return sprStaff;
}


CCSprite* DyStaffManager::getSelSprite()
{
    CCSprite* sprStaff;
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
	
    
    if(dataManager->m_private.m_current_staff == 0)
    {
        if(dataManager->m_public.m_staff_grade < 3)
            sprStaff = CCSprite::createWithSpriteFrameName("staff_ui_cashier_sel.png");
        else
            sprStaff = CCSprite::createWithSpriteFrameName("staff_ui_manager_sel.png");
    }
    else if(dataManager->m_private.m_current_staff == 1)
    {
        sprStaff = CCSprite::createWithSpriteFrameName("staff_ui_candy_sel.png");
    }
    
    return sprStaff;
}



DyStaffManager* DyStaffManager::sharedStaffManager()
{
	if(g_sharedStaffManager == NULL)
	{
		g_sharedStaffManager = new DyStaffManager();
	}
	return g_sharedStaffManager;
}

