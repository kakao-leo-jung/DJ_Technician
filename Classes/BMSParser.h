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

class BMSManager
{
private:
	/* 싱글턴 패턴 */
	BMSManager() {}
	static bool instanceFlag;
	static BMSManager* instance;

	/* 파싱 변수 */
	/* 파싱 관련 진법 */
	const int BPMEXTEND_PUNC = 36;							// BPM 확장 채널 진법(36진법 사용중) 0 - Z
	const int BPM_PUNC = 16;								// BPM 진법 (16진법 사용중)0 - F
	const int STOP_FUNC = 36;								// STOP 채널 진법(36진법 사용중) 0 - Z
	const int BMP_PUNC = 36;								// BMP 진법(36진법 사용중) 0 - Z
	const int BAR_PUNC = 10;								// 마디 진법(10 진수)
	const int CHANNEL_PUNC = 16;							// 채널 진법(16 진법)
	const int WAV_PUNC = 36;								// WAV 진법(36진법 사용중) 0 - Z

public:
	/* 싱글턴 패턴 */
	static BMSManager* getInstance();
	virtual ~BMSManager() { instanceFlag = false; };

	/* BMS 하위폴더 헤더 읽고 저장하기 */
	static void readAllBmsHeader(std::vector<MusicHeader> *musicHeaders);
	static void readBms(std::string dirs, std::string fileName, double &status_bpm,
		double str_bpmValue[], std::string str_wavFile[], std::string str_bmpFile[], int str_stopValue[],
		std::vector<NOTE::Note> *notes, double notes_barLength[]);

	/* 파싱용 함수 */
	std::string readLineFromIndex(char ch[], int idx);
	std::vector<int> readDataLineFromIndex(char ch[], int idx, int punc);
	bool isDataSection(char ch[]);
	int changeCharToInteger(char ch[], int idx);
	int getBarNumber(char ch[]);
	int getChannelNumber(char ch[], int idx, int punc);
	void setNoteFromTmpVector(std::vector<int> st, int bar_no, std::vector<NOTE::Note> *notes,
		char obj, int key);

};