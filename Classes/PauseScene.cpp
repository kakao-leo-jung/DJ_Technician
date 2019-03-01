#include "PauseScene.h"

USING_NS_CC;
using namespace CocosDenshion;

//scene 생성
Scene* PauseScene::createScene()
{
	auto scene = Scene::create();
	auto layer = PauseScene::create();
	scene->addChild(layer);\
	return scene;
}

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

//key Listener
void PauseScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event *event) {

	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_ENTER:
		/* 현재 선택 오퍼레이트 */
		
		break;
	case EventKeyboard::KeyCode::KEY_ESCAPE:
		/* 다시 게임 재개 - 되돌아가기 */
		Director::getInstance()->popScene();
		break;
	default:
		break;
	}

}
void PauseScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event) {

}

//Scene의 시작 init() 함수 정의
bool PauseScene::init()
{
	CCLOG("GameScene::init()..");
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	CCLOG("GameScene::init()..finished!");
	return true;
}

void PauseScene::onEnterTransitionDidFinish() {

	// 로딩화면 보여주기
	enableKeySetting();
}


// 키 세팅
void PauseScene::enableKeySetting() {
	//키 리스너 생성.
	auto key_listener = EventListenerKeyboard::create();
	key_listener->onKeyPressed = CC_CALLBACK_2(PauseScene::onKeyPressed, this);
	key_listener->onKeyReleased = CC_CALLBACK_2(PauseScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(key_listener, this);
}