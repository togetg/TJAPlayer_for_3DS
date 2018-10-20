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

C2D_Sprite sprites[Sprite_Number];			//画像用
static C2D_SpriteSheet spriteSheet;
C2D_TextBuf g_dynamicBuf;
char buf_main[160];
C2D_Text dynText;
bool isPause = false;

void load_sprites();

void draw_debug(float x, float y, const char *text) {

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

int main() {

	init_main();

	touchPosition tp;	//下画面タッチした座標

	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	TJA_HEADER_T TJA_Header;
	LIST_T SelectedSong;

	load_sprites();
	load_music();

	int cnt = 0, notes_cnt = 0, scene_state = SelectLoad, course = ONI;
	bool isNotesStart = false, isMusicStart = false, isPlayMain = false,isExit = false;
	double FirstMeasureTime = INT_MAX,
		offset = 0,
		NowTime = -1000;

	while (aptMainLoop()) {

		hidScanInput();
		hidTouchRead(&tp);
		unsigned int key = hidKeysDown();

		if (isExit == true) break;

		bool isDon = false, isKatsu = false;

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		C2D_TargetClear(top, C2D_Color32(0x42, 0x42, 0x42, 0xFF));	//上画面
		C2D_SceneBegin(top);

		switch (scene_state) {

		case SelectLoad:

			draw_select_text(30, 0, "Now Loading...");

			scene_state = SelectSong;
			cnt = -1;
			load_file_main();
			init_option();
			break;

		case SelectSong:	//選曲

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
				scene_state = MainLoad;
				cnt = -1;
			}
			get_SelectedId(&SelectedSong, &course);

			C2D_TargetClear(bottom, C2D_Color32(0x42, 0x42, 0x42, 0xFF));	//下画面
			C2D_SceneBegin(bottom);
			//C2D_DrawSprite(&sprites[bOttom]);
			draw_option(tp.px, tp.py, key);
			draw_debug(0, 40, buf_main);
			isPause = false;

			break;

		case MainLoad:

			init_tja();
			load_tja_head(course, SelectedSong);
			init_main_music();
			get_tja_header(&TJA_Header);
			init_score();
			init_notes(TJA_Header);
			load_tja_notes(course, SelectedSong);
			time_ini();
			offset = TJA_Header.offset;
			notes_cnt = 0;
			isNotesStart = false, isMusicStart = false, isPlayMain = false;
			FirstMeasureTime = INT_MAX;
			NowTime = -1000;

			scene_state = MainGame;
			cnt = -120;
			break;

		case MainGame:		//メイン

			if (key & KEY_START) isExit = true;

			C2D_DrawSprite(&sprites[tOp]);
			//C2D_DrawRectSolid(0, 86, 0, 62, 58, C2D_Color32f(1, 0, 0, 1));
			draw_emblem(sprites);

			if (cnt >= 0) NowTime = time_now(1);

			if (cnt == 0) {

				FirstMeasureTime = get_FirstMeasureTime();
				play_main_music(&isPlayMain, SelectedSong);
			}

			//譜面が先
			if (offset > 0 && (isNotesStart == false || isMusicStart == false)) {

				if (NowTime >= 0 && isNotesStart == false) isNotesStart = true;
				if (NowTime >= offset + FirstMeasureTime && isMusicStart == false) {
					isPlayMain = true;
					isMusicStart = true;
				}
			}

			//音楽が先
			else if (offset <= 0 && (isNotesStart == false || isMusicStart == false)) {

				if (NowTime >= FirstMeasureTime && isPlayMain == false) {
					isPlayMain = true;
					isMusicStart = true;
				}
				if (NowTime >= (-1.0) * offset && isNotesStart == false) {
					isNotesStart = true;
				}
			}

			if ((((tp.px - 160)*(tp.px - 160) + (tp.py - 135)*(tp.py - 135)) <= 105 * 105 && key & KEY_TOUCH) ||
				key & KEY_B ||
				key & KEY_Y ||
				key & KEY_RIGHT ||
				key & KEY_DOWN ||
				key & KEY_CSTICK_LEFT ||
				key & KEY_CSTICK_DOWN) {	//ドン
				isDon = true;
			}
			else if (key & KEY_TOUCH ||
				key & KEY_A ||
				key & KEY_X ||
				key & KEY_LEFT ||
				key & KEY_UP ||
				key & KEY_CSTICK_RIGHT ||
				key & KEY_CSTICK_UP ||
				key & KEY_R ||
				key & KEY_L ||
				key & KEY_ZR ||
				key & KEY_ZL) {				//カツ
				isKatsu = true;
			}
			if (isDon == true) music_play(0);		//ドン
			if (isKatsu == true) music_play(1);		//カツ

			//if (key & KEY_SELECT) toggle_auto();
			if (key & KEY_SELECT) {
				togglePlayback();
				toggle_time(0);
				toggle_time(1);
				isPause = !isPause;
			}
			draw_fps();
			draw_lane(sprites);
			draw_gauge(sprites);

			if (isNotesStart == true) {
				tja_to_notes(isDon, isKatsu, notes_cnt, sprites);
				if (isPause == false) notes_cnt++;
			}
			draw_score(sprites);
			//score_debug();

			C2D_TargetClear(bottom, C2D_Color32(0x42, 0x42, 0x42, 0xFF));	//下画面
			C2D_SceneBegin(bottom);
			C2D_DrawSprite(&sprites[bOttom]);

			if (get_notes_finish() == true && ndspChnIsPlaying(CHANNEL) == false) {
				scene_state = ResultGame;
				cnt = -1;
			}
			break;

		case ResultGame:

			if (key & KEY_START) {
				cnt = -1;
				scene_state = SelectSong;
			}
			draw_gauge_result(sprites);
			draw_result();
			break;
		}

		C3D_FrameEnd(0);
		if (isPause == false) cnt++;
	}

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
	C2D_SpriteSetCenterRaw(&sprites[bAlloon], 13, 13);
	C2D_SpriteSetCenterRaw(&sprites[bAlloon_1], 9, 12);
	C2D_SpriteSetCenterRaw(&sprites[bAlloon_2], 9, 26);
	C2D_SpriteSetCenterRaw(&sprites[bAlloon_3], 9, 31);
	C2D_SpriteSetCenterRaw(&sprites[bAlloon_4], 9, 45);
	C2D_SpriteSetCenterRaw(&sprites[bAlloon_5], 9, 51);
	C2D_SpriteSetCenterRaw(&sprites[bAlloon_6], 9, 59);
	for (int i = 0; i < 4; i++) C2D_SpriteSetPos(&sprites[eFfect_perfect + i], 93, 109);

	C2D_SpriteSetPos(&sprites[eFfect_gogo], 110, 92);

	C2D_SpriteSetPos(&sprites[tOp], TOP_WIDTH / 2, TOP_HEIGHT / 2);
	C2D_SpriteSetPos(&sprites[bOttom], BOTTOM_WIDTH / 2, BOTTOM_HEIGHT / 2);
	for (int i = 0; i < 5; i++)C2D_SpriteSetPos(&sprites[eMblem_easy + i], 31, 113);
}

bool get_isPause() {
	return isPause;
}