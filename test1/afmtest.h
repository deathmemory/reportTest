// amf3demo.cpp : Defines the entry point for the console application.


#pragma once
#include "AMF3.h"

typedef struct _tagIoData
{
	unsigned char* buf;
	int buflen;
	int pos;
}tagIoData;

bool IsHexChar(char chr);
unsigned int DecodeHex(const char* pData,BYTE* pOut,int inlen);
tagIoData* DecodeHex(const char* pData);
int read_data(void* file,size_t size,unsigned char* buf);
void write_data(void* file,const unsigned char* buf,size_t size);
