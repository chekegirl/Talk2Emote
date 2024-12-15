#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <regex>
#include <vector>
#include <iomanip>
#include <sstream>

extern "C" {
	__declspec(dllexport) std::string __cdecl predict_negaposi(std::string utf8_str_match_text) {
		std::vector<std::string> first_posi_words = {"嬉し", "うれし", "楽し", "たのし", "美味し", "おいし", "出来", "でき", "すご", "凄", "良", "いい", "よく", "よい", "綺麗", "きれい", "作れ", "素晴ら", "美い", "頑張", "成功", "完成", "積極", "上手", "丁寧", "安定", "知り", "知ら", "知っ", "向いて", "快適", "ちゃんと"};
		std::vector<std::string> first_nega_words = {"苦し", "悲し", "寂し", "残念", "失敗", "壊れ", "つまら", "消極的", "下手", "雑", "ダメ", "だめ", "駄目", "崩れ", "悪", "わる", "過ぎ", "すぎ", "まって", "まう", "間違い", "不安定", "物足りな", "気づかな", "気づきません", "気づかず", "忘れ", "汚い", "汚か", "分からな", "分からん", "疲れ", "つかれ", "ダル", "だる", "ウザ", "うざ", "しんど"};
		std::vector<std::string> second_nega_words = {"けど", "でも", "とはいえ", "とは言え", "ても", "とて", "だが", "るが"};
		std::vector<std::string> third_nega_words = {"なかった", "ない", "ません", "ではない", "なさそう", "なさげ"};

		std::string first_posi_words_or = first_posi_words[0];
		for (int i=1; i<first_posi_words.size(); i++){
			first_posi_words_or = first_posi_words_or + "|" + first_posi_words[i];
		}
		std::string kakko_first_posi_words_or = "(" + first_posi_words_or + ")";

		std::string first_nega_words_or = first_nega_words[0];
		for (int i=1; i<first_nega_words.size(); i++){
			first_nega_words_or = first_nega_words_or + "|" + first_nega_words[i];
		}
		std::string kakko_first_nega_words_or = "(" + first_nega_words_or + ")";
		
		std::string second_nega_words_or = second_nega_words[0];
		for (int i=1; i<second_nega_words.size(); i++){
			second_nega_words_or = second_nega_words_or + "|" + second_nega_words[i];
		}
		std::string kakko_second_nega_words_or = "(" + second_nega_words_or + ")";

		std::string third_nega_words_or = third_nega_words[0];
		for (int i=1; i<third_nega_words.size(); i++){
			third_nega_words_or = third_nega_words_or + "|" + third_nega_words[i];
		}
		std::string kakko_third_nega_words_or = "(" + third_nega_words_or + ")";

		std::string bun_shuutan_moji = R"(\s|。|、|((\.\.\.)(\.)*)|…|！|\!|？|\?|___EOS___)";
		
		std::string nega_posi_shuutan_igai = R"(((?:(?!()" + first_posi_words_or + R"(|)" + first_nega_words_or + R"(|)" + second_nega_words_or + R"(|)" + third_nega_words_or + R"(|)" + bun_shuutan_moji + R"()).)*))";
		std::string nega_posi_igai = R"(((?:(?!()" + first_posi_words_or + R"(|)" + first_nega_words_or + R"(|)" + second_nega_words_or + R"(|)" + third_nega_words_or + R"()).)*))";

		std::vector<std::string> regex_str_posi = {	"___START___" + nega_posi_igai + kakko_first_posi_words_or + nega_posi_igai + "___EOS___",
													kakko_first_nega_words_or + nega_posi_shuutan_igai + kakko_third_nega_words_or + nega_posi_igai + "___EOS___",
													kakko_first_nega_words_or + nega_posi_shuutan_igai + kakko_second_nega_words_or + nega_posi_shuutan_igai + kakko_first_posi_words_or + nega_posi_igai + "___EOS___"};
		std::vector<std::string> regex_str_nega = {	"___START___" + nega_posi_igai + kakko_first_nega_words_or + nega_posi_igai + "___EOS___",
													kakko_first_posi_words_or + nega_posi_shuutan_igai + kakko_third_nega_words_or + nega_posi_igai + "___EOS___",
													kakko_first_posi_words_or + nega_posi_shuutan_igai + kakko_second_nega_words_or + nega_posi_shuutan_igai + kakko_first_nega_words_or + nega_posi_igai + "___EOS___"};
		
		std::vector<std::regex> regex_posi = {};
		for (int i=0; i<regex_str_posi.size(); i++){
			std::regex regex_tmp(regex_str_posi[i]);
			regex_posi.push_back(regex_tmp);
		}

		std::vector<std::regex> regex_nega = {};
		for (int i=0; i<regex_str_nega.size(); i++){
			std::regex regex_tmp(regex_str_nega[i]);
			regex_nega.push_back(regex_tmp);
		}

		for(int i=0; i<regex_posi.size(); i++){
			if(std::regex_search(utf8_str_match_text, regex_posi[i])){
				return "positive";
			}
		}
		for(int i=0; i<regex_nega.size(); i++){
			if(std::regex_search(utf8_str_match_text, regex_nega[i])){
				return "negative";
			}
		}
		return "neutral";
	}

	__declspec(dllexport) std::vector<std::string> __cdecl all_classes() {
		std::vector<std::string> all_cls = {"negative", "neutral", "positive"};
		return all_cls;
	}
}