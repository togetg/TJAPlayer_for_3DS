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
	ROLL_END,
	BALLOON,
	BALLOON_BREAK,
};

void init_score();
void debug_score();
void update_score(int knd);
void send_gogotime(bool temp);
int round_down(int arg);
void calc_base_score(MEASURE_T Measure[MEASURE_MAX], char tja_notes[MEASURE_MAX][NOTES_MEASURE_MAX]);
void init_branch_section();
int start_branch(int knd, double x, double y);
void draw_lane(C2D_Sprite  sprites[SPRITES_NUMER]);
void draw_gauge(C2D_Sprite  sprites[SPRITES_NUMER]);
void draw_score(C2D_Sprite  sprites[SPRITES_NUMER]);
void update_balloon_count(int arg);
void draw_emblem(C2D_Sprite  sprites[SPRITES_NUMER]);
void draw_gauge_result(C2D_Sprite  sprites[SPRITES_NUMER]);