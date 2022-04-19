#pragma once

#include "../base/Types.h"

#include <winsock2.h>

#include <vector>

#ifdef _WIN32
using ssize_t = int32_t;
#endif

namespace muduo
{
	namespace net
	{
		class Buffer
		{
		public:
			static const int kCheapPrepend = 8;
			static const int kInitialSize = 1024;

			Buffer();

			int readableBytes() const;
			int writableBytes() const;
			int prependableBytes() const;

			/* preappend operation */
			void prepend(const void *val, int len);
			void prependInt32(int32_t val);

			/* append operation */
			void append(const std::string &s);
			void append(const void *val, int len);

			void appendInt8(int8_t val);
			void appendInt16(int16_t val);
			void appendInt32(int32_t val);
			void appendInt64(int64_t val);

			ssize_t readFd(SOCKET sock, int *savedError);
			//char *writePos();
			//void moveWritePos(int distance);

			/* peek operation */
			const char *peek() const;
			int8_t peekInt8() const;
			int16_t peekInt16() const;
			int32_t peekInt32() const;
			int64_t peekInt64() const;

			/* read operation */
			void retrieve(int len);
			void retrieveAll();
			std::string retrieveAsString(int len);
			std::string retrieveAllAsString();

			int8_t readInt8();
			int16_t readInt16();
			int32_t readInt32();
			int64_t readInt64();
			void readCharArray(char *data, int len);

			const char *findEOL() const;
			const char *findEOL(const char *start) const;

			/* space operation */
			void shrink(size_t reserve);

			void ensureWritableBytes(int len);
		private:
			std::vector<char> data;
			int readIndex;
			int writeIndex;

			void makeSpace(int len);

		};
	}

}

