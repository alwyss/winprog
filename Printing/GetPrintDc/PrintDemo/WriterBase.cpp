#include "stdafx.h"
#include "WriterBase.h"


WriterBase::WriterBase()
{
	m_PrinterHelper = new CPrintingLibrary;
}


WriterBase::~WriterBase()
{
	delete m_PrinterHelper;
}


bool WriterBase:: m_cancelPrinting = false;
HWND WriterBase:: m_hCancelPrintingDialog;

BOOL WriterBase::PrintDocument(HDC hdcPrn, DOCINFO di)
{
	int xPage, yPage;

	xPage = GetDeviceCaps(hdcPrn, HORZRES);
	yPage = GetDeviceCaps(hdcPrn, VERTRES);

	SetAbortProc(hdcPrn, m_AbortProc);
	if (StartDoc(hdcPrn, &di) <= 0)
	{
		return FALSE;
	}

	if (StartPage(hdcPrn) <= 0)
	{
		return FALSE;
	}

	PageGDICalls(hdcPrn, xPage, yPage);

	if (EndPage(hdcPrn) <= 0)
	{
		return FALSE;
	}

	if (EndDoc(hdcPrn) <= 0)
	{
		return FALSE;
	}

	return TRUE;
}



BOOL WriterBase::Print(HWND hWnd)
{
	int xPage, yPage;
	HDC hdcPrn;

	static DOCINFO di = { sizeof(DOCINFO), GetDocName().c_str() };
	if (NULL == (hdcPrn = GetPrinterDC()))
	{
		return FALSE;
	}

	EnableWindow(hWnd, FALSE);

	auto bUserAborted = false;
	m_hCancelPrintingDialog = ShowCancelDialog(hWnd, hdcPrn);

	auto result = PrintDocument(hdcPrn, di);

	if (!bUserAborted)
	{
		EnableWindow(hWnd, TRUE);
		DestroyWindow(m_hCancelPrintingDialog);
	}

	EnableWindow(hWnd, TRUE);
	DeleteDC(hdcPrn);
	return result;
}


void WriterBase::PageGDICalls(HDC hdcPrn, int xPage, int yPage)
{
	static TCHAR szTextStr[] = TEXT("Hello, Printer!");
	Rectangle(hdcPrn, 0, 0, xPage, yPage);
	MoveToEx(hdcPrn, 0, 0, nullptr);
	LineTo(hdcPrn, xPage, yPage);
	MoveToEx(hdcPrn, xPage, 0, nullptr);
	LineTo(hdcPrn, 0, yPage);

	SaveDC(hdcPrn);
	//SetWindowOrgEx(hdcPrn, -xPage / 4, -yPage / 4, nullptr);
	//SetViewportOrgEx(hdcPrn, xPage / 2, yPage / 2, nullptr);
	//SetWindowOrgEx(hdcPrn, -xPage / 4, -yPage / 4, nullptr);
	///*POINT point;
	//point.x = xPage;
	//point.y = yPage;
	//SetMapMode(hdcPrn, MM_LOENGLISH);
	//DPtoLP(hdcPrn, &point, 1);

	//TextOut(hdcPrn, 0, 0, szTextStr, lstrlen(szTextStr));*/

	Sleep(3000);

	SetMapMode(hdcPrn, MM_ISOTROPIC);
	SetWindowExtEx(hdcPrn, 1000, 1000, nullptr);
	SetViewportExtEx(hdcPrn, xPage / 2, -yPage / 2, nullptr);
	SetViewportOrgEx(hdcPrn, xPage / 2, yPage / 2, nullptr);
	////SetWindowOrgEx(hdcPrn, xPage / 4, yPage / 4, nullptr);

	Ellipse(hdcPrn, -500, 500, 500, -500);

	SetTextAlign(hdcPrn, TA_BASELINE | TA_CENTER);
	TextOut(hdcPrn, 0, 0, szTextStr, lstrlen(szTextStr));
	RestoreDC(hdcPrn, -1);
}
