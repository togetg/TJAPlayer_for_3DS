#include "header.h"
#include "notes.h"
#include "tja.h"
#include "main.h"
#include "score.h"

bool isGOGO;
int combo,init,diff,DiffMul,scoremode,HitScore,ScoreDiff,BaseCeilingPoint,
CurrentScore,TotalScore,CurrentTotalRollCount, CurrentRollCount, TotalRollCount,TotalPerfectCount,
TotalNiceCount,TotalBadCount, CurrentPerfectCount, CurrentNiceCount, CurrentBadCount, CurrentBalloonCount;
double tmp,Precision,CurrentPrecision;
TJA_HEADER_T TJA_Header;
char buf_score[160];
GAUGE_T Gauge;

void init_guage_structure() {

	Gauge.perfect = 0;
	Gauge.nice = 0;
	Gauge.bad = 0;
	Gauge.score = 0;
	Gauge.norma = 0;
	Gauge.soul = 0;
}

void init_score() {

	combo = 0;
	get_tja_header(&TJA_Header);
	init = TJA_Header.scoreinit;
	diff = TJA_Header.scorediff;
	DiffMul = 0;
	scoremode = TJA_Header.scoremode;
	HitScore = 0;
	ScoreDiff = 0;
	BaseCeilingPoint = 0;
	CurrentScore = 0;
	TotalScore = 0;
	CurrentRollCount = 0;
	CurrentTotalRollCount = 0;
	TotalRollCount = 0;
	TotalPerfectCount = 0;
	TotalNiceCount = 0;
	TotalBadCount = 0;
	CurrentPerfectCount = 0;
	CurrentNiceCount = 0;
	CurrentBadCount = 0;
	Precision = 0;
	CurrentPrecision = 0;
	CurrentBalloonCount = 0;
	init_guage_structure();
}


void score_update(int knd) {

	int PreScore = TotalScore;
	bool isCombo = false;

	//if (scoremode == 1 || scoremode == 2) {

		double GOGOMul;
		if (isGOGO == true) GOGOMul = 1.2;
		else GOGOMul = 1.0;

		if (scoremode == 0) {	//ドンダフル配点
			if (combo < 200) DiffMul = 0;
			else DiffMul = 1;
		}
		else if (scoremode == 2) {	//新配点
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

		case PERFECT:
			TotalScore += round_down(HitScore*GOGOMul);
			CurrentScore += round_down(HitScore*GOGOMul);
			combo++;
			isCombo = true;
			TotalPerfectCount++;
			CurrentPerfectCount++;
			Gauge.score += Gauge.perfect;
			break;

		case SPECIAL_PERFECT:
			TotalScore += round_down(HitScore * GOGOMul) * 2;
			CurrentScore += round_down(HitScore * GOGOMul) * 2;
			combo++;
			isCombo = true;
			TotalPerfectCount++;
			CurrentPerfectCount++;
			Gauge.score += Gauge.perfect;
			break;

		case NICE:
			TotalScore += round_down(HitScore / 2);
			CurrentScore += round_down(HitScore / 2);
			combo++;
			isCombo = true;
			TotalNiceCount++;
			CurrentNiceCount++;
			Gauge.score += Gauge.nice;
			break;

		case SPECIAL_NICE:
			TotalScore += round_down(HitScore - 10);
			CurrentScore += round_down(HitScore - 10);
			combo++;
			isCombo = true;
			TotalNiceCount++;
			CurrentNiceCount++;
			Gauge.score += Gauge.nice;
			break;

		case BAD:
			combo = 0;
			TotalBadCount++;
			CurrentBadCount++;
			Gauge.score -= Gauge.bad;
			break;

		case THROUGH:
			combo = 0;
			TotalBadCount++;
			CurrentBadCount++;
			Gauge.score -= Gauge.bad;
			break;

		case BALLOON:
		case ROLL:
			if (scoremode == 0 || scoremode == 1) {	//旧配点
				if (isGOGO == true) {
					TotalScore += 120;
					CurrentScore += 120;
				}
				else {
					TotalScore += 100;
					CurrentScore += 100;
				}
			}
			else if (scoremode == 2) {	//新配点
				if (isGOGO == true) {
					TotalScore += 360;
					CurrentScore += 360;
				}
				else {
					TotalScore += 300;
					CurrentScore += 300;
				}
			}

			if (knd == ROLL) {
				CurrentRollCount++;
				CurrentTotalRollCount++;
				TotalRollCount++;
			}
			break;

		case BIG_ROLL:
			if (scoremode == 0 || scoremode == 1) {	//旧配点
				if (isGOGO == true) {
					TotalScore += 430;
					CurrentScore += 430;
				}
				else {
					TotalScore += 360;
					CurrentScore += 360;
				}
			}
			else if (scoremode == 2) {	//新配点
				if (isGOGO == true) {
					TotalScore += 240;
					CurrentScore += 240;
				}
				else {
					TotalScore += 200;
					CurrentScore += 200;
				}
			}
			CurrentTotalRollCount++;
			TotalRollCount++;
			CurrentRollCount++;
			break;

		case BALLOON_BREAK:
			if (isGOGO == true) {
				TotalScore += 6000;
				CurrentScore += 6000;
			}
			else {
				TotalScore += 5000;
				CurrentScore += 5000;
			}
			break;

		case ROLL_END:
			CurrentRollCount = 0;
			break;

		default:
			break;
		}
	//}

	if (Gauge.score < 0) Gauge.score = 0;

	if (scoremode == 2) {	//100コンボ毎のボーナス(新配点のみ)
		if (isCombo == true && combo % 100 == 0) {
			TotalScore += 10000;
			CurrentScore += 10000;
		}
	}
	ScoreDiff = TotalScore - PreScore;
	if ((TotalPerfectCount + TotalNiceCount + TotalBadCount) != 0) Precision = (double)TotalPerfectCount / (TotalPerfectCount + TotalNiceCount + TotalBadCount)*100.0;
	else Precision = 0;
	if ((CurrentPerfectCount + CurrentNiceCount + CurrentBadCount) != 0) CurrentPrecision = (double)CurrentPerfectCount / (CurrentPerfectCount + CurrentNiceCount + CurrentBadCount)*100.0;
	else CurrentPrecision = 0;
}

void score_debug() {

	snprintf(buf_score, sizeof(buf_score), "Scoremode:%d   Init:%d   Diff:%d", TJA_Header.scoremode, init, diff);
	draw_debug(0, 10, buf_score);
	snprintf(buf_score, sizeof(buf_score), "%s %s    Course:%d    Level:%d    %s" ,TJA_Header.title, TJA_Header.subtitle,TJA_Header.course, TJA_Header.level,TJA_Header.wave);
	draw_debug(0, 30, buf_score);
	snprintf(buf_score, sizeof(buf_score), "Score:%d    %dCombo    diff:%d",TotalScore, combo, ScoreDiff);
	draw_debug(0, 150, buf_score);
	snprintf(buf_score, sizeof(buf_score), "Current   Score:%d    Roll:%d    Precision:%.1f", CurrentScore, CurrentTotalRollCount, CurrentPrecision);
	draw_debug(0, 160, buf_score);
	snprintf(buf_score, sizeof(buf_score), "良:%d 可:%d 不可:%d ゲージ%d",Gauge.perfect,Gauge.nice,Gauge.bad,Gauge.score);
	draw_debug(0, 170, buf_score);
	if (isGOGO == true) {
		snprintf(buf_score, sizeof(buf_score), "GOGOTIME");
		draw_debug(0, 190, buf_score);
	}
}

void draw_score(C2D_Sprite  sprites[Sprite_Number]) {

	//スコア
	for (int i = 0; i < 7; i++) {

		if (TotalScore / (int)pow(10, i) > 0) {
			int n = TotalScore / (int)pow(10, i) % 10;
			C2D_SpriteSetPos(&sprites[sCore_0 + n], 80 - i * 10, 70);
			C2D_DrawSprite(&sprites[sCore_0 + n]);
		}
	}

	int j;

	//コンボ
	for (j = 0; j < 5; j++) {
		if (combo / (int)pow(10, j) == 0) break;
	}
	for (int i = 0; i < 4; i++) {

		if (combo >= 10 && combo / (int)pow(10, i) > 0) {
			int n = combo / (int)pow(10, i) % 10;
			C2D_SpriteSetPos(&sprites[cOmbo_0 + n],  22 + j*8  - i * 16, 110);
			C2D_DrawSprite(&sprites[cOmbo_0 + n]);
		}
	}

	//連打
	for (j = 0; j < 4; j++) {
		if (CurrentRollCount / (int)pow(10, j) == 0) break;
	}
	if (CurrentRollCount > 0) {
		C2D_SpriteSetPos(&sprites[rOll_count], 110, 35);
		C2D_DrawSprite(&sprites[rOll_count]);
	}
	for (int i = 0; i < 4; i++) {

		if (CurrentRollCount / (int)pow(10, i) > 0) {

			int n = CurrentRollCount / (int)pow(10, i) % 10;
			C2D_SpriteSetPos(&sprites[rOll_0 + n], 95 + j * 10 - i * 20,30);
			C2D_DrawSprite(&sprites[rOll_0 + n]);
		}
	}

	//風船
	for (j = 0; j < 4; j++) {
		if (CurrentBalloonCount / (int)pow(10, j) == 0) break;
	}
	if (CurrentBalloonCount > 0) {
		C2D_SpriteSetPos(&sprites[bAlloon_count], 110, 35);
		C2D_DrawSprite(&sprites[bAlloon_count]);
	}
	for (int i = 0; i < 4; i++) {

		if (CurrentBalloonCount / (int)pow(10, i) > 0) {

			int n = CurrentBalloonCount / (int)pow(10, i) % 10;
			C2D_SpriteSetPos(&sprites[rOll_0 + n], 97 + j * 10 - i * 20, 30);
			C2D_DrawSprite(&sprites[rOll_0 + n]);
		}
	}
}

void draw_gauge(C2D_Sprite  sprites[Sprite_Number]) {
	
	double gauge = 1.0 * (Gauge.score/200)*200 / Gauge.soul;
	if (gauge > 1.0) gauge = 1.0;

	//赤
	C2D_DrawRectSolid(123, 76, 0, 250.0*Gauge.norma/Gauge.soul, 8, C2D_Color32f(102.0/255, 0, 0, 1) );
	C2D_DrawRectSolid(123, 76, 0, 250.0*gauge, 8, C2D_Color32f(1, 0, 0, 1));

	//黄
	C2D_DrawRectSolid(123 + 250.0*Gauge.norma / Gauge.soul, 67, 0, 250 - 250.0*Gauge.norma / Gauge.soul, 17, C2D_Color32f(102.0 / 255, 68.0 / 255, 0, 1));
	if (250 * gauge - (250.0*Gauge.norma / Gauge.soul) >= 0)
	C2D_DrawRectSolid(123 + 250.0*Gauge.norma / Gauge.soul, 67, 0, 250 * gauge - (250.0*Gauge.norma / Gauge.soul), 17, C2D_Color32f(1, 1, 12.0/255, 1));

	//魂
	if ((Gauge.score/200)*200 >= Gauge.soul) C2D_DrawSprite(&sprites[sOul_on]);
	else C2D_DrawSprite(&sprites[sOul_off]);
}

void draw_lane(C2D_Sprite  sprites[Sprite_Number]) {


	if (get_isBranch() == true) {

		int branch = get_branch_course();

		switch (branch) {
		case N:
		default:
			C2D_SpriteSetPos(&sprites[cHart_normal], 350, 110);
			C2D_DrawSprite(&sprites[cHart_normal]);
			break;

		case E:
			C2D_SpriteSetPos(&sprites[lAne_expert], 233, 109);
			C2D_DrawSprite(&sprites[lAne_expert]);
			C2D_SpriteSetPos(&sprites[cHart_expert], 350, 110);
			C2D_DrawSprite(&sprites[cHart_expert]);
			break;

		case M:
			C2D_SpriteSetPos(&sprites[lAne_master], 233, 109);
			C2D_DrawSprite(&sprites[lAne_master]);
			C2D_SpriteSetPos(&sprites[cHart_master], 350, 110);
			C2D_DrawSprite(&sprites[cHart_master]);
			break;
		}
	}

	C2D_SpriteSetPos(&sprites[jUdge_circle], Notes_Judge, 109);
	C2D_DrawSprite(&sprites[jUdge_circle]);

	if (isGOGO == true) {
		C2D_ImageTint Tint;
		C2D_AlphaImageTint(&Tint, 0.8);
		C2D_DrawSpriteTinted(&sprites[eFfect_gogo], &Tint);
	}
}

int branch_start(int knd,double x,double y) {	//分岐

	int branch;
	switch (knd) {
	case 0:	//連打
		if (y <= CurrentTotalRollCount) branch = M;
		else if (x <= CurrentTotalRollCount) branch = E;
		else branch = N;
		break;
	case 1:	//精度
		if (y <= CurrentPrecision) branch = M;
		else if (x <= CurrentPrecision) branch = E;
		else branch = N;
		break;
	case 2:	//スコア
		if (y <= CurrentScore) branch = M;
		else if (x <= CurrentScore) branch = E;
		else branch = N;
		break;
	default:
		branch = N;
		break;
	}
	return branch;
}

void init_branch_section() {	//#SECTION

	CurrentTotalRollCount = 0;
	CurrentPerfectCount = 0;
	CurrentNiceCount = 0;
	CurrentBadCount = 0;
	CurrentScore = 0;
	CurrentPrecision = 0;
}

void send_gogotime(bool arg) {
	isGOGO = arg;
}

int round_down(int arg) {
	int temp = arg % 10;
	return arg - temp;
}

void calc_base_score(MEASURE_T Measure[Measure_Max], char notes[Measure_Max][Max_Notes_Measure]) {	//初項と公差を計算　魂ゲージの伸びも
	
	int NotesCount = 0, i = 0, combo = 0, DiffTmp = 0, BalloonCnt = 0, TmpBaseCeilingPoint = 0,NotesCountMax = 0,RollCnt=0,RollKnd=0;
	bool isEND = false;
	double init_cnt=0,diff_cnt=0,gogo = 1,special = 1,roll_start_time=0, roll_end_time = 0;
	COMMAND_T Command;

	int PerfectNotesCount = 0;	//魂ゲージの伸び計算用

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


	while (isEND == false && i < Measure_Max && Measure[i].flag == true) {	//小節

		NotesCount = 0;

		if (Measure[i].branch != -1 && Measure[i].branch != M) {

			i++;
			continue;
		}

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

				if (knd == Don || knd == Katsu || knd == BigDon || knd == BigKatsu) {
					if (knd == BigDon || knd == BigKatsu) special = 2.0;
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

					PerfectNotesCount++;
				}
				else if (knd == Balloon) {		//風船

					TmpBaseCeilingPoint -= (TJA_Header.balloon[BalloonCnt] * 300 + 5000) * gogo;
					BalloonCnt++;
				}
				else if (knd == Roll) {			//連打

					roll_start_time = Measure[i].judge_time + 60.0 / Measure[i].bpm * 4 * Measure[i].measure * i / NotesCountMax;
					RollKnd = Roll;
				}
				else if (knd == BigRoll) {		//大連打

					roll_start_time = Measure[i].judge_time + 60.0 / Measure[i].bpm * 4 * Measure[i].measure * i / NotesCountMax;
					RollKnd = BigRoll;
				}
				else if (knd == RollEnd) {

					if (roll_start_time != 0) {

						roll_end_time = Measure[i].judge_time + 60.0 / Measure[i].bpm * 4 * Measure[i].measure * i / NotesCountMax;
						RollCnt = (int)((roll_end_time - roll_start_time) / (1.0 / 12.0));

						if (RollKnd == Roll) {
							if (scoremode == 1) {
								if (gogo == true) TmpBaseCeilingPoint -= RollCnt * 360;
								else TmpBaseCeilingPoint -= RollCnt * 300;
							}
							if (scoremode == 2) {
								if (gogo == true) TmpBaseCeilingPoint -= RollCnt * 120;
								else TmpBaseCeilingPoint -= RollCnt * 100;
							}
						}
						else if (RollKnd == BigRoll) {
							if (scoremode == 1) {
								if (gogo == true) TmpBaseCeilingPoint -= RollCnt * 430 * gogo;
								else TmpBaseCeilingPoint -= RollCnt * 360 * gogo;
							}
							if (scoremode == 1) {
								if (gogo == true) TmpBaseCeilingPoint -= RollCnt * 240 * gogo;
								else TmpBaseCeilingPoint -= RollCnt * 200 * gogo;
							}
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


	if ((TJA_Header.scoreinit == -1 || TJA_Header.scorediff == -1) && (scoremode == 1 || scoremode == 2)) {	//新配点と旧配点
		diff = (TmpBaseCeilingPoint - (int)(combo / 100) * 10000) / (init_cnt * 4 + diff_cnt);
		init = diff * 4;
	}
	else if (scoremode == 0) {
		init = 1000;
		diff = 1000;
	}

	Gauge.perfect = 10000 / PerfectNotesCount;

	switch (TJA_Header.course) {	//ゲージの伸び率の計算
	case 0:		//かんたん
		Gauge.nice = Gauge.perfect * 3 / 4;
		Gauge.bad = Gauge.perfect / 2;
		if (level <= 1) {
			Gauge.norma = 3600;
			Gauge.soul = 6000;
		}
		else if (level <= 3) {
			Gauge.norma = 3800;
			Gauge.soul = 6333;
		}
		else if (level >= 4) {
			Gauge.norma = 4400;
			Gauge.soul = 7333;
		}
		break;

	case 1:		//ふつう
		Gauge.nice = Gauge.perfect * 3 / 4;
		if (level <= 2) {
			Gauge.bad = Gauge.perfect / 2;
			Gauge.norma = 4595;
			Gauge.soul = 6560;
		}
		else if (level == 3) {
			Gauge.bad = Gauge.perfect / 2;
			Gauge.norma = 4868;
			Gauge.soul = 6955;
		}
		else if (level == 4) {
			Gauge.bad = Gauge.perfect * 3 / 4;
			Gauge.norma = 4925;
			Gauge.soul = 7035;
		}
		else if (level >= 5) {
			Gauge.bad = Gauge.perfect;
			Gauge.norma = 5250;
			Gauge.soul = 7500;
		}
		break;

	case 2:		//むずかしい
		Gauge.nice = Gauge.perfect * 3 / 4;
		if (level <= 2) {
			Gauge.bad = Gauge.perfect / 2;
			Gauge.norma = 5450;
			Gauge.soul = 7750;
		}
		else if (level == 3) {
			Gauge.bad = Gauge.perfect;
			Gauge.norma = 5080;
			Gauge.soul = 7250;
		}
		else if (level <= 4) {
			Gauge.bad = Gauge.perfect * 7 / 6;
			Gauge.norma = 4840;
			Gauge.soul = 6910;
		}
		else if (level <= 5) {
			Gauge.bad = Gauge.perfect * 5 / 4;
			Gauge.norma = 4724;
			Gauge.soul = 6750;
		}
		else if (level >= 6) {
			Gauge.bad = Gauge.perfect * 5 / 4;
			Gauge.norma = 4812;
			Gauge.soul = 6875;
		}
		break;

	case 3:		//おに
	default:
		Gauge.nice = Gauge.perfect / 2;
		if (level <= 7) {
			Gauge.bad = Gauge.perfect  * 8/ 5;
			Gauge.norma = 5660;
			Gauge.soul = 7075;
		}
		else if (level == 8) {
			Gauge.bad = Gauge.perfect * 2;
			Gauge.norma = 5600;
			Gauge.soul = 7000;
		}
		else if (level >= 9) {
			Gauge.bad = Gauge.perfect * 2;
			Gauge.norma = 6000;
			Gauge.soul = 7500;
		}
		break;
	}
	//init_score_after();
}

void balloon_count_update(int arg) {
	CurrentBalloonCount = arg;
}