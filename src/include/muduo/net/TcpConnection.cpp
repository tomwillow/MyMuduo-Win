#include "TcpConnection.h"

#include "../base/Logging.h"

#include "EventLoop.h"

#include <cassert>

using namespace std;
using namespace muduo;
using namespace muduo::net;

void muduo::net::defaultConnectionCallback(const TcpConnectionPtr &conn)
{
	LOG << "Connection: " << conn->LocalAddress().ToIPPortString() << " -> "
		<< conn->PeerAddress().ToIPPortString() << endl;
}

void muduo::net::defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time)
{
	LOG_INFO << "Message: " << conn->GetId() << " received " << buf->readableBytes() << endl;
	LOG << buf->retrieveAllAsString() << endl;
}

void muduo::net::defaultCloseCallback(const TcpConnectionPtr &conn)
{
	LOG << "Close: " << conn->LocalAddress().ToIPPortString() << " -> "
		<< conn->PeerAddress().ToIPPortString() << endl;
}

muduo::net::TcpConnection::TcpConnection(EventLoop *loop, SOCKET in_sock, unsigned int id) :
	loop(loop),
	state(kConnecting),
	channel(loop, in_sock),
	localAddr(in_sock, true),
	peerAddr(in_sock, false),
	id(id)
{
	LOG_INFO << "[" << id << "]\t" << "TcpConnection ctor. socket: " << in_sock << endl;
	channel.SetReadCallback(std::bind(&TcpConnection::HandleRead, this));
	channel.SetWriteCallback(std::bind(&TcpConnection::HandleWrite, this));
}

muduo::net::TcpConnection::~TcpConnection()
{
	LOG_INFO << "[" << id << "]\t" << "TcpConnection dtor. socket: " << channel.GetFd() << endl;
}

void muduo::net::TcpConnection::Send(const string &s)
{
	assert(state == kConnected);

	if (loop->IsInLoopThread())
	{
		SendInLoop(s);
	}
	else
	{
		loop->RunOnce(std::bind(&TcpConnection::SendInLoop, this, s));
	}
}

void muduo::net::TcpConnection::Send(const void *data, int size)
{
	string s;
	s.resize(size);
	const char *cData = static_cast<const char *>(data);
	std::copy(cData, cData + size, s.begin());
	Send(std::move(s));
}

void muduo::net::TcpConnection::ConnectEstablished()
{
	assert(loop->IsInLoopThread());

	assert(state == kConnecting);

	state = kConnected;

	//channel_->tie(shared_from_this());
	channel.EnableReading();

	onConnection(shared_from_this());
}

void muduo::net::TcpConnection::ConnectDestroyed()
{
	assert(loop->IsInLoopThread());

	assert(state == kConnected);

	state = kDisconnecting;

	loop->RemoveChannel(&channel);
}

void muduo::net::TcpConnection::ForceClose()
{
	assert(state == kConnected);

	if (loop->IsInLoopThread())
	{
		HandleClose();
	}
	else
	{
		loop->RunOnce(std::bind(&TcpConnection::HandleClose, this));
	}
}

const InetAddress &muduo::net::TcpConnection::LocalAddress() const 
{ 
	return localAddr;
}

const InetAddress &muduo::net::TcpConnection::PeerAddress() const 
{
	return peerAddr;
}

void muduo::net::TcpConnection::HandleRead()
{
	assert(loop->IsInLoopThread());

	int errCode = 0;
	auto ret = buf.readFd(channel.GetFd(), &errCode);
	if (ret > 0)
	{
		// 调用最外层的回调函数
		onMessage(shared_from_this(), &buf, Timestamp::now());
	}
	else
	{
		// 对方友好关闭
		if (ret == 0)
		{
			HandleClose();
			return;
		}
	}

	// 如果出错
	if (errCode)
	{
		if (errCode == WSAEWOULDBLOCK)
			return;

		LOG_SYSERR << "HandleRead Error: " << errCode << endl;
		HandleClose();
	}
}

void muduo::net::TcpConnection::HandleWrite()
{
	assert(loop->IsInLoopThread());

	if (channel.WantToWrite())
	{
		int nwrote = ::send(channel.GetFd(), sendBuf.peek(), sendBuf.readableBytes(), 0);
		if (nwrote > 0)
		{
			sendBuf.retrieve(nwrote);
			LOG_INFO << "HandleWrite: sent " << nwrote << endl;
			if (sendBuf.readableBytes() == 0) // 发完了
			{
				channel.DisableWriting();
			}
			else
			{
				// 下次再发
				LOG_INFO << "HandleWrite: I will send next time." << endl;
			}
		}
		else
		{
			int errCode = WSAGetLastError();
			if (errCode == WSAEWOULDBLOCK)
			{
				// 缓冲区满
			}
			else
			{
				// 其他错误
				LOG_SYSERR << "SendInLoop Error: " << errCode << endl;
				HandleClose();
				return;
			}
		}
	}
}

void muduo::net::TcpConnection::HandleClose()
{
	assert(loop->IsInLoopThread());

	assert(state == kConnected);

	channel.DisableWriting();

	onClose(shared_from_this());
}

void muduo::net::TcpConnection::SendInLoop(const muduo::string &msg)
{
	assert(loop->IsInLoopThread());

	int nwrote = 0;
	if (sendBuf.readableBytes() == 0)
	{
		// 缓冲为空，直接发送
		nwrote = ::send(channel.GetFd(), msg.data(), msg.size(), 0);

		// 发送出错
		if (nwrote < 0)
		{
			int errCode = WSAGetLastError();
			if (errCode == WSAEWOULDBLOCK)
			{
				// 缓冲区满
				nwrote = 0;
			}
			else
			{
				// 其他错误

				LOG_SYSERR << "SendInLoop Error: " << errCode << endl;
				HandleClose();
				return;
			}
		}
	}

	// 没发完的部分追加到缓冲
	if (nwrote < msg.size())
	{
		// 追加发送区buffer
		// 内存不足时sendBuf会抛出bad_alloc异常
		sendBuf.append(msg.data() + nwrote, msg.size() - nwrote);

		// 标记Channel有未发送的消息
		channel.EnableWriting();
	}

	LOG_INFO << "SendInLoop: send " << nwrote << endl;
}
