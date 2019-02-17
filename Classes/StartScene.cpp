#include "StartScene.h"
#include "MusicSelectScene.h"
#include "AppDelegate.h"
#include "DBManager.h"
#include "AudioEngine.h"
#include "SimpleAudioEngine.h"

/* 키 연속 입력 제한 매크로 */
#define _KEYUSING if(status_keyUsing){ return;} status_keyUsing = true
/* 키 연속 입력 제한 해제 매크로 */
#define _KEYRELEASE(num) this->scheduleOnce(schedule_selector(StartScene::releaseKeyInput), num)

USING_NS_CC;
using namespace experimental;
using namespace CocosDenshion;

//scene 생성
Scene* StartScene::createScene()
{
	auto scene = Scene::create();
	auto layer = StartScene::create();
	scene->addChild(layer);
	return scene;
}

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

/* 키 리스너 관련 함수 */
void StartScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event *event) {
	if (std::find(heldKeys.begin(), heldKeys.end(), keyCode) == heldKeys.end()) {
		heldKeys.push_back(keyCode);
	}
}
void StartScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event) {
	heldKeys.erase(std::remove(heldKeys.begin(), heldKeys.end(), keyCode), heldKeys.end());
}
void StartScene::onKeyHold(float interval) {

	/*
	
		벡터에 연속 키입력이 쌓인다 따라서 키 입력 시 각 함수에
		연속 키 입력이 필요하지 않을 때는 status_keyUsing 을 true 처리해주고
		함수의 동작이 끝났을때 releaseKeyInput() 해주어야 한다.
	
	*/

	/* 방향키 UP 입력*/
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_UP_ARROW) != heldKeys.end()) {

		/* LOGIN 현재 포커싱된 상태 감소*/
		if (status_layer == STATUS::LOGIN) {
			changeLoginStatus(status_login, status_login - 1, false);
		}

		/* MENUSELECT 현재 포커싱된 상태 감소 */
		if (status_layer == STATUS::MENUSELECT) {
			changeMenuSelectStatus(status_menuSelect, status_menuSelect - 1, false);
		}

	}

	/* 방향키 DOWN 입력*/
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_DOWN_ARROW) != heldKeys.end()) {
	
		/* LOGIN 현재 포커싱된 상태 증가*/
		if (status_layer == STATUS::LOGIN) {
			changeLoginStatus(status_login, status_login + 1, false);
		}

		/* MENUSELECT 현재 포커싱된 상태 증가 */
		if (status_layer == STATUS::MENUSELECT) {
			changeMenuSelectStatus(status_menuSelect, status_menuSelect + 1, false);
		}

	}

	/* 방향키 RIGHT 입력*/
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_RIGHT_ARROW) != heldKeys.end()) {

	}

	/* 방향키 LEFT 입력*/
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_LEFT_ARROW) != heldKeys.end()) {

	}

	/* ENTER 키 입력 */
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_ENTER) != heldKeys.end()) {
		
		/* PRESSANYKEY -> LOGIN 으로 LAYER 전환 */
		if (status_layer == STATUS::PRESSANYKEY) {
			enterLogin();
		}

		/* LOGIN 현재 포커싱된 상태 변환*/
		if (status_layer == STATUS::LOGIN && status_login != LOGINSTATUS::LOGINFAIL) {
			changeLoginStatus(status_login, status_login + 1, true);
		}

		/* LOGIN 실패 상태 -> 로그인 상태*/
		if (status_layer == STATUS::LOGIN && status_login == LOGINSTATUS::LOGINFAIL) {
			returnFailToLoginStatus();
		}

		/* MENUSELECT 현재 포커싱된 상태 증가 */
		if (status_layer == STATUS::MENUSELECT) {
			changeMenuSelectStatus(status_menuSelect, status_menuSelect + 1, true);
		}

	}

	/* ESC 키 입력 */
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_ESCAPE) != heldKeys.end()) {

	}

	/* BACKSPACE 키 입력 */
	if (std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_BACKSPACE) != heldKeys.end()) {
		
	}
}

void StartScene::releaseKeyInput(float interval) {
	status_keyUsing = false;
}

/*

	마우스 리스너 관련 함수
	
*/

/* 마우스 클릭 시 */
void StartScene::onMouseDown(cocos2d::Event* event) {

}

/* 마우스 클릭 후 땠을때 */
void StartScene::onMouseUp(cocos2d::Event* event) {

}

/* 마우스 이동 시 - 마우스 포인터 변경 */
void StartScene::onMouseMove(cocos2d::Event* event) {
	/*

		마우스 이동을 추적해서 마우스스프라이트의 위치를 계속 바꾸어줌
	
	*/
	auto mouseEvent = static_cast<EventMouse*>(event);
	Vec2 movePoint = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
	cursor_sprite->setPosition(movePoint);
}

/* 마우스 스크롤 시 */
void StartScene::onMouseScroll(cocos2d::Event* event) {

}

/* Scene의 시작 init() 함수 정의 */
bool StartScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

#ifdef _DEBUG
	/* 디버그 시에는 로고화면 건너뛴다 */
	setup();
#else
	/* 로딩 및 로고화면 */
	showOpening();
#endif



	return true;
}

void StartScene::setup() {
#ifdef _DEBUG
	/* 멤버변수 초기화 */
	size_window = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
	cache = Director::getInstance()->getTextureCache();
	status_keyUsing = true;
	status_layer = STATUS::SHOWLOGO;
	heldKeys.clear();
#endif
	// 키 및 마우스, 터치 입력 오픈
	enableKeySetting();

	// 각 레이어 세팅
	setLayerBasicLayer();
	setLayerPressAnyKey();
	setLayerLogin();
	setLayerSignUp();
	setLayerMenuSelect();
	setLayerEditor();
	setLayerSettings();

	// 시작하면 보여줌
	showPressAnyKey();
}

/* 로고 화면 보여주기 */
void StartScene::showOpening() {

	/* 멤버변수 초기화 */
	size_window = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
	cache = Director::getInstance()->getTextureCache();
	status_keyUsing = true;
	status_layer = STATUS::SHOWLOGO;
	heldKeys.clear();

	/*
	
			시작하면 오프닝영상을 보여줌 opencv로 잘라 텍스쳐벡터로 시작

	*/

	/* 레이어 초기화 */
	layer_openingLogo = Layer::create();
	layer_openingLogo->setPosition(0, 0);
	layer_openingLogo->setContentSize(Size(size_window.width, size_window.height));
	this->addChild(layer_openingLogo);

	/* 사운드 재생 */
	AudioEngine::play2d(OPENING_SOUNDFILENAME, false);

	/* OPENCV 비디오 캡쳐 파일 열기 및 리딩 */
	video_capture.open(OPENING_FILENAME);
	if (!video_capture.isOpened()) {
#ifdef _DEBUG
		CCLOG("can't open video background..");
#endif
		return;
	}
	else {

		/* 캡쳐한 비디오의 1프레임 MAT 형식을 Texture2D 로 변환 */
		video_capture >> video_frame;
		opening_texture = new Texture2D();
		opening_texture->initWithData(video_frame.data,
			video_frame.elemSize() * video_frame.cols * video_frame.rows,
			Texture2D::PixelFormat::RGB888,
			video_frame.cols,
			video_frame.rows,
			Size(video_frame.cols, video_frame.rows));

		/* 오프팅 스프라이트 세팅 */
		opening_sprite = Sprite::create();
		opening_sprite->initWithTexture(opening_texture);
		opening_sprite->setContentSize(Size(size_window.width, size_window.height));
		opening_sprite->setPosition(size_window.width / 2, size_window.height / 2);

		/* 오프팅 스프라이트 재생 틱 스케쥴 */
		this->schedule(schedule_selector(StartScene::opening_tick), 0.03f);

		layer_openingLogo->addChild(opening_sprite);
	}


}

/* 오프닝 동영상 재생 */
void StartScene::opening_tick(float interval) {

	/* 다음 프레임을 비디오캡쳐에서 가져옴 */
	video_capture >> video_frame;
	if (!video_frame.empty()) {

		/* 다음 프레임의 MAT 파일로 텍스쳐를 업데이트 한다 */
		opening_texture->updateWithData(video_frame.data, 0, 0, video_frame.cols, video_frame.rows);
	}
	else {
		/*

			오프닝 동영상 재생 끝. 현재 레이어를 제거하고
			setup() 으로 넘어감.

		*/
		layer_openingLogo->removeChild(opening_sprite);
		this->removeChild(layer_openingLogo);
		this->unschedule(schedule_selector(StartScene::opening_tick));
		setup();
	}

}

/* 키 입력 및 마우스 세팅 */
void StartScene::enableKeySetting() {

	/* 키 입력 세팅 */
	this->schedule(schedule_selector(StartScene::onKeyHold));
	auto key_listener = EventListenerKeyboard::create();
	key_listener->onKeyPressed = CC_CALLBACK_2(StartScene::onKeyPressed, this);
	key_listener->onKeyReleased = CC_CALLBACK_2(StartScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(key_listener, this);

	/* 커서 이미지 세팅*/
	cursor_sprite = Sprite::create(CURSOR_FILENAME);
	this->addChild(cursor_sprite, 10);

	/* 마우스 입력 세팅 */
	auto Mouse = EventListenerMouse::create();
	Mouse->onMouseDown = CC_CALLBACK_1(StartScene::onMouseDown, this);
	Mouse->onMouseUp = CC_CALLBACK_1(StartScene::onMouseUp, this);
	Mouse->onMouseMove = CC_CALLBACK_1(StartScene::onMouseMove, this);
	Mouse->onMouseScroll = CC_CALLBACK_1(StartScene::onMouseScroll, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(Mouse, this);

}

/* 기본 레이어 세팅*/
void StartScene::setLayerBasicLayer() {
	/*

			배경 영상을 opencv 로 잘라 텍스쳐벡터로 메모리에 로딩.
			텍스쳐벡터를 한 프레임당 순회하며 교체함으로써 영상재생

			10~20초 정도의 영상 루프는 이 방법이 훨씬 효율적!
			그러나 1분넘어가는 영상을 한번에 이미지로드하면 컴퓨터가 죽는다

			무조건 10~20초 정도 루프영상만 사용할 것.

	*/

	video_capture.open(BACKGROUND_FILENAME);
	if (!video_capture.isOpened()) {
		CCLOG("can't open video background..");
		return;
	}
	else {
		video_capture >> video_frame;


#ifdef _DEBUG /* 디버그 로딩 카운트 출력 변수 */
		int fAllCount = video_capture.get(CV_CAP_PROP_FRAME_COUNT);
		int fCount = 1;
#endif


		while (!video_frame.empty()) {


#ifdef _DEBUG /* 디버그 로딩 카운트 출력 */
			CCLOG("load Background Texture2D : %d/%d", fCount++, fAllCount / 2);
#endif

			/* 사전에 미리 캡쳐한 프레임들을 모두 texture 화 하여 벡터에 저장 */
			Texture2D *tp_texture = new Texture2D();
			tp_texture->initWithData(video_frame.data,
				video_frame.elemSize() * video_frame.cols * video_frame.rows,
				Texture2D::PixelFormat::RGB888,
				video_frame.cols,
				video_frame.rows,
				Size(video_frame.cols, video_frame.rows));
			background_texture.push_back(tp_texture);
			video_capture >> video_frame;

		}
		
		/* 배경 스프라이트 초기화 및 세팅 */
		background_sprite = Sprite::create();
		background_iter = background_texture.begin();
		background_sprite->initWithTexture(*background_iter);
		background_sprite->setContentSize(size_window);
		background_sprite->setPosition(size_window.width / 2, size_window.height / 2);
		background_sprite->setOpacity(100);
		background_sprite->setVisible(false);

		/* 한 틱당 스케쥴 함수 호출해서 영상 texture 갱신 */
		this->schedule(schedule_selector(StartScene::background_tick));
		
		/* 기본 레이어에 배경 스프라이트 등록*/
		background_sprite->setVisible(false);
		this->addChild(background_sprite);

		/* 메인로고 스프라이트 세팅 설정 - 등장 애니메이션을 위한 서브레이어 사용(layer_basic) */
		layer_basic = Layer::create();
		layer_basic->setPosition(0, 0);
		layer_basic->setContentSize(size_window);
		layer_basic->setCascadeOpacityEnabled(true);
		layer_basic->setVisible(false);
		this->addChild(layer_basic);
		logo_sprite = Sprite::create(LOGOSPRITE_FILENAME);
		logo_sprite->setPosition(Point(size_window.width / 2, size_window.height / 16 * 11));
		logo_sprite->setCascadeOpacityEnabled(true);
		layer_basic->addChild(logo_sprite, 1);
		auto fo = FadeTo::create(0.2f, 100);
		auto ft = FadeTo::create(0.2f, 150);
		auto dt = DelayTime::create(0.4f);
		auto seq = Sequence::create(fo, ft, dt, nullptr);
		auto rep = RepeatForever::create(seq);
		logo_sprite->runAction(rep);

		/* 서브로고 파티클 설정 */
		cache->addImage(SUBLOGOPARTICLE_FILENAME);
		sublogo_particle = ParticleGalaxy::create();
		sublogo_particle->setTexture(cache->getTextureForKey(SUBLOGOPARTICLE_FILENAME));
		sublogo_particle->setPosVar(Point(150, 20));
		sublogo_particle->setStartSize(25);
		sublogo_particle->setEndSize(10);
		sublogo_particle->setOpacity(50);
		sublogo_particle->setDuration(-1);
		sublogo_particle->setStartColor(Color4F::BLUE);
		sublogo_particle->setEndColor(Color4F::RED);
		sublogo_particle->setPosition(size_window.width / 2, size_window.height / 2);
		sublogo_particle->setVisible(false);
		this->addChild(sublogo_particle);

		/* 서브로고 스프라이트 설정 */
		sublogo_sprite = Sprite::create(SUBLOGOSPRITE_FILENAME);
		sublogo_sprite->setPosition(Point(size_window.width / 2, size_window.height / 2));
		sublogo_sprite->setVisible(false);
		this->addChild(sublogo_sprite);

	}
	
	
}

/* 배경 동영상 재생*/
void StartScene::background_tick(float interval) {
	/* 텍스쳐를 끝까지 재생한 경우 다시 처음부터 갱신*/
	if (background_iter == background_texture.end()) {
		background_iter = background_texture.begin();
	}
	background_sprite->setTexture(*background_iter++);
}

/* Press Any Key 레이어 세팅*/
void StartScene::setLayerPressAnyKey() {

	/* 레이어 초기화 */
	layer_pressAnyKey = Layer::create();
	layer_pressAnyKey->setPosition(0, 0);
	layer_pressAnyKey->setContentSize(size_window);
	layer_pressAnyKey->setVisible(false);
	layer_pressAnyKey->setCascadeOpacityEnabled(true);
	this->addChild(layer_pressAnyKey);


	/* 라벨 초기화 및 세팅 */
	pressAnyKey_label = Label::createWithTTF(PRESSANYKEY_STRING, PRESSANYKEY_FONTS, PRESSANYKEY_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER);
	pressAnyKey_label->setPosition(Point(size_window.width / 2, size_window.height / 16 * 5));
	pressAnyKey_label->setCascadeOpacityEnabled(true);
	layer_pressAnyKey->addChild(pressAnyKey_label);
	auto fo = FadeOut::create(0.2f);
	auto ft = FadeTo::create(0.2f, 150);
	auto dt = DelayTime::create(0.4f);
	auto seq = Sequence::create(fo, ft, dt, nullptr);
	auto rep = RepeatForever::create(seq);
	pressAnyKey_label->runAction(rep);

}

/* Login 화면 레이어 세팅 */
void StartScene::setLayerLogin() {

	/* 레이어 초기화 */
	layer_login = Layer::create();
	layer_login->setPosition(0, 0);
	layer_login->setContentSize(size_window);
	layer_login->setVisible(false);
	layer_login->setCascadeOpacityEnabled(true);
	this->addChild(layer_login);

	/* 로그인 상태 초기화 */
	status_login = LOGINSTATUS::IDINPUT;

	/*
	
		스프라이트 초기화 및 설정
		스프라이트는 캐시에 미리 넣어놓고 키를 통해 불러온다
	
	*/

	/* 아이디 입력 스프라이트 세팅 - 가장 초기 ON 상태 */
	cache->addImage(LOGINEDITON_FILENAME);
	cache->addImage(LOGINEDITOFF_FILENAME);
	login_sprite_id = Sprite::createWithTexture(cache->getTextureForKey(LOGINEDITON_FILENAME));
	login_sprite_id->setPosition(size_window.width / 2, size_window.height / 32 * 11 + 30);
	login_sprite_id->setCascadeOpacityEnabled(true);
	layer_login->addChild(login_sprite_id);

	/* 아이디 입력 텍스트필드 세팅 */
	login_textfield_id = TextFieldTTF::textFieldWithPlaceHolder("I D",
		login_sprite_id->getContentSize(), TextHAlignment::CENTER, LOGIN_FONTS, LOGIN_FONTSIZE);
	login_textfield_id->setPosition(login_sprite_id->getPosition());
	login_textfield_id->setColorSpaceHolder(Color3B::WHITE);
	login_textfield_id->setDelegate(this);
	login_textfield_id->attachWithIME();
	layer_login->addChild(login_textfield_id);

	/* 패스워드 입력 스프라이트 */
	cache->addImage(LOGINEDITOFF_FILENAME);
	cache->addImage(LOGINEDITON_FILENAME);
	login_sprite_pw = Sprite::createWithTexture(cache->getTextureForKey(LOGINEDITOFF_FILENAME));
	login_sprite_pw->setPosition(size_window.width / 2, size_window.height / 32 * 11 - 30);
	login_sprite_pw->setCascadeOpacityEnabled(true);
	layer_login->addChild(login_sprite_pw);

	/* 패스워드 입력 텍스트필드 세팅 */
	login_textfield_pw = TextFieldTTF::textFieldWithPlaceHolder("PASSWORD",
		login_sprite_pw->getContentSize(), TextHAlignment::CENTER, LOGIN_FONTS, LOGIN_FONTSIZE);
	login_textfield_pw->setPosition(login_sprite_pw->getPosition());
	login_textfield_pw->setColorSpaceHolder(Color3B::WHITE);
	login_textfield_pw->setDelegate(this);
	login_textfield_pw->setSecureTextEntry(true);
	layer_login->addChild(login_textfield_pw);

	/* 로그인 버튼 스프라이트 */
	cache->addImage(LOGINLOGINOFF_FILENAME);
	cache->addImage(LOGINLOGINON_FILENAME);
	login_sprite_login = Sprite::createWithTexture(cache->getTextureForKey(LOGINLOGINOFF_FILENAME));
	login_sprite_login->setPosition(size_window.width / 2 - 75, size_window.height / 32 * 11 - 90);
	login_sprite_login->setCascadeOpacityEnabled(true);
	layer_login->addChild(login_sprite_login);

	/* 회원가입 버튼 스프라이트 */
	cache->addImage(LOGINSIGNUPOFF_FILENAME);
	cache->addImage(LOGINSIGNUPON_FILENAME);
	login_sprite_signup = Sprite::createWithTexture(cache->getTextureForKey(LOGINSIGNUPOFF_FILENAME));
	login_sprite_signup->setPosition(size_window.width / 2 + 75, size_window.height / 32 * 11 - 90);
	login_sprite_signup->setCascadeOpacityEnabled(true);
	layer_login->addChild(login_sprite_signup);

	/* 로그인 정보 없음 스프라이트 */
	cache->addImage(LOGINFAIL_FILENAME);
	login_sprite_fail = Sprite::createWithTexture(cache->getTextureForKey(LOGINFAIL_FILENAME));
	login_sprite_fail->setPosition(size_window.width / 2, size_window.height / 2);
	login_sprite_fail->setOpacity(0);
	layer_login->addChild(login_sprite_fail, 10);

}

/* 현재 로그인 레이어에서 포커싱 상태를 변경한다 */
void StartScene::changeLoginStatus(int from, int to, bool isEnterKey) {

	/* 
	
		Enter 키로 진입했는데 3 이나 4가 to 로 들어온다면 그 때는
		각각 로그인 진입하거나 회원가입 레이어로 넘어가야 한다.
		그것 말고는 to 로 들어온 값으로 현재 상태를 전환.
	
	*/

	/* 키 입력 방지 */
	_KEYUSING;

	CCLOG("changeLoginStatus %d - %d", from, to);

	/* 로그인 정보를 확인하고 메인 메뉴 진입 */
	if (isEnterKey && from == LOGINSTATUS::LOGINBTN) {
		std::string id = login_textfield_id->getString();
		std::string pw = login_textfield_pw->getString();
		if (DBManager::getInstance()->checkLoginInfo(id, pw) == DBManager::NOTOKEN) {
			/* 로그인 정보 유효하지 않음 */
			SimpleAudioEngine::getInstance()->playEffect(SOUND_LOGINFAIL);
			status_login = LOGINSTATUS::LOGINFAIL;
			login_sprite_fail->setOpacity(255);
			_KEYRELEASE(0.2f);
			return;
		}
		enterMenuSelect();
		return;
	}

	/* 회원가입 레이어 진입 */
	if (isEnterKey && from == LOGINSTATUS::SIGNUPBTN) {
		enterSignUp();
		_KEYRELEASE(0.2f);
		return;
	}

	/* 벗어난 인덱스 - -1, 4 처리*/
	if (to == LOGINSTATUS::PREVERR_LOGIN || to == LOGINSTATUS::NEXTERR_LOGIN) {
		_KEYRELEASE(0.2f);
		return;
	}

	/* 효과음 재생 */
	SimpleAudioEngine::getInstance()->playEffect(SOUND_CHANGELOGIN);

	/* status 변경 */
	status_login = to;

	/* 스프라이트 강조 효과 변경*/
	switch (from) {
	/* 현재 포커싱 받고 있는 스프라이트 강조 해제 */
	case LOGINSTATUS::IDINPUT:
		login_sprite_id->setTexture(cache->getTextureForKey(LOGINEDITOFF_FILENAME));
		login_textfield_id->detachWithIME();
		break;
	case LOGINSTATUS::PWINPUT:
		login_sprite_pw->setTexture(cache->getTextureForKey(LOGINEDITOFF_FILENAME));
		login_textfield_pw->detachWithIME();
		break;
	case LOGINSTATUS::LOGINBTN:
		login_sprite_login->setTexture(cache->getTextureForKey(LOGINLOGINOFF_FILENAME));
		break;
	case LOGINSTATUS::SIGNUPBTN:
		login_sprite_signup->setTexture(cache->getTextureForKey(LOGINSIGNUPOFF_FILENAME));
		break;
	default:
		break;
	}

	switch (to) {
	/* 현재 포커싱 받고 있는 스프라이트 강조 */
	case LOGINSTATUS::IDINPUT:
		login_sprite_id->setTexture(cache->getTextureForKey(LOGINEDITON_FILENAME));
		login_textfield_id->attachWithIME();
		break;
	case LOGINSTATUS::PWINPUT:
		login_sprite_pw->setTexture(cache->getTextureForKey(LOGINEDITON_FILENAME));
		login_textfield_pw->attachWithIME();
		break;
	case LOGINSTATUS::LOGINBTN:
		login_sprite_login->setTexture(cache->getTextureForKey(LOGINLOGINON_FILENAME));
		break;
	case LOGINSTATUS::SIGNUPBTN:
		login_sprite_signup->setTexture(cache->getTextureForKey(LOGINSIGNUPON_FILENAME));
		break;
	default:
		break;
	}

	/* 키 입력 방지 해제*/
	_KEYRELEASE(0.2f);

}

/* 로그인 실패 팝업에서 다시 로그인 상태로 돌아간다 */
void StartScene::returnFailToLoginStatus() {
	_KEYUSING;
	SimpleAudioEngine::getInstance()->playEffect(SOUND_CHANGELOGIN);
	status_login = LOGINSTATUS::LOGINBTN;
	login_sprite_fail->setOpacity(0);
	_KEYRELEASE(0.2f);
}

/* 회원 가입 페이지 레이어 세팅 */
void StartScene::setLayerSignUp() {

}

/* 메뉴 선택 레이어 세팅 */
void StartScene::setLayerMenuSelect() {

	/*
	
		메뉴 목록
		1. 싱글플레이
		2. 멀티플레이
		3. 세팅
		4. 에디터
		5. 나가기
	
	*/

	/* 레이어 초기화 */
	layer_menuSelect = Layer::create();
	layer_menuSelect->setPosition(0, 0);
	layer_menuSelect->setContentSize(size_window);
	layer_menuSelect->setVisible(false);
	layer_menuSelect->setCascadeOpacityEnabled(true);
	this->addChild(layer_menuSelect);

	/* 메뉴 선택 상태 초기화 */
	status_menuSelect = MENUSELECTSTATUS::SINGLEPLAY;

	/* 
	
		라벨 초기화 및 세팅
		
	*/

	int i = 0;
	while (i < MENUSELECT_MENUSIZE) {
		menuSelect_label[i] = Label::createWithTTF(MENUSELECT_STR[i], MENUSELECT_FONTS, MENUSELECT_FONTSIZE,
			Size(), TextHAlignment::CENTER, TextVAlignment::CENTER);
		menuSelect_label[i]->setPosition(size_window.width / 2, size_window.height / 8 * 3 - (i * MENUSELECT_DIST));
		menuSelect_label[i]->setCascadeOpacityEnabled(true);
		menuSelect_label[i]->enableGlow(Color4B(0, 0, 0, 0));
		menuSelect_label[i]->setOpacity(100);
		layer_menuSelect->addChild(menuSelect_label[i]);
		i++;
	}

	/* 첫 싱글플레이 선택된 상태 액션 생성 */
	menuSelect_label[MENUSELECTSTATUS::SINGLEPLAY]->enableGlow(MENUSELECT_FOCUSEDCOLOR);
	auto fi = FadeIn::create(0.1f);
	auto ft = FadeTo::create(0.3f, 100);
	auto dt = DelayTime::create(0.4f);
	auto seq = Sequence::create(fi, ft, dt, nullptr);
	auto rep = RepeatForever::create(seq);
	menuSelect_label[MENUSELECTSTATUS::SINGLEPLAY]->runAction(rep);

}

/* 현재 메뉴선택 레이어에서 포커싱 상태를 변경한다 */
void StartScene::changeMenuSelectStatus(int from, int to, bool isEnterKey) {

	/*
	
		ENTER 키로 들어왔을때는 from 으로 들어온 상태로 진입한다
		그것이 아닐 경우 상태 변환 해줌
	
	*/

	/* 키 입력 방지 */
	_KEYUSING;

	/* 엔터키가 들어왔을 때 각 진입함수를 실행한다. */
	if (isEnterKey) {
		switch (from) {
		case MENUSELECTSTATUS::SINGLEPLAY:
			/* 싱글플레이 씬 진입 함수 실행 */
			goMusicSelectScene();
			break;
		case MENUSELECTSTATUS::MULTIPLAY:
			/* 멀티플레이 씬 진입 함수 실행 */
			break;
		case MENUSELECTSTATUS::SETTING:
			/* 세팅 레이어 진입 함수 실행 */
			break;
		case MENUSELECTSTATUS::EDITOR:
			/* 에디터 레이어 진입 함수 실행 */
			break;
		case MENUSELECTSTATUS::EXIT:
			/* EXIT 함수 실행*/
			break;
		default:
			break;
		}
		return;
	}

	/* 엔터키가 아닐때 범위 벗어날 경우 */
	if (to == MENUSELECTSTATUS::SINGLEPLAY - 1 || to == MENUSELECTSTATUS::EXIT + 1) {
		_KEYRELEASE(0.1f);
		return;
	}

	/* 효과음 재생 */
	SimpleAudioEngine::getInstance()->playEffect(SOUND_CHANGEMENUSELECT);

	/* 선택 상태 바꿈 */
	status_menuSelect = to;

	/* 라벨 변화 from 해제 */
	menuSelect_label[from]->enableGlow(Color4B(0, 0, 0, 0));
	menuSelect_label[from]->stopAllActions();
	menuSelect_label[from]->setOpacity(100);

	/* 라벨 변화 to 설정 */
	menuSelect_label[to]->enableGlow(MENUSELECT_FOCUSEDCOLOR);
	auto fi = FadeIn::create(0.1f);
	auto ft = FadeTo::create(0.3f, 100);
	auto dt = DelayTime::create(0.4f);
	auto seq = Sequence::create(fi, ft, dt, nullptr);
	auto rep = RepeatForever::create(seq);
	menuSelect_label[to]->runAction(rep);

	/* 키 제한 해제 */
	_KEYRELEASE(0.1f);

}

/* 에디터 레이어 세팅 */
void StartScene::setLayerEditor() {

}

/* 설정 레이어 세팅*/
void StartScene::setLayerSettings() {

}

/* PressAnyKey 진입*/
void StartScene::showPressAnyKey() {
	/*
	
		모든 setup() 동작이 완료되고나서 call 됨
		Loading Label을 끈 다음 배경과 showPressAnyKey layer를 띄워준다

		layer_status 변경
		키 입력방지 해제 - 지금부터 키 입력 가능
	
	*/

	/* 현재 씬 레이어 상태 변경*/
	status_layer = STATUS::PRESSANYKEY;
	
	/* setVisible = true */
	sublogo_sprite->setVisible(true);
	sublogo_particle->setVisible(false);
	background_sprite->setVisible(true);
	layer_pressAnyKey->setVisible(true);
	layer_basic->setVisible(true);

	/* 등장과 함께 배경음악 재생 */
	this->scheduleOnce(schedule_selector(StartScene::startBGM), 5.0f);

	/* 모든 애니메이션이 끝나고 키 입력 해제 */
	this->scheduleOnce(schedule_selector(StartScene::releaseKeyInput), 7.5f);

	/* 전기 효과음 재생 */
	SimpleAudioEngine::getInstance()->playEffect(SOUND_ELECTIC, true);

	/* 
	
		등장 씬 애니메이션 설정 및 재생 
		
	*/

	/* 전기 깜빡임 */
	auto fi = FadeIn::create(1.0f);
	auto dt4 = DelayTime::create(0.5f);
	auto ft = FadeTo::create(0.0f, 50);
	auto dt = DelayTime::create(0.6f);
	auto fi2 = FadeIn::create(0.0f);
	auto dt2 = DelayTime::create(0.3f);
	auto ft2 = FadeTo::create(0.0f, 50);
	auto dt7 = DelayTime::create(0.6f);
	auto fi3 = FadeIn::create(0.0f);
	auto seq = Sequence::create(fi, dt4, ft, dt, fi2, dt2, ft2, dt7, fi3, nullptr);
	sublogo_sprite->setOpacity(0);
	sublogo_sprite->runAction(seq);

	dt4 = DelayTime::create(7.5f);
	fi3 = FadeIn::create(2.0f);
	seq = Sequence::create(dt4, fi3, nullptr);
	layer_basic->setOpacity(0);
	layer_basic->runAction(seq);

	auto dt5 = DelayTime::create(6.0f);
	auto fi4 = FadeIn::create(2.0f);
	auto seq2 = Sequence::create(dt5, fi4, nullptr);
	sublogo_sprite->runAction(seq2);
	dt5 = DelayTime::create(6.0f);
	fi4 = FadeIn::create(2.0f);
	seq2 = Sequence::create(dt5, fi4, nullptr);
	background_sprite->setOpacity(0);
	background_sprite->runAction(seq2);

	auto dt6 = DelayTime::create(7.5f);
	fi4 = FadeIn::create(2.0f);
	auto seq3 = Sequence::create(dt6, fi4, nullptr);
	sublogo_particle->setOpacity(0);
	sublogo_particle->runAction(seq3);
	dt6 = DelayTime::create(7.5f);
	fi4 = FadeIn::create(2.0f);
	seq3 = Sequence::create(dt6, fi4, nullptr);
	layer_pressAnyKey->setOpacity(0);
	layer_pressAnyKey->runAction(seq3);

}

/* 메인 BGM재생*/
void StartScene::startBGM(float interval) {
	SimpleAudioEngine::getInstance()->stopAllEffects();
	AudioEngine::play2d(BGMMAIN_FILENAME, true, BGMMAIN_VOLUME);
}

/* Login 레이어 진입 */
void StartScene::enterLogin() {

	/* 키 입력 방지 */
	_KEYUSING;

	/* 기존 레이어 없앰(PressAnyKey) */
	layer_pressAnyKey->setVisible(false);

	/* 현재 씬 레이어 상태 변경*/
	status_layer = STATUS::LOGIN;

	/* setVisible = true */
	layer_login->setVisible(true);

	/* 등장 사운드 재생 */
	SimpleAudioEngine::getInstance()->playEffect(SOUND_CHANGELAYER);

	/* layer_login 보이기 */
	layer_login->setOpacity(0);
	auto fi = FadeIn::create(0.5f);
	layer_login->runAction(fi);

	/* 등장 애니메이션 */




	/* 애니메이션 끝나고 반드시 키 해제할것. */
	_KEYRELEASE(0.2f);
}

/* 메뉴 선택 레이어 진입 */
void StartScene::enterMenuSelect() {

	/* 기존 로그인 레이어 제거 */
	login_textfield_pw->setString("");
	layer_login->setVisible(false);

	/* 현재 씬 레이어 상태 변경*/
	status_layer = STATUS::MENUSELECT;

	/* 등장 사운드 재생 */
	SimpleAudioEngine::getInstance()->playEffect(SOUND_CHANGELAYER);

	/* setVisible = true */
	layer_menuSelect->setVisible(true);

	/* 메뉴 진입 애니메이션 */




	/* 애니메이션 끝나고 반드시 키 해제할것. */
	_KEYRELEASE(0.2f);

}

/* 계정 만들기 레이어 진입 */
void StartScene::enterSignUp() {

}

void StartScene::goMusicSelectScene() {
	
	SimpleAudioEngine::getInstance()->playEffect(SOUND_CHANGELAYER);
	AudioEngine::stopAll();
	auto musicSelectScene = MusicSelectScene::createScene(background_texture);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, musicSelectScene));
}