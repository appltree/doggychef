/*
DyItem.h
Doggy chef
@initialize at 121018

@history
121018	yoonsr	initialize
*/

#ifndef __DYITEM_H__
#define __DYITEM_H__

#include "YtLib.h"
#include "DyConfig.h"

#define DYITEM_SLOT_CNT		3
#define DYITEM_MAX_CNT      100

typedef enum tag_DyItemType
{
	DyItemType_Hint,
	DyItemType_Refresh,
	DyItemType_TimeIncrease,
	DyItemType_Candy,
	DyItemType_Bomb,
	DyItemType_Count,
    
}DyItemType;

//
//typedef enum tag_DyItemAttribute
//{
//	DyItemAttribute_Day,
//	DyItemAttribute_Stage,
//}DyItemAttribute;


//typedef struct tag_DyItemData
//{
//    bool m_itemView;
//	int m_itemcount;
//}DyItemData;

typedef unsigned char DyItemData;

//#define DyItemData_set_itemview(tgt, arg)   (tgt) = ((tgt)&0b01111111 | (((arg)&0x1)<<7))
//#define DyItemData_get_itemview(tgt)        (((tgt)>>7)&0x1)

#define DyItemData_set_itemcount(tgt, arg)   (tgt) = ((tgt)&0b10000000 | (((arg)&0b01111111)))
#define DyItemData_get_itemcount(tgt)        ((tgt)&0b01111111)

class DyItem : public CCObject
{
public:
	int m_id;
	std::string m_tag;
	std::string m_name;
	//DyItemAttribute m_attribute;
	int m_default_count;
	int m_price_delly;
	int m_price_gold;
	std::string m_urlIconTag;
    float m_cooltime;
    
	int m_cntHas;
    int m_cntAvailable;

	int m_stage;
	//bool m_isView;

	std::string m_explain_kor;

	DyItemType m_itemtype;
	DyItem();
	~DyItem();

	CCLabelTTF* m_lblAvailable;
	void updateStatus();
};


class DyItemManager : public CCObject
{
public:
	
	DyItemManager();
	~DyItemManager();

	DyItem* m_itemslot[DYITEM_SLOT_CNT];

	CCArray* m_arrItems;

	CCSprite* createItemIcon(DyItem* item, bool enable, bool isSlot);

	bool isAvailable(DyItem* item);

	int getSlotIndex(DyItemType type);
	DyItem* getItem(DyItemType type);
    DyItem* getItemByTag(std::string tag);

    //
    // 아이템 사고 줄때 갯수 변화
	int addItem(DyItemType type, int count);
    
    //
    // 게임중에 아이템 사용
    int useItem(DyItemType type, int count);
	//int getItemCount(DyItemType type);

	void reset();
	static DyItemManager* sharedItemManager();

	void dataSave();
};


#endif  // __DYITEM_H__