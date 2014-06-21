/* UpdateRgnView.cpp **********************************************************
Author:		Paul Watt
Date:		3/2/2002
Purpose:	
******************************************************************************/
#include "stdafx.h"
#include <atlframe.h>
#include <atlctrls.h>

#include "resource.h"

#include "UpdateRgnView.h"
#include "Mainfrm.h"

/* Global Variables **********************************************************/
COLORREF colors[16] =	{
						RGB(0xFF , 0x00, 0x00),
						RGB(0xFF , 0x80, 0x00),
						RGB(0xFF , 0x00, 0x80),
						RGB(0xFF , 0x80, 0x80),
						RGB(0xFF , 0xFF, 0x00),
						RGB(0xFF , 0x00, 0xFF),
						RGB(0xFF , 0xFF, 0x80),
						RGB(0xFF , 0x80, 0xFF),
						RGB(0x00 , 0x80, 0x00),
						RGB(0x00 , 0x00, 0x80),
						RGB(0x00 , 0x80, 0x80),
						RGB(0x00 , 0xFF, 0x00),
						RGB(0x00 , 0x00, 0xFF),
						RGB(0x00 , 0xFF, 0x80),
						RGB(0x00 , 0x80, 0xFF),
						RGB(0x00 , 0xFF, 0xF)
						};

/* Public *********************************************************************
Author:		Paul Watt
Date:		3/17/2002
Purpose:	Constructor
******************************************************************************/
CUpdateRgnView::CUpdateRgnView  ()
{
	m_nClrIndex = 0;

	m_hUpdateRgn = ::CreateRectRgn(0,0,0,0);
	m_isEraseBkgnd = TRUE;
}


/* Public *********************************************************************
Author:		Paul Watt
Date:		3/17/2002
Purpose:	Destructor
******************************************************************************/
CUpdateRgnView::~CUpdateRgnView ()
{
	::DeleteObject(m_hUpdateRgn);
	m_hUpdateRgn = NULL;
}


/* Private ********************************************************************
Author:		Paul Watt
Date:		3/17/2002
Purpose:	Returns the next color that should be painted around the update
			region.
Parameters:	NONE	
Return:		Returns the color.
******************************************************************************/
COLORREF CUpdateRgnView::GetNextColor()
{
	COLORREF clr = colors[m_nClrIndex%16];

	++m_nClrIndex;

	return clr;
}


/* Public *********************************************************************
Author:		Paul Watt
Date:		3/19/2002
Purpose:	Returns whether this view is handling the WM_ERASEBKGND message or not.
Parameters:	NONE
Return:		-
******************************************************************************/
BOOL CUpdateRgnView::GetEraseBkgnd()
{
	return m_isEraseBkgnd;
}


/* Public *********************************************************************
Author:		Paul Watt
Date:		3/19/2002
Purpose:	Sets whether or not the ERASEBKGND message should be handled.
Parameters:	isErase[in]: Indicates that the message should be handler if TRUE.
Return:		-
******************************************************************************/
void CUpdateRgnView::SetEraseBkgnd(BOOL isErase)
{
	m_isEraseBkgnd = isErase;
}


/* Public *********************************************************************
Author:		Paul Watt
Date:		3/19/2002
Purpose:	Message handler for the WM_ERASEBKGND message.  If the m_isEraseBkgnd
			value is set, then the background will be erased, otherwise the 
			background will not be erased.
******************************************************************************/
LRESULT CUpdateRgnView::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& isHandled)
{
			//C: If this view is supposed to handle the WM_ERASEBKGND message, 
			//   then send this message to the Default handler.
	if (m_isEraseBkgnd)
	{
		isHandled = FALSE;
	}
			//C: Exit with false.  If this message is not sent to the default
			//   handler, then the bErase field in the PAINTSTRUCT struct
			//   in WM_PAINT will be true, indicating that the WM_PAINT message
			//   handler should erase the background.
	return FALSE;
}

/* Public *********************************************************************
Author:		Paul Watt
Date:		3/17/2002
Purpose:	Paint handler for this view.  This view will paint the current 
			update region, and it will send the current update data to the log.
******************************************************************************/
LRESULT CUpdateRgnView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
			//C: Get the update region from this window.
	GetUpdateRgn(m_hUpdateRgn);
			//C: Create the paintDC for this window painting session.  The internal
			//   call to BeginPaint in the constructor of this object will validate the
			//   update region.
	CPaintDC dc(m_hWnd);
	
	COLORREF clr = this->GetNextColor();

	CBrush brush;
	brush.CreateSolidBrush(clr);

	dc.FrameRgn(m_hUpdateRgn, brush, 4, 4);
	dc.FrameRgn(m_hUpdateRgn, (HBRUSH)::GetStockObject(WHITE_BRUSH), 1, 1);
			//C: Update the paint struct structure in the parent window.
	CMainFrame *main = dynamic_cast<CMainFrame*>((CMainFrame*)::GetWindowLong(GetParent(), GWL_USERDATA));
	if (NULL != main)
	{
		main->m_ps = dc.m_ps;
		main->UpdateDataBar();
	}
			//C: Exit from this function successfully.
	return 0;
}


