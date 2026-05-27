/*
DyLayerShopStaff.cpp
Doggy chef
@initialize at 130516

@history
130516	yoonsr	initialize
*/

#include "DyConfig.h"
#include "DyLayerShopStaff.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyBlockInfo.h"
#include "DyPopup.h"
#include "DyStaff.h"


DyLayerShopStaff::DyLayerShopStaff()
{
    m_menu = NULL;
    setTouchEnabled(true);
    m_selectedIndex = -1;
}

DyLayerShopStaff::~DyLayerShopStaff()
{
}

void DyLayerShopStaff::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader)
{
}

SEL_MenuHandler DyLayerShopStaff::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName)
{
    return NULL;    
}

SEL_CCControlHandler DyLayerShopStaff::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}

bool DyLayerShopStaff::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerShopStaff::setPicture()
{
    YtLib* lib = YtLib::sharedLib();
    
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    CCLayer* layer = (CCLayer*)getChildByTag(SHOPSTAFF_UI_TAG_LAYER);
    
    layer->removeAllChildrenWithCleanup(true);
    
    CCScale9Sprite* sprLabelBg = CCScale9Sprite::createWithSpriteFrame(CCSprite::createWithSpriteFrameName("menushop_ui_box_00.png")->displayFrame());
    sprLabelBg->setContentSize(CCSizeMake(248, 368));
    sprLabelBg->setAnchorPoint(ccp(0.5, 0.5));
    sprLabelBg->setPosition(ccp(-294.0, 19));
    layer->addChild(sprLabelBg);

    CCSprite* sprLine = CCSprite::createWithSpriteFrameName("menushop_ui_vertical_line.png");
    sprLine->setPosition(ccp(-156.0, 19));
    layer->addChild(sprLine);

    CCSprite* sprLabelBg2 = CCSprite::createWithSpriteFrameName("menushop_ui_recipe_title.png");
    sprLabelBg2->setPosition(ccp(-298.0, 194));
    layer->addChild(sprLabelBg2);
    
    
    CCLabelTTF* labelName = CCLabelTTF::create("", FONT_NAME_EXP, 23);
    labelName->setPosition(ccp(-299.0, 199));
    labelName->setColor(ccc3(84,35,17));
    layer->addChild(labelName);
    
    
    CCLabelTTF* labelPictureDesc = CCLabelTTF::create("", FONT_NAME_EXP, 18, CCSizeMake(240,100), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
    labelPictureDesc->setPosition(ccp(-297.0, -116));
    labelPictureDesc->setColor(ccc3(51,51,51));
    layer->addChild(labelPictureDesc);
    
    
    CCSprite* sprShadow = CCSprite::createWithSpriteFrameName("menu2_ui_cashier_shadow.png");
    sprShadow->setPosition(ccp(-299.0, -54));
    layer->addChild(sprShadow);

    
    CCSprite* sprPicture = CCSprite::createWithSpriteFrameName("staff_ui_cashier.png");
    sprPicture->setPosition(ccp(-300.0, -54));
    sprPicture->setAnchorPoint(ccp(0.5, 0));
    layer->addChild(sprPicture);
    
    
    if(staffManager->getGrade(m_curStaff) == -1)
    {
        sprPicture->setDisplayFrame(staffManager->getImageWithGrade(m_curStaff, -1)->displayFrame());
        
        const char* name = staffManager->m_staff[m_curStaff]->getValue("name", 0);
        labelName->setString(name);
        
        const char* desc = staffManager->m_staff[m_curStaff]->getValue("condition_explain", 0);
        labelPictureDesc->setString(desc);
    }
    else if(staffManager->getGrade(m_curStaff) == m_selectedIndex)
    {
        sprPicture->setDisplayFrame(staffManager->getImageWithGrade(m_curStaff, m_selectedIndex)->displayFrame());
        
        const char* name = staffManager->m_staff[m_curStaff]->getValue("name", m_selectedIndex);
        labelName->setString(name);
        
        
        if(m_selectedIndex == staffManager->m_staff[m_curStaff]->count()-1)
            labelPictureDesc->setString(STRING_MORE_PROMOTE);
        else
            labelPictureDesc->setString(STRING_EXPLAIN_PROMOTE);
    }
    else if(staffManager->getGrade(m_curStaff)+1 == m_selectedIndex)
    {
        sprPicture->setDisplayFrame(staffManager->getImageWithGrade(m_curStaff, -1)->displayFrame());
        
        labelName->setString("?");
        
        if(staffManager->getRemainCount(m_curStaff) > 0)
        {
            const char* desc = staffManager->m_staff[m_curStaff]->getValue("condition_explain", m_selectedIndex);
            sprintf(lib->m_strTemp, desc, staffManager->getRemainCount(m_curStaff), staffManager->getRemainCount(m_curStaff));
            labelPictureDesc->setString(lib->m_strTemp);
        }
        else
        {
            labelPictureDesc->setString(STRING_NOW_PRMOTE);
        }
    }
}


void DyLayerShopStaff::setPannel(int index)
{
    YtLib* lib = YtLib::sharedLib();
    
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    m_sprPannel[index]->removeAllChildrenWithCleanup(true);
    
    m_sprPannel[index]->setColor(ccWHITE);
    
    m_sprPannel[index]->setDisplayFrame(CCSprite::createWithSpriteFrameName("staff_ui_pannel_normal.png")->displayFrame());
    
    
    if(index == m_selectedIndex)
    {
        m_sprPannel[index]->setDisplayFrame(CCSprite::createWithSpriteFrameName("staff_ui_pannel_select.png")->displayFrame());
    }
    
    if(staffManager->getGrade(m_curStaff)+1 == index && index > 0)
    {
        CCSprite* sprBallon = CCSprite::createWithSpriteFrameName("staff_ui_ballon.png");
        sprBallon->setPosition(ccp(80, 120));
        m_sprPannel[index]->addChild(sprBallon);
        
        if(staffManager->getRemainCount(m_curStaff) > 0)
        {
            sprintf(lib->m_strTemp, "%d", staffManager->getRemainCount(m_curStaff));
            
            CCLabelTTF* labelNumber = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 25);
            labelNumber->setPosition(ccp(35, 68));
            labelNumber->setColor(ccc3(48,48,48));
            sprBallon->addChild(labelNumber);
            
            
            CCLabelTTF* labelDesc1 = CCLabelTTF::create(STRING_MORE, FONT_NAME_EXP, 20);
            labelDesc1->setPosition(ccp(72, 64));
            labelDesc1->setColor(ccc3(48,48,48));
            sprBallon->addChild(labelDesc1);
            
            CCLabelTTF* labelDesc2 = CCLabelTTF::create(STRING_HIRE, FONT_NAME_EXP, 20);
            labelDesc2->setPosition(ccp(60, 37));
            labelDesc2->setColor(ccc3(48,48,48));
            sprBallon->addChild(labelDesc2);
        }
        else
        {
            CCLabelTTF* labelDesc = CCLabelTTF::create(staffManager->m_staff[m_curStaff]->getValue("explain", index), FONT_NAME_EXP, 18);
            labelDesc->setPosition(ccp(60, 64));
            labelDesc->setColor(ccc3(85,46,33));
            sprBallon->addChild(labelDesc);
            
            CCLabelTTF* labelName = CCLabelTTF::create(staffManager->m_staff[m_curStaff]->getValue("name", index), FONT_NAME_EXP, 25);
            labelName->setPosition(ccp(60, 37));
            labelName->setColor(ccc3(40,10,1));
            sprBallon->addChild(labelName);

        }
    }

    else if(staffManager->getGrade(m_curStaff) < index)
    {
        m_sprPannel[index]->setColor(ccc3(200, 200, 200));
        
        CCSprite* sprCashierIcon = CCSprite::createWithSpriteFrameName("staff_ui_icon_lock.png");
        sprCashierIcon->setPosition(ccp(80, 125));
        m_sprPannel[index]->addChild(sprCashierIcon);
        
        CCSprite* sprLock = CCSprite::createWithSpriteFrameName("staff_ui_padlock.png");
        sprLock->setPosition(ccp(80, 40));
        m_sprPannel[index]->addChild(sprLock);
    }
    else if(staffManager->getGrade(m_curStaff) >= index)
    {
        CCSprite* sprBallon = CCSprite::createWithSpriteFrameName("staff_ui_ballon.png");
        sprBallon->setPosition(ccp(80, 120));
        m_sprPannel[index]->addChild(sprBallon);

        CCLabelTTF* labelDesc = CCLabelTTF::create(staffManager->m_staff[m_curStaff]->getValue("explain", index), FONT_NAME_EXP, 18);
        labelDesc->setPosition(ccp(60, 64));
        labelDesc->setColor(ccc3(85,46,33));
        sprBallon->addChild(labelDesc);
        
        CCLabelTTF* labelName = CCLabelTTF::create(staffManager->m_staff[m_curStaff]->getValue("name", index), FONT_NAME_EXP, 25);
        labelName->setPosition(ccp(60, 37));
        labelName->setColor(ccc3(40,10,1));
        sprBallon->addChild(labelName);
        
        CCLabelTTF* labelFunction = CCLabelTTF::create(YtLib::getMultilineText(staffManager->m_staff[m_curStaff]->getValue("fuction_explain", index)).c_str(), FONT_NAME_EXP, 20,
                                                       CCSizeMake(150, 60), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
         
        
        labelFunction->setPosition(ccp(80, 35));
        labelFunction->setColor(ccc3(82,43,29));
        m_sprPannel[index]->addChild(labelFunction);
        
        if(staffManager->getGrade(m_curStaff) == index)
        {
            CCSprite* sprIcon;
            
            sprIcon = DyStaffManager::sharedStaffManager()->getStaffIcon(m_curStaff, index);
            
            sprIcon->setPosition(ccp(140, 93));
            m_sprPannel[index]->addChild(sprIcon);
        }
    }
}


void DyLayerShopStaff::pause()
{
    if(m_menu)
        m_menu->setEnabled(false);
    
    m_scrollView->setTouchEnabled(false);
}


void DyLayerShopStaff::resume()
{
    if(m_menu)
        m_menu->setEnabled(true);
    m_scrollView->setTouchEnabled(true);
}


void DyLayerShopStaff::hide()
{
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}

void DyLayerShopStaff::show()
{
    
}


void DyLayerShopStaff::resetStaffLayer()
{
    DyLib* lib = DyLib::sharedLib();
    
	//DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    m_layerContent->removeAllChildrenWithCleanup(true);
    
    m_menu = CCMenu::create();
    m_menu->setPosition(CCPointZero);
    
    
    
    for(int i = 0;i < staffManager->m_staff[m_curStaff]->count();i++)
    {
        m_sprPannel[i] = CCSprite::createWithSpriteFrameName("staff_ui_pannel_normal.png");
        m_sprPannel[i]->setPosition(ccp(100 + (i%3)*180, 300 - (i/3)*200));
        m_layerContent->addChild(m_sprPannel[i]);
        setPannel(i);
        
        m_menuItem[i] = CCMenuItemImage::create();
		
		m_menuItem[i]->initWithNormalSprite(CCSprite::createWithSpriteFrameName("menu2_ui_promote.png"), CCSprite::createWithSpriteFrameName("menu2_ui_promote_touch.png"), NULL, this, menu_selector(DyLayerShopStaff::onPromotionClicked));
        m_menuItem[i]->setPosition(ccp(100 + (i%3)*180, 250 - (i/3)*200));
        m_menuItem[i]->setTag(i);
        
        m_menu->addChild(m_menuItem[i]);
        
        m_sprDelly[i] = CCSprite::createWithSpriteFrameName("menu2_ui_spend_delly.png");
        m_sprDelly[i]->setPosition(ccp(110, 0));
        m_menuItem[i]->addChild(m_sprDelly[i]);
        
        sprintf(lib->m_strTemp, "%d", staffManager->getRemainCount(m_curStaff));
        m_labelDelly[i] = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 20);
        m_labelDelly[i]->setColor(ccBLACK);
        m_labelDelly[i]->setPosition(ccp(30, 15));
        m_sprDelly[i]->addChild(m_labelDelly[i]);
  
        
        if(staffManager->getRemainCount(m_curStaff) == 0)
        {
            m_sprDelly[i]->setVisible(false);
        }
            
        if(staffManager->getGrade(m_curStaff)+1 != i)
            m_menuItem[i]->setVisible(false);
    }
    if(staffManager->getGrade(m_curStaff) == -1)
        m_menuItem[0]->setVisible(false);
    
    m_layerContent->addChild(m_menu);

}



void DyLayerShopStaff::promote()
{   
    YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_SHOWPICTURE);
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    moneyManager->addDelly(-staffManager->getRemainCount(m_curStaff), true);
    
    
    staffManager->promote(m_curStaff);
    
    
    refresh(staffManager->getGrade(m_curStaff));
    m_menuItem[staffManager->getGrade(m_curStaff)]->setVisible(false);
    if(staffManager->getGrade(m_curStaff) < staffManager->m_staff[m_curStaff]->count()-1)
    {
        m_menuItem[staffManager->getGrade(m_curStaff)+1]->setVisible(true);
        m_sprDelly[staffManager->getGrade(m_curStaff)+1]->setVisible(true);
    }
    
    
    dataManager->dataSave();
}




void DyLayerShopStaff::onPopupClicked(cocos2d::CCObject * pSender)
{
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
    
    if(pPopup->m_active_tag == 1)
	{
        if(staffManager->getRemainCount(m_curStaff) > moneyManager->getDelly())
            DyMoneyManager::sharedMoneyManager()->alramNotEnoughDelly(true, true);
        else
        {
            promote();
        }
 	}
}


void DyLayerShopStaff::onPromotionClicked(CCObject * obj)
{
    YtLib* lib = YtLib::sharedLib();
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);
    
    
    if(staffManager->getRemainCount(m_curStaff) > 0)
    {
        sprintf(lib->m_strTemp, STRING_QUEST_PROMOTE, staffManager->getRemainCount(m_curStaff));
        
        DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShopStaff::onPopupClicked));
        
        popup->setText(lib->m_strTemp);
        
        popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 0);
        popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 1);
        popup->addButton(DyButtonType_Close, "", 2);
        
        YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    }
    else
        promote();
}


void DyLayerShopStaff::onEnter()
{
	CCLayer::onEnter();

	if(m_isInit)
    {
		return;
    }
	m_isInit = true;
    
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    m_selectedIndex = staffManager->getGrade(m_curStaff);

    setPicture();
    
    m_layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), 570, 400);
    resetStaffLayer();
    
    m_scrollView = CCScrollView::create(m_layerContent->getContentSize(), m_layerContent);
    m_scrollView->setDelegate(this);
    
    m_scrollView->setContentSize(m_layerContent->getContentSize());
    m_scrollView->setDirection(kCCScrollViewDirectionHorizontal);
    m_scrollView->setContentOffset(ccp(0,0));
    m_scrollView->setGlobalScale(YtLib::sharedLib()->m_scale);
    m_scrollView->setScrollEnable(false);
	 
    m_scrollView->setPosition(ccp(300, 40));
    
    addChild(m_scrollView);
}


void DyLayerShopStaff::onExit()
{
	CCLayer::onExit();
}


void DyLayerShopStaff::scrollViewDidScroll(CCScrollView* view)
{
    
}

void DyLayerShopStaff::scrollViewDidZoom(CCScrollView* view)
{
    
}


void DyLayerShopStaff::refresh(int idxSelect)
{
    YtLib* lib = YtLib::sharedLib();
   
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    m_selectedIndex = idxSelect;
    
    setPicture();
    
    for(int i = 0;i < staffManager->m_staff[m_curStaff]->count();i++)
    {
        setPannel(i);
        
        sprintf(lib->m_strTemp, "%d", staffManager->getRemainCount(m_curStaff));
        m_labelDelly[i]->setString(lib->m_strTemp);

    }
}


void DyLayerShopStaff::refresh()
{
    //DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
    resetStaffLayer();
    refresh(staffManager->getGrade(m_curStaff));
    
}


void DyLayerShopStaff::scrollViewTouchContent(CCScrollView* view, CCPoint position)
{
    DyStaffManager* staffManager = DyStaffManager::sharedStaffManager();
    
	//DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
	int select_index = -1;
    
    if(staffManager->getGrade(m_curStaff) < 0)
        return;
    
	for(unsigned int i = 0;i < staffManager->m_staff[m_curStaff]->count();i++)
	{
		if(m_sprPannel[i]->boundingBox().containsPoint(position))
		{   
            if(i <= staffManager->getGrade(m_curStaff)+1 && i >= staffManager->getGrade(m_curStaff))
                select_index = i;
		}
	}
    
	if(select_index >= 0)
    {
        refresh(select_index);
    }
}

