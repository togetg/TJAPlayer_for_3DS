#pragma once

#include <tremor/ivorbisfile.h>
#include <ogg/ogg.h>
#include "playback.h"

void setVorbis(struct decoder_fn* decoder);
int initVorbis(const char* file);
uint32_t rateVorbis(void);
uint8_t channelVorbis(void);
uint64_t decodeVorbis(void* buffer);
void exitVorbis(void);
//int playVorbis(const char* in, bool *p_isPlayMain);
uint64_t fillVorbisBuffer(char* bufferOut);
int isVorbis(const char* in);
double getVorbisTime();