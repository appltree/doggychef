/*
YtLayerLog.h
YongTrim Lib
@initialize at 121002

@history
121002	yoonsr	initialize
*/


#ifndef __YTLAYERLOG_H__
#define __YTLAYERLOG_H__

#include "YtLayer.h"

class YtLayerLog : public YtLayer
{
public:
	CCScrollView* m_scrollView;
	CCLabelTTF *m_content;
	CCSize m_size;
	CCSize m_sizeContent;
	YtLayerLog(CCSize size,const char* strMessage);
    
	~YtLayerLog();

	//void addLog(const char* strMessage);

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
};


#endif  // __YTLAYERLOG_H__