#include <limits.h>
#include "header.h"
#include "main.h"
#include "time.h"
#include "audio.h"
#include "tja.h"
#include "notes.h"

#define Notes_Area 400.0
#define Notes_Judge 93
#define Notes_Max 512

char buf_notes[160];

int notes_id_check(); 
void notes_calc(bool isDon, bool isKa,double bpm, double tempo, double NowTime, int cnt, C2D_Sprite sprites[12]);

typedef struct {
	int num, notes_max;
	double x_ini, x, create_time,judge_time,bpm;
	int kind;
	bool flag=false;
	int sec;
	C2D_Sprite spr;
} NOTES_T;
NOTES_T Notes[512];
COMMAND_T Command;

int bpm_count = 0;
int bpm_count2 = 1;	//次のbpmの時間計測用
int sec_count = 0;
int notes_main_count = 0;
int renda_flag = 0;
int notes_count;	
int notes_number = 0;	//何番目のノーツか　要初期化
double bar_x;
double speed;
double bpm_time;
double tempo_time[2], sec_time;
double SecTime[4] = { 0,0,0,0 };
bool bpm_tempo_flag = false, notes_load_flag = true;


int ctoi(char c) {

	switch (c) {
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	default : return 0;
	}
}


void notes_init() {
	Command.data[0] = 0; Command.data[1] = 0; Command.data[2] = 0;
	Command.knd = 0;
	Command.val = 0;
}

void notes_main(bool isDon,bool isKa, char tja_notes[2048][Max_Notes_Section], int cnt, char *tja_title, char *tja_subtitle, char *tja_level, char *tja_bpm, char *tja_wave, char *tja_offset, char *tja_balloon, char *tja_songvol, char *tja_sevol, char *tja_scoreinit, char *tja_scorediff, char *tja_course, char *tja_style, char *tja_game, char *tja_life, char *tja_demostart, char *tja_side, char *tja_scoremode, C2D_Sprite  sprites[12]) {

	double bpm = atof(tja_bpm);
	double tempo = 4;
	double NowTime = time_now(0);
	snprintf(buf_notes, sizeof(buf_notes), "NowTime0:%.2f", NowTime);
	debug_draw(0, 10, buf_notes);
	if (notes_main_count == 0) bpm_time = 60.0 / bpm * bpm_count2;

	//sftd_draw_textf(font, 100, 20, RGBA8(0, 255, 0, 255), 10, "BpmTime:%f", bpm_time);
	//sftd_draw_textf(font, 100, 30, RGBA8(0, 255, 0, 255), 10, "BpmCount2:%d", bpm_count2);

	if (bpm_time <= NowTime && cnt >= 0 && notes_load_flag == true) {
		
		bpm_count2++;
		bpm_time = 60.0 / bpm * bpm_count2;

		if (bpm_count % 4 == 0) {

			switch (sec_count) {
			case 0:
				SecTime[0] = NowTime;
				break;
			case 1:
				SecTime[1] = NowTime;
				break;
			case 2:
				SecTime[2] = NowTime;
				break;
			case 3:
				SecTime[3] = NowTime;
				break;
			}
			bpm_tempo_flag = true;
			tempo_time[0] = NowTime;
			notes_count = 0;

			if (sec_count == 0) sec_time = NowTime;
			
			while (tja_notes[sec_count][notes_count] != ',') {

				//命令
				if (notes_count == 0 && tja_notes[sec_count][0] == '#') {

					bool isEnd = false;
					get_command_value(tja_notes[sec_count],&Command);
					Command.notes = tja_notes[sec_count];

					switch (Command.knd) {
					case End:
						notes_load_flag = false;						
						isEnd = true;
						break;

					case Bpmchange:
						break;

					default:
						break;

					}

					if (isEnd == true) break;


					notes_count = 0;
					sec_count++;
					continue;
				}
				if (notes_count == 0 && strstr(tja_notes[sec_count], ",") == NULL) {
					notes_count = 0;
					sec_count++;
					continue;
				}
				notes_count++;
			}

			for (int i = 0; i < notes_count; i++) {

				int id = notes_id_check();
				
				if (id != -1 && tja_notes[sec_count][i] != '0') {

					speed = Notes_Area / (3600 / bpm * tempo);

					if (renda_flag == 0 || tja_notes[sec_count][i] == '8') {

						Notes[id].flag = true;
						Notes[id].notes_max = notes_count;	//用途不明
						Notes[id].num = notes_number;
						Notes[id].x_ini = (Notes_Area / notes_count)*i + 400.0;
						Notes[id].x = Notes[id].x_ini;
						Notes[id].bpm = bpm;
						Notes[id].kind = ctoi(tja_notes[sec_count][i]);
						Notes[id].sec = sec_count;
						Notes[id].create_time = NowTime;
						Notes[id].judge_time = Notes[id].create_time + (Notes[id].x_ini - Notes_Judge) / (Notes_Area / (60 / bpm * tempo));
						Notes[id].bpm = bpm;

						switch (Notes[id].kind) {

						case Renda:
							renda_flag = 1;
							break;
						case BigRenda:
							renda_flag = 2;
							break;
						case RendaEnd:
							switch (renda_flag) {
							case 1:
								Notes[id].kind = 8;
								break;
							case 2:
								Notes[id].kind = 9;
								break;
							}
							renda_flag = 0;
							break;
						}

						notes_number++;
					}
				}
			}
			bar_x = 400.0;
			sec_count++;
		}
		bpm_count++;
	}

	notes_main_count++;

	if (bpm_tempo_flag == true) bar_x = Notes_Area - Notes_Area * (NowTime - tempo_time[0]) / (60.0*tempo / bpm);
	

	C2D_DrawRectangle(bar_x,86,0,1,46, C2D_Color32f(1,1,1,1), C2D_Color32f(1,1,1,1), C2D_Color32f(1,1,1,1), C2D_Color32f(1,1,1,1));
	notes_calc(isDon,isKa,bpm, tempo, NowTime, cnt, sprites);
	C2D_DrawRectangle(0 ,86, 0, 62, 58, C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1));
	snprintf(buf_notes, sizeof(buf_notes), "%s", tja_notes[sec_count - 1]);
	debug_draw(0, 40, tja_notes[sec_count - 1]);


	snprintf(buf_notes, sizeof(buf_notes), "knd:%d", Command.knd);
	debug_draw(0, 170, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "val:%.2f", Command.val);
	debug_draw(100, 170, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "Com:%s", Command.command);
	debug_draw(200, 170, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "VAL:%s", Command.value);
	debug_draw(200, 180, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "%s", Command.notes);
	debug_draw(0, 180, buf_notes);
	
	snprintf(buf_notes, sizeof(buf_notes), "notes_cnt:%d", notes_count);
	debug_draw(300, 30, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "speed:%.2f", Notes_Area / (3600 / bpm * tempo));
	debug_draw(300, 50, buf_notes);
}

int notes_id_check() {

	for (int i = 0; i < Notes_Max-1; i++) {

		if (Notes[i].flag == false) {

			return i;
		}
	}
	return -1;
}

bool isJudgeDisp = false;	//要初期化
double JudgeMakeTime;		//
int JudgeDispKind;			//

void make_judge(int kind,double NowTime) {
	isJudgeDisp = true;
	JudgeMakeTime = NowTime;
	JudgeDispKind = kind;
}

void calc_judge(double NowTime) {

	if (isJudgeDisp == true) {
		switch (JudgeDispKind) {
		case 0:		//良
			debug_draw(80, 80, "ryou");
			break;
		case 1:		//可
			debug_draw(80, 80, "ka");
			break;
		case 2:		//不可
			debug_draw(80, 80, "fuka");
			break;
		}
		if (NowTime - JudgeMakeTime >= 0.25) isJudgeDisp = false;
	}
}

void notes_judge(double NowTime, bool isDon, bool isKa) {
	
	bool isAuto = true;
	int CurrentJudgeNotes[2] = { -1,-1 };		//現在判定すべきノーツ ドン,カツ
	double CurrentJudgeNotesLag[2] = { -1,-1 };	//判定すべきノーツの誤差(s)

	for (int i = 0; i < Notes_Max-1; i++) {

		if (Notes[i].flag == true) {

			if (Notes[i].kind == Don ||
				Notes[i].kind == BigDon ||
				Notes[i].kind == Renda ||
				Notes[i].kind == BigRenda ||
				Notes[i].kind == Balloon ||
				Notes[i].kind == RendaEnd ||
				Notes[i].kind == Potato) {	//ドン

				if (CurrentJudgeNotesLag[0] > fabs(Notes[i].judge_time - NowTime) ||
					CurrentJudgeNotesLag[0] == -1) {

					CurrentJudgeNotes[0] = i;
					CurrentJudgeNotesLag[0] = fabs(Notes[i].judge_time - NowTime);
				}
			}
			else if (
				Notes[i].kind == Ka ||
				Notes[i].kind == BigKa) {	//カツ

				if (CurrentJudgeNotesLag[1] > fabs(Notes[i].judge_time - NowTime) ||
					CurrentJudgeNotesLag[1] == -1) {

					CurrentJudgeNotes[1] = i;
					CurrentJudgeNotesLag[1] = fabs(Notes[i].judge_time - NowTime);
				}
			}
		}
	}

	if (isAuto == true) {	//オート

		bool isSe[2];

		for (int i = 0; i < Notes_Max-1; i++) {

			if (Notes[i].flag == true && Notes[i].judge_time <= NowTime) {

				if (isSe[0] == false ||
					Notes[i].kind == Don ||
					Notes[i].kind == BigDon ||
					Notes[i].kind == Renda ||
					Notes[i].kind == BigRenda ||
					Notes[i].kind == Balloon ||
					Notes[i].kind == RendaEnd ||
					Notes[i].kind == Potato) {
					
					isSe[0] = true;
					music_play(0);
				}
				else if (
					isSe[1] == false ||
					Notes[i].kind == Ka ||
					Notes[i].kind == BigKa) {

					isSe[1] = true;
					music_play(1);
				}

				Notes[i].flag = false;
			}
		}
	}

	else if (isAuto == false) {			//手動
		
		if (isDon == true && CurrentJudgeNotes[0] != -1) {	//ドン

			if (CurrentJudgeNotesLag[0] <= 0.034) {			//良
				make_judge(0,NowTime);
				Notes[CurrentJudgeNotes[0]].flag = false;
			}
			else if (CurrentJudgeNotesLag[0] <= 0.117) {	//可
				make_judge(1, NowTime);
				Notes[CurrentJudgeNotes[0]].flag = false;
			}
			else if (CurrentJudgeNotesLag[0] <= 0.150) {	//不可
				make_judge(2, NowTime);
				Notes[CurrentJudgeNotes[0]].flag = false;
			}
		}

		if (isKa == true && CurrentJudgeNotes[1] != -1) {			//カツ

			if (CurrentJudgeNotesLag[1] <= 0.034) {			//良
				make_judge(0, NowTime);
				Notes[CurrentJudgeNotes[1]].flag = false;
			}
			else if (CurrentJudgeNotesLag[1] <= 0.117) {	//可
				make_judge(1, NowTime);
				Notes[CurrentJudgeNotes[1]].flag = false;
			}
			else if (CurrentJudgeNotesLag[1] <= 0.150) {	//不可
				make_judge(2, NowTime);
				Notes[CurrentJudgeNotes[1]].flag = false;
			}
		}
	}

}

int cmp(const void *p, const void *q) {	//比較用

	int pp = ((NOTES_T*)p)->num;
	int qq = ((NOTES_T*)q)->num;

	if (((NOTES_T*)p)->flag == false) pp = INT_MAX;
	if (((NOTES_T*)p)->flag == false) qq = INT_MAX;

	return qq - pp;
}

void noted_sort() {	//ノーツを出現順にソート

	int n = sizeof Notes / sizeof(NOTES_T);
	qsort(Notes, n, sizeof(NOTES_T), cmp);
}

void notes_calc(bool isDon,bool isKa,double bpm, double tempo, double NowTime, int cnt, C2D_Sprite sprites[12]) {
	
	//int small_y = 95, big_y = 90;
	int notes_y = 108;
	
	for (int i = 0; i < Notes_Max; i++) {	//計算
		
		if (Notes[i].flag == true) {
			
			Notes[i].x = Notes[i].x_ini - Notes_Area * (NowTime - Notes[i].create_time) / (60 /Notes[i].bpm * tempo);

			snprintf(buf_notes, sizeof(buf_notes), "%d", i);
			debug_draw(Notes[i].x, notes_y+23, buf_notes);

		}
			
		if (Notes[i].x <= 40 ) Notes[i].flag = false;
	}

	noted_sort();	//ソート

	for (int i = 0; i < Notes_Max; i++) {	//描画

		if (Notes[i].flag == true) {

			switch (Notes[i].kind) {
			case Don:
				C2D_SpriteSetPos(&sprites[dOn], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[dOn]);
				break;
			case Ka:
				C2D_SpriteSetPos(&sprites[kA], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[kA]);
				break;
			case BigDon:
				C2D_SpriteSetPos(&sprites[bIg_don], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_don]);
				break;
			case BigKa:
				C2D_SpriteSetPos(&sprites[bIg_ka], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_ka]);
				break;
			case Renda:
				C2D_SpriteSetPos(&sprites[rEnda], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[rEnda]);
				break;
			case BigRenda:
				C2D_SpriteSetPos(&sprites[bIg_renda], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_renda]);
				break;
			case Balloon:
				C2D_SpriteSetPos(&sprites[bIg_renda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_renda_fini]);
				break;
			case RendaEnd:		//8:連打終了 9:大連打終了
				C2D_SpriteSetPos(&sprites[rEnda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[rEnda_fini]);
				break;
			case RendaEnd + 1:
				C2D_SpriteSetPos(&sprites[bIg_renda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_renda_fini]);
				break;
			default:
				break;
			}
		}
	}
	notes_judge(NowTime, isDon, isKa);
	calc_judge(NowTime);
}