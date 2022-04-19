#include "echo.h"

#include "muduo/base/Logging.h"

#include <vector>
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

using namespace std;
using namespace muduo;
using namespace muduo::net;

EchoServer::EchoServer(muduo::net::EventLoop *loop,
	const muduo::net::InetAddress &listenAddr)
	: server_(loop, listenAddr, "EchoServer")
{
	server_.SetConnectionCallback(std::bind(&EchoServer::onConnection, this, _1));
	server_.SetCloseCallback(std::bind(&EchoServer::onClose, this, _1));
	server_.SetMessageCallback(std::bind(&EchoServer::onMessage, this, _1, _2, _3));
}

EchoServer::~EchoServer()
{
	sendLoop->Quit();
	th.join();

	server_.Quit();
}

void EchoServer::start()
{
	server_.SetThreadNum(10);
	server_.Start();

	th = thread(&EchoServer::SendJunk, this);
}

void EchoServer::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
	LOG_INFO << "EchoServer - " << conn->PeerAddress().ToIPPortString() << " -> "
		<< conn->LocalAddress().ToIPPortString() << endl;

	{
		lock_guard<mutex> lk(m);
		conns.push_back(conn);
	}
}

void EchoServer::onClose(const muduo::net::TcpConnectionPtr &conn)
{
	lock_guard<mutex> lk(m);
	for (auto it = conns.begin(); it != conns.end(); ++it)
	{
		if ((*it)->fd() == conn->fd())
		{
			conns.erase(it);
			return;
		}
	}
	assert(0);
	//auto it = find(conns.begin(), conns.end(), [&](const TcpConnectionPtr &thing) {return thing->fd() == conn->fd(); });
	//conns.erase(it);
}

void EchoServer::onMessage(const muduo::net::TcpConnectionPtr &conn,
	muduo::net::Buffer *buf,
	muduo::Timestamp time)
{
	muduo::string msg(buf->retrieveAllAsString());
	LOG_INFO << conn->GetId() << " echo " << msg.size() << " bytes, "
		<< "data received at " << time.ToString() << endl;
	conn->Send(msg);

}

void EchoServer::SendJunk()
{
	sendLoop.reset(new EventLoop);

	sendLoop->runEvery(1000,[&]()
		{
			{
				lock_guard<mutex> lk(m);
				for (auto &conn : conns)
				{
					string junk(1000 * 1000 * 100, 'a');
					//conn->Send(junk);
				}
			}
		});

	sendLoop->runEvery(5000,[&]()
		{
			//LOG << "shutdown" << endl;
			lock_guard<mutex> lk(m);
			for (auto &conn : conns)
			{
				//conn->ForceClose();
			}
		});

	sendLoop->Loop();
}

