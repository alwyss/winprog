/* mainfrm.h ******************************************************************
Author:		Paul Watt
Date:		3/2/2002
Purpose:	
******************************************************************************/
#ifndef __MAINFRM_H
#define __MAINFRM_H

/* Global Variables **********************************************************/
const UINT DATABAR_WIDTH = 150;
const UINT MARGIN_WIDTH  = 5;
const UINT INDENT		 = 5;
const UINT ROW_HEIGHT	 = 20;
const UINT ROW_HEIGHT2	 = 2 * ROW_HEIGHT;
const UINT MARGIN_HEIGHT = 4;

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
{
private:
	UINT  m_nLogMode;
	CFont m_font;
	CFont m_fontBold;

public:
	PAINTSTRUCT m_ps;

	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/20/2002
	Purpose:	Constructor
	******************************************************************************/
	CMainFrame ()
	{
		m_nLogMode = ID_VIEW_NONCLIENT;

		LOGFONT lf;
		lf.lfHeight         = 16;
		lf.lfWidth          = 0;
		lf.lfEscapement     = 0;
		lf.lfOrientation    = 0;
		lf.lfWeight         = FW_NORMAL;
		lf.lfItalic         = FALSE;
		lf.lfUnderline      = FALSE;
		lf.lfStrikeOut      = FALSE;
		lf.lfCharSet        = ANSI_CHARSET;
		lf.lfOutPrecision   = OUT_TT_PRECIS;
		lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
		lf.lfQuality        = DEFAULT_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		_tcscpy(lf.lfFaceName, _T("Arial"));
		
		m_font.CreateFontIndirect(&lf);

		lf.lfWeight         = FW_BOLD;
		m_fontBold.CreateFontIndirect(&lf);

		memset(&m_ps, 0, sizeof(PAINTSTRUCT));
	}

	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CUpdateRgnView m_view;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return m_view.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UISetCheck(ID_VIEW_NOLOG, ID_VIEW_NOLOG == m_nLogMode);
		UISetCheck(ID_VIEW_NONCLIENT, ID_VIEW_NONCLIENT == m_nLogMode);
		UISetCheck(ID_VIEW_DISPLAY_1, ID_VIEW_DISPLAY_1 == m_nLogMode);
		UISetCheck(ID_VIEW_DISPLAY_2, ID_VIEW_DISPLAY_2 == m_nLogMode);

		UISetCheck(ID_VIEW_CSHREDRAW, 0 != (::GetClassLong(m_hWnd, GCL_STYLE) & CS_HREDRAW));
		UISetCheck(ID_VIEW_CSVREDRAW, 0 != (::GetClassLong(m_hWnd, GCL_STYLE) & CS_VREDRAW));

		UISetCheck(ID_VIEW_WMERASEBKGND, m_view.GetEraseBkgnd());

		BOOL isFullDrag;
		SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &isFullDrag, 0);
		UISetCheck(ID_VIEW_SHOWWINDOWCONTENTSWHILEDRAGGING, isFullDrag);

		return FALSE;
	}

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate) 
		MESSAGE_HANDLER(WM_MOVE, OnMove) 
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_RANGE_HANDLER(ID_VIEW_NOLOG, ID_VIEW_DISPLAY_2, OnLog)
		COMMAND_ID_HANDLER(ID_VIEW_CSHREDRAW, OnHRedraw)
		COMMAND_ID_HANDLER(ID_VIEW_CSVREDRAW, OnVRedraw)
		COMMAND_ID_HANDLER(ID_VIEW_WMERASEBKGND, OnErase)
		COMMAND_ID_HANDLER(ID_VIEW_SHOWWINDOWCONTENTSWHILEDRAGGING, OnFullDrag)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNCCalcSize)
		MESSAGE_HANDLER(WM_NCPAINT, OnNCPaint)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy) 
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_NOLOG, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_NONCLIENT, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_DISPLAY_1, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_DISPLAY_2, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_CSHREDRAW, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_CSVREDRAW, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_WMERASEBKGND, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_SHOWWINDOWCONTENTSWHILEDRAGGING, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()
  
	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/20/2002
	Purpose:	Creates the view window, and initializes the menu commands.  A pointer
				to this window is also placed in the USERDATA of the window so
				that other windows like the child view can call member functions
				of this class.
	******************************************************************************/
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE);
			//C: Initialize the styles for vertical and horizontal redraw.
		LONG lStyle = ::GetClassLong(m_hWndClient, GCL_STYLE);
			//C: Strip the horizontal and vertical redraw styles.
		lStyle &= ~(CS_HREDRAW | CS_VREDRAW);
		::SetClassLong(m_hWndClient, GCL_STYLE, lStyle);

			//C: Store a pointer to this object into the user date of the window object.
		SetWindowLong(GWL_USERDATA, (long)this);
			//C: Add the video devices to the menu.  Only display the first two display
			//   devices if there are more than two.
		HMENU menu = GetMenu();
		HMENU subMenu = ::GetSubMenu(menu, 1); 

		UINT nDeviceCount	= 0;
		UINT nIndex			= 0;
		BOOL isSuccess		= TRUE;

		while (isSuccess && nDeviceCount < 2)
		{
			DISPLAY_DEVICE dd;
			dd.cb = sizeof(DISPLAY_DEVICE);
			//C: query for a display device.
			isSuccess = EnumDisplayDevices(NULL, nIndex, &dd, 0);
			if (isSuccess && DISPLAY_DEVICE_ATTACHED_TO_DESKTOP & dd.StateFlags)
			{
				MENUITEMINFO mi;
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask  = MIIM_TYPE;
				mi.fType  = MFT_STRING;
				
				//C: Allocate space for the DeviceName.  This will be stored
				//   as menu item data.
				UINT nLen = _tcslen(dd.DeviceName) + 1;
				TCHAR *szName = new TCHAR[nLen];
				if (szName)
				{
					mi.fMask  |= MIIM_DATA;
					_tcscpy(szName, dd.DeviceName);
					mi.dwItemData = (DWORD)szName;
					mi.dwTypeData = dd.DeviceString; 
				}
				else
				{
					mi.dwTypeData = dd.DeviceName; 
				}

				//C: Update the menu item for device one to correspond to this device.
				::SetMenuItemInfo(subMenu, ID_VIEW_DISPLAY_1 + nDeviceCount, FALSE, &mi); 

				++nDeviceCount;
			}

			++nIndex;
		}
			//C: If there is not a second monitor, then remove this menu item.
		if (nDeviceCount < 2)
		{
			::DeleteMenu(subMenu, ID_VIEW_DISPLAY_2, MF_BYCOMMAND);
		}
		

		CMessageLoop* pLoop = _Module.GetMessageLoop();
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);


		return 0;
	}

	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/20/2002
	Purpose:	Deletes all of the memory that has been allocated and stored in
				the actual window.
	******************************************************************************/
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
			//C: Free menu item data.
		HMENU menu = GetMenu();
		HMENU subMenu = ::GetSubMenu(menu, 1); 

		TCHAR *szName;
		MENUITEMINFO mi;
		mi.cbSize = sizeof(MENUITEMINFO);
		mi.fMask  = MIIM_DATA;
		mi.dwItemData = NULL;
			//C: Get the menu data for the first device.
		::GetMenuItemInfo(subMenu, ID_VIEW_DISPLAY_1, FALSE, &mi);
		szName = (TCHAR*)mi.dwItemData;
		delete[] szName;

			//C: Get the menu data for the first device.
		mi.dwItemData = NULL;
		::GetMenuItemInfo(subMenu, ID_VIEW_DISPLAY_2, FALSE, &mi);
		szName = (TCHAR*)mi.dwItemData;
		delete[] szName;

		return 0;
	}

	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/20/2002
	Purpose:	Whenever this window is moved, the databar will be painted if 
				the bar should be painted on the one of the display monitors.
	******************************************************************************/
	LRESULT OnMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
				//C: If this window is moved the current databar mode is one of the
				//   display items then repaint the display item.
		if (ID_VIEW_DISPLAY_1 == m_nLogMode || ID_VIEW_DISPLAY_2 == m_nLogMode)
		{	
			UpdateDataBar();
		}

		return 0;
	}
	
	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/19/2002
	Purpose:	Exit menu handler.  Lets the user exit the program.
	******************************************************************************/
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/19/2002
	Purpose:	Sets the new log display mode.
	******************************************************************************/
	LRESULT OnLog(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_nLogMode = wID;

			//C: Make sure that the proper dimensions have been set for the client rect.
		SetWindowPos(NULL, 0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

		return 0;
	}


	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/17/2002
	Purpose:	Sets or disables the HRDRAW style for this class instance of the window.
	******************************************************************************/
	LRESULT OnHRedraw(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
			//C: Get the current style of this window class.
		LONG lStyle = ::GetClassLong(m_hWnd, GCL_STYLE);
		if (CS_HREDRAW & lStyle)
		{
			//C: If the HREDRAW style is set, then remove it.			
			lStyle &= ~CS_HREDRAW;
		}
		else
		{
			//C: If the HREDRAW style is not set, then set it.
			lStyle |= CS_HREDRAW;
		}

		::SetClassLong(m_hWnd, GCL_STYLE, lStyle);

		return 0;
	}

	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/17/2002
	Purpose:	Sets or disables the VRDRAW style for this class instance of the window.
	******************************************************************************/
	LRESULT OnVRedraw(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
			//C: Get the current style of this window class.
		LONG lStyle = ::GetClassLong(m_hWnd, GCL_STYLE);
		if (CS_VREDRAW & lStyle)
		{
			//C: If the VREDRAW style is set, then remove it.			
			lStyle &= ~CS_VREDRAW;
		}
		else
		{
			//C: If the VREDRAW style is not set, then set it.
			lStyle |= CS_VREDRAW;
		}

		::SetClassLong(m_hWnd, GCL_STYLE, lStyle);

		return 0;
	}

	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/19/2002
	Purpose:	Handles when the user clicks on the WM_ERASEBKGND menu item.
	******************************************************************************/
	LRESULT OnErase(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL isErase = m_view.GetEraseBkgnd();

		m_view.SetEraseBkgnd(!isErase);

		return 0;
	}

	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/19/2002
	Purpose:	Handles when the user clicks on the WM_ERASEBKGND menu item.
	******************************************************************************/
	LRESULT OnFullDrag(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL isFullDrag;
		SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &isFullDrag, 0);
		isFullDrag = !isFullDrag;
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, isFullDrag, NULL, 0);

		return 0;
	}


	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/19/2002
	Purpose:	Adjusts the client rect of the window to allow for the log to be 
				painted in the non-client area if m_nLogMode == ID_VIEW_NONCLIENT.
				Otherwise the default handler is called.
	******************************************************************************/
	LRESULT OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (ID_VIEW_NONCLIENT == m_nLogMode)
		{
			//C: Pass this message to the DefWindowProc in order to get the systems
			//   default client rect for this rectangle.
			::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

			//C: Is the structure in the lParam a NCCALCSIZE_PARAMS or a RECT.
			if (wParam)
			{
				NCCALCSIZE_PARAMS *pParams = (NCCALCSIZE_PARAMS*)lParam;
			//C: Adjust the new client rect to give the non-client border DATABAR_WDITH pixels.
				pParams->rgrc->left += DATABAR_WIDTH;
			}
			else
			{
				RECT *pRect = (RECT*)lParam;
			//C: Adjust the new client rect to give the non-client border DATABAR_WIDTH pixels.
				pRect->left += DATABAR_WIDTH;
			}

			return 0;
		}
		else
		{
			bHandled = FALSE;
		}
		
		return 0;
	}


	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/19/2002
	Purpose:	Paints the extra info on the nonclient region if the current mode
				is ID_VIEW_NONCLIENT.  Otherwise the message will be passed to the
				default handler.
	Parameters:			
	Return:		
	******************************************************************************/
	LRESULT OnNCPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (ID_VIEW_NONCLIENT == m_nLogMode)
		{
			RECT rWindow;
			RECT rClient;
			GetWindowRect(&rWindow);
			GetClientRect(&rClient);

			POINT pt = {0,0};
			::MapWindowPoints(m_hWnd, NULL, &pt, 1);

			::OffsetRect(&rClient, pt.x, pt.y);
			
			//C: Create a HDC for the non-client area that only encapsulates the left
			//   DATABAR_WIDTH pixels from top to bottom.
			RECT rClip;
			rClip.left		= rWindow.left + ::GetSystemMetrics(SM_CXSIZEFRAME);
			rClip.right		= rClip.left + DATABAR_WIDTH;
			rClip.top		= rClient.top;
			rClip.bottom	= rClient.bottom;

			HRGN hClipRgn = ::CreateRectRgnIndirect(&rClip);
			//C: if the operating system is not NT, then translate the region
			//   back into window coordinates.
			if (!g_isNT)
			{
				::OffsetRgn(hClipRgn, -pt.x, -pt.y);
			}

			HDC hdc = GetDCEx(hClipRgn, DCX_INTERSECTRGN | DCX_WINDOW | DCX_CACHE);
			//C: Paint the databar.
			::OffsetRect(&rClip, -rWindow.left, -rWindow.top);
			PaintDataBar(hdc, rClip);
			//C: Release resources.
			ReleaseDC(hdc);
		}
			//C: Even if the extra info has been painted, the borders and other 
			//   portions of the window need to be painted.  Send the message to
			//   the default handler.
		bHandled = FALSE;

		return 0;
	}

	/* Public *********************************************************************
	Author:		Paul Watt
	Date:		3/20/2002
	Purpose:	Forces the data bar to be drawn.	
	******************************************************************************/
	void UpdateDataBar()
	{
		if (ID_VIEW_NONCLIENT == m_nLogMode)
		{
			SendMessage(WM_NCPAINT, 1, 0);
		}
		else if(ID_VIEW_DISPLAY_1 == m_nLogMode || ID_VIEW_DISPLAY_2 == m_nLogMode)
		{
			HMENU menu = GetMenu();
			HMENU subMenu = ::GetSubMenu(menu, 1); 

			TCHAR *szName;
			MENUITEMINFO mi;
			mi.cbSize = sizeof(MENUITEMINFO);
			mi.fMask  = MIIM_DATA;
			mi.dwItemData = NULL;
			//C: Get the name of the device to display the databar on.
			::GetMenuItemInfo(subMenu, ID_VIEW_DISPLAY_1, FALSE, &mi);
			szName = (TCHAR*)mi.dwItemData;
			//C: Use CreateDC to create the DC for this device.
			CDC dc;
			dc.CreateDC(NULL, szName, NULL, NULL);
			//C: Create a clipping rectangle.
			RECT rClip;
			rClip.left		= 0;
			rClip.right		= DATABAR_WIDTH;
			rClip.top		= 0;
			rClip.bottom	= (2 * MARGIN_HEIGHT) + (13 * ROW_HEIGHT);
			//C: Clip the window rect for this window from the screen display.
			RECT rWindow;
			GetWindowRect(&rWindow);
			dc.ExcludeClipRect(&rWindow);
			//C: Call the Paint routine for the databar.
			PaintDataBar (dc, rClip);
		}
	}

private:
	/* Private ********************************************************************
	Author:		Paul Watt
	Date:		3/20/2002
	Purpose:	This function will paint the databar information on the DC that
				it has been given.
	Parameters:	hdc[in]: A handle to a DC that has been prepared for the databar.
	Return:		-
	******************************************************************************/
	void PaintDataBar (HDC hdc, RECT &rClip)
	{
				//C: Get the colors that will be used to paint the display.
		COLORREF clrBkgnd = ::GetSysColor(COLOR_INACTIVECAPTION);
		COLORREF clrText  = ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);
				//C: Paint the background.
		CBrush br;
		br.CreateSolidBrush(clrBkgnd);
		FillRect(hdc, &rClip, br);

		FrameRect(hdc, &rClip, (HBRUSH)::GetStockObject(BLACK_BRUSH));
				//C: Initialize the DC to paint the text.
		int nContext = ::SaveDC(hdc);
		::SelectObject(hdc, m_fontBold);
		::SetTextColor(hdc, clrText);
		::SetBkMode(hdc, TRANSPARENT);

				//C: Draw all of the field names.
		RECT rDraw;
		rDraw.left	= rClip.left + MARGIN_WIDTH;
		rDraw.right = rClip.left + DATABAR_WIDTH - (2 * MARGIN_WIDTH);
		rDraw.top   = rClip.top + MARGIN_HEIGHT;
		rDraw.bottom = rDraw.top + ROW_HEIGHT;
		DrawText(hdc, _T("PAINTSTRUCT:"), -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

		rDraw.left = rClip.left + MARGIN_WIDTH + INDENT;
		rDraw.top	+= ROW_HEIGHT;
		rDraw.bottom+= ROW_HEIGHT;
		DrawText(hdc, _T("HDC:"), -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

		rDraw.top	+= ROW_HEIGHT2;
		rDraw.bottom+= ROW_HEIGHT2;
		DrawText(hdc, _T("fErase:"), -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

		rDraw.top	+= ROW_HEIGHT2;
		rDraw.bottom+= ROW_HEIGHT2;
		DrawText(hdc, _T("rcPaint.left:"), -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

		rDraw.top	+= ROW_HEIGHT2;
		rDraw.bottom+= ROW_HEIGHT2;
		DrawText(hdc, _T("rcPaint.top:"), -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

		rDraw.top	+= ROW_HEIGHT2;
		rDraw.bottom+= ROW_HEIGHT2;
		DrawText(hdc, _T("rcPaint.right:"), -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

		rDraw.top	+= ROW_HEIGHT2;
		rDraw.bottom+= ROW_HEIGHT2;
		DrawText(hdc, _T("rcPaint.bottom:"), -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

					//C: Draw all of the data.
		::SelectObject(hdc, m_font);
		rDraw.left	= rClip.left + MARGIN_WIDTH + (5 * INDENT);
		rDraw.right = rClip.left + DATABAR_WIDTH - (2 * MARGIN_WIDTH);
		rDraw.top   = rClip.top + MARGIN_HEIGHT + ROW_HEIGHT2;
		rDraw.bottom = rDraw.top + ROW_HEIGHT;

		TCHAR szValue[32];
		wsprintf(szValue, _T("0x%.8x"), m_ps.hdc);
		DrawText(hdc, szValue, -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

		if (m_ps.fErase)
		{
			_tcscpy(szValue, _T("TRUE"));
		}
		else
		{
			_tcscpy(szValue, _T("FALSE"));
		}
		rDraw.top	+= ROW_HEIGHT2;
		rDraw.bottom+= ROW_HEIGHT2;
		DrawText(hdc, szValue, -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

		wsprintf(szValue, _T("%d"), m_ps.rcPaint.left);
		rDraw.top	+= ROW_HEIGHT2;
		rDraw.bottom+= ROW_HEIGHT2;
		DrawText(hdc, szValue, -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

		wsprintf(szValue, _T("%d"), m_ps.rcPaint.top);
		rDraw.top	+= ROW_HEIGHT2;
		rDraw.bottom+= ROW_HEIGHT2;
		DrawText(hdc, szValue, -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

		wsprintf(szValue, _T("%d"), m_ps.rcPaint.right);
		rDraw.top	+= ROW_HEIGHT2;
		rDraw.bottom+= ROW_HEIGHT2;
		DrawText(hdc, szValue, -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

		wsprintf(szValue, _T("%d"), m_ps.rcPaint.bottom);
		rDraw.top	+= ROW_HEIGHT2;
		rDraw.bottom+= ROW_HEIGHT2;
		DrawText(hdc, szValue, -1, &rDraw, DT_LEFT | DT_TOP | DT_SINGLELINE);

		::RestoreDC(hdc, nContext);
	}
};




#endif
