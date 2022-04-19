#include "Poller.h"

#include "../base/Logging.h"

#include "EventLoop.h"
#include "Channel.h"

#include <cassert>

using namespace muduo;
using namespace muduo::net;

Poller::Poller(EventLoop *loop) :loop(loop)
{
	FD_ZERO(&fdRead);
	FD_ZERO(&fdWrite);
}

void Poller::UpdateChannel(Channel *channel)
{
	assert(loop->IsInLoopThread());

	auto it = channels.find(channel->GetFd());

	if (it == channels.end())
	{
		// 添加观察
		channels[channel->GetFd()] = channel;
		LOG_INFO << "channels.size(): " << channels.size() << endl;

		// 不能超出Select限制
		assert(fdRead.fd_count < FD_SETSIZE);

		// 在Read集合中添加监听
		FD_SET(channel->GetFd(), &fdRead);

		if (channel->WantToWrite())
		{
			FD_SET(channel->GetFd(), &fdWrite);
		}
		else
		{
			FD_CLR(channel->GetFd(), &fdWrite);
		}

	}
	else
	{
		// 已有的Channel

		// 在Read集合中添加监听
		FD_SET(channel->GetFd(), &fdRead);

		if (channel->WantToWrite())
		{
			FD_SET(channel->GetFd(), &fdWrite);
		}
		else
		{
			FD_CLR(channel->GetFd(), &fdWrite);
		}
	}


}

void muduo::net::Poller::RemoveChannel(Channel *channel)
{
	assert(loop->IsInLoopThread());

	SOCKET fd = channel->GetFd();

	// 移除监听
	FD_CLR(fd, &fdRead);
	FD_CLR(fd, &fdWrite);

	// 从map中移除
	channels.erase(fd);
}

Timestamp Poller::Poll(int timeoutMs, ChannelList *activeChannels)
{
	if (fdRead.fd_count == 0)
		return Timestamp::now();

	timeval tv = { 0,timeoutMs };

	fd_set tempFdRead,tempFdWrite;
	memcpy(&tempFdRead, &fdRead, sizeof(fd_set));
	memcpy(&tempFdWrite, &fdWrite, sizeof(fd_set));

	// 调用select
	int numEvents = ::select(0, &tempFdRead, &tempFdWrite, nullptr, &tv);

	if (numEvents > 0)
	{
		// 有事件
		FillActiveChannels(tempFdRead,tempFdWrite, activeChannels);
	}
	else if (numEvents < 0)
	{
		// select出错
		int errCode = WSAGetLastError();
		LOG_SYSERR << "[ErrCode="<<errCode<< "] Poller::Poll()" << endl;
	}

	return Timestamp::now();
}

void Poller::FillActiveChannels(fd_set &tempFdRead, fd_set &tempFdWrite, ChannelList *activeChannels) const
{
	for (int i = 0; i < tempFdRead.fd_count; ++i)
	{
		SOCKET fd = tempFdRead.fd_array[i]; // 取得SOCKET
		Channel *channel = channels.at(fd); // 取得SOCKET对应的Channel

		// 标记Channel有读事件
		channel->SetCurrEvents(Channel::kReadEvent);

		assert(fd == channel->GetFd());

		activeChannels->insert(channel);
	}


	for (int i = 0; i < tempFdWrite.fd_count; ++i)
	{
		SOCKET fd = tempFdWrite.fd_array[i]; // 取得SOCKET
		Channel *channel = channels.at(fd); // 取得SOCKET对应的Channel

		// 标记Channel有写事件
		channel->SetCurrEvents(Channel::kWriteEvent);

		assert(fd == channel->GetFd());

		activeChannels->insert(channel);
	}
}
