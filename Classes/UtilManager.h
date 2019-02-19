#ifndef UTILMANAGER_H
#define UTILMANAGER_H

#pragma execution_character_set("utf-8")

#include "cocos2d.h"
#include "Note.h"
#include "MusicHeader.h"

/*

	BMS 파싱 클래스 .
	모든 BMS 파일 읽고 로딩 하는 작업은
	이 클래스에서 한다

	싱글턴 패턴으로 getInstance를 통해 호출해서 사용.

*/

class UtilManager
{
private:
	/* 싱글턴 패턴 */
	UtilManager() {}
	static bool instanceFlag;
	static UtilManager* instance;


public:
	/* 싱글턴 패턴 */
	static UtilManager* getInstance();
	virtual ~UtilManager() { instanceFlag = false; };

	

};

#endif