#include "stdafx.h"
#include "Writer2.h"


Writer2::Writer2()
{
	m_AbortProc = &Writer2::AbortProc;

	BOOL(CALLBACK Writer2::* foo)(HDC, int);
	static BOOL(CALLBACK*foo2)(HDC, int);
	foo = &Writer2::Foo;
	foo2 = AbortProc;
	(*foo2)(NULL, 0);
	foo2(NULL, 0);
	(this->*foo)(NULL, 0);
	((*this).*foo)(NULL, 0);
}


Writer2::~Writer2()
{
}

BOOL CALLBACK Writer2::Foo(HDC hdcPrn, int iCode)
{
	return TRUE;
}

BOOL CALLBACK Writer2::AbortProc(HDC hdcPrn, int iCode)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return TRUE;
}

