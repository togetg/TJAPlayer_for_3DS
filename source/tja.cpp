#include "header.h"
#include "notes.h"
#include "tja.h"


char tja_notes[2048][Max_Notes_Section];
int tja_cnt = 0;
char buf_tja[160];
TJA_HEADER_T Current_Header;

void tja_head_load(){
	
	FILE *fp; 
	char buf[128];

	char default_title[] = "No title", 
		 default_subtitle[] = "",
		 default_level[] = "0",
		 default_bpm[] = "60", 
		 default_wave[] = "test.wav", 
		 default_offset[] = "0", 
		 default_balloon[] = "5", 
		 default_songvol[] = "100", 
		 default_sevol[] = "100", 
		 default_scoreinit[] = "-1", 
		 default_scorediff[] = "-1",
		 default_course[] = "oni",
		 default_style[] = "1", 
		 default_life[] = "-1", 
		 default_demostart[] = "0", 
		 default_side[] = "3",
		 default_scoremode[] = "1";
	
	if ((fp = fopen("sdmc:/tjafiles/" File_Name "/" File_Name ".tja", "r")) != NULL) {
		
		Current_Header.title = default_title;
		Current_Header.subtitle = default_subtitle;
		Current_Header.level = atoi(default_level);
		Current_Header.bpm = atof(default_bpm);
		Current_Header.wave = default_wave;
		Current_Header.offset = atof(default_offset);
		Current_Header.balloon[0] = atoi(default_balloon);
		Current_Header.songvol = atoi(default_songvol);
		Current_Header.sevol = atoi(default_sevol);
		Current_Header.scoreinit = atoi(default_scoreinit);
		Current_Header.scorediff = atoi(default_scorediff);
		Current_Header.course = atoi(default_course);
		Current_Header.style = atoi(default_style);
		Current_Header.life = atoi(default_life);
		Current_Header.demostart = atof(default_demostart);
		Current_Header.side = atoi(default_side);
		Current_Header.scoremode = atoi(default_scoremode);

		char* temp;
		while (fgets(buf, 128, fp) != NULL) {

			if (strstr(buf, "#START") != NULL) {
				break;
			}

			if ((strstr(buf, "TITLE:") != NULL) && (strstr(buf, "SUBTITLE:") == NULL)) {
				temp = (char *)malloc((strlen(buf)+1));
				strcpy(temp, buf + 6);
				Current_Header.title = temp;
				continue;
			}

			if ((strstr(buf, "SUBTITLE:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 9);
				Current_Header.subtitle = temp;
				continue;
			}

			if ((strstr(buf, "LEVEL:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 6);
				Current_Header.level = atoi(temp);
				continue;
			}
			
			if ((strstr(buf, "BPM:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 4);
				Current_Header.bpm = atof(temp);
				continue;
			}

			if ((strstr(buf, "WAVE:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 5);
				Current_Header.wave = temp;
				continue;
			}

			if ((strstr(buf, "OFFSET:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 7);
				Current_Header.offset = atof(temp);
				continue;
			}

			if ((strstr(buf, "BALLOON:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 8);
				//Current_Header.balloon = temp;
				continue;
			}

			if ((strstr(buf, "SONGVOL:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 8);
				Current_Header.songvol = atoi(temp);
				continue;
			}

			if ((strstr(buf, "SEVOL:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 6);
				Current_Header.sevol = atoi(temp);
				continue;
			}

			if ((strstr(buf, "SCOREINIT:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 10);
				continue;
			}

			if ((strstr(buf, "SCOREDIFF:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 10);
				Current_Header.scorediff = atoi(temp);
				continue;}


			if ((strstr(buf, "COURSE:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 7);
				Current_Header.course = atoi(temp);
				continue;}

			if ((strstr(buf, "STYLE:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 6);
				Current_Header.style = atoi(temp);
				continue;
			}

			if ((strstr(buf, "LIFE:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 5);
				Current_Header.life = atoi(temp);
				continue;
			}

			if ((strstr(buf, "DEMOSTART:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 10);
				Current_Header.demostart = atof(temp);
				continue;
			}

			if ((strstr(buf, "SIDE:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 5);
				Current_Header.side = atoi(temp);
				continue;
			}

			if ((strstr(buf, "SCOREMODE:") != NULL)) {
				temp = (char *)malloc((strlen(buf) + 1));
				strcpy(temp, buf + 10);
				Current_Header.scoremode= atoi(temp);
				continue;
			}

		}

		fclose(fp);		

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

void get_head(TJA_HEADER_T *Tja_Header) {

	*Tja_Header = Current_Header;
}

void tja_to_notes(bool isDnon,bool isKa,int count, C2D_Sprite sprites[12]) {
	
	notes_main(isDnon, isKa, tja_notes,count,sprites);

}


char* str_concat(char *str1, const char *str2) {

	//char *top = str1;

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

	//char *top = temp;

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