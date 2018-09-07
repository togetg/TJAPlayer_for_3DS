#include "header.h"
#include "main.h"
#include "time.h"
#include "audio.h"
#include "tja.h"
#include "notes.h"

double bpm, offset;
char buf_notes[160];

int find_notes_id(), find_line_id();
void notes_calc(bool isDon, bool isKa,double bpm, double NowTime, int cnt, C2D_Sprite sprites[Sprite_Number]);
void notes_draw(C2D_Sprite sprites[Sprite_Number]);


NOTES_T Notes[Notes_Max];
COMMAND_T Command;
BARLINE_T BarLine[BarLine_Max];
RENDA_T RendaNotes[Renda_Max];

int MeasureCount = 0;
int RendaState = 0;
int NotesCount;	
int NotesNumber = 0;	//何番目のノーツか
bool  isNotesLoad= true,isAuto = true,isJudgeDisp = false;	//要初期化
double JudgeMakeTime, JudgeY;
int JudgeDispknd;


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

int notes_cmp(const void *p, const void *q) {	//比較用

	int pp = ((NOTES_T*)p)->judge_time*10000;
	int qq = ((NOTES_T*)q)->judge_time*10000;

	//if (((NOTES_T*)p)->flag == false) pp = INT_MAX;
	//if (((NOTES_T*)p)->flag == false) qq = INT_MAX;

	return qq - pp;
}

void notes_sort() {	//ノーツを出現順にソート
	int n = sizeof Notes / sizeof(NOTES_T);
	qsort(Notes, n, sizeof(NOTES_T), notes_cmp);
}

void delete_renda(int i) {

	if (i >= 0 && i <= Renda_Max) {
		RendaNotes[i].id = -1;
		RendaNotes[i].start_x = -1;
		RendaNotes[i].finish_x = -1;
		RendaNotes[i].flag = false;
	}
}

void renda_notes_init(){

	for (int i = 0; i < Renda_Max; i++) {
		delete_renda(i);
	}
}

int find_renda_id() {

	for (int i = 0; i < Renda_Max - 1; i++) {

		if (RendaNotes[i].flag == false) return i;
	}
	return -1;
}

int make_renda_start(double x) {

	int id = find_renda_id();
	if (id != -1) {

		RendaNotes[id].id = id;
		RendaNotes[id].start_x = x;
		RendaNotes[id].finish_x = -1;
		RendaNotes[id].flag = true;
		return id;
	}
	else return -1;
}

int find_renda_finish_id() {	//startの値だけ入ってる連打idを返す

	for (int i = 0; i < Renda_Max - 1; i++) {

		if (RendaNotes[i].flag == true &&
			RendaNotes[i].start_x != -1 &&
			RendaNotes[i].finish_x == -1) return i;
	}
	return -1;
}

int make_renda_finish(double x) {

	int id = find_renda_finish_id();
	if (id != -1) {

		RendaNotes[id].finish_x = x;
		return id;
	}
	else return -1;
}

void delete_notes(int i) {
	

	if (i >= 0 &&
		Notes[i].renda_id != -1 &&
		RendaNotes[Notes[i].renda_id].flag == true &&
		(Notes[i].knd == RendaEnd || Notes[i].knd == BigRendaEnd)) {	//連打削除

		delete_notes(RendaNotes[Notes[i].renda_id].start_id);
		delete_renda(Notes[i].renda_id);
	}

	if (i >= 0 && i <= Notes_Max) {
		Notes[i].flag = false;
		Notes[i].num = 0;
		Notes[i].knd = 0;
		Notes[i].notes_max = 0;
		Notes[i].x_ini = 0;
		Notes[i].x = 0;
		Notes[i].create_time = 0;
		Notes[i].judge_time = 0;
		Notes[i].pop_time = 0;
		Notes[i].bpm = 0;
		Notes[i].scroll = 0;
		Notes[i].renda_id = -1;
	}
}

void notes_structure_init() {

	for (int i = 0; i < Notes_Max; i++) {
		delete_notes(i);
	}
}

void notes_init(TJA_HEADER_T Tja_Header) {
	
	notes_structure_init();
	renda_notes_init();
	tja_notes_load();
	Command.data[0] = 0; Command.data[1] = 0; Command.data[2] = 0;
	Command.knd = 0;
	Command.val = 0;
	bpm = Tja_Header.bpm;
	offset = Tja_Header.offset;
	NotesNumber = 0;
	MeasureCount = 0;
	isNotesLoad = true;
	isJudgeDisp = false;
	JudgeMakeTime = 0;
	JudgeDispknd = -1;
	JudgeY = 70;
	isAuto = true;	//要変更
}

void notes_main(bool isDon,bool isKa, char tja_notes[Measure_Max][Max_Notes_Measure],MEASURE_T Measure[Measure_Max], int cnt, C2D_Sprite  sprites[Sprite_Number]) {

	//最初の小節のcreate_timeがマイナスだった時用に調整
	double NowTime = time_now(0) + Measure[0].create_time;

	snprintf(buf_notes, sizeof(buf_notes), "time0:%.2f", NowTime);
	debug_draw(0, 10, buf_notes);

	if ( cnt >= 0 && isNotesLoad== true) {

		if (Measure[MeasureCount].create_time <= NowTime) {

			int BarLineId = find_line_id();
			if (BarLineId != -1) {
				BarLine[BarLineId].flag = true;
				BarLine[BarLineId].scroll = Measure[MeasureCount].scroll;
				BarLine[BarLineId].x_ini =
					Notes_Judge_Range * BarLine[BarLineId].scroll + Notes_Judge;
				BarLine[BarLineId].x = BarLine[BarLineId].x_ini;
				BarLine[BarLineId].measure = MeasureCount;
				BarLine[BarLineId].create_time = NowTime;
				BarLine[BarLineId].isDisp = Measure[MeasureCount].isDispBarLine;
			}

			NotesCount = 0;

			while (isNotesLoad == true && tja_notes[Measure[MeasureCount].notes][NotesCount] != ',') {

				//命令
				if (NotesCount == 0 && tja_notes[Measure[MeasureCount].notes][0] == '#') {

					get_command_value(tja_notes[Measure[MeasureCount].notes], &Command);
					Command.notes = tja_notes[Measure[MeasureCount].notes];

					switch (Command.knd) {
					case ENd:
						isNotesLoad = false;
						break;

					default:
						break;

					}

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

					Notes[id].flag = true;
					Notes[id].notes_max = NotesCount;	//用途不明
					Notes[id].num = NotesNumber;
					Notes[id].scroll = Measure[MeasureCount].scroll;
					Notes[id].x_ini = ((Notes_Area*Measure[MeasureCount].measure / NotesCount)*i + Notes_Judge_Range)*Notes[id].scroll + Notes_Judge;
					Notes[id].x = Notes[id].x_ini;
					Notes[id].bpm = Measure[MeasureCount].bpm;
					Notes[id].knd = ctoi(tja_notes[Measure[MeasureCount].notes][i]);
					//Notes[id].create_time = NowTime;
					Notes[id].pop_time = Measure[MeasureCount].pop_time;
					Notes[id].judge_time = Measure[MeasureCount].judge_time + 60.0 / Measure[MeasureCount].bpm * 4 * Measure[MeasureCount].measure * i / NotesCount;
					Notes[id].renda_id = -1;

					int renda_id = -1;

					switch (Notes[id].knd) {

					case Renda:
						RendaState = Renda;
						renda_id = make_renda_start(Notes[id].x);
						Notes[id].renda_id = renda_id;
						break;

					case BigRenda:
						RendaState = BigRenda;
						break;

					case RendaEnd:

						switch (RendaState) {
						case Renda:
							renda_id = make_renda_finish(Notes[id].x);
							Notes[id].renda_id = renda_id;
							Notes[id].knd = RendaEnd;
							RendaState = 0;

							break;

						case BigRenda:
							Notes[id].knd = BigRendaEnd;
							break;

						default:
							Notes[id].flag = false;
							break;
						}
						RendaState = 0;
						break;
					default:
						break;
					}

					NotesNumber++;
				}
			}

			MeasureCount++;
			notes_sort();	//ソート
		}

	}

	
	for (int i = 0; i < (BarLine_Max - 1); i++) {

		if (BarLine[i].flag == true) {

			BarLine[i].x = BarLine[i].x_ini -
				Notes_Area * BarLine[i].scroll * (NowTime - Measure[BarLine[i].measure].pop_time) / (60 / Measure[BarLine[i].measure].bpm * 4);
			if (BarLine[i].isDisp == true)
				C2D_DrawRectangle(BarLine[i].x, 86, 0, 1, 46, C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1));
			
			if (BarLine[i].x < 62) BarLine[i].flag = false;
			//snprintf(buf_notes, sizeof(buf_notes), "%.1f", Measure[BarLine[i].measure].create_time);
			//debug_draw(BarLine[i].x-10, 133, buf_notes);
		}
	}

	notes_calc(isDon,isKa,bpm, NowTime, cnt, sprites);
	notes_draw(sprites);

	C2D_DrawRectangle(0 ,86, 0, 62, 58, C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1), C2D_Color32f(1,0,0,1));
	debug_draw(0, 40, tja_notes[Measure[MeasureCount-1].notes]);
	snprintf(buf_notes, sizeof(buf_notes), "%d",MeasureCount-1);
	debug_draw(0, 50, buf_notes);

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
}

int find_notes_id() {

	for (int i = 0; i < Notes_Max-1; i++) {

		if (Notes[i].flag == false) return i;
	}
	return -1;
}

int find_line_id() {

	for (int i = 0; i < BarLine_Max - 1; i++) {

		if (BarLine[i].flag == false) return i;
	}
	return -1;
}

void make_judge(int knd,double NowTime) {
	isJudgeDisp = true;
	JudgeMakeTime = NowTime;
	JudgeDispknd = knd;
	JudgeY = 73;
}

void calc_judge(double NowTime, C2D_Sprite sprites[Sprite_Number]) {

	if (isJudgeDisp == true) {

		if (NowTime - JudgeMakeTime < 0.05)  JudgeY = 73+(NowTime - JudgeMakeTime)*140;
		if (JudgeY >= 80) JudgeY = 80;

		switch (JudgeDispknd) {

		case 0:		//良
			C2D_SpriteSetPos(&sprites[jUdge_ryou], 92, JudgeY );
			C2D_DrawSprite(&sprites[jUdge_ryou]);
			break;

		case 1:		//可
			C2D_SpriteSetPos(&sprites[jUdge_ka], 92, JudgeY );
			C2D_DrawSprite(&sprites[jUdge_ka]);
			break;

		case 2:		//不可
			C2D_SpriteSetPos(&sprites[jUdge_fuka], 92, JudgeY );
			C2D_DrawSprite(&sprites[jUdge_fuka]);
			break;

		}
		//snprintf(buf_notes, sizeof(buf_notes), "%f", JudgeY);
		//debug_draw(92, JudgeY, buf_notes);
		if (NowTime - JudgeMakeTime >= 0.5) isJudgeDisp = false;
	}
}

void notes_judge(double NowTime, bool isDon, bool isKa) {
	
	int CurrentJudgeNotes[2] = { -1,-1 };		//現在判定すべきノーツ ドン,カツ
	double CurrentJudgeNotesLag[2] = { -1,-1 };	//判定すべきノーツの誤差(s)

	for (int i = 0; i < Notes_Max-1; i++) {

		if (Notes[i].flag == true) {

			if (Notes[i].knd == Don ||
				Notes[i].knd == BigDon ||
				Notes[i].knd == Renda ||
				Notes[i].knd == BigRenda ||
				Notes[i].knd == Balloon ||
				Notes[i].knd == RendaEnd ||
				Notes[i].knd == Potato) {	//ドン

				if (CurrentJudgeNotesLag[0] > fabs(Notes[i].judge_time - NowTime) ||
					CurrentJudgeNotesLag[0] == -1) {

					CurrentJudgeNotes[0] = i;
					CurrentJudgeNotesLag[0] = fabs(Notes[i].judge_time - NowTime);
				}
			}
			else if (
				Notes[i].knd == Ka ||
				Notes[i].knd == BigKa) {	//カツ

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

			if (Notes[i].flag == true && Notes[i].judge_time <= NowTime && 
				(Notes[i].knd != Renda && Notes[i].knd != BigRenda &&
				Notes[i].knd != RendaEnd)) {

				if (isSe[0] == false ||
					Notes[i].knd == Don ||
					Notes[i].knd == BigDon ||
					Notes[i].knd == Renda ||
					Notes[i].knd == BigRenda ||
					Notes[i].knd == Balloon ||
					Notes[i].knd == RendaEnd ||
					Notes[i].knd == Potato) {
					
					isSe[0] = true;
					music_play(0);
					make_judge(0, NowTime);
				}
				else if (
					isSe[1] == false ||
					Notes[i].knd == Ka ||
					Notes[i].knd == BigKa) {

					isSe[1] = true;
					music_play(1);
					make_judge(0, NowTime);
				}

				delete_notes(i);
			}
		}
	}

	else if (isAuto == false) {			//手動
		
		if (isDon == true && CurrentJudgeNotes[0] != -1) {	//ドン

			if (CurrentJudgeNotesLag[0] <= 0.034) {			//良
				make_judge(0,NowTime);
				delete_notes(CurrentJudgeNotes[0]);
			}
			else if (CurrentJudgeNotesLag[0] <= 0.117) {	//可
				make_judge(1, NowTime);
				delete_notes(CurrentJudgeNotes[0]);
			}
			else if (CurrentJudgeNotesLag[0] <= 0.150) {	//不可
				make_judge(2, NowTime);
				delete_notes(CurrentJudgeNotes[0]);
			}
		}

		if (isKa == true && CurrentJudgeNotes[1] != -1) {			//カツ

			if (CurrentJudgeNotesLag[1] <= 0.034) {			//良
				make_judge(0, NowTime);
				delete_notes(CurrentJudgeNotes[1]);
			}
			else if (CurrentJudgeNotesLag[1] <= 0.117) {	//可
				make_judge(1, NowTime);
				delete_notes(CurrentJudgeNotes[1]);
			}
			else if (CurrentJudgeNotesLag[1] <= 0.150) {	//不可
				make_judge(2, NowTime);
				delete_notes(CurrentJudgeNotes[1]);
			}
		}
	}

}

void notes_calc(bool isDon,bool isKa,double bpm, double NowTime, int cnt, C2D_Sprite sprites[Sprite_Number]) {
		
	for (int i = 0; i < Notes_Max; i++) {	//計算
		
		if (Notes[i].flag == true) {
			
			Notes[i].x = Notes[i].x_ini -
				Notes_Area* Notes[i].scroll * (NowTime - Notes[i].pop_time) / (60 / Notes[i].bpm * 4);

			switch (Notes[i].knd) {

			case Renda:
				if (Notes[i].renda_id != -1) {
					RendaNotes[Notes[i].renda_id].start_x = Notes[i].x;
					RendaNotes[Notes[i].renda_id].start_id = i;
				}
				break;

			case RendaEnd:
				if (Notes[i].renda_id != -1) {
					RendaNotes[Notes[i].renda_id].finish_x = Notes[i].x;
					RendaNotes[Notes[i].renda_id].finish_id = i;
				}
				break;

			default:
				break;
			}
		}
			
		if (Notes[i].x <= 40 && 
			Notes[i].knd != Renda && Notes[i].knd != BigRenda) delete_notes(i);
	}
	notes_judge(NowTime, isDon, isKa);
	calc_judge(NowTime, sprites);
}

void notes_draw(C2D_Sprite sprites[Sprite_Number]) {

	int notes_y = 109;

	for (int i = 0; i < Notes_Max; i++) {	//描画

		if (Notes[i].flag == true) {

			switch (Notes[i].knd) {
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
			case Renda: {
				for (int n = 0; n < (RendaNotes[Notes[i].renda_id].finish_x - RendaNotes[Notes[i].renda_id].start_x) / 9.0; n++) {
					C2D_SpriteSetPos(&sprites[rEnda_int], Notes[i].x + 9 * n, notes_y);
					C2D_DrawSprite(&sprites[rEnda_int]);
				}
				C2D_SpriteSetPos(&sprites[rEnda_start], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[rEnda_start]);
				break;
			}
			case BigRenda:
				C2D_SpriteSetPos(&sprites[bIg_renda], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_renda]);
				break;
			case Balloon:
				C2D_SpriteSetPos(&sprites[bAlloon], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bAlloon]);
				break;
			case RendaEnd:
				C2D_SpriteSetPos(&sprites[rEnda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[rEnda_fini]);
				break;
			case BigRendaEnd:
				C2D_SpriteSetPos(&sprites[bIg_renda_fini], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_renda_fini]);
				break;
			default:
				break;
			}
			snprintf(buf_notes, sizeof(buf_notes), "%d",i);
			debug_draw(Notes[i].x, 132, buf_notes);
		}
	}

}

void toggle_auto() {
	
	isAuto = !isAuto;
}