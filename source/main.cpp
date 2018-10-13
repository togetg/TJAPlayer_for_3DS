#include "header.h"
#include "time.h"
#include "notes.h"
#include "tja.h"
#include "audio.h"
#include "playback.h"
#include "score.h"
#include "select.h"

C2D_Sprite sprites[Sprite_Number];			//画像用
static C2D_SpriteSheet spriteSheet;
C2D_TextBuf g_dynamicBuf;
char buf_main[160];
C2D_Text dynText;

void sprites_load();

void debug_draw(float x, float y, const char *text) {

	C2D_TextBufClear(g_dynamicBuf);
	C2D_TextParse(&dynText, g_dynamicBuf, text);
	C2D_TextOptimize(&dynText);
	C2D_DrawText(&dynText, C2D_WithColor, x, y, 0.5f, 0.5f, 0.5f, C2D_Color32f(0.0f, 1.0f, 0.0f, 1.0f));
}

void main_init() {

	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
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

	main_init();

	touchPosition tp;	//下画面タッチした座標

	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	TJA_HEADER_T TJA_Header;
	LIST_T SelectedSong;

	g_dynamicBuf = C2D_TextBufNew(4096);
	sprites_load();	
	music_load();

	int cnt = 0, notes_cnt = 0, scene_state = SelectSong;
	bool isNotesStart = false, isMusicStart = false, isPlayMain = false;
	double FirstMeasureTime = INT_MAX,
		offset=0,
		NowTime;

	while (aptMainLoop()) {

		hidScanInput();
		hidTouchRead(&tp);

		unsigned int key = hidKeysDown();
		if (key & KEY_START) break;

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		C2D_TargetClear(top, C2D_Color32(0x42, 0x42, 0x42, 0xFF));	//上画面
		C2D_SceneBegin(top);

		switch (scene_state) {

		case SelectSong:	//選曲

			if (cnt == 0) {
				load_file_main();
			}

			if (key & KEY_UP)		cursor_update(KEY_UP);
			if (key & KEY_DOWN)		cursor_update(KEY_DOWN);
			if (key & KEY_RIGHT)	cursor_update(KEY_RIGHT);
			if (key & KEY_LEFT)		cursor_update(KEY_LEFT);
			if (key & KEY_A)		cursor_update(KEY_A);
			if (key & KEY_B)		cursor_update(KEY_B);

			disp_file_list();
			
			if (key & KEY_SELECT) {
				scene_state =  MainLoad;
				cnt = -1;
			}
			get_SelectedId(&SelectedSong);

			C2D_TargetClear(bottom, C2D_Color32(0x00, 0x00, 0x00, 0xFF));	//下画面
			C2D_SceneBegin(bottom);
			//C2D_DrawSprite(&sprites[bOttom]);
			debug_draw(0, 0, SelectedSong.path);
			debug_draw(0, 10, SelectedSong.tja);
			debug_draw(0, 20, SelectedSong.wave);
			debug_draw(0, 30, SelectedSong.title);

			break;

		case MainLoad:

			tja_init();
			tja_head_load(Score_Course);
			init_main_music();
			get_tja_header(&TJA_Header);
			score_init();
			notes_init(TJA_Header);
			offset = TJA_Header.offset;

			scene_state = MainGame;
			cnt = -1;
			break;

		case MainGame:		//メイン

			C2D_DrawSprite(&sprites[tOp]);

			NowTime = time_now(1);

			if (cnt == 0) {

				FirstMeasureTime = get_FirstMeasureTime();
				play_main_music(&isPlayMain,SelectedSong);
			}

			//譜面が先
			if (offset > 0 && (isNotesStart == false || isMusicStart == false)) {
				if (NowTime >= 0 && isNotesStart == false) isNotesStart = true;
				if (NowTime >= offset + FirstMeasureTime && isMusicStart == false) {
					isPlayMain = true;
					isMusicStart = true;
				}
			}

			//音が先
			else if (offset <= 0 && (isNotesStart == false || isMusicStart == false)) {

				if (NowTime >= FirstMeasureTime && isPlayMain == false) {
					isPlayMain = true;
					isMusicStart = true;
				}
				if (NowTime >= (-1.0) * offset && isNotesStart == false) {
					isNotesStart = true;
				}
			}

			bool isDon = false, isKatsu = false;
			if ((((tp.px - 160)*(tp.px - 160) + (tp.py - 145)*(tp.py - 145)) <= 105 * 105 && key & KEY_TOUCH) ||
				key & KEY_B ||
				key & KEY_Y ||
				key & KEY_RIGHT ||
				key & KEY_DOWN ||
				key & KEY_CSTICK_LEFT ||
				key & KEY_CSTICK_DOWN) {//ドン
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
				key & KEY_ZL) {//カツ
				isKatsu = true;
			}

			if (key & KEY_SELECT) toggle_auto();

			draw_fps();
			draw_lane(sprites);
			draw_gauge(sprites);

			if (isNotesStart == true) {
				tja_to_notes(isDon, isKatsu, notes_cnt, sprites);
				notes_cnt++;
			}
			draw_score(sprites);
			//score_debug();

			C2D_TargetClear(bottom, C2D_Color32(0x00, 0x00, 0x00, 0xFF));	//下画面
			C2D_SceneBegin(bottom);
			C2D_DrawSprite(&sprites[bOttom]);

			if (isDon == true) {	//ドン
				music_play(0);
			}
			if (isKatsu == true) {		//カツ
				music_play(1);
			}
			break;
		}

			C3D_FrameEnd(0);
			cnt++;
	}

	main_exit();
	return 0;
}

void sprites_load(){

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
	for (int i = 0; i < 2; i++) C2D_SpriteSetPos(&sprites[sOul_on + i], 385, 75);
	C2D_SpriteSetPos(&sprites[eFfect_gogo], 110, 92);

	C2D_SpriteSetPos(&sprites[tOp], TOP_WIDTH / 2, TOP_HEIGHT / 2);
	C2D_SpriteSetPos(&sprites[bOttom], BOTTOM_WIDTH / 2, BOTTOM_HEIGHT / 2);
}