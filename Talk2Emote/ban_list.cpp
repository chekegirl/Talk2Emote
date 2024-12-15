#include <Windows.h>
#include <stdio.h>
#include <string>
#include <set>
#include <map>
#include <vector>
#include "diff_type.h"

/**
 * ※ const char* str1, const char* str2は現段階では使っていないが、今後の拡張性を考えて引数に持たせてある。この2つの文字列を比較する方法も視野に入れている。
 * （word2vecみたいなのを使って類似度を確かめたりできそうだと思っている。もしくはface_predict_pluginsの推定結果が一致するかどうか、とかもできると思う。face_predict_pluginsの中身自体が高度な処理になっていないけど。）
 * Input  : 単語毎かどうか, Diffによる比較結果, 文字列1, 文字列2
 * Output : 文字列1と2を同一視することに問題があるか（あればtrue）
 */
extern "C" __declspec(dllexport) bool __cdecl BanListJudge(bool is_word, std::map<DiffType, std::vector<std::vector<std::string>>> diff_map, const char* str1, const char* str2){
	std::set<std::string> ban_list;
	std::set<std::set<std::string>> swap_able_pair;
	if(is_word){
		ban_list = {"良い", "いい", "悪い", "わるい", "無い", "非", "不", "ない", "できる", "すぎる", "過ぎる", "何故", "なぜ", "何で", "なんで", "どうして", "どう", "いくつ", "かねる", "損ねる", "逃す"};
		swap_able_pair = {{"良い","いい"},{"無い","ない"},{"過ぎる","すぎる"},{"何故","なぜ"},{"何で", "なんで"},{"どうして", "どう"},{"非", "不"},{"悪い", "わるい"},{"損ねる", "逃す"}};
	}else{
		ban_list = {"いい", "良い", "ない", "しなくも", "なくも", "なく", "なかっ", "なか", "かった", "かったか", "すぎ", "すぎる", "すぎた", "過ぎる", "過ぎ", "過ぎた", "てくれ", "しよう", "してくれ", "くれ", "よう", "がしなくもない", "しなくもない", "なくもない", "くもない", "もない", "なかった", "もなかった"};
		swap_able_pair = {{"良い","いい"},{"ない","なかった"},{"す","過"}};
	}
	bool ban_flag = false;
	// Insertされた文字列の中に挿入削除置換禁止の文字列が含まれるかどうか
	for (int j=0; j<diff_map[DiffType::Insert].size(); j++){
		if(ban_flag){
			break;
		}
		if(ban_list.find(diff_map[DiffType::Insert][j][0]) != ban_list.end()){
			ban_flag = true;
		}
	}
	// Deleteされた文字列の中に挿入削除置換禁止の文字列が含まれるかどうか
	for (int j=0; j<diff_map[DiffType::Delete].size(); j++){
		if(ban_flag){
			break;
		}
		if(ban_list.find(diff_map[DiffType::Delete][j][0]) != ban_list.end()){
			ban_flag = true;
		}
	}
	// Swapされた文字列の中に挿入削除置換禁止の文字列が含まれるかどうか
	for (int j=0; j<diff_map[DiffType::Swap].size(); j++){
		if(ban_flag){
			break;
		}
		for(int k=0; k<ban_list.size(); k++){
			if(ban_flag){
				break;
			}
			std::set<std::string> one_swap = {diff_map[DiffType::Swap][j][0], diff_map[DiffType::Swap][j][1]};
			if(swap_able_pair.find(one_swap) == swap_able_pair.end()){
				if(ban_list.find(diff_map[DiffType::Swap][j][0]) != ban_list.end()){
					ban_flag = true;
				}
				if(ban_list.find(diff_map[DiffType::Swap][j][1]) != ban_list.end()){
					ban_flag = true;
				}
			}
		}
	}
	return ban_flag;
}