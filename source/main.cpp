#include "header.h"
#include "time.h"
#include "notes.h"
#include "tja.h"
#include "audio.h"
#include "playback.h"
#include "score.h"
#include "select.h"
#include "option.h"
#include "result.h"
#include "main.h"
#include "vorbis.h"

C2D_Sprite sprites[Sprite_Number];			//画像用
static C2D_SpriteSheet spriteSheet;
C2D_TextBuf g_dynamicBuf;
C2D_Text dynText;
bool isPause = false, isNotesStart = false, isMusicStart = false, isPlayMain = false, isExit = false;
char buffer[BUFFER_SIZE];

void load_sprites();

void draw_debug(float x, float y, const char *text) {

	//使用例
	//snprintf(get_buffer(), BUFFER_SIZE, "%d", 10);
	// draw_debug(300, 0, get_buffer());

	C2D_TextBufClear(g_dynamicBuf);
	C2D_TextParse(&dynText, g_dynamicBuf, text);
	C2D_TextOptimize(&dynText);
	C2D_DrawText(&dynText, C2D_WithColor, x, y, 0.5f, 0.5f, 0.5f, C2D_Color32f(0.0f, 1.0f, 0.0f, 1.0f));
}

void init_main() {

	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	g_dynamicBuf = C2D_TextBufNew(4096);
}

void main_exit() {

	C2D_TextBufDelete(g_dynamicBuf);

	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	music_exit();
}

void button_game(bool *isDon,bool *isKatsu,OPTION_T Option, unsigned int key) {

	int numKeys = 20;

	int Optionkeys[] = {
		Option.KEY_A,Option.KEY_B,Option.KEY_X,Option.KEY_Y,Option.KEY_R,Option.KEY_ZR,Option.KEY_L,Option.KEY_ZL,Option.KEY_DUP,Option.KEY_DDOWN,Option.KEY_DRIGHT,Option.KEY_DLEFT,
		Option.KEY_CPAD_UP,Option.KEY_CPAD_DOWN,Option.KEY_CPAD_RIGHT,Option.KEY_CPAD_LEFT,Option.KEY_CSTICK_UP,Option.KEY_CSTICK_DOWN,Option.KEY_CSTICK_RIGHT,Option.KEY_CSTICK_LEFT };
	unsigned int keys[] = {
		KEY_A,KEY_B,KEY_X,KEY_Y,KEY_R,KEY_ZR,KEY_L,KEY_ZL,KEY_DUP,KEY_DDOWN,KEY_DRIGHT,KEY_DLEFT,
		KEY_CPAD_UP,KEY_CPAD_DOWN,KEY_CPAD_RIGHT,KEY_CPAD_LEFT,KEY_CSTICK_UP,KEY_CSTICK_DOWN,KEY_CSTICK_RIGHT,KEY_CSTICK_LEFT};

	for (int i = 0; i < numKeys;i++) {
		if (key & keys[i]) {
			if (Optionkeys[i] == KEY_DON) *isDon = true;
			else if (Optionkeys[i] == KEY_KATSU) *isKatsu = true;
		}
	}
}

bool check_dsp1() { //DSP1を起動しているか確認

	FILE* fp = fopen(PATH_DSP1, "r");

	if (fp == NULL) return false;
	fclose(fp);

	return true;
}

int touch_x, touch_y, touch_cnt,PreTouch_x,PreTouch_y;	//タッチ用

int main() {

	init_main();

	touchPosition tp;	//下画面タッチした座標

	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	TJA_HEADER_T TJA_Header;
	LIST_T SelectedSong;
	OPTION_T Option;

	load_sprites();
	load_music();

	int cnt = 0, notes_cnt = 0, scene_state = SCENE_SELECTLOAD,warning=-1, course = COURSE_ONI, tmp=0;

	double FirstMeasureTime = INT_MAX,
		offset = 0,CurrentTimeMain = -1000;

	while (aptMainLoop()) {

		hidScanInput();
		hidTouchRead(&tp);
		unsigned int key = hidKeysDown();

		if (isExit == true) break;

		bool isDon = false, isKatsu = false;

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		C2D_TargetClear(top, C2D_Color32(0x42, 0x42, 0x42, 0xFF));	//上画面
		C2D_SceneBegin(top);

		get_option(&Option);

		switch (scene_state) {

		case SCENE_SELECTLOAD:	//ロード画面

			snprintf(get_buffer(), BUFFER_SIZE, "TJAPlayer for 3DS %s", VERSION);
			draw_select_text(120, 70, get_buffer());
			draw_select_text(120, 100, "Now Loading...");
			C3D_FrameEnd(0);
			load_file_main();
			load_option();
			if (check_dsp1() == true) scene_state = SCENE_SELECTSONG;
			else { 
				warning = WARNING_DSP1;
				scene_state = SCENE_WARNING; 
			}
			break;

		case SCENE_WARNING:		//警告画面

			C2D_TargetClear(bottom, C2D_Color32(0x42, 0x42, 0x42, 0xFF));	//下画面
			C2D_SceneBegin(bottom);

			switch (warning) {
			case WARNING_DSP1:
				tmp = message_window(tp, key, TEXT_WARNING_DSP1);
				break;

			case WARNING_WAVE:
				tmp = message_window(tp, key, TEXT_WARNING_WAVE);
				break;
			}
			if (tmp == 1 || key & KEY_A) {
				scene_state = SCENE_SELECTSONG;
				warning = -1;
			}
			break;

		case SCENE_SELECTSONG:	//選曲

			if (key & KEY_START) isExit = true;

			if (cnt == 0) {
				select_ini();
			}

			if (key & KEY_UP)		cursor_update(KEY_UP);
			if (key & KEY_DOWN)		cursor_update(KEY_DOWN);
			if (key & KEY_RIGHT)	cursor_update(KEY_RIGHT);
			if (key & KEY_LEFT)		cursor_update(KEY_LEFT);
			if (key & KEY_A)		cursor_update(KEY_A);
			if (key & KEY_B)		cursor_update(KEY_B);

			disp_file_list();

			if (get_isGameStart() == true) {
				scene_state = SCENE_MAINLOAD;
				cnt = -1;
			}
			get_SelectedId(&SelectedSong, &course);

			C2D_TargetClear(bottom, C2D_Color32(0x42, 0x42, 0x42, 0xFF));	//下画面
			C2D_SceneBegin(bottom);
			draw_option(tp.px, tp.py, key, sprites);
			isPause = false;

			break;

		case SCENE_MAINLOAD:

			init_tja();
			load_tja_head(course, SelectedSong);
			//init_main_music();
			get_tja_header(&TJA_Header);
			init_score();
			init_notes(TJA_Header);
			if (SelectedSong.course_exist[course] == false) load_tja_notes(-1, SelectedSong);
			else load_tja_notes(course, SelectedSong);
			time_ini();
			offset = TJA_Header.offset + Option.offset;
			notes_cnt = 0;
			isNotesStart = false, isMusicStart = false, isPlayMain = false;
			FirstMeasureTime = INT_MAX;
			CurrentTimeMain = -1000;

			if (check_wave(SelectedSong) == true) scene_state = SCENE_MAINGAME;
			else {
				warning = WARNING_WAVE;
				scene_state = SCENE_WARNING;
				select_ini();
			}
			cnt = -60;
			break;

		case SCENE_MAINGAME:		//メイン

			C2D_DrawSprite(&sprites[SPRITE_TOP]);
			draw_title();
			draw_emblem(sprites);

			if (cnt == 0) {

				FirstMeasureTime = get_FirstMeasureTime();
				play_main_music(&isPlayMain, SelectedSong);
			}
			
			if (cnt >= 0) CurrentTimeMain = get_current_time(TIME_MAINGAME);

			if (Option.dispFps == true) draw_fps();

			//譜面が先
			if (offset > 0 && (isNotesStart == false || isMusicStart == false)) {

				if (CurrentTimeMain >= 0 && isNotesStart == false) isNotesStart = true;
				if (CurrentTimeMain >= offset + FirstMeasureTime && isMusicStart == false) {
					isPlayMain = true;
					isMusicStart = true;
				}
			}

			//音楽が先
			else if (offset <= 0 && (isNotesStart == false || isMusicStart == false)) {

				if (CurrentTimeMain >= FirstMeasureTime && isPlayMain == false) {
					isPlayMain = true;
					isMusicStart = true;
				}
				if (CurrentTimeMain >= (-1.0) * offset && isNotesStart == false) {
					isNotesStart = true;
				}
			}

			if (isPause == false) {

				if (tp.px != 0 && tp.py != 0) {

					PreTouch_x = touch_x, PreTouch_y = touch_y;
					touch_x = tp.px, touch_y = tp.py;

					if (
						(key & KEY_TOUCH || 
							pow((touch_x - PreTouch_x)*(touch_x - PreTouch_x) + (touch_y - PreTouch_y)*(touch_y - PreTouch_y), 0.5) > 20.0
						) &&
						(tp.px - 160)*(tp.px - 160) + (tp.py - 135)*(tp.py - 135) <= 105 * 105 &&
						touch_cnt < 2) {
						isDon = true;
						touch_cnt++;
					}
					else if (
						(
						key & KEY_TOUCH ||
						pow((touch_x - PreTouch_x)*(touch_x - PreTouch_x) + (touch_y - PreTouch_y)*(touch_y - PreTouch_y), 0.5) > 20.0 
							)&&
						touch_cnt < 2) {
						isKatsu = true;
						touch_cnt++;
					}
				}
				else {
					touch_x = 0, touch_y = 0, touch_cnt = 0, PreTouch_x = 0, PreTouch_y = 0;
				}

				button_game(&isDon, &isKatsu, Option, key);
			}

			if (isDon == true)   music_play(SOUND_DON);		//ドン
			if (isKatsu == true) music_play(SOUND_KATSU);		//カツ

			if (key & KEY_SELECT || key & KEY_START) {
				togglePlayback();
				toggle_time(0);
				toggle_time(1);
				isPause = !isPause;
			}

			draw_lane(sprites);
			draw_gauge(sprites);

			if (isNotesStart == true) {
				tja_to_notes(isDon, isKatsu, notes_cnt, sprites);
				if (isPause == false) notes_cnt++;
			}
			draw_score(sprites);

			C2D_TargetClear(bottom, C2D_Color32(0x42, 0x42, 0x42, 0xFF));	//下画面
			C2D_SceneBegin(bottom);
			C2D_DrawSprite(&sprites[SPRITE_BOTTOM]);
			//debug_touch(tp.px,tp.py);

			if (isPause == true) {
				
				tmp = pause_window(tp, key);

				switch (tmp) {
				case 0:
					break;

				case 1:
					isPlayMain = true;
					stopPlayback();
					scene_state = SCENE_MAINLOAD;
					break;

				case 2:
					isPlayMain = true;
					stopPlayback();
					cnt = -1;
					scene_state = SCENE_SELECTSONG;
					break;
				}

				if (tmp > -1) {
					togglePlayback();
					toggle_time(0);
					toggle_time(1);
					isPause = !isPause;
					music_play(SOUND_DON);
				}
			}
			if (get_notes_finish() == true && ndspChnIsPlaying(CHANNEL) == false) {
				scene_state = SCENE_RESULT;
				cnt = -1;
			}
			break;

		case SCENE_RESULT:

			if (key & KEY_START) {
				cnt = -1;
				scene_state = SCENE_SELECTSONG;
			}
			draw_gauge_result(sprites);
			draw_result();
			break;
		}

		C3D_FrameEnd(0);
		if (isPause == false) cnt++;
	}

	exit_option();
	main_exit();
	return 0;
}

void load_sprites() {

	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);

	for (int i = 0; i < Sprite_Number; i++) {
		C2D_SpriteFromSheet(&sprites[i], spriteSheet, i);
		C2D_SpriteSetCenter(&sprites[i], 0.5f, 0.5f);
	}
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON], 13, 13);
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON_1], 9, 12);
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON_2], 9, 26);
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON_3], 9, 31);
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON_4], 9, 45);
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON_5], 9, 51);
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON_6], 9, 59);
	for (int i = 0; i < 4; i++) C2D_SpriteSetPos(&sprites[SPRITE_EFFECT_PERFECT + i], 93, 109);

	C2D_SpriteSetPos(&sprites[SPRITE_EFFECT_GOGO], 110, 92);

	C2D_SpriteSetPos(&sprites[SPRITE_TOP], TOP_WIDTH / 2, TOP_HEIGHT / 2);
	C2D_SpriteSetPos(&sprites[SPRITE_BOTTOM], BOTTOM_WIDTH / 2, BOTTOM_HEIGHT / 2);
	for (int i = 0; i < 5; i++)C2D_SpriteSetPos(&sprites[SPRITE_EMBLEM_EASY + i], 31, 113);
}

bool get_isPause() {
	return isPause;
}

void debug_touch(int x,int y) {

	snprintf(buffer, sizeof(buffer), "%d:%d:%.1f\n%d:%d:%d", 
		PreTouch_x-touch_x,
		PreTouch_y-touch_y,
		pow((touch_x - PreTouch_x)*(touch_x - PreTouch_x) + (touch_y - PreTouch_y)*(touch_y - PreTouch_y), 0.5), 
		touch_x,touch_y,touch_cnt);
	draw_debug(0, 0, buffer);
}

bool get_isMusicStart() {
	return isMusicStart;
}

char *get_buffer() {
	return buffer;
}

int powi(int x, int y) {	//なぜかpowのキャストが上手くいかないので整数用powを自作

	int ans = 1;

	for (int i = 0; i < y; i++) {
		ans = ans * x;
	}
	return ans;
}

C2D_TextBuf g_MainText = C2D_TextBufNew(4096);
C2D_Text MainText;

void draw_window_text(float x, float y, const char* text, float* width, float* height,float size = 1.0) {

	C2D_TextBufClear(g_MainText);
	C2D_TextParse(&MainText, g_MainText, text);
	C2D_TextOptimize(&MainText);

	C2D_TextGetDimensions(&MainText, size, size, width, height);
	C2D_DrawText(&MainText, C2D_WithColor, BOTTOM_WIDTH / 2 - *width / 2, y, 1.0f, size, size, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
}

int pause_window(touchPosition tp, unsigned int key) {

	int margin = 20, result = -1, x, y;
	float width, height;

	C2D_DrawRectSolid(margin, margin, 0, BOTTOM_WIDTH - margin * 2, BOTTOM_HEIGHT - margin * 2, C2D_Color32f(0, 0, 0, 1));

	draw_window_text(-1, margin + 30, Text[get_lang()][TEXT_CONTINUE], &width, &height);		//続ける
	x = BOTTOM_WIDTH / 2 - width / 2, y = margin + 30;
	if ((y < tp.py && y + height > tp.py && x < tp.px && x + width > tp.px) && key & KEY_TOUCH) result = 0;

	draw_window_text(-1, margin + 80, Text[get_lang()][TEXT_STARTOVER], &width, &height);		//はじめから
	x = BOTTOM_WIDTH / 2 - width / 2, y = margin + 80;
	if ((y < tp.py && y + height > tp.py && x < tp.px && x + width > tp.px) && key & KEY_TOUCH) result = 1;

	draw_window_text(-1, margin + 130, Text[get_lang()][TEXT_RETURNSELECT], &width, &height);	//曲選択に戻る
	x = BOTTOM_WIDTH / 2 - width / 2, y = margin + 130;
	if ((y < tp.py && y + height > tp.py && x < tp.px && x + width > tp.px) && key & KEY_TOUCH) result = 2;

	return result;
}

int message_window(touchPosition tp, unsigned int key,int text) {

	int margin = 20,result = -1, x, y;
	float width, height;

	C2D_DrawRectSolid(margin, margin, 0, BOTTOM_WIDTH - margin * 2, BOTTOM_HEIGHT - margin * 2, C2D_Color32f(0, 0, 0, 1));
	draw_window_text(-1, margin + 50, Text[get_lang()][text], &width, &height,0.5);

	draw_window_text(-1, margin + 150, "OK", &width, &height);
	x = BOTTOM_WIDTH / 2 - width / 2, y = margin + 150;
	if ((y < tp.py && y + height > tp.py && x < tp.px && x + width > tp.px) && key & KEY_TOUCH) result = 1;

	return result;
}