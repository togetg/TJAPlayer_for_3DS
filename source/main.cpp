#include "header.h"
#include "time.h"
#include "notes.h"
#include "tja.h"
#include "audio.h"

int main(int argc, char* argv[]) {
	sf2d_init();
	sftd_init();
	ndspInit();
	romfsInit();
	touchPosition tp;
	const int image_number = 12;
	sf2d_texture *tex[image_number];
	tex[0] = sfil_load_PNG_file("romfs:/bg.png", SF2D_PLACE_RAM);
	tex[1] = sfil_load_PNG_file("romfs:/taiko.png", SF2D_PLACE_RAM);
	tex[2] = sfil_load_PNG_file("romfs:/circle.png", SF2D_PLACE_RAM);
	tex[3] = sfil_load_PNG_file("romfs:/don.png", SF2D_PLACE_RAM);
	tex[4] = sfil_load_PNG_file("romfs:/ka.png", SF2D_PLACE_RAM);
	tex[5] = sfil_load_PNG_file("romfs:/big_don.png", SF2D_PLACE_RAM);
	tex[6] = sfil_load_PNG_file("romfs:/big_ka.png", SF2D_PLACE_RAM);
	tex[7] = sfil_load_PNG_file("romfs:/renda.png", SF2D_PLACE_RAM);
	tex[8] = sfil_load_PNG_file("romfs:/big_renda.png", SF2D_PLACE_RAM);
	tex[9] = sfil_load_PNG_file("romfs:/renda_fini.png", SF2D_PLACE_RAM);
	tex[10] = sfil_load_PNG_file("romfs:/big_renda_fini.png", SF2D_PLACE_RAM);
	tex[11] = sfil_load_PNG_file("romfs:/balloon.png", SF2D_PLACE_RAM);
	sftd_font *font = sftd_load_font_file("romfs:/FreeSans.ttf");

	int cnt = 0, notes_cnt = 0;
	bool notes_start_flag = false,music_start_flag=false;
	double first_sec_time=9999.0,offset, bpm,time,first_time;int measure=4;double *p_offset = &offset, *p_bpm = &bpm;int *p_measure = &measure;
	char now_notes; char *p_now_notes = &now_notes;
	tja_head_load(p_offset,p_bpm,p_measure);
	tja_notes_load();
	music_load();

	while (aptMainLoop()) {
		hidScanInput();
		hidTouchRead(&tp);
		unsigned int key = hidKeysDown();
		if (key & KEY_START) break;
		time = time_now(1);
		if (cnt == 0) first_sec_time = (60.0/bpm*measure)*(307.0/400.0)+ 60.0 / bpm;
		if (offset >= 0 && (notes_start_flag == false || music_start_flag == false)) {
			if (cnt==0 && notes_start_flag==false) //notes_start_flag = true;
			if (cnt == (int)(offset * 60)) {
				//music_play(2);
				music_start_flag = true;
			}
			
		}else if (offset < 0 && (notes_start_flag == false || music_start_flag==false)) {
			//if (cnt == 0) first_time = time;
			if (time >= first_sec_time ) {
				music_play(2);
				music_start_flag = true;
			}
			if (time >= (-1.0) * offset && notes_start_flag == false) {
				notes_start_flag = true;
			}
		}
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(tex[0], 400 / 2 - tex[0]->width / 2, 240 / 2 - tex[0]->height / 2);
		//sf2d_draw_texture(tex[2],25,44);
		if (notes_start_flag == true) {
			tja_to_notes(p_now_notes, notes_cnt, font, tex[3], tex[4], tex[5], tex[6], tex[7], tex[8], tex[9], tex[10], tex[11]);
			notes_cnt++;
		}
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
		sf2d_end_frame();
		sf2d_swapbuffers();
		//sf2d_set_vblank_wait(true);
		cnt++;
	}
	for (int i = 0; i < image_number; i++) {
		sf2d_free_texture(tex[i]);
	}

	sf2d_fini();
	sftd_fini();
	romfsExit();
	music_exit();
	return 0;
}