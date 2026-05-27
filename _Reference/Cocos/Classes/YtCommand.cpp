/*
YtCommand.h
YongTrim Lib
@initialize at 120910

@history
120910	yoonsr	initialize
*/

#include "YtCommand.h"

YtCommand::YtCommand()
{
	m_type = YtCommandType_Tick;
	m_status = YtCommandStatus_Ready;
	m_commandsChild = new CCArray();

	m_endFunc = NULL;
}


YtCommand::~YtCommand()
{
	m_commandsChild->release();
}


void YtCommand::start()
{
}


void YtCommand::update(float dt)
{
}


void YtCommand::stop()
{
}



YtCommandManager::YtCommandManager()
{
	m_poolCommands = new CCArray();
}


YtCommandManager::~YtCommandManager()
{
	m_poolCommands->release();
}


void YtCommandManager::clear()
{
}


void YtCommandManager::update(float dt)
{
	if(m_poolCommands->count() == 0)
		return;
	

	for(unsigned int i = 0;i < m_poolCommands->count();)
	{
		YtCommand* command = (YtCommand*)m_poolCommands->objectAtIndex(i);

		if(command->m_type == YtCommandType_Direct)
		{
			command->start();
			m_poolCommands->removeObject(command);
		}
		else
			i++;
	}

	if(m_poolCommands->count() == 0)
		return;
	
	YtCommand* command = (YtCommand*)m_poolCommands->objectAtIndex(0);

	//CCLog("m_poolCommands->count() = %d, %d", m_poolCommands->count(), command->m_status);


	switch(command->m_status)
	{
		case YtCommandStatus_Ready:
			command->start();
			if(command->m_type == YtCommandType_Tick)
				command->m_status = YtCommandStatus_End;

			break;
		case YtCommandStatus_Process:
			command->update(dt);
			break;
		case YtCommandStatus_End:
			if(command->m_endFunc)
				command->m_endFunc(command);
			m_poolCommands->removeObjectAtIndex(0);
			break;
	}
}


void YtCommandManager::addLast(YtCommand* command)
{
	m_poolCommands->addObject(command);
}


void YtCommandManager::removeCommand(YtCommand* command)
{
}


void YtCommandManager::removeAll()
{
	m_poolCommands->removeAllObjects();
}