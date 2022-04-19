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
		// ��ӹ۲�
		channels[channel->GetFd()] = channel;
		LOG_INFO << "channels.size(): " << channels.size() << endl;

		// ���ܳ���Select����
		assert(fdRead.fd_count < FD_SETSIZE);

		// ��Read��������Ӽ���
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
		// ���е�Channel

		// ��Read��������Ӽ���
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

	// �Ƴ�����
	FD_CLR(fd, &fdRead);
	FD_CLR(fd, &fdWrite);

	// ��map���Ƴ�
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

	// ����select
	int numEvents = ::select(0, &tempFdRead, &tempFdWrite, nullptr, &tv);

	if (numEvents > 0)
	{
		// ���¼�
		FillActiveChannels(tempFdRead,tempFdWrite, activeChannels);
	}
	else if (numEvents < 0)
	{
		// select����
		int errCode = WSAGetLastError();
		LOG_SYSERR << "[ErrCode="<<errCode<< "] Poller::Poll()" << endl;
	}

	return Timestamp::now();
}

void Poller::FillActiveChannels(fd_set &tempFdRead, fd_set &tempFdWrite, ChannelList *activeChannels) const
{
	for (int i = 0; i < tempFdRead.fd_count; ++i)
	{
		SOCKET fd = tempFdRead.fd_array[i]; // ȡ��SOCKET
		Channel *channel = channels.at(fd); // ȡ��SOCKET��Ӧ��Channel

		// ���Channel�ж��¼�
		channel->SetCurrEvents(Channel::kReadEvent);

		assert(fd == channel->GetFd());

		activeChannels->insert(channel);
	}


	for (int i = 0; i < tempFdWrite.fd_count; ++i)
	{
		SOCKET fd = tempFdWrite.fd_array[i]; // ȡ��SOCKET
		Channel *channel = channels.at(fd); // ȡ��SOCKET��Ӧ��Channel

		// ���Channel��д�¼�
		channel->SetCurrEvents(Channel::kWriteEvent);

		assert(fd == channel->GetFd());

		activeChannels->insert(channel);
	}
}
