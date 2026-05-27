/*
DyBlockInfo.h
Doggy chef
@initialize at 121008

@history
121008	yoonsr	initialize
*/


#ifndef __DYBLOCKINFO_H__
#define __DYBLOCKINFO_H__

#include "DyConfig.h"
#include "YtObject.h"


class DyBlockInfo : public CCObject
{
public:
    float m_probability;
	int m_id;
    char m_tag[80];
	char m_urlImage[80];
	char m_urlGlowImage[80];
	bool m_isBlockAvailable;


    int m_sound_id;
	DyBlockInfo();
	~DyBlockInfo();
    
};


class DyBlockManager : public CCObject
{
private:
    bool m_changeStatus;
    
public:
	DyBlockManager();
	~DyBlockManager();

    float m_dummyblock_weight;
    float m_totalCntForCreate;
	int m_cntBlockAvailable;
	CCArray* m_arrBlockInfo;// 사용가능한 블럭을 처음부터 순서로 집어 넣는다.
    
	void reloadBlockInfo(const char* strBlockInfoUrl);
	const char* getImageName(int id);
	const char* getImageNameByTag(const char* tag);
	int getIdByTag(const char* tag);
	const char* getTagByID(int id);
	DyBlockInfo* setBlockAvailable(const char* tag);
    DyBlockInfo* getBlockInfo(int id);
	void arrangeBlocks();
    void changeStatus();
    
    DyBlockInfo* getBlockInfoByTag(const char* tag);
	static DyBlockManager* sharedBlockManager();
};


#endif  // __DYBLOCKINFO_H__