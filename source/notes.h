#pragma once
#include "tja.h"
#include <citro2d.h>

void notes_main(
	bool isDon,
	bool isKa,
	char tja_notes[2048][Max_Notes_Measure],
	int cnt,
	C2D_Sprite  sprites[12]);


void notes_init(TJA_HEADER_T Tja_Header);

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

enum Sprite_Notes_Kind {	//スプライト用
	dOn = 2,
	kA,
	bIg_don,
	bIg_ka,
	rEnda,
	rEnda_fini,
	bIg_renda,
	bIg_renda_fini,
	bAlloon,
	jUdge_ryou,
	jUdge_ka,
	jUdge_fuka,
};

enum Command_Kind {
	Start = 1,
	End,
	Bpmchange,
	Gogostart,
	Gogoend,
	Measure,
	Scroll,
	Delay,
	Section,
	Branchstart,
	Branchend,
	N,
	E,
	M,
	Levelhold,
	Bmscroll,
	Hbscroll,
	Barlineoff,
	Barlineon,

};

typedef struct {
	int num, notes_max;
	double x_ini, x, create_time, judge_time, bpm;
	int kind;
	bool flag = false;
	int sec;
	C2D_Sprite spr;
} NOTES_T;