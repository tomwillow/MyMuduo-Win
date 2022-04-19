#pragma once

#include "../base/Types.h"
#include "../base/Timestamp.h"
#include "../base/noncopyable.h"

#include "Channel.h"
#include "Callbacks.h"
#include "SocketFunction.h"

#include "InetAddress.h"
#include "Buffer.h"

#ifdef _DEBUG
#include <iostream>
#endif

namespace muduo
{
	namespace net
	{
		class EventLoop;
		class CellServer;
		class TcpConnection :muduo::noncopyable, public std::enable_shared_from_this<TcpConnection>
		{
		public:
			enum StateE { kConnecting, kConnected, kDisconnecting, kDisconnected };

			TcpConnection(EventLoop *loop, SOCKET in_sock, unsigned int id);

			~TcpConnection();

			SOCKET fd() const { return channel.GetFd(); }

			unsigned int GetId() const { return id; }
			bool IsConnecting() const { return state == kConnecting; }
			bool IsConnected() const { return state == kConnected; }

			// ��TcpServer����
			void SetConnectionCallback(const ConnectionCallback &cb) { onConnection = cb; }
			void SetMessageCallback(const MessageCallback &cb) { onMessage = cb; }
			void SetCloseCallback(const CloseCallback &cb) { onClose = cb; }

			// ��������
			// �������������
			// �̰߳�ȫ
			void Send(const string &s);

			// ��������
			// �������������
			// �̰߳�ȫ
			void Send(const void *data, int size);

			// ��������
			void ConnectEstablished();

			void ConnectDestroyed();

			void ForceClose();

			muduo::string IPPortString() const
			{
				return peerAddr.ToIPPortString();
			}

			muduo::string IPString() const
			{
				return peerAddr.ToIPString();
			}

			const InetAddress &LocalAddress() const;
			const InetAddress &PeerAddress() const;

		private:
			EventLoop *loop;
			StateE state;

			unsigned int id;

			Channel channel;
			InetAddress localAddr, peerAddr;

			Buffer buf;
			Buffer sendBuf;

			ConnectionCallback onConnection;
			MessageCallback onMessage;
			CloseCallback onClose;

			// ���������á�ע�ᵽChannel�Ļص��У���Channel�����¼�ʱ�ص�
			void HandleRead();

			// ���������á�ע�ᵽChannel�Ļص��У���Channel�����¼�ʱ�ص�
			void HandleWrite();

			// 
			void HandleClose();

			void SendInLoop(const muduo::string &msg);

			friend class CellServer;
		};

		void defaultConnectionCallback(const TcpConnectionPtr &conn);
		void defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buf, Timestamp);
		void defaultCloseCallback(const TcpConnectionPtr &conn);
	}
}