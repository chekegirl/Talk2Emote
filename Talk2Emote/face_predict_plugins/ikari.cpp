#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <regex>
#include <vector>
#include <iomanip>
#include <sstream>

extern "C" {
	__declspec(dllexport) std::string __cdecl predict_ikari(std::string utf8_str_match_text) {
		//文の頭と末尾のセットで疑問形の怒り判定する単語群
		std::string gimonshi_hyoujun = R"(どんだけ|だれ|誰|いくつ|何で|なんで|何だったら|なんだったら|何が|なにが|いつ|どう|どの|どれ|何処|どこ|いくら)";
		std::string gimonshi_kansai = R"(なんぼ|なんちゅう|どない|どげん|なんやったら|何やったら)";
		std::string nan_josuushi = R"(個|粒|本|枚|点|件|組|回|度|号|位|着|局|足|杯|膳|皿|丁|切|食|錠|軒|戸|面|階|室|畳|台|両|隻|機|冊|巻|部|通|行|列|束|合|人|名|体|匹|頭|羽|色|円|秒|秒間|分|分間|時|時間|日|日間|週|週間|月|年|年間|フロア|ドル|ユーロ|元|ウォン|平米|アール|ヘクタール|オクターブ|ヤード|ポンド|セント|ピクセル|ドット|ビット|ダース|((センチ|ミリ|ナノ|デシ|キロ|メガ|ギガ|テラ|ペタ|エクサ)?(リットル|バイト|メートル|ジュール|ワット|アンペア|ボルト)))";
		std::string gimonshi = R"(()" + gimonshi_hyoujun + R"(|)" + gimonshi_kansai + R"(|((何|なん)()" + nan_josuushi + R"())))";

		std::string with_gimon_matsubi_hyoujun = R"(だよ|てんだ|だ)";
		std::string with_gimon_matsubi_kansai = R"(とんや|とんじゃ|たんや|たんじゃ|なんや|なんじゃ|るんや|るんじゃ|やないか|やんか|や|ねん)";
		std::string with_gimon_matsubi = R"(()" +  with_gimon_matsubi_hyoujun + R"(|)" + with_gimon_matsubi_kansai + R"())";

		//文の終端を表す
		std::string bun_shuutan_moji = R"(\s|。|、|((\.\.\.)(\.)*)|…|！|\!|？|\?|___EOS___)";
		std::string bun_shuutan_kouho = R"(ー|～|\-|\~)";
		std::string bun_shuutan = R"(()" + bun_shuutan_moji + R"(|)" + bun_shuutan_kouho + R"()+)";
		std::string bun_shuutan_igai = R"(((?:(?!)" + bun_shuutan_moji + R"().)*))";

		//怒号
		std::string dogou = R"((ふざけんな|ふざけるな|クソが|くそが|アホか|あほか|はぁ？))";
		std::string dogou_with_shuutan = R"((こら|コラ|おい))";

		//文末が強いやつ
		std::string strong_bunmatu = R"((じゃねえ|じゃねぇ|るな|てんだろ|ねえぞ|ねぇぞ|んのか))";

		//最終的な正規表現を作る
		std::string regex_str1 = gimonshi + bun_shuutan_igai + with_gimon_matsubi + bun_shuutan;
		std::string regex_str2 = dogou;
		std::string regex_str3 = dogou_with_shuutan + bun_shuutan;
		std::string regex_str4 = strong_bunmatu + bun_shuutan;

		//これより正規表現の処理関連
		std::regex regex1(regex_str1);
		std::regex regex2(regex_str2);
		std::regex regex3(regex_str3);
		std::regex regex4(regex_str4);
		
		std::vector<std::regex> patterns = {regex1, regex2, regex3, regex4};
		
		for(int i=0; i<patterns.size(); i++){
			if(std::regex_search(utf8_str_match_text, patterns[i])){
				return "true";
			}
		}
		return "false";
	}
}