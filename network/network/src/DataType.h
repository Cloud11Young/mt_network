#ifndef DATATYPE_H_
#define DATATYPE_H_

#include <intsafe.h>
#include <stdlib.h>

#define MAX_DATA_SIZE 4096

typedef struct {
	unsigned int DataSize;
	unsigned int DataType;
}PacketHead;

typedef struct {
	PacketHead head;
	BYTE Data[MAX_DATA_SIZE];
}Packet;


typedef struct {
	CONNID  dwConnID;
	char	pAddress[50];
	int		iAddressLen;
	USHORT	usPort;
}RemoteAddress;


struct Buffer{
	BYTE* pData;
	int size;
	int used;
	RemoteAddress* pAddr;
	Buffer() :pData(NULL),pAddr(NULL){
		size = 0;
		used = 0;
	}
	~Buffer(){
		if (pData != NULL)
			delete[] pData;
		pData = NULL;
	}
	void init(int len){
		size = len;
		pData = new BYTE[size];
		used = 0;
	}
	void copy(const BYTE* src, int len){
		if (src == NULL || len == 0)	return;
		if (pData){
			if (len > size){
				delete[] pData;
				init(len);
			}			
		}
		else
			init(len);
		used = len;
		memcpy(pData, src, used);
	}
	int Append(Buffer* src, int len){
		if (len > src->used)	len = src->used;
		if (!pData){
			copy(src->pData, len);
			return len;
		}

		if (size >= (used + len)){
			memcpy(pData + used, src->pData, len);
			used += len;
			return used;
		}

		BYTE* pTmp = new BYTE[used + len];
		memcpy(pTmp, pData, used);
		memcpy(pTmp + used, src->pData, len);
	
		delete[] pData;
		pData = pTmp;
		used += len;
		size = used;		
		return size;
	}
	int TrunFront(int len){
		if (len >= used)	len = used;
		
		used -= len;
		memcpy(pData, pData + len, used);
		return used;
	}
};

#endif