#include "muduo/net/InetAddress.h"

#include "muduo/base/Logging.h"

//#define BOOST_TEST_MODULE InetAddressTest
#define BOOST_TEST_MAIN
//#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using muduo::string;
using muduo::net::InetAddress;

BOOST_AUTO_TEST_CASE(testInetAddress)
{
  InetAddress addr0(1234);
  BOOST_CHECK_EQUAL(addr0.ToIPString(), string("0.0.0.0"));
  BOOST_CHECK_EQUAL(addr0.ToIPPortString(), string("0.0.0.0:1234"));
  BOOST_CHECK_EQUAL(addr0.GetPort(), 1234);

  InetAddress addr1((unsigned short)4321, true);
  BOOST_CHECK_EQUAL(addr1.ToIPString(), string("127.0.0.1"));
  BOOST_CHECK_EQUAL(addr1.ToIPPortString(), string("127.0.0.1:4321"));
  BOOST_CHECK_EQUAL(addr1.GetPort(), 4321);

  InetAddress addr2("1.2.3.4", 8888);
  BOOST_CHECK_EQUAL(addr2.ToIPString(), string("1.2.3.4"));
  BOOST_CHECK_EQUAL(addr2.ToIPPortString(), string("1.2.3.4:8888"));
  BOOST_CHECK_EQUAL(addr2.GetPort(), 8888);

  InetAddress addr3("255.254.253.252", 65535);
  BOOST_CHECK_EQUAL(addr3.ToIPString(), string("255.254.253.252"));
  BOOST_CHECK_EQUAL(addr3.ToIPPortString(), string("255.254.253.252:65535"));
  BOOST_CHECK_EQUAL(addr3.GetPort(), 65535);
}

BOOST_AUTO_TEST_CASE(testInet6Address)
{
  InetAddress addr0(1234, false, true);
  BOOST_CHECK_EQUAL(addr0.ToIPString(), string("::"));
  BOOST_CHECK_EQUAL(addr0.ToIPPortString(), string("[::]:1234"));
  BOOST_CHECK_EQUAL(addr0.GetPort(), 1234);

  InetAddress addr1(1234, true, true);
  BOOST_CHECK_EQUAL(addr1.ToIPString(), string("::1"));
  BOOST_CHECK_EQUAL(addr1.ToIPPortString(), string("[::1]:1234"));
  BOOST_CHECK_EQUAL(addr1.GetPort(), 1234);

  InetAddress addr2("2001:db8::1", 8888, true);
  BOOST_CHECK_EQUAL(addr2.ToIPString(), string("2001:db8::1"));
  BOOST_CHECK_EQUAL(addr2.ToIPPortString(), string("[2001:db8::1]:8888"));
  BOOST_CHECK_EQUAL(addr2.GetPort(), 8888);

  InetAddress addr3("fe80::1234:abcd:1", 8888);
  BOOST_CHECK_EQUAL(addr3.ToIPString(), string("fe80::1234:abcd:1"));
  BOOST_CHECK_EQUAL(addr3.ToIPPortString(), string("[fe80::1234:abcd:1]:8888"));
  BOOST_CHECK_EQUAL(addr3.GetPort(), 8888);
}

BOOST_AUTO_TEST_CASE(testInetAddressResolve)
{
  try
  {
      string hostname = "ipw.cn";
      InetAddress addr=InetAddress::Resolve(hostname, "",true);
      std::cout << hostname<< " resolved to " << addr.ToIPPortString() << std::endl;
  }
  catch (std::runtime_error &e)
  {
      std::cout << e.what() << std::endl;
  }
}
