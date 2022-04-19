#pragma once

#include "InetAddress.h"
#include "TcpConnection.h"
#include "Acceptor.h"

#include "../base/Types.h"
#include "../base/ThreadPool.h"

#include <unordered_map>

namespace muduo
{
	namespace net
	{
		class EventLoop;
		class TcpServer
		{
		public:
			TcpServer(EventLoop *loop,const InetAddress &addr,std::string name);
			~TcpServer();

			void Start();

			void Quit();

			void SetThreadNum(int numThread);

			void SetConnectionCallback(const ConnectionCallback &cb);

			void SetMessageCallback(const MessageCallback &cb);

			void SetCloseCallback(const CloseCallback &cb);

		private:
			EventLoop *loop;
			InetAddress addr;
			muduo::string name;

			ThreadPool threadPool;

			Acceptor acceptor;
			unsigned int nextConnId;
			std::unordered_map<unsigned int, TcpConnectionPtr> connections;

			//
			ConnectionCallback onConnection;
			MessageCallback onMessage;
			CloseCallback onClose;

			// �����������������á�������Acceptor�ص�ʱ���á�
			// TcpConnection�����ﴴ��
			void NewConnection(SOCKET sock, const InetAddress &peerAddr);

			void RemoveConnection(const TcpConnectionPtr &conn);

			void RemoveConnectionInLoop(const TcpConnectionPtr &conn);
		};

	}
}

