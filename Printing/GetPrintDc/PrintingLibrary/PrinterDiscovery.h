#pragma once
#ifdef PRINTINGLIBRARY_EXPORTS
#define PRINTINGLIBRARY_API __declspec(dllexport)
#else
#define PRINTINGLIBRARY_API __declspec(dllimport)
#endif
class PRINTINGLIBRARY_API PrinterDiscovery
{
public:
	PrinterDiscovery();
	~PrinterDiscovery();
	//HDC GetPrinter();
};

