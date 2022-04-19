#pragma once

#include "../base/Timestamp.h"

#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <map>

using TimerCallback = std::function<void()>;
using TimerId = unsigned int;

namespace muduo
{
	namespace net
	{
		class Channel;
		class Poller;
		class EventLoop
		{
		public:
			using Functor = std::function<void()>;
			EventLoop();
			~EventLoop();

			bool IsLooping() const;

			// Loopѭ��
			void Loop();

			// �˳�Loop
			void Quit();

			// ע�᣺ִֻ��һ�ε��¼�
			void RunOnce(Functor cb);

			// ע�᣺ÿ��ѭ����Ҫִ�е��¼�
			void runEveryLoop(const Functor &cb);

			// ע�᣺ÿ��һ��ʱ���Ҫִ�е��¼�
			TimerId runEvery(int ms, const TimerCallback &cb);

			// ע�᣺��ĳ��ʱ���ִ�е��¼���ִֻ��һ��
			TimerId runAt(Timestamp timeStamp, const TimerCallback &cb);

			// ע�᣺��һ��ʱ��֮��ִ�е��¼���ִֻ��һ��
			TimerId RunAfter(double sec, const TimerCallback &cb);

			void AssertInLoopThread() const;

			bool IsInLoopThread() const;

			// ���Լ����е�poller���´�Channel
			// �̰߳�ȫ����
			void UpdateChannel(Channel *channel);

			// �̰߳�ȫ����
			void RemoveChannel(Channel *channel);

			static EventLoop *GetEventLoopOfCurrentThread();
		private:
			bool looping;
			bool doQuit;
			std::mutex m;
			const std::thread::id threadId;
			std::unique_ptr<Poller> poller;
			std::queue<Functor> pendingFunctors; // һ�����¼�

			// ÿ��ѭ����Ҫ���е��¼�
			std::vector<Functor> loopEvents;
			void doEveryLoopEvents();

			// ÿ��һ��ʱ�����е��¼�
			struct EveryEvent
			{
				TimerCallback cb;
				std::chrono::milliseconds interval;
				Timestamp lastActive;
				EveryEvent(TimerCallback _cb, std::chrono::milliseconds _interval) :
					cb(_cb), interval(_interval),lastActive(Timestamp::now()) {}
			};
			std::vector<EveryEvent> everyEvents;
			void doEveryEventOnce(); // ִ��һ��ÿ��һ��ʱ���Ҫִ�е��¼�

			// ��ʱ������ֻ����һ�ε��¼�
			struct TimeEvent
			{
				TimerCallback cb;
				Timestamp timeStamp;
			};
			std::multimap<Timestamp, TimeEvent> timeEvents;
			void doTimeEvent();// ��ʱ������ֻ����һ�ε��¼�

			/// Queues callback in the loop thread.
			/// Runs after finish pooling.
			/// Safe to call from other threads.
			void queueInLoop(Functor cb);

			// ִ��һ�����¼�
			void doPendingFunctors();
		};
	}
}