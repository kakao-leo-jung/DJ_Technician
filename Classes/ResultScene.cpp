#include "ResultScene.h"

USING_NS_CC;
using namespace CocosDenshion;
using namespace Judge;

Score _tpScore;

//scene 생성
Scene* ResultScene::createScene(Score score)
{
	_tpScore = score;
	auto scene = Scene::create();
	auto layer = ResultScene::create();
	scene->addChild(layer);
		return scene;
}

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

//key Listener
void ResultScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event *event) {

	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_ENTER:
		/* 현재 선택 오퍼레이트 */

		break;
	default:
		break;
	}

}
void ResultScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event) {

}

//Scene의 시작 init() 함수 정의
bool ResultScene::init()
{
	CCLOG("ResultScene::init()..");
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	CCLOG("ResultScene::init()..finished!");
	return true;
}

void ResultScene::onEnterTransitionDidFinish() {

	initData();
	setUiInfo();

	enableKeySetting();

}

/* 데이터 로드 */
void ResultScene::initData() {

	/* 변수 초기화 */
	size_window = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
	cache = Director::getInstance()->getTextureCache();

	/* 스코어 정보 받아오기 */
	score = _tpScore;

}

/* UI 데이터 로드 */
void ResultScene::setUiInfo() {

	setLayerBasicLayer();


	
}

/* 기본 레이어 세팅*/
void ResultScene::setLayerBasicLayer() {
	/*

			텍스쳐벡터를 한 프레임당 순회하며 교체함으로써 영상재생

	*/

	video_capture.open(BACKGROUND_FILENAME);
	if (!video_capture.isOpened()) {
		CCLOG("can't open video background..");
		return;
	}
	else {
		video_capture >> video_frame;

		/* BGR 에서 RGB 컬러 변경 */
		cv::cvtColor(video_frame, video_frame, CV_BGR2RGB);

		/* 사전에 미리 캡쳐한 프레임들을 모두 texture 화 하여 벡터에 저장 */
		background_texture = new Texture2D();
		background_texture->initWithData(video_frame.data,
			video_frame.elemSize() * video_frame.cols * video_frame.rows,
			Texture2D::PixelFormat::RGB888,
			video_frame.cols,
			video_frame.rows,
			Size(video_frame.cols, video_frame.rows));
		video_capture >> video_frame;

		/* 배경 스프라이트 초기화 및 세팅 */
		background_sprite = Sprite::create();
		background_sprite->initWithTexture(background_texture);
		background_sprite->setContentSize(size_window);
		background_sprite->setPosition(size_window.width / 2, size_window.height / 2);
		background_sprite->setOpacity(100);

		/* 한 틱당 스케쥴 함수 호출해서 영상 texture 갱신 */
		this->schedule(schedule_selector(ResultScene::background_tick), 1.0 / 30);

		/* 기본 레이어에 배경 스프라이트 등록*/
		this->addChild(background_sprite);

	}


}

/* 배경 동영상 재생*/
void ResultScene::background_tick(float interval) {
	/* 다음 프레임을 비디오캡쳐에서 가져옴 */
	video_capture >> video_frame;
	if (!video_frame.empty()) {

		/* BGR 에서 RGB 컬러 변경 */
		cv::cvtColor(video_frame, video_frame, CV_BGR2RGB);

		/* 다음 프레임의 MAT 파일로 텍스쳐를 업데이트 한다 */
		background_texture->updateWithData(video_frame.data, 0, 0, video_frame.cols, video_frame.rows);
	}
	else {
		/*

			배경동영상 무한 루프

		*/
		video_capture.open(BACKGROUND_FILENAME);
	}
}

// 키 세팅
void ResultScene::enableKeySetting() {
	//키 리스너 생성.
	auto key_listener = EventListenerKeyboard::create();
	key_listener->onKeyPressed = CC_CALLBACK_2(ResultScene::onKeyPressed, this);
	key_listener->onKeyReleased = CC_CALLBACK_2(ResultScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(key_listener, this);
}