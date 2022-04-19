#pragma once

#include "../base/Types.h"

namespace muduo
{
	namespace net
	{

		class InetAddress
		{
		public:
			explicit InetAddress(uint16_t port, bool loopbackOnly = false, bool ipv6 = false);

			// ip��ʽ����ȷ���׳�socket_error�쳣
			InetAddress(muduo::string ip, uint16_t port, bool ipv6 = false);

			// ��һ���ѽ�����socket���Ӵ���InetAddress��isLocalAddrָ����ʹ�ñ��˵�ַ���ǶԶ˵�ַ
			explicit InetAddress(SOCKET sock, bool isLocalAddr);

			ADDRESS_FAMILY Family() const { return addr_.sin_family; }

			const sockaddr *GetSockAddr() { return reinterpret_cast<sockaddr *>(&addr6_); }

			muduo::string ToIPPortString() const;

			muduo::string ToIPString() const;

			unsigned short GetPort() const;

			bool IsIPv6() const;

			static InetAddress Resolve(muduo::string hostName, muduo::string serviceName,bool ipv6=false);

		private:
			union
			{
				struct sockaddr_in addr_;
				struct sockaddr_in6 addr6_;
			};
		};

	}
}