#pragma once

#include "../base/noncopyable.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "Connector.h"

namespace muduo
{
	namespace net
	{
		class EventLoop;
		class TcpClient :noncopyable
		{
		public:
			TcpClient(EventLoop *loop, const InetAddress &addr, std::string name);

			void SetConnectionCallback(const ConnectionCallback &cb);

			void SetMessageCallback(const MessageCallback &cb);

			void SetCloseCallback(const CloseCallback &cb);

			void Connect();

		private:
			EventLoop *loop;
			muduo::string name;

			Connector connector;
			TcpConnectionPtr conn;

			//
			ConnectionCallback onConnection;
			MessageCallback onMessage;
			CloseCallback onClose;

			void OnNewConnection(SOCKET sock);
		};

	}
}

