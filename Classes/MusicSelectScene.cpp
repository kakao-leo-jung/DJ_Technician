#include "MusicSelectScene.h"
#include "AppDelegate.h"
#include "AudioEngine.h"
#include "SimpleAudioEngine.h"

/* 키 연속 입력 제한 매크로 */
#define _KEYUSING if(status_keyUsing){ return;} status_keyUsing = true
/* 키 연속 입력 제한 해제 매크로 */
#define _KEYRELEASE(num) this->scheduleOnce(schedule_selector(MusicSelectScene::releaseKeyInput), num)

USING_NS_CC;
using namespace experimental;
using namespace CocosDenshion;

std::vector<Texture2D*> tpbgv;

//scene 생성
Scene* MusicSelectScene::createScene(std::vector<Texture2D*> v)
{
	tpbgv.assign(v.begin(), v.end());
	auto scene = Scene::create();
	auto layer = MusicSelectScene::create();
	scene->addChild(layer);
	return scene;
}

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

/* 키 리스너 관련 함수 */
void MusicSelectScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event *event) {
	if (std::find(heldKeys.begin(), heldKeys.end(), keyCode) == heldKeys.end()) {
		heldKeys.push_back(keyCode);
	}
}
void MusicSelectScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event) {
	heldKeys.erase(std::remove(heldKeys.begin(), heldKeys.end(), keyCode), heldKeys.end());
}
void MusicSelectScene::onKeyHold(float interval) {

	/*

		벡터에 연속 키입력이 쌓인다 따라서 키 입력 시 각 함수에
		연속 키 입력이 필요하지 않을 때는 status_keyUsing 을 true 처리해주고
		함수의 동작이 끝났을때 releaseKeyInput() 해주어야 한다.

	*/

	/* 방향키 UP 입력*/
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_UP_ARROW) != heldKeys.end()) {

		if (status_layer == STATUS::MUSICSELECT) {
			/* 곡 선택 인덱스 가로 개수만큼 감소 */
			changeMusicSelect(status_musicSelect, status_musicSelect - widthCount);
		}

	}

	/* 방향키 DOWN 입력*/
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_DOWN_ARROW) != heldKeys.end()) {

		if (status_layer == STATUS::MUSICSELECT) {
			/* 곡 선택 인덱스 가로 개수만큼 증가 */
			changeMusicSelect(status_musicSelect, status_musicSelect + widthCount);
		}

	}

	/* 방향키 RIGHT 입력*/
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_RIGHT_ARROW) != heldKeys.end()) {

		if (status_layer == STATUS::MUSICSELECT) {
			/* 곡 선택 인덱스 증가 */
			changeMusicSelect(status_musicSelect, status_musicSelect + 1);
		}

	}

	/* 방향키 LEFT 입력*/
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_LEFT_ARROW) != heldKeys.end()) {

		if (status_layer == STATUS::MUSICSELECT) {
			/* 곡 선택 인덱스 감소 */
			changeMusicSelect(status_musicSelect, status_musicSelect - 1);
		}

	}

	/* ENTER 키 입력 */
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_ENTER) != heldKeys.end()) {

		if (status_layer == STATUS::MUSICSELECT) {
			/* 곡 상세보기 레이어 진입 */
			showMusicInfo();
		}

		if (status_layer == STATUS::MUSICINFO) {
			/* 현재 곡으로 게임 시작 */
			startGame();
		}

	}

	/* ESC 키 입력 */
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_ESCAPE) != heldKeys.end()) {

		if (status_layer == STATUS::MUSICINFO) {
			/* 곡 상세보기 취소 */
			hideMusicInfo();
		}

	}

	/* BACKSPACE 키 입력 */
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_BACKSPACE) != heldKeys.end()) {

		if (status_layer == STATUS::MUSICINFO) {
			/* 곡 상세보기 취소 */
			hideMusicInfo();
		}

	}
}

void MusicSelectScene::releaseKeyInput(float interval) {
	status_keyUsing = false;
}

/*

	마우스 리스너 관련 함수

*/

/* 마우스 클릭 시 */
void MusicSelectScene::onMouseDown(cocos2d::Event* event) {

}

/* 마우스 클릭 후 땠을때 */
void MusicSelectScene::onMouseUp(cocos2d::Event* event) {

}

/* 마우스 이동 시 - 마우스 포인터 변경 */
void MusicSelectScene::onMouseMove(cocos2d::Event* event) {
	/*

		마우스 이동을 추적해서 마우스스프라이트의 위치를 계속 바꾸어줌

	*/
	auto mouseEvent = static_cast<EventMouse*>(event);
	Vec2 movePoint = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
	cursor_sprite->setPosition(movePoint);
}

/* 마우스 스크롤 시 */
void MusicSelectScene::onMouseScroll(cocos2d::Event* event) {

}

/* Scene의 시작 init() 함수 정의 */
bool MusicSelectScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	return true;
}

/* 씬 전환 후 로딩 시작 */
void MusicSelectScene::onEnterTransitionDidFinish() {

	setup();

}

void MusicSelectScene::setup() {

	/* 멤버변수 초기화 */
	size_window = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
	cache = Director::getInstance()->getTextureCache();
	status_keyUsing = true;
	heldKeys.clear();
	background_texture.assign(tpbgv.begin(), tpbgv.end());

	/*

		BMS 리스트 읽어올 때 스레드를 사용하며 로딩화면을 보여주고 있는다.
		스레드 콜백을 통해 setBmsList() 가 완료되면 콜백을 통해 전달하고
		콜백함수에서 나머지 세팅을 진행한다.

	*/

	/* 로딩 화면 보여주기 */
	showLoading();

	/* BMS 모든 파일 리딩 및 불러오기 */
	setBmsList();

	/* 키 및 마우스, 터치 입력 오픈 */
	enableKeySetting();

	/* 각 레이어 세팅 */
	setLayerMusicSelect();
	setLayerSubBar();
	setLayerMusicInfo();

	/* 시작하면 보여줌 */
	showMusicSelect();

}

/* BMS 파일을 읽어들여 벡터에 저장한다. */
void MusicSelectScene::setBmsList() {

	//auto parser = BMSParser::getInstance();

	/* 쓰레드 사용해서 로딩 */
	std::thread loading(BMSParser::readAllBmsHeader, &musicHeaders);

	/* 파서에서 모든 폴더의 음악 헤더정보 읽어옴 */
	//BMSParser::getInstance()->readAllBmsHeader(&musicHeaders);

	/* 스레드 합류점 */
	loading.join();

#ifdef _DEBUG
	/* 파일 정보 체크 */
	CCLOG("################### FILE READ CHECK ####################");
	CCLOG("########################################################");
	for (MusicHeader mh : musicHeaders) {
		CCLOG("------------- /bms/%s/%s INFORMATION --------------", mh.getValues(MusicHeader::DIR).c_str(), mh.getValues(MusicHeader::FILENAME).c_str());
		CCLOG("PLAYER    : %s", mh.getValues(MusicHeader::PLAYER).c_str());
		CCLOG("GENRE     : %s", mh.getValues(MusicHeader::GENRE).c_str());
		CCLOG("TITLE     : %s", mh.getValues(MusicHeader::TITLE).c_str());
		CCLOG("ARTIST    : %s", mh.getValues(MusicHeader::ARTIST).c_str());
		CCLOG("BPM       : %s", mh.getValues(MusicHeader::BPM).c_str());
		CCLOG("PLAYLEVEL : %s", mh.getValues(MusicHeader::PLAYLEVEL).c_str());
		CCLOG("RANK      : %s", mh.getValues(MusicHeader::RANK).c_str());
		CCLOG("SUBTITLE  : %s", mh.getValues(MusicHeader::SUBTITLE).c_str());
		CCLOG("TOTAL     : %s", mh.getValues(MusicHeader::TOTAL).c_str());
		CCLOG("STAGEFILE : %s", mh.getValues(MusicHeader::STAGEFILE).c_str());
	}
	CCLOG("################### FILE READ FINISHED##################");
	CCLOG("########################################################");
#endif



}

/* 키 입력 및 마우스 세팅 */
void MusicSelectScene::enableKeySetting() {

	/* 키 입력 세팅 */
	this->schedule(schedule_selector(MusicSelectScene::onKeyHold));
	auto key_listener = EventListenerKeyboard::create();
	key_listener->onKeyPressed = CC_CALLBACK_2(MusicSelectScene::onKeyPressed, this);
	key_listener->onKeyReleased = CC_CALLBACK_2(MusicSelectScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(key_listener, this);

	/* 커서 이미지 세팅*/
	cursor_sprite = Sprite::create(CURSOR_FILENAME);
	this->addChild(cursor_sprite, 10);

	/* 마우스 입력 세팅 */
	auto Mouse = EventListenerMouse::create();
	Mouse->onMouseDown = CC_CALLBACK_1(MusicSelectScene::onMouseDown, this);
	Mouse->onMouseUp = CC_CALLBACK_1(MusicSelectScene::onMouseUp, this);
	Mouse->onMouseMove = CC_CALLBACK_1(MusicSelectScene::onMouseMove, this);
	Mouse->onMouseScroll = CC_CALLBACK_1(MusicSelectScene::onMouseScroll, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(Mouse, this);

}

/* 기본 레이어 세팅*/
void MusicSelectScene::setLayerBasicLayer() {
	/*

			StartScene 에서 텍스쳐벡터를 넘겨받아 메모리에 로딩.
			텍스쳐벡터를 한 프레임당 순회하며 교체함으로써 영상재생

			10~20초 정도의 영상 루프는 이 방법이 훨씬 효율적!
			그러나 1분넘어가는 영상을 한번에 이미지로드하면 컴퓨터가 죽는다

			무조건 10~20초 정도 루프영상만 사용할 것.

	*/

	/* 배경 스프라이트 초기화 및 세팅 */
	background_sprite = Sprite::create();
	background_iter = background_texture.begin();
	background_sprite->initWithTexture(*background_iter);
	background_sprite->setContentSize(size_window);
	background_sprite->setPosition(size_window.width / 2, size_window.height / 2);
	background_sprite->setVisible(false);

	/* 한 틱당 스케쥴 함수 호출해서 영상 texture 갱신 */
	this->schedule(schedule_selector(MusicSelectScene::background_tick));

	/* 기본 레이어에 배경 스프라이트 등록*/
	this->addChild(background_sprite);

}

/* 배경 동영상 재생*/
void MusicSelectScene::background_tick(float interval) {
	/* 텍스쳐를 끝까지 재생한 경우 다시 처음부터 갱신*/
	if (background_iter == background_texture.end()) {
		background_iter = background_texture.begin();
	}
	background_sprite->setTexture(*background_iter++);
}

/* 로딩 화면 보여주기 */
void MusicSelectScene::showLoading() {

	/* 배경 레이어 설정 */
	setLayerBasicLayer();

	/* 배경 스프라이트 온 */
	background_sprite->setVisible(true);
	background_sprite->setOpacity(100);

	/* 로딩 라벨 설정 */
	loading_label = Label::createWithTTF(LOADING_STR, LOADING_FONT, LOADING_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER);
	loading_label->setPosition(size_window.width / 2, size_window.height / 2);
	this->addChild(loading_label);
	auto fi = FadeIn::create(0.1f);
	auto ft = FadeTo::create(0.3f, 100);
	auto dt = DelayTime::create(0.4f);
	auto seq = Sequence::create(fi, ft, dt, nullptr);
	auto rep = RepeatForever::create(seq);
	loading_label->runAction(rep);

}

/* 곡 선택 레이어 세팅*/
void MusicSelectScene::setLayerMusicSelect() {

	/* 레이어 초기화 */
	layer_musicSelect = Layer::create();
	layer_musicSelect->setContentSize(size_window);
	layer_musicSelect->setPosition(0, 0);
	layer_musicSelect->setVisible(false);
	this->addChild(layer_musicSelect);

	/* 가로에 들어갈 곡 선택 object 개수 */
	widthCount = layer_musicSelect->getContentSize().width / MSO_SPRITE_WIDTH;


	/* 라벨 초기화 및 세팅 */
	auto iter = musicHeaders.begin();
	int idx = 0;
	while (iter != musicHeaders.end()) {
		/*

			모든 헤더정보를 통해 stage 파일 텍스쳐를 로드 및 세팅하고
			곡 타이틀과 랭킹정보를 초기화한다.

			이때 라벨은 스프라이트의 자식노드.

		*/

#ifdef _DEBUG
		CCLOG("========= this MusicHeaders (%d) make to SelectObject .. ==========", idx);
#endif

		/* 현재 스프라이트의 row 와 col 및 위치 계산 */
		int col = idx % widthCount;
		int row = idx / widthCount;
		int widthPos = (layer_musicSelect->getContentSize().width / widthCount + MSO_SPRITE_WIDTHDIFF)
			* col - MSO_SPRITE_WIDTH / 2;
		int heightPos = -(MSO_SPRITE_HEIGHT - MSO_LABEL_LEVELPOSH - MSO_LABEL_TITLEPOSH) * row + size_window.height;

		/* 현재 헤더정보를 통해 Node객체 생성 */
		MusicHeader curHeader = *iter;


		/* 스프라이트 경로 설정 */
		std::string spritePath;
		spritePath = "bms/" + curHeader.getValues(MusicHeader::DIR)
			+ "/" + curHeader.getValues(MusicHeader::STAGEFILE);

		/* 스프라이트 경로 유효성 검사 */
		struct stat buffer;
		if (curHeader.getValues(MusicHeader::STAGEFILE).compare("") == 0
			|| curHeader.getValues(MusicHeader::STAGEFILE).find(".bmp", 0, 4) != std::string::npos
			|| stat(spritePath.c_str(), &buffer) != 0) {
			/* 스테이지 파일이 존재하지 않을 때 || 스테이지 파일이 비트맵 포맷일 때 */
			/*

				향후 이미지 파일 로딩할 때 이 부분에서 텍스쳐를 못 읽는 것을 검사하고 예외처리한다.

				 1. BMS 파일 내부에 STAGEFILE 헤더가 존재하지 않는 경우
				 2. STAGEFILE 의 확장자가 비트맵일때 (cocos2dx 는 비트맵 지원 x - 나중에 비트맵 변환 공부해볼 것)
				 3. STAGEFILE 은 존재하지만 실제 얻은 경로에 해당하는 파일이 존재하지 않는 경우.

			*/
			spritePath = MSO_NOIMAGEFILE;	// 임시
		}

#ifdef _DEBUG
		CCLOG("stagefilePath : %s", spritePath.c_str());
#endif

		/* Batch Drawing 성능 비교 */
		SpriteBatchNode *batchNode = SpriteBatchNode::create(spritePath);
		this->addChild(batchNode);
		auto tpSprite = Sprite::createWithTexture(batchNode->getTexture());

		/* 일반 스프라이트 생성 할때 */
		//cache->addImage(spritePath);
		//auto tpSprite = Sprite::createWithTexture(cache->getTextureForKey(spritePath));

		tpSprite->setContentSize(Size(MSO_SPRITE_WIDTH, MSO_SPRITE_HEIGHT));
		tpSprite->setPosition(widthPos, heightPos);
		tpSprite->setOpacity(MSO_OPACITY_UNSELECTED);
		layer_musicSelect->addChild(tpSprite);
		auto tpLabelTitle = Label::createWithTTF("TITLE : " + curHeader.getValues(MusicHeader::TITLE) + curHeader.getValues(MusicHeader::SUBTITLE),
			MSO_FONT, MSO_FONTSIZE, Size(MSO_LABEL_WIDTH, MSO_LABEL_HEIGHT), TextHAlignment::LEFT, TextVAlignment::TOP);
		tpLabelTitle->setPosition(Point(tpSprite->getContentSize().width / 2, MSO_LABEL_TITLEPOSH));
		tpLabelTitle->setOpacity(MSO_OPACITY_UNSELECTED);
		tpSprite->addChild(tpLabelTitle);
		auto tpLabelLevel = Label::createWithTTF("LEVEL : " + curHeader.getValues(MusicHeader::PLAYLEVEL),
			MSO_FONT, MSO_FONTSIZE, Size(MSO_LABEL_WIDTH, MSO_LABEL_HEIGHT), TextHAlignment::LEFT, TextVAlignment::TOP);
		tpLabelLevel->setPosition(Point(tpSprite->getContentSize().width / 2, MSO_LABEL_LEVELPOSH));
		tpLabelLevel->setOpacity(MSO_OPACITY_UNSELECTED);
		tpSprite->addChild(tpLabelLevel);

		/* 곡 선택 객체 등록 */
		musicSelectObject.push_back(
			{
				/* stagefile_sprite, label_title, label_playerLevel */
				tpSprite, tpLabelTitle, tpLabelLevel
			}
		);

		/* 다음 헤더정보 탐색 */
		iter++;
		idx++;
	}

}

/* 서브 상 하단바 레이어 세팅 */
void MusicSelectScene::setLayerSubBar() {

	/* 레이어 초기화 */
	layer_subBar = Layer::create();
	layer_subBar->setContentSize(size_window);
	layer_subBar->setPosition(0, 0);
	layer_subBar->setVisible(false);
	this->addChild(layer_subBar);

	/* 상단바 스프라이트 세팅 */
	cache->addImage(SUBBAR_TOPFILE);
	subBarTop_sprite = Sprite::createWithTexture(cache->getTextureForKey(SUBBAR_TOPFILE));
	subBarTop_sprite->setPosition(size_window.width / 2, size_window.height - subBarTop_sprite->getContentSize().height / 2);
	layer_subBar->addChild(subBarTop_sprite);
	auto fi = FadeIn::create(0.1f);
	auto ft = FadeTo::create(0.3f, 100);
	auto dt = DelayTime::create(0.6f);
	auto seq = Sequence::create(fi, ft, dt, nullptr);
	auto rep = RepeatForever::create(seq);
	subBarTop_sprite->runAction(rep);

	/* 하단바 스프라이트 세팅 */
	cache->addImage(SUBBAR_BTMFILE);
	subBarBtm_sprite = Sprite::createWithTexture(cache->getTextureForKey(SUBBAR_BTMFILE));
	subBarBtm_sprite->setPosition(size_window.width / 2, subBarBtm_sprite->getContentSize().height / 2);
	layer_subBar->addChild(subBarBtm_sprite);
	fi = FadeIn::create(0.1f);
	ft = FadeTo::create(0.3f, 100);
	dt = DelayTime::create(0.6f);
	seq = Sequence::create(fi, ft, dt, nullptr);
	rep = RepeatForever::create(seq);
	subBarBtm_sprite->runAction(rep);

	/* 서브바 현재 인덱스 라벨 세팅 */
	subBar_label_curIdx = Label::createWithTTF("0", SUBBAR_INDEX_FONT, SUBBAR_INDEX_FONTSIZE,
		Size(100, subBarTop_sprite->getContentSize().height), TextHAlignment::RIGHT, TextVAlignment::CENTER);
	subBar_label_curIdx->setPosition(45, subBarTop_sprite->getContentSize().height / 2);
	subBarTop_sprite->addChild(subBar_label_curIdx);

	/* 서브바 전체 인덱스 라벨 세팅 */
	subBar_label_maxIdx = Label::createWithTTF("/ " + std::to_string(musicHeaders.size()),
		SUBBAR_INDEX_FONT, SUBBAR_INDEX_FONTSIZE, Size(100, subBarTop_sprite->getContentSize().height),
		TextHAlignment::LEFT, TextVAlignment::CENTER);
	subBar_label_maxIdx->setPosition(100 + 50, subBarTop_sprite->getContentSize().height / 2);
	subBarTop_sprite->addChild(subBar_label_maxIdx);

	/* 서브바 타이틀 라벨 세팅 */
	subBar_label_title = Label::create("", SUBBAR_TITLE_FONT, SUBBAR_TITLE_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER);
	subBar_label_title->setPosition(subBarTop_sprite->getContentSize().width / 2, subBarTop_sprite->getContentSize().height / 2);
	subBarTop_sprite->addChild(subBar_label_title);

}

/* 곡 상세정보 레이어 세팅 */
void MusicSelectScene::setLayerMusicInfo() {

	/* 레이어 초기화 */
	layer_musicInfo = Layer::create();
	layer_musicInfo->setContentSize(size_window);
	layer_musicInfo->setPosition(0, 0);
	layer_musicInfo->setVisible(false);
	this->addChild(layer_musicInfo);

	/* 백보드 스프라이트 세팅 */
	cache->addImage(MUSICINFO_BACKBOARDFILE);
	musicInfo_sprite_backBoard = Sprite::createWithTexture(cache->getTextureForKey(MUSICINFO_BACKBOARDFILE));
	musicInfo_sprite_backBoard->setPosition(size_window.width / 4 * 3, size_window.height / 2);
	layer_musicInfo->addChild(musicInfo_sprite_backBoard);
	auto fi = FadeIn::create(0.1f);
	auto ft = FadeTo::create(0.3f, 200);
	auto dt = DelayTime::create(0.6f);
	auto seq = Sequence::create(fi, ft, dt, nullptr);
	auto rep = RepeatForever::create(seq);
	musicInfo_sprite_backBoard->runAction(rep);

#pragma region PLAYER 라벨 세팅
	/* 백보드 PLAYER 세팅 */
	musicInfo_label_ui[MUSICINFO_LABEL::PLAYER] = Label::createWithTTF(
		MUSICINFO_LABELSTR[MUSICINFO_LABEL::PLAYER],
		MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(100, 30), TextHAlignment::LEFT, TextVAlignment::CENTER
	);
	musicInfo_label_ui[MUSICINFO_LABEL::PLAYER]->setPosition(
		MUSICINFO_LEFTPADDING,
		musicInfo_sprite_backBoard->getContentSize().height - 60);
	musicInfo_label_ui[MUSICINFO_LABEL::PLAYER]->setOpacity(150);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_ui[MUSICINFO_LABEL::PLAYER]);

	musicInfo_label_info[MUSICINFO_LABEL::PLAYER] = Label::createWithTTF(
		"", MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER
	);
	musicInfo_label_info[MUSICINFO_LABEL::PLAYER]->setPosition(
		(musicInfo_sprite_backBoard->getContentSize().width) / 2,
		musicInfo_label_ui[MUSICINFO_LABEL::PLAYER]->getPositionY()
	);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_info[MUSICINFO_LABEL::PLAYER]);
#pragma endregion

#pragma region GENRE 세팅
	/* 백보드 GENRE 세팅 */
	musicInfo_label_ui[MUSICINFO_LABEL::GENRE] = Label::createWithTTF(
		MUSICINFO_LABELSTR[MUSICINFO_LABEL::GENRE],
		MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(100, 30), TextHAlignment::LEFT, TextVAlignment::CENTER
	);
	musicInfo_label_ui[MUSICINFO_LABEL::GENRE]->setPosition(
		MUSICINFO_LEFTPADDING,
		musicInfo_label_info[MUSICINFO_LABEL::PLAYER]->getPositionY() - 40
	);
	musicInfo_label_ui[MUSICINFO_LABEL::GENRE]->setOpacity(150);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_ui[MUSICINFO_LABEL::GENRE]);

	musicInfo_label_info[MUSICINFO_LABEL::GENRE] = Label::createWithTTF(
		"", MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER
	);
	musicInfo_label_info[MUSICINFO_LABEL::GENRE]->setPosition(
		(musicInfo_sprite_backBoard->getContentSize().width) / 2,
		musicInfo_label_ui[MUSICINFO_LABEL::GENRE]->getPositionY() - 40
	);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_info[MUSICINFO_LABEL::GENRE]);
#pragma endregion

#pragma region TITLE 세팅
	/* 백보드 TITLE 세팅 */
	musicInfo_label_ui[MUSICINFO_LABEL::TITLE] = Label::createWithTTF(
		MUSICINFO_LABELSTR[MUSICINFO_LABEL::TITLE],
		MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(100, 30), TextHAlignment::LEFT, TextVAlignment::CENTER
	);
	musicInfo_label_ui[MUSICINFO_LABEL::TITLE]->setPosition(
		MUSICINFO_LEFTPADDING,
		musicInfo_label_info[MUSICINFO_LABEL::GENRE]->getPositionY() - 40
	);
	musicInfo_label_ui[MUSICINFO_LABEL::TITLE]->setOpacity(150);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_ui[MUSICINFO_LABEL::TITLE]);

	musicInfo_label_info[MUSICINFO_LABEL::TITLE] = Label::createWithTTF(
		"", MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER
	);
	musicInfo_label_info[MUSICINFO_LABEL::TITLE]->setPosition(
		(musicInfo_sprite_backBoard->getContentSize().width) / 2,
		musicInfo_label_ui[MUSICINFO_LABEL::TITLE]->getPositionY() - 40
	);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_info[MUSICINFO_LABEL::TITLE]);
#pragma endregion

#pragma region ARTIST 세팅
	/* 백보드 ARTIST 세팅 */
	musicInfo_label_ui[MUSICINFO_LABEL::ARTIST] = Label::createWithTTF(
		MUSICINFO_LABELSTR[MUSICINFO_LABEL::ARTIST],
		MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(100, 30), TextHAlignment::LEFT, TextVAlignment::CENTER
	);
	musicInfo_label_ui[MUSICINFO_LABEL::ARTIST]->setPosition(
		MUSICINFO_LEFTPADDING,
		musicInfo_label_info[MUSICINFO_LABEL::TITLE]->getPositionY() - 40
	);
	musicInfo_label_ui[MUSICINFO_LABEL::ARTIST]->setOpacity(150);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_ui[MUSICINFO_LABEL::ARTIST]);

	musicInfo_label_info[MUSICINFO_LABEL::ARTIST] = Label::createWithTTF(
		"", MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER
	);
	musicInfo_label_info[MUSICINFO_LABEL::ARTIST]->setPosition(
		(musicInfo_sprite_backBoard->getContentSize().width) / 2,
		musicInfo_label_ui[MUSICINFO_LABEL::ARTIST]->getPositionY() - 40
	);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_info[MUSICINFO_LABEL::ARTIST]);
#pragma endregion

#pragma region BPM 세팅
	/* 백보드 ARTIST 세팅 */
	musicInfo_label_ui[MUSICINFO_LABEL::BPM] = Label::createWithTTF(
		MUSICINFO_LABELSTR[MUSICINFO_LABEL::BPM],
		MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(100, 30), TextHAlignment::LEFT, TextVAlignment::CENTER
	);
	musicInfo_label_ui[MUSICINFO_LABEL::BPM]->setPosition(
		MUSICINFO_LEFTPADDING,
		musicInfo_label_info[MUSICINFO_LABEL::ARTIST]->getPositionY() - 40
	);
	musicInfo_label_ui[MUSICINFO_LABEL::BPM]->setOpacity(150);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_ui[MUSICINFO_LABEL::BPM]);

	musicInfo_label_info[MUSICINFO_LABEL::BPM] = Label::createWithTTF(
		"", MUSICINFO_FONT, MUSICINFO_FONTSIZE + 50,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER
	);
	musicInfo_label_info[MUSICINFO_LABEL::BPM]->setPosition(
		(musicInfo_sprite_backBoard->getContentSize().width) / 2,
		musicInfo_label_ui[MUSICINFO_LABEL::BPM]->getPositionY() - 40
	);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_info[MUSICINFO_LABEL::BPM]);
#pragma endregion

#pragma region PLAYLEVEL 세팅
	/* 백보드 PLAYLEVEL 세팅 */
	musicInfo_label_ui[MUSICINFO_LABEL::PLAYLEVEL] = Label::createWithTTF(
		MUSICINFO_LABELSTR[MUSICINFO_LABEL::PLAYLEVEL],
		MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(100, 30), TextHAlignment::LEFT, TextVAlignment::CENTER
	);
	musicInfo_label_ui[MUSICINFO_LABEL::PLAYLEVEL]->setPosition(
		MUSICINFO_LEFTPADDING,
		musicInfo_label_info[MUSICINFO_LABEL::BPM]->getPositionY() - 50
	);
	musicInfo_label_ui[MUSICINFO_LABEL::PLAYLEVEL]->setOpacity(150);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_ui[MUSICINFO_LABEL::PLAYLEVEL]);

	musicInfo_label_info[MUSICINFO_LABEL::PLAYLEVEL] = Label::createWithTTF(
		"", MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER
	);
	musicInfo_label_info[MUSICINFO_LABEL::PLAYLEVEL]->setPosition(
		(musicInfo_sprite_backBoard->getContentSize().width) / 2,
		musicInfo_label_ui[MUSICINFO_LABEL::PLAYLEVEL]->getPositionY() - 40
	);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_info[MUSICINFO_LABEL::PLAYLEVEL]);
#pragma endregion

#pragma region RANK 세팅
	/* 백보드 RANK 세팅 */
	musicInfo_label_ui[MUSICINFO_LABEL::RANK] = Label::createWithTTF(
		MUSICINFO_LABELSTR[MUSICINFO_LABEL::RANK],
		MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(100, 30), TextHAlignment::LEFT, TextVAlignment::CENTER
	);
	musicInfo_label_ui[MUSICINFO_LABEL::RANK]->setPosition(
		MUSICINFO_LEFTPADDING,
		musicInfo_label_info[MUSICINFO_LABEL::PLAYLEVEL]->getPositionY() - 40
	);
	musicInfo_label_ui[MUSICINFO_LABEL::RANK]->setOpacity(150);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_ui[MUSICINFO_LABEL::RANK]);

	musicInfo_label_info[MUSICINFO_LABEL::RANK] = Label::createWithTTF(
		"", MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER
	);
	musicInfo_label_info[MUSICINFO_LABEL::RANK]->setPosition(
		(musicInfo_sprite_backBoard->getContentSize().width) / 2,
		musicInfo_label_ui[MUSICINFO_LABEL::RANK]->getPositionY()
	);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_info[MUSICINFO_LABEL::RANK]);
#pragma endregion

#pragma region TOTAL 세팅
	/* 백보드 TOTAL 세팅 */
	musicInfo_label_ui[MUSICINFO_LABEL::TOTAL] = Label::createWithTTF(
		MUSICINFO_LABELSTR[MUSICINFO_LABEL::TOTAL],
		MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(100, 30), TextHAlignment::LEFT, TextVAlignment::CENTER
	);
	musicInfo_label_ui[MUSICINFO_LABEL::TOTAL]->setPosition(
		MUSICINFO_LEFTPADDING,
		musicInfo_label_info[MUSICINFO_LABEL::RANK]->getPositionY() - 40
	);
	musicInfo_label_ui[MUSICINFO_LABEL::TOTAL]->setOpacity(150);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_ui[MUSICINFO_LABEL::TOTAL]);

	musicInfo_label_info[MUSICINFO_LABEL::TOTAL] = Label::createWithTTF(
		"", MUSICINFO_FONT, MUSICINFO_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER
	);
	musicInfo_label_info[MUSICINFO_LABEL::TOTAL]->setPosition(
		(musicInfo_sprite_backBoard->getContentSize().width) / 2,
		musicInfo_label_ui[MUSICINFO_LABEL::TOTAL]->getPositionY()
	);
	musicInfo_sprite_backBoard->addChild(musicInfo_label_info[MUSICINFO_LABEL::TOTAL]);
#pragma endregion

}

/* 곡 선택 레이어 진입*/
void MusicSelectScene::showMusicSelect() {
	/*

		모든 setup() 동작이 완료되고나서 call 됨
		Loading Label을 끈 다음 배경과 showPressAnyKey layer를 띄워준다

		layer_status 변경
		키 입력방지 해제 - 지금부터 키 입력 가능

	*/

	/* 배경음 재생 */
	startBGM(0.0f);

	/* 현재 씬 레이어 상태 변경*/
	status_layer = STATUS::MUSICSELECT;

	/* 로딩 씬 해제 */
	loading_label->setVisible(false);

	/* setVisible = true */
	layer_musicSelect->setVisible(true);
	layer_subBar->setVisible(true);

	/* 현재 선택된 곡(0번 인덱스)에 따라 레이어 위치 조정 */
	int col = status_musicSelect % widthCount;
	int row = status_musicSelect / widthCount;
	int widthStatic = size_window.width / 2 + (MSO_SPRITE_WIDTHDIFF + MSO_SPRITE_WIDTH) / 2;
	int widthDiff = (layer_musicSelect->getContentSize().width / widthCount + MSO_SPRITE_WIDTHDIFF);
	int heightStatic = -size_window.height / 2 + MSO_SPRITE_WIDTHDIFF;
	int heightDiff = (MSO_SPRITE_HEIGHT - MSO_LABEL_LEVELPOSH - MSO_LABEL_TITLEPOSH);

	layer_musicSelect->setPosition(Point(widthStatic - (col * widthDiff)
		, heightStatic + (row * heightDiff)));

	musicSelectObject[status_musicSelect].stagefile_sprite->setOpacity(255);
	musicSelectObject[status_musicSelect].title_label->setOpacity(255);
	musicSelectObject[status_musicSelect].rank_label->setOpacity(255);

	/* 모든 애니메이션이 끝나고 키 입력 해제 */
	_KEYRELEASE(0.2f);

	/* 등장 효과음 재생 */

	/*

		등장 씬 애니메이션 설정 및 재생

	*/



}

/* 곡 상세정보 레이어 진입 */
void MusicSelectScene::showMusicInfo() {
	/*

		현재 status_musicSelect 에 맞는 곡 정보를 불러와
		musicInfo_label_info[] 의 스트링을 바꾸어준다.

	*/

	/* 키 입력 잠금 */
	_KEYUSING;

	/* 효과음 재생 */
	SimpleAudioEngine::getInstance()->playEffect(SOUND_SELECT);

	/* 현재 씬 레이어 상태 변경*/
	status_layer = STATUS::MUSICINFO;

	/* 현재 선택 씬 스프라이트 확대 이동 애니메이션 */
	auto sb = ScaleBy::create(0.15f, 2.0f);
	auto mov = MoveBy::create(0.15f, Point(-size_window.width / 4, 0));
	auto spawn = Spawn::create(sb, mov, nullptr);
	musicSelectObject[status_musicSelect].stagefile_sprite->runAction(spawn);

	auto dt = DelayTime::create(0.15f);
	auto fi = FadeIn::create(0.1f);
	auto cb = CallFunc::create(CC_CALLBACK_0(MusicSelectScene::releaseKeyInput, this, 0.0f));
	auto seq = Sequence::create(dt, fi, cb, nullptr);
	musicInfo_sprite_backBoard->setOpacity(0);
	musicInfo_sprite_backBoard->runAction(seq);

	/* setVisible = true */
	layer_musicInfo->setVisible(true);

	/* 곡 세부정보 라벨 입력 */
	musicInfo_label_info[MUSICINFO_LABEL::PLAYER]->setString(
		musicHeaders[status_musicSelect].getValues(MusicHeader::PLAYER)
	);
	musicInfo_label_info[MUSICINFO_LABEL::GENRE]->setString(
		musicHeaders[status_musicSelect].getValues(MusicHeader::GENRE)
	);
	musicInfo_label_info[MUSICINFO_LABEL::TITLE]->setString(
		musicHeaders[status_musicSelect].getValues(MusicHeader::TITLE)
	);
	musicInfo_label_info[MUSICINFO_LABEL::ARTIST]->setString(
		musicHeaders[status_musicSelect].getValues(MusicHeader::ARTIST)
	);
	musicInfo_label_info[MUSICINFO_LABEL::BPM]->setString(
		musicHeaders[status_musicSelect].getValues(MusicHeader::BPM)
	);
	musicInfo_label_info[MUSICINFO_LABEL::PLAYLEVEL]->setString(
		musicHeaders[status_musicSelect].getValues(MusicHeader::PLAYLEVEL)
	);
	musicInfo_label_info[MUSICINFO_LABEL::RANK]->setString(
		musicHeaders[status_musicSelect].getValues(MusicHeader::RANK)
	);
	musicInfo_label_info[MUSICINFO_LABEL::TOTAL]->setString(
		musicHeaders[status_musicSelect].getValues(MusicHeader::TOTAL)
	);

	/* 모든 전환 애니메이션이 종료하고 후에 콜백으로 키 해제 */
	//_KEYRELEASE(0.2f);

}

/* 곡 상세정보 레이어 나오기*/
void MusicSelectScene::hideMusicInfo() {

	/* 키 입력 잠금 */
	_KEYUSING;

	/* 효과음 재생 */
	SimpleAudioEngine::getInstance()->playEffect(SOUND_BACK);

	/* 현재 씬 레이어 상태 변경*/
	status_layer = STATUS::MUSICSELECT;

	/* 현재 선택 씬 스프라이트 축소 복귀 애니메이션 */
	auto sb = ScaleBy::create(0.2f, 0.5f);
	auto mov = MoveBy::create(0.2f, Point(size_window.width / 4, 0));
	auto spawn = Spawn::create(sb, mov, nullptr);
	auto cb = CallFunc::create(CC_CALLBACK_0(MusicSelectScene::releaseKeyInput, this, 0.0f));
	auto seq = Sequence::create(spawn, cb, nullptr);
	musicSelectObject[status_musicSelect].stagefile_sprite->runAction(seq);

	/* setVisible = true */
	layer_musicInfo->setVisible(false);

	/* 모든 전환 애니메이션이 종료하고 후에 콜백으로 키 해제 */
	//_KEYRELEASE(0.2f);

}

/* 곡 선택 변경 */
void MusicSelectScene::changeMusicSelect(int from, int to) {

	/* 키 제한 */
	_KEYUSING;

	/* 인덱스 벗어나는경우 처리 */
	if (to >= musicSelectObject.size() || to < 0) {
		/* 진행불가 사운드 재생 */
		SimpleAudioEngine::getInstance()->playEffect(SOUND_TICK);

		/* 키 제한 해제 */
		_KEYRELEASE(0.1f);
		return;

	}

	/* 효과음 재생 */
	SimpleAudioEngine::getInstance()->playEffect(SOUND_CHANGEMUSICSELECT);

	/* 선택 인덱스 변경 */
	status_musicSelect = to;

	/* 서브바 인덱스 및 타이틀 라벨 변경 */
	subBar_label_curIdx->setString(std::to_string(status_musicSelect));
	subBar_label_title->setString(musicHeaders[status_musicSelect].getValues(MusicHeader::TITLE));
	subBar_label_title->setOpacity(0);
	auto fi = FadeIn::create(MSO_ACTIONTIME_CHANGE);
	subBar_label_title->runAction(fi);

	/* 인덱스 변경에 따른 레이어 위치 조정 */
	int col = status_musicSelect % widthCount;
	int row = status_musicSelect / widthCount;
	int widthStatic = size_window.width / 2 + (MSO_SPRITE_WIDTHDIFF + MSO_SPRITE_WIDTH) / 2;
	int widthDiff = (layer_musicSelect->getContentSize().width / widthCount + MSO_SPRITE_WIDTHDIFF);
	int heightStatic = -size_window.height / 2 + MSO_SPRITE_WIDTHDIFF;
	int heightDiff = (MSO_SPRITE_HEIGHT - MSO_LABEL_LEVELPOSH - MSO_LABEL_TITLEPOSH);

	auto mt = MoveTo::create(MSO_ACTIONTIME_CHANGE, Point(widthStatic - (col * widthDiff)
		, heightStatic + (row * heightDiff)));
	auto el = EaseElasticOut::create(mt);
	auto cb = CallFunc::create(CC_CALLBACK_0(MusicSelectScene::releaseKeyInput, this, 0.0f));
	auto seq = Sequence::create(el, cb, nullptr);

	layer_musicSelect->runAction(seq);

	/* 선택된 오브젝트 설정 */
	musicSelectObject[status_musicSelect].stagefile_sprite->setOpacity(255);
	musicSelectObject[status_musicSelect].title_label->setOpacity(255);
	musicSelectObject[status_musicSelect].rank_label->setOpacity(255);

	/* 해제된 오브젝트 설정 */
	musicSelectObject[from].stagefile_sprite->setOpacity(MSO_OPACITY_UNSELECTED);
	musicSelectObject[from].title_label->setOpacity(MSO_OPACITY_UNSELECTED);
	musicSelectObject[from].rank_label->setOpacity(MSO_OPACITY_UNSELECTED);

	/* 키 잠금 해제 */
}

/* 게임 시작 */
void MusicSelectScene::startGame() {

	_KEYUSING;

	SimpleAudioEngine::getInstance()->playEffect(SOUND_CHANGELAYER);
	AudioEngine::stopAll();
	auto musicSelectScene = GameScene::createScene(
		musicHeaders[status_musicSelect].getValues(MusicHeader::DIR),
		musicHeaders[status_musicSelect].getValues(MusicHeader::FILENAME),
		1.0f,
		true
	);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, musicSelectScene));

}

/* 메인 BGM재생*/
void MusicSelectScene::startBGM(float interval) {
	AudioEngine::play2d(BGMMAIN_FILENAME, true, BGMMAIN_VOLUME);
}
