// LineStyleWnd.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "LineStyleWnd.h"

#include "PCDraftDoc.h"
#include "DrawPrim.h"
#include "DraftContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLineStyleWnd

CLineStyleWnd::CLineStyleWnd():m_pDoc(NULL)
{
}

CLineStyleWnd::~CLineStyleWnd()
{
}


BEGIN_MESSAGE_MAP(CLineStyleWnd, CWnd)
	//{{AFX_MSG_MAP(CLineStyleWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLineStyleWnd message handlers
BOOL CLineStyleWnd::Create(CRect& rc,CWnd *pParent,UINT nID)
{
	return CWnd::Create(AfxRegisterWndClass(0,0,(HBRUSH)GetStockObject(WHITE_BRUSH)),"",WS_CHILD|WS_VISIBLE|WS_BORDER,rc,pParent,nID,NULL);
}

void CLineStyleWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);

	if(m_pDoc && IsWindowEnabled())
	{
		CPen pen;
		CBrush penBrush;
		bool bUseBrush=false;
		int nWidth=m_Pen.m_nWidth & 0x1F;
		COLORREF crText=RGB(0,0,0);
		if(m_Pen.m_nPenType==kPenInvis || (m_Pen.m_nPenType<kPenPatEmbed && m_Pen.m_nIndex<0)) pen.CreateStockObject(BLACK_PEN);
		else
		{
			switch(m_Pen.m_nPenType)
			{
			case kPenColor:
//				crText=m_Pen.m_crColor;
				if(m_Pen.Dashing())
				{
					bUseBrush = true;
					penBrush.CreateSolidBrush(crText);
				}
				if(m_Pen.Dashing()) pen.CreateStockObject(NULL_PEN);
				else pen.CreatePen(PS_SOLID,nWidth,crText);
				break;
			case kPenPat:
				{
					CBitmap bmp;
					bmp.Attach(m_pDoc->GetPatternBitmap(m_Pen.m_nIndex,m_Pen.m_iPenOpacity)); // Gradient/Opacity - Infinisys Ltd
					if(m_Pen.Dashing())
					{
						bUseBrush = true;
						penBrush.CreatePatternBrush(&bmp);
					}
					else
					{
						LOGBRUSH lb;
						lb.lbStyle=BS_PATTERN;
						lb.lbColor=0;
						lb.lbHatch=(LONG)bmp.GetSafeHandle();
						pen.CreatePen(PS_GEOMETRIC,nWidth,&lb);
					}
				}
				break;
			case kPenPatEmbed:
				{
					CBitmap bmp;
					DIBSECTION ds;
					GetObject(m_Pen.m_hDib,sizeof(DIBSECTION),&ds);
					if(ds.dsBmih.biBitCount==1) ds.dsBitfields[1]=0xFFFFFF;		// tweak
					HBITMAP hBmp=CreateDIBitmap(dc.GetSafeHdc(),&ds.dsBmih,CBM_INIT,ds.dsBm.bmBits,(BITMAPINFO*)&ds.dsBmih,DIB_RGB_COLORS);
					bmp.Attach(hBmp);
					if(m_Pen.Dashing())
					{
						bUseBrush = true;
						penBrush.CreatePatternBrush(&bmp);
					}
					else
					{
						LOGBRUSH lb;
						lb.lbStyle=BS_PATTERN;
						lb.lbColor=0;
						lb.lbHatch=(LONG)bmp.GetSafeHandle();
						pen.CreatePen(PS_GEOMETRIC,nWidth,&lb);
					}
				}
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}
		if(bUseBrush && !penBrush.GetSafeHandle()) penBrush.CreateStockObject(BLACK_BRUSH);
		if(!pen.GetSafeHandle()) pen.CreateStockObject(BLACK_PEN);

		CPalette* pPalette=dc.SelectPalette(&m_pDoc->m_Palette,FALSE);
		dc.RealizePalette();

		CDrawPrim dp;
		dp.setDashTable((const CDrawPrim::DashTable*)m_pDoc->m_DashStyles);

		CDraftContext pdc(&dc,&m_Mapper,m_pDoc);
      LongRect lr = m_Mapper.DPtoLP(rc);
      UseRect userect(&pdc, lr);
		pdc.SetPen(m_Pen);

		//CGDIObject<CPen> gdiPen(&dc,&pen);
		//CGDIObject<CBrush> gdiBrush(&dc,&penBrush);
		dc.MoveTo(rc.left,(rc.top+rc.bottom)/2);

		m_Mapper.m_bZoomLineWeights = FALSE;
      pdc.path_new(false);
#undef new
      pdc.m_PenPlus.reset(new Gdiplus::Pen(Gdiplus::Color::Black, 1.0f));
		dp.dashLine(&pdc,pointd(rc.left,(rc.top+rc.bottom)/2), pointd(rc.right,(rc.top+rc.bottom)/2),true);

		{
			CString text;
			if(m_Pen.m_nWidth & HairlineMask)
			{
				switch(m_Pen.HairlineCnt())
				{
				case 0:
					text="H";
					break;
				case 1:
					text=".5";
					break;
				case 2:
					text=".75";
					break;
				}
			}
			else text.Format("%d",m_Pen.m_nWidth);
			text=((m_Pen.m_nPenType==kPenInvis || (m_Pen.m_nPenType<kPenPatEmbed && m_Pen.m_nIndex<0))?" N":" ")+text+" ";
			CGDIStockObject font(&dc,DEFAULT_GUI_FONT);
			CGDITextColor tc(&dc,crText);
//			CGDITextAlign ta(&dc,TA_TOP|TA_CENTER);
//			dc.ExtTextOut((rc.left+rc.right)>>1,(rc.top+rc.bottom-14)>>1,ETO_CLIPPED,&rc,text,NULL);
			dc.DrawText(text, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
		
		dc.SelectPalette(pPalette,FALSE);
	}
	else
	{
		CGDIBackColor bk(&dc,GetSysColor(COLOR_BTNFACE));
		dc.ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
	}
}

void CLineStyleWnd::SetDoc(CPCDraftDoc* pDoc)
{
	m_pDoc=pDoc;
	m_Pen=m_pDoc->m_Pen;
	m_Mapper.SetDocument(pDoc);
}

void CLineStyleWnd::SetPen(const PenModel &pm)
{
	if(m_Pen.m_nWidth!=pm.m_nWidth || m_Pen.m_nPenType!=pm.m_nPenType)
	{
		m_Pen=pm;
		Invalidate();
	}
}

void CLineStyleWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GetParent()->SendMessage(WM_COMMAND,MAKEWPARAM(ID_LINEINFO,0),(LPARAM)GetSafeHwnd());
}
