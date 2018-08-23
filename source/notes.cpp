#include <limits.h>
#include "header.h"
#include "main.h"
#include "time.h"
#include "audio.h"
#include "tja.h"
#include "notes.h"

#define Notes_Max 512
#define Line_Max 64

char buf_notes[160];

int find_notes_id(), find_line_id();
void notes_calc(bool isDon, bool isKa,double bpm, double tempo, double NowTime, int cnt, C2D_Sprite sprites[12]);


NOTES_T Notes[Notes_Max];
COMMAND_T Command;
LINE_T Line[Line_Max];

int MeasureCount = 0;
int RendaState = 0;
int NotesCount;	
int NotesNumber = 0;	//何番目のノーツか
double MeasureTime[2];
bool  isNotesLoad= true;


int ctoi(char c) {

	switch (c) {
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	default : return 0;
	}
}

int tempo;
double bpm,offset;

int cmp(const void *p, const void *q) {	//比較用

	int pp = ((NOTES_T*)p)->num;
	int qq = ((NOTES_T*)q)->num;

	if (((NOTES_T*)p)->flag == false) pp = INT_MAX;
	if (((NOTES_T*)p)->flag == false) qq = INT_MAX;

	return qq - pp;
}

void notes_sort() {	//ノーツを出現順にソート

	int n = sizeof Notes / sizeof(NOTES_T);
	qsort(Notes, n, sizeof(NOTES_T), cmp);
}

void notes_init(TJA_HEADER_T Tja_Header) {
	
	tja_notes_load();
	Command.data[0] = 0; Command.data[1] = 0; Command.data[2] = 0;
	Command.knd = 0;
	Command.val = 0;
	bpm = Tja_Header.bpm;
	offset = Tja_Header.offset;
	tempo = 4;
	NotesNumber = 0;
	MeasureCount = 0;

}

void notes_main(bool isDon,bool isKa, char tja_notes[2048][Max_Notes_Measure],MEASURE_T Measure[Measure_Max], int cnt, C2D_Sprite  sprites[12]) {

	double NowTime = time_now(0);

	snprintf(buf_notes, sizeof(buf_notes), "time0:%.2f", NowTime);
	debug_draw(0, 10, buf_notes);

	bool isEnd = false;

	if ( cnt >= 0 && isNotesLoad== true) {

		if (Measure[MeasureCount].create_time <= NowTime) {

			int LineId = find_line_id();
			if (LineId != -1) {
				Line[LineId].flag = true;
				Line[LineId].x = 400;
				Line[LineId].measure = MeasureCount;
			}

			MeasureTime[0] = NowTime;
			NotesCount = 0;
			
			while (tja_notes[Measure[MeasureCount].notes][NotesCount] != ',') {

				//命令
				if (NotesCount == 0 && tja_notes[Measure[MeasureCount].notes][0] == '#') {

					get_command_value(tja_notes[Measure[MeasureCount].notes],&Command);
					Command.notes = tja_notes[Measure[MeasureCount].notes];

					switch (Command.knd) {
					case ENd:
						isNotesLoad= false;						
						isEnd = true;
						break;

					case BPmchange:
						//bpm = Command.val;
						break;

					default:
						break;

					}

					if (isEnd == true) break;

					NotesCount = 0;
					MeasureCount++;
					continue;
				}
				if (NotesCount == 0 && strstr(tja_notes[Measure[MeasureCount].notes], ",") == NULL) {
					NotesCount = 0;
					MeasureCount++;
					continue;
				}
				NotesCount++;
			}

			for (int i = 0; i < NotesCount; i++) {

				int id = find_notes_id();
				
				if (id != -1 && tja_notes[Measure[MeasureCount].notes][i] != '0') {

					if (RendaState == 0 || tja_notes[Measure[MeasureCount].notes][i] == '8') {

						Notes[id].flag = true;
						Notes[id].notes_max = NotesCount;	//用途不明
						Notes[id].num = NotesNumber;
						Notes[id].x_ini = (Notes_Area / NotesCount)*i + 400.0;
						Notes[id].x = Notes[id].x_ini;
						Notes[id].bpm = Measure[MeasureCount].bpm;
						Notes[id].kind = ctoi(tja_notes[Measure[MeasureCount].notes][i]);
						//Notes[id].sec = MeasureCount;
						Notes[id].create_time = NowTime;
						Notes[id].judge_time = Notes[id].create_time + (Notes[id].x_ini - Notes_Judge) / (Notes_Area / (60 / Notes[id].bpm * tempo));

						switch (Notes[id].kind) {

						case Renda:
							RendaState = 1;
							break;
						case BigRenda:
							RendaState = 2;
							break;
						case RendaEnd:
							switch (RendaState) {
							case 1:
								Notes[id].kind = 8;
								break;
							case 2:
								Notes[id].kind = 9;
								break;
							}
							RendaState = 0;
							break;
						}

						NotesNumber++;
					}
				}
			}
			MeasureCount++;

			notes_sort();	//ソート
		}

	}

	
	for (int i = 0; i < Line_Max - 1; i++) {

		if (Line[i].flag == true) {

			Line[i].x = Notes_Area - Notes_Area * (NowTime - MeasureTime[0]) / (60.0*Measure[Line[i].measure].tempo /Measure[Line[i].measure].bpm);
			C2D_DrawRectangle(Line[i].x, 86, 0, 1, 46, C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1));
			if (Line[i].x < 62) Line[i].flag = false;
		}
	}

	notes_calc(isDon,isKa,bpm, tempo, NowTime, cnt, sprites);
	C2D_DrawRectangle(0 ,86, 0, 62, 58, C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1));
	debug_draw(0, 40, tja_notes[Measure[MeasureCount].notes - 1]);


	snprintf(buf_notes, sizeof(buf_notes), "time1:%.2f", NowTime);
	debug_draw(0, 0, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "cnt :%d", cnt);
	debug_draw(150, 0, buf_notes);

	snprintf(buf_notes, sizeof(buf_notes), "judge :%.1f", Measure[MeasureCount].judge_time);
	debug_draw(100, 30, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "bpm :%.1f", Measure[MeasureCount].bpm);
	debug_draw(0, 30, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "create :%.1f", Measure[MeasureCount].create_time);
	debug_draw(200, 30, buf_notes);

	snprintf(buf_notes, sizeof(buf_notes), "knd:%d", Command.knd);
	debug_draw(0, 170, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "val:%.2f", Command.val);
	debug_draw(100, 170, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "Com:%s", Command.command);
	debug_draw(200, 170, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "VAL:%s", Command.value);
	debug_draw(200, 180, buf_notes);
	snprintf(buf_notes, sizeof(buf_notes), "%s", Command.notes);
	debug_draw(0, 180, buf_notes);

	snprintf(buf_notes, sizeof(buf_notes), "creT:%f", Measure[0].create_time);
	debug_draw(0, 190, buf_notes);
	
	snprintf(buf_notes, sizeof(buf_notes), "END%d sec_cnt:%d",isEnd, MeasureCount);
	debug_draw(300, 30, buf_notes);
}

int find_notes_id() {

	for (int i = 0; i < Notes_Max-1; i++) {

		if (Notes[i].flag == false) return i;
	}
	return -1;
}

int find_line_id() {

	for (int i = 0; i < Line_Max - 1; i++) {

		if (Line[i].flag == false) return i;
	}
	return -1;
}

bool isJudgeDisp = false;	//要初期化
double JudgeMakeTime;		//
int JudgeDispKind;			//

void make_judge(int kind,double NowTime) {
	isJudgeDisp = true;
	JudgeMakeTime = NowTime;
	JudgeDispKind = kind;
}

void calc_judge(double NowTime) {

	if (isJudgeDisp == true) {
		switch (JudgeDispKind) {
		case 0:		//良
			debug_draw(80, 80, "ryou");
			break;
		case 1:		//可
			debug_draw(80, 80, "ka");
			break;
		case 2:		//不可
			debug_draw(80, 80, "fuka");
			break;
		}
		if (NowTime - JudgeMakeTime >= 0.25) isJudgeDisp = false;
	}
}

void notes_judge(double NowTime, bool isDon, bool isKa) {
	
	bool isAuto = true;
	int CurrentJudgeNotes[2] = { -1,-1 };		//現在判定すべきノーツ ドン,カツ
	double CurrentJudgeNotesLag[2] = { -1,-1 };	//判定すべきノーツの誤差(s)

	for (int i = 0; i < Notes_Max-1; i++) {

		if (Notes[i].flag == true) {

			if (Notes[i].kind == Don ||
				Notes[i].kind == BigDon ||
				Notes[i].kind == Renda ||
				Notes[i].kind == BigRenda ||
				Notes[i].kind == Balloon ||
				Notes[i].kind == RendaEnd ||
				Notes[i].kind == Potato) {	//ドン

				if (CurrentJudgeNotesLag[0] > fabs(Notes[i].judge_time - NowTime) ||
					CurrentJudgeNotesLag[0] == -1) {

					CurrentJudgeNotes[0] = i;
					CurrentJudgeNotesLag[0] = fabs(Notes[i].judge_time - NowTime);
				}
			}
			else if (
				Notes[i].kind == Ka ||
				Notes[i].kind == BigKa) {	//カツ

				if (CurrentJudgeNotesLag[1] > fabs(Notes[i].judge_time - NowTime) ||
					CurrentJudgeNotesLag[1] == -1) {

					CurrentJudgeNotes[1] = i;
					CurrentJudgeNotesLag[1] = fabs(Notes[i].judge_time - NowTime);
				}
			}
		}
	}

	if (isAuto == true) {	//オート

		bool isSe[2];

		for (int i = 0; i < Notes_Max-1; i++) {

			if (Notes[i].flag == true && Notes[i].judge_time <= NowTime) {

				if (isSe[0] == false ||
					Notes[i].kind == Don ||
					Notes[i].kind == BigDon ||
					Notes[i].kind == Renda ||
					Notes[i].kind == BigRenda ||
					Notes[i].kind == Balloon ||
					Notes[i].kind == RendaEnd ||
					Notes[i].kind == Potato) {
					
					isSe[0] = true;
					music_play(0);
				}
				else if (
					isSe[1] == false ||
					Notes[i].kind == Ka ||
					Notes[i].kind == BigKa) {

					isSe[1] = true;
					music_play(1);
				}

				Notes[i].flag = false;
			}
		}
	}

	else if (isAuto == false) {			//手動
		
		if (isDon == true && CurrentJudgeNotes[0] != -1) {	//ドン

			if (CurrentJudgeNotesLag[0] <= 0.034) {			//良
				make_judge(0,NowTime);
				Notes[CurrentJudgeNotes[0]].flag = false;
			}
			else if (CurrentJudgeNotesLag[0] <= 0.117) {	//可
				make_judge(1, NowTime);
				Notes[CurrentJudgeNotes[0]].flag = false;
			}
			else if (CurrentJudgeNotesLag[0] <= 0.150) {	//不可
				make_judge(2, NowTime);
				Notes[CurrentJudgeNotes[0]].flag = false;
			}
		}

		if (isKa == true && CurrentJudgeNotes[1] != -1) {			//カツ

			if (CurrentJudgeNotesLag[1] <= 0.034) {			//良
				make_judge(0, NowTime);
				Notes[CurrentJudgeNotes[1]].flag = false;
			}
			else if (CurrentJudgeNotesLag[1] <= 0.117) {	//可
				make_judge(1, NowTime);
				Notes[CurrentJudgeNotes[1]].flag = false;
			}
			else if (CurrentJudgeNotesLag[1] <= 0.150) {	//不可
				make_judge(2, NowTime);
				Notes[CurrentJudgeNotes[1]].flag = false;
			}
		}
	}

}

void notes_calc(bool isDon,bool isKa,double bpm, double tempo, double NowTime, int cnt, C2D_Sprite sprites[12]) {
	
	//int small_y = 95, big_y = 90;
	int notes_y = 108;
	
	for (int i = 0; i < Notes_Max; i++) {	//計算
		
		if (Notes[i].flag == true) {
			
			Notes[i].x = Notes[i].x_ini - Notes_Area * (NowTime - Notes[i].create_time) / (60 /Notes[i].bpm * tempo);

			snprintf(buf_notes, sizeof(buf_notes), "%d", i);
			debug_draw(Notes[i].x, notes_y+23, buf_notes);

		}
			
		if (Notes[i].x <= 40 ) Notes[i].flag = false;
	}

	for (int i = 0; i < Notes_Max; i++) {	//描画

		if (Notes[i].flag == true) {

			switch (Notes[i].kind) {
			case Don:
				C2D_SpriteSetPos(&sprites[dOn], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[dOn]);
				break;
			case Ka:
				C2D_SpriteSetPos(&sprites[kA], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[kA]);
				break;
			case BigDon:
				C2D_SpriteSetPos(&sprites[bIg_don], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_don]);
				break;
			case BigKa:
				C2D_SpriteSetPos(&sprites[bIg_ka], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_ka]);
				break;
			case Renda:
				C2D_SpriteSetPos(&sprites[rEnda], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[rEnda]);
				break;
			case BigRenda:
				C2D_SpriteSetPos(&sprites[bIg_renda], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_renda]);
				break;
			case Balloon:
				C2D_SpriteSetPos(&sprites[bIg_renda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_renda_fini]);
				break;
			case RendaEnd:		//8:連打終了 9:大連打終了
				C2D_SpriteSetPos(&sprites[rEnda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[rEnda_fini]);
				break;
			case RendaEnd + 1:
				C2D_SpriteSetPos(&sprites[bIg_renda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_renda_fini]);
				break;
			default:
				break;
			}
		}
	}
	notes_judge(NowTime, isDon, isKa);
	calc_judge(NowTime);
}