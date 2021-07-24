#pragma once
#include "result.h"

typedef struct {

	int level[5],x,y,tmp,genre;
	char title[256],path[256],tja[256],wave[256];
	bool course[5],course_exist[5];
} LIST_T;

typedef struct {

	int genre_color,font_color;
	char name[256], path[256];
	bool isOpened;
} GENRE_T;

void disp_file_list();
void load_file_main();
void update_cursor(int knd);
void get_SelectedId(LIST_T *TMP, int *arg);
void draw_select_text(float x, float y, const char* text, int color = 0xffffff);
bool get_isGameStart();
void select_ini();
void draw_option_text(float x, float y, const char *text, bool state, float *width, float *height,float sizex=0.7,float sizey = 0.7);
void draw_result_text(float x, float y, float size, const char *text);
void get_result(RESULT_T *Result);
void calc_result_text(const char *text, float *width, float *height);