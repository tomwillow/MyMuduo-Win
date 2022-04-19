#include "MyString.h"

#include <string.h>

namespace muduo
{
	MyString::MyString()
	{
		rawData = new char[1];
		rawData[0] = 0;
		len = 0;
	}

	MyString::~MyString()
	{
		delete[] rawData;
	}

	MyString::MyString(const char other[])
	{
		len = strlen(other);
		rawData = new char[len + 1];
		memcpy(rawData, other, len + 1);
	}

	MyString::MyString(const std::string &stdStr) :MyString(stdStr.c_str())
	{
	}

	MyString::MyString(const MyString &other)
	{
		len = other.len;
		rawData = new char[len + 1];
		memcpy(rawData, other.rawData, len + 1);
	}

	MyString::MyString(MyString &&other) noexcept
	{
		rawData = other.rawData;
		len = other.len;

		other.rawData = nullptr;
		other.len = 0;
	}

	MyString &MyString::operator=(const MyString &other)
	{
		if (&other == this) return *this;
		delete[] rawData;
		len = other.len;
		rawData = new char[len + 1];
		memcpy(rawData, other.rawData, len + 1);
		return *this;
	}

	MyString &MyString::operator=(MyString &&other) noexcept
	{
		if (&other == this) return *this;
		delete[] rawData;
		rawData = other.rawData;
		len = other.len;
		other.rawData = nullptr;
		other.len = 0;
		return *this;
	}

	void MyString::clear()
	{
		rawData[0] = 0;
		len = 0;
	}

	bool MyString::operator==(const MyString &rhs) const
	{
		return strcmp(rawData, rhs.c_str()) == 0;
	}

	bool MyString::operator==(const char *rhs) const
	{
		return strcmp(rawData, rhs) == 0;
	}

	MyString &MyString::operator+=(const MyString &rhs)
	{
		char *newData = new char[len + rhs.size() + 1];
		memcpy(newData, rawData, len);
		memcpy(newData + len, rhs.rawData, rhs.size() + 1);

		delete[] rawData;
		rawData = newData;
		len += rhs.size();
		return *this;
	}

	MyString MyString::operator+(const MyString &rhs) const
	{
		MyString ans;
		delete[] ans.rawData;

		ans.rawData = new char[len + rhs.size() + 1];
		memcpy(ans.rawData, rawData, len);
		memcpy(ans.rawData + len, rhs.rawData, rhs.size() + 1);

		ans.len = len + rhs.size();
		return ans;
	}

	MyString MyString::operator+(const char *rhs) const
	{
		MyString ans;
		delete[] ans.rawData;

		int rhsLen = strlen(rhs);
		ans.rawData = new char[len + rhsLen + 1];
		memcpy(ans.rawData, rawData, len);
		memcpy(ans.rawData + len, rhs, rhsLen + 1);

		ans.len = len + rhsLen;
		return ans;
	}

	MyString MyString::operator+(const std::string &rhs) const
	{
		return operator+(rhs.c_str());
	}

	std::ostream &operator<<(std::ostream &out, const MyString &s)
	{
		out.write(s.c_str(), s.size() + 1);
		return out;
	}

	bool operator==(const char *lhs, const MyString &rhs)
	{
		return strcmp(lhs, rhs.c_str());
	}


	MyString operator+(const char *lhs, const MyString &rhs)
	{
		int lhsLen = strlen(lhs);
		MyString ans;
		delete[] ans.rawData;

		ans.len = lhsLen + rhs.size();
		ans.rawData = new char[ans.len + 1];
		memcpy(ans.rawData, lhs, lhsLen);
		memcpy(ans.rawData + lhsLen, rhs.rawData, rhs.size() + 1);
		return ans;
	}

	/* -------------------------------------------------------------- */
	MyWString::MyWString()
	{
		rawData = new wchar_t[1];
		rawData[0] = 0;
		len = 0;
	}

	MyWString::~MyWString()
	{
		delete[] rawData;
	}

	MyWString::MyWString(const wchar_t other[])
	{
		len = wcslen(other);
		rawData = new wchar_t[len + 1];
		memcpy(rawData, other, (len + 1)*sizeof(wchar_t));
	}

	MyWString::MyWString(const std::wstring &stdStr) :MyWString(stdStr.c_str())
	{
	}

	MyWString::MyWString(const MyWString &other)
	{
		len = other.len;
		rawData = new wchar_t[len + 1];
		memcpy(rawData, other.rawData, (len + 1) * sizeof(wchar_t));
	}

	MyWString::MyWString(MyWString &&other) noexcept
	{
		rawData = other.rawData;
		len = other.len;

		other.rawData = nullptr;
		other.len = 0;
	}

	MyWString &MyWString::operator=(const MyWString &other)
	{
		if (&other == this) return *this;
		delete[] rawData;
		len = other.len;
		rawData = new wchar_t[len + 1];
		memcpy(rawData, other.rawData, (len + 1) * sizeof(wchar_t));
		return *this;
	}

	MyWString &MyWString::operator=(MyWString &&other) noexcept
	{
		if (&other == this) return *this;
		delete[] rawData;
		rawData = other.rawData;
		len = other.len;
		other.rawData = nullptr;
		other.len = 0;
		return *this;
	}

	void MyWString::clear()
	{
		rawData[0] = 0;
		len = 0;
	}

	bool MyWString::operator==(const MyWString &rhs) const
	{
		return wcscmp(rawData, rhs.c_str()) == 0;
	}

	bool MyWString::operator==(const wchar_t *rhs) const
	{
		return wcscmp(rawData, rhs) == 0;
	}

	MyWString &MyWString::operator+=(const MyWString &rhs)
	{
		wchar_t *newData = new wchar_t[len + rhs.size() + 1];
		memcpy(newData, rawData, (len) * sizeof(wchar_t));
		memcpy(newData + len, rhs.rawData, (rhs.size() + 1) * sizeof(wchar_t));

		delete[] rawData;
		rawData = newData;
		len += rhs.size();
		return *this;
	}

	MyWString MyWString::operator+(const MyWString &rhs) const
	{
		MyWString ans;
		delete[] ans.rawData;

		ans.rawData = new wchar_t[len + rhs.size() + 1];
		memcpy(ans.rawData, rawData, (len) * sizeof(wchar_t));
		memcpy(ans.rawData + len, rhs.rawData, (rhs.size() + 1) * sizeof(wchar_t));

		ans.len = len + rhs.size();
		return ans;
	}

	MyWString MyWString::operator+(const wchar_t *rhs) const
	{
		MyWString ans;
		delete[] ans.rawData;

		int rhsLen = wcslen(rhs);
		ans.rawData = new wchar_t[len + rhsLen + 1];
		memcpy(ans.rawData, rawData, (len) * sizeof(wchar_t));
		memcpy(ans.rawData + len, rhs, (rhsLen + 1) * sizeof(wchar_t));

		ans.len = len + rhsLen;
		return ans;
	}

	MyWString MyWString::operator+(const std::wstring &rhs) const
	{
		return operator+(rhs.c_str());
	}

	std::wostream &operator<<(std::wostream &out, const MyWString &s)
	{
		out.write(s.c_str(), s.size() + 1);
		return out;
	}

	std::ostream &operator<<(std::ostream &out, const MyWString &s)
	{
		out.write((const char*)s.c_str(), s.size() + 1);
		return out;
	}

	bool operator==(const wchar_t *lhs, const MyWString &rhs)
	{
		return wcscmp(lhs, rhs.c_str());
	}


	MyWString operator+(const wchar_t *lhs, const MyWString &rhs)
	{
		int lhsLen = wcslen(lhs);
		MyWString ans;
		delete[] ans.rawData;

		ans.len = lhsLen + rhs.size();
		ans.rawData = new wchar_t[ans.len + 1];
		memcpy(ans.rawData, lhs, (lhsLen) * sizeof(wchar_t));
		memcpy(ans.rawData + lhsLen, rhs.rawData, (rhs.size() + 1) * sizeof(wchar_t));
		return ans;
	}
}
