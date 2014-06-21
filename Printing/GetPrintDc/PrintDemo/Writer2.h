#pragma once
#include "WriterBase.h"

class Writer2 :
	public WriterBase
{
private:
	BOOL static CALLBACK AbortProc(HDC hdcPrn, int iCode);

	BOOL  CALLBACK Foo(HDC hdcPrn, int iCode);

protected:
	std::wstring GetDocName() const override{ return std::wstring(TEXT("Print2: printing")); }

public:
	Writer2();
	virtual ~Writer2();
};

