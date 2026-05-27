/*
DyToast.h
Doggy chef
@initialize at 120907

@history
120907	yoonsr	initialize
*/


#ifndef __DYTOAST_H__
#define __DYTOAST_H__

#include "YtLib.h"
#include "DyPopup.h"

#include "YtLayerLog.h"
#include "YtAniObject.h"
#include "YtSpriteEx.h"


typedef enum tag_DyToastType
{
	DyToastType_Text,
    //DyToastType_Notice,
    
}DyToastType;


class DyToast : public YtLayer
{
private:
	DyToastType m_toasttype;
public:
    CCObject*			m_pListener;
    SEL_CallFuncO		m_pfnSelector;

	CCScale9Sprite* m_background;

	CCArray* m_arrButtons;
	CCLabelTTF* m_content;
	CCSize m_size;
	CCMenu *m_menu;

	void onButtonClicked(CCObject *obj);

	DyToast();
	~DyToast();
	void arrangeButton();
	void setText(const char* text);
	int m_active_tag;

	void addButton(DyButtonType type, const char* name, int tag);

	void showToast();
	void hideToast();

	virtual void destory();
	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();
	static DyToast* create(DyToastType type, CCObject *rec, SEL_CallFuncO selector);

    
    void soundEffect(CCNode* node);
	void endAnimationHideToast(CCNode* node);

};


#endif  // __DYTOAST_H__