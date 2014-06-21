#pragma once
#include <windows.h>

class PopPrint
{
public:
	PopPrint();
	~PopPrint();
	static BOOL CALLBACK PrintDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static HWND m_hDlgPrint;
	static BOOL m_bUserAborted;
};

