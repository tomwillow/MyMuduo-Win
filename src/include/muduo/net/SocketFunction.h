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

	//解析域名为ip
	//解析失败抛runtime_error异常
	std::string DnsParse(const std::string &domain);

	//计算校验和
	USHORT CheckSum(USHORT *buffer, int size);

	bool CheckIP(std::string sip);

	bool CheckNum(std::string s, int a, int b);

	// 检查端口号属于[1025,65534]
	bool CheckPort(std::string s);

	// 检查字符串是否属于 127.0.0.1:8999 形式，且内容有效
	bool CheckIPPort(std::string s);

	bool CheckName(std::string sip);

	bool CheckEmailAddress(std::string addr);

	// 转换ip port字符串为网络序ip port数据
	std::pair<uint32_t, uint16_t> SplitIPPort(std::string ipport);
	std::pair<std::string, std::string> SplitIPPortToStr(std::string ipport);

	void SendUntilSuccess(SOCKET socket, const void *data, int len, int flags = 0);

	void SetNonBlock(SOCKET sockfd);

	void SetSendBufSize(SOCKET sockfd, int bufSize);

	int Connect(SOCKET sockfd, const sockaddr *addr);


}
