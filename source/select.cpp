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
int count = 0,cursor = 0;

void load_file_main() {

	load_file_list_tjafiles();
	cursor = 0;
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
	SongNumber = count;
}


void disp_file_list() {
	
	//select_draw(100, 0, "Press Select to start.");

	for (int i = 0; i < SongNumber; i++) {

		select_draw(30, (i+cursor) * 20 + 60, List[i].title);

		snprintf(buf_select, sizeof(buf_select), "★x%d",List[i].level[ONI]);
		select_draw(360, (i+cursor) * 20 + 60, buf_select );
	}

	select_draw(10, 60, "◆");
}

void cursor_update(int knd) {

	if (knd == 0) {			//上
		cursor++;
	}
	else if (knd == 1) {	//下
		cursor--;
	}
	else if (knd == 2) {	//右
		cursor -= 5;
	}
	else if (knd == 3) {	//左
	cursor += 5;
	}
}

void load_file_list(const char *path) {

	DIR* d;
	struct dirent *dp;

	if ((d = opendir(path)) != NULL) {

		DIR* db;
		char filename[512];
		while ((dp = readdir(d)) != NULL) {

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
		}
	}
}

C2D_TextBuf g_SelectText = C2D_TextBufNew(4096);
C2D_Text SelectText;

void select_draw(float x, float y, const char *text) {

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);
	C2D_DrawText(&SelectText, C2D_WithColor, x, y, 0.5f, 0.5f, 0.5f, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
}