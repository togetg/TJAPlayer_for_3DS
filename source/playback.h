#pragma once

#include "select.h"

#define CHANNEL 5

enum file_types{
	FILE_TYPE_ERROR = -1,
	FILE_TYPE_WAV,
	FILE_TYPE_FLAC,
	FILE_TYPE_VORBIS,
	FILE_TYPE_OPUS,
	FILE_TYPE_MP3
};

struct decoder_fn{
	int (* init)(const char* file);
	uint32_t (* rate)(void);
	uint8_t (* channels)(void);
	size_t buffSize;
	uint64_t (* decode)(void*);
	void (* exit)(void);
};

struct playbackInfo_t{
	char*		file;
	bool*		isPlay;
};

bool togglePlayback(void);
void stopPlayback(void);
bool isPlaying(void);
int getFileType(const char *file);
void playFile(void* infoIn);

int changeFile(const char* ep_file, struct playbackInfo_t* playbackInfo,bool *p_isPlayMain);
void play_main_music(bool *p_isPlayMain, LIST_T Song);
void pasue_main_music();
void stop_main_music();
void init_main_music();
