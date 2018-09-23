void score_init();
void scoer_debug();
void score_update(int knd);

enum Judge_Knd {
	RYOU = 0,
	KA,
	FUKA,
	THROUGH,
	ROLL,
	BIG_ROLL,
	BALOON,
	BALLOON_BREAK,
};