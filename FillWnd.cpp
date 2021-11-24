// FillWnd.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "FillWnd.h"

#include "PCDraftDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFillWnd

CFillWnd::CFillWnd():m_pDoc(NULL)
{
}

CFillWnd::~CFillWnd()
{
}


BEGIN_MESSAGE_MAP(CFillWnd, CWnd)
	//{{AFX_MSG_MAP(CFillWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFillWnd message handlers

BOOL CFillWnd::Create(CRect& rc,CWnd *pParent,UINT nID)
{
	return CWnd::Create(AfxRegisterWndClass(0,0,(HBRUSH)GetStockObject(WHITE_BRUSH)),"",WS_CHILD|WS_VISIBLE|WS_BORDER,rc,pParent,nID,NULL);
}

void CFillWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);

	if(m_pDoc && IsWindowEnabled())
	{
		if(m_Fill.m_nFillType==kNoFill)
		{
			CGDIStockObject font(&dc,DEFAULT_GUI_FONT);
			CGDITextAlign align(&dc,TA_CENTER|TA_TOP);
			dc.ExtTextOut((rc.left+rc.right)>>1,(rc.top+rc.bottom-14)>>1,ETO_OPAQUE|ETO_CLIPPED,&rc,"N",1,NULL);
		}
		else
		{
			CPalette* pPalette=dc.SelectPalette(&m_pDoc->m_Palette,FALSE);
			dc.RealizePalette();
			CBitmap bmp;
			CBrush brush;
			switch(m_Fill.m_nFillType)
			{
			case kFillColor:
				brush.CreateSolidBrush(PALETTEINDEX(m_Fill.m_nIndex));
				break;
			case kFillPat:
            m_pDoc->FillModelBitmap(m_Fill.m_nIndex,dc,rc, m_Fill.m_iFillOpacity);// Opacity/Transparency - Infinisys Ltd
				//m_pDoc->GetPatternBitmap(m_Fill.m_nIndex,&bmp);
				//brush.CreatePatternBrush(&bmp);
				break;
			default:
				ASSERT(FALSE);
				break;
			}
         if(m_Fill.m_nFillType != kFillPat)// Opacity/Transparency - Infinisys Ltd
         {
            dc.FillRect(&rc,&brush);
         }
			dc.SelectPalette(pPalette,FALSE);
		}
	}
	else
	{
		CGDIBackColor bk(&dc,GetSysColor(COLOR_BTNFACE));
		dc.ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
	}
}

void CFillWnd::SetDoc(CPCDraftDoc* pDoc)
{
	m_pDoc=pDoc;
	m_Fill=m_pDoc->m_Fill;
}

void CFillWnd::SetFill(const FillModel &fm)
{
	m_Fill=fm;
	Invalidate();
}

void CFillWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GetParent()->SendMessage(WM_COMMAND,MAKEWPARAM(ID_FILLINFO,0),(LPARAM)GetSafeHwnd());
}
