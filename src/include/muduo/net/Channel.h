#pragma once

#include "SocketFunction.h"
#include <functional>

namespace muduo
{
	namespace net
	{
		class EventLoop;

		// 负责持有SOCKET
		// Channel本身应该被Acceptor或TcpConnection持有
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

			// 启用Read事件，并通知EventLoop更新
			void EnableReading();

			// 启用监听Write事件，并通知EventLoop更新
			// 应该仅在持有本Channel的Owner的output buffer非空时调用，并在
			// output buffer空了之后禁用并回复
			void EnableWriting();

			// 禁用监听Write事件，并通知EventLoop更新
			// 应该仅在持有本Channel的Owner的output buffer为空时调用
			void DisableWriting();

			// 返回这个Channel是否有未发送的消息。
			// 本质上是指示持有本Channel的Owner的output buffer是否非空。
			bool WantToWrite() const;

			// 将由EventLoop直接调用
			void HandleEvent();

			// 通知EventLoop更新
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

