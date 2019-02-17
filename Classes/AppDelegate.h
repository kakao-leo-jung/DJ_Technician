#pragma execution_character_set("utf-8")

#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

/**
@brief    The cocos2d Application.

Private inheritance here hides part of interface from Director.
*/
class  AppDelegate : private cocos2d::Application
{
public:
	AppDelegate();
	virtual ~AppDelegate();

	virtual void initGLContextAttrs();

	/**
	@brief    Implement Director and Scene init code here.
	@return true    Initialize success, app continue.
	@return false   Initialize failed, app terminate.
	*/
	virtual bool applicationDidFinishLaunching();

	/**
	@brief  Called when the application moves to the background
	@param  the pointer of the application
	*/
	virtual void applicationDidEnterBackground();

	/**
	@brief  Called when the application reenters the foreground
	@param  the pointer of the application
	*/
	virtual void applicationWillEnterForeground();

	static void changeCmdBms();
	static void changeCmdSound();
	static void changeCmdResource();
	static cocos2d::Point getCursorPos();

};

#endif // _APP_DELEGATE_H_

