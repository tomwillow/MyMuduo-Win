#include "MyMD5.h"

#include <boost/uuid/detail/md5.hpp>

MyMD5::MyMD5(const char md5_raw[])
{
	for (int i = 0; i < md5_size; ++i)
		val.push_back(md5_raw[i]);
}

MyMD5::MyMD5(const void *data, int size)
{
	boost::uuids::detail::md5 boost_md5;
	boost_md5.process_bytes(data, size);
	boost::uuids::detail::md5::digest_type digest;
	boost_md5.get_digest(digest);
	const char *char_digest = reinterpret_cast<const char *>(&digest);
	for (int i = 0; i < sizeof(digest); ++i)
	{
		val.push_back(char_digest[i]);
	}

	//str_md5.clear();
	//boost::algorithm::hex(char_digest, char_digest + sizeof(boost::uuids::detail::md5::digest_type), std::back_inserter(str_md5));

}

const char *MyMD5::raw() const
{
	return val.data();
}
