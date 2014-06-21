#pragma once
#include <windows.h>

template <class DerivedWindow>
class WindowBase
{
public:
	WindowBase();
	virtual ~WindowBase();
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL Create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle = 0, int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT, int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT, HWND hWndParent = 0, HMENU hMenu = 0);
	HWND Window() const { return m_hWnd; }

protected:
	virtual PCWSTR ClassName() const = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	HWND m_hWnd;
};

