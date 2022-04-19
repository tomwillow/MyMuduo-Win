#pragma once

#include <iostream>

namespace muduo
{
	class MyString
	{
	private:
		char *rawData;
		int len;
	public:
		MyString();
		MyString(const char other[]);
		MyString(const std::string &stdStr);
		MyString(const MyString &other);
		MyString(MyString &&other) noexcept;

		~MyString();

		MyString &operator=(const MyString &other);
		MyString &operator=(MyString &&other) noexcept;

		int size() const { return len; }

		const char *data() const { return rawData; }

		const char *c_str() const { return rawData; }

		void clear();

		bool operator==(const MyString &rhs) const;
		bool operator==(const char *rhs) const;

		MyString &operator+=(const MyString &rhs);

		MyString operator+(const MyString &rhs) const;

		MyString operator+(const std::string &rhs) const;
		MyString operator+(const char *rhs) const;
		friend MyString operator+(const char *lhs, const MyString &rhs);
	};

	std::ostream &operator<<(std::ostream &out, const MyString &s);

	bool operator==(const char *lhs, const MyString &rhs);

	MyString operator+(const char *lhs, const MyString &rhs);

	/* -------------------------------------------------------------- */
	class MyWString
	{
	private:
		wchar_t *rawData;
		int len;
	public:
		MyWString();
		MyWString(const wchar_t other[]);
		MyWString(const std::wstring &stdStr);
		MyWString(const MyWString &other);
		MyWString(MyWString &&other) noexcept;

		~MyWString();

		MyWString &operator=(const MyWString &other);
		MyWString &operator=(MyWString &&other) noexcept;

		int size() const { return len; }

		const wchar_t *data() const { return rawData; }

		const wchar_t *c_str() const { return rawData; }

		void clear();

		bool operator==(const MyWString &rhs) const;
		bool operator==(const wchar_t *rhs) const;

		MyWString &operator+=(const MyWString &rhs);

		MyWString operator+(const MyWString &rhs) const;

		MyWString operator+(const std::wstring &rhs) const;
		MyWString operator+(const wchar_t *rhs) const;
		friend MyWString operator+(const wchar_t *lhs, const MyWString &rhs);
	};

	std::wostream &operator<<(std::wostream &out, const MyWString &s);
	std::ostream &operator<<(std::ostream &out, const MyWString &s);

	bool operator==(const wchar_t *lhs, const MyWString &rhs);

	MyWString operator+(const wchar_t *lhs, const MyWString &rhs);

}