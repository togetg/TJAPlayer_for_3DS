#include "header.h"
#include "result.h"
#include "select.h"
#include "option.h"
#include "tja.h"
#include "main.h"

RESULT_T result;

void draw_result() {

	TJA_HEADER_T Tja;
	get_tja_header(&Tja);
	get_result(&result);
	
	float size = 0.7,width,height;

	draw_result_text(50, 10,size, Tja.title);
	draw_result_text(50, 30, size*0.8,Tja.subtitle);

	int iniX=100,iniY = 100, x2 = 300,YSense = 20, count = 0;
	draw_result_text(iniX, iniY + YSense * count, size, Text[get_lang()][TEXT_SCORE]);
	snprintf(get_buffer(), BUFFER_SIZE, "%d", result.score);
	calc_result_text(get_buffer(), &width, &height);
	draw_result_text(x2 - width, iniY + YSense * count, size, get_buffer());
	count++;
	draw_result_text(iniX, iniY + YSense * count, size,Text[get_lang()][TEXT_PERFECT]);
	snprintf(get_buffer(), BUFFER_SIZE, "%d", result.perfect);
	calc_result_text(get_buffer(), &width, &height);
	draw_result_text(x2 - width, iniY + YSense * count, size, get_buffer());
	count++;
	draw_result_text(iniX, iniY + YSense * count, size,Text[get_lang()][TEXT_NICE]);
	snprintf(get_buffer(), BUFFER_SIZE, "%d", result.nice);
	calc_result_text(get_buffer(), &width, &height);
	draw_result_text(x2 - width, iniY + YSense * count, size, get_buffer());
	count++;
	draw_result_text(iniX, iniY + YSense * count, size,Text[get_lang()][TEXT_BAD]);
	snprintf(get_buffer(), BUFFER_SIZE, "%d", result.bad);
	calc_result_text(get_buffer(), &width, &height);
	draw_result_text(x2 - width, iniY + YSense * count, size, get_buffer());
	count++;
	draw_result_text(iniX, iniY + YSense * count, size,Text[get_lang()][TEXT_ROLLCOUNT]);
	snprintf(get_buffer(), BUFFER_SIZE, "%d", result.roll);
	calc_result_text(get_buffer(), &width, &height);
	draw_result_text(x2 - width, iniY + YSense * count, size, get_buffer());
	count++;
	draw_result_text(iniX, iniY + YSense * count, size,Text[get_lang()][TEXT_MAXCOMBO]);
	snprintf(get_buffer(), BUFFER_SIZE, "%d", result.combo);
	calc_result_text(get_buffer(), &width, &height);
	draw_result_text(x2 - width, iniY + YSense * count, size, get_buffer());
	count++;
	draw_result_text(3, TOP_HEIGHT - 15, 0.5, Text[get_lang()][TEXT_PRESSSTART]);
}