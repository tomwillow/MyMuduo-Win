#pragma once

#include <tstring.h>

#include <Windows.h>
#include <vector>

// 不停地寻找窗口。没找到则抛出异常
HWND WaitForWindowOpen(const TCHAR className[], const TCHAR windowName[], int timeout);

// 不停地按照pattern正则表达式指定的窗口标题寻找窗口。没找到则抛出异常
HWND WaitForWindowOpenByRegEx(std::tstring pattern, int timeout);

void WaitForWindowDead(HWND hWnd,int timeout);

struct WindowTree
{
	const TCHAR *className;
	const TCHAR *windowName;
	int index;
	std::tstring ToString() const
	{
		std::tstring s = TEXT("(");
		if (className) s += className;
		s += TEXT(",");
		if (windowName) s += windowName;
		s += TEXT(")");
		return s;
	}
};

// 根据窗口class和name逐层向下查找窗口
HWND FindWindowTree(HWND hParent, std::vector<WindowTree> nameTree);

// 使用PostMessage发送WM_COMMAND中的BN_CLICKED消息
LRESULT SendBtnClickedEvent(HWND hParent, HWND hBtn);

// 使用PostMessage发送WM_COMMAND中的BN_CLICKED消息
LRESULT SendBtnClickedEvent(HWND hParent, UINT wID);

// 使用PostMessage发送WM_COMMAND中的BN_CLICKED消息
LRESULT SendMenuActionEvent(HWND hParent, UINT wID);

void Countdown(int second);

void KillProcessByHWND(HWND hWnd);

std::tstring MyGetWindowText(HWND hWnd);
void MySetWindowText(HWND hEdit, std::tstring s);

// not thread-safe
class EnumWindowWrapper
{
public:
	struct WindowItem
	{
		HWND hWnd;
		std::tstring className;
		std::tstring windowName;
		WindowItem() :hWnd(0) {}
		WindowItem(HWND hWnd,std::tstring className,std::tstring windowName) :
			hWnd(hWnd),className(className),windowName(windowName) {}
	};

	EnumWindowWrapper() {}
	std::vector<WindowItem> DoEnumWindow();

private:
	static std::vector<WindowItem> windowItems;
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
};