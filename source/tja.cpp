#include "header.h"
#include "notes.h"
#include "tja.h"

char *tja_title, *tja_subtitle, *tja_level, *tja_bpm, *tja_wave, *tja_offset, *tja_balloon, *tja_songvol, *tja_sevol, *tja_scoreinit, *tja_scorediff, *tja_course, *tja_style, *tja_game, *tja_life, *tja_demostart, *tja_side, *tja_scoremode;
double bpm = -1;
char tja_notes[2048][Max_Notes_Section];
int tja_cnt = 0;
char buf_tja[160];

void tja_head_load(double *p_offset, double *p_bpm,int *p_measure){
	
	FILE *fp; 
	char buf[128];

	char default_title[9] = "No title", 
		 default_subtitle[1] = "",
		 default_level[2] = "0",
		 default_bpm[3] = "60", 
		 default_wave[9] = "test.wav", 
		 default_offset[2] = "0", 
		 default_balloon[2] = "5", 
		 default_songvol[4] = "100", 
		 default_sevol[4] = "100", 
		 default_scoreinit[3] = "-1", 
		 default_scorediff[3] = "-1",
		 default_course[4] = "oni",
		 default_style[2] = "1", 
		 default_game[6] = "taiko", 
		 default_life[3] = "-1", 
		 default_demostart[2] = "0", 
		 default_side[2] = "3",
		 default_scoremode[2]="1";
	
	if ((fp = fopen("sdmc:/tjafiles/" File_Name "/" File_Name ".tja", "r")) != NULL) {

		tja_title = &(default_title[0]);
		tja_subtitle = &(default_subtitle[0]);
		tja_level = &(default_level[0]);
		tja_bpm = &(default_bpm[0]);
		tja_wave = &(default_wave[0]);
		tja_offset = &(default_offset[0]);
		tja_balloon = &(default_balloon[0]);
		tja_songvol = &(default_songvol[0]);
		tja_sevol = &(default_sevol[0]);
		tja_scoreinit = &(default_scoreinit[0]);
		tja_scorediff = &(default_scorediff[0]);
		tja_course = &(default_course[0]);
		tja_style = &(default_style[0]);
		tja_game = &(default_game[0]);
		tja_life = &(default_life[0]);
		tja_demostart = &(default_demostart[0]);
		tja_side = &(default_side[0]);
		tja_scoremode = &(default_scoremode[0]);

		while (fgets(buf, 128, fp) != NULL) {

			if (strstr(buf, "#START") != NULL) {
				break;
			}

			if ((strstr(buf, "TITLE:") != NULL) && (strstr(buf, "SUBTITLE:") == NULL)) {
				tja_title = (char *)malloc((strlen(buf)+1));
				strcpy(tja_title, buf + 6);
				continue;
			}

			if ((strstr(buf, "SUBTITLE:") != NULL)) {
				tja_subtitle = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_subtitle, buf + 9);
				continue;
			}

			if ((strstr(buf, "LEVEL:") != NULL)) {
				tja_level = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_level, buf + 6);
				continue;
			}
			
			if ((strstr(buf, "BPM:") != NULL)) {
				tja_bpm = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_bpm, buf + 4);
				continue;
			}

			if ((strstr(buf, "WAVE:") != NULL)) {
				tja_wave = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_wave, buf + 5);
				continue;
			}

			if ((strstr(buf, "OFFSET:") != NULL)) {
				tja_offset = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_offset, buf + 7);
				continue;
			}

			if ((strstr(buf, "BALLOON:") != NULL)) {
				tja_balloon = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_balloon, buf + 8);
				continue;
			}

			if ((strstr(buf, "SONGVOL:") != NULL)) {
				tja_songvol = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_songvol, buf + 8);
				continue;
			}

			if ((strstr(buf, "SEVOL:") != NULL)) {
				tja_sevol = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_sevol, buf + 6);
				continue;
			}

			if ((strstr(buf, "SCOREINIT:") != NULL)) {
				tja_scoreinit = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_scoreinit, buf + 10);
				continue;
			}

			if ((strstr(buf, "SCOREDIFF:") != NULL)) {
				tja_scorediff = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_scorediff, buf + 10);
				continue;}


			if ((strstr(buf, "COURSE:") != NULL)) {
				tja_course = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_course, buf + 7);
				continue;}

			if ((strstr(buf, "STYLE:") != NULL)) {
				tja_style = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_style, buf + 6);
				continue;
			}

			if ((strstr(buf, "GAME:") != NULL)) {
				tja_game = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_game, buf + 5);
				continue;
			}

			if ((strstr(buf, "LIFE:") != NULL)) {
				tja_life = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_life, buf + 5);
				continue;
			}

			if ((strstr(buf, "DEMOSTART:") != NULL)) {
				tja_demostart = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_demostart, buf + 10);
				continue;
			}

			if ((strstr(buf, "SIDE:") != NULL)) {
				tja_side = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_side, buf + 5);
				continue;
			}

			if ((strstr(buf, "SCOREMODE:") != NULL)) {
				tja_scoremode = (char *)malloc((strlen(buf) + 1));
				strcpy(tja_scoremode, buf + 10);
				continue;
			}

		}

		fclose(fp);
		*p_offset = atof(tja_offset);
		*p_bpm = atof(tja_bpm);
		

	}else {
		//tjaファイルが開けなかった時
	}
}

void tja_draw(int cnt) {

	int cnt3 = 0;

	for (int i = 0; i < tja_cnt; i++) { 

		cnt3 = 0;

		while (tja_notes[cnt/60][cnt3] !='\0') {

			//sftd_draw_textf(font, 0, 0, RGBA8(255, 0, 0, 255), 10, "%c", tja_notes[cnt/60][cnt3]);
			cnt3++;

		}
	}
}

void tja_notes_load() {

	bool start_flag = false;
	FILE *fp;

	if ((fp = fopen("sdmc:/tjafiles/" File_Name  "/" File_Name ".tja", "r")) != NULL) {

		while (fgets(tja_notes[tja_cnt], Max_Notes_Section, fp) != NULL) {

			if (strstr(tja_notes[tja_cnt], "#START") != NULL) {

				start_flag = true;
				continue;

			}
			if (start_flag == true) {

				if (strstr(tja_notes[tja_cnt], "#END") != NULL) {

					break;

				}

				tja_cnt++;
			}
		}

		fclose(fp);
	}
}
void tja_to_notes(bool isDnon,bool isKa,int count, C2D_Sprite sprites[12]) {
	
	notes_main(isDnon, isKa, tja_notes,count,tja_title,tja_subtitle,tja_level,tja_bpm,tja_wave,tja_offset,tja_balloon,tja_songvol,tja_sevol,tja_scoreinit,tja_scorediff,tja_course,tja_style,tja_game,tja_life,tja_demostart,tja_side,tja_scoremode,sprites);

}


char* str_concat(char *str1, const char *str2) {

	char *top = str1;

	while (*(str1++) != '\0');

	str1 -= 1;

	do {
		*(str1++) = *str2;
	} while (*(str2++) != '\0');

	return str1;
}

char* str_concat2(const char *str1, const char *str2, char **result) {

	size_t size = sizeof(char) * (strlen(str1) + strlen(str2) + 1);
	char *work = (char*)malloc(size);
	if (work == NULL) {
		printf("Cannot allocate memory.\n");
		return NULL;
	}

	char *top = work;

	strcpy(work, str1);

	work += strlen(str1);

	strcpy(work, str2);

	*result = top;

	return top;
}

void str_replace(const char *src, const char *target, const char *replace, char **result) {

	char *temp = (char*)malloc(sizeof(char) * 1000);
	if (temp == NULL) {
		printf("Cannot allocate memory.\n");
		return;
	}

	char *top = temp;

	char *work = (char*)malloc(sizeof(char) * strlen(src));
	strcpy(work, src);

	char *p;
	while ((p = strstr(work, target)) != NULL) {

		*p = '\0';
		p += strlen(target);

		strcpy(temp, p);

		str_concat(work, replace);
		str_concat(work, temp);
	}

	free(temp);

	*result = work;
}

void get_command_value(char* buf,COMMAND_T *Command) {	//コマンドと値を取り出す

	int comment, space, length;
	double val;

	if (buf[0] == '#') {

		length = strlen(buf);
		comment = length-3;

		char* command = (char *)malloc((strlen(buf) + 1));
		char* value = (char *)malloc((strlen(buf) + 1));

		if (strstr(buf, "//") != NULL) {	//コメント処理

			comment = strstr(buf, "//") - buf - 1;
			strncpy(command, buf + 1, comment);
		}

		if (strstr(buf, " ") != NULL) {		//値処理

			space = strstr(buf, " ") - buf;

			if (space < comment) {

				strncpy(command, buf + 1, space - 1);
				strncpy(value, buf + strlen(command) + 1, comment - strlen(command) );

			}else strncpy(command, buf + 1, comment);
		}
		else strncpy(command, buf + 1, comment);
		

		Command->command = command;
		Command->value = value;
		

		     if (strcmp(command, "START") == 0) Command->knd = Start;
		else if (strcmp(command, "END") == 0) Command->knd = End;
		else if (strcmp(command, "BPMCHANGE") == 0) {

				 Command->knd = Bpmchange;
				 Command->val = strtod(value, NULL);
			 }
		else if (strcmp(command, "GOGOSTART") == 0) Command->knd = Gogostart;
		else if (strcmp(command, "GOGOEND") == 0) Command->knd = Gogoend;
		else if (strcmp(command, "MEASURE") == 0) Command->knd = Measure;
		else if (strcmp(command, "SCROLL") == 0) Command->knd = Scroll;
		else if (strcmp(command, "DELAY") == 0) Command->knd = Delay;
		else if (strcmp(command, "SECTION") == 0) Command->knd = Section;
		else if (strcmp(command, "BRANCHSTART") == 0) Command->knd = Branchstart;
		else if (strcmp(command, "BRANCHEND") == 0) Command->knd = Branchend;
		else if (strcmp(command, "N") == 0) Command->knd = N;
		else if (strcmp(command, "E") == 0) Command->knd = E;
		else if (strcmp(command, "M") == 0) Command->knd = M;
		else if (strcmp(command, "LEVELHOLD") == 0) Command->knd = Levelhold;
		else if (strcmp(command, "BMSCROLl") == 0) Command->knd = Bmscroll;
		else if (strcmp(command, "HBSCROLL") == 0) Command->knd = Hbscroll;
		else if (strcmp(command, "BARLINEOFF") == 0) Command->knd = Barlineoff;
		else if (strcmp(command, "BARLINEON") == 0) Command->knd = Barlineon;
		else Command->knd = -1;

	}

	else {

		Command->knd = -1;
	}
}