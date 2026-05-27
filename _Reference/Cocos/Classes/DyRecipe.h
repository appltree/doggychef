/*
DyRecipe.h
Doggy chef
@initialize at 121008

@history
121008	yoonsr	initialize
*/


#ifndef __DYRECIPE_H__
#define __DYRECIPE_H__

#include "DyConfig.h"

#include "YtObject.h"
#include "DyCharacter.h"

#include "YtLoader.h"

#include "YtCommand.h"

#define DYRECIPE_MATERIAL_MAX_CNT		9

typedef enum tag_DyRecipeAttr
{
	DyRecipeAttr_Default,
	DyRecipeAttr_Additional,
	DyRecipeAttr_Secret,
}DyRecipeAttr;

class DyMaterial : public CCObject
{
public:
	DyMaterial();
	~DyMaterial();
	std::string m_tag;
	//int m_id;

	int m_count;
	int m_unAvailableCount;
    
	CCLabelTTF* m_labelCount;
};


class DyRecipeInfo : public CCObject
{
public:
	DyRecipeInfo();
	~DyRecipeInfo();
	CCArray* m_arrMaterial;
	int m_id;
	int m_idShop;
	int m_selling_price;
	int m_purchase_gold;
    int m_purchase_delly;
    
	std::string m_tag;
	std::string m_name;
	std::string m_explain;
	std::string m_urlResultImage;	
	std::string m_mainMaterial;

	bool m_hasDish;
	float m_scale;

	DyRecipeAttr m_attr;
	int m_showStage;

	bool m_isAvailable;
	bool m_isSawInShop;
	bool m_isSawInGame;
	CCPoint m_anchorPoint;
    
    int m_crazy_mux;
    int m_heart_plus;
    
    bool m_isCheckedInGame;
};


class DyOrderRecipe : public CCObject
{
public:
	DyOrderRecipe(DyRecipeInfo* info);
	~DyOrderRecipe();

	DyRecipeInfo* m_info;
	CCArray* m_arrMateiralStatus;
};


class DyRecipeManager : public CCObject
{
public:

    YtPlatform_CallFunc m_callFunc;
    
	CCDictionary* m_dicRecipes;
	CCArray* m_arrAvailableRecipes;
	CCArray* m_arrWantedRecipe;

    std::string m_process_recipe_tag;
    
    int m_removeCakeType;
    
	DyRecipeManager();
	~DyRecipeManager();

    void loadRecipe();
    
	DyRecipeInfo* getRecipe(std::string tag);

	void reorderingAvailable(int shopID);
	void alramNewRecipe(std::string tag, bool alreadHas);
	void onPopupClicked(cocos2d::CCObject * pSender);
	void buyedRecipe(DyRecipeInfo* recipe);
	void loadRecipeResource(int id_shop);

    void buyedAllRecipe(int shop);
    
	bool checkNewRecipe(YtPlatform_CallFunc callFunc);

	void addSellingCount(DyRecipeInfo* recipe);
    
	int getSellingCount(DyRecipeInfo* recipe);
    
    void removeRecipe(CCArray* array, std::string tag);
    
	bool isAllbuy(int shopID);
    
    int getRecipeTotalCnt(int shopID);
    int getRecipeHasCnt(int shopID);
    int getDefaultCnt(int shopID);
    void removeCakeOrMuffine(CCArray* array, bool isCake);
    
    bool hasRecipe(CCArray* array, std::string tag);
    
    
    bool isAvailableRecipe(std::string tag);
    
    void clearGameCheck();
    
    
	static DyRecipeManager* sharedRecipeManager();
};



#endif  // __DYRECIPE_H__