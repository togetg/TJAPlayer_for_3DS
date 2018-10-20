#include "header.h"
#include "main.h"
#include <sys/time.h>
char buf_time[160];

enum msec_status {
	InIT,
	CuRRENT,
	DiFF,
	LaST_DiFF,
};

struct timeval myTime;
int cnt[5], msec[5][4], sec[5];
int isStop[5];
double PreTime[5],Time[5];

double time_now(int id) {

	if (isStop[id] != 1) {
		gettimeofday(&myTime, NULL);

		if (cnt[id] == 0) msec[id][InIT] = (int)myTime.tv_usec;

		msec[id][CuRRENT] = (int)myTime.tv_usec;
		msec[id][LaST_DiFF] = msec[id][DiFF];
		msec[id][DiFF] = msec[id][CuRRENT] - msec[id][InIT];

		if (msec[id][DiFF] < 0) msec[id][DiFF] = 1000000 - msec[id][DiFF] * -1;

		if (msec[id][LaST_DiFF] > msec[id][DiFF]) sec[id]++;

		cnt[id]++;
		//printf("%04d:%06d\n", sec, msec[id][DiFF]);
		Time[id] = sec[id] + msec[id][DiFF] / 1000000.0 + PreTime[id];
	}
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

#define Fps_Sample 60
double fps_time[2],fps_cnt,fps_sum,fps;	//要初期化
void draw_fps() {
	
	fps_time[0] = fps_time[1];
	fps_time[1] = time_now(2);

	fps_sum += (fps_time[1] - fps_time[0]);
	fps_cnt++;

	if (fps_cnt == Fps_Sample) {
		fps_cnt = 0;
		fps = Fps_Sample / fps_sum;
		fps_sum = 0;
	}
	snprintf(buf_time, sizeof(buf_time), "%.1ffps", fps);
	draw_debug(300, 0, buf_time);
}

void time_ini() {

	for (int i = 0; i < 5; i++) {

		for (int n = 0; n < 4; n++) {
			msec[i][n] = 0;

		}

		sec[i] = 0;
		cnt[i] = 0;
		isStop[i] = 0;
		PreTime[i] = 0;
		Time[i] = 0;
	}
	fps_time[0] = 0;
	fps_time[1] = 0;
	fps_cnt = 0;
	fps_sum = 0;
	fps = 0;
}