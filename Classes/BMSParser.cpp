#include <io.h>
#include "BMSParser.h"

/* 싱글턴 패턴 */
bool BMSParser::instanceFlag = false;
BMSParser* BMSParser::instance = nullptr;
BMSParser* BMSParser::getInstance() {
	if (!instance) {
		instance = new BMSParser();
		instanceFlag = true;
	}
	return instance;
}

/* BMS 헤더 파일 읽기 - 폴더 내 전체 곡 정보 스캔 */
void BMSParser::readAllBmsHeader(std::vector<MusicHeader> *musicHeaders) {
	/* 
	
		Bms 폴더 하위에 있는 모든 디렉터리에 접근하여 bms, bme, bml 파일 헤더를 읽고
		내용을 MusicHeader의 형태로 저장하여 벡터에 저장해준다.

		이 함수는 MusicSelectScene에서 곡 선택 화면을 보여주기 전에 미리 폴더에 있는
		모든 곡의 헤더 정보를 다 로딩시키는데 사용된다.
	
	*/

	/* 넘어온 벡터를 초기화 */
	musicHeaders->clear();

	/* 임시 디렉터리 명들 저장할 벡터 */
	std::vector<std::string> bmsDirs;
	std::vector<std::pair<std::string, std::string >> bmsFiles;

#pragma region BMS 하위 디렉터리 스캔 후 디렉터리 명 bmsDirs 저장

#ifdef _DEBUG
	//CCLOG("BMSParser : scanning bmsDirs...");
#endif

	/* bms 하위 디렉터리의 모든 폴더명을 다 찾아 bmsDirs에 일단 넣는다. */
	_finddata_t fd;
	long handle;
	int result = 1;
	/* BMS 폴더의 하위 bms폴더들 다 찾음 */
	handle = _findfirst("bms/*", &fd);

	if (handle == -1)
	{
		CCLOG("file read failed");
		return;
	}

	while (result != -1)
	{
		bmsDirs.push_back(fd.name);
		//CCLOG("%s", fd.name);
		result = _findnext(handle, &fd);
	}

	_findclose(handle);

	/* . 과 .. 폴더는 제거 */
	bmsDirs.erase(bmsDirs.begin());
	bmsDirs.erase(bmsDirs.begin());

#ifdef _DEBUG
	//CCLOG("BMSParser : scanning bmsDirs... - finished");
#endif

#pragma endregion

#pragma region 각 디렉터리를 방문하면서 bms, bml, bme 파일을 찾는다.

	auto iter = bmsDirs.begin();
	while (iter != bmsDirs.end()) {
		/* bms/curDirectory/*.* 를 다 읽고 bms, bml, bme 확장자만 걸러낸다. */
		std::string curDirectory = *iter;
		std::string path = "bms/" + curDirectory + "/*.*";
		iter++;
		_finddata_t fdd;
		result = 1;
		/* bms 폴더의 curDirectory 하위 파일들 다 찾음 */
		handle = _findfirst(path.c_str(), &fdd);

		if (handle == -1)
		{
			CCLOG("file read failed");
			return;
		}

		while (result != -1)
		{

			std::string tpFileName = fdd.name;
			if (tpFileName.find(".bms", 0, 4) != std::string::npos
				|| tpFileName.find(".bme", 0, 4) != std::string::npos
				|| tpFileName.find(".bml", 0, 4) != std::string::npos) {
				/* bms 파일임 */
				bmsFiles.push_back(make_pair(curDirectory, tpFileName));
				//CCLOG("%s", tpFileName.c_str());
			}
			result = _findnext(handle, &fdd);
		}
		_findclose(handle);

	}

#pragma endregion

#pragma region 각 bms, bml, bme 파일을 열고 vector<MusicHeader> 에 저장한다.

	auto iter2 = bmsFiles.begin();
	while (iter2 != bmsFiles.end()) {
		/* 현재 파일의 경로와 파일이름 */
		std::string curDir = iter2->first;
		std::string curFile = iter2->second;
		std::string path = "bms/" + curDir + "/" + curFile;
		iter2++;

		/* 현재 파일을 오픈하여 읽는다. */
#ifdef _DEBUG
		//CCLOG("*********** %s file open start ********", path.c_str());
#endif
		MusicHeader curHeader;

		/* 폴더 이름과 파일 이름 부터 저장 */
		curHeader.setValues(MusicHeader::DIR, curDir);
		curHeader.setValues(MusicHeader::FILENAME, curFile);

		FILE *fp = fopen(path.c_str(), "r");
		int inHeaderSection = 0;
		char ch[1024];
		while (!feof(fp)) {
			fgets(ch, sizeof(ch), fp);
			if (ch[0] != '#') {
				if (ch[0] == '*') {
					/* 헤더 섹션을 빠져 나옴 읽기 중단 */
					if (++inHeaderSection >= 2) {
						break;
					}
				}
				else {
					continue;
				}
			}
			std::string readValue;
			int idx = -1;
			int valIdx = 0;
			if (strncmp(ch, "#WAV", 4) == 0) {
				/* 헤더섹션에서 WAV 파일은 읽지 않는다. */
				continue;
			}
			else if (strncmp(ch, "#BMP", 4) == 0) {
				/* 헤더섹션에서 BMP 파일은 읽지 않는다. */
				continue;
			}
			else if (strncmp(ch, "#PLAYER", 7) == 0) {
				idx = 8;
				valIdx = MusicHeader::PLAYER;
			}
			else if (strncmp(ch, "#GENRE", 6) == 0) {
				idx = 7;
				valIdx = MusicHeader::GENRE;
			}
			else if (strncmp(ch, "#TITLE", 6) == 0) {
				idx = 7;
				valIdx = MusicHeader::TITLE;
			}
			else if (strncmp(ch, "#ARTIST", 7) == 0) {
				idx = 8;
				valIdx = MusicHeader::ARTIST;
			}
			else if (strncmp(ch, "#BPM", 4) == 0) {
				idx = 5;
				valIdx = MusicHeader::BPM;
			}
			else if (strncmp(ch, "#PLAYLEVEL", 10) == 0) {
				idx = 11;
				valIdx = MusicHeader::PLAYLEVEL;
			}
			else if (strncmp(ch, "#RANK", 5) == 0) {
				idx = 6;
				valIdx = MusicHeader::RANK;
			}
			else if (strncmp(ch, "#TOTAL", 6) == 0) {
				idx = 7;
				valIdx = MusicHeader::TOTAL;
			}
			else if (strncmp(ch, "#STAGEFILE", 10) == 0) {
				idx = 11;
				valIdx = MusicHeader::STAGEFILE;
			}
			else if (strncmp(ch, "#SUBTITLE", 9) == 0) {
				idx = 10;
				valIdx = MusicHeader::SUBTITLE;
			}
			else {
				//CCLOG("music Parse Failed %s", ch);
			}
			if (idx != -1) {
				while (ch[idx] != '\n') {
					readValue += ch[idx++];
				}
				/* 각 해당하는 idx 에 읽은 값을 저장한다. */
				curHeader.setValues(valIdx, readValue);
#ifdef _DEBUG
				//CCLOG("valIdx %d : %s", valIdx, curHeader.getValues(valIdx).c_str());
#endif
			}
		}
		fclose(fp);

		/* 다 읽고 세팅한 현재 곡 MusicHeader를 vector에 저장한다. */
		musicHeaders->push_back(curHeader);

#ifdef _DEBUG
		//CCLOG("*********** %s file open finished!! ********", path.c_str());
#endif

	}

#pragma endregion

}

/* '\n' 끝까지 읽기 */
std::string BMSParser::readLineFromIndex(char ch[], int idx) {

	/*
	
		ch 로 주어진 입력 라인의
		idx값 부터 끝까지 읽어서 리턴한다.
	
	*/

	std::string ret;

	while (ch[idx] != '\n') {
		ret += ch[idx++];
	}

	return ret;
}

/* '\n' 끝까지 데이터 섹션 읽기 */
std::vector<int> BMSParser::readDataLineFromIndex(char ch[], int idx, int punc) {

	/*
	
		데이터 섹션의 각 비트정보를 읽고 채널값을 벡터에 임시저장하여 반환한다.
	
	*/

	std::vector<int> st;

	while (ch[idx] != '\n') {
		int channel = getChannelNumber(ch, idx, punc);
		idx += 2;
		// wav 설정 해주어야함
		st.push_back(channel);
	}

	return st;
}

/* 읽은 데이터 섹션을 노트데이터로 재생산 */
void BMSParser::setNoteFromTmpVector(std::vector<int> st, int bar_no,
	std::vector<NOTE::Note> *notes, char obj, int key) {

	/* 각 채널의 비트값 = 마디번호 + 현재인덱스 / 전체사이즈 */
	int stackSize = st.size();
	int inIdx = 0;

	/* 다시 스택을 순회하며 구한 노트 정보를 입력한다. */
	for (int channel : st) {
		/* 00 인 채널값은 없는 것으로 간주한다. 전체 비트를 구분하기 위한 용도로만 사용됨 */
		if (channel != 0) {

			/* 새로운 노트 정보를 입력하고 벡터에 저장 */
			NOTE::Note note = NOTE::Note();
			note.note_object = obj;
			note.note_key = key;
			note.note_channel = channel;
			note.note_beat = bar_no + ((double)inIdx / stackSize);
			note.isChecked = false;
			note.note_time = 0;
			note.note_posy = 0;
			note.sprite = cocos2d::Sprite::create();

			notes->push_back(note);

#ifdef _DEBUG
			NOTE::Note tpNote = notes->back();
			CCLOG(" * read Note Info : \n obj:%c / key:%s / ch:%d / beat:%lf", tpNote.note_object, NOTE::STR_KEY[tpNote.note_key].c_str(), tpNote.note_channel, tpNote.note_beat);
#endif // _DEBUG

		}
		inIdx++;
	}

}

/* 읽어들인 라인이 데이터 섹션인지 판별 */
bool BMSParser::isDataSection(char ch[]) {
	/*
	
		읽어들인 라인의 1번째 인덱스가 문자이면 HeaderSection
						   인덱스가 0-9 사이 숫자이면 DataSection
	
	*/
	return (ch[1] >= '0' && ch[1] <= '9');
}

/* char - 진법 숫자 변환 */
int BMSParser::changeCharToInteger(char ch[], int idx) {

	/*
	
		들어온 char 형식을 숫자값으로 바꾸어준다.
		char 모양이 숫자이면 - '0' 해주고
		char 모양이 문자(10진수 이상일 때) 이면 -'7' 해주어
		해당 char 형식의 값이 가진 수를 구한다.
	
	*/

	return (ch[idx] > '9') ? ch[idx] - '7' : ch[idx] - '0';

}

/* 현재 줄 마디 번호 구함 */
int BMSParser::getBarNumber(char ch[]) {

	/*
	
		입력 줄의 1,2,3번째는 10진수의 최대 3자리 수로 표현되어 있다.
		구하고자 하는 마디값 #XXX.. 를 반환
	
	*/

	int ch1tp = changeCharToInteger(ch, 1);
	int ch2tp = changeCharToInteger(ch, 2);
	int ch3tp = changeCharToInteger(ch, 3);

	return ch1tp * (BAR_PUNC*BAR_PUNC) + ch2tp * BAR_PUNC + ch3tp;
}

/* 현재 줄 채널 번호 구함 */
int BMSParser::getChannelNumber(char ch[], int idx, int punc) {

	/*
	
		구하고자 하는 채널값 #...XX 값을 주어진 진수로 변환해서 반환 함.
	
	*/

	return changeCharToInteger(ch, idx) * punc + changeCharToInteger(ch, idx + 1);
}

/* BMS 전체 파일 읽기 */
void BMSParser::readBms(std::string dirs, std::string fileName, double &status_bpm,
	double str_bpmValue[], std::string str_wavFile[], std::string str_bmpFile[], int str_stopValue[],
	std::vector<NOTE::Note> *notes, double notes_barLength[]) {

	/*
	
		선택된 곡의 BMS 파일을 읽고 정보를 입력한다.
	
	*/

	/* filename 확장자를 찾았을 경우 설정된 filePath으로 read */
	std::string filePath = "bms/" + dirs + "/" + fileName;

#ifdef _DEBUG
	CCLOG("*********** %s file open start ********", filePath.c_str());
#endif

	FILE *fp = fopen(filePath.c_str(), "r");
	char ch[1024];
	while (!feof(fp)) {

		/* 한 줄 (최대 1024) 읽어옴 */
		fgets(ch, sizeof(ch), fp);

		/* bms 형식은 #으로 시작하는 것만 읽는다. */
		if (ch[0] != '#') {
			continue;
		}

		/* 파싱용 인덱스 선언 */
		int idx = -1;

		if (!getInstance()->isDataSection(ch)) {

#pragma region :: 헤더 파싱 영역

			/*

				헤더 섹션 파싱

			*/

#pragma region :: BPM 파싱 영역
			if (strncmp(ch, "#BPM ", 5) == 0) {
				idx = 5;
				status_bpm = std::stod(getInstance()->readLineFromIndex(ch, idx));

#ifdef _DEBUG
				CCLOG("status_bpm = %lf", status_bpm);
#endif

			}
#pragma endregion
#pragma region :: BPM 확장 채널 파싱 영역
			/* #BPM 179 가 아니라 # BPMxx 222 같은 채널값을 담고 있는 형식일 경우 */
			else if (strncmp(ch, "#BPM", 4) == 0) {
				idx = 7;
				int bpm_channel = getInstance()->getChannelNumber(ch, 4, getInstance()->BPMEXTEND_PUNC);
				str_bpmValue[bpm_channel] = std::stod(getInstance()->readLineFromIndex(ch, idx));

#ifdef _DEBUG
				CCLOG("str_bpmValue[%d] = %lf", bpm_channel, str_bpmValue[bpm_channel]);
#endif

			}
			// ########################################
#pragma endregion
#pragma region :: WAV 파싱 영역
			else if (strncmp(ch, "#WAV", 4) == 0) {
				idx = 7;
				int wav_channel = getInstance()->getChannelNumber(ch, 4, getInstance()->WAV_PUNC);
				str_wavFile[wav_channel] = getInstance()->readLineFromIndex(ch, idx);

#ifdef _DEBUG
				CCLOG("str_wavFile[%d] = %s", wav_channel, str_wavFile[wav_channel].c_str());
#endif

			}
#pragma endregion
#pragma region :: BMP 이미지 파싱 영역
			else if (strncmp(ch, "#BMP", 4) == 0) {
				idx = 7;
				int bmp_channel = getInstance()->getChannelNumber(ch, 4, getInstance()->BMP_PUNC);
				str_bmpFile[bmp_channel] = getInstance()->readLineFromIndex(ch, idx);

#ifdef _DEBUG
				CCLOG("str_bmpFile[%d] = %s", bmp_channel, str_bmpFile[bmp_channel].c_str());
#endif

			}
#pragma endregion
#pragma region :: STOP 파싱 영역
			else if (strncmp(ch, "#STOP", 5) == 0) {
				idx = 8;
				int stop_channel = getInstance()->getChannelNumber(ch, 4, getInstance()->STOP_FUNC);		
				str_stopValue[stop_channel] = std::stoi(getInstance()->readLineFromIndex(ch, idx));

#ifdef _DEBUG
				CCLOG("str_stopFile[%d] = %d", stop_channel, str_stopValue[stop_channel]);
#endif

			}

#pragma endregion
#pragma region :: 파서에 없는 헤더 정보일 때
			else {

#ifdef _DEBUG
				CCLOG("music Parse Failed %s", ch);
#endif

			}
#pragma endregion

#pragma endregion

		}
		else {

#pragma region :: 데이터 파싱 영역
			/*

				데이터 섹션 파싱
				일단 해당 정보의 줄을 읽으며 끝까지 임시 스택에 넣고
				다시 스택을 돌면서 각 스택의 인덱스 / 스택의 사이즈 로 세부 비트 소수점 정보를 구한다.
				이렇게하면 마디를 16, 32비트 제한이 있는 것이 아니라 아무리 비트를 쪼개어도
				다 구분이 가능하다.

			*/

			/*

				TODO : 데이터 영역의 개수가 짝수인지 체크하는 함수 만들어서 체크해주어야 함

			*/

			/* 현재 줄의 마디 번호 구함 */
			int bar_no = getInstance()->getBarNumber(ch);

			/* 데이터를 임시 저장할 벡터 선언 */
			std::vector<int> st;
			
			/* 7번째 인덱스부터 데이터영역 시작 */
			idx = 7;

			/*

				채널 속성 구분
				읽은 줄의 4번 인덱스가 0,1,3 이 존재
				
			*/
			switch (ch[4]) {
			case '0':
#pragma region :: 이벤트 채널인 경우

				if (ch[5] == '1') {
#pragma region :: BGM 재생 채널 "01"

					/*

						BGM 채널은 일반 노트와 같이 취급해서 bpm 이나 마디길이에 영향을 받는다.(정확한 확인 필요)
						따라서 일반 notes 벡터에서 같이 스크롤 하고 time 을 계산하되 자동연주 처리해준다.

					*/

					/* 데이터채널을 2개씩 파싱하여 각 비트의 채널값을 벡터에 저장한다. */
					st = getInstance()->readDataLineFromIndex(ch, idx, getInstance()->WAV_PUNC);

					/* 읽은 스택을 기반으로 노트 정보를 생성 및 저장 한다. */
					getInstance()->setNoteFromTmpVector(st, bar_no, notes, NOTE::OBJ_EVENT, NOTE::KEY_BGM);

#pragma endregion
				}
				if (ch[5] == '2') {
#pragma region :: 마디 변경 채널 "02"

					/*

						마디 변경 채널 인 경우
						마디같은경우 노트 형식이 아니라 배열에 따로 저장해 놓는다.

					*/

					notes_barLength[bar_no] = std::stod(getInstance()->readLineFromIndex(ch, idx));

#pragma endregion					
				}
				if (ch[5] == '3') {
#pragma region :: BPM 변경 채널 "03"

					/*

						BPM 채널은 일반 노트와 같이 취급해서 bpm 이나 마디길이에 영향을 받는다.
						따라서 일반 notes 벡터에서 같이 스크롤 하고 time 을 계산하고 BPM 을 변경해준다.

					*/

					st = getInstance()->readDataLineFromIndex(ch, idx, getInstance()->BPM_PUNC);
					getInstance()->setNoteFromTmpVector(st, bar_no, notes, NOTE::OBJ_EVENT, NOTE::KEY_BPM);

#pragma endregion
				}
				if (ch[5] == '8') {
#pragma region :: BPM 변경 확장 채널 "08"

					/*

						BPM 확장 채널은 일반 노트와 같이 취급해서 bpm 이나 마디길이에 영향을 받는다.
						따라서 일반 notes 벡터에서 같이 스크롤 하고 time 을 계산하고 BPM 을 변경해준다.
						단 이때, 저장한 노트의 채널값은 BPM 을 가르키는게 아니라 BPM 확장 채널 배열의 인덱스를
						나타낸다. 따라서 "03" 같은경우는 바로 채널값으로 BPM 값을 적용시켜 주지만 "08"의 경우
						해당 str_bpm[notes_channel] 의 값으로 BPM 값을 변경해주어야 한다.

					*/

					st = getInstance()->readDataLineFromIndex(ch, idx, getInstance()->BPMEXTEND_PUNC);
					getInstance()->setNoteFromTmpVector(st, bar_no, notes, NOTE::OBJ_EVENT, NOTE::KEY_BPMEXTEND);

#pragma endregion
				}
				if (ch[5] == '8') {
#pragma region :: 시퀀스 STOP 채널 "09"

					/*

						시퀀스 정지 채널은 역시 일반 노트와 같은 취급.
						단 해당 노트일 경우 str_stop[] 채널의 값만큼 정지시킨다.

					*/

					st = getInstance()->readDataLineFromIndex(ch, idx, getInstance()->STOP_FUNC);
					getInstance()->setNoteFromTmpVector(st, bar_no, notes, NOTE::OBJ_EVENT, NOTE::KEY_STOP);

#pragma endregion
				}
				if (ch[5] == '4') {
#pragma region BGA 변경 채널 "04"

					/*

						BGA 변경 채널은 일반 노트와 같이 취급해서 bpm 이나 마디길이에 영향을 받는다.
						따라서 일반 notes 벡터에서 같이 스크롤 하고 time 을 계산하고 BGA 를 변경해준다.
						단 이때, 저장한 노트의 채널값은 BGA 파일명을 가르키는게 아니라 BGM 확장 채널 배열의 인덱스를
						나타낸다. 따라서 "08"의 경우와 같이
						해당 str_bmp[notes_channel] 의 값으로 v_bga_bga[notes_channel] BGA 텍스쳐 또는 동영상을
						로드해주고 v_bga_bga[notes_channel] 로 해당 텍스쳐나 영상을 변경한다.

					*/

					st = getInstance()->readDataLineFromIndex(ch, idx, getInstance()->BMP_PUNC);
					getInstance()->setNoteFromTmpVector(st, bar_no, notes, NOTE::OBJ_EVENT, NOTE::KEY_BGA);

#pragma endregion

				}
				break;
#pragma endregion
			case '1':
#pragma region :: 노트 키 채널인 경우
				
				/*
				
					실제 재생되는 노트들을 저장.

					1키는 notes_key = 1
					2키는 notes_key = 2
					3키는 notes_key = 3
					4키는 notes_key = 4
					5키는 notes_key = 5
					스크래치키는 notes_key = 6
					6키는 notes_key = 8
					7키는 notes_key = 9

					가 저장되어야 함
				
				*/

				st = getInstance()->readDataLineFromIndex(ch, idx, getInstance()->WAV_PUNC);

				/* 키값에 ch[5] 인덱스를 int 로 변환하여 키값을 넣는다. */
				getInstance()->setNoteFromTmpVector(st, bar_no, notes, NOTE::OBJ_NOTE, getInstance()->changeCharToInteger(ch, 5));
				
				break;
#pragma endregion 
			case '3':
#pragma region :: 투명 노트 키 채널인 경우
				
				/*

					실제 재생되는 노트들을 저장.

					1키는 notes_key = 11
					2키는 notes_key = 12
					3키는 notes_key = 13
					4키는 notes_key = 14
					5키는 notes_key = 15
					스크래치키는 notes_key = 16
					6키는 notes_key = 18
					7키는 notes_key = 19

					가 저장되어야 함

				*/

				st = getInstance()->readDataLineFromIndex(ch, idx, getInstance()->WAV_PUNC);

				/* 키값에 ch[5] 인덱스를 int 로 변환하고 10을 더하여 키값을 넣는다. */
				getInstance()->setNoteFromTmpVector(st, bar_no, notes, NOTE::OBJ_NOTE, getInstance()->changeCharToInteger(ch, 5) + 10);

				break;

#pragma endregion
			default:
				break;
			}

#pragma endregion

		}


	}

#ifdef _DEBUG
	CCLOG("*********** %s file read finished! ********", filePath.c_str());
#endif

}