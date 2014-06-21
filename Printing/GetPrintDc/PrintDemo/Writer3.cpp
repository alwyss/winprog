#include "stdafx.h"
#include "Writer3.h"


Writer3::Writer3()
{
	m_AbortProc = Writer3::AbortProc;
}


Writer3::~Writer3()
{
}

BOOL CALLBACK Writer3::CancelPrintingDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool bUserAborted;

	switch (message)
	{
	case WM_INITDIALOG:
		m_cancelPrinting = false;
		SetWindowText(hDlg, __T("Cancel Printing"));
		EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
		break;

	case WM_COMMAND:
		m_cancelPrinting = true;
		EnableWindow(GetParent(hDlg), TRUE);
		DestroyWindow(hDlg);
		if (hDlg == m_hCancelPrintingDialog)
		{
			DebugBreak();
		}

		hDlg = NULL;
		m_hCancelPrintingDialog = NULL;

		break;

	default:
		return FALSE;
	}

	return TRUE;
}

HWND Writer3::ShowCancelDialog(HWND hWnd, HDC hdc)
{
	m_cancelPrinting = false;
	return CreateDialogParam(NULL, TEXT("CancelPrintingDialog"), hWnd, CancelPrintingDlgProc, 0);
}

BOOL CALLBACK Writer3::AbortProc(HDC hdcPrn, int iCode)
{
	MSG msg;
	while (!m_cancelPrinting && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (!m_hCancelPrintingDialog || !IsDialogMessage(m_hCancelPrintingDialog, &msg))
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return !m_cancelPrinting;
}
