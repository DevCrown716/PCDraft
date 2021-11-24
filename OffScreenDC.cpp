#include "stdafx.h"
#include "offscreendc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CBitmap COffScreenDC::bmOffScreen;

COffScreenDC::COffScreenDC(CDC* pDC) : m_pDC(pDC), m_pOldBitmap(NULL)
{
	ASSERT(pDC);

	m_rcClipBox.SetRectEmpty();
	pDC->GetClipBox(&m_rcClipBox);
	m_bAttached=true;
	if(!m_rcClipBox.IsRectEmpty() && CreateCompatibleDC(pDC))
	{
/*		if(bmOffScreen.GetSafeHandle())
		{
			BITMAP bm;
			bmOffScreen.GetBitmap(&bm);
			if(m_rcClipBox.right!=bm.bmWidth || m_rcClipBox.bottom!=bm.bmHeight) bmOffScreen.DeleteObject();
		}
		if(!bmOffScreen.GetSafeHandle())*/ bmOffScreen.CreateCompatibleBitmap(pDC,m_rcClipBox.right,m_rcClipBox.bottom);
		if(bmOffScreen.GetSafeHandle())
		{
			VERIFY(m_pOldBitmap=SelectObject(&bmOffScreen));
			HRGN hRgn=::CreateRectRgnIndirect(&m_rcClipBox);
			::GetClipRgn(pDC->GetSafeHdc(),hRgn);
			CRgn clip;
			clip.Attach(hRgn);
			CDC::SelectClipRgn(&clip);
			m_bAttached=false;
		}
	}
	if(m_bAttached) Attach(pDC->GetSafeHdc());
}

COffScreenDC::~COffScreenDC()
{
	Flush();
	if(m_bAttached) Detach();
}
/*
int	 COffScreenDC::GetClipBox(LPRECT lpRect)
{
	if(!m_rcClipBox.IsRectEmpty())
	{
		::CopyRect(lpRect,&m_rcClipBox);
		::DPtoLP(m_hDC,(LPPOINT)lpRect,2);
		return SIMPLEREGION;
	}
	return m_pDC->GetClipBox(lpRect);
}
*/

BOOL COffScreenDC::Flush()
{
	if(bmOffScreen.GetSafeHandle())
	{
		CDC::SetMapMode(MM_TEXT);
		CDC::SetWindowOrg(0,0);
		CDC::SetViewportOrg(0,0);

		m_pDC->BitBlt(m_rcClipBox.left,m_rcClipBox.top,m_rcClipBox.right,m_rcClipBox.bottom,this,m_rcClipBox.left,m_rcClipBox.top,SRCCOPY);
		if(m_pOldBitmap)
			SelectObject(m_pOldBitmap);
		bmOffScreen.DeleteObject();
		return TRUE;
	}
	return FALSE;
}

