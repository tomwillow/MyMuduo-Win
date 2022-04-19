#include "echo.h"

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"


using namespace muduo;
// using namespace muduo::net;

int main()
{
	LOG_INFO.SetLogFileName("echo.log");
	LOG_INFO << "pid = " << _getpid() << endl;
	muduo::net::EventLoop loop;
	muduo::net::InetAddress listenAddr(2000);
	EchoServer server(&loop, listenAddr);
	server.start();

	loop.RunAfter(1, [&]() {
		//loop.Quit();
		});
	loop.Loop();

	return 0;
}

