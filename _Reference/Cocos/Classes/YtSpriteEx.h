/*
YtSpriteEx.h
YongTrim Lib.
@initialize at 120401

@history
120401	yoonsr	initialize
*/


#ifndef __YTSPRITEEX_H__
#define __YTSPRITEEX_H__


#define YTSPRITEEX_NOTCROPNODE      10000

#include "YtLib.h"

using namespace cocos2d;

class YtMaskSprite : public CCSprite
{
private:
	CCSpriteFrame *m_pMaskSpriteFrame;

	virtual void draw(void);

	void drawTexture();
	~YtMaskSprite();
public:

	static YtMaskSprite* spriteWithSpriteFrame(CCSpriteFrame *pSpriteFrame, CCSpriteFrame *pMaskSpriteFrame);
};

class YtBlankSprite : public CCSprite
{
public:
	static YtBlankSprite* spriteWithSizeAndColor(CCSize size, ccColor3B color); 
};

class YtBoxSprite : public CCLayer
{
public:
	static YtBoxSprite* spriteWithSize(CCSize size, const char *strFile);
};


class YtPatternSprite : public CCLayerColor
{
public:
	CCSize m_size;
	virtual void visit();

	static YtPatternSprite* spriteWithSize(CCSize size, const char* fileImage);
};

class YtCropSprite : public CCSprite
{
private:
	virtual void visit(void);
public:
    bool m_isCrop;
	static YtCropSprite* create(const char *pszFileName);
};


class YtAsyncSprite : public CCSprite
{
private:
    std::string m_url;
    std::string m_tag;
    CCSize m_size;
    bool m_save;
    bool m_stop;
    
public:
    static YtAsyncSprite* create(const char *pszDefaultSpriteFrameName, const char *strUrl, const char* strTag, bool saveFile,CCSize size);
    
    
    void downloadPicture(SEL_HttpResponse pSelector);
    void stopDownload();
    
    void onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response);
};




//
//class YTClipSprite : public CCSprite
//{
//	CCRect m_rectClip;
//private:
//	virtual void visit(void);
//public:
//	CCRect getClip();
//	void setClip(CCRect rect);
//	static YTClipSprite* spriteWithSpriteFrame(CCSpriteFrame *pSpriteFrame);
//};
//
//
//class YTClipTo : public CCActionInterval
//{
//public:
//	/** initializes the action */
//	bool initWithDuration(ccTime duration, const CCRect& rect);
//
//	//virtual CCObject* copyWithZone(CCZone* pZone);
//	virtual void startWithTarget(CCNode *pTarget);
//	virtual void update(ccTime time);
//
//public:
//	/** creates the action */
//	static YTClipTo* actionWithDuration(ccTime duration, const CCRect& rect);
//
//protected:
//	CCRect m_endRect;
//	CCRect m_startRect;
//	CCRect m_deltaRect;
//};

#endif // __YTSPRITEEX_H__
