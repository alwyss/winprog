// PrintingLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PrintingLibrary.h"
#include <cstdlib>
#include "Winspool.h"


// This is an example of an exported variable
PRINTINGLIBRARY_API int nPrintingLibrary=0;

// This is an example of an exported function.
PRINTINGLIBRARY_API int fnPrintingLibrary(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see PrintingLibrary.h for the class definition
CPrintingLibrary::CPrintingLibrary()
{
	return;
}

HDC CPrintingLibrary::GetPrinterDC()
{
	DWORD dwNeeded, dwReturned;
	HDC hdc;
	PRINTER_INFO_4 *pinfo4;
	PRINTER_INFO_5 *pinfo5;

	/*if (GetVersion() & 0x80000000)
	{
	EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 5, (PBYTE)pinfo5, dwNeeded, &dwNeeded, &dwReturned);
	pinfo5 = (PRINTER_INFO_5*)malloc(dwNeeded);
	EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 5, (PBYTE)pinfo5, dwNeeded, &dwNeeded, &dwReturned);
	hdc = CreateDC(NULL, pinfo5->pPrinterName, NULL, NULL);
	free(pinfo5);
	}
	else*/
	{
		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &dwNeeded, &dwReturned);
		pinfo4 = (PRINTER_INFO_4*)malloc(dwNeeded);
		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE)pinfo4, dwNeeded, &dwNeeded, &dwReturned);
		hdc = CreateDC(NULL, (++++pinfo4)->pPrinterName, NULL, NULL);
	}

	return hdc;
}
