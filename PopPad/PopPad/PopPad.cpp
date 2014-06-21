#include <windows.h>
#include "resource.h"

#define ID_EDIT 1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT("PopPad");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    HACCEL hAccel;
    HWND hWnd;
    MSG msg;
    WNDCLASS wndClass;
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(hInstance, szAppName);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = szAppName;
    wndClass.lpszClassName = szAppName;

    if(!RegisterClass(&wndClass))
    {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
        return 0;
    }

    hWnd = CreateWindow(szAppName, szAppName,
        WS_OVERLAPPEDWINDOW, GetSystemMetrics(SM_CXSCREEN)/4,
        GetSystemMetrics(SM_CYSCREEN)/4,
        GetSystemMetrics(SM_CXSCREEN)/2,
        GetSystemMetrics(SM_CYSCREEN)/2, 
        NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    //hAccel = LoadAccelerators(HINSTANCE, szAppName);

    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

int AskConfirmation( HWND hWnd )
{
    return MessageBox(hWnd, TEXT("Really want to close PopPad?"), szAppName, MB_YESNO | MB_ICONQUESTION);
    //throw std::exception("The method or operation is not implemented.");
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hwndEdit;
    int iSelect, iEnable;
   
    switch (message)
    {
    case WM_CREATE:
        hwndEdit = CreateWindow(TEXT("edit"), NULL,
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
            0, 0, 0, 0, hWnd, (HMENU) ID_EDIT,
            ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        break;

    case WM_SETFOCUS:
        SetFocus(hwndEdit);
        break;

    case WM_SIZE:
        MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        break;

    case WM_INITMENUPOPUP:
        if(lParam == 1) // popup menu item index(0-based)
        {
            // EM_CANUNDO message returns whether the edit control can perform the undo action or not
            EnableMenuItem((HMENU) wParam, IDM_EDIT_UNDO,
                SendMessage(hwndEdit, EM_CANUNDO, 0, 0) ? MF_ENABLED : MF_GRAYED);

            // IsClipboardFormatAvailable(CF_TEXT) checks if the clipboard contains text
            EnableMenuItem((HMENU) wParam, IDM_EDIT_PASTE, IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED);

            iSelect = SendMessage(hwndEdit, EM_GETSEL, 0, 0);

            // low word is the position of the first selected character; the high word is the position of the character following the selection
            // if both are equal, no text has been selected
            if(HIWORD(iSelect) == LOWORD(iSelect))
                iEnable = MF_GRAYED;
            else
                iEnable = MF_ENABLED;
            EnableMenuItem((HMENU)wParam, IDM_EDIT_CUT, iEnable);
            EnableMenuItem((HMENU)wParam, IDM_EDIT_COPY, iEnable);
            EnableMenuItem((HMENU)wParam, IDM_EDIT_DELETE, iEnable);
        }
        break;

    case WM_COMMAND:
        if(lParam)
        {
            if(LOWORD(lParam) == ID_EDIT && (HIWORD(wParam) == EN_ERRSPACE || HIWORD(wParam) == EN_MAXTEXT))
                MessageBox(hWnd, TEXT("Edit control out of space"), szAppName, MB_OK | MB_ICONSTOP);
        }
        else
        {
            switch (LOWORD(wParam))
            {
            case IDM_FILE_NEW:
            case IDM_FILE_OPEN:
            case IDM_FILE_SAVE:
            case IDM_FILE_SAVEAS:
            case IDM_FILE_PRINT:
                MessageBeep(0);
                break;

            case IDM_FILE_EXIT:
                SendMessage(hWnd, WM_CLOSE, 0, 0);
                break;

            case IDM_EDIT_UNDO:
                SendMessage(hwndEdit, WM_UNDO, 0, 0);
                break;
            case IDM_EDIT_CUT:
                SendMessage(hwndEdit, WM_CUT, 0, 0);
                break;
            case IDM_EDIT_COPY:
                SendMessage(hwndEdit, WM_COPY, 0, 0);
                break;
            case IDM_EDIT_PASTE:
                SendMessage(hwndEdit, WM_PASTE, 0, 0);
                break;
            case IDM_EDIT_DELETE:
                SendMessage(hwndEdit, WM_CLEAR, 0, 0);
                break;
            case IDM_EDIT_SELECTALL:
                SendMessage(hwndEdit, EM_SETSEL, 0, -1);
                break;
            case IDM_HELP_HELP:
                MessageBox(hWnd, TEXT("Help not yet implemented!"), szAppName, MB_OK | MB_ICONEXCLAMATION);
                break;
            case IDM_HELP_ABOUT:
                MessageBox(hWnd, TEXT("PopPad(c), 2013"), szAppName, MB_OK | MB_ICONQUESTION);
                break;
            default:
                break;
            }
        }
        break;

    case WM_CLOSE:
        if(IDYES == AskConfirmation(hWnd))
            DestroyWindow(hWnd);
        break;

    case WM_QUERYENDSESSION:
        // windows sends this message to each application when it is shut down. The return value 0 indicates the session will be terminated.
        if(IDYES == AskConfirmation(hWnd))
            return 1;
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
