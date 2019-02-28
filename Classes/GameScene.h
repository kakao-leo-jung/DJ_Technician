#ifndef	GAMESCENE_H
#define GAMESCENE_H

#include "cocos2d.h"
#include "opencv2/opencv.hpp"
#include "BMSParser.h"
#include <chrono>
#include <fmod.hpp>

USING_NS_CC;


class GameScene : public cocos2d::Layer
{
private:
	//--------------------------------------------------------------------------------
	// 현재 윈도우 사이즈
	Size size_window;									// 현재 윈도우의 사이즈
	TextureCache *cache;								// 텍스쳐 캐시관리자
	int status_screenMode;								// 현재 화면 모드
	enum SCREEN {
		WINDOW, FULLSCREEN
	};
	//--------------------------------------------------------------------------------
	/* 사운드 관련 정보 */
	const std::string SOUND_CHANGESPEED
		= "sound/change2.wav";							// 배속 전환 사운드
	const std::string SOUND_READY
		= "sound/robot_ready.wav";						// 로봇 준비 사운드
	const std::string SOUND_GO
		= "sound/robot_go.wav";							// 로봇 시작 사운드
	const std::string SOUND_PAUSE
		= "sound/pause.wav";							// 중단 사운드
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	/*

		연속입력을 벡터에 계속 축적해서 받는 것이 아니다.
		GameScene 에서 연속입력으로 어떤 이동명령을 지속 할 필요가 없음.

		다만 롱 노트 인식을 위해 Flag 를 둬서 onKeyPress가 눌리면 onKeyRelease가 발동하기
		전까지 Flag 를 True로 둔다.

	*/
	bool status_keyUsing;								// 키 사용 여부(액션진행 중 키입력 방지)
	bool status_keyPressing[50];						// 키 누름 지속 여부(1 - 7 키 까지)
	enum KEY {
		S = 1, D, F, SPACE, J, K = 8, L = 9, ALT
	};
	//--------------------------------------------------------------------------------
	// 현재 곡 정보 (이전 MusicSelectScene 에서 넘긴다)
	std::string status_bmsName;							// 현재 bms 파일 이름
	std::string status_dirs;							// 현재 bms 디렉터리 이름
	float status_speed;									// 현재 곡 배속(변경가능)
	double status_bpm;									// 현재 곡 BPM
	double first_bpm;									// 현재 곡 시작 BPM
	int status_bpm_tick;								// 현재까지 BPM 울린 횟수
	double status_next_tick_time;						// 다음 틱이 되어야 할 시간

	std::chrono::system_clock::time_point time_start;	// 곡 시작 시간
	std::chrono::system_clock::time_point bga_start;	// BGA 시작 시간
	std::chrono::duration<double> time_music;			// 곡 진행 시간
	std::chrono::duration<double> time_bga;				// BGA 진행 시간

	int score_noteSize;									// 전체 노트 개수(Max 콤보 수)

	std::vector<NOTE::Note>::iterator note_iter_latest;	// 가장 최신 노트
	int bar_iter_latest;								// 가장 최신 마디번호
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	/*

		게임 진행 관련 변수

	*/
	std::chrono::time_point
		<std::chrono::system_clock> chrono_time;		// 틱당 현재 절대 시간 저장
	std::chrono::duration<double> pause_time;			// 일시 정지 시간
	std::chrono::duration<double> pause_time_all;		// 총 일시 정지 누적 시간
	std::chrono::time_point
		<std::chrono::system_clock> chrono_pauseTime;	// 진행 중단 시점 시간 저장
	double currentTime;									// 곡의 현재 진행 시간
	double currentTime_bga;								// 현재채널 bga 진행을 위한 시간
	double yPosOffset;									// 스크롤 거리 측정
	double prevTime;									// 이전 틱의 시간
	int currentBarIdx;									// 곡의 현재 진행 마디
	double offsetPosY;									// 노트 위치 계산 offset
	double offsetBar;									// 마디 길이 offset
	double offsetBpm;									// BPM 초 계산 offset
	int thisKeyChannel;									// 현재 누른 키의 사운드 채널값
	bool status_autoPlay;								// 자동 연주 모드
	int status_playing;									// 게임 진행 상태
	enum PLAY_STATUS {
		NOSTART, PLAYING, PAUSED
	};
	const double STARTEMPTYTIME = 2.0f;					// 시작하고 빈 노트 시간
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	/*

		판정 관련 변수

	*/
	const float JUDGE[5] = {
		0.021f, 0.084f, 0.147f, 0.21f, 0.273f
	};													// 판정 라인
	const std::string JUDGE_STR[5] = {
		"PERFECT", "GREAT",  "GOOD", "BAD", "MISS"
	};													// 판정 STRING 값
	enum JUDGE {
		PERFECT, GREAT, GOOD, BAD, MISS
	};
	const std::string DJLEVEL_STR[9] = {
		"F", "E", "D", "C", "B", "A", "AA", "AAA", "S"
	};
	const Color3B DJLEVEL_COLOR[9] = {
		Color3B(50,50,50), Color3B(100, 100, 100), Color3B(150,150,150),
		Color3B(200,200,200), Color3B(200,200,0), Color3B(255,150,0),
		Color3B(255,100,0), Color3B(255, 50, 0), Color3B(255, 0, 0)
	};
	enum DJLEVEL {
		RANK_F, RANK_E, RANK_D, RANK_C, RANK_B, RANK_A, RANK_AA, RANK_AAA, RANK_S
	};
	Label *combo_label;									// 현재 콤보 라벨
	Label *combo_label_ui;								// 현재 콤보 UI 라벨
	Label *judge_label;									// 현재 판정
	Label *judge_time_label;							// 현재 판정시간 라벨
	
	Label *score_currentScore_label;					// 누적 점수 라벨
	const int UI_LABEL_CURRENTSCORE_X = 117;			// 누적 점수 라벨 X
	const int UI_LABEL_CURRENTSCORE_Y = 45;				// 누적 점수 라벨 Y
	const std::string UI_LABEL_CURRENTSCORE_FONT
		= "fonts/Teko-Bold.ttf";						// 현재 곡 BPM 폰트
	const int UI_LABEL_CURRENTSCORE_FONTSIZE = 20;		// 현재 곡 BPM 폰트사이즈

	Label *score_currentCombo_label;					// 현재 콤보 라벨

	Label *score_maxCombo_label;						// 현재 맥스콤보 라벨
	const int UI_LABEL_MAXCOMBO_X = 1085;				// 맥스 콤보 라벨 X
	const int UI_LABEL_MAXCOMBO_Y = 45;					// 맥스 콤보 라벨 Y
	const std::string UI_LABEL_MAXCOMBO_FONT
		= "fonts/Teko-Bold.ttf";						// 현재 곡 BPM 폰트
	const int UI_LABEL_MAXCOMBO_FONTSIZE = 20;			// 현재 곡 BPM 폰트사이즈

	Label *score_count_label[5];						// 현재 판정
	const int UI_LABEL_COUNT_X = 578;					// 현재 판정 X
	const int UI_LABEL_COUNT_Y = 295;					// 현재 판정 Y

	Label *score_countUi_label[5];
	const int UI_LABEL_COUNTUI_X = 408;					// 현재 판정 UI X	
	const int UI_LABEL_COUNTUI_Y = 295;					// 현재 판정 UI Y

	Label *score_djLevel_label;							// DJ Level 라벨
	const int UI_LABEL_DJLEVEL_X = 470;					// DJ Level X
	const int UI_LABEL_DJLEVEL_Y = 118;					// DJ Level Y
	const std::string UI_LABEL_DJLEVEL_FONT
		= "fonts/Teko-Bold.ttf";						// 현재 곡 BPM 폰트
	const int UI_LABEL_DJLEVEL_FONTSIZE = 60;			// 현재 곡 BPM 폰트사이즈

	const std::string COMBO_FONT
		= "fonts/Teko-Light.ttf";						// 콤보 폰트
	const int COMBO_FONTSIZE = 200;						// 콤보 폰트사이즈
	const std::string COMBO_UI_FONT
		= "fonts/Teko-Light.ttf";						// 콤보 ui 폰트
	const int COMBO_UI_FONTSIZE = 50;					// 콤보 ui 폰트사이즈
	const std::string JUDGE_FONT
		= "fonts/Teko-Light.ttf";						// 저지 폰트
	const int JUDGE_FONTSIZE = 100;						// 저지 폰트사이즈
	const int JUDGE_TIME_FONTSIZE = 25;					// 저지 타임 폰트사이즈
	const std::string SYSTEMS_FONT
		= "fonts/Teko-Bold.ttf";						// 시스템 폰트
	const int SYSTEMS_FONTSIZE = 22;					// 시스템 폰트사이즈
	const float COMBO_ACTIONTIME = 0.02f;				// 콤보 액션 시간
	const float COMBO_ACTIONDELAYTIME = 1.0f;			// 콤보 유지 시간
	int score_currentScore;								// 누적 점수
	int score_currentCombo;								// 현재 콤보
	int score_maxCombo;									// 최대 콤보
	int score_judgeCount[5];							// 누적 판정
	int score_currentSize;								// 현재 누적 노트 수
	int score_djLevel;									// 현재 DJ Level
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	/*

		영상 재생 관련 변수

	*/
	const std::string UI_TEXTURE_NOBGA
		= "images/noImage2.png";						// BGA 없을때 대체 텍스쳐
	bool status_isVideo[1296];							// 현재 채널이 영상인지 이미지인지 구분
	Sprite *bga_sprite;									// BGA 영상 스프라이트
	Texture2D *bga_texture;								// BGA 영상 텍스쳐
	cv::VideoCapture video_capture;						// 동영상 캡쳐객체
	cv::Mat video_frame;								// 동영상 1프레임
	int status_bga;										// 영상 채널 인덱스
	int status_bgaCh;									// 영상 채널 변경 감지 인덱스

	//--------------------------------------------------------------------------------
	// 사운드 관련 정보(FMOD)
	FMOD::System *sound_system;							// fmod 사운드 시스템
	FMOD::Channel *sound_channel[1296];					// 채널 개수(일단은 각 라인key개수 별로 채널 생성해본다)
	std::vector<FMOD::Sound*> v_sound_sound;			// 사운드 백터(노트의 wav 를 바탕으로 사운드를 할당한다)
	//--------------------------------------------------------------------------------
	// 곡 UI 정보
	const std::string UI_SPRITE_KEYPRESSFILE[10] = {		// 키 클릭 스프라이트 
		"",
		"images/game_note_background_key1.png",
		"images/game_note_background_key2.png",
		"images/game_note_background_key3.png",
		"images/game_note_background_key4.png",
		"images/game_note_background_key5.png",
		"images/game_note_background_key0.png",
		"images/game_note_background_key0.png",
		"images/game_note_background_key6.png",
		"images/game_note_background_key7.png",
	};
	const std::string UI_SPRITE_BACKGROUND
		= "images/game_background_hd.png";				// 전체 배경 스프라이트 파일
	const std::string UI_SPRITE_NOATEBACKGROUND
		= "images/game_note_background.png";			// 노트 배경 스프라이트 파일
	const std::string UI_SPRITE_SMALLNOTE
		= "images/game_note_small.png";					// 작은 노트 스프라이트 파일
	const std::string UI_SPRITE_LARGENOTE
		= "images/game_note_large.png";					// 큰 노트 스프라이트 파일
	const std::string UI_SPRITE_MIDNOTE
		= "images/game_note_mid.png";					// 중간 노트 스프라이트 파일
	const std::string UI_SPRITE_BOMB
		= "images/bomb2.png";							// 노트 이펙트
	const int UI_SPRITE_ANIM_BOMB_WIDTH = 181;			// 이펙트 가로 사이즈
	const int UI_SPRITE_ANIM_BOMB_HEIGHT = 192;			// 이펙트 세로 사이즈
	const std::string UI_SPRITE_EQUALIZER
		= "images/equalizer.png";						// 이퀄라이저 이펙트
	const std::string UI_SPRITE_BAR
		= "images/bars.png";							// 마디 스프라이트 파일
	const std::string UI_SPRITE_JUDGEBAR
		= "images/judgeBar.png";						// 판정선 스프라이트 파일
	const std::string UI_SPRITE_TURNTABLE
		= "images/turntable.png";						// 턴테이블 스프라이트 파일
	const int UI_SPRITE_TURNTABLE_X = 87;				// 턴테이블 스프라이트 가로위치
	const int UI_SPRITE_TURNTABLE_Y = 107;				// 턴테이블 스프라이트 세로위치
	const std::string UI_SPRITE_TURNBAR
		= "images/turnbar.png";							// 턴 바 스프라이트 파일
	const int UI_SPRITE_TURNBAR_X = 443;				// 턴 바 스프라이트 가로위치
	const int UI_SPRITE_TURNBAR_Y = 370;				// 턴 바 스프라이트 세로위치
	const int OPACITY_NOTE_SPRITE_BACKGROUND = 225;		// 노트 배경 투명도 - BGA 영상을 뒤에 얼마나 보여줄 것인가
	const int OPACITY_JUDGEBAR = 255;					// 판정선 투명도
	const int JUDGE_HEIGHT = 5;							// 판정 높이 조정

	Label *label_time_music;							// 곡 진행 시간 표시 라벨
	const int UI_LABEL_TIMEMUSIC_X = 688;				// 곡 진행 시간 표시 X
	const int UI_LABEL_TIMEMUSIC_Y = 27;				// 곡 진행 시간 표시 Y
	const std::string UI_LABEL_TIMEMUSIC_FONT
		= "fonts/Teko-Bold.ttf";						// 현재 곡 BPM 폰트
	const int UI_LABEL_TIMEMUSIC_FONTSIZE = 20;			// 현재 곡 BPM 폰트사이즈

	Label *label_speed;									// 현재 곡 배속 라벨
	const int UI_LABEL_SPEED_X = 338;					// 현재 곡 배속 X
	const int UI_LABEL_SPEED_Y = 27;					// 현재 곡 배속 Y
	const std::string UI_LABEL_SPEED_FONT
		= "fonts/Teko-Bold.ttf";						// 현재 곡 BPM 폰트
	const int UI_LABEL_SPEED_FONTSIZE = 20;				// 현재 곡 BPM 폰트사이즈

	Label *label_bpm;									// 현재 곡 BPM 라벨
	const int UI_LABEL_BPM_X = 453;						// 현재 곡 BPM X
	const int UI_LABEL_BPM_Y = 19;						// 현재 곡 BPM Y
	const std::string UI_LABEL_BPM_FONT
		= "fonts/Teko-Bold.ttf";						// 현재 곡 BPM 폰트
	const int UI_LABEL_BPM_FONTSIZE = 20;				// 현재 곡 BPM 폰트사이즈

	Label *label_bar;									// 현재 곡 Bar 라벨
	Sprite *ui_sprite_background;						// 전체 배경 스프라이트
	Sprite *note_sprite_background;						// 노트 배경 스프라이트
	Sprite *keyPressed_sprite[10];						// 곡 눌렀을때 스프라이트
	Sprite *bomb_sprite[10];							// 제대로 눌렀을 때 폭탄 스프라이트
	Sprite *equalizer_sprite;							// 이퀄라이저 스프라이트
	Sprite *bars_sprite[1000];							// 마디 스프라이트
	Sprite *judgeBar_sprite;							// 판정선 스프라이트
	Sprite *turntable_sprite;							// 턴테이블 스프라이트
	Sprite *turnbar_sprite;								// 턴 바 스프라이트
	//--------------------------------------------------------------------------------
	// 노트 정보
	std::vector<NOTE::Note> notes;						// 노트 정보(키채널 11 ~ 19)
	double notes_barLength[1000];						// 마디 길이 정보(이벤트채널 02)
	double notes_barPosition[1000];						// 각 마디당 위치 정보

	const int ZORDER_NOTES = 4;							// 노트의 z order
	const int ZORDER_LAYOUT = 1;						// 레이아웃의 z order
	const int ZORDER_NOTEBACKGROUND = 2;				// 노트 배경의 z order
	const int ZORDER_BARS = 3;							// 노트 마디의 z order
	const int ZORDER_LABEL = 5;							// 라벨의 z order

	const int KEY_SIZE = 7;								// 키 사이즈 (7 키만 지원한다)

	Layer *layer_notes;									// 노트 레이어(노트 스프라이트 붙일것)
	const int LAYER_POSITIONX = 127;					// 노트 레이어 초기 x 위치
	const int LAYER_POSITIONY = 135;					// 노트 레이어 초기 y 위치
	const int LAYER_WIDTH = 259;						// 노트 레이어 가로 사이즈
	const int LAYER_HEIGHT = 584;						// 노트 레이어 세로 사이즈

	const int NOTE_LARGEWIDTH = 37;						// 한 큰노트 가로 사이즈
	const int NOTE_SMALLWIDTH = 29;						// 한 작은노트 가로 사이즈
	const int NOTE_HEIGHT = 10;							// 한 노트 세로 사이즈

	int bar_length = 250;								// 한 마디 길이(임시 : 300 - 배속과 bpm 에 따라 조절해야함)
	const int BAR_MAXSIZE = 10000;						// 최대 마디수

	enum {
		CHANNEL_NOTE_KEYS1 = 11,						// 키 번호 채널 (1 - 7)
		CHANNEL_NOTE_KEYS2 = 12,
		CHANNEL_NOTE_KEYS3 = 13,
		CHANNEL_NOTE_KEYS4 = 14,
		CHANNEL_NOTE_KEYS5 = 15,
		CHANNEL_NOTE_KEYS6 = 18,
		CHANNEL_NOTE_KEYS7 = 19,
		CHANNEL_TRANS_KEYS1 = 31,						// 투명 키 번호 채널 (1 - 7)
		CHANNEL_TRANS_KEYS2 = 32,
		CHANNEL_TRANS_KEYS3 = 33,
		CHANNEL_TRANS_KEYS4 = 34,
		CHANNEL_TRANS_KEYS5 = 35,
		CHANNEL_TRANS_KEYS6 = 38,
		CHANNEL_TRANS_KEYS7 = 39,
		CHANNEL_EVENT_BGM = 1,							// BGM 재생 이벤트채널
		CHANNEL_EVENT_SHORTENBAR = 2,					// 마디 단축 이벤트채널
		CHANNEL_EVENT_CHANGEBPM = 3,					// BPM 변화 이벤트채널
		CHANNEL_EVENT_BGA = 4,							// BGA 보여주기 이벤트채널
		CHANNEL_EVENT_POORBGA = 6,						// MISS 시 BGA 보여주기 이벤트채널
		CHANNEL_EVENT_STOPSEQUENCE = 9					// 오브젝트 멈춤 이벤트채널
	};



	//--------------------------------------------------------------------------------
	// BMS 파싱 변수
	const int STR_CHANNELSIZE = 1296;					// 채널 사이즈(36진수 2자리)
	double str_bpmValue[1296];							// BPM 확장 채널 보관
	int str_stopValue[1296];							// STOP 확장 채널 보관
	std::string str_wavFile[1296];						// WAV 파일 보관(36진수 2자리 36*36)
	std::string str_bmpFile[1296];						// BMP 파일 보관(36진수 2자리)

	//--------------------------------------------------------------------------------

public:

	static cocos2d::Scene* createScene(std::string musicName, std::string bmsName, float speed, bool autoPlay);

	virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event *event);
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode, cocos2d::Event *event);
	void releaseKeyInput(float dt);
	void showLoadingState();
	void enableKeySetting();
	void initData();
	void setNotes();
	void parseBMS();
	void setUiInfo();
	void gameStart();
	void tickOperate(float interval);
	void operateNoteKey(NOTE::Note &note);
	void changeSpeed(bool isSpeedUp);
	void operateKeyEffect(int keyNo);
	void endKeyEffect(float interval, int keyNo);
	void operateComboEffect(std::vector<NOTE::Note>::iterator cur_note);
	int calculateCombo(double judgeTime);
	void tickOperateBGA();
	void loadBGA();
	void loadTexture();
	void operateESC();
	void operatePauseMenu();
	void goBackMusicSelectScene();
	void releaseData();
	void changeScreenMode();
	void setFullscreen();

	// implement the "static create()" method manually
	CREATE_FUNC(GameScene);
};

#endif