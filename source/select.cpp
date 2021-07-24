#include "header.h"
#include "main.h"
#include "select.h"
#include "tja.h"
#include "audio.h"
#include "notes.h"
#include "option.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <jansson.h>

void load_file_list(const char* path);
void set_genres();

LIST_T List[LIST_MAX];
GENRE_T Genre[GENRE_MAX];
char buf_select[256];
int SongNumber = 0;			//曲の総数
int GenreNumber = 0;		//ジャンルの総数
int ClosedSongNumber = 0;	//閉じたジャンル内の曲数
int GenreCount = 0, SongCount = 0, cursor = 0, course_cursor = 0, course_count = 0, SelectedId = 0, SelectedGenreId = 0 ,course = COURSE_ONI;
bool isSelectCourse = false, isCursorGenre = false, isGameStart = false;

int cmp_list(const void* p, const void* q) {	//比較用

	int pp = ((LIST_T*)p)->genre;
	int qq = ((LIST_T*)q)->genre;

	return pp - qq;
}

void sort_list() {	//曲をジャンル順にソート

	qsort(List, SongNumber, sizeof(LIST_T), cmp_list);
}

void load_file_main() {

	chdir(DEFAULT_DIR);
	load_file_list(DEFAULT_DIR);
	SongNumber = SongCount;
	GenreNumber = GenreCount;
	set_genres();
	sort_list();
}

void load_genre_file(int id) {

	json_t* json;
	json_error_t error_json;
	char tmp[256];

	chdir(Genre[id].path);
	json = json_load_file(GENRE_FILE, 0, &error_json);

	strlcpy(Genre[id].name, "Genre", 256);
	Genre[id].genre_color = 0x111111;
	Genre[id].font_color = 0xffffff;

	if (json != NULL) {

		Genre[id].isOpened = false;
		strlcpy(Genre[id].name, json_string_value(json_object_get(json, "GenreName")), sizeof(Genre[id].name));
		strlcpy(tmp, json_string_value(json_object_get(json, "GenreColor")), sizeof(tmp));
		if (tmp[0] == '#') tmp[0] = ' ';
		Genre[id].genre_color = strtol(tmp, NULL, 16);
		strlcpy(tmp, json_string_value(json_object_get(json, "FontColor")), sizeof(tmp));
		if (tmp[0] == '#') tmp[0] = ' ';
		Genre[id].font_color = strtol(tmp, NULL, 16);
	}
	if (json == NULL) {	//開けなかった時
	}

	json_decref(json);
}

void load_file_list(const char* path) {

	DIR* dir;
	struct dirent* dp;

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

						strlcpy(List[SongCount].tja, dp->d_name, strlen(dp->d_name) + 1);
						getcwd(List[SongCount].path, 256);
						List[SongCount].genre = GENRE_MAX + 1;
						load_tja_head_simple(&List[SongCount]);
						SongCount++;
					}

					if (strstr(dp->d_name, GENRE_FILE) != NULL) {

						getcwd(Genre[GenreCount].path, 256);
						load_genre_file(GenreCount);
						GenreCount++;
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

void set_genres() {

	for (int i = 0; i < GenreNumber; i++) {

		for (int j = 0; j < SongNumber; j++) {

			if (strstr(List[j].path, Genre[i].path) != NULL) List[j].genre = i;
		}
	}
}

void draw_select_box(float x,float y,float w,float h,int color= 0x424242) {

	float r = ((color >> 16) & 0xFF) / 255.0;
	float g = ((color >> 8) & 0xFF) / 255.0;
	float b = ((color >> 0) & 0xFF) / 255.0;
	C2D_DrawRectangle(x, y, 0, w, h, C2D_Color32f(r, g, b, 1), C2D_Color32f(r, g, b, 1), C2D_Color32f(r, g, b, 1), C2D_Color32f(r, g, b, 1));
}



void disp_file_list() {

	int n = 0, g = 0;	//コース用調整、ジャンル用調整
	bool isGenre = false;
	course_count = 0;

	if (cursor > 0) cursor = -1 * (SongNumber + GenreNumber - ClosedSongNumber - 1);
	if (cursor < -1 * (SongNumber + GenreNumber - ClosedSongNumber- 1)) cursor = 0;

	ClosedSongNumber = 0;

	for (int i = 0; i < SongNumber; i++) {

		isGenre = false;

		if (List[i].genre != GENRE_MAX + 1 && (i == 0 || List[i].genre != List[i - 1].genre)) {	//ジャンルの最初の曲

			g++;
			isGenre = true;
		}


		if ((n + g + cursor) * 20 + 60 >= 0 && (n + g + cursor) * 20 + 60 <= TOP_HEIGHT) {

			if (isGenre == true) {
				
				draw_select_box(30, (n + g + cursor - 1) * 20 + 60-3,320,20, Genre[List[i].genre].genre_color);
				snprintf(buf_select, sizeof(buf_select), "%s", Genre[List[i].genre].name);
				draw_select_text(30, (n + g + cursor - 1) * 20 + 60, buf_select,Genre[List[i].genre].font_color);

				if (n + g -1== (cursor * -1)) {

					SelectedGenreId = List[i].genre;
					isCursorGenre = true;
				}
			}
			
			if (List[i].genre == GENRE_MAX + 1 || (List[i].genre != GENRE_MAX + 1 && Genre[List[i].genre].isOpened == true)) {

				snprintf(buf_select, sizeof(buf_select), "%s", List[i].title);
				int x = (List[i].genre != GENRE_MAX + 1) * 25 + 30;
				draw_select_text(x, (n + g + cursor) * 20 + 60, buf_select);

				if (i + g != (cursor * -1)) {
					snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[COURSE_ONI]);
					draw_select_text(360, (n + g + cursor) * 20 + 60, buf_select);
				}
			}
		}

		if (List[i].genre != GENRE_MAX + 1 && Genre[List[i].genre].isOpened == false) {

			ClosedSongNumber++;
			continue;
		}

		n++;

		if (n + g-1 == (cursor * -1)) {

			SelectedId = i;
			int level;
			isCursorGenre = false;

			if (List[i].course[COURSE_EDIT] == true) {

				if ((n + g + cursor - 1) == course_cursor) course = COURSE_EDIT;
				level = List[i].level[COURSE_EDIT];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select_text(200 + j * 10, (n + g + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select_text(200 + (j + level) * 10, (n + g + cursor) * 20 + 60, "・");
				}
				draw_select_text(80, (n + g + cursor) * 20 + 60, Text[get_lang()][TEXT_ONI]);
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[COURSE_EDIT]);
				draw_select_text(360, (n + g + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[COURSE_ONI] == true) {

				if ((n + g + cursor - 1) == course_cursor) course = COURSE_ONI;
				level = List[i].level[COURSE_ONI];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select_text(200 + j * 10, (n + g + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select_text(200 + (j + level) * 10, (n + g + cursor) * 20 + 60, "・");
				}
				draw_select_text(80, (n + g + cursor) * 20 + 60, Text[get_lang()][TEXT_ONI]);
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[COURSE_ONI]);
				draw_select_text(360, (n + g + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[COURSE_HARD] == true) {

				if ((n + g + cursor - 1) == course_cursor) course = COURSE_HARD;
				level = List[i].level[COURSE_HARD];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select_text(200 + j * 10, (n + g + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select_text(200 + (j + level) * 10, (n + g + cursor) * 20 + 60, "・");
				}
				draw_select_text(80, (n + g + cursor) * 20 + 60, Text[get_lang()][TEXT_HARD]);
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[COURSE_HARD]);
				draw_select_text(360, (n + g + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[COURSE_NORMAL] == true) {

				if ((n + g + cursor - 1) == course_cursor) course = COURSE_NORMAL;
				level = List[i].level[COURSE_NORMAL];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select_text(200 + j * 10, (n + g + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select_text(200 + (j + level) * 10, (n + g + cursor) * 20 + 60, "・");
				}
				draw_select_text(80, (n + g + cursor) * 20 + 60, Text[get_lang()][TEXT_NORMAL]);
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[COURSE_NORMAL]);
				draw_select_text(360, (n + g + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (List[i].course[COURSE_EASY] == true) {

				if ((n + g + cursor - 1) == course_cursor) course = COURSE_EASY;
				level = List[i].level[COURSE_EASY];
				if (level > 10) level = 10;
				for (int j = 0; j < level; j++) {
					draw_select_text(200 + j * 10, (n + g + cursor) * 20 + 60, "★");
				}
				for (int j = 0; j < (10 - level); j++) {
					draw_select_text(200 + (j + level) * 10, (n + g + cursor) * 20 + 60, "・");
				}
				draw_select_text(80, (n + g + cursor) * 20 + 60, Text[get_lang()][TEXT_EASY]);
				snprintf(buf_select, sizeof(buf_select), "★x%d", List[i].level[COURSE_EASY]);
				draw_select_text(360, (n + g + cursor) * 20 + 60, buf_select);
				n++;
				course_count++;
			}

			if (isSelectCourse == true) {

				draw_select_text(60, (course_cursor + 1) * 20 + 60, ">>");
				//snprintf(buf_select, sizeof(buf_select), "%d",course_cursor);
				//draw_select_text(60, (course_cursor + 1) * 20 + 60, buf_select);
			}
		}
	}

	draw_select_text(10, 60, ">>");
	//snprintf(buf_select, sizeof(buf_select), "isCS:%d SI:%d SGI:%dGf:%d",isCursorGenre,SelectedId, SelectedGenreId,Genre[SelectedGenreId].flag);
	//draw_select_text(0, 50, buf_select);

}

void update_cursor(int knd) {

	if (knd == KEY_UP) {
		if (isSelectCourse == false) cursor++;
		else if (course_cursor > 0) course_cursor--;
		play_sound(SOUND_KATSU);
	}
	else if (knd == (int)KEY_DOWN) {
		if (isSelectCourse == false) cursor--;
		else if (course_cursor < (course_count - 1)) course_cursor++;
		play_sound(SOUND_KATSU);
	}
	else if (knd == KEY_RIGHT) {
		if (isSelectCourse == false) cursor -= 5;
		play_sound(SOUND_KATSU);
	}
	else if (knd == KEY_LEFT) {
		if (isSelectCourse == false) cursor += 5;
		play_sound(SOUND_KATSU);
	}
	else if (knd == KEY_A && (course_count != 0 || isCursorGenre == true)) {
		if (isCursorGenre == true) Genre[SelectedGenreId].isOpened = !Genre[SelectedGenreId].isOpened;
		else if (isSelectCourse == true) isGameStart = true;
		else isSelectCourse = true;
		play_sound(SOUND_DON);
	}
	else if (knd == KEY_B) {
		isSelectCourse = false;
		course_cursor = 0;
		play_sound(SOUND_KATSU);
	}

}

C2D_TextBuf g_SelectText = C2D_TextBufNew(4096);
C2D_Text SelectText;

void draw_select_text(float x, float y, const char* text,int color) {	//color省略可(0xffffff)

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);
	float r = ((color >> 16) & 0xFF)/255.0;
	float g = ((color >> 8) & 0xFF)/255.0;
	float b = ((color >> 0) & 0xFF)/255.0;
	C2D_DrawText(&SelectText, C2D_WithColor, x, y, 1.0f, 0.5f, 0.5f, C2D_Color32f(r, g, b, 1.0f));
}

void draw_result_text(float x, float y, float size, const char* text) {

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);
	C2D_DrawText(&SelectText, C2D_WithColor, x, y, 0.5f, size, size, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
}

void calc_result_text(const char* text, float* width, float* height) {

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);
	float size = 0.7;
	C2D_TextGetDimensions(&SelectText, size, size, width, height);
}

void draw_option_text(float x, float y, const char* text, bool state, float* width, float* height, float sizex, float sizey) {	//size省略可(0.7)

	C2D_TextBufClear(g_SelectText);
	C2D_TextParse(&SelectText, g_SelectText, text);
	C2D_TextOptimize(&SelectText);
	C2D_TextGetDimensions(&SelectText, sizex, sizey, width, height);

	if (x == -1) x = BOTTOM_WIDTH / 2 - *width / 2;

	if (state == false) {
		C2D_DrawText(&SelectText, C2D_WithColor, x, y, 1.0f, sizex, sizey, C2D_Color32f(100.0 / 255.0, 100.0 / 255.0, 100.0 / 255.0, 1.0f));
	}
	else if (state == true) {
		C2D_DrawText(&SelectText, C2D_WithColor, x, y, 1.0f, sizex, sizey, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
	}
	
}

void get_SelectedId(LIST_T* TMP, int* arg) {

	for (int i = 0; i < 5; i++) {
		TMP->course[i] = List[SelectedId].course[i];
		TMP->course_exist[i] = List[SelectedId].course_exist[i];
		TMP->level[i] = List[SelectedId].level[i];
	}
	strlcpy(TMP->tja, List[SelectedId].tja, strlen(List[SelectedId].tja) + 1);
	strlcpy(TMP->path, List[SelectedId].path, strlen(List[SelectedId].path) + 1);
	strlcpy(TMP->title, List[SelectedId].title, strlen(List[SelectedId].title) + 1);
	strlcpy(TMP->wave, List[SelectedId].wave, strlen(List[SelectedId].wave) + 1);
	*arg = course;
}

bool get_isGameStart() {
	return isGameStart;
}

void select_ini() {
	//cursor = 0;
	course_cursor = 0;
	course_count = 0;
	SelectedId = 0;
	SelectedGenreId = 0;
	course = COURSE_ONI;
	isSelectCourse = false;
	isCursorGenre = false;
	isGameStart = false;
}