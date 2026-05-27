/*
DyTycoonCommand.h
Doggy chef
@initialize at 121008

@history
121008	yoonsr	initialize
*/


#ifndef __DYTYCOONCOMMAND_H__
#define __DYTYCOONCOMMAND_H__

#include "DyConfig.h"
#include "YtCommand.h"

typedef enum tag_DyTycoonCommandType
{
	DyTycoonCommandType_AddBlockToCustomer,
	DyTycoonCommandType_AddBlockToCustomerReal,

	DyTycoonCommandType_AddSpecialBlockToCustomer,
	DyTycoonCommandType_AddSpecialBlockToCustomerReal,

	DyTycoonCommandType_AddSideToCustomer,
	DyTycoonCommandType_AddSideToCustomerReal,

	DyTycoonCommandType_AddGoldReal,


	//DyTycoonCommandType_AddHeartReal,

	DyTycoonCommandType_Delay,
	DyTycoonCommandType_AddTime,

	DyTycoonCommandType_Candy,
    DyTycoonCommandType_CandyToCustomer,
    
    DyTycoonCommandType_AddGoldCrazyReal,
    DyTycoonCommandType_AddDellyCrazyReal,
    DyTycoonCommandType_AddHeartCrazyReal,
    
    DyTycoonCommandType_ChagneStatus,

}DyTycoonCommandType;

class DyTycoonCommand : public YtCommand
{
public:
	DyTycoonCommandType m_commandtype;
	void* m_param0;
	void* m_param1;
	void* m_param2;
		
	int m_value0;
	int m_value1;

	float m_dtSleep;
	CCPoint m_point;

	DyTycoonCommand(DyTycoonCommandType type, void* param0, void* param1, void* param2, int value0, int value1, float fvalue);
	~DyTycoonCommand();

	virtual void start();
	virtual void update(float dt);
	virtual void stop();
};

#endif  // __DYTYCOONCOMMAND_H__