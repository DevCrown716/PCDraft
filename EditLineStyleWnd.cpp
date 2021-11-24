// EditLineStyleWnd.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "EditLineStyleWnd.h"
#include "EditLineStyleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditLineStyleWnd

CEditLineStyleWnd::CEditLineStyleWnd()
{
	m_nPos=1;
	m_bTrack=false;
	m_bWhite=false;
	m_nLength=1;
}

CEditLineStyleWnd::~CEditLineStyleWnd()
{
}


BEGIN_MESSAGE_MAP(CEditLineStyleWnd, CWnd)
	//{{AFX_MSG_MAP(CEditLineStyleWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditLineStyleWnd message handlers

void CEditLineStyleWnd::SetParentDlg(CEditLineStyleDlg *parentDlg)
{
	m_pParentDlg=parentDlg;
	m_Style=m_pParentDlg->m_EditStyle;
	CalcLength();
}

void CEditLineStyleWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);

	CRgn clipRgn;
	clipRgn.CreateRectRgnIndirect(&rc);
	dc.SelectClipRgn(&clipRgn);

	{	//fill background
		CGDIBackColor bc(&dc,GetSysColor(COLOR_BTNFACE));
		dc.ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
	}

	rc.right=m_nLength*3+1;
	rc.top=rc.bottom/2-3;
	rc.bottom=rc.top+6;

	{
		CGDIBackColor bc(&dc,RGB(0,0,0));
		dc.ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
	}

	rc.top++;
	rc.bottom--;

	{
		CGDIBackColor bc(&dc,RGB(255,255,255));
		for(int i=1;i<=m_Style[0];i++)
		{
			rc.right=rc.left+m_Style[i]*3;
			if(!(i%2)) dc.ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
			rc.left=rc.right;
		}
	}

	{
		GetClientRect(&rc);
		rc.left=m_nPos*3;
		rc.right=rc.left+1;
		if(m_bWhite)
		{
			rc.top+=3;
			rc.bottom=rc.bottom/2-5;
		}
		else
		{
			rc.top=rc.bottom/2+5;
			rc.bottom-=3;
		}
		CGDIBackColor bc(&dc,RGB(0,0,0));
		dc.ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);

		rc.left-=3;
		rc.right+=3;
		if(m_bWhite) rc.bottom=(rc.top+rc.bottom)/2+3;
		else rc.top=(rc.top+rc.bottom)/2-3;
		CGDIStockObject brush(&dc,m_bWhite?WHITE_BRUSH:BLACK_BRUSH);
		dc.RoundRect(rc,CPoint(8,10));
	}
}

void CEditLineStyleWnd::OnLButtonDown(UINT nFlags, CPoint pt) 
{
	m_pParentDlg->m_draw.ShowWindow(SW_SHOW);
	memcpy(m_EditStyle,m_Style,16);
	m_nEditPos=m_nPos;
	m_bTrack=true;
	SetCapture();
}

void CEditLineStyleWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_bTrack=false;

	if(m_Style[0]>14)		// 
	{
		m_Style[0]=14;
		m_Style[15]=0xFF;
		CalcLength();
		Invalidate();
		m_pParentDlg->GetDlgItem(IDC_POS)->Invalidate();
	}
	m_pParentDlg->m_draw.ShowWindow(SW_HIDE);

	m_pParentDlg->Update();
}

void CEditLineStyleWnd::OnMouseMove(UINT nFlags, CPoint pt) 
{
	int pos=pt.x/3;
	if(pos<1) pos=1;
	if(pos>m_nLength && !m_bTrack) pos=m_nLength;

	if(pos!=m_nPos)
	{
		m_nPos=pos;
		if(m_bTrack)
		{
			if(m_nPos==m_nEditPos) memcpy(m_Style,m_EditStyle,16);
			else 
			{
				int i=1;
				pos=min(m_nPos,m_nEditPos);
				memset(m_Style,0,18);
				while(m_EditStyle[i]<pos)		// copy head
				{
					m_Style[i]=m_EditStyle[i];
					pos-=m_EditStyle[i++];
				}
				m_Style[i]=pos;
				int rest=m_EditStyle[i]-pos;
				int j=i;

				if(m_nPos<m_nEditPos)
				{
					pos=m_nEditPos-m_nPos;
					if(pos>rest)
					{
						j++;
						pos-=rest;
						while(m_EditStyle[j]<pos) pos-=m_EditStyle[j++];
						rest=m_EditStyle[j]-pos;
					}
					else rest-=pos;
				}
				else
				{
					if((i%2==1)==m_bWhite) i++;
					SegAdd(m_Style[i],m_nPos-m_nEditPos);
				}
												// copy tail
				if(rest==0) j++;
				if(m_EditStyle[j]!=0xFF)
				{
					if((i%2)!=(j%2)) i++;
					SegAdd(m_Style[i],rest?rest:m_EditStyle[j]);
					while(i<16 && m_EditStyle[++j]!=0xFF)
					{
						m_Style[++i]=m_EditStyle[j];
					}
				}

				m_Style[0]=i++;
				while(i<18) m_Style[i++]=0xFF;	// fill tail
			}
			CalcLength();
			m_pParentDlg->GetDlgItem(IDC_POS)->Invalidate();
			m_pParentDlg->GetDlgItem(IDC_SAMPLE)->Invalidate();
		}
		Invalidate();
	}

	if(!m_bTrack)
	{
		CRect rc;
		GetClientRect(&rc);
		bool white=pt.y<(rc.bottom/2);
		if(white!=m_bWhite)
		{
			m_bWhite=white;
			Invalidate();
		}
		m_pParentDlg->m_white.ShowWindow((m_bWhite)?SW_SHOW:SW_HIDE);
		m_pParentDlg->m_black.ShowWindow((!m_bWhite)?SW_SHOW:SW_HIDE);
	}

	UpdateWindow();
}

UINT CEditLineStyleWnd::OnGetDlgCode() 
{
	return 0;
}

void CEditLineStyleWnd::CalcLength()
{
	m_nLength=0;
	for(int i=1;i<=m_Style[0];i++) m_nLength+=m_Style[i];
}

void CEditLineStyleWnd::SegAdd(unsigned char &seg, int add)
{
	if((seg+add)>254) seg=254;
	else seg+=add;
}

