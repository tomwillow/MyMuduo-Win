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

		// ָ��
		LogRecord &operator<<(void *p);

		// ���
		LogRecord &operator<<(HWND hWnd);

		// ʱ���
		LogRecord &operator<<(muduo::Timestamp timeStamp);

		// ���ַ���
		LogRecord &operator<<(const wchar_t s[]);

		// ansi�ַ���
		LogRecord &operator<<(const char s[]);

		// tstring
		LogRecord &operator<<(const muduo::wstring &s);

		// string
		LogRecord &operator<<(const muduo::string &s);

		// LogRecord���պ�����Ϊ<<���������
		LogRecord &operator<<(LogRecord &(*func)(LogRecord &));

		void Flush();
	private:
		std::ostream &out;
		muduo::string filename;
		std::mutex m;

		muduo::string buf;
	};

	// ����ģ��
	template <typename T>
	LogRecord &operator<<(LogRecord &logRecord, const T &t)
	{
		logRecord.AddBuffer(std::to_string(t));
		return logRecord;
	}

	// ��LogRecord֧��endl
	LogRecord &endl(LogRecord &logRecord);

}
