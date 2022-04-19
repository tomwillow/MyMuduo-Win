#include "TcpClient.h"

#include "EventLoop.h"
#include "TcpConnection.h"

muduo::net::TcpClient::TcpClient(EventLoop *loop, const InetAddress &addr, std::string name):
	loop(loop),
	name(name),
	connector(loop,addr)
{
	connector.SetNewConnectionCallback(std::bind(&TcpClient::OnNewConnection, this, std::placeholders::_1));
}

void muduo::net::TcpClient::SetConnectionCallback(const ConnectionCallback &cb)
{
	onConnection = cb;
}

void muduo::net::TcpClient::SetMessageCallback(const MessageCallback &cb)
{
	onMessage = cb;
}

void muduo::net::TcpClient::SetCloseCallback(const CloseCallback &cb)
{
	onClose = cb;
}

void muduo::net::TcpClient::Connect()
{
	connector.Start();
}

void muduo::net::TcpClient::OnNewConnection(SOCKET sock)
{
	loop->AssertInLoopThread();
	conn.reset(new TcpConnection(loop, sock, 0));

	conn->SetConnectionCallback(onConnection);
	conn->SetMessageCallback(onMessage);
	conn->SetCloseCallback(onClose);

	conn->ConnectEstablished();
}
