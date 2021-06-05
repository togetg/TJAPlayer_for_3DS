#include "header.h"
#include "main.h"
#include "select.h"
#include "tja.h"
#include "audio.h"
#include "notes.h"
#include "option.h"
#include <sys/types.h>
#include <sys/stat.h>

void load_file_list(const char *path);
void draw_select_text(float x, float y, const char *text);
void draw_option_text(float x, float y, const char *text, bool state,float *width,float *height);

LIST_T List[List_Max];
char buf_select[256];
int SongNumber = 0;		//曲の総数
int count = 0,cursor = 0,course_cursor = 0,course_count = 0,SelectedId = 0,course = COURSE_ONI;
bool isSelectCourse = false,isGameStart = false;

void load_file_main() {

	load_file_list_tjafiles();
}

void load_file_list_tjafiles() {	//バグがおこるため再帰は使わない

	chdir(DEFAULT_DIR);

	DIR *dir;
	struct dirent *dp;

	dir = opendir(DEFAULT_DIR);

	for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {

		struct stat st;
		stat(dp->d_name, &st);
		if ((st.st_mode & S_IFMT) != S_IFDIR) {

			if (strstr(dp->d_name, ".tja") != NULL) {

				snprintf(List[count].tja, sizeof(List[count].tja), dp->d_name);
				getcwd(List[count].path, 256);
				load_tja_head_simple(&List[count]);
				count++;
			}
		}
		else {
			load_file_list(dp->d_name);
			chdir("../");
		}
	}
	closedir(dir);
	SongNumber = count;
}


void disp_file_list() {

	int n = 0;
	course_count = 0;

	if (cursor > 0) cursor = -1*(SongNumber - 1);
	if (cursor < -1 * (SongNumber - 1)) cursor = 0;

	for (int i = 0; i < SongNumber; i++) {

		if ((n + cursor) * 20 + 60 >= 0 && (n + cursor) * 20 + 60 <= 220) {

			draw_select_text(30, (n + cursor) * 20 + 60, List[i].title);

			if (i != (cursor*-1)) {
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[COURSE_ONI]);
				draw_select_text(360, (n + cursor) * 20 + 60, buf_select);
			}
		}

		n++;

		if (i == (cursor*-1)) {

			SelectedId = i;
			int level;

			if (List[i].course[COURSE_EDIT] == true) {

				if ((n + cursor - 1) == course_cursor) course = COURSE_EDIT;
				level = List[i].level[COURSE_EDIT];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select_text(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select_text(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				draw_select_text(80, (n + cursor) * 20 + 60, Text[get_lang()][TEXT_ONI]);
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[COURSE_EDIT]);
				draw_select_text(360, (n + cursor) * 20 + 60, buf_select);
				draw_select_text(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[COURSE_ONI] == true) {

				if ((n + cursor - 1) == course_cursor) course = COURSE_ONI;
				level = List[i].level[COURSE_ONI];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select_text(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select_text(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				draw_select_text(80, (n + cursor) * 20 + 60, Text[get_lang()][TEXT_ONI]);
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[COURSE_ONI]);
				draw_select_text(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[COURSE_HARD] == true) {

				if ((n + cursor - 1) == course_cursor) course = COURSE_HARD;
				level = List[i].level[COURSE_HARD];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select_text(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select_text(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				draw_select_text(80, (n + cursor) * 20 + 60, Text[get_lang()][TEXT_HARD]);
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[COURSE_HARD]);
				draw_select_text(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[COURSE_NORMAL] == true) {

				if ((n + cursor - 1) == course_cursor) course = COURSE_NORMAL;
				level = List[i].level[COURSE_NORMAL];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select_text(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select_text(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				draw_select_text(80, (n + cursor) * 20 + 60, Text[get_lang()][TEXT_NORMAL]);
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[COURSE_NORMAL]);
				draw_select_text(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[COURSE_EASY] == true) {

				if ((n + cursor - 1) == course_cursor) course = COURSE_EASY;
				level = List[i].level[COURSE_EASY];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select_text(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select_text(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				draw_select_text(80, (n + cursor) * 20 + 60, Text[get_lang()][TEXT_EASY]);
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[COURSE_EASY]);
				draw_select_text(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (isSelectCourse == true) {

				draw_select_text(60, (course_cursor+1) * 20 + 60, ">>");
				//snprintf(buf_select, sizeof(buf_select), "%d",course_cursor);
				//draw_select_text(60, (course_cursor + 1) * 20 + 60, buf_select);
			}
		}
	}

	draw_select_text(10, 60, ">>");
	//snprintf(buf_select, sizeof(buf_select), "%d",course);
	//draw_select_text(0, 60, buf_select);

}

void cursor_update(int knd) {

	if (knd == KEY_UP) {
		if (isSelectCourse == false) cursor++;
		else if (course_cursor > 0) course_cursor--;
		music_play(1);
	}
	else if (knd == (int)KEY_DOWN) {
		if (isSelectCourse == false) cursor--;
		else if (course_cursor < (course_count-1)) course_cursor++;
		music_play(1);
	}
	else if (knd == KEY_RIGHT) {
		if (isSelectCourse == false) cursor -= 5;
		music_play(1);
	}
	else if (knd == KEY_LEFT) {
		if (isSelectCourse == false) cursor += 5;
		music_play(1);
	}
	else if (knd == KEY_A && course_count != 0) {
		if (isSelectCourse == true) isGameStart = true;
		else isSelectCourse = true;
		music_play(0);
	}
	else if (knd == KEY_B) {
		isSelectCourse = false;
		course_cursor = 0;
		music_play(1);
	}

}

void load_file_list(const char *path) {

	DIR* dir;
	struct dirent *dp;

	if ((dir = opendir(path)) != NULL) {

		DIR* db;
		char filename[512];
		while ((dp = readdir(dir)) != NULL) {

			chdir(path);

			strlcpy(filename, path, strlen(path));
			strcat(filename, "/");
			strcat(filename, dp->d_name);

			db = opendir(filename);

			struct stat st;
			stat(dp->d_name, &st);

			if ((st.st_mode & S_IFMT) != S_IFDIR) {

				if (db == NULL) {

					if (strstr(dp->d_name, ".tja") != NULL) {

						strlcpy(List[count].tja, dp->d_name, strlen(dp->d_name) + 1);
						getcwd(List[count].path, 256);
						load_tja_head_simple(&List[count]);
						count++;
					}
				}
			}
			else {
				load_file_list(dp->d_name);
				chdir("../");
			}
			closedir(db);
		}
	}
	closedir(dir);
}

C2D_TextBuf g_SelectText = C2D_TextBufNew(4096);
C2D_Text SelectText;

void draw_select_text(float x, float y, const char *text) {

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);
	C2D_DrawText(&SelectText, C2D_WithColor, x, y, 1.0f, 0.5f, 0.5f, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
}

void draw_result_text(float x, float y,float size, const char *text) {

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);
	C2D_DrawText(&SelectText, C2D_WithColor, x, y, 0.5f, size, size, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
}

void calc_result_text(const char *text, float *width, float *height) {

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);
	float size = 0.7;
	C2D_TextGetDimensions(&SelectText, size, size, width, height);
}

void draw_option_text(float x, float y, const char *text,bool state, float *width, float *height, float sizex, float sizey) {	//size省略可(0.7)

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);

	if (state == false) {
		C2D_DrawText(&SelectText, C2D_WithColor, x, y, 1.0f, sizex, sizey, C2D_Color32f(100.0 / 255.0, 100.0 / 255.0, 100.0 / 255.0, 1.0f));
	}
	else if (state == true) {
		C2D_DrawText(&SelectText, C2D_WithColor, x, y, 1.0f, sizex, sizey, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
	}
	C2D_TextGetDimensions(&SelectText, sizex, sizey, width, height);
}

void draw_pause_text(float x, float y, const char *text, float *width, float *height) {

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);
	float size = 1.0;

	C2D_TextGetDimensions(&SelectText, size, size, width, height);
	C2D_DrawText(&SelectText, C2D_WithColor, BOTTOM_WIDTH/2-*width/2, y, 1.0f, size, size, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
}

int pause_window(u16 px, u16 py, unsigned int key) {

	int margin = 20,ReturnVal = -1,x,y;
	float width, height;

	C2D_DrawRectSolid(margin, margin, 0, BOTTOM_WIDTH-margin*2, BOTTOM_HEIGHT-margin*2, C2D_Color32f(0, 0, 0, 1));

	draw_pause_text(-1, margin + 30, Text[get_lang()][TEXT_CONTINUE], &width, &height);		//続ける
	x = BOTTOM_WIDTH / 2 - width / 2, y = margin + 30;
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) ReturnVal = 0;

	draw_pause_text(-1, margin + 80, Text[get_lang()][TEXT_STARTOVER], &width, &height);		//はじめから
	x = BOTTOM_WIDTH / 2 - width / 2, y = margin + 80;
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) ReturnVal = 1;

	draw_pause_text(-1, margin + 130, Text[get_lang()][TEXT_RETURNSELECT], &width, &height);	//曲選択に戻る
	x = BOTTOM_WIDTH / 2 - width / 2, y = margin + 130;
	if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) ReturnVal = 2;

	return ReturnVal;
}

void get_SelectedId(LIST_T *TMP,int *arg) {

	for (int i = 0; i < 5; i++) {
		TMP->course[i] = List[SelectedId].course[i];
		TMP->course_exist[i] = List[SelectedId].course_exist[i];
		TMP->level[i] = List[SelectedId].level[i];
	}
	strlcpy(TMP->tja, List[SelectedId].tja, strlen(List[SelectedId].tja) + 1);
	strlcpy(TMP->path, List[SelectedId].path, strlen(List[SelectedId].path) + 1);
	strlcpy(TMP->title, List[SelectedId].title, strlen(List[SelectedId].title) + 1);
	strlcpy(TMP->wave, List[SelectedId].wave, strlen(List[SelectedId].wave) + 1);
	*arg = course;
}

bool get_isGameStart() {
	return isGameStart;
}

void select_ini() {
	//cursor = 0;
	course_cursor = 0;
	course_count = 0;
	SelectedId = 0;
	course = COURSE_ONI;
	isSelectCourse = false;
	isGameStart = false;
}