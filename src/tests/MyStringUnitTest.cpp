
#include <muduo/base/Types.h>

//#define BOOST_TEST_MODULE BufferTest
#define BOOST_TEST_MAIN
//#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(TestMyString)
{
	muduo::string s;
	BOOST_CHECK_EQUAL(s.size(), 0);
}

BOOST_AUTO_TEST_CASE(TestMyWString)
{
	muduo::wstring s;
	BOOST_CHECK_EQUAL(s.size(), 0);
}

BOOST_AUTO_TEST_CASE(TestMyStringCopy)
{
	{
		muduo::string s = "aaa";
		muduo::string s2(s);
		BOOST_CHECK_EQUAL(s.size(), 3);
		BOOST_CHECK_EQUAL(s2.size(), 3);
		BOOST_CHECK_NE((void *)s.c_str(), (void *)s2.c_str());
	}

	{
		muduo::string s = "aaa";
		muduo::string s2 = s;
		BOOST_CHECK_EQUAL(s.size(), 3);
		BOOST_CHECK_EQUAL(s2.size(), 3);
		BOOST_CHECK_NE((void *)s.c_str(), (void *)s2.c_str());
	}

	{
		muduo::string s = "aaa";
		s = s;
		BOOST_CHECK_EQUAL(s.size(), 3);
	}
}

BOOST_AUTO_TEST_CASE(TestMyWStringCopy)
{
	{
		muduo::wstring s = L"aaa";
		muduo::wstring s2(s);
		BOOST_CHECK_EQUAL(s.size(), 3);
		BOOST_CHECK_EQUAL(s2.size(), 3);
		BOOST_CHECK_NE((void *)s.c_str(), (void *)s2.c_str());
	}

	{
		muduo::wstring s = L"aaa";
		muduo::wstring s2 = s;
		BOOST_CHECK_EQUAL(s.size(), 3);
		BOOST_CHECK_EQUAL(s2.size(), 3);
		BOOST_CHECK_NE((void *)s.c_str(), (void *)s2.c_str());
	}

	{
		muduo::wstring s = L"aaa";
		s = s;
		BOOST_CHECK_EQUAL(s.size(), 3);
	}
}

BOOST_AUTO_TEST_CASE(TestMyStringMove)
{
	{
		muduo::string s = "aaa";
		muduo::string s2(std::move(s));
		BOOST_CHECK_EQUAL(s.size(), 0);
		BOOST_CHECK_EQUAL(s2.size(), 3);
	}

	{
		muduo::string s = "aaa";
		muduo::string s2 = std::move(s);
		BOOST_CHECK_EQUAL(s.size(), 0);
		BOOST_CHECK_EQUAL(s2.size(), 3);
	}

	{
		muduo::string s = "aaa";
		s = std::move(s);
		BOOST_CHECK_EQUAL(s.size(), 3);
	}
}

BOOST_AUTO_TEST_CASE(TestMyWStringMove)
{
	{
		muduo::wstring s = L"aaa";
		muduo::wstring s2(std::move(s));
		BOOST_CHECK_EQUAL(s.size(), 0);
		BOOST_CHECK_EQUAL(s2.size(), 3);
	}

	{
		muduo::wstring s = L"aaa";
		muduo::wstring s2 = std::move(s);
		BOOST_CHECK_EQUAL(s.size(), 0);
		BOOST_CHECK_EQUAL(s2.size(), 3);
	}

	{
		muduo::wstring s = L"aaa";
		s = std::move(s);
		BOOST_CHECK_EQUAL(s.size(), 3);
	}
}

BOOST_AUTO_TEST_CASE(TestMyStringWithChar)
{
	std::string stdStr("hello");
	muduo::string s("hello");
	BOOST_CHECK_EQUAL(s.size(), stdStr.size());
	BOOST_CHECK_EQUAL(s, "hello");
	BOOST_CHECK_EQUAL("hello", s);
}

BOOST_AUTO_TEST_CASE(TestMyWStringWithChar)
{
	std::wstring stdStr(L"hello");
	muduo::wstring s(L"hello");
	BOOST_CHECK_EQUAL(s.size(), stdStr.size());
	BOOST_CHECK_EQUAL(s, L"hello");
	BOOST_CHECK_EQUAL(L"hello", s);
}

BOOST_AUTO_TEST_CASE(TestMyStringAdd)
{
	muduo::string s("hello");
	BOOST_CHECK_EQUAL(s.size(), 5);

	BOOST_CHECK_EQUAL(s + "world", "helloworld");
	BOOST_CHECK_EQUAL(s.size(), 5);

	s += "nono";
	BOOST_CHECK_EQUAL(s, "hellonono");
	BOOST_CHECK_EQUAL(s.size(), 9);

	s.clear();
	s += "abc";
	BOOST_CHECK_EQUAL(s, "abc");
	BOOST_CHECK_EQUAL(s.size(), 3);

	s = "but " + s;
	BOOST_CHECK_EQUAL(s, "but abc");
	BOOST_CHECK_EQUAL(s.size(), 7);
}

BOOST_AUTO_TEST_CASE(TestMyWStringAdd)
{
	muduo::wstring s(L"hello");
	BOOST_CHECK_EQUAL(s.size(), 5);

	BOOST_CHECK_EQUAL(s + L"world", L"helloworld");
	BOOST_CHECK_EQUAL(s.size(), 5);

	s += L"nono";
	BOOST_CHECK_EQUAL(s, L"hellonono");
	BOOST_CHECK_EQUAL(s.size(), 9);

	s.clear();
	s += L"abc";
	BOOST_CHECK_EQUAL(s, L"abc");
	BOOST_CHECK_EQUAL(s.size(), 3);

	s = L"but " + s;
	BOOST_CHECK_EQUAL(s, L"but abc");
	BOOST_CHECK_EQUAL(s.size(), 7);
}