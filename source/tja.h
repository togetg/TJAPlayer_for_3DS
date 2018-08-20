#pragma once
#include <citro2d.h>

typedef struct {
	int knd, data[3],test;
	double val;
	char* command,*notes,*value;
} COMMAND_T;

void tja_head_load(double *,double *,int *);
void tja_notes_load();
void tja_draw(int);
void tja_to_notes(bool isDnon, bool isKa, int count, C2D_Sprite sprites[12]);
void get_command_value(char* buf, COMMAND_T *Command);