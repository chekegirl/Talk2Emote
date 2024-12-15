#include <windows.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <tuple>
#include <vector>
#include <map>
#include <time.h>
#include <set>
#include "diff_type.h"

// 辞書に必要なパラメータ
#define MAX_STRINGS 65536
#define STRINGS_LENGTH 512

// 共有メモリを表す構造体
struct DictionaryMemory {
	// 全立ち絵共通の辞書データ
	int general_no_saved_end = 0;								// ファイルに保存していない辞書キューの終了位置（この数字を含まない）
	int general_no_saved_start = 0;								// ファイルに保存していない辞書キューの終了位置（この数字を含む）
	int general_no_saved_index[MAX_STRINGS];					// ファイルに保存していない辞書のインデックス（キューのイメージ）
	int general_count = 0;										// 文字列の数
	char general_strings[MAX_STRINGS][STRINGS_LENGTH];			// 最大65536個の文字列（各2048バイトまで）
	char general_params_1[MAX_STRINGS][STRINGS_LENGTH];			// 最大65536個のパラメータ1（各2048バイトまで）
	char general_params_2[MAX_STRINGS][STRINGS_LENGTH];			// 最大65536個のパラメータ2（各2048バイトまで）
	//高速化のために保存しておくことにしたデータ
	char general_wakati_b[MAX_STRINGS][STRINGS_LENGTH];			// 最大65536個の文字列(分かち書きして基本形に直した状態で保存しておく)（各2048バイトまで）
	int general_wakati_b_divide[MAX_STRINGS][STRINGS_LENGTH];		// 最大65536個の文字列の分かち書き基本形の単語の区切れ目(-1で開始し、終端に-2が入っている形)
	int general_strings_divide[MAX_STRINGS][STRINGS_LENGTH];		// 最大65536個の文字列の文字の区切れ目(-1で開始し、終端に-2が入っている形)
	int general_wakati_b_divide_size[MAX_STRINGS];					// 最大65536個の文字列の分かち書き基本形の単語の区切れ目(-1で開始し、終端に-2が入っている形)のサイズ
	int general_strings_divide_size[MAX_STRINGS];					// 最大65536個の文字列の文字の区切れ目(-1で開始し、終端に-2が入っている形)のサイズ
	int general_len_bucket[STRINGS_LENGTH][MAX_STRINGS];			// 文字列の長さ番目の要素がその長さの文字列のインデックスが入っている配列
	int general_len_bucket_count[STRINGS_LENGTH];					// 上記配列の各要素のインデックス数（カウント）
	int general_strlen[MAX_STRINGS];								// 文字列の長さ(strlen)
	int general_wakati_len[MAX_STRINGS];							// 分かち書きの単語数
	//キャッシュ
	char general_strings_cache[MAX_STRINGS][STRINGS_LENGTH];			// 最大65536個の文字列（各2048バイトまで）（キャッシュ）（完全一致による検索のみ）
	char general_wakati_b_cache[MAX_STRINGS][STRINGS_LENGTH];			// 最大65536個の文字列（各2048バイトまで）の分かち書きの基本形にしたもの（キャッシュ）
	int general_dict_index_cache[STRINGS_LENGTH];						// 最大65536個の辞書上のどの項目に該当していたかのインデックス（キャッシュ）（もし「何も該当するものを得られなかった」場合には、キャッシュ値として何も得られなかった-1を入れておく）
	int general_diff_cache[MAX_STRINGS];								// 最大65536個のdiff値（キャッシュ）（もし「何も該当するものを得られなかった」場合には、キャッシュ値として何も得られなかったdiff_thrを入れておく）
	bool general_is_word_diff_cache[MAX_STRINGS];						// 最大65536個のdiffの取り方が単語ごとかどうか（単語毎ならtrue）（キャッシュ）
	bool general_is_ban_list_diff_cache[MAX_STRINGS];					// 最大65536個のdiff判定に編集禁止単語リスト（ban_list）を用いているかどうか（キャッシュ）
	bool general_cache_deleted[MAX_STRINGS];							// 最大65536個のキャッシュが削除済みかどうか（キャッシュ本体は消さずに削除済みのキャッシュはここのフラグをtrueにする感じで）
	int general_wakati_b_divide_cache[MAX_STRINGS][STRINGS_LENGTH];		// 最大65536個の文字列の分かち書き基本形の単語の区切れ目(-1で開始し、終端に-2が入っている形)
	int general_strings_divide_cache[MAX_STRINGS][STRINGS_LENGTH];		// 最大65536個の文字列の文字の区切れ目(-1で開始し、終端に-2が入っている形)
	int general_wakati_b_divide_size_cache[MAX_STRINGS];					// 最大65536個の文字列の分かち書き基本形の単語の区切れ目(-1で開始し、終端に-2が入っている形)のサイズ
	int general_strings_divide_size_cache[MAX_STRINGS];					// 最大65536個の文字列の文字の区切れ目(-1で開始し、終端に-2が入っている形)のサイズ
	int general_count_cache = 0;										// 文字列の数（キャッシュ）
};

std::string dictionary_name;						//現在選択している辞書ファイル名
DictionaryMemory *dictionary_memory = nullptr;		//現在選択しているDictionaryMemory*
//std::map<std::string, DictionaryMemory*> dictionary_memorys_list = {};	//全てのDictionaryMemory*

std::map<std::string, HANDLE> dictionary_file_handles_list = {};	//全てのメモリのハンドル

//良く使うライブラリを事前にロードしておこう
HMODULE mecab_dllHandle = nullptr;
HMODULE BanList = nullptr;

//編集禁止リストの類は最初に読み込んでおいた方がいい。
typedef bool (*BanListJudgeFunc)(bool, std::map<DiffType, std::vector<std::vector<std::string>>>, const char*, const char*);
BanListJudgeFunc BanListJudge;

//関数を引っ張り出すときのやつ
typedef char* (*Str2WordArrayFunc)(const char*);
typedef void (*MakeMecabObjectFunc)();
typedef void (*DestroyMecabObjectFunc)();

/**
 * ＜全立ち絵共通、立ち絵個別、両方用の処理＞
 * 文字列と検索する配列から同じテキストがあるかを検索する関数。使用前にMutexを必ず取得する事。
 * Input  : テキスト, 検索する配列, 配列の長さ（辞書に登録されているテキストの数）,文字列長からインデックスへの対応配列, 文字列長からインデックスへの対応配列各要素のインデックス数
 * Output : インデックス
 */
int SearchStringsIndex(const char* str, const char strings[][STRINGS_LENGTH], int len_bucket[][MAX_STRINGS], int len_bucket_count[]){
	int str_len = strlen(str);
	for (int i=0; i<len_bucket_count[str_len]; i++){
		int result = strcmp(str, strings[len_bucket[str_len][i]]);
		if (result == 0){
			return len_bucket[str_len][i];
		}
	}
	return -1;
}


/**
 * SJISと分かる形で区切った文字列に変換
 * Input  : 文字列（const char*）
 * Output : std::vector<std::vector<char>>({1バイトなら1つ},{2バイトなら2つ},...)みたいなベクトル
 */
std::vector<std::vector<char>> Str2VStr(const char* str){
	std::vector<std::vector<char>> str_sjis = {};
	int str_ptr = 0;
	while(str[str_ptr] != '\0'){
		unsigned char byte = static_cast<unsigned char>(str[str_ptr]);
		int byte_num = static_cast<int>(byte);
		if ((byte_num <= 127) || ((161 <= byte_num) && (byte_num <= 223))){
			//半角文字の場合
			std::vector<char> char_sjis = {str[str_ptr]};
			str_sjis.push_back(char_sjis);
			str_ptr ++;
		}else{
			//全角文字の場合
			std::vector<char> char_sjis = {str[str_ptr],str[str_ptr+1]};
			str_sjis.push_back(char_sjis);
			str_ptr += 2;
		}
	}
	return str_sjis;
}


/**
 * SJISと分かる形で単語ごとに区切った文字列に変換
 * Input  : 文字列（const char*）
 * Output : std::vector<std::vector<char>>({単語のバイトベクトル},{単語のバイトベクトル},...)みたいなベクトル
 */
std::vector<std::vector<char>> Str2WordStr(const char* str){
	std::vector<std::vector<char>> str_sjis = {};
	int str_ptr = 0;
	std::vector<char> word_vec = {};
	while(str[str_ptr] != '\0'){
		unsigned char byte = static_cast<unsigned char>(str[str_ptr]);
		int byte_num = static_cast<int>(byte);
		if ((byte_num <= 127) || ((161 <= byte_num) && (byte_num <= 223))){
			//半角文字の場合
			if(str[str_ptr] == ' '){
				str_sjis.push_back(word_vec);
				word_vec = {};
				str_ptr ++;
			}else{
				word_vec.push_back(str[str_ptr]);
				str_ptr ++;
			}
		}else{
			//全角文字の場合
			word_vec.push_back(str[str_ptr]);
			word_vec.push_back(str[str_ptr+1]);
			str_ptr += 2;
		}
	}
	return str_sjis;
}


/**
 * std::vector<char>どうしの比較
 * Input  : std::vector<char>, std::vector<char>
 * Output : bool(一致：true,不一致：false)
 */
bool CompVC(const std::vector<char>& c1, const std::vector<char>& c2){
	if (c1.size() != c2.size()){
		return false;
	}
	for (int i=0; i<c1.size(); i++){
		if(c1[i] != c2[i]){
			return false;
		}
	}
	return true;
}

/**
 * デバッグ用
 * 計算途中を表示する
 */
void PrintLenTable(std::vector<std::vector<int>> len_table){
	std::cout << "---------------------------------------------------------------------------------------------------------------------" << std::endl;
	for(int i=0; i<len_table.size(); i++){
		for(int j=0; j<len_table[i].size(); j++){
			int num = len_table[i][j];
			int keta = 0;
			for (int k=0; num>0; k++){
				num /= 10;
				keta = k;
			}
			for (int k=20-keta; k>0; k--){
				std::cout << " ";
			}
			std::cout << len_table[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << "---------------------------------------------------------------------------------------------------------------------" << std::endl;
}


/**
 * デバッグ用
 * 計算途中を表示する
 */
//enum DiffType {NoData, Through, Swap, Delete, Insert};
void PrintPathTable(std::vector<std::vector<DiffType>> len_table){
	std::cout << "---------------------------------------------------------------------------------------------------------------------" << std::endl;
	for(int i=0; i<len_table.size(); i++){
		for(int j=0; j<len_table[i].size(); j++){
			DiffType path_type = len_table[i][j];
			if(path_type == DiffType::Delete){
				std::cout << "  Delete";
			}
			if(path_type == DiffType::Insert){
				std::cout << "  Insert";
			}
			if(path_type == DiffType::Swap){
				std::cout << "    Swap";
			}
			if(path_type == DiffType::Through){
				std::cout << " Through";
			}
			if(path_type == DiffType::NoData){
				std::cout << "  NoData";
			}
		}
		std::cout << std::endl;
	}
	std::cout << "---------------------------------------------------------------------------------------------------------------------" << std::endl;
}

void PrintVVC(std::vector<std::vector<std::string>> vvc){
	std::cout << "---------------------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "{ ";
	for(int i=0; i<vvc.size(); i++){
		std::cout << "{ ";
		for(int j=0; j<vvc[i].size(); j++){
			std::cout << vvc[i][j];
			if (j != vvc[i].size()-1){
				std::cout << " | ";
			}
		}
		std::cout << " }, ";
	}
	std::cout << " }" << std::endl << "---------------------------------------------------------------------------------------------------------------------" << std::endl;
}


/**
 * ＜全立ち絵共通、立ち絵個別、両方用の処理＞※注意：高速化のため静的な変数を用いている。これを使うときは必ずMutexを確保するなりして、同時に2か所からこの関数を使わない事。
 * ２つの文字列のDiffの値とDiffの位置を取得する。ただし、diff_thrよりもdiffが大きくなることが確定した時点で-1を返す。diff_thr=-1なら最後まで計算する。
 * Input  : テキスト, テキスト, diffを返すdiffの値の閾値, diffを返す文字数or単語数に対する閾値の割合（str1基準、diff_thrが-2のときのみ有効（str1が5単語でratioが0.4なら、5*0.4+1=3単語未満の誤差しか許容しない））,単語ベースか文字ベースか
 * Output : diffの値,map{Swap:{{str1上の文字列,str2上の文字列},...}, Delete:{{str1上の文字列},...}, Insert:{{str2上の文字列},...},}
 */
enum DiffMode {Word, Charactor};
#define STACK_SIZE 65536
#define DIFFTYPE_NODATA 0
#define DIFFTYPE_THROUGH 1
#define DIFFTYPE_SWAP 2
#define DIFFTYPE_DELETE 3
#define DIFFTYPE_INSERT 4
std::tuple<int, std::map<DiffType, std::vector<std::vector<std::string>>>> DiffListStrings(const char* str1, const char* str2, int diff_thr, float diff_ratio, DiffMode diff_mode, const int str1_divide[], int str1_divide_size, const int str2_divide[], int str2_divide_size){
	// 初期設定
	const int big_num = 2147483647;
	if (diff_thr == -1){
		diff_thr = big_num;
	}else if(diff_thr == -2){
		diff_thr = (str1_divide_size-2)*diff_ratio+1;
	}

	// メモ化用のテーブルの宣言
	static int len_table[STRINGS_LENGTH][STRINGS_LENGTH];
	static int path_table[STRINGS_LENGTH][STRINGS_LENGTH];
	// メモ化用のテーブルの初期化
	for(int i=0; i<str1_divide_size-1; i++){
		for(int j=0; j<str2_divide_size-1; j++){
			len_table[i][j] = big_num;
		}
	}
	for(int i=0; i<str1_divide_size-1; i++){
		for(int j=0; j<str2_divide_size-1; j++){
			path_table[i][j] = DIFFTYPE_NODATA;
		}
	}
	
	len_table[0][0] = 0;
	path_table[0][0] = DIFFTYPE_THROUGH;
	static int searched_coord[STACK_SIZE][2];	// 前回探索した座標(上下,左右)
	searched_coord[0][0] = 0;
	searched_coord[0][1] = 0;
	int searched_coord_size = 1;	// searched_coordに入ってる座標の数
	int diff = -1;
	bool diff_flag = false;
	// 高速化のため、while内で使う変数を先に宣言しておく。
	int coord[2];
	bool right_step_able;
	bool under_step_able;
	bool comp_vc;
	int str1_ptr;
	int str1_ptr_end;
	int str2_ptr;
	bool comp_flag;
	while(searched_coord_size > 0){
		coord[0] = searched_coord[searched_coord_size-1][0];	//Y軸(上下)方向
		coord[1] = searched_coord[searched_coord_size-1][1];	//X軸(左右)方向
		searched_coord_size --;

		// 右もしくは下に今の座標から１つ進めるかどうか
		right_step_able = ((coord[1]+1) < (str2_divide_size-1));
		under_step_able = ((coord[0]+1) < (str1_divide_size-1));

		// 右と下の端両方にぶつかっていなければ、編集不要かどうかのチェックを行う。
		comp_vc = false;
		if (right_step_able && under_step_able){
			// 文字や単語が一致しているかどうか
			if((str1_divide[coord[0]+1] - (str1_divide[coord[0]]+1)) == (str2_divide[coord[1]+1] - (str2_divide[coord[1]]+1))){
				str1_ptr = str1_divide[coord[0]]+1;
				str1_ptr_end = str1_divide[coord[0]+1];
				str2_ptr = str2_divide[coord[1]]+1;
				comp_flag = true;
				while(str1_ptr <= str1_ptr_end){
					if(str1[str1_ptr] != str2[str2_ptr]){
						comp_flag = false;
						break;
					}
					str1_ptr ++;
					str2_ptr ++;
				}
				comp_vc = comp_flag;
			}
		}
		if (comp_vc == false){
			// 右
			if (right_step_able){
				if ((len_table[coord[0]][coord[1]]+1 < len_table[coord[0]][coord[1]+1]) && (len_table[coord[0]][coord[1]]+1 < len_table[str1_divide_size-2][str2_divide_size-2]) && (len_table[coord[0]][coord[1]]+1 < diff_thr)){
					len_table[coord[0]][coord[1]+1] = len_table[coord[0]][coord[1]]+1;
					path_table[coord[0]][coord[1]+1] = DIFFTYPE_INSERT;
					searched_coord[searched_coord_size][0] = coord[0];
					searched_coord[searched_coord_size][1] = coord[1]+1;
					searched_coord_size++;
				}
			}
			// 下
			if (under_step_able){
				if ((len_table[coord[0]][coord[1]]+1 < len_table[coord[0]+1][coord[1]]) && (len_table[coord[0]][coord[1]]+1 < len_table[str1_divide_size-2][str2_divide_size-2]) && (len_table[coord[0]][coord[1]]+1 < diff_thr)){
					len_table[coord[0]+1][coord[1]] = len_table[coord[0]][coord[1]]+1;
					path_table[coord[0]+1][coord[1]] = DIFFTYPE_DELETE;
					searched_coord[searched_coord_size][0] = coord[0]+1;
					searched_coord[searched_coord_size][1] = coord[1];
					searched_coord_size++;
				}
			}
		}
		// 右下
		if (right_step_able && under_step_able){
			if (len_table[coord[0]][coord[1]] < len_table[coord[0]+1][coord[1]+1]){
				if (comp_vc){
					if ((len_table[coord[0]][coord[1]] < len_table[str1_divide_size-2][str2_divide_size-2]) && (len_table[coord[0]][coord[1]] < diff_thr)){
						len_table[coord[0]+1][coord[1]+1] = len_table[coord[0]][coord[1]];
						path_table[coord[0]+1][coord[1]+1] = DIFFTYPE_THROUGH;
						searched_coord[searched_coord_size][0] = coord[0]+1;
						searched_coord[searched_coord_size][1] = coord[1]+1;
						searched_coord_size++;
					}
				}else{
					if ((len_table[coord[0]][coord[1]]+1 < len_table[coord[0]+1][coord[1]+1]) && (len_table[coord[0]][coord[1]]+1 < len_table[str1_divide_size-2][str2_divide_size-2]) && (len_table[coord[0]][coord[1]]+1 < diff_thr)){
						len_table[coord[0]+1][coord[1]+1] = len_table[coord[0]][coord[1]]+1;
						path_table[coord[0]+1][coord[1]+1] = DIFFTYPE_SWAP;
						searched_coord[searched_coord_size][0] = coord[0]+1;
						searched_coord[searched_coord_size][1] = coord[1]+1;
						searched_coord_size++;
					}
				}
			}
		}
		if(((coord[1]) == (str2_divide_size-2)) && ((coord[0]) == (str1_divide_size-2))){
			diff_flag = true;
		}
	}
	std::map<DiffType, std::vector<std::vector<std::string>>> return_map;
	if (diff_flag){
		//前から持ってきた処理
		std::vector<std::vector<char>> str1_sjis;
		std::vector<std::vector<char>> str2_sjis;
		if(diff_mode == DiffMode::Charactor){
			// 文字列をSJISとして文字毎に区切る
			str1_sjis = Str2VStr(str1);
			str2_sjis = Str2VStr(str2);
		}else if(diff_mode == DiffMode::Word){
			// 文字列をSJISとして単語毎に区切る
			str1_sjis = Str2WordStr(str1);
			str2_sjis = Str2WordStr(str2);
		}
		std::vector<char> str1_start = {0};
		str1_sjis.insert(str1_sjis.begin(), str1_start);
		std::vector<char> str2_start = {0};
		str2_sjis.insert(str2_sjis.begin(), str2_start);
		//ここからが本処理
		diff = len_table[str1_sjis.size()-1][str2_sjis.size()-1];	// ここはlen_table[str1_divide_size-3][str2_divide_size-3]でも成り立つはず。
		std::vector<std::vector<std::string>> swap_list = {};
		std::string swap_buf1 = "";
		std::string swap_buf2 = "";
		std::vector<std::vector<std::string>> insert_list = {};
		std::string insert_buf = "";
		std::vector<std::vector<std::string>> delete_list = {};
		std::string delete_buf = "";
		std::vector<int> back_coord = {(int)str1_sjis.size()-1, (int)str2_sjis.size()-1};

		while((back_coord[0] >= 0) && (back_coord[1] >= 0)){
			if(diff_mode == DiffMode::Charactor){
				if(path_table[back_coord[0]][back_coord[1]] == DIFFTYPE_DELETE){
					if (insert_buf != ""){
						insert_list.push_back({insert_buf});
						insert_buf = "";
					}
					if (swap_buf1 != ""){
						swap_list.push_back({swap_buf1, swap_buf2});
						swap_buf1 = "";
						swap_buf2 = "";
					}
					std::string buf_c = "";
					for(int i=0; i<str1_sjis[back_coord[0]].size(); i++){
						buf_c = buf_c + str1_sjis[back_coord[0]][i];
					}
					delete_buf = buf_c + delete_buf;
					back_coord = {back_coord[0]-1, back_coord[1]};
				}else if(path_table[back_coord[0]][back_coord[1]] == DIFFTYPE_INSERT){
					if (delete_buf != ""){
						delete_list.push_back({delete_buf});
						delete_buf = "";
					}
					if (swap_buf1 != ""){
						swap_list.push_back({swap_buf1, swap_buf2});
						swap_buf1 = "";
						swap_buf2 = "";
					}
					std::string buf_c = "";
					for(int i=0; i<str2_sjis[back_coord[1]].size(); i++){
						buf_c = buf_c + str2_sjis[back_coord[1]][i];
					}
					insert_buf = buf_c + insert_buf;
					back_coord = {back_coord[0], back_coord[1]-1};
				}else if(path_table[back_coord[0]][back_coord[1]] == DIFFTYPE_SWAP){
					if (delete_buf != ""){
						delete_list.push_back({delete_buf});
						delete_buf = "";
					}
					if (insert_buf != ""){
						insert_list.push_back({insert_buf});
						insert_buf = "";
					}
					std::string buf_c1 = "";
					for(int i=0; i<str1_sjis[back_coord[0]].size(); i++){
						buf_c1 = buf_c1 + str1_sjis[back_coord[0]][i];
					}
					swap_buf1 = buf_c1 + swap_buf1;
					std::string buf_c2 = "";
					for(int i=0; i<str2_sjis[back_coord[1]].size(); i++){
						buf_c2 = buf_c2 + str2_sjis[back_coord[1]][i];
					}
					swap_buf2 = buf_c2 + swap_buf2;
					back_coord = {back_coord[0]-1, back_coord[1]-1};
				}else if(path_table[back_coord[0]][back_coord[1]] == DIFFTYPE_THROUGH){
					if (delete_buf != ""){
						delete_list.push_back({delete_buf});
						delete_buf = "";
					}
					if (insert_buf != ""){
						insert_list.push_back({insert_buf});
						insert_buf = "";
					}
					if (swap_buf1 != ""){
						swap_list.push_back({swap_buf1, swap_buf2});
						swap_buf1 = "";
						swap_buf2 = "";
					}
					back_coord = {back_coord[0]-1, back_coord[1]-1};
				}
			}else if(diff_mode == DiffMode::Word){
				if(path_table[back_coord[0]][back_coord[1]] == DIFFTYPE_DELETE){
					std::string buf_c = "";
					for(int i=0; i<str1_sjis[back_coord[0]].size(); i++){
						buf_c = buf_c + str1_sjis[back_coord[0]][i];
					}
					delete_list.push_back({buf_c});
					back_coord = {back_coord[0]-1, back_coord[1]};
				}else if(path_table[back_coord[0]][back_coord[1]] == DIFFTYPE_INSERT){
					std::string buf_c = "";
					for(int i=0; i<str2_sjis[back_coord[1]].size(); i++){
						buf_c = buf_c + str2_sjis[back_coord[1]][i];
					}
					insert_list.push_back({buf_c});
					back_coord = {back_coord[0], back_coord[1]-1};
				}else if(path_table[back_coord[0]][back_coord[1]] == DIFFTYPE_SWAP){
					std::string buf_c1 = "";
					for(int i=0; i<str1_sjis[back_coord[0]].size(); i++){
						buf_c1 = buf_c1 + str1_sjis[back_coord[0]][i];
					}
					std::string buf_c2 = "";
					for(int i=0; i<str2_sjis[back_coord[1]].size(); i++){
						buf_c2 = buf_c2 + str2_sjis[back_coord[1]][i];
					}
					swap_list.push_back({buf_c1, buf_c2});
					back_coord = {back_coord[0]-1, back_coord[1]-1};
				}else if(path_table[back_coord[0]][back_coord[1]] == DIFFTYPE_THROUGH){
					back_coord = {back_coord[0]-1, back_coord[1]-1};
				}
			}
		}
		return_map[DiffType::Delete] = delete_list;
		return_map[DiffType::Insert] = insert_list;
		return_map[DiffType::Swap] = swap_list;
		// std::cout << "Delete : " << std::endl;
		// PrintVVC(return_map[DiffType::Delete]);
		// std::cout << "Insert : " << std::endl;
		// PrintVVC(return_map[DiffType::Insert]);
		// std::cout << "Swap : " << std::endl;
		// PrintVVC(return_map[DiffType::Swap]);
	}
	std::tuple<int, std::map<DiffType, std::vector<std::vector<std::string>>>> result_tuple(diff, return_map);

	return result_tuple;
}

/**
 * SJISと分かる形の文字列で分かたれているものについて、スペースの数をカウントして単語数をカウント
 * Input  : 文字列（const char*）
 * Output : 単語数 (int)
 */
int WakatiWordCount(const char* str){
	int str_ptr = 0;
	int ctr = 0;
	while(str[str_ptr] != '\0'){
		unsigned char byte = static_cast<unsigned char>(str[str_ptr]);
		int byte_num = static_cast<int>(byte);
		if ((byte_num <= 127) || ((161 <= byte_num) && (byte_num <= 223))){
			//半角文字の場合
			if(str[str_ptr] == ' '){
				ctr ++;
			}
			str_ptr ++;
		}else{
			//全角文字の場合
			str_ptr += 2;
		}
	}
	return ctr;
}


/**
 * 単語及び文字の区切れ目データを生成する。（※この関数は帰ってきた配列を次もう1回関数呼び出しする＝壊れる）
 */
int* Str2CharDivideIndex(const char* str){
	// 文字毎
	static int divide_data[STRINGS_LENGTH];
	int str_ptr = 0;
	int divide_ptr = 1;
	divide_data[0] = -1;
	while(str[str_ptr] != '\0'){
		unsigned char byte = static_cast<unsigned char>(str[str_ptr]);
		int byte_num = static_cast<int>(byte);
		if ((byte_num <= 127) || ((161 <= byte_num) && (byte_num <= 223))){
			//半角文字の場合
			str_ptr ++;
		}else{
			//全角文字の場合
			str_ptr += 2;
		}
		divide_data[divide_ptr] = str_ptr-1;
		divide_ptr ++;
	}
	divide_data[divide_ptr] = -2;
	return divide_data;
}

/**
 * 単語及び文字の区切れ目データを生成する。（※この関数は帰ってきた配列を次もう1回関数呼び出しする＝壊れる）
 */
int* Str2WordDivideIndex(const char* str){
	static int divide_data[STRINGS_LENGTH];
	int str_ptr = 0;
	int divide_ptr = 1;
	divide_data[0] = -1;
	while(str[str_ptr] != '\0'){
		unsigned char byte = static_cast<unsigned char>(str[str_ptr]);
		int byte_num = static_cast<int>(byte);
		if ((byte_num <= 127) || ((161 <= byte_num) && (byte_num <= 223))){
			//半角文字の場合
			if(str[str_ptr] == ' '){
				divide_data[divide_ptr] = str_ptr;
				divide_ptr ++;
			}
			str_ptr ++;
		}else{
			//全角文字の場合
			str_ptr += 2;
		}
	}
	divide_data[divide_ptr] = -2;
	return divide_data;
}


/**
 * キャッシュへの追加（ここの外でMutexの確保が必要）
 * Input  : 文字列, この文字列とこのパラメータが登録されている辞書上の文字列のDiff値, Diffの取り方は単語ごとか, Diffを取る際にban_listを用いたか, 辞書上の何番目のインデックスの物か
 * Output : なし
 */
void AddGeneralCache(const char* str, int diff, bool is_word, bool is_ban_list, int dict_index){
	int written_index = -1;
	for (int i=0; i<dictionary_memory->general_count_cache; i++){
		int cmp_val = strcmp(str, dictionary_memory->general_strings_cache[i]);
		if (cmp_val == 0){
			if ((dictionary_memory->general_is_word_diff_cache[i] == is_word) && dictionary_memory->general_is_ban_list_diff_cache[i] == is_ban_list){
				if (dictionary_memory->general_cache_deleted[i] == false){
					written_index = i;
					break;
				}
			}
		}
	}
	if(written_index == -1){
		// キャッシュが無い場合
		if (dictionary_memory->general_count_cache < MAX_STRINGS) {
			strcpy(dictionary_memory->general_strings_cache[dictionary_memory->general_count_cache], str);
			dictionary_memory->general_cache_deleted[dictionary_memory->general_count_cache] = false;
			dictionary_memory->general_is_ban_list_diff_cache[dictionary_memory->general_count_cache] = is_ban_list;
			dictionary_memory->general_is_word_diff_cache[dictionary_memory->general_count_cache] = is_word;
			dictionary_memory->general_diff_cache[dictionary_memory->general_count_cache] = diff;
			dictionary_memory->general_dict_index_cache[dictionary_memory->general_count_cache] = dict_index;
			Str2WordArrayFunc Str2WordArray = (Str2WordArrayFunc)GetProcAddress(mecab_dllHandle, "Str2WordArray");
			strcpy(dictionary_memory->general_wakati_b_cache[dictionary_memory->general_count_cache], Str2WordArray(str));
			
			// キャッシュにもdivideの値を保存
			int* divide_data_word = Str2WordDivideIndex(dictionary_memory->general_wakati_b_cache[dictionary_memory->general_count_cache]);
			int* divide_data_char = Str2CharDivideIndex(dictionary_memory->general_strings_cache[dictionary_memory->general_count_cache]);
			int divide_data_size_word;
			for(int i=0; divide_data_word[i] >= -1; i++){
				dictionary_memory->general_wakati_b_divide_cache[dictionary_memory->general_count_cache][i] = divide_data_word[i];
				divide_data_size_word = i;
			}
			dictionary_memory->general_wakati_b_divide_cache[dictionary_memory->general_count_cache][divide_data_size_word+1] = divide_data_word[divide_data_size_word+1];
			divide_data_size_word += 2;
			dictionary_memory->general_wakati_b_divide_size_cache[dictionary_memory->general_count_cache] = divide_data_size_word;
			int divide_data_size_char;
			for(int i=0; divide_data_char[i] >= -1; i++){
				dictionary_memory->general_strings_divide_cache[dictionary_memory->general_count_cache][i] = divide_data_char[i];
				divide_data_size_char = i;
			}
			dictionary_memory->general_strings_divide_cache[dictionary_memory->general_count_cache][divide_data_size_char+1] = divide_data_char[divide_data_size_char+1];
			divide_data_size_char += 2;
			dictionary_memory->general_strings_divide_size_cache[dictionary_memory->general_count_cache] = divide_data_size_char;
			
			// キャッシュの数をカウントアップ
			dictionary_memory->general_count_cache ++;
			//std::cout << "cached!!" << std::endl;
		}
	}
}


/**
 * 辞書への追加（新規文字列）によるキャッシュの更新（ここの外でMutexの確保が必要）
 * Input  : 辞書に追加する文字列, 辞書のアップデートした項目番号
 * Output : なし
 */
void DictUpdateGeneralCache(const char* str, int update_index){
	Str2WordArrayFunc Str2WordArray = (Str2WordArrayFunc)GetProcAddress(mecab_dllHandle, "Str2WordArray");
	const char* wakati_str = Str2WordArray(str);
	int written_index = -1;
	int* divide_data_word = Str2WordDivideIndex(wakati_str);
	int* divide_data_char = Str2CharDivideIndex(str);
	int divide_data_size_word;
	for(int i=0; divide_data_word[i] >= -1; i++){
		divide_data_size_word = i;
	}
	divide_data_size_word += 2;
	int divide_data_size_char;
	for(int i=0; divide_data_char[i] >= -1; i++){
		divide_data_size_char = i;
	}
	divide_data_size_char += 2;
	for (int i=0; i<dictionary_memory->general_count_cache; i++){
		if (dictionary_memory->general_cache_deleted[i] == false){
			// 本家の辞書のうちdictionary_memory->general_dict_index_cache[i]番目の要素からの、キャッシュにあるis_word, is_ban_list設定でのDiffを取る。
			int diff = -1;
			std::map<DiffType, std::vector<std::vector<std::string>>> diff_map;
			if(dictionary_memory->general_is_word_diff_cache[i]){
				std::tie(diff, diff_map) = DiffListStrings(wakati_str, dictionary_memory->general_wakati_b_cache[i], dictionary_memory->general_diff_cache[i], 1.0, DiffMode::Word, divide_data_word, divide_data_size_word, dictionary_memory->general_wakati_b_divide_cache[i], dictionary_memory->general_wakati_b_divide_size_cache[i]);
			}else{
				std::tie(diff, diff_map) = DiffListStrings(str, dictionary_memory->general_strings_cache[i], dictionary_memory->general_diff_cache[i], 1.0, DiffMode::Charactor, divide_data_char, divide_data_size_char, dictionary_memory->general_strings_divide_cache[i], dictionary_memory->general_strings_divide_size_cache[i]);
			}
			if(dictionary_memory->general_is_ban_list_diff_cache[i] == true){
				// 編集禁止リストを使う場合キャッシュの場合
				// キャッシュされているDiffの値（general_diff_cache）よりもDiffが小さいなら、キャッシュのdictionary_memory->general_dict_index_cache[i]をupdate_indexにした上で、Diffの値などを更新する。
				if((diff >= 0) && (dictionary_memory->general_diff_cache[i] > diff)){
					std::vector<std::string> bl;
					std::vector<std::vector<std::string>>sap;
					bool ban_flag = BanListJudge(dictionary_memory->general_is_word_diff_cache[i], diff_map, str, dictionary_memory->general_strings_cache[i]);
					if(!ban_flag){
						dictionary_memory->general_diff_cache[i] = diff;
						dictionary_memory->general_dict_index_cache[i] = update_index;
					}
				}
			}else{
				// 編集禁止リストを使わない場合キャッシュの場合
				// キャッシュされているDiffの値（general_diff_cache）よりもDiffが小さいなら、キャッシュのdictionary_memory->general_dict_index_cache[i]をupdate_indexにした上で、Diffの値などを更新する。
				if((diff >= 0) && (dictionary_memory->general_diff_cache[i] > diff)){
					dictionary_memory->general_diff_cache[i] = diff;
					dictionary_memory->general_dict_index_cache[i] = update_index;
				}
			}
		}
	}
}


/**
 * キャッシュへの探索（ここの外でMutexの確保が必要）
 * Input  : 文字列, Diffの閾値, Diffの取り方は単語ごとか, Diffを取る際にban_listを用いたか
 * Output : 該当するインデックス(-1 : 該当するキャッシュデータなし , -2 : キャッシュよりどの辞書データも条件を満たさない事が分かっている。)
 */
int SearchGeneralCache(const char* str, int diff_thr, bool is_word, bool is_ban_list){
	int written_index = -1;
	for (int i=0; i<dictionary_memory->general_count_cache; i++){
		int cmp_val = strcmp(str, dictionary_memory->general_strings_cache[i]);
		if (cmp_val == 0){
			if ((dictionary_memory->general_is_word_diff_cache[i] == is_word) && (dictionary_memory->general_is_ban_list_diff_cache[i] == is_ban_list)){
				if (dictionary_memory->general_cache_deleted[i] == false){
					if (dictionary_memory->general_diff_cache[i] < diff_thr){
						written_index = dictionary_memory->general_dict_index_cache[i];
					}else{
						written_index = -2;
					}
					break;
				}
			}
		}
	}
	return written_index;
}


/**
 * ＜全立ち絵共通、立ち絵個別、両方用の処理＞
 * 文字列と検索する配列から同じ、もしくは閾値以下量の差分のテキストがあるかを検索する関数。使用前にMutexを必ず取得する事。
 * Input  : テキスト, 検索する配列, 配列の長さ（辞書に登録されているテキストの数）,許容する誤差(0が完全一致、1が完全不一致。0.1くらいにしとくのが良さそう。), 単語ごとかどうか, Diff後の処理(ban_listによる同一視していいかどうか)を行うか, 基本形の検索する配列
 * Output : インデックス
 */
int BanListDiffSearchStringsIndex(const char* str, const char strings[][STRINGS_LENGTH], const int divide_strings[][STRINGS_LENGTH], const int divide_strings_size[], const int string_len[], int dict_size, float diff_ratio, bool is_word, bool is_ban_list, const char strings_basic[][STRINGS_LENGTH]){
	//LARGE_INTEGER freq;		//Debug
	//QueryPerformanceFrequency(&freq);		//Debug
	//LARGE_INTEGER search_start, search_end, diff_start, diff_end;//, len_start, len_end, blj_start, blj_end, for_start, fprintf_start, fprintf_end;		//Debug
	//QueryPerformanceCounter(&search_start);		//Debug
	Str2WordArrayFunc Str2WordArray = (Str2WordArrayFunc)GetProcAddress(mecab_dllHandle, "Str2WordArray");
	char* wakati_str = Str2WordArray(str);
	int min_index = -1;
	int diff_thr;		//差分をこの数値"未満"に抑える。
	int wakati_word_ctr = WakatiWordCount(wakati_str);
	int str_char_ctr = strlen(str);
	int* divide_data;
	if(is_word){
		diff_thr = int(wakati_word_ctr*diff_ratio)+1;
		divide_data = Str2WordDivideIndex(wakati_str);
	}else{
		diff_thr = int(str_char_ctr*diff_ratio)+1;
		divide_data = Str2CharDivideIndex(str);
	}
	int divide_data_size;
	for(int i=0; divide_data[i] >= -1; i++){
		divide_data_size = i;
	}
	divide_data_size+=2;
	int cache_written_index = SearchGeneralCache(str, diff_thr, is_word, is_ban_list);
	if(cache_written_index == -2){
		return -1;
	}else if(cache_written_index != -1){
		return cache_written_index;
	}
	int min_diff = diff_thr;
	for (int i=0; i<dict_size; i++){
		int diff = -1;
		std::map<DiffType, std::vector<std::vector<std::string>>> diff_map;
		if(is_word){
			if((string_len[i] - wakati_word_ctr < min_diff) && (wakati_word_ctr - string_len[i] < min_diff)){
				std::tie(diff, diff_map) = DiffListStrings(wakati_str, strings[i], min_diff, 1.0, DiffMode::Word, divide_data, divide_data_size, divide_strings[i], divide_strings_size[i]);
			}
		}else{
			if ((string_len[i] - str_char_ctr < min_diff) && (str_char_ctr - string_len[i] < min_diff)){
				std::tie(diff, diff_map) = DiffListStrings(str, strings[i], min_diff, 1.0, DiffMode::Charactor, divide_data, divide_data_size, divide_strings[i], divide_strings_size[i]);
			}
		}
		if((diff >= 0) && (min_diff > diff)){
			bool ban_flag = false;
			if (is_ban_list){
				ban_flag = BanListJudge(is_word, diff_map, str, strings_basic[i]);
			}
			if(!ban_flag){
				min_diff = diff;
				min_index = i;
				//std::cout << "min_diff = " << min_diff << std::endl;
			}
		}
		if((min_diff == 0) && (min_index != -1)){
			AddGeneralCache(str, min_diff, is_word, is_ban_list, min_index);
			return min_index;
		}
	}
	AddGeneralCache(str, min_diff, is_word, is_ban_list, min_index);
	//QueryPerformanceCounter(&search_end);		//Debug
	//std::cout << "Search : " << ((search_end.QuadPart - search_start.QuadPart)  * 1000.0 / freq.QuadPart) << std::endl;		//Debug
	return min_index;
}


/**
 * ＜全立ち絵共通処理＞(パラメータ分割処理済み)
 * 辞書データの書き込み
 * Input  : テキスト、特殊パラメータ、極性パラメータ
 * Output : なし
 */
void SaveItem(const char* str, const char* param_1, const char* param_2, const char* filename){
	//buf, buf_p1, buf_p2にstr, param_1, param_2の数字にしたものを入れていく。
	int buf[STRINGS_LENGTH];
	int buf_ptr = 0;
	for(int i=0; str[i] != '\0'; i++){
		int value = static_cast<unsigned char>(str[i]);
		buf[buf_ptr] = value;
		buf_ptr++;
	}

	int buf_p1[STRINGS_LENGTH];
	int buf_ptr_p1 = 0;
	for(int i=0; param_1[i] != '\0'; i++){
		int value = static_cast<unsigned char>(param_1[i]);
		buf_p1[buf_ptr_p1] = value;
		buf_ptr_p1++;
	}

	int buf_p2[STRINGS_LENGTH];
	int buf_ptr_p2 = 0;
	for(int i=0; param_2[i] != '\0'; i++){
		int value = static_cast<unsigned char>(param_2[i]);
		buf_p2[buf_ptr_p2] = value;
		buf_ptr_p2++;
	}

	//buf, buf_p1, buf_p2をファイルに書き出す。
	FILE *fp;
	fp = fopen(filename, "a");
	if (fp == nullptr) {
		std::cerr << "Failed to open the file: " << filename << std::endl;
		return;
	}
	for (int i=0; i<buf_ptr; i++){
		fprintf(fp, "%d ", buf[i]);
	}
	fprintf(fp, ",");
	for (int i=0; i<buf_ptr_p1; i++){
		fprintf(fp, "%d ", buf_p1[i]);
	}
	//fprintf(fp, param_1);
	fprintf(fp, ",");
	for (int i=0; i<buf_ptr_p2; i++){
		fprintf(fp, "%d ", buf_p2[i]);
	}
	//fprintf(fp, param_2);
	fprintf(fp, "\n");
	fclose(fp);
}




/**
 * 単語及び文字の区切れ目データを生成する。
 */
void MakeCharDivideIndex(int index){
	// 文字毎
	int str_ptr = 0;
	int divide_ptr = 1;
	dictionary_memory->general_strings_divide[index][0] = -1;
	while(dictionary_memory->general_strings[index][str_ptr] != '\0'){
		unsigned char byte = static_cast<unsigned char>(dictionary_memory->general_strings[index][str_ptr]);
		int byte_num = static_cast<int>(byte);
		if ((byte_num <= 127) || ((161 <= byte_num) && (byte_num <= 223))){
			//半角文字の場合
			str_ptr ++;
		}else{
			//全角文字の場合
			str_ptr += 2;
		}
		dictionary_memory->general_strings_divide[index][divide_ptr] = str_ptr-1;
		divide_ptr ++;
	}
	dictionary_memory->general_strings_divide[index][divide_ptr] = -2;
	dictionary_memory->general_strings_divide_size[index] = divide_ptr+1;
}


/**
 * 単語及び文字の区切れ目データを生成する。
 */
void MakeWordDivideIndex(int index){
	// 単語毎
	int str_ptr = 0;
	int divide_ptr = 1;
	dictionary_memory->general_wakati_b_divide[index][0] = -1;
	while(dictionary_memory->general_wakati_b[index][str_ptr] != '\0'){
		unsigned char byte = static_cast<unsigned char>(dictionary_memory->general_wakati_b[index][str_ptr]);
		int byte_num = static_cast<int>(byte);
		if ((byte_num <= 127) || ((161 <= byte_num) && (byte_num <= 223))){
			//半角文字の場合
			if(dictionary_memory->general_wakati_b[index][str_ptr] == ' '){
				dictionary_memory->general_wakati_b_divide[index][divide_ptr] = str_ptr;
				divide_ptr ++;
			}
			str_ptr ++;
		}else{
			//全角文字の場合
			str_ptr += 2;
		}
	}
	dictionary_memory->general_wakati_b_divide[index][divide_ptr] = -2;
	dictionary_memory->general_wakati_b_divide_size[index] = divide_ptr+1;
}


/**
 * ＜全立ち絵共通処理＞(パラメータ分割処理済み) 
 * 文字列、パラメータを共有メモリ上の辞書に追加
 * Input  : テキスト、そのときのパラメータ
 * Output : 追加結果フラグ（false : 辞書書き込み不可能（辞書インデックスオーバー）、true : 辞書書き込み可能）
 */
bool SimpleAddGeneralItem(const char* str, const char* param_1, const char* param_2, bool update_cache, bool search_add){
	bool result_flag = false;
	// 辞書にテキストが既に存在すれば書き換えモード、そうでなければ追加モード
	int written_index;
	if(search_add){
		written_index = SearchStringsIndex(str, dictionary_memory->general_strings, dictionary_memory->general_len_bucket, dictionary_memory->general_len_bucket_count);
	}else{
		written_index = -1;
	}
	if (written_index == -1){
		// 辞書がいっぱいでなければ、文字列とパラメータの対を書き込む。
		if (dictionary_memory->general_count < MAX_STRINGS) {
			// 辞書に新たに追加する処理
			strcpy (dictionary_memory->general_strings[dictionary_memory->general_count], str);
			strcpy (dictionary_memory->general_params_1[dictionary_memory->general_count], param_1);
			strcpy (dictionary_memory->general_params_2[dictionary_memory->general_count], param_2);
			Str2WordArrayFunc Str2WordArray = (Str2WordArrayFunc)GetProcAddress(mecab_dllHandle, "Str2WordArray");
			char* wakati_str = Str2WordArray(str);
			strcpy (dictionary_memory->general_wakati_b[dictionary_memory->general_count], wakati_str);
			dictionary_memory->general_count++;
			result_flag = true;
			int str_len = strlen(str);
			dictionary_memory->general_len_bucket[str_len][dictionary_memory->general_len_bucket_count[str_len]] = dictionary_memory->general_count-1;
			dictionary_memory->general_len_bucket_count[str_len] ++;

			dictionary_memory->general_strlen[dictionary_memory->general_count-1] = str_len;
			dictionary_memory->general_wakati_len[dictionary_memory->general_count-1] = WakatiWordCount(wakati_str);
			MakeCharDivideIndex(dictionary_memory->general_count-1);
			MakeWordDivideIndex(dictionary_memory->general_count-1);
			if(update_cache == true){
				DictUpdateGeneralCache(str, dictionary_memory->general_count-1);
			}
		}
	}else{
		// 辞書を更新する処理
		strcpy (dictionary_memory->general_params_1[written_index], param_1);
		strcpy (dictionary_memory->general_params_2[written_index], param_2);
		result_flag = true;
	}
	return result_flag;
}


/**
 * ＜全立ち絵共通、立ち絵個別、両方用の処理＞
 * 辞書データを読んできて共有メモリに載せる関数
 * Input  : なし
 * Output : なし
 */
#define READ_BUF_SIZE 12190
void LoadItems(const char* filename){
	LARGE_INTEGER freq;		//Debug
	QueryPerformanceFrequency(&freq);		//Debug
	LARGE_INTEGER load_start, load_end;//, file_start, file_end, hash_start, hash_end;		//Debug
	double load_time = 0;	//Debug
	QueryPerformanceCounter(&load_start);		//Debug
	//ファイルから読み込む
	FILE* fp = fopen(filename, "r");
	if (fp == nullptr) {
		std::cerr << "Failed to open the file: " << filename << std::endl;
		return;
	}
	char buffer[READ_BUF_SIZE];
	int dict_index = 0;
	//whileの中で使う変数だが、高速化のため外で定義
	char val_c_buf[16];
	char strings[STRINGS_LENGTH];
	char param_1[STRINGS_LENGTH];
	char param_2[STRINGS_LENGTH];
	while(fgets(buffer, sizeof(buffer), fp) != nullptr){
		int end_point = strcspn(buffer, "\n");
		buffer[end_point] = '\0';
		dict_index++;
		int num_buf_ptr = 0;
		int param_num = 0;
		int char_pos = 0;
		for(int j=0; j<=end_point; j++){
			if (param_num == 0){
				if (buffer[j] == ' '){
					val_c_buf[num_buf_ptr] = '\0';
					int value = std::stoi(val_c_buf);
					char val_char = static_cast<char>(value);
					strings[char_pos] = val_char;
					num_buf_ptr = 0;
					char_pos++;
				}else if(buffer[j] == ','){
					strings[char_pos] = '\0';
					param_num++;
					char_pos = 0;
					num_buf_ptr = 0;
				}else{
					val_c_buf[num_buf_ptr] = buffer[j];
					num_buf_ptr++;
				}
			}else if(param_num == 1){
				if (buffer[j] == ' '){
					val_c_buf[num_buf_ptr] = '\0';
					int value = std::stoi(val_c_buf);
					char val_char = static_cast<char>(value);
					param_1[char_pos] = val_char;
					num_buf_ptr = 0;
					char_pos++;
				}else if(buffer[j] == ','){
					param_1[char_pos] = '\0';
					param_num++;
					char_pos = 0;
					num_buf_ptr = 0;
				}else{
					val_c_buf[num_buf_ptr] = buffer[j];
					num_buf_ptr++;
				}
			}else if(param_num == 2){
				if (buffer[j] == ' '){
					val_c_buf[num_buf_ptr] = '\0';
					int value = std::stoi(val_c_buf);
					char val_char = static_cast<char>(value);
					param_2[char_pos] = val_char;
					num_buf_ptr = 0;
					char_pos++;
				}else if(buffer[j] == '\0'){
					param_2[char_pos] = '\0';
					break;
				}else{
					val_c_buf[num_buf_ptr] = buffer[j];
					num_buf_ptr++;
				}
			}
		}
		
		SimpleAddGeneralItem(strings, param_1, param_2, false, true);
	}
	fclose(fp);
	
	QueryPerformanceCounter(&load_end);		//Debug
	load_time += static_cast<double>(load_end.QuadPart - load_start.QuadPart) * 1000.0 / freq.QuadPart;		//Debug
	//std::cout << "load_time : " << load_time << std::endl;
}


/**
 * 入力されたテキストchar*が長さオーバーじゃないかを確認し、オーバーならtrue、そうでなければfalseを返す関数
 * Input  ：文字列, 長さの最大値
 * Output : bool（オーバー：true、範囲内：false）
 */
bool IsStrLengthOver(const char* str, int max_length){
	Str2WordArrayFunc Str2WordArray = (Str2WordArrayFunc)GetProcAddress(mecab_dllHandle, "Str2WordArray");
	char* wakati_str = Str2WordArray(str);
	int wakati_len = strlen(wakati_str);
	int str_len = strlen(str);
	if((wakati_len > max_length) || (str_len > max_length)){
		return true;
	}else{
		return false;
	}
}


/**
 * メモリリーク対策用のクラス
 * 異常終了した時にDllMainのDLL_PROCESS_DETACHが呼ばれない可能性があるため、コンストラクタデストラクタによる管理をしている。
 * なお、このクラスがそれ以外で役に立つことはない。
 */
class DllMainClass{
	public:
		/**
		 * 開始処理
		 */
		DllMainClass() {
			//ライブラリの読み込み
			mecab_dllHandle = LoadLibrary("C:/Talk2Emote/mecab_funcs.dll");
			BanList = LoadLibrary("C:/Talk2Emote/ban_list.dll");
			BanListJudge = (BanListJudgeFunc)GetProcAddress(BanList, "BanListJudge");
			//mecabのインスタンスを作る
			MakeMecabObjectFunc MakeMecabObject = (MakeMecabObjectFunc)GetProcAddress(mecab_dllHandle, "MakeMecabObject");
			MakeMecabObject();
		}
	
	/**
	 * 終了処理
	 */
	~DllMainClass(){
		//ライブラリについて解放
		DestroyMecabObjectFunc DestroyMecabObject = (DestroyMecabObjectFunc)GetProcAddress(mecab_dllHandle, "DestroyMecabObject");
		DestroyMecabObject();
		FreeLibrary(mecab_dllHandle);
		FreeLibrary(BanList);
		if (dictionary_memory != nullptr){
			UnmapViewOfFile(dictionary_memory);
		}
		
		for (auto it = dictionary_file_handles_list.begin(); it != dictionary_file_handles_list.end(); ++it) {
			CloseHandle(dictionary_file_handles_list[it->first]);		// ハンドルを閉じる
		}
		dictionary_memory = nullptr;
	}
};
/**
 * このコンストラクタが呼ばれることによって名前付き共有メモリとかその辺が初期化される
*/
DllMainClass dll_main_class;


/**
 * 今から使うメモリを選択する（何かの関数を呼ぶ前に必ずこれを呼ぶこと。）
 */
extern "C" __declspec(dllexport) void ChangeDictionary(std::string dictionary_file_name){
	dictionary_name = dictionary_file_name;
	if (dictionary_memory != nullptr){
		UnmapViewOfFile(dictionary_memory);
	}
	dictionary_memory = (DictionaryMemory*)MapViewOfFile(dictionary_file_handles_list[dictionary_file_name], FILE_MAP_ALL_ACCESS, 0, 0, sizeof(DictionaryMemory));	// 共有メモリのマッピング
	//dictionary_memory = dictionary_memorys_list[dictionary_file_name];
}


/**
 * 新しい辞書の追加（ChangeDictionaryを行わなくても自動で変更されます）
 */
extern "C" __declspec(dllexport) void AddNewDictionary(std::string dictionary_file_name){
	if(dictionary_file_handles_list.count(dictionary_file_name) == 0){
		HANDLE dictionary_file_handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(DictionaryMemory), NULL);			// DictionaryMemoryサイズの共有メモリを作る
		dictionary_file_handles_list[dictionary_file_name] = dictionary_file_handle;
		ChangeDictionary(dictionary_file_name);
		memset(dictionary_memory, 0, sizeof(DictionaryMemory));		// メモリのゼロ埋め（初期値）
		LoadItems(dictionary_file_name.c_str());
	}
}


/**
 * ＜全立ち絵共通処理＞(パラメータ分割処理済み)
 * 文字列からパラメータの値を出力する関数。辞書に登録されていなければnullptrが返される。（必ずstrcpy等でディープコピーして結果を返すこと）
 * Input  : テキスト
 * Output : パラメータ
 */
extern "C" __declspec(dllexport) std::tuple<std::string, std::string> __cdecl DiffSearchGeneralParam(const char* str, float diff_ratio, bool is_word, bool is_ban_list) {
	if (dictionary_memory->general_count > 0){
		// 文字列長さオーバーの場合
		if(IsStrLengthOver(str, STRINGS_LENGTH)){
			std::string result_param_1;
			std::string result_param_2;
			std::tuple<std::string, std::string> result_tuple(result_param_1, result_param_2);
			return result_tuple;
		}
		
		int written_index = -1;
		if(is_word){
			written_index = SearchStringsIndex(str, dictionary_memory->general_strings, dictionary_memory->general_len_bucket, dictionary_memory->general_len_bucket_count);
			if(written_index == -1){
				written_index = BanListDiffSearchStringsIndex(str, dictionary_memory->general_wakati_b, dictionary_memory->general_wakati_b_divide, dictionary_memory->general_wakati_b_divide_size, dictionary_memory->general_wakati_len, dictionary_memory->general_count, diff_ratio, is_word, is_ban_list, dictionary_memory->general_strings);
			}
		}else{
			written_index = BanListDiffSearchStringsIndex(str, dictionary_memory->general_strings, dictionary_memory->general_strings_divide, dictionary_memory->general_strings_divide_size, dictionary_memory->general_strlen, dictionary_memory->general_count, diff_ratio, is_word, is_ban_list, dictionary_memory->general_strings);
		}

		std::string result_param_1;
		std::string result_param_2;
		if(written_index != -1){
			result_param_1.assign(dictionary_memory->general_params_1[written_index]);
			result_param_2.assign(dictionary_memory->general_params_2[written_index]);
		}
		
		std::tuple<std::string, std::string> result_tuple(result_param_1, result_param_2);
		return result_tuple;
	}else{
		std::tuple<std::string, std::string> result_tuple("", "");
		return result_tuple;
	}
}


/**
 * ＜全立ち絵共通処理＞(パラメータ分割処理済み)
 * 文字列からパラメータの値を出力する関数。辞書に登録されていなければnullptrが返される。（必ずstrcpy等でディープコピーして結果を返すこと）
 * Input  : テキスト
 * Output : パラメータ
 */
extern "C" __declspec(dllexport) std::tuple<std::string, std::string> __cdecl SearchGeneralParam(const char* str) {
	// 文字列長さオーバーの場合
	if(IsStrLengthOver(str, STRINGS_LENGTH)){
		std::string result_param_1;
		std::string result_param_2;
		std::tuple<std::string, std::string> result_tuple(result_param_1, result_param_2);
		return result_tuple;
	}
	
	int written_index = SearchStringsIndex(str, dictionary_memory->general_strings, dictionary_memory->general_len_bucket, dictionary_memory->general_len_bucket_count);

	std::string result_param_1;
	std::string result_param_2;
	if(written_index != -1){
		result_param_1.assign(dictionary_memory->general_params_1[written_index]);
		result_param_2.assign(dictionary_memory->general_params_2[written_index]);
	}


	std::tuple<std::string, std::string> result_tuple(result_param_1, result_param_2);
	return result_tuple;
}


/**
 * ＜全立ち絵共通処理＞(パラメータ分割処理済み)
 * インデックス番号から辞書のテキストとパラメータを返す。インデックスが無ければnullptr, nullptrを返す。（必ずstrcpy等でディープコピーして結果を返すこと）
 * Input  : インデックス番号
 * Output : テキスト, パラメータ
 */
extern "C" __declspec(dllexport) std::tuple<std::string, std::tuple<std::string, std::string>> __cdecl GetGeneralParam(int index_num) {
	std::string result_str;
	std::string result_param_1;
	std::string result_param_2;
	if (index_num < dictionary_memory->general_count){
		result_str.assign(dictionary_memory->general_strings[index_num]);
		result_param_1.assign(dictionary_memory->general_params_1[index_num]);
		result_param_2.assign(dictionary_memory->general_params_2[index_num]);
	}

	
	std::tuple<std::string, std::string> result_param_tuple(result_param_1, result_param_2);
	std::tuple<std::string, std::tuple<std::string, std::string> >  result_tuple(result_str, result_param_tuple);
	return result_tuple;
}


/**
 * ＜全立ち絵共通処理＞(パラメータ分割処理済み)
 * 文字列、パラメータを共有メモリ上の辞書に追加し、更に辞書にセーブする関数（同じ文字列が辞書に入っていたら追加ではなく変更の処理にすること）
 * Input  : テキスト、そのときのパラメータ
 * Output : 追加結果フラグ（false : 辞書書き込み不可能（辞書インデックスオーバー or 文字列長さオーバー ※パラメータの値名が長すぎる場合のケアはしていません）、true : 辞書書き込み可能）
 */
extern "C" __declspec(dllexport) bool __cdecl SaveAddGeneralItem(const char* str, const char* param_1, const char* param_2) {
	if(IsStrLengthOver(str, STRINGS_LENGTH)){
		return false;
	}
	bool result_flag = SimpleAddGeneralItem(str, param_1, param_2, true, true);
	if (result_flag){
		SaveItem(str, param_1, param_2, dictionary_name.c_str());
	}
	return result_flag;
}


/**
 * ＜全立ち絵共通処理＞(パラメータ分割処理済み)
 * 文字列、パラメータを共有メモリ上の辞書に追加する関数（同じ文字列が辞書に入っていたら追加ではなく変更の処理にすること）
 * Input  : テキスト、そのときのパラメータ、既にセーブ済みかどうか（セーブ済みならtrue, そうでなければfalse。主にソフト立ち上げ時の辞書読み込み時などがtrue）
 * Output : 追加結果フラグ（false : 辞書書き込み不可能（辞書インデックスオーバー or 文字列長さオーバー ※パラメータの値名が長すぎる場合のケアはしていません）、true : 辞書書き込み可能）
 */
extern "C" __declspec(dllexport) bool __cdecl AddGeneralItem(const char* str, const char* param_1, const char* param_2, bool is_saved) {
	if(IsStrLengthOver(str, STRINGS_LENGTH)){
		return false;
	}
	
	bool result_flag = false;
	// 辞書にテキストが既に存在すれば書き換えモード、そうでなければ追加モード
	int written_index = SearchStringsIndex(str, dictionary_memory->general_strings, dictionary_memory->general_len_bucket, dictionary_memory->general_len_bucket_count);
	if (written_index == -1){
		// 辞書がいっぱいでなければ、文字列とパラメータの対を書き込む。
		if (dictionary_memory->general_count < MAX_STRINGS) {
			// 辞書に新たに追加する処理
			strcpy (dictionary_memory->general_strings[dictionary_memory->general_count], str);
			strcpy (dictionary_memory->general_params_1[dictionary_memory->general_count], param_1);
			strcpy (dictionary_memory->general_params_2[dictionary_memory->general_count], param_2);
			Str2WordArrayFunc Str2WordArray = (Str2WordArrayFunc)GetProcAddress(mecab_dllHandle, "Str2WordArray");
			char* wakati_str = Str2WordArray(str);
			strcpy (dictionary_memory->general_wakati_b[dictionary_memory->general_count], wakati_str);
			dictionary_memory->general_count++;
			// セーブ済みフラグが立ってなければファイルに未保存のキューを更新
			if(!is_saved){
				dictionary_memory->general_no_saved_index[dictionary_memory->general_no_saved_end] = (dictionary_memory->general_count - 1);
				dictionary_memory->general_no_saved_end = (dictionary_memory->general_no_saved_end + 1) % MAX_STRINGS;
			}
			
			result_flag = true;

			int str_len = strlen(str);
			dictionary_memory->general_len_bucket[str_len][dictionary_memory->general_len_bucket_count[str_len]] = dictionary_memory->general_count-1;
			dictionary_memory->general_len_bucket_count[str_len] ++;

			dictionary_memory->general_strlen[dictionary_memory->general_count-1] = str_len;
			dictionary_memory->general_wakati_len[dictionary_memory->general_count-1] = WakatiWordCount(wakati_str);

			MakeCharDivideIndex(dictionary_memory->general_count-1);
			MakeWordDivideIndex(dictionary_memory->general_count-1);
			DictUpdateGeneralCache(str, dictionary_memory->general_count-1);
		}
	}else{
		// 辞書を更新する処理
		strcpy (dictionary_memory->general_params_1[written_index], param_1);
		strcpy (dictionary_memory->general_params_2[written_index], param_2);
		// セーブ済みフラグが立ってなければファイルに未保存のキューを更新
		if(!is_saved){
			dictionary_memory->general_no_saved_index[dictionary_memory->general_no_saved_end] = written_index;
			dictionary_memory->general_no_saved_end = (dictionary_memory->general_no_saved_end + 1) % MAX_STRINGS;
		}
		result_flag = true;
	}

	return result_flag;
}


/**
 * ＜全立ち絵共通処理＞(パラメータ分割処理済み)
 * 未保存キューに入ってるインデックスの辞書情報を１つ返す。キューが空なら何も返さない。返すとき、キューのスタートポイントを１つ進めること。
 * Input  : なし
 * Output : テキスト, パラメータ
 */
extern "C" __declspec(dllexport) std::tuple<std::string, std::tuple<std::string, std::string>> __cdecl PopGeneralNoSavedQueue() {
	std::string result_str;
	std::string result_param_1;
	std::string result_param_2;
	if ((dictionary_memory->general_no_saved_start) < (dictionary_memory->general_no_saved_end)){
		int re_index = dictionary_memory->general_no_saved_index[dictionary_memory->general_no_saved_start];
		dictionary_memory->general_no_saved_start = (dictionary_memory->general_no_saved_start + 1) % MAX_STRINGS;
		result_str.assign(dictionary_memory->general_strings[re_index]);
		result_param_1.assign(dictionary_memory->general_params_1[re_index]);
		result_param_2.assign(dictionary_memory->general_params_2[re_index]);
	}

	std::tuple<std::string, std::string> result_param_tuple(result_param_1, result_param_2);
	std::tuple<std::string, std::tuple<std::string, std::string>> result_tuple(result_str, result_param_tuple);
	return result_tuple;
}
