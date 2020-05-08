#ifndef NETWORK_SERVER_IMPL_H_
#define NETWORK_SERVER_IMPL_H_

#include "IServer.h"

class ICallback;

class CServer : public IServer
{
public:
	CServer(ICallback* callback) : m_pCallback(callback) {}
	virtual ~CServer() {}

	virtual int Start(){ return 0; }
	virtual int Stop() { return 0; }
	virtual int Send() { return 0; }
	virtual int DisConnect() { return 0; }

private:
	ICallback* m_pCallback;
};

#endif