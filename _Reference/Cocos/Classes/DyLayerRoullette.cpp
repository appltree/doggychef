/*
DyLayerRoullette.cpp
Doggy chef
@initialize at 130608

@history
130608	yoonsr	initialize
*/


#include "DyConfig.h"
#include "DyLayerRoullette.h"
#include "DyLib.h"
#include "YtSpriteEx.h"
#include "DyData.h"
#include "DyTycoon.h"
#include "DyMoney.h"
#include "DyPopup.h"
#include "DyFriend.h"
#include "DyCostume.h"
#include "DyGuest.h"
#include "DyPopup.h"
#include "DyData.h"

#define MAX_RUNTIME         5.f


DyLayerRoullette::DyLayerRoullette()
{
    m_tag = DYLAYERROULLETTE_TAG;
    m_runRoulette = false;
    m_vel = 50.0f;
    m_runsnd = -1;
}

DyLayerRoullette::~DyLayerRoullette()
{
    if(m_runsnd >= 0)
    {
        YtLib::sharedLib()->stopEffect(m_runsnd);
        m_runsnd = -1;
    }

}


void DyLayerRoullette::refresh()
{
    DyLib* lib = DyLib::sharedLib();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    CCSprite* sprNoti = (CCSprite*)getChildByTag(ROULLETTE_UI_TAG_NOTI);
    CCLabelTTF* labelCount = (CCLabelTTF*)getChildByTag(ROULLETTE_UI_TAG_NOTI_CNT);
    
    if(dataManager->m_private.m_cntFreeRoullete > 0)
    {
        sprNoti->setDisplayFrame(CCSprite::createWithSpriteFrameName("roullette_info_bonus.png")->displayFrame());
        
        sprintf(lib->m_strTemp, "%d", dataManager->m_private.m_cntFreeRoullete);
        
        labelCount->setString(lib->m_strTemp);
        labelCount->setVisible(true);
    }
    else
    {
        sprNoti->setDisplayFrame(CCSprite::createWithSpriteFrameName("roullette_info.png")->displayFrame());
        labelCount->setVisible(false);
    }
}


void DyLayerRoullette::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader)
{
	m_menu = (CCMenu*)getChildByTag(ROULLETTE_UI_TAG_MENU);
    
    m_sprCircle = (CCSprite*)getChildByTag(ROULLETTE_UI_TAG_CIRCLE);
    m_sprFrame = (CCSprite*)getChildByTag(ROULLETTE_UI_TAG_FRAME);
    
    m_angle = rand()%360;
    this->scheduleUpdate();
    
    for(int i = 0;i < 8;i++)
    {
        m_sprLEDYellow[i] = (CCSprite*)getChildByTag(ROULLETTE_UI_TAG_YELLOW0+i);
        m_sprLEDBlue[i] = (CCSprite*)getChildByTag(ROULLETTE_UI_TAG_BLUE0+i);
    }
    refresh();
    setKeypadEnabled(true);
}


SEL_MenuHandler DyLayerRoullette::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onCloseClicked", DyLayerRoullette::onCloseClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPlayClicked", DyLayerRoullette::onPlayClicked);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onStopClicked", DyLayerRoullette::onStopClicked);

    return NULL;
}


SEL_CCControlHandler DyLayerRoullette::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}


bool DyLayerRoullette::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerRoullette::callfunc_update_me_success(float dt)
{
    unschedule(schedule_selector(DyLayerRoullette::callfunc_update_me_success));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    YtLayerManager::sharedLayerManager()->popLayer();
}


void DyLayerRoullette::onPopupClicked(cocos2d::CCObject * pSender)
{
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    
    if(pPopup->m_active_tag == 1000)
    {
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
    }
}


void DyLayerRoullette::callfunc_update_me_fail(float dt)
{
    unschedule(schedule_selector(DyLayerRoullette::callfunc_update_me_fail));
    
    DyLib* lib = DyLib::sharedLib();
    
    lib->show_network_alaram(false);
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerRoullette::onPopupClicked));
    popup->setText(STRING_TITLE_UPDATE_DATA_FAILURE);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 1000);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerRoullette_callfunc_update_me(int value, int code)
{
    DyLayerRoullette *layerRoullette = (DyLayerRoullette*)YtLayerManager::sharedLayerManager()->getLayerByTag(DYLAYERROULLETTE_TAG);
    
    if(value == 0)
    {
        layerRoullette->schedule(schedule_selector(DyLayerRoullette::callfunc_update_me_success), 0.05f);
    }
    else
    {
        layerRoullette->schedule(schedule_selector(DyLayerRoullette::callfunc_update_me_fail), 0.05f);
    }
}


void DyLayerRoullette::onCloseClicked(cocos2d::CCObject * pSender)
{
    DyLib* lib = DyLib::sharedLib();
    
	YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLOSE);
    
    lib->network_kakao_update_me(0, DyLayerRoullette_callfunc_update_me);
	//YtLayerManager::sharedLayerManager()->popLayer();
}


void DyLayerRoullette::keyBackClicked()
{
    if(DyLib::sharedLib()->m_showLoadingPopup)
        return;

    setKeypadEnabled(false);
    onCloseClicked(NULL);
}




void DyLayerRoullette::pause()
{
    if(m_runsnd >= 0)
    {
        YtLib::sharedLib()->stopEffect(m_runsnd);
        m_runsnd = -1;
    }

    m_menu->setEnabled(false);
    setKeypadEnabled(false);
}


void DyLayerRoullette::resume()
{
    m_menu->setEnabled(true);
    setKeypadEnabled(true);
}


void DyLayerRoullette::hide()
{
	YtLib* lib = YtLib::sharedLib();

	runAction(CCSequence::create(

		CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)), 
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height)),

		CCCallFunc::create(this, callfunc_selector(DyLayerRoullette::destory)),
		NULL));
}


void DyLayerRoullette::show()
{
	YtLib* lib = YtLib::sharedLib();

	setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));

	runAction(CCSequence::create(

		CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
		NULL));
}

void DyLayerRoullette::onStopClicked(CCObject * obj)
{
    if(m_runsnd >= 0)
    {
        YtLib::sharedLib()->stopEffect(m_runsnd);
        m_runsnd = -1;
    }
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_ROULLETTE_STOP);
    m_dtRun = MAX_RUNTIME;
    ((CCMenuItemImage*)m_menu->getChildByTag(ROULLETTE_UI_TAG_MENUITEM_STOP))->setEnabled(false);
}

void DyLayerRoullette::onPlayClicked(CCObject * obj)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    bool run_roullette = false;
    if(m_runRoulette)
        return;
    
    CCDate dateCurrent = dataManager->getCurrentDate();
    

    if(dataManager->m_private.m_cntRoulletteDay != dateCurrent.mday())
    {
        dataManager->m_private.m_cntRoulletteDay = dateCurrent.mday();
        dataManager->m_private.m_cntRoulletteToday = 0;
    }
    
    dataManager->m_private.m_cntRoulletteToday++;
    
    if(dataManager->m_private.m_cntRoulletteToday > dataManager->m_cntRoulletPerDay)
    {
        DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
     
        sprintf(lib->m_strTemp, "하루 %d번만 할 수있어요.", dataManager->m_cntRoulletPerDay);
        popup->setText(lib->m_strTemp);
        popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);

        return;
    }
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    m_runsnd = YtLib::sharedLib()->playEffect(YtLibSndEff_ROULLETTE_RUN);
    
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    
    if(dataManager->m_private.m_cntFreeRoullete > 0)
    {
        dataManager->m_private.m_cntFreeRoullete--;
        run_roullette = true;
    }
    else
    {
        if(5000 > moneyManager->getGold())
        {
            moneyManager->alramNotEnoughGold(true, true);
        }
        else
        {
            moneyManager->addGold(-5000, true);
            moneyManager->dataSave();
            
            run_roullette = true;
        }
    }
    if(run_roullette)
    {
        m_runRoulette = true;
        m_dtRun = 0;
        m_vel = 600.0f;
        m_dtBlink = 0;
        //m_menu->setEnabled(false);
        m_yellow = false;
        m_dtRunTime = (float)(rand()%100)/100.f + (MAX_RUNTIME-1);
        
        ((CCMenuItemImage*)m_menu->getChildByTag(ROULLETTE_UI_TAG_MENUITEM_PLAY))->setVisible(false);
        
        ((CCMenuItemImage*)m_menu->getChildByTag(ROULLETTE_UI_TAG_MENUITEM_STOP))->setEnabled(true);
        ((CCMenuItemImage*)m_menu->getChildByTag(ROULLETTE_UI_TAG_MENUITEM_STOP))->setVisible(true);
        
        ((CCMenuItemImage*)m_menu->getChildByTag(ROULLETTE_UI_TAG_MENUITEM_CLOSE))->setEnabled(false);
        
    }
    refresh();
}

/*
 0 델리 0~25, 335~360
 1 골드 35~85
 2 껌 95~145
 3 아이템 155~205
 4 모자 215~265
 5 개스트 275~325
 */
void DyLayerRoullette::getPresent(int index)
{
    DyLib* lib = DyLib::sharedLib();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    int value;
    int ramdom_value = rand()%100;
    
    switch(index)
    {
        case 0://델리
            {
                int probability[5] = {58,   20,     15,     6,     1};
                int value_delly[5] = {1,    3,      5,     20,     50};
                
                if(ramdom_value < probability[4])
                    value = value_delly[4];
                else if(ramdom_value < probability[4]+probability[3])
                    value = value_delly[3];
                else if(ramdom_value < probability[4]+probability[3]+probability[2])
                    value = value_delly[2];
                else if(ramdom_value < probability[4]+probability[3]+probability[2]+probability[1])
                    value = value_delly[1];
                else if(ramdom_value < probability[4]+probability[3]+probability[2]+probability[1]+probability[0])
                    value = value_delly[0];
                
                if(dataManager->m_private.m_cntRunRoulette == 0)
                    value = value_delly[1];
                
                moneyManager->addDelly(value, true);
                sprintf(lib->m_strTemp, STRING_ROULLETTE_DELLY, value);
            }
            
            break;
        case 1://골드
            {
                int probability[4] = {30,   60,     9,     1};
                int value_gold[4] =  {1000, 3000,   30000,  100000};

                if(ramdom_value < probability[3])
                    value = value_gold[3];
                else if(ramdom_value < probability[3]+probability[2])
                    value = value_gold[2];
                else if(ramdom_value < probability[3]+probability[2]+probability[1])
                    value = value_gold[1];
                else if(ramdom_value < probability[3]+probability[2]+probability[1]+probability[0])
                    value = value_gold[0];
                
                if(dataManager->m_private.m_cntRunRoulette == 0)
                    value = value_gold[2];
                
                moneyManager->addGold(value, true);
                sprintf(lib->m_strTemp, STRING_ROULLETTE_GOLD, value);
            }
            break;
        case 2://검
            {
                int probability[4] = {60,   30,     9,     1};
                int value_gum[4] =   {1,    5,     10,     50};
                
                if(ramdom_value < probability[3])
                    value = value_gum[3];
                else if(ramdom_value < probability[3]+probability[2])
                    value = value_gum[2];
                else if(ramdom_value < probability[3]+probability[2]+probability[1])
                    value = value_gum[1];
                else if(ramdom_value < probability[3]+probability[2]+probability[1]+probability[0])
                    value = value_gum[0];
                
                if(dataManager->m_private.m_cntRunRoulette == 0)
                    value = value_gum[1];

                dataManager->m_rewardedGum += value;
                moneyManager->addGum(value, true);
                sprintf(lib->m_strTemp, STRING_ROULLETTE_GUM, value);
            }
            break;
        case 3://아이템
            {
                DyItem* item = DyItemManager::sharedItemManager()->getItem((DyItemType)(rand()%DyItemType_Count));
                
                int probability[4] = {20,   60,     15,     5};
                int value_item[4] =   {5,    10,     20,     30};
                
                if(ramdom_value < probability[3])
                    value = value_item[3];
                else if(ramdom_value < probability[3]+probability[2])
                    value = value_item[2];
                else if(ramdom_value < probability[3]+probability[2]+probability[1])
                    value = value_item[1];
                else if(ramdom_value < probability[3]+probability[2]+probability[1]+probability[0])
                    value = value_item[0];
                
                if(dataManager->m_private.m_cntRunRoulette == 0)
                    value = value_item[2];

  
                DyItemManager::sharedItemManager()->addItem(item->m_itemtype, value);
                
                lib->setMoneyStyle(lib->m_strTemp2, value);
                sprintf(lib->m_strTemp, STRING_ROULLETTE_ITEM, item->m_name.c_str(), lib->m_strTemp2);
            }
            break;
        case 4://모자
            {
                //블루클럽컷/동정심유발냄비/히릿가발세트/마법사모자/핫서머 물안경/히어로복면

                
                int probability_org[6] = {15,   35,     20,     15,     10,  5};
                int probability[6] = {15,   35,     20,     15,     10,  5};
                int value_hat[6] = {4,    6,      8,     12,     15,    16};
              
                
                probability[5] = dataManager->m_cntPovabilityHero;
                probability[4] = (100-dataManager->m_cntPovabilityHero)*probability_org[4]/(probability_org[0]+probability_org[1]+probability_org[2]+probability_org[3]+probability_org[4]);
                probability[3] = (100-dataManager->m_cntPovabilityHero)*probability_org[3]/(probability_org[0]+probability_org[1]+probability_org[2]+probability_org[3]+probability_org[4]);
                probability[2] = (100-dataManager->m_cntPovabilityHero)*probability_org[2]/(probability_org[0]+probability_org[1]+probability_org[2]+probability_org[3]+probability_org[4]);
                probability[1] = (100-dataManager->m_cntPovabilityHero)*probability_org[1]/(probability_org[0]+probability_org[1]+probability_org[2]+probability_org[3]+probability_org[4]);
                probability[0] = (100-dataManager->m_cntPovabilityHero)*probability_org[0]/(probability_org[0]+probability_org[1]+probability_org[2]+probability_org[3]+probability_org[4]);
                
                
//                for(int i = 0;i < 6;i++)
//                {
//                    CCLog("probability[%d] = %d", i, probability[i]);
//                }
//                
//                CCLog("ramdom_value = %d", ramdom_value);
                
                if(ramdom_value < probability[5])
                    value = value_hat[5];//히어로 복면
                else if(ramdom_value < probability[5]+probability[4])
                    value = value_hat[4];//핫서머 물안경
                else if(ramdom_value < probability[5]+probability[4]+probability[3])
                    value = value_hat[3];//마법사모자
                else if(ramdom_value < probability[5]+probability[4]+probability[3]+probability[2])
                    value = value_hat[2];//히릿가발세트
                else if(ramdom_value < probability[5]+probability[4]+probability[3]+probability[2]+probability[1])
                    value = value_hat[1];//동정심유발
                else
                    value = value_hat[0];//블루클럽컷
                
//                if(dataManager->m_private.m_cntRunRoulette == 0)
//                    value = value_hat[3];

                sprintf(lib->m_strTemp, "hat_%02d", value);
                DyCostume* costume = DyCostumeManager::sharedCostumeManager()->getCostume(lib->m_strTemp);
                
                if(DyCostumeManager::sharedCostumeManager()->buyedCostume(costume))
                    sprintf(lib->m_strTemp, STRING_ROULLETTE_HAT, costume->m_name.c_str());
                else
                {
                    int bonus_gold = 0;
                    std::string hat;
                    switch(value)
                    {
                        case 4://블루클럽컷
                            bonus_gold = 1500;
                            break;
                        case 6://동정심유발
                            bonus_gold = 300;
                            break;
                        case 8://히릿가발세트
                            bonus_gold = 3000;
                            break;
                        case 12://마법사모자
                            bonus_gold = 4000;
                            break;
                        case 15://핫서머 물안경
                            bonus_gold = 7000;
                            break;
                        case 16://히어로 복면
                            bonus_gold = 10000;
                            break;
                    }
                    moneyManager->addGold(bonus_gold, true);
                    sprintf(lib->m_strTemp, "%s(을)를 이미 보유중이므로 %d골드로 대신 보상받았어요.", costume->m_name.c_str(), bonus_gold);
                    
                }
            }
            
            
            break;
        case 5://개스트
            {
                DyGuestManager *guestManager = DyGuestManager::sharedGuestManager();
                
                int probability[4] = {55,   32,     10,     3};
                int value_guest[4] = {1,    5,      10,     50};
                
                if(ramdom_value < probability[3])
                    value = value_guest[3];
                else if(ramdom_value < probability[3]+probability[2])
                    value = value_guest[2];
                else if(ramdom_value < probability[3]+probability[2]+probability[1])
                    value = value_guest[1];
                else if(ramdom_value < probability[3]+probability[2]+probability[1]+probability[0])
                    value = value_guest[0];
                
                
                if(dataManager->m_private.m_cntRunRoulette == 0)
                    value = value_guest[1];

                
                guestManager->addGuest(value, DyGuestType_Gacha, NULL);
                sprintf(lib->m_strTemp, STRING_ROULLETTE_GUEST, value);
            }
            break;
    }
    
    dataManager->m_private.m_cntRunRoulette++;
    
    if(dataManager->m_private.m_cntRunRoulette >= MAX_CHARNUM)
        dataManager->m_private.m_cntRunRoulette = MAX_CHARNUM;
    
    dataManager->dataSave();
    
    
    DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
    
    popup->setText(lib->m_strTemp);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
}


void DyLayerRoullette::blinkLED()
{
    if(m_yellow)
    {
        for(int i = 0;i < 8;i++)
        {
            m_sprLEDYellow[i]->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("roullette_light_yell_on.png"));
            m_sprLEDBlue[i]->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("roullette_light_blu_off.png"));
        }
        
    }
    else
    {
        for(int i = 0;i < 8;i++)
        {
            m_sprLEDYellow[i]->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("roullette_light_yell_off.png"));
            m_sprLEDBlue[i]->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("roullette_light_blu_on.png"));
        }

    }
    
    m_yellow = !m_yellow;
}

void DyLayerRoullette::update(float dt)
{
    if(m_runRoulette)
    {
        m_dtRun += dt;
        
        if(m_dtRun > m_dtRunTime)
        {
            m_vel -= 10.f;
            
            if(m_vel < 0)
            {
                m_vel = 0;
                m_runRoulette = false;
                
                ((CCMenuItemImage*)m_menu->getChildByTag(ROULLETTE_UI_TAG_MENUITEM_PLAY))->setVisible(true);
                ((CCMenuItemImage*)m_menu->getChildByTag(ROULLETTE_UI_TAG_MENUITEM_STOP))->setVisible(false);
                ((CCMenuItemImage*)m_menu->getChildByTag(ROULLETTE_UI_TAG_MENUITEM_CLOSE))->setEnabled(true);
                
                //m_menu->setEnabled(true);
                for(int i = 0;i < 8;i++)
                {
                    m_sprLEDYellow[i]->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("roullette_light_yell_off.png"));
                    m_sprLEDBlue[i]->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("roullette_light_blu_off.png"));
                }
                

                int angle = (int)m_angle%360;
                int targetIdx;
                if((angle < 30) || (angle >= 330))
                {
                    targetIdx = 0;
                }
                else if(angle >= 30 && angle < 90)
                    targetIdx = 1;
                else if(angle >= 90 && angle < 150)
                    targetIdx = 2;
                else if(angle >= 150 && angle < 210)
                    targetIdx = 3;
                else if(angle >= 210 && angle < 270)
                    targetIdx = 4;
                else if(angle >= 270 && angle < 330)
                    targetIdx = 5;
                
                getPresent(targetIdx);
            }
        }
        
        
        m_dtBlink += dt;
        if(m_dtRun > m_dtRunTime)
        {
            if(m_dtBlink > 0.1)
            {
                m_dtBlink = 0;
                blinkLED();
            }
        }
        else
        {
            if(m_dtBlink > 0.05)
            {
                m_dtBlink = 0;
                blinkLED();
            }
        }
    }
    
    m_angle += m_vel*dt;
    
    m_sprCircle->setRotation(m_angle);
}

DyLayerRoullette* DyLayerRoullette::create(CCNode* owner, bool callByMainMenu)
{
	DyLayerRoullette* layerRoullette = (DyLayerRoullette*)YtLayerManager::create("DyLayerRoullette", DyLayerRoulletteLoader::loader(), "layer_roullette.ccbi", owner);
    return layerRoullette;
}