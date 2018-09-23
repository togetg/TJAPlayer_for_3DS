#include "header.h"
#include "notes.h"
#include "tja.h"
#include "main.h"
#include "score.h"

int CurrentScore,CurrentRollCount,TotalRollCount,combo,init,diff,DiffMul,HitScore;
TJA_HEADER_T TJA_Header;
char buf_score[160];

void score_init() {

	CurrentScore = 0;
	combo = 0;
	get_tja_header(&TJA_Header);
	init = TJA_Header.scoreinit;
	diff = TJA_Header.scorediff;
}

void score_update(int knd) {

	int temp;

	if (0 <= combo && combo <= 9) DiffMul = 0;
	else if (10 <= combo && combo <= 29) DiffMul = 1;
	else if (30 <= combo && combo <= 49) DiffMul = 1;
	else if (30 <= combo && combo <= 49) DiffMul = 2;
	else if (50 <= combo && combo <= 99) DiffMul = 4;
	else if (100 <= combo) DiffMul = 8;

	HitScore = init + diff * DiffMul;

	switch (knd) {

	case RYOU:
		CurrentScore += HitScore;
		combo++;
		break;

	case KA:
		temp = HitScore / 20;
		temp *= 10;
		CurrentScore += temp;
		combo++;
		break;

	case FUKA:
		CurrentScore += 0;
		combo = 0;
		break;

	case THROUGH:
		combo = 0;
		break;

	default:
		break;
	}
}

void scoer_debug() {

	snprintf(buf_score, sizeof(buf_score), "%s %s    %d:%d    %s" ,TJA_Header.title, TJA_Header.subtitle,TJA_Header.course, TJA_Header.level,TJA_Header.wave);
	debug_draw(0, 150, buf_score);
	snprintf(buf_score, sizeof(buf_score), "Scoremode:%d   Init:%d   Diff:%d",TJA_Header.scoremode,TJA_Header.scoreinit, TJA_Header.scorediff);
	debug_draw(0, 160, buf_score);
	snprintf(buf_score, sizeof(buf_score), "%dCombo    Score:%d    HitScore:%d:%d",combo,CurrentScore, HitScore, DiffMul);
	debug_draw(0, 180, buf_score);
}