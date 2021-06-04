#include "header.h"
#include "option.h"
#include "select.h"
#include "main.h"
#include <jansson.h>

OPTION_T Option;
json_t *json;
json_error_t error_json;

void draw_option(u16 px, u16 py, unsigned int key) {
	
	float width,height,x,y;
	int XSense=65, YSense=30,XCnt = 0,YCnt = 1,gap = 15;

	//オート
	x = XSense * XCnt, y = YSense*YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][autO], true ,&width ,&height);
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][ofF], Option.isAuto==false, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isAuto = false;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][oN], Option.isAuto==true, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isAuto = true;
	XCnt = 0, YCnt++;

	//はやさ
	x = XSense * XCnt, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][speeD], true, &width, &height);
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][x1], Option.speed == 1.0, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 1.0;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][x2], Option.speed == 2.0, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 2.0;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][x3], Option.speed == 3.0, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 3.0;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][x4], Option.speed == 4.0, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 4.0;
	XCnt = 0, YCnt++;

	//ステルス
	x = XSense * XCnt, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][steltH], true, &width, &height);
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][ofF], Option.isStelth == false, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isStelth = false;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][oN], Option.isStelth == true, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isStelth = true;
	XCnt = 0, YCnt++;

	//あべこべ
	x = XSense * XCnt, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][swaP], true, &width, &height);
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][ofF], Option.isSwap == false, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isSwap = false;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][oN], Option.isSwap == true, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isSwap = true;
	XCnt = 0, YCnt++;

	//ランダム
	x = XSense * XCnt, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][randoM], true, &width, &height);
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][ofF], Option.random == 0, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.random = 0;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][r25], Option.random == 0.25, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.random = 0.25;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][r50], Option.random == 0.5, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.random = 0.5;
	XCnt = 0, YCnt++;

	//言語
	x = XSense * XCnt, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][lanG], true, &width, &height);
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][jP], Option.lang == LANG_JP, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.lang = LANG_JP;
	x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
	draw_option_text(x, y, Text[Option.lang][eN], Option.lang == LANG_EN, &width, &height);
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.lang = LANG_EN;

	//C2D_DrawRectSolid(0, 0, 0, width, height, C2D_Color32f(1, 34.0 / 255.0, 204.0 / 255.0, 0.2));
}

int get_lang() {
	return Option.lang;
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
	Option.lang = LANG_JP;
}

void save_option() {

	json_object_set(json, "lang", json_integer(Option.lang));
	json_object_set(json, "isAuto", json_boolean(Option.isAuto));
	json_object_set(json, "isStelth", json_boolean(Option.isStelth));
	json_object_set(json, "isSwap", json_boolean(Option.isSwap));
	json_object_set(json, "random", json_real(Option.random));
	json_object_set(json, "speed", json_real(Option.speed));
	json_dump_file(json, SETTING_FILE, 0);
}

void load_option() {

	init_option();
	
	json = json_load_file(SETTING_FILE, 0, &error_json);

	if (json != NULL){

		Option.lang = json_integer_value(json_object_get(json, "lang"));
		Option.isAuto = json_boolean_value(json_object_get(json, "isAuto"));
		Option.isStelth = json_boolean_value(json_object_get(json, "isStelth"));
		Option.isSwap = json_boolean_value(json_object_get(json, "isSwap"));
		Option.random = json_real_value(json_object_get(json, "random"));
		Option.speed = json_real_value(json_object_get(json, "speed"));

	}
	if (json == NULL) {	//開けなかった時
		json = json_pack("{}");	//ファイル空の時はこれしないとセットできなくなる
		save_option();			//書き込み
	}
}

void exit_option() {

	save_option();
	json_decref(json);
}

void get_option(OPTION_T *TMP) {

	TMP->isAuto = Option.isAuto;
	TMP->isStelth = Option.isStelth;
	TMP->random = Option.random;
	TMP->speed = Option.speed;
	TMP->isSwap = Option.isSwap;
	TMP->lang = Option.lang;
}