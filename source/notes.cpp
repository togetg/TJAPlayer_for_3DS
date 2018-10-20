#include "header.h"
#include "main.h"
#include "time.h"
#include "audio.h"
#include "tja.h"
#include "notes.h"
#include "score.h"
#include "option.h"

int balloon[256], BalloonCount;
double bpm, offset;
char buf_notes[160];

int find_notes_id(), find_line_id(), make_roll_start(int NotesId), make_roll_end(int NotesId), make_balloon_start(int NotesId), make_balloon_end(int NotesId);
void notes_calc(bool isDon, bool isKatsu, double bpm, double NowTime, int cnt, C2D_Sprite sprites[Sprite_Number]);
void notes_draw(C2D_Sprite sprites[Sprite_Number]), notes_sort(), delete_roll(int i), init_notes(TJA_HEADER_T TJA_Header), delete_notes(int i), make_balloon_break();
void draw_judge(double NowTime, C2D_Sprite sprites[Sprite_Number]);

NOTES_T Notes[Notes_Max];
COMMAND_T Command;
BARLINE_T BarLine[BarLine_Max];
ROLL_T RollNotes[Roll_Max];
BALLOON_T BalloonNotes[Balloon_Max];
BRANCH_T Branch;

int MeasureCount, RollState, NotesCount, JudgeDispknd, JudgeRollState, BalloonBreakCount,
NotesNumber;	//何番目のノーツか
bool  isNotesLoad = true, isJudgeDisp = false, isBalloonBreakDisp = false, isGOGOTime = false, isLevelHold = false;	//要初期化
double JudgeMakeTime, JudgeY,JudgeEffectCnt;


void notes_main(bool isDon, bool isKatsu, char tja_notes[Measure_Max][Max_Notes_Measure], MEASURE_T Measure[Measure_Max], int cnt, C2D_Sprite  sprites[Sprite_Number]) {

	OPTION_T Option;
	get_option(&Option);

	//最初の小節のcreate_timeがマイナスだった時用に調整
	double NowTime = time_now(0) + Measure[0].create_time;

	snprintf(buf_notes, sizeof(buf_notes), "time:%.2f", NowTime);
	draw_debug(0, 0, buf_notes);

	if (cnt >= 0 && isNotesLoad == true) {

		//分岐
		if (Branch.next == true) {

			while (MeasureCount < Measure_Max) {

				MeasureCount++;
				break;
			}
			Branch.next = false;
		}

		while (Measure[MeasureCount].create_time <= NowTime && Branch.wait == false) {

			NotesCount = 0;

			if (Measure[MeasureCount].branch != Branch.course && Measure[MeasureCount].branch != -1) {

				MeasureCount++;
				continue;
			}

			//ノーツ数の計測
			while (isNotesLoad == true && tja_notes[Measure[MeasureCount].notes][NotesCount] != ',' && tja_notes[Measure[MeasureCount].notes][NotesCount] != '\n' && tja_notes[Measure[MeasureCount].notes][NotesCount] != '/') {

				//生成時に発動する命令
				if (NotesCount == 0 && tja_notes[Measure[MeasureCount].notes][0] == '#' && (Measure[MeasureCount].branch == Branch.course || Measure[MeasureCount].branch == -1)) {

					get_command_value(tja_notes[Measure[MeasureCount].notes], &Command);
					Command.notes = tja_notes[Measure[MeasureCount].notes];

					switch (Command.knd) {
					case ENd:
						isNotesLoad = false;
						break;
					case BRanchstart:

						if (isLevelHold == false) {

							Branch.knd = Command.val[0];
							Branch.x = Command.val[1];
							Branch.y = Command.val[2];
							Branch.wait = true;
						}
						break;
					case BRanchend:
						Branch.course = -1;
						break;
					default:
						break;
					}

					NotesCount = 0;
					MeasureCount++;
					if (Branch.wait == true) break;
					else continue;
				}

				NotesCount++;
			}

			if (isNotesLoad == false || Branch.wait == true) break;

			//小節線
			int BarLineId = find_line_id();
			if (BarLineId != -1 && Measure[MeasureCount].branch == Branch.course) {
				BarLine[BarLineId].flag = true;
				BarLine[BarLineId].scroll = Measure[MeasureCount].scroll * Option.speed;
				BarLine[BarLineId].x = BarLine[BarLineId].x_ini;
				BarLine[BarLineId].measure = MeasureCount;
				BarLine[BarLineId].x_ini = Notes_Judge_Range * BarLine[BarLineId].scroll + Notes_Judge;
				BarLine[BarLineId].create_time = NowTime;
				BarLine[BarLineId].isDisp = Measure[MeasureCount].isDispBarLine;
			}

			int NotesCountMax;

			if (Measure[MeasureCount].firstmeasure != -1 && MeasureIdFromOriginalId(Measure[MeasureCount].firstmeasure) != -1
				) {

				NotesCountMax = Measure[MeasureIdFromOriginalId(Measure[MeasureCount].firstmeasure)].max_notes;
			}
			else {
				NotesCountMax = NotesCount;
			}

			for (int i = 0; i < NotesCount; i++) {

				int id = find_notes_id();

				if (id != -1 && ctoi(tja_notes[Measure[MeasureCount].notes][i]) != 0 && Measure[MeasureCount].branch == Branch.course) {

					int knd = ctoi(tja_notes[Measure[MeasureCount].notes][i]);

					if (knd == Potato) knd = Don;	//イモ連打はドンに置換

					if (Option.random > 0) {		//ランダム(きまぐれ,でたらめ)
						if (rand() % 100 < Option.random * 100) {
							switch (knd) {
							case Don: knd = Katsu; break;
							case Katsu: knd = Don; break;
							case BigDon: knd = BigKatsu; break;
							case BigKatsu: knd = BigDon; break;
							}
						}
					}
					if (Option.isSwap == true) {	//あべこべ
						switch (knd) {
						case Don: knd = Katsu; break;
						case Katsu: knd = Don; break;
						case BigDon: knd = BigKatsu; break;
						case BigKatsu: knd = BigDon; break;
						}
					}

					Notes[id].flag = true;
					Notes[id].notes_max = NotesCount;
					Notes[id].num = NotesNumber;
					Notes[id].scroll = Measure[MeasureCount].scroll * Option.speed;
					Notes[id].x_ini = ((Notes_Area*Measure[MeasureCount].measure / NotesCountMax)*i + Notes_Judge_Range)*Notes[id].scroll + Notes_Judge;
					Notes[id].x = Notes[id].x_ini;
					Notes[id].bpm = Measure[MeasureCount].bpm;
					Notes[id].knd = knd;
					//Notes[id].create_time = NowTime;
					Notes[id].pop_time = Measure[MeasureCount].pop_time;
					Notes[id].judge_time = Measure[MeasureCount].judge_time + 60.0 / Measure[MeasureCount].bpm * 4 * Measure[MeasureCount].measure * i / NotesCountMax;
					Notes[id].roll_id = -1;
					Notes[id].isThrough = false;

					int roll_id = -1;

					switch (Notes[id].knd) {

					case Roll:
						RollState = Roll;
						roll_id = make_roll_start(id);
						if (roll_id != -1) {
							Notes[id].roll_id = roll_id;
						}
						else {
							delete_notes(id);
						}
						break;

					case BigRoll:
						RollState = BigRoll;
						roll_id = make_roll_start(id);
						if (roll_id != -1) {
							Notes[id].roll_id = roll_id;
						}
						else {
							delete_notes(id);
						}
						break;

					case Balloon:
						RollState = Balloon;
						roll_id = make_balloon_start(id);
						if (roll_id != -1) {
							Notes[id].roll_id = roll_id;
						}
						else {
							delete_notes(id);
						}
						break;

					case RollEnd:

						switch (RollState) {
						case Roll:
							roll_id = make_roll_end(id);
							if (roll_id != -1) {
								Notes[id].roll_id = roll_id;
								Notes[id].knd = RollEnd;
								RollState = 0;
							}
							else {
								delete_notes(id);
							}
							break;

						case BigRoll:
							roll_id = make_roll_end(id);
							if (roll_id != -1) {
								Notes[id].roll_id = roll_id;
								Notes[id].knd = BigRollEnd;
								RollState = 0;
							}
							else {
								delete_notes(id);
							}
							break;

						case Balloon:
							roll_id = make_balloon_end(id);
							if (roll_id != -1) {
								Notes[id].roll_id = roll_id;
								Notes[id].knd = BalloonEnd;
							}
							else {
								delete_notes(id);
							}
							break;

						default:
							Notes[id].flag = false;
							break;
						}
						RollState = 0;
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

	for (int i = 0; i < BarLine_Max - 1; i++) {

		if (BarLine[i].flag == true) {

			BarLine[i].x = BarLine[i].x_ini -
				Notes_Area * BarLine[i].scroll * (NowTime - Measure[BarLine[i].measure].pop_time) / (60 / Measure[BarLine[i].measure].bpm * 4);

			if (BarLine[i].isDisp == true) {
				C2D_DrawRectangle(BarLine[i].x, 86, 0, 1, 46, C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1));

				//snprintf(buf_notes, sizeof(buf_notes), "%d", Measure[BarLine[i].measure].branch);
				//draw_debug(BarLine[i].x - 10, 133, buf_notes);
			}

			if (BarLine[i].x < 62) BarLine[i].flag = false;

		}
	}

	send_gogotime(isGOGOTime);

	notes_calc(isDon, isKatsu, bpm, NowTime, cnt, sprites);
	
	for (int i = 0; i < Measure_Max - 1; i++) {	//判定時に発動する命令

		if ((Measure[i].branch == Branch.course || Measure[i].branch == -1) && Measure[i].flag == true) {

			bool NotFalse = false;

			if (Measure[i].command != -1 && Measure[i].judge_time <= NowTime) {

				switch (Measure[i].command) {
				case GOgostart:
					isGOGOTime = true;
					break;
				case GOgoend:
					isGOGOTime = false;
					break;
				case SEction:
					init_branch_section();
					break;
				case BRanchstart:

					if (isLevelHold == false && (Branch.knd != 0 || JudgeRollState == -1) ) {	//連打分岐の時は連打が無くなってから分岐

						Branch.course = branch_start(Branch.knd, Branch.x, Branch.y);
						Branch.next = true;
						Branch.wait = false;
					}
					else NotFalse = true;
					break;
				case LEvelhold:
					isLevelHold = true;
					break;
				default:
					break;
				}
			}

			if (NotFalse == false && Measure[i].judge_time <= NowTime) Measure[i].flag = false;
		}
	}
	
	if (Option.isStelth == false) notes_draw(sprites);
	draw_emblem(sprites);
	draw_judge(NowTime, sprites);

	/*
	snprintf(buf_notes, sizeof(buf_notes), "cnt :%d", cnt);
	draw_debug(100, 0, buf_notes);

	snprintf(buf_notes, sizeof(buf_notes), "Bpm:%.1f     Measure:%.1f     Scroll:%.1f", Measure[MeasureCount].bpm, Measure[MeasureCount].measure, Measure[MeasureCount].scroll);
	draw_debug(0, 20, buf_notes);

	snprintf(buf_notes, sizeof(buf_notes), "%d:%.1f:%s", MeasureCount - 1,Measure[MeasureCount-1].judge_time, tja_notes[MeasureCount - 1]);
	draw_debug(0, 50, buf_notes);
	int n = 273;
	snprintf(buf_notes, sizeof(buf_notes), "%d:%s:%f:%d:%d", n, tja_notes[Measure[n].notes],Measure[n].create_time,Measure[n].branch,Branch.course);
	draw_debug(0, 30, buf_notes);
	*/

	if (Option.isAuto == true) draw_debug(0, 200, "Auto");
	else draw_debug(0, 200, "Manual");

	/*
	snprintf(buf_notes, sizeof(buf_notes), "%s", tja_notes[4]);
	draw_debug(0, 210, buf_notes);
	*/
}

int find_notes_id() {

	for (int i = 0; i < Notes_Max - 1; i++) {

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

void make_judge(int knd, double NowTime) {
	isJudgeDisp = true;
	JudgeMakeTime = NowTime;
	JudgeDispknd = knd;
	JudgeY = 73;
	JudgeEffectCnt = 0;
}

void draw_judge(double NowTime, C2D_Sprite sprites[Sprite_Number]) {

	if (isJudgeDisp == true) {

		JudgeEffectCnt++;

		C2D_ImageTint Tint;
		C2D_AlphaImageTint(&Tint, 1.0 - JudgeEffectCnt * 1.0 / 20);

		//アニメーション
		if (NowTime - JudgeMakeTime < 0.05)  JudgeY = 73 + (NowTime - JudgeMakeTime) * 140;
		if (JudgeY >= 80) JudgeY = 80;

		switch (JudgeDispknd) {

		case PERFECT:			//良
			C2D_DrawSpriteTinted(&sprites[eFfect_perfect], &Tint);
			C2D_SpriteSetPos(&sprites[jUdge_perfect], 93, JudgeY);
			C2D_DrawSprite(&sprites[jUdge_perfect]);
			break;

		case SPECIAL_PERFECT:	//特良
			C2D_DrawSpriteTinted(&sprites[eFfect_special_perfect], &Tint);
			C2D_SpriteSetPos(&sprites[jUdge_perfect], 93, JudgeY);
			C2D_DrawSprite(&sprites[jUdge_perfect]);
			break;

		case NICE:				//可
			C2D_DrawSpriteTinted(&sprites[eFfect_nice], &Tint);
			C2D_SpriteSetPos(&sprites[jUdge_nice], 93, JudgeY);
			C2D_DrawSprite(&sprites[jUdge_nice]);
			break;

		case SPECIAL_NICE:		//特可
			C2D_DrawSpriteTinted(&sprites[eFfect_special_nice], &Tint);
			C2D_SpriteSetPos(&sprites[jUdge_nice], 93, JudgeY);
			C2D_DrawSprite(&sprites[jUdge_nice]);
			break;

		case BAD:				//不可
			C2D_SpriteSetPos(&sprites[jUdge_bad], 92, JudgeY);
			C2D_DrawSprite(&sprites[jUdge_bad]);
			break;

		}
		//snprintf(buf_notes, sizeof(buf_notes), "%f", JudgeY);
		//draw_debug(92, JudgeY, buf_notes);
		if (NowTime - JudgeMakeTime >= 0.5) isJudgeDisp = false;
	}

}

void notes_judge(double NowTime, bool isDon, bool isKatsu, int cnt) {

	OPTION_T Option;
	get_option(&Option);

	int CurrentJudgeNotes[2] = { -1,-1 };		//現在判定すべきノーツ ドン,カツ
	double CurrentJudgeNotesLag[2] = { -1,-1 };	//判定すべきノーツの誤差(s)

	JudgeRollState = -1;

	//連打の状態
	for (int i = 0; i < Roll_Max - 1; i++) {

		if (RollNotes[i].flag == true &&
			Notes[RollNotes[i].start_id].judge_time < NowTime &&
			(RollNotes[i].end_id == -1 || (RollNotes[i].end_id != -1 && Notes[RollNotes[i].end_id].judge_time > NowTime))) JudgeRollState = RollNotes[i].knd;
	}

	//風船の処理
	int JudgeBalloonState = -1;
	for (int i = 0; i < Balloon_Max - 1; i++) {

		if (BalloonNotes[i].flag == true && Notes[BalloonNotes[i].start_id].judge_time <= NowTime) {
			JudgeBalloonState = i;
			break;
		}
	}

	//判定すべきノーツを検索
	for (int i = 0; i < Notes_Max - 1; i++) {

		if (Notes[i].flag == true) {

			if (Notes[i].knd == Don ||
				Notes[i].knd == BigDon ||
				Notes[i].knd == Potato) {	//ドン

				if (CurrentJudgeNotesLag[0] > fabs(Notes[i].judge_time - NowTime) ||
					CurrentJudgeNotesLag[0] == -1) {

					CurrentJudgeNotes[0] = i;
					CurrentJudgeNotesLag[0] = fabs(Notes[i].judge_time - NowTime);
				}
			}
			else if (
				Notes[i].knd == Katsu ||
				Notes[i].knd == BigKatsu) {	//カツ

				if (CurrentJudgeNotesLag[1] > fabs(Notes[i].judge_time - NowTime) ||
					CurrentJudgeNotesLag[1] == -1) {

					CurrentJudgeNotes[1] = i;
					CurrentJudgeNotesLag[1] = fabs(Notes[i].judge_time - NowTime);
				}
			}
		}
	}

	if (Option.isAuto == true) {	//オート

		for (int i = 0; i < Notes_Max - 1; i++) {

			if (Notes[i].flag == true && Notes[i].judge_time <= NowTime &&
				(Notes[i].knd != Roll && Notes[i].knd != BigRoll && Notes[i].knd != BigRollEnd &&
					Notes[i].knd != RollEnd && Notes[i].knd != Balloon && Notes[i].knd != BalloonEnd)) {

				if (Notes[i].knd == Don ||
					Notes[i].knd == Potato) {

					music_play(0);
					make_judge(PERFECT, NowTime);
				}
				else if (Notes[i].knd == BigDon) {
					music_play(0);
					make_judge(SPECIAL_PERFECT, NowTime);
				}
				else if (Notes[i].knd == Katsu) {
					music_play(1);
					make_judge(PERFECT, NowTime);
				}
				else if (Notes[i].knd == BigKatsu) {
					music_play(1);
					make_judge(SPECIAL_PERFECT, NowTime);
				}

				if (Notes[i].knd == BigDon || Notes[i].knd == BigKatsu) score_update(SPECIAL_PERFECT);
				else if (Notes[i].knd == Don || Notes[i].knd == Katsu) score_update(PERFECT);

				delete_notes(i);
			}
		}

		if (JudgeRollState != -1) {	//連打

			if (cnt % 5 == 0) {

				if (JudgeRollState == Roll) score_update(ROLL);
				else if (JudgeRollState == BigRoll) score_update(BIG_ROLL);

				music_play(0);
			}
		}

		if (JudgeBalloonState != -1) {	//風船

			if (cnt % 4 == 0) {

				music_play(0);
				BalloonNotes[JudgeBalloonState].current_hit++;

				if (BalloonNotes[JudgeBalloonState].current_hit >= BalloonNotes[JudgeBalloonState].need_hit) {

					score_update(BALLOON_BREAK);	//破裂
					make_balloon_break();
				}
				else score_update(BALLOON);
			}
		}
	}

	else if (Option.isAuto == false) {			//手動

		bool isBig;
		if (Notes[CurrentJudgeNotes[0]].knd == BigDon || Notes[CurrentJudgeNotes[0]].knd == BigKatsu) isBig = true;
		else isBig = false;

		if (isDon == true && CurrentJudgeNotes[0] != -1) {	//ドン

			if (CurrentJudgeNotesLag[0] <= 0.034) {			//良
				delete_notes(CurrentJudgeNotes[0]);
				if (isBig == true) {
					make_judge(SPECIAL_PERFECT, NowTime);
					score_update(SPECIAL_PERFECT);
				}
				else {
					make_judge(PERFECT, NowTime);
					score_update(PERFECT);
				}
			}
			else if (CurrentJudgeNotesLag[0] <= 0.117) {	//可
				make_judge(1, NowTime);
				delete_notes(CurrentJudgeNotes[0]);
				if (isBig == true) {
					make_judge(SPECIAL_NICE, NowTime);
					score_update(SPECIAL_NICE);
				}
				else {
					make_judge(NICE, NowTime);
					score_update(NICE);
				}
			}
			else if (CurrentJudgeNotesLag[0] <= 0.150) {	//不可
				make_judge(BAD, NowTime);
				delete_notes(CurrentJudgeNotes[0]);
				score_update(BAD);
			}
		}

		if (isKatsu == true && CurrentJudgeNotes[1] != -1) {	//カツ

			if (CurrentJudgeNotesLag[1] <= 0.034) {			//良
				delete_notes(CurrentJudgeNotes[1]);
				if (isBig == true) {
					make_judge(SPECIAL_PERFECT, NowTime);
					score_update(SPECIAL_PERFECT);
				}
				else {
					make_judge(PERFECT, NowTime);
					score_update(PERFECT);
				}
			}
			else if (CurrentJudgeNotesLag[1] <= 0.117) {	//可
				make_judge(1, NowTime);
				delete_notes(CurrentJudgeNotes[1]);
				if (isBig == true) {
					make_judge(SPECIAL_NICE, NowTime);
					score_update(SPECIAL_NICE);
				}
				else {
					make_judge(NICE, NowTime);
					score_update(NICE);
				}
			}
			else if (CurrentJudgeNotesLag[1] <= 0.150) {	//不可
				make_judge(BAD, NowTime);
				delete_notes(CurrentJudgeNotes[1]);
				score_update(BAD);
			}
		}

		if (JudgeRollState != -1) {	//連打

			if (isDon == true) {
				if (JudgeRollState == Roll) score_update(ROLL);
				else if (JudgeRollState == BigRoll) score_update(BIG_ROLL);
			}
			if (isKatsu == true) {
				if (JudgeRollState == Roll) score_update(ROLL);
				else if (JudgeRollState == BigRoll) score_update(BIG_ROLL);
			}
		}

		if (JudgeBalloonState != -1 && isDon == true) {	//風船

			BalloonNotes[JudgeBalloonState].current_hit++;

			if (BalloonNotes[JudgeBalloonState].current_hit >= BalloonNotes[JudgeBalloonState].need_hit) {

				score_update(BALLOON_BREAK);	//破裂
				make_balloon_break();
			}
			else score_update(BALLOON);
		}
	}

	//風船の消去処理
	if (JudgeBalloonState != -1 && BalloonNotes[JudgeBalloonState].current_hit >= BalloonNotes[JudgeBalloonState].need_hit) {

		if (BalloonNotes[JudgeBalloonState].end_id != -1) delete_notes(BalloonNotes[JudgeBalloonState].end_id);
		else delete_notes(BalloonNotes[JudgeBalloonState].start_id);

		music_play(2);
		balloon_count_update(0);
	}
}

void notes_calc(bool isDon, bool isKatsu, double bpm, double NowTime, int cnt, C2D_Sprite sprites[Sprite_Number]) {

	for (int i = 0; i < Notes_Max - 1; i++) {	//計算

		if (Notes[i].flag == true) {

			Notes[i].x = Notes[i].x_ini -
				Notes_Area * Notes[i].scroll * (NowTime - Notes[i].pop_time) / (60 / Notes[i].bpm * 4);

			switch (Notes[i].knd) {

			case Roll:
				if (Notes[i].roll_id != -1 && RollNotes[Notes[i].roll_id].flag == true) {
					RollNotes[Notes[i].roll_id].start_x = Notes[i].x;
					RollNotes[Notes[i].roll_id].start_id = i;
				}
				break;

			case RollEnd:
				if (Notes[i].roll_id != -1 && RollNotes[Notes[i].roll_id].flag == true) {
					RollNotes[Notes[i].roll_id].end_x = Notes[i].x;
					RollNotes[Notes[i].roll_id].end_id = i;
				}
				break;

			case BigRoll:
				if (Notes[i].roll_id != -1 && RollNotes[Notes[i].roll_id].flag == true) {
					RollNotes[Notes[i].roll_id].start_x = Notes[i].x;
					RollNotes[Notes[i].roll_id].start_id = i;
				}
				break;

			case BigRollEnd:
				if (Notes[i].roll_id != -1 && RollNotes[Notes[i].roll_id].flag == true) {
					RollNotes[Notes[i].roll_id].end_x = Notes[i].x;
					RollNotes[Notes[i].roll_id].end_id = i;
				}
				break;

			case Balloon:
				if (Notes[i].x <= Notes_Judge) Notes[i].x = Notes_Judge;
				if (Notes[i].roll_id != -1) {
					BalloonNotes[Notes[i].roll_id].start_id = i;
				}
				break;

			case BalloonEnd:
				if (Notes[i].roll_id != -1) {
					BalloonNotes[Notes[i].roll_id].end_id = i;
				}
				if (Notes[i].judge_time <= NowTime) {
					delete_notes(i);
				}
				break;

			case Don:
			case Katsu:
			case BigDon:
			case BigKatsu:
				if (NowTime - Notes[i].judge_time > 0.150 && Notes[i].isThrough == false) {
					score_update(THROUGH);
					Notes[i].isThrough = true;
				}
				break;

			default:
				break;
			}
		}
	}

	for (int i = 0; i < Notes_Max - 1; i++) {	//連打のバグ回避のためノーツの削除は一番最後

		if (Notes[i].flag == true &&
			Notes[i].x <= 20 &&
			Notes[i].knd != Roll && Notes[i].knd != BigRoll) {

			if (Notes[i].isThrough == false && 
				(Notes[i].knd == Don || Notes[i].knd == Katsu || Notes[i].knd == BigDon || Notes[i].knd == BigKatsu)) {
				score_update(THROUGH);
				Notes[i].isThrough = true;
			}
			delete_notes(i);
		}
	}

	notes_judge(NowTime, isDon, isKatsu, cnt);
}

void notes_draw(C2D_Sprite sprites[Sprite_Number]) {

	int notes_y = 109;

	for (int i = 0; i < Notes_Max - 1; i++) {	//描画

		if (Notes[i].flag == true) {

			switch (Notes[i].knd) {
			case Don:
				C2D_SpriteSetPos(&sprites[dOn], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[dOn]);
				break;
			case Katsu:
				C2D_SpriteSetPos(&sprites[kAtsu], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[kAtsu]);
				break;
			case BigDon:
				C2D_SpriteSetPos(&sprites[bIg_don], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_don]);
				break;
			case BigKatsu:
				C2D_SpriteSetPos(&sprites[bIg_katsu], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_katsu]);
				break;
			case Roll: {

				if (RollNotes[Notes[i].roll_id].flag == true) {

					double end_x;
					if (RollNotes[Notes[i].roll_id].end_id == -1) end_x = TOP_WIDTH;
					else end_x = RollNotes[Notes[i].roll_id].end_x;

					for (int n = 0; n < (end_x - RollNotes[Notes[i].roll_id].start_x) / 9.0; n++) {
						C2D_SpriteSetPos(&sprites[rOll_int], Notes[i].x + 9 * n, notes_y);
						C2D_DrawSprite(&sprites[rOll_int]);
					}
					C2D_SpriteSetPos(&sprites[rOll_start], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[rOll_start]);
				}
				break;
			}
			case BigRoll: {

				if (RollNotes[Notes[i].roll_id].flag == true) {

					double end_x;
					if (RollNotes[Notes[i].roll_id].end_id == -1) end_x = TOP_WIDTH;
					else end_x = RollNotes[Notes[i].roll_id].end_x;

					for (int n = 0; n < (end_x - RollNotes[Notes[i].roll_id].start_x) / 9.0; n++) {
						C2D_SpriteSetPos(&sprites[bIg_roll_int], Notes[i].x + 9 * n, notes_y);
						C2D_DrawSprite(&sprites[bIg_roll_int]);
					}

					C2D_SpriteSetPos(&sprites[bIg_roll_start], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[bIg_roll_start]);
					break;
				}
			}
			case Balloon:

				if (BalloonNotes[Notes[i].roll_id].current_hit == 0) {

					C2D_SpriteSetPos(&sprites[bAlloon], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[bAlloon]);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit * 1 / 5) {

					C2D_SpriteSetPos(&sprites[bAlloon_1], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[bAlloon_1]);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit * 2 / 5) {

					C2D_SpriteSetPos(&sprites[bAlloon_2], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[bAlloon_2]);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit * 3 / 5) {

					C2D_SpriteSetPos(&sprites[bAlloon_3], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[bAlloon_3]);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit * 4 / 5) {

					C2D_SpriteSetPos(&sprites[bAlloon_4], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[bAlloon_4]);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit) {

					C2D_SpriteSetPos(&sprites[bAlloon_5], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[bAlloon_5]);
				}

				if (BalloonNotes[Notes[i].roll_id].current_hit >= 1) balloon_count_update(BalloonNotes[Notes[i].roll_id].need_hit - BalloonNotes[Notes[i].roll_id].current_hit);

				//snprintf(buf_notes, sizeof(buf_notes), "%d", BalloonNotes[Notes[i].roll_id].need_hit - BalloonNotes[Notes[i].roll_id].current_hit);
				//draw_debug(Notes[i].x, 132, buf_notes);
				break;
			case RollEnd:
				C2D_SpriteSetPos(&sprites[rOll_end], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[rOll_end]);
				break;
			case BigRollEnd:
				C2D_SpriteSetPos(&sprites[bIg_roll_end], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[bIg_roll_end]);
				break;
			default:
				break;
			}
			//snprintf(buf_notes, sizeof(buf_notes), "%d", i);
			//draw_debug(Notes[i].x, 132, buf_notes);
		}
	}

	//割れた風船
	if (isBalloonBreakDisp == true) {
		BalloonBreakCount--;
		C2D_ImageTint Tint;
		C2D_AlphaImageTint(&Tint, BalloonBreakCount / 40.0);
		C2D_SpriteSetPos(&sprites[bAlloon_6], Notes_Judge, notes_y);
		C2D_DrawSpriteTinted(&sprites[bAlloon_6], &Tint);
	}
	if (BalloonBreakCount <= 0) isBalloonBreakDisp = false;

}

int get_branch_course() {

	return Branch.course;
}

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
	default: return 0;
	}
}

int notes_cmp(const void *p, const void *q) {	//比較用

	int pp = ((NOTES_T*)p)->judge_time * 10000;
	int qq = ((NOTES_T*)q)->judge_time * 10000;

	//if (((NOTES_T*)p)->flag == false) pp = INT_MAX;
	//if (((NOTES_T*)p)->flag == false) qq = INT_MAX;

	return qq - pp;
}

void notes_sort() {	//ノーツを出現順にソート
	int n = sizeof Notes / sizeof(NOTES_T);
	qsort(Notes, n, sizeof(NOTES_T), notes_cmp);
}

void delete_roll(int i) {

	if (i >= 0 && i < Roll_Max) {
		if (RollNotes[i].start_id != -1 && Notes[RollNotes[i].start_id].flag == true) delete_notes(RollNotes[i].start_id);
		RollNotes[i].id = -1;
		RollNotes[i].start_x = -1;
		RollNotes[i].start_id = -1;
		RollNotes[i].end_x = -1;
		RollNotes[i].end_id = -1;
		RollNotes[i].flag = false;
		RollNotes[i].knd = -1;
	}
}

void init_roll__notes() {

	for (int i = 0; i < Roll_Max - 1; i++) {
		delete_roll(i);
	}
}

int find_roll_id() {

	for (int i = 0; i < Roll_Max - 1; i++) {

		if (RollNotes[i].flag == false) return i;
	}
	return -1;
}

int make_roll_start(int NotesId) {

	int id = find_roll_id();
	if (id != -1) {

		RollNotes[id].id = id;
		RollNotes[id].start_x = Notes[NotesId].x;
		RollNotes[id].knd = Notes[NotesId].knd;
		RollNotes[id].end_x = -1;
		RollNotes[id].flag = true;
		return id;
	}
	else return -1;
}

int find_roll_end_id() {	//startの値だけ入ってる連打idを返す

	for (int i = 0; i < Roll_Max - 1; i++) {

		if (RollNotes[i].flag == true &&
			RollNotes[i].start_x != -1 &&
			RollNotes[i].end_x == -1) return i;
	}
	return -1;
}

int make_roll_end(int NotesId) {

	int id = find_roll_end_id();
	if (id != -1) {

		RollNotes[id].end_x = Notes[NotesId].x;
		return id;
	}
	else return -1;
}

void make_balloon_break() {

	isBalloonBreakDisp = true;
	BalloonBreakCount = 40;
}

void delete_balloon(int i) {

	if (i >= 0 && i < Balloon_Max) {
		BalloonNotes[i].id = -1;
		BalloonNotes[i].start_id = -1;
		BalloonNotes[i].end_id = -1;
		BalloonNotes[i].need_hit = 5;
		BalloonNotes[i].current_hit = 0;
		BalloonNotes[i].flag = false;
	}
}

void init_balloon_notes() {

	for (int i = 0; i < Balloon_Max - 1; i++) {
		delete_balloon(i);
	}
}

int find_balloon_id() {

	for (int i = 0; i < Balloon_Max - 1; i++) {

		if (BalloonNotes[i].flag == false) return i;
	}
	return -1;
}

int make_balloon_start(int NotesId) {

	int id = find_balloon_id();
	if (id != -1) {

		BalloonNotes[id].id = id;
		BalloonNotes[id].start_id = NotesId;
		BalloonNotes[id].end_id = -1;
		if (balloon[BalloonCount] != 0) BalloonNotes[id].need_hit = balloon[BalloonCount];
		else  BalloonNotes[id].need_hit = 5;
		BalloonNotes[id].current_hit = 0;
		BalloonNotes[id].flag = true;
		BalloonCount++;
		return id;
	}
	else return -1;
}

int find_balloon_end_id() {	//startの値だけ入ってる連打idを返す

	for (int i = 0; i < Balloon_Max - 1; i++) {

		if (BalloonNotes[i].flag == true &&
			BalloonNotes[i].start_id != -1 &&
			BalloonNotes[i].end_id == -1) return i;
	}
	return -1;
}

int make_balloon_end(int NotesId) {

	int id = find_balloon_end_id();
	if (id != -1) return id;
	else return -1;
}

void delete_notes(int i) {

	if (i >= 0 &&
		Notes[i].roll_id != -1 &&
		(Notes[i].knd == RollEnd || Notes[i].knd == BigRollEnd) &&
		RollNotes[Notes[i].roll_id].flag == true
		) {	//連打削除

		delete_notes(RollNotes[Notes[i].roll_id].start_id);
		delete_roll(Notes[i].roll_id);
		score_update(ROLL_END);

	}

	if (i >= 0 &&
		Notes[i].roll_id != -1 &&
		BalloonNotes[Notes[i].roll_id].flag == true) {					//風船削除

		if (Notes[i].knd == BalloonEnd) {

			if (BalloonNotes[Notes[i].roll_id].start_id != -1) delete_notes(BalloonNotes[Notes[i].roll_id].start_id);
			delete_balloon(Notes[i].roll_id);
			balloon_count_update(0);
		}
		else if (Notes[i].knd == Balloon) {

			BalloonNotes[Notes[i].roll_id].start_id = -1;

			if (BalloonNotes[Notes[i].roll_id].end_id == -1) {

				delete_balloon(Notes[i].roll_id);
			}
		}
	}

	if (i >= 0 && i < Notes_Max) {
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
		Notes[i].roll_id = -1;
		Notes[i].isThrough = false;
	}
}
bool get_notes_finish() {

	if (isNotesLoad == true) return false;
	for (int i = 0; i < Notes_Max - 1; i++) {

		if (Notes[i].flag == true) return false;
	}
	return true;
}
void init_notes_structure() {

	for (int i = 0; i < Notes_Max - 1; i++) {
		delete_notes(i);
	}
}

void init_notes(TJA_HEADER_T TJA_Header) {

	init_notes_structure();
	init_roll__notes();
	init_balloon_notes();
	Command.data[0] = 0; Command.data[1] = 0; Command.data[2] = 0;
	Command.knd = 0;
	Command.val[0] = 0;
	Command.val[1] = 0;
	Command.val[2] = 0;
	bpm = TJA_Header.bpm;
	offset = TJA_Header.offset;
	for (int i = 0; i < (int)(sizeof(balloon) / sizeof(balloon[0])); i++) {
		balloon[i] = TJA_Header.balloon[i];
	}
	NotesNumber = 0;
	NotesCount = 0;
	RollState = 0;
	MeasureCount = 0;
	isNotesLoad = true;
	isJudgeDisp = false;
	JudgeMakeTime = 0;
	JudgeDispknd = -1;
	JudgeY = 70;
	JudgeRollState = -1;
	//isAuto = true;	//要変更
	BalloonCount = 0;
	BalloonBreakCount = 0;
	isBalloonBreakDisp = false;
	isGOGOTime = false;
	Branch.knd = 0;
	Branch.x = 0;
	Branch.y = 0;
	Branch.course = -1;
	Branch.next = false;
	Branch.wait = false;
	isLevelHold = false;
}