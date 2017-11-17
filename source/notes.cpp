#include "header.h"
int notes_id_check();void notes_calc(sf2d_texture *don, sf2d_texture *ka);

typedef struct{
	double x,speed;
	char kind;
	bool flag;
} NOTES_T;
NOTES_T Notes[64];

int bpm_count = 0;
int sec_count = 0;
int notes_count;
double bar_x;
double speed;

void notes_main(char tja_notes[2048][128],int cnt, char *tja_title, char *tja_subtitle, char *tja_level, char *tja_bpm, char *tja_wave, char *tja_offset, char *tja_balloon, char *tja_songvol, char *tja_sevol, char *tja_scoreinit, char *tja_scorediff, char *tja_course, char *tja_style, char *tja_game, char *tja_life, char *tja_demostart, char *tja_side, char *tja_scoremode, sftd_font* font, sf2d_texture *don, sf2d_texture *ka) {
	double bpm = atof(tja_bpm);
	double tempo = 4;
	if (int(fmod(double(cnt), (3600 / bpm))) == 0 && cnt >= 0) {
		if (bpm_count % 4 == 0) {
			notes_count = 0;
			while (tja_notes[sec_count][notes_count] != ',') {
				if (notes_count == 0 && tja_notes[sec_count][0] == '#') {
					notes_count = 0;
					sec_count++;
					continue;
				}
				if (notes_count == 0 && strstr(tja_notes[sec_count], ",") == NULL) {
					notes_count = 0;
					sec_count++;
					continue;
				}
				notes_count++;
			}

			for (int i = 0; i < notes_count; i++) {
				int id = notes_id_check();
				if (id != -1 && tja_notes[sec_count][i] != '0') {
					Notes[id].flag = true;
					Notes[id].x = (400 / notes_count)*i + 400;
					Notes[id].speed = 400 / (3600 / bpm*tempo);
					Notes[id].kind = tja_notes[sec_count][i];
				}
			}

			bar_x = 400.0;
			speed = 400 / (3600 / bpm*tempo);
			sec_count++;
		}
		bpm_count++;
	}
	notes_calc(don,ka);
	bar_x -= speed;
	sf2d_draw_rectangle(bar_x, 43, 1, 47, RGBA8(255, 255, 255, 255));
	sftd_draw_textf(font, 10, 40, RGBA8(0, 255, 0, 255), 30, "%s", tja_notes[sec_count-1]);
	sftd_draw_textf(font, 10, 20, RGBA8(0, 255, 0, 255), 30, "%d", notes_count);
}

int notes_id_check() {
	for (int i=0; i < 64; i++) {
		if (Notes[i].flag == false) {
			return i;
		}
	}
	return -1;
}

void notes_calc(sf2d_texture *don, sf2d_texture *ka) {
	for (int i=0; i < 64; i++) {
		if (Notes[i].flag == true) {
			Notes[i].x -= Notes[i].speed;
			switch (Notes[i].kind) {
				case '1' :
					sf2d_draw_texture(don, Notes[i].x, 43);
					break;
				case '2' :
					sf2d_draw_rectangle(Notes[i].x, 43, 20, 47, RGBA8(0, 0, 255, 255));
					break;
				case '3':
					sf2d_draw_rectangle(Notes[i].x, 43, 20, 47, RGBA8(255, 0, 0, 255));
					break;
				case '4':
					sf2d_draw_rectangle(Notes[i].x, 43, 20, 47, RGBA8(0, 0, 255, 255));
					break;
			}
			if (Notes[i].x <= -20) { Notes[i].flag = false; }
		}
	}
}