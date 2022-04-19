#pragma once

#include "../base/noncopyable.h"
#include "../base/Timestamp.h"

#include <unordered_map>
#include <unordered_set>
#include <WinSock2.h> // fd_set

namespace muduo
{
	namespace net
	{
		class Channel;
		class EventLoop;
		class Poller :noncopyable
		{
		public:
			using ChannelList = std::unordered_set<Channel *>;

			Poller(EventLoop *loop);

			// ����Channel����۲�
			void UpdateChannel(Channel *channel);

			// ����Channel�Ƴ��۲�
			void RemoveChannel(Channel *channel);

			// �����Ƿ��з����¼���Channel�����뵽activeChannels��
			Timestamp Poll(int timeoutMs, ChannelList *activeChannels);

		private:
			EventLoop *loop;
			fd_set fdRead, fdWrite;

			std::unordered_map<SOCKET, Channel *> channels; // ������Channel����Observe

			void FillActiveChannels(fd_set &tempFdRead, fd_set &tempFdWrite, ChannelList *activeChannels) const;
		};

	}
}

