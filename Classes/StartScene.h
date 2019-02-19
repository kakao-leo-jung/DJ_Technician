#ifndef STARTSCENE_H
#define STARTSCENE_H

#pragma execution_character_set("utf-8")

#include "cocos2d.h"
#include "opencv2/opencv.hpp"

USING_NS_CC;

class StartScene : public cocos2d::Layer, public TextFieldDelegate
{
private:
	//--------------------------------------------------------------------------------
	// 기본 변수
	Size size_window;												// 현재 윈도우의 사이즈
	TextureCache *cache;											// 텍스쳐 캐시관리자
	Sprite *cursor_sprite;											// 커서 스프라이트
	const std::string CURSOR_FILENAME = "images/cursor_arrow.png";	// 커서 파일경로
	//--------------------------------------------------------------------------------
	// 키 입력
	bool status_keyUsing;											// 키 사용 여부(액션진행 중 키입력 방지)
	std::vector<EventKeyboard::KeyCode> heldKeys;					// 키 입력 정보 저장
	//--------------------------------------------------------------------------------
	// 영상 재생 opencv 변수
	cv::VideoCapture video_capture;									// 동영상 캡쳐객체
	cv::Mat video_frame;											// 동영상 1프레임
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	// 사운드 관련 변수
	const std::string BGMMAIN_FILENAME = "bgm/intro_Masader.mp3";	// 메인 BGM
	const float BGMMAIN_VOLUME = 0.4f;								// 메인 BGM 볼륨
	const char SOUND_ELECTIC[30] = "sound/electric.wav";			// 등장 전기소리
	const char SOUND_CHANGELAYER[30] = "sound/init.mp3";			// 장면 진입 소리
	const char SOUND_CHANGELOGIN[30] = "sound/select.wav";			// 로그인 포커스 전환 소리
	const char SOUND_LOGINFAIL[30] = "sound/tick.wav";				// 로그인 실패 소리
	const char SOUND_CHANGEMENUSELECT[30] = "sound/select.wav";		// 메뉴 선택 전환 소리
	//--------------------------------------------------------------------------------
	// 씬 상태 변수 - 레이어 변수
	// START 씬은 크게 6가지 레이어 전환이 있다
	// 로고동영상재생(0) - 아무키나 누르시오(1) - 로그인화면(2) - 메뉴(4)
	//											ㅣ			 ㅣ-- 1. singleplay
	//										 회원가입 화면	(3)	 |-- 2. multiplay
	//														 |-- 3. editor(5)
	//														 |-- 4. settings(6)
	//														 |-- 5. exit
	int status_layer;
	enum STATUS {
		SHOWLOGO,
		PRESSANYKEY,
		LOGIN,
		SIGNUP,
		MENUSELECT
	};
	Layer *layer_openingLogo;
	Layer *layer_basic;
	Layer *layer_pressAnyKey;
	Layer *layer_login;
	Layer *layer_signup;
	Layer *layer_menuSelect;
	Layer *layer_editor;
	Layer *settings;
	//--------------------------------------------------------------------------------
	/*
		SHOW OPENING 관련 변수
		게임이 시작하고 가장 먼저 보여줄 영상
		VideoCapture 를 사용해 mp4 파일을 프레임으로 자르고 매 루프마다 schedule로 함수를 호출해
		스프라이트 텍스쳐를 갱신시켜 영상을 재생한다

		재생이 끝나면 스케쥴함수 호출을 종료하고 자연스레 로딩화면을 부른다
	*/

	const std::string OPENING_FILENAME = "video/opening.avi";		// 오프닝 파일 이름
	const std::string OPENING_SOUNDFILENAME = "bgm/opening.mp3";	// 오프닝 사운드파일 이름
	Texture2D *opening_texture;										// 캡쳐한 1프레임을 담음
	Sprite *opening_sprite;											// 받은 텍스쳐를 올림
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	/*
		기본레이어 관련 변수 - 배경 동영상을 재생한다
		로딩화면을 진행하는 동안 동영상을 opencv 로 잘라 texture2d 배열로 저장한다
		iter 를 순회하며 영상을 무한루프 시키는 schedule을 호출한다.

		기본 DJTechnician 메인 로고 스프라이트도 기본 레이어에서 관리한다.

	*/
	
	const std::string BACKGROUND_FILENAME = "video/background7.mpg";// 동영상 파일이름 ( 20 초 이내의 파일만 사용!!)
	const std::string LOGOSPRITE_FILENAME = "images/dj10.png";		// 로고 메인 스프라이트 파일이름
	const std::string SUBLOGOSPRITE_FILENAME
		= "images/technician.png";									// 서브로고 스프라이트 파일이름
	const std::string SUBLOGOPARTICLE_FILENAME
		= "particle/logo_particle.png";								// 서브로고 파티클 파일이름
	Texture2D* background_texture;									// 캡쳐한 1프레임을 담을 texture
	Sprite *background_sprite;										// 캡쳐한 texture를 담을 배경 sprite
	Sprite *logo_sprite;											// 로고 스프라이트
	Sprite *sublogo_sprite;											// 서브로고 스프라이트
	ParticleGalaxy *sublogo_particle;								// 서브로고 파티클
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	// PRESSANYKEY 관련 변수
	const std::string PRESSANYKEY_STRING = "PRESS ENTER TO START";	// 라벨 대사
	const std::string PRESSANYKEY_FONTS = "fonts/LuluMonospace.ttf";// 폰트 종료
	const int PRESSANYKEY_FONTSIZE = 35;							// 폰트 사이즈
	Label *pressAnyKey_label;										// pressAnyKey 라벨
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	// LOGIN 관련 변수
	const std::string LOGINEDITON_FILENAME
		= "images/login_edittext_on.png";					// 에디트 이미지off 파일
	const std::string LOGINEDITOFF_FILENAME
		= "images/login_edittext_off.png";					// 에디트 이미지on 파일
	const std::string LOGINLOGINOFF_FILENAME
		= "images/login_login_off.png";						// 로그인 이미지off 파일
	const std::string LOGINLOGINON_FILENAME
		= "images/login_login_on.png";						// 로그인 이미지on 파일
	const std::string LOGINSIGNUPOFF_FILENAME
		= "images/login_signup_off.png";					// 회원가입 이미지off 파일
	const std::string LOGINSIGNUPON_FILENAME
		= "images/login_signup_on.png";						// 회원가입 이미지on 파일
	const std::string LOGINFAIL_FILENAME
		= "images/login_fail.png";							// 로그인 실패 창 파일
	const std::string LOGIN_FONTS
		= "fonts/LuluMonospace.ttf";						// 폰트 종료
	const int LOGIN_FONTSIZE = 20;							// 폰트 사이즈
	int status_login;										// 현재 로그인 화면 포거싱 상태
	enum LOGINSTATUS {
		PREVERR_LOGIN,
		IDINPUT, PWINPUT, LOGINBTN, SIGNUPBTN,
		NEXTERR_LOGIN, LOGINFAIL
	};
	Sprite *login_sprite_id;								// 아이디 입력 텍스트스프라이트
	TextFieldTTF *login_textfield_id;						// 아이디 입력 텍스트필드
	Sprite *login_sprite_pw;								// 패스워드 입력 텍스트스프라이트
	TextFieldTTF *login_textfield_pw;						// 패스워드 입력 텍스트필드
	Sprite *login_sprite_login;								// 로그인 버튼 스프라이트
	Sprite *login_sprite_signup;							// 회원가입 버튼 스프라이트
	Sprite *login_sprite_fail;								// 로그인 실패 스프라이트
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	// SIGNUP 관련 변수

	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	// MENUSELECT 관련 변수
	const std::string MENUSELECT_FONTS
		= "fonts/gnuolane.ttf";								// 폰트 종료
	const int MENUSELECT_FONTSIZE = 50;						// 폰트 사이즈
	const int MENUSELECT_DIST = 55;							// 메뉴간 간격
	const Color4B MENUSELECT_FOCUSEDCOLOR = Color4B::RED;	// 포커싱된 메뉴 색상
	RepeatForever *menuSelect_rep;							// 반짝이기
	int status_menuSelect;									// 현재 메뉴선택 화면 포거싱 상태
	const int MENUSELECT_MENUSIZE = 5;						// 총 메뉴 개수
	enum MENUSELECTSTATUS {
		SINGLEPLAY, MULTIPLAY, SETTING, EDITOR, EXIT
	};
	const std::string MENUSELECT_STR[5] = {
		"SINGLE PLAY",
		"MULTI  PLAY",
		"SETTINGS",
		"E D I T O R",
		"E  X  I  T"
	};														// 라벨 에 들어갈 글
	Label *menuSelect_label[5];								// 메뉴 라벨
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	// EDITOR 관련 변수

	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	// SETTINGS 관련 변수

	//--------------------------------------------------------------------------------


public:

	static cocos2d::Scene* createScene();

	virtual bool init();
	/* ---------------------------- INPUT FUNCTIONS -------------------------------*/
	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event *event);
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode, cocos2d::Event *event);
	void onKeyHold(float interval);
	void releaseKeyInput(float interval);
	void onMouseDown(cocos2d::Event* event);
	void onMouseUp(cocos2d::Event* event);
	void onMouseMove(cocos2d::Event* event);
	void onMouseScroll(cocos2d::Event* event);
	/* ----------------------------------------------------------------------------*/

	/* -------------------------- SETTING FUNCTIONS -------------------------------*/
	void showOpening();
	void opening_tick(float interval);
	void setup();
	void enableKeySetting();
	void setLayerBasicLayer();
	void background_tick(float interval);
	void setLayerPressAnyKey();
	void setLayerLogin();
	void setLayerSignUp();
	void setLayerMenuSelect();
	void setLayerEditor();
	void setLayerSettings();
	/* ----------------------------------------------------------------------------*/

	/* -------------------------- SHOW FUNCTIONS ----------------------------------*/
	void showPressAnyKey();
	void startBGM(float interval);
	void enterLogin();
	void changeLoginStatus(int from, int to, bool isEnterKey);
	void returnFailToLoginStatus();
	void enterMenuSelect();
	void changeMenuSelectStatus(int from, int to, bool isEnterKey);
	void enterSignUp();
	/* ----------------------------------------------------------------------------*/

	/* ------------------------ SCENE CHANEGE FUNCTIONS ---------------------------*/
	void goMusicSelectScene();

	// implement the "static create()" method manually
	CREATE_FUNC(StartScene);
};

#endif