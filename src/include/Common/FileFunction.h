#pragma once

#include "tstring.h"
#include <vector>
#include <fstream>
#include <functional>
#include <Windows.h>
#include <commdlg.h> // OPENFILENAME

class TFileDialog
{
private:
	OPENFILENAME ofn;
	TCHAR szTitle[MAX_PATH];
	TCHAR szFile[MAX_PATH];
	TCHAR szFilter[MAX_PATH];
	std::string initialDir;
public:
	TFileDialog() = delete;
	TFileDialog(const TFileDialog&) = delete;
	TFileDialog(HWND hwndOwner)
	{
		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwndOwner;
		szFile[0] = 0;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH;
		
		ofn.nFilterIndex = 1;
		ofn.lpstrInitialDir = NULL;
	}

	//示例：m_hWnd, {{"txt文本文件","*.txt"}}
	TFileDialog(HWND hwndOwner, std::vector<std::pair<std::tstring, std::tstring>> vecFilter):TFileDialog(hwndOwner)
	{
		SetFilter(vecFilter);
	}

	void SetFilter(std::vector<std::pair<std::tstring, std::tstring>> vecFilter);

	void SetTitle(std::tstring title);

	bool Open(std::tstring& fileName);

	void SetszFile(const std::tstring& s);

	std::tstring GetszFile();

	bool Save(std::tstring& fileName);
};

class TFolderBrowser
{
public:
	TFolderBrowser();
	void Init(HWND hwndOwner,std::tstring title=TEXT("请选择一个文件夹"));

	//fileName用于赋予初始路径，若用户点了取消，将不会对值产生影响
	bool Open(std::tstring& fileName);
private:
	std::tstring title;
	HWND hWndOwner;
};

// 自定义的文件IO异常。继承自runtime_error
class file_io_error :public std::runtime_error
{
public:
	std::tstring _filename;
	file_io_error(std::string s, std::tstring filename) :std::runtime_error(s), _filename(filename) {}
	const std::tstring &filename() { return _filename; }
};

//传入index=1则得到传入文件名
//失败返回空串
std::wstring GetCommandLineByIndex(int index);

//判断文件是否存在
bool GetFileExists(const std::tstring filename);

//判断是否是文件夹
bool IsFolder(const std::tstring dir);

//分割完整路径为 {路径，文件名不带后缀，.后缀}
std::vector<std::tstring> SplitPath(const std::tstring& s);

//从[文件名+后缀]的字符串中分割出 { 文件名不带后缀，.后缀 }
std::vector<std::tstring> SplitFileName(const std::tstring& s);

// 从完整路径得到 文件名+后缀，若本身不含正反斜杠，则返回自身
std::tstring GetNameAndExt(std::tstring s);

//e.g. ext="txt"
std::tstring ChangeExtend(std::tstring fileName, std::tstring ext);

//e.g. ext="txt"
std::tstring GetExtend(std::tstring fileName);

//取得文件大小，不改变读写位置
uint64_t GetFileSize(FILE* fp);

//取得文件大小
//失败抛出file_io_error类型自定义异常
uint64_t GetFileSize(std::tstring fileName);

std::tstring FileSizeToTString(uint64_t fileSize);

//失败抛出file_io_error类型自定义异常
void ReadFileToBuffer(std::tstring fileName, std::unique_ptr<unsigned char[]> &buf, uint64_t &bufSize);

//失败抛出file_io_error类型自定义异常
void WriteFileFromBuffer(std::tstring fileName, const char buf[], uint64_t bufSize);

//弹出文件对话框，然后按照给定文件名写入一个文本文件
//传入的filename会在文件对话框中显示为默认文件名
//fnWrite中定义要写入的内容，不需要进行打开关闭操作
//写入成功或者失败均会弹出对话框提示
void WriteDetailFile(HWND hWnd, std::tstring filename, std::function<void(std::tofstream & ofs)> fnWrite);

//失败抛出file_io_error类型自定义异常
std::vector<std::tstring> TraversalAllFileNames(std::tstring lpPath, std::vector<std::tstring> dotextNames = {}, bool enterSubFolder = true);