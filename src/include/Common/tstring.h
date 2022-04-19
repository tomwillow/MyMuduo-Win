#pragma once
#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>//TEXT macro
#include <tchar.h>

#ifdef _UNICODE

#define tstring wstring

#define tios wios

#define tistream wistream

#define tostream wostream

#define tifstream wifstream

#define tofstream wofstream

#define tstringstream wstringstream

#define tistringstream wistringstream

#define tostringstream wostringstream

#define tstreambuf wstreambuf

#define tcout wcout

#define to_tstring to_wstring

#define to_tstring_from_utf8 utf8_to_wstring

#define utf8_to_tstring utf8_to_wstring

#else

#define tstring string

#define tios ios

#define tistream istream

#define tostream ostream

#define tifstream ifstream

#define tofstream ofstream

#define tstringstream stringstream

#define tistringstream istringstream

#define tostringstream ostringstream

#define tstreambuf streambuf

#define tcout cout

#define to_tstring to_string

#define to_tstring_from_utf8 utf8_to_string

#define utf8_to_tstring utf8_to_string

#endif

std::string to_string(const std::wstring &ws);
std::string to_string(const std::string &s);

std::wstring to_wstring(const std::string &s);
std::wstring to_wstring(const std::wstring &s);
std::wstring to_wstring(const wchar_t ws[]);
std::wstring to_wstring(HWND hWnd);

void tolower(std::string &s);
void toupper(std::string &s);

std::string to_utf8(const std::wstring &wstr);
std::string to_utf8(const std::string &str);

std::string utf8_to_string(const std::string &str);
std::wstring utf8_to_wstring(const std::string &str);

std::string to_hex(std::string s);
std::wstring to_hex(std::wstring s);

std::tistream &safeGetline(std::tistream &is, std::tstring &t);