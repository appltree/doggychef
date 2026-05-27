/*
DyLayerRank_list.h
Doggy chef
@initialize at 130710

@history
130710	yoonsr	initialize
*/


#ifndef __DYLAYERRANKLIST_H__
#define __DYLAYERRANKLIST_H__

#include "YtScene.h"
#include "YtLayer.h"
#include "YtAniObject.h"
#include "DyFriend.h"
#include "DyRank.h"



class DyStageRank : public CCObject
{
public:
    int m_order;
    DyFriend* m_friend;
    
    //DyStageState m_state;
    int m_score;
};


typedef enum tag_DyLayerRankListType
{
    DyLayerRankListType_Friend,
    DyLayerRankListType_SubStage,
    DyLayerRankListType_Pride,
    DyLayerRankListType_Global,
}DyLayerRankListType;


class DyLayerRank_list : public CCLayer , public CCScrollViewDelegate
{
public:
    DyLayerRank_list(CCRect rect);
    ~DyLayerRank_list();
    
    int m_pridePoint;
    
    CCMenu* m_menuList;

    CCRect m_rect;
	CCSize m_sizeItem;
    
	int m_itemCount;
    int m_itemCountForHeight;
    
    int m_select_index;
    
    //int m_myIndex;
    
    DyFriend* m_friendProcess;
    int m_indexProcess;
    
    CCMenuItemImage* m_menuitemMine;
    
    DyLayerRankListType m_listType;
    DyRankType m_rankType;
 
    CCArray* m_arrRank;
    
    void setRankLayer(DyLayerRankListType listType, DyRankType rankType, CCArray* arrRank);
    void setRankGlobalLayer(DyRankType rankType);
   
    CCArray* m_arrPictureFrame;
    CCArray* m_arrAppFriend;
    CCArray* m_arrRankInList;
    
    YtAniObject* m_doggy;
    CCArray* m_arrButtons;

    bool m_isVisible;
    
	bool m_isScrollable;
    
    CCScrollView* m_scrollView;
    
    void pause();
	void resume();
    
    virtual void scrollViewDidScroll(CCScrollView* view);
	virtual void scrollViewDidZoom(CCScrollView* view);
	virtual void scrollViewTouchContent(CCScrollView* view, CCPoint position);
    
    void scrollViewTouchContentGlobal(CCScrollView* view, CCPoint position);

    void onMessageBlockClicked(CCObject * obj);
    void onPresentClicked(CCObject * obj);
    void onPrideClicked(CCObject * obj);
    void onInvitefirendClicked(CCObject * obj);
    
    DyFriend* getUser(int index);
    void setVisible(bool value);
    
    void onPopupClicked(cocos2d::CCObject * pSender);
    
//    void callfunc_sendMessagePride_success(float dt);
//    void callfunc_sendMessagePride_fail(float dt);
    
    void callfunc_update_me_success(float dt);
    void callfunc_update_me_fail(float dt);
    
    void callfunc_message_block_success(float dt);
    void callfunc_message_block_fail(float dt);
    
    void showDetailView(int global_order);
};


#endif  // __DYLAYERRANKLIST_H__