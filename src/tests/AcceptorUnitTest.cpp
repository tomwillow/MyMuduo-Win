#include <muduo/net/Acceptor.h>
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
	InetAddress listenAddr(1234);
	Acceptor acceptor(&loop, listenAddr, "test");

	acceptor.SetNewConnectionCallback(NewConnection);
	acceptor.Listen();
	LOG_INFO << "Listenning..." << endl;

	loop.Loop();

	return 0;
}