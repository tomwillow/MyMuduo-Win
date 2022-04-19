#pragma once

#include "../base/noncopyable.h"
#include "InetAddress.h"

#include <functional>

namespace muduo
{
	namespace net
	{
		class EventLoop;
		class Connector : noncopyable
		{
		public:
			using NewConnectionCallback = std::function<void(SOCKET)>;

			Connector(EventLoop *loop, const InetAddress &serverAddr);

			void Start();

			void SetNewConnectionCallback(NewConnectionCallback cb);

			void Restart();

			void Stop();

		private:
			double coldTime;
			EventLoop *loop;
			InetAddress serverAddr;

			NewConnectionCallback onNewConnection;

			void ConnectInLoop();
		};


	}

}