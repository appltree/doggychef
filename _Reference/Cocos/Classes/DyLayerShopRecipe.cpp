/*
DyLayerShopRecipe.cpp
Doggy chef
@initialize at 121207

@history
121207	yoonsr	initialize
*/


#include "DyLayerShopRecipe.h"
#include "DyLib.h"
#include "DyRecipe.h"
#include "DyStage.h"
#include "YtSpriteEx.h"
#include "DyBlockInfo.h"
#include "DyPopup.h"

#define ALLRECIPE_TAG   3000

DyLayerShopRecipeTab::DyLayerShopRecipeTab(CCRect rect):CCTabView(rect)
{
#if SHOP_COUNT_ADDED == 0
    m_leftMargin = 200.0;
#else
    m_leftMargin = 150.0;
#endif
	m_upMargin = 0;
    m_midPad = 5.0;
	m_selectedTab = 0;
	m_contentOffsetX = 0;
    
	m_contentOffsetY = 55-20;

	m_thick = 50;
    
    
#if SHOP_COUNT_ADDED > 1
    m_leftMargin = 50.0;
#endif
    
#if SHOP_COUNT_ADDED > 2
    m_leftMargin = -10.0;
    m_midPad = 0.0;
#endif

}



DyLayerShopRecipe::DyLayerShopRecipe()
{
	m_startTab = 0;
	
    m_isSuspend = false;
    
	for(int i = 0;i < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;i++)
	{
		m_arrRecipe[i] = new CCArray();
		m_arrShown[i] = new CCArray();

		m_arrLayer[i] = new CCArray();

		m_tabScrollView[i] = NULL;
        
        m_menuitemAllBuy[i] = NULL;
        
        m_initShopLayer[i] = false;
        

	}
}

DyLayerShopRecipe::~DyLayerShopRecipe()
{
	m_contentLayer->release();
	for(int i = 0;i < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;i++)
	{
		m_arrRecipe[i]->release();
		m_arrShown[i]->release();

		m_arrLayer[i]->release();
	}
}


void DyLayerShopRecipe::setAllRecipeLayer(CCLayer* layer, int shop)
{
    DyLib* lib = DyLib::sharedLib();
    
  	layer->removeAllChildrenWithCleanup(true);
    
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    
    CCPoint posCenter = CCPointMake(0, 0);
    
    CCSprite* sprBg = CCSprite::createWithSpriteFrameName("menushop_ui_box_00.png");
    CCScale9Sprite *background = CCScale9Sprite::createWithSpriteFrame(sprBg->displayFrame());
    background->setContentSize(CCSizeMake(184, 362));
    background->setPosition(posCenter);
    layer->addChild(background);
    
    CCSprite* sprBox = CCSprite::createWithSpriteFrameName("menushop_ui_recipe_box_enable.png");
    sprBox->setPosition(ccp(posCenter.x-3, posCenter.y+113));
    layer->addChild(sprBox);
    
    CCSprite* sprRecipe = CCSprite::createWithSpriteFrameName(stageManager->m_shopinfo->getValue("recipeset_icon", shop));
    sprRecipe->setPosition(ccp(posCenter.x-3-12, posCenter.y+120+4));
    layer->addChild(sprRecipe);
    
    CCSprite* sprTitle = CCSprite::createWithSpriteFrameName("menushop_ui_recipe_title.png");
    sprTitle->setPosition(ccp(posCenter.x-3, posCenter.y+51));
    layer->addChild(sprTitle);
    
    
    
    const char* shop_name = stageManager->m_shopinfo->getValue("name_kor", shop);
    
    sprintf(lib->m_strTemp, STRING_RECIPE_SET, shop_name);
    CCLabelTTF* labelName = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 20.f);

    labelName->setColor(ccc3(62, 31, 7));
    labelName->setAnchorPoint(ccp(0.5, 0.5));
    labelName->setPosition(ccp(posCenter.x-3, posCenter.y+56));
    layer->addChild(labelName);
    
    
    CCLabelTTF* labelExplain = CCLabelTTF::create(STRING_RECIPE_SET_DESC, FONT_NAME_EXP, 18.f, CCSizeMake(162,120), kCCTextAlignmentLeft);
    labelExplain->setColor(ccc3(100, 57, 38));
    labelExplain->setAnchorPoint(ccp(0.5, 0.5));
    labelExplain->setPosition(ccp(posCenter.x-3, posCenter.y-59-5+30+5));
    layer->addChild(labelExplain);
    
    CCSprite* sprLine = CCSprite::createWithSpriteFrameName("menushop_ui_horizontal_line.png");
    sprLine->setPosition(ccp(posCenter.x-3, posCenter.y-96));
    layer->addChild(sprLine);
    
    
    

    int priceSet = atoi(stageManager->m_shopinfo->getValue("recipeset_delly", shop));
    YtLib::setMoneyStyle(lib->m_strTemp, priceSet);
    CCLabelTTF* labelPurchasePrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 20.f);

    labelPurchasePrice->setColor(ccBLACK);
    layer->addChild(labelPurchasePrice);
    
    CCSprite* sprPurchasePriceMark = CCSprite::createWithSpriteFrameName("menu_ui_delly.png");
    sprPurchasePriceMark->setScale(0.6f);
    layer->addChild(sprPurchasePriceMark);
    
    sprPurchasePriceMark->setPosition(ccp(sprPurchasePriceMark->getPosition().x, posCenter.y-113));
    labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, posCenter.y-113));
    YtLib::arrangeNodesH(ccp(posCenter.x, posCenter.y-113), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);

    
    
    CCSprite* sprSaleMark = CCSprite::createWithSpriteFrameName("menushop_ui_sticker_sale.png");
    layer->addChild(sprSaleMark);
    sprSaleMark->setPosition(ccp(85, 150));
    
    //sprSaleMark->setPosition(ccp(posCenter.x+60, posCenter.y-113+20));
    
    int priceSale = atoi(stageManager->m_shopinfo->getValue("recipeset_sale", shop));
    sprintf(lib->m_strTemp, "-%d%%", priceSale);
    
    CCLabelTTF* labelSale = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 18.f);
    labelSale->setColor(ccWHITE);
    layer->addChild(labelSale);
    labelSale->setPosition(sprSaleMark->getPosition());
    
    
    
    m_menuitemAllBuy[shop] = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("menushop_ui_btn_buy.png"),
                                                             CCSprite::createWithSpriteFrameName("menushop_ui_btn_buy_sel.png"),
                                                             CCSprite::createWithSpriteFrameName("menushop_ui_btn_buy_disable.png"),
                                                             this, menu_selector(DyLayerShopRecipe::onBuyClicked));
    
    m_menuitemAllBuy[shop]->setPosition(ccp(posCenter.x-3, posCenter.y-148));
    m_menuitemAllBuy[shop]->setTag(ALLRECIPE_TAG+shop);
    
    CCMenu *pMenu = CCMenu::create(m_menuitemAllBuy[shop], NULL);
    pMenu->setPosition(CCPointZero);
    pMenu->setTag(ALLRECIPE_TAG+shop);
    layer->addChild(pMenu);
    //menuitemBuy->setVisible(false);
}

void DyLayerShopRecipe::setRecipeLayer(CCLayer* layer, DyRecipeInfo* recipe, bool isShown, int shop, int index, bool replace)
{
	DyLib* lib = DyLib::sharedLib();
	
	DyBlockManager* blockManager = DyBlockManager::sharedBlockManager();
	DyStageManager* stageManager = DyStageManager::sharedStageManager();
	DyRecipeManager* recipeManager = DyRecipeManager::sharedRecipeManager();
    
	layer->removeAllChildrenWithCleanup(true);

	CCPoint posCenter = CCPointMake(0, 0);
	if(isShown)
	{
		CCSprite* sprBg = CCSprite::createWithSpriteFrameName("menushop_ui_box_00.png");
		CCScale9Sprite *background = CCScale9Sprite::createWithSpriteFrame(sprBg->displayFrame());
		background->setContentSize(CCSizeMake(184, 362));
		background->setPosition(posCenter);
		layer->addChild(background);

		CCSprite* sprBox = CCSprite::createWithSpriteFrameName("menushop_ui_recipe_box_enable.png");
		sprBox->setPosition(ccp(posCenter.x-3, posCenter.y+113));
		layer->addChild(sprBox);
			
		if(recipe->m_hasDish)
		{
			CCSprite* sprDish = CCSprite::createWithSpriteFrameName("game_ui_dish.png");
			sprDish->setPosition(ccp(posCenter.x-3, posCenter.y+106));
			layer->addChild(sprDish);
            
            
		}

		CCSprite* sprRecipe = CCSprite::createWithSpriteFrameName(recipe->m_urlResultImage.c_str());
		sprRecipe->setPosition(ccp(posCenter.x-3, posCenter.y+111));
		sprRecipe->setAnchorPoint(ccp(recipe->m_anchorPoint.x, recipe->m_anchorPoint.y));
		sprRecipe->setScale(0.8f*recipe->m_scale);
		layer->addChild(sprRecipe);

		CCSprite* sprTitle = CCSprite::createWithSpriteFrameName("menushop_ui_recipe_title.png");
		sprTitle->setPosition(ccp(posCenter.x-3, posCenter.y+51));
		layer->addChild(sprTitle);

		CCSprite* sprSellingPriceMark = CCSprite::createWithSpriteFrameName("menu_ui_gold.png");
		sprSellingPriceMark->setScale(0.5f);
		sprSellingPriceMark->setPosition(ccp(posCenter.x-60, posCenter.y+153));
		layer->addChild(sprSellingPriceMark);

		sprintf(lib->m_strTemp, "%d", recipe->m_selling_price);
		CCLabelTTF* labelSellingPrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 15.f);
		labelSellingPrice->setColor(ccc3(100, 57, 38));
		labelSellingPrice->setAnchorPoint(ccp(0, 0.5));
		labelSellingPrice->setPosition(ccp(posCenter.x-45, posCenter.y+153));
		layer->addChild(labelSellingPrice);

		CCLabelTTF* labelName = CCLabelTTF::create(recipe->m_name.c_str(), FONT_NAME_EXP, 20.f);
		labelName->setColor(ccc3(62, 31, 7));
		labelName->setAnchorPoint(ccp(0.5, 0.5));
		labelName->setPosition(ccp(posCenter.x-3, posCenter.y+56));
		layer->addChild(labelName);

        
        bool isSam = recipe->m_tag.compare("sam_kimchi") == 0 || recipe->m_name.compare("sam_vegetable") == 0;
        
        
		for(unsigned int j = 0;j < recipe->m_arrMaterial->count();j++)
		{	
			DyMaterial* material = (DyMaterial*)recipe->m_arrMaterial->objectAtIndex(j);

			CCSprite* bg = CCSprite::createWithSpriteFrameName("game_ui_block_socket.png");

			CCPoint position;

			float scaleBox = .8f;
			float scaleMaterial = 0.5f;
			float scaleNumber = .75f;
			float sizeNumberFont = 20;
			float offsetNumber = 7;

			switch(recipe->m_arrMaterial->count())
			{
				case 1:
					position = ccp(posCenter.x-3, posCenter.y);
					break;
				case 2:
					position = ccp(posCenter.x-26 + j*46, posCenter.y);
					break;
				case 3:
					position = ccp(posCenter.x-49 + j*46, posCenter.y);
					break;
				case 4:
					position = ccp(posCenter.x-68 + j*42, posCenter.y);

					scaleBox = .75f;
					scaleMaterial = 0.4f;
					scaleNumber = .65f;
					sizeNumberFont = 18;
					offsetNumber = 7;
					break;

				case 5:
					position = ccp(posCenter.x-70 + j*33, posCenter.y);

					scaleBox = .6f;
					scaleMaterial = 0.35f;
					scaleNumber = .6f;
					sizeNumberFont = 15;
					offsetNumber = 11;
					break;

				case 6:
					if(j < 3)
						position = ccp(posCenter.x-45 + j*40, posCenter.y+20);
					else
						position = ccp(posCenter.x-45 + (j-3)*40, posCenter.y-15);

					scaleBox = .6f;
					scaleMaterial = 0.35f;
					scaleNumber = .6f;
					sizeNumberFont = 15;
					offsetNumber = 11;
					break;
				case 7:
					if(j < 3)
						position = ccp(posCenter.x-45 + j*40, posCenter.y+20);
					else
						position = ccp(posCenter.x-55 + (j-3)*35, posCenter.y-15);

					scaleBox = .6f;
					scaleMaterial = 0.35f;
					scaleNumber = .6f;
					sizeNumberFont = 15;
					offsetNumber = 11;
					break;
				case 8:
					if(j < 4)
						position = ccp(posCenter.x-55 + j*35, posCenter.y+20);
					else
						position = ccp(posCenter.x-55 + (j-4)*35, posCenter.y-15);

					scaleBox = .6f;
					scaleMaterial = 0.35f;
					scaleNumber = .6f;
					sizeNumberFont = 15;
					offsetNumber = 11;
					break;
				default:
					position = ccp(posCenter.x-3, posCenter.y+10);
					break;
			}
            
            if(isSam)
                position.x -= 50;
            
			bg->setPosition(position);
			bg->setScale(scaleBox);

			layer->addChild(bg);

			//material->m_id = blockManager->getIdByTag(material->m_tag.c_str());

			CCSprite* sprMaterial = CCSprite::createWithSpriteFrameName(blockManager->getImageNameByTag(material->m_tag.c_str()));
			sprMaterial->setScale(scaleMaterial);
			sprMaterial->setPosition(position);
			layer->addChild(sprMaterial);

			CCSprite* sprNumberBg = CCSprite::createWithSpriteFrameName("game_ui_block_count_bg.png");
			sprNumberBg->setScale(scaleNumber);
			sprNumberBg->setPosition(ccp(position.x + bg->getContentSize().width/2-offsetNumber,
				position.y + bg->getContentSize().height/2-offsetNumber));
			layer->addChild(sprNumberBg);

			sprintf(lib->m_strTemp, "%d", material->m_count);
				
			CCLabelTTF* labelCount = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, sizeNumberFont);
			labelCount->setColor(ccYELLOW);
			labelCount->setPosition(sprNumberBg->getPosition());

			layer->addChild(labelCount);
		}
        
        if(isSam)
        {
            CCLabelTTF* labelExplainMaterial = CCLabelTTF::create("+ 각종 재료", FONT_NAME_EXP, 20.f);
            labelExplainMaterial->setColor(ccc3(100, 57, 38));
            labelExplainMaterial->setAnchorPoint(ccp(0.5, 0.5));
            labelExplainMaterial->setPosition(ccp(posCenter.x+20, posCenter.y));
            layer->addChild(labelExplainMaterial);
        }

		CCLabelTTF* labelExplain = CCLabelTTF::create(recipe->m_explain.c_str(), FONT_NAME_EXP, 17.f, CCSizeMake(162,73), kCCTextAlignmentLeft);
		labelExplain->setColor(ccc3(100, 57, 38));
		labelExplain->setAnchorPoint(ccp(0.5, 0.5));
		labelExplain->setPosition(ccp(posCenter.x-3, posCenter.y-59-5));
		layer->addChild(labelExplain);

		CCSprite* sprLine = CCSprite::createWithSpriteFrameName("menushop_ui_horizontal_line.png");
		sprLine->setPosition(ccp(posCenter.x-3, posCenter.y-96));
		layer->addChild(sprLine);

		if(m_shopType == DyLayerShopType_Normal && recipe->m_purchase_gold > 0 )
		{   
			YtLib::setMoneyStyle(lib->m_strTemp, recipe->m_purchase_gold);
			CCLabelTTF* labelPurchasePrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 20.f);
			labelPurchasePrice->setColor(ccBLACK);
			layer->addChild(labelPurchasePrice);

			CCSprite* sprPurchasePriceMark = CCSprite::createWithSpriteFrameName("menu_ui_gold.png");
			sprPurchasePriceMark->setScale(0.6f);
			layer->addChild(sprPurchasePriceMark);

			sprPurchasePriceMark->setPosition(ccp(sprPurchasePriceMark->getPosition().x, posCenter.y-113));
			labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, posCenter.y-113));
			YtLib::arrangeNodesH(ccp(posCenter.x, posCenter.y-113), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
		}
		if(m_shopType == DyLayerShopType_Normal && recipe->m_purchase_delly > 0 )
		{
			YtLib::setMoneyStyle(lib->m_strTemp, recipe->m_purchase_delly);
			CCLabelTTF* labelPurchasePrice = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 20.f);
			labelPurchasePrice->setColor(ccBLACK);
			layer->addChild(labelPurchasePrice);
            
			CCSprite* sprPurchasePriceMark = CCSprite::createWithSpriteFrameName("menu_ui_delly.png");
			sprPurchasePriceMark->setScale(0.6f);
			layer->addChild(sprPurchasePriceMark);
            
			sprPurchasePriceMark->setPosition(ccp(sprPurchasePriceMark->getPosition().x, posCenter.y-113));
			labelPurchasePrice->setPosition(ccp(labelPurchasePrice->getPosition().x, posCenter.y-113));
			YtLib::arrangeNodesH(ccp(posCenter.x, posCenter.y-113), ccp(0.5, 0.5), 2, sprPurchasePriceMark, labelPurchasePrice, NULL);
		}


		CCSprite* sprCheck = CCSprite::createWithSpriteFrameName("menushop_ui_select.png");
		sprCheck->setPosition(ccp(posCenter.x-3, posCenter.y-148));
		layer->addChild(sprCheck);
		sprCheck->setVisible(false);

		CCMenuItemSprite *menuitemBuy = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("menushop_ui_btn_buy.png"),
			CCSprite::createWithSpriteFrameName("menushop_ui_btn_buy_sel.png"),
			CCSprite::createWithSpriteFrameName("menushop_ui_btn_buy.png"),
			this, menu_selector(DyLayerShopRecipe::onBuyClicked));

		menuitemBuy->setPosition(ccp(posCenter.x-3, posCenter.y-148));
		menuitemBuy->setTag(index);

		CCMenu *pMenu = CCMenu::create(menuitemBuy, NULL);
		pMenu->setPosition(CCPointZero);
		pMenu->setTag(index);
		layer->addChild(pMenu);
		menuitemBuy->setVisible(false);

		if(recipe->m_isAvailable)
		{
			if(m_shopType == DyLayerShopType_Normal)
				sprCheck->setVisible(true);
			else
			{
				CCLabelTTF* labelSellCount = CCLabelTTF::create(STRING_sell_count, FONT_NAME_EXP, 18.f);
				labelSellCount->setColor(ccc3(100, 57, 38));
				labelSellCount->setAnchorPoint(ccp(0, 0.5));
				labelSellCount->setPosition(ccp(posCenter.x-80, posCenter.y-118));
				layer->addChild(labelSellCount);
		
				YtLib::setMoneyStyle(lib->m_strTemp, recipeManager->getSellingCount(recipe));
			
				CCLabelTTF* labelSellCountDigit = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 18.f);
				labelSellCountDigit->setColor(ccc3(100, 57, 38));
				labelSellCountDigit->setAnchorPoint(ccp(1.0, 0.5));
				labelSellCountDigit->setPosition(ccp(posCenter.x, posCenter.y-118-30));
				layer->addChild(labelSellCountDigit);
			}
		}
		else
		{
			menuitemBuy->setVisible(true);
		}

		if(!recipe->m_isSawInShop)
		{
			CCSprite* sprNew = CCSprite::createWithSpriteFrameName("menu_ui_new_mark.png");
			sprNew->setPosition(ccp(85, 150));
			layer->addChild(sprNew);

            DyDataManager::sharedDataManager()->recipe_setSawInShop(recipe->m_id, true);
			recipe->m_isSawInShop = true;
			DyDataManager::sharedDataManager()->dataSave();
		}
	}
	else
	{
		CCSprite* sprBg = CCSprite::createWithSpriteFrameName("menushop_ui_box_00.png");
		CCScale9Sprite *background = CCScale9Sprite::createWithSpriteFrame(sprBg->displayFrame());
		background->setContentSize(CCSizeMake(184, 362));
		background->setPosition(posCenter);
		layer->addChild(background);

		CCSprite* sprBox = CCSprite::createWithSpriteFrameName("menushop_ui_recipe_box_disable.png");
		sprBox->setPosition(ccp(posCenter.x-3, posCenter.y+113));
		layer->addChild(sprBox);

		CCSprite* sprRecipe = CCSprite::createWithSpriteFrameName("menushop_ui_lock_recipe.png");
		sprRecipe->setPosition(ccp(posCenter.x-3, posCenter.y+120));
		layer->addChild(sprRecipe);

		CCSprite* sprTitle = CCSprite::createWithSpriteFrameName("menushop_ui_recipe_title.png");
		sprTitle->setPosition(ccp(posCenter.x-3, posCenter.y+51));
		layer->addChild(sprTitle);

		CCLabelTTF* labelName = CCLabelTTF::create("?", FONT_NAME_EXP, 20.f);
		labelName->setColor(ccc3(62, 31, 7));
		labelName->setAnchorPoint(ccp(0.5, 0.5));
		labelName->setPosition(ccp(posCenter.x-3, posCenter.y+56));
		layer->addChild(labelName);

		if(DyStageManager::sharedStageManager()->isShopOpen(recipe->m_idShop) == false)
		{
			CCSprite* sprLock = CCSprite::createWithSpriteFrameName("menushop_ui_lock.png");
			sprLock->setPosition(ccp(posCenter.x-3, posCenter.y-34));
			layer->addChild(sprLock);

			CCSprite* sprLockIcon = CCSprite::createWithSpriteFrameName(stageManager->m_shopinfo->getValue("lock_icon", shop));
			sprLockIcon->setPosition(ccp(posCenter.x-3, posCenter.y-34-20));
			layer->addChild(sprLockIcon);


			sprintf(lib->m_strTemp, STRING_shop_recipe,
				stageManager->m_shopinfo->getValue("name_kor", shop));

			CCLabelTTF* labelNoti = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 15.f, CCSizeMake(150,100), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);

			labelNoti->setPosition(ccp(posCenter.x-3, posCenter.y-134));
			labelNoti->setColor(ccc3(129, 83, 48));

			layer->addChild(labelNoti);

		}
		else if(recipe->m_attr == DyRecipeAttr_Additional)
		{
			CCSprite* sprLock = CCSprite::createWithSpriteFrameName("menushop_ui_lock.png");
			sprLock->setPosition(ccp(posCenter.x-3, posCenter.y-34));
			layer->addChild(sprLock);

			CCLabelTTF* labelStage = CCLabelTTF::create("STAGE", FONT_NAME_DEFAULT, 24.f);
			labelStage->setColor(ccc3(129, 83, 48));
			labelStage->setPosition(ccp(posCenter.x-3, posCenter.y-44));
			layer->addChild(labelStage);

			sprintf(lib->m_strTemp, "%02d", recipe->m_showStage+1);
			CCLabelTTF* labelStageNumber = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_DEFAULT, 24.f);
			labelStageNumber->setColor(ccc3(129, 83, 48));
			labelStageNumber->setPosition(ccp(posCenter.x-3, posCenter.y-44-30));
			layer->addChild(labelStageNumber);
		}
		else
		{
			CCSprite* sprLock = CCSprite::createWithSpriteFrameName("menushop_ui_lock.png");
			sprLock->setPosition(ccp(posCenter.x-3, posCenter.y+10));
			sprLock->setScale(0.5f);
			layer->addChild(sprLock);

			sprintf(lib->m_strTemp, STRING_secret_recipe,
				stageManager->m_shopinfo->getValue("name_kor", shop));

			CCLabelTTF* labelNoti = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 18.f, CCSizeMake(150,120), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);

			labelNoti->setPosition(ccp(posCenter.x-3, posCenter.y-44-40));
			labelNoti->setColor(ccc3(129, 83, 48));

			layer->addChild(labelNoti);
		}
	}
}


void DyLayerShopRecipe::reloadRecipeResource(int id_shop)
{
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("game_ui.plist");
	DyRecipeManager::sharedRecipeManager()->loadRecipeResource(id_shop);
}


int DyLayerShopRecipe::checkRecipe(int id_shop, bool first)
{
	DyRecipeManager* recipeManager = DyRecipeManager::sharedRecipeManager();
	DyStageManager* stageManager = DyStageManager::sharedStageManager();

	CCArray* allkeys = recipeManager->m_dicRecipes->allKeys();
	int cnt_recipe = 0;

	//
	// search new recipe	
	bool isAllReciepeHaveForSecret = true;

	for(unsigned int i = 0;i < allkeys->count();i++)
	{
		DyRecipeInfo* recipe = (DyRecipeInfo*)recipeManager->m_dicRecipes->objectForKey(((CCInteger*)allkeys->objectAtIndex(i))->getValue());
		bool isShown = true;

		if(recipe->m_idShop == id_shop)
		{
			if(recipe->m_attr == DyRecipeAttr_Additional)
			{
				if(recipe->m_isAvailable == false && recipe->m_showStage > stageManager->getCurrentLevel(id_shop))
				{
					isShown = false;
				}
				if(recipe->m_isAvailable == false)
					isAllReciepeHaveForSecret = false;
			}
			else if(recipe->m_attr == DyRecipeAttr_Secret)
			{
				if(isAllReciepeHaveForSecret == false)
				{
					isShown = false;
				}
			}

			bool isAvailable = false;

			if(m_shopType == DyLayerShopType_Myroom)
			{
				if(recipe->m_isAvailable == true)
					isAvailable = true;
			}
			else
				isAvailable = true;

			if(stageManager->isShopOpen(recipe->m_idShop) == false)
			{
				if(m_shopType == DyLayerShopType_Myroom)
					isAvailable = false;

				isShown = false;
			}

			if(isAvailable)
			{
				if(first)
				{
					m_arrRecipe[id_shop]->addObject(recipe);
					m_arrShown[id_shop]->addObject(CCInteger::create(isShown));
				}
				else
				{
					m_arrShown[id_shop]->replaceObjectAtIndex(cnt_recipe, CCInteger::create(isShown));
				}
				cnt_recipe++;
			}
		}
	}

	return cnt_recipe;
}


void DyLayerShopRecipe::setRecipeLayer(int id_shop, CCLayerColor* layerContent)
{
    if(m_initShopLayer[id_shop])
        return;
    
    m_initShopLayer[id_shop] = true;

	reloadRecipeResource(id_shop);

	for(unsigned int i = 0;i < m_arrRecipe[id_shop]->count()+m_cnt_allrecipe[id_shop];i++)
	{
        CCLayer* layer = CCLayer::create();
        
        if(i >= m_cnt_allrecipe[id_shop])
        {
            DyRecipeInfo* recipe = (DyRecipeInfo*)m_arrRecipe[id_shop]->objectAtIndex(i-m_cnt_allrecipe[id_shop]);
            setRecipeLayer(layer, recipe, ((CCInteger*)m_arrShown[id_shop]->objectAtIndex(i-m_cnt_allrecipe[id_shop]))->getValue() == 1, id_shop, i-m_cnt_allrecipe[id_shop], false);
            m_arrLayer[id_shop]->addObject(layer);
        }
        else
        {
            setAllRecipeLayer(layer, id_shop);
        }
        
		layerContent->addChild(layer);
		layer->setPosition(ccp(140+i*230, layerContent->getContentSize().height/2-2));
        
	}
}


CCLayerColor* DyLayerShopRecipe::createRecipeLayer(int id_shop, int layerHight)
{
	DyRecipeManager* recipeManager = DyRecipeManager::sharedRecipeManager();
	
    int cnt_recipe = checkRecipe(id_shop, true);
    
    m_cnt_allrecipe[id_shop] = 0;
    if(m_shopType == DyLayerShopType_Normal)
        m_cnt_allrecipe[id_shop] = 1;
    if(DyStageManager::sharedStageManager()->isShopOpen(id_shop) == false || recipeManager->isAllbuy(id_shop))
        m_cnt_allrecipe[id_shop] = 0;
    
    CCLayerColor* layerContent = CCLayerColor::create(ccc4(255, 50, 50, 0), (cnt_recipe+m_cnt_allrecipe[id_shop])*230 + (140*2-230), layerHight);
	
  
	return layerContent;
}


void DyLayerShopRecipe::onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
	m_contentLayer = (CCLayer* )getChildByTag(SHOPRECIPE_UI_TAG_BG_BOTTOM);
	m_contentLayer->removeFromParentAndCleanup(false);
	m_contentLayer->retain();
}


void DyLayerShopRecipe::scrollViewDidScroll(CCScrollView* view)
{
}


void DyLayerShopRecipe::scrollViewDidZoom(CCScrollView* view)
{
}



void DyLayerShopRecipe::onPopupClicked(cocos2d::CCObject * pSender)
{
    DyStageManager* stageManager = DyStageManager::sharedStageManager();
    DyMoneyManager* moneyManager = DyMoneyManager::sharedMoneyManager();
    DyRecipeManager* recipeManager = DyRecipeManager::sharedRecipeManager();
    
	DyPopup *pPopup = dynamic_cast<DyPopup*>(pSender);
    
    
	if(pPopup->m_active_tag%10 == 1)
	{
        YtLayerManager::sharedLayerManager()->destoryTopLayer(false);
        
        int shop = (pPopup->m_active_tag-10)/10;
        int delly = atoi(stageManager->m_shopinfo->getValue("recipeset_delly", shop));
                         
        if(delly > moneyManager->getDelly())
            moneyManager->alramNotEnoughDelly(true, false);
        else
        {
            recipeManager->buyedAllRecipe(shop);
            moneyManager->addDelly(-delly, true);
            moneyManager->dataSave();
            
            DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShopRecipe::onPopupClicked));
            popup->setText(STRING_RECIPE_SET_BUY_SUCCESS);
            popup->addButton(DyButtonType_Red, STRING_POPUP_OK, 10000);
            
            YtLayerManager::sharedLayerManager()->pushLayer(popup, true, false);
        }
        return;
        
	}
    //else if(pPopup->m_active_tag == 10000)
    {
        YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
        
    }
}


// tab clicked events
void DyLayerShopRecipe::onBuyClicked(CCObject * obj)
{
    if(m_isSuspend)
        return;
    
    YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

    CCMenuItemSprite * pMenu = dynamic_cast<CCMenuItemSprite*>(obj);
	
    if(pMenu->getTag() >= ALLRECIPE_TAG)
    {
        DyStageManager* stageManager = DyStageManager::sharedStageManager();
        
        int shop = pMenu->getTag() - ALLRECIPE_TAG;
        
//        if(DyRecipeManager::sharedRecipeManager()->isAllbuy(shop))
//        {
//            DyPopup* popup = DyPopup::create(DyPopupType_Text, NULL, NULL);
//            popup->setText(STRING_RECIPE_SET_ALREADY_BUYED);
//            popup->addButton(DyButtonType_Blue, STRING_POPUP_OK, 0);
//            YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
//        }
//        else
        {
            DyLib* lib = DyLib::sharedLib();
            
            sprintf(lib->m_strTemp, STRING_CONFIRM_RECIPESET_BUY, stageManager->m_shopinfo->getValue("name_kor", shop), atoi(stageManager->m_shopinfo->getValue("recipeset_delly", shop)));
            
            DyPopup* popup = DyPopup::create(DyPopupType_Text, this, callfuncO_selector(DyLayerShopRecipe::onPopupClicked));
            popup->setText(lib->m_strTemp);
            
            popup->addButton(DyButtonType_Blue, STRING_POPUP_NO, 10 + shop*10);
            popup->addButton(DyButtonType_Red, STRING_POPUP_YES, 11 + shop*10);
            popup->addButton(DyButtonType_Close, "", 10 + shop*10);
            
            YtLayerManager::sharedLayerManager()->pushLayer(popup, true, true);
        }
    }
    else
    {
        DyRecipeInfo* recipe = (DyRecipeInfo*)m_arrRecipe[m_tabIndex]->objectAtIndex(pMenu->getTag());
        DyMoneyManager::sharedMoneyManager()->questBuy(DyBuyItemType_Recipe, recipe, recipe->m_purchase_gold > 0 ? DyBuyMethod_Gold : DyBuyMethod_Delly);
    }
}


void DyLayerShopRecipe::tabDidChange(CCTabView* tabView, int index, CCLayer* contentLayer, bool init, bool redo)
{
    if(redo)
        return;
    
    if(!init)
        YtLib::sharedLib()->playEffect(YtLibSndEff_BUTTON_CLICK);

	CCScrollView* view = (CCScrollView*)contentLayer;
	m_tabIndex = index;

    setRecipeLayer(m_tabIndex, m_layerContent[m_tabIndex]);
	scrollViewDidScroll(view);
}


SEL_MenuHandler DyLayerShopRecipe::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName) 
{
    return NULL;    
}

SEL_CCControlHandler DyLayerShopRecipe::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName) 
{
	return NULL;
}

bool DyLayerShopRecipe::onAssignCCBMemberVariable(CCObject * pTarget, CCString * pMemberVariableName, CCNode * pNode) 
{
    return false;
}


void DyLayerShopRecipe::refresh(bool enable)
{
	for(int id_shop = 0;id_shop < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;id_shop++)
	{
		if(m_tabScrollView[id_shop])
			m_tabScrollView[id_shop]->setTouchEnabled(enable);
    
        if(id_shop == m_tabIndex)
        {
            reloadRecipeResource(id_shop);
            
            if(DyRecipeManager::sharedRecipeManager()->isAllbuy(id_shop))
            {
                if(m_menuitemAllBuy[id_shop])
                    m_menuitemAllBuy[id_shop]->setEnabled(false);
            }
            
            if(m_arrRecipe[id_shop]->count() > 0)
                checkRecipe(id_shop, false);
            
            for(unsigned int i = 0;i < m_arrRecipe[id_shop]->count();i++)
            {
                DyRecipeInfo* recipe = (DyRecipeInfo*)m_arrRecipe[id_shop]->objectAtIndex(i);

                if(((CCInteger*)m_arrShown[id_shop]->objectAtIndex(i))->getValue())
                {
                    CCLayer* layer = (CCLayer*)m_arrLayer[id_shop]->objectAtIndex(i);

                    setRecipeLayer(layer, recipe, ((CCInteger*)m_arrShown[id_shop]->objectAtIndex(i))->getValue() == 1, id_shop, i, true);
                }
            }
        }
	}
}


void DyLayerShopRecipe::pause()
{
    m_isSuspend = true;
	refresh(false);
}


void DyLayerShopRecipe::resume()
{
    m_isSuspend = false;
	refresh(true);
}


void DyLayerShopRecipe::hide()
{
    YtLayerManager::sharedLayerManager()->destoryTopLayer(true);
}

void DyLayerShopRecipe::show()
{
    
}


void DyLayerShopRecipe::onEnter()
{
	CCLayer::onEnter();

	if(m_isInit)
		return;
	m_isInit = true;
	
	m_scrollViewSize = CCSizeMake(880, 465);

	DyLayerShopRecipeTab* tabView = new DyLayerShopRecipeTab(CCRectMake(0, 0, m_scrollViewSize.width, m_scrollViewSize.height));
	tabView->m_delegate = this;
	tabView->m_selectedTab = m_startTab;

	const char* tab_resource_name[(SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED)*3] =
	{
		"menushop_ui_recipe_tab_snack.png",
		"menushop_ui_recipe_tab_snack_sel.png",
		"menushop_ui_recipe_tab_snack_highlight.png",

		"menushop_ui_recipe_tab_hamburger.png",
		"menushop_ui_recipe_tab_hamburger_sel.png",
		"menushop_ui_recipe_tab_hamburger_highlight.png",

		"menushop_ui_recipe_tab_coffee.png",
		"menushop_ui_recipe_tab_coffee_sel.png",
		"menushop_ui_recipe_tab_coffee_highlight.png",
#if SHOP_COUNT_ADDED > 0
        "menushop_ui_recipe_tab_china.png",
		"menushop_ui_recipe_tab_china_sel.png",
		"menushop_ui_recipe_tab_china_highlight.png",
#endif
        
#if SHOP_COUNT_ADDED > 1
        "menushop_ui_recipe_tab_pizza.png",
		"menushop_ui_recipe_tab_pizza_sel.png",
		"menushop_ui_recipe_tab_pizza_highlight.png",
#endif

#if SHOP_COUNT_ADDED > 2
        "menushop_ui_recipe_tab_korean.png",
		"menushop_ui_recipe_tab_korean_sel.png",
		"menushop_ui_recipe_tab_korean_highlight.png",
#endif

	};

	for(int i = 0;i < SHOP_COUNT_DEFAULT+SHOP_COUNT_ADDED;i++)
	{
		float height = m_scrollViewSize.height - tabView->m_thick - 55;

		m_layerContent[i] = createRecipeLayer(i, height);
        
        if(i == m_tabIndex)
            setRecipeLayer(i, m_layerContent[i]);
		
		m_tabScrollView[i] = CCScrollView::create(CCSizeMake(m_scrollViewSize.width, height), m_layerContent[i]);
		m_tabScrollView[i]->setDelegate(this);

		m_tabScrollView[i]->setContentSize(m_layerContent[i]->getContentSize());

		m_tabScrollView[i]->setDirection(kCCScrollViewDirectionHorizontal);
		//layerTab->setContentOffset(layerTab->minContainerOffset());
		m_tabScrollView[i]->setContentOffset(ccp(0,0));
		m_tabScrollView[i]->setGlobalScale(YtLib::sharedLib()->m_scale);

		tabView->addTab(CCSprite::createWithSpriteFrameName(tab_resource_name[i*3]),
			CCSprite::createWithSpriteFrameName(tab_resource_name[i*3+1]),
			CCSprite::createWithSpriteFrameName(tab_resource_name[i*3+2]), 
			m_tabScrollView[i]);

		if(m_tabScrollView[i])
		{
			m_tabScrollView[i]->setTouchEnabled(true);
			if(m_scrollViewSize.width > m_tabScrollView[i]->getContentSize().width)
			{	
				m_tabScrollView[i]->setScrollEnable(false);
			}
			else
				m_tabScrollView[i]->setScrollEnable(true);
		}

		if(m_arrRecipe[i]->count() == 0)
		{
            DyLib* lib = DyLib::sharedLib();
            DyStageManager* stageManager = DyStageManager::sharedStageManager();
			sprintf(lib->m_strTemp, STRING_not_have_recipe, stageManager->m_shopinfo->getValue("name_kor", i));
			CCLabelTTF* labelNotiShop = CCLabelTTF::create(lib->m_strTemp, FONT_NAME_EXP, 18.f, CCSizeMake(400,100), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter);
			labelNotiShop->setPosition(ccp(260, 140));
			labelNotiShop->setColor(ccc3(100, 57, 38));
			m_tabScrollView[i]->addChild(labelNotiShop);
		}
	}
    
    
    
    

	addChild(tabView);
	tabView->release();
	tabView->setContentBg(m_contentLayer);
}


void DyLayerShopRecipe::onExit()
{
	CCLayer::onExit();
}


