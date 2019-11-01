#pragma once

#include "IHttpServer.h"

class CHttpServer : public IHttpServer
{
public:
	CHttpServer();
	~CHttpServer();

public:
	virtual int Start() { return 0; }
};