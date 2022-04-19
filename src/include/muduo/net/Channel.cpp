#include "Channel.h"

#include "EventLoop.h"
#include "../base/Logging.h"

#include <cassert>

using namespace muduo;
using namespace muduo::net;

const int Channel::kNoneEvent=0;
const int Channel::kReadEvent=1;
const int Channel::kWriteEvent=2;

muduo::net::Channel::Channel(EventLoop *loop, SOCKET fd):loop(loop),fd(fd)
{
    LOG_INFO << "[" << fd << "]\t" << "Channel ctor()" << endl;
}

muduo::net::Channel::~Channel()
{
    LOG_INFO << "[" << fd << "]\t" << "Channel dtor()" << endl;
    ::closesocket(fd);
}

EventLoop *muduo::net::Channel::GetOwnerLoop() const
{
    return loop;
}

SOCKET muduo::net::Channel::GetFd() const
{
    return fd;
}

void muduo::net::Channel::SetCurrEvents(int currEvents)
{
    this->currEvents = currEvents;
}

void muduo::net::Channel::SetReadCallback(const EventCallback &cb)
{
    readCallback = cb;
}

void muduo::net::Channel::SetWriteCallback(const EventCallback &cb)
{
    writeCallback = cb;
}

void muduo::net::Channel::EnableReading()
{
    careEvents |= kReadEvent;
    Update();
}

void muduo::net::Channel::EnableWriting()
{
    careEvents |= kWriteEvent;
    Update();
}

void muduo::net::Channel::DisableWriting()
{
    careEvents &= ~kWriteEvent;
    Update();
}

bool muduo::net::Channel::WantToWrite() const
{
    return careEvents & kWriteEvent;
}

void muduo::net::Channel::HandleEvent()
{
    assert(loop->IsInLoopThread());

    if (currEvents & kReadEvent)
    {
        readCallback();
    }

    if (currEvents & kWriteEvent)
    {
        writeCallback();
    }

    // 回调结束后，currEvents的复位应由Channel的Owner进行
}

void muduo::net::Channel::Update()
{
    loop->UpdateChannel(this);
}
