#include "header.h"
#include "main.h"
#include "time.h"
#include "audio.h"
#include "tja.h"
#include "notes.h"
#include "score.h"
#include "option.h"

#define AUTO_ROLL_FRAME 2 //オート時の連打の間隔

int balloon[256], BalloonCount;
double bpm, offset;

int find_notes_id(), find_line_id(), make_roll_start(int NotesId), make_roll_end(int NotesId), make_balloon_start(int NotesId), make_balloon_end(int NotesId);
void notes_calc(bool isDon, bool isKatsu, double bpm, double CurrentTime, int cnt, C2D_Sprite sprites[Sprite_Number]);
void notes_draw(C2D_Sprite sprites[Sprite_Number]), notes_sort(), delete_roll(int i), init_notes(TJA_HEADER_T TJA_Header), delete_notes(int i), make_balloon_break();
void draw_judge(double CurrentTime, C2D_Sprite sprites[Sprite_Number]);

NOTES_T Notes[Notes_Max];
COMMAND_T Command;
BARLINE_T BarLine[BarLine_Max];
ROLL_T RollNotes[Roll_Max];
BALLOON_T BalloonNotes[Balloon_Max];
BRANCH_T Branch;

int MeasureCount, RollState, NotesCount, JudgeDispknd, JudgeRollState, BalloonBreakCount, PreNotesKnd,
NotesNumber;	//何番目のノーツか
bool  isNotesLoad = true, isJudgeDisp = false, isBalloonBreakDisp = false, isGOGOTime = false, isLevelHold = false;	//要初期化
double JudgeMakeTime, JudgeY,JudgeEffectCnt;


void notes_main(bool isDon, bool isKatsu, char tja_notes[Measure_Max][Max_Notes_Measure], MEASURE_T Measure[Measure_Max], int cnt, C2D_Sprite  sprites[Sprite_Number]) {

	//snprintf(get_buffer(), BUFFER_SIZE, "400");
	//draw_debug(400-20, 0, get_buffer());

	OPTION_T Option;
	get_option(&Option);

	//最初の小節のcreate_timeがマイナスだった時用に調整
	double CurrentTime = get_current_time(0) + Measure[0].create_time;	

	if (cnt >= 0 && isNotesLoad == true) {

		//分岐
		if (Branch.next == true) {

			while (MeasureCount < Measure_Max) {

				MeasureCount++;
				break;
			}
			Branch.next = false;
		}

		while (Measure[MeasureCount].create_time <= CurrentTime && Branch.wait == false) {

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
					case COMMAND_END:
						isNotesLoad = false;
						break;
					case COMMAND_BRANCHSTART:

						if (isLevelHold == false) {

							Branch.knd = Command.val[0];
							Branch.x = Command.val[1];
							Branch.y = Command.val[2];
							Branch.wait = true;
						}
						break;
					case COMMAND_BRANCHEND:
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
				BarLine[BarLineId].create_time = CurrentTime;
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

					if ((knd == NOTES_ROLL || knd == NOTES_BIGROLL) && (PreNotesKnd == NOTES_ROLL || PreNotesKnd == NOTES_BIGROLL )) {	//55558のような表記に対応
						
						continue;
					}

					if (knd == NOTES_POTATO) knd = NOTES_DON;	//イモ連打はドンに置換

					if (Option.random > 0) {		//ランダム(きまぐれ,でたらめ)
						if (rand() % 100 < Option.random * 100) {
							switch (knd) {
							case NOTES_DON: knd = NOTES_KATSU; break;
							case NOTES_KATSU: knd = NOTES_DON; break;
							case NOTES_BIGDON: knd = NOTES_BIGKATSU; break;
							case NOTES_BIGKATSU: knd = NOTES_BIGDON; break;
							}
						}
					}
					if (Option.isSwap == true) {	//あべこべ
						switch (knd) {
						case NOTES_DON: knd = NOTES_KATSU; break;
						case NOTES_KATSU: knd = NOTES_DON; break;
						case NOTES_BIGDON: knd = NOTES_BIGKATSU; break;
						case NOTES_BIGKATSU: knd = NOTES_BIGDON; break;
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
					//Notes[id].create_time = CurrentTime;
					Notes[id].pop_time = Measure[MeasureCount].pop_time;
					Notes[id].judge_time = Measure[MeasureCount].judge_time + 60.0 / Measure[MeasureCount].bpm * 4 * Measure[MeasureCount].measure * i / NotesCountMax;
					Notes[id].roll_id = -1;
					Notes[id].isThrough = false;

					PreNotesKnd = knd;

					int roll_id = -1;

					switch (Notes[id].knd) {

					case NOTES_ROLL:
						RollState = NOTES_ROLL;
						roll_id = make_roll_start(id);
						if (roll_id != -1) {
							Notes[id].roll_id = roll_id;
						}
						else {
							delete_notes(id);
						}
						break;

					case NOTES_BIGROLL:
						RollState = NOTES_BIGROLL;
						roll_id = make_roll_start(id);
						if (roll_id != -1) {
							Notes[id].roll_id = roll_id;
						}
						else {
							delete_notes(id);
						}
						break;

					case NOTES_BALLOON:
						RollState = NOTES_BALLOON;
						roll_id = make_balloon_start(id);
						if (roll_id != -1) {
							Notes[id].roll_id = roll_id;
						}
						else {
							delete_notes(id);
						}
						break;

					case NOTES_ROLLEND:

						switch (RollState) {
						case NOTES_ROLL:
							roll_id = make_roll_end(id);
							if (roll_id != -1) {
								Notes[id].roll_id = roll_id;
								Notes[id].knd = NOTES_ROLLEND;
								RollState = 0;
							}
							else {
								delete_notes(id);
							}
							break;

						case NOTES_BIGROLL:
							roll_id = make_roll_end(id);
							if (roll_id != -1) {
								Notes[id].roll_id = roll_id;
								Notes[id].knd = NOTES_BIGROLLEND;
								RollState = 0;
							}
							else {
								delete_notes(id);
							}
							break;

						case NOTES_BALLOON:
							roll_id = make_balloon_end(id);
							if (roll_id != -1) {
								Notes[id].roll_id = roll_id;
								Notes[id].knd = NOTES_BALLOONEND;
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
				else if (ctoi(tja_notes[Measure[MeasureCount].notes][i]) == 0){
				PreNotesKnd = 0;
				}
			}

			MeasureCount++;
			notes_sort();	//ソート
		}

	}

	for (int i = 0; i < BarLine_Max - 1; i++) {

		if (BarLine[i].flag == true) {

			BarLine[i].x = BarLine[i].x_ini -
				Notes_Area * BarLine[i].scroll * (CurrentTime - Measure[BarLine[i].measure].pop_time) / (60 / Measure[BarLine[i].measure].bpm * 4);

			if (BarLine[i].isDisp == true) {
				C2D_DrawRectangle(BarLine[i].x, 86, 0, 1, 46, C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1), C2D_Color32f(1, 1, 1, 1));

				//snprintf(buf_notes, sizeof(buf_notes), "%d", Measure[BarLine[i].measure].branch);
				//draw_debug(BarLine[i].x - 10, 133, buf_notes);
			}

			if (BarLine[i].x < 62) BarLine[i].flag = false;

		}
	}

	send_gogotime(isGOGOTime);

	if (get_isPause() == false) notes_calc(isDon, isKatsu, bpm, CurrentTime, cnt, sprites);
	
	for (int i = 0; i < Measure_Max - 1; i++) {	//判定時に発動する命令

		if ((Measure[i].branch == Branch.course || Measure[i].branch == -1) && Measure[i].flag == true) {

			bool NotFalse = false;

			if (Measure[i].command != -1 && Measure[i].judge_time <= CurrentTime) {

				switch (Measure[i].command) {
				case COMMAND_GOGOSTART:
					isGOGOTime = true;
					break;
				case COMMAND_GOGOEND:
					isGOGOTime = false;
					break;
				case COMMAND_SECTION:
					init_branch_section();
					break;
				case COMMAND_BRANCHSTART:

					if (isLevelHold == false && (Branch.knd != 0 || JudgeRollState == -1) ) {	//連打分岐の時は連打が無くなってから分岐

						Branch.course = branch_start(Branch.knd, Branch.x, Branch.y);
						Branch.next = true;
						Branch.wait = false;
					}
					else NotFalse = true;
					break;
				case COMMAND_LEVELHOLD:
					isLevelHold = true;
					break;
				default:
					break;
				}
			}

			if (NotFalse == false && Measure[i].judge_time <= CurrentTime) Measure[i].flag = false;
		}
	}
	
	if (Option.isStelth == false) notes_draw(sprites);
	draw_emblem(sprites);
	draw_judge(CurrentTime, sprites);

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

	if (Option.isAuto == true) draw_debug(0, 200, "Auto");
	else draw_debug(0, 200, "Manual");

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

void make_judge(int knd, double CurrentTime) {
	isJudgeDisp = true;
	JudgeMakeTime = CurrentTime;
	JudgeDispknd = knd;
	JudgeY = 73;
	JudgeEffectCnt = 0;
}

void draw_judge(double CurrentTime, C2D_Sprite sprites[Sprite_Number]) {

	if (isJudgeDisp == true) {

		JudgeEffectCnt++;

		C2D_ImageTint Tint;
		C2D_AlphaImageTint(&Tint, 1.0 - JudgeEffectCnt * 1.0 / 20);

		//アニメーション
		if (CurrentTime - JudgeMakeTime < 0.05)  JudgeY = 73 + (CurrentTime - JudgeMakeTime) * 140;
		if (JudgeY >= 80) JudgeY = 80;

		switch (JudgeDispknd) {

		case PERFECT:			//良
			C2D_DrawSpriteTinted(&sprites[SPRITE_EFFECT_PERFECT], &Tint);
			C2D_SpriteSetPos(&sprites[SPRITE_JUDGE_PERFECT], 93, JudgeY);
			C2D_DrawSprite(&sprites[SPRITE_JUDGE_PERFECT]);
			break;

		case SPECIAL_PERFECT:	//特良
			C2D_DrawSpriteTinted(&sprites[SPRITE_EFFECT_SPECIAL_PERFECT], &Tint);
			C2D_SpriteSetPos(&sprites[SPRITE_JUDGE_PERFECT], 93, JudgeY);
			C2D_DrawSprite(&sprites[SPRITE_JUDGE_PERFECT]);
			break;

		case NICE:				//可
			C2D_DrawSpriteTinted(&sprites[SPRITE_EFFECT_NICE], &Tint);
			C2D_SpriteSetPos(&sprites[SPRITE_JUDGE_NICE], 93, JudgeY);
			C2D_DrawSprite(&sprites[SPRITE_JUDGE_NICE]);
			break;

		case SPECIAL_NICE:		//特可
			C2D_DrawSpriteTinted(&sprites[SPRITE_EFFECT_SPECIAL_NICE], &Tint);
			C2D_SpriteSetPos(&sprites[SPRITE_JUDGE_NICE], 93, JudgeY);
			C2D_DrawSprite(&sprites[SPRITE_JUDGE_NICE]);
			break;

		case BAD:				//不可
			C2D_SpriteSetPos(&sprites[SPRITE_JUDGE_BAD], 92, JudgeY);
			C2D_DrawSprite(&sprites[SPRITE_JUDGE_BAD]);
			break;

		}
		//snprintf(buf_notes, sizeof(buf_notes), "%f", JudgeY);
		//draw_debug(92, JudgeY, buf_notes);
		if (CurrentTime - JudgeMakeTime >= 0.5) isJudgeDisp = false;
	}

}

void notes_judge(double CurrentTime, bool isDon, bool isKatsu, int cnt) {

	OPTION_T Option;
	get_option(&Option);

	int CurrentJudgeNotes[2] = { -1,-1 };		//現在判定すべきノーツ ドン,カツ
	double CurrentJudgeNotesLag[2] = { -1,-1 };	//判定すべきノーツの誤差(s)

	JudgeRollState = -1;

	//連打の状態
	for (int i = 0; i < Roll_Max - 1; i++) {

		if (RollNotes[i].flag == true &&
			Notes[RollNotes[i].start_id].judge_time < CurrentTime &&
			(RollNotes[i].end_id == -1 || (RollNotes[i].end_id != -1 && Notes[RollNotes[i].end_id].judge_time > CurrentTime))) JudgeRollState = RollNotes[i].knd;
	}

	//風船の処理
	int JudgeBalloonState = -1;
	for (int i = 0; i < Balloon_Max - 1; i++) {

		if (BalloonNotes[i].flag == true && Notes[BalloonNotes[i].start_id].judge_time <= CurrentTime) {
			JudgeBalloonState = i;
			break;
		}
	}

	//判定すべきノーツを検索
	for (int i = 0; i < Notes_Max - 1; i++) {

		if (Notes[i].flag == true) {

			if (Notes[i].knd == NOTES_DON ||
				Notes[i].knd == NOTES_BIGDON ||
				Notes[i].knd == NOTES_POTATO) {	//ドン

				if (CurrentJudgeNotesLag[0] > fabs(Notes[i].judge_time - CurrentTime) ||
					CurrentJudgeNotesLag[0] == -1) {

					CurrentJudgeNotes[0] = i;
					CurrentJudgeNotesLag[0] = fabs(Notes[i].judge_time - CurrentTime);
				}
			}
			else if (
				Notes[i].knd == NOTES_KATSU ||
				Notes[i].knd == NOTES_BIGKATSU) {	//カツ

				if (CurrentJudgeNotesLag[1] > fabs(Notes[i].judge_time - CurrentTime) ||
					CurrentJudgeNotesLag[1] == -1) {

					CurrentJudgeNotes[1] = i;
					CurrentJudgeNotesLag[1] = fabs(Notes[i].judge_time - CurrentTime);
				}
			}
		}
	}

	if (Option.isAuto == true) {	//オート

		for (int i = 0; i < Notes_Max - 1; i++) {

			if (Notes[i].flag == true && Notes[i].judge_time <= CurrentTime &&
				(Notes[i].knd != NOTES_ROLL && Notes[i].knd != NOTES_BIGROLL && Notes[i].knd != NOTES_BIGROLLEND &&
					Notes[i].knd != NOTES_ROLLEND && Notes[i].knd != NOTES_BALLOON && Notes[i].knd != NOTES_BALLOONEND)) {

				if (Notes[i].knd == NOTES_DON ||
					Notes[i].knd == NOTES_POTATO) {

					music_play(SOUND_DON);
					make_judge(PERFECT, CurrentTime);
				}
				else if (Notes[i].knd == NOTES_BIGDON) {
					music_play(SOUND_DON);
					make_judge(SPECIAL_PERFECT, CurrentTime);
				}
				else if (Notes[i].knd == NOTES_KATSU) {
					music_play(SOUND_KATSU);
					make_judge(PERFECT, CurrentTime);
				}
				else if (Notes[i].knd == NOTES_BIGKATSU) {
					music_play(SOUND_KATSU);
					make_judge(SPECIAL_PERFECT, CurrentTime);
				}

				if (Notes[i].knd == NOTES_BIGDON || Notes[i].knd == NOTES_BIGKATSU) score_update(SPECIAL_PERFECT);
				else if (Notes[i].knd == NOTES_DON || Notes[i].knd == NOTES_KATSU) score_update(PERFECT);

				delete_notes(i);
			}
		}

		if (JudgeRollState != -1) {	//連打

			if (cnt % AUTO_ROLL_FRAME == 0) {

				if (JudgeRollState == NOTES_ROLL) score_update(ROLL);
				else if (JudgeRollState == NOTES_BIGROLL) score_update(BIG_ROLL);

				music_play(SOUND_DON);
			}
		}

		if (JudgeBalloonState != -1) {	//風船

			if (cnt % AUTO_ROLL_FRAME == 0) {

				music_play(SOUND_DON);
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
		if (Notes[CurrentJudgeNotes[0]].knd == NOTES_BIGDON || Notes[CurrentJudgeNotes[0]].knd == NOTES_BIGKATSU) isBig = true;
		else isBig = false;

		if (isDon == true && CurrentJudgeNotes[0] != -1) {	//ドン

			if (CurrentJudgeNotesLag[0] <= 0.034) {			//良
				delete_notes(CurrentJudgeNotes[0]);
				if (isBig == true) {
					make_judge(SPECIAL_PERFECT, CurrentTime);
					score_update(SPECIAL_PERFECT);
				}
				else {
					make_judge(PERFECT, CurrentTime);
					score_update(PERFECT);
				}
			}
			else if (CurrentJudgeNotesLag[0] <= 0.117) {	//可
				make_judge(1, CurrentTime);
				delete_notes(CurrentJudgeNotes[0]);
				if (isBig == true) {
					make_judge(SPECIAL_NICE, CurrentTime);
					score_update(SPECIAL_NICE);
				}
				else {
					make_judge(NICE, CurrentTime);
					score_update(NICE);
				}
			}
			else if (CurrentJudgeNotesLag[0] <= 0.150) {	//不可
				make_judge(BAD, CurrentTime);
				delete_notes(CurrentJudgeNotes[0]);
				score_update(BAD);
			}
		}

		if (isKatsu == true && CurrentJudgeNotes[1] != -1) {	//カツ

			if (CurrentJudgeNotesLag[1] <= 0.034) {			//良
				delete_notes(CurrentJudgeNotes[1]);
				if (isBig == true) {
					make_judge(SPECIAL_PERFECT, CurrentTime);
					score_update(SPECIAL_PERFECT);
				}
				else {
					make_judge(PERFECT, CurrentTime);
					score_update(PERFECT);
				}
			}
			else if (CurrentJudgeNotesLag[1] <= 0.117) {	//可
				make_judge(1, CurrentTime);
				delete_notes(CurrentJudgeNotes[1]);
				if (isBig == true) {
					make_judge(SPECIAL_NICE, CurrentTime);
					score_update(SPECIAL_NICE);
				}
				else {
					make_judge(NICE, CurrentTime);
					score_update(NICE);
				}
			}
			else if (CurrentJudgeNotesLag[1] <= 0.150) {	//不可
				make_judge(BAD, CurrentTime);
				delete_notes(CurrentJudgeNotes[1]);
				score_update(BAD);
			}
		}

		if (JudgeRollState != -1) {	//連打

			if (isDon == true) {
				if (JudgeRollState == NOTES_ROLL) score_update(ROLL);
				else if (JudgeRollState == NOTES_BIGROLL) score_update(BIG_ROLL);
			}
			if (isKatsu == true) {
				if (JudgeRollState == NOTES_ROLL) score_update(ROLL);
				else if (JudgeRollState == NOTES_BIGROLL) score_update(BIG_ROLL);
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

		music_play(SOUND_BALLOONBREAK);
		balloon_count_update(0);
	}
}

void notes_calc(bool isDon, bool isKatsu, double bpm, double CurrentTime, int cnt, C2D_Sprite sprites[Sprite_Number]) {

	OPTION_T Option;
	get_option(&Option);

	for (int i = 0; i < Notes_Max - 1; i++) {	//計算

		if (Notes[i].flag == true) {

			Notes[i].x = Notes[i].x_ini -
				Notes_Area * Notes[i].scroll * (CurrentTime - Notes[i].pop_time) / (60 / Notes[i].bpm * 4);

			switch (Notes[i].knd) {

			case NOTES_ROLL:
				if (Notes[i].roll_id != -1 && RollNotes[Notes[i].roll_id].flag == true) {
					RollNotes[Notes[i].roll_id].start_x = Notes[i].x;
					RollNotes[Notes[i].roll_id].start_id = i;
				}
				break;

			case NOTES_ROLLEND:
				if (Notes[i].roll_id != -1 && RollNotes[Notes[i].roll_id].flag == true) {
					RollNotes[Notes[i].roll_id].end_x = Notes[i].x;
					RollNotes[Notes[i].roll_id].end_id = i;
				}
				break;

			case NOTES_BIGROLL:
				if (Notes[i].roll_id != -1 && RollNotes[Notes[i].roll_id].flag == true) {
					RollNotes[Notes[i].roll_id].start_x = Notes[i].x;
					RollNotes[Notes[i].roll_id].start_id = i;
				}
				break;

			case NOTES_BIGROLLEND:
				if (Notes[i].roll_id != -1 && RollNotes[Notes[i].roll_id].flag == true) {
					RollNotes[Notes[i].roll_id].end_x = Notes[i].x;
					RollNotes[Notes[i].roll_id].end_id = i;
				}
				break;

			case NOTES_BALLOON:
				if (Notes[i].x <= Notes_Judge) Notes[i].x = Notes_Judge;
				if (Notes[i].roll_id != -1) {
					BalloonNotes[Notes[i].roll_id].start_id = i;
				}
				break;

			case NOTES_BALLOONEND:
				if (Notes[i].roll_id != -1) {
					BalloonNotes[Notes[i].roll_id].end_id = i;
				}
				if (Notes[i].judge_time <= CurrentTime) {
					delete_notes(i);
				}
				break;

			case NOTES_DON:
			case NOTES_KATSU:
			case NOTES_BIGDON:
			case NOTES_BIGKATSU:
				if (CurrentTime - Notes[i].judge_time > 0.150 && Notes[i].isThrough == false) {
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
			Notes[i].knd != NOTES_ROLL && Notes[i].knd != NOTES_BIGROLL) {

			if (Notes[i].isThrough == false && 
				(Notes[i].knd == NOTES_DON || Notes[i].knd == NOTES_KATSU || Notes[i].knd == NOTES_BIGDON || Notes[i].knd == NOTES_BIGKATSU)) {

				if (Option.isAuto == false) {
					score_update(THROUGH);
					Notes[i].isThrough = true;
				}
				else {	//オート時はスルーでも良判定に
					if (Notes[i].knd == NOTES_DON || Notes[i].knd == NOTES_KATSU) score_update(PERFECT);
					else if (Notes[i].knd == NOTES_BIGDON || Notes[i].knd == NOTES_BIGKATSU) score_update(SPECIAL_PERFECT);
					if (Notes[i].knd == NOTES_DON || Notes[i].knd == NOTES_BIGDON) music_play(SOUND_DON);
					if (Notes[i].knd == NOTES_KATSU || Notes[i].knd == NOTES_BIGKATSU) music_play(SOUND_KATSU);
				}
			}
			delete_notes(i);
		}
	}

	notes_judge(CurrentTime, isDon, isKatsu, cnt);
}

void notes_draw(C2D_Sprite sprites[Sprite_Number]) {

	int notes_y = 109;

	for (int i = 0; i < Notes_Max - 1; i++) {	//描画

		if (Notes[i].flag == true) {

			switch (Notes[i].knd) {
			case NOTES_DON:
				C2D_SpriteSetPos(&sprites[SPRITE_DON], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[SPRITE_DON]);
				break;
			case NOTES_KATSU:
				C2D_SpriteSetPos(&sprites[SPRITE_KATSU], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[SPRITE_KATSU]);
				break;
			case NOTES_BIGDON:
				C2D_SpriteSetPos(&sprites[SPRITE_BIG_DON], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[SPRITE_BIG_DON]);
				break;
			case NOTES_BIGKATSU:
				C2D_SpriteSetPos(&sprites[SPRITE_BIG_KATSU], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[SPRITE_BIG_KATSU]);
				break;
			case NOTES_ROLL: {

				if (RollNotes[Notes[i].roll_id].flag == true) {

					double end_x;
					if (RollNotes[Notes[i].roll_id].end_id == -1) end_x = TOP_WIDTH;
					else end_x = RollNotes[Notes[i].roll_id].end_x;

					for (int n = 0; n < (end_x - RollNotes[Notes[i].roll_id].start_x) / 9.0; n++) {
						C2D_SpriteSetPos(&sprites[SPRITE_ROLL_INT], Notes[i].x + 9 * n, notes_y);
						C2D_DrawSprite(&sprites[SPRITE_ROLL_INT]);
					}
					C2D_SpriteSetPos(&sprites[SPRITE_ROLL_START], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[SPRITE_ROLL_START]);
				}
				break;
			}
			case NOTES_BIGROLL: {

				if (RollNotes[Notes[i].roll_id].flag == true) {

					double end_x;
					if (RollNotes[Notes[i].roll_id].end_id == -1) end_x = TOP_WIDTH;
					else end_x = RollNotes[Notes[i].roll_id].end_x;

					for (int n = 0; n < (end_x - RollNotes[Notes[i].roll_id].start_x) / 9.0; n++) {
						C2D_SpriteSetPos(&sprites[SPRITE_BIG_ROLL_INT], Notes[i].x + 9 * n, notes_y);
						C2D_DrawSprite(&sprites[SPRITE_BIG_ROLL_INT]);
					}

					C2D_SpriteSetPos(&sprites[SPRITE_BIG_ROLL_START], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[SPRITE_BIG_ROLL_START]);
					break;
				}
			}
			case NOTES_BALLOON:

				if (BalloonNotes[Notes[i].roll_id].current_hit == 0) {

					C2D_SpriteSetPos(&sprites[SPRITE_BALLOON], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[SPRITE_BALLOON]);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit * 1 / 5) {

					C2D_SpriteSetPos(&sprites[SPRITE_BALLOON_1], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[SPRITE_BALLOON_1]);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit * 2 / 5) {

					C2D_SpriteSetPos(&sprites[SPRITE_BALLOON_2], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[SPRITE_BALLOON_2]);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit * 3 / 5) {

					C2D_SpriteSetPos(&sprites[SPRITE_BALLOON_3], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[SPRITE_BALLOON_3]);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit * 4 / 5) {

					C2D_SpriteSetPos(&sprites[SPRITE_BALLOON_4], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[SPRITE_BALLOON_4]);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit) {

					C2D_SpriteSetPos(&sprites[SPRITE_BALLOON_5], Notes[i].x, notes_y);
					C2D_DrawSprite(&sprites[SPRITE_BALLOON_5]);
				}

				if (BalloonNotes[Notes[i].roll_id].current_hit >= 1) balloon_count_update(BalloonNotes[Notes[i].roll_id].need_hit - BalloonNotes[Notes[i].roll_id].current_hit);

				//snprintf(buf_notes, sizeof(buf_notes), "%d", BalloonNotes[Notes[i].roll_id].need_hit - BalloonNotes[Notes[i].roll_id].current_hit);
				//draw_debug(Notes[i].x, 132, buf_notes);
				break;
			case NOTES_ROLLEND:
				C2D_SpriteSetPos(&sprites[SPRITE_ROLL_END], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[SPRITE_ROLL_END]);
				break;
			case NOTES_BIGROLLEND:
				C2D_SpriteSetPos(&sprites[SPRITE_BIG_ROLL_END], Notes[i].x, notes_y);
				C2D_DrawSprite(&sprites[SPRITE_BIG_ROLL_END]);
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
		C2D_SpriteSetPos(&sprites[SPRITE_BALLOON_6], Notes_Judge, notes_y);
		C2D_DrawSpriteTinted(&sprites[SPRITE_BALLOON_6], &Tint);
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
		(Notes[i].knd == NOTES_ROLLEND || Notes[i].knd == NOTES_BIGROLLEND) &&
		RollNotes[Notes[i].roll_id].flag == true
		) {	//連打削除

		delete_notes(RollNotes[Notes[i].roll_id].start_id);
		delete_roll(Notes[i].roll_id);
		score_update(ROLL_END);

	}

	if (i >= 0 &&
		Notes[i].roll_id != -1 &&
		BalloonNotes[Notes[i].roll_id].flag == true) {					//風船削除

		if (Notes[i].knd == NOTES_BALLOONEND) {

			if (BalloonNotes[Notes[i].roll_id].start_id != -1) delete_notes(BalloonNotes[Notes[i].roll_id].start_id);
			delete_balloon(Notes[i].roll_id);
			balloon_count_update(0);
		}
		else if (Notes[i].knd == NOTES_BALLOON) {

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

C2D_TextBuf g_NotesText = C2D_TextBufNew(4096);
C2D_Text NotesText;

void draw_notes_text(float x, float y, const char *text, float *width, float *height) {

	C2D_TextBufClear(g_NotesText);
	C2D_TextParse(&NotesText, g_NotesText, text);
	C2D_TextOptimize(&NotesText);
	float size = 0.7;

	C2D_TextGetDimensions(&NotesText, size, size, width, height);

	C2D_DrawText(&NotesText, C2D_WithColor, x-*width, y, 1.0f, size, size, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
}

void draw_title() {

	TJA_HEADER_T Header;
	get_tja_header(&Header);
	float width = 0, height = 0;

	if (Header.subtitle_state != -1 && Header.subtitle_state != 1) {

		draw_notes_text(TOP_WIDTH, 160, Header.subtitle, &width, &height);
	}
	draw_notes_text(TOP_WIDTH-width, 160, Header.title, &width, &height);
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
	PreNotesKnd = 0;
}