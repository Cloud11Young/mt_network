#ifndef NETWORK_CLIENT_IMPL_H_
#define NETWORK_CLIENT_IMPL_H_

#include "IClient.h"

class ICallback;
class CClient : public IClient
{
public:
	CClient(ICallback* callback):m_pCallback(callback)
	{}
	virtual ~CClient() {}
	virtual int Start(){ return 0; }
	virtual int Stop(){ return 0; }
	virtual int Send(){ return 0; }

private:
	ICallback* m_pCallback;
};
#endif