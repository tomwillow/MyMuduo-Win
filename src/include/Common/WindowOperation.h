#pragma once

#include <tstring.h>

#include <Windows.h>
#include <vector>

// ��ͣ��Ѱ�Ҵ��ڡ�û�ҵ����׳��쳣
HWND WaitForWindowOpen(const TCHAR className[], const TCHAR windowName[], int timeout);

// ��ͣ�ذ���pattern������ʽָ���Ĵ��ڱ���Ѱ�Ҵ��ڡ�û�ҵ����׳��쳣
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

// ���ݴ���class��name������²��Ҵ���
HWND FindWindowTree(HWND hParent, std::vector<WindowTree> nameTree);

// ʹ��PostMessage����WM_COMMAND�е�BN_CLICKED��Ϣ
LRESULT SendBtnClickedEvent(HWND hParent, HWND hBtn);

// ʹ��PostMessage����WM_COMMAND�е�BN_CLICKED��Ϣ
LRESULT SendBtnClickedEvent(HWND hParent, UINT wID);

// ʹ��PostMessage����WM_COMMAND�е�BN_CLICKED��Ϣ
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