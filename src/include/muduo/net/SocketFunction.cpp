#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "SocketFunction.h"

#ifdef _WIN32
#include <WS2tcpip.h>
#pragma comment(lib, "WS2_32.lib")
#else
#include <netdb.h>//gethostbyname
#endif

#include <string>
#include <cassert>

using namespace std;
using namespace muduo;

SOCKET muduo::CreateSocket(bool isIPv6)
{
	SOCKET sock = socket(isIPv6?AF_INET6:AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET)
	{
		throw socket_error("Invalid socket. Reason: maybe not call WSAStartup.", WSAGetLastError());
	}
	return sock;
}

std::string muduo::DnsParse(const std::string& domain)
{
	hostent* p;

	if ((p = gethostbyname(domain.c_str())) == NULL)
	{
#ifdef _WIN32
		throw socket_error(("DNS Parse fail on \"" + domain + "\" Error code:").c_str(), WSAGetLastError());
#else
		throw socket_error("DNS Parse fail on \"" + domain + "\" Error code:", errno);
#endif
	}

	std::string sip;
	sip += to_string((unsigned char)p->h_addr_list[0][0]) + ".";
	sip += to_string((unsigned char)p->h_addr_list[0][1]) + ".";
	sip += to_string((unsigned char)p->h_addr_list[0][2]) + ".";
	sip += to_string((unsigned char)p->h_addr_list[0][3]);

	return sip;
}

USHORT muduo::CheckSum(USHORT* buffer, int size)
{
	unsigned long cksum = 0;
	while (size > 1)
	{
		cksum += *buffer;
		buffer++;
		size -= sizeof(USHORT);
	}
	if (size)
		cksum += *(UCHAR*)buffer;

	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (USHORT)(~cksum);
}

bool muduo::CheckIP(std::string sip)
{
	sockaddr_in sa;
	int ret = inet_pton(AF_INET, sip.c_str(), &sa.sin_addr);
	return ret == 1;//if success
}

bool muduo::CheckNum(std::string s, int a, int b)
{
	try
	{
		int port = stoi(s);
		return port >= a && port <= b;
	}
	catch (...)
	{
		return false;
	}
}

bool muduo::CheckPort(std::string s)
{
	return CheckNum(s, 1025, 65534);
}

bool muduo::CheckIPPort(std::string s)
{
	auto colon = s.find(":");
	if (colon == std::string::npos)
		return false;

	return CheckIP(s.substr(0, colon)) && CheckPort(s.substr(colon + 1));
}

bool muduo::CheckName(std::string sip)
{
	auto found = sip.find_first_of("-+=|\\:;'\",.<>/?");
	return found == std::string::npos && !sip.empty();//if success
}

bool muduo::CheckEmailAddress(std::string addr)
{
	std::string name, web;
	auto at_pos = addr.find('@');
	if (at_pos == std::string::npos)
		return false;

	name = addr.substr(0, at_pos);
	web = addr.substr(at_pos + 1);

	if (name.empty() || web.empty())
		return false;

	auto is_alpha_num_underline = [](std::string s)->bool
	{
		for (auto c : s)
		{
			if (isalnum(c) == 0)
				if (c != '_')
					if (c != '.')
						return false;
		}
		return true;
	};

	return is_alpha_num_underline(name) && is_alpha_num_underline(web);
}

//uint32_t muduo::IPStr2Val(muduo::string sip)
//{
//	return inet_addr(sip.c_str());
//}
//
//muduo::string muduo::IPVal2Str(uint32_t ip)
//{
//	sockaddr_in remoteAddr;
//	remoteAddr.sin_addr.S_un.S_addr = ip;
//	return inet_ntoa(remoteAddr.sin_addr);
//}
//
//
//std::pair<uint32_t, uint16_t> muduo::SplitIPPort(muduo::string ipport)
//{
//	pair<uint32_t, uint16_t> ret;
//	auto colon = ipport.find(':');
//	if (colon != string::npos)
//	{
//		ret.first = IPStr2Val(ipport.substr(0, colon));
//		ret.second = htons(stoi(ipport.substr(colon + 1)));
//	}
//	return ret;
//}

std::pair<std::string, std::string> muduo::SplitIPPortToStr(std::string ipport)
{
	pair<std::string, std::string> ret;
	auto colon = ipport.find(':');
	if (colon != std::string::npos)
	{
		ret.first = ipport.substr(0, colon);
		if (!CheckIP(ret.first))
			ret.first.clear();

		ret.second = ipport.substr(colon + 1);
		if (!CheckPort(ret.second))
			ret.second.clear();
	}
	return ret;
}

void muduo::SendUntilSuccess(SOCKET socket, const void* data, int len, int flags)
{
	int count = 0;
	while (count < len)
	{
		int bytes = send(socket, (const char*)data + count, len - count, flags);
		if (bytes <= 0)
		{
#ifdef _WIN32
			throw socket_error("Send fail. Error code:", WSAGetLastError());
#else
			throw socket_error("Send fail. Error code:", errno);
#endif
		}
		count += bytes;
	}
}


#ifdef _WIN32

void muduo::SetNonBlock(SOCKET sockfd)
{
	u_long val = 1;
	int ret = ioctlsocket(sockfd, FIONBIO, &val);
	assert(ret == NO_ERROR);
}

void muduo::SetSendBufSize(SOCKET sockfd, int bufSize)
{
	int send_buffer_sizeof = sizeof(int);
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&bufSize, send_buffer_sizeof);
}
#else

bool isNonBlock(int sockfd)
{
	int flags = ::fcntl(sockfd, F_GETFL, 0);
	return flags & O_NONBLOCK;
}

void setNonBlockAndCloseOnExec(int sockfd)
{
	// non-block
	int flags = ::fcntl(sockfd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	int ret = ::fcntl(sockfd, F_SETFL, flags);
	// FIXME check

	// close-on-exec
	flags = ::fcntl(sockfd, F_GETFD, 0);
	flags |= FD_CLOEXEC;
	ret = ::fcntl(sockfd, F_SETFD, flags);
	// FIXME check

	(void)ret;
}
#endif

int muduo::Connect(SOCKET sockfd, const sockaddr *addr)
{
	return ::connect(sockfd, addr, sizeof(sockaddr_in6));
}
