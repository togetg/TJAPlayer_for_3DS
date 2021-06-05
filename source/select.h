#pragma once
#include "result.h"

typedef struct {

	int level[5],x,y,tmp;
	char title[256],path[256],tja[256],wave[256],genre[256];
	bool course[5],course_exist[5];
} LIST_T;

void load_file_list_tjafiles();
void disp_file_list();
void load_file_main();
void cursor_update(int knd);
void get_SelectedId(LIST_T *TMP, int *arg);
void draw_select_text(float x, float y, const char *text);
bool get_isGameStart();
void select_ini();
void draw_option_text(float x, float y, const char *text, bool state, float *width, float *height,float sizex=0.7,float sizey = 0.7);
void draw_result_text(float x, float y, float size, const char *text);
void get_result(RESULT_T *Result);
void calc_result_text(const char *text, float *width, float *height);
int pause_window(u16 px, u16 py, unsigned int key);