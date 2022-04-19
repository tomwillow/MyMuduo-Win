#pragma once

#ifdef _WIN32

#include <winsock2.h>
#include <WS2tcpip.h> // sockaddr_in6

#else
#include <unistd.h>
#include <arpa/inet.h>
typedef unsigned int SOCKET;
typedef unsigned short USHORT;
typedef unsigned char UCHAR;
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (~0)
#endif

#include <stdint.h>
#include <string>

//#include "MyString.h"

namespace muduo
{
	using std::string;
	using std::wstring;
	//using string = MyString;
	//using wstring = MyWString;
}