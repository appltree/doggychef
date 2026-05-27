/*
TmCommand.h
Three Match Lib
@initialize at 120910

@history
120910	yoonsr	initialize
*/


#ifndef __TMCOMMAND_H__
#define __TMCOMMAND_H__

#include "YtCommand.h"

typedef enum tag_TmCommandType
{
	TmCommandType_SwapBlock,	
	//TmCommandType_AddSpeicalBlock,
	TmCommandType_DestoryBlock,			// only call by candy or msg
    TmCommandType_DestoryBlockLastPang,	// last pang
	TmCommandType_DecayAroundBlock,

	TmCommandType_DestoryBlockReal,
	TmCommandType_FallingBlockReal,
	TmCommandType_SwapBlockReal,
	TmCommandType_SwapReverseBlockReal,

	TmCommandType_Refresh,
	
	TmCommandType_Hint,
	TmCommandType_HintReal,

	TmCommandType_BombReady,
	TmCommandType_Bomb,
	TmCommandType_BombReal,
    
    TmCommandType_AddSpeicalAttr,
    
    TmCommandType_ClearBlocks,
	
}TmCommandType;

class TmCommand : public YtCommand
{
public:
	TmCommandType m_commandtype;
	void* m_param0;
	void* m_param1;
	void* m_param2;

	int m_value0;
	int m_value1;


	TmCommand(TmCommandType type, void* param0, void* param1, void* param2, int value0, int value1);
	~TmCommand();

	virtual void start();
	virtual void update(float dt);
	virtual void stop();
};


#endif  // __TMCOMMAND_H__