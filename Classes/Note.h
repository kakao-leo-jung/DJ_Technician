#pragma execution_character_set("utf-8")

#include "cocos2d.h"

//USING_NS_CC;

/*

	BMS 의 데이터 영역 정보를 읽어서 저장하는 클래스
	Note 는 키값

*/

namespace NOTE {

	/*

		노트 클래스와 관련된 전역 변수 값

	*/

	/* note_object */
	const char OBJ_EVENT = '0';
	const char OBJ_NOTE = '1';

	/* note_key */
	enum {
		KEY_BGM = 0,
		KEY_1 = 1,
		KEY_2 = 2,
		KEY_3 = 3,
		KEY_4 = 4,
		KEY_5 = 5,
		KEY_SCRATCH = 6,
		KEY_PAD = 7, /* no-use */
		KEY_6 = 8,
		KEY_7 = 9,
		KEY_STOP = 10,
		KEY_TRANS1 = 11,
		KEY_TRANS2 = 12,
		KEY_TRANS3 = 13,
		KEY_TRANS4 = 14,
		KEY_TRANS5 = 15,
		KEY_TRANSSCRATCH = 16,
		KEY_TRANSPAD = 17,
		KEY_TRANS6 = 18,
		KEY_TRANS7 = 19,
		KEY_BPM = 20,
		KEY_BPMEXTEND = 21,
		KEY_BGA = 22
	};

	const std::string STR_KEY[23] = {
		"KEY_BGM",
		"KEY_1",
		"KEY_2",
		"KEY_3",
		"KEY_4",
		"KEY_5",
		"KEY_SCRATCH",
		"KEY_PAD",
		"KEY_6",
		"KEY_7",
		"KEY_STOP",
		"KEY_TRANS1",
		"KEY_TRANS2",
		"KEY_TRANS3",
		"KEY_TRANS4",
		"KEY_TRANS5",
		"KEY_TRANSSCRATCH",
		"KEY_TRANSPAD",
		"KEY_TRANS6",
		"KEY_TRANS7",
		"KEY_BPM",
		"KEY_BPMEXTEND",
		"KEY_BGA"
	};

	const double NOTE_POS_X[23] = {
		282,											// BGM 키 x 축 좌표
		19,												// 1번 키 x 축 좌표
		55,												// 2번 키 x 축 좌표
		92,												// 3번 키 x 축 좌표
		131,											// 4번 키 x 축 좌표
		167,											// 5번 키 x 축 좌표
		-80,											// 스크래치 키 x 축 좌표
		-100,											// 미사용
		205,											// 6번 키 x 축 좌표
		242,											// 7번 키 x 축 좌표
		362,											// STOP 키 x 축 좌표
		410,											// 1번 투명 키 x 축 좌표
		410,											// 2번 투명 키 x 축 좌표
		410,											// 3번 투명 키 x 축 좌표
		410,											// 4번 투명 키 x 축 좌표
		410,											// 5번 투명 키 x 축 좌표
		410,											// 스크래치 투명 키 x 축 좌표
		420,											// 미사용
		410,											// 6번 투명 키 x 축 좌표
		410,											// 7번 투명 키 x 축 좌표
		322,											// BPM 키 x 축 좌표
		322,											// BPM 확장 키 x 축 좌표
		322												// BGA 변경 키 x 축 좌표
	};

	class Note {
	private:

		/* 헤더 변수 */

	public:
		/* 생성자 */
		Note() {}

		/* 소멸자 */
		~Note() {}

		/* 멤버 변수 */

		/*

			object 값이 EVENT 이면 note_key 는 채널 속성을 나타낸다
			 1 : bgm
			 2 : 마디단축           - 마디 단축의 경우 Note 가 아니라 따로 GameScene 내의 배열에 저장한다.
			 3 : bpm 변속
			 4 : bga
			   ...
			 8 : bpm extend 변속
			 9 : stop

			object 값이 NOTE 이면 note_key 는 키 번호를 나타낸다.
			 1 - 5 : 1 - 5번 키
			 6 : 스크래치
			 7 : 페달(사용x)
			 8, 9 : 8, 9번 키

		*/
		char note_object;
		int note_key;

		/*

			노트가 가지고 있는 사운드 채널값이다.
			note_channel은 대개 미리 저장한 배열의 인덱스를 나타낼 수도 있고 값을 직접 가질 수도 있음

			 현재 노트가

			   1. bgm 채널 일때(01)
				- note_channel : str_wav[] 배열의 인덱스를 나타냄 (36진수 2자리를 사용하므로 최대 인덱스는 36*36 = 1296)
				- str_wav[] 는 BMSParser 가 지니고 있는 변수로 #WAVxx 에서 읽어들인 사운드파일의 파일명을 가지고 있음
				- str_wav[] 의 인덱스는 GameScene에서 FMOD Sound[] 실제 로드한 사운드파일의 인덱스와도 일치함.
				- 즉 BGM 을 연주할때 FMOD_sound[note_channel] 로 사운드를 재생하면 됨.

			   2. bpm 변속 채널 일때 (03)
				- note_channel : 바꾸어야할 BPM 값을 지님 (16진수 2자리를 사용하므로 1 - 255 BPM 지님)

			   3. bga 채널 일때 (04)
				-

			   4. bpm extend 채널 일때 (08)
				- note_channel : str_bpm[] 배열의 인덱스를 나타냄 추후 str_bpm[note_channel] 로 바꾸여야할 BPM 값을 구함.

		*/
		int note_channel;

		/*

			해당 노트의 비트값이다.
			비트가 만약 4.5이면 4번째 마디 절반지점에 노트가 위치함을 나타낸다.

		*/

		double note_beat;
		double note_time;
		double note_posy;
		cocos2d::Sprite *sprite;
		bool isChecked;

		/* Operator 연산자 */
		bool operator < (const Note &n) {
			return note_beat < n.note_beat;
		}

	};
}