#include <lua.hpp>
#include <string>
#include <stdio.h>
#include <cstring>
#include <string.h>
#include <windows.h>
#include <vector>
#include <tchar.h>
#include <tuple>
#include <iomanip>
#include <sstream>
#include <regex>
#include <map>
#include <mutex>
#include <queue>

//--プログラム全体で持って置くべき情報-----------------------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HANDLE mutex_handle;

// UI上に表示するもののハンドル
HWND hwnd_layernum_text;				// レイヤ番号表示欄
HWND hwnd_is_word_text;					// is_word表示欄
HWND hwnd_is_ban_list_text;				// is_ban_list表示欄
HWND hwnd_diff_ratio_text;				// diffratio表示欄
HWND hwnd_ptkl_seed_text;					// ptkl選択シード値の表示欄
HWND hwnd_str_text;						// セリフ表示欄
HWND hwnd_ptkf_text;					// ptkf表示欄
HWND hwnd_ptkl_by_str_text;				// ptkl表示欄(現在値)
HWND hwnd_ptkl_by_face_text;			// ptkl表示欄(保存値)
HWND hwnd_face_tokushu_text;			// 「表情特殊」
HWND hwnd_face_tokushu_combobox;		// 表情特殊の選択欄
HWND hwnd_face_tokushu_edit;			// 表情特殊の入力欄
HWND hwnd_face_tokushu_saved_text;		// 表情特殊表示欄(保存値)
HWND hwnd_face_tokushu_predict_text;	// 表情特殊表示欄(推定値)
HWND hwnd_face_kyokusei_text;			// 「表情極性」
HWND hwnd_face_kyokusei_combobox;		// 表情極性の選択欄
HWND hwnd_face_kyokusei_edit;			// 表情極性の入力欄
HWND hwnd_face_kyokusei_saved_text;		// 表情極性表示欄(保存値)
HWND hwnd_face_kyokusei_predict_text;	// 表情極性表示欄(推定値)
HWND hwnd_face2ptkl_delete_button;		// 表情からptklの削除ボタン
HWND hwnd_str2face_save_button;			// セリフから表情の保存ボタン
HWND hwnd_face2ptkl_save_button;		// 表情からptklの保存ボタン
HWND hwnd_str2face2ptkl_save_button;	// セリフから表情と表情からptklの両方同時保存ボタン

HWND hwnd_debug_text;					// DebugLogのテキスト

// 保有しておくべき情報(Luaから送られてくる部分)
int holddata_layer_num;									// レイヤ番号
bool holddata_is_word;									// 辞書引きを単語単位で行うかどうか
bool holddata_is_ban_list;								// 辞書引きで編集禁止リストを用いるかどうか
float holddata_diff_ratio;								// 辞書引きで許容する差分の割合(完全一致：0.0, 全文字(単語)置換：1.0)
int holddata_ptkl_seed;									// ptkl選択のシード値
std::string holddata_str;								// セリフ
std::string holddata_ptkf;								// ptkf値
std::string holddata_ptkl_by_str;						// ptkl値(現在値)

// 保有しておくべき情報(辞書DLLにより更新する物)
std::string holddata_ptkl_by_face;						// ptkl値(保存値)
std::string holddata_face_tokushu_saved = "auto";		// 表情特殊の保存値
std::string holddata_face_kyokusei_saved = "auto";		// 表情極性の保存値

// 保有しておくべき情報(辞書DLLにより更新する物で、Aviutl全体で共通のもの)
std::vector<std::string> holddata_face_tokushu_list;	// 表情特殊の一覧
std::vector<std::string> holddata_face_kyokusei_list;	// 表情極性の一覧

// 保有しておくべき情報(推定DLLにより更新する物)
std::string holddata_face_tokushu_predict;				// 表情特殊の推定値
std::string holddata_face_kyokusei_predict;				// 表情極性の推定値

// windowdata_〇〇を定義する（Aviutl全体で共通のもの以外）
	//これは何故かというと、holddata_〇〇は複数レイヤにまたがって常に更新されているため、ウインドウ上での表示には向かないからである。
	//また、ウインドウ上で表示しているデータを保持しておかないと、ボタンに対する応答がおかしくなるため。
// 保有しておくべき情報(Luaから送られてくる部分)
int windowdata_layer_num;								// レイヤ番号
bool windowdata_is_word;								// 辞書引きを単語単位で行うかどうか
bool windowdata_is_ban_list;							// 辞書引きで編集禁止リストを用いるかどうか
float windowdata_diff_ratio;							// 辞書引きで許容する差分の割合(完全一致：0.0, 全文字(単語)置換：1.0)
int windowdata_ptkl_seed;								// ptkl選択のシード値
std::string windowdata_str;								// セリフ
std::string windowdata_ptkf;							// ptkf値
std::string windowdata_ptkl_by_str;						// ptkl値(現在値)

// 保有しておくべき情報(辞書DLLにより更新する物)
std::string windowdata_ptkl_by_face;					// ptkl値(保存値)
std::string windowdata_face_tokushu_saved = "auto";		// 表情特殊の保存値
std::string windowdata_face_kyokusei_saved = "auto";	// 表情極性の保存値

// 保有しておくべき情報(推定DLLにより更新する物)
std::string windowdata_face_tokushu_predict;				// 表情特殊の推定値
std::string windowdata_face_kyokusei_predict;				// 表情極性の推定値

// DebugLog欄のメッセージ
std::queue<std::string> log_mes_que;	// DebugLog欄のメッセージ

// DLLファイルを扱う上で持っておくべきもの
HINSTANCE application_instance;	// アプリケーションのインスタンス
HWND window_handle = NULL;		// ウインドウのハンドル

#define WINDOW_CLASS_NAME "Talk2EmoteMain"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * ログを流す
 */
void updateLogs(std::string log_str){
	const int log_size_max = 10;
	log_mes_que.push(log_str);
	if (log_mes_que.size() > log_size_max){
		log_mes_que.pop();
	}
	std::string all_log_str = "Log:\r\n";
	std::queue<std::string> new_queue;
	int log_size = log_mes_que.size();
	for (int i=0; i<log_size; i++){
		std::string current_log = log_mes_que.front();
		new_queue.push(current_log);
		all_log_str += current_log + "\r\n";
		log_mes_que.pop();
	}
	log_mes_que = new_queue;
	SetWindowText(hwnd_debug_text, all_log_str.c_str());	//Debug
}

/**
 * デバッグ用関数
 */
void DebugFileOutput(std::string str){
	FILE *fp_debug;
	fp_debug = fopen("C:/Talk2Emote/Talk2Emote_Debug_Log.txt", "a");
	fprintf(fp_debug, str.c_str());
	fclose(fp_debug);
}

//--発言内容から表情を推定する機能-----------------------------------------------------------------------------------------------------------------------------
std::vector<std::string> kyokusei_classes;
std::vector<std::string> tokushu_classes;

HINSTANCE face_predict_instance = nullptr;

typedef std::tuple<std::string, std::string> (*StrJudgeFunc)(const char*);
typedef void (*InitStrJudgeFunc)();
typedef void (*DeInitStrJudgeFunc)();
typedef std::tuple<std::vector<std::string>, std::vector<std::string>> (*GetFaceListFunc)();
StrJudgeFunc StrJudge;
InitStrJudgeFunc InitStrJudge;
DeInitStrJudgeFunc DeInitStrJudge;
GetFaceListFunc GetFaceList;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//--辞書をいい感じに作って保存/検索する機能-----------------------------------------------------------------------------------------------------------------------------

typedef std::tuple<std::string, std::string> (*DiffSearchGeneralParamFunc)(const char*, float, bool, bool);
typedef std::tuple<std::string, std::string> (*SearchGeneralParamFunc)(const char*);
typedef std::tuple<std::string, std::tuple<std::string, std::string>> (*GetGeneralParamFunc)(int);
typedef bool (*SaveAddGeneralItemFunc)(const char*, const char*, const char*);
typedef std::vector<std::string> (*BanListFunc) ();
typedef std::vector<std::vector<std::string>> (*SwapAbleListFunc) ();
typedef void (*AddNewDictionaryFunc)(std::string);
typedef void (*ChangeDictionaryFunc)(std::string);

AddNewDictionaryFunc AddNewDictionary;
ChangeDictionaryFunc ChangeDictionary;
SaveAddGeneralItemFunc SaveAddGeneralItem;
SearchGeneralParamFunc SearchGeneralParam;
GetGeneralParamFunc GetGeneralParam;
DiffSearchGeneralParamFunc DiffSearchGeneralParam;

HINSTANCE dictionary_memory_instance = nullptr;

#define STR_TO_FACE_DICT_PATH "C:/Talk2Emote/dictionary/Str2FaceDictionary.t2edict"
#define FACE_DICT_PATH "C:/Talk2Emote/dictionary/FaceDictionary.t2edict"
#define STR2PTKL_TYPE ".str2ptkl.t2edict"
#define FACE2PTKL_TYPE ".face2ptkl.t2edict"

/**
 * テキストと表情の辞書に書き込む
 */
void set_str2face(const char* set_str, const char* set_param_tokushu, const char* set_param_kyokusei){
	AddNewDictionary(STR_TO_FACE_DICT_PATH);
	ChangeDictionary(STR_TO_FACE_DICT_PATH);
	SaveAddGeneralItem(set_str, set_param_tokushu, set_param_kyokusei);
}

/**
 * テキストと表情の辞書からパラメータを取得
 */
std::tuple<std::string, std::string> get_str2face(const char* get_str){
	AddNewDictionary(STR_TO_FACE_DICT_PATH);
	ChangeDictionary(STR_TO_FACE_DICT_PATH);
	std::string get_param_tokushu;
	std::string get_param_kyokusei;
	std::tie(get_param_tokushu, get_param_kyokusei) = SearchGeneralParam(get_str);
	if (get_param_tokushu.size() == 0){
		get_param_tokushu = "auto";
	}
	if (get_param_kyokusei.size() == 0){
		get_param_kyokusei = "auto";
	}
	std::tuple<std::string, std::string> result(get_param_tokushu, get_param_kyokusei);
	return result;
}

/**
 * テキストと表情の辞書からパラメータを取得
 */
std::tuple<std::string, std::string> diff_get_str2face(const char* get_str, bool is_word, bool is_ban_list, float diff_ratio){
	AddNewDictionary(STR_TO_FACE_DICT_PATH);
	ChangeDictionary(STR_TO_FACE_DICT_PATH);
	std::string get_param_tokushu;
	std::string get_param_kyokusei;
	std::tie(get_param_tokushu, get_param_kyokusei) = DiffSearchGeneralParam(get_str, diff_ratio, is_word, is_ban_list);
	if (get_param_tokushu.size() == 0){
		get_param_tokushu = "auto";
	}
	if (get_param_kyokusei.size() == 0){
		get_param_kyokusei = "auto";
	}
	std::tuple<std::string, std::string> result(get_param_tokushu, get_param_kyokusei);
	return result;
}

/**
 * テキストとptklの辞書に書き込む
 */
int set_str2ptkl(const char* set_ptkf, const char* set_str, const char* set_ptkl){
	std::string dict_path = set_ptkf;
	dict_path = dict_path + STR2PTKL_TYPE;
	AddNewDictionary(dict_path);
	ChangeDictionary(dict_path);
	SaveAddGeneralItem(set_str, set_ptkl, "-");
}

/**
 * テキストとptklの辞書からパラメータを取得(テキストに対してptklが無い場合は""(長さ0の文字列)を返す。)
 */
std::string get_str2ptkl(const char* get_ptkf, const char* get_str){
	std::string dict_path = get_ptkf;
	dict_path = dict_path + STR2PTKL_TYPE;
	AddNewDictionary(dict_path);
	ChangeDictionary(dict_path);
	std::string get_ptkl;
	std::string get_none;
	std::tie(get_ptkl, get_none) = SearchGeneralParam(get_str);
	if (get_ptkl.size() == 0){
		get_ptkl = "";
	}
	return get_ptkl;
}

/**
 * テキストとptklの辞書からパラメータを取得(Diff利用)
 */
std::string diff_get_str2ptkl(const char* get_ptkf, const char* get_str, bool is_word, bool is_ban_list, float diff_ratio){
	std::string dict_path = get_ptkf;
	dict_path = dict_path + STR2PTKL_TYPE;
	AddNewDictionary(dict_path);
	ChangeDictionary(dict_path);
	std::string get_ptkl;
	std::string get_none;
	std::tie(get_ptkl, get_none) = DiffSearchGeneralParam(get_str, diff_ratio, is_word, is_ban_list);
	if (get_ptkl.size() == 0){
		get_ptkl = "";
	}
	return get_ptkl;
}

/**
 * 表情とptklの辞書に書き込む（str=[表情特殊,表情極性,連番], param1=ptkl, param2=[削除済みの時:"deleted", 削除されていない時:"-"]）
 */
void set_face2ptkl(const char* set_ptkf, const char* set_param_tokushu, const char* set_param_kyokusei, const char* set_ptkl){
	std::string dict_path = set_ptkf;
	dict_path = dict_path + FACE2PTKL_TYPE;
	AddNewDictionary(dict_path);
	ChangeDictionary(dict_path);

	// これを辞書のテキスト部分のベースとする
	std::string param_tokushu = set_param_tokushu;
	std::string param_kyokusei = set_param_kyokusei;
	std::string set_str_base = param_tokushu + "," + param_kyokusei;

	// まず同じパラメータ、同じptklの組が登録されていない事を確認する
	bool is_register = false;
	std::string str_set_ptkl = set_ptkl;
	for(int i=0; true; i++){
		std::string get_ptkl;
		std::string get_del_flag;
		std::tie(get_ptkl, get_del_flag) = SearchGeneralParam((set_str_base + "," + std::to_string(i)).c_str());
		if(get_ptkl.size() == 0){
			break;
		}
		if((get_ptkl == str_set_ptkl) && (get_del_flag != "deleted")){
			is_register = true;
			break;
		}
	}
	if(is_register == false){
		// まず2つのパラメータの組に対するptklがいくつ既に登録されているか調べて、次の追加する番号を付け足したstrを作る
		std::string set_str;
		for(int i=0; true; i++){
			std::string get_ptkl;
			std::string get_del_flag;
			std::tie(get_ptkl, get_del_flag) = SearchGeneralParam((set_str_base + "," + std::to_string(i)).c_str());
			if ((get_ptkl.size() == 0) || (get_del_flag == "deleted")){
				set_str = set_str_base + "," + std::to_string(i);
				break;
			}
		}

		// 次に新たに表情に対するptklを追加する。
		SaveAddGeneralItem(set_str.c_str(), set_ptkl, "-");
	}
}

/**
 * 表情とptklの辞書からptklを削除
 */
void delete_face2ptkl(const char* delete_ptkf, const char* delete_param_tokushu, const char* delete_param_kyokusei, const char* delete_ptkl){
	std::string dict_path = delete_ptkf;
	dict_path = dict_path + FACE2PTKL_TYPE;
	AddNewDictionary(dict_path);
	ChangeDictionary(dict_path);

	// これを辞書のテキスト部分のベースとする
	std::string param_tokushu = delete_param_tokushu;
	std::string param_kyokusei = delete_param_kyokusei;
	std::string delete_str_base = param_tokushu + "," + param_kyokusei;

	// まず2つのパラメータの組に対するptklのうち既に登録されているものを調べて、削除する番号を付け足したstrを作る
	std::string delete_str;
	for(int i=0; true; i++){
		std::string get_ptkl;
		std::string get_del_flag;
		std::tie(get_ptkl, get_del_flag) = SearchGeneralParam((delete_str_base + "," + std::to_string(i)).c_str());
		if (get_ptkl.size() == 0){
			break;
		}
		if ((get_ptkl == delete_ptkl)){
			delete_str = delete_str_base + "," + std::to_string(i);
			break;
		}
	}

	// 次に新たに表情に対するptklをdeletedにする。
	if(delete_str.size() != 0){
		SaveAddGeneralItem(delete_str.c_str(), "-", "deleted");
	}
}

/**
 * 表情とptklの辞書からパラメータを取得(シード値から表情に対して複数あるptklの内どれかを返すようにする。)
 */
std::string get_face2ptkl(const char* get_ptkf, const char* get_param_tokushu, const char* get_param_kyokusei, int get_seed){
	std::string dict_path = get_ptkf;
	dict_path = dict_path + FACE2PTKL_TYPE;
	AddNewDictionary(dict_path);
	ChangeDictionary(dict_path);
	
	// これを辞書のテキスト部分のベースとする
	std::string param_tokushu = get_param_tokushu;
	std::string param_kyokusei = get_param_kyokusei;
	std::string get_str_base = param_tokushu + "," + param_kyokusei;

	
	// まず表情に対するptklのvector<string>を作成する。
	std::vector<std::string> ptkl_list = {};

	for(int i=0; true; i++){
		std::string get_ptkl;
		std::string get_del_flag;
		std::tie(get_ptkl, get_del_flag) = SearchGeneralParam((get_str_base + "," + std::to_string(i)).c_str());
		if (get_ptkl.size() == 0){
			break;
		}
		if (get_del_flag != "deleted"){
			ptkl_list.push_back(get_ptkl);
		}
	}

	// 次に上記vectorのうち [シード値] % [ptklの数] 番目のptklを返す。
	if(ptkl_list.size() > 0){
		//return debug_text;	//Debug
		return ptkl_list[get_seed % ptkl_list.size()];
	}else{
		return "";
	}
}

/**
 * 表情一覧の辞書に表情を書き込む（str=[表情パラメータ,["tokushu" | "kyokusei"]], param1="-", param2=["deleted" | "-"]）
 */
void set_face(const char* set_param, const char* set_param_meta){
	AddNewDictionary(FACE_DICT_PATH);
	ChangeDictionary(FACE_DICT_PATH);
	std::string string_set_param_meta = set_param_meta;
	std::string string_set_param = set_param;
	std::string str_key = string_set_param + "," + string_set_param_meta;
	
	// set_param_metaの値に応じて表情パラメータを書き込む
	if(strlen(set_param) > 0){
		if(string_set_param_meta == "tokushu" || string_set_param_meta == "kyokusei"){
			SaveAddGeneralItem(str_key.c_str(), "-", "-");
		}
	}
}

/**
 * 表情一覧の辞書から表情を削除（str=[表情パラメータ,["tokushu" | "kyokusei"]], param1=表情パラメータ, param2=["deleted" | "-"]）
 */
void delete_face(const char* delete_param, const char* delete_param_meta){
	AddNewDictionary(FACE_DICT_PATH);
	ChangeDictionary(FACE_DICT_PATH);
	std::string string_delete_param = delete_param;
	std::string string_delete_param_meta = delete_param_meta;
	std::string str_key = string_delete_param + "," + string_delete_param_meta;

	SaveAddGeneralItem(str_key.c_str(), "-", "deleted");
}

/**
 * 表情一覧の辞書から表情を取得(luaにテーブルを返す感じになる...のかな)
 */
std::vector<std::string> get_face(const char* get_param_meta){
	AddNewDictionary(FACE_DICT_PATH);
	ChangeDictionary(FACE_DICT_PATH);
	std::string string_get_param_meta = get_param_meta;

	// テーブルを作っていく
	std::vector<std::string> result;
	for(int i=0; true; i++){
		std::tuple<std::string, std::tuple<std::string, std::string>> one_face = GetGeneralParam(i);
		std::string str_key = std::get<0>(one_face);
		std::tuple<std::string, std::string> pair_data = std::get<1>(one_face);
		std::string no_data = std::get<0>(pair_data);
		std::string del_flag = std::get<1>(pair_data);
		if (del_flag.size() == 0){
			break;
		}

		if (del_flag == "-"){
			// paramとparam_metaを個別に取り出す
			std::string param;
			std::string param_meta;
			bool p_flag = false;
			for(int j=0; j<str_key.size(); j++){
				if (str_key[j] == ','){
					p_flag = true;
				}else{
					if(p_flag){
						param_meta = param_meta + str_key[j];
					}else{
						param = param + str_key[j];
					}
				}
			}

			// param_metaが所望の物だったら以下を行う
			if(param_meta == string_get_param_meta){
				result.push_back(param);
			}
		}
	}
	return result;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define WINDOW_TEXT_HEIGHT 30
#define LOG_SPACE_HEIGHT 200

// DLLのエントリーポイント
BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		mutex_handle = CreateMutex(NULL, FALSE, NULL);
		application_instance = hInstance;
		break;
	case DLL_PROCESS_DETACH:
		WaitForSingleObject(mutex_handle, INFINITE);
		CloseHandle(mutex_handle);
		// ウインドウが存在する場合に破棄する
		if (window_handle != NULL)
		{
			DestroyWindow(window_handle);
			window_handle = NULL;
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

// ウインドウを作成する関数
int CreateWindowInDLL(lua_State* L){
	WNDCLASSEX wc = { 0 };
	HWND hWnd;
	MSG msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = WINDOW_CLASS_NAME;

	if (RegisterClassEx(&wc))
	{
		hWnd = CreateWindowEx(
			0,
			WINDOW_CLASS_NAME,
			"Talk2Emote_for_Aviutl_PSDToolKit",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 895, 20+WINDOW_TEXT_HEIGHT*7+LOG_SPACE_HEIGHT+45,
			NULL,
			NULL,
			GetModuleHandle(NULL),
			NULL);

		if (hWnd)
		{
			ShowWindow(hWnd, SW_SHOWNORMAL);
			UpdateWindow(hWnd);

			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	return 1;
}

/**
 * Input  : テキストのレイヤ番号, セリフ文字列, ptkf, 取得するパラメータ種類(ptkl | face_tokushu | face_kyokusei)
 * Output : 取得が成功したかどうか, パラメータ
 * テキストのレイヤ番号とセリフ文字列が保存されている物と合致しない場合、取得が成功したかどうかでfalseを返す。
*/ 
int GetParam(lua_State* L){
	int get_layer = lua_tonumber(L, 1);
	const char* get_str = lua_tostring(L, 2);
	const char* get_ptkf = lua_tostring(L, 3);
	const char* get_command = lua_tostring(L, 4);
	std::string str_get_str = get_str;
	std::string str_get_ptkf = get_ptkf;
	std::string str_get_command = get_command;
	bool del_save_flag = false;
	if ((holddata_layer_num == get_layer) && (holddata_str == str_get_str) && (holddata_ptkf == str_get_ptkf)){
		if(str_get_command == "ptkl"){
			del_save_flag = true;
			lua_pushboolean(L, 1);
			if (holddata_ptkl_by_str == ""){
				lua_pushstring(L, holddata_ptkl_by_face.c_str());
			}else{
				lua_pushstring(L, holddata_ptkl_by_str.c_str());
			}
		}else if(str_get_command == "face_tokushu"){
			del_save_flag = true;
			lua_pushboolean(L, 1);
			if (holddata_face_tokushu_saved != "auto"){
				lua_pushstring(L, holddata_face_tokushu_saved.c_str());
			}else{
				lua_pushstring(L, holddata_face_tokushu_predict.c_str());
			}
		}else if(str_get_command == "face_kyokusei"){
			del_save_flag = true;
			lua_pushboolean(L, 1);
			if (holddata_face_kyokusei_saved != "auto"){
				lua_pushstring(L, holddata_face_kyokusei_saved.c_str());
			}else{
				lua_pushstring(L, holddata_face_kyokusei_predict.c_str());
			}
		}
	}
	if(del_save_flag == false){
		lua_pushboolean(L, 0);
		lua_pushstring(L, nullptr);
	}
	return 2;
}


/**
 * Input  : テキストのレイヤ番号, セリフ文字列, ptkf, ptkl_by_str, is_word, is_ban_list, diff_ratio
 * Output : なし
 * テキストのレイヤ番号とセリフ文字列が保存されている物と合致しない場合、テキストのレイヤ番号とセリフ文字列を更新した上で、他の情報はクリアする。ボタン押下情報やUI上で選択した表情情報もクリアする。
 * ※新しい表情パラメータが追加されたかどうかはlua側での判定を行い、辞書に登録する。こちらでごちゃごちゃ考えない。
 */
int SetParams(lua_State* L){
	int set_layer = lua_tonumber(L, 1);
	const char* set_str = lua_tostring(L, 2);
	const char* set_ptkf = lua_tostring(L, 3);
	const char* set_ptkl_by_str = lua_tostring(L, 4);
	bool set_is_word = lua_toboolean(L, 5);
	bool set_is_ban_list = lua_toboolean(L, 6);
	float set_diff_ratio = lua_tonumber(L, 7);
	int set_ptkl_seed = lua_tonumber(L, 8);
	std::string str_set_str = set_str;
	std::string str_set_ptkf = set_ptkf;
	std::string str_set_ptkl_by_str = set_ptkl_by_str;

	// holddata_〇〇を更新する
	holddata_layer_num = set_layer;
	holddata_str = str_set_str;
	holddata_ptkf = str_set_ptkf;
	if(str_set_ptkl_by_str.size() != 0){
		set_str2ptkl(str_set_ptkf.c_str(), str_set_str.c_str(), str_set_ptkl_by_str.c_str());	// str2ptkl辞書に対して保存操作を行う
		updateLogs(str_set_ptkf + " : [save] " + str_set_str + " -> " + str_set_ptkl_by_str);
	}
	holddata_is_word = set_is_word;
	holddata_is_ban_list = set_is_ban_list;
	holddata_diff_ratio = set_diff_ratio;
	holddata_ptkl_seed = set_ptkl_seed;


	// 推論や辞書引き等をやり直す。(str2face, str2ptkl, face2ptklを行う)
	holddata_ptkl_by_str = diff_get_str2ptkl(holddata_ptkf.c_str(), holddata_str.c_str(), holddata_is_word, holddata_is_ban_list, holddata_diff_ratio);
	std::tie(holddata_face_tokushu_saved, holddata_face_kyokusei_saved) = diff_get_str2face(holddata_str.c_str(), holddata_is_word, holddata_is_ban_list, holddata_diff_ratio);
	std::tie(holddata_face_tokushu_predict, holddata_face_kyokusei_predict) = StrJudge(holddata_str.c_str());
	
	std::string temp_face_tokushu = holddata_face_tokushu_saved;
	if (holddata_face_tokushu_saved == "auto"){
		temp_face_tokushu = holddata_face_tokushu_predict;
	}
	std::string temp_face_kyokusei = holddata_face_kyokusei_saved;
	if (holddata_face_kyokusei_saved == "auto"){
		temp_face_kyokusei = holddata_face_kyokusei_predict;
	}
	holddata_ptkl_by_face = get_face2ptkl(holddata_ptkf.c_str(), temp_face_tokushu.c_str(), temp_face_kyokusei.c_str(), holddata_ptkl_seed);
	return 0;
}

void UpdateWindowRow(){
	std::string set_text;
	set_text = R"(レイヤ：)" + std::to_string(holddata_layer_num);
	SetWindowText(hwnd_layernum_text, set_text.c_str());
	windowdata_layer_num = holddata_layer_num;

	set_text = R"(is_word：)" + std::to_string(holddata_is_word);
	SetWindowText(hwnd_is_word_text, set_text.c_str());
	windowdata_is_word = holddata_is_word;

	set_text = R"(is_ban_list：)" + std::to_string(holddata_is_ban_list);
	SetWindowText(hwnd_is_ban_list_text, set_text.c_str());
	windowdata_is_ban_list = holddata_is_ban_list;

	set_text = R"(diff_ratio：)" + std::to_string(holddata_diff_ratio);
	SetWindowText(hwnd_diff_ratio_text, set_text.c_str());
	windowdata_diff_ratio = holddata_diff_ratio;

	set_text = R"(ptkl_seed：)" + std::to_string(holddata_ptkl_seed);
	SetWindowText(hwnd_ptkl_seed_text, set_text.c_str());
	windowdata_ptkl_seed = holddata_ptkl_seed;

	set_text = R"(発言内容：)" + holddata_str;
	SetWindowText(hwnd_str_text, set_text.c_str());
	windowdata_str = holddata_str;

	set_text = R"(ptkf：)" + holddata_ptkf;
	SetWindowText(hwnd_ptkf_text, set_text.c_str());
	windowdata_ptkf = holddata_ptkf;

	set_text = R"(ptkl(発言値)：)" + holddata_ptkl_by_str;
	SetWindowText(hwnd_ptkl_by_str_text, set_text.c_str());
	windowdata_ptkl_by_str = holddata_ptkl_by_str;

	set_text = R"(ptkl(表情値)：)" + holddata_ptkl_by_face;
	SetWindowText(hwnd_ptkl_by_face_text, set_text.c_str());
	windowdata_ptkl_by_face = holddata_ptkl_by_face;

	set_text = R"(保存値：)" + holddata_face_tokushu_saved;
	SetWindowText(hwnd_face_tokushu_saved_text, set_text.c_str());
	windowdata_face_tokushu_saved = holddata_face_tokushu_saved;

	set_text = R"(推定値：)" + holddata_face_tokushu_predict;
	SetWindowText(hwnd_face_tokushu_predict_text, set_text.c_str());
	windowdata_face_tokushu_predict = holddata_face_tokushu_predict;

	set_text = R"(保存値：)" + holddata_face_kyokusei_saved;
	SetWindowText(hwnd_face_kyokusei_saved_text, set_text.c_str());
	windowdata_face_kyokusei_saved = holddata_face_kyokusei_saved;

	set_text = R"(推定値：)" + holddata_face_kyokusei_predict;
	SetWindowText(hwnd_face_kyokusei_predict_text, set_text.c_str());
	windowdata_face_kyokusei_predict = holddata_face_kyokusei_predict;
}

/**
 * Input  : なし
 * Output : なし
 * 今の状態のholddata_〇〇でウインドウを更新する。
 */
int UpdateWindow(lua_State* L){
	UpdateWindowRow();
	return 0;
}

int MutexLock(lua_State* L){
	WaitForSingleObject(mutex_handle, INFINITE);
	return 0;
}

int MutexUnlock(lua_State* L){
	ReleaseMutex(mutex_handle);
	return 0;
}

// 入力類のコントロールID
#define FACE_TOKUSHU_COMBOBOX 1001
#define FACE_TOKUSHU_EDIT 1002
#define FACE_KYOKUSEI_COMBOBOX 1003
#define FACE_KYOKUSEI_EDIT 1004
#define FACE2PTKL_DELETE_BUTTON 1005
#define STR2FACE_SAVE_BUTTON 1006
#define FACE2PTKL_SAVE_BUTTON 1007
#define STR2FACE2PTKL_SAVE_BUTTON 1008

// ウインドウのに来たメッセージ毎の処理
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:{
		window_handle = hWnd;
		WaitForSingleObject(mutex_handle, INFINITE);

		//--Log-------------------------------------------------------------------
		hwnd_debug_text = CreateWindow(
			"EDIT",							// 静的テキストクラス名
			"Log:",								// 初期テキスト
			WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE | ES_READONLY,	// スタイル
			10, 20+WINDOW_TEXT_HEIGHT*7, 860, LOG_SPACE_HEIGHT,					// 位置とサイズ(x, y, width, height)
			hWnd,								// 親ウインドウのハンドル
			NULL,								// コントロールのID
			NULL,								// インスタンスハンドル
			NULL								// ボタンの追加パラメータ
		);
		//--------------------------------------------------------------------------


		std::string text_buf;
		text_buf = R"(レイヤ：)" + std::to_string(holddata_layer_num);
		hwnd_layernum_text = CreateWindow(
			"STATIC",							// 静的テキストクラス名
			text_buf.c_str(),					// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,	// スタイル
			10, 10, 172, WINDOW_TEXT_HEIGHT,			// 位置とサイズ(x, y, width, height)
			hWnd,								// 親ウインドウのハンドル
			NULL,								// コントロールのID
			NULL,								// インスタンスハンドル
			NULL								// ボタンの追加パラメータ
		);

		text_buf = R"(is_word：)" + std::to_string(holddata_is_word);
		hwnd_is_word_text = CreateWindow(
			"STATIC",							// 静的テキストクラス名
			text_buf.c_str(),					// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,	// スタイル
			(10+172), 10, 172, WINDOW_TEXT_HEIGHT,		// 位置とサイズ(x, y, width, height)
			hWnd,								// 親ウインドウのハンドル
			NULL,								// コントロールのID
			NULL,								// インスタンスハンドル
			NULL								// ボタンの追加パラメータ
		);

		text_buf = R"(is_ban_list：)" + std::to_string(holddata_is_ban_list);
		hwnd_is_ban_list_text = CreateWindow(
			"STATIC",							// 静的テキストクラス名
			text_buf.c_str(),					// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,	// スタイル
			(10+172*2), 10, 172, WINDOW_TEXT_HEIGHT,	// 位置とサイズ(x, y, width, height)
			hWnd,								// 親ウインドウのハンドル
			NULL,								// コントロールのID
			NULL,								// インスタンスハンドル
			NULL								// ボタンの追加パラメータ
		);

		text_buf = R"(diff_ratio：)" + std::to_string(holddata_diff_ratio);
		hwnd_diff_ratio_text = CreateWindow(
			"STATIC",							// 静的テキストクラス名
			text_buf.c_str(),					// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,	// スタイル
			(10+172*3), 10, 172, WINDOW_TEXT_HEIGHT,	// 位置とサイズ(x, y, width, height)
			hWnd,								// 親ウインドウのハンドル
			NULL,								// コントロールのID
			NULL,								// インスタンスハンドル
			NULL								// ボタンの追加パラメータ
		);

		text_buf = R"(ptkl_seed：)" + std::to_string(holddata_ptkl_seed);
		hwnd_ptkl_seed_text = CreateWindow(
			"STATIC",							// 静的テキストクラス名
			text_buf.c_str(),					// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,	// スタイル
			(10+172*4), 10, 172, WINDOW_TEXT_HEIGHT,	// 位置とサイズ(x, y, width, height)
			hWnd,								// 親ウインドウのハンドル
			NULL,								// コントロールのID
			NULL,								// インスタンスハンドル
			NULL								// ボタンの追加パラメータ
		);

		text_buf = R"(発言内容：)" + holddata_str;
		hwnd_str_text = CreateWindow(
			"STATIC",								// 静的テキストクラス名
			text_buf.c_str(),						// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// スタイル
			10, 10+WINDOW_TEXT_HEIGHT, 860, WINDOW_TEXT_HEIGHT,	// 位置とサイズ(x, y, width, height)
			hWnd,									// 親ウインドウのハンドル
			NULL,									// コントロールのID
			NULL,									// インスタンスハンドル
			NULL									// ボタンの追加パラメータ
		);

		text_buf = R"(ptkf：)" + holddata_ptkf;
		hwnd_ptkf_text = CreateWindow(
			"STATIC",								// 静的テキストクラス名
			text_buf.c_str(),						// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// スタイル
			10, 10+WINDOW_TEXT_HEIGHT*2, 860, WINDOW_TEXT_HEIGHT,	// 位置とサイズ(x, y, width, height)
			hWnd,									// 親ウインドウのハンドル
			NULL,									// コントロールのID
			NULL,									// インスタンスハンドル
			NULL									// ボタンの追加パラメータ
		);

		text_buf = R"(ptkl(発言値)：)" + holddata_ptkl_by_str;
		hwnd_ptkl_by_str_text = CreateWindow(
			"STATIC",								// 静的テキストクラス名
			text_buf.c_str(),						// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// スタイル
			10, 10+WINDOW_TEXT_HEIGHT*3, 700, WINDOW_TEXT_HEIGHT,	// 位置とサイズ(x, y, width, height)
			hWnd,									// 親ウインドウのハンドル
			NULL,									// コントロールのID
			NULL,									// インスタンスハンドル
			NULL									// ボタンの追加パラメータ
		);

		text_buf = R"(ptkl(表情値)：)" + holddata_ptkl_by_face;
		hwnd_ptkl_by_face_text = CreateWindow(
			"STATIC",								// 静的テキストクラス名
			text_buf.c_str(),						// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// スタイル
			10, 10+WINDOW_TEXT_HEIGHT*4, 700, WINDOW_TEXT_HEIGHT,	// 位置とサイズ(x, y, width, height)
			hWnd,									// 親ウインドウのハンドル
			NULL,									// コントロールのID
			NULL,									// インスタンスハンドル
			NULL									// ボタンの追加パラメータ
		);

		hwnd_face_tokushu_text = CreateWindow(
			"STATIC",								// 静的テキストクラス名
			R"(表情特殊：)",						// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// スタイル
			10, 10+WINDOW_TEXT_HEIGHT*5, 100, WINDOW_TEXT_HEIGHT,	// 位置とサイズ(x, y, width, height)
			hWnd,									// 親ウインドウのハンドル
			NULL,									// コントロールのID
			NULL,									// インスタンスハンドル
			NULL									// ボタンの追加パラメータ
		);

		hwnd_face_tokushu_combobox = CreateWindow(
			"COMBOBOX",									// 静的テキストクラス名
			NULL,										// 初期テキスト
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,	// スタイル（リストをドロップダウン形式で表示）
			110, 10+WINDOW_TEXT_HEIGHT*5, 100, 200,			// 位置とサイズ(x, y, width, height)
			hWnd,										// 親ウインドウのハンドル
			(HMENU)FACE_TOKUSHU_COMBOBOX,				// コントロールのID
			application_instance,						// インスタンスハンドル
			NULL										// ボタンの追加パラメータ
		);

		SendMessage(hwnd_face_tokushu_combobox, CB_ADDSTRING, (LPARAM)0, (LPARAM)"auto");
		// 表情特殊の一覧を取得
		holddata_face_tokushu_list = get_face("tokushu");
		for (int i=0; i<holddata_face_tokushu_list.size(); i++){
			SendMessage(hwnd_face_tokushu_combobox, CB_ADDSTRING, (LPARAM)0, (LPARAM)(holddata_face_tokushu_list[i].c_str()));
		}
		SendMessage(hwnd_face_tokushu_combobox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		// tokushu_classesに入っているプラグインはリストに含める
		tokushu_classes.push_back("none");
		for(int j=0; j<tokushu_classes.size(); j++){
			bool is_listed = false;
			for(int i=0; i<holddata_face_tokushu_list.size(); i++){
				if(holddata_face_tokushu_list[i] == tokushu_classes[j]){
					is_listed = true;
					break;
				}
			}
			if (!is_listed){
				set_face(tokushu_classes[j].c_str(), "tokushu");
				holddata_face_tokushu_list.push_back(tokushu_classes[j]);
				SendMessage(hwnd_face_tokushu_combobox, CB_ADDSTRING, (LPARAM)0, (LPARAM)(tokushu_classes[j].c_str()));
			}
		}

		hwnd_face_tokushu_edit = CreateWindow(
			"EDIT",												// 静的テキストクラス名
			NULL,												// 初期テキスト
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,	// スタイル（枠線と自動水平スクロール）
			210, 10+WINDOW_TEXT_HEIGHT*5, 100, WINDOW_TEXT_HEIGHT,			// 位置とサイズ(x, y, width, height)
			hWnd,												// 親ウインドウのハンドル
			(HMENU)FACE_TOKUSHU_EDIT,							// コントロールのID
			application_instance,								// インスタンスハンドル
			NULL												// ボタンの追加パラメータ
		);

		text_buf = R"(保存値：)" + holddata_face_tokushu_saved;
		hwnd_face_tokushu_saved_text = CreateWindow(
			"STATIC",								// 静的テキストクラス名
			text_buf.c_str(),						// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// スタイル
			310, 10+WINDOW_TEXT_HEIGHT*5, 200, WINDOW_TEXT_HEIGHT,// 位置とサイズ(x, y, width, height)
			hWnd,									// 親ウインドウのハンドル
			NULL,									// コントロールのID
			NULL,									// インスタンスハンドル
			NULL									// ボタンの追加パラメータ
		);
		
		text_buf = R"(推定値：)" + holddata_face_tokushu_predict;
		hwnd_face_tokushu_predict_text = CreateWindow(
			"STATIC",								// 静的テキストクラス名
			text_buf.c_str(),						// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// スタイル
			510, 10+WINDOW_TEXT_HEIGHT*5, 200, WINDOW_TEXT_HEIGHT,// 位置とサイズ(x, y, width, height)
			hWnd,									// 親ウインドウのハンドル
			NULL,									// コントロールのID
			NULL,									// インスタンスハンドル
			NULL									// ボタンの追加パラメータ
		);

		hwnd_face_kyokusei_text = CreateWindow(
			"STATIC",								// 静的テキストクラス名
			R"(表情極性：)",						// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// スタイル
			10, 10+WINDOW_TEXT_HEIGHT*6, 100, WINDOW_TEXT_HEIGHT,	// 位置とサイズ(x, y, width, height)
			hWnd,									// 親ウインドウのハンドル
			NULL,									// コントロールのID
			NULL,									// インスタンスハンドル
			NULL									// ボタンの追加パラメータ
		);

		hwnd_face_kyokusei_combobox = CreateWindow(
			"COMBOBOX",									// 静的テキストクラス名
			NULL,										// 初期テキスト
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,	// スタイル（リストをドロップダウン形式で表示）
			110, 10+WINDOW_TEXT_HEIGHT*6, 100, 200,			// 位置とサイズ(x, y, width, height)
			hWnd,										// 親ウインドウのハンドル
			(HMENU)FACE_KYOKUSEI_COMBOBOX,				// コントロールのID
			application_instance,						// インスタンスハンドル
			NULL										// ボタンの追加パラメータ
		);

		SendMessage(hwnd_face_kyokusei_combobox, CB_ADDSTRING, (LPARAM)0, (LPARAM)"auto");
		// 表情極性の一覧を取得
		holddata_face_kyokusei_list = get_face("kyokusei");
		for (int i=0; i<holddata_face_kyokusei_list.size(); i++){
			SendMessage(hwnd_face_kyokusei_combobox, CB_ADDSTRING, (LPARAM)0, (LPARAM)(holddata_face_kyokusei_list[i].c_str()));
		}
		SendMessage(hwnd_face_kyokusei_combobox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		// kyokusei_classesに入っているkyokuseiのプラグインはリストに含める
		for(int j=0; j<kyokusei_classes.size(); j++){
			bool is_listed = false;
			for(int i=0; i<holddata_face_kyokusei_list.size(); i++){
				if(holddata_face_kyokusei_list[i] == kyokusei_classes[j]){
					is_listed = true;
					break;
				}
			}
			if (!is_listed){
				set_face(kyokusei_classes[j].c_str(), "kyokusei");
				holddata_face_kyokusei_list.push_back(kyokusei_classes[j]);
				SendMessage(hwnd_face_kyokusei_combobox, CB_ADDSTRING, (LPARAM)0, (LPARAM)(kyokusei_classes[j].c_str()));
			}
		}

		hwnd_face_kyokusei_edit = CreateWindow(
			"EDIT",												// 静的テキストクラス名
			NULL,												// 初期テキスト
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,	// スタイル（枠線と自動水平スクロール）
			210, 10+WINDOW_TEXT_HEIGHT*6, 100, WINDOW_TEXT_HEIGHT,			// 位置とサイズ(x, y, width, height)
			hWnd,												// 親ウインドウのハンドル
			(HMENU)FACE_KYOKUSEI_EDIT,							// コントロールのID
			application_instance,								// インスタンスハンドル
			NULL												// ボタンの追加パラメータ
		);

		text_buf = R"(保存値：)" + holddata_face_kyokusei_saved;
		hwnd_face_kyokusei_saved_text = CreateWindow(
			"STATIC",								// 静的テキストクラス名
			text_buf.c_str(),						// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// スタイル
			310, 10+WINDOW_TEXT_HEIGHT*6, 200, WINDOW_TEXT_HEIGHT,// 位置とサイズ(x, y, width, height)
			hWnd,									// 親ウインドウのハンドル
			NULL,									// コントロールのID
			NULL,									// インスタンスハンドル
			NULL									// ボタンの追加パラメータ
		);

		text_buf = R"(推定値：)" + holddata_face_kyokusei_predict;
		hwnd_face_kyokusei_predict_text = CreateWindow(
			"STATIC",								// 静的テキストクラス名
			text_buf.c_str(),						// 初期テキスト
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// スタイル
			510, 10+WINDOW_TEXT_HEIGHT*6, 200, WINDOW_TEXT_HEIGHT,// 位置とサイズ(x, y, width, height)
			hWnd,									// 親ウインドウのハンドル
			NULL,									// コントロールのID
			NULL,									// インスタンスハンドル
			NULL									// ボタンの追加パラメータ
		);

		hwnd_face2ptkl_delete_button = CreateWindow(
			"BUTTON",									// ボタンクラス名
			R"(face2ptklを削除)",						// ボタンのテキスト
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,		// スタイル
			710, 10+WINDOW_TEXT_HEIGHT*3, 160, WINDOW_TEXT_HEIGHT,	// 位置とサイズ
			hWnd,										// 親ウインドウのハンドル
			(HMENU)FACE2PTKL_DELETE_BUTTON,				// コントロールのID
			application_instance,						// インスタンスハンドル
			NULL										// ボタンの追加パラメータ
		);

		hwnd_face2ptkl_save_button = CreateWindow(
			"BUTTON",									// ボタンクラス名
			R"(face2ptklを保存)",						// ボタンのテキスト
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,		// スタイル
			710, 10+WINDOW_TEXT_HEIGHT*4, 160, WINDOW_TEXT_HEIGHT,	// 位置とサイズ
			hWnd,										// 親ウインドウのハンドル
			(HMENU)FACE2PTKL_SAVE_BUTTON,				// コントロールのID
			application_instance,						// インスタンスハンドル
			NULL										// ボタンの追加パラメータ
		);

		hwnd_str2face_save_button = CreateWindow(
			"BUTTON",									// ボタンクラス名
			R"(str2faceを保存)",						// ボタンのテキスト
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,		// スタイル
			710, 10+WINDOW_TEXT_HEIGHT*5, 160, WINDOW_TEXT_HEIGHT,	// 位置とサイズ
			hWnd,										// 親ウインドウのハンドル
			(HMENU)STR2FACE_SAVE_BUTTON,				// コントロールのID
			application_instance,						// インスタンスハンドル
			NULL										// ボタンの追加パラメータ
		);

		hwnd_str2face2ptkl_save_button = CreateWindow(
			"BUTTON",									// ボタンクラス名
			R"(str2face2ptklを保存)",					// ボタンのテキスト
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,		// スタイル
			710, 10+WINDOW_TEXT_HEIGHT*6, 160, WINDOW_TEXT_HEIGHT,	// 位置とサイズ
			hWnd,										// 親ウインドウのハンドル
			(HMENU)STR2FACE2PTKL_SAVE_BUTTON,			// コントロールのID
			application_instance,						// インスタンスハンドル
			NULL										// ボタンの追加パラメータ
		);

		ReleaseMutex(mutex_handle);
		break;
	}
	case WM_COMMAND:{	// ボタンがクリックされたときの処理
		// TODO:windowdata_〇〇を参照/更新する。
		if ((LOWORD(wParam) == FACE2PTKL_DELETE_BUTTON) || (LOWORD(wParam) == FACE2PTKL_SAVE_BUTTON) || (LOWORD(wParam) == STR2FACE_SAVE_BUTTON) || (LOWORD(wParam) == STR2FACE2PTKL_SAVE_BUTTON)){
			WaitForSingleObject(mutex_handle, INFINITE);
			// EDITもしくはCOMBOBOXからkyokusei, tokushuの値を取得する。また、EDITを使った場合はそれを記録（tokushu_edit_flag, kyokusei_edit_flag）
			bool tokushu_edit_flag = true;
			bool kyokusei_edit_flag = true;
			int selected_tokushu_index;
			int selected_kyokusei_index;
			TCHAR selected_tokushu_item[256];
			std::string str_selected_tokushu_item;
			TCHAR selected_kyokusei_item[256];
			std::string str_selected_kyokusei_item;
			
			// 表情特殊の入力値取得
			selected_tokushu_index = (int)SendMessage(hwnd_face_tokushu_combobox, CB_GETCURSEL, 0, 0);
			GetWindowText(hwnd_face_tokushu_edit, selected_tokushu_item, sizeof(selected_tokushu_item) / sizeof(TCHAR));
			if(_tcslen(selected_tokushu_item) == 0){
				SendMessage(hwnd_face_tokushu_combobox, CB_GETLBTEXT, (WPARAM)selected_tokushu_index, (LPARAM)selected_tokushu_item);
				tokushu_edit_flag = false;
			}
			str_selected_tokushu_item = selected_tokushu_item;

			// 表情極性の入力値取得
			selected_kyokusei_index = (int)SendMessage(hwnd_face_kyokusei_combobox, CB_GETCURSEL, 0, 0);
			GetWindowText(hwnd_face_kyokusei_edit, selected_kyokusei_item, sizeof(selected_kyokusei_item) / sizeof(TCHAR));
			if(_tcslen(selected_kyokusei_item) == 0){
				SendMessage(hwnd_face_kyokusei_combobox, CB_GETLBTEXT, (WPARAM)selected_kyokusei_index, (LPARAM)selected_kyokusei_item);
				kyokusei_edit_flag = false;
			}
			str_selected_kyokusei_item = selected_kyokusei_item;

			// 表情の保存値を更新
			if((LOWORD(wParam) == STR2FACE_SAVE_BUTTON) || (LOWORD(wParam) == STR2FACE2PTKL_SAVE_BUTTON)){
				windowdata_face_tokushu_saved = str_selected_tokushu_item;
				windowdata_face_kyokusei_saved = str_selected_kyokusei_item;
				holddata_face_tokushu_saved = str_selected_tokushu_item;
				holddata_face_kyokusei_saved = str_selected_kyokusei_item;
			}

			// 推定や辞書引きから表情の値を決める
			std::string temp_tokushu = windowdata_face_tokushu_saved;
			std::string temp_kyokusei = windowdata_face_kyokusei_saved;
			if(windowdata_face_tokushu_saved == "auto"){
				temp_tokushu = windowdata_face_tokushu_predict;
			}
			if(windowdata_face_kyokusei_saved == "auto"){
				temp_kyokusei = windowdata_face_kyokusei_predict;
			}

			// 辞書への保存
			switch (LOWORD(wParam)){
				case FACE2PTKL_DELETE_BUTTON:
					delete_face2ptkl(windowdata_ptkf.c_str(), temp_tokushu.c_str(), temp_kyokusei.c_str(), windowdata_ptkl_by_face.c_str());	// face2ptkl辞書に対して削除操作を行う
					updateLogs(windowdata_ptkf + " : [delete] ( " + temp_tokushu + " , " + temp_kyokusei + " ) -> " + windowdata_ptkl_by_face);
					break;
				case FACE2PTKL_SAVE_BUTTON:
					set_face2ptkl(windowdata_ptkf.c_str(), temp_tokushu.c_str(), temp_kyokusei.c_str(), windowdata_ptkl_by_str.c_str());	// face2ptkl辞書に対して保存操作を行う
					updateLogs(windowdata_ptkf + " : [save] ( " + temp_tokushu + " , " + temp_kyokusei + " ) -> " + windowdata_ptkl_by_face);
					break;
				case STR2FACE_SAVE_BUTTON:
					set_str2face(windowdata_str.c_str(), windowdata_face_tokushu_saved.c_str(), windowdata_face_kyokusei_saved.c_str());	// str2face辞書に対して保存操作を行う
					updateLogs(windowdata_ptkf + " : [save] " + windowdata_str + " -> ( " + windowdata_face_tokushu_saved + " , " + windowdata_face_kyokusei_saved + " )");
					break;
				case STR2FACE2PTKL_SAVE_BUTTON:
					set_str2face(windowdata_str.c_str(), windowdata_face_tokushu_saved.c_str(), windowdata_face_kyokusei_saved.c_str());	// str2face辞書に対して保存操作を行う
					set_face2ptkl(windowdata_ptkf.c_str(), temp_tokushu.c_str(), temp_kyokusei.c_str(), windowdata_ptkl_by_str.c_str());	// face2ptkl辞書に対して保存操作を行う
					updateLogs(windowdata_ptkf + " : [save] ( " + temp_tokushu + " , " + temp_kyokusei + " ) -> " + windowdata_ptkl_by_face);
					updateLogs(windowdata_ptkf + " : [save] " + windowdata_str + " -> ( " + windowdata_face_tokushu_saved + " , " + windowdata_face_kyokusei_saved + " )");
					break;
			}

			// windowdata_ptkl_by_faceを更新(辞書引きを行う)
			windowdata_ptkl_by_face = get_face2ptkl(windowdata_ptkf.c_str(), temp_tokushu.c_str(), temp_kyokusei.c_str(), windowdata_ptkl_seed);
			//holddata_ptkl_by_face = get_face2ptkl(windowdata_ptkf.c_str(), temp_tokushu.c_str(), temp_kyokusei.c_str(), windowdata_ptkl_seed);

			// holddataをこのキャラクターの物にする
			holddata_layer_num = windowdata_layer_num;
			holddata_is_word = windowdata_is_word;
			holddata_is_ban_list = windowdata_is_ban_list;
			holddata_diff_ratio = windowdata_diff_ratio;
			holddata_ptkl_seed = windowdata_ptkl_seed;
			holddata_str = windowdata_str;
			holddata_ptkf = windowdata_ptkf;
			holddata_ptkl_by_str = windowdata_ptkl_by_str;
			holddata_ptkl_by_face = windowdata_ptkl_by_face;
			holddata_face_tokushu_saved = windowdata_face_tokushu_saved;
			holddata_face_tokushu_predict = windowdata_face_tokushu_predict;
			holddata_face_kyokusei_saved = windowdata_face_kyokusei_saved;
			holddata_face_kyokusei_predict = windowdata_face_kyokusei_predict;

			//ウインドウの値を色々更新する。
			UpdateWindowRow();

			// 表情一覧に関しては、発言者ごとに異ならず、全ての発言者共通のものを用いるので、ウインドウはこのタイミングでしか更新しない
			// 表情一覧辞書への追加を行う
			// holddata_face_〇〇_listを更新
			// COMBOBOXの項目も追加
			if (tokushu_edit_flag){
				set_face(str_selected_tokushu_item.c_str(), "tokushu");
				bool is_listed = false;
				for(int i=0; i<holddata_face_tokushu_list.size(); i++){
					if(holddata_face_tokushu_list[i] == str_selected_tokushu_item){
						is_listed = true;
						break;
					}
				}
				if (!is_listed){
					holddata_face_tokushu_list.push_back(str_selected_tokushu_item);
					SendMessage(hwnd_face_tokushu_combobox, CB_ADDSTRING, (LPARAM)0, (LPARAM)(str_selected_tokushu_item.c_str()));
				}
			}
			if (kyokusei_edit_flag){
				set_face(str_selected_kyokusei_item.c_str(), "kyokusei");
				bool is_listed = false;
				for(int i=0; i<holddata_face_kyokusei_list.size(); i++){
					if(holddata_face_kyokusei_list[i] == str_selected_kyokusei_item){
						is_listed = true;
						break;
					}
				}
				if (!is_listed){
					holddata_face_kyokusei_list.push_back(str_selected_kyokusei_item);
					SendMessage(hwnd_face_kyokusei_combobox, CB_ADDSTRING, (LPARAM)0, (LPARAM)(str_selected_kyokusei_item.c_str()));
				}
			}
			ReleaseMutex(mutex_handle);
		}
		break;
	}
	case WM_DESTROY:{
		// ウインドウが破棄されたときにクラス名の登録を解除する
		if (window_handle != NULL)
		{
			DestroyWindow(window_handle); // ウインドウを破棄する
			window_handle = NULL;
		}
		UnregisterClass(WINDOW_CLASS_NAME, GetModuleHandle(NULL));
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/**
 * 字幕準備から引っ張ってきた文字列をいい塩梅に綺麗にする。
 */
int NormalizeText(lua_State* L){
	const char* raw_c_str = lua_tostring(L, 1);
	char buf[4096];
	int buf_ptr = 0;
	int i = 0;
	if(raw_c_str != nullptr){
		while(raw_c_str[i] != '\0'){
			unsigned char byte = static_cast<unsigned char>(raw_c_str[i]);
			int byte_num = static_cast<int>(byte);
			if ((byte_num <= 127) || ((161 <= byte_num) && (byte_num <= 223))){
				//半角文字の場合
				if(raw_c_str[i] != '\n'){	//改行は無視
					buf[buf_ptr] = raw_c_str[i];
					buf_ptr++;
				}
				i++;
			}else{
				//全角文字の場合
				buf[buf_ptr] = raw_c_str[i];
				buf[buf_ptr+1] = raw_c_str[i+1];
				buf_ptr += 2;
				i+=2;
				if (raw_c_str[i-1] == '\\'){
					i++;
				}
			}
		}
	}
	buf[buf_ptr] = '\0';
	lua_pushstring(L, buf);
	return 1;
}

static luaL_Reg functions[] = {
	{"CreateWindowInDLL", CreateWindowInDLL},
	{"GetParam", GetParam},
	{"SetParams", SetParams},
	{"UpdateWindow", UpdateWindow},
	{"MutexLock", MutexLock},
	{"MutexUnlock", MutexUnlock},
	{"NormalizeText", NormalizeText},
	{ nullptr, nullptr }
};

extern "C" {
	__declspec(dllexport) int luaopen_Talk2Emote_for_Aviutl_PSDToolKit(lua_State* L) {
		luaL_register(L, "Talk2Emote_for_Aviutl_PSDToolKit", functions);
		if(dictionary_memory_instance == nullptr){	// 表情辞書引き機能の初期化
			dictionary_memory_instance = LoadLibrary("C:/Talk2Emote/dictionary_memory.dll");
			AddNewDictionary = (AddNewDictionaryFunc)GetProcAddress(dictionary_memory_instance, "AddNewDictionary");
			ChangeDictionary = (ChangeDictionaryFunc)GetProcAddress(dictionary_memory_instance, "ChangeDictionary");
			SaveAddGeneralItem = (SaveAddGeneralItemFunc)GetProcAddress(dictionary_memory_instance, "SaveAddGeneralItem");
			SearchGeneralParam = (SearchGeneralParamFunc)GetProcAddress(dictionary_memory_instance, "SearchGeneralParam");
			GetGeneralParam = (GetGeneralParamFunc)GetProcAddress(dictionary_memory_instance, "GetGeneralParam");
			DiffSearchGeneralParam = (DiffSearchGeneralParamFunc)GetProcAddress(dictionary_memory_instance, "DiffSearchGeneralParam");
		}
		if (face_predict_instance == nullptr){		// 表情推定機能の初期化
			face_predict_instance = LoadLibrary("C:/Talk2Emote/face_predict.dll");
			StrJudge = (StrJudgeFunc)GetProcAddress(face_predict_instance, "StrJudge");
			InitStrJudge = (InitStrJudgeFunc)GetProcAddress(face_predict_instance, "InitStrJudge");
			DeInitStrJudge = (DeInitStrJudgeFunc)GetProcAddress(face_predict_instance, "DeInitStrJudge");
			GetFaceList = (GetFaceListFunc)GetProcAddress(face_predict_instance, "GetFaceList");
			InitStrJudge();
			std::tie(tokushu_classes, kyokusei_classes) = GetFaceList();
		}
		return 1;
	}

	// LuaでこのDLLを呼びだした状態のオブジェクトが用済みになった時に呼ばれる、はず。
	__declspec(dllexport) int release_Talk2Emote_for_Aviutl_PSDToolKit(lua_State* L) {
		if (dictionary_memory_instance != nullptr) {
			FreeLibrary(dictionary_memory_instance);
		}
		if (face_predict_instance != nullptr){
			DeInitStrJudge();
			FreeLibrary(face_predict_instance);
		}
		return 0;
	}
}