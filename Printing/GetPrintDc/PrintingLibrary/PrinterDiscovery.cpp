#include "stdafx.h"
#include "PrinterDiscovery.h"
#include <cstdlib>
#include "Winspool.h"


PrinterDiscovery::PrinterDiscovery()
{
}


PrinterDiscovery::~PrinterDiscovery()
{
}


//HDC PrinterDiscovery::GetPrinter()
//{
//	DWORD dwNeeded, dwReturned;
//	HDC hdc;
//	PRINTER_INFO_4 *pinfo4;
//	PRINTER_INFO_5 *pinfo5;
//
//	/*if (GetVersion() & 0x80000000)
//	{
//	EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 5, (PBYTE)pinfo5, dwNeeded, &dwNeeded, &dwReturned);
//	pinfo5 = (PRINTER_INFO_5*)malloc(dwNeeded);
//	EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 5, (PBYTE)pinfo5, dwNeeded, &dwNeeded, &dwReturned);
//	hdc = CreateDC(NULL, pinfo5->pPrinterName, NULL, NULL);
//	free(pinfo5);
//	}
//	else*/
//	{
//		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &dwNeeded, &dwReturned);
//		pinfo4 = (PRINTER_INFO_4*)malloc(dwNeeded);
//		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE)pinfo4, dwNeeded, &dwNeeded, &dwReturned);
//		hdc = CreateDC(NULL, (++++pinfo4)->pPrinterName, NULL, NULL);
//	}
//
//	return hdc;
//}
