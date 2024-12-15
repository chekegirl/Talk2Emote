#include <Windows.h>
#include <stdio.h>
#include <string>
#include <set>
#include <map>
#include <vector>
#include "diff_type.h"

/**
 * �� const char* str1, const char* str2�͌��i�K�ł͎g���Ă��Ȃ����A����̊g�������l���Ĉ����Ɏ������Ă���B����2�̕�������r������@������ɓ���Ă���B
 * �iword2vec�݂����Ȃ̂��g���ėގ��x���m���߂���ł��������Ǝv���Ă���B��������face_predict_plugins�̐��茋�ʂ���v���邩�ǂ����A�Ƃ����ł���Ǝv���Bface_predict_plugins�̒��g���̂����x�ȏ����ɂȂ��Ă��Ȃ����ǁB�j
 * Input  : �P�ꖈ���ǂ���, Diff�ɂ���r����, ������1, ������2
 * Output : ������1��2�𓯈ꎋ���邱�Ƃɖ�肪���邩�i�����true�j
 */
extern "C" __declspec(dllexport) bool __cdecl BanListJudge(bool is_word, std::map<DiffType, std::vector<std::vector<std::string>>> diff_map, const char* str1, const char* str2){
	std::set<std::string> ban_list;
	std::set<std::set<std::string>> swap_able_pair;
	if(is_word){
		ban_list = {"�ǂ�", "����", "����", "��邢", "����", "��", "�s", "�Ȃ�", "�ł���", "������", "�߂���", "����", "�Ȃ�", "����", "�Ȃ��", "�ǂ�����", "�ǂ�", "������", "���˂�", "���˂�", "����"};
		swap_able_pair = {{"�ǂ�","����"},{"����","�Ȃ�"},{"�߂���","������"},{"����","�Ȃ�"},{"����", "�Ȃ��"},{"�ǂ�����", "�ǂ�"},{"��", "�s"},{"����", "��邢"},{"���˂�", "����"}};
	}else{
		ban_list = {"����", "�ǂ�", "�Ȃ�", "���Ȃ���", "�Ȃ���", "�Ȃ�", "�Ȃ���", "�Ȃ�", "������", "��������", "����", "������", "������", "�߂���", "�߂�", "�߂���", "�Ă���", "���悤", "���Ă���", "����", "�悤", "�����Ȃ����Ȃ�", "���Ȃ����Ȃ�", "�Ȃ����Ȃ�", "�����Ȃ�", "���Ȃ�", "�Ȃ�����", "���Ȃ�����"};
		swap_able_pair = {{"�ǂ�","����"},{"�Ȃ�","�Ȃ�����"},{"��","��"}};
	}
	bool ban_flag = false;
	// Insert���ꂽ������̒��ɑ}���폜�u���֎~�̕����񂪊܂܂�邩�ǂ���
	for (int j=0; j<diff_map[DiffType::Insert].size(); j++){
		if(ban_flag){
			break;
		}
		if(ban_list.find(diff_map[DiffType::Insert][j][0]) != ban_list.end()){
			ban_flag = true;
		}
	}
	// Delete���ꂽ������̒��ɑ}���폜�u���֎~�̕����񂪊܂܂�邩�ǂ���
	for (int j=0; j<diff_map[DiffType::Delete].size(); j++){
		if(ban_flag){
			break;
		}
		if(ban_list.find(diff_map[DiffType::Delete][j][0]) != ban_list.end()){
			ban_flag = true;
		}
	}
	// Swap���ꂽ������̒��ɑ}���폜�u���֎~�̕����񂪊܂܂�邩�ǂ���
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