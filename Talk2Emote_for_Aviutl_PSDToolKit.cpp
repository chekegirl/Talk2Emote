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

//--�v���O�����S�̂Ŏ����Ēu���ׂ����-----------------------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HANDLE mutex_handle;

// UI��ɕ\��������̂̃n���h��
HWND hwnd_layernum_text;				// ���C���ԍ��\����
HWND hwnd_is_word_text;					// is_word�\����
HWND hwnd_is_ban_list_text;				// is_ban_list�\����
HWND hwnd_diff_ratio_text;				// diffratio�\����
HWND hwnd_ptkl_seed_text;					// ptkl�I���V�[�h�l�̕\����
HWND hwnd_str_text;						// �Z���t�\����
HWND hwnd_ptkf_text;					// ptkf�\����
HWND hwnd_ptkl_by_str_text;				// ptkl�\����(���ݒl)
HWND hwnd_ptkl_by_face_text;			// ptkl�\����(�ۑ��l)
HWND hwnd_face_tokushu_text;			// �u�\�����v
HWND hwnd_face_tokushu_combobox;		// �\�����̑I��
HWND hwnd_face_tokushu_edit;			// �\�����̓��͗�
HWND hwnd_face_tokushu_saved_text;		// �\�����\����(�ۑ��l)
HWND hwnd_face_tokushu_predict_text;	// �\�����\����(����l)
HWND hwnd_face_kyokusei_text;			// �u�\��ɐ��v
HWND hwnd_face_kyokusei_combobox;		// �\��ɐ��̑I��
HWND hwnd_face_kyokusei_edit;			// �\��ɐ��̓��͗�
HWND hwnd_face_kyokusei_saved_text;		// �\��ɐ��\����(�ۑ��l)
HWND hwnd_face_kyokusei_predict_text;	// �\��ɐ��\����(����l)
HWND hwnd_face2ptkl_delete_button;		// �\���ptkl�̍폜�{�^��
HWND hwnd_str2face_save_button;			// �Z���t����\��̕ۑ��{�^��
HWND hwnd_face2ptkl_save_button;		// �\���ptkl�̕ۑ��{�^��
HWND hwnd_str2face2ptkl_save_button;	// �Z���t����\��ƕ\���ptkl�̗��������ۑ��{�^��

HWND hwnd_debug_text;					// DebugLog�̃e�L�X�g

// �ۗL���Ă����ׂ����(Lua���瑗���Ă��镔��)
int holddata_layer_num;									// ���C���ԍ�
bool holddata_is_word;									// ����������P��P�ʂōs�����ǂ���
bool holddata_is_ban_list;								// ���������ŕҏW�֎~���X�g��p���邩�ǂ���
float holddata_diff_ratio;								// ���������ŋ��e���鍷���̊���(���S��v�F0.0, �S����(�P��)�u���F1.0)
int holddata_ptkl_seed;									// ptkl�I���̃V�[�h�l
std::string holddata_str;								// �Z���t
std::string holddata_ptkf;								// ptkf�l
std::string holddata_ptkl_by_str;						// ptkl�l(���ݒl)

// �ۗL���Ă����ׂ����(����DLL�ɂ��X�V���镨)
std::string holddata_ptkl_by_face;						// ptkl�l(�ۑ��l)
std::string holddata_face_tokushu_saved = "auto";		// �\�����̕ۑ��l
std::string holddata_face_kyokusei_saved = "auto";		// �\��ɐ��̕ۑ��l

// �ۗL���Ă����ׂ����(����DLL�ɂ��X�V���镨�ŁAAviutl�S�̂ŋ��ʂ̂���)
std::vector<std::string> holddata_face_tokushu_list;	// �\�����̈ꗗ
std::vector<std::string> holddata_face_kyokusei_list;	// �\��ɐ��̈ꗗ

// �ۗL���Ă����ׂ����(����DLL�ɂ��X�V���镨)
std::string holddata_face_tokushu_predict;				// �\�����̐���l
std::string holddata_face_kyokusei_predict;				// �\��ɐ��̐���l

// windowdata_�Z�Z���`����iAviutl�S�̂ŋ��ʂ̂��̈ȊO�j
	//����͉��̂��Ƃ����ƁAholddata_�Z�Z�͕������C���ɂ܂������ď�ɍX�V����Ă��邽�߁A�E�C���h�E��ł̕\���ɂ͌����Ȃ�����ł���B
	//�܂��A�E�C���h�E��ŕ\�����Ă���f�[�^��ێ����Ă����Ȃ��ƁA�{�^���ɑ΂��鉞�������������Ȃ邽�߁B
// �ۗL���Ă����ׂ����(Lua���瑗���Ă��镔��)
int windowdata_layer_num;								// ���C���ԍ�
bool windowdata_is_word;								// ����������P��P�ʂōs�����ǂ���
bool windowdata_is_ban_list;							// ���������ŕҏW�֎~���X�g��p���邩�ǂ���
float windowdata_diff_ratio;							// ���������ŋ��e���鍷���̊���(���S��v�F0.0, �S����(�P��)�u���F1.0)
int windowdata_ptkl_seed;								// ptkl�I���̃V�[�h�l
std::string windowdata_str;								// �Z���t
std::string windowdata_ptkf;							// ptkf�l
std::string windowdata_ptkl_by_str;						// ptkl�l(���ݒl)

// �ۗL���Ă����ׂ����(����DLL�ɂ��X�V���镨)
std::string windowdata_ptkl_by_face;					// ptkl�l(�ۑ��l)
std::string windowdata_face_tokushu_saved = "auto";		// �\�����̕ۑ��l
std::string windowdata_face_kyokusei_saved = "auto";	// �\��ɐ��̕ۑ��l

// �ۗL���Ă����ׂ����(����DLL�ɂ��X�V���镨)
std::string windowdata_face_tokushu_predict;				// �\�����̐���l
std::string windowdata_face_kyokusei_predict;				// �\��ɐ��̐���l

// DebugLog���̃��b�Z�[�W
std::queue<std::string> log_mes_que;	// DebugLog���̃��b�Z�[�W

// DLL�t�@�C����������Ŏ����Ă����ׂ�����
HINSTANCE application_instance;	// �A�v���P�[�V�����̃C���X�^���X
HWND window_handle = NULL;		// �E�C���h�E�̃n���h��

#define WINDOW_CLASS_NAME "Talk2EmoteMain"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * ���O�𗬂�
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
 * �f�o�b�O�p�֐�
 */
void DebugFileOutput(std::string str){
	FILE *fp_debug;
	fp_debug = fopen("C:/Talk2Emote/Talk2Emote_Debug_Log.txt", "a");
	fprintf(fp_debug, str.c_str());
	fclose(fp_debug);
}

//--�������e����\��𐄒肷��@�\-----------------------------------------------------------------------------------------------------------------------------
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

//--���������������ɍ���ĕۑ�/��������@�\-----------------------------------------------------------------------------------------------------------------------------

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
 * �e�L�X�g�ƕ\��̎����ɏ�������
 */
void set_str2face(const char* set_str, const char* set_param_tokushu, const char* set_param_kyokusei){
	AddNewDictionary(STR_TO_FACE_DICT_PATH);
	ChangeDictionary(STR_TO_FACE_DICT_PATH);
	SaveAddGeneralItem(set_str, set_param_tokushu, set_param_kyokusei);
}

/**
 * �e�L�X�g�ƕ\��̎�������p�����[�^���擾
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
 * �e�L�X�g�ƕ\��̎�������p�����[�^���擾
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
 * �e�L�X�g��ptkl�̎����ɏ�������
 */
int set_str2ptkl(const char* set_ptkf, const char* set_str, const char* set_ptkl){
	std::string dict_path = set_ptkf;
	dict_path = dict_path + STR2PTKL_TYPE;
	AddNewDictionary(dict_path);
	ChangeDictionary(dict_path);
	SaveAddGeneralItem(set_str, set_ptkl, "-");
}

/**
 * �e�L�X�g��ptkl�̎�������p�����[�^���擾(�e�L�X�g�ɑ΂���ptkl�������ꍇ��""(����0�̕�����)��Ԃ��B)
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
 * �e�L�X�g��ptkl�̎�������p�����[�^���擾(Diff���p)
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
 * �\���ptkl�̎����ɏ������ށistr=[�\�����,�\��ɐ�,�A��], param1=ptkl, param2=[�폜�ς݂̎�:"deleted", �폜����Ă��Ȃ���:"-"]�j
 */
void set_face2ptkl(const char* set_ptkf, const char* set_param_tokushu, const char* set_param_kyokusei, const char* set_ptkl){
	std::string dict_path = set_ptkf;
	dict_path = dict_path + FACE2PTKL_TYPE;
	AddNewDictionary(dict_path);
	ChangeDictionary(dict_path);

	// ����������̃e�L�X�g�����̃x�[�X�Ƃ���
	std::string param_tokushu = set_param_tokushu;
	std::string param_kyokusei = set_param_kyokusei;
	std::string set_str_base = param_tokushu + "," + param_kyokusei;

	// �܂������p�����[�^�A����ptkl�̑g���o�^����Ă��Ȃ������m�F����
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
		// �܂�2�̃p�����[�^�̑g�ɑ΂���ptkl���������ɓo�^����Ă��邩���ׂāA���̒ǉ�����ԍ���t��������str�����
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

		// ���ɐV���ɕ\��ɑ΂���ptkl��ǉ�����B
		SaveAddGeneralItem(set_str.c_str(), set_ptkl, "-");
	}
}

/**
 * �\���ptkl�̎�������ptkl���폜
 */
void delete_face2ptkl(const char* delete_ptkf, const char* delete_param_tokushu, const char* delete_param_kyokusei, const char* delete_ptkl){
	std::string dict_path = delete_ptkf;
	dict_path = dict_path + FACE2PTKL_TYPE;
	AddNewDictionary(dict_path);
	ChangeDictionary(dict_path);

	// ����������̃e�L�X�g�����̃x�[�X�Ƃ���
	std::string param_tokushu = delete_param_tokushu;
	std::string param_kyokusei = delete_param_kyokusei;
	std::string delete_str_base = param_tokushu + "," + param_kyokusei;

	// �܂�2�̃p�����[�^�̑g�ɑ΂���ptkl�̂������ɓo�^����Ă�����̂𒲂ׂāA�폜����ԍ���t��������str�����
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

	// ���ɐV���ɕ\��ɑ΂���ptkl��deleted�ɂ���B
	if(delete_str.size() != 0){
		SaveAddGeneralItem(delete_str.c_str(), "-", "deleted");
	}
}

/**
 * �\���ptkl�̎�������p�����[�^���擾(�V�[�h�l����\��ɑ΂��ĕ�������ptkl�̓��ǂꂩ��Ԃ��悤�ɂ���B)
 */
std::string get_face2ptkl(const char* get_ptkf, const char* get_param_tokushu, const char* get_param_kyokusei, int get_seed){
	std::string dict_path = get_ptkf;
	dict_path = dict_path + FACE2PTKL_TYPE;
	AddNewDictionary(dict_path);
	ChangeDictionary(dict_path);
	
	// ����������̃e�L�X�g�����̃x�[�X�Ƃ���
	std::string param_tokushu = get_param_tokushu;
	std::string param_kyokusei = get_param_kyokusei;
	std::string get_str_base = param_tokushu + "," + param_kyokusei;

	
	// �܂��\��ɑ΂���ptkl��vector<string>���쐬����B
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

	// ���ɏ�Lvector�̂��� [�V�[�h�l] % [ptkl�̐�] �Ԗڂ�ptkl��Ԃ��B
	if(ptkl_list.size() > 0){
		//return debug_text;	//Debug
		return ptkl_list[get_seed % ptkl_list.size()];
	}else{
		return "";
	}
}

/**
 * �\��ꗗ�̎����ɕ\����������ށistr=[�\��p�����[�^,["tokushu" | "kyokusei"]], param1="-", param2=["deleted" | "-"]�j
 */
void set_face(const char* set_param, const char* set_param_meta){
	AddNewDictionary(FACE_DICT_PATH);
	ChangeDictionary(FACE_DICT_PATH);
	std::string string_set_param_meta = set_param_meta;
	std::string string_set_param = set_param;
	std::string str_key = string_set_param + "," + string_set_param_meta;
	
	// set_param_meta�̒l�ɉ����ĕ\��p�����[�^����������
	if(strlen(set_param) > 0){
		if(string_set_param_meta == "tokushu" || string_set_param_meta == "kyokusei"){
			SaveAddGeneralItem(str_key.c_str(), "-", "-");
		}
	}
}

/**
 * �\��ꗗ�̎�������\����폜�istr=[�\��p�����[�^,["tokushu" | "kyokusei"]], param1=�\��p�����[�^, param2=["deleted" | "-"]�j
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
 * �\��ꗗ�̎�������\����擾(lua�Ƀe�[�u����Ԃ������ɂȂ�...�̂���)
 */
std::vector<std::string> get_face(const char* get_param_meta){
	AddNewDictionary(FACE_DICT_PATH);
	ChangeDictionary(FACE_DICT_PATH);
	std::string string_get_param_meta = get_param_meta;

	// �e�[�u��������Ă���
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
			// param��param_meta���ʂɎ��o��
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

			// param_meta�����]�̕���������ȉ����s��
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

// DLL�̃G���g���[�|�C���g
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
		// �E�C���h�E�����݂���ꍇ�ɔj������
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

// �E�C���h�E���쐬����֐�
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
 * Input  : �e�L�X�g�̃��C���ԍ�, �Z���t������, ptkf, �擾����p�����[�^���(ptkl | face_tokushu | face_kyokusei)
 * Output : �擾�������������ǂ���, �p�����[�^
 * �e�L�X�g�̃��C���ԍ��ƃZ���t�����񂪕ۑ�����Ă��镨�ƍ��v���Ȃ��ꍇ�A�擾�������������ǂ�����false��Ԃ��B
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
 * Input  : �e�L�X�g�̃��C���ԍ�, �Z���t������, ptkf, ptkl_by_str, is_word, is_ban_list, diff_ratio
 * Output : �Ȃ�
 * �e�L�X�g�̃��C���ԍ��ƃZ���t�����񂪕ۑ�����Ă��镨�ƍ��v���Ȃ��ꍇ�A�e�L�X�g�̃��C���ԍ��ƃZ���t��������X�V������ŁA���̏��̓N���A����B�{�^����������UI��őI�������\������N���A����B
 * ���V�����\��p�����[�^���ǉ����ꂽ���ǂ�����lua���ł̔�����s���A�����ɓo�^����B������ł����Ⴒ����l���Ȃ��B
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

	// holddata_�Z�Z���X�V����
	holddata_layer_num = set_layer;
	holddata_str = str_set_str;
	holddata_ptkf = str_set_ptkf;
	if(str_set_ptkl_by_str.size() != 0){
		set_str2ptkl(str_set_ptkf.c_str(), str_set_str.c_str(), str_set_ptkl_by_str.c_str());	// str2ptkl�����ɑ΂��ĕۑ�������s��
		updateLogs(str_set_ptkf + " : [save] " + str_set_str + " -> " + str_set_ptkl_by_str);
	}
	holddata_is_word = set_is_word;
	holddata_is_ban_list = set_is_ban_list;
	holddata_diff_ratio = set_diff_ratio;
	holddata_ptkl_seed = set_ptkl_seed;


	// ���_�⎫������������蒼���B(str2face, str2ptkl, face2ptkl���s��)
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
	set_text = R"(���C���F)" + std::to_string(holddata_layer_num);
	SetWindowText(hwnd_layernum_text, set_text.c_str());
	windowdata_layer_num = holddata_layer_num;

	set_text = R"(is_word�F)" + std::to_string(holddata_is_word);
	SetWindowText(hwnd_is_word_text, set_text.c_str());
	windowdata_is_word = holddata_is_word;

	set_text = R"(is_ban_list�F)" + std::to_string(holddata_is_ban_list);
	SetWindowText(hwnd_is_ban_list_text, set_text.c_str());
	windowdata_is_ban_list = holddata_is_ban_list;

	set_text = R"(diff_ratio�F)" + std::to_string(holddata_diff_ratio);
	SetWindowText(hwnd_diff_ratio_text, set_text.c_str());
	windowdata_diff_ratio = holddata_diff_ratio;

	set_text = R"(ptkl_seed�F)" + std::to_string(holddata_ptkl_seed);
	SetWindowText(hwnd_ptkl_seed_text, set_text.c_str());
	windowdata_ptkl_seed = holddata_ptkl_seed;

	set_text = R"(�������e�F)" + holddata_str;
	SetWindowText(hwnd_str_text, set_text.c_str());
	windowdata_str = holddata_str;

	set_text = R"(ptkf�F)" + holddata_ptkf;
	SetWindowText(hwnd_ptkf_text, set_text.c_str());
	windowdata_ptkf = holddata_ptkf;

	set_text = R"(ptkl(�����l)�F)" + holddata_ptkl_by_str;
	SetWindowText(hwnd_ptkl_by_str_text, set_text.c_str());
	windowdata_ptkl_by_str = holddata_ptkl_by_str;

	set_text = R"(ptkl(�\��l)�F)" + holddata_ptkl_by_face;
	SetWindowText(hwnd_ptkl_by_face_text, set_text.c_str());
	windowdata_ptkl_by_face = holddata_ptkl_by_face;

	set_text = R"(�ۑ��l�F)" + holddata_face_tokushu_saved;
	SetWindowText(hwnd_face_tokushu_saved_text, set_text.c_str());
	windowdata_face_tokushu_saved = holddata_face_tokushu_saved;

	set_text = R"(����l�F)" + holddata_face_tokushu_predict;
	SetWindowText(hwnd_face_tokushu_predict_text, set_text.c_str());
	windowdata_face_tokushu_predict = holddata_face_tokushu_predict;

	set_text = R"(�ۑ��l�F)" + holddata_face_kyokusei_saved;
	SetWindowText(hwnd_face_kyokusei_saved_text, set_text.c_str());
	windowdata_face_kyokusei_saved = holddata_face_kyokusei_saved;

	set_text = R"(����l�F)" + holddata_face_kyokusei_predict;
	SetWindowText(hwnd_face_kyokusei_predict_text, set_text.c_str());
	windowdata_face_kyokusei_predict = holddata_face_kyokusei_predict;
}

/**
 * Input  : �Ȃ�
 * Output : �Ȃ�
 * ���̏�Ԃ�holddata_�Z�Z�ŃE�C���h�E���X�V����B
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

// ���͗ނ̃R���g���[��ID
#define FACE_TOKUSHU_COMBOBOX 1001
#define FACE_TOKUSHU_EDIT 1002
#define FACE_KYOKUSEI_COMBOBOX 1003
#define FACE_KYOKUSEI_EDIT 1004
#define FACE2PTKL_DELETE_BUTTON 1005
#define STR2FACE_SAVE_BUTTON 1006
#define FACE2PTKL_SAVE_BUTTON 1007
#define STR2FACE2PTKL_SAVE_BUTTON 1008

// �E�C���h�E�̂ɗ������b�Z�[�W���̏���
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:{
		window_handle = hWnd;
		WaitForSingleObject(mutex_handle, INFINITE);

		//--Log-------------------------------------------------------------------
		hwnd_debug_text = CreateWindow(
			"EDIT",							// �ÓI�e�L�X�g�N���X��
			"Log:",								// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE | ES_READONLY,	// �X�^�C��
			10, 20+WINDOW_TEXT_HEIGHT*7, 860, LOG_SPACE_HEIGHT,					// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,								// �e�E�C���h�E�̃n���h��
			NULL,								// �R���g���[����ID
			NULL,								// �C���X�^���X�n���h��
			NULL								// �{�^���̒ǉ��p�����[�^
		);
		//--------------------------------------------------------------------------


		std::string text_buf;
		text_buf = R"(���C���F)" + std::to_string(holddata_layer_num);
		hwnd_layernum_text = CreateWindow(
			"STATIC",							// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),					// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,	// �X�^�C��
			10, 10, 172, WINDOW_TEXT_HEIGHT,			// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,								// �e�E�C���h�E�̃n���h��
			NULL,								// �R���g���[����ID
			NULL,								// �C���X�^���X�n���h��
			NULL								// �{�^���̒ǉ��p�����[�^
		);

		text_buf = R"(is_word�F)" + std::to_string(holddata_is_word);
		hwnd_is_word_text = CreateWindow(
			"STATIC",							// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),					// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,	// �X�^�C��
			(10+172), 10, 172, WINDOW_TEXT_HEIGHT,		// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,								// �e�E�C���h�E�̃n���h��
			NULL,								// �R���g���[����ID
			NULL,								// �C���X�^���X�n���h��
			NULL								// �{�^���̒ǉ��p�����[�^
		);

		text_buf = R"(is_ban_list�F)" + std::to_string(holddata_is_ban_list);
		hwnd_is_ban_list_text = CreateWindow(
			"STATIC",							// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),					// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,	// �X�^�C��
			(10+172*2), 10, 172, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,								// �e�E�C���h�E�̃n���h��
			NULL,								// �R���g���[����ID
			NULL,								// �C���X�^���X�n���h��
			NULL								// �{�^���̒ǉ��p�����[�^
		);

		text_buf = R"(diff_ratio�F)" + std::to_string(holddata_diff_ratio);
		hwnd_diff_ratio_text = CreateWindow(
			"STATIC",							// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),					// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,	// �X�^�C��
			(10+172*3), 10, 172, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,								// �e�E�C���h�E�̃n���h��
			NULL,								// �R���g���[����ID
			NULL,								// �C���X�^���X�n���h��
			NULL								// �{�^���̒ǉ��p�����[�^
		);

		text_buf = R"(ptkl_seed�F)" + std::to_string(holddata_ptkl_seed);
		hwnd_ptkl_seed_text = CreateWindow(
			"STATIC",							// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),					// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,	// �X�^�C��
			(10+172*4), 10, 172, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,								// �e�E�C���h�E�̃n���h��
			NULL,								// �R���g���[����ID
			NULL,								// �C���X�^���X�n���h��
			NULL								// �{�^���̒ǉ��p�����[�^
		);

		text_buf = R"(�������e�F)" + holddata_str;
		hwnd_str_text = CreateWindow(
			"STATIC",								// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),						// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// �X�^�C��
			10, 10+WINDOW_TEXT_HEIGHT, 860, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,									// �e�E�C���h�E�̃n���h��
			NULL,									// �R���g���[����ID
			NULL,									// �C���X�^���X�n���h��
			NULL									// �{�^���̒ǉ��p�����[�^
		);

		text_buf = R"(ptkf�F)" + holddata_ptkf;
		hwnd_ptkf_text = CreateWindow(
			"STATIC",								// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),						// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// �X�^�C��
			10, 10+WINDOW_TEXT_HEIGHT*2, 860, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,									// �e�E�C���h�E�̃n���h��
			NULL,									// �R���g���[����ID
			NULL,									// �C���X�^���X�n���h��
			NULL									// �{�^���̒ǉ��p�����[�^
		);

		text_buf = R"(ptkl(�����l)�F)" + holddata_ptkl_by_str;
		hwnd_ptkl_by_str_text = CreateWindow(
			"STATIC",								// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),						// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// �X�^�C��
			10, 10+WINDOW_TEXT_HEIGHT*3, 700, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,									// �e�E�C���h�E�̃n���h��
			NULL,									// �R���g���[����ID
			NULL,									// �C���X�^���X�n���h��
			NULL									// �{�^���̒ǉ��p�����[�^
		);

		text_buf = R"(ptkl(�\��l)�F)" + holddata_ptkl_by_face;
		hwnd_ptkl_by_face_text = CreateWindow(
			"STATIC",								// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),						// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// �X�^�C��
			10, 10+WINDOW_TEXT_HEIGHT*4, 700, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,									// �e�E�C���h�E�̃n���h��
			NULL,									// �R���g���[����ID
			NULL,									// �C���X�^���X�n���h��
			NULL									// �{�^���̒ǉ��p�����[�^
		);

		hwnd_face_tokushu_text = CreateWindow(
			"STATIC",								// �ÓI�e�L�X�g�N���X��
			R"(�\�����F)",						// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// �X�^�C��
			10, 10+WINDOW_TEXT_HEIGHT*5, 100, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,									// �e�E�C���h�E�̃n���h��
			NULL,									// �R���g���[����ID
			NULL,									// �C���X�^���X�n���h��
			NULL									// �{�^���̒ǉ��p�����[�^
		);

		hwnd_face_tokushu_combobox = CreateWindow(
			"COMBOBOX",									// �ÓI�e�L�X�g�N���X��
			NULL,										// �����e�L�X�g
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,	// �X�^�C���i���X�g���h���b�v�_�E���`���ŕ\���j
			110, 10+WINDOW_TEXT_HEIGHT*5, 100, 200,			// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,										// �e�E�C���h�E�̃n���h��
			(HMENU)FACE_TOKUSHU_COMBOBOX,				// �R���g���[����ID
			application_instance,						// �C���X�^���X�n���h��
			NULL										// �{�^���̒ǉ��p�����[�^
		);

		SendMessage(hwnd_face_tokushu_combobox, CB_ADDSTRING, (LPARAM)0, (LPARAM)"auto");
		// �\�����̈ꗗ���擾
		holddata_face_tokushu_list = get_face("tokushu");
		for (int i=0; i<holddata_face_tokushu_list.size(); i++){
			SendMessage(hwnd_face_tokushu_combobox, CB_ADDSTRING, (LPARAM)0, (LPARAM)(holddata_face_tokushu_list[i].c_str()));
		}
		SendMessage(hwnd_face_tokushu_combobox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		// tokushu_classes�ɓ����Ă���v���O�C���̓��X�g�Ɋ܂߂�
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
			"EDIT",												// �ÓI�e�L�X�g�N���X��
			NULL,												// �����e�L�X�g
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,	// �X�^�C���i�g���Ǝ��������X�N���[���j
			210, 10+WINDOW_TEXT_HEIGHT*5, 100, WINDOW_TEXT_HEIGHT,			// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,												// �e�E�C���h�E�̃n���h��
			(HMENU)FACE_TOKUSHU_EDIT,							// �R���g���[����ID
			application_instance,								// �C���X�^���X�n���h��
			NULL												// �{�^���̒ǉ��p�����[�^
		);

		text_buf = R"(�ۑ��l�F)" + holddata_face_tokushu_saved;
		hwnd_face_tokushu_saved_text = CreateWindow(
			"STATIC",								// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),						// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// �X�^�C��
			310, 10+WINDOW_TEXT_HEIGHT*5, 200, WINDOW_TEXT_HEIGHT,// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,									// �e�E�C���h�E�̃n���h��
			NULL,									// �R���g���[����ID
			NULL,									// �C���X�^���X�n���h��
			NULL									// �{�^���̒ǉ��p�����[�^
		);
		
		text_buf = R"(����l�F)" + holddata_face_tokushu_predict;
		hwnd_face_tokushu_predict_text = CreateWindow(
			"STATIC",								// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),						// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// �X�^�C��
			510, 10+WINDOW_TEXT_HEIGHT*5, 200, WINDOW_TEXT_HEIGHT,// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,									// �e�E�C���h�E�̃n���h��
			NULL,									// �R���g���[����ID
			NULL,									// �C���X�^���X�n���h��
			NULL									// �{�^���̒ǉ��p�����[�^
		);

		hwnd_face_kyokusei_text = CreateWindow(
			"STATIC",								// �ÓI�e�L�X�g�N���X��
			R"(�\��ɐ��F)",						// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// �X�^�C��
			10, 10+WINDOW_TEXT_HEIGHT*6, 100, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,									// �e�E�C���h�E�̃n���h��
			NULL,									// �R���g���[����ID
			NULL,									// �C���X�^���X�n���h��
			NULL									// �{�^���̒ǉ��p�����[�^
		);

		hwnd_face_kyokusei_combobox = CreateWindow(
			"COMBOBOX",									// �ÓI�e�L�X�g�N���X��
			NULL,										// �����e�L�X�g
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,	// �X�^�C���i���X�g���h���b�v�_�E���`���ŕ\���j
			110, 10+WINDOW_TEXT_HEIGHT*6, 100, 200,			// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,										// �e�E�C���h�E�̃n���h��
			(HMENU)FACE_KYOKUSEI_COMBOBOX,				// �R���g���[����ID
			application_instance,						// �C���X�^���X�n���h��
			NULL										// �{�^���̒ǉ��p�����[�^
		);

		SendMessage(hwnd_face_kyokusei_combobox, CB_ADDSTRING, (LPARAM)0, (LPARAM)"auto");
		// �\��ɐ��̈ꗗ���擾
		holddata_face_kyokusei_list = get_face("kyokusei");
		for (int i=0; i<holddata_face_kyokusei_list.size(); i++){
			SendMessage(hwnd_face_kyokusei_combobox, CB_ADDSTRING, (LPARAM)0, (LPARAM)(holddata_face_kyokusei_list[i].c_str()));
		}
		SendMessage(hwnd_face_kyokusei_combobox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		// kyokusei_classes�ɓ����Ă���kyokusei�̃v���O�C���̓��X�g�Ɋ܂߂�
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
			"EDIT",												// �ÓI�e�L�X�g�N���X��
			NULL,												// �����e�L�X�g
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,	// �X�^�C���i�g���Ǝ��������X�N���[���j
			210, 10+WINDOW_TEXT_HEIGHT*6, 100, WINDOW_TEXT_HEIGHT,			// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,												// �e�E�C���h�E�̃n���h��
			(HMENU)FACE_KYOKUSEI_EDIT,							// �R���g���[����ID
			application_instance,								// �C���X�^���X�n���h��
			NULL												// �{�^���̒ǉ��p�����[�^
		);

		text_buf = R"(�ۑ��l�F)" + holddata_face_kyokusei_saved;
		hwnd_face_kyokusei_saved_text = CreateWindow(
			"STATIC",								// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),						// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// �X�^�C��
			310, 10+WINDOW_TEXT_HEIGHT*6, 200, WINDOW_TEXT_HEIGHT,// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,									// �e�E�C���h�E�̃n���h��
			NULL,									// �R���g���[����ID
			NULL,									// �C���X�^���X�n���h��
			NULL									// �{�^���̒ǉ��p�����[�^
		);

		text_buf = R"(����l�F)" + holddata_face_kyokusei_predict;
		hwnd_face_kyokusei_predict_text = CreateWindow(
			"STATIC",								// �ÓI�e�L�X�g�N���X��
			text_buf.c_str(),						// �����e�L�X�g
			WS_VISIBLE | WS_CHILD | SS_LEFT,		// �X�^�C��
			510, 10+WINDOW_TEXT_HEIGHT*6, 200, WINDOW_TEXT_HEIGHT,// �ʒu�ƃT�C�Y(x, y, width, height)
			hWnd,									// �e�E�C���h�E�̃n���h��
			NULL,									// �R���g���[����ID
			NULL,									// �C���X�^���X�n���h��
			NULL									// �{�^���̒ǉ��p�����[�^
		);

		hwnd_face2ptkl_delete_button = CreateWindow(
			"BUTTON",									// �{�^���N���X��
			R"(face2ptkl���폜)",						// �{�^���̃e�L�X�g
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,		// �X�^�C��
			710, 10+WINDOW_TEXT_HEIGHT*3, 160, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y
			hWnd,										// �e�E�C���h�E�̃n���h��
			(HMENU)FACE2PTKL_DELETE_BUTTON,				// �R���g���[����ID
			application_instance,						// �C���X�^���X�n���h��
			NULL										// �{�^���̒ǉ��p�����[�^
		);

		hwnd_face2ptkl_save_button = CreateWindow(
			"BUTTON",									// �{�^���N���X��
			R"(face2ptkl��ۑ�)",						// �{�^���̃e�L�X�g
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,		// �X�^�C��
			710, 10+WINDOW_TEXT_HEIGHT*4, 160, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y
			hWnd,										// �e�E�C���h�E�̃n���h��
			(HMENU)FACE2PTKL_SAVE_BUTTON,				// �R���g���[����ID
			application_instance,						// �C���X�^���X�n���h��
			NULL										// �{�^���̒ǉ��p�����[�^
		);

		hwnd_str2face_save_button = CreateWindow(
			"BUTTON",									// �{�^���N���X��
			R"(str2face��ۑ�)",						// �{�^���̃e�L�X�g
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,		// �X�^�C��
			710, 10+WINDOW_TEXT_HEIGHT*5, 160, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y
			hWnd,										// �e�E�C���h�E�̃n���h��
			(HMENU)STR2FACE_SAVE_BUTTON,				// �R���g���[����ID
			application_instance,						// �C���X�^���X�n���h��
			NULL										// �{�^���̒ǉ��p�����[�^
		);

		hwnd_str2face2ptkl_save_button = CreateWindow(
			"BUTTON",									// �{�^���N���X��
			R"(str2face2ptkl��ۑ�)",					// �{�^���̃e�L�X�g
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,		// �X�^�C��
			710, 10+WINDOW_TEXT_HEIGHT*6, 160, WINDOW_TEXT_HEIGHT,	// �ʒu�ƃT�C�Y
			hWnd,										// �e�E�C���h�E�̃n���h��
			(HMENU)STR2FACE2PTKL_SAVE_BUTTON,			// �R���g���[����ID
			application_instance,						// �C���X�^���X�n���h��
			NULL										// �{�^���̒ǉ��p�����[�^
		);

		ReleaseMutex(mutex_handle);
		break;
	}
	case WM_COMMAND:{	// �{�^�����N���b�N���ꂽ�Ƃ��̏���
		// TODO:windowdata_�Z�Z���Q��/�X�V����B
		if ((LOWORD(wParam) == FACE2PTKL_DELETE_BUTTON) || (LOWORD(wParam) == FACE2PTKL_SAVE_BUTTON) || (LOWORD(wParam) == STR2FACE_SAVE_BUTTON) || (LOWORD(wParam) == STR2FACE2PTKL_SAVE_BUTTON)){
			WaitForSingleObject(mutex_handle, INFINITE);
			// EDIT��������COMBOBOX����kyokusei, tokushu�̒l���擾����B�܂��AEDIT���g�����ꍇ�͂�����L�^�itokushu_edit_flag, kyokusei_edit_flag�j
			bool tokushu_edit_flag = true;
			bool kyokusei_edit_flag = true;
			int selected_tokushu_index;
			int selected_kyokusei_index;
			TCHAR selected_tokushu_item[256];
			std::string str_selected_tokushu_item;
			TCHAR selected_kyokusei_item[256];
			std::string str_selected_kyokusei_item;
			
			// �\�����̓��͒l�擾
			selected_tokushu_index = (int)SendMessage(hwnd_face_tokushu_combobox, CB_GETCURSEL, 0, 0);
			GetWindowText(hwnd_face_tokushu_edit, selected_tokushu_item, sizeof(selected_tokushu_item) / sizeof(TCHAR));
			if(_tcslen(selected_tokushu_item) == 0){
				SendMessage(hwnd_face_tokushu_combobox, CB_GETLBTEXT, (WPARAM)selected_tokushu_index, (LPARAM)selected_tokushu_item);
				tokushu_edit_flag = false;
			}
			str_selected_tokushu_item = selected_tokushu_item;

			// �\��ɐ��̓��͒l�擾
			selected_kyokusei_index = (int)SendMessage(hwnd_face_kyokusei_combobox, CB_GETCURSEL, 0, 0);
			GetWindowText(hwnd_face_kyokusei_edit, selected_kyokusei_item, sizeof(selected_kyokusei_item) / sizeof(TCHAR));
			if(_tcslen(selected_kyokusei_item) == 0){
				SendMessage(hwnd_face_kyokusei_combobox, CB_GETLBTEXT, (WPARAM)selected_kyokusei_index, (LPARAM)selected_kyokusei_item);
				kyokusei_edit_flag = false;
			}
			str_selected_kyokusei_item = selected_kyokusei_item;

			// �\��̕ۑ��l���X�V
			if((LOWORD(wParam) == STR2FACE_SAVE_BUTTON) || (LOWORD(wParam) == STR2FACE2PTKL_SAVE_BUTTON)){
				windowdata_face_tokushu_saved = str_selected_tokushu_item;
				windowdata_face_kyokusei_saved = str_selected_kyokusei_item;
				holddata_face_tokushu_saved = str_selected_tokushu_item;
				holddata_face_kyokusei_saved = str_selected_kyokusei_item;
			}

			// ����⎫����������\��̒l�����߂�
			std::string temp_tokushu = windowdata_face_tokushu_saved;
			std::string temp_kyokusei = windowdata_face_kyokusei_saved;
			if(windowdata_face_tokushu_saved == "auto"){
				temp_tokushu = windowdata_face_tokushu_predict;
			}
			if(windowdata_face_kyokusei_saved == "auto"){
				temp_kyokusei = windowdata_face_kyokusei_predict;
			}

			// �����ւ̕ۑ�
			switch (LOWORD(wParam)){
				case FACE2PTKL_DELETE_BUTTON:
					delete_face2ptkl(windowdata_ptkf.c_str(), temp_tokushu.c_str(), temp_kyokusei.c_str(), windowdata_ptkl_by_face.c_str());	// face2ptkl�����ɑ΂��č폜������s��
					updateLogs(windowdata_ptkf + " : [delete] ( " + temp_tokushu + " , " + temp_kyokusei + " ) -> " + windowdata_ptkl_by_face);
					break;
				case FACE2PTKL_SAVE_BUTTON:
					set_face2ptkl(windowdata_ptkf.c_str(), temp_tokushu.c_str(), temp_kyokusei.c_str(), windowdata_ptkl_by_str.c_str());	// face2ptkl�����ɑ΂��ĕۑ�������s��
					updateLogs(windowdata_ptkf + " : [save] ( " + temp_tokushu + " , " + temp_kyokusei + " ) -> " + windowdata_ptkl_by_face);
					break;
				case STR2FACE_SAVE_BUTTON:
					set_str2face(windowdata_str.c_str(), windowdata_face_tokushu_saved.c_str(), windowdata_face_kyokusei_saved.c_str());	// str2face�����ɑ΂��ĕۑ�������s��
					updateLogs(windowdata_ptkf + " : [save] " + windowdata_str + " -> ( " + windowdata_face_tokushu_saved + " , " + windowdata_face_kyokusei_saved + " )");
					break;
				case STR2FACE2PTKL_SAVE_BUTTON:
					set_str2face(windowdata_str.c_str(), windowdata_face_tokushu_saved.c_str(), windowdata_face_kyokusei_saved.c_str());	// str2face�����ɑ΂��ĕۑ�������s��
					set_face2ptkl(windowdata_ptkf.c_str(), temp_tokushu.c_str(), temp_kyokusei.c_str(), windowdata_ptkl_by_str.c_str());	// face2ptkl�����ɑ΂��ĕۑ�������s��
					updateLogs(windowdata_ptkf + " : [save] ( " + temp_tokushu + " , " + temp_kyokusei + " ) -> " + windowdata_ptkl_by_face);
					updateLogs(windowdata_ptkf + " : [save] " + windowdata_str + " -> ( " + windowdata_face_tokushu_saved + " , " + windowdata_face_kyokusei_saved + " )");
					break;
			}

			// windowdata_ptkl_by_face���X�V(�����������s��)
			windowdata_ptkl_by_face = get_face2ptkl(windowdata_ptkf.c_str(), temp_tokushu.c_str(), temp_kyokusei.c_str(), windowdata_ptkl_seed);
			//holddata_ptkl_by_face = get_face2ptkl(windowdata_ptkf.c_str(), temp_tokushu.c_str(), temp_kyokusei.c_str(), windowdata_ptkl_seed);

			// holddata�����̃L�����N�^�[�̕��ɂ���
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

			//�E�C���h�E�̒l��F�X�X�V����B
			UpdateWindowRow();

			// �\��ꗗ�Ɋւ��ẮA�����҂��ƂɈقȂ炸�A�S�Ă̔����ҋ��ʂ̂��̂�p����̂ŁA�E�C���h�E�͂��̃^�C�~���O�ł����X�V���Ȃ�
			// �\��ꗗ�����ւ̒ǉ����s��
			// holddata_face_�Z�Z_list���X�V
			// COMBOBOX�̍��ڂ��ǉ�
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
		// �E�C���h�E���j�����ꂽ�Ƃ��ɃN���X���̓o�^����������
		if (window_handle != NULL)
		{
			DestroyWindow(window_handle); // �E�C���h�E��j������
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
 * ��������������������Ă�����������������~���Y��ɂ���B
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
				//���p�����̏ꍇ
				if(raw_c_str[i] != '\n'){	//���s�͖���
					buf[buf_ptr] = raw_c_str[i];
					buf_ptr++;
				}
				i++;
			}else{
				//�S�p�����̏ꍇ
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
		if(dictionary_memory_instance == nullptr){	// �\��������@�\�̏�����
			dictionary_memory_instance = LoadLibrary("C:/Talk2Emote/dictionary_memory.dll");
			AddNewDictionary = (AddNewDictionaryFunc)GetProcAddress(dictionary_memory_instance, "AddNewDictionary");
			ChangeDictionary = (ChangeDictionaryFunc)GetProcAddress(dictionary_memory_instance, "ChangeDictionary");
			SaveAddGeneralItem = (SaveAddGeneralItemFunc)GetProcAddress(dictionary_memory_instance, "SaveAddGeneralItem");
			SearchGeneralParam = (SearchGeneralParamFunc)GetProcAddress(dictionary_memory_instance, "SearchGeneralParam");
			GetGeneralParam = (GetGeneralParamFunc)GetProcAddress(dictionary_memory_instance, "GetGeneralParam");
			DiffSearchGeneralParam = (DiffSearchGeneralParamFunc)GetProcAddress(dictionary_memory_instance, "DiffSearchGeneralParam");
		}
		if (face_predict_instance == nullptr){		// �\���@�\�̏�����
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

	// Lua�ł���DLL���Ăт�������Ԃ̃I�u�W�F�N�g���p�ς݂ɂȂ������ɌĂ΂��A�͂��B
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