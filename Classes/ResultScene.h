#ifndef RESULTSCENE_H
#define RESULTSCENE_H

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "Score.h"
#include "opencv2/opencv.hpp"

USING_NS_CC;

class ResultScene : public cocos2d::Layer
{
private:
	//--------------------------------------------------------------------------------
	// 기본 변수
	Size size_window;												// 현재 윈도우의 사이즈
	TextureCache *cache;											// 텍스쳐 캐시관리자
	Sprite *cursor_sprite;											// 커서 스프라이트
	const std::string CURSOR_FILENAME = "images/cursor_arrow.png";	// 커서 파일경로
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	/*

		영상 재생 opencv 변수 및 기본 레이어 설정
		- 배경을 세팅.
		- 영상은 StartScene에서 넘어온 영상 vector<Texture2D> 를 그대로 넘겨받아 사용한다.
		- StartScene 과 동일하게 schedule을 통해 매 틱당 iter를 돌며 background_sprite를 업데이트 해준다.
		- 추가로 검은 사각형 틀을 만들어 덧씌운다.

	*/

	const std::string BACKGROUND_FILENAME = "video/background8.mpg";// 동영상 파일이름 ( 20 초 이내의 파일만 사용!!)
	Texture2D* background_texture;									// 캡쳐한 1프레임을 담을 texture
	Sprite *background_sprite;										// 캡쳐한 texture를 담을 배경 sprite
	cv::VideoCapture video_capture;									// 동영상 캡쳐객체
	cv::Mat video_frame;											// 동영상 1프레임
	//--------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------
	/*

		점수 관련 설정

	*/
	Score score;													// 기본 스코어 - GameScene에서 넘겨받는다.
	//--------------------------------------------------------------------------------

public:

	static cocos2d::Scene* createScene(Score score);

	virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event *event);
	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode, cocos2d::Event *event);
	void enableKeySetting();

	/* setting */
	void initData();
	void setUiInfo();
	void setLayerBasicLayer();
	void background_tick(float interval);

	// implement the "static create()" method manually
	CREATE_FUNC(ResultScene);
};

#endif