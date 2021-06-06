#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "main.h"
#include "vorbis.h"
#include "header.h"

static OggVorbis_File	vorbisFile;
static vorbis_info		*vi;
static FILE				*f;
size_t		vorbis_buffer_size = DEFAULT_BUFFER_SIZE;// 8 * 4096;

void setVorbis(struct decoder_fn* decoder){
	decoder->init = &initVorbis;
	decoder->rate = &rateVorbis;
	decoder->channels = &channelVorbis;
	decoder->vorbis_buffer_size = vorbis_buffer_size;
	decoder->decode = &decodeVorbis;
	decoder->exit = &exitVorbis;
}

int initVorbis(const char* file){
	int err = -1;

	if((f = fopen(file, "rb")) == NULL)
		goto out;

	if(ov_open(f, &vorbisFile, NULL, 0) < 0)
		goto out;

	if((vi = ov_info(&vorbisFile, -1)) == NULL)
		goto out;

	err = 0;

out:
	return err;
}


uint32_t rateVorbis(void){
	return vi->rate;
}

uint8_t channelVorbis(void)
{
	return vi->channels;
}

uint64_t decodeVorbis(void* buffer)
{
	return fillVorbisBuffer((char*)buffer);
}

void exitVorbis(void)
{
	ov_clear(&vorbisFile);
	fclose(f);
}

double vorbis_time = 0;

uint64_t fillVorbisBuffer(char* bufferOut)
{
	uint64_t samplesRead = 0;
	int samplesToRead = vorbis_buffer_size;

	while(samplesToRead > 0)
	{
		static int current_section;
		int samplesJustRead =
			ov_read(&vorbisFile, bufferOut,
					samplesToRead > 4096 ? 4096	: samplesToRead,
					&current_section);

		if(samplesJustRead < 0)
			return samplesJustRead;
		else if(samplesJustRead == 0){
			break;
		}

		samplesRead += samplesJustRead;
		samplesToRead -= samplesJustRead;
		bufferOut += samplesJustRead;
	}
	//vorbis_time = (double)ov_time_tell(&vorbisFile)/1000.0;
	return samplesRead / sizeof(int16_t);
}

int isVorbis(const char *in){
	FILE *ft = fopen(in, "r");
	OggVorbis_File testvf;
	int err;

	if(ft == NULL)
		return -1;

	err = ov_test(ft, &testvf, NULL, 0);

	ov_clear(&testvf);
	fclose(ft);
	return err;
}

double getVorbisTime() {

	if (get_isMusicStart() == true) return vorbis_time = (double)ov_time_tell(&vorbisFile) / 1000.0;	//再生前に呼び出すとクラッシュ
	else return -1000;
}

int get_buffer_size() {
	return (int)vorbis_buffer_size;
}

void put_buffer_size(int tmp) {
	vorbis_buffer_size = (size_t)tmp;
}