#include "header.h"
#include "notes.h"
char *tja_title, *tja_subtitle, *tja_level, *tja_bpm, *tja_wave, *tja_offset, *tja_balloon, *tja_songvol, *tja_sevol, *tja_scoreinit, *tja_scorediff, *tja_course, *tja_style, *tja_game, *tja_life, *tja_demostart, *tja_side, *tja_scoremode;
double bpm = -1;
char tja_notes[2048][128];
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

		while (fgets(tja_notes[tja_cnt], 128, fp) != NULL) {

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