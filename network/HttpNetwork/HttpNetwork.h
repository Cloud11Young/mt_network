#pragma once

#include "IHttpServer.h"
#include "IHttpClient.h"

#ifdef HTTPNETWORK_EXPORTS
#define HTTP_API __declspec(dllexport)
#else
#define HTTP_API __declspec(dllimport)
#endif

namespace MT
{
	HTTP_API IHttpServer* CreateHttpServer();
	HTTP_API IHttpClient* CreateHttpClient();
	HTTP_API void DestroyHttpServer(IHttpServer*);
	HTTP_API void DestroyHttpClient(IHttpClient*);
}