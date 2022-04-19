#pragma once

#include "SocketFunction.h"
#include <functional>

namespace muduo
{
	namespace net
	{
		class EventLoop;

		// �������SOCKET
		// Channel����Ӧ�ñ�Acceptor��TcpConnection����
		class Channel
		{
		public:
			using EventCallback = std::function<void()>;

			static const int kNoneEvent;
			static const int kReadEvent;
			static const int kWriteEvent;

			Channel(EventLoop *loop, SOCKET fd);
			~Channel();

			EventLoop *GetOwnerLoop() const;
			SOCKET GetFd() const;

			void SetCurrEvents(int currEvents);

			void SetReadCallback(const EventCallback &cb);
			void SetWriteCallback(const EventCallback &cb);

			// ����Read�¼�����֪ͨEventLoop����
			void EnableReading();

			// ���ü���Write�¼�����֪ͨEventLoop����
			// Ӧ�ý��ڳ��б�Channel��Owner��output buffer�ǿ�ʱ���ã�����
			// output buffer����֮����ò��ظ�
			void EnableWriting();

			// ���ü���Write�¼�����֪ͨEventLoop����
			// Ӧ�ý��ڳ��б�Channel��Owner��output bufferΪ��ʱ����
			void DisableWriting();

			// �������Channel�Ƿ���δ���͵���Ϣ��
			// ��������ָʾ���б�Channel��Owner��output buffer�Ƿ�ǿա�
			bool WantToWrite() const;

			// ����EventLoopֱ�ӵ���
			void HandleEvent();

			// ֪ͨEventLoop����
			void Update();
		private:

			EventLoop *loop;
			const SOCKET fd;
			int currEvents, careEvents;

			EventCallback readCallback;
			EventCallback writeCallback;
		};
	}
}

