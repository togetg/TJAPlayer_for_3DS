#include "header.h"
#include "notes.h"
#include "tja.h"
#include "main.h"
#include "score.h"

bool isGOGO;
int CurrentScore,CurrentRollCount,TotalRollCount,combo,init,diff,DiffMul,scoremode,HitScore,ScoreDiff,BaseCeilingPoint;
double tmp;
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
	BaseCeilingPoint = 0;
}

void score_init_after() {

	CurrentScore = 0;
	CurrentRollCount = 0;
	TotalRollCount = 0;
	combo = 0;
	DiffMul = 0;
	HitScore = 0;
	ScoreDiff = 0;
}

void score_update(int knd) {

	int PreScore = CurrentScore;
	bool isCombo = false;

	if (scoremode == 1 || scoremode == 2) {

		double GOGOMul;
		if (isGOGO == true) GOGOMul = 1.2;
		else GOGOMul = 1.0;

		if (scoremode == 2) {	//新配点
			if (0 <= combo && combo < 9) DiffMul = 0;
			else if (9 <= combo && combo < 29) DiffMul = 1;
			else if (29 <= combo && combo < 49) DiffMul = 2;
			else if (49 <= combo && combo < 99) DiffMul = 4;
			else if (99 <= combo) DiffMul = 8;
		}
		else if (scoremode == 1) {	//旧配点
			DiffMul = (combo+1)/10;
			if (combo > 100) DiffMul = 10;
		}

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

	if (scoremode == 2) {	//100コンボ毎のボーナス(新配点のみ)
		if (isCombo == true && combo % 100 == 0) CurrentScore += 10000;
	}
	ScoreDiff = CurrentScore - PreScore;
}

void scoer_debug() {

	snprintf(buf_score, sizeof(buf_score), "%s %s    %d:%d    %s" ,TJA_Header.title, TJA_Header.subtitle,TJA_Header.course, TJA_Header.level,TJA_Header.wave);
	debug_draw(0, 150, buf_score);
	snprintf(buf_score, sizeof(buf_score), "Scoremode:%d   Init:%d   Diff:%d",TJA_Header.scoremode,init, diff);
	debug_draw(0, 160, buf_score);
	snprintf(buf_score, sizeof(buf_score), "Score:%d    %dCombo    diff:%d",CurrentScore, combo, ScoreDiff);
	debug_draw(0, 180, buf_score);
	if (isGOGO == true) {
		snprintf(buf_score, sizeof(buf_score), "GOGOTIME");
		debug_draw(0, 190, buf_score);
	}
	debug_draw(100, 190, buf_score);
}

void send_gogotime(bool arg) {
	isGOGO = arg;
}

int round_down(int arg) {
	int temp = arg % 10;
	return arg - temp;
}

void calc_base_score(MEASURE_T Measure[Measure_Max], char notes[Measure_Max][Max_Notes_Measure]) {	//初項と公差を計算
	
	int NotesCount = 0, i = 0, combo = 0, DiffTmp = 0, BalloonCnt = 0, TmpBaseCeilingPoint = 0,NotesCountMax = 0,RollCnt=0,RollKnd=0;
	bool isEND = false;
	double init_cnt=0,diff_cnt=0,gogo = 1,special = 1,roll_start_time=0, roll_end_time = 0;
	COMMAND_T Command;

	int level = TJA_Header.level;
	if (level > 10) level = 10;

	switch (TJA_Header.course) {	//基本天井点を設定
	case 0:	//かんたん
		BaseCeilingPoint = 280000 + level * 20000;
		break;
	case 1:	//ふつう
		BaseCeilingPoint = 350000 + level * 50000;
		break;
	case 2:	//むずかしい
		BaseCeilingPoint = 500000 + level * 50000;
		break;
	case 3:	//おに
	case 4:
		if (level == 10) BaseCeilingPoint = 1200000;
		else BaseCeilingPoint = 650000 + level * 50000;
		break;
	}
	TmpBaseCeilingPoint = BaseCeilingPoint;

	if ((TJA_Header.scoreinit == -1 || TJA_Header.scorediff == -1) && (scoremode == 1 || scoremode == 2)) {	//新配点と旧配点

		while (isEND == false && i < Measure_Max && Measure[i].flag == true) {	//小節

			NotesCount = 0;

			if (NotesCount == 0 && notes[i][0] == '#') {

				get_command_value(notes[i], &Command);

				switch (Command.knd) {
				case GOgostart:
					gogo = 1.2;
					break;
				case GOgoend:
					gogo = 1.0;
					break;
				case ENd:
					isEND = true;
					break;
				default:
					break;

				}
				NotesCount = 0;
				i++;
				continue;
			}

			while (notes[i][NotesCount] != ',' && notes[i][NotesCount] != '\n' && notes[i][NotesCount] != '/') {

				NotesCount++;
			}
			if (Measure[i].firstmeasure != -1) NotesCountMax = Measure[Measure[i].firstmeasure].max_notes;
			else NotesCountMax = NotesCount;

			for (int j = 0; j < NotesCount; j++) {	//ノーツ

				int knd = ctoi(notes[i][j]);

				if (knd != 0) {

					if (knd == Don || knd == Ka || knd == BigDon || knd == BigKa) {
						if (knd == BigDon || knd == BigKa) special = 2.0;
						else special = 1.0;
						combo++;
						init_cnt += 1 * gogo * special;

						if (scoremode == 1) {		//旧配点

							if (combo > 100) DiffTmp = 10;
							else DiffTmp = combo / 10;
						}
						else if (scoremode == 2) {	//新配点

							if (combo >= 1 && combo <= 9) DiffTmp = 0;
							else if (combo >= 10 && combo <= 29) DiffTmp = 1;
							else if (combo >= 30 && combo <= 49) DiffTmp = 2;
							else if (combo >= 50 && combo <= 99) DiffTmp = 4;
							else if (combo >= 100) DiffTmp = 8;
						}

						diff_cnt += DiffTmp * gogo * special;
					}
					else if (knd == Balloon) {	//風船

						TmpBaseCeilingPoint -= (TJA_Header.balloon[BalloonCnt] * 300 + 5000) * gogo;
						BalloonCnt++;
					}
					else if (knd == Roll) { //連打
					
						roll_start_time = Measure[i].judge_time + 60.0 / Measure[i].bpm * 4 * Measure[i].measure * i / NotesCountMax;
						RollKnd = Roll;
					}
					else if (knd == BigRoll) { //大連打

						roll_start_time = Measure[i].judge_time + 60.0 / Measure[i].bpm * 4 * Measure[i].measure * i / NotesCountMax;
						RollKnd = BigRoll;
					}
					else if (knd == RollEnd) {

						if (roll_start_time != 0) {

							roll_end_time = Measure[i].judge_time + 60.0 / Measure[i].bpm * 4 * Measure[i].measure * i / NotesCountMax;
							RollCnt = (int)((roll_end_time - roll_start_time) / (1.0 / 12.0));

							if (RollKnd == Roll) {
								if (gogo == true) TmpBaseCeilingPoint -= RollCnt * 360;
								else TmpBaseCeilingPoint -= RollCnt * 300;
							}
							else if (RollKnd == BigRoll) {
								if (gogo == true) TmpBaseCeilingPoint -= RollCnt * 430 * gogo;
								else TmpBaseCeilingPoint -= RollCnt * 360 * gogo;
							}
							roll_start_time = 0;
							roll_end_time = 0;
							RollCnt = 0;
						}
					}
				}
			}
			i++;
		}
		diff = (TmpBaseCeilingPoint - (int)(combo / 100) * 10000) / (init_cnt * 4 + diff_cnt);
		init = diff * 4;
	}
	//score_init_after();
}