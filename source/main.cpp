#include "header.h"

#include "taiko_png.h"
#include "bg_png.h"
#include "FreeSans_ttf.h"

int main(){
	ndspInit();
	sf2d_init();
	sftd_init();
	romfsInit();
	dspInit();
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

	touchPosition tp;	//タッチ座標
	sf2d_texture *tex[2];
	sftd_font *font = sftd_load_font_mem(FreeSans_ttf, FreeSans_ttf_size);
	tex[0] = sfil_load_PNG_buffer(bg_png, SF2D_PLACE_RAM);
	tex[1] = sfil_load_PNG_buffer(taiko_png, SF2D_PLACE_RAM);

	//以下ndspサンプルのコピペ
	ndspWaveBuf waveBuf[2];
	constexpr int channel[2] = { 1,1 };
	u32 sampleRate[2];
	u32 dataSize;
	u16 channels;
	u16 bitsPerSample;
	u16 ndspFormat;

	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspSetOutputCount(2); // Num of buffers

						   // Reading wav file
	FILE* fp = fopen("romfs:/resources/sounds/don.wav", "rb");
	if (!fp) {
		printf("Could not open the example.wav file.\n");
		//return 1;
	}

	char signature[4];
	fread(signature, 1, 4, fp);
	if (signature[0] != 'R' && signature[1] != 'I' && signature[2] != 'F' && signature[3] != 'F') {
		//printf("Wrong file format.\n");
		fclose(fp);
		//return 1;
	}

	fseek(fp, 0, SEEK_END);
	dataSize = ftell(fp);
	fseek(fp, 22, SEEK_SET);
	fread(&channels, 2, 1, fp);
	fseek(fp, 24, SEEK_SET);
	fread(&sampleRate[0], 4, 1, fp);
	fseek(fp, 34, SEEK_SET);
	fread(&bitsPerSample, 2, 1, fp);

	if (dataSize == 0 || (channels != 1 && channels != 2) || (bitsPerSample != 8 && bitsPerSample != 16)) {
		//printf("Corrupted wav file.\n");
		fclose(fp);
		//return 1;
	}

	// Allocating and reading samples
	u8* data = static_cast<u8*>(linearAlloc(dataSize));

	if (!data) {
		fclose(fp);
		//return 1;
	}

	fseek(fp, 44, SEEK_SET);
	fread(data, 1, dataSize, fp);
	fclose(fp);

	// Find the right format

	if (bitsPerSample == 8) {
		ndspFormat = (channels == 1) ?
			NDSP_FORMAT_MONO_PCM8 :
			NDSP_FORMAT_STEREO_PCM8;
	}
	else {
		ndspFormat = (channels == 1) ?
			NDSP_FORMAT_MONO_PCM16 :
			NDSP_FORMAT_STEREO_PCM16;
	}

	// Create and play a wav buffer
	std::memset(&waveBuf[0], 0, sizeof(ndspWaveBuf));

	//waveBuf.data_vaddr = reinterpret_cast<u32>(data);
	waveBuf[0].data_vaddr = (const void*)data;
	waveBuf[0].nsamples = dataSize / (bitsPerSample >> 3);
	waveBuf[0].looping = false; // Loop enabled
	waveBuf[0].status = NDSP_WBUF_FREE;

	ndspChnReset(channel[0]);
	ndspChnSetInterp(channel[0], NDSP_INTERP_NONE);
	ndspChnSetRate(channel[0], float(sampleRate[0]));
	ndspChnSetFormat(channel[0], ndspFormat);
	DSP_FlushDataCache(data, dataSize);

	//ndspChnWaveBufAdd(channel, &waveBuf);
	bool head;
	
	while (aptMainLoop()) {
		hidScanInput();
		if (hidKeysHeld() & KEY_START) break;
		hidTouchRead(&tp);//タッチ位置を読み取る
		unsigned int key = hidKeysDown();

		sf2d_start_frame(GFX_TOP, GFX_LEFT);//上画面描画開始

			sf2d_draw_texture(tex[0], 400/2 - tex[0]->width/2, 240/2 - tex[0]->height/2);
			DSP_GetHeadphoneStatus(&head);
			sftd_draw_textf(font, 20, 0, RGBA8(0, 255, 0, 255), 20, "FPS %f", sf2d_get_fps());
			sftd_draw_textf(font,0,0,RGBA8(0,255,0,255),20,"%d",head);
		sf2d_end_frame();//上画面終了


		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);//下画面描画開始

			sf2d_draw_texture(tex[1], 320/2 - tex[1]->width/2, 240/2 - tex[1]->height/2);
			//sf2d_draw_fill_circle(160,145,105,RGBA8(0xFF, 0x00, 0xFF, 0xFF));

			if (((tp.px-160)*(tp.px-160) + (tp.py-145)*(tp.py-145)) <= 105*105 && key & KEY_TOUCH){//ドン
				sftd_draw_text(font, 10, 10,  RGBA8(255, 0, 0,   255), 20, "Don!");
				ndspChnReset(channel[0]);
				ndspChnSetInterp(channel[0], NDSP_INTERP_NONE);
				ndspChnSetRate(channel[0], float(sampleRate[0]));
				ndspChnSetFormat(channel[0], ndspFormat);
				ndspChnWaveBufAdd(channel[0], &waveBuf[0]);
			}else if (key & KEY_TOUCH){//カ
				sftd_draw_text(font, 10, 10,  RGBA8(0, 0, 255,   255), 20, "Ka!");
			}
		sf2d_end_frame();//下画面終了

		sf2d_swapbuffers();
	}

	sf2d_free_texture(tex[0]);
	sf2d_free_texture(tex[1]);
	//ndspChnWaveBufClear(channel[0]);
	//linearFree(data);

	romfsExit();
	sf2d_fini();
	sftd_fini();
	//fclose(fp);
	//ov_clear(&ovf);
	ndspExit();
	dspExit();
	return 0;
}




int tjafile_load_header(){	//tjaファイルのヘッダー情報の読み取り
	//gfxInitDefault();
    //consoleInit(GFX_TOP, NULL);
	FILE *fp; 
	char buf[500];
	char *tja[100];
	char *tja_title,*tja_subtitle,*tja_level,*tja_bpm,*tja_wave,*tja_offset,*tja_balloon,*tja_songvol,*tja_sevol,*tja_scoreinit,*tja_scorediff,*tja_course,*tja_style,*tja_game,*tja_life,*tja_demostart,*tja_side,*tja_scoremode;
	if ((fp = fopen("tjafiles/syaruru.tja", "r")) != NULL) {
		while (fgets(buf, 512, fp) != NULL) {
			if (strstr(buf, "#START") != NULL) {
				break;
			}
			if ((strstr(buf, "TITLE:") != NULL) && (strstr(buf, "SUBTITLE:") == NULL)) {
				tja_title = (char *)malloc((strlen(buf)+1));
				strcpy(tja_title, buf + 6);
				continue;
			}
			if ((strstr(buf, "SUBTITLE:") != NULL)) {
				tja_subtitle = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_subtitle, buf + 9);
				continue;
			}
			if ((strstr(buf, "LEVEL:") != NULL)) {
				tja_level = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_level, buf + 6);
				continue;
			}
			if ((strstr(buf, "BPM:") != NULL)) {
				tja_bpm = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_bpm, buf + 4);
				continue;
			}
			if ((strstr(buf, "WAVE:") != NULL)) {
				tja_wave = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_wave, buf + 5);
				continue;
			}
			if ((strstr(buf, "OFFSET:") != NULL)) {
				tja_offset = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_offset, buf + 7);
				continue;
			}
			if ((strstr(buf, "BALLOON:") != NULL)) {
				tja_balloon = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_balloon, buf + 8);
				continue;
			}
			if ((strstr(buf, "SONGVOL:") != NULL)) {
				tja_songvol = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_songvol, buf + 8);
				continue;
			}
			if ((strstr(buf, "SEVOL:") != NULL)) {
				tja_sevol = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_sevol, buf + 6);
				continue;
			}
			if ((strstr(buf, "SCOREINIT:") != NULL)) {
				tja_scoreinit = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_scoreinit, buf + 10);
				continue;
			}
			if ((strstr(buf, "SCOREDIFF:") != NULL)) {
				tja_scorediff = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_scorediff, buf + 10);
				continue;
			}
			if ((strstr(buf, "COURSE:") != NULL)) {
				tja_course = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_course, buf + 7);
				continue;
			}
			if ((strstr(buf, "STYLE:") != NULL)) {
				tja_style = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_style, buf + 6);
				continue;
			}
			if ((strstr(buf, "GAME:") != NULL)) {
				tja_game = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_game, buf + 5);
				continue;
			}
			if ((strstr(buf, "LIFE:") != NULL)) {
				tja_life = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_life, buf + 5);
				continue;
			}
			if ((strstr(buf, "DEMOSTART:") != NULL)) {
				tja_demostart = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_demostart, buf + 10);
				continue;
			}
			if ((strstr(buf, "SIDE:") != NULL)) {
				tja_side = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_side, buf + 5);
				continue;
			}
			if ((strstr(buf, "SCOREMODE:") != NULL)) {
				tja_scoremode = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_scoremode, buf + 10);
				continue;
			}
		}
	}
	/*printf("TITLE...%s", tja_title);
	printf("SUBTITLE...%s", tja_subtitle);
	printf("LEVEL...%s", tja_level);
	printf("BPM...%s", tja_bpm);
	printf("WAVE...%s", tja_wave);
	printf("OFFSET...%s", tja_offset);
	printf("BALLOON...%s", tja_balloon);
	printf("SONGVOL...%s", tja_songvol);
	printf("SEVOL...%s", tja_sevol);
	printf("SCOREINIT...%s", tja_scoreinit);
	printf("SCOREDIFF...%s", tja_scorediff);
	printf("COURSE...%s", tja_course);
	printf("STYLE...%s\n", tja_style);
	printf("GAME...%s\n", tja_game);
	printf("LIFE...%s\n", tja_life);
	printf("DEMOSTART...%s", tja_demostart);
	printf("SIDE...%s\n", tja_side);
	printf("SCOREMODE...%s", tja_scoremode);*/

	
	/*while (aptMainLoop()) {//Mainloop
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    };
    gfxExit();*/
	return 0;
}