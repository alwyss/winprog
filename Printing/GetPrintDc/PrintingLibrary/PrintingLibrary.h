#pragma once
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PRINTINGLIBRARY_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PRINTINGLIBRARY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PRINTINGLIBRARY_EXPORTS
#define PRINTINGLIBRARY_API __declspec(dllexport)
#else
#define PRINTINGLIBRARY_API __declspec(dllimport)
#endif

// This class is exported from the PrintingLibrary.dll
class PRINTINGLIBRARY_API CPrintingLibrary {
public:
	CPrintingLibrary(void);
	// TODO: add your methods here.
	HDC GetPrinterDC();
};

extern PRINTINGLIBRARY_API int nPrintingLibrary;

PRINTINGLIBRARY_API int fnPrintingLibrary(void);
