#pragma once
#include <citro2d.h>

void tja_head_load(double *,double *,int *);
void tja_notes_load();
void tja_draw(int);
void tja_to_notes(bool isDnon, bool isKa, int count, C2D_Sprite sprites[12]);