#include "header.h"
#include "notes.h"
#include "tja.h"
#include "score.h"
#include "main.h"
#include "select.h"
#include "option.h"


char tja_notes[Measure_Max][Max_Notes_Measure];
int tja_cnt = 0, MeasureMaxNumber = 0;
double MainFirstMeasureTime;	//最初に"到達"する小節の到達所要時間　最初に"生成"はMeasure[0]で取得
bool isBranch = false;


TJA_HEADER_T Current_Header;
MEASURE_T Measure[Measure_Max];

void get_command_value(char* buf, COMMAND_T *Command);

void init_measure_structure() {

	for (int i = 0; i < Measure_Max; i++) {

		Measure[i].create_time = INT_MAX;
		Measure[i].judge_time = INT_MAX;
		Measure[i].pop_time = INT_MAX;
		Measure[i].bpm = 0;
		Measure[i].scroll = 0;
		Measure[i].notes = 0;
		Measure[i].flag = false;
		Measure[i].isDispBarLine = true;
		Measure[i].firstmeasure = -1;
		Measure[i].start_measure_count = 0;
		Measure[i].max_notes = 0;
		Measure[i].original_id = -1;
		Measure[i].notes_count = 0;
		Measure[i].command = -1;
	}
}

void init_tja() {

	init_measure_structure();
	tja_cnt = 0;
	MeasureMaxNumber = 0;
	MainFirstMeasureTime = 0;
	isBranch = false;
}

void load_tja_head(int course,LIST_T Song) {

	FILE *fp;
	char buf[128];
	bool isCourseMatch = true;

	Current_Header.title = (char*)"No title";
	Current_Header.subtitle = (char*)"";
	Current_Header.subtitle_state = -1;
	Current_Header.level = 0;
	Current_Header.bpm = 60.0;
	Current_Header.wave = (char*)"audio.ogg";
	Current_Header.offset = 0;
	Current_Header.balloon[0] = 5;
	Current_Header.songvol = 100;
	Current_Header.sevol = 100;
	Current_Header.scoreinit = -1;
	Current_Header.scorediff = -1;
	Current_Header.course = course;
	Current_Header.style = 1;
	Current_Header.life = -1;
	Current_Header.demostart = 0;
	Current_Header.side = 3;
	Current_Header.scoremode = 1;

	chdir(Song.path);
	int cnt = -1;

	if ((fp = fopen(Song.tja, "r")) != NULL) {

		char* temp = NULL;
		while (fgets(buf, 128, fp) != NULL) {

			cnt++;
			temp = (char *)malloc((strlen(buf) + 1));


			if (isCourseMatch == true && strstr(buf, "#START") == buf) {
				break;
			}

			if (strstr(buf, "TITLE:") == buf) {
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					Current_Header.title = temp;
				}
				continue;
			}
			if (cnt == 0) {
				if (strstr(buf, "TITLE:") == buf + 3 && strstr(buf, "SUBTITLE:") == 0) {
					if (buf[9] != '\n' && buf[9] != '\r') {
						strlcpy(temp, buf + 9, strlen(buf) - 10);
						Current_Header.title = temp;
					}
					continue;
				}
			}

			if (strstr(buf, "SUBTITLE:") == buf) {
				if (buf[9] != '\n' && buf[9] != '\r') {

					strlcpy(temp, buf + 9, strlen(buf) - 10);

					if (strstr(temp, "--") == temp) Current_Header.subtitle_state = 1;
					else if (strstr(temp, "++") == temp) Current_Header.subtitle_state = 2;
					else Current_Header.subtitle_state = 0;

					if (Current_Header.subtitle_state != 0) {
						strlcpy(temp, buf + 9 + 2, strlen(buf) - 10 - 2);
					}
					Current_Header.subtitle = temp;
				}
				continue;
			}

			if (strstr(buf, "LEVEL:") == buf) {
				if (buf[6] != '\n' && buf[6] != '\r') {
				strlcpy(temp, buf + 6, strlen(buf) - 7);
				Current_Header.level = atoi(temp);
			}
				continue;
			}

			if (strstr(buf, "BPM:") == buf) {
				if (buf[4] != '\n' && buf[4] != '\r') {
					strlcpy(temp, buf + 4, strlen(buf) - 5);
					Current_Header.bpm = atof(temp);
				}
				continue;
			}

			if (strstr(buf, "WAVE:") == buf) {
				if (buf[5] != '\n' && buf[5] != '\r') {
					strlcpy(temp, buf + 5, strlen(buf) - 6);
					Current_Header.wave = temp;
				}
				continue;
			}

			if (strstr(buf, "OFFSET:") == buf) {
				if (buf[7] != '\n' && buf[7] != '\r') {
					strlcpy(temp, buf + 7, strlen(buf) - 8);
					Current_Header.offset = atof(temp);
				}
				continue;
			}

			if (strstr(buf, "BALLOON:") == buf) {
				if (buf[8] != '\n' && buf[8] != '\r') {
					strlcpy(temp, buf + 8, strlen(buf) - 9);
					char *tp = strtok(temp, ",");
					Current_Header.balloon[0] = atoi(tp);
					int cnt = 1;
					while ((tp = strtok(NULL, ","))) {
						Current_Header.balloon[cnt] = atoi(tp);
						cnt++;
					}
				}
				continue;
			}

			if (strstr(buf, "SONGVOL:") == buf) {
				if (buf[8] != '\n' && buf[8] != '\r') {
					strlcpy(temp, buf + 8, strlen(buf) - 9);
					Current_Header.songvol = atoi(temp);
				}
				continue;
			}

			if (strstr(buf, "SEVOL:") == buf) {
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					Current_Header.sevol = atoi(temp);
				}
				continue;
			}

			if (strstr(buf, "SCOREINIT:") == buf) {
				if (buf[10] != '\n' && buf[10] != '\r') {
					strlcpy(temp, buf + 10, strlen(buf) - 11);
					Current_Header.scoreinit = atoi(temp);
				}
				continue;
			}

			if (strstr(buf, "SCOREDIFF:") == buf) {
				if (buf[10] != '\n' && buf[10] != '\r') {
					strlcpy(temp, buf + 10, strlen(buf) - 11);
					Current_Header.scorediff = atoi(temp);
				}
				continue;
			}


			if (strstr(buf, "COURSE:") == buf) {
				if (buf[7] != '\n' && buf[7] != '\r') {
					strlcpy(temp, buf + 7, strlen(buf) - 8);
					if (strlen(temp) == 1) Current_Header.course = atoi(temp);		//数字表記
					else if (strcmp(temp, "Easy") == 0 || strcmp(temp, "easy") == 0)   course = COURSE_EASY;	//文字表記
					else if (strcmp(temp, "Normal") == 0 || strcmp(temp, "normal") == 0) course = COURSE_NORMAL;
					else if (strcmp(temp, "Hard") == 0 || strcmp(temp, "hard") == 0)   course = COURSE_HARD;
					else if (strcmp(temp, "Oni") == 0 || strcmp(temp, "oni") == 0)    course = COURSE_ONI;
					else if (strcmp(temp, "Edit") == 0 || strcmp(temp, "edit") == 0)   course = COURSE_EDIT;

					if (Current_Header.course == course) {
						isCourseMatch = true;
					}
					else isCourseMatch = false;
				}
				continue;
			}

			if (strstr(buf, "STYLE:") == buf) {
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					Current_Header.style = atoi(temp);
				}
				continue;
			}

			if (strstr(buf, "LIFE:") == buf) {
				if (buf[5] != '\n' && buf[5] != '\r') {
					strlcpy(temp, buf + 5, strlen(buf) - 6);
					Current_Header.life = atoi(temp);
				}
				continue;
			}

			if (strstr(buf, "DEMOSTART:") == buf) {
				if (buf[10] != '\n' && buf[10] != '\r') {
					strlcpy(temp, buf + 10, strlen(buf) - 11);
					Current_Header.demostart = atof(temp);
				}
				continue;
			}

			if (strstr(buf, "SIDE:") == buf) {
				if (buf[5] != '\n' && buf[5] != '\r') {
					strlcpy(temp, buf + 5, strlen(buf) - 6);
					Current_Header.side = atoi(temp);
				}
				continue;
			}

			if (strstr(buf, "SCOREMODE:") == buf) {
				if (buf[10] != '\n' && buf[10] != '\r') {
					strlcpy(temp, buf + 10, strlen(buf) - 11);
					Current_Header.scoremode = atoi(temp);
				}
				continue;
			}

			free(temp);
		}

		fclose(fp);
		free(temp);

	}
	else {
		//tjaファイルが開けなかった時
	}
}

void load_tja_head_simple(LIST_T *List) {		//選曲用のヘッダ取得


	snprintf(List->title, sizeof(List->title), "No Title");
	snprintf(List->wave, sizeof(List->title), "audio.ogg");
	for (int i = 0; i < 5; i++) {
		List->level[i] = 0;
		List->course[i] = false;
	}

	FILE *fp;
	char buf[128],*temp = NULL;;
	int course = COURSE_ONI,cnt = 0;

	chdir(List->path);

	if ((fp = fopen(List->tja, "r")) != NULL) {

		while (fgets(buf, 128, fp) != NULL) {

			temp = (char *)malloc((strlen(buf) + 1));

			if (strstr(buf, "TITLE:") == buf) {
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(List->title, buf + 6, strlen(buf) - 7);
				}
				continue;
			}
			if (cnt == 0) {
				if (strstr(buf, "TITLE:") == buf+3 && strstr(buf, "SUBTITLE:") == 0) {
					if (buf[9] != '\n' && buf[9] != '\r') {
						strlcpy(List->title, buf + 9, strlen(buf) - 10);
					}
					continue;
				}
			}

			if (strstr(buf, "WAVE:") == buf) {
				if (buf[5] != '\n' && buf[5] != '\r') {
					strlcpy(List->wave, buf + 5, strlen(buf) - 6);
				}
				continue;
			}

			if (strstr(buf, "COURSE:") == buf) {
				if (buf[7] != '\n' && buf[7] != '\r') {
					strlcpy(temp, buf + 7, strlen(buf) - 8);
					if (strlen(temp) == 1) course = atoi(temp);			//数字表記
					else if (strcmp(temp, "Easy") == 0 || strcmp(temp, "easy") == 0)   course = COURSE_EASY;	//文字表記
					else if (strcmp(temp, "Normal") == 0 || strcmp(temp, "normal") == 0) course = COURSE_NORMAL;
					else if (strcmp(temp, "Hard") == 0 || strcmp(temp, "hard") == 0)   course = COURSE_HARD;
					else if (strcmp(temp, "Oni") == 0 || strcmp(temp, "oni") == 0)    course = COURSE_ONI;
					else if (strcmp(temp, "Edit") == 0 || strcmp(temp, "edit") == 0)   course = COURSE_EDIT;


					List->course[course] = true;
					List->course_exist[course] = true;
				}

				continue;
			}

			if (strstr(buf, "LEVEL:") == buf) {
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					List->level[course] = atoi(temp);

					List->course[course] = true;
				}
				continue;
			}
			cnt++;
		}
	}


	free(temp);
	fclose(fp);
}

void MeasureInsertionSort(MEASURE_T t[], int array_size) {	//create_timeでソート

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

double calc_first_measure_time() {	//最初に到達する小節の所要時間を計算

	int tmp = -1;

	for (int i = 0; i < Measure_Max; i++) {

		if (Measure[i].flag == true && Measure[i].command == -1) {

			if (tmp == -1) {	//初回
				tmp = i;
				continue;
			}

			if (Measure[i].judge_time < Measure[tmp].judge_time) tmp = i;
		}
	}

	return Measure[tmp].judge_time + Measure[0].create_time * -1;

}

void load_tja_notes(int course, LIST_T Song) {

	int FirstMultiMeasure = -1,	//複数行の小節の最初の小節id 複数出ない場合は-1
		NotesCount = 0, BranchCourse = -1;
	bool isStart = false, isEnd = false, isDispBarLine = true, isNoComma = false,isCourseMatch = false;
	FILE *fp;
	COMMAND_T Command;
	OPTION_T Option;
	get_option(&Option);

	double bpm = Current_Header.bpm,
		NextBpm = bpm,
		measure = 1,
		scroll = 1,
		NextMeasure = 1,
		delay = 0,
		percent = 1,
		BeforeBranchJudgeTime = 0,
		BeforeBranchCreateTime = 0,
		BeforeBranchPreJudge = 0,
		BeforeBranchBpm = 0,
		BeforeBranchDelay = 0,
		BeforeBranchMeasure = 0,
		BeforeBranchScroll = 1;

	if (course == -1) isCourseMatch = true;		//コース表記なし

	chdir(Song.path);
	if ((fp = fopen(Song.tja, "r")) != NULL) {

		tja_cnt = 0;
		int MeasureCount = 0,CurrentCourse = -1;
		double PreJudge = 0, FirstMeasureTime = 0;

		FirstMeasureTime = (60.0 / bpm * 4 * measure)*(Notes_Judge_Range / Notes_Area) - 60.0 / bpm * 4 * measure;
		PreJudge = FirstMeasureTime;


		while (
			(fgets(tja_notes[tja_cnt], Max_Notes_Measure, fp) != NULL || tja_cnt < Measure_Max) &&
			isEnd == false
			) {

			if (strstr(tja_notes[tja_cnt], "COURSE:") == tja_notes[tja_cnt]) {

				char* temp = NULL;
				temp = (char *)malloc((strlen(tja_notes[tja_cnt]) + 1));

				strlcpy(temp, tja_notes[tja_cnt] + 7, strlen(tja_notes[tja_cnt]) - 8);
				if (strlen(temp) == 1) CurrentCourse = atoi(temp);		//数字表記
				else if (strcmp(temp, "Easy") ==   0 || strcmp(temp, "easy") == 0)   CurrentCourse = COURSE_EASY;	//文字表記
				else if (strcmp(temp, "Normal") == 0 || strcmp(temp, "normal") == 0) CurrentCourse = COURSE_NORMAL;
				else if (strcmp(temp, "Hard") ==   0 || strcmp(temp, "hard") == 0)   CurrentCourse = COURSE_HARD;
				else if (strcmp(temp, "Oni") ==    0 || strcmp(temp, "oni") == 0)    CurrentCourse = COURSE_ONI;
				else if (strcmp(temp, "Edit") ==   0 || strcmp(temp, "edit") == 0)   CurrentCourse = COURSE_EDIT;

				free(temp);

				if (course == CurrentCourse) isCourseMatch = true;

				continue;
			}

			if (isStart == false && isCourseMatch == true && strstr(tja_notes[tja_cnt], "#START") == tja_notes[tja_cnt]) {

				isStart = true;
				continue;
			}

			if (isStart == true && isCourseMatch == true) {

				//一文字目がコメントアウトの時スキップ
				if (strstr(tja_notes[tja_cnt], "//") == tja_notes[tja_cnt] || strstr(tja_notes[tja_cnt], "\r") == tja_notes[tja_cnt]) {

					tja_cnt++;
					continue;
				}

				if (strstr(tja_notes[tja_cnt], ",") == NULL && tja_notes[tja_cnt][0] != '#') {
					isNoComma = true;

					if (FirstMultiMeasure == -1) {

						FirstMultiMeasure = MeasureCount;
						Measure[FirstMultiMeasure].original_id = FirstMultiMeasure;	//ソート前のidを格納
					}
				}
				else {
					isNoComma = false;
				}

				if (tja_notes[tja_cnt][0] == '#') {

					get_command_value(tja_notes[tja_cnt], &Command);
					Measure[MeasureCount].command = Command.knd;
					switch (Command.knd) {
					case COMMAND_BPMCHANGE:
						NextBpm = Command.val[0];
						break;
					case COMMAND_MEASURE:
						NextMeasure = Command.val[0];
						break;
					case COMMAND_SCROLL:
						scroll = Command.val[0];
						break;
					case COMMAND_DELAY:
						delay = Command.val[0];
						break;
					case COMMAND_BARLINEON:
						isDispBarLine = true;
						break;
					case COMMAND_BARLINEOFF:
						isDispBarLine = false;
						break;
					case COMMAND_N:
						BranchCourse = COMMAND_N;
						break;
					case COMMAND_E:
						BranchCourse = COMMAND_E;
						break;
					case COMMAND_M:
						BranchCourse = COMMAND_M;
						break;
					case COMMAND_BRANCHSTART:
						isBranch = true;
						break;
					case COMMAND_BRANCHEND:
						BranchCourse = -1;
						break;
					case COMMAND_SECTION:
						Measure[MeasureCount].command = COMMAND_SECTION;
						break;
					case COMMAND_END:
						isEnd = true;
						break;
					default:
						break;
					}
				}
				else {

					if (isNoComma == true || NotesCount != 0) {	//複数小節

						Measure[MeasureCount].start_measure_count = NotesCount;
						int i = 0;
						while (tja_notes[tja_cnt][i] != '\n' && tja_notes[tja_cnt][i] != ',' && tja_notes[tja_cnt][i] != '/') i++;
						NotesCount += i - 1;
						if (tja_notes[tja_cnt][i] == '/') NotesCount++;
						if (tja_notes[tja_cnt][i] != ',' && tja_notes[tja_cnt][i] != '/') i--;
						Measure[MeasureCount].notes_count = i;

					}
				}

				Measure[MeasureCount].flag = true;
				Measure[MeasureCount].notes = tja_cnt;
				Measure[MeasureCount].firstmeasure = FirstMultiMeasure;
				Measure[MeasureCount].bpm = NextBpm;
				Measure[MeasureCount].measure = NextMeasure;
				Measure[MeasureCount].scroll = scroll;
				Measure[MeasureCount].judge_time = 60.0 / bpm * 4 * measure * percent + PreJudge + delay;
				Measure[MeasureCount].pop_time = Measure[MeasureCount].judge_time - (60.0 / Measure[MeasureCount].bpm * 4)*(Notes_Judge_Range / Notes_Area);
				Measure[MeasureCount].create_time = Measure[MeasureCount].judge_time - (60.0 / Measure[MeasureCount].bpm * 4)*(Notes_Judge_Range / (Notes_Area*scroll* Option.speed));
				Measure[MeasureCount].isDispBarLine = isDispBarLine;
				Measure[MeasureCount].branch = BranchCourse;

				if (tja_notes[tja_cnt][0] == '#') {

					if (MeasureCount > 0) {
						Measure[MeasureCount].judge_time = Measure[MeasureCount - 1].judge_time;
						Measure[MeasureCount].create_time = Measure[MeasureCount - 1].create_time;
						//Measure[MeasureCount].isDispBarLine = false;
					}
					switch (Command.knd) {
					case COMMAND_BRANCHSTART:
						BeforeBranchJudgeTime = Measure[MeasureCount].judge_time;
						BeforeBranchCreateTime = Measure[MeasureCount].create_time;
						BeforeBranchBpm = bpm;
						BeforeBranchDelay = delay;
						BeforeBranchMeasure = measure;
						BeforeBranchPreJudge = PreJudge;
						BeforeBranchScroll = scroll;
						break;
					case COMMAND_N:
					case COMMAND_E:
					case COMMAND_M:
						bpm = BeforeBranchBpm;
						measure = BeforeBranchMeasure;
						delay = BeforeBranchDelay;
						scroll = BeforeBranchScroll;
						Measure[MeasureCount].judge_time = BeforeBranchJudgeTime;
						Measure[MeasureCount].create_time = BeforeBranchCreateTime;
						PreJudge = BeforeBranchPreJudge;
						break;
					default:
						break;
					}
				}
				else {
					if (isNoComma == false) PreJudge = Measure[MeasureCount].judge_time;
					bpm = NextBpm;
					measure = NextMeasure;
					delay = 0;
				}


				if (isNoComma == false && NotesCount != 0 && tja_notes[tja_cnt][0] != '#') {	//複数行小節の最後の行

					Measure[Measure[MeasureCount].firstmeasure].max_notes = NotesCount + 1;
					FirstMultiMeasure = -1;
					NotesCount = 0;

					for (int i = 1; i < MeasureCount - Measure[MeasureCount].firstmeasure + 1; i++) {	//judge_timeの調整
						
						if (tja_notes[Measure[MeasureCount].notes][0] != '#') {	//複数行小節の最初の小節以外

							Measure[Measure[MeasureCount].firstmeasure + i].judge_time =
								Measure[Measure[MeasureCount].firstmeasure + i - 1].judge_time +
								(60.0 / Measure[Measure[MeasureCount].firstmeasure + i - 1].bpm * 4 * Measure[Measure[MeasureCount].firstmeasure + i - 1].measure)
								* Measure[Measure[MeasureCount].firstmeasure + i - 1].notes_count / Measure[Measure[MeasureCount].firstmeasure].max_notes;	//delayはとりあえず放置
							
							Measure[Measure[MeasureCount].firstmeasure + i].pop_time    = Measure[Measure[MeasureCount].firstmeasure + i].judge_time - (60.0 / Measure[Measure[MeasureCount].firstmeasure + i].bpm * 4)*(Notes_Judge_Range / (Notes_Area));
							Measure[Measure[MeasureCount].firstmeasure + i].create_time = Measure[Measure[MeasureCount].firstmeasure + i].judge_time - (60.0 / Measure[Measure[MeasureCount].firstmeasure + i].bpm * 4)*((1.0/Option.speed)*Notes_Judge_Range / (Notes_Area*Measure[Measure[MeasureCount].firstmeasure + i].scroll));
							percent = (double)Measure[Measure[MeasureCount].firstmeasure + i].notes_count / (double)Measure[Measure[MeasureCount].firstmeasure].max_notes;

							Measure[Measure[MeasureCount].firstmeasure + i].isDispBarLine = false;	//最初の小節は小節線をオフにしない
						}
					}

					PreJudge = Measure[MeasureCount].judge_time;
				}
				else if (tja_notes[tja_cnt][0] != '#') {
					percent = 1;
				}


				if (isEnd == true) {
					break;
				}

				tja_cnt++;
				MeasureCount++;
			}
		}

		MeasureMaxNumber = tja_cnt;

		for (int i = 0; i < MeasureMaxNumber; i++) {	//次の小節の判定時に発動する命令の調整

			if (Measure[i].command == COMMAND_SECTION) {
				int n = i + 1;
				while (n <= MeasureMaxNumber && tja_notes[n][0] == '#') n++;
				Measure[i].judge_time = Measure[n].judge_time;
			}
		}

		//基本天井点を計算

		calc_base_score(Measure, tja_notes);

		fclose(fp);
		MeasureInsertionSort(Measure, Measure_Max);
		MainFirstMeasureTime = calc_first_measure_time();
	}
}

void get_tja_header(TJA_HEADER_T *TJA_Header) {

	*TJA_Header = Current_Header;
}

void tja_to_notes(bool isDon, bool isKatsu, int count, C2D_Sprite sprites[Sprite_Number]) {

	notes_main(isDon, isKatsu, tja_notes, Measure, count, sprites);

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
		Command->val[0] = 0;
		Command->val[1] = 0;
		Command->val[2] = 0;

		if (strcmp(command, "START") == 0) Command->knd = COMMAND_START;
		else if (strcmp(command, "END") == 0) Command->knd = COMMAND_END;
		else if (strcmp(command, "BPMCHANGE") == 0) {
			Command->knd = COMMAND_BPMCHANGE;
			Command->val[0] = strtod(value, NULL);
		}
		else if (strcmp(command, "GOGOSTART") == 0) Command->knd = COMMAND_GOGOSTART;
		else if (strcmp(command, "GOGOEND") == 0) Command->knd = COMMAND_GOGOEND;

		else if (strcmp(command, "MEASURE") == 0) {
			Command->knd = COMMAND_MEASURE;
			if (strstr(value, "/") != NULL) {

				int srash = strstr(value, "/") - value;
				char *denominator = (char *)malloc((strlen(buf) + 1)),
					*molecule = (char *)malloc((strlen(buf) + 1));
				strlcpy(molecule, value + 1, srash);
				strlcpy(denominator, value + srash + 1, strlen(buf) - srash);
				Command->val[0] = strtod(molecule, NULL) / strtod(denominator, NULL);
				free(denominator);
				free(molecule);
			}
			else {
				if (strtod(value, NULL) != 0) Command->val[0] = strtod(value, NULL);
				else Command->val[0] = 1.0;
			}
		}
		else if (strcmp(command, "SCROLL") == 0) {
			Command->knd = COMMAND_SCROLL;
			Command->val[0] = strtod(value, NULL);
		}
		else if (strcmp(command, "DELAY") == 0) {
			Command->knd = COMMAND_DELAY;
			Command->val[0] = strtod(value, NULL);
		}
		else if (strcmp(command, "SECTION") == 0) Command->knd = COMMAND_SECTION;
		else if (strcmp(command, "BRANCHSTART") == 0) {
			Command->knd = COMMAND_BRANCHSTART;
			char* tp;
			tp = strtok(value, ",");
			switch (value[1]) {
			case 'r':Command->val[0] = 0; break;	//連打
			case 'p':Command->val[0] = 1; break;	//精度
			case 's':Command->val[0] = 2; break;	//スコア
			default: break;
			}
			int i = 1;
			while ((tp = strtok(NULL, ","))) {
				Command->val[i] = strtod(tp, NULL);
				i++;
			}
		}
		else if (strcmp(command, "BRANCHEND") == 0) Command->knd = COMMAND_BRANCHEND;
		else if (strcmp(command, "N") == 0) Command->knd = COMMAND_N;
		else if (strcmp(command, "E") == 0) Command->knd = COMMAND_E;
		else if (strcmp(command, "M") == 0) Command->knd = COMMAND_M;
		else if (strcmp(command, "LEVELHOLD") == 0) Command->knd = COMMAND_LEVELHOLD;
		else if (strcmp(command, "BMSCROLl") == 0) Command->knd = COMMAND_BMSCROLL;
		else if (strcmp(command, "HBSCROLL") == 0) Command->knd = COMMAND_HBSCROLL;
		else if (strcmp(command, "BARLINEOFF") == 0) Command->knd = COMMAND_BARLINEOFF;
		else if (strcmp(command, "BARLINEON") == 0) Command->knd = COMMAND_BARLINEON;
		else Command->knd = -1;

		free(command);
		free(value);
	}

	else Command->knd = -1;
}

double get_FirstMeasureTime() {
	return MainFirstMeasureTime;
}

int MeasureIdFromOriginalId(int id) {

	for (int i = 0; i < Measure_Max; i++) {

		if (Measure[i].original_id == id) return i;
	}
	return -1;
}

bool get_isBranch() {
	return isBranch;
}