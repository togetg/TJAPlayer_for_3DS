#pragma once
#include <citro2d.h>
#include "header.h"

typedef struct {

	int knd, data[3],test;
	double val;
	char* command_s,*notes,*value_s;
} COMMAND_T;

typedef struct {

	char *title, *subtitle,*wave;
	int level,balloon[100],songvol,sevol,scoreinit,scorediff,
		course,style,life,side,scoremode,total;
	double bpm, offset, demostart;

}TJA_HEADER_T;

typedef struct {
	double judge_time, create_time,create_time_cmp,pop_time,
		bpm, speed,measure,scroll;
	int notes;
	bool flag,isDispBarLine;
} MEASURE_T;

void tja_head_load();
void tja_notes_load();
void tja_to_notes(bool isDnon, bool isKa, int count, C2D_Sprite sprites[Sprite_Number]);
void get_command_value(char* buf, COMMAND_T *Command);
void get_head(TJA_HEADER_T *Tja_Header);
void tja_init();
double get_FirstMeasureTime();