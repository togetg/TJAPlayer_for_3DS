#pragma once


typedef struct {

	int level[4],x,y,tmp;
	char title[256],path[256],tja[256],ogg[256],genre[256];
} LIST_T;

void load_file_list_tjafiles();
void disp_file_list();
void load_file_main();