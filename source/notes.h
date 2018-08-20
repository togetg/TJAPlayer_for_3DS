#pragma once
#include <citro2d.h>

void notes_main(
	bool isDnon,
	bool isKa, 
	char tja_notes[2048][Max_Notes_Section],
	int cnt, 
	char *tja_title, 
	char *tja_subtitle, 
	char *tja_level, 
	char *tja_bpm, 
	char *tja_wave, 
	char *tja_offset, 
	char *tja_balloon, 
	char *tja_songvol, 
	char *tja_sevol, 
	char *tja_scoreinit, 
	char *tja_scorediff,
	char *tja_course, 
	char *tja_style, 
	char *tja_game, 
	char *tja_life, 
	char *tja_demostart, 
	char *tja_side, 
	char *tja_scoremode, 
	C2D_Sprite sprites[12]
);

void notes_init();

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