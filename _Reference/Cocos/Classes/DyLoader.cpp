/*
DyLoader.cpp
Doggy chef
@initialize at 120913

@history
120907	yoonsr	initialize
*/

#include "DyLoader.h"

extern YtLoader *g_sharedLoader;

DyLoader::DyLoader()
{
}


DyLoader::~DyLoader()
{
}


DyLoader* DyLoader::sharedLoader()
{
	if(g_sharedLoader == NULL)
	{
		g_sharedLoader = new DyLoader();
	}
	return (DyLoader*)g_sharedLoader;
}


int DyLoader::getType(const char* tag)
{
	if(strcmp(tag, "level") == 0)
		return (int)DyObjectGroupType_Level;
	else if(strcmp(tag, "block") == 0)
		return (int)DyObjectGroupType_Block;
	else if(strcmp(tag, "shop") == 0)
		return (int)DyObjectGroupType_Shop;
	else if(strcmp(tag, "recipe") == 0)
		return (int)DyObjectGroupType_Recipe;
	else if(strcmp(tag, "character") == 0)
		return (int)DyObjectGroupType_Character;
	else if(strcmp(tag, "item") == 0)
		return (int)DyObjectGroupType_Item;
	else if(strcmp(tag, "costume") == 0)
		return (int)DyObjectGroupType_Costume;
	else if(strcmp(tag, "text") == 0)
		return (int)DyObjectGroupType_Text;
	else if(strcmp(tag, "delly") == 0)
		return (int)DyObjectGroupType_Delly;
	else if(strcmp(tag, "parameters") == 0)
		return (int)DyObjectGroupType_Parameters;
	else if(strcmp(tag, "staff") == 0)
		return (int)DyObjectGroupType_Staff;

	return -1;
}