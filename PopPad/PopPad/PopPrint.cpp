#include "PopPrint.h"


PopPrint::PopPrint()
{
}


PopPrint::~PopPrint()
{
}


BOOL CALLBACK PopPrint::PrintDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
		break;

	case WM_COMMAND:
		m_bUserAborted = TRUE;
		EnableWindow(GetParent(hDlg), TRUE);
		DestroyWindow(hDlg);
		m_hDlgPrint = NULL;
		break;

	default:
		return FALSE;
	}

	return TRUE;
}