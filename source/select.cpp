#include "header.h"
#include "main.h"
#include "select.h"

LIST_T List[List_Max];
int SongNumber = 0;		//曲の総数

void load_file_list() {

	char buf[256];

	DIR *dir;
	struct dirent *dp;
	char path[] = "sdmc:/tjafiles/";

	dir = opendir(path);

	//FILE *fp;
	chdir("sdmc:/tjafiles/");

	int i = 0;
	for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {

		debug_draw(0, i * 10, dp->d_name);

		//strlcpy(List[i].title, dp->d_name, strlen(dp->d_name) );
		snprintf(List[i].title, sizeof(List[i].title), dp->d_name);

		//snprintf(buf, sizeof(buf), "%s" "/",dp->d_name);
		
		//if ((fp = fopen(buf, "r")) == NULL) debug_draw(300, i * 10, "Failed");
		//else debug_draw(300, i * 10, "Success");

		//fclose(fp);
		i++;
	}
	SongNumber = i;
}

void disp_file_list() {
	
	debug_draw(0, 0, "You can't select a song.      Press Select to start.");

	for (int i = 0; i < SongNumber; i++) {

		debug_draw(0, (i+1) * 10, List[i].title);
	}
}