#include "WindowBase.h"


template <class DerivedWindow>
WindowBase<DerivedWindow>::WindowBase() : m_hWnd(NULL)
{
}

template <class DerivedWindow>
WindowBase<DerivedWindow>::~WindowBase()
{
}

template <class DerivedWindow>
LRESULT WindowBase<DerivedWindow>::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DerivedWindow *pThis = nullptr;
	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
		pThis = (DerivedWindow *)pCreate->lpCreateParams;
		SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)pThis);
		pThis->m_hWnd = hWnd;
	}
	else
	{
		pThis = (DerivedWindow *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	if (pThis)
	{
		return pThis->HandleMessage(uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, wParam, lParam);
	}

	return 0;
}

template <class DerivedWindow>
BOOL  WindowBase<DerivedWindow>::Create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, int x,
	int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = DerivedWindow::WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = ClassName();

	RegisterClass(&wc);
	m_hWnd = CreateWindowEx(dwExStyle, ClassName(), lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this);

	return (m_hWnd ? TRUE : FALSE);
}
