#pragma once

#include "Timestamp.h"
#include "Types.h"

#include <mutex>
#include <sstream>

namespace muduo
{
	class LogRecord;
	extern LogRecord logRecord;
	extern LogRecord logSysErr;
	extern LogRecord logTrace;

	class LogRecord
	{
	public:
		LogRecord(std::ostream &out):out(out) {}
		~LogRecord();

		void SetLogFileName(muduo::string filename);
		void AddBuffer(muduo::string content);
		void AddBuffer(muduo::wstring content);

		// 指针
		LogRecord &operator<<(void *p);

		// 句柄
		LogRecord &operator<<(HWND hWnd);

		// 时间戳
		LogRecord &operator<<(muduo::Timestamp timeStamp);

		// 宽字符串
		LogRecord &operator<<(const wchar_t s[]);

		// ansi字符串
		LogRecord &operator<<(const char s[]);

		// tstring
		LogRecord &operator<<(const muduo::wstring &s);

		// string
		LogRecord &operator<<(const muduo::string &s);

		// LogRecord接收函数作为<<运算符参数
		LogRecord &operator<<(LogRecord &(*func)(LogRecord &));

		void Flush();
	private:
		std::ostream &out;
		muduo::string filename;
		std::mutex m;

		muduo::string buf;
	};

	// 接收模板
	template <typename T>
	LogRecord &operator<<(LogRecord &logRecord, const T &t)
	{
		logRecord.AddBuffer(std::to_string(t));
		return logRecord;
	}

	// 让LogRecord支持endl
	LogRecord &endl(LogRecord &logRecord);

}
