#pragma once

// 用于提供Winsock库加载
// 同名.cpp文件中已经提供一个MyInitSock类静态实例。
// 所以，编译时只要此同名.cpp已编译，就可以自动加载Winsock库。
// 若进行了多次加载，只要没有卸载过，就不会出错。
class MyInitSock
{
public:
	MyInitSock();

	~MyInitSock();
};

