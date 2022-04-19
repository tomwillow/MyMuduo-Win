
#include "MyInitSock.h"
#include <string>
#include <stdexcept>

#ifdef _WIN32

	#ifndef _WINSOCK2API_
		#include <winsock2.h>
		#pragma comment(lib,"WS2_32.lib")
	#endif
#else
	#include <unistd.h>
	#include <arpa/inet.h>
#endif

// 提供一个MyInitSock类静态实例。
// 所以，编译时只要此.cpp已编译，就可以自动加载Winsock库。
static MyInitSock initSock;

MyInitSock::MyInitSock()
{
#ifdef _WIN32
	//装入Winsock库
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	if (WSAStartup(version, &wsaData))
	{
		throw std::runtime_error("WSAStartup error.");
	}
#endif
}

MyInitSock::~MyInitSock()
{
#ifdef _WIN32
	//释放Winsock库
	WSACleanup();
#endif
}