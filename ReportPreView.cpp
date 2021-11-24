// ReportPreView.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ReportPreView.h"
#include "ReportPreFrame.h"
#include "MDReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportPreView

IMPLEMENT_DYNCREATE(CReportPreView, CScrollView)

CReportPreView::CReportPreView()
{
}

CReportPreView::~CReportPreView()
{
}


BEGIN_MESSAGE_MAP(CReportPreView, CScrollView)
	//{{AFX_MSG_MAP(CReportPreView)
	ON_UPDATE_COMMAND_UI(ID_DATA_FIND_REPLACE, OnUpdateDataFindReplace)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT,OnFilePrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportPreView drawing

void CReportPreView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	m_pReport=GetDocument()->m_pActiveReport;
	CString sTitle;
	sTitle.Format(IDS_PRINT,m_pReport->m_sTitle);
	GetParentFrame()->SetWindowText(sTitle);

	LOGFONT lfScaled=m_pReport->m_LogFont;
	lfScaled.lfHeight=MulDiv(lfScaled.lfHeight,72,/*m_pDC->GetDeviceCaps(LOGPIXELSY)*/96);
	m_ReportFont.CreateFontIndirect(&lfScaled);

	SetReportSize();
	Calculate();
}

void CReportPreView::DoUpdate()
{
	SetReportSize();
	Calculate();
	Invalidate();
}

void CReportPreView::SetReportSize()
{
	CSize sizeTotal(0,0);
	for(int i=0;i<6;i++)
		sizeTotal.cx+=m_pReport->m_nColumnWidths[i];
	{
		CClientDC theDC(this);
		CGDIObject<CFont> theFont(&theDC,&m_ReportFont);
		TEXTMETRIC tm;
		CSize sz=theDC.GetTextMetrics(&tm);
		m_nRowHeight=tm.tmHeight+1;
	}
	sizeTotal.cy+=m_nRowHeight*m_pReport->m_Rows.GetSize();

	SetScrollSizes(MM_TEXT,sizeTotal);
}

void CReportPreView::Calculate()
{
	m_pReport->Calculate(GetDocument());
}

void CReportPreView::OnDraw(CDC* pDC)
{
	CGDITextColor theTextColor(pDC,pDC->IsPrinting()?RGB(0,0,0):GetSysColor(COLOR_WINDOWTEXT));
	CGDIBackColor theBackColor(pDC,pDC->IsPrinting()?RGB(255,255,255):GetSysColor(COLOR_WINDOW));
	CGDIObject<CFont> theFont(pDC,&m_ReportFont);
	CRect rc;
	rc.bottom=0;
	CRect rcVisible;
	GetClientRect(&rcVisible);
	CMDReportRow* pLastCriteriaRow=NULL;
	for(int i=0;i<m_pReport->m_Rows.GetSize();i++)
	{
		rc.top=rc.bottom;
		rc.bottom=rc.top+m_nRowHeight;
		CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[i];
		if(pReportRow->m_nRowType==kCriteriaRow)
		{
			pLastCriteriaRow=pReportRow;
		}
		else
		{
			if(pDC->IsPrinting())
			{
				if(i<m_nPrintPageNo*m_nNumRowInPage || i>=(m_nPrintPageNo+1)*m_nNumRowInPage)
					continue;
			}
			rcVisible.top=rc.top;
			rcVisible.bottom=rc.bottom;
			if(!pDC->RectVisible(&rcVisible))
				continue;
			int cx=0;
			for(int n=0;n<6;n++)
			{
				int x=cx;
				cx+=m_pReport->m_nColumnWidths[n];
				if(cx-x>0)
				{
					CString theText=pReportRow->m_sRowText[n];
					int nJust=pReportRow->m_nJust[n];
					if(pReportRow->m_nRowType==kFunctionRow && pLastCriteriaRow && pLastCriteriaRow->m_nFieldRefNum[n]==kResultField)
					{
						rc.left=x;
						rc.right=cx;
						pDC->DrawText(theText,rc,nJust|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
					}
					else
					{
						if(!theText.IsEmpty())
						{
							int m;
							if(nJust==kJustRight || nJust==kJustCenter)
							{
								m=n;
								while(--m>=0 && pReportRow->m_sRowText[m].IsEmpty() && !(pReportRow->m_nRowType==kFunctionRow && pLastCriteriaRow && pLastCriteriaRow->m_nFieldRefNum[m]==kResultField))
								{
									x-=m_pReport->m_nColumnWidths[m];
								}
							}
							if(nJust==kJustLeft || nJust==kJustCenter)
							{
								m=n;
								while(++m<6 && pReportRow->m_sRowText[m].IsEmpty() && !(pReportRow->m_nRowType==kFunctionRow && pLastCriteriaRow && pLastCriteriaRow->m_nFieldRefNum[m]==kResultField))
								{
									cx+=m_pReport->m_nColumnWidths[m];
								}
								n=--m;
							}
							rc.left=x+1;
							rc.right=cx-2;
							pDC->DrawText(theText,rc,nJust|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
						}
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CReportPreView diagnostics

#ifdef _DEBUG
void CReportPreView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CReportPreView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CReportPreView message handlers

void CReportPreView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	switch(lHint)
	{
		case kSaveRef:
			m_nReportTag=m_pReport?m_pReport->GetTag():0;
			break;
		case kResolveRef:
			m_pReport=GetDocument()->GetReportByTag(m_nReportTag);
			if(!m_pReport)
				GetParentFrame()->SendMessage(WM_CLOSE);
			break;
		case kReportTitleChanged:
			GetParentFrame()->SetWindowText(m_pReport->m_sTitle);
			break;
		case kReportDeleted:
			if(m_pReport==pHint)
				GetParentFrame()->SendMessage(WM_CLOSE);
			break;
	}
}

CScrollBar* CReportPreView::GetScrollBarCtrl(int nBar) const
{
	CReportPreFrame* pFrame=(CReportPreFrame*)GetParentFrame();
	return pFrame->GetScrollBar(nBar);
}

BOOL CReportPreView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	return DoPreparePrinting(pInfo);
}

void CReportPreView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CScrollView::OnPrepareDC(pDC, pInfo);
	if(pDC->IsPrinting())
	{
		m_nPrintPageNo=pInfo->m_nCurPage-1;
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetWindowExt(72,72);
		pDC->SetViewportExt(pDC->GetDeviceCaps(LOGPIXELSX),pDC->GetDeviceCaps(LOGPIXELSY));
		pDC->SetViewportOrg(0,MulDiv(-1*(pInfo->m_nCurPage-1)*m_nRowHeight*m_nNumRowInPage,pDC->GetDeviceCaps(LOGPIXELSY),72));
	}
}

void CReportPreView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	m_nNumRowInPage=pDC->GetDeviceCaps(VERTRES)/MulDiv(m_nRowHeight,pDC->GetDeviceCaps(LOGPIXELSY),72);
	pInfo->SetMaxPage(m_pReport->m_Rows.GetSize()/m_nNumRowInPage+1);
}

void CReportPreView::OnUpdateDataFindReplace(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}
