#include "EventLoop.h"

#include "../base/Logging.h"
#include "Poller.h"
#include "Channel.h"

#include <cassert>

#ifdef _DEBUG
#include <iostream>
#endif

using namespace std;
using namespace muduo::net;

thread_local EventLoop *t_loopInThisThread = nullptr;

EventLoop::EventLoop() :
	looping(false), doQuit(false),
	threadId(this_thread::get_id())
{
	assert(t_loopInThisThread == nullptr);
	t_loopInThisThread = this;

	poller.reset(new Poller(this));
}

EventLoop::~EventLoop()
{
	//assert(looping == false); // 因异常而退出循环时，looping不会复位。此行为应被允许
	t_loopInThisThread = nullptr;
}

bool muduo::net::EventLoop::IsLooping() const
{
	return looping;
}

void EventLoop::Quit()
{
	doQuit = true;
}

void EventLoop::Loop()
{
	using namespace std::chrono;
	assert(looping == false);
	assert(t_loopInThisThread == this);
	doQuit = false;
	looping = true;

	Poller::ChannelList channelList;

	Timestamp lastTime = Timestamp::now();
	while (!doQuit)
	{
		auto currTime = Timestamp::now();
		double dt = TimeDifference(currTime, lastTime);
		//LOG_INFO << "dt: " << dt << endl;
		SetConsoleTitle(to_wstring(dt).c_str());

		channelList.clear();
		poller->Poll(0, &channelList);
		for (auto it = channelList.cbegin(); it != channelList.cend(); ++it)
		{
			(*it)->HandleEvent();
		}

		// 一次性事件
		doPendingFunctors();

		// 每轮循环都要运行的事件
		doEveryLoopEvents();

		// 每隔一段时间运行的事件
		doEveryEventOnce();

		// 定时运行且只运行一次的事件
		doTimeEvent();

		this_thread::yield();

		lastTime = currTime;
	}
	looping = false;
}

void EventLoop::RunOnce(Functor cb)
{
	if (IsInLoopThread())
	{
		cb();
	}
	else
	{
		queueInLoop(std::move(cb));
	}
}

void EventLoop::runEveryLoop(const Functor &cb)
{
	lock_guard<mutex> lk(m);
	loopEvents.push_back(cb);
}

TimerId EventLoop::runEvery(int ms, const TimerCallback &cb)
{
	lock_guard<mutex> lk(m);
	everyEvents.push_back(EveryEvent(cb, chrono::milliseconds(ms)));
	return TimerId();
}

TimerId EventLoop::runAt(Timestamp timeStamp, const TimerCallback &cb)
{
	lock_guard<mutex> lk(m);
	timeEvents.insert({ timeStamp,{ cb,timeStamp } });
	return TimerId();
}

TimerId EventLoop::RunAfter(double sec, const TimerCallback &cb)
{
	lock_guard<mutex> lk(m);
	Timestamp timeStamp = Timestamp::now() + sec;
	timeEvents.insert({ timeStamp,{ cb,timeStamp } });
	return TimerId();
}

void muduo::net::EventLoop::AssertInLoopThread() const
{
	assert(IsInLoopThread());
}

void EventLoop::doEveryEventOnce()
{
	std::vector<EveryEvent> tempEveryEvents;

	{
		lock_guard<mutex> lk(m);
		tempEveryEvents = everyEvents;
	}

	for (auto &everyEvent : everyEvents)
	{
		if (Timestamp::now() >= everyEvent.lastActive + everyEvent.interval)
		{
			everyEvent.cb();
			everyEvent.lastActive = Timestamp::now();
		}
	}
}

void EventLoop::doTimeEvent()
{
	std::multimap<Timestamp, TimeEvent> events;

	{
		std::lock_guard<std::mutex> lk(m);
		auto now = Timestamp::now();
		auto it = timeEvents.lower_bound(now);
		events.insert(timeEvents.begin(), it);
		timeEvents.erase(timeEvents.begin(), it);
	}

	for (auto &pr : events)
	{
		pr.second.cb();
	}
}

void EventLoop::queueInLoop(Functor cb)
{
	{
		std::lock_guard<std::mutex> lk(m);
		pendingFunctors.push(std::move(cb));
	}
}

bool EventLoop::IsInLoopThread() const
{
	return this_thread::get_id() == threadId;
}

void EventLoop::UpdateChannel(Channel *channel)
{
	assert(channel->GetOwnerLoop() == this);
	assert(IsInLoopThread());
	poller->UpdateChannel(channel);
}

void EventLoop::doEveryLoopEvents()
{
	std::vector<Functor> functors;

	{
		lock_guard<mutex> lk(m);
		functors=loopEvents;
	}

	for (auto &cb : functors)
	{
		cb();
	}
}

void EventLoop::doPendingFunctors()
{
	std::queue<Functor> functors;

	{
		lock_guard<mutex> lk(m);
		functors.swap(pendingFunctors);
	}

	while (!functors.empty())
	{
		functors.front()();
		functors.pop();
	}
}

void muduo::net::EventLoop::RemoveChannel(Channel *channel)
{
	assert(channel->GetOwnerLoop() == this);
	assert(IsInLoopThread());
	poller->RemoveChannel(channel);
}

EventLoop *EventLoop::GetEventLoopOfCurrentThread()
{
	return t_loopInThisThread;
}
