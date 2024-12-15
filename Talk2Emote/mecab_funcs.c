#include <mecab.h>
#include <stdio.h>
#include <string.h>


#define BUF_SIZE 4096

mecab_t *mecab;

/**
 * mecabのインスタンスをN E W
 */
__declspec(dllexport) void __cdecl MakeMecabObject(){
	mecab = mecab_new2("");
}

/**
 * mecabのデストロイ
 */
__declspec(dllexport) void __cdecl DestroyMecabObject(){
	mecab_destroy(mecab);
}

/**
 * 分かち書きして更に各単語を基本形にしたものを返す。(SJIS専用)
 */
__declspec(dllexport) char* __cdecl Str2WordArray(const char* str){
	const mecab_node_t *node;
	node = mecab_sparse_tonode(mecab, str);
	static char buf[BUF_SIZE];
	int b_ptr = 0;
	for (; node; node = node->next) {
		if (node->stat == MECAB_NOR_NODE || node->stat == MECAB_UNK_NODE) {
			const char* f_str = node->feature;
			int f_ptr = 0;		// featureの読み取り文字位置
			int c_ctr = 0;		// カンマの出現回数
			//printf("%s\n", node->feature);
			if(strlen(f_str) > 0){
				while(f_str[f_ptr] != '\0'){
					unsigned char ucfc = (unsigned char)f_str[f_ptr];
					if((ucfc <= 127) || ((161 <= ucfc) && (ucfc <= 223))){
						// 半角文字の場合
						if(f_str[f_ptr] == ','){
							c_ctr ++;
						}else if(c_ctr == 6){
							buf[b_ptr] = f_str[f_ptr];
							b_ptr++;
						}
						f_ptr++;
					}else{
						// 全角文字の場合
						if(c_ctr == 6){
							buf[b_ptr] = f_str[f_ptr];
							buf[b_ptr+1] = f_str[f_ptr+1];
							b_ptr += 2;
						}
						f_ptr += 2;
					}
					if(c_ctr == 7){
						break;
					}
				}
				buf[b_ptr] = ' ';
				b_ptr++;
			}
		}
	}
	buf[b_ptr] = '\0';
	return buf;
}