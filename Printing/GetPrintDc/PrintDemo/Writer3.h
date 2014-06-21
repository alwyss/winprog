#pragma once
#include "WriterBase.h"
class Writer3 :
	public WriterBase
{
private:
	static BOOL CALLBACK CancelPrintingDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK Writer3::AbortProc(HDC hdcPrn, int iCode);

protected:
	std::wstring GetDocName() const override{ return std::wstring(TEXT("Print3: printing")); }

public:
	Writer3();
	virtual ~Writer3();
	HWND ShowCancelDialog(HWND hWnd, HDC hdc) override;
};

