#include "TcpServer.h"

#include "EventLoop.h"

#include <cassert>

#ifdef _DEBUG
//#include <iostream>
#endif

using namespace std;
using namespace muduo::net;

TcpServer::TcpServer(EventLoop *loop, const InetAddress &addr, std::string name):
	loop(loop),addr(addr),name(name),
	onConnection(defaultConnectionCallback),
	onMessage(defaultMessageCallback),
	onClose(defaultCloseCallback),
	threadPool(loop),
	acceptor(loop,addr,name),
	nextConnId(0)
{
}

TcpServer::~TcpServer()
{
	threadPool.Quit();
}

void muduo::net::TcpServer::Start()
{
	acceptor.SetNewConnectionCallback(std::bind(&TcpServer::NewConnection,this,placeholders::_1,placeholders::_2));
	acceptor.Listen();

}

void muduo::net::TcpServer::Quit()
{
	for (auto &pr : connections)
	{
		RemoveConnection(pr.second);
	}
	connections.clear();

	threadPool.Quit();
}

void muduo::net::TcpServer::SetThreadNum(int numThread)
{
	assert(numThread >= 0);
	assert(loop->IsLooping() == false);

	if (numThread>0)
		threadPool.SetThreadNum(numThread);
}

void muduo::net::TcpServer::SetConnectionCallback(const ConnectionCallback &cb)
{
	onConnection = cb;
}

void muduo::net::TcpServer::SetMessageCallback(const MessageCallback &cb)
{
	onMessage = cb;
}

void muduo::net::TcpServer::SetCloseCallback(const CloseCallback &cb)
{
	onClose = cb;
}

void muduo::net::TcpServer::NewConnection(SOCKET sock, const InetAddress &peerAddr)
{
	assert(loop->IsInLoopThread());

	//SetSendBufSize(sock, 1024);
	//SetNonBlock(sock);

	// 若反复连接、断开，以致nextConnId发生回环，此循环确保ConnId不重合
	while (connections.find(nextConnId)!=connections.end())
	{
		nextConnId++;
	}

	InetAddress localAddr(sock, true);
	TcpConnectionPtr conn(new TcpConnection(threadPool.GetNextLoop(), sock, nextConnId));

	connections[nextConnId] = conn;
	nextConnId++;

	//
	conn->SetConnectionCallback(onConnection);
	conn->SetMessageCallback(onMessage);
	conn->SetCloseCallback(std::bind(&TcpServer::RemoveConnection, this, placeholders::_1));
	conn->ConnectEstablished();
}

void muduo::net::TcpServer::RemoveConnection(const TcpConnectionPtr &conn)
{
	loop->RunOnce(std::bind(&TcpServer::RemoveConnectionInLoop, this, conn));
}

void muduo::net::TcpServer::RemoveConnectionInLoop(const TcpConnectionPtr &conn)
{
	assert(loop->IsInLoopThread());

	onClose(conn);

	auto n=connections.erase(conn->GetId());
	assert(n == 1);

	// ConnectDestroyed需要放到队列里执行，因为loop正在遍历ChannelList，此时删掉channel会导致迭代器失效
	loop->RunOnce(std::bind(&TcpConnection::ConnectDestroyed, conn.get()));
}
