
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

// �ṩһ��MyInitSock�ྲ̬ʵ����
// ���ԣ�����ʱֻҪ��.cpp�ѱ��룬�Ϳ����Զ�����Winsock�⡣
static MyInitSock initSock;

MyInitSock::MyInitSock()
{
#ifdef _WIN32
	//װ��Winsock��
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
	//�ͷ�Winsock��
	WSACleanup();
#endif
}