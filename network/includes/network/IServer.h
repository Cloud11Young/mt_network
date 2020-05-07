#ifndef NETWORK_SERVER_H_
#define NETWORK_SERVER_H_

class IServer
{
public:
	virtual ~IServer() {}
	
	virtual int Start() = 0;
	virtual int Stop() = 0;
	virtual int Send() = 0;
	virtual int DisConnect() = 0;
};

#endif