#pragma once
#include <windows.h>
#define _PAIRS
namespace PAIRS
{
	CHAR* GetKeyValueStr(CHAR *in_src_str,CHAR *keyname,CHAR *out_value);//����ֵΪ���key�Ľ�β,"key:value,"��β���Ǹ�keyҲҪ��ôд�����,��
	CHAR* split(CHAR *in,CHAR splitFilter,int &count);
};