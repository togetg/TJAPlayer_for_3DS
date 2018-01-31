#include "header.h"
#include "time.h"
#include "audio.h"

#define Notes_Area 400
#define Notes_Judge 93
#define Notes_Max 64

enum Notes_Kind {
	Rest,		//休符
	Don,		//ドン
	Ka,			//カツ
	BigDon,		//ドン(大)
	BigKa,		//カツ(大)
	Renda,		//連打開始
	BigRenda,	//連打(大)開始
	Balloon,	//風船開始
	RendaEnd,	//連打終了
	Potato,		//お芋音符開始
};

int notes_id_check(); void notes_calc(bool isDon, bool isKa,double bpm, double tempo, double time, int cnt, sftd_font* font, sf2d_texture *don, sf2d_texture *ka, sf2d_texture *big_don, sf2d_texture *big_ka, sf2d_texture *renda, sf2d_texture *big_renda, sf2d_texture *renda_fini, sf2d_texture *big_renda_fini, sf2d_texture *balloon); void frame_draw();

typedef struct {
	int notes_number, notes_max;
	double x_ini, x, speed, bpm, create_time,judge_time;
	int kind;
	bool flag;
	int sec;
} NOTES_T;
NOTES_T Notes[64];

int bpm_count = 0;
int bpm_count2 = 1;	//次のbpmの時間計測用
int sec_count = 0;
int notes_main_count = 0;
int renda_flag = 0;
int notes_count;
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
	default: return 0;
	}
}

void notes_main(bool isDon,bool isKa, char tja_notes[2048][128], int cnt, char *tja_title, char *tja_subtitle, char *tja_level, char *tja_bpm, char *tja_wave, char *tja_offset, char *tja_balloon, char *tja_songvol, char *tja_sevol, char *tja_scoreinit, char *tja_scorediff, char *tja_course, char *tja_style, char *tja_game, char *tja_life, char *tja_demostart, char *tja_side, char *tja_scoremode, sftd_font* font, sf2d_texture *don, sf2d_texture *ka, sf2d_texture *big_don, sf2d_texture *big_ka, sf2d_texture *renda, sf2d_texture *big_renda, sf2d_texture *renda_fini, sf2d_texture *big_renda_fini, sf2d_texture *balloon) {
	
	double bpm = atof(tja_bpm);
	double tempo = 4;
	double time = time_now(0);
	if (notes_main_count == 0) bpm_time = 60.0 / bpm * bpm_count2;

	sftd_draw_textf(font, 100, 20, RGBA8(0, 255, 0, 255), 10, "BpmTime:%f", bpm_time);
	sftd_draw_textf(font, 100, 30, RGBA8(0, 255, 0, 255), 10, "BpmCount2:%d", bpm_count2);
	
	if (bpm_time <= time && cnt >= 0 && notes_load_flag == true) {

		//if (int(fmod(double(cnt), (3600 / bpm))) == 0 && cnt >= 0) {
		bpm_count2++;
		bpm_time = 60.0 / bpm * bpm_count2;

		if (bpm_count % 4 == 0) {

			switch (sec_count) {
			case 0:
				SecTime[0] = time;
				break;
			case 1:
				SecTime[1] = time;
				break;
			case 2:
				SecTime[2] = time;
				break;
			case 3:
				SecTime[3] = time;
				break;
			}
			bpm_tempo_flag = true;
			tempo_time[0] = time;
			notes_count = 0;
			if (sec_count == 0) sec_time = time;
			while (tja_notes[sec_count][notes_count] != ',') {
				if (notes_count == 0 && tja_notes[sec_count][0] == '#') {
					if (tja_notes[sec_count][1] == 'E' &&
						tja_notes[sec_count][2] == 'N' &&
						tja_notes[sec_count][3] == 'D') {
						notes_load_flag = false;
						break;
					}
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

					if (renda_flag == 0 || tja_notes[sec_count][i] == '8') {

						Notes[id].flag = true;
						Notes[id].notes_max = notes_count;
						Notes[id].notes_number = i;
						Notes[id].x_ini = (Notes_Area / notes_count)*i + 400.0;
						Notes[id].x = Notes[id].x_ini;
						//Notes[id].speed = Notes_Area / (3600 / bpm * tempo);
						Notes[id].speed = Notes_Area / (60.0 / bpm * tempo);
						Notes[id].kind = ctoi(tja_notes[sec_count][i]);
						Notes[id].sec = sec_count;
						Notes[id].create_time = time;
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
					}
				}
			}
			bar_x = 400.0;
			speed = Notes_Area / (3600 / bpm * tempo);
			sec_count++;
		}
		bpm_count++;
	}
	notes_main_count++;

	if (bpm_tempo_flag == true) bar_x = Notes_Area - Notes_Area * (time - tempo_time[0]) / (60.0*tempo / bpm);
	
	sf2d_draw_rectangle(bar_x, 86, 1, 46, RGBA8(255, 255, 255, 255));
	notes_calc(isDon,isKa,bpm, tempo, time, cnt, font, don, ka, big_don, big_ka, renda, big_renda, renda_fini, big_renda_fini, balloon);
	sf2d_draw_rectangle(Notes_Judge, 86, 1, 46, RGBA8(255, 0, 255, 255));
	//sf2d_draw_rectangle(65, 86, 1, 46, RGBA8(255, 255, 0, 255));
	sftd_draw_textf(font, 0, 60, RGBA8(0, 255, 0, 255), 10, "%s", tja_notes[sec_count - 1]);
}

int notes_id_check() {

	for (int i = 0; i < Notes_Max; i++) {

		if (Notes[i].flag == false) {

			return i;
		}
	}
	return -1;
}

bool isJudgeDisp = false;	//要初期化
double JudgeMakeTime;		//
int JudgeDispKind;			//

void make_judge(int kind,double time) {
	isJudgeDisp = true;
	JudgeMakeTime = time;
	JudgeDispKind = kind;
}

void calc_judge(double time, sftd_font* font) {

	if (isJudgeDisp == true) {
		switch (JudgeDispKind) {
		case 0:		//良
			sftd_draw_textf(font, 80, 80, RGBA8(255, 0, 0, 255), 10, "Ryou");
			break;
		case 1:		//可
			sftd_draw_textf(font, 80, 80, RGBA8(255, 255, 255, 255), 10, "Ka");
			break;
		case 2:		//不可
			sftd_draw_textf(font, 80, 80, RGBA8(0, 255, 255, 255), 10, "Huka");
			break;
		}
		if (time - JudgeMakeTime >= 0.25) isJudgeDisp = false;
	}
}

void notes_judge(double time, bool isDon, bool isKa, sftd_font* font) {
	
	sftd_draw_textf(font, 100, 60, RGBA8(0, 255, 0, 255), 10, "%f", time);
	bool isAuto = false;
	int CurrentJudgeNotes[2] = { -1,-1 };		//現在判定すべきノーツ ドン,カツ
	double CurrentJudgeNotesLag[2] = { -1,-1 };	//判定すべきノーツの誤差(s)

	for (int i = 0; i < Notes_Max; i++) {

		if (Notes[i].flag == true) {

			if (Notes[i].kind == Don ||
				Notes[i].kind == BigDon ||
				Notes[i].kind == Renda ||
				Notes[i].kind == BigRenda ||
				Notes[i].kind == Balloon ||
				Notes[i].kind == RendaEnd ||
				Notes[i].kind == Potato) {	//ドン

				if (CurrentJudgeNotesLag[0] > fabs(Notes[i].judge_time - time) ||
					CurrentJudgeNotesLag[0] == -1) {

					CurrentJudgeNotes[0] = i;
					CurrentJudgeNotesLag[0] = fabs(Notes[i].judge_time - time);
				}
			}
			else if (
				Notes[i].kind == Ka ||
				Notes[i].kind == BigKa) {	//カツ

				if (CurrentJudgeNotesLag[1] > fabs(Notes[i].judge_time - time) ||
					CurrentJudgeNotesLag[1] == -1) {

					CurrentJudgeNotes[1] = i;
					CurrentJudgeNotesLag[1] = fabs(Notes[i].judge_time - time);
				}
			}
		}
	}

	for (int n = 0; n < 2; n++) {

		if (CurrentJudgeNotes[n] != -1) {

			if (isAuto == true &&	//オート
				Notes[CurrentJudgeNotes[n]].judge_time <= time) {

				music_play(n);
				Notes[CurrentJudgeNotes[n]].flag = false;
			}
		}
	}

	if (isAuto == false) {			//手動
		
		if (isDon == true && CurrentJudgeNotes[0] != -1) {		//ドン

			if (CurrentJudgeNotesLag[0] <= 0.034) {			//良
				make_judge(0,time);
				Notes[CurrentJudgeNotes[0]].flag = false;
			}
			else if (CurrentJudgeNotesLag[0] <= 0.117) {	//可
				make_judge(1, time);
				Notes[CurrentJudgeNotes[0]].flag = false;
			}
			else if (CurrentJudgeNotesLag[0] <= 0.150) {	//不可
				make_judge(2, time);
				Notes[CurrentJudgeNotes[0]].flag = false;
			}
		}

		if (isKa == true && CurrentJudgeNotes[1] != -1) {			//カツ

			if (CurrentJudgeNotesLag[1] <= 0.034) {			//良
				make_judge(0, time);
				Notes[CurrentJudgeNotes[1]].flag = false;
			}
			else if (CurrentJudgeNotesLag[1] <= 0.117) {	//可
				make_judge(1, time);
				Notes[CurrentJudgeNotes[1]].flag = false;
			}
			else if (CurrentJudgeNotesLag[1] <= 0.150) {	//不可
				make_judge(2, time);
				Notes[CurrentJudgeNotes[1]].flag = false;
			}
		}
	}

	sftd_draw_textf(font, 100, 40, RGBA8(0, 255, 0, 255), 10, "%f", CurrentJudgeNotesLag[0]);
	sftd_draw_textf(font, 100, 50, RGBA8(0, 255, 0, 255), 10, "%f", CurrentJudgeNotesLag[1]);
	//sftd_draw_textf(font, 100, 60, RGBA8(0, 255, 0, 255), 10, "%f", time);
}


void notes_calc(bool isDon,bool isKa,double bpm, double tempo, double time, int cnt, sftd_font* font, sf2d_texture *don, sf2d_texture *ka, sf2d_texture *big_don, sf2d_texture *big_ka, sf2d_texture *renda, sf2d_texture *big_renda, sf2d_texture *renda_fini, sf2d_texture *big_renda_fini, sf2d_texture *balloon) {
	
	int small_y = 95, big_y = 90;

	for (int i = 64; i >= 0; i--) {

		if (Notes[i].flag == true) {


			//Notes[i].x = Notes[i].x_ini - 400 * (time - Notes[i].create_time) / (60 / bpm * tempo);
			Notes[i].x = Notes[i].x_ini - (time - Notes[i].create_time) * Notes[i].speed;

			switch (Notes[i].kind) {
			case Don:
				sf2d_draw_texture(don, Notes[i].x - 15, small_y);
				break;
			case Ka:
				sf2d_draw_texture(ka, Notes[i].x - 15, small_y);
				break;
			case BigDon:
				sf2d_draw_texture(big_don, Notes[i].x - 20, big_y);
				break;
			case BigKa:
				sf2d_draw_texture(big_ka, Notes[i].x - 20, big_y);
				break;
			case Renda:
				sf2d_draw_texture(renda, Notes[i].x - 15, small_y);
				break;
			case BigRenda:
				sf2d_draw_texture(big_renda, Notes[i].x - 20, big_y);
				break;
			case Balloon:
				sf2d_draw_texture(balloon, Notes[i].x - 15, small_y);
				break;
			case RendaEnd:		//8:連打終了 9:大連打終了
				sf2d_draw_texture(renda_fini, Notes[i].x - 15, small_y);
				break;
			case RendaEnd + 1:
				sf2d_draw_texture(big_renda_fini, Notes[i].x - 20, big_y);
				break;
			default:
				break;
			}

			sftd_draw_textf(font, Notes[i].x, 132, RGBA8(0, 255, 0, 255), 10, "%d", i);
			if (Notes[i].x <= 64 - 20) { Notes[i].flag = false; }
		}
	}
	//sftd_draw_textf(font, 100, 60, RGBA8(0, 255, 0, 255), 10, "%f", time);
	notes_judge(time, isDon, isKa, font);
	calc_judge(time, font);
}