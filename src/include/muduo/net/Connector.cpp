#include "Connector.h"

#include "../base/Logging.h"
#include "EventLoop.h"
#include "SocketFunction.h"

muduo::net::Connector::Connector(EventLoop *loop, const InetAddress &serverAddr) :
	loop(loop),
	serverAddr(serverAddr),
	coldTime(0.5)
{

}

void muduo::net::Connector::Start()
{
	loop->RunOnce(std::bind(&Connector::ConnectInLoop, this));
}

void muduo::net::Connector::SetNewConnectionCallback(NewConnectionCallback cb)
{
	onNewConnection = cb;
}

void muduo::net::Connector::ConnectInLoop()
{
	loop->AssertInLoopThread();
	SOCKET sock = CreateSocket(serverAddr.IsIPv6());
	int ret = Connect(sock, serverAddr.GetSockAddr());

	if (ret == NO_ERROR)
	{
		LOG << "connected " << endl;
		SetNonBlock(sock);
		onNewConnection(sock);
	}
	else
	{
		if (coldTime < 30.0)
			coldTime *= 2.0;
		else
			coldTime = 30.0;
		LOG << "connect fail. err="<<WSAGetLastError()<<". retry after " << coldTime << " seconds" << endl;
		loop->RunAfter(coldTime, std::bind(&Connector::ConnectInLoop, this));
	}
}
