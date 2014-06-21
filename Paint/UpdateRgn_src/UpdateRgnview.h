/* UpdateRgnview.h ************************************************************
Author:		Paul Watt
Date:		3/2/2002
Purpose:	
******************************************************************************/
#ifndef __UPDATERGN_VIEW_H
#define __UPDATERGN_VIEW_H

extern COLORREF colors[16];

/* Class **********************************************************************
Author:		Paul Watt
Date:		3/2/2002
Purpose:	
******************************************************************************/
class CUpdateRgnView : public CWindowImpl<CUpdateRgnView>
{
private:
	HRGN m_hUpdateRgn;
	int  m_nClrIndex;

	BOOL m_isEraseBkgnd;

	COLORREF GetNextColor();

public:
	DECLARE_WND_CLASS(NULL)

	CUpdateRgnView  ();
	~CUpdateRgnView ();

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CUpdateRgnView)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	BOOL GetEraseBkgnd();
	void SetEraseBkgnd(BOOL isErase);

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

};


#endif 
