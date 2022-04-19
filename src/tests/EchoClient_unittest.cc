#include "muduo/net/TcpClient.h"

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include <muduo/net/TcpConnection.h>

#include <utility>

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;
using namespace std::placeholders;

int numThreads = 0;
class EchoClient;
std::vector<std::unique_ptr<EchoClient>> clients;
int current = 0;

class EchoClient : muduo::noncopyable
{
public:
	EchoClient(EventLoop *loop, const InetAddress &listenAddr, const string &id)
		: loop_(loop),
		client_(loop, listenAddr, "EchoClient" + id)
	{
		client_.SetConnectionCallback(
			std::bind(&EchoClient::onConnection, this, _1));
		client_.SetMessageCallback(
			std::bind(&EchoClient::onMessage, this, _1, _2, _3));
		//client_.enableRetry();
	}

	void connect()
	{
		client_.Connect();
	}
	// void stop();

private:
	void onConnection(const TcpConnectionPtr &conn)
	{
		LOG_TRACE << conn->LocalAddress().ToIPPortString() << " -> "
			<< conn->PeerAddress().ToIPPortString() << " is "
			<< (conn->IsConnected() ? "UP" : "DOWN");

		if (conn->IsConnected())
		{
			++current;
			if (static_cast<size_t>(current) < clients.size())
			{
				clients[current]->connect();
			}
			LOG_INFO << "*** connected " << current;
		}
		conn->Send("world\n");
	}

	void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time)
	{
		string msg(buf->retrieveAllAsString());
		LOG_TRACE << conn->GetId() << " recv " << msg.size() << " bytes at " << time.ToString();
		if (msg == "quit\n")
		{
			conn->Send("bye\n");
			conn->ForceClose();
		}
		else if (msg == "shutdown\n")
		{
			loop_->Quit();
		}
		else
		{
			conn->Send(msg);
		}
	}

	EventLoop *loop_;
	TcpClient client_;
};

int main(int argc, char *argv[])
{
	//LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
	if (argc > 1)
	{
		EventLoop loop;
		bool ipv6 = argc > 3;
		InetAddress serverAddr(argv[1], 2000, ipv6);

		int n = 1;
		if (argc > 2)
		{
			n = atoi(argv[2]);
		}

		clients.reserve(n);
		for (int i = 0; i < n; ++i)
		{
			char buf[32];
			snprintf(buf, sizeof buf, "%d", i + 1);
			clients.emplace_back(new EchoClient(&loop, serverAddr, buf));
		}

		clients[current]->connect();
		loop.Loop();
	}
	else
	{
		printf("Usage: %s host_ip [current#]\n", argv[0]);
	}
}

