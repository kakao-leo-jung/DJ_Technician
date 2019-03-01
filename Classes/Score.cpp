#include "Score.h"

using namespace Judge;

/* 노트 개수 늘임 */
void Score::plusNoteSize() {
	noteSize++;
	updateOffset();
}

void Score::updateOffset() {
	double b = 100000.0 / noteSize;
	double v = 50000.0 / (10 * noteSize - 55);
}

/* 주어진 판정시간에 대한 판정을 리턴하고 점수를 산정한다 */
int Score::calculateCombo(double judgeTime) {

	/* 판정 값 */
	int judgeNo;

	/* 점수 산정용 변수 */
	double plusScore = 0;

	if (judgeTime >= -JUDGE[JUDGE::PERFECT] && judgeTime <= JUDGE[JUDGE::PERFECT]) {
		/* 퍼펙트 +- 0.15f 이내 */
		judgeNo = JUDGE::PERFECT;
		currentCombo = (currentCombo == maxCombo)
			? maxCombo = currentCombo + 1 : currentCombo + 1;

		/* 점수 산정 */
		int offset = (currentCombo <= 10) ? currentCombo - 1 : 10;
		plusScore = 1.5 * b + offset * v;

	}
	else if (judgeTime >= -JUDGE[JUDGE::GREAT] && judgeTime <= JUDGE[JUDGE::GREAT]) {
		/* 그레이트 */
		judgeNo = JUDGE::GREAT;
		currentCombo = (currentCombo == maxCombo)
			? maxCombo = currentCombo + 1 : currentCombo + 1;

		/* 점수 산정 */
		int offset = (currentCombo <= 10) ? currentCombo - 1 : 10;
		plusScore = 1.0 * b + offset * v;
	}
	else if (judgeTime >= -JUDGE[JUDGE::GOOD] && judgeTime <= JUDGE[JUDGE::GOOD]) {
		/* 굿 +- 0.3f 이내 */
		judgeNo = JUDGE::GOOD;
		currentCombo = (currentCombo == maxCombo)
			? maxCombo = currentCombo + 1 : currentCombo + 1;

		/* 점수 산정 */
		int offset = (currentCombo <= 10) ? currentCombo - 1 : 10;
		plusScore = 0.2 * b + offset * v;
	}
	else if (judgeTime >= -JUDGE[JUDGE::BAD] && judgeTime <= JUDGE[JUDGE::BAD]) {
		/* 배드 +- 0.4f 이내 */
		judgeNo = JUDGE::BAD;
		currentCombo = 0;
	}
	else if (judgeTime < -JUDGE[JUDGE::BAD]) {
		/* 미스 - 0.4f 초과 */
		judgeNo = JUDGE::MISS;
		currentCombo = 0;
	}

	judgeCount[judgeNo]++;
	currentScore += plusScore;
	currentSize++;

	int ex_score = judgeCount[JUDGE::PERFECT] * 2 + judgeCount[JUDGE::GREAT];
	int max_score = currentSize * 2;
	double level_score = (double)ex_score / max_score;
	if (level_score >= 8.5 / 9) {
		djLevel = 8;
	}
	else {
		djLevel = level_score * 9;
	}

	return judgeNo;
}