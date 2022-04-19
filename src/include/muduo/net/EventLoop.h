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

			// Loop循环
			void Loop();

			// 退出Loop
			void Quit();

			// 注册：只执行一次的事件
			void RunOnce(Functor cb);

			// 注册：每轮循环都要执行的事件
			void runEveryLoop(const Functor &cb);

			// 注册：每隔一段时间就要执行的事件
			TimerId runEvery(int ms, const TimerCallback &cb);

			// 注册：在某个时间点执行的事件，只执行一次
			TimerId runAt(Timestamp timeStamp, const TimerCallback &cb);

			// 注册：隔一段时间之后执行的事件，只执行一次
			TimerId RunAfter(double sec, const TimerCallback &cb);

			void AssertInLoopThread() const;

			bool IsInLoopThread() const;

			// 让自己持有的poller更新此Channel
			// 线程安全：否
			void UpdateChannel(Channel *channel);

			// 线程安全：否
			void RemoveChannel(Channel *channel);

			static EventLoop *GetEventLoopOfCurrentThread();
		private:
			bool looping;
			bool doQuit;
			std::mutex m;
			const std::thread::id threadId;
			std::unique_ptr<Poller> poller;
			std::queue<Functor> pendingFunctors; // 一次性事件

			// 每轮循环都要运行的事件
			std::vector<Functor> loopEvents;
			void doEveryLoopEvents();

			// 每隔一段时间运行的事件
			struct EveryEvent
			{
				TimerCallback cb;
				std::chrono::milliseconds interval;
				Timestamp lastActive;
				EveryEvent(TimerCallback _cb, std::chrono::milliseconds _interval) :
					cb(_cb), interval(_interval),lastActive(Timestamp::now()) {}
			};
			std::vector<EveryEvent> everyEvents;
			void doEveryEventOnce(); // 执行一次每隔一段时间就要执行的事件

			// 定时运行且只运行一次的事件
			struct TimeEvent
			{
				TimerCallback cb;
				Timestamp timeStamp;
			};
			std::multimap<Timestamp, TimeEvent> timeEvents;
			void doTimeEvent();// 定时运行且只运行一次的事件

			/// Queues callback in the loop thread.
			/// Runs after finish pooling.
			/// Safe to call from other threads.
			void queueInLoop(Functor cb);

			// 执行一次性事件
			void doPendingFunctors();
		};
	}
}