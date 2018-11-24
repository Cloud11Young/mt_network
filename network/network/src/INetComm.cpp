#include "stdafx.h"
#include "INetComm.h"
#include "CNetComm.h"

BOOL INetComm::CreateInstance(INetComm **ppINetComm){
	*ppINetComm = new CNetComm;
	if (*ppINetComm != NULL)
		return TRUE;
	return FALSE;
}