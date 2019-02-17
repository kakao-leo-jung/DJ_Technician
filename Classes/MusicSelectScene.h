#pragma execution_character_set("utf-8")

#include "cocos2d.h"
#include "opencv2/opencv.hpp"
#include "GameScene.h"

USING_NS_CC;

/*

	곡 선택화면 한 곡을 보여주는데 사용할 정보들을
	구조체로 저장한다.

*/
struct MusicSelectObject {
	Sprite *stagefile_sprite;			// 타이틀 사진 보여줄 곳.
	Label *title_label;					// 곡 제목 보여줄 곳(title + subtitle)
	Label *rank_label;					// 곡 난이도 보여줄 곳 
};

class MusicSelectScene : public cocos2d::Layer, public TextFieldDelegate
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
	// 사운드 관련 변수
	const std::string BGMMAIN_FILENAME
		= "bgm/musicSelect_Bossfight.mp3";							// 메인 BGM
	const float BGMMAIN_VOLUME = 0.2f;								// 메인 BGM 볼륨
	const char SOUND_CHANGELAYER[30] = "sound/init.mp3";			// 장면 진입 소리
	const char SOUND_CHANGELOGIN[30] = "sound/select.wav";			// 로그인 포커스 전환 소리
	const char SOUND_TICK[30] = "sound/tick.wav";					// 전환 불가 소리
	const char SOUND_CHANGEMUSICSELECT[30] = "sound/change.wav";	// 곡 선택 전환 소리
	const char SOUND_SELECT[30] = "sound/select2.wav";				// 상세정보 전환 소리
	const char SOUND_BACK[30] = "sound/back.wav";					// 상세정보 뒤로가기 소리
	//--------------------------------------------------------------------------------
	/*

		BMS 파싱 관련 변수

	*/
	std::vector<MusicHeader> musicHeaders;							// 모든 곡의 헤더 파일 저장
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	/*
		
		Layer 관련 변수.
		곡 선택 씬의 흐름은 다음과 같다.
		
		1. 로딩화면 보여주기
		-> (Thread를 사용해 bms 파일을 읽고 Callback 을 통해 로딩을 진행해야 한다.)
		-> (Main Thread는 로딩 레이어 UI 애니메이션을 보여주도록 한다.)
		-> (bms/ 하위폴더의 모든 bms, bme, bml 파일의 헤더를 읽고 관련 정보를 vector<MusicHeader>에 저장한다.)
		-> (이 때 STAGEFILE 이미지도 cache에 미리 로드해 놓는다.)
		-> (본 파서는 7Key 만 지원하므로 5키나 14키 등의 bms파일은 제거해 놓는 것을 권장한다.)
		-> 파싱한 vector<MusicHeader> 를 바탕으로 기본 스프라이트와 라벨 등 레이어 정보를 setup.
		
		2. 로딩이 끝나면 로딩 레이어를 지우고 메뉴 레이어를 보여준다.
		-> 240x160 비율로 사진과 공간을(세로는 더 타이틀2줄까지 160+a ) (1280 에서 총 가로 5개 들어감)
		-> 현재 선택된 사진을 센터에 맞춤.

		3. 이동해서 곡을 선택하면 곡에 대한 상세정보 레이어를 보여준다.


	*/
	int status_layer;												// 레이어 상태
	enum STATUS {
		MUSICSELECT,
		MUSICINFO
	};
	Layer *layer_musicSelect;										// 곡 선택 레이어 
	Layer *layer_subBar;											// 서브 상 하단바
	Layer *layer_musicInfo;											// 곡 상세정보 레이어
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	/*

		영상 재생 opencv 변수 및 기본 레이어 설정
		- 배경을 세팅.
		- 영상은 StartScene에서 넘어온 영상 vector<Texture2D> 를 그대로 넘겨받아 사용한다.
		- StartScene 과 동일하게 schedule을 통해 매 틱당 iter를 돌며 background_sprite를 업데이트 해준다.
		- 추가로 검은 사각형 틀을 만들어 덧씌운다.

	*/
	std::vector<Texture2D*> background_texture;						// 캡쳐한 1프레임을 담을 texture
	std::vector<Texture2D*>::iterator background_iter;				// 배경 vector iterator
	Sprite *background_sprite;										// 캡쳐한 texture를 담을 배경 sprite
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	// 로딩 라벨 설정
	const std::string LOADING_STR = "LOADING..";					// 로딩 텍스트
	const std::string LOADING_FONT = "fonts/LuluMonospace.ttf";		// 로딩 폰트
	const int LOADING_FONTSIZE = 50;								// 로딩 폰트 사이즈
	Label *loading_label;											// 로딩 라벨
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	/*
	
		곡 선택 메뉴 레이어
		스프라이트 캐시 로드는 std::vector<MusicHeaders> musicHeaders 에 BMS 헤더 정보가
		push가 완료된 후 musicHeaders 에서 musicHeaders.getValue(STAGEFILE) 로 가져온다.

		스프라이트 밑에 라벨을 child로 붙여준다
	
	*/
	int status_musicSelect;											// 현재 선택된 곡 인덱스
	const float MSO_ACTIONTIME_CHANGE = 0.15f;						// 전환 애니메이션 시간

	int widthCount;													// 곡 선택 가로 개수
	const int MSO_OPACITY_UNSELECTED = 50;							// 선택되지 않은 오브젝트의 투명도
	const int MSO_SPRITE_WIDTH = 240;								// 곡 선택 스프라이트 가로 사이즈
	const int MSO_SPRITE_HEIGHT = 160;								// 곡 선택 스프라이트 세로 사이즈
	const int MSO_SPRITE_WIDTHDIFF = 30;							// 곡 선택 스프라이트 가로 간격
	const std::string MSO_NOIMAGEFILE = "images/noImage.png";		// 곡 선택 스프라이트 NOIMAGE 
	const int MSO_LABEL_WIDTH = 240;								// 곡 선택 라벨 가로 사이즈
	const int MSO_LABEL_HEIGHT = 100;								// 곡 선택 라벨 세로 사이즈
	const int MSO_LABEL_TITLEPOSH = -60;							// 곡 선택 라벨 세로 위치(스프라이트에 상대적)
	const int MSO_LABEL_LEVELPOSH = -100;							// 곡 선택 라벨 세로 위치(스프라이트에 상대적)
	const std::string MSO_FONT = "fonts/LuluMonospace.ttf";			// 곡 선택 폰트
	const int MSO_FONTSIZE = 15;									// 곡 선택 폰트 사이즈
	std::vector<MusicSelectObject> musicSelectObject;				// 곡 선택 정보
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	/*

		곡 선택 상 하단 보조바 레이어

		화면 상/하단에 보조 바를 그리고
		세부정보를 표시한다.

		+ 세부정보
		 1. 현재 인덱스 / 전체 곡 개수
		 2. 현재 볼륨 + 볼륨설정 바
		 3. 현재 선택된 제목
		 4. 환경 설정
		 5. 도움말

		 #일단 가장 기본적인 1,2 만 만들고 추후에 추가.

	*/
	const std::string SUBBAR_TOPFILE = "images/subBar_top.png";		// 서브 상단바 파일이름
	const std::string SUBBAR_BTMFILE
		= "images/subBar_bottom.png";								// 서브 하단바 파일이름
	Sprite *subBarTop_sprite;										// 서브 상단바 스프라이트
	Sprite *subBarBtm_sprite;										// 서브 하단바 스프라이트
	const std::string SUBBAR_INDEX_FONT = "fonts/swagger.ttf";		// 인덱스 폰트
	const int SUBBAR_INDEX_FONTSIZE = 20;							// 인덱스 폰트 사이즈
	Label *subBar_label_curIdx;										// 서브바 현재 인덱스
	Label *subBar_label_maxIdx;										// 서브바 전체 인덱스
	const std::string SUBBAR_TITLE_FONT = "fonts/LuluMonospace.ttf";// 타이틀 폰트
	const int SUBBAR_TITLE_FONTSIZE = 30;							// 타이틀 폰트 사이즈
	Label *subBar_label_title;										// 서브바 현재곡 제목
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	/*

		곡 상세정보 레이어
		현재 선택한 곡의 8가지 정보를 보여준다.
		PLAYER, GENRE, TITLE, ARTIST, BPM, PLAYLEVEL, RANK, TOTAL

	*/
	const std::string MUSICINFO_BACKBOARDFILE
		= "images/music_info.png";									// 상세 정보 보드 파일이름
	Sprite *musicInfo_sprite_backBoard;								// 상세 정보 보드 스프라이트
	Label *musicInfo_label_ui[8];									// 상세 정보 UI라벨
	Label *musicInfo_label_info[8];									// 상세 정보 Info 라벨
	const int MUSICINFO_LEFTPADDING = 100;							// 상세 정보 UI 왼쪽 패딩 값
	const std::string MUSICINFO_LABELSTR[8] = {
		"PLAYER", "GENRE", "TITLE", "ARTIST", "BPM", "PLAYLEVEL", "RANK", "TOTAL"
	};
	enum MUSICINFO_LABEL {
		PLAYER, GENRE, TITLE, ARTIST, BPM, PLAYLEVEL, RANK, TOTAL
	};
	const std::string MUSICINFO_FONT
		= "fonts/LuluMonospace.ttf";								// 상세 정보 폰트
	const int MUSICINFO_FONTSIZE = 15;								// 상세 정보 폰트 사이즈


	//--------------------------------------------------------------------------------


public:

	static cocos2d::Scene* createScene(std::vector<Texture2D*> v);
	virtual bool init();
	virtual void onEnterTransitionDidFinish();

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
	void setup();
	void showLoading();
	void setBmsList();
	void enableKeySetting();
	void setLayerBasicLayer();
	void background_tick(float interval);
	void setLayerMusicSelect();
	void setLayerSubBar();
	void setLayerMusicInfo();
	/* ----------------------------------------------------------------------------*/

	/* --------------------------- SHOW FUNCTIONS ---------------------------------*/
	void startBGM(float interval);
	void showMusicSelect();
	void showMusicInfo();
	void hideMusicInfo();
	void changeMusicSelect(int from, int to);
	void startGame();
	/* ----------------------------------------------------------------------------*/
	// implement the "static create()" method manually
	CREATE_FUNC(MusicSelectScene);
};
