/*
YtCommand.h
YongTrim Lib
@initialize at 120910

@history
120910	yoonsr	initialize
*/


#ifndef __YTCOMMAND_H__
#define __YTCOMMAND_H__

#include "YtLib.h"


typedef enum tag_YtCommandStatus
{
	YtCommandStatus_Ready = 0,
	YtCommandStatus_Process = 1,
	YtCommandStatus_End = 2,
}YtCommandStatus;

typedef enum tag_YtCommandType
{
	YtCommandType_Tick,
	YtCommandType_Holding,	
	YtCommandType_Direct,	
}YtCommandType;

class YtCommand;
typedef YtCommand* (*YtCommandEndFunc)(YtCommand*);


class YtCommand : public CCObject
{
public:
	YtCommandStatus m_status;
	CCArray* m_commandsChild;

	YtCommandEndFunc m_endFunc;
	YtCommandType m_type;

	YtCommand();
	~YtCommand();

	virtual void start();
	virtual void update(float dt);
	virtual void stop();
};


class YtCommandManager : public CCObject
{
private:
	
public:
	CCArray* m_poolCommands;

	YtCommandManager();
	~YtCommandManager();

	void clear();
	void update(float dt);
	void addLast(YtCommand* command);
	void removeCommand(YtCommand* command);
	void removeAll();
};

#endif  // __YTCOMMAND_H__