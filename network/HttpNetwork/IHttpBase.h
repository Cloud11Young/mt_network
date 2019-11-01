#pragma once

class IHttpBase
{
public:
	virtual ~IHttpBase() {}

	virtual int Start() = 0;
};