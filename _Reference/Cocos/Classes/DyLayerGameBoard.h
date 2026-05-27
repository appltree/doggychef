/*
DyLayerGameBoard.h
Doggy chef
@initialize at 120907

@history
120907	yoonsr	initialize
*/


#ifndef __DYLAYERGAMEBOARD_H__
#define __DYLAYERGAMEBOARD_H__

#include "TmLayerMain.h"
#include "TmBoard.h"
#include "TmBlock.h"

class DyBlock : public TmBlock
{
public:
    
	DyBlock();
	DyBlock(std::string tag, int id, const char *pszSpriteFileName, const char *pszSpriteGlowFileName, TmBoard* board);

	virtual void startAnimation();
	virtual void setForm();
	virtual void setBlockType(TmBlockType type);
    virtual void setAttr(TmBlockAttr attr, int attrValue);

	void endAnimationEffect(CCNode* node);
	void endAnimationHint(CCNode* node);
    void endAnimationHide(CCNode* node);
};

class DyBoard : public TmBoard
{
public:

	DyBoard();
	DyBoard(int cntCol, int cntRow);
    
    TmBlock* getBlockByIndex(int index);

	
    virtual void displayStatus();
	virtual TmBlock* getBlock(bool ignoreICE);
	virtual void addPoint(TmBlock* block);
	virtual void update(float dt);
	virtual void commandResult(TmCommandType type, bool success);
	
	virtual void bombArround(TmBlock* block);
	virtual bool selectSpecialBlock(TmBlock* block);
	virtual void bombEnable(bool value);
    virtual void comboEvent();
    
	void startAnimationAddCustomer(CCNode* node);	
	void endAnimationCombo(CCNode* node);
	void endAnimationEffect(CCNode* node);
	void endAnimationNoMove(CCNode* node);
	void endAnimationBomb(CCNode* node);
	virtual void startNoMoreMoveEvent();

	virtual void destoryBlock(std::string block_tag, CCPoint position);
};


class DyLayerGameBoard : public TmLayerMain
{
public:
    
	DyLayerGameBoard(bool isHard);
	~DyLayerGameBoard();
    
	virtual void ccTouchesBegan(CCSet* touches, CCEvent* event);
	virtual void ccTouchesEnded(CCSet* touches, CCEvent* event);
	virtual void ccTouchesMoved(CCSet* touches, CCEvent* event);

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
};


#endif  // __DYLAYERGAMEBOARD_H__