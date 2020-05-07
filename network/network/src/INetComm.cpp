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

NetAPI IServer* Network::CreateServer(NetworkCallback* callback)
{
	return new CServer(callback);
}

NetAPI IClient* Network::CreateClient(NetworkCallback* callback)
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
