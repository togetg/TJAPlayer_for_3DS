#include <tremor/ivorbisfile.h>
#include <ogg/ogg.h>

#include "header.h"

#define MUSIC_CHANNEL 1
#define BUFFER_SIZE 4096
#define STACKSIZE (4 * 1024)
#define MUSIC_NUMBER 2

typedef struct {
	float rate;
	u32 channels;
	u32 encoding;
	u32 nsamples;
	u32 size;
	char* data;
	bool loop;
	int audiochannel;
	float mix[12];
	ndspInterpType interp;
	OggVorbis_File ovf;
} Music;
Music music[3];
ndspWaveBuf waveBuf[3];

void music_load() {

	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspSetOutputCount(1);
	char sound_address[MUSIC_NUMBER][30] = {
		"romfs:/don.ogg",
		"romfs:/ka.ogg"
	};

	for (int i = 0; i < MUSIC_NUMBER; i++) {
		memset(&music[i], 0, sizeof(music[i]));
		music[i].mix[0] = 1.0f;
		music[i].mix[1] = 1.0f;
		FILE * file = fopen(sound_address[i], "rb");
		if (file == 0) {
			printf("no file\n");
			while (1);
		}
		if (ov_open(file, &music[i].ovf, NULL, 0) < 0) {
			printf("ogg vorbis file error\n");
			while (1);
		}
		vorbis_info * vorbisInfo = ov_info(&music[i].ovf, -1);
		if (vorbisInfo == NULL) {
			printf("could not retrieve ogg audio stream information\n");
			while (1);
		}
		music[i].rate = (float)vorbisInfo->rate;
		music[i].channels = (u32)vorbisInfo->channels;
		music[i].encoding = NDSP_ENCODING_PCM16;
		music[i].nsamples = (u32)ov_pcm_total(&music[i].ovf, -1);
		music[i].size = music[i].nsamples * music[i].channels * 2;
		music[i].audiochannel = i;
		music[i].interp = NDSP_INTERP_NONE;
		music[i].loop = false;
		if (linearSpaceFree() < music[i].size) {
			printf("not enough linear memory available %ld\n", music[i].size);
		}
		music[i].data = (char*)linearAlloc(music[i].size);
		if (music[i].data == 0) {
			printf("null\n");
			while (1);
		}
		printf("rate:%f\n", music[i].rate);
		printf("channels:%ld\n", music[i].channels);
		printf("encoding:%ld\n", music[i].encoding);
		printf("nsamples:%ld\n", music[i].nsamples);
		printf("size:%ld\n", music[i].size);
		printf("Now Loading...");
		int offset = 0;
		int eof = 0;
		int currentSection;
		while (!eof) {
			long ret = ov_read(&music[i].ovf, &music[i].data[offset], BUFFER_SIZE, &currentSection);
			if (ret == 0) {
				eof = 1;
			}
			else if (ret < 0) {
				ov_clear(&music[i].ovf);
				linearFree(music[i].data);
				printf("error in the ogg vorbis stream\n");
				while (1);
			}
			else {
				offset += ret;
			}
			//printf("%ld %d\n", ret, currentSection);
		}
		memset(&waveBuf[i], 0, sizeof(ndspWaveBuf));
		waveBuf[i].data_vaddr = music[i].data;
		waveBuf[i].nsamples = music[i].nsamples;
		waveBuf[i].looping = music[i].loop;
		waveBuf[i].status = NDSP_WBUF_FREE;
		DSP_FlushDataCache(music[i].data, music[i].size);
		//linearFree(&music[i].ovf);
		ov_clear(&music[i].ovf);
		fclose(file);
	}
}
int music_play(int id) {
	if (music[id].audiochannel == -1) {
		printf("No available audio channel\n");
		return -1;
	}
	ndspChnWaveBufClear(music[id].audiochannel);
	ndspChnReset(music[id].audiochannel);
	ndspChnInitParams(music[id].audiochannel);
	ndspChnSetMix(music[id].audiochannel, music[id].mix);
	ndspChnSetInterp(music[id].audiochannel, music[id].interp);
	ndspChnSetRate(music[id].audiochannel, music[id].rate);
	ndspChnSetFormat(music[id].audiochannel, NDSP_CHANNELS(music[id].channels) | NDSP_ENCODING(music[id].encoding));
	ndspChnWaveBufAdd(music[id].audiochannel, &waveBuf[id]);
	// while (1);
	return 0;
}

void music_exit() {
	ndspChnWaveBufClear(music[0].audiochannel);
	for (int i = 0; i < MUSIC_NUMBER; i++) {
		ndspChnWaveBufClear(music[i].audiochannel);
		linearFree(music[i].data);
	}
	ndspExit();
}

int music_SamplePos(int id) {
	return (int)ndspChnGetSamplePos(music[id].audiochannel);
}