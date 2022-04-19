#define _CRT_SECURE_NO_WARNINGS
#include "LogRecord.h"

#include <fstream>

#include <cassert>
#include <iostream>

#if (defined _WIN32) && (defined UNICODE)

#define to_tstring(a) string2wstring(a)
#else
#define to_tstring(a) (a)
#endif

muduo::LogRecord muduo::logRecord(std::cout);
muduo::LogRecord muduo::logSysErr(std::cerr);
muduo::LogRecord muduo::logTrace(std::cout);

namespace muduo
{
	std::string to_utf8(const std::wstring &wstr)
	{
		// 取得大小
		int nLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		if (nLen == 0) // 大小为0或者失败，返回空串
			return std::string("");

		// 分配空间
		std::unique_ptr<char[]> buf(new char[nLen]);

		// 进行转换
		nLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buf.get(), nLen, NULL, NULL);
		if (nLen == 0) // 大小为0或者失败，返回空串
			return std::string("");

		return std::string(buf.get());
	}

	std::string to_utf8(const std::string &str)
	{
		int nwLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		std::unique_ptr<wchar_t[]> pwBuf(new wchar_t[nwLen]);
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf.get(), nwLen);

		int nLen = WideCharToMultiByte(CP_UTF8, 0, pwBuf.get(), -1, NULL, NULL, NULL, NULL);
		std::unique_ptr<char[]> pBuf(new char[nLen]);
		WideCharToMultiByte(CP_UTF8, 0, pwBuf.get(), nwLen, pBuf.get(), nLen, NULL, NULL);

		return std::string(pBuf.get());
	}

	std::wstring string2wstring(const std::string &str)
	{
		LPCSTR pszSrc = str.c_str();
		int nLen = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, NULL, 0);
		if (nLen == 0)
			return std::wstring(L"");

		std::unique_ptr<wchar_t[]> up(new wchar_t[nLen]);
		if (!up)
			return std::wstring(L"");

		MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, up.get(), nLen);

		return std::wstring(up.get());
	}

	std::string wstring2string(const std::wstring &wstr)
	{
		// 取得大小
		int nLen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		if (nLen == 0) // 大小为0或者失败，返回空串
			return std::string("");

		// 分配空间
		std::unique_ptr<char[]> buf(new char[nLen]);

		// 进行转换
		nLen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, buf.get(), nLen, NULL, NULL);
		if (nLen == 0) // 大小为0或者失败，返回空串
			return std::string("");

		return std::string(buf.get());
	}

	//muduo::string wstring2string(const muduo::wstring &wstr)
	//{
	//	return wstring2string(std::wstring(wstr.c_str()));
	//}
	LogRecord::~LogRecord()
	{
		Flush();
	}

	void LogRecord::SetLogFileName(muduo::string in_filename)
	{
		filename = in_filename;

		std::ios::sync_with_stdio(false);    // Linux gcc.
		std::locale::global(std::locale(""));
		setlocale(LC_CTYPE, "");    // MinGW gcc.
		out.imbue(std::locale(""));
	}

	void LogRecord::AddBuffer(muduo::string content)
	{
		std::lock_guard<std::mutex> lk(m);
		buf += content;
	}

	void muduo::LogRecord::AddBuffer(muduo::wstring content)
	{
		std::lock_guard<std::mutex> lk(m);
		buf += wstring2string(content);
	}

	void LogRecord::Flush()
	{
		std::lock_guard<std::mutex> lk(m);

		out << Timestamp::now().ToString() << " " << buf;

		FILE *fp = nullptr;
		fopen_s(&fp, filename.c_str(), "ab+");
		if (fp)
		{

			string timestamp = Timestamp::now().ToString() + " ";

			fwrite(timestamp.c_str(), timestamp.size(), 1, fp);

			fwrite(buf.c_str(), buf.size(), 1, fp);
			fclose(fp);
		}

		buf.clear();
	}

	LogRecord &LogRecord::operator<<(void *p)
	{
		logRecord.AddBuffer(std::to_string((INT_PTR)p));
		return logRecord;
	}

	LogRecord &LogRecord::operator<<(HWND hWnd)
	{
		char buf[16];
		sprintf(buf, "%0p", hWnd);
		logRecord.AddBuffer(buf);
		return logRecord;
	}

	LogRecord &LogRecord::operator<<(Timestamp timeStamp)
	{
		logRecord.AddBuffer(timeStamp.ToString());
		return logRecord;
	}

	LogRecord &LogRecord::operator<<(const wchar_t s[])
	{
		logRecord.AddBuffer(s);
		return logRecord;
	}

	LogRecord &LogRecord::operator<<(const char s[])
	{
		logRecord.AddBuffer(s);
		return logRecord;
	}

	LogRecord &LogRecord::operator<<(const muduo::wstring &s)
	{
		logRecord.AddBuffer(s);
		return logRecord;
	}

	LogRecord &LogRecord::operator<<(const muduo::string &s)
	{
		logRecord.AddBuffer(s);
		return logRecord;
	}

	LogRecord &LogRecord::operator<<(LogRecord &(*func)(LogRecord &))
	{
		return func(logRecord);
	}

	LogRecord &muduo::endl(LogRecord &logRecord)
	{
		logRecord.AddBuffer(TEXT("\r\n"));
		logRecord.Flush();
		return logRecord;
	}
}
