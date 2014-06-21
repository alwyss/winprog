// MemoryDcDemo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MemoryDcDemo.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MEMORYDCDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MEMORYDCDEMO));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MEMORYDCDEMO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MEMORYDCDEMO);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);

			// Test with two HDCs
			HDC hMemDcA = ::CreateCompatibleDC(hdc);
			HDC hMemDcB = ::CreateCompatibleDC(hdc);

			HBITMAP hBmp = ::CreateCompatibleBitmap(hdc, 100, 100);

			::SelectObject(hMemDcA, hBmp);

			// Wipe the canvas clean
			RECT base = {0, 0, 100, 100};
			::FillRect(hMemDcA, &base, (HBRUSH)::GetStockObject(WHITE_BRUSH));

			// Paint a small square in the upper left corner
			RECT rc1 = {10, 10, 60, 60};
			::FillRect(hMemDcA, &rc1, (HBRUSH)::GetStockObject(BLACK_BRUSH));

			::BitBlt( hdc, 10, 10, 100, 100,
				hMemDcA, 0, 0, SRCCOPY);
			// Delete the bitmap while it is selected
			BOOL isDel = ::DeleteObject(hBmp);
			// Yes, isDel is TRUE

			// Query info from the bitmap handle.
			BITMAP bm1 = {0};
			int retVal = ::GetObject(hBmp, sizeof(BITMAP), &bm1);
			// result: bm1 is populated appropriately, it appears hBmp is still valid.

			// Paint another shape with the same DC to test if the bitmap went away.
			RECT rc2 = {50, 50, 75, 90};
			::FillRect(hMemDcA, &rc2, (HBRUSH)::GetStockObject(BLACK_BRUSH));
			::BitBlt( hdc, 110, 10, 100, 100,
				hMemDcA, 0, 0, SRCCOPY);
			// result: The bitmap is still available and usable by the memDC.

			::DeleteDC(hMemDcA);

			// Query info from the bitmap handle.
			BITMAP bm2 = {0};
			int retVal2 = ::GetObject(hBmp, sizeof(BITMAP), &bm2);
			// result: The bitmap is no longer valid

			// Attempt to use the bitmap with the second memory DC.
			RECT rc3 = {80, 20, 100, 80};
			HGDIOBJ hOld = ::SelectObject(hMemDcB, hBmp);
			::FillRect(hMemDcB, &rc3, (HBRUSH)::GetStockObject(BLACK_BRUSH));

			::BitBlt( hdc, 210, 10, 100, 100,
				hMemDcB, 0, 0, SRCCOPY);
			// result: This operation fails as expected.

			::DeleteDC(hMemDcB);
			::DeleteObject(hBmp);

			EndPaint(hWnd, &ps);
			break;
		}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
