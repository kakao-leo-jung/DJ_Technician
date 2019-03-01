#ifndef SCORE_H
#define SCORE_H

#pragma execution_character_set("utf-8")

#include "cocos2d.h"

USING_NS_CC;

/*

	게임에서 진행되는 유저 점수를 정의하는 클래스

*/

namespace Judge {
	const float JUDGE[5] = {
	0.021f, 0.084f, 0.147f, 0.21f, 0.273f
	};													// 판정 라인
	const std::string JUDGE_STR[5] = {
		"PERFECT", "GREAT",  "GOOD", "BAD", "MISS"
	};													// 판정 STRING 값
	enum JUDGE {
		PERFECT, GREAT, GOOD, BAD, MISS
	};
	const std::string DJLEVEL_STR[9] = {
		"F", "E", "D", "C", "B", "A", "AA", "AAA", "S"
	};
	const Color3B DJLEVEL_COLOR[9] = {
		Color3B(50,50,50), Color3B(100, 100, 100), Color3B(150,150,150),
		Color3B(200,200,200), Color3B(200,200,0), Color3B(255,150,0),
		Color3B(255,100,0), Color3B(255, 50, 0), Color3B(255, 0, 0)
	};
	enum DJLEVEL {
		RANK_F, RANK_E, RANK_D, RANK_C, RANK_B, RANK_A, RANK_AA, RANK_AAA, RANK_S
	};
}

class Score {
private:

	/* 멤버 변수 */
	int currentScore;								// 누적 점수
	int currentCombo;								// 현재 콤보
	int maxCombo;									// 최대 콤보
	int judgeCount[5];								// 누적 판정
	int currentSize;								// 현재 누적 노트 수
	int noteSize;									// 전체 노트 개수(Max 콤보 수)
	int djLevel;									// 현재 DJ Level	

	/* 점수 산정 offset */
	double b;
	double v;

public:
	/* 생성자 */
	Score() {}

	/* 소멸자 */
	~Score() {}

	/* func */
	void plusNoteSize();							// 전체 노트 세는 과정에서 하나씩 더한다.
	int calculateCombo(double judgeTime);			// 주어진 판정시간을 기준으로 판정점수를 갱신하고 판정값을 리턴한다
	void updateOffset();

	/* getter */
	int getCurrentScore() {
		return currentScore;
	}
	int getCurrentCombo() {
		return currentCombo;
	}
	int getMaxCombo() {
		return maxCombo;
	}
	int getjudgeCount(int idx) {
		return (idx >= 5 || idx < 0) ? -1:judgeCount[idx];
	}
	int getCurrentSize() {
		return currentSize;
	}
	int getDjLevel() {
		return djLevel;
	}
	int getNoteSize() {
		return noteSize;
	}
	double getB() {
		return b;
	}
	double getV() {
		return v;
	}

	/* setter */
	void setCurrentScore(int currentScore) {
		this->currentScore = currentScore;
	}
	void setCurrentCombo(int currentCombo) {
		this->currentCombo = currentCombo;
	}
	void setMaxCombo(int maxCombo) {
		this->maxCombo = maxCombo;
	}
	void setJudgeCount(int idx, int judgeCount) {
		if (idx >= 5 || idx < 0) {
			return;
		}
		this->judgeCount[idx] = judgeCount;
	}
	void setCurrentSize(int currentSize) {
		this->currentSize = currentSize;
	}
	void setDjLevel(int djLevel) {
		this->djLevel = djLevel;
	}
	void setNoteSize(int noteSize) {
		this->noteSize = noteSize;
	}
	void setB(double b) {
		this->b = b;
	}
	void setV(double v) {
		this->v = v;
	}

};


#endif