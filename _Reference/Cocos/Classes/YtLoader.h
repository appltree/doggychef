/*
YtLoader.h
YongTrim Lib
@initialize at 120907

@history
120913	yoonsr	initialize
*/


#ifndef __YTLOADER_H__
#define __YTLOADER_H__

#include "YtLib.h"

class YtObjectGroup : public CCObject
{
public:
	int m_resourceType;
	char m_resource_id[32];

	CCDictionary* m_dicMap;

	YtObjectGroup();
	~YtObjectGroup();

	const char* getValue(const char* tag, int index);
	CCArray* getArray(const char* tag, int index);
	int count();
};


class YtLoader : public CCObject
{
public:
	YtObjectGroup* loadObjects(const char* pszFileName);
	virtual int getType(const char* tag) = 0;
	static YtLoader* sharedLoader();
};


#endif  // __YTLOADER_H__