#pragma once

#include <vector>

const int md5_size = 16;

class MyMD5
{
public:
	MyMD5() { val = std::vector<char>(0, md5_size); }
	explicit MyMD5(const char md5_raw[]);
	explicit MyMD5(const void *data, int size);
	bool operator==(const MyMD5 &other) { return val == other.val; }
	bool operator!=(const MyMD5 &other) { return val != other.val; }
	bool operator<(const MyMD5 &other) const { return val < other.val; }

	const char *raw()const;

private:
	std::vector<char> val;
};

