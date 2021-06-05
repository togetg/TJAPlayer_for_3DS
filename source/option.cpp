#include "header.h"
#include "option.h"
#include "select.h"
#include "main.h"
#include "vorbis.h"
#include <jansson.h>

OPTION_T Option;
json_t *json;
json_error_t error_json;
int option_page = 1,max_option_page = 3;

//数字キーボードで入力
double input_number_keyboard(int max_digits,bool isFloat) {	//最大桁数、少数かどうか

	SwkbdState swkbd;
	swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 1, max_digits);
	//swkbdSetPasswordMode(&swkbd, SWKBD_PASSWORD_HIDE_DELAY);
	swkbdSetValidation(&swkbd, SWKBD_ANYTHING, 0, 0);
	swkbdSetFeatures(&swkbd, SWKBD_FIXED_WIDTH);
	if (isFloat==true) swkbdSetNumpadKeys(&swkbd, L'.', 0);
	swkbdInputText(&swkbd, get_buffer(), BUFFER_SIZE);
	return atof(get_buffer());
}

void calc_option_page(u16 px, u16 py, unsigned int key) {

	float width, height,x,y;

	if (option_page > 1) {
		x = 30, y = 205;
		draw_option_text(x, y, "←", true, &width, &height, 2.5, 1.5);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			if (option_page != 1) option_page--;
		}
	}
	if (option_page < max_option_page) {
		x = 230, y = 205;
		draw_option_text(x, y, "→", true, &width, &height, 2.5, 1.5);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			if (option_page < max_option_page) option_page++;
		}
	}
	snprintf(get_buffer(), BUFFER_SIZE, "%d/%d", option_page,max_option_page);
	draw_option_text(145, 218, get_buffer(), true, &width, &height);
}

void draw_option(u16 px, u16 py, unsigned int key) {
	
	float width,height,x,y;
	int XSense=65, YSense=30,XCnt = 0,YCnt = 1,gap = 15;

	switch (option_page) {

	case 1:
		//オート
		x = XSense * XCnt, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_AUTO], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_OFF], Option.isAuto == false, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isAuto = false;
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_ON], Option.isAuto == true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isAuto = true;
		XCnt = 0, YCnt++;

		//はやさ
		x = XSense * XCnt, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_SPEED], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_X1], Option.speed == 1.0, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 1.0;
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_X2], Option.speed == 2.0, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 2.0;
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_X3], Option.speed == 3.0, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 3.0;
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_X4], Option.speed == 4.0, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 4.0;
		XCnt = 0, YCnt++;

		//ステルス
		x = XSense * XCnt, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_STELTH], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_OFF], Option.isStelth == false, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isStelth = false;
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_ON], Option.isStelth == true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isStelth = true;
		XCnt = 0, YCnt++;

		//あべこべ
		x = XSense * XCnt, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_SWAP], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_OFF], Option.isSwap == false, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isSwap = false;
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_ON], Option.isSwap == true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isSwap = true;
		XCnt = 0, YCnt++;

		//ランダム
		x = XSense * XCnt, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_RANDOM], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_OFF], Option.random == 0, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.random = 0;
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_R25], Option.random == 0.25, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.random = 0.25;
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_R50], Option.random == 0.5, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.random = 0.5;
		XCnt = 0, YCnt++;

		//言語
		x = XSense * XCnt, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_LANG], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_JP], Option.lang == LANG_JP, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.lang = LANG_JP;
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_EN], Option.lang == LANG_EN, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.lang = LANG_EN;
		XCnt = 0, YCnt++;
		break;

	case 2:
		XSense = 65, YSense = 30, XCnt = 0, YCnt = 1, gap = 50;
		//バッファ
		x = XSense * XCnt, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_BUFFERSIZE], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		snprintf(get_buffer(), BUFFER_SIZE, "%d", get_buffer_size());
		draw_option_text(x, y, get_buffer(), true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			Option.buffer_size = (int)input_number_keyboard(5, false);
			put_buffer_size(Option.buffer_size);
		}
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_RESET], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			Option.buffer_size = DEFAULT_BUFFER_SIZE;
			put_buffer_size(Option.buffer_size);
		}
		XCnt = 0, YCnt++;
		
		//はやさ
		x = XSense * XCnt, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_SPEED], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		snprintf(get_buffer(), BUFFER_SIZE, "%.2f", Option.speed);
		draw_option_text(x, y, get_buffer(), true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			Option.speed = input_number_keyboard(5, true);
			if (Option.speed > 10.0) Option.speed = 10.0;
		}
		x = XSense * XCnt + gap, y = YSense * YCnt, XCnt++;
		draw_option_text(x, y, Text[Option.lang][TEXT_RESET], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 1.0;
		XCnt = 0, YCnt++;
		break;

	default:
		break;
	}

	calc_option_page(px,py,key);
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
	Option.buffer_size = DEFAULT_BUFFER_SIZE;

	option_page = 1;
}

void save_option() {

	json_object_set(json, "lang", json_integer(Option.lang));
	json_object_set(json, "buffer_size", json_integer(Option.buffer_size));
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
		Option.buffer_size = json_integer_value(json_object_get(json, "buffer_size"));
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
	TMP->buffer_size = Option.buffer_size;
}