// SlideShowView.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "SlideShowView.h"

#include "PCDraftDoc.h"
#include "Layer.h"
#include "FactorMapper.h"
#include "DraftContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlideShowView

IMPLEMENT_DYNCREATE(CSlideShowView, CView)

CSlideShowView::CSlideShowView()
{
	m_nTimerID=0;
	m_nSlide=0;
	m_nSeconds=0;
	m_bLayer1AsMaster=false;
}

CSlideShowView::~CSlideShowView()
{
}


BEGIN_MESSAGE_MAP(CSlideShowView, CView)
	//{{AFX_MSG_MAP(CSlideShowView)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_CHAR()
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlideShowView drawing

void CSlideShowView::OnDraw(CDC* pDC)
{
	CPCDraftDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect rc;
	GetClientRect(&rc);
	double factor=pDoc->GetFitFactor(rc.Size());
	if(factor>1) factor=1;
	CFactorMapper mapper(factor);
	CDraftContext dc(pDC,&mapper,pDoc);

	CPalette* pPalette=pDC->SelectPalette(&pDoc->m_Palette,FALSE);
	pDC->RealizePalette();

	if(m_bLayer1AsMaster) pDoc->GetLayer(0)->Draw(&dc,true);
	pDoc->GetLayer(m_nSlide)->Draw(&dc,true);
	pDC->SelectPalette(pPalette,FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CSlideShowView diagnostics

#ifdef _DEBUG
void CSlideShowView::AssertValid() const
{
	CView::AssertValid();
}

void CSlideShowView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPCDraftDoc* CSlideShowView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPCDraftDoc)));
	return (CPCDraftDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSlideShowView message handlers

#define kSlideTimer	100

void CSlideShowView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	CPCDraftDoc* pDoc=GetDocument();
	m_nSeconds=pDoc->m_nSlideSeconds;
	m_bLayer1AsMaster=pDoc->m_bLayer1AsMaster;
	m_nSlide=m_bLayer1AsMaster?1:0;

	if(m_nSeconds) m_nTimerID=SetTimer(kSlideTimer,m_nSeconds*1000,NULL);
}	

void CSlideShowView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_nSeconds==0) NextSlide(true);
}

void CSlideShowView::OnTimer(UINT_PTR nIDEvent) 
{
	if(nIDEvent==kSlideTimer)
	{
		if(m_nSlide<GetDocument()->GetLayersCount()-1) m_nSlide++;
		else m_nSlide=m_bLayer1AsMaster?1:0;
		Invalidate();
	}
	else CView::OnTimer(nIDEvent);
}


void CSlideShowView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar==VK_ESCAPE || nChar==VK_CANCEL) GetParent()->DestroyWindow();
	else CView::OnChar(nChar, nRepCnt, nFlags);
}

void CSlideShowView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	GetParent()->DestroyWindow();
}

void CSlideShowView::OnDestroy() 
{
	CView::OnDestroy();

	if(m_nTimerID) KillTimer(m_nTimerID);
}

void CSlideShowView::NextSlide(bool bForward)
{
	if(bForward)
	{
		if(m_nSlide<GetDocument()->GetLayersCount()-1)
		{
			m_nSlide++;
			Invalidate();
		}
		else GetParent()->DestroyWindow();
	}
	else
	{
		if(m_nSlide>(m_bLayer1AsMaster?1:0))
		{
			m_nSlide--;
			Invalidate();
		}
		else GetParent()->DestroyWindow();
	}
}

void CSlideShowView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
/*	if(nChar==VK_CANCEL || (nChar==VK_PAUSE && GetAsyncKeyState(VK_CONTROL)<0)) GetParent()->DestroyWindow();
	else
	{
		if(m_nSeconds==0)
		{
			switch(nChar)
			{
			case VK_RIGHT:
				NextSlide(true);
				break;
			case VK_LEFT:
				NextSlide(false);
				break;
			}
		}
	}*/
}


BOOL CSlideShowView::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message>=WM_KEYFIRST && pMsg->message<=WM_KEYLAST)
	{
		if(pMsg->message==WM_KEYDOWN)
		{
			UINT nChar=pMsg->wParam;
			if(nChar==VK_ESCAPE || nChar==VK_CANCEL || (nChar==VK_PAUSE && GetAsyncKeyState(VK_CONTROL)<0)) GetParent()->DestroyWindow();
			else
			{
				if(m_nSeconds==0)
				{
					switch(nChar)
					{
					case VK_RIGHT:
						NextSlide(true);
						break;
					case VK_LEFT:
						NextSlide(false);
						break;
					}
				}
			}
		}
		return TRUE;
	}
	return CView::PreTranslateMessage(pMsg);
}
