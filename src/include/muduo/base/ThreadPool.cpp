#include "ThreadPool.h"

#include "../net/EventLoop.h"

#include <cassert>

using namespace std;
using namespace muduo;
using namespace muduo::net;

muduo::ThreadPool::ThreadPool(EventLoop *loop):baseLoop(loop),started(false),next(0),numThreads(thread::hardware_concurrency())
{
}

muduo::ThreadPool::~ThreadPool()
{
	for (auto &loop : loops)
	{
		loop->Quit();
	}
	for (auto &th : threads)
	{
		th.join();
	}
}

void muduo::ThreadPool::SetThreadNum(int numThreads)
{
	assert(numThreads > 0);
	assert(started == false);
	this->numThreads = numThreads;
}

void muduo::ThreadPool::Start()
{
	baseLoop->RunOnce(std::bind(&ThreadPool::StartBlocked,this));
}

void muduo::ThreadPool::Quit()
{
	for (auto &loop : loops)
	{
		loop->Quit();
	}
}

EventLoop *muduo::ThreadPool::GetNextLoop()
{
	if (!threads.empty())
	{
		if (next == loops.size())
		{
			next = 0;
		}
		return loops[next++].get();
	}
	return baseLoop;
}

void muduo::ThreadPool::StartBlocked()
{
	for (int i = 0; i < numThreads; ++i)
	{
		loops.push_back(unique_ptr<EventLoop>(new EventLoop()));
		threads.push_back(thread(std::bind(&EventLoop::Loop, loops.back().get())));
	}
	started = true;
}
