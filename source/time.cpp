#include "header.h"
#include "main.h"
#include "vorbis.h"
#include "tja.h"
#include "time.h"
#include <sys/time.h>

enum msec_status {
	MSEC_INIT,
	MSEC_CURRENT,
	MSEC_DIFF,
	MSEC_LAST_DIFF,
};
#define TIME_NUM 5

struct timeval myTime;
int cnt[TIME_NUM], msec[TIME_NUM][4], sec[TIME_NUM];
int isStop[TIME_NUM];
double PreTime[TIME_NUM],Time[TIME_NUM],CurrentTime[TIME_NUM],IniVorbisTime[TIME_NUM];

double get_current_time(int id) {
	
	if ((id == 0 || id == 1) && get_isMusicStart() == true) { //メインのカウントの時はVorbis基準の時間を返す 要曲終了時の処理
		//if (isStop[id] != 1) stop_time(id);
		if (CurrentTime[id] == 0 && Time[id] == 0 && IniVorbisTime[id]==0) IniVorbisTime[id] = getVorbisTime();
		CurrentTime[id] = Time[id] + getVorbisTime() - IniVorbisTime[id];
		//return CurrentTime[id]; 
		//snprintf(get_buffer(), BUFFER_SIZE, "vbt:%.1f", CurrentTime[id]);
		//draw_debug(100, id*10, get_buffer()); 
	}

	if (isStop[id] != 1) {
		gettimeofday(&myTime, NULL);

		if (cnt[id] == 0) msec[id][MSEC_INIT] = (int)myTime.tv_usec;

		msec[id][MSEC_CURRENT] = (int)myTime.tv_usec;
		msec[id][MSEC_LAST_DIFF] = msec[id][MSEC_DIFF];
		msec[id][MSEC_DIFF] = msec[id][MSEC_CURRENT] - msec[id][MSEC_INIT];

		if (msec[id][MSEC_DIFF] < 0) msec[id][MSEC_DIFF] = 1000000 - msec[id][MSEC_DIFF] * -1;

		if (msec[id][MSEC_LAST_DIFF] > msec[id][MSEC_DIFF]) sec[id]++;

		cnt[id]++;
		//printf("%04d:%06d\n", sec, msec[id][MSEC_DIFF]);
		Time[id] = sec[id] + msec[id][MSEC_DIFF] / 1000000.0 + PreTime[id];
	}
	//snprintf(get_buffer(), BUFFER_SIZE, "t:%.1f", Time[id]);
	//draw_debug(0, id*10, get_buffer());
	return Time[id];
}

void restart_time(int id) {
	isStop[id] = 0;
}

void stop_time(int id) {

	isStop[id] = 1;
	for (int n = 0; n < 4; n++) {
		msec[id][n] = 0;
	}

	PreTime[id] = Time[id] += 0.0178;
	sec[id] = 0;
	cnt[id] = 0;
}

void toggle_time(int id) {

	if (Time[id] != 0) {
		if (isStop[id] == 1) restart_time(id);
		else stop_time(id);
	}
}

int get_time_isStop(int id) {
	return isStop[id];
}

#define FPS_SAMPLE 20
double fps_time[2],fps_cnt,fps_sum,fps;	//要初期化
void draw_fps() {
	
	fps_time[0] = fps_time[1];
	fps_time[1] = get_current_time(TIME_FPS);

	fps_sum += (fps_time[1] - fps_time[0]);
	fps_cnt++;

	if (fps_cnt == FPS_SAMPLE) {
		fps_cnt = 0;
		fps = FPS_SAMPLE / fps_sum;
		fps_sum = 0;
	}
	snprintf(get_buffer(), BUFFER_SIZE, "%.1ffps", fps);
	draw_debug(300, 0, get_buffer());
}

double preVorbisTime = -1000,startVorbisTime = -1000;
double calc_vorbis_time(double CurrentTimeNotes) {

	double result,vorbisTime = getVorbisTime();
	if (vorbisTime == -1000) result = CurrentTimeNotes;	//曲開始前はそのまま返す
	if (preVorbisTime == -1000 && vorbisTime != -1000)	startVorbisTime = CurrentTimeNotes;//曲開始時間を保存(timenotes換算)
	if (vorbisTime != -1000) result = vorbisTime + startVorbisTime;

	preVorbisTime = vorbisTime;
	return result;
}

void time_ini() {

	for (int i = 0; i < TIME_NUM; i++) {

		for (int n = 0; n < 4; n++) {
			msec[i][n] = 0;

		}

		sec[i] = 0;
		cnt[i] = 0;
		isStop[i] = 0;
		PreTime[i] = 0;
		Time[i] = 0;
		CurrentTime[i] = 0;
		IniVorbisTime[i] = 0;
	}
	fps_time[0] = 0;
	fps_time[1] = 0;
	fps_cnt = 0;
	fps_sum = 0;
	fps = 0;
	preVorbisTime = -1000;
	startVorbisTime = -1000;
}