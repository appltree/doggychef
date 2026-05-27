/*
DyLib.cpp
Doggy chef
@initialize at 120907

@history
120907	yoonsr	initialize
*/


#include "DyConfig.h"
#include "DyPopup.h"


#include "DySceneGame.h"
#include "DySceneTitle.h"
#include "DySceneMainMenu.h"
#include "DySceneAnimationTest.h"
#include "DySceneStage.h"
#include "DySceneCheat.h"
#include "DySceneStory.h"
#include "DySceneTip.h"

#include "DyLoader.h"
#include "DyFriend.h"

#include "YtNetwork.h"
#include "DyRecipe.h"

#include "pugixml.hpp"




DyPopupCostume::DyPopupCostume()
{
}

DyPopupCostume::~DyPopupCostume()
{
}


void DyPopupCostume::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader)
{
    
}



SEL_MenuHandler DyPopupCostume::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName)
{
    return NULL;
}

SEL_CCControlHandler DyPopupCostume::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName)
{
	return NULL;
}

bool DyPopupCostume::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode)
{
    return false;
}


void DyPopupCostume::pause()
{
}


void DyPopupCostume::resume()
{
}


void DyPopupCostume::hide()
{
    //	YtLib* lib = YtLib::sharedLib();
    //
    //	runAction(CCSequence::create(
    //
    //		CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)),
    //		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
    //		CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height)),
    //
    //		CCCallFunc::create(this, callfunc_selector(DyLayerOption::destory)),
    //		NULL));
}


void DyPopupCostume::show()
{
}


DyPopupCostume* DyPopupCostume::create(CCNode* owner, DyCostumeStatus* costumeStatus, DyCostumeFunction* costumeFunction)
{
//    DyLib* lib = DyLib::sharedLib();
    
	DyPopupCostume* popupCostume = (DyPopupCostume*)YtLayerManager::create("DyPopupCostume", DyPopupCostumeLoader::loader(), "layer_popup_costume.ccbi", owner);
    
    popupCostume->m_costumeStatus = costumeStatus;
    popupCostume->m_costumeFunction = costumeFunction;
    
    
//    CCLabelTTF* labelDecay = (CCLabelTTF*)popupCostume->getChildByTag(1000);
//    CCLabelTTF* labelIce = (CCLabelTTF*)popupCostume->getChildByTag(1001);
//    CCLabelTTF* labelGold = (CCLabelTTF*)popupCostume->getChildByTag(1002);
//    
//    CCLabelTTF* labelHeart = (CCLabelTTF*)popupCostume->getChildByTag(1003);
//    CCLabelTTF* labelCooling = (CCLabelTTF*)popupCostume->getChildByTag(1004);
//    
//    labelDecay->setFontName(FONT_NAME_DEFAULT);
//    labelIce->setFontName(FONT_NAME_DEFAULT);
//    labelHeart->setFontName(FONT_NAME_DEFAULT);
//    labelGold->setFontName(FONT_NAME_DEFAULT);
//    labelCooling->setFontName(FONT_NAME_DEFAULT);
//   
//    
//    CCLabelTTF* labelDescDecay = (CCLabelTTF*)popupCostume->getChildByTag(1100);
//    CCLabelTTF* labelDescIce = (CCLabelTTF*)popupCostume->getChildByTag(1101);
//    CCLabelTTF* labelDescGold = (CCLabelTTF*)popupCostume->getChildByTag(1102);
//    
//    CCLabelTTF* labelDescHeart = (CCLabelTTF*)popupCostume->getChildByTag(1103);
//    CCLabelTTF* labelDescCooling = (CCLabelTTF*)popupCostume->getChildByTag(1104);
//    
//    
//    if(costumeFunction->m_decay > 0)
//    {
//        sprintf(lib->m_strTemp, "-%d%%", costumeFunction->m_decay);
//        sprintf(lib->m_strTemp2, "재료블럭 썩는 속도를 %d%% 감소시켜줍니다.", costumeFunction->m_decay);
//    }
//    else
//    {
//        sprintf(lib->m_strTemp, "0%%");
//        sprintf(lib->m_strTemp2, "재료블럭 썩는 속도 감소 능력치");
//    }
//    
//    labelDecay->setString(lib->m_strTemp);
//    labelDescDecay->setString(lib->m_strTemp2);
//    
//    
//    if(costumeFunction->m_ice > 0)
//    {
//        sprintf(lib->m_strTemp, "-%d%%", costumeFunction->m_ice);
//        sprintf(lib->m_strTemp2, "얼음블럭 나오는 확률을 %d%% 감소시켜줍니다.", costumeFunction->m_ice);
//    }
//    else
//    {
//        sprintf(lib->m_strTemp, "0%%");
//        sprintf(lib->m_strTemp2, "얼음블럭 확률 감소 능력치");
//    }
//    labelIce->setString(lib->m_strTemp);
//    labelDescIce->setString(lib->m_strTemp2);
//    
//    
//    if(costumeFunction->m_heart > 0)
//    {
//        sprintf(lib->m_strTemp, "+%d%%", costumeFunction->m_heart);
//        sprintf(lib->m_strTemp2, "손님만족도를 %d%%정도 높혀줍니다.", costumeFunction->m_heart);
//
//    }
//    else
//    {
//        sprintf(lib->m_strTemp, "0%%");
//        sprintf(lib->m_strTemp2, "손님만족도 향상 능력치");
//    }
//    
//    labelHeart->setString(lib->m_strTemp);
//    labelDescHeart->setString(lib->m_strTemp2);
//    
//    if(costumeFunction->m_tip > 0)
//    {
//        sprintf(lib->m_strTemp, "+%d%%", costumeFunction->m_tip);
//        sprintf(lib->m_strTemp2, "손님팁을 %d%%정도 높혀줍니다.", costumeFunction->m_tip);
//    }
//    else
//    {
//        sprintf(lib->m_strTemp, "0%%");
//        sprintf(lib->m_strTemp2, "손님 팁 향상 능력치");
//    }
//    labelGold->setString(lib->m_strTemp);
//    labelDescGold->setString(lib->m_strTemp2);
//    
//    if(costumeFunction->m_cooling > 0)
//    {
//        sprintf(lib->m_strTemp, "-%d%%", costumeFunction->m_cooling);
//        sprintf(lib->m_strTemp2, "특수아이템 쿨타임을 %d%%정도 줄여줍니다.", costumeFunction->m_cooling);
//    }
//    else
//    {
//        sprintf(lib->m_strTemp, "0%%");
//        sprintf(lib->m_strTemp2, "특수아이템 쿨타임 감소 능력치");
//        
//    }
//    labelCooling->setString(lib->m_strTemp);
//    labelDescCooling->setString(lib->m_strTemp2);
    
    
    
	return popupCostume;
}




DyPopup::DyPopup()
{
    m_tag = "DyPopup";

	m_character = NULL;
	m_sprCircleWhiteRT = NULL;
	m_sprCircleBlownRT = NULL;
	m_sprCircleMaskRT = NULL;
	m_rt = NULL;
	m_arrButtons = new CCArray();
    m_resumeAndDestory = true;
}


DyPopup::~DyPopup()
{
	if(m_sprCircleWhiteRT)
		m_sprCircleWhiteRT->release();
	if(m_sprCircleBlownRT)
		m_sprCircleBlownRT->release();
	if(m_sprCircleMaskRT)
		m_sprCircleMaskRT->release();

	if(m_character)
		YtAniObject::unloadAniObject(m_character);
	m_arrButtons->release();
}



DyPopup* DyPopup::create(DyPopupType type)
{
	DyPopup* popup = new DyPopup();
	popup->m_popuptype = type;	
	popup->m_size = CCSizeMake(440, 293);

    popup->m_pListener = NULL;
	popup->m_pfnSelector = NULL;

	popup->initWithColor(ccc4(255, 0, 0, 0), popup->m_size.width, popup->m_size.height);

    
    if(type == DyPopupType_LoadingCycle)
    {
        CCSprite* sprCup = CCSprite::createWithSpriteFrameName("main_ui_loading_cup.png");
        sprCup->setPosition(ccp(0, 0));
        popup->addChild(sprCup);

        popup->m_sprCircleWhite = CCSprite::createWithSpriteFrameName("main_ui_loading_pattern0.png");
        popup->m_sprCircleWhite->setPosition(ccp(-13, 6));
        popup->addChild(popup->m_sprCircleWhite);

        popup->m_sprCircleBlown = CCSprite::createWithSpriteFrameName("main_ui_loading_pattern1.png");
        popup->m_sprCircleBlown->setPosition(ccp(-13, 6));
        popup->addChild(popup->m_sprCircleBlown);
        popup->m_sprCircleBlown->setVisible(false);

        popup->m_sprCircleWhiteRT = CCSprite::createWithSpriteFrameName("main_ui_loading_pattern0.png");
        popup->m_sprCircleWhiteRT->retain();
        popup->m_sprCircleBlownRT = CCSprite::createWithSpriteFrameName("main_ui_loading_pattern1.png");
        popup->m_sprCircleBlownRT->retain();
        popup->m_sprCircleMaskRT = CCSprite::createWithSpriteFrameName("main_ui_loading_mask.png");
        popup->m_sprCircleMaskRT->retain();

        popup->cbCirclePopup(0);
    }


	popup->m_spr9TextBox = CCScale9Sprite::createWithSpriteFrame(CCSprite::createWithSpriteFrameName("menu_ui_box_pattern.png")->displayFrame());
	popup->m_spr9TextBox->setPosition(ccp(0, -80));
	popup->m_spr9TextBox->setAnchorPoint(ccp(0.5,0.5));
	popup->addChild(popup->m_spr9TextBox);

    
	popup->m_content = CCLabelTTF::create("", FONT_NAME_EXP, 25.f);
	popup->m_content->setColor(ccWHITE);//ccc3(64, 64, 64));
	popup->m_content->setAnchorPoint(ccp(0.5, 0.5));
	popup->m_content->setPosition(ccp(0, -80));
	popup->addChild(popup->m_content);

	popup->autorelease();
	

	return popup;
}

void DyPopup::onEnter()
{
	YtLayer::onEnter();
    setKeypadEnabled(true);
}




DyPopup* DyPopup::create(DyPopupType type, CCObject *rec, SEL_CallFuncO selector, CCSize size)
{
	DyPopup* popup = new DyPopup();
	popup->m_popuptype = type;
    
	switch(type)
	{
        case DyPopupType_Text:
            popup->m_size = size;
            break;
        case DyPopupType_GumWarning:
            popup->m_size = CCSizeMake(440, 350);
            break;
		case DyPopupType_Quest:
			popup->m_size = CCSizeMake(440, 350);
            break;
		case DyPopupType_Quest2:
			popup->m_size = CCSizeMake(440, 400);
            break;

		case DyPopupType_NewCustomer:
		case DyPopupType_NewRecipe:
        case DyPopuptype_RandomBox:
            CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("game_ui.plist");
			popup->m_size = CCSizeMake(450, 340); // default size
			break;
        case DyPopupType_Present:
            CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("game_ui.plist");
			popup->m_size = CCSizeMake(450+240, 400); // default size

            break;
        case DyPopupType_Costume:
        case DyPopupType_Costume2:
            CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("game_ui.plist");
			popup->m_size = CCSizeMake(500, 440); // default size
            
            break;
 
        default:
            break;
	}
	
    
    popup->m_pListener = rec;
	popup->m_pfnSelector = selector;
    
	popup->initWithColor(ccc4(255, 0, 0, 0), popup->m_size.width, popup->m_size.height);
    
	CCSprite* sprBgWhite = CCSprite::createWithSpriteFrameName("menu_ui_white_box.png");
	CCScale9Sprite* backgroundWhite = CCScale9Sprite::createWithSpriteFrame(sprBgWhite->displayFrame());
	backgroundWhite->setContentSize(CCSizeMake(popup->m_size.width-20, popup->m_size.height-20));
	popup->addChild(backgroundWhite);
    
	switch(type)
	{
		case DyPopupType_Text:
        case DyPopupType_GumWarning:
        case DyPopupType_Quest:
        case DyPopupType_Quest2:
        {
            YtPatternSprite* sprRace = YtPatternSprite::spriteWithSize(CCSizeMake(popup->m_size.width-20, popup->m_size.height-20),  "pattern_popup_pattern_bottom.png");
            sprRace->setPosition(ccp(0, -popup->m_size.height/2 + 120));
            popup->addChild(sprRace);
        }
			break;
		case DyPopupType_NewCustomer:
		case DyPopupType_NewRecipe:
        case DyPopuptype_RandomBox:
        {
            YtPatternSprite* sprRace = YtPatternSprite::spriteWithSize(CCSizeMake(popup->m_size.width-20, popup->m_size.height-20),  "pattern_popup_pattern2.png");
            sprRace->setPosition(ccp(0, -popup->m_size.height/2 + 210));
            popup->addChild(sprRace);
        }
			break;
        case DyPopupType_Present:
        {
            YtPatternSprite* sprRace = YtPatternSprite::spriteWithSize(CCSizeMake(popup->m_size.width-20, popup->m_size.height-20),  "pattern_popup_pattern_bottom.png");
            sprRace->setPosition(ccp(0, -popup->m_size.height/2 + 50));
            popup->addChild(sprRace);
        }
			break;
            
        case DyPopupType_Costume:
        case DyPopupType_Costume2:
        {
            YtPatternSprite* sprRace = YtPatternSprite::spriteWithSize(CCSizeMake(popup->m_size.width-20, popup->m_size.height-20),  "pattern_popup_pattern_bottom.png");
            sprRace->setPosition(ccp(0, -popup->m_size.height/2 + 50));
            popup->addChild(sprRace);
        }
			break;

        default:
            break;
	}
    
    
	CCSprite* sprBg = CCSprite::createWithSpriteFrameName("menu_ui_popup_mini.png");
	CCScale9Sprite* background = CCScale9Sprite::createWithSpriteFrame(sprBg->displayFrame());
	background->setContentSize(popup->m_size);
	popup->addChild(background);
    
    
	popup->autorelease();
    
    
    popup->m_content = CCLabelTTF::create("", FONT_NAME_EXP, 25.f, CCSizeMake(390,500), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
	popup->m_content->setColor(ccc3(64, 64, 64));
	popup->m_content->setAnchorPoint(ccp(0.5, 0.5));
	popup->m_content->setPosition(ccp(0, 40));

    
	switch(type)
	{
		case DyPopupType_Text:
			break;
		case DyPopupType_GumWarning:
        {
            CCSprite* sprPicture = CCSprite::createWithSpriteFrameName("game_ui_gum_warning.png");
            sprPicture->setPosition(ccp(0, 40));
            popup->addChild(sprPicture);
            
        }

			break;

        case DyPopupType_Quest:
        case DyPopupType_Quest2:
        {
            CCSprite* sprIcon = CCSprite::createWithSpriteFrameName("quest_quest_icon.png");
            sprIcon->setPosition(ccp(-popup->m_size.width/2 + 50, popup->m_size.height/2 - 80));
            popup->addChild(sprIcon);
            
        }
            break;
            
		case DyPopupType_NewCustomer:
        {
            CCSprite* sprTitleBg = CCSprite::createWithSpriteFrameName("menu2_ui_ribbon_minipopup.png");
            sprTitleBg->setPosition(ccp(0, popup->m_size.height/2 - 50));
            popup->addChild(sprTitleBg);
            
            CCSprite* sprTitle = CCSprite::createWithSpriteFrameName("menu2_ui_title_customer_minipopup.png");
            sprTitle->setPosition(ccp(0, popup->m_size.height/2 - 37));
            popup->addChild(sprTitle);
            
            CCSprite* sprTextBox = CCSprite::createWithSpriteFrameName("menu2_ui_character_pannel_minipopup.png");
            sprTextBox->setPosition(ccp(50, 35));
            popup->addChild(sprTextBox);

        }
            break;
		case DyPopupType_NewRecipe:
        {
            CCSprite* sprTitleBg = CCSprite::createWithSpriteFrameName("menu2_ui_ribbon_minipopup.png");
            sprTitleBg->setPosition(ccp(0, popup->m_size.height/2 - 50));
            popup->addChild(sprTitleBg);
            
            CCSprite* sprTitle = CCSprite::createWithSpriteFrameName("menu2_ui_title_recipe_minipopup.png");
            sprTitle->setPosition(ccp(0, popup->m_size.height/2 - 37));
            popup->addChild(sprTitle);
            
            CCSprite* sprPictureBox = CCSprite::createWithSpriteFrameName("menu2_ui_recipe_pannel_minipopup.png");
            sprPictureBox->setPosition(ccp(-110, 35));
            popup->addChild(sprPictureBox);
            
            CCSprite* sprTextBox = CCSprite::createWithSpriteFrameName("menu2_ui_recipe_pannel2_minipopup.png");
            sprTextBox->setPosition(ccp(70, 35));
            popup->addChild(sprTextBox);
            
        }
            break;
		case DyPopuptype_RandomBox:
        {
            CCSprite* sprTitleBg = CCSprite::createWithSpriteFrameName("menu2_ui_ribbon_minipopup.png");
            sprTitleBg->setPosition(ccp(0, popup->m_size.height/2 - 50));
            popup->addChild(sprTitleBg);
            
            CCSprite* sprTitle = CCSprite::createWithSpriteFrameName("menushop_ui_randombox_title.png");
            sprTitle->setPosition(ccp(0, popup->m_size.height/2 - 37));
            popup->addChild(sprTitle);
            
            CCSprite* sprPictureBox = CCSprite::createWithSpriteFrameName("menu2_ui_character_pannel_minipopup.png");
            sprPictureBox->setPosition(ccp(-60, 35));
            popup->addChild(sprPictureBox);
            
            CCSprite* sprTextBox = CCSprite::createWithSpriteFrameName("menushop_ui_randombox_deco.png");
            sprTextBox->setPosition(ccp(100, 35));
            popup->addChild(sprTextBox);
            
            popup->m_content->setDimensions(CCSizeMake(200, 60));
            popup->m_content->setPosition(ccp(-60, 30));
            popup->m_content->setFontSize(23);
        }
            break;

        case DyPopupType_Present:
        {
            CCSprite* sprTitleBg = CCSprite::createWithSpriteFrameName("menu2_ui_ribbon_minipopup.png");
            sprTitleBg->setPosition(ccp(0, popup->m_size.height/2 - 25));
            popup->addChild(sprTitleBg);
            
            CCSprite* sprTitle = CCSprite::createWithSpriteFrameName("menu2_ui_title_present.png");
            sprTitle->setPosition(ccp(0, popup->m_size.height/2 - 12));
            popup->addChild(sprTitle);

            
        }
			break;
        case DyPopupType_Costume:
        case DyPopupType_Costume2:
        {
            CCSprite* sprTitleBg = CCSprite::createWithSpriteFrameName("menu2_ui_ribbon_minipopup.png");
            sprTitleBg->setPosition(ccp(0, popup->m_size.height/2 - 50+20));
            popup->addChild(sprTitleBg);
            
            CCLabelTTF* labelTitle = CCLabelTTF::create("커스튬 능력 설명", FONT_NAME_EXP, 30.f);
            labelTitle->setPosition(ccp(0, popup->m_size.height/2 - 50+30));
            labelTitle->setColor(ccc3(42,21,5));
            popup->addChild(labelTitle);
            
            CCSprite* sprIcon = CCSprite::createWithSpriteFrameName("menu_ui_info.png");
            sprIcon->setPosition(ccp(-130, popup->m_size.height/2 - 50+30));
            sprIcon->setColor(ccc3(42,21,5));
            popup->addChild(sprIcon);
            
        }
            break;
        default:
            break;
	}
	popup->addChild(popup->m_content);
    
	popup->m_menu = CCMenu::create();
	popup->m_menu->setPosition(CCPointZero);
    
	popup->addChild(popup->m_menu, 5);

	return popup;
}




DyPopup* DyPopup::create(DyPopupType type, CCObject *rec, SEL_CallFuncO selector)
{
    return DyPopup::create(type, rec, selector, CCSizeMake(440, 293));
}


void DyPopup::endAnimationRewardFoot0(CCNode* node)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_DOJANG);

}


void DyPopup::endAnimationRewardFoot1(CCNode* node)
{
    CCMenuItemImage *pMenu = (CCMenuItemImage*)node->getUserData();
    
    m_active_tag = pMenu->getTag();
    
    YtLayerManager::sharedLayerManager()->popLayer();
}



void DyPopup::keyBackClicked()
{
    if(m_popuptype != DyPopupType_LoadingCycle)
    {
        if(m_arrButtons->count() > 1)
        {
            setKeypadEnabled(false);
            onButtonClicked(NULL);
        }
    }
}


void DyPopup::onButtonClicked(CCObject *obj)
{
    CCMenuItemImage *pMenu = dynamic_cast<CCMenuItemImage*>(obj);

    if(obj == NULL)
    {
        m_active_tag = 0;
        YtLayerManager::sharedLayerManager()->popLayer();
        return;
    }
    
    if(m_popuptype == DyPopupType_Present)
    {
        if(m_curday == pMenu->getTag())
        {
            m_sprIndicator->setVisible(false);
            
            m_sprFoot[m_curday]->setOpacity(255);
            m_sprFoot[m_curday]->setScale(1.2);
            m_sprFoot[m_curday]->runAction(CCSequence::create(CCScaleTo::create(0.05, 1.0f), CCCallFuncN::create(this, callfuncN_selector(DyPopup::endAnimationRewardFoot0)), CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(DyPopup::endAnimationRewardFoot1)), NULL));
            m_sprFoot[m_curday]->setUserData(pMenu);
        }
    }
    else
    {
        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

        m_active_tag = pMenu->getTag();

        YtLayerManager::sharedLayerManager()->popLayer();
    }
}


void DyPopup::setPicture(CCSprite* sprite, CCPoint pos)
{
    sprite->setPosition(pos);
    addChild(sprite);
    
    m_content->setPosition(ccp(0, 0));
}


void DyPopup::setText(const char* text)
{
	std::string strBuffer = YtLib::getMultilineText(text);
	
    m_content->setString(strBuffer.c_str());

	if(m_popuptype == DyPopupType_LoadingCycle)
	{	
		m_spr9TextBox->setContentSize(CCSizeMake(m_content->getContentSize().width + 10, m_content->getContentSize().height + 10));
	}
}



void DyPopup::setCustomer(std::string tag)
{
	DyCharacterInfo* characterInfo = DyCharacterManager::sharedCharacterManager()->getCharacterInfo(tag);

	CCLabelTTF* explain = CCLabelTTF::create(characterInfo->m_explain.c_str(), FONT_NAME_EXP, 20.f);
	explain->setPosition(ccp(50, 40));
	explain->setColor(ccc3(51, 51, 51));
	addChild(explain);

	for(int i = 0;i < 5;i++)
	{
		CCSprite* sprHeart;
		if(i < characterInfo->m_heart_max/2)
		{
			sprHeart = CCSprite::createWithSpriteFrameName("game_ui_heart_full.png");
		}
		else
			sprHeart = CCSprite::createWithSpriteFrameName("game_ui_heart_zero.png");

		sprHeart->setPosition(ccp(10+22*i, 12));
		addChild(sprHeart);
	}

	assert(m_character == NULL);

	m_character = YtAniObject::loadAniObject(this, characterInfo->m_url.c_str());
	m_character->m_actionManager->runAnimations("order");

	CCSize sizeBox = CCSizeMake(127, 176);
	if(tag.compare("duck") == 0)
		m_character->setPosition(ccp(-140, -60));
	else
		m_character->setPosition(ccp(-130, -60));
	
	m_character->setScale(sizeBox.height/characterInfo->m_size.height);
	
	addChild(m_character);
}


void DyPopup::setRecipe(std::string tag, bool alreadHas)
{
	DyLib* lib = DyLib::sharedLib();

	DyRecipeInfo* recipe = DyRecipeManager::sharedRecipeManager()->getRecipe(tag);

	CCPoint posPicture = ccp(-110, 35);

	if(recipe->m_hasDish)
	{
		CCSprite* sprDish = CCSprite::createWithSpriteFrameName("game_ui_dish.png");
		sprDish->setPosition(ccp(posPicture.x, posPicture.y-3));
		addChild(sprDish);
	}

	CCSprite* sprRecipe = CCSprite::createWithSpriteFrameName(recipe->m_urlResultImage.c_str());
	sprRecipe->setPosition(ccp(posPicture.x, posPicture.y+2));
	sprRecipe->setAnchorPoint(ccp(recipe->m_anchorPoint.x, recipe->m_anchorPoint.y));
	sprRecipe->setScale(0.8f*recipe->m_scale);
	addChild(sprRecipe);

	CCLabelTTF* labelName = CCLabelTTF::create(recipe->m_name.c_str(), FONT_NAME_EXP, 22.f);
	labelName->setPosition(ccp(-20, 50));
	labelName->setAnchorPoint(ccp(0, 0.5));
	labelName->setColor(ccc3(102, 51, 0));
	addChild(labelName);

    if(alreadHas)
    {
        CCLabelTTF* labelHas = CCLabelTTF::create("(보유중)", FONT_NAME_EXP, 22.f);
        labelHas->setPosition(ccp(-20, 18));
        labelHas->setAnchorPoint(ccp(0, 0.5));
        labelHas->setColor(ccc3(255, 0, 0));
        addChild(labelHas);
    }
    else
    {
        if(recipe->m_purchase_gold > 0 )
        {
            CCSprite* sprSellingPriceMark = CCSprite::createWithSpriteFrameName("menu_ui_gold.png");
            sprSellingPriceMark->setScale(.8f);
            sprSellingPriceMark->setPosition(ccp(-20, 18));
            sprSellingPriceMark->setAnchorPoint(ccp(0, 0.5));
            addChild(sprSellingPriceMark);
            
            YtLib::setMoneyStyle(lib->m_strTemp, recipe->m_purchase_gold);
            
            CCLabelTTF* labelSellingPrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 22.f);
            labelSellingPrice->setColor(ccc3(102, 51, 0));
            labelSellingPrice->setAnchorPoint(ccp(0, 0.5));
            labelSellingPrice->setPosition(ccp(18, 16));
            addChild(labelSellingPrice);
        }
        if(recipe->m_purchase_delly > 0 )
        {
            CCSprite* sprSellingPriceMark = CCSprite::createWithSpriteFrameName("menu_ui_delly.png");
            sprSellingPriceMark->setScale(.8f);
            sprSellingPriceMark->setPosition(ccp(-20, 18));
            sprSellingPriceMark->setAnchorPoint(ccp(0, 0.5));
            addChild(sprSellingPriceMark);
            
            YtLib::setMoneyStyle(lib->m_strTemp, recipe->m_purchase_delly);
            
            CCLabelTTF* labelSellingPrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 22.f);
            labelSellingPrice->setColor(ccc3(102, 51, 0));
            labelSellingPrice->setAnchorPoint(ccp(0, 0.5));
            labelSellingPrice->setPosition(ccp(18, 16));
            addChild(labelSellingPrice);
        }
    }
}


void DyPopup::setCostumeInfo(DyCostumeStatus* costumeStatus, DyCostumeFunction* costumeFunction)
{
    DyPopupCostume* popupCostume = DyPopupCostume::create(this, costumeStatus, costumeFunction);
    popupCostume->setPosition(ccp(0, 20));
    
    addChild(popupCostume);
    
    
//    sprintf(lib->m_strTemp, "-%d%%", m_costumeFunction.m_decay);
//    labelDecay->setString(lib->m_strTemp);
//    sprintf(lib->m_strTemp, "-%d%%", m_costumeFunction.m_ice);
//    labelIce->setString(lib->m_strTemp);
//    sprintf(lib->m_strTemp, "+%d%%", m_costumeFunction.m_heart);
//    labelHeart->setString(lib->m_strTemp);
//    sprintf(lib->m_strTemp, "+%d%%", m_costumeFunction.m_tip);
//    labelGold->setString(lib->m_strTemp);
//    sprintf(lib->m_strTemp, "-%d%%", m_costumeFunction.m_cooling);
//    labelCooling->setString(lib->m_strTemp);

}


void DyPopup::setQuestInfo(std::string text, std::string icon)
{
	std::string strBuffer = YtLib::getMultilineText(text.c_str());
	
    m_content->setString(strBuffer.c_str());
    
    if(icon.compare("NULL") == 0)
    {
        m_content->setPosition(ccp(0, 40));
    }
    else
    {
        std::string picture_url = "quest_";
        picture_url += icon;
        
        
        CCSprite* sprIcon = CCSprite::createWithSpriteFrameName(picture_url.c_str());
        
        sprIcon->setPosition(ccp(0, 70));
        addChild(sprIcon);
        
        m_content->setPosition(ccp(0, 0));
    }
}


void DyPopup::setQuestInfo2(std::string text, std::string text2, std::string icon)
{
	std::string strBuffer = YtLib::getMultilineText(text.c_str());
	
    m_content->setString(strBuffer.c_str());
    
    if(icon.compare("NULL") == 0)
    {
        m_content->setPosition(ccp(0, 40));
    }
    else
    {
        std::string picture_url = "quest_";
        picture_url += icon;
        
        CCSprite* sprIcon = CCSprite::createWithSpriteFrameName(picture_url.c_str());
        
        sprIcon->setPosition(ccp(0, 90));
        addChild(sprIcon);
        
        m_content->setPosition(ccp(0, 20));
    }
    
    CCLabelTTF* content = CCLabelTTF::create(text2.c_str(), FONT_NAME_EXP, 25.f);
    content->setColor(ccRED);//ccc3(64, 64, 64));
	content->setAnchorPoint(ccp(1.0, 0.5));
	content->setPosition(ccp(180, -35));
	addChild(content);

}




void DyPopup::setDay(int day)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyLib* lib = DyLib::sharedLib();
    
    m_curday = day;
    
    for(int i = 0;i < PRESENT_CHECK_DAY;i++)
    {
        CCPoint pos = ccp(-120-120 + 120*(i%5), 90 - 90*(i/5));
        
        CCSprite* sprBox;
        
        if(i == day)
        {
            sprBox = CCSprite::createWithSpriteFrameName("menu2_ui_present_day_02.png");
            
            sprintf(lib->m_strTemp, "menu2_ui_present_number_%d.png", i+1);
            CCSprite* sprDay = CCSprite::createWithSpriteFrameName(lib->m_strTemp);
            
            sprDay->setPosition(ccp(50, 55));
            sprBox->addChild(sprDay);
        }
        else
        {
            sprBox = CCSprite::createWithSpriteFrameName("menu2_ui_present_day_01.png");
            
            sprintf(lib->m_strTemp, "%d", i+1);
            CCLabelTTF* labelDay = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 30);
            labelDay->setColor(ccc3(109, 65, 35));
            labelDay->setPosition(ccp(50, 50));
            
            sprBox->addChild(labelDay);
        }
        
        
        const char* type = lib->m_parameters->getValue("value_string", i);
        int count = atoi(lib->m_parameters->getValue("value_integer", i));

        if(strcmp(type, "gold") == 0)
        {
            sprintf(lib->m_strTemp, "%d", count);
          
            CCLabelTTF* labelPurchasePrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 22.f);
            labelPurchasePrice->setColor(ccc3(39, 18, 12));
            sprBox->addChild(labelPurchasePrice);
            
            CCSprite* sprPurchasePriceMark = CCSprite::createWithSpriteFrameName("menu_ui_gold.png");
            sprPurchasePriceMark->setScale(0.6f);
            sprBox->addChild(sprPurchasePriceMark);
            
            
            if(i == day)
            {
                sprPurchasePriceMark->setPosition(ccp(0, 18));
                labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, 17));
                YtLib::arrangeNodesH(ccp(45, 18), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
            }
            else
            {
                sprPurchasePriceMark->setPosition(ccp(0, 15));
                labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, 14));
                YtLib::arrangeNodesH(ccp(45, 15), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
            }
        }
        else if(strcmp(type, "delly") == 0)
        {
            sprintf(lib->m_strTemp, "%d", count);
            
            CCLabelTTF* labelPurchasePrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 22.f);
            labelPurchasePrice->setColor(ccc3(39, 18, 12));
            sprBox->addChild(labelPurchasePrice);
            
            CCSprite* sprPurchasePriceMark = CCSprite::createWithSpriteFrameName("menu_ui_delly.png");
            sprPurchasePriceMark->setScale(0.6f);
            sprBox->addChild(sprPurchasePriceMark);
            
            if(i == day)
            {
                sprPurchasePriceMark->setPosition(ccp(0, 18));
                labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, 17));
                YtLib::arrangeNodesH(ccp(45, 18), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
            }
            else
            {
                sprPurchasePriceMark->setPosition(ccp(0, 15));
                labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, 14));
                YtLib::arrangeNodesH(ccp(45, 15), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
            }
        }
        else if(strcmp(type, "roullette") == 0)
        {
            sprintf(lib->m_strTemp, "X%d", count);
            
            CCLabelTTF* labelPurchasePrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 22.f);
            labelPurchasePrice->setColor(ccc3(39, 18, 12));
            sprBox->addChild(labelPurchasePrice);
            
            CCSprite* sprPurchasePriceMark = CCSprite::createWithSpriteFrameName("menu2_ui_present_roullette.png");
            //sprPurchasePriceMark->setScale(0.6f);
            sprBox->addChild(sprPurchasePriceMark);
            
            if(i == day)
            {
                sprPurchasePriceMark->setPosition(ccp(0, 18));
                labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, 17));
                YtLib::arrangeNodesH(ccp(45, 18), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
            }
            else
            {
                sprPurchasePriceMark->setPosition(ccp(0, 15));
                labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, 14));
                YtLib::arrangeNodesH(ccp(45, 15), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
            }
        }
        else if(strcmp(type, "gum") == 0)
        {
            sprintf(lib->m_strTemp, "X%d", count);
            
            CCLabelTTF* labelPurchasePrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 22.f);
            labelPurchasePrice->setColor(ccc3(39, 18, 12));
            sprBox->addChild(labelPurchasePrice);
            
            CCSprite* sprPurchasePriceMark = CCSprite::createWithSpriteFrameName("menu2_ui_present_gum.png");
            //sprPurchasePriceMark->setScale(0.6f);
            sprBox->addChild(sprPurchasePriceMark);
            
            if(i == day)
            {
                sprPurchasePriceMark->setPosition(ccp(0, 18));
                labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, 17));
                YtLib::arrangeNodesH(ccp(45, 18), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
            }
            else
            {
                sprPurchasePriceMark->setPosition(ccp(0, 15));
                labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, 14));
                YtLib::arrangeNodesH(ccp(45, 15), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
            }
        }

        else if(strcmp(type, "random") == 0)
        {   
            sprintf(lib->m_strTemp, "%d", count);
            
            CCLabelTTF* labelPurchasePrice = CCLabelTTF::create(STRING_random_box, FONT_NAME_EXP, 18.f);
            labelPurchasePrice->setColor(ccc3(39, 18, 12));
            sprBox->addChild(labelPurchasePrice);
            
            CCSprite* sprPurchasePriceMark = CCSprite::createWithSpriteFrameName("menu2_ui_randombox.png");
            //sprPurchasePriceMark->setScale(0.6f);
            sprBox->addChild(sprPurchasePriceMark);
            
            if(i == day)
            {
                sprPurchasePriceMark->setPosition(ccp(0, 17));
                
                labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, 17));
                
                YtLib::arrangeNodesH(ccp(50, 17), ccp(0.5, 0.5), 1, sprPurchasePriceMark, labelPurchasePrice, NULL);
       
            }
            else
            {
                sprPurchasePriceMark->setPosition(ccp(0, 14));
                
                labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, 14));
                
                YtLib::arrangeNodesH(ccp(50, 14), ccp(0.5, 0.5), 1, sprPurchasePriceMark, labelPurchasePrice, NULL);
            }
        }
        
        if(dataManager->m_present_award > 1)
        {
            if(i < day)
            {
                m_sprFoot[i] = CCSprite::createWithSpriteFrameName("menu2_ui_present_dog_foot.png");
                m_sprFoot[i]->setPosition(ccp(5, 55));
            }
            else
            {
                m_sprFoot[i] = CCSprite::createWithSpriteFrameName("menu2_ui_present_dog_foot_event.png");
                //m_sprFoot[i]->setPosition(ccp(90, 65));
                m_sprFoot[i]->setPosition(ccp(5, 55));
            }
        }
        else
        {
            m_sprFoot[i] = CCSprite::createWithSpriteFrameName("menu2_ui_present_dog_foot.png");
            m_sprFoot[i]->setPosition(ccp(5, 55));
        }
        
        sprBox->addChild(m_sprFoot[i]);
        
        if(i < day)
        {
            m_sprFoot[i]->setOpacity(255);
        }
        else
        {
            m_sprFoot[i]->setOpacity(0);
        }
        
        if(i == day)
        {
            m_sprIndicator = lib->createIndicator(ccp(90, 70));
            sprBox->addChild(m_sprIndicator);
        }
        
        DyPopupButton* popupButton = new DyPopupButton();
        popupButton->m_buttontype = DyButtonType_Custom;
        popupButton->m_strName = "";
        popupButton->m_tag = i;
        
        popupButton->m_menuItem = CCMenuItemImage::create();
        popupButton->m_menuItem->initWithTarget(this, menu_selector(DyPopup::onButtonClicked));

        popupButton->m_menuItem->setNormalImage(sprBox);
        popupButton->m_menuItem->setPosition(pos);
        
        m_menu->addChild(popupButton->m_menuItem);
        
        m_arrButtons->addObject(popupButton);
        popupButton->release();
        
        popupButton->m_menuItem->setTag(i);
    }
    
    
    CCSprite* sprTextBox = CCSprite::createWithSpriteFrameName("menu2_ui_textbox_present.png");
    sprTextBox->setPosition(ccp(0, -85));
    addChild(sprTextBox);
    
//    CCLabelTTF* labelExplain = CCLabelTTF::create(lib->getMultilineText(STRING_present_explain).c_str(), FONT_NAME_EXP, 24);
//    if(dataManager->m_present_award > 1)
//        labelExplain->setString(lib->getMultilineText(STRING_present_explain_event).c_str());
    
//    labelExplain->setColor(ccc3(40, 18, 13));
//    labelExplain->setPosition(ccp(0, -85));
//    addChild(labelExplain);

}


DyPopupButton* DyPopup::addButton(DyButtonType type, const char* name, int tag)
{
	DyPopupButton* popupButton = new DyPopupButton();
	popupButton->m_buttontype = type;
	popupButton->m_strName = name;
	popupButton->m_tag = tag;

    popupButton->m_menuItem = CCMenuItemImage::create();
	popupButton->m_menuItem->initWithTarget(this, menu_selector(DyPopup::onButtonClicked));
    
    
    if(strlen(name) >= 4*2)
        DyLib::setMenuItemText_kor(popupButton->m_menuItem, type, name, 28, 33);
    else
        DyLib::setMenuItemText_kor(popupButton->m_menuItem, type, name);
	
    
    
    m_menu->addChild(popupButton->m_menuItem);

	m_arrButtons->addObject(popupButton);
	popupButton->release();

	popupButton->m_menuItem->setTag(tag);
	popupButton->m_menuItem->setVisible(false);
	arrangeButton();
    
    return popupButton;
}


void DyPopup::arrangeButton()
{
	int box_button_cnt = 0;
	//bool hasCloseButton = false;
	CCSize size_boxbutton;
	for(unsigned int i = 0;i < m_arrButtons->count();i++)
	{
		DyPopupButton* button = (DyPopupButton*)m_arrButtons->objectAtIndex(i);
		if(button->m_buttontype == DyButtonType_Close)
		{
			//hasCloseButton = true;
		}
		else
		{
			box_button_cnt++;
			size_boxbutton = button->m_menuItem->getContentSize();
		}
	}

	int x_boxbutton = 0;
	int gap_boxbutton = 0;


	switch(box_button_cnt)
	{
		case 1:
			x_boxbutton = 0;
			break;
		case 2:
			gap_boxbutton = 20;
			x_boxbutton = -(size_boxbutton.width/2 + gap_boxbutton/2);
			break;
	}

	for(unsigned int i = 0;i < m_arrButtons->count();i++)
	{
		DyPopupButton* button = (DyPopupButton*)m_arrButtons->objectAtIndex(i);
		if(button->m_buttontype == DyButtonType_Close)
		{
			//hasCloseButton = true;

			button->m_menuItem->setPosition(ccp(m_size.width/2-40, m_size.height/2-40));
			button->m_menuItem->setVisible(true);
		}
		else
		{
			button->m_menuItem->setPosition(ccp(x_boxbutton, -m_size.height/2 + 65));
			x_boxbutton += size_boxbutton.width + gap_boxbutton;
			button->m_menuItem->setVisible(true);
		}
	}
}


void DyPopup::pause()
{
	if(m_popuptype == DyPopupType_LoadingCycle)
	{
		if(m_rt)
			m_rt->removeFromParentAndCleanup(true);
		m_rt = NULL;
	}
	unscheduleUpdate();
    setKeypadEnabled(false);
}

void DyPopup::cbCirclePopup(float dt)
{
	//http://www.cocos2d-x.org/boards/6/topics/6197?r=15453#message-15453
	m_rt = CCRenderTexture::create(m_sprCircleWhiteRT->getContentSize().width, m_sprCircleWhiteRT->getContentSize().height,
		kCCTexture2DPixelFormat_RGBA8888);

	m_rt->setPosition(ccp(-13, 6));

	addChild(m_rt);

	unschedule(schedule_selector(DyPopup::cbCirclePopup));

	scheduleUpdate();

	m_dtCircle = 0;
	m_isBlownDown = true;
	m_sprCircleBlown->setVisible(false);
	m_sprCircleWhite->setVisible(true);
}
	

void DyPopup::update(float dt)
{
    CCPoint offset = ccp(0,0);
    
    if(cocos2d::CCEGLView::sharedOpenGLView()->isRetinaEnabled())
    {
        CCSize winSize = CCDirector::sharedDirector()->getWinSizeInPixels();
        offset.x -= m_sprCircleWhiteRT->getContentSize().width/4;
        offset.y -= m_sprCircleWhiteRT->getContentSize().height/4;
    }
	m_dtCircle += dt;
	m_rt->beginWithClear(0, 0, 0, 0, 0, 0);
	m_rt->getSprite()->getTexture()->setAntiAliasTexParameters();
	ccBlendFunc blend1 = {GL_ONE, GL_ZERO};
	m_sprCircleMaskRT->setBlendFunc(blend1);
	m_sprCircleMaskRT->setPosition(ccp(m_sprCircleMaskRT->getContentSize().width/2+offset.x, m_sprCircleMaskRT->getContentSize().height/2+100-m_dtCircle*80+offset.y));
	m_sprCircleMaskRT->visit();
	m_sprCircleMaskRT->getTexture()->setAntiAliasTexParameters();
	ccBlendFunc blend2 = {GL_DST_ALPHA, GL_ZERO};

	if(m_isBlownDown)
	{
		m_sprCircleBlownRT->getTexture()->setAntiAliasTexParameters();
		m_sprCircleBlownRT->setBlendFunc(blend2);
		m_sprCircleBlownRT->setPosition(ccp(m_sprCircleWhiteRT->getContentSize().width/2+offset.x, m_sprCircleWhiteRT->getContentSize().height/2+offset.y));
		m_sprCircleBlownRT->visit();
	}
	else
	{
		m_sprCircleWhiteRT->getTexture()->setAntiAliasTexParameters();
		m_sprCircleWhiteRT->setBlendFunc(blend2);
		m_sprCircleWhiteRT->setPosition(ccp(m_sprCircleWhiteRT->getContentSize().width/2+offset.x, m_sprCircleWhiteRT->getContentSize().height/2+offset.y));
		m_sprCircleWhiteRT->visit();
	}

	m_rt->end();

	if(m_dtCircle*80 > 135)
	{
		m_dtCircle = 0;
		m_isBlownDown = !m_isBlownDown;
		if(m_isBlownDown)
		{
			m_sprCircleBlown->setVisible(false);
			m_sprCircleWhite->setVisible(true);
		}
		else
		{
			m_sprCircleBlown->setVisible(true);
			m_sprCircleWhite->setVisible(false);
		}
	}
}

void DyPopup::resume()
{
	//http://www.cocos2d-x.org/boards/6/topics/10593
	if(m_popuptype == DyPopupType_LoadingCycle)
	{
		schedule(schedule_selector(DyPopup::cbCirclePopup), .01f);
	}
    
    setKeypadEnabled(true);
}


void DyPopup::destory()
{
	if(m_pListener)
		(m_pListener->*m_pfnSelector)(this);
	else
	{
		YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
	}
}


void DyPopup::hide()
{
	YtLib* lib = YtLib::sharedLib();

    //CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    
	switch(m_popuptype)
	{
        case DyPopupType_Text:
		case DyPopupType_GumWarning:
        case DyPopupType_Quest:
        case DyPopupType_Quest2:
		case DyPopupType_NewCustomer:
		case DyPopupType_NewRecipe:
        case DyPopuptype_RandomBox:
        case DyPopupType_Present:
            runAction(CCSequence::create(
                CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-50)),
                CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-55)),
                CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height)),

                CCCallFunc::create(this, callfunc_selector(DyPopup::destory)),
			NULL));
			break;
        case DyPopupType_Costume:
            runAction(CCSequence::create(
                                         CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2+160,lib->m_sizeWin.height/2-50-60)),
                                         CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2+160,lib->m_sizeWin.height/2-55-60)),
                                         CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2+160, lib->m_sizeWin.height+lib->m_sizeWin.height)),
                                         
                                         CCCallFunc::create(this, callfunc_selector(DyPopup::destory)),
                                         NULL));
			break;
        case DyPopupType_Costume2:
            runAction(CCSequence::create(
                                         CCMoveTo::create(0.15f,ccp(lib->m_sizeWin.width/2+220,lib->m_sizeWin.height/2-50)),
                                         CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2+220,lib->m_sizeWin.height/2-55)),
                                         CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2+200, lib->m_sizeWin.height+lib->m_sizeWin.height)),
                                         
                                         CCCallFunc::create(this, callfunc_selector(DyPopup::destory)),
                                         NULL));
			break;
//        case DyPopupType_LoadingCycle:
//            YtLayerManager::sharedLayerManager()->destoryTopLayer(false);
//            break;
		default:
			YtLayerManager::sharedLayerManager()->destoryTopLayer(m_resumeAndDestory);
			break;
	}
	//YtLayerManager::sharedLayerManager()->destoryTopLayer();
}


void DyPopup::show()
{
	YtLib* lib = YtLib::sharedLib();
    //CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,0,true);

	switch(m_popuptype)
	{
        case DyPopupType_GumWarning:
		case DyPopupType_Text:
        case DyPopupType_Quest:
        case DyPopupType_Quest2:
		case DyPopupType_NewCustomer:
		case DyPopupType_NewRecipe:
        case DyPopuptype_RandomBox:
        case DyPopupType_Present:
            setPosition(ccp(lib->m_sizeWin.width/2, lib->m_sizeWin.height+lib->m_sizeWin.height/2));

			runAction(CCSequence::create(

				CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-20)),
				CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2+20)),
				CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2,lib->m_sizeWin.height/2-3)),
				NULL));
			break;
        case DyPopupType_Costume:
            
            setPosition(ccp(lib->m_sizeWin.width/2+160, lib->m_sizeWin.height+lib->m_sizeWin.height/2));
            
			runAction(CCSequence::create(
                                         
                                         CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2+160,lib->m_sizeWin.height/2-20-60)),
                                         CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2+160,lib->m_sizeWin.height/2+20-60)),
                                         CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2+160,lib->m_sizeWin.height/2-3-60)),
                                         NULL));

            break;
        case DyPopupType_Costume2:
            
            setPosition(ccp(lib->m_sizeWin.width/2+220, lib->m_sizeWin.height+lib->m_sizeWin.height/2));
            
			runAction(CCSequence::create(
                                         
                                         CCMoveTo::create(0.2f,ccp(lib->m_sizeWin.width/2+220,lib->m_sizeWin.height/2-20)),
                                         CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2+220,lib->m_sizeWin.height/2+20)),
                                         CCMoveTo::create(0.1f,ccp(lib->m_sizeWin.width/2+220,lib->m_sizeWin.height/2-3)),
                                         NULL));
            
            break;

        default:
            break;
	}
}