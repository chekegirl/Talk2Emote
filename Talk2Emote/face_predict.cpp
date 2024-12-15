#include <string>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <vector>
#include <tuple>
#include <regex>
#include <map>
#include <iostream>
#include <fstream>


typedef std::string (*PredictFunc)(std::string);
typedef std::vector<std::string> (*NegaposiAllClassesFunc)();
std::vector<std::string> judge_data_dll_names;
std::vector<std::string> judge_data_dll_names_notype;
std::vector<HMODULE> judge_data_dlls;
std::vector<PredictFunc> judge_data_predict_funcs;
std::vector<int> judge_data_priority_index;
int judge_data_kyokusei_index = -1;

/**
 * デバッグ用関数
 */
void DebugFileOutput(std::string str){
	FILE *fp_debug;
	fp_debug = fopen("C:/Talk2Emote/face_predict_Debug_Log.txt", "a");
	fprintf(fp_debug, str.c_str());
	fclose(fp_debug);
}

// Shift-JISからUTF-8に変換する関数
std::string shiftJISToUTF8(const std::string& shiftJISStr) {
	// Shift-JISからUTF-16に変換
	int utf16Size = MultiByteToWideChar(CP_ACP, 0, shiftJISStr.c_str(), -1, NULL, 0);
	std::wstring utf16Str(utf16Size, L'\0');
	MultiByteToWideChar(CP_ACP, 0, shiftJISStr.c_str(), -1, &utf16Str[0], utf16Size);
	// UTF-16からUTF-8に変換
	int utf8Size = WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, NULL, 0, NULL, NULL);
	std::string utf8Str(utf8Size, '\0');
	WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, &utf8Str[0], utf8Size, NULL, NULL);
	// 結果を返す
	return utf8Str;
}

/** 
 * DLLファイル全てのパスリストを返す関数を作る(ファイル名に条件を付ける事ができる)
 * Input  : std::string folder_path(探索するフォルダのパス), std::regex file_regex(ファイル名の条件（正規表現）)
 * Output : std::vector<std::string>(ファイル名のベクトル)
*/
std::vector<std::string> GetFileList(std::string folder_path, std::regex file_regex){
	WIN32_FIND_DATAA findFileData;
	HANDLE hFind;

	std::string wildcard = folder_path + "\\*";
	hFind = FindFirstFileA(wildcard.c_str(), &findFileData);

	std::vector<std::string> result = {};
	if (hFind == INVALID_HANDLE_VALUE) {
		return result;
	}

	do {
		std::string file_name = findFileData.cFileName;
		if(std::regex_match(file_name, file_regex)){
			result.push_back(file_name);
		}
	} while (FindNextFileA(hFind, &findFileData) != 0);

	return result;
}

/**
 * 表情特殊推定プラグインの判定優先順位の読み込み
 * Input  : 読み込み対象のプラグイン一覧(std::vector<std::string>) ※拡張子なし
 * Output : 優先度順位のインデックス番号(std::vector<int>)
 */
std::vector<int> GetTokushuDLLPriority(std::vector<std::string> dll_list){
	const std::string file_path = "C:/Talk2Emote/face_predict_plugins/priority.txt";
	std::ifstream priority_file(file_path);
	std::vector<int> priority_index = {};
	if (!priority_file) {
		for (int i=0; i<dll_list.size(); i++){
			priority_index.push_back(i);
		}
	}else{
		std::string dll_name;
		std::vector<std::string> priority_dll_list = {};
		while (std::getline(priority_file, dll_name)) {
			priority_dll_list.push_back(dll_name);
		}
		priority_file.close();
		for (int i=0; i<priority_dll_list.size(); i++){
			for (int j=0; j<dll_list.size(); j++){
				if (priority_dll_list[i] == dll_list[j]){
					priority_index.push_back(j);
					break;
				}
			}
		}
	}
	return priority_index;
}


extern "C" {
	/**
	 * 入力されたテキストに対し、face_predict_pluginsフォルダに入ってる全てのdllファイルについて処理を行う。その結果を連結したchar*として返す。
	 * Input  : 表情を判定するセリフのテキスト(char* 型、Shift_JIS)
	 * Output : 表情のデータが入ったテキスト(std::tuple<std::string, std::string> 型、特殊,極性)
	 */
	__declspec(dllexport) std::tuple<std::string, std::string> __cdecl StrJudge(const char* match_text){
		std::string str_match_text = match_text;
		str_match_text = str_match_text + "___START___" + match_text + "___EOS___";
		std::string utf8_str_match_text = shiftJISToUTF8(str_match_text);

		std::map<std::string, std::string> result_map = {};
		for(int i=0; i<judge_data_dll_names.size(); i++){
			std::string result = judge_data_predict_funcs[i](utf8_str_match_text);
			result_map[judge_data_dll_names[i]] = result;
		}

		std::string temp_tokushu = "none";
		std::string temp_kyokusei = "neutral";
		// judge_data_kyokusei_indexが-1じゃなければそのインデックスの関数を呼び出す。
		if (judge_data_kyokusei_index != -1){
			temp_kyokusei = result_map[judge_data_dll_names[judge_data_kyokusei_index]];
		}
		// priorityに応じた読み込み順番で表情特殊を判定
		for(int i=0; i< judge_data_priority_index.size(); i++){
			if (result_map[judge_data_dll_names[judge_data_priority_index[i]]] == "true"){
				temp_tokushu = judge_data_dll_names_notype[judge_data_priority_index[i]];
				break;
			}
		}
		std::tuple<std::string, std::string> result_tuple(temp_tokushu, temp_kyokusei);
		
		return result_tuple;
	}

	/** 
	 * テキストから表情の推定を行うための事前準備
	 * Input  : なし
	 * Output : なし
	 */
	__declspec(dllexport) void __cdecl InitStrJudge(){
		std::string predict_plugins_folder = "C:/Talk2Emote/face_predict_plugins";
		std::regex dll_file_re(R"(.*\.dll)");
		judge_data_dll_names = GetFileList(predict_plugins_folder, dll_file_re);
		for(int i=0; i<judge_data_dll_names.size(); i++){
			std::string dll_path = predict_plugins_folder + "/" + judge_data_dll_names[i];
			HMODULE hDll = LoadLibrary(dll_path.c_str());
			judge_data_dlls.push_back(hDll);
			std::string dll_file_name = judge_data_dll_names[i];
			std::string dll_file_name_notype = dll_file_name.erase(dll_file_name.size()-4);
			judge_data_dll_names_notype.push_back(dll_file_name_notype);
			std::string predict_func_name = "predict_" + dll_file_name_notype;
			PredictFunc predict_func = (PredictFunc)GetProcAddress(hDll, predict_func_name.c_str());
			judge_data_predict_funcs.push_back(predict_func);
			if(judge_data_dll_names[i] == "negaposi.dll"){
				judge_data_kyokusei_index = i;
			}
		}
		// 表情特殊プラグインの優先順位の取得
		judge_data_priority_index = GetTokushuDLLPriority(judge_data_dll_names_notype);
	}

	/** 
	 * テキストから表情の推定を行うためのdllの解放
	 * Input  : なし
	 * Output : なし
	 */
	__declspec(dllexport) void __cdecl DeInitStrJudge(){
		for(int i=0; i<judge_data_dlls.size(); i++){
			FreeLibrary(judge_data_dlls[i]);
		}
	}

	/**
	 * 表情特殊と表情極性の一覧を取得
	 * Input  : なし
	 * Output : タプル(表情特殊の一覧, 表情極性の一覧) (std::tuple<std::vector<std::string>, std::vector<std::string>>)
	 */
	__declspec(dllexport) std::tuple<std::vector<std::string>, std::vector<std::string>> __cdecl GetFaceList(){
		std::vector<std::string> kyokusei_classes;
		std::vector<std::string> tokushu_classes;
		for(int i=0; i<judge_data_dll_names_notype.size(); i++){
			if(judge_data_dll_names_notype[i] != "negaposi"){
				tokushu_classes.push_back(judge_data_dll_names_notype[i]);
			}
		}
		NegaposiAllClassesFunc NegaposiAllClasses = (NegaposiAllClassesFunc)GetProcAddress(judge_data_dlls[judge_data_kyokusei_index], "all_classes");
		kyokusei_classes = NegaposiAllClasses();
		std::tuple<std::vector<std::string>, std::vector<std::string>> result_tuple(tokushu_classes, kyokusei_classes);
		return result_tuple;
	}
}