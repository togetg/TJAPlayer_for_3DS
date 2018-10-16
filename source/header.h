#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <citro2d.h>
#include <limits.h>
#include <math.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>

#define File_Name "シャルル"
#define DEFAULT_DIR	"sdmc:/tjafiles/"

#define Max_Notes_Measure 130	//一小節の最大ノーツ数+1
#define Measure_Max 4096
#define Notes_Area 400.0
#define Notes_Judge 93
#define Notes_Judge_Range 307

#define TOP_WIDTH  400
#define TOP_HEIGHT 240
#define BOTTOM_WIDTH  320
#define BOTTOM_HEIGHT 240

#define Sprite_Number 83

#define Notes_Max 512
#define BarLine_Max 64
#define Roll_Max 64
#define Balloon_Max 64

#define Score_Course 3		//難易度

#define List_Max 1024		//選曲リストの最大数

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
	sOul_effect,
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
	cOmbo_0_red,
	cOmbo_1_red,
	cOmbo_2_red,
	cOmbo_3_red,
	cOmbo_4_red,
	cOmbo_5_red,
	cOmbo_6_red,
	cOmbo_7_red,
	cOmbo_8_red,
	cOmbo_9_red,
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
	eMblem_easy,
	eMblem_normal,
	eMblem_hard,
	eMblem_oni,
	eMblem_edit,
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

enum Scene_state {

	SelectLoad = 0,
	SelectSong = 10,
	MainLoad = 50,
	MainGame = 100,
	ResultGame = 110,
};