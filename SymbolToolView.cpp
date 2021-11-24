// SymbolToolView.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "SymbolLibraryDoc.h"
#include "SymbolToolView.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString	LoadString(UINT nID);

/////////////////////////////////////////////////////////////////////////////
// CSymbolToolView

IMPLEMENT_DYNCREATE(CSymbolToolView, CFormView)

CSymbolToolView::CSymbolToolView()
	: CFormView(CSymbolToolView::IDD)
{
	//{{AFX_DATA_INIT(CSymbolToolView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSymbolToolView::~CSymbolToolView()
{
}

void CSymbolToolView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSymbolToolView)
	DDX_Control(pDX, IDC_UNSCALED, m_Unscaled);
	DDX_Control(pDX, IDC_UNITS_SCALE, m_ScaleUnits);
	DDX_Control(pDX, IDC_PLACE, m_Place);
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSymbolToolView, CFormView)
	//{{AFX_MSG_MAP(CSymbolToolView)
	ON_BN_CLICKED(IDC_RESIZE_MORE, OnResizeMore)
	ON_CBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_PLACE, OnPlace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSymbolToolView diagnostics

#ifdef _DEBUG
void CSymbolToolView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSymbolToolView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSymbolToolView message handlers

void CSymbolToolView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();
	m_List.ResetContent();
	CSymbolLibraryDoc* pDoc=GetDocument();
	int nActive=0;
	for(int i=0;i<pDoc->m_Shapes.GetSize();i++)
	{
		CSymLibEntry* pEntry=pDoc->m_Shapes.GetAt(i);
		CString sName,sF2,sF3,sF4,sF5;
		pEntry->m_pShape->GetValues(sName,sF2,sF3,sF4,sF5);
		int n=m_List.AddString(sName);
		if(pEntry==GetDocument()->m_pActiveEntry)
			nActive=n;
		m_List.SetItemData(n,i);
	}
	bool bEnabled;
	if(pDoc->m_Shapes.GetSize())
	{
		m_List.SetCurSel(nActive);
		bEnabled=true;
	}
	else
	{
		bEnabled=false;
	}
	OnSelchangeList();
	m_Place.EnableWindow(bEnabled);
}

void CSymbolToolView::OnResizeMore() 
{
	CFrameWnd* pNewFrame=((CPCDraftApp*)AfxGetApp())->m_pSymbolDocTemplate->CreateNewFrame(GetDocument(),NULL);
	if(pNewFrame)
	{
		CRect rc;
		GetParentFrame()->GetWindowRect(&rc);
		pNewFrame->GetParent()->ScreenToClient(&rc);
		pNewFrame->SetWindowPos(NULL,rc.left,rc.top,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
		pNewFrame->InitialUpdateFrame(GetDocument(),TRUE);
		GetParentFrame()->DestroyWindow();
	}
}

CSymbolLibraryDoc* CSymbolToolView::GetDocument()
{
	return (CSymbolLibraryDoc*)CFormView::GetDocument();
}

void CSymbolToolView::OnSelchangeList() 
{
	int n=m_List.GetCurSel();
	if(n>=0)
	{
		CSymLibEntry* pEntry=GetDocument()->m_Shapes.GetAt(m_List.GetItemData(n));
		GetDocument()->m_pActiveEntry=pEntry;
		CString theScaleUnits;
		theScaleUnits.Format("%s %s",LoadString(pEntry->m_nDisplayRes==kMetRes?IDS_METRIC_UNITS:IDS_ENGLISH_UNITS),LoadString((pEntry->m_nDisplayRes==kMetRes?IDS_METRIC_SCALE_FIRST:IDS_ENGLISH_SCALE_FIRST)+pEntry->m_nScale-1));
		m_ScaleUnits.SetWindowText(theScaleUnits);
		GetDlgItem(IDC_UNSCALED)->EnableWindow(TRUE);
	}
	else
	{
		m_ScaleUnits.SetWindowText("");
		GetDlgItem(IDC_UNSCALED)->EnableWindow(FALSE);
	}
}

void CSymbolToolView::OnPlace() 
{
	CMDIFrameWnd* pMainFrame=(CMDIFrameWnd*)AfxGetMainWnd();
	CFrameWnd* pFrame=pMainFrame->GetActiveFrame();
	if(pFrame)
	{
		CView* pView=pFrame->GetActiveView();
		if(pView && pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
		{
			int n=m_List.GetCurSel();
			ASSERT(n>=0);
			CSymLibEntry* pEntry=GetDocument()->m_Shapes.GetAt(m_List.GetItemData(n));
			CMDShape* pShape=pEntry->m_pShape;
			CMemFile mf;
			CArchive ar(&mf,CArchive::store);
			ar << pEntry->m_nDisplayRes << pEntry->m_nScale;
			
			LongPoint lptOrigin=pShape->GetShapeDatum();
			ar << lptOrigin;
			// by TSM
			ar << int(0);
			// end by TSM
			ar << int(1);	// count
			ar << pShape;
			ar.Close();
			DWORD len=mf.GetLength();
			BYTE* data=mf.Detach();
			HANDLE hClip=GlobalAlloc(GMEM_DDESHARE,len);
			CopyMemory(GlobalLock(hClip),data,len);
			GlobalUnlock(hClip);
			free(data);
			((CPCDraftView*)pView)->DoPlace(hClip,m_Unscaled.GetCheck()?true:false);
		}
	}
}
