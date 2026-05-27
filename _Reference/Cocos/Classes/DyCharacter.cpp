/*
DyCharacter.cpp
Doggy chef
@initialize at 121008

@history
121008	yoonsr	initialize
*/

#include "DyCharacter.h"
#include "DyLoader.h"
#include "DyRecipe.h"
#include "DyCustomer.h"
#include "DyStage.h"
#include "DyTycoon.h"
#include "DySceneGame.h"
#include "DyTycoonCommand.h"
#include "DyLib.h"
#include "DyPopup.h"
#include "DyData.h"
#include "DyFriend.h"
#include "DyGuest.h"
#include "DyTutorial.h"

DyCharacterManager* g_sharedCharacterManager;


DyCharacterInfo::DyCharacterInfo()
{
}

DyCharacterInfo::~DyCharacterInfo()
{
}



void DyCharacter::hangOn(CCNode* node)
{
    DyCharacterManager* characterManager = DyCharacterManager::sharedCharacterManager();
    characterManager->m_idSndCall = YtLib::sharedLib()->playEffect(YtLibSndEff_DELIVERY_CALL);
}


DyCharacter::DyCharacter(int characterID, DyCustomer* customer, CCLayer* layer)
{
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    m_sprPhone = NULL;
    m_comming = 0;
    m_checkCandy = false;
    
	m_layerCharacter = CCLayer::create();
    m_customer = customer;
    
    CCLayer* layerCharacterStand = (CCLayer*)layer->getChildByTag(GAME_UI_TAG_SLOT_CHARACTER_0 + m_customer->m_slotidx);
    m_layerParent = layer;
    m_posBar = layerCharacterStand->getPosition();
    m_posStart = ccp(-120, m_posBar.y);
    
    m_layerTable = (CCLayer*)m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_PAY_0 + m_customer->m_slotidx);
    
    if(m_customer->m_customerType == DyCustomerType_Gaest)
    {
        CCSprite* sprPicure = DyGuestManager::sharedGuestManager()->getCurrentGuestPicture();
        
         m_animation = YtAniObject::loadAniObject(this, "node_animal_sister.ccbi");
        
        if(!customerManager->m_isCreateOpportunityGeast)
        {
            if(sprPicure)
            {
                m_animation->addChild(sprPicure);
                sprPicure->setPosition(ccp(0, 220-10));
                sprPicure->setAnchorPoint(ccp(0.5, 0.5));
            }
            
            std::string name = DyGuestManager::sharedGuestManager()->getCurrentGuestName();
            if(name.size() > 0)
            {
                CCLabelTTF* labelName = CCLabelTTF::create(name.c_str(), FONT_NAME_EXP, 22.f);
                
                CCScale9Sprite* sprLabelBg = CCScale9Sprite::createWithSpriteFrame(CCSprite::createWithSpriteFrameName("meun_ui_name_bg.png")->displayFrame());
                sprLabelBg->setContentSize(CCSizeMake(labelName->getContentSize().width+10, 24));
                sprLabelBg->setAnchorPoint(ccp(0.5, 0.5));
                sprLabelBg->setPosition(ccp(0, 180-10));
                m_animation->addChild(sprLabelBg);
                
                labelName->setColor(ccWHITE);
                m_animation->addChild(labelName);
                labelName->setPosition(sprLabelBg->getPosition());
                labelName->setAnchorPoint(ccp(0.5, 0.5));
            }
        }
        
        m_characterInfo = NULL;
        
        m_heart_start = 10;
        m_heart_max = 10;
        customer->m_rectTouch = CCRectMake(m_posBar.x - 141/2, m_posBar.y, 141, 200);

       
        
    }
    else if(m_customer->m_customerType == DyCustomerType_Delivery)
    {
        m_characterInfo = NULL;
        
        m_heart_start = 6;
        m_heart_max = 6;
        customer->m_rectTouch = CCRectMake(m_posBar.x - 141/2, m_posBar.y, 141, 200);
        
        m_animation = YtAniObject::loadAniObject(this, "node_animal_delivery.ccbi");
        
        m_sprPhone = CCSprite::createWithSpriteFrameName("game_ui_phone.png");
        
        CCArray* animFrames = CCArray::create();
        
        const char* strPhone[20] = {
            "game_ui_phone_ring0.png",
            "game_ui_phone_ring1.png",
            "game_ui_phone_ring0.png",
            "game_ui_phone_ring1.png",
            "game_ui_phone_ring0.png",
            "game_ui_phone_ring1.png",
            "game_ui_phone_ring0.png",
            "game_ui_phone_ring1.png",
            "game_ui_phone_ring0.png",
            "game_ui_phone_ring1.png",
            "game_ui_phone_ring0.png",
            "game_ui_phone_ring1.png",
            "game_ui_phone_ring0.png",
            "game_ui_phone_ring1.png",
            "game_ui_phone_ring0.png",
            "game_ui_phone_ring1.png",

            "game_ui_phone.png",
            "game_ui_phone.png",
            "game_ui_phone.png",
            "game_ui_phone.png",

        };
        
        for(int i = 0; i < 20; i++)
        {
            animFrames->addObject(CCSprite::createWithSpriteFrameName(strPhone[i])->displayFrame());
        }
        
        CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.1f);
        m_sprPhone->runAction(CCSequence::create(CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::hangOn)), CCAnimate::create(animation), CCDelayTime::create(2.f), CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::hangOn)), CCAnimate::create(animation), CCDelayTime::create(2.f), CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::hangOn)), CCAnimate::create(animation), NULL));
        
        m_layerTable->addChild(m_sprPhone);
        m_sprPhone->setPosition(ccp(m_layerTable->getContentSize().width/2, m_layerTable->getContentSize().height/2));
        
        m_sprBag = CCSprite::createWithSpriteFrameName(tycoonManager->m_urlBag.c_str());
        m_layerTable->addChild(m_sprBag);
        m_sprBag->setPosition(ccp(m_layerTable->getContentSize().width/2, m_layerTable->getContentSize().height/2));
        m_sprBag->setVisible(false);
        
        m_sprAngryBallon = CCSprite::createWithSpriteFrameName("game_ui_angry_balloon.png");
        //m_layerTable->addChild(m_sprAngryBallon);
        //m_sprAngryBallon->setPosition(ccp(-70, m_layerTable->getContentSize().height/2+30));
        
        m_layerCharacter->addChild(m_sprAngryBallon);
        m_sprAngryBallon->setPosition(ccp(180, 80));
        
        m_sprAngryBallon->runAction(CCRepeatForever::create((CCActionInterval*)(CCSequence::create(CCScaleTo::create(0.25, 1.1), CCScaleTo::create(.25f, 1.0), NULL))));
        m_sprAngryBallon->setVisible(false);
    }
    else
    {
        m_characterInfo = (DyCharacterInfo*)DyCharacterManager::sharedCharacterManager()->m_arrCharaterInfo->objectAtIndex(characterID);
        
        m_heart_start = m_characterInfo->m_heart_start - tycoonManager->m_heart_minus;
        m_markStart = DyCharacterManager::sharedCharacterManager()->getCharacterMark(m_characterInfo->m_id);
        if(m_markStart == DyCharacterMarkStatus_Angry)
            m_heart_start--;
        m_heart_max = m_characterInfo->m_heart_max;
        
        customer->m_rectTouch = CCRectMake(m_posBar.x - m_characterInfo->m_size.width/2, m_posBar.y, m_characterInfo->m_size.width, m_characterInfo->m_size.height);
        
        m_animation = YtAniObject::loadAniObject(this, m_characterInfo->m_url.c_str());

    }
    
	m_layerCharacter->addChild(m_animation);
	m_layerParent->addChild(m_layerCharacter, ZORDER_GAME_CHARACTER+m_customer->m_slotidx);
	m_layerCharacter->setPosition(m_posStart);
    
	m_countHeart = 0;
	m_heartLife = m_heart_start*0.1f;
	m_dtHeart = 0.f;
	m_cntOrder = 0;
    m_heartAni = NULL;
	m_isEat = false;
    
    m_status = DyCharacterStatus_None;
    m_statusFeel = DyCharacterFeelStatus_None;
    setStatusFeel(DyCharacterFeelStatus_Normal);
    
    DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, this, NULL, NULL, DyCharacterStatus_Walkin, m_customer->m_idCharacter<<2 | m_customer->m_slotidx, 0);
    tycoonManager->m_commandManager->addLast(commandTycoon);
    commandTycoon->release();
}


DyCharacter::~DyCharacter()
{
	YtAniObject::unloadAniObject(m_animation);
	if(m_heartAni != NULL)
    {
        m_heartAni->removeFromParentAndCleanup(true);
		YtAniObject::unloadAniObject(m_heartAni);
    }
    m_layerCharacter->removeFromParentAndCleanup(true);
}


void DyCharacter::endAnimationMark(CCNode* node)
{
    node->removeFromParentAndCleanup(true);
}


void DyCharacter::updateCharacterMark()
{
    DyCharacterManager* characterManager = DyCharacterManager::sharedCharacterManager();
    //DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    
    if(m_customer->m_customerType != DyCustomerType_Normal)
        return;
	
    DyCharacterMarkStatus markCur = characterManager->getCharacterMark(m_characterInfo->m_id);
    if(m_markStart != markCur)
    {
        m_markStart = markCur;
        
        CCSprite* sprMark = characterManager->getCharacterMarkSprite(m_characterInfo->m_id);
        
        if(sprMark)
        {
            m_layerParent->addChild(sprMark, ZORDER_GAME_CHARACTER+m_customer->m_slotidx);
            sprMark->setPosition(ccp(m_posBar.x+50, m_posBar.y + 80));
            
            sprMark->runAction(CCSequence::create(CCDelayTime::create(1.0), CCFadeOut::create(1.0), NULL));
            sprMark->runAction(CCSequence::create(CCDelayTime::create(1.0), CCMoveTo::create(1.0, ccp(m_posBar.x+50, m_posBar.y + 120)),
                                                  CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::endAnimationMark)), NULL));
            
            
            for(unsigned int i = 0;i < customerManager->m_arrCustomerOrder->count();i++)
            {
                DyCustomer* customer = (DyCustomer*)customerManager->m_arrCustomerOrder->objectAtIndex(i);
                
                if(customer->m_character->m_status <= DyCharacterStatus_Order)
                    customer->m_character->m_markStart = markCur;
            }
        }
    }
}

void DyCharacter::showHeartCombo(int combo, bool special)
{
    DyLib* lib = DyLib::sharedLib();
    CCLabelBMFont* label;
    
    sprintf(lib->m_strTemp, "%dC", combo);
    if(special)
        label = CCLabelBMFont::create(lib->m_strTemp,  "font_combo_guest_0.fnt");
    else
        label = CCLabelBMFont::create(lib->m_strTemp,  "font_combo_guest_1.fnt");

    m_layerParent->addChild(label, ZORDER_GAME_CHARACTER+m_customer->m_slotidx);
    label->setPosition(ccp(m_posBar.x+20, m_posBar.y + 160));
    
    label->runAction(CCSequence::create(CCDelayTime::create(.2), CCFadeOut::create(.5), NULL));
    label->runAction(CCSequence::create(CCDelayTime::create(.2), CCMoveTo::create(.5, ccp(m_posBar.x+20, m_posBar.y+190)),
                                          CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::endAnimationMark)), NULL));

}



void DyCharacter::setHeartAni(CCNode* node, bool init)
{
//	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	CCLayer* layerHeart = (CCLayer*)node;
	DyCharacter* character = (DyCharacter*)node->getUserData();

	bool isDropLeft = false;
	int nDropIndex = 0;

	for(int i = 0;i < 5;i++)
	{
		CCSprite* sprHeartBg = (CCSprite*)layerHeart->getChildByTag(i);

		sprHeartBg->removeAllChildrenWithCleanup(true);

		if(i >= m_heart_max/2)
			sprHeartBg->setVisible(false);
		else
			sprHeartBg->setVisible(true);
	}

	for(int i = 0;i < character->m_countHeart/2;i++)
	{
		CCSprite* sprHeartBg = (CCSprite*)layerHeart->getChildByTag(i);
		CCSprite* sprHeart;
        
        if(m_customer->m_customerType == DyCustomerType_Delivery)
            sprHeart = CCSprite::createWithSpriteFrameName("game_ui_heart_blue_full.png");
        else
            sprHeart = CCSprite::createWithSpriteFrameName("game_ui_heart_full.png");
        
		sprHeartBg->addChild(sprHeart);
		sprHeart->setAnchorPoint(ccp(0.5,0.5));
		sprHeart->setPosition(ccp(sprHeart->getContentSize().width/2, sprHeart->getContentSize().height/2));
	}

	if(character->m_countHeart%2 == 1)
	{
		CCSprite* sprHeartBg = (CCSprite*)layerHeart->getChildByTag(character->m_countHeart/2);
        CCSprite* sprHeart;
        
        if(m_customer->m_customerType == DyCustomerType_Delivery)
            sprHeart = CCSprite::createWithSpriteFrameName("game_ui_heart_blue_left.png");
        else
            sprHeart = CCSprite::createWithSpriteFrameName("game_ui_heart_left.png");
       sprHeartBg->addChild(sprHeart);

		sprHeart->setAnchorPoint(ccp(0.5,0.5));
		sprHeart->setPosition(ccp(sprHeart->getContentSize().width/2, sprHeart->getContentSize().height/2));
	}

	if(init == false && character->m_countHeart < 10)
	{
		if(character->m_countHeart%2 == 1)
		{
			isDropLeft = false;
			nDropIndex = character->m_countHeart/2;
		}
		else
		{
			isDropLeft = true;
			nDropIndex = character->m_countHeart/2;
		}
	}
	else
		nDropIndex = -1;


	if(nDropIndex >= 0)
	{
		CCSprite* sprHeartDrop;
		CCSprite* sprHeartBg = (CCSprite*)layerHeart->getChildByTag(nDropIndex);

		CCPoint srcPoint = sprHeartBg->getPosition();
		CCPoint tgtPoint;
		if(isDropLeft)
		{
			tgtPoint = ccp(srcPoint.x-5, srcPoint.y-25);
            
            if(m_customer->m_customerType == DyCustomerType_Delivery)
                sprHeartDrop = CCSprite::createWithSpriteFrameName("game_ui_heart_blue_left.png");
            else
                sprHeartDrop = CCSprite::createWithSpriteFrameName("game_ui_heart_left.png");
			
			//sprHeartDrop->setPosition(ccp(srcPoint.x-5, srcPoint.y-5));
			//sprHeartDrop->setRotation(-25);
			sprHeartDrop->setPosition(srcPoint);
			
		}
		else
		{
			tgtPoint = ccp(srcPoint.x+5, srcPoint.y-25);
            
            if(m_customer->m_customerType == DyCustomerType_Delivery)
                sprHeartDrop = CCSprite::createWithSpriteFrameName("game_ui_heart_blue_right.png");
            else
                sprHeartDrop = CCSprite::createWithSpriteFrameName("game_ui_heart_right.png");

			//sprHeartDrop->setPosition(ccp(srcPoint.x+5, srcPoint.y-5));
			//sprHeartDrop->setRotation(25);
			sprHeartDrop->setPosition(srcPoint);
		}

		sprHeartDrop->setAnchorPoint(ccp(0.5, 0.5));

		//CCActionInterval* actionTo = CCMoveTo::create(.5f, tgtPoint);
		CCActionInterval* actionFadeOut = CCFadeOut::create(.5f);

		//sprHeartDrop->runAction(CCSequence::create(actionTo, NULL));	
		sprHeartDrop->runAction(CCSequence::create(actionFadeOut, NULL));	

		layerHeart->addChild(sprHeartDrop, ZORDER_GAME_MOVE_OBJECT);
	}
}


void DyCharacter::updateHeart()
{
    if(m_comming > 0)
        return;
    
	CCLayer* layerHeart = (CCLayer*)m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_HEART_0 + m_customer->m_slotidx);
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
//	DyDataManager* dataManager = DyDataManager::sharedDataManager();

    //if(m_foodReady)
        //return;
    
    float heartVelocity;// 작으면 작을 수록 좋음
	
    //heartVelocity = 0.004f*(100.f-tycoonManager->m_functionGame.m_heart)/100.f;
    heartVelocity = 0.003f*(100.f-tycoonManager->m_functionGame.m_heart)/100.f;

    if(tycoonManager->m_isCrazyMode)
    {
        heartVelocity *= ((float)(tycoonManager->m_crazyHeartVel)/100.f);
        
        heartVelocity *= (0.75f + 0.25f*(float)(tycoonManager->m_recipe_addable_cnt-tycoonManager->m_recipe_add_cnt)/(float)tycoonManager->m_recipe_addable_cnt);
    }
    
    if(m_customer->m_customerType == DyCustomerType_Gaest)
        heartVelocity = 0;
    
  	m_heartLife -= heartVelocity;

	if(m_heartLife > m_heart_max*0.1f)
		m_heartLife = m_heart_max*0.1f;

	int countHeart = (int)(m_heartLife*10.f+0.5f);
	
	if(countHeart < 0)
		countHeart = 0;

	if(m_countHeart != countHeart)
	{
		layerHeart->setUserData(this);
        
		if(countHeart < m_countHeart)
		{
			m_countHeart = countHeart;
            setHeartAni(layerHeart, false);
		}
		else
		{
			m_countHeart = countHeart;
			setHeartAni(layerHeart, true);
		}
	}
    
	if(m_heartLife <= 0)
	{
        DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, this, NULL, NULL, DyCharacterStatus_Walkout, m_customer->m_idCharacter<<2 | m_customer->m_slotidx, 0);
        tycoonManager->m_commandManager->addLast(commandTycoon);
        commandTycoon->release();
	}
    
    if(m_heartLife < 0.21 && !m_checkCandy)
    {
        m_checkCandy = true;
        
        if(DyStaffManager::sharedStaffManager()->processCandy())
        {
            tycoonManager->m_aniCashier->m_actionManager->runAnimations("candy");
            
            DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_CandyToCustomer, NULL, NULL, NULL, m_customer->m_slotidx, 0, 0);
            tycoonManager->m_commandManager->addLast(commandTycoon);
            commandTycoon->release();
        }
    }
    
	if(isAngry())
	{
        setStatusFeel(DyCharacterFeelStatus_Angry);
	}
	else
	{
		setStatusFeel(DyCharacterFeelStatus_Normal);
	}

}


bool DyCharacter::isAngry()
{
    return m_heartLife < ANDGRY_HEARTLIFE;
}


bool DyCharacter::isHappy()
{
    if(m_customer->m_customerType == DyCustomerType_Delivery)
        return true;
    return m_heartLife > LIKE_HEARTLIFE;
}

//void DyCharacter::setOrders(CCArray* arrOrders)
//{
//	m_arrOrders = arrOrders;
//}


void DyCharacter::endAnimationWalkin(CCNode* node)
{
	DyCharacter* character = (DyCharacter*)node->getUserData();
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    
    if(character->m_customer->m_customerType == DyCustomerType_Delivery)
    {
        DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, character, NULL, NULL, DyCharacterStatus_Walkout, character->m_customer->m_idCharacter<<2 | character->m_customer->m_slotidx, 0);
        tycoonManager->m_commandManager->addLast(commandTycoon);
        commandTycoon->release();
    }
    else
    {
        DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, character, NULL, NULL, DyCharacterStatus_Order, character->m_customer->m_idCharacter<<2 | character->m_customer->m_slotidx, 0);
        tycoonManager->m_commandManager->addLast(commandTycoon);
        commandTycoon->release();
    }
}

void DyCharacter::endAnimationLike(CCNode* node)
{
	DyCharacter* character = (DyCharacter*)node->getUserData();

	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, character, NULL, NULL, DyCharacterStatus_Eat, character->m_customer->m_idCharacter<<2 | character->m_customer->m_slotidx, 0);
	tycoonManager->m_commandManager->addLast(commandTycoon);
	commandTycoon->release();
}


void DyCharacter::endAnimationEat(CCNode* node)
{
	DyCharacter* character = (DyCharacter*)node->getUserData();

	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, character, NULL, NULL, DyCharacterStatus_Walkout, character->m_customer->m_idCharacter<<2 | character->m_customer->m_slotidx, 0);
	tycoonManager->m_commandManager->addLast(commandTycoon);
	commandTycoon->release();
}


void DyCharacter::endAnimationWalkout(CCNode* node)
{
	DyCharacter* character = (DyCharacter*)node->getUserData();

	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
	DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, character, NULL, NULL, DyCharacterStatus_Destroy, character->m_customer->m_idCharacter<<2 | character->m_customer->m_slotidx, 0);
	tycoonManager->m_commandManager->addLast(commandTycoon);
	commandTycoon->release();
	//node->removeFromParentAndCleanup(true);
}


void DyCharacter::setStatusFeel(DyCharacterFeelStatus status)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
	if(m_status != DyCharacterStatus_Order)
	{
		return;
	}
    if(m_statusFeel == status)
        return;
    
	m_statusFeel = status;
    
	switch(m_statusFeel)
	{
		case DyCharacterFeelStatus_Normal:
            if(m_customer->m_customerType != DyCustomerType_Delivery)
                m_animation->m_actionManager->runAnimations("order");
			break;
		case DyCharacterFeelStatus_Angry:
            if(rand()%2 == 0)
                YtLib::sharedLib()->playEffect(YtLibSndEff_ANGRY);
            else
                YtLib::sharedLib()->playEffect(YtLibSndEff_ANGRY2);
            
            if(m_customer->m_customerType == DyCustomerType_Delivery)
                m_sprAngryBallon->setVisible(true);
            else
                m_animation->m_actionManager->runAnimations("angry");
            
            tycoonManager->questProcess(QUESTTAG_ETC, "ANGRY", 1);
            tycoonManager->questProcess(QUESTTAG_ETC, "NOTANGRY", 0);
            
            tycoonManager->m_combo_heart = 0;
            
			break;
        default:
            break;
	}
}


//void DyCharacter::endAnimationHeart(CCNode* node)
//{
//	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
//
//	CCInteger* heart = (CCInteger*)node->getUserData();
//
//	DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_AddHeartReal, NULL, NULL, NULL, heart->getValue()/2, 0, 0);
//	tycoonManager->m_commandManager->addLast(commandTycoon);
//	commandTycoon->release();
//	heart->release();
//
//	node->removeFromParentAndCleanup(true);
//    
//    //
//    // 하트가 붙는 애니메이션
//    CCSprite* sprHeart = (CCSprite*)tycoonManager->m_layer->getChildByTag(GAME_UI_TAG_DISP_HEART_ICON);
//    CCSprite* sprHeartEff = (CCSprite*)sprHeart->getChildByTag(GAME_UI_TAG_DISP_HEART_EFF);
//    CCSprite* sprGlow = (CCSprite*)sprHeart->getChildByTag(GAME_UI_TAG_DISP_HEART_GLOW);
//    
//    sprHeartEff->setOpacity(255);
//    sprGlow->setOpacity(0);
//    
//    sprHeart->setScale(1.0f);
//    sprHeart->runAction(CCSequence::create(
//                                          CCScaleTo::create(0.4f, 1.2f),
//                                          CCScaleTo::create(0.4f, 1.0f),
//                                          NULL
//                                          ));
//    sprGlow->runAction(CCSequence::create(
//                                          CCFadeIn::create(0.4f),
//                                          CCFadeOut::create(0.4f),
//                                          NULL
//                                          ));
//    sprHeartEff->runAction(CCSequence::create(
//                                        CCDelayTime::create(0.75f),
//                                        CCFadeOut::create(0.05f),
//                                        NULL
//                                        ));
//
//}


void DyCharacter::setDon(CCLayer* layer)
{
	int pay = m_customer->m_pay*m_customer->m_payMux;
	int unit = 100;

	int count = pay/unit;

	//count = 20;

	int index = 0;

	if(count > 20)
		count = 20;
    if(count == 0)
        count = 1;

    int height = 7;
	while(index < count)
	{
		int x = layer->getContentSize().width/2;
		int y = layer->getContentSize().height/2;

		if(index < 7)
		{
			x = layer->getContentSize().width/2;
			y = layer->getContentSize().height/2;

			y += height*(index%7);
		}
		else if(index < 12)
		{
			x = layer->getContentSize().width/2 - 20;
			y = layer->getContentSize().height/2 - 10;

			y += height*((index-7)%5);
		}
		else if(index < 20)
		{
			x = layer->getContentSize().width/2 + 30;
			y = layer->getContentSize().height/2 - 20;

			y += height*((index-12)%8);
		}

		CCSprite* sprGold = CCSprite::createWithSpriteFrameName("game_ui_desk_gold.png");
		sprGold->setPosition(ccp(x, y));
		layer->addChild(sprGold);
		index++;
	}

}


void DyCharacter::endAnimationLikeByCandy(CCNode* node)
{
	DyCharacter* character = (DyCharacter*)node->getUserData();

	if(character->m_status == DyCharacterStatus_Order)
		character->m_animation->m_actionManager->runAnimations("order");
}


void DyCharacter::eatCandy()
{
    //DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
	m_heartLife = m_heart_max*0.1f;
	updateHeart();

	m_animation->m_actionManager->runAnimations("like");

	CCCallFunc* callfun = CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::endAnimationLikeByCandy));
	CCFiniteTimeAction *seq = CCSequence::create(CCDelayTime::create(m_animation->m_actionManager->getRunningSequenceDuration()), callfun, NULL);
	m_layerCharacter->runAction(seq);
	m_layerCharacter->setUserData(this);
}


void DyCharacter::endAnimationDoubleEffect(CCNode* node)
{
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
	DyCharacter* character = (DyCharacter*)node->getUserData();

    DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, character, NULL, NULL, DyCharacterStatus_Order, character->m_customer->m_idCharacter<<2 | character->m_customer->m_slotidx, 0);
    tycoonManager->m_commandManager->addLast(commandTycoon);
    commandTycoon->release();
}


void DyCharacter::showDoubleEffect()
{
	CCSprite* sprBallon = CCSprite::createWithSpriteFrameName("game_ui_alarm_doubleorder_bg.png");
	CCSprite* sprFeel = CCSprite::createWithSpriteFrameName("game_ui_alarm_doubleorder.png");

	sprBallon->setAnchorPoint(ccp(0, 0.3));
	sprBallon->setPosition(ccp(50, 130));

	sprBallon->setScale(0.3f);
	m_animation->addChild(sprBallon);
	sprBallon->addChild(sprFeel);


	sprFeel->setPosition(ccp(45, 37+15));
	sprFeel->setOpacity(0);
	sprBallon->setUserData(this);

	sprBallon->runAction(CCSequence::create(
		CCScaleTo::create(0.3f, 1),
		CCDelayTime::create(0.61f),
		CCFadeOut::create(0.2f),
		CCCallFuncN::create(this, callfuncN_selector(DyCharacter::endAnimationDoubleEffect)),
		NULL));

	sprFeel->runAction(CCSequence::create(
		CCDelayTime::create(0.3f), 
		CCFadeIn::create(0.01f),
		CCMoveTo::create(0.2f, ccp(45, 37-10)),
		CCMoveTo::create(0.2f, ccp(45, 37+5)),
		CCMoveTo::create(0.2f, ccp(45, 37)),
		CCFadeOut::create(0.2f),
		NULL));

	m_animation->m_actionManager->runAnimations("order");
	m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_DISH_0 + m_customer->m_slotidx)->setVisible(false);
}


void DyCharacter::hangUp(CCNode* node)
{
    DyCharacter* character = (DyCharacter*)node->getUserData();
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    //tycoonManager->m_layer->unschedule(schedule_selector(DyCharacter::hangUp));
    
    
    DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, character, NULL, NULL, DyCharacterStatus_Walkout, character->m_customer->m_idCharacter<<2 | character->m_customer->m_slotidx, 0);
    tycoonManager->m_commandManager->addLast(commandTycoon);
    commandTycoon->release();
}


void DyCharacter::setCharacterStatus(DyCharacterStatus status)
{
	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    DyCustomerManager* customerManager = DyCustomerManager::sharedCustomerManager();
    DyCharacterManager* characterManager = DyCharacterManager::sharedCharacterManager();
    
    if(m_status == status)
        return;
    
    if(m_status == DyCharacterStatus_Destroy)
        return;
    
	m_status = status;
    
    if(m_status != DyCharacterStatus_Destroy)
        m_layerCharacter->stopAllActions();
    
	switch(m_status)
	{
		case DyCharacterStatus_Doubling:
			{
             	showDoubleEffect();
			}
			break;
		case DyCharacterStatus_Order:
			{
				CCLayer* layerBlockInfo = (CCLayer*)m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_BLOCK_0 + m_customer->m_slotidx);
				layerBlockInfo->setVisible(true);
                
                if(m_customer->m_customerType == DyCustomerType_Delivery)
                {
                    m_sprPhone->setVisible(false);
                    m_sprPhone->stopAllActions();
                    m_layerTable->stopAllActions();
                    
                    if(characterManager->m_idSndCall >= 0)
                    {
                        YtLib::sharedLib()->stopEffect(characterManager->m_idSndCall);
                        characterManager->m_idSndCall = -1;
                    }
                    YtLib::sharedLib()->playEffect(YtLibSndEff_DELIVERY_ACCEPT);
                
                    
                }
                else
                {
                    if(strcmp(m_animation->m_actionManager->getRunningSequenceName(), "order") != 0)
                        m_animation->m_actionManager->runAnimations("order");
                }

				m_customer->addPay(0, 1);
				m_customer->updateStatus(true);
				updateHeart();
	
				m_cntOrder++;
				m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_HEART_0 + m_customer->m_slotidx)->setVisible(true);
			}
			break;
		case DyCharacterStatus_Walkin:
			{
                if(m_customer->m_customerType == DyCustomerType_Delivery)
                {
                    m_layerTable->setVisible(true);
                    m_layerTable->setUserData(this);
                    
                    
                    DyTutorialManager* tutorialManager = DyTutorialManager::sharedTutorialManager();
                    DyTutorial* tutorial = tutorialManager->getCurrentTutorial();
                    
                    if(!(tutorial && tutorialManager->m_index == 6))
                    {
                        m_layerTable->runAction(CCSequence::create(CCDelayTime::create(10.0), CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::hangUp)), NULL));
                    }
                    
                    
                }
                else
                {
                    m_animation->m_actionManager->runAnimations("walkin");
                    CCCallFunc* callfun = CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::endAnimationWalkin));
                    CCActionInterval* moveto = CCMoveTo::create(.5f, m_posBar);
                    CCFiniteTimeAction *seq = CCSequence::create(moveto, callfun, NULL);
                    m_layerCharacter->runAction(seq);
                    m_layerCharacter->setUserData(this);
                
                    YtLib::sharedLib()->playEffect(YtLibSndEff_CHARACTER_ENTER);
                }
                
			}
			break;
		case DyCharacterStatus_Like:
			m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_PRICE_0 + m_customer->m_slotidx)->setVisible(false);
			tycoonManager->m_labelDon[m_customer->m_slotidx]->setVisible(false);
			tycoonManager->m_sprDonIcon[m_customer->m_slotidx]->setVisible(false);
			m_customer->updateStatus(false);
            
            if(m_customer->m_customerType == DyCustomerType_Delivery)
            {
                m_sprAngryBallon->setVisible(false);
                m_sprBag->setVisible(true);
                m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_DISH_0 + m_customer->m_slotidx)->setVisible(false);
                m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_HEART_0 + m_customer->m_slotidx)->setVisible(false);
                
                DyTutorial* tutorial = DyTutorialManager::sharedTutorialManager()->getCurrentTutorial();
                
                if(tutorial && tutorial->m_command_type == DyTutorialCommandType_DeliveryMake)
                {
                    DyTutorialManager::sharedTutorialManager()->removeTutorial(tutorial);
                }
            }
            else
            {
                if(isHappy())
                {
                    m_animation->m_actionManager->runAnimations("like");

                    CCCallFunc* callfun = CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::endAnimationLike));
                    CCFiniteTimeAction *seq = CCSequence::create(CCDelayTime::create(m_animation->m_actionManager->getRunningSequenceDuration()), callfun, NULL);
                    m_layerCharacter->runAction(seq);
                    m_layerCharacter->setUserData(this);

                    
                    if(m_customer->m_customerType == DyCustomerType_Normal && m_markStart != DyCharacterMarkStatus_None)
                    {
                        m_customer->m_payTip = (float)(10.f*m_heartLife + (float)m_customer->m_pay/20.f);
                        m_customer->m_payTip *= ((float)m_customer->m_character->m_characterInfo->m_tip_percent/100.f);
                        m_customer->m_payTip *= (float)(100 + tycoonManager->m_functionGame.m_tip)/100.f;
                        
                        if(m_markStart == DyCharacterMarkStatus_Angry)
                        {
                            m_customer->m_payTip *= ((float)m_customer->m_character->m_characterInfo->m_angry_tip/100.f);
                        }
                        
                        else if(m_markStart == DyCharacterMarkStatus_Smile)
                        {
                            m_customer->m_payTip *= ((float)m_customer->m_character->m_characterInfo->m_smile_tip/100.f);
                        }
                    }
                }
                else
                {
                    DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, this, NULL, NULL, DyCharacterStatus_Eat, m_customer->m_idCharacter<<2 | m_customer->m_slotidx, 0);
                    tycoonManager->m_commandManager->addLast(commandTycoon);
                    commandTycoon->release();
                    m_customer->m_payTip = 0;
                }
                
                m_isEat = true;
            }
						
			break;
		case DyCharacterStatus_Eat:
            if(m_customer->m_customerType == DyCustomerType_Delivery)
            {
                m_sprBag->setDisplayFrame(CCSprite::createWithSpriteFrameName(tycoonManager->m_urlBag_disable.c_str())->displayFrame());
                
                m_isEat = true;
                m_animation->m_actionManager->runAnimations("walkin");
                CCCallFunc* callfun = CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::endAnimationWalkin));
                CCActionInterval* moveto = CCMoveTo::create(.5f, m_posBar);
                CCFiniteTimeAction *seq = CCSequence::create(moveto, callfun, NULL);
                m_layerCharacter->runAction(seq);
                m_layerCharacter->setUserData(this);
                
                YtLib::sharedLib()->playEffect(YtLibSndEff_CHARACTER_ENTER);
            }
            else
			{
             	if(rand()%2 == 0)
					YtLib::sharedLib()->playEffect(YtLibSndEff_EAT);
				else
					YtLib::sharedLib()->playEffect(YtLibSndEff_EAT2);

				m_animation->m_actionManager->runAnimations("eat");

				CCCallFunc* callfun = CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::endAnimationEat));
				CCFiniteTimeAction *seq = CCSequence::create(CCDelayTime::create(2.0f), callfun, NULL);
				m_layerCharacter->runAction(seq);
				m_layerCharacter->setUserData(this);
                
                int max_heart_plus = 0;
                for(unsigned int i = 0;i < m_customer->m_arrOrderRecipe->count();i++)
				{
					DyOrderRecipe* orderRecipe = (DyOrderRecipe*)m_customer->m_arrOrderRecipe->objectAtIndex(i);
					if(max_heart_plus < orderRecipe->m_info->m_heart_plus)
                        max_heart_plus = orderRecipe->m_info->m_heart_plus;
				}

                m_heartLife += max_heart_plus*0.1f;
                updateHeart();
			}
			break;
		case DyCharacterStatus_Walkout:
            if(m_customer->m_customerType == DyCustomerType_Normal)
                tycoonManager->questProcess(QUESTTAG_GUEST, m_characterInfo->m_tag.c_str(), m_isEat /*&& isHappy()*/ ? 1:0);
            else if(m_customer->m_customerType == DyCustomerType_Delivery)
            {
                m_sprAngryBallon->removeFromParentAndCleanup(false);

                tycoonManager->questProcess(QUESTTAG_GUEST, "delivery", m_isEat /*&& isHappy()*/ ? 1:0);
            }
            
            if(m_isEat)
			{
                //m_foodReady = false;
				updateHeart();

				for(unsigned int i = 0;i < m_customer->m_arrOrderRecipe->count();i++)
				{
					DyOrderRecipe* orderRecipe = (DyOrderRecipe*)m_customer->m_arrOrderRecipe->objectAtIndex(i);
					m_customer->m_arrSellRecipe->addObject(orderRecipe->m_info);
				}

                if(tycoonManager->m_isCrazyMode && tycoonManager->m_crazyRemainHeart == 0)
                {
                    //
                    // 이중주문 못하게
                    m_cntOrder = 2;
                }
                
				if(m_customer->m_customerType == DyCustomerType_Normal && m_cntOrder == 1 && m_characterInfo->m_type == DyCharacterType_Double)
				{
                  	if(customerManager->m_isCreatableCustomer && rand()%100 < tycoonManager->m_double_probability)
					{
						m_isEat = false;
						tycoonManager->setOrderRecipe(m_customer);
                        DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, this, NULL, NULL, DyCharacterStatus_Doubling, m_customer->m_idCharacter<<2 | m_customer->m_slotidx, 0);
                        tycoonManager->m_commandManager->addLast(commandTycoon);
                        commandTycoon->release();
						break;
					}
				}

                customerManager->m_cntCustomerSuccess++;
                //CCLog("먹고 간 동물 갯수 = %d", tycoonManager->m_cntCustomerSuccess);

                
				m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_DISH_0 + m_customer->m_slotidx)->setVisible(false);
				
				m_layerTable->setVisible(true);
				m_layerTable->removeAllChildrenWithCleanup(true);
                m_sprPhone = NULL;
				m_layerTable->stopAllActions();
				
				setDon(m_layerTable);
                
                

				m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_HEART_0 + m_customer->m_slotidx)->setVisible(false);

                
                if(isHappy())
                {
//                    if(!tycoonManager->m_isCrazyMode)
//                    {
//                        CCLayer* layerHeart = (CCLayer*)m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_HEART_0 + m_customer->m_slotidx);
//                        
//                        CCInteger* heart = CCInteger::create(m_countHeart);
//                        heart->retain();
//                        
//                        CCSprite* sprHeart = CCSprite::createWithSpriteFrameName("game_ui_gage_heart.png");
//                        sprHeart->setPosition(ccp(layerHeart->getPosition().x, layerHeart->getPosition().y));
//                        m_layerParent->addChild(sprHeart, ZORDER_GAME_MOVE_OBJECT);
//                        sprHeart->setUserData(heart);
//                        float distance = ccpDistance(sprHeart->getPosition(), m_layerParent->getChildByTag(GAME_UI_TAG_DISP_HEART_ICON)->getPosition());
//                        CCActionInterval* moveto = CCMoveTo::create(0.001f*distance, m_layerParent->getChildByTag(GAME_UI_TAG_DISP_HEART_ICON)->getPosition());
//                        CCActionInterval* move_ease_in = CCEaseIn::create(moveto, 0.2f);
//                        
//                        CCCallFunc* callfun = CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::endAnimationHeart));
//                        CCFiniteTimeAction *seq = CCSequence::create(move_ease_in, callfun, NULL);
//                        sprHeart->runAction(seq);
//                        sprHeart->runAction(CCSequence::create(CCFadeTo::create(0.001f*distance, 0), NULL));
//                    }

                    
                    tycoonManager->m_combo_heart++;
                    tycoonManager->m_customer_happy_cnt++;

                    bool special = false;
                    
                    if(tycoonManager->m_combo_heart >= COMBO_HEART_CANDY && tycoonManager->m_combo_heart%COMBO_HEART_CANDY == 0)
                    {
//                        TmCommand* command = new TmCommand(TmCommandType_AddSpeicalBlock, tycoonManager->m_board, NULL, NULL, TmBlockType_Candy, 0);
//                        tycoonManager->m_board->m_commandManager->addLast(command);
//                        command->release();
                        
                        tycoonManager->m_board->m_arrMustCreateSpecialBlock->addObject(CCInteger::create(TmBlockType_Candy));
                        
                        special = true;
                    }
                    if(tycoonManager->m_combo_heart >= COMBO_HEART_MSG && tycoonManager->m_combo_heart%COMBO_HEART_MSG == 0)
                    {
//                        TmCommand* command2 = new TmCommand(TmCommandType_AddSpeicalBlock, tycoonManager->m_board, NULL, NULL, TmBlockType_MSG, 0);
//                        tycoonManager->m_board->m_commandManager->addLast(command2);
//                        command2->release();
                        
                        tycoonManager->m_board->m_arrMustCreateSpecialBlock->addObject(CCInteger::create(TmBlockType_MSG));
                        
                        special = true;
                    }
                    
                    if(tycoonManager->m_combo_heart >= 2)
                    {
                        this->showHeartCombo(tycoonManager->m_combo_heart, special);
                    }
                    
                    if(tycoonManager->m_isCrazyMode)
                    {
                        if((tycoonManager->m_customer_happy_cnt > 0 && tycoonManager->m_customer_happy_cnt%15 == 0) ||
                           (tycoonManager->m_customer_happy_cnt > 15 && (tycoonManager->m_customer_happy_cnt-15)%10 == 0))
                        {
                            TmCommand* command = new TmCommand(TmCommandType_AddSpeicalAttr, tycoonManager->m_board, NULL, NULL, TmBlockAttr_Heart, 1);
                            tycoonManager->m_board->m_commandManager->addLast(command);
                            command->release();
                        }
                    }
                    
                    if(m_customer->m_customerType == DyCustomerType_Delivery)
                    {
                        //m_sprBag->setVisible(false);
                    }
                    else if(m_customer->m_customerType == DyCustomerType_Normal && !tycoonManager->m_isCrazyMode)
                    {
                        DyDataManager* dataManager = DyDataManager::sharedDataManager();
                        
                        if(DyCharacterManager::sharedCharacterManager()->isDangol(m_customer->m_character->m_characterInfo->m_id))
                        {
                            signed char count = dataManager->character_get_heart(m_customer->m_character->m_characterInfo->m_id);
                            
                            if(count < MAX_SCHARNUM)
                            {
                                dataManager->character_set_heart(m_customer->m_character->m_characterInfo->m_id, count + 1);
                                dataManager->checkAwards(DyAwardsCheckType_Character, true);
                            }
                            
                            count = dataManager->character_get_eat(m_customer->m_character->m_characterInfo->m_id);
                            if(count <= m_customer->m_character->m_characterInfo->m_status_smile_max)
                            {
                                dataManager->character_eat_inc(m_customer->m_character->m_characterInfo->m_id);
                            }
                        }
                    }
                }
                else
                {
                    tycoonManager->m_combo_heart = 0;
                }
                
                if(m_customer->m_customerType == DyCustomerType_Delivery)
                {
                    tycoonManager->sellRecipe(m_customer, false);
                    YtLib::sharedLib()->playEffect(YtLibSndEff_DELIVERY_BIKE);
                }
			}
			else
			{
				m_customer->m_pay = 0;
                
                if(m_customer->m_customerType == DyCustomerType_Normal && !tycoonManager->m_isCrazyMode)
                {
                    DyDataManager* dataManager = DyDataManager::sharedDataManager();
                    
                    if(DyCharacterManager::sharedCharacterManager()->isDangol(m_customer->m_character->m_characterInfo->m_id))
                    {   
                        signed char count = dataManager->character_get_eat(m_customer->m_character->m_characterInfo->m_id);
                        if(count >= -m_customer->m_character->m_characterInfo->m_status_angry_max)
                        {
                            dataManager->character_eat_dec(m_customer->m_character->m_characterInfo->m_id);
                        }
                    }
                }
			}
            
            updateCharacterMark();
            
			tycoonManager->m_labelDon[m_customer->m_slotidx]->setVisible(false);
			tycoonManager->m_sprDonIcon[m_customer->m_slotidx]->setVisible(false);
			m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_PRICE_0 + m_customer->m_slotidx)->setVisible(false);
			m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_BALLOON_0 + m_customer->m_slotidx)->setVisible(false);
			m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_DISH_0 + m_customer->m_slotidx)->setVisible(false);
			m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_HEART_0 + m_customer->m_slotidx)->setVisible(false);
			m_layerParent->getChildByTag(GAME_UI_TAG_SLOT_BLOCK_0 + m_customer->m_slotidx)->setVisible(false);
            
        
            if(m_customer->m_customerType == DyCustomerType_Delivery && !m_isEat)
            {
                if(m_sprPhone)
                {
                    m_sprPhone->stopAllActions();
                    m_sprPhone->setVisible(false);
                }
                
             	DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
                DyTycoonCommand* commandTycoon = new DyTycoonCommand(DyTycoonCommandType_ChagneStatus, this, NULL, NULL, DyCharacterStatus_Destroy, m_customer->m_idCharacter<<2 | m_customer->m_slotidx, 0);
                tycoonManager->m_commandManager->addLast(commandTycoon);
                commandTycoon->release();
            }
            else
            {
                if(m_customer->m_customerType == DyCustomerType_Normal && this->m_characterInfo->m_id >= 8)
                {
                    m_animation->setScaleX(-1.f);
                    m_animation->m_actionManager->runAnimations("walkin");
                }
                else
                {
                    DyStageManager* stageManager = DyStageManager::sharedStageManager();

                    if(m_customer->m_customerType == DyCustomerType_Delivery && stageManager->m_curShopType == DyStageType_Pizza)
                        m_animation->m_actionManager->runAnimations("walkout_pizza");
                    else
                        m_animation->m_actionManager->runAnimations("walkout");
                }
                m_layerCharacter->runAction(CCSequence::create(CCMoveTo::create(.6f, m_posStart), CCCallFuncN::create(m_layerParent, callfuncN_selector(DyCharacter::endAnimationWalkout)), NULL));
                m_layerCharacter->setUserData(this);
            }
            
			if(!m_isEat)
                tycoonManager->addCrazyHeart(-1);
            
			break;
		default:
            break;
	}
}


DyCharacterManager::DyCharacterManager()
{
    m_idSndCall = -1;
	m_arrCharaterInfo = new CCArray();

	YtObjectGroup* chracterinfo = DyLoader::sharedLoader()->loadObjects("character.txt");

	for(int i = 0;i < chracterinfo->count();i++)
	{
		DyCharacterInfo* characterInfo = new DyCharacterInfo();

		characterInfo->m_id = atoi(chracterinfo->getValue("id", i));
		characterInfo->m_tag = chracterinfo->getValue("tag", i);
		characterInfo->m_name = chracterinfo->getValue("name", i);
		characterInfo->m_url = chracterinfo->getValue("resource", i);
		characterInfo->m_explain = chracterinfo->getValue("explain_kor", i);

		const char* type = chracterinfo->getValue("type", i);
		if(strcmp(type, "double") == 0)
			characterInfo->m_type = DyCharacterType_Double;
		else
			characterInfo->m_type = DyCharacterType_Single;

		CCArray* arrSize = chracterinfo->getArray("size", i);
		characterInfo->m_size.width = atoi(((CCString*)arrSize->objectAtIndex(0))->getCString());
		characterInfo->m_size.height = atoi(((CCString*)arrSize->objectAtIndex(1))->getCString());

		characterInfo->m_heart_start = atoi(chracterinfo->getValue("heart_start", i));
		characterInfo->m_heart_max = atoi(chracterinfo->getValue("heart_max", i));
        characterInfo->m_tip_percent = atoi(chracterinfo->getValue("tip_percent", i));
        
        
        characterInfo->m_status_angry_max = atoi(chracterinfo->getValue("status_angry_max", i));
        
        characterInfo->m_status_angry_from = atoi(chracterinfo->getValue("status_angry", i));
        
        characterInfo->m_status_smile_max = atoi(chracterinfo->getValue("status_smile_max", i));
        
        characterInfo->m_status_smile_from = atoi(chracterinfo->getValue("status_smile", i));
        
        characterInfo->m_angry_tip = atoi(chracterinfo->getValue("angry_tip", i));
        characterInfo->m_smile_tip = atoi(chracterinfo->getValue("smile_tip", i));

      	characterInfo->m_isAvailable = false;

		m_arrCharaterInfo->addObject(characterInfo);
		characterInfo->release();
	}
}


DyCharacterManager::~DyCharacterManager()
{
	m_arrCharaterInfo->release();
}



void DyCharacterManager::onPopupClicked(cocos2d::CCObject * pSender)
{
	YtLayerManager::sharedLayerManager()->destoryTopLayer(true);

	if(checkNewCustomerInner() == false)
		m_callFunc(0, 0);
}


void DyCharacterManager::alramNewCustomer(std::string tag)
{
	DyPopup* popup = DyPopup::create(DyPopupType_NewCustomer, this, callfuncO_selector(DyCharacterManager::onPopupClicked));
	popup->setCustomer(tag);
	popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
	YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    YtLib::sharedLib()->playEffect(YtLibSndEff_CANDY);
}


bool DyCharacterManager::checkNewCustomerInner()
{
	bool hasNewCustomer = false;
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyTycoonManager* tycoonManager = DyTycoonManager::sharedTycoonManager();
    
    if(tycoonManager->m_isCrazyMode)
    {
        for(unsigned int i = 0;i < m_arrCharaterInfo->count();i++)
        {
            DyCharacterInfo* characterInfo = (DyCharacterInfo*)m_arrCharaterInfo->objectAtIndex(i);
            
            if(characterInfo->m_isAvailable && dataManager->character_get_see(characterInfo->m_id) == false)
            {
                dataManager->character_set_see(characterInfo->m_id, true);
                alramNewCustomer(characterInfo->m_tag);
                hasNewCustomer = true;
                dataManager->dataSave();
                break;
            }
        }

    }
    else
    {
        CCArray* animals = stageManager->m_curShopInfo->getArray("animal", stageManager->m_curLevel);

        for(unsigned int i = 0;i < animals->count();i++)
        {
            CCString* animal = (CCString*)animals->objectAtIndex(i);
            DyCharacterInfo* characterInfo = getCharacterInfo(animal->getCString());

            if(characterInfo && dataManager->character_get_see(characterInfo->m_id) == false)
            {
                dataManager->character_set_see(characterInfo->m_id, true);
                alramNewCustomer(animal->getCString());
                hasNewCustomer = true;
                dataManager->dataSave();
                break;
            }
        }
    }
    
    
	return hasNewCustomer;
}

bool DyCharacterManager::checkNewCustomer(YtPlatform_CallFunc callFunc)
{
	m_callFunc = callFunc;
    
	return checkNewCustomerInner();
}

void DyCharacterManager::allInvailable()
{
	for(unsigned int i = 0;i < m_arrCharaterInfo->count();i++)
	{
		DyCharacterInfo* characterInfo = (DyCharacterInfo*)m_arrCharaterInfo->objectAtIndex(i);
		characterInfo->m_isAvailable = false;
	}
	m_cntAvailable = 0;
}





void DyCharacterManager::setAvailable(const char* tag, bool isAvailable)
{
	for(unsigned int i = 0;i < m_arrCharaterInfo->count();i++)
	{
		DyCharacterInfo* characterInfo = (DyCharacterInfo*)m_arrCharaterInfo->objectAtIndex(i);

		if(strcmp(characterInfo->m_tag.c_str(), tag) == 0 && !characterInfo->m_isAvailable)
		{
			characterInfo->m_isAvailable = isAvailable;
			characterInfo->retain();
			m_arrCharaterInfo->removeObject(characterInfo);
			m_arrCharaterInfo->insertObject(characterInfo, 0);
			characterInfo->release();
			m_cntAvailable++;
			return;
		}
	}
}


int DyCharacterManager::getCharacterTotalCnt(int shopType)
{
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    CCArray* animals = stageManager->m_shopinfo->getArray("animals", stageManager->m_curShopType);
   
    return animals->count();
    //return m_arrCharaterInfo->count();
}


int DyCharacterManager::getCharacterDangolCnt(int shopType, bool isCrazyMode)
{
//    int cnt = 0;
//    DyDataManager* dataManager = DyDataManager::sharedDataManager();
//    
//    for(unsigned int i = 0;i < m_arrCharaterInfo->count();i++)
//	{
//        DyCharacterInfo* characterInfo = (DyCharacterInfo*)m_arrCharaterInfo->objectAtIndex(i);
//
//		if(dataManager->m_data.m_awards.m_service[i].m_complete_count > 0)
//        {
//            cnt++;
//        }
//	}
//    return cnt;
    
    int cnt = 0;
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyCharacterManager* characterManager = DyCharacterManager::sharedCharacterManager();
    
    CCArray* animals = stageManager->m_shopinfo->getArray("animals", stageManager->m_curShopType);
    
    for(unsigned int i = 0;i < animals->count();i++)
    {
        CCString* animal = (CCString*)animals->objectAtIndex(i);
        
        DyCharacterInfo* infoAnimal = characterManager->getCharacterInfo(animal->getCString());
        
        if(dataManager->awards_get_complete(4, infoAnimal->m_id))
            cnt++;
    }
    
    return cnt;
}



bool DyCharacterManager::isDangol(int index)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    return dataManager->awards_get_complete(4, index);
}


bool DyCharacterManager::setDangol(int index)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    
    // 강제로 단골 시킴
    if(dataManager->awards_get_complete(4, index) == false)
    {
        dataManager->awards_set_complete(4, index, true);
        dataManager->awards_set_isSawAward(4, index, false);
        dataManager->awards_set_complete_count(4, index, 1);
        return true;
    }

    return false;
}


DyCharacterMarkStatus DyCharacterManager::getCharacterMark(int index)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
        
    if(dataManager->awards_get_complete(4, index) == false)
        return DyCharacterMarkStatus_None;
    
    DyCharacterInfo* characterInfo = getCharacterInfoByID(index);

    signed char heart_cnt = dataManager->character_get_eat(index);
    
    //CCLog("index = %d, heart_cnt = %d", index, heart_cnt);
    
    if(heart_cnt <= characterInfo->m_status_angry_from)
    {
        return DyCharacterMarkStatus_Angry;
    }
    else if(heart_cnt >= characterInfo->m_status_smile_from)
    {
        return DyCharacterMarkStatus_Smile;
    }
    
    return DyCharacterMarkStatus_Normal;
}


CCSprite* DyCharacterManager::getCharacterMarkSprite(int index)
{
    DyCharacterMarkStatus mark = getCharacterMark(index);
    switch(mark)
    {
        case DyCharacterMarkStatus_None:
            return NULL;
        case DyCharacterMarkStatus_Angry:
            return CCSprite::createWithSpriteFrameName("main_ui_guest_icon_angry.png");
        case DyCharacterMarkStatus_Normal:
            return CCSprite::createWithSpriteFrameName("main_ui_guest_icon_normal.png");
        case DyCharacterMarkStatus_Smile:
            return CCSprite::createWithSpriteFrameName("main_ui_guest_icon_smile.png");
    }
    return NULL;
}


DyCharacterInfo* DyCharacterManager::getCharacterInfo(std::string tag)
{
	for(unsigned int i = 0;i < m_arrCharaterInfo->count();i++)
	{
		DyCharacterInfo* characterInfo = (DyCharacterInfo*)m_arrCharaterInfo->objectAtIndex(i);

		if(tag.compare(characterInfo->m_tag) == 0)
		{
			return characterInfo;
		}
	}
	return NULL;
}


DyCharacterInfo* DyCharacterManager::getCharacterInfoByID(int id)
{
	for(unsigned int i = 0;i < m_arrCharaterInfo->count();i++)
	{
		DyCharacterInfo* characterInfo = (DyCharacterInfo*)m_arrCharaterInfo->objectAtIndex(i);

		if(characterInfo->m_id == id)
		{
			return characterInfo;
		}
	}
	return NULL;
}

int DyCharacterManager::getChracterID(std::string tag)
{
   	for(unsigned int i = 0;i < m_arrCharaterInfo->count();i++)
	{
		DyCharacterInfo* characterInfo = (DyCharacterInfo*)m_arrCharaterInfo->objectAtIndex(i);
        
		if(tag.compare(characterInfo->m_tag) == 0)
		{
			return i;
		}
	}
    return -1;
}


DyCharacterManager* DyCharacterManager::sharedCharacterManager()
{
	if(g_sharedCharacterManager == NULL)
	{
		g_sharedCharacterManager = new DyCharacterManager();
	}
	return g_sharedCharacterManager;
}