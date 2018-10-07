#pragma once
#include "tja.h"
#include <citro2d.h>

enum Notes_knd {

	Rest = 0,		//休符
	Don,			//ドン
	Katsu,			//カツ
	BigDon,			//ドン(大)
	BigKatsu,		//カツ(大)
	Roll,			//連打開始
	BigRoll,		//連打(大)開始
	Balloon,		//風船開始
	RollEnd,		//連打終了
	Potato,			//お芋音符開始
	BigRollEnd,		//大連打終了
	BalloonEnd,		//風船終了
};

enum Sprite_Notes_knd {	//スプライト用

	tOp = 0,
	bOttom,
	dOn,
	kAtsu,
	bIg_don,
	bIg_katsu,
	rOll_start,
	rOll_int,
	rOll_end,
	bIg_roll_start,
	bIg_roll_int,
	bIg_roll_end,
	bAlloon,
	bAlloon_1,
	bAlloon_2,
	bAlloon_3,
	bAlloon_4,
	bAlloon_5,
	bAlloon_6,
	jUdge_perfect,
	jUdge_nice,
	jUdge_bad,
	jUdge_circle,
	cHart_normal,
	cHart_expert,
	cHart_master,
	lAne_expert,
	lAne_master,
	eFfect_perfect,
	eFfect_special_perfect,
	eFfect_nice,
	eFfect_special_nice,
	sOul_on,
	sOul_off,
	eFfect_gogo,
	sCore_0,
	sCore_1,
	sCore_2,
	sCore_3,
	sCore_4,
	sCore_5,
	sCore_6,
	sCore_7,
	sCore_8,
	sCore_9,
	cOmbo_0,
	cOmbo_1,
	cOmbo_2,
	cOmbo_3,
	cOmbo_4,
	cOmbo_5,
	cOmbo_6,
	cOmbo_7,
	cOmbo_8,
	cOmbo_9,
	rOll_0,
	rOll_1,
	rOll_2,
	rOll_3,
	rOll_4,
	rOll_5,
	rOll_6,
	rOll_7,
	rOll_8,
	rOll_9,
	rOll_count,
	bAlloon_count,
};

enum Command_knd {

	STart = 1,
	ENd,
	BPmchange,
	GOgostart,
	GOgoend,
	MEasure,
	SCroll,
	DElay,
	SEction,
	BRanchstart,
	BRanchend,
	N,
	E,
	M,
	LEvelhold,
	BMscroll,
	HBscroll,
	BArlineoff,
	BArlineon,
};

enum Header_knd {

	TITle,
	SUBtitle,
	BPM,
	WAVe,
	OFFset,
	BALloon,
	SONgvol,
	SEVol,
	SCOreinit,
	SCOrediff,
	COUrse,
	STYle,
	LIFe,
	DEMostart,
	SIDe,
	SCOremode,
	TOTal,
};

enum Course_knd {
	
	EASY = 0,
	NORMAL,
	HARD,
	ONI,
	EDIT,
};

typedef struct {
	int num, notes_max, knd, roll_id;
	double x_ini, x, create_time, judge_time, pop_time, bpm, scroll;
	bool flag, isThrough;
	C2D_Sprite spr;

} NOTES_T;

typedef struct {
	int measure;
	double x, x_ini, create_time, scroll;
	bool flag, isDisp;

} BARLINE_T;

typedef struct {
	int id, start_id, end_id, knd;
	double start_x, end_x;
	bool flag;

}Roll_T;

typedef struct {
	int id, start_id, end_id,
		need_hit, current_hit;
	bool flag;

}BALLOON_T;

typedef struct {
	int knd, course;
	double x, y;
	bool next, wait;

}BRANCH_T;

void notes_main(
	bool isDon,
	bool isKatsu,
	char tja_notes[Measure_Max][Max_Notes_Measure],
	MEASURE_T Measure[Measure_Max],
	int cnt,
	C2D_Sprite  sprites[Sprite_Number]);
void toggle_auto();
int ctoi(char c);
int get_branch_course();
void notes_init(TJA_HEADER_T TJA_Header);
