/*
YtScene.h
YongTrim Lib
@initialize at 120907

@history
120907	yoonsr	initialize
*/


#ifndef __YTSCENE_H__
#define __YTSCENE_H__

#include "YtLib.h"

class YtScene : public CCScene
{
private:
	CCSprite* m_sprLeftBox;
	CCSprite* m_sprRightBox;
public:
	CCBAnimationManager *m_actionManager;

	virtual void didSceneChanged(){};
    YtScene();
	~YtScene();
    
    virtual void resumeByPhone(){};
    virtual void pauseByPhone(){};
};


class YtSceneMananger : public CCObject
{
public:
	CCScene* m_sceneMain;
	YtScene* m_sceneCur;

	YtSceneMananger();
	void destoryCurrentScene();

	static YtSceneMananger* sharedSceneManager();
	//static void changeScene(YtScene* scene);
	static void changeScene(YtScene* scene, CCNodeLoader * pCCNodeLoader, const char* pCCBFileName, const char* className);
	//static void pushScene(YtScene* scene, CCNodeLoader * pCCNodeLoader, const char* pCCBFileName, const char* className);
};

#endif  // __YTSCENE_H__