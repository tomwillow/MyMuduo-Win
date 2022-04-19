#ifndef MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H
#define MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H

#include "muduo/net/TcpServer.h"
#include <muduo/net/EventLoop.h>

#include <thread>

// RFC 862
class EchoServer
{
public:
	EchoServer(muduo::net::EventLoop *loop,
		const muduo::net::InetAddress &listenAddr);

	~EchoServer();

	void start();  // calls server_.start();

private:
	void onConnection(const muduo::net::TcpConnectionPtr &conn);
	void onClose(const muduo::net::TcpConnectionPtr &conn);

	void onMessage(const muduo::net::TcpConnectionPtr &conn,
		muduo::net::Buffer *buf,
		muduo::Timestamp time);

	muduo::net::TcpServer server_;

	std::mutex m;
	std::thread th;
	std::unique_ptr<muduo::net::EventLoop> sendLoop;
	std::vector<muduo::net::TcpConnectionPtr> conns;

	void SendJunk();
};

#endif  // MUDUO_EXAMPLES_SIMPLE_ECHO_ECHO_H
