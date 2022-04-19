#include <muduo/net/Connector.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;

void NewConnection(SOCKET sock, const muduo::net::InetAddress &peerAddr)
{
	LOG_INFO << "NewConnection: " << peerAddr.ToIPPortString() << endl;
	const char buf[] = "Hello!";
	::send(sock, buf, sizeof(buf), 0);
	::closesocket(sock);
}

int main()
{
	EventLoop loop;
	InetAddress serverAddr("127.0.0.1",1234);
	Connector connector(&loop, serverAddr);

	connector.Start();

	loop.Loop();

	return 0;
}