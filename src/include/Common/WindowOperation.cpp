#include "WindowOperation.h"

#include <LogRecord.h>

#include <stdexcept>
#include <functional>
#include <regex>
#include <cassert>

using namespace std;

//void WaitForSomeThing(std::function<void()> cb, int timeout)
//{
//	HWND hRet = 0;
//	while (timeout)
//	{
//		hRet = ::FindWindow(className, windowName);
//		if (hRet)
//		{
//			LOG << TEXT("ÕÒµ½£º") << windowName << TEXT("=") << hRet << endl;
//			return hRet;
//		}
//
//		Sleep(1000);
//		timeout--;
//		LOG << TEXT("Ñ°ÕÒ´°¿ÚÊ§°Ü£º") << windowName << TEXT(" Ê£Óà³¢ÊÔ£º") << timeout << endl;
//	}
//	tstring info = tstring(TEXT("Ñ°ÕÒ´°¿ÚÊ§°Ü£º")) + windowName;
//	throw runtime_error(to_string(info));
//}

HWND WaitForWindowOpen(const TCHAR className[], const TCHAR windowName[], int timeout)
{
	HWND hRet = 0;
	while (timeout)
	{
		hRet = ::FindWindow(className, windowName);
		if (hRet)
		{
			LOG << TEXT("ÕÒµ½£º") << windowName << TEXT("=") << hRet << endl;
			return hRet;
		}

		LOG << TEXT("Ñ°ÕÒ´°¿ÚÊ§°Ü£º") << windowName << TEXT(" Ê£Óà³¢ÊÔ£º") << timeout << endl;
		Sleep(1000);
		timeout--;
	}
	tstring info = tstring(TEXT("Ñ°ÕÒ´°¿ÚÊ§°Ü£º")) + windowName;
	throw runtime_error(to_string(info));
}


EnumWindowWrapper::WindowItem FindWindowByRegEx(HWND hParent, std::tstring pattern)
{
	EnumWindowWrapper wrapper;
	auto items = wrapper.DoEnumWindow();

	wregex r(pattern);
	wsmatch results;
	for (auto &item : items)
	{
		if (regex_match(item.windowName, results, r))
		{
			return item;
		}
	}
	return EnumWindowWrapper::WindowItem();
}

HWND WaitForWindowOpenByRegEx(std::tstring pattern, int timeout)
{
	EnumWindowWrapper::WindowItem hRet;
	assert(timeout > 0);
	while (timeout)
	{
		hRet = FindWindowByRegEx(0, pattern);
		if (hRet.hWnd)
		{
			LOG << TEXT("ÕÒµ½£º") << hRet.windowName << TEXT("=") << hRet.hWnd << endl;
			return hRet.hWnd;
		}

		Sleep(1000);
		timeout--;
		LOG << TEXT("Ñ°ÕÒ´°¿ÚÊ§°Ü(by regex)£º") << pattern << TEXT(" Ê£Óà³¢ÊÔ£º") << timeout << endl;
	}
	tstring info = tstring(TEXT("Ñ°ÕÒ´°¿ÚÊ§°Ü(by regex)£º")) + pattern;
	throw runtime_error(to_string(info));
}

void WaitForWindowDead(HWND hWnd, int timeout)
{
	while (timeout)
	{
		if (IsWindow(hWnd)==0)
		{
			LOG << TEXT("´°¿ÚÒÑ¹Ø±Õ£º") << hWnd << endl;
			return;
		}

		Sleep(1000);
		timeout--;
		LOG << TEXT("´°¿ÚÃ»ÓÐ¹Ø±Õ£º") << hWnd << TEXT(" Ê£Óà³¢ÊÔ£º") << timeout << endl;
	}
	tstring info = tstring(TEXT("´°¿ÚÃ»ÓÐÕý³£¹Ø±Õ£º")) + to_tstring(hWnd);
	throw runtime_error(to_string(info));
}

HWND FindWindowTree(HWND hParent, vector<WindowTree> nameTree)
{
	HWND ret = NULL;
	for (auto &tr : nameTree)
	{
		HWND hAfter = NULL;
		for (int i = 0; i <= tr.index; ++i)
		{
			hAfter = ::FindWindowEx(hParent, hAfter, tr.className, tr.windowName);
		}
		ret = hAfter;
		if (ret == NULL)
		{
			tstring info = TEXT("Ñ°ÕÒ´°¿ÚÊ§°Ü£º") + tr.ToString();
			throw runtime_error(to_string(info));
			return 0;
		}
		LOG << TEXT("ÕÒµ½£º") << tr.ToString() << TEXT("=") << ret << endl;
		hParent = ret;
	}
	return ret;
}

LRESULT SendBtnClickedEvent(HWND hParent, HWND hBtn)
{
	int wID = ::GetDlgCtrlID(hBtn);
	//LOG << TEXT("ID=") << wID << endl;
	return ::PostMessage(hParent, WM_COMMAND, MAKEWPARAM(wID, BN_CLICKED), (LPARAM)hBtn);
}

std::tstring GetLoginUserName()
{
	TCHAR buf[128];
	DWORD bufSize = sizeof(buf);
	if (!GetUserName(buf, &bufSize))
	{
		throw runtime_error("GetUserName failed");
	}
	return buf;
}

//void WaitForEtServerWindowAndClose()
//{
//	// DESKTOP-U7PBR5G[2] Á¬½Óµ½·þÎñÆ÷: 192.168.99.244[192.168.99.244]
//
//}

//HWND FindWindowByStyle(HWND hParent, DWORD style)
//{
//	return 0;
//}

LRESULT SendBtnClickedEvent(HWND hParent, UINT wID)
{
	HWND hWnd = GetDlgItem(hParent, wID);
	return ::PostMessage(hParent, WM_COMMAND, MAKEWPARAM(wID, BN_CLICKED), (LPARAM)hWnd);
}

LRESULT SendMenuActionEvent(HWND hParent, UINT wID)
{
	return ::PostMessage(hParent, WM_COMMAND, MAKEWPARAM(wID, BN_CLICKED), (LPARAM)0);
}

void Countdown(int second)
{
	while (second > 0)
	{
		LOG << TEXT("µ¹¼ÆÊ±£º") << second << endl;
		Sleep(1000);
		second--;
	}
}

void KillProcessByHWND(HWND hWnd)
{
	DWORD processId = 0;
	auto ret = GetWindowThreadProcessId(hWnd, &processId);
	if (ret == NULL)
	{
		throw runtime_error("GetWindowThreadProcessId failed");
	}
	const auto etSoft = OpenProcess(PROCESS_TERMINATE, false, processId);
	TerminateProcess(etSoft, 1);
	CloseHandle(etSoft);
}

std::tstring MyGetWindowText(HWND hWnd)
{
	TCHAR buf[MAX_PATH];
	GetWindowText(hWnd, buf, MAX_PATH);
	return std::tstring(buf);
}

void MySetWindowText(HWND hEdit, tstring s)
{
	::SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)s.c_str());

	unique_ptr<TCHAR[]> up(new TCHAR[s.size() + 1]);

	::SendMessage(hEdit, WM_GETTEXT, s.size() + 1, (LPARAM)up.get());
	up.get()[s.size()] = 0;

	if (up.get() != s)
	{
		throw runtime_error("MySetWindowText failed. Maybe need Administrator priority.");
	}
}


std::vector<EnumWindowWrapper::WindowItem> EnumWindowWrapper::windowItems;

BOOL CALLBACK EnumWindowWrapper::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	/*
	 * Remarks
		The EnumWindows function does not enumerate child windows,
		with the exception of a few top-level windows owned by the
		system that have the WS_CHILD style.
	 */
	TCHAR szTitle[MAX_PATH] = { 0 };
	TCHAR szClass[MAX_PATH] = { 0 };
	int nMaxCount = MAX_PATH;

	GetClassName(hwnd, szClass, nMaxCount);
	GetWindowText(hwnd, szTitle, nMaxCount);

	WindowItem item{ hwnd,szClass,szTitle };
	windowItems.push_back(item);

	return TRUE;
}

std::vector<EnumWindowWrapper::WindowItem> EnumWindowWrapper::DoEnumWindow()
{
	windowItems.clear();
	EnumWindows(EnumWindowsProc, 0);
	return windowItems;
}
