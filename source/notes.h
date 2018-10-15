#pragma once
#include "tja.h"

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

}ROLL_T;

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
void init_notes(TJA_HEADER_T TJA_Header);
bool get_isAuto();