/*
DyTutorial.h
Doggy chef
@initialize at 130516

@history
130516	yoonsr	initialize
*/


#ifndef __DYTUTORIAL_H__
#define __DYTUTORIAL_H__

#include "DyConfig.h"
#include "YtObject.h"
#include "TmBlock.h"


typedef enum tag_DyTutorialType
{
    DyTutorialType_Start,
    DyTutorialType_End,
    
    DyTutorialType_BlockFallStart,
    
    DyTutorialType_BlockSettingDefault,
    DyTutorialType_OrderRecipe,
    DyTutorialType_OrderRecipeByDelivery,
    
    DyTutorialType_Customer,
    DyTutorialType_AddSpecialBlock,
    DyTutorialType_Delay,
    DyTutorialType_Hold,
   
    DyTutorialType_Message,
    
    DyTutorialType_ReadyCommand,
    
}DyTutorialType;


typedef enum tag_DyTutorialCommandType
{
    DyTutorialCommandType_None,
    DyTutorialCommandType_Must,
    DyTutorialCommandType_ThreeMatch,
    DyTutorialCommandType_GetMoney,
    DyTutorialCommandType_Sell,
    DyTutorialCommandType_SideMenuClick,
    DyTutorialCommandType_SideMoveToCustomer,
    DyTutorialCommandType_TouchQuestDetail,
    DyTutorialCommandType_RemoveCurrentBlock,
    DyTutorialCommandType_UseItem,
    DyTutorialCommandType_DeliveryGetPhone,
    DyTutorialCommandType_DeliveryMake,
    DyTutorialCommandType_DeliveryCall,
    
    
}DyTutorialCommandType;

typedef enum tag_DyTutorialStatus
{
    DyTutorialStatus_Ready,
    DyTutorialStatus_Process,
    DyTutorialStatus_Terminate,
    
    
}DyTutorialStatus;

//
class DyTutorial : public CCObject
{
private:
    
    void processCommand();
    void processMessage();

public:
    
    DyTutorialType m_tutorial_type;
    DyTutorialCommandType m_command_type;
    
    int m_messageType; // 고기굽는 튜토리얼용 
    
    CCArray* m_arrNodes;
    
    CCMenu* m_menu_ok;
    
    DyTutorialStatus m_tutorial_status;
    float m_time_excute;
    
    void* m_param0;
    int m_paramInt0;
    YtLibSndEff m_sndEffect;
    
    std::string m_message;
    
    DyTutorial(DyTutorialType type, DyTutorialCommandType commandType, void* param0, void *param1, int paramInt0, YtLibSndEff sndEffect);
	~DyTutorial();
    
    void onOKClicked(CCObject * obj);
    void onSkipClicked(CCObject * obj);
    void initMessageBox();
    void initBox(CCRect rectBox);
    
    void update(float dt);
    
    void pause();
    void resume();
    
    int m_value_internal;
    
    CCPoint m_posDoggy;
    CCRect m_rectBox;
    CCPoint m_posIndicator;
    
    CCSprite* m_sprIndicator;
//    void update();
//    void display();
    
    CCPoint m_posArrow;
    int m_directArrow;
    
    //
    // direct 0:up, 1:right, 2:down, 3:left
    void addArrow(CCPoint position, int direct);
};


class DyTutorialManager : public CCObject
{
public:
    CCLayer* m_layer;
    CCSprite* m_boardSprite;
    CCSprite* m_sprGrill;
    
    bool m_doingSpecial;
    
    bool m_pauseBoard;
    
    CCArray* m_poolTutorials;
    
    TmBlock* m_currentBlock;
    
    int m_index;
    CCLayer* getMainLayer();
    CCPoint getCenterPos();
    
	DyTutorialManager();
	~DyTutorialManager();

    void clearTutorial();
    void update(float dt);
    void addLast(DyTutorial* tutorial);
    DyTutorial* getCurrentTutorial();
    void removeTutorial(DyTutorial* tutorial);
    
    void buildTutorial_0();
    void buildTutorial_1();
    void buildTutorial_2();
    void buildTutorial_3();
    void buildTutorial_4();
    void buildTutorial_5();
    void buildTutorial_6();
    void buildTutorial_7();
    void buildTutorial_8();
    
    void buildTutorial(int index);
    
    void showTutorial(bool show);
    
    static DyTutorialManager* sharedTutorialManager();
};



#endif  // __DYTUTORIAL_H__