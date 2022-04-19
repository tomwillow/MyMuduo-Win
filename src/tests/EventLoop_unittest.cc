#include "muduo/net/EventLoop.h"

#include <thread>

#include <assert.h>
#include <stdio.h>

using namespace std;
using namespace muduo;
using namespace muduo::net;

EventLoop* g_loop;

void callback()
{
  printf("callback(): pid = %d, tid = %d\n", _getpid(), this_thread::get_id());
  //EventLoop anotherLoop; // should trigger assert at here
}

void threadFunc()
{
  printf("threadFunc(): pid = %d, tid = %d\n", _getpid(), this_thread::get_id());

  assert(EventLoop::GetEventLoopOfCurrentThread() == NULL);
  EventLoop loop;
  assert(EventLoop::GetEventLoopOfCurrentThread() == &loop);
  loop.RunAfter(1.0, callback);
  loop.Loop();
}

int main()
{
  printf("main(): pid = %d, tid = %d\n", _getpid(), this_thread::get_id());

  assert(EventLoop::GetEventLoopOfCurrentThread() == NULL);
  EventLoop loop;
  assert(EventLoop::GetEventLoopOfCurrentThread() == &loop);

  thread thread(threadFunc);

  loop.Loop();

  thread.join();
}
