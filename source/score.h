void score_init();
void scoer_debug();
void score_update(int knd);
void send_gogotime(bool temp);
int round_down(int arg);

enum Judge_Knd {
	RYOU = 0,
	SPECIAL_RYOU,
	KA,
	SPECIAL_KA,
	FUKA,
	THROUGH,
	ROLL,
	BIG_ROLL,
	BALLOON,
	BALLOON_BREAK,
};