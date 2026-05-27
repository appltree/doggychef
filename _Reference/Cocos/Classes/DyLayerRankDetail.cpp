/*
DyLayerRankDetail.cpp
Doggy chef
@initialize at 130104

@history
130104	yoonsr	initialize
*/


#include "DyLayerRankDetail.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyFriend.h"
#include "DyItem.h"
#include "DyLayerShop.h"
#include "DyStaff.h"


DyLayerRankDetail::DyLayerRankDetail()
{
    m_isGlobal = false;
    this->initWithColor(ccc4(255, 0, 0, 0), 200, 200);
}


DyLayerRankDetail::~DyLayerRankDetail()
{
}


/////Menu - Events
void DyLayerRankDetail::registerWithTouchDispatcher()
{
	CCTouchDispatcher* pDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
    
	pDispatcher->addTargetedDelegate(this, 0, false);
}


void DyLayerRankDetail::onEnter()
{
    DyLib* lib = DyLib::sharedLib();
    
	CCLayer::onEnter();

	if(m_isInit)
		return;
	m_isInit = true;
    
    setTouchEnabled(true);
    
    m_sprBg = CCSprite::create("images_ui_info.png");
    addChild(m_sprBg);
    
    m_sprBg->setAnchorPoint(ccp(0, 0));
    
    m_sprBg->setPosition(ccp(-50, 10));
    
    
    std::string nickname;
    DyCostumeStatus* costume_status;
    DyCostumeFunction* costume_function;
    DyDataPublic* _public;

    bool is_public_data_invalid;
    if(m_isGlobal)
    {
        is_public_data_invalid = m_rankGlobal->m_public.m_version == DYDATA_PUBLICDATA_VER && m_rankGlobal->m_public.m_stageRecord == m_rankGlobal->m_stage_record;
        
        if(!is_public_data_invalid)
            DyCostumeManager::setDefaultCostumeData(&m_rankGlobal->m_public.m_costume_status, &m_rankGlobal->m_public.m_costume_status_ex);
 
        nickname = m_rankGlobal->m_nickname;
        costume_status = &m_rankGlobal->m_public.m_costume_status;
        costume_function = &m_rankGlobal->m_costume_function;
        
        _public = &m_rankGlobal->m_public;
    }
    else
    {
        nickname = m_friendInfo->m_nickname;
        if(m_friendInfo->m_friendType == DyFriendType_Mine)
        {
            costume_status = &DyDataManager::sharedDataManager()->m_public.m_costume_status;
            costume_function = &DyDataManager::sharedDataManager()->m_custume_function;
            
            _public = &DyDataManager::sharedDataManager()->m_public;
        }
        else
        {
            costume_status = &m_friendInfo->m_public.m_costume_status;
            costume_function = &m_friendInfo->m_costume_function;
            
             _public = &m_friendInfo->m_public;
        }
        is_public_data_invalid = true;
    }
	   
    CCLabelTTF* labelName = CCLabelTTF::create(nickname.c_str(), FONT_NAME_EXP, 28, CCSizeMake(220, 33), kCCTextAlignmentRight);
    labelName->setColor(ccc3(43,20,7));
    m_sprBg->addChild(labelName);
    labelName->setPosition(ccp(210,173));
    labelName->setAnchorPoint(ccp(1,0.5));
    
    if(m_isGlobal)
        labelName->setPosition(ccp(210+20,173));

    CCLabelTTF* labelGlobalOrder = CCLabelTTF::create("", FONT_NAME_EXP, 22);
    labelGlobalOrder->setColor(ccWHITE);
    labelGlobalOrder->setAnchorPoint(ccp(1,0.5));
    labelGlobalOrder->setPosition(ccp(212-27,110+27));
    m_sprBg->addChild(labelGlobalOrder);
 
    if(m_global_order > 0)
    {
        sprintf(lib->m_strTemp, "%d", m_global_order);
        labelGlobalOrder->setString(lib->m_strTemp);
    }
    else
         labelGlobalOrder->setString("");
    
    YtAniObject* doggy = YtAniObject::loadAniObject(this, "node_animal_doggy.ccbi");
    doggy->setPosition(ccp(278, 38));
    doggy->setScale(0.5f);
    m_sprBg->addChild(doggy);
    
    
    
    DyCostumeManager::sharedCostumeManager()->setCostume(doggy, costume_status, costume_function);
    
    //
    // 스토리
    if(m_type == DyRankType_Stage|| m_type == DyRankType_SubStage)
    {
        DyStageManager* stageManager = DyStageManager::sharedStageManager();
        
        //menu2_ui_info_text_score.png
        CCSprite* sprObject = CCSprite::createWithSpriteFrameName("menu2_ui_info_text_local.png");
        sprObject->setAnchorPoint(ccp(0,0.5));
        sprObject->setPosition(ccp(27-7, 112));
        m_sprBg->addChild(sprObject);
        
        
        CCLabelTTF* labelShop = CCLabelTTF::create("", FONT_NAME_EXP, 22);
        labelShop->setColor(ccc3(2,0,39));
        labelShop->setAnchorPoint(ccp(1,0.5));
        labelShop->setPosition(ccp(212,110));
        m_sprBg->addChild(labelShop);
        
        CCLabelTTF* labelStage = CCLabelTTF::create("", FONT_NAME_EXP, 22);
        labelStage->setColor(ccc3(2,0,39));
        labelStage->setAnchorPoint(ccp(1,0.5));
        labelStage->setPosition(ccp(212,82));
        m_sprBg->addChild(labelStage);

        if(is_public_data_invalid)
        {
            int max_shop = 0;
            int max_level = 0;
            
            stageManager->getCurrentStage(_public, &max_shop, &max_level);
            std::string shop_name = DyStageManager::sharedStageManager()->getShopName(max_shop);
            labelShop->setString(shop_name.c_str());
            
            sprintf(lib->m_strTemp, "%d스테이지", max_level+1);
            labelStage->setString(lib->m_strTemp);
        }
        else
        {
            labelShop->setString("알수없음");
            
            labelStage->setString("");
        }
    }
    //
    // 크레이지
    else
    {
        CCSprite* sprObject = CCSprite::createWithSpriteFrameName("menu2_ui_info_text_score.png");
        sprObject->setAnchorPoint(ccp(0,0.5));
        sprObject->setPosition(ccp(27-7, 112));
        m_sprBg->addChild(sprObject);

        lib->setMoneyStyle(lib->m_strTemp2, m_scoreBest);
        
        sprintf(lib->m_strTemp, "%s점", lib->m_strTemp2);
        CCLabelTTF* labelStage = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 22);
        labelStage->setColor(ccc3(2,0,39));
        labelStage->setAnchorPoint(ccp(1,0.5));
        labelStage->setPosition(ccp(212,82));
        m_sprBg->addChild(labelStage);
    }
    
    if(is_public_data_invalid)
    {
        signed char staff_grade = _public->m_staff_grade;
        std::string strStaff;
        
        CCSprite* sprStaff;
        
        if(staff_grade == -1)
        {
            strStaff = "직원없음";
            
        }
        else if(staff_grade < 3)
        {
            strStaff = "캐셔";
        }
        else
        {
            strStaff = "매니져";
        }
        
        sprStaff = DyStaffManager::sharedStaffManager()->getStaffIcon(0, staff_grade);
        
        sprStaff->setPosition(ccp(210,54-5));
        sprStaff->setScale(0.6);
        m_sprBg->addChild(sprStaff);
        
        CCLabelTTF* labelStaff = CCLabelTTF::create(strStaff.c_str(), FONT_NAME_EXP, 20);
        labelStaff->setColor(ccc3(2,0,39));
        labelStaff->setAnchorPoint(ccp(0,0.5));
        labelStaff->setPosition(ccp(27,58-5));
        m_sprBg->addChild(labelStaff);
        
        
        sprintf(lib->m_strTemp, "Lv. %d", staff_grade+1);
        
        CCLabelTTF* labelStaffLevel = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 20);
        labelStaffLevel->setColor(ccc3(2,0,39));
        labelStaffLevel->setAnchorPoint(ccp(1,0.5));
        labelStaffLevel->setPosition(ccp(175,58-5));
        m_sprBg->addChild(labelStaffLevel);
    }
    
    setKeypadEnabled(true);
}


void DyLayerRankDetail::onExit()
{
	CCLayer::onExit();
}


void DyLayerRankDetail::pause()
{
    setKeypadEnabled(false);
}


void DyLayerRankDetail::resume()
{
    setKeypadEnabled(true);
}

void DyLayerRankDetail::hide()
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}


void DyLayerRankDetail::show()
{
	//YtLib* lib = YtLib::sharedLib();

	setPosition(m_posCenter);

	//runAction(CCSequence::create(

	//	CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
	//	CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
	//	CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
	//	NULL));
}



void DyLayerRankDetail::keyBackClicked()
{
    setKeypadEnabled(false);
    YtLayerManager::sharedLayerManager()->popLayer();
}




bool DyLayerRankDetail::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    setTouchEnabled(false);
    
    YtLayerManager::sharedLayerManager()->popLayer();
    

    return true;
}


//void DyLayerRankDetail::ccTouchesBegan(CCSet* touches, CCEvent* event)
//{
//	CCSetIterator it;
//	CCTouch* touch;
//    
//	for(it = touches->begin();it != touches->end();it++)
//	{
//		touch = (CCTouch*)(*it);
//		if(!touch)
//			break;
//        
//        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
//        setTouchEnabled(false);
//        
//        YtLayerManager::sharedLayerManager()->popLayer();
//        
//     	return;
//	}
//}
//


DyLayerRankDetail* DyLayerRankDetail::create(CCPoint posCenter, DyFriend* friendInfo, DyRankType type, unsigned int score, unsigned int best_score, int order_global)
{
	DyLayerRankDetail* layerRankDetail = new DyLayerRankDetail();
	layerRankDetail->autorelease();
    
    layerRankDetail->setPosition(posCenter);
    layerRankDetail->m_posCenter = posCenter;
    layerRankDetail->m_friendInfo = friendInfo;
    layerRankDetail->m_type = type;
    
    layerRankDetail->m_score = score;
    layerRankDetail->m_scoreBest = best_score;
    
    layerRankDetail->m_global_order = order_global;
    
    return layerRankDetail;
}


DyLayerRankDetail* DyLayerRankDetail::create(CCPoint posCenter, DyRankType type, DyRankGlobal *rank)
{
	DyLayerRankDetail* layerRankDetail = new DyLayerRankDetail();
	layerRankDetail->autorelease();
    
    layerRankDetail->m_isGlobal = true;
    
    layerRankDetail->setPosition(posCenter);
    layerRankDetail->m_posCenter = posCenter;
    
    layerRankDetail->m_rankGlobal = rank;
    layerRankDetail->m_type = type;
 
    
    layerRankDetail->m_score = rank->m_point;
    layerRankDetail->m_scoreBest = rank->m_point;
    
    layerRankDetail->m_global_order = rank->m_order;
    
    
    return layerRankDetail;
}
