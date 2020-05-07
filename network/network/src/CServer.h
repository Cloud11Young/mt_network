#ifndef NETWORK_SERVER_IMPL_H_
#define NETWORK_SERVER_IMPL_H_

#include "IServer.h"

class CServer : public IServer
{
public:
	CServer(NetworkCallback* callback);
	virtual ~CServer() {}

	virtual int Start(){ return 0; }
	virtual int Stop() { return 0; }
	virtual int Send() { return 0; }
	virtual int DisConnect() { return 0; }

private:
	NetworkCallback* m_pCallback;
};

#endif