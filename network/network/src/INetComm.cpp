#include "stdafx.h"
#include "INetComm.h"
#include "CNetComm.h"

#include "CServer.h"
#include "CClient.h"

BOOL INetComm::CreateInstance(INetComm **ppINetComm){
	*ppINetComm = new CNetComm;
	if (*ppINetComm != NULL)
		return TRUE;
	return FALSE;
}

NetAPI IServer* Network::CreateServer(ICallback* callback)
{
	return new CServer(callback);
}

NetAPI IClient* Network::CreateClient(ICallback* callback)
{
	return new CClient(callback);
}

NetAPI void Network::ReleaseServer(IServer* pServer)
{
	if (pServer)
		delete pServer;
}

NetAPI void Network::ReleaseClient(IClient* pClient)
{
	if (pClient)
		delete pClient;
}
