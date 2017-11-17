#include "header.h"
#include "notes.h"
#include "tja.h"

#include "taiko_png.h"
#include "bg_png.h"
#include "circle_png.h"
#include "don_png.h"
#include "ka_png.h"
#include "FreeSans_ttf.h"

int main(int argc, char* argv[]) {
	sf2d_init();
	sftd_init();
	ndspInit();
	touchPosition tp;
	const int image_number = 5;
	sf2d_texture *tex[image_number];
	tex[0] = sfil_load_PNG_buffer(bg_png, SF2D_PLACE_RAM);
	tex[1] = sfil_load_PNG_buffer(taiko_png, SF2D_PLACE_RAM);
	tex[2] = sfil_load_PNG_buffer(circle_png, SF2D_PLACE_RAM);
	tex[3] = sfil_load_PNG_buffer(don_png, SF2D_PLACE_RAM);
	tex[4] = sfil_load_PNG_buffer(ka_png, SF2D_PLACE_RAM);
	sftd_font *font = sftd_load_font_mem(FreeSans_ttf, FreeSans_ttf_size);

	const int sounds_number = 3;
	int channel[sounds_number];
	for (int i = 0; i < sounds_number;i++) {
		channel[i] = i;
	}
	char sound_address[sounds_number][30] = { "tjafiles/don.wav","tjafiles/ka.wav","tjafiles/syaruru.wav" };
	u32 sampleRate[sounds_number];
	u32 dataSize[sounds_number];
	u16 channels[sounds_number];
	u16 bitsPerSample[sounds_number];
	u8* data[sounds_number];
	u16 ndspFormat[sounds_number];
	ndspWaveBuf waveBuf[sounds_number];

	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspSetOutputCount(1); // Num of buffers

	for (int i = 0; i < sounds_number; i++) {
		FILE* fp = fopen(sound_address[i], "rb");
		if (!fp) {
			printf("Could not open the example.wav file.\n");
			return 1;
		}
		char signature[4];
		fread(signature, 1, 4, fp);
		if (signature[0] != 'R' && signature[1] != 'I' && signature[2] != 'F' && signature[3] != 'F') {
			printf("Wrong file format.\n");
			fclose(fp);
			return 1;
		}
		fseek(fp, 40, SEEK_SET); fread(&dataSize[i], 4, 1, fp);
		fseek(fp, 22, SEEK_SET); fread(&channels[i], 2, 1, fp);
		fseek(fp, 24, SEEK_SET); fread(&sampleRate[i], 4, 1, fp);
		fseek(fp, 34, SEEK_SET); fread(&bitsPerSample[i], 2, 1, fp);

		if (dataSize[i] == 0 || (channels[i] != 1 && channels[i] != 2) || (bitsPerSample[i] != 8 && bitsPerSample[i] != 16)) {
			printf("Corrupted wav file.\n");
			fclose(fp);
			return 1;
		}
		// Allocating and reading samples
		data[i] = static_cast<u8*>(linearAlloc(dataSize[i]));
		if (!data[i]) {
			fclose(fp);
			return 1;
		}

		fseek(fp, 44, SEEK_SET); fread(data[i], 1, dataSize[i], fp);
		fclose(fp);
		// Find the right format
		if (bitsPerSample[i] == 8) {
			ndspFormat[i] = (channels[i] == 1) ?
				NDSP_FORMAT_MONO_PCM8 :
				NDSP_FORMAT_STEREO_PCM8;
		}
		else {
			ndspFormat[i] = (channels[i] == 1) ?
				NDSP_FORMAT_MONO_PCM16 :
				NDSP_FORMAT_STEREO_PCM16;
		}
		ndspChnReset(channel[i]);
		ndspChnSetInterp(channel[i], NDSP_INTERP_NONE);
		ndspChnSetRate(channel[i], float(sampleRate[i]));
		ndspChnSetFormat(channel[i], ndspFormat[i]);
		// Create and play a wav buffer
		std::memset(&waveBuf[i], 0, sizeof(ndspWaveBuf));
		waveBuf[i].data_vaddr = (const void*)data[i];
		waveBuf[i].nsamples = dataSize[i] / (bitsPerSample[i] >> 3);
		waveBuf[i].looping = false; // Loop enabled
		waveBuf[i].status = NDSP_WBUF_FREE;

		DSP_FlushDataCache(data[i], dataSize[i]);
	}
	int cnt = 0;
	tja_head_load();
	tja_notes_load();

	while (aptMainLoop()) {
		hidScanInput();
		hidTouchRead(&tp);
		unsigned int key = hidKeysDown();

		if (key & KEY_START) break;

		if (cnt == 30) ndspChnWaveBufAdd(channel[2], &waveBuf[2]);

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(tex[0], 400 / 2 - tex[0]->width / 2, 240 / 2 - tex[0]->height / 2);
		sf2d_draw_texture(tex[2],25,44);
		sftd_draw_textf(font, 10, 0, RGBA8(0, 255, 0, 255), 20, "FPS %f", sf2d_get_fps());
		tja_to_notes(cnt, font,tex[3],tex[4]);

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
			ndspChnReset(channel[0]);
			ndspChnSetInterp(channel[0], NDSP_INTERP_NONE);
			ndspChnSetRate(channel[0], float(sampleRate[0]));
			ndspChnSetFormat(channel[0], ndspFormat[0]);
			ndspChnWaveBufAdd(channel[0], &waveBuf[0]);
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
			ndspChnReset(channel[1]);
			ndspChnSetInterp(channel[1], NDSP_INTERP_NONE);
			ndspChnSetRate(channel[1], float(sampleRate[1]));
			ndspChnSetFormat(channel[1], ndspFormat[1]);
			ndspChnWaveBufAdd(channel[1], &waveBuf[1]);
		}
		sf2d_end_frame();
		sf2d_swapbuffers();
		//sf2d_set_vblank_wait(true);
		cnt++;
	}

	for (int i = 0; i < sounds_number; i++) {
		ndspChnWaveBufClear(channel[i]);
		linearFree(data[i]);
	}
	for (int i = 0; i < image_number; i++) {
		sf2d_free_texture(tex[i]);
	}

	sf2d_fini();
	sftd_fini();
	ndspExit();
	
	return 0;
}