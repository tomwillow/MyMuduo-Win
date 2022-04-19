#include "InetAddress.h"

#include "Endian.h"
#include "SocketFunction.h"

#include <string>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace muduo::net::sockets;

InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6)
{
	if (ipv6)
	{
		memset(&addr6_, 0, sizeof addr6_);
		addr6_.sin6_family = AF_INET6;
		addr6_.sin6_addr = loopbackOnly ? in6addr_loopback : in6addr_any;;
		addr6_.sin6_port = hostToNetwork16(port);
	}
	else
	{
		memset(&addr6_, 0, sizeof addr_);
		addr_.sin_family = AF_INET;
		addr_.sin_addr.s_addr = loopbackOnly ? hostToNetwork32(INADDR_LOOPBACK) : INADDR_ANY;
		addr_.sin_port = hostToNetwork16(port);
	}
}

InetAddress::InetAddress(muduo::string ip, uint16_t port, bool ipv6)
{
	if (ipv6 || strchr(ip.c_str(), ':'))
	{
		memset(&addr6_, 0, sizeof addr6_);
		addr6_.sin6_family = AF_INET6;
		if (::inet_pton(AF_INET6, ip.c_str(), &addr6_.sin6_addr) <= 0)
		{
			throw socket_error(("Invalid IPv6 address: " + ip).c_str(), WSAGetLastError());
		}
		addr6_.sin6_port = hostToNetwork16(port);
	}
	else
	{
		memset(&addr6_, 0, sizeof addr_);
		addr_.sin_family = AF_INET;
		if (inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0)
		{
			throw socket_error(("Invalid IPv4 address: " + ip).c_str(), WSAGetLastError());
		}
		addr_.sin_port = htons(port);
	}
}

muduo::net::InetAddress::InetAddress(SOCKET sock, bool isLocalAddr)
{
	using PFunGetXXName = int(__stdcall *)(SOCKET s, struct sockaddr *name, int *namelen);
	PFunGetXXName pFunGetXXName;
	if (isLocalAddr)
		pFunGetXXName = &getsockname;
	else
		pFunGetXXName = &getpeername;

	int len = sizeof(addr_);
	pFunGetXXName(sock, (sockaddr *)&addr_, &len);

	if (IsIPv6())
	{
		len = sizeof(addr6_);
		pFunGetXXName(sock, (sockaddr *)&addr6_, &len);
	}
}

muduo::string InetAddress::ToIPString() const
{
	char buf[64];
	if (IsIPv6())
		inet_ntop(addr6_.sin6_family, (void *)&addr6_.sin6_addr, buf, 64);
	else
		inet_ntop(addr_.sin_family, (void *)&addr_.sin_addr, buf, 64);
	return string(buf);
}

muduo::string muduo::net::InetAddress::ToIPPortString() const
{
	if (IsIPv6())
		return string("[") + ToIPString() + "]:" + to_string(GetPort());
	return ToIPString() + ":" + to_string(GetPort());
}

unsigned short muduo::net::InetAddress::GetPort() const
{
	return networkToHost16(addr_.sin_port);
}

bool muduo::net::InetAddress::IsIPv6() const
{
	return Family() == AF_INET6;
}

InetAddress muduo::net::InetAddress::Resolve(string hostName, string serviceName, bool ipv6)
{
	InetAddress out(0);

	struct addrinfo *result = NULL;
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	DWORD dwRetval = getaddrinfo(hostName.c_str(), serviceName.c_str(), &hints, &result);
	if (dwRetval != 0)
	{
		switch (dwRetval)
		{
		case WSAHOST_NOT_FOUND:
			throw runtime_error(("getaddrinfo failed: host not found: " + hostName).c_str());
		default:
			throw socket_error("getaddrinfo failed", dwRetval);
		}
	}

	struct addrinfo *ptr = result;
	bool found = false;
	while (ptr)
	{
		if (ipv6 && ptr->ai_family == AF_INET6)
		{
			memcpy(&out.addr6_, ptr->ai_addr, sizeof(sockaddr_in6));
			found = true;
			break;
		}
		if (ipv6==false && ptr->ai_family == AF_INET)
		{
			memcpy(&out.addr_, ptr->ai_addr, sizeof(sockaddr_in));
			found = true;
			break;
		}
		ptr = ptr->ai_next;
	}

	freeaddrinfo(result);

	if (found==false)
		throw runtime_error(("Resolve " + hostName+" at "+(ipv6?"IPv6":"IPv4") + " no results.").c_str());

	return out;
}
