#include "header.h"
#include "main.h"
#include "select.h"
#include <sys/types.h>
#include <sys/stat.h>

LIST_T List[List_Max];
char buf_select[256];
int SongNumber = 0;		//曲の総数
int count = 0;

void load_file_main() {

	//load_file_list((char*)DEFAULT_DIR);
	load_file_list_tjafiles();
}

/*
void load_file_list(char* path) {
	
	DIR* d;
	struct dirent *dp;

	if ((d = opendir(path)) != NULL) {

		DIR* db;
		char filename[512];
		while ((dp = readdir(d)) != NULL) {

			strlcpy(filename, path,strlen(path));
			strcat(filename, "/");
			strcat(filename, dp->d_name);

			db = opendir(filename);

			if (db == NULL) {

				if (strstr(dp->d_name, ".tja") != NULL) {

					strlcpy(List[count].tja, dp->d_name, strlen(dp->d_name)+1);
					strlcpy(List[count].path, path, strlen(path)+1);
					strcat(List[count].path, "/");
					count++;
				}
			}
			else {
				load_file_list(filename);
			}
		}
	}
	SongNumber = count;
	
}*/

void load_file_list(const char *path) {

	DIR* d;
	struct dirent *dp;

	if ((d = opendir(path)) != NULL) {

		DIR* db;
		char filename[512];
		while ((dp = readdir(d)) != NULL) {

			strlcpy(filename, path, strlen(path));
			strcat(filename, "/");
			strcat(filename, dp->d_name);

			db = opendir(filename);

			if (db == NULL) {

				if (strstr(dp->d_name, ".tja") != NULL) {

					strlcpy(List[count].tja, dp->d_name, strlen(dp->d_name) + 1);
					strlcpy(List[count].path, path, strlen(path) + 1);
					strcat(List[count].path, "/");
					count++;
				}
			}
			else {
				//load_file_list(filename);
			}
		}
	}
}

void load_file_list_tjafiles() {	//バグがおこるため再帰は使わない

	DIR *dir;
	struct dirent *dp;

	chdir(DEFAULT_DIR);
	dir = opendir(DEFAULT_DIR);

	for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {

		struct stat st;
		stat(dp->d_name, &st);
		if ((st.st_mode & S_IFMT) != S_IFDIR) {

			if (strstr(dp->d_name, ".tja") != NULL) {

				snprintf(List[count].tja, sizeof(List[count].title), dp->d_name);
				count++;
			}
		}
		else {
			load_file_list(dp->d_name);
		}
	}
	SongNumber = count;
}

void disp_file_list() {
	
	debug_draw(0, 0, "You can't select a song.      Press Select to start.");

	for (int i = 0; i < SongNumber; i++) {

		debug_draw(0, (i+2) * 10, List[i].tja);
		if (List[i].tmp == 5) debug_draw(200, (i + 2) * 10, "ディレクトリ");
		//snprintf(buf_select, sizeof(buf_select), "%d", SongNumber);
		//debug_draw(0, (i+2) * 10, buf_select);
	}
}