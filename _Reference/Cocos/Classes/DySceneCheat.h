/*
DySceneCheat.h
Doggy chef
@initialize at 130128

@history
130128	yoonsr	initialize
*/


#ifndef __DYSCENECHEAT_H__
#define __DYSCENECHEAT_H__

#include "YtScene.h"
#include "YtLayer.h"


#define DYLAYERCHEAT_TAG                        "DyLayerCheat"

#define CHAET_UI_TAG_MENU						1000
#define CHAET_UI_TAG_MENUITEM_CLOSE				1001

#define CHEAT_UI_TAG_MENUITEM_PLUS_SNACK		1200
#define CHEAT_UI_TAG_MENUITEM_PLUS_HAMBURGER	1201
#define CHEAT_UI_TAG_MENUITEM_PLUS_COFFEE		1202

#define CHEAT_UI_TAG_MENUITEM_PLUS_CHINA        1203

#define CHEAT_UI_TAG_MENUITEM_PLUS_PIZZA        1204

#define CHEAT_UI_TAG_MENUITEM_PLUS_KOREAN        1205


#define CHEAT_UI_TAG_STATUS_SNACK				2000
#define CHEAT_UI_TAG_STATUS_HAMBURGER			2001
#define CHEAT_UI_TAG_STATUS_COFFEE				2002
#define CHEAT_UI_TAG_STATUS_CHINA				2003
#define CHEAT_UI_TAG_STATUS_PIZZA				2004

#define CHEAT_UI_TAG_STATUS_KOREAN				2005



class DyLayerCheat
: public YtLayer
, public cocos2d::extension::CCBSelectorResolver
, public cocos2d::extension::CCBMemberVariableAssigner
, public cocos2d::extension::CCNodeLoaderListener
{
public:

	CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(DyLayerCheat, create);
	virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual cocos2d::extension::SEL_CCControlHandler onResolveCCBCCControlSelector(cocos2d::CCObject * pTarget, cocos2d::CCString * pSelectorName);
	virtual bool onAssignCCBMemberVariable(cocos2d::CCObject * pTarget, cocos2d::CCString * pMemberVariableName, cocos2d::CCNode * pNode);
	virtual void onNodeLoaded(cocos2d::CCNode * pNode, cocos2d::extension::CCNodeLoader * pNodeLoader);

	void onBackClicked(CCObject * pSender);
	void onPlusClicked(CCObject * pSender);
	void onResetClicked(CCObject * pSender);
    void onPresentClicked(CCObject * pSender);
    void onPresentAllClicked(CCObject * pSender);
    void onStaffClicked(CCObject * pSender);

    virtual void update(float dt);

    
	DyLayerCheat();
	virtual ~DyLayerCheat();

	virtual void pause();
	virtual void resume();
	virtual void hide();
	virtual void show();

	CCMenu* m_menu;

    void callfunc_update_result_success(float dt);
    void callfunc_update_result_fail(float dt);

    
    void network_send_present(SEL_HttpResponse pSelector);
    bool network_send_present_ret(CCHttpClient *sender, CCHttpResponse *response);

    void network_send_present_cafe24(SEL_HttpResponse pSelector);
    bool network_send_present_cafe24_ret(CCHttpClient *sender, CCHttpResponse *response);

    void network_send_present_all(SEL_HttpResponse pSelector);
    bool network_send_present_all_ret(CCHttpClient *sender, CCHttpResponse *response);
    void network_send_present_all_cafe24(SEL_HttpResponse pSelector);
    bool network_send_present_all_cafe24_ret(CCHttpClient *sender, CCHttpResponse *response);
    
    
    void network_regist_notice(SEL_HttpResponse pSelector);
    bool network_regist_notice_ret(CCHttpClient *sender, CCHttpResponse *response);
    void network_regist_notice_cafe24(SEL_HttpResponse pSelector);
    bool network_regist_notice_cafe24_ret(CCHttpClient *sender, CCHttpResponse *response);
    
    
    void onHttpRequestCompleted(CCHttpClient *sender, CCHttpResponse *response);
	
	void reloadStatus();
};


class DyLayerCheatLoader : public CCLayerLoader 
{
    public:
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(DyLayerCheatLoader, loader);

    protected:
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(DyLayerCheat);
};

class DySceneCheat : public YtScene
{
public:
	DySceneCheat();
	~DySceneCheat();
};


#endif  // __DYSCENETITLE_H__