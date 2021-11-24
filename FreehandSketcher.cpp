// FreehandSketcher.cpp: implementation of the CFreehandSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FreehandSketcher.h"

#include "gu.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDPolygonShape.h"
#include "DraftContext.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	kMinDraw ((long)5)

//////////////////////////////////////////////////////////////////////

void CFreehandSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC,pt);
	//pDC->SetROP2(R2_NOT);
	//pDC->SetROP2(R2_BLACK);
	m_pt[m_nCurPoint]=pt;
}

bool CFreehandSketcher::DoSketch(CPCDraftView* pView, CPoint& ptStart, LPRECT lpRectClip,UINT nSketchMode)
{
	CFlag cFlag;

	m_pView=pView;
	m_nSketchMode=nSketchMode;
	wasErased = false;

	LongPoint pt,ptOld;
	bool wasScrolled = false;
	pt=pView->m_Mapper.DPtoLP(ptStart);
	if((m_nSketchMode & NoSnap)==0 && GetAsyncKeyState(VK_COMMAND)>=0) pView->SnapLPt(pt);
	ptOld=pt;

	CClientDC dc(pView);
	CRgn rgn;
	if(lpRectClip)
	{
		rgn.CreateRectRgnIndirect(lpRectClip);
		dc.SelectClipRgn(&rgn);
	}
	pView->OnPrepareDC(&dc);
	// the show will begin...
	pView->SetCapture();
	pointd ptOrg=m_pView->GetDeviceScrollPosition();
	dc.SetViewportOrg(-ptOrg);

	bool bPromptHidden=false;
	OnBeginSketch(&dc,pt);
	pView->ShowCursorPromptNoLock(false);
	pView->ShowCrossCursor(false);

	OnDrawSketcher(&dc);
	pView->ShowCrossCursor();
	pView->ShowCursorPromptNoLock();

	while(1)
	{
		LONG lIdle = 0;
		wasScrolled = false;
		if(PeekMessage(&m_msg,NULL,0,0,PM_REMOVE))
		{
			lIdle = 0;

			pointd ptMsg=m_msg.pt;
			pView->ScreenToClient(&ptMsg);
			
			ContinueMode nMode=ContinueAddPt;
			bool bForceUpdate=false;
			bool bForceQuery=false;
			switch(m_msg.message)
			{
			case WM_KEYDOWN:
				if(m_msg.lParam&0x40000000) break;
			case WM_KEYUP:
				switch(m_msg.wParam)
				{
				case VK_SHIFT:
				case VK_CONTROL:
				case VK_OPTION:
					DispatchMessage(&m_msg);
				case VK_BACK:
					bForceQuery=true;
					break;
				}
			case WM_MOUSEMOVE:
				{
					pt=pView->m_Mapper.DPtoLP(ptMsg+pView->GetDeviceScrollPosition());
					bool bDocSnap=pView->SnapPtToDocument(pt);
					if((m_nSketchMode & NoSnap)==0 && GetAsyncKeyState(VK_COMMAND)>=0) pView->SnapLPt(pt,true);

					int nType=0;
					if(GetAsyncKeyState(0x43)<0) nType=IDC_CENTER_SNAP;		// VK_C
					else if(GetAsyncKeyState(0x45)<0) nType=IDC_END_SNAP;	// VK_E
					else if(GetAsyncKeyState(0x4D)<0) nType=IDC_MID_SNAP;	// VK_M
					if(nType)
					{
						if(!bPromptHidden)
						{
							bPromptHidden=true;
							m_pView->ShowCursorPrompt(false);
							SetCursor(AfxGetApp()->LoadCursor(nType));
							bForceUpdate=true;
						}
					}
					else if(bPromptHidden)
					{
						bPromptHidden=false;
						m_pView->ShowCursorPrompt();
						bForceUpdate=true;
					}
					if(!bForceQuery && (ptOld==pt)) break;
					pt=AdjustPosition(pt);
					if(!bForceQuery && (ptOld==pt)) break;

					CPoint ptCursor;
					if(bDocSnap) ptCursor=ptMsg;
					else ptCursor=pView->m_Mapper.LPtoDPF(pt)-pView->GetDeviceScrollPosition();
					pView->AdjustCursor(ptCursor,true);
					AfxGetApp()->OnIdle(0); // update show size
				}

			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_LBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MBUTTONDBLCLK:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_RBUTTONDBLCLK:
				nMode=QueryContinue();
				if(nMode==Continue && bForceUpdate) nMode=ContinueUpdatePt;
				if(m_msg.message==WM_LBUTTONDOWN)
				{
					if(CWnd::WindowFromPoint(m_msg.pt) != m_pView)
						nMode=Cancel;
				}
				if(nMode!=Continue)
				{
					pView->ShowCursorPromptNoLock(false);
					pView->ShowCrossCursor(false);
					wasScrolled = pView->SketcherScroll(ptMsg, FALSE);
					//if(wasScrolled || m_bEraser) OnEraseSketcher(&dc);
					if(wasScrolled) OnEraseSketcher(&dc);
					pView->ShowCrossCursor();
					pView->ShowCursorPromptNoLock();
				}
				switch(nMode)
				{
				case Complete:
					ReleaseCapture();
					if(bPromptHidden) m_pView->ShowCursorPrompt();
					return OnCompleteSketch(&dc, pt);
				case ContinueAddPt:
					OnAddPt(pt);
					break;
				case ContinueUpdatePt:
					pView->ShowCursorPromptNoLock(false);
					pView->ShowCrossCursor(false);
					//if(!(wasScrolled || m_bEraser)) OnQuickRedraw(&dc);
					if(!(wasScrolled)) OnQuickRedraw(&dc);
					pView->ShowCrossCursor();
					pView->ShowCursorPromptNoLock();
					OnUpdatePt(pt);
					break;
				}
				if(nMode!=Cancel)
				{
					if(nMode!=Continue)
					{
						pView->ShowCursorPromptNoLock(false);
						pView->ShowCrossCursor(false);
					}
					if((m_nSketchMode & NoScroll)==0) pView->SketcherScroll(ptMsg);
					pointd ptOrg=m_pView->GetDeviceScrollPosition();
					dc.SetViewportOrg(-ptOrg);
					if(nMode!=Continue)
					{
						//if(wasScrolled || m_bEraser) OnDrawSketcher(&dc);
						if(wasScrolled) OnDrawSketcher(&dc);
						else OnQuickRedraw(&dc);
						pView->ShowCrossCursor();
						pView->ShowCursorPromptNoLock();
					}
					ptOld=pt;
					break;
				}
				// fall 
			case WM_CANCELMODE:
			case WM_CAPTURECHANGED:
				OnCancelSketch(&dc,pt);
				if(m_msg.message!=WM_CAPTURECHANGED) ReleaseCapture();
				if(bPromptHidden) m_pView->ShowCursorPrompt();
				return false;
			default:
				DispatchMessage(&m_msg);
				break;
			}
		}
		else
		{
			// let MFC do its idle processing
			while(AfxGetApp()->OnIdle(lIdle++));  

			if((m_nSketchMode & NoScroll)==0)
			{
				point ptMsg;
				GetCursorPos(&ptMsg);
				pView->ScreenToClient(&ptMsg);

            CPoint ptClientCorner;
				pView->GetClientCorner(ptClientCorner);
				CSize szClientSize;
				pView->GetClientSize(szClientSize);
				CRect rcClient(ptClientCorner,szClientSize);
				if(!rcClient.PtInRect(ptMsg))
				{
					pView->ShowCursorPrompt(false);
					pView->ShowCrossCursor(false);
					OnEraseSketcher(&dc);

					pView->SketcherScroll(ptMsg);
					pView->UpdateWindow();
					pointd ptOrg=m_pView->GetDeviceScrollPosition();
					dc.SetViewportOrg(-ptOrg);

					OnDrawSketcher(&dc);
					//OnQuickRedraw(&dc);
					pView->ShowCrossCursor();
					pView->ShowCursorPrompt();

					WPARAM fwKeys=0;
					if(GetAsyncKeyState(VK_CONTROL)<0) fwKeys|=MK_CONTROL;
					if(GetAsyncKeyState(VK_LBUTTON)<0) fwKeys|=MK_LBUTTON;
					if(GetAsyncKeyState(VK_MBUTTON)<0) fwKeys|=MK_MBUTTON;
					if(GetAsyncKeyState(VK_RBUTTON)<0) fwKeys|=MK_RBUTTON;
					if(GetAsyncKeyState(VK_SHIFT)<0) fwKeys|=MK_SHIFT;
					PostMessage(pView->GetSafeHwnd(),WM_MOUSEMOVE,fwKeys,MAKELONG(ptMsg.x,ptMsg.y));
				}
			}
		}
	}
	return false;
}

CSketcher::ContinueMode CFreehandSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_KEYDOWN:
	case WM_MOUSEMOVE:
		if(m_nCurPoint==0) return ContinueAddPt;
		if(GetAsyncKeyState(VK_SHIFT)<0)
		{
			if(!m_bEraser)
			{
				m_pView->ShowCursorPrompt(false);
				m_pView->ShowCrossCursor(false);
				m_bEraser=true;
			}
			return ContinueUpdatePt;
		}
		else HideEraser();
		return GetAsyncKeyState(VK_OPTION)<0?ContinueUpdatePt:ContinueAddPt;
	default:
		return CSketcher::QueryContinue();
	}
}

void CFreehandSketcher::OnAddPt(const LongPoint& ptAdd)
{
	if(m_nCurPoint<kMaxPoint) m_nCurPoint++;
	m_pt[m_nCurPoint]=ptAdd;
}

void CFreehandSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	wasErased = false;
	if(GetAsyncKeyState(VK_SHIFT)<0 && m_nCurPoint>1)
	{
		LongPoint delta=Abs(ptUpdate-m_pt[m_nCurPoint-1]);
		WrFixed minDraw=m_pView->m_Mapper.DPtoLP(kMinDraw);
		if(delta.x<minDraw && delta.y<minDraw) {
			m_nCurPoint--;
			wasErased = true;
			oldPoint = m_pt[m_nCurPoint];
		}
	}
	m_pt[m_nCurPoint]=ptUpdate;
}

void CFreehandSketcher::OnDrawSketcher(CDC * pDC)
{
	pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_pt[0]));
	for(int i=1;i<=m_nCurPoint;i++)
		pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pt[i]));
}

void CFreehandSketcher::OnQuickRedraw(CDC* pDC)
{
	if(wasErased) {
		pDC->MoveTo(m_pView->m_Mapper.LPtoDP(oldPoint));
		pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pt[m_nCurPoint-1]));
		pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pt[m_nCurPoint]));
		wasErased = false;
	} else {
		if(m_nCurPoint >0) 
			pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_pt[m_nCurPoint - 1]));
		else 
			pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_pt[0]));
		pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pt[m_nCurPoint]));
	}
}

void CFreehandSketcher::OnCancelSketch(CDC* pDC, const LongPoint& pt)
{
	HideEraser();
	CSketcher::OnCancelSketch(pDC, pt);
}

bool CFreehandSketcher::OnCompleteSketch(CDC *pDC, const LongPoint& pt)
{
	HideEraser();
	CSketcher::OnCompleteSketch(pDC, pt);
	if(m_nCurPoint>(m_bClosed?1:0))
	{
		m_pView->SaveUndo(IDS_CREATION);
		CMDPolygonShape* pShape=new CMDPolygonShape(CMDPolygonShape::kFreehand,m_nCurPoint+1,m_bClosed);

		for(int i=0;i<=m_nCurPoint;i++)
			pShape->SetShapePoint(i,m_pt[i]);

		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		if(m_bClosed) pShape->SetFillModel(m_pView->GetDocument()->m_Fill);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);
		return true;
	}
	return false;
}

void CFreehandSketcher::HideEraser()
{
	if(m_bEraser)
	{
		m_pView->ShowCrossCursor();
		m_pView->ShowCursorPrompt();
		m_bEraser=false;
	}
}

void CFreehandSketcher::OnEraseSketcher(CDC * pDC) 
{ 
	OnDrawSketcher(pDC); 
}