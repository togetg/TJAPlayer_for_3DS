#include "header.h"
#include "main.h"
#include "select.h"
#include "tja.h"
#include <sys/types.h>
#include <sys/stat.h>

void load_file_list(const char *path);
void select_draw(float x, float y, const char *text);

LIST_T List[List_Max];
char buf_select[256];
int SongNumber = 0;		//曲の総数
int count = 0,cursor = 0,course_cursor = 0,course_count = 0;
bool isSelectCourse = false;

void load_file_main() {

	load_file_list_tjafiles();
	cursor = 0;
	course_cursor = 0;
	course_count = 0;
	isSelectCourse = false;
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
				tja_head_load_simple(&List[count]);
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

			select_draw(30, (n + cursor) * 20 + 60, List[i].title);

			if (i != (cursor*-1)) {
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[ONI]);
				select_draw(360, (n + cursor) * 20 + 60, buf_select);
			}
		}

		n++;

		if (i == (cursor*-1)) {

			int level;

			if (List[i].course[EDIT] == true) {

				level = List[i].level[EDIT];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					select_draw(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					select_draw(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				select_draw(80, (n + cursor) * 20 + 60, "おに");
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[EDIT]);
				select_draw(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[ONI] == true) {

				level = List[i].level[ONI];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					select_draw(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					select_draw(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				select_draw(80, (n + cursor) * 20 + 60, "おに");
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[ONI]);
				select_draw(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[HARD] == true) {

				level = List[i].level[HARD];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					select_draw(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					select_draw(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				select_draw(80, (n + cursor) * 20 + 60, "むずかしい");
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[HARD]);
				select_draw(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[NORMAL] == true) {

				level = List[i].level[NORMAL];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					select_draw(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					select_draw(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				select_draw(80, (n + cursor) * 20 + 60, "ふつう");
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[NORMAL]);
				select_draw(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[EASY] == true) {

				level = List[i].level[EASY];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					select_draw(200 + j * 10, (n + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					select_draw(200 + (j + level) * 10, (n + cursor) * 20 + 60, "・");
				}
				select_draw(80, (n + cursor) * 20 + 60, "かんたん");
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[EASY]);
				select_draw(360, (n + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (isSelectCourse == true) {

				select_draw(60, (course_cursor+1) * 20 + 60, ">>");
			}
		}
	}

	select_draw(10, 60, ">>");
	//snprintf(buf_select, sizeof(buf_select), "%d:%d",cursor,SongNumber);
	//select_draw(0, 60, buf_select);
}

void cursor_update(int knd) {

	if (knd == KEY_UP) {
		if (isSelectCourse == false) cursor++;
		else if (course_cursor > 0) course_cursor--;
	}
	else if (knd == (int)KEY_DOWN) {
		if (isSelectCourse == false) cursor--;
		else if (course_cursor < (course_count-1)) course_cursor++;
	}
	else if (knd == KEY_RIGHT) {
		if (isSelectCourse == false) cursor -= 5;
	}
	else if (knd == KEY_LEFT) {
		if (isSelectCourse == false) cursor += 5;
	}
	else if (knd == KEY_A && course_count != 0) {
		isSelectCourse = true;
	}
	else if (knd == KEY_B) {
		isSelectCourse = false;
		course_cursor = 0;
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
						tja_head_load_simple(&List[count]);
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

void select_draw(float x, float y, const char *text) {

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);
	C2D_DrawText(&SelectText, C2D_WithColor, x, y, 0.5f, 0.5f, 0.5f, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
}