#ifndef MUSICHEADER_H
#define MUSICHEADER_H

#pragma execution_character_set("utf-8")

#include "cocos2d.h"

/*

	BMS 의 헤더정보를 읽어서 저장하는 클래스
	MusicHeader 는 하나의 .bms 파일에 대한 헤더정보를 가진다.

*/

class MusicHeader {
private:

	/* 헤더 변수 */
	std::string values[12];			/* values에 다 저장한다. */

public:
	/* 생성자 */
	MusicHeader(){}

	/* 소멸자 */
	~MusicHeader(){}

	/* 멤버 변수 */
	const int valueSize = 12;		/* 전체 헤더종류 개수 */
	enum {
		DIR, FILENAME, PLAYER, GENRE, TITLE, ARTIST, BPM, PLAYLEVEL,
		RANK, SUBTITLE, TOTAL, STAGEFILE
	};

	/* 헤더 변수 setter & getter */
	std::string getValues(int valueIdx);
	void setValues(int valueIdx, std::string val);

};

#endif