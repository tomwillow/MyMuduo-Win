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

			// 将此Channel加入观察
			void UpdateChannel(Channel *channel);

			// 将此Channel移除观察
			void RemoveChannel(Channel *channel);

			// 查找是否有发生事件的Channel，加入到activeChannels里
			Timestamp Poll(int timeoutMs, ChannelList *activeChannels);

		private:
			EventLoop *loop;
			fd_set fdRead, fdWrite;

			std::unordered_map<SOCKET, Channel *> channels; // 不持有Channel，仅Observe

			void FillActiveChannels(fd_set &tempFdRead, fd_set &tempFdWrite, ChannelList *activeChannels) const;
		};

	}
}

