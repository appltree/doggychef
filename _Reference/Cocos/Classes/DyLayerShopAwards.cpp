/*
DyLayerShopAwards.cpp
Doggy chef
@initialize at 121207

@history
121207	yoonsr	initialize
*/


#include "DyLayerShopAwards.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyBlockInfo.h"
#include "DyPopup.h"


DyLayerShopAwardsTab::DyLayerShopAwardsTab(CCRect rect):CCTabView(rect)
{
    if(SHOPAWARDS_CNT == 5)
    {
        m_leftMargin = 2.0;
        m_midPad = 0;
    }
    else
    {
        m_leftMargin = 50;
        m_midPad = 30;
	}
    
    m_thick = 50;
	m_upMargin = 0;
    m_selectedTab = 0;
	m_contentOffsetX = 80;
	m_contentOffsetY = 55;
}


DyLayerShopAwards::DyLayerShopAwards()
{
	m_tabIndex = 0;
	m_startTab = 0;
	m_tabView = NULL;

	m_labelNotiShop = NULL;
	m_labelNotiService = NULL;

	for(int i = 0;i < SHOPAWARDS_CNT;i ++)
	{
		m_tabScrollView[i] = NULL;
		m_menu[i] = NULL;
	}

	for(int i = 0;i < 8;i++)
	{
		m_customer[i] = NULL;
	}
    
    for(int i = 0;i < SHOPAWARDS_CNT;i++)
    {
        for(int j = 0;j < DYDATA_CUSTOMER_DEFAULT_CNT+DYDATA_CUSTOMER_ADDED_CNT;j++)
        {
            m_sprIndicator[i][j] = NULL;
            m_sprPicture[i][j] = NULL;
        }
    }
    
    m_posIndicatorGap = ccp(0,0);
    m_indicatorGapInc = false;
    m_isLockButton = false;
}


DyLayerShopAwards::~DyLayerShopAwards()
{
	m_contentLayer->release();

	for(int i = 0;i < 8;i++)
	{
		if(m_customer[i])
			YtAniObject::unloadAniObject(m_customer[i]);
	}
}



void DyLayerShopAwards::addIndicator(CCNode* node, CCPoint position, int mainIndex, int subIndex)
{
    m_posIndicator[mainIndex][subIndex] = position;
    m_sprIndicator[mainIndex][subIndex] = CCSprite::createWithSpriteFrameName("game_ui_hand.png");
    m_sprIndicator[mainIndex][subIndex]->setPosition(m_posIndicator[mainIndex][subIndex]);
    node->addChild(m_sprIndicator[mainIndex][subIndex], 10000);
}



CCLayerColor* DyLayerShopAwards::createAwardsLayer(DyLayerShopAwardsType type)
{
	//DyLib* lib = DyLib::sharedLib();

	DyDataManager* dataManager = DyDataManager::sharedDataManager();

	CCLayerColor* layerContent;
	switch(type)
	{
		case DyLayerShopAwardsType_Recipe:			
			{
				const char* arrFileNameRecipe[SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED] = {
					"awards_ui_recipe_snack_01.png",
					"awards_ui_recipe_hambuger_01.png",
					"awards_ui_recipe_coffee_01.png",
                    "awards_ui_recipe_china_01.png",
                    "awards_ui_recipe_pizza_01.png",
                    "awards_ui_recipe_korean_01.png",
				};
                
				const char* arrFileNameCostume[SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED] = {
					"awards_ui_recipe_snack_03.png",
					"awards_ui_recipe_hambuger_03.png",
					"awards_ui_recipe_coffee_03.png",
                    "awards_ui_recipe_china_03.png",
                    "awards_ui_recipe_pizza_03.png",
                    "awards_ui_recipe_korean_03.png",
				};

				layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), 860, 350);

				//m_menu[0] = CCMenu::create();

				//m_menu[0]->setPosition(CCPointZero);
				//layerContent->addChild(m_menu[0], 100);

				int width = 50;	
				for(int i = 0;i < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;i++)
				{
					float widthBg = 200;
					float widthGap = 25;

 					//CCMenuItemSprite *menuitemRecipe;
					//CCMenuItemSprite *menuitemCostume;
					
					if(dataManager->awards_get_complete(0, i))
					{
                        m_sprPicture[DyLayerShopAwardsType_Recipe][i*2] = CCSprite::createWithSpriteFrameName(arrFileNameRecipe[i]);
//						menuitemRecipe = CCMenuItemSprite::create(sprPicture,
//							NULL,
//							NULL,
//							this, menu_selector(DyLayerShopAwards::onPictureClicked));
    
						if(dataManager->awards_get_isSawAward(0, i) == false)
						{
							CCSprite* sprNew = CCSprite::createWithSpriteFrameName("menu_ui_new_mark.png");
							m_sprPicture[DyLayerShopAwardsType_Recipe][i*2]->addChild(sprNew);
							sprNew->setPosition(ccp(35+120, 110));
							dataManager->awards_set_isSawAward(0, i, true);
						}
                        
                        if(dataManager->awards_get_reward_count(0, i) < dataManager->awards_get_complete_count(0, i))
                        {
                            m_sprPicture[DyLayerShopAwardsType_Recipe][i*2]->setOpacity(128);
                            addIndicator(m_sprPicture[DyLayerShopAwardsType_Recipe][i*2], ccp(120, 80), DyLayerShopAwardsType_Recipe, i);
                        }
                  	}
					else
                    {
						m_sprPicture[DyLayerShopAwardsType_Recipe][i*2] = CCSprite::createWithSpriteFrameName("awards_ui_stamp_box_01.png");
                    }
                    
					m_sprPicture[DyLayerShopAwardsType_Recipe][i*2]->setPosition(ccp(50 + widthBg/2+(widthBg+widthGap)*i, 280));
					m_sprPicture[DyLayerShopAwardsType_Recipe][i*2]->setTag(i*2);
					layerContent->addChild(m_sprPicture[DyLayerShopAwardsType_Recipe][i*2]);

					if(dataManager->awards_get_complete(1, i))
					{
                        m_sprPicture[DyLayerShopAwardsType_Recipe][i*2+1] = CCSprite::createWithSpriteFrameName(arrFileNameCostume[i]);
//						menuitemCostume = CCMenuItemSprite::create(sprPicture,
//							NULL,
//							NULL,
//							this, menu_selector(DyLayerShopAwards::onPictureClicked));

						if(dataManager->awards_get_isSawAward(1, i) == false)
						{
							CCSprite* sprNew = CCSprite::createWithSpriteFrameName("menu_ui_new_mark.png");
							m_sprPicture[DyLayerShopAwardsType_Recipe][i*2+1]->addChild(sprNew);
							sprNew->setPosition(ccp(15, 170));
							dataManager->awards_set_isSawAward(1, i, true);
						}
                        
                        if(dataManager->awards_get_reward_count(1, i) < dataManager->awards_get_complete_count(1, i))
                        {
                            m_sprPicture[DyLayerShopAwardsType_Recipe][i*2+1]->setOpacity(128);
                            addIndicator(m_sprPicture[DyLayerShopAwardsType_Recipe][i*2+1], ccp(150, 150), DyLayerShopAwardsType_Recipe, i+3);
                        }
					}
					else
						m_sprPicture[DyLayerShopAwardsType_Recipe][i*2+1] = CCSprite::createWithSpriteFrameName("awards_ui_photo_box_01.png");

					m_sprPicture[DyLayerShopAwardsType_Recipe][i*2+1]->setPosition(ccp(50 + widthBg/2+(widthBg+widthGap)*i, 100));
					m_sprPicture[DyLayerShopAwardsType_Recipe][i*2+1]->setTag(i*2+1);
					layerContent->addChild(m_sprPicture[DyLayerShopAwardsType_Recipe][i*2+1]);

					width += (widthBg+widthGap);
				}

				layerContent->changeWidth(width);
			}
			break;
		case DyLayerShopAwardsType_Rank:
			{
//				const char* arrFileNameRank[4] = {
//					"awards_ui_trophy_01.png",
//					"awards_ui_trophy_02.png",
//					"awards_ui_trophy_03.png",
//					"awards_ui_trophy_04.png",
//				};

				const char* arrFileNameGold[3] = {
					"awards_ui_gold_01.png",
					"awards_ui_gold_02.png",
					"awards_ui_gold_03.png",
				};
                
				layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), 860, 350);

				m_menu[1] = CCMenu::create();

				m_menu[1]->setPosition(CCPointZero);
				layerContent->addChild(m_menu[1], 100);

				int width = 20;	
//				for(int i = 0;i < 4;i++)
//				{
//					float widthBg = 150;
//					float widthGap = 25;
//
// 					CCMenuItemSprite *menuitemRank;
//				
////                    dataManager->m_data.m_awards.m_rank[i].m_complete = true;
////                    dataManager->m_data.m_awards.m_rank[i].m_complete_count = 1;
//                    
//					if(DyAwardsDataSub_get_complete(dataManager->m_public.m_awards.m_rank[i]))
//					{
//                        CCSprite* sprPicture = CCSprite::createWithSpriteFrameName(arrFileNameRank[i]);
//                        
//						menuitemRank = CCMenuItemSprite::create(sprPicture,
//							NULL,
//							NULL,
//							this, menu_selector(DyLayerShopAwards::onPictureClicked));
//
//						if(DyAwardsDataSub_get_isSawAward(dataManager->m_public.m_awards.m_rank[i]) == false)
//						{
//							CCSprite* sprNew = CCSprite::createWithSpriteFrameName("menu_ui_new_mark.png");
//							menuitemRank->addChild(sprNew);
//							sprNew->setPosition(ccp(5, 120));
//							DyAwardsDataSub_set_isSawAward(dataManager->m_public.m_awards.m_rank[i], true);
//						}
//                        
//                        if(DyAwardsDataSub_get_reward_count(dataManager->m_public.m_awards.m_rank[i]) < DyAwardsDataSub_get_complete_count(dataManager->m_public.m_awards.m_rank[i]))
//                        {
//                            sprPicture->setOpacity(128);
//                            addIndicator(menuitemRank, ccp(80, 80), DyLayerShopAwardsType_Rank, i);
//                        }
//					}
//					else
//						menuitemRank = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("awards_ui_trophy_box.png"),
//							NULL,
//							NULL,
//							this, menu_selector(DyLayerShopAwards::onPictureClicked));
//
//					menuitemRank->setPosition(ccp(50 + widthBg/2+(widthBg+widthGap)*i, 270));
//					menuitemRank->setTag(i);
//					m_menu[1]->addChild(menuitemRank);
//
//					width += (widthBg+widthGap);
//				}

				width = 100;	
				for(int i = 0;i < 3;i++)
				{
					float widthBg = 200;
					float widthGap = 25;

 					CCMenuItemSprite *menuitemGold;
					
//                    dataManager->m_data.m_awards.m_gold[i].m_complete = true;
//                    dataManager->m_data.m_awards.m_gold[i].m_complete_count = 1;
                    
					if(dataManager->awards_get_complete(2, i))
					{
                        CCSprite* sprPicture = CCSprite::createWithSpriteFrameName(arrFileNameGold[i]);
                        
						menuitemGold = CCMenuItemSprite::create(sprPicture,
							NULL,
							NULL,
							this, menu_selector(DyLayerShopAwards::onPictureClicked));

						if(dataManager->awards_get_isSawAward(2, i) == false)
						{
							CCSprite* sprNew = CCSprite::createWithSpriteFrameName("menu_ui_new_mark.png");
							menuitemGold->addChild(sprNew);
							sprNew->setPosition(ccp(15, 130));
							dataManager->awards_set_isSawAward(2, i, true);
						}
                        
                        if(dataManager->awards_get_reward_count(2, i) < dataManager->awards_get_complete_count(2, i))
                        {
                            sprPicture->setOpacity(128);
                            addIndicator(menuitemGold, ccp(120, 100), DyLayerShopAwardsType_Rank, i+4);
                        }
					}
					else
						menuitemGold = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("awards_ui_stamp_box_01.png"),
							NULL,
							NULL,
							this, menu_selector(DyLayerShopAwards::onPictureClicked));

					menuitemGold->setPosition(ccp(50 + widthBg/2+(widthBg+widthGap)*i, 200));
					menuitemGold->setTag(i+4);
					m_menu[1]->addChild(menuitemGold);

					width += (widthBg+widthGap);
				}

				layerContent->changeWidth(width);
			}
			break;
		case DyLayerShopAwardsType_Shop:
			{
				layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), 0, 350);

				const char* arrFileName[SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED] = {
					"image_photo_snack_gray.png",
					"image_photo_hamberger_gray.png",
					"image_photo_coffee_gray.png",
                    "image_photo_china_gray.png",
                    
#if SHOP_COUNT_ADDED > 1
                    "image_photo_pizza_gray.png",
#endif
                    
#if SHOP_COUNT_ADDED > 2
                    "image_photo_korean_gray.png",
#endif
                    

				};

				const char* arrFileName_Color[SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED] = {
					"image_photo_snack_color.png",
					"image_photo_hamberger_color.png",
					"image_photo_coffee_color.png",
                    "image_photo_china_color.png",
#if SHOP_COUNT_ADDED > 1
                    "image_photo_pizza_color.png",
#endif
                    
#if SHOP_COUNT_ADDED > 2
                    "image_photo_korean_color.png",
#endif
                
                    
				};

				int width = 0;
                
                //dataManager->m_data.m_awards.m_shop[0].m_complete_count = 4;
                
				for(int i = 0;i < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;i++)
				{
					m_sprPicture[DyLayerShopAwardsType_Shop][i] = CCSprite::create("image_photo_bg.png");

                    float widthBg = m_sprPicture[DyLayerShopAwardsType_Shop][i]->getContentSize().width;
					float widthGap = 25;

					CCSprite* sprPin = CCSprite::createWithSpriteFrameName("awards_ui_photo_pin.png");
					m_sprPicture[DyLayerShopAwardsType_Shop][i]->addChild(sprPin);
					sprPin->setPosition(ccp(widthBg/2-10, m_sprPicture[DyLayerShopAwardsType_Shop][i]->getContentSize().height - 5));
					
					CCSprite* sprPicture;
					
					if(dataManager->awards_get_complete(3, i))
						sprPicture = CCSprite::create(arrFileName_Color[i]);
					else
						sprPicture = CCSprite::create(arrFileName[i]);
					m_sprPicture[DyLayerShopAwardsType_Shop][i]->addChild(sprPicture);
					sprPicture->setPosition(ccp(widthBg/2-4, 160));
					
					for(int j = 0;j < 3;j++)
					{
						CCSprite* star;
                        
                        CCSprite* starDim = CCSprite::createWithSpriteFrameName("awards_ui_star_01.png");
                        m_sprPicture[DyLayerShopAwardsType_Shop][i]->addChild(starDim);
                        starDim->setPosition(ccp(widthBg/2-75-4 + 75*j, 45));
                        
						if(j < dataManager->awards_get_complete_count(3, i)-1)
						{
							star = CCSprite::createWithSpriteFrameName("awards_ui_star_02.png");
                            star->setTag(SHOPAWARDS_UI_TAG_DOJANG + j);
                            
                            if(j >= dataManager->awards_get_reward_count(3, i)-1)
                            {
                                star->setOpacity(0);
                            }
                            m_sprPicture[DyLayerShopAwardsType_Shop][i]->addChild(star);
                            
                            star->setPosition(starDim->getPosition());
						}
					}

					if(dataManager->awards_get_complete(3, i) && dataManager->awards_get_isSawAward(3, i) == false)
					{
						CCSprite* sprNew = CCSprite::createWithSpriteFrameName("menu_ui_new_mark.png");
						m_sprPicture[DyLayerShopAwardsType_Shop][i]->addChild(sprNew);
						sprNew->setPosition(ccp(30, 260));
						dataManager->awards_set_isSawAward(3, i, true);
					}
                    
                    if(dataManager->awards_get_reward_count(3, i) < dataManager->awards_get_complete_count(3, i))
                    {
                        addIndicator(m_sprPicture[DyLayerShopAwardsType_Shop][i], ccp(210, 210), DyLayerShopAwardsType_Shop, i);
                    }

					m_sprPicture[DyLayerShopAwardsType_Shop][i]->setPosition(ccp(widthBg/2+(widthBg+widthGap)*i, 185));
					layerContent->addChild(m_sprPicture[DyLayerShopAwardsType_Shop][i], 100);
                    
					width += (widthBg+widthGap);
				}

				layerContent->changeWidth(width);

			}
			break;
		case DyLayerShopAwardsType_Service:
			{
				DyCharacterManager* characterManager = DyCharacterManager::sharedCharacterManager();

				layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), 860, 350);

				int width = 0;
                
                //dataManager->m_data.m_awards.m_service[0].m_complete = true;
                //dataManager->m_data.m_awards.m_service[0].m_complete_count = 4;
                
				for(unsigned int i = 0;i < DYDATA_CUSTOMER_DEFAULT_CNT+DYDATA_CUSTOMER_ADDED_CNT;i++)
				{
					DyCharacterInfo* characterInfo = characterManager->getCharacterInfoByID(i);
                    
                    if(characterInfo == NULL)
                        continue;
                    
					m_sprPicture[DyLayerShopAwardsType_Service][i] = CCSprite::createWithSpriteFrameName("awards_ui_customer_bg.png");

					float widthBg = m_sprPicture[DyLayerShopAwardsType_Service][i]->getContentSize().width;
					float widthGap = 25;

					CCLabelTTF* labelName;
                    
                    //
                    // 캐릭터 강제 오픈 
                    //dataManager->awards_set_complete_count(4, i, 1);
                    
					if(dataManager->awards_get_complete_count(4, i) > 0)
					{
						CCSprite* sprShadow = CCSprite::createWithSpriteFrameName("awards_ui_shadow.png");
						sprShadow->setPosition(ccp(widthBg/2, 110));
                        sprShadow->setTag(SHOPAWARDS_UI_TAG_SERVICE_SHADOW);
						m_sprPicture[DyLayerShopAwardsType_Service][i]->addChild(sprShadow);

						m_customer[i] = YtAniObject::loadAniObject(this, characterInfo->m_url.c_str());
						m_customer[i]->setPosition(ccp(widthBg/2, 110));
						m_customer[i]->m_actionManager->runAnimations("order");
						m_customer[i]->setScale(0.8f);
                        
						m_sprPicture[DyLayerShopAwardsType_Service][i]->addChild(m_customer[i]);

						labelName = CCLabelTTF::create(characterInfo->m_explain.c_str(), FONT_NAME_EXP, 20.f);

						labelName->setColor(ccc3(57,27,5));
						labelName->setPosition(ccp(widthBg/2, 75));
						m_sprPicture[DyLayerShopAwardsType_Service][i]->addChild(labelName);
                        labelName->setTag(SHOPAWARDS_UI_TAG_SERVICE_NAME);

                        for(int j = 0;j < 3;j++)
                        {   
                            CCSprite* heartDim = CCSprite::createWithSpriteFrameName("awards_ui_heart_01.png");
                            
                            m_sprPicture[DyLayerShopAwardsType_Service][i]->addChild(heartDim);
                            heartDim->setPosition(ccp(widthBg/2-55-4 + 55*j, 40));
                            
                            if(j < dataManager->awards_get_complete_count(4, i)-1)
                            {
                                CCSprite* heart = CCSprite::createWithSpriteFrameName("awards_ui_heart_02.png");
                                
                                m_sprPicture[DyLayerShopAwardsType_Service][i]->addChild(heart);
                                heart->setPosition(heartDim->getPosition());
                                heart->setTag(SHOPAWARDS_UI_TAG_SERVICE_DOJANG+j);
                                
                                if(j >= dataManager->awards_get_reward_count(4, i)-1)
                                {
                                    heart->setOpacity(0);
                                }
                                
                            }
                        }

                        CCSprite* sprMark = characterManager->getCharacterMarkSprite(i);
                        if(sprMark)
                        {
                            sprMark->setPosition(ccp(147, 240));
                            m_sprPicture[DyLayerShopAwardsType_Service][i]->addChild(sprMark, 100);
                        }

						if(dataManager->awards_get_complete(4, i) && dataManager->awards_get_isSawAward(4, i) == false)
						{
							CCSprite* sprNew = CCSprite::createWithSpriteFrameName("menu_ui_new_mark.png");
							m_sprPicture[DyLayerShopAwardsType_Service][i]->addChild(sprNew);
							sprNew->setPosition(ccp(30, 260));
							dataManager->awards_set_isSawAward(4, i, true);
						}
                        
                        if(dataManager->awards_get_reward_count(4, i) < dataManager->awards_get_complete_count(4, i))
                        {
                            if(dataManager->awards_get_reward_count(4, i) == 0)
                            {
                                m_sprPicture[DyLayerShopAwardsType_Service][i]->setOpacity(128);
                                sprShadow->setOpacity(128);
                                m_sprPicture[DyLayerShopAwardsType_Service][i]->reorderChild(m_customer[i], -1);
                                labelName->setOpacity(128);
                            }
                            addIndicator(m_sprPicture[DyLayerShopAwardsType_Service][i], ccp(130, 220), DyLayerShopAwardsType_Service, i);
                        }
                        
                        m_sprPicture[DyLayerShopAwardsType_Service][i]->setPosition(ccp(widthBg/2+(widthBg+widthGap)*i, 185));
						layerContent->addChild(m_sprPicture[DyLayerShopAwardsType_Service][i], 100);
                    }
					else
					{
						m_sprPicture[DyLayerShopAwardsType_Service][i] = CCSprite::createWithSpriteFrameName("awards_ui_photo_box_02.png");
						m_sprPicture[DyLayerShopAwardsType_Service][i]->setPosition(ccp(widthBg/2+(widthBg+widthGap)*i, 185));
						layerContent->addChild(m_sprPicture[DyLayerShopAwardsType_Service][i], 100);
                    }
					
					width += (widthBg+widthGap);
                    
				}
				layerContent->changeWidth(width);
			}
			break;
//        case DyLayerShopAwardsType_Social:
//            {
//                layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), 860, 350);
//                
//                m_menu[4] = CCMenu::create();
//                
//                m_menu[4]->setPosition(CCPointZero);
//                layerContent->addChild(m_menu[4], 100);
//                
//                
//                for(int i = 0;i < 3;i++)
//                {
//                    for(int j = 0;j < 5;j++)
//                    {
//                        int gapX;
//                        int startX;
//                        CCSprite* sprBack;
//                            
//                        if(i == 1)
//                        {
//                            startX = 100;
//                            gapX = 140;
//                            
//                            sprBack = CCSprite::createWithSpriteFrameName("awards_ui_dog_back.png");
//                        }
//                        else
//                        {
//                            startX = 120;
//                            gapX = 130;
//                            
//                            sprBack = CCSprite::createWithSpriteFrameName("awards_ui_gum_foot_back.png");
//                        }
//
//                        CCMenuItemSprite *menuitem = CCMenuItemSprite::create(sprBack,
//                                                                NULL,
//                                                                NULL,
//                                                                this, menu_selector(DyLayerShopAwards::onPictureClicked));
//                        
//                        menuitem->setPosition(ccp(startX + gapX*j, 280 - i*110));
//                        menuitem->setTag(i*5+j);
//                        m_menu[4]->addChild(menuitem);
//                    }
//                }
//                
//                for(int i = 0;i < 5;i++)
//                {
//                    CCMenuItemSprite *menuitem = (CCMenuItemSprite *)m_menu[4]->getChildByTag(i);
//                    
//                    CCSprite* sprPicture;
//                    
//    //                if(i == 4)
//    //                    sprPicture = CCSprite::createWithSpriteFrameName("awards_ui_gum_off_2.png");
//    //                else
//    //                    sprPicture = CCSprite::createWithSpriteFrameName("awards_ui_gum_off.png");
//                    if(i == 4)
//                    {
//                        sprPicture = CCSprite::createWithSpriteFrameName("awards_ui_gum_on_2.png");
//                        sprPicture->setColor(ccc3(130,0,5));
//                    }
//                    else
//                    {
//                        sprPicture = CCSprite::createWithSpriteFrameName("awards_ui_gum_on.png");
//                        sprPicture->setColor(ccc3(212,0,5));
//                        sprPicture->setOpacity(100+(i*155/3));
//                    }
//
//                    sprPicture->setPosition(ccp(menuitem->getContentSize().width/2-3, menuitem->getContentSize().height/2+3));
//                    menuitem->addChild(sprPicture);
//                }
//                
//                for(int i = 0;i < 5;i++)
//                {
//                    CCMenuItemSprite *menuitem = (CCMenuItemSprite *)m_menu[4]->getChildByTag(i+5);
//                    
//                    CCSprite* sprPicture;
//                    
//    //                if(i == 4)
//    //                    sprPicture = CCSprite::createWithSpriteFrameName("awards_ui_dog_off_2.png");
//    //                else
//    //                    sprPicture = CCSprite::createWithSpriteFrameName("awards_ui_dog_off.png");
//                    if(i == 4)
//                    {
//                        sprPicture = CCSprite::createWithSpriteFrameName("awards_ui_dog_on_2.png");
//                        sprPicture->setColor(ccc3(14,56,49));
//                    }
//                    else
//                    {
//                        sprPicture = CCSprite::createWithSpriteFrameName("awards_ui_dog_on.png");
//                        sprPicture->setColor(ccc3(24,100,88));
//                        sprPicture->setOpacity(100+(i*155/3));
//                    }
//                    
//                    sprPicture->setPosition(ccp(menuitem->getContentSize().width/2-3, menuitem->getContentSize().height/2+3));
//                    menuitem->addChild(sprPicture);
//                }
//                
//                for(int i = 0;i < 5;i++)
//                {
//                    CCMenuItemSprite *menuitem = (CCMenuItemSprite *)m_menu[4]->getChildByTag(i+5*2);
//                    
//                    CCSprite* sprPicture;
//                    
////                    if(i == 4)
////                        sprPicture = CCSprite::createWithSpriteFrameName("awards_ui_foot_off_2.png");
////                    else
////                        sprPicture = CCSprite::createWithSpriteFrameName("awards_ui_foot_off.png");
//                    if(i == 4)
//                    {
//                        sprPicture = CCSprite::createWithSpriteFrameName("awards_ui_foot_on_2.png");
//                        sprPicture->setColor(ccc3(19,31,62));
//                    }
//                    else
//                    {
//                        sprPicture = CCSprite::createWithSpriteFrameName("awards_ui_foot_on.png");
//                        sprPicture->setColor(ccc3(24,45,105));
//                        sprPicture->setOpacity(100+(i*155/3));
//                    }
//                    
//                    sprPicture->setPosition(ccp(menuitem->getContentSize().width/2-3, menuitem->getContentSize().height/2+3));
//                    menuitem->addChild(sprPicture);
//                }
//                
//            }
//            break;
	}

	dataManager->dataSave();

	return layerContent;
}

//void DyLayerShopAwards::onSocialClicked(cocos2d::CCObject * pSender)
//{
//    
//}


void DyLayerShopAwards::scrollViewTouchContent(CCScrollView* view, CCPoint position)
{
    DyLib* lib = DyLib::sharedLib();
    
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    DyItemManager* itemManager = DyItemManager::sharedItemManager();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
	int select_index = -1;
    
	if(m_tabIndex == 0)
	{
        DyPopup* popup = NULL;
        
        for(unsigned int i = 0;i < (SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED)*2;i++)
        {
            if(m_sprPicture[DyLayerShopAwardsType_Recipe][i]->boundingBox().containsPoint(position))
            {
                select_index = i;
                break;
            }
        }

        
		if(select_index >= 0 && select_index%2 == 0)
		{
            if(dataManager->awards_get_reward_count(0, select_index/2) < dataManager->awards_get_complete_count(0, select_index/2))
            {
                int reward_gum = 10;
                
                YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_SHOWPICTURE);
                
                //CCSprite* sprNormal = (CCSprite*)pMenu->getNormalImage();
                m_sprPicture[DyLayerShopAwardsType_Recipe][select_index]->runAction(CCSequence::create(CCFadeIn::create(0.5), CCCallFuncN::create(this, callfuncN_selector(DyLayerShopAwards::endAnimationReward)), NULL));
                
                sprintf(m_strReward, "%s %d%s%s",
                        STRING_gum,
                        reward_gum,
                        STRING_count,
                        STRING_rewarded
                        );
                
                m_sprPicture[DyLayerShopAwardsType_Recipe][select_index]->setUserData(this);
                
                dataManager->awards_set_reward_count(0, select_index/2, 1);
                
                DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
                
                moneyManager->addGum(reward_gum, true);
                DyDataManager::sharedDataManager()->m_rewardedGum += reward_gum;
                dataManager->dataSave();
                
                m_isLockButton = true;
                m_sprIndicator[DyLayerShopAwardsType_Recipe][select_index/2]->setVisible(false);
            }
            else
            {
                popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
                
                sprintf(lib->m_strTemp2, STRING_awards_recipe_0,
                        stageManager->getShopName(select_index/2).c_str());
                
                if(dataManager->awards_get_complete(0, select_index/2))
                    sprintf(lib->m_strTemp, "%s%s", lib->m_strTemp2, STRING_awards_word_complete);
                else
                    sprintf(lib->m_strTemp, "%s%s", lib->m_strTemp2, STRING_awards_word_complete_notyet);
            }
		}
		else if(select_index >= 0)
		{
            if(dataManager->awards_get_reward_count(1, select_index/2) < dataManager->awards_get_complete_count(1, select_index/2))
            {
                int reward_gum = 5;
                
                YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_SHOWPICTURE);
                
                //CCSprite* sprNormal = (CCSprite*)pMenu->getNormalImage();
                m_sprPicture[DyLayerShopAwardsType_Recipe][select_index]->runAction(CCSequence::create(CCFadeIn::create(0.5), CCCallFuncN::create(this, callfuncN_selector(DyLayerShopAwards::endAnimationReward)), NULL));
                
                sprintf(m_strReward, "%s %d%s%s",
                        STRING_gum,
                        reward_gum,
                        STRING_count,
                        STRING_rewarded
                        );
                
                
                m_sprPicture[DyLayerShopAwardsType_Recipe][select_index]->setUserData(this);
                
                dataManager->awards_set_reward_count(1, select_index/2, 1);
                
                DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
                
                moneyManager->addGum(reward_gum, true);
                DyDataManager::sharedDataManager()->m_rewardedGum += reward_gum;
                
                dataManager->dataSave();
                
                m_isLockButton = true;
                m_sprIndicator[DyLayerShopAwardsType_Recipe][select_index/2+3]->setVisible(false);
            }
            else
            {
                DyCostume* costume = NULL;
                
                switch(select_index/2)
                {
                    case 0:
                        costume = DyCostumeManager::sharedCostumeManager()->getCostume("set_01");
                        break;
                    case 1:
                        costume = DyCostumeManager::sharedCostumeManager()->getCostume("set_05");
                        break;
                    case 2:
                        costume = DyCostumeManager::sharedCostumeManager()->getCostume("set_06");
                        break;
                    case 3:
                        costume = DyCostumeManager::sharedCostumeManager()->getCostume("set_14");
                        break;
                    case 4:
                        costume = DyCostumeManager::sharedCostumeManager()->getCostume("set_19");
                        break;
                    case 5:
                        costume = DyCostumeManager::sharedCostumeManager()->getCostume("set_20");
                        break;
         
                }
                //assert(select_index/2 < 3);
                
                popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
                
                
                sprintf(lib->m_strTemp2, STRING_awards_recipe_1,
                        stageManager->getShopName(select_index/2).c_str(), costume->m_name.c_str());
                
                if(dataManager->awards_get_complete(1, select_index/2))
                    sprintf(lib->m_strTemp, "%s%s", lib->m_strTemp2, STRING_awards_word_complete);
                else
                    sprintf(lib->m_strTemp, "%s%s", lib->m_strTemp2, STRING_awards_word_complete_notyet);
            }
		}
        
        if(popup)
        {
            popup->setText(lib->m_strTemp);
            popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
            YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        }

	}
    if(m_tabIndex == 2)
    {
        for(unsigned int i = 0;i < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;i++)
        {
            if(m_sprPicture[DyLayerShopAwardsType_Shop][i]->boundingBox().containsPoint(position))
            {
                select_index = i;
                break;
            }
        }
        
        if(select_index >= 0 && dataManager->awards_get_reward_count(3, select_index) < dataManager->awards_get_complete_count(3, select_index))
        {
            int delly_reward[4] = {0, 1, 3, 5};
            
            int delly_cnt = 0;// 델리가 0인경우는 없다.(13.12.29)
            for(int i = dataManager->awards_get_reward_count(3, select_index);i < dataManager->awards_get_complete_count(3, select_index);i++)
            {
                delly_cnt += delly_reward[i];
                
                CCSprite* sprDojang = (CCSprite*)m_sprPicture[DyLayerShopAwardsType_Shop][select_index]->getChildByTag(SHOPAWARDS_UI_TAG_DOJANG + i-1);
                sprDojang->setOpacity(255);
                sprDojang->setScale(1.2);
  
                if(i == dataManager->awards_get_complete_count(3, select_index)-1)
                {
                    if(delly_cnt == 0)
                    {
                        sprintf(m_strReward, "시계 %d개 받으셨어요.", 3);
                        itemManager->addItem(DyItemType_TimeIncrease, 3);
                    }
                    else
                    {
                        sprintf(m_strReward, "%d 델리를 받으셨어요.", delly_cnt);
                        moneyManager->addDelly(delly_cnt, true);
                    }
                    
                    sprDojang->setUserData(this);
                    
                    dataManager->awards_set_reward_count(3, select_index, dataManager->awards_get_complete_count(3, select_index));
                    
                    dataManager->dataSave();
                    
                    m_isLockButton = true;
                    m_sprIndicator[DyLayerShopAwardsType_Shop][select_index]->setVisible(false);

                    sprDojang->runAction(CCSequence::create(CCScaleTo::create(0.05, 1.0f), CCCallFuncN::create(this, callfuncN_selector(DyLayerShopAwards::endAnimationRewardDojang)), NULL));
                }
                else
                {
                    sprDojang->runAction(CCSequence::create(CCScaleTo::create(0.05, 1.0f), NULL));
                    
                }
            }
        }
    }
    if(m_tabIndex == 3)
    {
        DyCharacterManager* characterManager = DyCharacterManager::sharedCharacterManager();

        for(unsigned int i = 0;i < DYDATA_CUSTOMER_DEFAULT_CNT+DYDATA_CUSTOMER_ADDED_CNT;i++)
        {
            DyCharacterInfo* characterInfo = characterManager->getCharacterInfoByID(i);
            
            if(characterInfo == NULL)
                continue;
            if(m_sprPicture[DyLayerShopAwardsType_Service][i]->boundingBox().containsPoint(position))
            {
                select_index = i;
                break;
            }
        }
        
        if(select_index >= 0 && dataManager->awards_get_reward_count(4, select_index) < dataManager->awards_get_complete_count(4, select_index))
        {
            int cnt_candy = 0;
            if(dataManager->awards_get_reward_count(4, select_index) == 0)
            {
                YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_SHOWPICTURE);
                
                m_sprPicture[DyLayerShopAwardsType_Service][select_index]->setOpacity(255);
                CCSprite* sprShadow = (CCSprite*)m_sprPicture[DyLayerShopAwardsType_Service][select_index]->getChildByTag(SHOPAWARDS_UI_TAG_SERVICE_SHADOW);
                
                sprShadow->runAction(CCSequence::create(CCFadeIn::create(0.5), NULL));
                
                m_sprPicture[DyLayerShopAwardsType_Service][select_index]->reorderChild(m_customer[select_index], 1);
                
                CCLabelTTF* labelName = (CCLabelTTF*)m_sprPicture[DyLayerShopAwardsType_Service][select_index]->getChildByTag(SHOPAWARDS_UI_TAG_SERVICE_NAME);
                labelName->setUserData(this);
                
                
                if(dataManager->awards_get_complete_count(4, select_index) == 1)
                    labelName->runAction(CCSequence::create(CCFadeIn::create(0.5), CCCallFuncN::create(this, callfuncN_selector(DyLayerShopAwards::endAnimationReward)), NULL));
                else
                    labelName->runAction(CCSequence::create(CCFadeIn::create(0.5), NULL));
                
                dataManager->awards_set_reward_count(4, select_index, 1);
                cnt_candy = 1;
            }
            
            for(int i = dataManager->awards_get_reward_count(4, select_index);i < dataManager->awards_get_complete_count(4, select_index);i++)
            {
                cnt_candy += 3;
                
                
                CCSprite* sprDojang = (CCSprite*)m_sprPicture[DyLayerShopAwardsType_Service][select_index]->getChildByTag(SHOPAWARDS_UI_TAG_SERVICE_DOJANG + i-1);
                sprDojang->setOpacity(255);
                sprDojang->setScale(1.2);
                sprDojang->setUserData(this);
                
                if(i == dataManager->awards_get_complete_count(4, select_index)-1)
                {   
                    sprDojang->runAction(CCSequence::create(CCScaleTo::create(0.05, 1.0f), CCCallFuncN::create(this, callfuncN_selector(DyLayerShopAwards::endAnimationRewardDojang)), NULL));
                }
                else
                {
                    sprDojang->runAction(CCSequence::create(CCScaleTo::create(0.05, 1.0f), NULL));
                    
                }
            }
            
            sprintf(m_strReward, "%s %d%s%s",
                    itemManager->getItem(DyItemType_Candy)->m_name.c_str(),
                    cnt_candy,
                    STRING_count,
                    STRING_rewarded
                    );
            
            itemManager->addItem(DyItemType_Candy, cnt_candy);
            
           
            dataManager->awards_set_reward_count(4, select_index, dataManager->awards_get_complete_count(4, select_index));
            
            dataManager->dataSave();
            
            m_isLockButton = true;
            m_sprIndicator[DyLayerShopAwardsType_Service][select_index]->setVisible(false);
        }
        
        
        if(select_index >= 0 && dataManager->awards_get_complete(4, select_index) == false)
        {
            DyPopup *popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
            popup->setText(STRING_DANGOL_ALRAM);
            popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
            YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
 
        }
    }
}

void DyLayerShopAwards::scrollViewDidScroll(CCScrollView* view)
{
}


void DyLayerShopAwards::scrollViewDidZoom(CCScrollView* view)
{
}





void DyLayerShopAwards::updateAnimation()
{
    if(m_indicatorGapInc)
    {
        m_posIndicatorGap.x -= 6.f;
        m_posIndicatorGap.y -= 6.f;
        
        if(m_posIndicatorGap.x < 0)
        {
            m_posIndicatorGap.x = 0;
            m_posIndicatorGap.y = 0;
            m_indicatorGapInc = false;
        }
    }
    else
    {
        m_posIndicatorGap.x += 10.f;
        m_posIndicatorGap.y += 10.f;
        
        if(m_posIndicatorGap.x > 20)
        {
            m_posIndicatorGap.x = 20;
            m_posIndicatorGap.y = 20;
            m_indicatorGapInc = true;
        }
        
    }

    for(int i = 0;i < SHOPAWARDS_CNT;i++)
    {
        for(int j = 0;j < DYDATA_CUSTOMER_DEFAULT_CNT+DYDATA_CUSTOMER_ADDED_CNT;j++)
        {
            
            if(m_sprIndicator[i][j])
                m_sprIndicator[i][j]->setPosition(ccp(m_posIndicator[i][j].x + m_posIndicatorGap.x, m_posIndicator[i][j].y + m_posIndicatorGap.y));
        }
    }
}


void DyLayerShopAwards::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader)
{
	m_contentLayer = (CCLayer* )getChildByTag(SHOPAWARDS_UI_TAG_BG_BOTTOM);
	m_contentLayer->removeFromParentAndCleanup(false);
	m_contentLayer->retain();

}


void DyLayerShopAwards::endAnimationReward(CCNode* node)
{
    DyLayerShopAwards* layerShopAwards = (DyLayerShopAwards*)node->getUserData();
    
    DyPopup *popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
    popup->setText(layerShopAwards->m_strReward);
    popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
    YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
    
    layerShopAwards->m_isLockButton = false;
}



void DyLayerShopAwards::endAnimationRewardDojang(CCNode* node)
{
    YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_DOJANG);
    endAnimationReward(node);
}


void DyLayerShopAwards::onPictureClicked(cocos2d::CCObject * pSender)
{
    if(m_isLockButton)
        return;
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	//DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyDataManager* dataManager = DyDataManager::sharedDataManager();

	CCMenuItemSprite * pMenu = dynamic_cast<CCMenuItemSprite*>(pSender);
	DyPopup* popup = NULL;
	DyLib* lib = DyLib::sharedLib();

	int index = pMenu->getTag();
	/*if(m_tabIndex == 0)
	{
		if(index%2 == 0)
		{
            if(dataManager->awards_get_reward_count(0, index/2) < dataManager->awards_get_complete_count(0, index/2))
            {
                int reward_gum = 10;
                
                YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_SHOWPICTURE);
                
                CCSprite* sprNormal = (CCSprite*)pMenu->getNormalImage();
                sprNormal->runAction(CCSequence::create(CCFadeIn::create(0.5), CCCallFuncN::create(this, callfuncN_selector(DyLayerShopAwards::endAnimationReward)), NULL));
                
                sprintf(m_strReward, "%s %d%s%s",
                        STRING_gum,
                        reward_gum,
                        STRING_count,
                        STRING_rewarded
                        );
                
                sprNormal->setUserData(this);
                
                dataManager->awards_set_reward_count(0, index/2, 1);
                
                DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
                
                moneyManager->addGum(reward_gum, true);
                DyDataManager::sharedDataManager()->m_rewardedGum += reward_gum;
                dataManager->dataSave();
                
                m_isLockButton = true;
                m_sprIndicator[DyLayerShopAwardsType_Recipe][index/2]->setVisible(false);
            }
            else
            {
                popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
                
                sprintf(lib->m_strTemp2, STRING_awards_recipe_0,
                    stageManager->getShopName(index/2).c_str());

                if(dataManager->awards_get_complete(0, index/2))
                    sprintf(lib->m_strTemp, "%s%s", lib->m_strTemp2, STRING_awards_word_complete);
                else
                    sprintf(lib->m_strTemp, "%s%s", lib->m_strTemp2, STRING_awards_word_complete_notyet);
            }
		}
		else
		{
            if(dataManager->awards_get_reward_count(1, index/2) < dataManager->awards_get_complete_count(1, index/2))
            {
                int reward_gum = 5;
                
                YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_SHOWPICTURE);
                
                CCSprite* sprNormal = (CCSprite*)pMenu->getNormalImage();
                sprNormal->runAction(CCSequence::create(CCFadeIn::create(0.5), CCCallFuncN::create(this, callfuncN_selector(DyLayerShopAwards::endAnimationReward)), NULL));
                
                sprintf(m_strReward, "%s %d%s%s",
                        STRING_gum,
                        reward_gum,
                        STRING_count,
                        STRING_rewarded
                        );
                
                
                sprNormal->setUserData(this);
                
                dataManager->awards_set_reward_count(1, index/2, 1);
                
                DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
                
                moneyManager->addGum(reward_gum, true);
                DyDataManager::sharedDataManager()->m_rewardedGum += reward_gum;
                
                dataManager->dataSave();
                
                m_isLockButton = true;
                m_sprIndicator[DyLayerShopAwardsType_Recipe][index/2+3]->setVisible(false);
            }
            else
            {
                DyCostume* costume = NULL;
                
                switch(index/2)
                {
                    case 0:
                        costume = DyCostumeManager::sharedCostumeManager()->getCostume("set_01");
                        break;
                    case 1:
                        costume = DyCostumeManager::sharedCostumeManager()->getCostume("set_05");
                        break;
                    case 2:
                        costume = DyCostumeManager::sharedCostumeManager()->getCostume("set_06");
                        break;
                    case 3:
                        costume = DyCostumeManager::sharedCostumeManager()->getCostume("set_14");
                        break;

                }
                assert(index/2 < 3);
                
                popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
                
                
                sprintf(lib->m_strTemp2, STRING_awards_recipe_1,
                    stageManager->getShopName(index/2).c_str(), costume->m_name.c_str());

                if(dataManager->awards_get_complete(1, index/2))
                    sprintf(lib->m_strTemp, "%s%s", lib->m_strTemp2, STRING_awards_word_complete);
                else
                    sprintf(lib->m_strTemp, "%s%s", lib->m_strTemp2, STRING_awards_word_complete_notyet);
            }
		}
	}
	else*/ if(m_tabIndex == 1)
	{
//    	if(index < 4)
//		{
//            if(DyAwardsDataSub_get_reward_count(dataManager->m_public.m_awards.m_award_rank[index]) < DyAwardsDataSub_get_complete_count(dataManager->m_public.m_awards.m_award_rank[index]))
//            {
//                DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
// 
//                YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_SHOWPICTURE);
//                
//                CCSprite* sprNormal = (CCSprite*)pMenu->getNormalImage();
//                sprNormal->runAction(CCSequence::create(CCFadeIn::create(0.5), CCCallFuncN::create(this, callfuncN_selector(DyLayerShopAwards::endAnimationReward)), NULL));
//                
//                int gold = 10000;
//                
//                switch(index)
//                {
//                    case 0:
//                        gold = 10000;
//                        break;
//                    case 1:
//                        gold = 30000;
//                        break;
//                    case 2:
//                        gold = 5000;
//                        break;
//                    case 3:
//                        gold = 1500;
//                        break;
//                }
//                
//                sprintf(m_strReward, "%d%s%s",
//                        gold,
//                        lib->m_dicText->valueForKey("gold")->getCString(),
//                        lib->m_dicText->valueForKey("rewarded")->getCString()
//                        );
//                moneyManager->addGold(gold, true);
//                
//                sprNormal->setUserData(this);
//                
//                DyAwardsDataSub_set_reward_count(dataManager->m_public.m_awards.m_award_rank[index], 1);
//                
//                dataManager->dataSave();
//                
//                m_isLockButton = true;
//                m_sprIndicator[DyLayerShopAwardsType_Rank][index]->setVisible(false);
//            }
//            else
//            {
//                popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
//                
//                sprintf(lib->m_strTemp3, "awards_rank_thropy_%d", index);
//                sprintf(lib->m_strTemp2, "%s", lib->m_dicText->valueForKey(lib->m_strTemp3)->getCString());
//
//                if(DyAwardsDataSub_get_complete(dataManager->m_public.m_awards.m_award_rank[index]))
//                    sprintf(lib->m_strTemp, "%s%s", lib->m_strTemp2, lib->m_dicText->valueForKey("awards_word_complete")->getCString());
//                else
//                    sprintf(lib->m_strTemp, "%s%s", lib->m_strTemp2, lib->m_dicText->valueForKey("awards_word_complete_notyet")->getCString());
//            }
//		}
//		else
		{
            if(dataManager->awards_get_reward_count(2, index-4) < dataManager->awards_get_complete_count(2, index-4))
            {
                DyItemManager* itemManager = DyItemManager::sharedItemManager();
                
                YtLib::sharedLib()->playEffect(YtLibSndEff_AWARD_SHOWPICTURE);
                
                CCSprite* sprNormal = (CCSprite*)pMenu->getNormalImage();
                sprNormal->runAction(CCSequence::create(CCFadeIn::create(0.5), CCCallFuncN::create(this, callfuncN_selector(DyLayerShopAwards::endAnimationReward)), NULL));
                
                int bomb = 10;
                
                switch(index)
                {
                    case 4:
                        bomb = 10;
                        break;
                    case 5:
                        bomb = 30;
                        break;
                    case 6:
                        bomb = 100;
                        break;
                }
                
                sprintf(m_strReward, "%s %d%s%s",
                        itemManager->getItem(DyItemType_Bomb)->m_name.c_str(),
                        bomb,
                        STRING_count,
                        STRING_rewarded
                        );

                itemManager->addItem(DyItemType_Bomb, bomb);
                sprNormal->setUserData(this);
                
                dataManager->awards_set_reward_count(2, index-4, 1);
                
                dataManager->dataSave();
                
                m_isLockButton = true;
                m_sprIndicator[DyLayerShopAwardsType_Rank][index]->setVisible(false);
            }
            else
            {
                popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
                
                static std::string awards_rank_stamp[3] =
                {
                    STRING_awards_rank_stamp_0,
                    STRING_awards_rank_stamp_1,
                    STRING_awards_rank_stamp_2,
                };
                
                sprintf(lib->m_strTemp2, "%s", awards_rank_stamp[index-4].c_str());

                if(dataManager->awards_get_complete(2, index-4))
                    sprintf(lib->m_strTemp, "%s%s", lib->m_strTemp2, STRING_awards_word_complete);
                else
                    sprintf(lib->m_strTemp, "%s%s", lib->m_strTemp2, STRING_awards_word_complete_notyet);
            }
		}
	}

	if(popup)
	{
		popup->setText(lib->m_strTemp);
		popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 0);
		YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
	}
}


void DyLayerShopAwards::tabDidChange(CCTabView* tabView, int index, CCLayer* contentLayer, bool init, bool redo)
{
    if(redo)
        return;
    
    if(!init)
        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	if(m_labelNotiShop)
		m_labelNotiShop->setVisible(false);
	if(m_labelNotiService)
		m_labelNotiService->setVisible(false);

	switch(index)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		if(m_labelNotiShop)
			m_labelNotiShop->setVisible(true);
		break;
	case 3:
		if(m_labelNotiService)
			m_labelNotiService->setVisible(true);
		break;
	}

	m_tabIndex = index;
}


SEL_MenuHandler DyLayerShopAwards::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;    
}

SEL_CCControlHandler DyLayerShopAwards::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}

bool DyLayerShopAwards::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}

void DyLayerShopAwards::pause()
{
	if(m_tabView)
		m_tabView->setEnable(false);
	for(int i = 0;i < SHOPAWARDS_CNT;i++)
	{
		if(m_menu[i])
			m_menu[i]->setEnabled(false);
        
        if(m_tabScrollView[i])
            m_tabScrollView[i]->setTouchEnabled(false);
	}
}


void DyLayerShopAwards::resume()
{
	if(m_tabView)
  		m_tabView->setEnable(true);
	for(int i = 0;i < SHOPAWARDS_CNT;i++)
	{
		if(m_menu[i])
			m_menu[i]->setEnabled(true);
        
        if(m_tabScrollView[i])
            m_tabScrollView[i]->setTouchEnabled(true);
	}
}


void DyLayerShopAwards::hide()
{
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}

void DyLayerShopAwards::show()
{
    
}


void DyLayerShopAwards::onEnter()
{
	//DyLib* lib = DyLib::sharedLib();
	CCLayer::onEnter();

	if(m_isInit)
    {
         schedule(schedule_selector(DyLayerShopAwards::updateAnimation), .05f);
		return;
    }
	m_isInit = true;

	//YtLib* lib = YtLib::sharedLib();

	m_tabView = new DyLayerShopAwardsTab(CCRectMake(0, 0, 880, 465));
	m_tabView->m_delegate = this;
	m_tabView->m_selectedTab = m_startTab;

	m_scrollViewSize = CCSizeMake(m_tabView->getContentSize().width-m_tabView->m_contentOffsetX, m_tabView->getContentSize().height);

	const char* tab_resource_name[5*3] =
	{
		"awards_ui_tab_recipe.png",
		"awards_ui_tab_recipe_sel.png",
		"awards_ui_tab_recipe_highlight.png",

		"awards_ui_tab_gold.png",
		"awards_ui_tab_gold_sel.png",
		"awards_ui_tab_gold_highlight.png",

		"awards_ui_tab_shop.png",
		"awards_ui_tab_shop_sel.png",
		"awards_ui_tab_shop_highlight.png",

		"awards_ui_tab_service.png",
		"awards_ui_tab_service_sel.png",
		"awards_ui_tab_service_highlight.png",
        
        "awards_ui_tab_social.png",
		"awards_ui_tab_social_sel.png",
		"awards_ui_tab_social_highlight.png",
	};

	for(int i = 0;i < SHOPAWARDS_CNT;i++)
	{
		float height = m_scrollViewSize.height - m_tabView->m_thick - 55;

		CCLayer* layerContent = createAwardsLayer((DyLayerShopAwardsType)i);

		m_tabScrollView[i] = CCScrollView::create(CCSizeMake(m_scrollViewSize.width, height), layerContent);
		//m_tabScrollView[i]->setDelegate(this);

		m_tabScrollView[i]->setContentSize(layerContent->getContentSize());

		m_tabScrollView[i]->setDirection(kCCScrollViewDirectionHorizontal);
		//layerTab->setContentOffset(layerTab->minContainerOffset());
		m_tabScrollView[i]->setContentOffset(ccp(0,0));
		m_tabScrollView[i]->setGlobalScale(YtLib::sharedLib()->m_scale);

		m_tabView->addTab(CCSprite::createWithSpriteFrameName(tab_resource_name[i*3]),
			CCSprite::createWithSpriteFrameName(tab_resource_name[i*3+1]),
			CCSprite::createWithSpriteFrameName(tab_resource_name[i*3+2]), 
			m_tabScrollView[i]);


		if(m_tabScrollView[i])
		{
			m_tabScrollView[i]->setTouchEnabled(true);
			if(m_scrollViewSize.width > m_tabScrollView[i]->getContentSize().width || i == 4)
			{	
				m_tabScrollView[i]->setScrollEnable(false);
			}
			else
				m_tabScrollView[i]->setScrollEnable(true);
		}
        
        m_tabScrollView[i]->setDelegate(this);

	}

	addChild(m_tabView);

	m_labelNotiShop = CCLabelTTF::create(STRING_awards_shop_explain, FONT_NAME_EXP, 20.f);
	m_labelNotiShop->setAnchorPoint(ccp(1, 0));
	m_labelNotiShop->setPosition(ccp(850, 65));
	m_labelNotiShop->setColor(ccc3(129,83,48));
	m_labelNotiShop->setVisible(false);
	addChild(m_labelNotiShop);

	m_labelNotiService = CCLabelTTF::create(STRING_awards_character_explain, FONT_NAME_EXP, 20.f);
	m_labelNotiService->setAnchorPoint(ccp(1, 0));
	m_labelNotiService->setPosition(ccp(850, 65));
	m_labelNotiService->setColor(ccc3(129,83,48));
	m_labelNotiService->setVisible(false);
	addChild(m_labelNotiService);

	m_tabView->release();
	m_tabView->setContentBg(m_contentLayer);
    
    
    schedule(schedule_selector(DyLayerShopAwards::updateAnimation), .05f);
}


void DyLayerShopAwards::onExit()
{
	CCLayer::onExit();
}
