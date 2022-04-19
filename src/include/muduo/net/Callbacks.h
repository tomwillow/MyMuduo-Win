#pragma once

#include <memory>
#include <functional>

#include "../base/Timestamp.h"

namespace muduo
{
	namespace net
	{

		class Buffer;
		class TcpConnection;
		using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

		typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
		typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
		typedef std::function<void(const TcpConnectionPtr &, Buffer *, Timestamp)> MessageCallback;
		//typedef std::function<void()> TimerCallback;
		//typedef std::function<void(const TcpConnectionPtr &)> WriteCompleteCallback;

		void defaultConnectionCallback(const TcpConnectionPtr &conn);

		void defaultMessageCallback(const TcpConnectionPtr &, Buffer *buf, Timestamp);
	}
}
