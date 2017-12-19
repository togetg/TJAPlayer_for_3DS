#include "header.h"
#include "notes.h"
#include "tja.h"
#include "audio.h"
#include "taiko_png.h"
#include "bg_png.h"
#include "circle_png.h"
#include "don_png.h"
#include "ka_png.h"
#include "big_don_png.h"
#include "big_ka_png.h"
#include "renda_png.h"
#include "big_renda_png.h"
#include "renda_fini_png.h"
#include "big_renda_fini_png.h"
#include "balloon_png.h"
#include "FreeSans_ttf.h"

int main(int argc, char* argv[]) {
	sf2d_init();
	sftd_init();
	ndspInit();
	touchPosition tp;
	const int image_number = 12;
	sf2d_texture *tex[image_number];
	tex[0] = sfil_load_PNG_buffer(bg_png, SF2D_PLACE_RAM);
	tex[1] = sfil_load_PNG_buffer(taiko_png, SF2D_PLACE_RAM);
	tex[2] = sfil_load_PNG_buffer(circle_png, SF2D_PLACE_RAM);
	tex[3] = sfil_load_PNG_buffer(don_png, SF2D_PLACE_RAM);
	tex[4] = sfil_load_PNG_buffer(ka_png, SF2D_PLACE_RAM);
	tex[5] = sfil_load_PNG_buffer(big_don_png, SF2D_PLACE_RAM);
	tex[6] = sfil_load_PNG_buffer(big_ka_png, SF2D_PLACE_RAM);
	tex[7] = sfil_load_PNG_buffer(renda_png, SF2D_PLACE_RAM);
	tex[8] = sfil_load_PNG_buffer(big_renda_png, SF2D_PLACE_RAM);
	tex[9] = sfil_load_PNG_buffer(renda_fini_png, SF2D_PLACE_RAM);
	tex[10] = sfil_load_PNG_buffer(big_renda_fini_png, SF2D_PLACE_RAM);
	tex[11] = sfil_load_PNG_buffer(balloon_png, SF2D_PLACE_RAM);
	sftd_font *font = sftd_load_font_mem(FreeSans_ttf, FreeSans_ttf_size);

	int cnt = 0, notes_cnt = 0, first_sec_time;
	bool notes_start_flag = false,music_start_flag=false;
	double offset, bpm;int measure=4;double *p_offset = &offset, *p_bpm = &bpm;int *p_measure = &measure;
	char now_notes; char *p_now_notes = &now_notes;
	tja_head_load(p_offset,p_bpm,p_measure);
	tja_notes_load();
	music_load();

	while (aptMainLoop()) {
		hidScanInput();
		hidTouchRead(&tp);
		unsigned int key = hidKeysDown();
		if (key & KEY_START) break;
		if (cnt == 0) first_sec_time = (int)(400-93)*(3600 / bpm*measure)/400;
		if (offset >= 0 && (notes_start_flag == false || music_start_flag == false)) {
			if (cnt==0 && notes_start_flag==false) //notes_start_flag = true;
			if (cnt == (int)(offset * 60)) {
				//music_play(2);
				music_start_flag = true;
			}
			
		}else if (offset < 0 && (notes_start_flag == false || music_start_flag==false)) {
			if (cnt == first_sec_time - (int)(offset*-60)) {
				music_play(2);
				music_start_flag = true;
			}
			//if (cnt == (int)(offset*-60) && notes_start_flag == false) {
			if (cnt == 0 && notes_start_flag == false) {
				notes_start_flag = true;
			}
		}
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(tex[0], 400 / 2 - tex[0]->width / 2, 240 / 2 - tex[0]->height / 2);
		//sf2d_draw_texture(tex[2],25,44);
		sftd_draw_textf(font, 10, 0, RGBA8(0, 255, 0, 255), 20, "FPS %f", sf2d_get_fps());
		if (notes_start_flag == true) tja_to_notes(p_now_notes, notes_cnt, font, tex[3], tex[4], tex[5], tex[6], tex[7], tex[8], tex[9], tex[10], tex[11]);
		switch (*p_now_notes) {
		case '1':
			music_play(0);
			break;
		case '2':
			music_play(1);
			break;
		default:
			break;
		}
		sf2d_draw_rectangle(0, 86, 63, 58, RGBA8(255, 0, 0, 255));
		sftd_draw_textf(font, 0, 100, RGBA8(0, 255, 0, 255), 10, "CLOCK:%d", clock());
		//sf2d_draw_rectangle(100, 43, 1, 47, RGBA8(255, 255, 255, 255));
		sf2d_end_frame();

		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(tex[1], 320 / 2 - tex[1]->width / 2, 240 / 2 - tex[1]->height / 2);

		sf2d_draw_fill_circle(160, 145, 105, RGBA8(0xFF, 0x00, 0xFF, 0xFF));
		if ((((tp.px - 160)*(tp.px - 160) + (tp.py - 145)*(tp.py - 145)) <= 105 * 105 && key & KEY_TOUCH )|| 
			key & KEY_B ||
			key & KEY_Y ||
			key & KEY_RIGHT ||
			key & KEY_DOWN ||
			key & KEY_CSTICK_LEFT ||
			key & KEY_CSTICK_DOWN) {//ƒhƒ“
			sftd_draw_text(font, 10, 10, RGBA8(255, 0, 0, 255), 20, "Don!");
			music_play(0);
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
			key & KEY_ZL) {//ƒJ
			sftd_draw_text(font, 10, 10, RGBA8(0, 0, 255, 255), 20, "Ka!");
			music_play(1);
		}
		sftd_draw_textf(font, 10, 0, RGBA8(0, 255, 0, 255), 15, "cnt:%d", cnt);
		sftd_draw_textf(font, 10, 15, RGBA8(0, 255, 0, 255), 15, "notes_cnt:%d", notes_cnt);
		sftd_draw_textf(font, 10, 30, RGBA8(0, 255, 0, 255), 15, "notes_cnt-30:%d", notes_cnt-30);
		sftd_draw_textf(font, 10, 45, RGBA8(0, 255, 0, 255), 15, "%c", now_notes);
		sftd_draw_textf(font, 10, 60, RGBA8(0, 255, 0, 255), 15, "notes_start_flag:%d", notes_start_flag);
		sftd_draw_textf(font, 10, 75, RGBA8(0, 255, 0, 255), 15, "music_start_flag:%d", music_start_flag);
		sftd_draw_textf(font, 10, 90, RGBA8(0, 255, 0, 255), 15, "first_sec_time:%d", first_sec_time);
		sf2d_end_frame();
		sf2d_swapbuffers();
		//sf2d_set_vblank_wait(true);
		if (notes_start_flag == true) notes_cnt++;
		cnt++;
	}
	for (int i = 0; i < image_number; i++) {
		sf2d_free_texture(tex[i]);
	}

	sf2d_fini();
	sftd_fini();
	music_exit();
	return 0;
}