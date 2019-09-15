#include "ResultScene.h"

USING_NS_CC;
using namespace CocosDenshion;
using namespace Judge;

Score _tpScore;
MusicHeader _info;

//scene 생성
Scene* ResultScene::createScene(MusicHeader header, Score score)
{
	_info = header;
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
		/* select scene으로 넘어가기 */
		enterOperate();
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

	/* 곡, 스코어 정보 받아오기 */
	score = _tpScore;
	music_header = _info;

	/* 스코어 계산 */
	calculateScore();

}

/* 스코어 정보 계산 */
void ResultScene::calculateScore() {

	/* 여기서 total_score 를 산정한다 */
	total_score = score.getCurrentScore();

}

/* UI 데이터 로드 */
void ResultScene::setUiInfo() {

	setLayerBasicLayer();

	/* 검은 바탕 레이어 생성 */
	cache->addImage(RECTBACKGROUND_FILENAME);
	rectBackground_sprite = Sprite::createWithTexture(cache->getTextureForKey(RECTBACKGROUND_FILENAME));
	rectBackground_sprite->setAnchorPoint(Point(0, 0));
	rectBackground_sprite->setPosition(0,0);
	rectBackground_sprite->setOpacity(RECTBACKGROUND_OPACITY);
	this->addChild(rectBackground_sprite);

	/* 타이틀 라벨 생성 */
	title_label = Label::createWithTTF(music_header.getValues(MusicHeader::TITLE), UI_LABEL_TITLE_FONT, UI_LABEL_TITLE_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER);
	title_label->setPosition(size_window.width / 2, size_window.height - 100);
	rectBackground_sprite->addChild(title_label);

	/* DjLevel 라벨 생성 */
	djLevel_label = Label::createWithTTF(Judge::DJLEVEL_STR[score.getDjLevel()], UI_LABEL_DJLEVEL_FONT, UI_LABEL_DJLEVEL_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER);
	djLevel_label->setPosition(size_window.width / 4, size_window.height / 16 * 11);
	djLevel_label->setColor(Judge::DJLEVEL_COLOR[score.getDjLevel()]);
	rectBackground_sprite->addChild(djLevel_label);

	/* 곡 stagefile 스프라이트 생성 */
	//std::string path = "bms/" + music_header.getValues(MusicHeader::DIR) + "/" + music_header.getValues(MusicHeader::FILENAME);
	//cache->addImage(path);
	//music_sprite = Sprite::createWithTexture(cache->getTextureForKey(path));
	//music_sprite->setPosition(size_window.width / 4 * 3, size_window.height / 16 * 9);
	//music_sprite->setContentSize(Size(256, 256));
	//rectBackground_sprite->addChild(music_sprite);

	/* score 라벨 생성 */
	score_label = Label::createWithTTF(std::to_string(total_score), UI_LABEL_SYSTEM_FONT, UI_LABEL_SYSTEMSCORE_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER);
	score_label->setPosition(size_window.width / 4, size_window.height / 16*11 - 70);
	rectBackground_sprite->addChild(score_label);

	/* info 라벨 생성 */
	info_label = Label::createWithTTF("SCORE INFORMATION", UI_LABEL_SYSTEM_FONT, UI_LABEL_SYSTEM_FONTSIZE + 10,
		Size(), TextHAlignment::LEFT, TextVAlignment::CENTER);
	info_label->setPosition(size_window.width / 4, size_window.height / 16 * 11 - 90);
	rectBackground_sprite->addChild(info_label);

	/* max 콤보 ui 라벨 생성 */
	maxCombo_label_ui = Label::createWithTTF("MAX", UI_LABEL_SYSTEM_FONT, UI_LABEL_SYSTEM_FONTSIZE,
		Size(150, 50), TextHAlignment::LEFT, TextVAlignment::CENTER);
	maxCombo_label_ui->setPosition(size_window.width / 4, size_window.height / 2 - 90);
	rectBackground_sprite->addChild(maxCombo_label_ui);

	/* max 콤보 라벨 생성 */
	maxCombo_label = Label::createWithTTF(":   " + std::to_string(score.getMaxCombo()), UI_LABEL_SYSTEM_FONT, UI_LABEL_SYSTEM_FONTSIZE,
		Size(150, 50), TextHAlignment::LEFT, TextVAlignment::CENTER);
	maxCombo_label->setPosition(size_window.width / 4 + 150, size_window.height / 2 - 80);
	rectBackground_sprite->addChild(maxCombo_label);

	/* judgeCount 라벨 생성 */
	int i = 0;
	while (i < 5) {
		
		judgeCount_label_ui[i] = Label::createWithTTF(Judge::JUDGE_STR[i], UI_LABEL_SYSTEM_FONT, UI_LABEL_SYSTEM_FONTSIZE,
			Size(150,50), TextHAlignment::LEFT, TextVAlignment::CENTER);
		judgeCount_label_ui[i]->setPosition(size_window.width / 4, maxCombo_label->getPositionY() - ((i + 1) * 30));
		rectBackground_sprite->addChild(judgeCount_label_ui[i]);

		judgeCount_label[i] = Label::createWithTTF(":   " + std::to_string(score.getjudgeCount(i)), UI_LABEL_SYSTEM_FONT, UI_LABEL_SYSTEM_FONTSIZE,
			Size(150, 50), TextHAlignment::LEFT, TextVAlignment::CENTER);
		judgeCount_label[i]->setPosition(size_window.width / 4 + 150, maxCombo_label->getPositionY() - ((i + 1) * 30));
		rectBackground_sprite->addChild(judgeCount_label[i]);

		i++;
	}
	
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

/* Enter 키 눌렀을 때 동작 */
void ResultScene::enterOperate() {

	/* 재시작, 등 여러 선택지 만들어야 함 */
	
	/* 임시 - 돌아가기 */
	auto musicSelectScene = MusicSelectScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, musicSelectScene));

}