#pragma once

#include "InetAddress.h"
#include "Channel.h"

namespace muduo
{
	namespace net
	{
		class EventLoop;
		class Acceptor
		{
		public:
			using NewConnectionCallback = std::function<void(SOCKET, const InetAddress &)>;

			Acceptor(EventLoop *loop, const InetAddress &addr, muduo::string name);
			~Acceptor();

			void Listen();

			void HandleRead();

			void SetNewConnectionCallback(const NewConnectionCallback &cb);
		private:
			EventLoop *loop;
			muduo::string name;
			InetAddress addr;
			Channel acceptChannel; // º‡Ã˝Channel”…Acceptor≥÷”–

			NewConnectionCallback newConnectionCallback;
		};
	}
}

