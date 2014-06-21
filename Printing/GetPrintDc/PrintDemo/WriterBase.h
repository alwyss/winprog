#pragma once

#include "PrintingLibrary.h"

class WriterBase
{
private:
	CPrintingLibrary *m_PrinterHelper;
	BOOL PrintDocument(HDC hdcPrn, DOCINFO di);
	HDC GetPrinterDC(){ return m_PrinterHelper->GetPrinterDC(); }
	void PageGDICalls(HDC hdcPrn, int xPage, int yPage);

protected:
	static bool m_cancelPrinting;
	static HWND m_hCancelPrintingDialog;
	virtual HWND ShowCancelDialog(HWND hWnd, HDC hdc){ return NULL; }
	BOOL (CALLBACK* m_AbortProc)(HDC, int);
	virtual std::wstring GetDocName() const = 0;

public:
	WriterBase();
	virtual ~WriterBase();
	BOOL Print(HWND hWnd);
};

