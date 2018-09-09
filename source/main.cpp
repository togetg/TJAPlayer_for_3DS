#include "header.h"
#include "time.h"
#include "notes.h"
#include "tja.h"
#include "audio.h"
#include "playback.h"

C2D_Sprite sprites[Sprite_Number];			//画像用
static C2D_SpriteSheet spriteSheet;
C2D_TextBuf g_dynamicBuf;
char buf_main[160];
C2D_Text dynText;
TJA_HEADER_T Tja_Header;

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
	init_main_music();
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

	g_dynamicBuf = C2D_TextBufNew(4096);

	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);

	for (int i = 0; i < Sprite_Number; i++) {
		C2D_SpriteFromSheet(&sprites[i], spriteSheet, i);
		C2D_SpriteSetCenter(&sprites[i], 0.5f, 0.5f);
	}
	C2D_SpriteSetCenterRaw(&sprites[bAlloon], 13, 13);
	C2D_SpriteSetCenterRaw(&sprites[bAlloon_1], 8.5, 12);
	C2D_SpriteSetCenterRaw(&sprites[bAlloon_2], 9, 26);
	C2D_SpriteSetCenterRaw(&sprites[bAlloon_3], 9, 31);
	C2D_SpriteSetCenterRaw(&sprites[bAlloon_4], 9, 45);
	C2D_SpriteSetCenterRaw(&sprites[bAlloon_5], 9, 51);
	C2D_SpriteSetCenterRaw(&sprites[bAlloon_6], 9, 59);

	C2D_SpriteSetPos(&sprites[0], TOP_WIDTH / 2, TOP_HEIGHT / 2);
	C2D_SpriteSetPos(&sprites[1], BOTTOM_WIDTH / 2, BOTTOM_HEIGHT / 2);

	tja_init();
	tja_head_load();
	music_load();
	init_main_music();
	get_head(&Tja_Header);
	notes_init(Tja_Header);

	int cnt = 0, notes_cnt = 0;
	bool isNotesStart = false, isMusicStart = false;
	double FirstMeasureTime = INT_MAX,
		offset = Tja_Header.offset,
		NowTime;

	while (aptMainLoop()) {

		hidScanInput();
		hidTouchRead(&tp);

		unsigned int key = hidKeysDown();
		if (key & KEY_START) break;

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		C2D_TargetClear(top, C2D_Color32(0x00, 0x00, 0x00, 0xFF));	//上画面
		C2D_SceneBegin(top);
		C2D_DrawSprite(&sprites[0]);

		NowTime = time_now(1);
		bool isPlayMain;

		if (cnt == 0) {

			FirstMeasureTime = get_FirstMeasureTime();
			play_main_music(&isPlayMain);
		}

		draw_fps();

		debug_draw(200, 20, buf_main);
		debug_draw(50, 200, "日本語テスト");
		
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
		
		bool isDon = false, isKa = false;
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
		isKa = true;
		}

		if (key & KEY_SELECT) toggle_auto();

		if (isNotesStart == true) {
			tja_to_notes(isDon, isKa, notes_cnt,sprites);
			notes_cnt++;
		}

		C2D_TargetClear(bottom, C2D_Color32(0x00, 0x00, 0x00, 0xFF));	//下画面
		C2D_SceneBegin(bottom);
		C2D_DrawSprite(&sprites[1]);
		
		if (isDon == true) {	//ドン
		music_play(0);
		}
		if (isKa == true) {		//カツ
		music_play(1);
		}
		
		C3D_FrameEnd(0);
		cnt++;
	}

	main_exit();
	return 0;
}