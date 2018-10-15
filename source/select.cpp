#include "header.h"
#include "main.h"
#include "select.h"
#include "tja.h"
#include "audio.h"
#include "notes.h"
#include <sys/types.h>
#include <sys/stat.h>

void load_file_list(const char *path);
void draw_select(float x, float y, const char *text);
void draw_option_text(float x, float y, const char *text, bool state);

LIST_T List[List_Max];
char buf_select[256];
int SongNumber = 0;		//曲の総数
int count = 0,cursor = 0,course_cursor = 0,course_count = 0,SelectedId = 0,course = ONI;
bool isSelectCourse = false,isGameStart = false;

void load_file_main() {

	load_file_list_tjafiles();
	cursor = 0;
	course_cursor = 0;
	course_count = 0;
	SelectedId = 0;
	course = ONI;
	isSelectCourse = false;
	isGameStart = false;
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

			draw_select(30, (n + cursor) * 20 + 60, List[i].title);

			if (i != (cursor*-1)) {
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[ONI]);
				draw_select(360, (n + cursor) * 20 + 60, buf_select);
			}
		}

		n++;

		if (i == (cursor*-1)) {

			SelectedId = i;
			int level;

			if (List[i].course[EDIT] == true) {

				if ((n + cursor - 1) == course_cursor) course = EDIT;
				level = List[i].level[EDIT];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				draw_select(80, (n + cursor) * 20 + 60, "おに");
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[EDIT]);
				draw_select(360, (n + cursor) * 20 + 60, buf_select);
				draw_select(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[ONI] == true) {

				if ((n + cursor - 1) == course_cursor) course = ONI;
				level = List[i].level[ONI];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				draw_select(80, (n + cursor) * 20 + 60, "おに");
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[ONI]);
				draw_select(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[HARD] == true) {

				if ((n + cursor - 1) == course_cursor) course = HARD;
				level = List[i].level[HARD];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				draw_select(80, (n + cursor) * 20 + 60, "むずかしい");
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[HARD]);
				draw_select(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[NORMAL] == true) {

				if ((n + cursor - 1) == course_cursor) course = NORMAL;
				level = List[i].level[NORMAL];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				draw_select(80, (n + cursor) * 20 + 60, "ふつう");
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[NORMAL]);
				draw_select(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[EASY] == true) {

				if ((n + cursor - 1) == course_cursor) course = EASY;
				level = List[i].level[EASY];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				draw_select(80, (n + cursor) * 20 + 60, "かんたん");
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[EASY]);
				draw_select(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (isSelectCourse == true) {

				draw_select(60, (course_cursor+1) * 20 + 60, ">>");
				//snprintf(buf_select, sizeof(buf_select), "%d",course_cursor);
				//draw_select(60, (course_cursor + 1) * 20 + 60, buf_select);
			}
		}
	}

	draw_select(10, 60, ">>");
	//snprintf(buf_select, sizeof(buf_select), "%d",course);
	//draw_select(0, 60, buf_select);

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
		music_play(1); music_play(1);
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

void draw_select(float x, float y, const char *text) {

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);
	C2D_DrawText(&SelectText, C2D_WithColor, x, y, 0.5f, 0.5f, 0.5f, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
}

void draw_option_text(float x, float y, const char *text,bool state) {

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);
	float size = 1.3;
	if (state == false) {
		C2D_DrawText(&SelectText, C2D_WithColor, x, y, 1.0f, size, size, C2D_Color32f(100.0 / 255.0, 100.0 / 255.0, 100.0 / 255.0, 1.0f));
	}
	else if (state == true) {
		C2D_DrawText(&SelectText, C2D_WithColor, x, y, 1.0f, size, size, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

void draw_option(u16 px, u16 py, unsigned int key) {

	if ((102 < py && 132 > py && 125 < px && 205 > px) && key & KEY_TOUCH) toggle_auto();
	//C2D_DrawRectSolid(125, 102, 0, 80, 30, C2D_Color32f(1, 34.0 / 255.0, 204.0 / 255.0, 1));
	draw_option_text(130, 100, "Auto", get_isAuto());
}

void get_SelectedId(LIST_T *TMP,int *arg) {

	for (int i = 0; i < 5; i++) {
		TMP->course[i] = List[SelectedId].course[i];
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