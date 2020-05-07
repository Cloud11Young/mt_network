#ifndef NETWORK_CLIENT_H_
#define NETWORK_CLIENT_H_

class IClient
{
public:
	virtual ~IClient() {}
	virtual int Start() = 0;
	virtual int Stop() = 0;
	virtual int Send() = 0;
};

#endif