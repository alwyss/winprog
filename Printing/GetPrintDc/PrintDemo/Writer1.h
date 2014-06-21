#pragma once
#include "WriterBase.h"

class Writer1 :
	public WriterBase
{
protected:
	std::wstring GetDocName() const override{ return std::wstring(TEXT("Print1: printing")); }

public:
	Writer1();
	~Writer1();
};

