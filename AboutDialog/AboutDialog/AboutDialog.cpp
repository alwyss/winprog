#include "AboutDialog.h"

#include <cstdio>
#include <stdarg.h>
#include <exception>
#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

int iCurrentColor = IDC_BLACK, iCurrentFigure = IDC_RECTANGLE;

void Output(const wchar_t* szFormat, ...)
{
	wchar_t szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnwprintf(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);

	OutputDebugString(szBuff);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
    static TCHAR szAppName[] = TEXT("AboutDialog");
    MSG msg;
    HWND hWnd;
    WNDCLASS wndClass;

    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(hInstance, szAppName);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = szAppName;
    wndClass.lpszClassName = szAppName;

    if(!RegisterClass(&wndClass))
    {
        MessageBox(NULL, TEXT("This program requires Windows NT"), szAppName, MB_ICONERROR);
        return 0;
    }

    hWnd = CreateWindow(szAppName, TEXT("About Box Demo Program"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void PaintTheWindow( HWND hWnd, int iColor, int iFigure )
{
	static COLORREF crColor[8] =
	{
		RGB(0, 0, 0), RGB(0, 0, 255),
		RGB(0, 255, 0), RGB(0, 255, 255),
		RGB(255, 0, 0), RGB(255, 0, 255),
		RGB(255, 255, 0), RGB(255, 255, 255)
	};

	HBRUSH hBrush;
	HDC hdc;
	RECT rect;

	hdc = GetDC(hWnd);
	GetClientRect(hWnd, &rect);
	hBrush = CreateSolidBrush(crColor[iColor - IDC_BLACK]);
	hBrush = (HBRUSH) SelectObject(hdc, hBrush);

	if(iFigure == IDC_RECTANGLE)
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	else
		Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);

	DeleteObject(SelectObject(hdc, hBrush));
	ReleaseDC(hWnd, hdc);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
    static HINSTANCE hInstance;
    switch (message)
    {
    case WM_CREATE:
        hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_HELP_ABOUT:
            if (DialogBox(hInstance, TEXT("AboutBox"), hWnd, AboutDlgProc))
            {
				InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        default:
            break;
        }
        break;

	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);

		PaintTheWindow(hWnd, iCurrentColor, iCurrentFigure);
		break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void PaintTheBlock( HWND hCtrl, int iColor, int iFigure )
{
	InvalidateRect(hCtrl, NULL, TRUE);
	UpdateWindow(hCtrl);
	PaintTheWindow(hCtrl, iColor, iFigure);
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hCtrlBlock;
    static int iColor, iFigure;
    switch (message)
    {
    case WM_INITDIALOG:
		iColor = iCurrentColor;
		iFigure = iCurrentFigure;

		CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, iColor);
		CheckRadioButton(hDlg, IDC_RECTANGLE, IDC_ELLIPSE, iFigure);
		hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT);

		SetFocus(GetDlgItem(hDlg, iColor));
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            iCurrentColor = iColor;
            iCurrentFigure = iFigure;
            EndDialog(hDlg, TRUE);
            break;

        case IDCANCEL:
            EndDialog(hDlg, FALSE);
			break;

        case IDC_BLACK:
        case IDC_RED:
        case IDC_GREEN:
        case IDC_YELLOW:
        case IDC_BLUE:
        case IDC_MAGENTA:
        case IDC_CYAN:
        case IDC_WHITE:
            iColor = LOWORD(wParam);
            CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, LOWORD(wParam));
            PaintTheBlock(hCtrlBlock, iColor, iFigure);
			return FALSE;
            break;

        case IDC_RECTANGLE:
        case IDC_ELLIPSE:
			iFigure = LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_RECTANGLE, IDC_ELLIPSE, LOWORD(wParam));
            PaintTheBlock(hCtrlBlock, iColor, iFigure);
			return FALSE;
			break;

        default:
            return FALSE;
        }
		return TRUE;

    case WM_PAINT:
		
		Output(TEXT("fsdfsdjfkl"));
        //PaintTheBlock(hCtrlBlock, iColor, iFigure);
		return TRUE;

    default:
        return FALSE;
    }

    return TRUE;
}

