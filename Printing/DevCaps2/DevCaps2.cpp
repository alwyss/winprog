// DevCaps2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DoBasicInfo(HDC, HDC, int, int);
void DoOtherInfo(HDC, HDC, int, int);
void DoBitCodedCaps(HDC, HDC, int, int, int);
extern WNDCLASS CreateAndRegisterClass(HINSTANCE, TCHAR[]);

typedef struct  
{
	int iMask;
	TCHAR *szDesc;
}BITS;

#define IDM_DEVMODE 1000

//int _tmain(int argc, _TCHAR* argv[])
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppNamne[] = TEXT("DevCaps2");
	HWND hwnd;
	MSG msg;

	auto wndclass = CreateAndRegisterClass(hInstance, szAppNamne);
	if (wndclass.lpszClassName == nullptr)
	{
		return 0;
	}

	hwnd = CreateWindow(szAppNamne, NULL, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

WNDCLASS CreateAndRegisterClass(HINSTANCE hInstance, TCHAR szAppName[])
{
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = szAppName;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT"), szAppName, MB_ICONERROR);
	}

	return wndclass;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MENUITEMINFO menuItemInfo;
	PRINTER_INFO_4 *pinfo4;
	static DWORD dwNeeded, dwReturned;
	HANDLE hPrint;
	HMENU hMenu;
	static int nCurrentInfo = IDM_BASIC;
	PAINTSTRUCT ps;
	HDC hdcInfo = NULL;
	static TCHAR szDevice[32];
	static int nCurrentDevice = IDM_SCREEN;
	static TCHAR szWindowText[64];
	static int cxChar, cyChar;
	HDC hdc;
	TEXTMETRIC tm;

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		ReleaseDC(hwnd, hdc);

	case WM_SETTINGCHANGE:
		hMenu = GetSubMenu(GetMenu(hwnd), 0);
		while (GetMenuItemCount(hMenu) > 1)
		{
			DeleteMenu(hMenu, 1, MF_BYPOSITION);
		}

		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &dwNeeded, &dwReturned);
		pinfo4 = (PRINTER_INFO_4 *)malloc(dwNeeded);
		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE)pinfo4, dwNeeded, &dwNeeded, &dwReturned);
		for (DWORD i = 0; i < dwReturned; i++)
		{
			AppendMenu(hMenu, (i + 1) % 16 ? 0 : MF_MENUBARBREAK, i + 1, pinfo4[i].pPrinterName);
		}
		free(pinfo4);
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, MF_STRING, IDM_DEVMODE, TEXT("Properties"));
		wParam = IDM_SCREEN;

	case WM_COMMAND:
		hMenu = GetMenu(hwnd);
		if (LOWORD(wParam) == IDM_SCREEN || LOWORD(wParam) < IDM_DEVMODE)
		{
			CheckMenuItem(hMenu, nCurrentDevice, MF_UNCHECKED);
			nCurrentDevice = LOWORD(wParam);
			CheckMenuItem(hMenu, nCurrentDevice, MF_CHECKED);
		}
		else if (LOWORD(wParam) == IDM_DEVMODE)
		{
			GetMenuString(hMenu, nCurrentDevice, szDevice, sizeof(szDevice) / sizeof(TCHAR), MF_BYCOMMAND);
			//GetMenuItemInfo(hMenu, nCurrentDevice, false, &menuItemInfo);
			if (OpenPrinter(szDevice, &hPrint, NULL))
			{
				PrinterProperties(hwnd, hPrint);
				ClosePrinter(hPrint);
			}

		}
		else{
			CheckMenuItem(hMenu, nCurrentInfo, MF_UNCHECKED);
			nCurrentInfo = LOWORD(wParam);
			CheckMenuItem(hMenu, nCurrentInfo, MF_CHECKED);
		}
		InvalidateRect(hwnd, NULL, TRUE);
		break;

	case WM_PAINT:
		lstrcpyn(szWindowText, TEXT("Device Capabilities:"), sizeof(szWindowText)/sizeof(TCHAR));
		if (nCurrentDevice == IDM_SCREEN)
		{
			lstrcpyn(szDevice, TEXT("DISPLAY"), sizeof(szDevice)/sizeof(TCHAR));
			hdcInfo = CreateIC(szDevice, NULL, NULL, NULL);
		}
		else{
			hMenu = GetMenu(hwnd);
			GetMenuString(hMenu, nCurrentDevice, szDevice, sizeof(szDevice), MF_BYCOMMAND);
			hdcInfo = CreateIC(NULL, szDevice, NULL, NULL);
		}
		//TODO: else printer

		wcsncat_s(szWindowText, szDevice, sizeof(szDevice)/sizeof(wchar_t));
		SetWindowText(hwnd, szWindowText);

		hdc = BeginPaint(hwnd, &ps);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		if (hdcInfo)
		{
			switch (nCurrentInfo)
			{
			case IDM_BASIC:
				DoBasicInfo(hdc, hdcInfo, cxChar, cyChar);
				break;
			default:
				break;
			}
			DeleteDC(hdc);
		}
		EndPaint(hwnd, &ps);

		break;

	case WM_INITMENUPOPUP:
		if (lParam == 0)
		{
			EnableMenuItem(GetMenu(hwnd), IDM_DEVMODE, nCurrentDevice == IDM_SCREEN ? MF_GRAYED : MF_ENABLED);
		}
		break;

	

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void DoBasicInfo(HDC hdc, HDC hdcInfo, int cxChar, int cyChar)
{
	static struct{
		int nIndex;
		TCHAR *szDesc;
	}
	info[] =
	{
		HORZSIZE, TEXT("HORZSIZE Width in millimeters")
	};

	int i;
	TCHAR szBuffer[80];
	for (i = 0; i < sizeof(info) / sizeof(info[0]); i++)
	{
		TextOut(hdc, cxChar, (i + 1)*cyChar, szBuffer, _snwprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR),
			L"%-45s%8d", info[i].szDesc, GetDeviceCaps(hdcInfo, info[i].nIndex)));
	}
}