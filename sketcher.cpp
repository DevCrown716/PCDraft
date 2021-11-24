// Sketcher.cpp: implementation of the CSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PCDraft.h"
#include "Sketcher.h"

#include "PCDraftView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CSketcher::CSketcher() : 
   m_pView(NULL), 
m_pPen(NULL) 
{

}


CSketcher::~CSketcher()
{ 

   if (m_pPen)
   {

      delete m_pPen;

   }

}


bool CSketcher::DoSketch(CPCDraftView* pView, CPoint& ptStart, LPRECT lpRectClip,UINT nSketchMode)
{
	CFlag cFlag;
	m_pView=pView;
	m_nSketchMode=nSketchMode;

	LongPoint pt,ptOld;
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
	// Need to prepare back buffer w/o curve and after that make modifications
	OnBeginSketch(&dc,pt);
	pView->ShowCursorPromptNoLock(false);
	pView->ShowCrossCursor(false);

	OnDrawSketcher(&dc);
	pView->ShowCrossCursor();
	pView->ShowCursorPromptNoLock();

	while(1)
	{
		LONG lIdle = 0;
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
               bool bDocSnap = false;
               
               if (!(m_nSketchMode & NoSnapToDocument))
               {

                  bDocSnap = pView->SnapPtToDocument(pt);

               }

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
					if(!bForceQuery && (ptOld==pt/* || !pView->PtInDocument(pt)*/)) break;
					pt=AdjustPosition(pt);
					if(!bForceQuery && (ptOld==pt/* || !pView->PtInDocument(pt)*/)) break;

					CPoint ptCursor;
					if(bDocSnap) ptCursor=ptMsg;
					else ptCursor=pView->m_Mapper.LPtoDP(pt)-pView->GetDeviceScrollPosition();
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
					OnEraseSketcher(&dc);
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
						OnDrawSketcher(&dc);
						pView->ShowCrossCursor();
						pView->ShowCursorPromptNoLock();
					}
					ptOld=pt;
					break;
				}
				/* fall */
			case WM_CANCELMODE:
			case WM_CAPTURECHANGED:
				OnCancelSketch(&dc,pt);
				if(m_msg.message!=WM_CAPTURECHANGED) ReleaseCapture();
				if(bPromptHidden) m_pView->ShowCursorPrompt();
				return false;
			default:
//				if (!AfxGetApp()->PumpMessage ()) return false;
            if (m_msg.message != WM_KICKIDLE && !AfxPreTranslateMessage(&m_msg))
            {
               ::TranslateMessage(&m_msg);
               ::DispatchMessage(&m_msg);
            }
            break;
			}
		}
		else
		{
			// let MFC do its idle processing
			while(AfxGetApp()->OnIdle(lIdle++));  

			if((m_nSketchMode & NoScroll)==0)
			{
				CPoint ptMsg;
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

void CSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	pDC->SetROP2(R2_NOT);
	//pDC->SetROP2(R2_BLACK);
   m_pPen = new CPen(PS_SOLID, 0, RGB(255, 255, 255));
   m_pOldPen = (CPen*)m_pPen;
	m_pOldBrush=(CBrush*)pDC->SelectStockObject(NULL_BRUSH);
}

void CSketcher::OnCancelSketch(CDC* pDC, const LongPoint& pt)
{
	pDC->SelectObject(m_pOldBrush);
	pDC->SelectObject(m_pOldPen);
	m_pView->ClearShowSize();
}

CSketcher::ContinueMode CSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_KEYUP:
	case WM_MOUSEMOVE:
		return ContinueUpdatePt;

	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		return Complete;

	case WM_KEYDOWN:
		if(m_msg.wParam!=VK_ESCAPE) return ContinueUpdatePt;
	case WM_RBUTTONDOWN:
		return Cancel;
	}
	return Continue;
}

bool CSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& ptTo)
{
	pDC->SelectObject(m_pOldBrush);
	pDC->SelectObject(m_pOldPen);
	m_pView->ClearShowSize();
	return true;
}

