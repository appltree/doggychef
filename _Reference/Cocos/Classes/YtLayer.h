/*
YtLayer.h
YongTrim Lib
@initialize at 120907

@history
120907	yoonsr	initialize
*/


#ifndef __YTLAYER_H__
#define __YTLAYER_H__

#include "YtLib.h"

class YtLayer : public CCLayerColor
{
public:
	CCBAnimationManager *m_actionManager;

	std::string m_tag;

	bool m_isInit;
	bool m_isTop;

	bool m_isClip;
	bool m_isClipOut;

	CCNode* m_clipBoard;
	CCRect m_rectClip;

	void setClip(CCRect rect);
	void setClipOut(CCRect rect, CCNode* clipBoard);


	virtual void visit();
	YtLayer();
	~YtLayer();

    virtual void didLayerCreated(){};
    
	virtual void pause(){};
	virtual void resume(){};
	virtual void hide(){};
	virtual void show(){};

	void destory();

	static YtLayer* create();
};


class YtLayerInfo : public CCObject
{
public:
	YtLayer* m_layer;
	bool m_hasDimEffect;
	bool m_storeTouch;
    
	YtLayerInfo(YtLayer* layer);
	~YtLayerInfo();
};

class YtLayerManager : public CCObject
{
public:
	CCArray* m_arrLayerInfos;
	CCArray* m_arrToastInfos;

	CCLayerColor* m_layerDim;

	YtLayerManager();
	~YtLayerManager();

	void pushToast(YtLayer* layer);
	void popToast(YtLayer* layer);
	void destoryToast(YtLayer* layer);

	void pushLayer(YtLayer* layer, bool hasDimEffect, bool needPause);
	void popLayer();
	void clearLayers();
	void destoryTopLayer(bool needResume);
	YtLayer* getTopLayer();
    YtLayer* getLayerByFromTop(int index);
    YtLayer* getBaseLayer();
    YtLayer* getLayerByTag(std::string tag);
    
    int getLayerCount();
    
    
	static YtLayer* create(const char* pClassName, CCNodeLoader* pCCNodeLoader, const char* pCCBFileName, CCObject* owner);
	static void destory(YtLayer* layer);
	static YtLayerManager* sharedLayerManager();
};


#endif  // __YTLAYER_H__