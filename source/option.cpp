#include "header.h"
#include "option.h"
#include "select.h"
#include "main.h"

int lang = jp;

OPTION_T Option;
char buf_option[160];

void draw_option(u16 px, u16 py, unsigned int key) {
	
	float width,height,x,y;
	int XSense=65, YSense=30,XCnt = 0,YCnt = 1,gap = 15;

	//オート
	x = XSense * XCnt, y = YSense*YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][autO], true ,&width ,&height);
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][ofF], Option.isAuto==false, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isAuto = false;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][oN], Option.isAuto==true, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isAuto = true;
	XCnt = 0, YCnt++;

	//はやさ
	x = XSense * XCnt, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][speeD], true, &width, &height);
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][x1], Option.speed == 1.0, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 1.0;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][x2], Option.speed == 2.0, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 2.0;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][x3], Option.speed == 3.0, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 3.0;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][x4], Option.speed == 4.0, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 4.0;
	XCnt = 0, YCnt++;

	//ステルス
	x = XSense * XCnt, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][steltH], true, &width, &height);
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][ofF], Option.isStelth == false, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isStelth = false;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][oN], Option.isStelth == true, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isStelth = true;
	XCnt = 0, YCnt++;

	//あべこべ
	x = XSense * XCnt, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][swaP], true, &width, &height);
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][ofF], Option.isSwap == false, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isSwap = false;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][oN], Option.isSwap == true, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isSwap = true;
	XCnt = 0, YCnt++;

	//ランダム
	x = XSense * XCnt, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][randoM], true, &width, &height);
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][ofF], Option.random == 0, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.random = 0;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][r25], Option.random == 0.25, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.random = 0.25;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][r50], Option.random == 0.5, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.random = 0.5;
	XCnt = 0, YCnt++;

	//言語
	x = XSense * XCnt, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][lanG], true, &width, &height);
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][jP], lang == jp, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) lang = jp;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[lang][eN], lang == en, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) lang = en;

	//C2D_DrawRectSolid(0, 0, 0, width, height, C2D_Color32f(1, 34.0 / 255.0, 204.0 / 255.0, 0.2));
}

int get_lang() {
	return lang;
}

void toggle_auto() {
	Option.isAuto = !Option.isAuto;
}

void init_option() {

	Option.isAuto = false;
	Option.isStelth = false;
	Option.random = 0;
	Option.speed = 1.0;
	Option.isSwap = false;
}

void get_option(OPTION_T *TMP) {

	TMP->isAuto = Option.isAuto;
	TMP->isStelth = Option.isStelth;
	TMP->random = Option.random;
	TMP->speed = Option.speed;
	TMP->isSwap = Option.isSwap;
}