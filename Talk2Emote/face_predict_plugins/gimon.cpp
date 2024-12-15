#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <regex>
#include <vector>
#include <iomanip>
#include <sstream>

extern "C" {
	__declspec(dllexport) std::string __cdecl predict_gimon(std::string utf8_str_match_text) {
		//文の頭と末尾のセットで疑問文判定する単語群
		std::string gimonshi_hyoujun = R"(どんだけ|なぜ|だれ|誰|どなた|いくつ|なんだっけ|何で|なんで|何か|なにか|なんか|何だったら|なんだったら|何が|なにが|いつ|どう|どの|どれ|何処|どこ|如何に|いかに|いくら)";
		std::string gimonshi_kansai = R"(なんぼ|なんちゅう|どない|どげん|どした|なんやっけ|何やっけ|なんやったら|何やったら)";
		std::string nan_josuushi = R"(個|粒|本|枚|点|件|組|回|度|号|位|着|局|足|杯|膳|皿|丁|切|食|錠|軒|戸|面|階|室|畳|台|両|隻|機|冊|巻|部|通|行|列|束|合|人|名|体|匹|頭|羽|色|円|秒|秒間|分|分間|時|時間|日|日間|週|週間|月|年|年間|フロア|ドル|ユーロ|元|ウォン|平米|アール|ヘクタール|オクターブ|ヤード|ポンド|セント|ピクセル|ドット|ビット|ダース|((センチ|ミリ|ナノ|デシ|キロ|メガ|ギガ|テラ|ペタ|エクサ)?(リットル|バイト|メートル|ジュール|ワット|アンペア|ボルト)))";
		std::string gimonshi = R"(()" + gimonshi_hyoujun + R"(|)" + gimonshi_kansai + R"(|((何|なん)()" + nan_josuushi + R"())))";

		std::string with_gimon_matsubi_hyoujun = R"(すか|かね|しょうね|たの|なの|るの|かな|だよね|よね|でしょう|だろ|だろう|だろうか|のか)";
		std::string with_gimon_matsubi_kansai = R"(なんか|たん|なん|るん|かいな|やんな|やっけ|やないか|やんか|やろか|やろな|や|やろ|やろうか)";
		std::string with_gimon_matsubi = R"(()" +  with_gimon_matsubi_hyoujun + R"(|)" + with_gimon_matsubi_kansai + R"())";

		//文の末尾についていれば疑問文判定する単語群
		std::string gimon_matsubi_hyoujun = R"(しょうか|しょうかね|だっけ|のかな|ませんか|ですかね|ますかね|かなぁ|かなあ|ですかねぇ|ですかねえ|ますかねぇ|ますかねえ)";
		std::string gimon_matsubi_kansai = R"(やっけ|かいな)";
		std::string gimon_matsubi = R"(()" +  gimon_matsubi_hyoujun + R"(|)" + gimon_matsubi_kansai + R"())";

		//疑問符
		std::string gimonhu = R"((？|\?))";

		//文の終端を表す
		std::string bun_shuutan_moji = R"(\s|。|、|((\.\.\.)(\.)*)|…|！|\!|？|\?|___EOS___)";
		std::string bun_shuutan_kouho = R"(ー|～|\-|\~)";
		std::string bun_shuutan = R"(()" + bun_shuutan_moji + R"(|)" + bun_shuutan_kouho + R"()+)";
		std::string bun_shuutan_igai = R"(((?:(?!)" + bun_shuutan_moji + R"().)*))";

		std::string regex_str1 = gimonshi + bun_shuutan_igai + with_gimon_matsubi + bun_shuutan;
		std::string regex_str2 = gimonshi + bun_shuutan;
		std::string regex_str3 = gimon_matsubi + bun_shuutan;
		std::string regex_str4 = gimonhu;

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