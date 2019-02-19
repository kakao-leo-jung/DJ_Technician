#include "cocos2d.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

class PauseScene : public cocos2d::Layer
{
private:
	

public:

	static cocos2d::Scene* createScene();

	virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event *event);
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode, cocos2d::Event *event);
	void enableKeySetting();

	// implement the "static create()" method manually
	CREATE_FUNC(PauseScene);
};
