#include "header.h"
#include "notes.h"
#include "tja.h"


char tja_notes[Measure_Max][Max_Notes_Measure];
int tja_cnt = 0,MeasureMaxNumber=0;
double MainFirstMeasureTime;


TJA_HEADER_T Current_Header;
MEASURE_T Measure[Measure_Max];

void get_command_value(char* buf, COMMAND_T *Command);

void measure_structure_init() {

	for (int i = 0; i < Measure_Max; i++) {

		Measure[i].create_time = INT_MAX;
		Measure[i].judge_time = INT_MAX;
		Measure[i].pop_time = INT_MAX;
		Measure[i].bpm = 0;
		Measure[i].scroll = 0;
		Measure[i].notes = 0;
		Measure[i].flag = false;
		Measure[i].isDispBarLine = true;
	}
}

void tja_init() {
	measure_structure_init();
}

void tja_head_load(){
	
	FILE *fp; 
	char buf[128];

	char default_title[] = "No title", 
		 default_subtitle[] = "",
		 default_level[] = "0",
		 default_bpm[] = "60", 
		 default_wave[] = "test.ogg", 
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

void MeasureInsertionSort(MEASURE_T t[], int array_size) {

	for (int i = 1; i < array_size; i++) {

		MEASURE_T temp = t[i];
		if (t[i - 1].create_time > temp.create_time) {

			int j = i;
			do {
				t[j] = t[j - 1];
				--j;
			} while (j > 0 && t[j - 1].create_time > temp.create_time);
			t[j] = temp;
		}
	}
}

void tja_notes_load() {

	bool isStart = false,isEnd = false,isDispBarLine = true;
	FILE *fp;
	COMMAND_T Command;
	double bpm = Current_Header.bpm,
		NextBpm = bpm,
		measure = 1,
		scroll = 1,
		NextMeasure = 1,
		delay = 0;

	if ((fp = fopen("sdmc:/tjafiles/" File_Name  "/" File_Name ".tja", "r")) != NULL) {

		tja_cnt = 0;
		int MeasureCount = 0;
		double PreJudge=0,FirstMeasureTime = 0;


		//MainFirstMeasureTime = (60.0 / bpm * measure*4)*((Notes_Area - Notes_Judge) / Notes_Area);

		FirstMeasureTime = (60.0 / bpm * 4*measure)*(Notes_Judge_Range / Notes_Area) - 60.0 / bpm * 4*measure;
		PreJudge = FirstMeasureTime;


		while (
			(fgets(tja_notes[tja_cnt], Max_Notes_Measure, fp) != NULL ||
			tja_cnt < Measure_Max) &&
			isEnd == false) {

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
					case MEasure:
						NextMeasure = Command.val;
						break;
					case SCroll:
						scroll = Command.val;
						break;
					case DElay:
						delay = Command.val;
						break;
					case BArlineon:
						isDispBarLine = true;
						break;
					case BArlineoff:
						isDispBarLine = false;
						break;
					case ENd:
						isEnd = true;
						break;
					default:
						break;
					}
				}
				
				Measure[MeasureCount].notes = tja_cnt;
				Measure[MeasureCount].bpm = NextBpm;
				Measure[MeasureCount].measure = NextMeasure;
				Measure[MeasureCount].scroll = scroll;
				Measure[MeasureCount].judge_time = 60.0 / bpm * 4*measure + PreJudge + delay;
				Measure[MeasureCount].pop_time = Measure[MeasureCount].judge_time - (60.0 / Measure[MeasureCount].bpm * 4)*(Notes_Judge_Range / Notes_Area);
				Measure[MeasureCount].create_time = Measure[MeasureCount].judge_time - (60.0 / Measure[MeasureCount].bpm * 4)*(Notes_Judge_Range / (Notes_Area*scroll));
				Measure[MeasureCount].create_time_cmp = Measure[MeasureCount].create_time;
				if (MeasureCount != 0 &&
					Measure[MeasureCount].create_time_cmp == Measure[MeasureCount - 1].create_time_cmp) {
					Measure[MeasureCount].create_time_cmp += 0.0001;
				}
				Measure[MeasureCount].isDispBarLine = isDispBarLine;

				if (tja_notes[tja_cnt][0] != '#') {
					PreJudge = Measure[MeasureCount].judge_time;
					bpm = NextBpm;
					measure = NextMeasure;
					delay = 0;
				}

				if (isEnd == true) {
					break;
				}

				tja_cnt++;
				MeasureCount++;
			}
		}

		MeasureMaxNumber = tja_cnt;
		fclose(fp);
		MeasureInsertionSort(Measure,Measure_Max);
		MainFirstMeasureTime = Measure[0].judge_time - Measure[0].create_time;
	}
}

void get_head(TJA_HEADER_T *Tja_Header) {

	*Tja_Header = Current_Header;
}

void tja_to_notes(bool isDnon,bool isKa,int count, C2D_Sprite sprites[12]) {
	
	notes_main(isDnon, isKa, tja_notes,Measure,count,sprites);

}


//コマンドと値を取り出す
void get_command_value(char* buf, COMMAND_T *Command) {

	bool isComment = false;
	int comment, space, length;

	if (buf[0] == '#') {

		length = strlen(buf);
		comment = 0;

		char* command = (char *)malloc((strlen(buf) + 1));
		char* value = (char *)malloc((strlen(buf) + 1));

		Command->notes = buf;

		if (strstr(buf, "//") != NULL) {	//コメント処理

			comment = strstr(buf, "//") - buf - 1;
			strlcpy(command, buf + 1, comment);
			isComment = true;
		}

		if (strstr(buf, " ") != NULL) {		//値処理

			space = strstr(buf, " ") - buf;

			if (space < comment && isComment == true) {	//値ありコメントあり

				strlcpy(command, buf + 1, space);
				strlcpy(value, buf + 1 + strlen(command), comment - strlen(command) + 1);

			}
			else {	//値ありコメントなし
				strlcpy(command, buf + 1, space);
				strlcpy(value, buf + 1 + strlen(command), length - strlen(command));
			}
		}
		else {	//値なし

			//コメントあり
			if (isComment == true) strlcpy(command, buf + 1, comment + 1);
			//コメントなし 改行あり
			else if (strstr(buf, "\n") != NULL) strlcpy(command, buf + 1, length - 2);
			//コメントなし　改行なし
			else strlcpy(command, buf + 1, length);

			strlcpy(value, "0", 1);
		}


		Command->command_s = command;
		Command->value_s = value;
		Command->val = 0;

		if (strcmp(command, "START") == 0) Command->knd = STart;
		else if (strcmp(command, "END") == 0) Command->knd = ENd;
		else if (strcmp(command, "BPMCHANGE") == 0) {
			Command->knd = BPmchange;
			Command->val = strtod(value, NULL);
		}
		else if (strcmp(command, "GOGOSTART") == 0) Command->knd = GOgostart;
		else if (strcmp(command, "GOGOEND") == 0) Command->knd = GOgoend;

		else if (strcmp(command, "MEASURE") == 0) {
			Command->knd = MEasure;
			if (strstr(value, "/") != NULL) {

				int srash = strstr(value, "/") - value;
				char *denominator = (char *)malloc((strlen(buf) + 1)),
					*molecule = (char *)malloc((strlen(buf) + 1));
				strlcpy(molecule, value + 1, srash);
				strlcpy(denominator, value + srash + 1, strlen(buf) - srash);
				Command->val = strtod(molecule, NULL) / strtod(denominator, NULL);
			}
			else {
				if(strtod(value, NULL) != 0) Command->val = strtod(value, NULL);
				else Command->val = 1.0;
			}
		}
		else if (strcmp(command, "SCROLL") == 0) {
			Command->knd = SCroll;
			Command->val = strtod(value, NULL);
		}
		else if (strcmp(command, "DELAY") == 0) {
			Command->knd = DElay;
			Command->val = strtod(value, NULL);
		}
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

	else Command->knd = -1;
}

double get_FirstMeasureTime() {
	return MainFirstMeasureTime;
}