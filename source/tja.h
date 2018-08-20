#pragma once
#include <citro2d.h>

typedef struct {

	int knd, data[3],test;
	double val;
	char* command,*notes,*value;
} COMMAND_T;

typedef struct {

	char *title, *subtitle,*wave;
	int level,balloon[100],songvol,sevol,scoreinit,scorediff,
		course,style,life,side,scoremode,total;
	double bpm, offset, demostart;

}TJA_HEADER_T;

void tja_head_load();
void tja_notes_load();
void tja_draw(int);
void tja_to_notes(bool isDnon, bool isKa, int count, C2D_Sprite sprites[12]);
void get_command_value(char* buf, COMMAND_T *Command);
void get_head(TJA_HEADER_T *Tja_Header);