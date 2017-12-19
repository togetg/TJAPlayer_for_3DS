#pragma once
void tja_head_load(double *,double *,int *);
void tja_notes_load();
void tja_draw(sftd_font*,int);
void tja_to_notes(char *p_now_notes, int cnt, sftd_font* font, sf2d_texture *don, sf2d_texture *ka, sf2d_texture *big_don, sf2d_texture *big_ka,sf2d_texture *renda, sf2d_texture *big_renda, sf2d_texture *renda_fini, sf2d_texture *big_renda_fini, sf2d_texture *balloon);