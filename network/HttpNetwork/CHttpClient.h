#pragma once
#include "IHttpClient.h"

class CHttpClient : public IHttpClient
{
public:
	CHttpClient();
	~CHttpClient();

public:
	virtual int Start() { return 0; }
};