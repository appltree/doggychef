/*
DyLoader.h
Doggy chef
@initialize at 120913

@history
120907	yoonsr	initialize
*/


#ifndef __DYLOADER_H__
#define __DYLOADER_H__

#include "YtLoader.h"

typedef enum tag_DyObjectGroupType
{
	DyObjectGroupType_Shop,
	DyObjectGroupType_Level,
	DyObjectGroupType_Block,
	DyObjectGroupType_Recipe,
	DyObjectGroupType_Character,
	DyObjectGroupType_Item,
	DyObjectGroupType_Costume,
	DyObjectGroupType_Text,
	DyObjectGroupType_Delly,
    DyObjectGroupType_Parameters,
    DyObjectGroupType_Staff,
    

}DyObjectGroupType;


class DyLoader : public YtLoader
{
public:
	DyLoader();
	~DyLoader();

	virtual int getType(const char* tag);

	static DyLoader* sharedLoader();
};


#endif  // __DYLOADER_H__