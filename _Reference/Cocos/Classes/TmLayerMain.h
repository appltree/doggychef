/*
TmLayerMain.h
Three Match Lib
@initialize at 120907

@history
120907	yoonsr	initialize
*/

#ifndef __TMLAYERMAIN_H__
#define __TMLAYERMAIN_H__

#include "YtLayer.h"

class TmBoard;

class TmLayerMain : public YtLayer
{
public:
	
	TmBoard* m_board;

	CCSize m_size;

	TmLayerMain(CCSize size); 
	
	void update(float dt);

	virtual void visit();
};



#endif  // __TMLAYERMAIN_H__