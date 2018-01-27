#include <3ds.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "playback.h"
#include "vorbis.h"

#define delete(ptr) \
	free((void*) ptr); ptr = NULL

static volatile bool stop = true;

bool togglePlayback(void){
	bool paused = ndspChnIsPaused(CHANNEL);
	ndspChnSetPaused(CHANNEL, !paused);
	return !paused;
}

void stopPlayback(void){
	stop = true;
}

bool isPlaying(void){
	return !stop;
}

int getFileType(const char *file){
	FILE* ftest = fopen(file, "rb");
	uint32_t fileSig;
	enum file_types file_type = FILE_TYPE_ERROR;

	/* Failure opening file */
	if(ftest == NULL)
		return -1;

	if(fread(&fileSig, 4, 1, ftest) == 0)
		goto err;

	switch(fileSig)
	{
		// "RIFF"
		case 0x46464952:
			if(fseek(ftest, 4, SEEK_CUR) != 0)
				break;

			// "WAVE"
			// Check required as AVI file format also uses "RIFF".
			if(fread(&fileSig, 4, 1, ftest) == 0)
				break;

			if(fileSig != 0x45564157)
				break;

			file_type = FILE_TYPE_WAV;
			break;

		// "fLaC"
		case 0x43614c66:
			file_type = FILE_TYPE_FLAC;
			break;

		// "OggS"
		case 0x5367674F:
			if(isVorbis(file) == 0)
				file_type = FILE_TYPE_VORBIS;

			break;

		default:

			if((fileSig << 16) == 0xFBFF0000 ||
					(fileSig << 16) == 0xFAFF0000 ||
					(fileSig << 8) == 0x33444900)
			{
				file_type = FILE_TYPE_MP3;
				break;
			}
			break;
	}

err:
	fclose(ftest);
	return file_type;
}

void playFile(void* infoIn){
	struct decoder_fn decoder;
	struct playbackInfo_t* info = (playbackInfo_t*)infoIn;
	int16_t*		buffer1 = NULL;
	int16_t*		buffer2 = NULL;
	ndspWaveBuf		waveBuf[2];
	bool			lastbuf = false;
	int				ret = -1;
	const char*		file = info->file;
	bool			isNdspInit = false;

	/* Reset previous stop command */
	stop = false;

	switch(getFileType(file))
	{
		case FILE_TYPE_VORBIS:
			setVorbis(&decoder);
			break;

		default:
			goto err;
	}

	if(ndspInit() < 0)
	{
		goto err;
	}

	isNdspInit = true;

	if((ret = (*decoder.init)(file)) != 0)
	{
		goto err;
	}

	if((*decoder.channels)() > 2 || (*decoder.channels)() < 1)
	{
		goto err;
	}
	 
	buffer1 = (int16_t*)linearAlloc(decoder.buffSize * sizeof(int16_t));
	buffer2 = (int16_t*)linearAlloc(decoder.buffSize * sizeof(int16_t));

	ndspChnReset(CHANNEL);
	ndspChnWaveBufClear(CHANNEL);
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspChnSetInterp(CHANNEL, NDSP_INTERP_POLYPHASE);
	ndspChnSetRate(CHANNEL, (*decoder.rate)());
	ndspChnSetFormat(CHANNEL,
			(*decoder.channels)() == 2 ? NDSP_FORMAT_STEREO_PCM16 :
			NDSP_FORMAT_MONO_PCM16);

	memset(waveBuf, 0, sizeof(waveBuf));

	while (*info->isPlay == false) svcSleepThread(100 * 1000);

	waveBuf[0].nsamples = (*decoder.decode)(&buffer1[0]) / (*decoder.channels)();
	waveBuf[0].data_vaddr = &buffer1[0];
	ndspChnWaveBufAdd(CHANNEL, &waveBuf[0]);

	waveBuf[1].nsamples = (*decoder.decode)(&buffer2[0]) / (*decoder.channels)();
	waveBuf[1].data_vaddr = &buffer2[0];
	ndspChnWaveBufAdd(CHANNEL, &waveBuf[1]);

	while(ndspChnIsPlaying(CHANNEL) == false);

	while(stop == false){
		svcSleepThread(100 * 1000);

		if(lastbuf == true && waveBuf[0].status == NDSP_WBUF_DONE &&
				waveBuf[1].status == NDSP_WBUF_DONE)
			break;

		if(ndspChnIsPaused(CHANNEL) == true || lastbuf == true)
			continue;

		if(waveBuf[0].status == NDSP_WBUF_DONE)
		{
			size_t read = (*decoder.decode)(&buffer1[0]);

			if(read <= 0)
			{
				lastbuf = true;
				continue;
			}
			else if(read < decoder.buffSize)
				waveBuf[0].nsamples = read / (*decoder.channels)();

			ndspChnWaveBufAdd(CHANNEL, &waveBuf[0]);
		}

		if(waveBuf[1].status == NDSP_WBUF_DONE)
		{
			size_t read = (*decoder.decode)(&buffer2[0]);

			if(read <= 0)
			{
				lastbuf = true;
				continue;
			}
			else if(read < decoder.buffSize)
				waveBuf[1].nsamples = read / (*decoder.channels)();

			ndspChnWaveBufAdd(CHANNEL, &waveBuf[1]);
		}

		DSP_FlushDataCache(buffer1, decoder.buffSize * sizeof(int16_t));
		DSP_FlushDataCache(buffer2, decoder.buffSize * sizeof(int16_t));
	}

	(*decoder.exit)();
out:
	if(isNdspInit == true)
	{
		ndspChnWaveBufClear(CHANNEL);
		ndspExit();
	}

	delete(info->file);
	linearFree(buffer1);
	linearFree(buffer2);

	threadExit(0);
	return;

err:
	goto out;
}

#define DEFAULT_DIR		"sdmc:/"
struct playbackInfo_t playbackInfo;

int changeFile(const char* ep_file, struct playbackInfo_t* playbackInfo, bool *p_isPlayMain){
	s32 prio;
	static Thread thread = NULL;

	if (ep_file != NULL && getFileType(ep_file) == FILE_TYPE_ERROR) return -1;
	if (thread != NULL) {
		stopPlayback();

		threadJoin(thread, U64_MAX);
		threadFree(thread);
		thread = NULL;
	}

	if (ep_file == NULL || playbackInfo == NULL)
		return 0;

	playbackInfo->file = strdup(ep_file);
	playbackInfo->isPlay = p_isPlayMain;

	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	thread = threadCreate(playFile, playbackInfo, 32 * 1024, prio - 1, -2, false);

	return 0;
}

void play_main_music(bool *p_isPlayMain) {
	changeFile("tjafiles/yawaraka.ogg", &playbackInfo, p_isPlayMain);
}

void pasue_main_music() {
	if (isPlaying() == true) {
		togglePlayback();
	}
}

void stop_main_music() {
	stopPlayback();
	changeFile(NULL, &playbackInfo ,NULL);
}

void init_main_music() {
	playbackInfo.file = NULL;
	//chdir(DEFAULT_DIR);
}
