// ReportEditView.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ReportEditView.h"
#include "ReportEditFrame.h"
#include "CloneOptionsDlg.h"
#include "CloneAgainDlg.h"
#include "PCDraftView.h"
#include "MDReport.h"
#include <strstream>
// #include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define kFunctionWidthStr		"PARAMETER"
#define kDefaultWidthStr		"0000000000"

#define	kMaxRowPopOutWidth		18
#define	kColumnHeaderHeight		16

#define kFunctionColWidth		100
#define kMinCriteriaRowWidth	10

#define kMinFunctionRowHeight	15
#define kColumnSenseOffset		3

/////////////////////////////////////////////////////////////////////////////
// CReportEditView

IMPLEMENT_DYNCREATE(CReportEditView, CScrollView)

CReportEditView::CReportEditView()
{
}

CReportEditView::~CReportEditView()
{
	if(m_DotBrush.GetSafeHandle())
		m_DotBrush.DeleteObject();
	if(m_ReportFont.GetSafeHandle())
		m_ReportFont.DeleteObject();
}

BEGIN_MESSAGE_MAP(CReportEditView, CScrollView)
	//{{AFX_MSG_MAP(CReportEditView)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_TEXT_JUSTIFY_CENTER, OnTextJustifyCenter)
	ON_COMMAND(ID_TEXT_JUSTIFY_LEFT, OnTextJustifyLeft)
	ON_COMMAND(ID_TEXT_JUSTIFY_RIGHT, OnTextJustifyRight)
	ON_COMMAND(ID_TEXT_FONT, OnTextFont)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_DELETE_ROW, OnEditDeleteRow)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE_ROW, OnUpdateEditDeleteRow)
	ON_COMMAND(ID_EDIT_INSERT_ROW, OnEditInsertRow)
	ON_COMMAND(ID_FILE_EXPORT_REPORT, OnFileExportReport)
	ON_WM_ERASEBKGND()
	ON_UPDATE_COMMAND_UI(ID_DATA_FIND_REPLACE, OnUpdateDataFindReplace)
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_LAYER_SETUP, OnUpdateLayoutLayerSetup)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_REPORT, OnUpdateFileExportReport)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_INPLACEEDIT,OnInplaceEditChange)
	ON_UPDATE_COMMAND_UI(ID_TEXT_JUSTIFY_CENTER, OnUpdateTextJustify)
	ON_UPDATE_COMMAND_UI(ID_TEXT_JUSTIFY_LEFT, OnUpdateTextJustify)
	ON_UPDATE_COMMAND_UI(ID_TEXT_JUSTIFY_RIGHT, OnUpdateTextJustify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportEditView drawing

void CReportEditView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	m_pReport=GetDocument()->m_pActiveReport;
	GetParentFrame()->SetWindowText(m_pReport->m_sTitle);

	{
		CBitmap theBitmap;
		theBitmap.LoadBitmap(IDB_PATDOT);
		m_DotBrush.CreatePatternBrush(&theBitmap);
	}
	m_SysFont.CreateStockObject(DEFAULT_GUI_FONT);
	CreateReportFont();

	m_nEndSelectedRow=m_nStartSelectedRow=-1;
	m_nEndSelectedCol=m_nStartSelectedCol=-1;

	SetReportSize();
}

void CReportEditView::SetReportSize()
{
	CSize sizeTotal(0,0);
	sizeTotal.cx=kFunctionColWidth;
	for(int i=0;i<6;i++)
		sizeTotal.cx+=m_pReport->m_nColumnWidths[i];
	sizeTotal.cy=kColumnHeaderHeight;
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

void CReportEditView::InternalDraw(CDC* pDC,bool bSelected)
{
	CRect rc(0,0,kFunctionColWidth,kColumnHeaderHeight);
	pDC->DrawFrameControl(&rc,DFC_BUTTON,DFCS_BUTTONPUSH);
	{
		CGDIObject<CFont> theSysFont(pDC,&m_SysFont);
		CGDITextColor theTextColor(pDC,::GetSysColor(COLOR_BTNTEXT));
		CGDIBackMode theBackMode(pDC,TRANSPARENT);
		CString strText;
		strText.LoadString(IDS_CLONE);
		pDC->DrawText(strText,rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}
    int i = 0;
	for(;i<6;i++)
	{
		rc.left=rc.right;
		rc.right=rc.left+m_pReport->m_nColumnWidths[i];
		if(rc.left!=rc.right)
			pDC->DrawFrameControl(&rc,DFC_BUTTON,DFCS_BUTTONPUSH|(IsSelectedCol(i)?DFCS_PUSHED:0));
	}
	int nWidth=rc.right;
	rc.bottom=kColumnHeaderHeight;

	m_DotBrush.UnrealizeObject();
	pDC->SetBrushOrg(-GetScrollPosition());
	CGDIObject<CBrush> theDotBrush(pDC,&m_DotBrush);

	CMDReportRow* pLastCriteriaRow=NULL;
	CRect rcVisible;
	GetClientRect(&rcVisible);
	if(m_totalDev.cx>rcVisible.right)
		rcVisible.right=m_totalDev.cx;
	for(i=0;i<m_pReport->m_Rows.GetSize();i++)
	{
		rc.left=0;
		rc.right=kMaxRowPopOutWidth;
		rc.top=rc.bottom;
		rc.bottom=rc.top+m_nRowHeight;
		rcVisible.top=rc.top;
		rcVisible.bottom=rc.bottom;
		BOOL bRowVisible=pDC->RectVisible(&rcVisible);
		if(bRowVisible)
		{
			pDC->DrawFrameControl(&rc,DFC_BUTTON,DFCS_BUTTONPUSH);
			((CPCDraftApp*)AfxGetApp())->m_imgReport.Draw(pDC,0,CPoint(rc.left+(rc.Width()-7)/2,rc.top+(rc.Height()-7)/2),ILD_NORMAL);
		}
		CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[i];
		if(pReportRow->m_nRowType==kCriteriaRow)
		{
			pLastCriteriaRow=pReportRow;
			if(!bRowVisible)
				continue;
			UINT nState=DFCS_BUTTONPUSH|((bSelected || IsSelectedRow(i))?DFCS_PUSHED:0);
			rc.left=rc.right;
			rc.right=kFunctionColWidth;
			pDC->DrawFrameControl(&rc,DFC_BUTTON,nState);
			CRect trc=rc;
			trc.DeflateRect(kColumnSenseOffset,kColumnSenseOffset-2,kColumnSenseOffset,kColumnSenseOffset);
			CGDIObject<CFont> theSysFont(pDC,&m_SysFont);
			CGDITextColor theTextColor(pDC,::GetSysColor(COLOR_BTNTEXT));
			CGDIBackMode theBackMode(pDC,TRANSPARENT);
			pDC->DrawText(GetCriteriaName(),trc,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
			for(int n=0;n<6;n++)
			{
				rc.left=rc.right;
				rc.right=rc.left+m_pReport->m_nColumnWidths[n];
				if(rc.left!=rc.right)
				{
					pDC->DrawFrameControl(&rc,DFC_BUTTON,nState);
					if(rc.Width()>16)
					{
						trc=rc;
						trc.DeflateRect(kColumnSenseOffset,kColumnSenseOffset-2,7+2*kColumnSenseOffset,kColumnSenseOffset);
						pDC->DrawText(GetFieldRefNumName(pReportRow->m_nFieldRefNum[n]),trc,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
						((CPCDraftApp*)AfxGetApp())->m_imgReport.Draw(pDC,1,CPoint(rc.right-11,rc.top+(rc.Height()-7)/2),ILD_NORMAL);
					}
				}
			}
		}
		else
		{
			if(!bRowVisible)
				continue;
			{
				CRect trc=rc;
				trc.left=rc.right;
				trc.right=kFunctionColWidth;
				{
					CGDIBackColor theBackColor(pDC,::GetSysColor((bSelected || IsSelectedRow(i))?COLOR_HIGHLIGHT:COLOR_WINDOW));
					pDC->ExtTextOut(0,0,ETO_OPAQUE,&trc,NULL,0,NULL);
				}
				if(pReportRow->m_nRowType==kFunctionRow)
				{
					CGDITextColor theTextColor(pDC,::GetSysColor((bSelected || IsSelectedRow(i))?COLOR_HIGHLIGHTTEXT:COLOR_WINDOWTEXT));
					CGDIBackMode theBackMode(pDC,TRANSPARENT);
					CGDIObject<CFont> theSysFont(pDC,&m_SysFont);
					pDC->DrawText(GetFunctionName(pReportRow->m_nFunction),trc,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
				}
			}
			pDC->MoveTo(rc.right,rc.bottom-1);
			pDC->LineTo(kFunctionColWidth-1,rc.bottom-1);
			pDC->LineTo(kFunctionColWidth-1,rc.top-1);
			{
				CRect trc=rc;
				trc.left=kFunctionColWidth;
				trc.right=trc.left;
				for(int n=0;n<6;n++)
					trc.right+=m_pReport->m_nColumnWidths[n];
				CGDIBackColor theBackColor(pDC,::GetSysColor((bSelected || IsSelectedRow(i))?COLOR_HIGHLIGHT:COLOR_WINDOW));
				pDC->ExtTextOut(0,0,ETO_OPAQUE,&trc,NULL,0,NULL);
			}
			if(i<=m_pReport->m_Rows.GetSize()-1)
				pDC->PatBlt(kFunctionColWidth,rc.bottom-1,nWidth-kFunctionColWidth,1,PATCOPY);
			int cx=kFunctionColWidth-1;
            int n = 0;
			for(;n<5;n++)
			{
				cx+=m_pReport->m_nColumnWidths[n];
				pDC->PatBlt(cx,rc.top,1,m_nRowHeight,PATCOPY);
			}
			cx=kFunctionColWidth-1;
			for(n=0;n<6;n++)
			{
				int x=cx;
				cx+=m_pReport->m_nColumnWidths[n];
				if(cx-x>0)
				{
					if(pReportRow->m_nRowType==kFunctionRow && pLastCriteriaRow && pLastCriteriaRow->m_nFieldRefNum[n]==kResultField)
					{
						pDC->PatBlt(x,rc.top,cx-x,m_nRowHeight,PATCOPY);
					}
					else
					{
						CString theText=pReportRow->m_sRowText[n];
						int nJust=pReportRow->m_nJust[n];
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
							CRect trc=rc;
							trc.left=x+1;
							trc.right=cx-2;
							CGDITextColor theTextColor(pDC,::GetSysColor((bSelected || IsSelectedRow(i))?COLOR_HIGHLIGHTTEXT:COLOR_WINDOWTEXT));
							CGDIBackColor theBackColor(pDC,::GetSysColor((bSelected || IsSelectedRow(i))?COLOR_HIGHLIGHT:COLOR_WINDOW));
							CGDIObject<CFont> theFont(pDC,&m_ReportFont);
							pDC->DrawText(theText,trc,nJust|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
						}
					}
				}
			}
		}
	}
	pDC->MoveTo(nWidth-1,kColumnHeaderHeight);
	pDC->LineTo(nWidth-1,rc.bottom-1);
	pDC->LineTo(kFunctionColWidth-1,rc.bottom-1);
}

void CReportEditView::OnDraw(CDC* pDC)
{
	if(IsSelectedCol())
	{
		CPoint pt=GetDeviceScrollPosition();
		CRect rc;
		GetClientRect(&rc);
		CRgn theBigRgn;
		theBigRgn.CreateRectRgnIndirect(&rc);
		int x=kFunctionColWidth-pt.x;
		for(int n=0;n<6;n++)
		{
			if(n==m_nStartSelectedCol)
				rc.left=x;
			x+=m_pReport->m_nColumnWidths[n];
			if(n==m_nEndSelectedCol)
				break;
		}
		rc.right=x;
		CRgn theSelRgn;
		theSelRgn.CreateRectRgnIndirect(&rc);

		pDC->SelectClipRgn(&theBigRgn);
		pDC->SelectClipRgn(&theSelRgn,RGN_DIFF);
		if(IsSelectedRow())
		{
			int nStartRow=m_nStartSelectedRow;
			int nEndRow=m_nEndSelectedRow;
			m_nStartSelectedRow=m_nEndSelectedRow=-1;
			InternalDraw(pDC,false);
			m_nStartSelectedRow=nStartRow;
			m_nEndSelectedRow=nEndRow;
		}
		else
		{
			InternalDraw(pDC,false);
		}
		pDC->SelectClipRgn(NULL);

		pDC->SelectClipRgn(&theSelRgn);
		InternalDraw(pDC,IsSelectedRow()?false:true);
		pDC->SelectClipRgn(NULL);
	}
	else
		InternalDraw(pDC,false);
}

/////////////////////////////////////////////////////////////////////////////
// CReportEditView diagnostics

#ifdef _DEBUG
void CReportEditView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CReportEditView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CReportEditView message handlers

void CReportEditView::InvalidateSelectedRows()
{
	CPoint theCorner=GetScrollPosition();
	CRect rc;
	GetClientRect(&rc);
	rc.top=kColumnHeaderHeight+m_nStartSelectedRow*m_nRowHeight-theCorner.y;
	rc.bottom=rc.top+(m_nEndSelectedRow-m_nStartSelectedRow+1)*m_nRowHeight;
	InvalidateRect(&rc);
}

void CReportEditView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nStartRow,nEndRow;
	int nStartCol,nEndCol;
	CPoint theCorner=GetScrollPosition();
	point.x+=theCorner.x;
	point.y+=theCorner.y;
	if(m_wndInplaceEdit.GetSafeHwnd())
	{
		m_wndInplaceEdit.DestroyWindow();
		UpdateWindow();
	}
	if(IsSelectedRow())
	{
		InvalidateSelectedRows();
		nStartRow=m_nStartSelectedRow;
		nEndRow=m_nEndSelectedRow;
		m_nEndSelectedRow=m_nStartSelectedRow=-1;
		UpdateWindow();
	}
	else
		nStartRow=nEndRow=-1;
	if(IsSelectedCol())
	{
		InvalidateSelectedCols();
		nStartCol=m_nStartSelectedCol;
		nEndCol=m_nEndSelectedCol;
		m_nEndSelectedCol=m_nStartSelectedCol=-1;
		UpdateWindow();
	}
	else
		nStartCol=nEndCol=-1;
	if(point.y<kColumnHeaderHeight && point.x<kFunctionColWidth)
	{
		if(DoClick(CRect(0,0,kFunctionColWidth,kColumnHeaderHeight),point))
		{
			CPCDraftDoc* pDoc=GetDocument();
			if(m_pReport->IsLinked())
			{
				CCloneAgainDlg theDlg;
				theDlg.m_CloneAgain.Format(IDS_CLONE_AGAIN,m_pReport->m_sLinkFile);
				theDlg.m_nType=0;
				if(theDlg.DoModal()!=IDOK)
					return;
				if(theDlg.m_nType!=0)
				{
					if(m_pReport->Calculate(pDoc,true))
					{
						m_pReport->Clone();
						m_pReport->Calculate(pDoc);
					}
					return;
				}
			}
			CCloneOptionsDlg theDlg;
			theDlg.m_nCopyType=0;
			if(theDlg.DoModal()==IDOK)
			{
				if(m_pReport->Calculate(pDoc,true))
				{
					m_pReport->Clone(theDlg.m_nCopyType);
					m_pReport->Calculate(pDoc);
				}
			}
		}
		return;
	}
	if(point.y<kColumnHeaderHeight && point.x>kFunctionColWidth)
	{
		int x=kFunctionColWidth-1;
		for(int i=0;i<6;i++)
		{
			int minx=x;
			x+=m_pReport->m_nColumnWidths[i];
			if(abs(point.x-x)<::GetSystemMetrics(SM_CXDOUBLECLK))
			{
				while(i+1<6 && m_pReport->m_nColumnWidths[i+1]==0)	// move hiden column first (zero width)
				{
					minx=x;
					i++;
				}
				SetCapture();
				point.x=x;
				point.x-=theCorner.x;
				point.y-=theCorner.y;
				ClientToScreen(&point);
				::SetCursorPos(point.x,point.y);
				CClientDC theDC(this);

				m_DotBrush.UnrealizeObject();
				theDC.SetBrushOrg(-GetScrollPosition());
				CGDIObject<CBrush> theDotBrush(&theDC,&m_DotBrush);

				int cy=kColumnHeaderHeight+m_pReport->m_Rows.GetSize()*m_nRowHeight;
				theDC.PatBlt(x-theCorner.x,0-theCorner.y,1,cy,PATINVERT);
				MSG msg;
				while(1)
				{
					GetMessage(&msg,NULL,0,0);
					ScreenToClient(&msg.pt);
					msg.pt.x+=theCorner.x;
					msg.pt.y+=theCorner.y;
					switch(msg.message)
					{
						case WM_MOUSEMOVE:
							if(msg.pt.x<minx)
							{
								msg.pt.x=minx;
							}
							theDC.PatBlt(x-theCorner.x,0-theCorner.y,1,cy,PATINVERT);
							x=msg.pt.x;
							theDC.PatBlt(x-theCorner.x,0-theCorner.y,1,cy,PATINVERT);
							break;
						case WM_LBUTTONUP:
							theDC.PatBlt(x-theCorner.x,0-theCorner.y,1,cy,PATINVERT);
							ReleaseCapture();
							m_pReport->m_nColumnWidths[i]=x-minx;
							GetDocument()->SetModifiedFlag();
							SetReportSize();
							Invalidate();
							UpdateWindow();
							return;
					}
				}
			}
			if(minx<point.x && point.x<x)
			{
				if(::GetAsyncKeyState(VK_SHIFT)<0
					&& nStartCol!=-1
					&& nEndCol!=-1)
				{
					if(i<nStartCol) nStartCol=i;
					else nEndCol=i;
					if(nStartCol<=nEndCol)
					{
						m_nStartSelectedCol=nStartCol;
						m_nEndSelectedCol=nEndCol;
					}
					else
					{
						m_nStartSelectedCol=nEndCol;
						m_nEndSelectedCol=nStartCol;
					}
				}
				else
					m_nEndSelectedCol=m_nStartSelectedCol=nStartCol=i;
				InvalidateSelectedCols();
				UpdateWindow();
				SetCapture();
				MSG msg;
				while(1)
				{
					GetMessage(&msg,NULL,0,0);
					ScreenToClient(&msg.pt);
					msg.pt.x+=theCorner.x;
					msg.pt.y+=theCorner.y;
					switch(msg.message)
					{
						case WM_MOUSEMOVE:
							{
								int x=kFunctionColWidth-1;
								for(nEndCol=0;nEndCol<6;nEndCol++)
								{
									x+=m_pReport->m_nColumnWidths[nEndCol];
									if(msg.pt.x<x)
										break;
								}
								if(nEndCol>=6)
									nEndCol=5;
							}
							InvalidateSelectedCols();
							if(nStartCol<=nEndCol)
							{
								m_nStartSelectedCol=nStartCol;
								m_nEndSelectedCol=nEndCol;
							}
							else
							{
								m_nStartSelectedCol=nEndCol;
								m_nEndSelectedCol=nStartCol;
							}
							InvalidateSelectedCols();
							UpdateWindow();
							break;
						case WM_LBUTTONUP:
							ReleaseCapture();
							return;
					}
				}
			}
		}
	}
	if(point.x<kFunctionColWidth && point.y>kColumnHeaderHeight && point.y<kColumnHeaderHeight+m_pReport->m_Rows.GetSize()*m_nRowHeight)
	{
		int n=(point.y-kColumnHeaderHeight)/m_nRowHeight;
		int y=kColumnHeaderHeight+n*m_nRowHeight;
		if(GetAsyncKeyState(VK_SHIFT)>=0 && point.x<kMaxRowPopOutWidth)
		{
			if(DoClick(CRect(0,y,kMaxRowPopOutWidth,y+m_nRowHeight),point))
			{
				CMenu theMenu;
				theMenu.LoadMenu(IDR_REPORT_POPUP);
				CPoint pt(point.x-theCorner.x,y-theCorner.y);
				ClientToScreen(&pt);
				CMenu* pMenu=theMenu.GetSubMenu(0);
				CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[n];
				UINT nID;
				switch(pReportRow->m_nRowType)
				{
					case kTextRow: nID=ID_STYLE_TEXT; break;
					case kCriteriaRow: nID=ID_STYLE_CRITERIA; break;
					case kFunctionRow:
						switch(pReportRow->m_nFunction)
						{
							case kFunctArea: nID=ID_FUNC_AREA; break;
							case kFunctCount: nID=ID_FUNC_COUNT; break;
							case kFunctHeight: nID=ID_FUNC_HEIGHT; break;
							case kFunctLength: nID=ID_FUNC_LENGTH; break;
							case kFunctPerimeter: nID=ID_FUNC_PERIMETER; break;
							case kFunctWidth: nID=ID_FUNC_WIDTH; break;
							case kFunctXDim: nID=ID_FUNC_XDIM; break;
							case kFunctYDim: nID=ID_FUNC_YDIM; break;
							case kFunctTotalName: nID=ID_FUNC_TOTAL_NAME; break;
							case kFunctTotalF2: nID=ID_FUNC_TOTAL_F2; break;
							case kFunctTotalF3: nID=ID_FUNC_TOTAL_F3; break;
							case kFunctTotalF4: nID=ID_FUNC_TOTAL_F4; break;
							case kFunctTotalF5: nID=ID_FUNC_TOTAL_F5; break;
							default:
								ASSERT(FALSE);
						}
						break;
					default:
						ASSERT(FALSE);
				}
				pMenu->ModifyMenu(ID_FUNC_TOTAL_NAME,MF_STRING|MF_BYCOMMAND,ID_FUNC_TOTAL_NAME,GetFunctionName(kFunctTotalName));
				pMenu->ModifyMenu(ID_FUNC_TOTAL_F2,MF_STRING|MF_BYCOMMAND,ID_FUNC_TOTAL_F2,GetFunctionName(kFunctTotalF2));
				pMenu->ModifyMenu(ID_FUNC_TOTAL_F3,MF_STRING|MF_BYCOMMAND,ID_FUNC_TOTAL_F3,GetFunctionName(kFunctTotalF3));
				pMenu->ModifyMenu(ID_FUNC_TOTAL_F4,MF_STRING|MF_BYCOMMAND,ID_FUNC_TOTAL_F4,GetFunctionName(kFunctTotalF4));
				pMenu->ModifyMenu(ID_FUNC_TOTAL_F5,MF_STRING|MF_BYCOMMAND,ID_FUNC_TOTAL_F5,GetFunctionName(kFunctTotalF5));
				pMenu->CheckMenuItem(nID,MF_BYCOMMAND|MF_CHECKED);
				nID=::TrackPopupMenu(pMenu->GetSafeHmenu(),TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTBUTTON|TPM_LEFTALIGN,pt.x,pt.y,0,this->GetSafeHwnd(),NULL);
				theMenu.DestroyMenu();
				if(nID)
				{
					short nRowType=kFunctionRow,nFunction=0;
					switch(nID)
					{
						case ID_STYLE_TEXT: nRowType=kTextRow; break;
						case ID_STYLE_CRITERIA: nRowType=kCriteriaRow; break;
						case ID_FUNC_AREA: nFunction=kFunctArea; break;
						case ID_FUNC_COUNT: nFunction=kFunctCount; break;
						case ID_FUNC_HEIGHT: nFunction=kFunctHeight; break;
						case ID_FUNC_LENGTH: nFunction=kFunctLength; break;
						case ID_FUNC_PERIMETER: nFunction=kFunctPerimeter; break;
						case ID_FUNC_WIDTH: nFunction=kFunctWidth; break;
						case ID_FUNC_XDIM: nFunction=kFunctXDim; break;
						case ID_FUNC_YDIM: nFunction=kFunctYDim; break;
						case ID_FUNC_TOTAL_NAME: nFunction=kFunctTotalName; break;
						case ID_FUNC_TOTAL_F2: nFunction=kFunctTotalF2; break;
						case ID_FUNC_TOTAL_F3: nFunction=kFunctTotalF3; break;
						case ID_FUNC_TOTAL_F4: nFunction=kFunctTotalF4; break;
						case ID_FUNC_TOTAL_F5: nFunction=kFunctTotalF5; break;
						default:
							ASSERT(FALSE);
					}
					if(SetRowType(n,nRowType,nFunction))
					{
						GetDocument()->SetModifiedFlag();
						CRect rc;
						GetClientRect(&rc);
						rc.top=y-theCorner.y;
						InvalidateRect(&rc);
					}
				}
			}
		}
		else
		{
			if(CRect(GetAsyncKeyState(VK_SHIFT)<0?0:kMaxRowPopOutWidth,y,kFunctionColWidth,y+m_nRowHeight).PtInRect(point))
			{
				if(::GetAsyncKeyState(VK_SHIFT)<0
					&& nStartRow!=-1
					&& nEndRow!=-1)
				{
					nEndRow=n;
					if(nStartRow<=nEndRow)
					{
						m_nStartSelectedRow=nStartRow;
						m_nEndSelectedRow=nEndRow;
					}
					else
					{
						m_nStartSelectedRow=nEndRow;
						m_nEndSelectedRow=nStartRow;
					}
				}
				else
					m_nEndSelectedRow=m_nStartSelectedRow=nStartRow=n;
				InvalidateSelectedRows();
				UpdateWindow();
				SetCapture();
				MSG msg;
				while(1)
				{
					GetMessage(&msg,NULL,0,0);
					ScreenToClient(&msg.pt);
					msg.pt.x+=theCorner.x;
					msg.pt.y+=theCorner.y;
					switch(msg.message)
					{
						case WM_MOUSEMOVE:
							if(msg.pt.y<kColumnHeaderHeight)
							{
								nEndRow=0;
							}
							else
							{
								nEndRow=(msg.pt.y-kColumnHeaderHeight)/m_nRowHeight;
								if(nEndRow>=m_pReport->m_Rows.GetSize())
									nEndRow=m_pReport->m_Rows.GetSize()-1;
							}
							InvalidateSelectedRows();
							if(nStartRow<=nEndRow)
							{
								m_nStartSelectedRow=nStartRow;
								m_nEndSelectedRow=nEndRow;
							}
							else
							{
								m_nStartSelectedRow=nEndRow;
								m_nEndSelectedRow=nStartRow;
							}
							InvalidateSelectedRows();
							UpdateWindow();
							break;
						case WM_LBUTTONUP:
							ReleaseCapture();
							return;
					}
				}
			}
		}
		return;
	}
	CSize sz=GetTotalSize();
	if(point.x>kFunctionColWidth && point.x<sz.cx && point.y>kColumnHeaderHeight && point.y<sz.cy)
	{
		int row=(point.y-kColumnHeaderHeight)/m_nRowHeight,col;
		int mx=kFunctionColWidth,x;
		CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[row];
		for(col=0;col<6;col++)
		{
			x=mx;
			mx+=m_pReport->m_nColumnWidths[col];
			if(point.x<mx)
				break;
		}
		CRect rc;
		rc.top=kColumnHeaderHeight+row*m_nRowHeight;
		rc.bottom=rc.top+m_nRowHeight;
		rc.left=x;
		rc.right=mx;
		if(pReportRow->m_nRowType==kCriteriaRow)
		{
			if(DoClick(rc,point))
			{
				CMenu theMenu;
				theMenu.LoadMenu(IDR_REPORT_POPUP);
				CPoint pt(rc.left-theCorner.x,rc.bottom-theCorner.y);
				ClientToScreen(&pt);
				CMenu* pMenu=theMenu.GetSubMenu(1);
				UINT nID;
				switch(pReportRow->m_nFieldRefNum[col])
				{
					case kUnDefined: nID=ID_UNDEFINED; break;
					case kNameField: nID=ID_NAME; break;
					case kF2Field: nID=ID_F2; break;
					case kF3Field: nID=ID_F3; break;
					case kF4Field: nID=ID_F4; break;
					case kF5Field: nID=ID_F5; break;
					case kFunctionNameField: nID=ID_FX_NAME; break;
					case kResultField: nID=ID_RESULTS; break;
					default:
						ASSERT(FALSE);
				}
				pMenu->ModifyMenu(ID_NAME,MF_STRING|MF_BYCOMMAND,ID_NAME,GetFieldRefNumName(kNameField));
				pMenu->ModifyMenu(ID_F2,MF_STRING|MF_BYCOMMAND,ID_F2,GetFieldRefNumName(kF2Field));
				pMenu->ModifyMenu(ID_F3,MF_STRING|MF_BYCOMMAND,ID_F3,GetFieldRefNumName(kF3Field));
				pMenu->ModifyMenu(ID_F4,MF_STRING|MF_BYCOMMAND,ID_F4,GetFieldRefNumName(kF4Field));
				pMenu->ModifyMenu(ID_F5,MF_STRING|MF_BYCOMMAND,ID_F5,GetFieldRefNumName(kF5Field));
				pMenu->CheckMenuItem(nID,MF_BYCOMMAND|MF_CHECKED);
				nID=::TrackPopupMenu(pMenu->GetSafeHmenu(),TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTBUTTON|TPM_LEFTALIGN,pt.x,pt.y,0,this->GetSafeHwnd(),NULL);
				theMenu.DestroyMenu();
				if(nID)
				{
					short nFieldRefNum;
					switch(nID)
					{
						case ID_UNDEFINED: nFieldRefNum=kUnDefined; break;
						case ID_NAME: nFieldRefNum=kNameField; break;
						case ID_F2: nFieldRefNum=kF2Field; break;
						case ID_F3: nFieldRefNum=kF3Field; break;
						case ID_F4: nFieldRefNum=kF4Field; break;
						case ID_F5: nFieldRefNum=kF5Field; break;
						case ID_FX_NAME: nFieldRefNum=kFunctionNameField; break;
						case ID_RESULTS: nFieldRefNum=kResultField; break;
						default:
							ASSERT(FALSE);
					}
					if(SetFieldRefNum(row,col,nFieldRefNum))
					{
						GetDocument()->SetModifiedFlag();
						CRect crc;
						GetClientRect(&crc);
						crc.top=rc.top-theCorner.y;
						InvalidateRect(&crc);
					}
				}
			}
			return;
		}

		if(::GetAsyncKeyState(VK_SHIFT)<0
			&& nStartCol!=-1
			&& nEndCol!=-1)
		{
			nEndCol=col;
			if(nStartCol<=nEndCol)
			{
				m_nStartSelectedCol=nStartCol;
				m_nEndSelectedCol=nEndCol;
			}
			else
			{
				m_nStartSelectedCol=nEndCol;
				m_nEndSelectedCol=nStartCol;
			}
		}
		else
			m_nEndSelectedCol=m_nStartSelectedCol=nStartCol=col;

		if(::GetAsyncKeyState(VK_SHIFT)<0
			&& nStartRow!=-1
			&& nEndRow!=-1)
		{
			nEndRow=row;
			if(nStartRow<=nEndRow)
			{
				m_nStartSelectedRow=nStartRow;
				m_nEndSelectedRow=nEndRow;
			}
			else
			{
				m_nStartSelectedRow=nEndRow;
				m_nEndSelectedRow=nStartRow;
			}
		}
		else
			m_nEndSelectedRow=m_nStartSelectedRow=nStartRow=row;

		InvalidateSelectedCols();
		UpdateWindow();
		SetCapture();
		MSG msg;
		while(1)
		{
			GetMessage(&msg,NULL,0,0);
			ScreenToClient(&msg.pt);
			msg.pt.x+=theCorner.x;
			msg.pt.y+=theCorner.y;
			switch(msg.message)
			{
				case WM_MOUSEMOVE:
					{
						int x=kFunctionColWidth-1;
						for(nEndCol=0;nEndCol<6;nEndCol++)
						{
							x+=m_pReport->m_nColumnWidths[nEndCol];
							if(msg.pt.x<x)
								break;
						}
						if(nEndCol>=6)
							nEndCol=5;
					}
					if(msg.pt.y<kColumnHeaderHeight)
					{
						nEndRow=0;
					}
					else
					{
						nEndRow=(msg.pt.y-kColumnHeaderHeight)/m_nRowHeight;
						if(nEndRow>=m_pReport->m_Rows.GetSize())
							nEndRow=m_pReport->m_Rows.GetSize()-1;
					}
					InvalidateSelectedCols();
					if(nStartCol<=nEndCol)
					{
						m_nStartSelectedCol=nStartCol;
						m_nEndSelectedCol=nEndCol;
					}
					else
					{
						m_nStartSelectedCol=nEndCol;
						m_nEndSelectedCol=nStartCol;
					}
					if(nStartRow<=nEndRow)
					{
						m_nStartSelectedRow=nStartRow;
						m_nEndSelectedRow=nEndRow;
					}
					else
					{
						m_nStartSelectedRow=nEndRow;
						m_nEndSelectedRow=nStartRow;
					}
					InvalidateSelectedCols();
					UpdateWindow();
					break;
				case WM_LBUTTONUP:
					ReleaseCapture();
					if(m_nStartSelectedRow==m_nEndSelectedRow && m_nStartSelectedCol==m_nEndSelectedCol)
					{
						InvalidateSelectedCols();
						m_nStartSelectedRow=m_nEndSelectedRow=m_nStartSelectedCol=m_nEndSelectedCol=-1;
						UpdateWindow();
						m_nActiveRow=row;
						m_nActiveCol=col;
						CMDReportRow* pLastCriteriaRow=NULL;
						if(pReportRow->m_nRowType==kFunctionRow)
						{
							while(--row>=0)
							{
								CMDReportRow* pTempRow=(CMDReportRow*)m_pReport->m_Rows[row];
								if(pTempRow->m_nRowType==kCriteriaRow)
								{
									pLastCriteriaRow=pTempRow;
									break;
								}
							}
						}
						if(!pLastCriteriaRow || pLastCriteriaRow->m_nFieldRefNum[col]!=kResultField)
						{
							AdjustInplaceEdit();
							m_wndInplaceEdit.SetWindowText(pReportRow->m_sRowText[col]);
							m_wndInplaceEdit.ShowWindow(SW_SHOW);
							m_wndInplaceEdit.SetFocus();
							m_wndInplaceEdit.SetSel(0,-1);
						}
					}
					return;
			}
		}
		return;
	}
	CScrollView::OnLButtonDown(nFlags, point);
}

void CReportEditView::ShowInplaceEdit()
{
	if(m_wndInplaceEdit.GetSafeHwnd())
	{
		m_wndInplaceEdit.DestroyWindow();
		UpdateWindow();
	}
	int row=m_nActiveRow,col=m_nActiveCol;
	bool bGoodCell=false;
	do
	{
		CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[m_nActiveRow];
		if(pReportRow->m_nRowType==kCriteriaRow)
		{
			if(++m_nActiveRow==m_pReport->m_Rows.GetSize())
				m_nActiveRow=0;
			m_nActiveCol=0;
			if(m_nActiveRow==row && m_nActiveCol==col)
				break;
		}
		else
		{
			CMDReportRow* pLastCriteriaRow=NULL;
			int i=m_nActiveRow;
			while(--i>=0)
			{
				CMDReportRow* pRow=(CMDReportRow*)m_pReport->m_Rows[i];
				if(pRow->m_nRowType==kCriteriaRow)
				{
					pLastCriteriaRow=pRow;
					break;
				}
			}
			if(pReportRow->m_nRowType==kFunctionRow && pLastCriteriaRow && pLastCriteriaRow->m_nFieldRefNum[m_nActiveCol]==kResultField)
			{
				if(++m_nActiveCol==6)
				{
					m_nActiveCol=0;
					if(++m_nActiveRow==m_pReport->m_Rows.GetSize())
						m_nActiveRow=0;
				}
				if(m_nActiveRow==row && m_nActiveCol==col)
					break;
			}
			else
				bGoodCell=true;
		}
	}
	while(!bGoodCell);
	if(bGoodCell)
	{
		AdjustInplaceEdit();
		m_wndInplaceEdit.SetWindowText(((CMDReportRow*)m_pReport->m_Rows[m_nActiveRow])->m_sRowText[m_nActiveCol]);
		m_wndInplaceEdit.ShowWindow(SW_SHOW);
		m_wndInplaceEdit.SetFocus();
		m_wndInplaceEdit.SetSel(0,-1);
	}
}

void CReportEditView::AdjustInplaceEdit()
{
	CRect rc;
	rc.top=kColumnHeaderHeight+m_nActiveRow*m_nRowHeight;
	rc.bottom=rc.top+m_nRowHeight;
	rc.right=kFunctionColWidth;
    int i = 0;
	for(;i<=m_nActiveCol;i++)
	{
		rc.left=rc.right;
		rc.right=rc.left+m_pReport->m_nColumnWidths[i];
	}
	CMDReportRow* pLastCriteriaRow=NULL;
	CMDReportRow* pReportRow;
	i=m_nActiveRow;
	while(--i>=0)
	{
		pReportRow=(CMDReportRow*)m_pReport->m_Rows[i];
		if(pReportRow->m_nRowType==kCriteriaRow)
		{
			pLastCriteriaRow=pReportRow;
			break;
		}
	}
	pReportRow=(CMDReportRow*)m_pReport->m_Rows[m_nActiveRow];
	int nJust=pReportRow->m_nJust[m_nActiveCol];
	if(nJust==kJustRight || nJust==kJustCenter)
	{
		i=m_nActiveCol;
		while(--i>=0 && pReportRow->m_sRowText[i].IsEmpty() && !(pReportRow->m_nRowType==kFunctionRow && pLastCriteriaRow && pLastCriteriaRow->m_nFieldRefNum[i]==kResultField))
		{
			rc.left-=m_pReport->m_nColumnWidths[i];
		}
	}
	if(nJust==kJustLeft || nJust==kJustCenter)
	{
		i=m_nActiveCol;
		while(++i<6 && pReportRow->m_sRowText[i].IsEmpty() && !(pReportRow->m_nRowType==kFunctionRow && pLastCriteriaRow && pLastCriteriaRow->m_nFieldRefNum[i]==kResultField))
		{
			rc.right+=m_pReport->m_nColumnWidths[i];
		}
	}
	rc.right-=1;
	rc.bottom-=1;
	DWORD dwFlags;
	switch(nJust)
	{
		case kJustLeft: dwFlags=ES_LEFT; break;
		case kJustCenter: dwFlags=ES_CENTER; break;
		case kJustRight: dwFlags=ES_RIGHT; break;
		default:
			ASSERT(FALSE);
	}
	CPoint theCorner=GetScrollPosition();
	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.OffsetRect(theCorner);
	CPoint theScroll(0,0);
	if(rc.top<rcClient.top)
	{
		theScroll.y=rc.top-rcClient.top;
	}
	if(rc.left<rcClient.left)
	{
		theScroll.x=rc.left-rcClient.left;
	}
	if(rc.bottom>rcClient.bottom)
	{
		theScroll.y=rc.bottom-rcClient.bottom;
	}
	if(rc.right>rcClient.right)
	{
		theScroll.x=rc.right-rcClient.right;
	}
	if(theScroll.x || theScroll.y)
	{
		theCorner+=theScroll;
		ScrollToPosition(theCorner);
		Invalidate();
		UpdateWindow();
	}
	rc.OffsetRect(-theCorner.x,-theCorner.y);
	m_wndInplaceEdit.Create(WS_CHILD|dwFlags,rc,this,IDC_INPLACEEDIT);
	m_wndInplaceEdit.SetFont(&m_ReportFont);
}

BOOL CReportEditView::DoClick(const CRect &rc,CPoint &pt)
{
	SetCapture();
	MSG msg;
	while(1)
	{
		GetMessage(&msg,NULL,0,0);
		if(msg.message==WM_LBUTTONUP)
			break;
	}
	ScreenToClient(&msg.pt);
	ReleaseCapture();
	pt=GetScrollPosition();
	pt.x+=msg.pt.x;
	pt.y+=msg.pt.y;
	return rc.PtInRect(pt);
}

BOOL CReportEditView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(nHitTest==HTCLIENT)
	{
		CPoint pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);
		CPoint theCorner=GetScrollPosition();
		pt.x+=theCorner.x;
		pt.y+=theCorner.y;
		if(pt.y<kColumnHeaderHeight && pt.x>kFunctionColWidth)
		{
			int x=kFunctionColWidth-1;
			for(int i=0;i<6;i++)
			{
				x+=m_pReport->m_nColumnWidths[i];
				if(abs(pt.x-x)<::GetSystemMetrics(SM_CXDOUBLECLK))
				{
					::SetCursor(AfxGetApp()->LoadCursor(IDC_SPLIT));
					return TRUE;
				}
			}
		}
	}
	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

CString CReportEditView::GetCriteriaName()
{
	CString theName;
	theName.LoadString(IDS_CRITERIA);
	return theName;
}

CString CReportEditView::GetFunctionName(short nFunction)
{
	CString theName;
	if(nFunction<kFunctTotalName)
	{
		theName.LoadString(IDS_FUNCTION_NAME+nFunction-kFirstFunction);
	}
	else
	{
		CString theTemp;
		if(nFunction==kFunctTotalName)
			theTemp.LoadString(IDS_FUNCTION_TOTAL_NAME);
		else
			theTemp=GetDocument()->m_sFieldNames[nFunction-kFunctTotalName];
		theName.Format(IDS_FUNCTION_TOTAL_FORMAT,theTemp);
	}
	return theName;
}

CString CReportEditView::GetFieldRefNumName(short nFieldRefNum)
{
	UINT nID;
	switch(nFieldRefNum)
	{
		case kF2Field:
		case kF3Field:
		case kF4Field:
		case kF5Field:
			return GetDocument()->m_sFieldNames[nFieldRefNum-kNameField];
			break;
		case kUnDefined: nID=IDS_CRITERIA_UNDEFINED; break;
		case kNameField: nID=IDS_CRITERIA_NAME; break;
		case kFunctionNameField: nID=IDS_CRITERIA_FXNAME; break;
		case kResultField: nID=IDS_CRITERIA_RESULTS; break;
		default:
			ASSERT(FALSE);
	}
	CString theName;
	theName.LoadString(nID);
	return theName;
}

bool CReportEditView::SetRowType(int nIndex,short nRowType,short nFunction)
{
	CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[nIndex];
	if(pReportRow->m_nRowType!=nRowType || pReportRow->m_nFunction!=nFunction)
	{
		pReportRow->m_nRowType=nRowType;
		pReportRow->m_nFunction=nFunction;
		int i;
		switch(nRowType)
		{
			case kTextRow:
				break;
			case kCriteriaRow:
				for(i=0;i<5;i++)
					pReportRow->m_nFieldRefNum[i]=kUnDefined;
				pReportRow->m_nFieldRefNum[5]=kResultField;
				for(i=0;i<6;i++)
				{
					pReportRow->m_sRowText[i].Empty();
					pReportRow->m_nJust[i]=kJustLeft;
				}
				break;
			case kFunctionRow:
				while(--nIndex>=0)
				{
					CMDReportRow* pCriteriaRow=(CMDReportRow*)m_pReport->m_Rows[nIndex];
					if(pCriteriaRow->m_nRowType==kCriteriaRow)
					{
						for(i=0;i<6;i++)
							if(pCriteriaRow->m_nFieldRefNum[i]==kFunctionNameField)
								pReportRow->m_sRowText[i]=GetFunctionName(pReportRow->m_nFunction);
						break;
					}
				}
				break;
			default:
				ASSERT(FALSE);
		}
		return true;
	}
	return false;
}

bool CReportEditView::SetFieldRefNum(int row,int col,short nFieldRefNum)
{
	CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[row];
	ASSERT(pReportRow->m_nRowType==kCriteriaRow);
	if(pReportRow->m_nFieldRefNum[col]!=nFieldRefNum)
	{
		pReportRow->m_nFieldRefNum[col]=nFieldRefNum;
		if(nFieldRefNum==kFunctionNameField)
			while(++row<m_pReport->m_Rows.GetSize())
			{
				pReportRow=(CMDReportRow*)m_pReport->m_Rows[row];
				if(pReportRow->m_nRowType==kCriteriaRow)
					break;
				if(pReportRow->m_nRowType==kFunctionRow)
					pReportRow->m_sRowText[col]=GetFunctionName(pReportRow->m_nFunction);
			}
		return true;
	}
	return false;
}

void CReportEditView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(bActivate && m_wndInplaceEdit.GetSafeHwnd())
	{
		m_wndInplaceEdit.SetFocus();
	}
}

void CReportEditView::OnInplaceEditChange()
{
	CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[m_nActiveRow];
	m_wndInplaceEdit.GetWindowText(pReportRow->m_sRowText[m_nActiveCol]);
	GetDocument()->SetModifiedFlag();
}

void CReportEditView::SetTextJustify(int nJust)
{
	if(m_wndInplaceEdit.GetSafeHwnd())
	{
		CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[m_nActiveRow];
		if(pReportRow->m_nJust[m_nActiveCol]!=nJust)
		{
			pReportRow->m_nJust[m_nActiveCol]=nJust;
			GetDocument()->SetModifiedFlag();
			CString theText;
			m_wndInplaceEdit.GetWindowText(theText);
			m_wndInplaceEdit.DestroyWindow();
			AdjustInplaceEdit();
			m_wndInplaceEdit.SetWindowText(theText);
			m_wndInplaceEdit.ShowWindow(SW_SHOW);
			m_wndInplaceEdit.SetFocus();
			m_wndInplaceEdit.SetSel(0,-1);
		}
	}
	else
	{
		bool bSelectedRow=IsSelectedRow(),bSelectedCol=IsSelectedCol();
		if(bSelectedRow && bSelectedCol)
		{
			for(int n=m_nStartSelectedRow;n<=m_nEndSelectedRow;n++)
			{
				CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[n];
				if(pReportRow->m_nRowType!=kCriteriaRow)
					for(int i=m_nStartSelectedCol;i<=m_nEndSelectedCol;i++)
						pReportRow->m_nJust[i]=nJust;
			}
			GetDocument()->SetModifiedFlag();
			Invalidate();
			UpdateWindow();
		}
		else if(bSelectedRow)
		{
			for(int n=m_nStartSelectedRow;n<=m_nEndSelectedRow;n++)
			{
				CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[n];
				if(pReportRow->m_nRowType!=kCriteriaRow)
					for(int i=0;i<6;i++)
						pReportRow->m_nJust[i]=nJust;
			}
			GetDocument()->SetModifiedFlag();
			InvalidateSelectedRows();
			UpdateWindow();
		}
		else if(bSelectedCol)
		{
			for(int n=0;n<m_pReport->m_Rows.GetSize();n++)
			{
				CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[n];
				if(pReportRow->m_nRowType!=kCriteriaRow)
					for(int i=m_nStartSelectedCol;i<=m_nEndSelectedCol;i++)
						pReportRow->m_nJust[i]=nJust;
			}
			GetDocument()->SetModifiedFlag();
			Invalidate();
			UpdateWindow();
		}
	}
}

void CReportEditView::OnTextJustifyCenter() 
{
	SetTextJustify(kJustCenter);
}

void CReportEditView::OnTextJustifyLeft() 
{
	SetTextJustify(kJustLeft);
}

void CReportEditView::OnTextJustifyRight() 
{
	SetTextJustify(kJustRight);
}

int CReportEditView::GetSelectedRowJustify()
{
	int nJust=-1;
	for(int n=m_nStartSelectedRow;n<=m_nEndSelectedRow;n++)
	{
		CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[n];
		if(pReportRow->m_nRowType!=kCriteriaRow)
			for(int i=0;i<6;i++)
				if(nJust!=pReportRow->m_nJust[i])
				{
					if(nJust!=-1)
						return -1;
					nJust=pReportRow->m_nJust[i];
				}
	}
	return nJust;
}

void CReportEditView::OnUpdateTextJustify(CCmdUI* pCmdUI) 
{
	int nJust;
	if(m_wndInplaceEdit.GetSafeHwnd())
	{
		CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[m_nActiveRow];
		nJust=pReportRow->m_nJust[m_nActiveCol];
	}
	else
	{
		bool bSelectedRow=IsSelectedRow(),bSelectedCol=IsSelectedCol();
		if(bSelectedRow && bSelectedCol)
			nJust=GetSelectedRowColJustify();
		else if(bSelectedRow)
			nJust=GetSelectedRowJustify();
		else if(bSelectedCol)
			nJust=GetSelectedColJustify();
		else
		{
			pCmdUI->Enable(FALSE);
			return;
		}
	}
	pCmdUI->Enable(TRUE);
	BOOL bOn;
	switch(pCmdUI->m_nID)
	{
		case ID_TEXT_JUSTIFY_LEFT: bOn=nJust==kJustLeft; break;
		case ID_TEXT_JUSTIFY_CENTER: bOn=nJust==kJustCenter; break;
		case ID_TEXT_JUSTIFY_RIGHT: bOn=nJust==kJustRight; break;
		default:
			ASSERT(FALSE);
	}
	pCmdUI->SetCheck(bOn);
}

void CReportEditView::OnTextFont() 
{
	if(m_wndInplaceEdit.GetSafeHwnd())
		m_wndInplaceEdit.DestroyWindow();
	LOGFONT lf;
	memcpy(&lf,&m_pReport->m_LogFont,sizeof(LOGFONT));
	CFontDialog theFontDlg(&lf,CF_SCREENFONTS|CF_TTONLY|CF_EFFECTS|CF_ENABLETEMPLATE);
	theFontDlg.m_cf.lpTemplateName=MAKEINTRESOURCE(IDD_CHOOSE_FONT);
	theFontDlg.m_cf.hInstance=AfxGetResourceHandle();
	if(theFontDlg.DoModal()==IDOK)
	{
		memcpy(&m_pReport->m_LogFont,&lf,sizeof(LOGFONT));
		m_ReportFont.DeleteObject();
		CreateReportFont();
		SetReportSize();
		Invalidate();
	}
}

void CReportEditView::OnEditCopy() 
{
	if(m_wndInplaceEdit.GetSafeHwnd())
	{
		m_wndInplaceEdit.Copy();
	}
	else
	{
	}
}

void CReportEditView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	BOOL bEnable;
	if(m_wndInplaceEdit.GetSafeHwnd())
	{
		int nStartChar, nEndChar;
		m_wndInplaceEdit.GetSel(nStartChar,nEndChar);
		bEnable=nStartChar!=nEndChar;
	}
	else
	{
		bEnable=false;
	}
	pCmdUI->Enable(bEnable);
}

void CReportEditView::OnEditCut() 
{
	if(m_wndInplaceEdit.GetSafeHwnd())
	{
		m_wndInplaceEdit.Cut();
	}
	else
	{
	}
}

void CReportEditView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	BOOL bEnable;
	if(m_wndInplaceEdit.GetSafeHwnd())
	{
		int nStartChar, nEndChar;
		m_wndInplaceEdit.GetSel(nStartChar,nEndChar);
		bEnable=nStartChar!=nEndChar;
	}
	else
	{
		bEnable=false;
	}
	pCmdUI->Enable(bEnable);
}

void CReportEditView::OnEditPaste() 
{
	if(m_wndInplaceEdit.GetSafeHwnd())
	{
		m_wndInplaceEdit.Paste();
	}
	else
	{
	}
}

void CReportEditView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	BOOL bEnable;
	if(m_wndInplaceEdit.GetSafeHwnd())
	{
		bEnable=::IsClipboardFormatAvailable(CF_TEXT);
	}
	else
	{
		bEnable=false;
	}
	pCmdUI->Enable(bEnable);
}

void CReportEditView::OnEditUndo() 
{
	if(m_wndInplaceEdit.GetSafeHwnd())
	{
		m_wndInplaceEdit.Undo();
	}
	else
	{
	}
}

void CReportEditView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	BOOL bEnable;
	if(m_wndInplaceEdit.GetSafeHwnd())
	{
		bEnable=m_wndInplaceEdit.CanUndo();
	}
	else
	{
		bEnable=false;
	}
	pCmdUI->Enable(bEnable);
}

void CReportEditView::OnEditDeleteRow() 
{
	for(int n=m_nStartSelectedRow;n<=m_nEndSelectedRow && m_pReport->m_Rows.GetSize()>1;n++)
	{
		delete m_pReport->m_Rows.GetAt(m_nStartSelectedRow);
		m_pReport->m_Rows.RemoveAt(m_nStartSelectedRow);
	}
	while(m_nEndSelectedRow>=m_pReport->m_Rows.GetSize())
	{
		if(m_nStartSelectedRow>0)
			--m_nStartSelectedRow;
		--m_nEndSelectedRow;
	}
	GetDocument()->SetModifiedFlag();
	SetReportSize();
	Invalidate();
}

void CReportEditView::OnUpdateEditDeleteRow(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsSelectedRow() && m_pReport->m_Rows.GetSize()>1);
}

void CReportEditView::OnEditInsertRow() 
{
	if(IsSelectedRow())
	{
		for(int n=m_nEndSelectedRow-m_nStartSelectedRow+1;n>0;--n)
		{
			m_pReport->m_Rows.InsertAt(m_nStartSelectedRow,new CMDReportRow);
			++m_nStartSelectedRow;
			++m_nEndSelectedRow;
		}
	}
	else m_pReport->m_Rows.Add(new CMDReportRow);
	GetDocument()->SetModifiedFlag();
	SetReportSize();
	Invalidate();
}

void CReportEditView::OnFileExportReport()
{
#ifndef	_DEMO
	CString strFilter;
	strFilter.LoadString(IDS_EXPORTREPORTFILTERS);
	CFileDialog theDlg(FALSE,"txt",m_pReport->m_sTitle,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strFilter);
	if(theDlg.DoModal()==IDOK)
	{
		CWaitCursor wait;
		m_pReport->Calculate(GetDocument());
        std::ostrstream os;
		CMDReportRow* pLastCriteriaRow=NULL;
		for(int i=0;i<m_pReport->m_Rows.GetSize();i++)
		{
			CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[i];
			if(pReportRow->m_nRowType==kCriteriaRow)
			{
				pLastCriteriaRow=pReportRow;
			}
			else
			{
				for(int n=0;n<6;n++)
				{
					os << pReportRow->m_sRowText[n];
					if(n<5) os << '\t';
				}
				os << "\r\n";
			}
		}
		TRY
		{
			CFile file(theDlg.GetPathName(),CFile::typeBinary|CFile::modeWrite|CFile::modeCreate);
			file.Write(os.str(),os.pcount());
			os.rdbuf()->freeze(0);
			file.Close();
		}
		CATCH(CFileException,e)
		{
			e->ReportError();
			CFile::Remove(theDlg.GetPathName());
		}
		END_CATCH
	}
#endif
}

CScrollBar* CReportEditView::GetScrollBarCtrl(int nBar) const
{
	CReportEditFrame* pFrame=(CReportEditFrame*)GetParentFrame();
	return pFrame->GetScrollBar(nBar);
}


void CReportEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
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

void CReportEditView::DoSelInfo()
{
	CPCDraftDoc* pDoc=GetDocument();
	POSITION pos=pDoc->GetFirstViewPosition();
	while(pos!=NULL)
	{
		CView* pView=pDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
		{
			pView->GetParentFrame()->ActivateFrame();
			((CPCDraftView*)pView)->DoShapeSelect(this);
			break;
		}
	}   
}

void CReportEditView::ShapeSelected(CMDShape* pShape)
{
	CString sValues[5];
	pShape->GetValues(sValues);
	if(sValues[0].IsEmpty() && sValues[1].IsEmpty() && sValues[2].IsEmpty() && sValues[3].IsEmpty() && sValues[4].IsEmpty())
	{
		AfxMessageBox(IDS_INVALID_SHAPE,MB_ICONSTOP|MB_OK);
		return;
	}
	GetParentFrame()->ActivateFrame();
	int nRow;
	if(IsSelectedRow())
	{
		nRow=m_nStartSelectedRow;
	}
	else if(m_wndInplaceEdit.GetSafeHwnd())
	{
		nRow=m_nActiveRow;
		m_wndInplaceEdit.DestroyWindow();
		UpdateWindow();
	}
	else
		return;
	for(int i=nRow;--i>=0;)
	{
		CMDReportRow* pCriteriaRow=(CMDReportRow*)m_pReport->m_Rows[i];
		if(pCriteriaRow->m_nRowType==kCriteriaRow)
		{
			CMDReportRow* pTargetRow=(CMDReportRow*)m_pReport->m_Rows[nRow];
			CString theValues[5];
			pShape->GetValues(theValues);
			for(int n=0;n<6;n++)
			{
				short nFieldRefNum=pCriteriaRow->m_nFieldRefNum[n];
				switch(nFieldRefNum)
				{
					case kNameField:
					case kF2Field:
					case kF3Field:
					case kF4Field:
					case kF5Field:
						pTargetRow->m_sRowText[n]=theValues[nFieldRefNum-1];
						break;
				}
			}
			Invalidate();
			break;
		}
	}
}

bool CReportEditView::IsSelectedRow(int nRow)
{
	if(m_nStartSelectedRow==-1 && m_nEndSelectedRow==-1)
		return false;
	if(nRow==-1)
		return true;
	return m_nStartSelectedRow<=nRow && nRow<=m_nEndSelectedRow;
}

void CReportEditView::InvalidateSelectedCols()
{
	CPoint pt=GetScrollPosition();
	CRect rc;
	GetClientRect(&rc);
	int x=kFunctionColWidth-pt.x;
	for(int n=0;n<6;n++)
	{
		if(n==m_nStartSelectedCol)
			rc.left=x;
		x+=m_pReport->m_nColumnWidths[n];
		if(n==m_nEndSelectedCol)
			break;
	}
	rc.right=x;
	InvalidateRect(&rc);
}

bool CReportEditView::IsSelectedCol(int nCol)
{
	if(m_nStartSelectedCol==-1 && m_nEndSelectedCol==-1)
		return false;
	if(nCol==-1)
		return true;
	return m_nStartSelectedCol<=nCol && nCol<=m_nEndSelectedCol;
}

int CReportEditView::GetSelectedColJustify()
{
	int nJust=-1;
	for(int n=0;n<m_pReport->m_Rows.GetSize();n++)
	{
		CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[n];
		if(pReportRow->m_nRowType!=kCriteriaRow)
			for(int i=m_nStartSelectedCol;i<=m_nEndSelectedCol;i++)
				if(nJust!=pReportRow->m_nJust[i])
				{
					if(nJust!=-1)
						return -1;
					nJust=pReportRow->m_nJust[i];
				}
	}
	return nJust;
}

int CReportEditView::GetSelectedRowColJustify()
{
	int nJust=-1;
	for(int n=m_nStartSelectedRow;n<=m_nEndSelectedRow;n++)
	{
		CMDReportRow* pReportRow=(CMDReportRow*)m_pReport->m_Rows[n];
		if(pReportRow->m_nRowType!=kCriteriaRow)
			for(int i=m_nStartSelectedCol;i<=m_nEndSelectedCol;i++)
				if(nJust!=pReportRow->m_nJust[i])
				{
					if(nJust!=-1)
						return -1;
					nJust=pReportRow->m_nJust[i];
				}
	}
	return nJust;
}

BOOL CReportEditView::OnEraseBkgnd(CDC* pDC) 
{
	CRect rc;
	GetClientRect(rc);
	rc.left=m_totalDev.cx;
	CGDIBackColor theBackColor(pDC,::GetSysColor(COLOR_WINDOW));
	if(!rc.IsRectEmpty())
		pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);    // vertical strip along the side
	rc.left=0;
	rc.right=m_totalDev.cx;
	rc.top=m_totalDev.cy;
	if(!rc.IsRectEmpty())
		pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);    // horizontal strip along the bottom
	return TRUE;
}

void CReportEditView::CreateReportFont()
{
	LOGFONT lfScaled=m_pReport->m_LogFont;
	lfScaled.lfHeight=MulDiv(lfScaled.lfHeight,72,/*m_pDC->GetDeviceCaps(LOGPIXELSY)*/96);
	m_ReportFont.CreateFontIndirect(&lfScaled);
}

BOOL CReportEditView::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_TAB || pMsg->wParam==VK_RETURN) && GetAsyncKeyState(VK_CONTROL)>=0 && GetAsyncKeyState(VK_MENU)>=0 && GetAsyncKeyState(VK_SHIFT)>=0)
	{
		if(m_wndInplaceEdit.GetSafeHwnd())
		{
			if(++m_nActiveCol==6)
			{
				m_nActiveCol=0;
				if(++m_nActiveRow==m_pReport->m_Rows.GetSize())
					m_nActiveRow=0;
			}
		}
		else
		{
			m_nActiveRow=0;
			m_nActiveCol=0;
		}
		ShowInplaceEdit();
		return TRUE;
	}
	return CScrollView::PreTranslateMessage(pMsg);
}

void CReportEditView::OnUpdateDataFindReplace(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);	
}

void CReportEditView::OnUpdateLayoutLayerSetup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);	
}

void CReportEditView::OnFileClose() 
{
	GetParentFrame()->DestroyWindow();
}

void CReportEditView::OnUpdateFileExportReport(CCmdUI* pCmdUI) 
{
#ifdef	_DEMO
	pCmdUI->Enable(FALSE);
#else
	pCmdUI->Enable(!((CPCDraftApp*)AfxGetApp())->m_bDemoMode);
#endif
}
