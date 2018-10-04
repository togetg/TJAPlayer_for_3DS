#pragma once

typedef struct {
	int perfect, nice, bad,
		score, norma, soul;

}GAUGE_T;

enum Judge_Knd {
	PERFECT = 0,
	SPECIAL_PERFECT,
	NICE,
	SPECIAL_NICE,
	BAD,
	THROUGH,
	ROLL,
	BIG_ROLL,
	BALLOON,
	BALLOON_BREAK,
};

void score_init();
void scoer_debug();
void score_update(int knd);
void send_gogotime(bool temp);
int round_down(int arg);
void calc_base_score(MEASURE_T Measure[Measure_Max], char tja_notes[Measure_Max][Max_Notes_Measure]);
void branch_section_init();
int branch_start(int knd, double x, double y);
void draw_lane(C2D_Sprite  sprites[Sprite_Number]);
void draw_gauge(C2D_Sprite  sprites[Sprite_Number]);
void draw_score(C2D_Sprite  sprites[Sprite_Number]);