#pragma once


typedef struct {

	int level[4],x,y;
	char* tja, *ogg, *genre;
	char title[256];
} LIST_T;

void load_file_list();
void disp_file_list();