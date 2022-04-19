#include "Acceptor.h"

#include "SocketFunction.h"
#include "EventLoop.h"
#include "../base/Logging.h"

#include <cassert>

using namespace std;
using namespace muduo;
using namespace muduo::net;

muduo::net::Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr, muduo::string name):
	loop(loop),addr(addr),name(name),
	acceptChannel(loop, CreateSocket(addr.IsIPv6()))
{
	LOG_INFO << "[" << name << "]\t" << "Acceptor ctor()" << endl;

	SOCKET sock = acceptChannel.GetFd();
	SetNonBlock(sock);

	acceptChannel.SetReadCallback(std::bind(&Acceptor::HandleRead, this)); // 注册Read回调
	acceptChannel.EnableReading(); // 启用Read事件，将自己注册到EventLoop的Poller里

	// bind
	int ret = ::bind(sock, this->addr.GetSockAddr(), sizeof(*this->addr.GetSockAddr()));
	if (ret == SOCKET_ERROR)
	{
		throw socket_error("Failed bind(): port = " + to_string(this->addr.GetPort()), WSAGetLastError());
	}
}

muduo::net::Acceptor::~Acceptor()
{
	LOG_INFO <<"["<<name<<"]\t"<< "Acceptor dtor()" << endl;
}

void muduo::net::Acceptor::Listen()
{
	assert(loop->IsInLoopThread());

	// listen
	SOCKET sock = acceptChannel.GetFd();
	int ret = listen(sock, 10);
	if (ret == SOCKET_ERROR)
	{
		throw socket_error("Failed listen(): port = " + to_string(this->addr.GetPort()), WSAGetLastError());
	}
}

void muduo::net::Acceptor::HandleRead()
{
	assert(loop->IsInLoopThread());

	sockaddr_in remoteAddr;
#ifdef _WIN32
	int addrLen = sizeof(remoteAddr);
#else
	unsigned int addrLen = sizeof(remoteAddr);
#endif

	//接收到客户端的连接
	SOCKET sClient = accept(acceptChannel.GetFd(), (sockaddr *)&remoteAddr, &addrLen);
	if (sClient == INVALID_SOCKET)
	{
		throw socket_error("Failed accept().", WSAGetLastError());
		return;
	}

	newConnectionCallback(sClient, InetAddress(sClient, false));
}

void muduo::net::Acceptor::SetNewConnectionCallback(const NewConnectionCallback &cb)
{
	newConnectionCallback = cb;
}
