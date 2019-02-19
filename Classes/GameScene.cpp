#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "AudioEngine.h"
#include "AppDelegate.h"

#include <sys/stat.h>
#include <algorithm>
#include <chrono>
#include <fmod.hpp>

#define KEY_USINGSET if (status_keyUsing) return; status_keyUsing = true

USING_NS_CC;
using namespace CocosDenshion;
using namespace experimental;

std::string mn; std::string bn;  float sp; bool ap;

//scene 생성
Scene* GameScene::createScene(std::string dirs, std::string bmsName, float speed, bool autoPlay)
{
	mn = dirs; sp = speed; bn = bmsName; ap = autoPlay;
	CCLOG("GameScene::createScene()..");
	auto scene = Scene::create();
	auto layer = GameScene::create();
	scene->addChild(layer);
	CCLOG("GameScene::createScene()..finished!");
	return scene;
}

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

//key Listener
void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event *event) {

	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_S:
		CCLOG("keyS : Pressed");
		if (status_autoPlay) { return; }
		operateKeyEffect(KEY::S);
		break;
	case EventKeyboard::KeyCode::KEY_D:
		CCLOG("keyD : Pressed");
		if (status_autoPlay) { return; }
		operateKeyEffect(KEY::D);
		break;
	case EventKeyboard::KeyCode::KEY_F:
		CCLOG("keyF : Pressed");
		if (status_autoPlay) { return; }
		operateKeyEffect(KEY::F);
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		CCLOG("keySpace : Pressed");
		if (status_autoPlay) { return; }
		operateKeyEffect(KEY::SPACE);
		break;
	case EventKeyboard::KeyCode::KEY_J:
		CCLOG("keyJ : Pressed");
		if (status_autoPlay) { return; }
		operateKeyEffect(KEY::J);
		break;
	case EventKeyboard::KeyCode::KEY_K:
		CCLOG("keyK : Pressed");
		if (status_autoPlay) { return; }
		operateKeyEffect(KEY::K);
		break;
	case EventKeyboard::KeyCode::KEY_L:
		CCLOG("keyL : Pressed");
		if (status_autoPlay) { return; }
		operateKeyEffect(KEY::L);
		break;
	case EventKeyboard::KeyCode::KEY_ENTER:
		if (status_playing == PLAY_STATUS::NOSTART) {
			CCLOG("game start");
			gameStart();
		}
		else if (status_playing == PLAY_STATUS::PAUSED) {
			operatePauseMenu();
		}
		break;
	case EventKeyboard::KeyCode::KEY_1:
		changeSpeed(false);
		break;
	case EventKeyboard::KeyCode::KEY_2:
		changeSpeed(true);
		break;
	case EventKeyboard::KeyCode::KEY_ESCAPE:
		operateESC();
		break;
	default:
		break;
	}

}
void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event) {

	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_S:
		endKeyEffect(0.0f, KEY::S);
		break;
	case EventKeyboard::KeyCode::KEY_D:
		endKeyEffect(0.0f, KEY::D);
		break;
	case EventKeyboard::KeyCode::KEY_F:
		endKeyEffect(0.0f, KEY::F);
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		endKeyEffect(0.0f, KEY::SPACE);
		break;
	case EventKeyboard::KeyCode::KEY_J:
		endKeyEffect(0.0f, KEY::J);
		break;
	case EventKeyboard::KeyCode::KEY_K:
		endKeyEffect(0.0f, KEY::K);
		break;
	case EventKeyboard::KeyCode::KEY_L:
		endKeyEffect(0.0f, KEY::L);
		break;
	default:
		break;
	}

}

void GameScene::releaseKeyInput(float dt) {
	status_keyUsing = false;
}

//Scene의 시작 init() 함수 정의
bool GameScene::init()
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

void GameScene::onEnterTransitionDidFinish() {

	// 로딩화면 보여주기
	showLoadingState();

	// 기본정보 가져오기
	initData();
}

// 로딩창 설정
void GameScene::showLoadingState() {
	CCLOG("GameScene::initData()...");
	//변수 초기화 및 곡 기본 상태정보 불러오기
	size_window = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
	cache = TextureCache::getInstance();
	layer_notes = Layer::create();
	FMOD::System_Create(&sound_system);
	sound_system->setSoftwareChannels(3000);
	sound_system->init(320, FMOD_INIT_NORMAL, NULL);
	status_keyUsing = false;
	status_bmsName = bn;
	CCLOG("########## bmsFile : %s", status_bmsName.c_str());
	status_dirs = mn;
	CCLOG("########## bmsDirs : %s", status_dirs.c_str());
	status_speed = sp;
	CCLOG("########## speed : %lf", status_speed);
	status_autoPlay = ap;
	CCLOG("########## autoPlay : %d", status_autoPlay);
	status_playing = PLAY_STATUS::NOSTART;

	// 화면 UI 생성
	setUiInfo();

	//키 리스너 생성.
	enableKeySetting();

	// Effect 사운드 로딩
	SimpleAudioEngine::getInstance()->preloadEffect(SOUND_GO.c_str());
	SimpleAudioEngine::getInstance()->preloadEffect(SOUND_READY.c_str());
	SimpleAudioEngine::getInstance()->preloadEffect(SOUND_CHANGESPEED.c_str());

	// 스프라이트 로딩
	cache->addImage(UI_SPRITE_NOATEBACKGROUND);
	cache->addImage(UI_SPRITE_SMALLNOTE);
	cache->addImage(UI_SPRITE_LARGENOTE);
	cache->addImage(UI_SPRITE_MIDNOTE);
}

// 키 세팅
void GameScene::enableKeySetting() {
	//키 리스너 생성.
	auto key_listener = EventListenerKeyboard::create();
	key_listener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
	key_listener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(key_listener, this);
}

// 시작 데이터 불러오기
void GameScene::initData() {
	
	/* 마디 변경 채널은 미리 1(4/4 박자)로 초기화 해놓는다. */
	std::fill_n(notes_barLength, 1000, 1);

	/* BMS 로드 */
	BMSManager::getInstance()->readBms(status_dirs, status_bmsName, status_bpm,
		str_bpmValue, str_wavFile, str_bmpFile, str_stopValue, &notes, notes_barLength);

	// 노트 생성
	setNotes();

	// 기타 라벨 생성
	label_time_music = Label::createWithSystemFont("music_time : 0", "consolas", 20);
	label_speed = Label::createWithSystemFont("music_speed : " + std::to_string(status_speed), "consolas", 20);
	label_bpm = Label::createWithSystemFont("music_bpm : " + std::to_string(status_bpm), "consolas", 20);
	label_bar = Label::createWithSystemFont("music_bar : " + std::to_string(bar_iter_latest), "consolas", 20);

	// 라벨 설정
	label_time_music->setPosition(size_window.width / 2, size_window.height / 2);
	this->addChild(label_time_music);

	label_speed->setPosition(size_window.width / 2, size_window.height / 2 - 20);
	this->addChild(label_speed);

	label_bpm->setPosition(size_window.width / 2, size_window.height / 2 - 40);
	this->addChild(label_bpm);

	label_bar->setPosition(size_window.width / 2, size_window.height / 2 - 60);
	this->addChild(label_bar);

	/* 레디 사운드 재생 */
	SimpleAudioEngine::getInstance()->playEffect(SOUND_READY.c_str());

	CCLOG("GameScene::initData() - finished!");
}

// 노트생성
void GameScene::setNotes() {
	CCLOG("GameScene::setNotes()..");

	/* layer_notes 기본 설정 */
	layer_notes->setPosition(Point(LAYER_POSITIONX, LAYER_POSITIONY));
	layer_notes->setContentSize(Size(LAYER_WIDTH, LAYER_HEIGHT));

	/* 각 노트의 Y축 위치 선정하는 오프셋 설정 */
	first_bpm = status_bpm;
	offsetPosY = (status_speed * bar_length);
	offsetBar = 1.0;

	/* Batch Drawing */
	SpriteBatchNode *batchSmallNote = SpriteBatchNode::create(UI_SPRITE_SMALLNOTE);
	this->addChild(batchSmallNote);
	SpriteBatchNode *batchLargeNote = SpriteBatchNode::create(UI_SPRITE_LARGENOTE);
	this->addChild(batchLargeNote);
	SpriteBatchNode *batchMidNote = SpriteBatchNode::create(UI_SPRITE_MIDNOTE);
	this->addChild(batchMidNote);
	SpriteBatchNode *batchBar = SpriteBatchNode::create(UI_SPRITE_BAR);
	this->addChild(batchBar);

	/* 현재 노트 iterator 초기화 */
	note_iter_latest = notes.begin();

#pragma region 백그라운드 이미지 붙이기

	/* 백그라운드 선 이미지 붙이기 */
	note_sprite_background = Sprite::createWithTexture(cache->getTextureForKey(UI_SPRITE_NOATEBACKGROUND));
	note_sprite_background->setAnchorPoint(Point(0, 0));
	note_sprite_background->setPosition(Point(0, 0));
	note_sprite_background->setOpacity(OPACITY_NOTE_SPRITE_BACKGROUND);
	layer_notes->addChild(note_sprite_background, ZORDER_NOTEBACKGROUND);

#pragma endregion

#pragma region 프레스 스프라이트 설정

	/* 프레스 스프라이트 설정 */
	int i = 1;
	while (i < 10) {
		cache->addImage(UI_SPRITE_KEYPRESSFILE[i]);
		keyPressed_sprite[i] = Sprite::createWithTexture(cache->getTextureForKey(UI_SPRITE_KEYPRESSFILE[i]));
		keyPressed_sprite[i]->setAnchorPoint(Point(0, 0));
		keyPressed_sprite[i]->setPosition(Point(-2, -LAYER_POSITIONY));
		keyPressed_sprite[i]->setVisible(false);
		note_sprite_background->addChild(keyPressed_sprite[i]);
		i++;
	}

#pragma endregion

#pragma region 폭탄 이펙트

	/* 폭탄 이펙트 */
	i = 1;
	cache->addImage(UI_SPRITE_BOMB);
	while (i < 10) {
		bomb_sprite[i] = Sprite::createWithTexture(cache->getTextureForKey(UI_SPRITE_BOMB),
			Rect(0, 0, UI_SPRITE_ANIM_BOMB_WIDTH - 20, UI_SPRITE_ANIM_BOMB_HEIGHT));
		bomb_sprite[i]->setPosition(Point(NOTE::NOTE_POS_X[i], -10));

		/* 검은 배경 없애기 위한 SCREEN 블렌딩 효과 */
		BlendFunc blend = { GL_ONE_MINUS_DST_COLOR, GL_ONE };
		bomb_sprite[i]->setBlendFunc(blend);
		bomb_sprite[i]->setOpacity(0);

		note_sprite_background->addChild(bomb_sprite[i]);
		i++;
	}

#pragma endregion

#pragma region 이퀄라이저 이펙트

	/* 이퀄라이저 이펙트 */
	i = 0;
	cache->addImage(UI_SPRITE_EQUALIZER);
	equalizer_sprite = Sprite::createWithTexture(cache->getTextureForKey(UI_SPRITE_EQUALIZER));
	equalizer_sprite->setAnchorPoint(Point(0, 0));
	equalizer_sprite->setPosition(Point(0, 0));
	BlendFunc blend = { GL_ONE_MINUS_DST_COLOR, GL_ONE };
	equalizer_sprite->setBlendFunc(blend);
	note_sprite_background->addChild(equalizer_sprite);
	auto fi = FadeIn::create(0.3f);
	auto ft = FadeTo::create(0.3f, 50);
	auto seq = Sequence::create(fi, ft, nullptr);
	auto rep = RepeatForever::create(seq);
	equalizer_sprite->runAction(rep);

#pragma endregion

#pragma region 콤보 라벨 설정

	/* 콤보 라벨 설정 */
	combo_label = Label::createWithTTF("", COMBO_FONT, COMBO_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER);
	combo_label->setPosition(
		note_sprite_background->getContentSize().width / 2,
		note_sprite_background->getContentSize().height / 8 * 5
	);
	combo_label->setOpacity(0);
	combo_label->enableOutline(Color4B::BLACK, 4);
	note_sprite_background->addChild(combo_label);
	auto rgbRed = TintTo::create(0.05f, Color3B(255, 000, 051));
	auto rgbGreen = TintTo::create(0.05f, Color3B(000, 204, 102));
	auto rgbBlue = TintTo::create(0.05f, Color3B(051, 153, 255));
	auto rgbGray = TintTo::create(0.05f, Color3B(204, 204, 204));
	seq = Sequence::create(rgbRed, rgbGreen, rgbBlue, rgbGray, nullptr);
	rep = RepeatForever::create(seq);
	combo_label->runAction(rep);

	combo_label_ui = Label::createWithTTF("COMBO", COMBO_UI_FONT, COMBO_UI_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER);
	combo_label_ui->setPosition(
		note_sprite_background->getContentSize().width / 2,
		(note_sprite_background->getContentSize().height / 4 * 3) + 50
	);
	combo_label_ui->setOpacity(0);
	combo_label_ui->enableOutline(Color4B::BLACK, 4);
	note_sprite_background->addChild(combo_label_ui);

	judge_label = Label::createWithTTF("", JUDGE_FONT, JUDGE_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER);
	judge_label->setPosition(
		note_sprite_background->getContentSize().width / 2,
		note_sprite_background->getContentSize().height / 5
	);
	judge_label->setOpacity(0);
	judge_label->enableOutline(Color4B::BLACK, 4);
	note_sprite_background->addChild(judge_label);
	rgbRed = TintTo::create(0.05f, Color3B(255, 000, 051));
	rgbGreen = TintTo::create(0.05f, Color3B(000, 204, 102));
	rgbBlue = TintTo::create(0.05f, Color3B(051, 153, 255));
	rgbGray = TintTo::create(0.05f, Color3B(204, 204, 204));
	seq = Sequence::create(rgbRed, rgbGreen, rgbBlue, rgbGray, nullptr);
	rep = RepeatForever::create(seq);
	judge_label->runAction(rep);

	judge_time_label = Label::createWithTTF("", JUDGE_FONT, JUDGE_TIME_FONTSIZE,
		Size(), TextHAlignment::CENTER, TextVAlignment::CENTER);
	judge_time_label->setPosition(
		note_sprite_background->getContentSize().width / 2,
		note_sprite_background->getContentSize().height / 5 + 50
	);
	judge_time_label->setOpacity(0);
	judge_time_label->enableOutline(Color4B::BLACK, 4);
	note_sprite_background->addChild(judge_time_label);

#pragma endregion
	 
#pragma region 사운드 로딩

	/* BMS 파일 사운드파일 유효성 검사*/
	std::string sf = str_wavFile[2];
	std::string fp = "bms/" + status_dirs + "/" + sf;
	std::string chStr;
	bool chPathFlag = false;
	struct stat buffer;
	if (stat(fp.c_str(), &buffer) != 0) {
		/* 해당 사운드 파일이 존재하지 않을 때 */
		CCLOG("No SoundFile : ");
		/*

			BMS 중 실제 파일은 .ogg 인데 .wav 로 bms 가 파싱되어있는 경우가 있다.
			이때 파일이 존재하지 않으면 확장자를 바꾸어서 사운드파일을 로드한다.

		*/
		if (fp.find(".ogg", 0, 4) != std::string::npos) {
			/* 현재 확장자가 .ogg 인 경우 전부 .wav 로 바꾸어봄 */
			chStr = ".wav";
		}
		else {
			/* 현재 확장자가 .wav 인 경우 전부 .ogg 로 바꾸어봄 */
			chStr = ".ogg";
		}
		chPathFlag = true;
	}

	// 사운드 벡터 공간 할당, 사운드 로딩
	v_sound_sound.resize(STR_CHANNELSIZE);
	for (int i = 0; i < v_sound_sound.size(); i++) {
		std::string soundFile = str_wavFile[i];
		std::string filePath = "bms/" + status_dirs + "/" + soundFile;
		if (chPathFlag) {
			/* 기존 BMS 경로가 유효하지 않으므로 확장자를 바꾸어 로드해본다. */
			filePath.pop_back(); filePath.pop_back();
			filePath.pop_back(); filePath.pop_back();
			filePath += chStr;

#ifdef _DEBUG
			CCLOG("Path 확장자 변경 %s", chStr.c_str());
#endif

		}
		FMOD_RESULT ret = sound_system->createSound(filePath.c_str(), FMOD_DEFAULT, 0, &v_sound_sound[i]);
		v_sound_sound[i]->setMode(FMOD_DEFAULT);
		sound_channel[i]->setVolume(1.0f);

#ifdef _DEBUG
		CCLOG("load sound(createSound) - %s, %d", filePath.c_str(), i);
#endif

	}

#pragma endregion

#pragma region 마디 및 노트 세팅

	/* 마디 길이 제대로 저장? */
	i = 0;
	while (i < 1000) {
		if (notes_barLength[i] == 0) {
			notes_barLength[i] = 1;
		}
		CCLOG("notes_barLength[%d] = %lf", i, notes_barLength[i]);
		i++;
	}

	/* 노트 스프라이트 및 시간 세팅 */
	sort(notes.begin(), notes.end());

	i = 1;
	double cur_bpm = first_bpm;
	double cur_time = STARTEMPTYTIME;
	double pre_beat = 0.0f;
	double cur_pos = cur_time * cur_bpm * status_speed;
	double bar_stack = cur_pos;
	//double cur_pos = 0;
	int cur_bar = 0;
	int last_bar_pos = 0;
	for (auto it = notes.begin(); it != notes.end(); it++) {

		auto cur_note = it;

		/* 스프라이트 세팅 */
		if (cur_note->note_key == 4) {
			cur_note->sprite->initWithTexture(batchMidNote->getTexture());
		}
		else if (cur_note->note_key % 2 == 0) {
			cur_note->sprite->initWithTexture(batchSmallNote->getTexture());
		}
		else {
			cur_note->sprite->initWithTexture(batchLargeNote->getTexture());
		}

		/* x, y 축 위치 결정 */
		double notePosX = NOTE::NOTE_POS_X[cur_note->note_key];
		layer_notes->addChild(cur_note->sprite, ZORDER_NOTES);

		cur_note->sprite->setVisible(false);

		/* 노트 타임 세팅 */
		/*
	
			이전 비트의 시간에다가 계속 더해가는 방식
			이전 비트와의 차이를 통해 현재 비트에 도달할때까지
			bpm 과 bar_length 를 이용하여 시간을 구해준다.

		*/

		double cur_beat = cur_note->note_beat;

		/*

			이전 비트의 마디에서 현재 비트까지 각 마디를 따로(마디길이가 다를 수 있으므로)
			구분하여 합산하는 방식으로 시간 차를 구한다.

		*/

		/* 각 루프(마디)당 구해야하는 구간 */
		double min, max;

		cur_bar = (int)pre_beat;
		double time_diff_sum = 0.0f;
		double pos_diff_sum = 0.0f;
		while (cur_bar <= cur_beat) {

			/* min 설정 */
			min = (cur_bar == (int)pre_beat) ? pre_beat : cur_bar;

			/* max 설정 */
			max = (cur_bar == (int)cur_beat) ? cur_beat : cur_bar + 1;

			/* 정수 비트 마디 세팅 */
			if (min == cur_bar) {
				notes_barPosition[cur_bar] = bar_stack;
				last_bar_pos = cur_bar;
				CCLOG("BAR : posy : %lf / (beat : %lf)", notes_barPosition[cur_bar], cur_bar);
			}

			/* 현재 마디에서 비트 차이 */
			double beat_diff = (double)max - min;

			/* 1비트당 걸리는 초 */
			double secPerBeat = (60.0 / cur_bpm);

			/* 현재 마디의 길이 - 기본(1) 일때 4/4 박자 */
			double cur_barLength = 4.0 * notes_barLength[cur_bar];

			/* 비트 차이만큼 의 시간차이를 계산 */
			double time_diff = beat_diff * secPerBeat * cur_barLength;
			//CCLOG("time_diff : %lf / beat_diff : %lf / secPerBeat : %lf / cur_barLength : %lf", time_diff, beat_diff, secPerBeat, cur_barLength);

			/* 이전 시간에 합산 */
			time_diff_sum += time_diff;
			pos_diff_sum += time_diff * status_speed * cur_bpm;
			bar_stack += time_diff * status_speed * cur_bpm;

			cur_bar += 1;
		}

		/* 현재 노트의 시간 입력 */
		cur_time += time_diff_sum;
		cur_note->note_time = cur_time;

		/* 시간 변화에 따른 거리 설정 */
		cur_pos += pos_diff_sum;
		cur_note->note_posy = cur_pos;

		CCLOG("NOTE : posy %s : %lf / (beat : %lf)", NOTE::STR_KEY[cur_note->note_key].c_str(), cur_note->note_posy, cur_note->note_beat);

		/* 거리 세팅 */
		cur_note->sprite->setPosition(Point(notePosX, cur_note->note_posy));

		/* 현재 노트가 BPM Change나 STOP 일때 status 변경 */
		switch (cur_note->note_key) {
		case NOTE::KEY_STOP:
			/*

				STOP 값은 val / 192 * bar_length 로 실제 시간을 구해주어야 함

			*/
			cur_time += cur_note->note_channel / 192 * str_stopValue[(int)cur_beat];
			break;
		case NOTE::KEY_BPM:
			cur_bpm = cur_note->note_channel;
			break;
		case NOTE::KEY_BPMEXTEND:
			cur_bpm = str_bpmValue[cur_note->note_channel];
			break;
		default:
			break;
		}

		pre_beat = cur_beat;
	}

#ifdef _DEBUG
	i = 1;
	for (NOTE::Note cur_note : notes) {
		CCLOG("%d 's note : %d, %lf, (%lf, %lf)", i++, cur_note.note_key, cur_note.note_time,
			cur_note.sprite->getPosition().x, cur_note.sprite->getPosition().y);
	}
#endif // _DEBUG

	/* 마디 길이정보 곡 종료 이후 마디 위치 설정 */
	double diff = notes_barPosition[last_bar_pos] - notes_barPosition[last_bar_pos - 1];
	i = last_bar_pos + 1;
	while (i < 1000) {
		notes_barPosition[i] = notes_barPosition[i - 1] + diff;
		i++;
	}

	/* 마디 스프라이트 세팅 */
	i = 0;
	while (i < 1000) {
		/* 스프라이트 초기화 */
		bars_sprite[i] = Sprite::createWithTexture(batchBar->getTexture());
		//bars_sprite[i]->setPosition(Point(LAYER_WIDTH / 2, size_window.height + 30));
		bars_sprite[i]->setPosition(Point(LAYER_WIDTH / 2, notes_barPosition[i]));
		bars_sprite[i]->setVisible(false);
		layer_notes->addChild(bars_sprite[i], ZORDER_BARS);
		i++;
	}

#pragma endregion

#pragma region BGA 로딩

	loadBGA();

#pragma endregion

	/* 노트레이어 부착 */
	this->addChild(layer_notes, 1);
	CCLOG("GameScene::setNotes()..finished!");
}

// UI 정보 설정
void GameScene::setUiInfo() {
	//배경 스프라이트 생성
	CCLOG("setting Background ...");
	//auto winSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();

	ui_sprite_background = Sprite::create(UI_SPRITE_BACKGROUND);
	ui_sprite_background->setPosition(Point(size_window.width / 2, size_window.height / 2));
	this->addChild(ui_sprite_background, ZORDER_LAYOUT);

	CCLOG("setting Background ... finished!");
}

/* 게임 시작 */
void GameScene::gameStart() {

	/* 게임이 전혀 시작하지 않은 상태에서만 작동해야 한다. */
	if (status_playing != PLAY_STATUS::NOSTART) {
		return;
	}

	/* 게임 시작 효과음 재생 */
	SimpleAudioEngine::getInstance()->playEffect(SOUND_GO.c_str());

	/* 진행상태 갱신 */
	status_playing = PLAY_STATUS::PLAYING;

	/* 시작 시간 스타트 */
	time_start = std::chrono::system_clock::now();
	bga_start = std::chrono::system_clock::now();


	/* BGA 변화 초기화 */
	status_bgaCh = -1;

	/* 틱 오퍼레이션 작동 0.015 초 마다 호출 */
	this->schedule(schedule_selector(GameScene::tickOperate));
	
}

void GameScene::loadTexture() {



}

/* BGA 재생 틱 */
void GameScene::tickOperateBGA() {
	/* 현재 BGA 나타내기 */
	/* BGA 채널이 변경했나 감지 */
	if (status_bga != status_bgaCh) {

		/* 
		
			바뀐 채널이 영상인지 이미지 파일인지 파악
		
		*/

		/* BGA 채널 변경 */
		std::string filePath = "bms/" + status_dirs + "/" + str_bmpFile[status_bga];

		/* BGA 재생 시간 갱신 */
		bga_start = chrono_time;

		if (status_isVideo[status_bga]) {
			/* 영상임 */
			/* 새 영상 로드 */
			video_capture.open(filePath);
			if (!video_capture.isOpened()) {
				CCLOG("can't open BGA background.. %s", filePath.c_str());
				/* 대체 이미지 전환 */
				status_isVideo[status_bga] = false;
				str_bmpFile[status_bga] = "../../images/" + UI_TEXTURE_NOBGA;
			}
			else {
				CCLOG("open BGA background.. %s", filePath.c_str());

				/* 캡쳐한 비디오의 1프레임 MAT 형식을 Texture2D 로 변환 */
				video_capture >> video_frame;

				/* BGR 에서 RGB 컬러 변경 */
				cv::cvtColor(video_frame, video_frame, CV_BGR2RGB);

				/* 텍스쳐 변환 */
				CCLOG("Texture Setting.. %s", filePath.c_str());
				delete bga_texture;
				bga_texture = new Texture2D();
				bga_texture->initWithData(video_frame.data,
					video_frame.elemSize() * video_frame.cols * video_frame.rows,
					Texture2D::PixelFormat::RGB888,
					video_frame.cols,
					video_frame.rows,
					Size(video_frame.cols, video_frame.rows));
				CCLOG("Texture Setting..finished %s", filePath.c_str());
				CCLOG("Sprite Initting.. %s", filePath.c_str());
				bga_sprite->initWithTexture(bga_texture);
				bga_sprite->setContentSize(Size(size_window.width, size_window.height));
				bga_sprite->setPosition(size_window.width / 2, size_window.height / 2);
				CCLOG("Sprite Initting..finished %s", filePath.c_str());
			}
		}
		else {
			/* 이미지 변경 */
			bga_texture = Director::getInstance()->getTextureCache()->getTextureForKey(filePath);

			/* 이미지 애니메이션이 돌아가지 않는다면 여기서 더 코딩 해주어야함 
			
				1. bga_texture가 업데이트 되지 않아서 수동으로 스프라이트를 갱신해 주어야 하거나
				2. .bmp 확장자일 경우.
			
			*/

		}

		status_bgaCh = status_bga;
	}
	else {
		/* BGA 채널 변경 x 그대로 진행 */
		/* 이미지 채널인 경우 아무것도 하지 않는다. */
		if (status_isVideo[status_bga]) {
			/* 동영상일 경우 재생 */
			/* 아직 다음 프레임의 시간에 도달하지 않았으면 리턴한다 */
			double nextFrameTime = video_capture.get(CV_CAP_PROP_POS_MSEC) / 1000;
			if (nextFrameTime > currentTime_bga) {
				return;
			}

			video_capture >> video_frame;
			if (!video_frame.empty()) {
				/* BGR 에서 RGB 컬러 변경 */
				cv::cvtColor(video_frame, video_frame, CV_BGR2RGB);

				/* 다음 프레임의 MAT 파일로 텍스쳐를 업데이트 한다 */
				bga_texture->updateWithData(video_frame.data, 0, 0, video_frame.cols, video_frame.rows);
			}
			else {
				/* 대략 100 초 이상의 동영상이면 반복재생 하지 않는다 */
				if (currentTime_bga < 100) {
					/* BGA 재생 시간 갱신 */
					bga_start = chrono_time;

					/* 다시 오픈 - 무한반복 - 종료 확인하고 반복 안해도 됨 */
					std::string filePath = "bms/" + status_dirs + "/" + str_bmpFile[status_bga];
					video_capture.open(filePath);
				}
			}
		}

		
	}
}

// 노트 재생 시 한 틱당 계산해야 할 것.(음악의 시작)
void GameScene::tickOperate(float interval) {

	/* 타이머 현재 시각 구함 */
	chrono_time = std::chrono::system_clock::now();

	if (status_playing == PLAY_STATUS::PAUSED) {
		pause_time = chrono_time - chrono_pauseTime;
		return;
	}

	/* 채널 갱신 */
	sound_system->update();

	/* 타이머 현재 시각 구함 */
	time_music = chrono_time - time_start;
	time_music -= pause_time_all;
	time_bga = chrono_time - bga_start;
	time_bga -= pause_time_all;
	currentTime = time_music.count();
	currentTime_bga = time_bga.count();
	label_time_music->setString("music_time : " + std::to_string(currentTime));

	/* 현재 틱의 시각과 이전 틱의 시각과의 차이 */
	double tick_diff = currentTime - prevTime;
	prevTime = currentTime;

	/* 현재 시간까지 스크롤 된 오프셋을 구한다 */
	yPosOffset += tick_diff * status_speed * status_bpm;

	/* 현재 BPM 나타내기 */
	label_bpm->setString("music_bpm : " + std::to_string(status_bpm));

	/* 현재 Bar 번호 나타내기 */
	label_bar->setString("music_bar : " + std::to_string(bar_iter_latest - 1));

	/* iterator 시작점 설정 */
	std::vector<NOTE::Note>::iterator cur_note = note_iter_latest;

	/* BGA 애니메이션 해당 BGA의 fps 마다 호출한다. */
	tickOperateBGA();

	/* 마디를 순회하면서 위치 값을 구한다. */
	int cur_bar = bar_iter_latest;
	while (cur_bar < 1000) {

		double bar_posy = notes_barPosition[cur_bar] - yPosOffset;

		if (bar_posy > size_window.height) {
			break;
		}

		bars_sprite[cur_bar]->setPosition(Point(LAYER_WIDTH / 2, bar_posy));
		bars_sprite[cur_bar]->setVisible(true);

		if (bar_posy <= 0) {
			bars_sprite[cur_bar]->setVisible(false);
			bar_iter_latest++;
		}

		cur_bar++;
	}

	/* 노트를 순회하면서 위치 값을 구한다. */
	while (cur_note != notes.end()) {

		/* 시간 차 */
		double time_diff_curTime = cur_note->note_time - currentTime;

		/*
		
			현재 노트의 위치 값
		
		*/

		if (cur_note->note_posy - yPosOffset > size_window.height) {
			break;
		}

		cur_note->sprite->setPosition(Point(cur_note->sprite->getPositionX(), cur_note->note_posy - yPosOffset));
		if (cur_note->note_object == NOTE::OBJ_NOTE && !cur_note->isChecked) {
			cur_note->sprite->setVisible(true);
		}

		if (time_diff_curTime <= 0) {
			if (cur_note->note_object == NOTE::OBJ_EVENT) {
				operateNoteKey(*cur_note);
			}
			else {
				cur_note->sprite->setVisible(false);
				if (status_autoPlay && !cur_note->isChecked) {
					CCLOG("auto Inside : %d", cur_note->note_key);
					operateKeyEffect(cur_note->note_key);
				}
			}
		}

		if (time_diff_curTime <= -JUDGE[JUDGE::BAD]) {
			note_iter_latest++;
			if (!cur_note->isChecked) {
				operateComboEffect(cur_note);
			}
		}
		cur_note++;
	}

}

/* 키의 타입에 따라 달리 동작 한다. */
void GameScene::operateNoteKey(NOTE::Note &note) {

	if (note.isChecked) {
		return;
	}

	switch (note.note_key) {
	case NOTE::KEY_BPM:
		status_bpm = note.note_channel;
		note.isChecked = true;
		break;
	case NOTE::KEY_BPMEXTEND:
		status_bpm = str_bpmValue[note.note_channel];
		note.isChecked = true;
		break;
	case NOTE::KEY_STOP:
		/* 아무동작 취하지 않음 */
		note.isChecked = true;
		break;
	case NOTE::KEY_BGM:
		sound_system->playSound(v_sound_sound[note.note_channel], NULL, false, &sound_channel[note.note_channel]);
		note.isChecked = true;
		break;
	case NOTE::KEY_BGA:
		status_bga = note.note_channel;
		note.isChecked = true;
		break;
	default:
		break;
	}

#ifdef _DEBUG
	CCLOG("* Operate Note : %d, %lf", note.note_key, note.note_time);
#endif // _DEBUG

}

/* 변속 */
void GameScene::changeSpeed(bool isSpeedUp) {
	KEY_USINGSET;
	double before_speed = status_speed;
	if (status_speed > 1 && !isSpeedUp) {
		status_speed -= 1;
	}
	else if (status_speed < 8 && isSpeedUp) {
		status_speed += 1;
	}
	double span = status_speed / before_speed;

	/* 변속 사운드 효과음 재생 */
	SimpleAudioEngine::getInstance()->playEffect(SOUND_CHANGESPEED.c_str());

	/*
	
		전체 위치 재 지정
	
	*/

	/* 현재까지 진행 스크롤 길이 변경 */
	yPosOffset *= span;

	double cur_bpm = first_bpm;
	double cur_time = STARTEMPTYTIME;
	double pre_beat = 0.0f;
	double cur_pos = cur_time * cur_bpm * status_speed;
	//double cur_pos = 0;
	int cur_bar = 0;
	double bar_stack = cur_pos;
	int last_bar_pos;
	for (auto it = notes.begin(); it != notes.end(); it++) {

		auto cur_note = it;

		double cur_beat = cur_note->note_beat;

		double min, max;

		cur_bar = (int)pre_beat;
		double time_diff_sum = 0.0f;
		double pos_diff_sum = 0.0f;
		while (cur_bar <= cur_beat) {

			/* min 설정 */
			min = (cur_bar == (int)pre_beat) ? pre_beat : cur_bar;

			/* max 설정 */
			max = (cur_bar == (int)cur_beat) ? cur_beat : cur_bar + 1;

			/* 정수 비트 마디 세팅 */
			if (min == cur_bar) {
				notes_barPosition[cur_bar] = bar_stack;
				last_bar_pos = cur_bar;
			}

			/* 현재 마디에서 비트 차이 */
			double beat_diff = max - min;

			/* 1비트당 걸리는 초 */
			double secPerBeat = (60.0 / cur_bpm);

			/* 현재 마디의 길이 - 기본(1) 일때 4/4 박자 */
			double cur_barLength = 4 * notes_barLength[cur_bar];

			/* 비트 차이만큼 의 시간차이를 계산 */
			double time_diff = beat_diff * secPerBeat * cur_barLength;

			/* 이전 시간에 합산 */
			time_diff_sum += time_diff;
			pos_diff_sum += time_diff * status_speed * cur_bpm;
			bar_stack += time_diff * status_speed * cur_bpm;

			cur_bar += 1;
		}

		/* 현재 노트의 시간 입력 */
		cur_time += time_diff_sum;
		cur_note->note_time = cur_time;

		/* 시간 변화에 따른 거리 설정 */
		cur_pos += pos_diff_sum;
		cur_note->note_posy = cur_pos;

		/* 위치 재조정을 위해 모든 노드를 순간적으로 setVisible = false 해준다. */
		cur_note->sprite->setPosition(Point(cur_note->sprite->getPositionX(), cur_note->note_posy - yPosOffset));

		/* 현재 노트가 BPM Change나 STOP 일때 status 변경 */
		switch (cur_note->note_key) {
		case NOTE::KEY_STOP:
			/*

				STOP 값은 val / 192 * bar_length 로 실제 시간을 구해주어야 함

			*/
			cur_time += cur_note->note_channel / 192 * str_stopValue[(int)cur_beat];
			break;
		case NOTE::KEY_BPM:
			cur_bpm = cur_note->note_channel;
			break;
		case NOTE::KEY_BPMEXTEND:
			cur_bpm = str_bpmValue[cur_note->note_channel];
			break;
		default:
			break;
		}

		pre_beat = cur_beat;
	}

	/* 마디 길이정보 곡 종료 이후 마디 위치 설정 */
	double diff = notes_barPosition[last_bar_pos] - notes_barPosition[last_bar_pos - 1];
	int i = last_bar_pos + 1;
	while (i < 1000) {
		notes_barPosition[i] = notes_barPosition[i - 1] + diff;
		i++;
	}

	/* 마디 위치 재조정 */
	i = 0;
	while (i < 1000) {
		bars_sprite[i]->setPosition(Point(LAYER_WIDTH / 2, notes_barPosition[i] - yPosOffset));
		i++;
	}

	CCLOG("Changed speed : %lf", status_speed);
	label_speed->setString("music_speed : " + std::to_string(status_speed));
	this->scheduleOnce(schedule_selector(GameScene::releaseKeyInput), 0.015f);
}

/* 키 연주 입력 처리 */
void GameScene::operateKeyEffect(int keyNo) {

	//CCLOG("keyNo : %d", keyNo);

	/*

		S D F SPACE J K L
		키가 입력되었을 때 판정 및 이펙트 설정을 한다.
		해당 함수에서는 키눌림 이펙트와 사운드 재생을 하고
		판정여부는 operateComboEffect 에서 수행한다.

	*/

	/* 게임 시작 안했을 경우 나감 */
	if (status_playing == PLAY_STATUS::NOSTART) {
		return;
	}

	/* 현재 누르고 있는 상태일 경우 나감 - 중복입력 방지 */
	//if (status_keyPressing[keyNo]) {
	//	return;
	//}

	///* 키 누른 상태 ON */
	//status_keyPressing[keyNo] = true;

	/* 현재 눌린 키에 유효한 노트를 찾는다. */
	auto cur_note = note_iter_latest;

	/* 노트 탐색 */
	while (cur_note != notes.end()) {
			if (cur_note->note_key == keyNo && !cur_note->isChecked) {
				/*

					현재 누른 키값을 가진
					가장 가까운 체크되지 않은 노트를 찾는다.
					
					가장 가까운 노트를 찾으면 해당 노트가 유효범위 내에 있는지 확인한다
					operateComboEffect 에서 noteTime을 기준으로 판정한다.

				*/
				operateComboEffect(cur_note);
				CCLOG("find right Note : %d", cur_note->note_key);
				/* 사운드 채널 */
				thisKeyChannel = cur_note->note_channel;
				//CCLOG("%d channel sound pressed", thisKeyChannel);
				break;
			}
		cur_note++;
	}

	/* 노트 이펙트 */
	keyPressed_sprite[keyNo]->setVisible(true);
	if (status_autoPlay) {
		keyPressed_sprite[keyNo]->stopAllActions();
		auto dt = DelayTime::create(0.15f);
		auto cb = CallFunc::create(CC_CALLBACK_0(GameScene::endKeyEffect, this, 0.0f, keyNo));
		auto seq = Sequence::create(dt, cb, nullptr);
		keyPressed_sprite[keyNo]->runAction(seq);
	}

	/* 각 키의 사운드 세팅이 끝나면 입력받은 키의 음을 재생 */
	sound_system->playSound(v_sound_sound[thisKeyChannel], NULL, false, &sound_channel[thisKeyChannel]);
	
}

/* 키 연주 입력해제 처리 */
void GameScene::endKeyEffect(float interval, int keyNo) {

	/* 키 연속입력 해제 */
	status_keyPressing[keyNo] = false;

	/* 노트 이펙트 해제 */
	keyPressed_sprite[keyNo]->setVisible(false);

}

/* 판정 처리 */
void GameScene::operateComboEffect(std::vector<NOTE::Note>::iterator cur_note) {

	//CCLOG("noteTime : %lf", cur_note->note_time);

	/*

		들어온 노트와 판정시간에 따라
		각 판정에 따라 콤보와 점수계산을 하고
		해당 이펙트를 띄운다.

	*/

	/* 들어온 노트정보 분석 */
	int keyNo = cur_note->note_key;
	double judgeTime = cur_note->note_time - currentTime;
	CCLOG("comboEffect enter : keyNo : %d", keyNo);

	/* 6번 스크래치 사운드는 콤보에 반영하지 않는다. */
	if (keyNo == 6) {
		cur_note->isChecked = true;
		cur_note->sprite->setVisible(false);
		return;
	}

	/*

		판정 여부 검사

	*/
	int judgeNo = 0;

	if (judgeTime > JUDGE[JUDGE::BAD]) {
		/* 미리 눌렀을때 무시함 */
		return;
	}

	if (judgeTime >= -JUDGE[JUDGE::PERFECT] && judgeTime <= JUDGE[JUDGE::PERFECT]) {
		/* 퍼펙트 +- 0.15f 이내 */
		cur_note->isChecked = true;
		CCLOG("Perfect Checked : %d, %d", cur_note->isChecked, cur_note->note_key);
		cur_note->sprite->setVisible(false);
		judgeNo = JUDGE::PERFECT;
		if (currentCombo == maxCombo) {
			maxCombo = ++currentCombo;
		}
		else {
			currentCombo++;
		}
	}
	else if (judgeTime >= -JUDGE[JUDGE::GOOD] && judgeTime <= JUDGE[JUDGE::GOOD]) {
		/* 굿 +- 0.3f 이내 */
		cur_note->isChecked = true;
		cur_note->sprite->setVisible(false);
		judgeNo = JUDGE::GOOD;
		if (currentCombo == maxCombo) {
			maxCombo = ++currentCombo;
		}
		else {
			currentCombo++;
		}
	}
	else if (judgeTime >= -JUDGE[JUDGE::BAD] && judgeTime <= JUDGE[JUDGE::BAD]) {
		/* 배드 +- 0.4f 이내 */
		cur_note->isChecked = true;
		cur_note->sprite->setVisible(false);
		judgeNo = JUDGE::BAD;
		if (currentCombo == maxCombo) {
			maxCombo = ++currentCombo;
		}
		else {
			currentCombo++;
		}
	}
	else if (judgeTime < -JUDGE[JUDGE::BAD]) {
		/* 미스 - 0.4f 초과 */
		cur_note->isChecked = true;
		judgeNo = JUDGE::MISS;
		currentCombo = 0;
	}

	/*

		콤보 애니메이션 동작

	*/

	/* 판정 이펙트 보이기 */
	judge_label->setString(JUDGE_STR[judgeNo]);
	judge_label->setPosition(
		note_sprite_background->getContentSize().width / 2,
		note_sprite_background->getContentSize().height / 5
	);
	auto mov = MoveBy::create(0.0f, Point(0, -10));
	auto mov1 = MoveBy::create(COMBO_ACTIONTIME, Point(0, 10));
	auto ft = FadeTo::create(COMBO_ACTIONTIME, 150);
	auto spawn = Spawn::create(mov1, ft, nullptr);
	auto dt = DelayTime::create(COMBO_ACTIONDELAYTIME);
	auto fo = FadeOut::create(COMBO_ACTIONTIME);
	auto seq = Sequence::create(mov, spawn, dt, fo, nullptr);
	seq->setTag(101);
	judge_label->stopActionByTag(101);
	judge_label->runAction(seq);

	if (judgeTime >= 0) {
		judge_time_label->setString("+" + std::to_string(judgeTime));
		judge_time_label->setTextColor(Color4B::BLUE);
	}
	else {
		judge_time_label->setString(std::to_string(judgeTime));
		judge_time_label->setTextColor(Color4B::RED);
	}
	if (judgeTime >= -JUDGE[JUDGE::PERFECT] && judgeTime <= JUDGE[JUDGE::PERFECT]) {
		judge_time_label->setTextColor(Color4B::WHITE);
	}
	judge_time_label->setOpacity(0);
	ft = FadeTo::create(COMBO_ACTIONTIME, 150);
	dt = DelayTime::create(COMBO_ACTIONDELAYTIME);
	fo = FadeOut::create(COMBO_ACTIONTIME);
	seq = Sequence::create(ft, dt, fo, nullptr);
	judge_time_label->stopAllActions();
	judge_time_label->runAction(seq);

	/* 미스가 안 났을 때만 보여주는 이펙트 */
	if (judgeNo != JUDGE::MISS) {

		/* 건반 이펙트 보이기 */
		auto fi = FadeIn::create(0.0f);
		auto foo = FadeOut::create(0.0f);
		auto anim = Animation::create();
		anim->setDelayPerUnit(0.01f);
		for (int i = 0; i < 16; i++) {
			anim->addSpriteFrameWithTexture(cache->getTextureForKey(UI_SPRITE_BOMB),
				Rect(UI_SPRITE_ANIM_BOMB_WIDTH*i, 0, UI_SPRITE_ANIM_BOMB_WIDTH - 20, UI_SPRITE_ANIM_BOMB_HEIGHT));
		}
		auto animate = Animate::create(anim);
		auto seqan = Sequence::create(fi, animate, foo, nullptr);
		bomb_sprite[keyNo]->stopAllActions();
		bomb_sprite[keyNo]->runAction(seqan);

		/* 점수 이펙트 보이기 */
		combo_label->setString(std::to_string(currentCombo));
		combo_label->setPosition(LAYER_WIDTH / 2, note_sprite_background->getContentSize().height / 8 * 5);
		mov = MoveBy::create(0.0f, Point(0, -10));
		mov1 = MoveBy::create(COMBO_ACTIONTIME, Point(0, 10));
		ft = FadeTo::create(COMBO_ACTIONTIME, 150);
		spawn = Spawn::create(mov1, ft, nullptr);
		dt = DelayTime::create(COMBO_ACTIONDELAYTIME);
		fo = FadeOut::create(COMBO_ACTIONTIME);
		seq = Sequence::create(mov, spawn, dt, fo, nullptr);
		seq->setTag(100);
		combo_label->stopActionByTag(100);
		combo_label->runAction(seq);

		combo_label_ui->setOpacity(0);
		ft = FadeTo::create(COMBO_ACTIONTIME, 150);
		dt = DelayTime::create(COMBO_ACTIONDELAYTIME);
		fo = FadeOut::create(COMBO_ACTIONTIME);
		seq = Sequence::create(ft, dt, fo, nullptr);
		combo_label_ui->stopAllActions();
		combo_label_ui->runAction(seq);

	}
	else {
		/* 미스 났을 때 점수 안보이게 */
		combo_label_ui->setOpacity(0);
		combo_label->setOpacity(0);
	}

}

/* BGA 관련 정보 로드 */
void GameScene::loadBGA() {

	/*

		각 BGA 채널을 순회하면서
		만약 BGA 가 동영상 파일인지, 아니면 일반 이미지 파일인지
		검사부터 한다.

	*/

	int i = 0;
	while (i < STR_CHANNELSIZE) {
		std::string imgf = str_bmpFile[i];
		std::string fp = "bms/" + status_dirs + "/" + imgf;
		std::string chStr = "";
		bool chPathFlag = false;
		if (imgf.compare("") == 0) {
			/* 공백 - 빈 채널인 경우 */
			i++;
			continue;
		}
		if (fp.find(".mpeg", 0, 5) != std::string::npos
			|| fp.find(".mpg", 0, 4) != std::string::npos
			|| fp.find(".wmv", 0, 4) != std::string::npos
			|| fp.find(".avi", 0, 4) != std::string::npos) {

			/* 현재 채널이 동영상 파일일 때 */
			status_isVideo[i] = true;

		}
		else {

			/* 확장자 유효성 검사 - 존재하는 확장자 찾아서 변경 없을시 No image */
			struct stat buffer;
			if (stat(fp.c_str(), &buffer) != 0) {
				/* 사진 파일이 유효하지 않을 때 - jpg, png, bmp 파일 확인 해 봄 */
				fp.pop_back(); fp.pop_back();
				fp.pop_back(); fp.pop_back();
				str_bmpFile[i].pop_back();
				str_bmpFile[i].pop_back();
				str_bmpFile[i].pop_back();
				str_bmpFile[i].pop_back();
				if (stat((fp + ".jpg").c_str(), &buffer) == 0) {
					/* jpg 확장자 였을 때 */
					str_bmpFile[i] += ".jpg";
				}
				else if (stat((fp + ".png").c_str(), &buffer) == 0) {
					/* jpg 확장자 였을 때 */
					str_bmpFile[i] += ".png";
				}
				else if (stat((fp + ".bmp").c_str(), &buffer) == 0) {
					/* jpg 확장자 였을 때 */
					str_bmpFile[i] += ".bmp";
				}
				else {
					/* 확장자 지원 안함 */
					str_bmpFile[i] = "../../images/" + UI_TEXTURE_NOBGA;
				}
			}
#ifdef _DEBUG
			CCLOG("str_bmpFile[%d] = %s", i, str_bmpFile[i].c_str());
#endif // _DEBUG

			/* 현재 채널이 사진 파일 일 때 */
			status_isVideo[i] = false;

		}
		i++;
	}

	/* 오프닝 스프라이트 세팅 */
	bga_texture = new Texture2D();
	auto img = new Image();
	img->initWithImageFile(UI_TEXTURE_NOBGA);
	bga_texture->initWithImage(img, Texture2D::PixelFormat::RGB888);

	bga_sprite = Sprite::create();
	bga_sprite->initWithTexture(bga_texture);
	bga_sprite->setContentSize(Size(size_window.width, size_window.height));
	bga_sprite->setPosition(size_window.width / 2, size_window.height / 2);

	this->addChild(bga_sprite);

}

/* ESC 키 눌렸을 때 */
void GameScene::operateESC() {

	/*
	
		곡이 진행중일 때는 연주를 중단하고 나가기 옵션 창을 띄움.
		곡이 중단중(옵션창)일 때는 옵션창을 없애고 연주를 재개 함
	
	*/

	/* 효과음 사운드 재생 */
	SimpleAudioEngine::getInstance()->playEffect(SOUND_PAUSE.c_str());

	if (status_playing == PLAY_STATUS::PLAYING) {
		/* 중단해야 함 */
		chrono_pauseTime = chrono_time;
		status_playing = PLAY_STATUS::PAUSED;
		for (FMOD::Channel *channel : sound_channel) {
			channel->setPaused(true);
		}
	}
	else if(status_playing == PLAY_STATUS::PAUSED){
		/* 재개해야 함 */
		pause_time_all += pause_time;
		status_playing = PLAY_STATUS::PLAYING;
		for (FMOD::Channel *channel : sound_channel) {
			channel->setPaused(false);
		}
	}

	/* 옵션창이 꺼져있으면 보여주고 보여주고 있으면 끔 */

}

/* Pause 메뉴일 때 상황에 맞는 동작 */
void GameScene::operatePauseMenu() {
	
	/* 일단은 메뉴선택으로 되돌아가기 밖에 없다 */
	goBackMusicSelectScene();

}

/* 다시 음악 선택 씬으로 돌아감 */
void GameScene::goBackMusicSelectScene() {
	SimpleAudioEngine::getInstance()->playEffect(SOUND_CHANGESPEED);
	AudioEngine::stopAll();
	auto musicSelectScene = MusicSelectScene::createScene(background_texture);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, musicSelectScene));
}