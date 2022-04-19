// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.
#pragma once

#include <stdint.h>

#ifdef _WIN32
#include <WinSock2.h>
#undef max

#pragma comment(lib, "Ws2_32.lib")
#else
#include <endian.h>
#endif

namespace muduo
{
	namespace net
	{
		namespace sockets
		{

#ifdef _WIN32

			inline uint64_t hostToNetwork64(uint64_t host64)
			{
				return htonll(host64);
			}

			inline uint32_t hostToNetwork32(uint32_t host32)
			{
				return htonl(host32);
			}

			inline uint16_t hostToNetwork16(uint16_t host16)
			{
				return htons(host16);
			}

			inline uint64_t networkToHost64(uint64_t net64)
			{
				return ntohll(net64);
			}

			inline uint32_t networkToHost32(uint32_t net32)
			{
				return ntohl(net32);
			}

			inline uint16_t networkToHost16(uint16_t net16)
			{
				return ntohs(net16);
			}

#else

			inline uint64_t hostToNetwork64(uint64_t host64)
			{
				return htobe64(host64);
			}

			inline uint32_t hostToNetwork32(uint32_t host32)
			{
				return htobe32(host32);
			}

			inline uint16_t hostToNetwork16(uint16_t host16)
			{
				return htobe16(host16);
			}

			inline uint64_t networkToHost64(uint64_t net64)
			{
				return be64toh(net64);
			}

			inline uint32_t networkToHost32(uint32_t net32)
			{
				return be32toh(net32);
			}

			inline uint16_t networkToHost16(uint16_t net16)
			{
				return be16toh(net16);
			}
#endif
		}
	}
}