#include "header.h"
#include "notes.h"
#include "tja.h"
#include "main.h"
#include "score.h"

bool isGOGO;
int CurrentScore,CurrentRollCount,TotalRollCount,combo,init,diff,DiffMul,scoremode,HitScore,ScoreDiff;
TJA_HEADER_T TJA_Header;
char buf_score[160];

void score_init() {

	CurrentScore = 0;
	CurrentRollCount = 0;
	TotalRollCount = 0;
	combo = 0;
	get_tja_header(&TJA_Header);
	init = TJA_Header.scoreinit;
	diff = TJA_Header.scorediff;
	DiffMul = 0;
	scoremode = TJA_Header.scoremode;
	HitScore = 0;
	ScoreDiff = 0;
}


void score_update(int knd) {

	int PreScore = CurrentScore;
	bool isCombo = false;



	if (scoremode == 2) {

		double GOGOMul;
		if (isGOGO == true) GOGOMul = 1.2;
		else GOGOMul = 1.0;

		if (0 <= combo && combo < 9) DiffMul = 0;
		else if (9 <= combo && combo < 29) DiffMul = 1;
		else if (29 <= combo && combo < 49) DiffMul = 2;
		else if (49 <= combo && combo < 99) DiffMul = 4;
		else if (99 <= combo) DiffMul = 8;

		HitScore = init + diff * DiffMul;

		switch (knd) {

		case RYOU:
			CurrentScore += round_down(HitScore*GOGOMul);
			combo++;
			isCombo = true;
			break;

		case SPECIAL_RYOU:
			CurrentScore += round_down(HitScore * GOGOMul) * 2;
			combo++;
			isCombo = true;
			break;

		case KA:
			CurrentScore += round_down(HitScore / 2);
			combo++;
			isCombo = true;
			break;

		case SPECIAL_KA:
			CurrentScore += round_down(HitScore - 10);
			combo++;
			isCombo = true;
			break;

		case FUKA:
			CurrentScore += 0;
			combo = 0;
			break;

		case THROUGH:
			combo = 0;
			break;

		case BALLOON:
		case ROLL:
			if (isGOGO == true) CurrentScore += 360;
			else CurrentScore += 300;
			break;

		case BIG_ROLL:
			if (isGOGO == true) CurrentScore += 430;
			else CurrentScore += 360;
			break;

		case BALLOON_BREAK:
			if (isGOGO == true) CurrentScore += 6000;
			else CurrentScore += 5000;
			break;
			break;

		default:
			break;
		}
	}

	if (isCombo == true && combo % 100 == 0) CurrentScore += 10000;
	ScoreDiff = CurrentScore - PreScore;
}

void scoer_debug() {

	snprintf(buf_score, sizeof(buf_score), "%s %s    %d:%d    %s" ,TJA_Header.title, TJA_Header.subtitle,TJA_Header.course, TJA_Header.level,TJA_Header.wave);
	debug_draw(0, 150, buf_score);
	snprintf(buf_score, sizeof(buf_score), "Scoremode:%d   Init:%d   Diff:%d",TJA_Header.scoremode,TJA_Header.scoreinit, TJA_Header.scorediff);
	debug_draw(0, 160, buf_score);
	snprintf(buf_score, sizeof(buf_score), "Score:%d    %dCombo    diff:%d",CurrentScore, combo, ScoreDiff);
	debug_draw(0, 180, buf_score);
	if (isGOGO == true) {
		snprintf(buf_score, sizeof(buf_score), "GOGOTIME");
		debug_draw(0, 190, buf_score);
	}
}

void send_gogotime(bool arg) {
	isGOGO = arg;
}

int round_down(int arg) {
	int temp = arg % 10;
	return arg - temp;
}