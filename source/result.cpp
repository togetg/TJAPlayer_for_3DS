#include "header.h"
#include "result.h"
#include "select.h"
#include "option.h"
#include "tja.h"

RESULT_T result;
char buf_result[160];

void draw_result() {

	TJA_HEADER_T Tja;
	get_tja_header(&Tja);
	get_result(&result);
	
	float size = 0.7,width,height;

	draw_result_text(50, 10,size, Tja.title);
	draw_result_text(50, 30, size*0.8,Tja.subtitle);

	int iniX=100,iniY = 100, x2 = 300,YSense = 20, count = 0;
	draw_result_text(iniX, iniY + YSense * count, size, Text[get_lang()][scorE]);
	snprintf(buf_result, sizeof(buf_result), "%d", result.score);
	calc_result_text(buf_result, &width, &height);
	draw_result_text(x2 - width, iniY + YSense * count, size, buf_result);
	count++;
	draw_result_text(iniX, iniY + YSense * count, size,Text[get_lang()][perfecT]);
	snprintf(buf_result, sizeof(buf_result), "%d", result.perfect);
	calc_result_text(buf_result, &width, &height);
	draw_result_text(x2 - width, iniY + YSense * count, size, buf_result);
	count++;
	draw_result_text(iniX, iniY + YSense * count, size,Text[get_lang()][nicE]);
	snprintf(buf_result, sizeof(buf_result), "%d", result.nice);
	calc_result_text(buf_result, &width, &height);
	draw_result_text(x2 - width, iniY + YSense * count, size, buf_result);
	count++;
	draw_result_text(iniX, iniY + YSense * count, size,Text[get_lang()][baD]);
	snprintf(buf_result, sizeof(buf_result), "%d", result.bad);
	calc_result_text(buf_result, &width, &height);
	draw_result_text(x2 - width, iniY + YSense * count, size, buf_result);
	count++;
	draw_result_text(iniX, iniY + YSense * count, size,Text[get_lang()][rollcounT]);
	snprintf(buf_result, sizeof(buf_result), "%d", result.roll);
	calc_result_text(buf_result, &width, &height);
	draw_result_text(x2 - width, iniY + YSense * count, size, buf_result);
	count++;
	draw_result_text(iniX, iniY + YSense * count, size,Text[get_lang()][maxcombO]);
	snprintf(buf_result, sizeof(buf_result), "%d", result.combo);
	calc_result_text(buf_result, &width, &height);
	draw_result_text(x2 - width, iniY + YSense * count, size, buf_result);
	count++;
	draw_result_text(3, TOP_HEIGHT - 15, 0.5, Text[get_lang()][pressstarT]);
}