#include "header.h"
#define Notes_Area 400
int notes_id_check(); void notes_calc(int cnt,char *p_now_notes, sftd_font* font,sf2d_texture *don, sf2d_texture *ka, sf2d_texture *big_don, sf2d_texture *big_ka, sf2d_texture *renda, sf2d_texture *big_renda, sf2d_texture *renda_fini, sf2d_texture *big_renda_fini,sf2d_texture *balloon); void frame_draw();

typedef struct{
	double x,speed;
	char kind;
	bool flag;
	int time;
} NOTES_T;
NOTES_T Notes[64];

int bpm_count = 0;
int sec_count = 0;
int renda_flag = 0;
int notes_count;
double bar_x;
double speed;

void notes_main(char *p_now_notes, char tja_notes[2048][128],int cnt, char *tja_title, char *tja_subtitle, char *tja_level, char *tja_bpm, char *tja_wave, char *tja_offset, char *tja_balloon, char *tja_songvol, char *tja_sevol, char *tja_scoreinit, char *tja_scorediff, char *tja_course, char *tja_style, char *tja_game, char *tja_life, char *tja_demostart, char *tja_side, char *tja_scoremode, sftd_font* font, sf2d_texture *don, sf2d_texture *ka, sf2d_texture *big_don, sf2d_texture *big_ka, sf2d_texture *renda, sf2d_texture *big_renda, sf2d_texture *renda_fini, sf2d_texture *big_renda_fini, sf2d_texture *balloon) {
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
					if (renda_flag == 0 || tja_notes[sec_count][i] == '8') {
						Notes[id].flag = true;
						Notes[id].x = (Notes_Area / notes_count)*i + 400.0;
						Notes[id].speed = Notes_Area / (3600 / bpm*tempo);
						Notes[id].kind = tja_notes[sec_count][i];
						Notes[id].time = cnt + (400-90)/ Notes[id].speed + (( 3600/bpm*tempo) / notes_count)*i;
						switch (Notes[id].kind) {
						case '5':
							renda_flag = 1;
							break;
						case '6':
							renda_flag = 2;
							break;
						case '8' :
							switch (renda_flag) {
							case 1:
								Notes[id].kind = '8';
								break;
							case 2:
								Notes[id].kind = '9';
								break;
							}
							renda_flag = 0;
							break;
						}
					}
				}
			}
			bar_x = 400.0;
			speed = Notes_Area / (3600 / bpm*tempo);
			sec_count++;
		}
		bpm_count++;
	}
	bar_x -= speed;
	sf2d_draw_rectangle(bar_x, 86, 1, 46, RGBA8(255, 255, 255, 255));
	notes_calc(cnt,p_now_notes, font,don,ka,big_don,big_ka,renda,big_renda,renda_fini,big_renda_fini,balloon);
	sf2d_draw_rectangle(93, 86, 1, 46, RGBA8(255, 0, 255, 255));
	//sf2d_draw_rectangle(65, 86, 1, 46, RGBA8(255, 255, 0, 255));
	sftd_draw_textf(font, 10, 40, RGBA8(0, 255, 0, 255), 20, "%s", tja_notes[sec_count-1]);
	sftd_draw_textf(font, 10, 20, RGBA8(0, 255, 0, 255), 20, "%d", notes_count);
	sftd_draw_textf(font, 10, 60, RGBA8(0, 255, 0, 255), 20, "%d", int(bar_x));
}

int notes_id_check() {
	for (int i=0; i < 64; i++) {
		if (Notes[i].flag == false) {
			return i;
		}
	}
	return -1;
}

void notes_calc(int cnt,char *p_now_notes, sftd_font* font,sf2d_texture *don, sf2d_texture *ka, sf2d_texture *big_don, sf2d_texture *big_ka, sf2d_texture *renda, sf2d_texture *big_renda, sf2d_texture *renda_fini, sf2d_texture *big_renda_fini,sf2d_texture *balloon) {
	int small_y = 95, big_y = 90;
	*p_now_notes = 0;
	for (int i=64; i >= 0; i--) {
		if (Notes[i].flag == true) {
			Notes[i].x -= Notes[i].speed;
			switch (Notes[i].kind) {
				case '1' :
					sf2d_draw_texture(don, Notes[i].x-15, small_y);
					break;
				case '2' :
					sf2d_draw_texture(ka, Notes[i].x-15, small_y);
					break;
				case '3':
					sf2d_draw_texture(big_don, Notes[i].x - 20, big_y);
					break;
				case '4':
					sf2d_draw_texture(big_ka, Notes[i].x - 20, big_y); 
					break;
				case '5':
					sf2d_draw_texture(renda, Notes[i].x - 15, small_y);
					break;
				case '6':
					sf2d_draw_texture(big_renda, Notes[i].x - 20, big_y);
					break;
				case '7':
					sf2d_draw_texture(balloon, Notes[i].x - 15, small_y);
					break;
				case '8'://8:連打終了 9:大連打終了
					sf2d_draw_texture(renda_fini, Notes[i].x - 15, small_y);
					break;
				case '9':
					sf2d_draw_texture(big_renda_fini, Notes[i].x - 20, big_y);
					break;
				default:
					break;
			}
			if (cnt == Notes[i].time) {
				*p_now_notes = Notes[i].kind;
			}
			sftd_draw_textf(font, Notes[i].x, 132, RGBA8(0, 255, 0, 255), 10, "%d", Notes[i].time);
			if (Notes[i].x <= 64-20) { Notes[i].flag = false; }
		}
	}
}
