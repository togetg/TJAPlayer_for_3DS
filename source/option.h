#pragma once

typedef struct {
	bool isAuto, isStelth,isSwap;
	double speed, random;

} OPTION_T;

enum Lang_knd {
	jp = 0,
	en,
};

enum Text_knd {
	oN,
	ofF,
	easY,
	normaL,
	harD,
	onI,
	autO,
	speeD,
	x1,
	x2,
	x3,
	x4,
	steltH,
	swaP,
	randoM,
	r25,
	r50,
	lanG,
	jP,
	eN,
	scorE,
	maxcombO,
	perfecT,
	nicE,
	baD,
	rollcounT,
	pressstarT,
	continuE,
	startoveR,
	returnselecT,
};

const char Text[2][64][64] = {
	{
		"ON",
		"OFF",
		"かんたん",
		"ふつう",
		"むずかしい",
		"おに",
		"オート",
		"はやさ",
		"x1",
		"x2",
		"x3",
		"x4",
		"ステルス",
		"あべこべ",
		"ランダム",
		"25%",
		"50%",
		"language",
		"日本語",
		"English",
		"スコア",
		"最大ｺﾝﾎﾞ数",
		"良",
		"可",
		"不可",
		"連打数",
		"STARTボタンを押して戻る",
		"演奏を続ける",
		"はじめからやり直す",
		"曲選択に戻る"
	},
	{
		"ON",
		"OFF",
		"Easy",
		"Normal",
		"Hard",
		"Oni",
		"Auto",
		"Speed",
		"x1",
		"x2",
		"x3",
		"x4",
		"Stelth",
		"Swap",
		"Random",
		"25%",
		"50%",
		"language",
		"日本語",
		"English",
		"Scoere",
		"MaxCombo",
		"Perfect",
		"Nice",
		"Bad",
		"RollCount",
		"Press START",
		"Continue",
		"Start over",
		"Return to select song"
	}
};

int get_lang();
void draw_option(u16 px, u16 py, unsigned int key);
void toggle_auto();
void get_option(OPTION_T *TMP);
void init_option();