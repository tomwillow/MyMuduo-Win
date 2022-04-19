#pragma once

#include "noncopyable.h"

#include <vector>
#include <memory>
#include <thread>

namespace muduo
{
	namespace net
	{
		class EventLoop;
	}

	class ThreadPool :muduo::noncopyable
	{
	public:
		ThreadPool(muduo::net::EventLoop *loop);
		~ThreadPool();

		void SetThreadNum(int numThreads);

		void Start();

		void Quit();

		muduo::net::EventLoop *GetNextLoop();

	private:
		muduo::net::EventLoop *baseLoop;
		bool started;
		int next;
		int numThreads;
		std::vector<std::thread> threads;
		std::vector<std::unique_ptr<muduo::net::EventLoop>> loops;

		void StartBlocked();
	};
}

