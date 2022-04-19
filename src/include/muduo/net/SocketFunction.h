#pragma once

#include "../base/Types.h"

#ifdef _WIN32

#ifdef FD_SETSIZE
#undef FD_SETSIZE
#define FD_SETSIZE 1024
#else
#define FD_SETSIZE 1024
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
typedef unsigned int SOCKET;
typedef unsigned short USHORT;
typedef unsigned char UCHAR;
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (~0)
#endif

#include <stdexcept>

namespace muduo
{

	class socket_error :public std::runtime_error
	{
	public:
		const int err_code;
		socket_error(const char *info, int in_err_code) :std::runtime_error(info), err_code(in_err_code) {}
		socket_error(std::string info, int in_err_code) :std::runtime_error(info), err_code(in_err_code) {}
	};

	SOCKET CreateSocket(bool isIPv6 = false);

	//��������Ϊip
	//����ʧ����runtime_error�쳣
	std::string DnsParse(const std::string &domain);

	//����У���
	USHORT CheckSum(USHORT *buffer, int size);

	bool CheckIP(std::string sip);

	bool CheckNum(std::string s, int a, int b);

	// ���˿ں�����[1025,65534]
	bool CheckPort(std::string s);

	// ����ַ����Ƿ����� 127.0.0.1:8999 ��ʽ����������Ч
	bool CheckIPPort(std::string s);

	bool CheckName(std::string sip);

	bool CheckEmailAddress(std::string addr);

	// ת��ip port�ַ���Ϊ������ip port����
	std::pair<uint32_t, uint16_t> SplitIPPort(std::string ipport);
	std::pair<std::string, std::string> SplitIPPortToStr(std::string ipport);

	void SendUntilSuccess(SOCKET socket, const void *data, int len, int flags = 0);

	void SetNonBlock(SOCKET sockfd);

	void SetSendBufSize(SOCKET sockfd, int bufSize);

	int Connect(SOCKET sockfd, const sockaddr *addr);


}
