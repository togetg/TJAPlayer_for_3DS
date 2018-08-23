#include "header.h"
#include "notes.h"
#include "tja.h"


char tja_notes[Measure_Max][Max_Notes_Measure];
int tja_cnt = 0;

TJA_HEADER_T Current_Header;
MEASURE_T Measure[Measure_Max];

void get_command_value(char* buf, COMMAND_T *Command);

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

		char* temp = NULL;
		while (fgets(buf, 128, fp) != NULL) {

			temp = (char *)malloc((strlen(buf) + 1));

			if (strstr(buf, "#START") == buf) {
				break;
			}

			if (strstr(buf, "TITLE:") == buf) {
				strcpy(temp, buf + 6);
				Current_Header.title = temp;
				continue;
			}

			if (strstr(buf, "SUBTITLE:") == buf) {
				strcpy(temp, buf + 9);
				Current_Header.subtitle = temp;
				continue;
			}

			if (strstr(buf, "LEVEL:") == buf) {
				strcpy(temp, buf + 6);
				Current_Header.level = atoi(temp);
				continue;
			}
			
			if (strstr(buf, "BPM:") == buf) {
				strcpy(temp, buf + 4);
				Current_Header.bpm = atof(temp);
				continue;
			}

			if (strstr(buf, "WAVE:") == buf) {
				strcpy(temp, buf + 5);
				Current_Header.wave = temp;
				continue;
			}

			if (strstr(buf, "OFFSET:") == buf) {
				strcpy(temp, buf + 7);
				Current_Header.offset = atof(temp);
				continue;
			}

			if (strstr(buf, "BALLOON:") == buf) {
				strcpy(temp, buf + 8);
				//Current_Header.balloon = temp;
				continue;
			}

			if (strstr(buf, "SONGVOL:") == buf) {
				strcpy(temp, buf + 8);
				Current_Header.songvol = atoi(temp);
				continue;
			}

			if (strstr(buf, "SEVOL:") == buf) {
				strcpy(temp, buf + 6);
				Current_Header.sevol = atoi(temp);
				continue;
			}

			if (strstr(buf, "SCOREINIT:") == buf) {
				strcpy(temp, buf + 10);
				continue;
			}

			if (strstr(buf, "SCOREDIFF:") == buf) {
				strcpy(temp, buf + 10);
				Current_Header.scorediff = atoi(temp);
				continue;}


			if (strstr(buf, "COURSE:") == buf) {
				strcpy(temp, buf + 7);
				Current_Header.course = atoi(temp);
				continue;}

			if (strstr(buf, "STYLE:") == buf) {
				strcpy(temp, buf + 6);
				Current_Header.style = atoi(temp);
				continue;
			}

			if (strstr(buf, "LIFE:") == buf) {
				strcpy(temp, buf + 5);
				Current_Header.life = atoi(temp);
				continue;
			}

			if (strstr(buf, "DEMOSTART:") == buf) {
				strcpy(temp, buf + 10);
				Current_Header.demostart = atof(temp);
				continue;
			}

			if (strstr(buf, "SIDE:") == buf) {
				strcpy(temp, buf + 5);
				Current_Header.side = atoi(temp);
				continue;
			}

			if (strstr(buf, "SCOREMODE:") == buf) {
				strcpy(temp, buf + 10);
				Current_Header.scoremode= atoi(temp);
				continue;
			}

		}

		fclose(fp);		
		free(temp);

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

	bool isStart = false,isEnd = false;
	FILE *fp;
	COMMAND_T Command;

	double bpm = Current_Header.bpm,
		tempo = 4, 
		NextBpm = bpm;


	if ((fp = fopen("sdmc:/tjafiles/" File_Name  "/" File_Name ".tja", "r")) != NULL) {

		tja_cnt = 0;
		int measure_cnt = 0;
		double PreJudge=0,FirstMeasureTime = 0;


		FirstMeasureTime = (60.0 / bpm * tempo)*(Notes_Judge_Range / Notes_Area) - 60.0 / bpm * tempo;
		PreJudge = FirstMeasureTime;


		while (fgets(tja_notes[tja_cnt], Max_Notes_Measure, fp) != NULL ||
			tja_cnt < Measure_Max) {

			if (isStart == false && strstr(tja_notes[tja_cnt], "#START") == tja_notes[tja_cnt]) {

				isStart = true;
				continue;
			}

			if (isStart == true) {
				
				if (
					(strstr(tja_notes[tja_cnt], "//") == tja_notes[tja_cnt] || 
					strstr(tja_notes[tja_cnt], ",") == NULL ||
						(
							strstr(tja_notes[tja_cnt], ",") != NULL &&
							strstr(tja_notes[tja_cnt], "//") != NULL &&
							strstr(tja_notes[tja_cnt], ",") > strstr(tja_notes[tja_cnt], "//")
						)
					) &&
					tja_notes[tja_cnt][0] != '#'
					) {

					tja_cnt++;
					continue;
				}

				if (tja_notes[tja_cnt][0] == '#') {

					get_command_value(tja_notes[tja_cnt], &Command);
					switch (Command.knd) {
					case BPmchange:
						NextBpm = Command.val;
						break;
					case ENd:
						isEnd = true;
						break;
					default:
						break;
					}
				}
				
				Measure[measure_cnt].notes = tja_cnt;
				Measure[measure_cnt].bpm = NextBpm;
				Measure[measure_cnt].tempo = tempo;
				Measure[measure_cnt].judge_time = 60.0 / bpm * tempo + PreJudge;
				Measure[measure_cnt].create_time = Measure[measure_cnt].judge_time - (60.0 / Measure[measure_cnt].bpm * tempo)*(Notes_Judge_Range / Notes_Area);
				
				if (tja_notes[tja_cnt][0] != '#') {
					PreJudge = Measure[measure_cnt].judge_time;
					bpm = NextBpm;
				}

				if (isEnd == true) break;

				tja_cnt++;
				measure_cnt++;
			}
		}

		fclose(fp);
	}
}

void get_head(TJA_HEADER_T *Tja_Header) {

	*Tja_Header = Current_Header;
}

void tja_to_notes(bool isDnon,bool isKa,int count, C2D_Sprite sprites[12]) {
	
	notes_main(isDnon, isKa, tja_notes,Measure,count,sprites);

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

	bool isComment = false;
	int comment, space, length;

	if (buf[0] == '#') {

		length = strlen(buf);
		comment = length - 3;

		char* command = (char *)malloc((strlen(buf) + 1));
		char* value = (char *)malloc((strlen(buf) + 1));

		if (strstr(buf, "//") != NULL) {	//コメント処理

			comment = strstr(buf, "//") - buf - 1;
			strlcpy(command, buf + 1, comment);
			isComment = true;
		}

		if (strstr(buf, " ") != NULL) {		//値処理

			space = strstr(buf, " ") - buf;

			if (space < comment && isComment == true) {	//値&コメントあり

				strlcpy(command, buf + 1, space);
				strlcpy(value, buf + 1 + strlen(command), comment - strlen(command) + 1);

			}
			else {	//値ありコメントなし
				strlcpy(command, buf + 1, space);
				strlcpy(value, buf + 1 + strlen(command), length - strlen(command));
			}
		}
		else {	//値無し
			strlcpy(command, buf + 1, comment+1);
			strlcpy(value, "0", 1);
		}


		Command->command = command;
		Command->value = value;
		Command->val = 0;

		     if (strcmp(command, "START") == 0) Command->knd = STart;
		else if (strcmp(command, "END") == 0) Command->knd = ENd;
		else if (strcmp(command, "BPMCHANGE") == 0) {

				 Command->knd = BPmchange;
				 Command->val = strtod(value, NULL);
			 }
		else if (strcmp(command, "GOGOSTART") == 0) Command->knd = GOgostart;
		else if (strcmp(command, "GOGOEND") == 0) Command->knd = GOgoend;
		else if (strcmp(command, "MEASURE") == 0) Command->knd = MEasure;
		else if (strcmp(command, "SCROLL") == 0) Command->knd = SCroll;
		else if (strcmp(command, "DELAY") == 0) Command->knd = DElay;
		else if (strcmp(command, "SECTION") == 0) Command->knd = SEction;
		else if (strcmp(command, "BRANCHSTART") == 0) Command->knd = BRanchstart;
		else if (strcmp(command, "BRANCHEND") == 0) Command->knd = BRanchend;
		else if (strcmp(command, "N") == 0) Command->knd = N;
		else if (strcmp(command, "E") == 0) Command->knd = E;
		else if (strcmp(command, "M") == 0) Command->knd = M;
		else if (strcmp(command, "LEVELHOLD") == 0) Command->knd = LEvelhold;
		else if (strcmp(command, "BMSCROLl") == 0) Command->knd = BMscroll;
		else if (strcmp(command, "HBSCROLL") == 0) Command->knd = HBscroll;
		else if (strcmp(command, "BARLINEOFF") == 0) Command->knd = BArlineoff;
		else if (strcmp(command, "BARLINEON") == 0) Command->knd = BArlineon;
		else Command->knd = -1;

	}

	else {

		Command->knd = -1;
	}
}