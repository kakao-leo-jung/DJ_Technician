#include <cstdlib>
#include <ctime>
#include "AppDelegate.h"
#include "StartScene.h"
#include "GameScene.h"

#define USE_AUDIO_ENGINE 1
//#define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_CC;

const int HDSIZE_X = 1280;
const int HDSIZE_Y = 720;
const int FHDSIZE_X = 1920;
const int FHDSIZE_Y = 1080;
const int QHDSIZE_X = 2560;
const int QHDSIZE_Y = 1440;

const int BGMSIZE = 6;

static cocos2d::Size designResolutionSize = cocos2d::Size(HDSIZE_X, HDSIZE_Y);
static cocos2d::Size smallResolutionSize = cocos2d::Size(HDSIZE_X, HDSIZE_Y);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(FHDSIZE_X, FHDSIZE_Y);
static cocos2d::Size largeResolutionSize = cocos2d::Size(QHDSIZE_X, QHDSIZE_Y);

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
#if USE_AUDIO_ENGINE
	AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
	SimpleAudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
	// set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8, 0 };

	GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
	return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
	// initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = GLViewImpl::createWithRect("DJTechnician", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
		//glview->setFrameSize(mediumResolutionSize.width, mediumResolutionSize.height);
		//glview = GLViewImpl::createWithFullScreen("DJ TECHNICIAN");
#else
		glview = GLViewImpl::create("DJTechnician");
#endif
		director->setOpenGLView(glview);
	}

	/* 디버깅용 fps 측정 */
	director->setDisplayStats(true);

	// set FPS. the default value is 1.0/60 if you don't call this
	/* 최대 144fps 까지 지원 */
	director->setAnimationInterval(1.0f / 200);

	/* 화면 커서 설정 및 변경 */
	glview->setCursorVisible(false);

	// Set the design resolution
	glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
	//glview->setFrameSize(mediumResolutionSize.width, mediumResolutionSize.height);

	auto frameSize = glview->getFrameSize();
	// if the frame's height is larger than the height of medium size.
	if (frameSize.height > mediumResolutionSize.height)
	{
		director->setContentScaleFactor(MIN(largeResolutionSize.height / designResolutionSize.height, largeResolutionSize.width / designResolutionSize.width));
	}
	// if the frame's height is larger than the height of small size.
	else if (frameSize.height > smallResolutionSize.height)
	{
		director->setContentScaleFactor(MIN(mediumResolutionSize.height / designResolutionSize.height, mediumResolutionSize.width / designResolutionSize.width));
	}
	// if the frame's height is smaller than the height of medium size.
	else
	{
		director->setContentScaleFactor(MIN(smallResolutionSize.height / designResolutionSize.height, smallResolutionSize.width / designResolutionSize.width));
	}

	register_all_packages();

	// create a scene. it's an autorelease object
	//AudioEngine::play2d("bgm/intro_Masader.mp3", true, 0.4f);


	//auto scene = GameScene::createScene(
	//	"AltMirrorBell",
	//	"AltMirroBell_N.bme",
	//	1,
	//	true
	//);

	//auto scene = GameScene::createScene(
	//"giselle",
	//"_s_h.bme",
	//2,
	//true
	//);

	//auto scene = GameScene::createScene(
	//	"bohemian",
	//	"bohemian_7h.bme",
	//	1,
	//	true
	//);

	auto scene = StartScene::createScene();

	// run
	director->runWithScene(scene);

	return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
	Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
	AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
	SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
	AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
	SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}

void AppDelegate::changeCmdBms() {
	//검색 폴더 경로 설정
	auto fileLocation = FileUtils::getInstance();
	std::vector<std::string> resDirOrders;
	resDirOrders.push_back("bms");
	fileLocation->setSearchResolutionsOrder(resDirOrders);
}

void AppDelegate::changeCmdSound() {
	//검색 폴더 경로 설정
	auto fileLocation = FileUtils::getInstance();
	std::vector<std::string> resDirOrders;
	resDirOrders.push_back("sound");
	fileLocation->setSearchResolutionsOrder(resDirOrders);
}

void AppDelegate::changeCmdResource() {
	//검색 폴더 경로 설정
	auto fileLocation = FileUtils::getInstance();
	std::vector<std::string> resDirOrders;
	fileLocation->setSearchResolutionsOrder(resDirOrders);
}

/* 현재 마우스 커서 위치 가져오기 */
Point AppDelegate::getCursorPos()
{

	auto glview = static_cast<cocos2d::GLViewImpl*>(cocos2d::Director::getInstance()->getOpenGLView());
	auto window = glview->getWindow();
	double px, py;
	glfwGetCursorPos(window, &px, &py);

	//TODO: cache window size
	int x, y;
	glfwGetWindowSize(window, &x, &y);

	//CInputManager::GetInstance().GetMousePos();


	return Point(px, y - py);
}