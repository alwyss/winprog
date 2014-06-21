// UpdateRgn.cpp : main source file for UpdateRgn.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>

#include "resource.h"
 
#include "UpdateRgnView.h"
#include "MainFrm.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

#if (_WIN32_IE >= 0x0300)
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(iccx);
	iccx.dwICC = ICC_BAR_CLASSES;	// change to support other controls
	BOOL bRet = ::InitCommonControlsEx(&iccx);
	bRet;
	ATLASSERT(bRet);
#else
	::InitCommonControls();
#endif

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

			//C: Determine which flavor of the windows kernel that the user has.
	OSVERSIONINFO info;
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	::GetVersionEx(&info);
	if (VER_PLATFORM_WIN32_NT == info.dwPlatformId)
	{
		g_isNT = true;
	}

			//C: Get the current FullDrag setting.
	BOOL isFullDrag;
	SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &isFullDrag, 0);

	int nRet = Run(lpstrCmdLine, nCmdShow);
			//C: Restore the Full drag flag.
	SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, isFullDrag, NULL, 0);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
