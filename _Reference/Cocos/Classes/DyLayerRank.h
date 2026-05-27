/*
DyLayerRank.h
Doggy chef
@initialize at 130104

@history
130104	yoonsr	initialize
*/


#ifndef __DYLAYERRANK_H__
#define __DYLAYERRANK_H__


#include "YtScene.h"
#include "YtLayer.h"
#include "YtAniObject.h"
#include "DyLayerRank_list.h"
#include "DyRank.h"


typedef enum tag_DyLayerRankType
{
    DyLayerRankType_Friend,
    DyLayerRankType_Global
}DyLayerRankType;

class DyLayerRank : public YtLayer
{
public:
    DyLayerRankType m_layerRankType;
    
    DyRankType m_tmpType;
    
    CCMenu* m_menu;
	
	DyLayerRank();
	virtual ~DyLayerRank();

    CCLabelTTF* m_labelRemainTime;
 
    DyLayerRank_list* m_layer_rank_list_story;
    DyLayerRank_list* m_layer_rank_list_crazy;
    
    DyRankType m_status_friend;
    
    void displayModeType();
    
    void setModeType(DyRankType type);
    
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

    virtual void onEnter();
	virtual void onExit();
    
    void cbCrazyModeRefresh(float dt);

	void onTabClicked(CCObject * obj);

    void callfunc_load_leader_board_success(float dt);
    void callfunc_load_leader_board_fail(float dt);

    
    void loadGlobalRankList(DyRankType rankType);
    
	static DyLayerRank* create(DyLayerRankType type);
    
    

};


#endif  // __DYLAYERPOSTBOX_H__