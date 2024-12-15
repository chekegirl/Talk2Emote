#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <regex>
#include <vector>
#include <iomanip>
#include <sstream>

extern "C" {
	__declspec(dllexport) std::string __cdecl predict_odoroki(std::string utf8_str_match_text) {
		//ビックリハテナ符
		std::string bikkuri_hu = "！|\\!";
		std::string hatena_hu = "？|\\?";
		std::string bikuhate_hu1 = "((" + bikkuri_hu + "|" + hatena_hu + ")*(" + bikkuri_hu + ")+(" + hatena_hu + ")+(" + bikkuri_hu + "|" + hatena_hu + ")*)";
		std::string bikuhate_hu2 = "((" + bikkuri_hu + "|" + hatena_hu + ")*(" + hatena_hu + ")+(" + bikkuri_hu + ")+(" + bikkuri_hu + "|" + hatena_hu + ")*)";
		std::string bikuhate_hu = "(" + bikuhate_hu1 + "|" + bikuhate_hu2 + ")";
		std::regex bikuhate_hu_regex(bikuhate_hu);

		std::string one_bikuhate_hu = "((" + bikkuri_hu + "|" + hatena_hu + ")*(" + hatena_hu + ")+(" + bikkuri_hu + "|" + hatena_hu + ")*)";

		//感嘆の発声
		std::string kantan_hassei = "((っ)+" + one_bikuhate_hu + ")";
		std::regex kantan_hassei_regex(kantan_hassei);

		//探索する正規表現リスト
		std::vector<std::regex> patterns = {bikuhate_hu_regex, kantan_hassei_regex};
		
		for(int i=0; i<patterns.size(); i++){
			if(std::regex_search(utf8_str_match_text, patterns[i])){
				return "true";
			}
		}
		return "false";
	}
}