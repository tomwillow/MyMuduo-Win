#include "Buffer.h"

#include "Endian.h"

#include <algorithm>
#include <cassert>

const int bufSize = 10240;

using namespace muduo::net;
using namespace muduo::net::sockets;

/*

			-------------------------------------------------
name:		|  prependable  |    readable   |    writable   |
			-------------------------------------------------
content:	|               |xxxxxxxxxxxxxxx|               |
			-------------------------------------------------
			^               ^               ^               ^
			|               |               |               |
			0           readIndex       writeIndex

initial:      kCheapPrepend                    kInitialSize

*/

Buffer::Buffer() :readIndex(kCheapPrepend), writeIndex(kCheapPrepend)
{
	data.resize(writeIndex + kInitialSize);
}

int Buffer::readableBytes() const
{
	return writeIndex - readIndex;
}

int Buffer::writableBytes() const
{
	return data.size() - writeIndex;
}

int Buffer::prependableBytes() const
{
	return readIndex;
}

void Buffer::prepend(const void *val, int len)
{
	assert(len <= prependableBytes());
	readIndex -= len;
	const char *d = static_cast<const char *>(val);
	std::copy(d, d + len, data.begin() + readIndex);
}

void Buffer::prependInt32(int32_t val)
{
	val = hostToNetwork32(val);
	prepend(&val, sizeof(int32_t));
}

void Buffer::append(const std::string &s)
{
	ensureWritableBytes(s.size());
	std::copy(s.begin(), s.end(), data.begin() + writeIndex);
	writeIndex += s.size();
}

void Buffer::append(const void *val, int len)
{
	ensureWritableBytes(len);
	const char *d = static_cast<const char *>(val);

	// 写入内容：[d, d+len]；
	// 写入目标：data.begin()+writeIndex
	std::copy(d,d+len, data.begin() + writeIndex);
	writeIndex += len;
}

void Buffer::appendInt8(int8_t val)
{
	append(&val, sizeof(int8_t));
}

void Buffer::appendInt16(int16_t val)
{
	val = hostToNetwork16(val);
	append(&val, sizeof(int16_t));
}

void Buffer::appendInt32(int32_t val)
{
	val = hostToNetwork32(val);
	append(&val, sizeof(int32_t));
}

void Buffer::appendInt64(int64_t val)
{
	val = hostToNetwork64(val);
	append(&val, sizeof(int64_t));
}

ssize_t muduo::net::Buffer::readFd(SOCKET sock, int *savedError)
{
	char buf[bufSize];
	int totalRecvLen = 0;
	while (1)
	{
		int recvLen = ::recv(sock, buf, bufSize, 0);
		if (recvLen == 0)
			break;
		else
			if (recvLen > 0)
			{
				append(buf, recvLen);
				totalRecvLen += recvLen;
			}
			else
			{
				*savedError=WSAGetLastError();
				break;
			}
	}
	return totalRecvLen;
}

//char *Buffer::writePos()
//{
//	return &data[writeIndex];
//}
//
//void Buffer::moveWritePos(int distance)
//{
//	assert(writableBytes() >= distance);
//	writeIndex += distance;
//}

void Buffer::retrieve(int len)
{
	assert(len >= 0);
	assert(len <= readableBytes());

	readIndex += len;

	if (readableBytes() == 0)
	{
		readIndex = writeIndex = kCheapPrepend;
	}
}

void Buffer::retrieveAll()
{
	readIndex = writeIndex = kCheapPrepend;
}

std::string Buffer::retrieveAsString(int len)
{
	assert(len >= 0);
	assert(len <= readableBytes());

	std::string s;
	s.insert(s.begin(), data.begin() + readIndex, data.begin() + readIndex + len);
	readIndex += len;

	if (readableBytes() == 0)
	{
		readIndex = writeIndex = kCheapPrepend;
	}
	return s;
}

std::string Buffer::retrieveAllAsString()
{
	std::string s;
	s.insert(s.begin(), data.begin() + readIndex, data.begin() + writeIndex);
	readIndex = writeIndex = kCheapPrepend;
	return s;
}

const char *Buffer::peek() const
{
	return &data[readIndex];
}

int8_t Buffer::peekInt8() const
{
	assert(readableBytes() >= sizeof(int8_t));
	return data[readIndex];
}

int16_t Buffer::peekInt16() const
{
	assert(readableBytes() >= sizeof(int16_t));
	int16_t val = *reinterpret_cast<const int16_t *>(peek());
	val = networkToHost16(val);
	return val;
}

int32_t Buffer::peekInt32() const
{
	assert(readableBytes() >= sizeof(int32_t));
	int32_t val = *reinterpret_cast<const int32_t *>(peek());
	val = networkToHost32(val);
	return val;
}

int64_t Buffer::peekInt64() const
{
	assert(readableBytes() >= sizeof(int64_t));
	int64_t val = *reinterpret_cast<const int64_t *>(peek());
	val = networkToHost64(val);
	return val;
}

int8_t Buffer::readInt8()
{
	assert(readableBytes() >= sizeof(int8_t));
	int8_t val = data[readIndex];
	readIndex += sizeof(int8_t);
	if (readableBytes() == 0)
	{
		readIndex = writeIndex = kCheapPrepend;
	}
	return val;
}

int16_t Buffer::readInt16()
{
	assert(readableBytes() >= sizeof(int16_t));
	int16_t val = networkToHost16(*reinterpret_cast<const int16_t *>(peek()));
	readIndex += sizeof(int16_t);
	if (readableBytes() == 0)
	{
		readIndex = writeIndex = kCheapPrepend;
	}
	return val;
}

int32_t Buffer::readInt32()
{
	assert(readableBytes() >= sizeof(int32_t));
	int32_t val = networkToHost32(*reinterpret_cast<const int32_t *>(peek()));
	readIndex += sizeof(int32_t);
	if (readableBytes() == 0)
	{
		readIndex = writeIndex = kCheapPrepend;
	}
	return val;
}

int64_t Buffer::readInt64()
{
	assert(readableBytes() >= sizeof(int64_t));
	int64_t val = networkToHost64(*reinterpret_cast<const int64_t *>(peek()));
	readIndex += sizeof(int64_t);
	if (readableBytes() == 0)
	{
		readIndex = writeIndex = kCheapPrepend;
	}
	return val;
}

void Buffer::readCharArray(char *data, int len)
{
	assert(len >= 0);
	assert(readableBytes() >= len);

	memcpy(data, peek(), len);
	readIndex += len;
	if (readableBytes() == 0)
	{
		readIndex = writeIndex = kCheapPrepend;
	}
}

const char *muduo::net::Buffer::findEOL() const
{
	return findEOL(peek());
}

const char *muduo::net::Buffer::findEOL(const char *start) const
{
	auto it = std::find(data.begin() + readIndex, data.begin() + writeIndex, 0);
	if (it==data.end())
		return nullptr;
	return &(*it);
}

void Buffer::shrink(size_t reserve)
{
	makeSpace(reserve);
	data.resize(std::max(writeIndex,kCheapPrepend+kInitialSize));
}

void Buffer::ensureWritableBytes(int len)
{
	assert(len >= 0);

	if (writableBytes() < len)
	{
		makeSpace(len);
	}
	assert(writableBytes() >= len);
}

void Buffer::makeSpace(int len)
{
	assert(len >= 0);

	if (writableBytes() + prependableBytes() < len + kCheapPrepend)
	{
		if (prependableBytes() > kCheapPrepend)
		{
			int shrink_size = readIndex - kCheapPrepend;
			data.erase(data.begin() + kCheapPrepend, data.begin() + readIndex);
			readIndex -= shrink_size;
			writeIndex -= shrink_size;
		}
		data.resize(writeIndex + len);
	}
	else
	{
		// move readable data to the front, make space inside buffer
		assert(kCheapPrepend < readIndex);
		size_t readable = readableBytes();
		std::copy(data.begin() + readIndex,
			data.begin() + writeIndex,
			data.begin() + kCheapPrepend);
		readIndex = kCheapPrepend;
		writeIndex = readIndex + readable;

		assert(readable == readableBytes());
	}
}
