/*
DyLayerRankDetail.h
Doggy chef
@initialize at 130707

@history
130707	yoonsr	initialize
*/


#ifndef __DYLAYERRANKDETAIL_H__
#define __DYLAYERRANKDETAIL_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "YtAniObject.h"
#include "DyFriend.h"
#include "YtSpriteEx.h"
#include "DyRank.h"

class DyLayerRankDetail : public YtLayer
{
public:
    bool m_isGlobal;
    
	DyLayerRankDetail();
	virtual ~DyLayerRankDetail();

    CCSprite* m_sprBg;
    
    CCPoint m_posCenter;
    DyFriend* m_friendInfo;
    DyRankType m_type;
    
    unsigned int m_score;
    unsigned int m_scoreBest;
    
    int m_global_order;
    
    DyRankGlobal *m_rankGlobal;
    
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
    virtual void keyBackClicked();

    virtual void onEnter();
	virtual void onExit();
    
    
    virtual void registerWithTouchDispatcher();
	 
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
	static DyLayerRankDetail* create(CCPoint posCenter, DyFriend* friendInfo, DyRankType type, unsigned int score, unsigned int best_score, int order_global);
    static DyLayerRankDetail* create(CCPoint posCenter, DyRankType type, DyRankGlobal *rank);
    
};


#endif  // __DYLAYERRANKDETAIL_H__