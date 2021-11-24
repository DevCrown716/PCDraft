// AttribPalette.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "AttribPalette.h"
#include "CustomMenus.h"
#include "CustomMiniDockFrameWnd.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CAttribPalette

CAttribPalette::CAttribPalette():
	m_wndFill(IDB_BUCKET,IDB_BUCKET,IDB_BUCKET_MASK,IDB_BUCKET_MASK, NULL), // Interface Update - Infinisys Ltd.
   m_wndPFill(IDB_PFILL,IDB_PFILL,IDB_PFILL_MASK,IDB_PFILL_MASK,NULL), // Interface Update - Infinisys Ltd.
   m_wndPen(IDB_PEN,IDB_PEN,IDB_PEN_MASK,IDB_PEN_MASK2,NULL), // Interface Update - Infinisys Ltd.
   m_wndFillTransparency(&m_editFill), // Opacity/Transparency - Infinisys Ltd
   m_wndPFillTransparency(&m_editPFill), // Opacity/Transparency - Infinisys Ltd
   m_wndPenTransparency(&m_editPen) // Opacity/Transparency - Infinisys Ltd,
{
   m_pview = NULL;
	m_bShowParallel=true;
}

CAttribPalette::~CAttribPalette()
{
}


// Interface Update - Infinisys Ltd.
LRESULT CAttribPalette::WindowProc(UINT nMsg,WPARAM wParam,LPARAM lParam)
{
   if(nMsg == WM_CTLCOLORSTATIC || nMsg == WM_CTLCOLORDLG || nMsg == 25 || nMsg == WM_CTLCOLOREDIT || nMsg == WM_ERASEBKGND || nMsg == WM_PRINTCLIENT)
   {
   }
   else if(nMsg == WM_NCPAINT)
   {
   }
   else if(nMsg == WM_PAINT)
   {
   }
   else if(nMsg == WM_COMMAND)
   {
   }
   else if(nMsg == WM_WINDOWPOSCHANGING)
   {
   }
   else if(nMsg == WM_SETCURSOR)
   {
   }
   else if(nMsg == WM_NCHITTEST)
   {
   }
   else if(nMsg == WM_MOUSEMOVE)
   {
   }
   else if(nMsg == 0x363)
   {
   }
   else if(nMsg == WM_CAPTURECHANGED)
   {
   }
   else
   {
      if(nMsg == WM_LBUTTONDOWN)
      {
         TRACE("xxx");
      }
      if(nMsg == WM_WINDOWPOSCHANGED)
      {
         TRACE("yyy");
      }
   }
   return CDialogBar::WindowProc(nMsg,wParam,lParam);
}

BEGIN_MESSAGE_MAP(CAttribPalette, CDialogBar)
	//{{AFX_MSG_MAP(CAttribPalette)
	//}}AFX_MSG_MAP
   ON_REGISTERED_MESSAGE(UM_MAXIMIZE_CLICK,OnMaximizeClick)
   ON_COMMAND(IDOK,&CAttribPalette::OnOK)
   ON_WM_NCLBUTTONDOWN() // Interface Update - Infinisys Ltd.
   ON_WM_PAINT() // Interface Update - Infinisys Ltd.
   ON_WM_NCPAINT() // Interface Update - Infinisys Ltd.
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttribPalette message handlers

BOOL CAttribPalette::Create(CWnd *pParentWnd)
{
	BOOL bResult=CDialogBar::Create(pParentWnd,IDD_ATTRIB_PALETTE,CBRS_NOALIGN|CBRS_SIZE_FIXED|CBRS_FLOATING,IDR_ATTRIB_PALETTE)
					&& m_wndFill.SubclassDlgItem(ID_TOOL_ATTRIB_FILL,this)
					&& m_wndPFill.SubclassDlgItem(ID_TOOL_ATTRIB_PFILL,this)
					&& m_wndPen.SubclassDlgItem(ID_TOOL_ATTRIB_PEN,this)
               && m_wndFillTransparency.SubclassDlgItem(ID_TOOL_ATTRIB_FILL_TRANSPARENCY,this) // Opacity/Transparency - Infinisys Ltd
               && m_wndPFillTransparency.SubclassDlgItem(ID_TOOL_ATTRIB_PFILL_TRANSPARENCY,this) // Opacity/Transparency - Infinisys Ltd
               && m_wndPenTransparency.SubclassDlgItem(ID_TOOL_ATTRIB_PEN_TRANSPARENCY,this) // Opacity/Transparency - Infinisys Ltd
               && m_wndLineWidth.SubclassDlgItem(ID_TOOL_ATTRIB_LINE_WEIGTH,this)
					&& m_wndLineStyle.SubclassDlgItem(ID_TOOL_ATTRIB_LINE_STYLE,this)
               && m_wndEndmarks.SubclassDlgItem(ID_TOOL_ATTRIB_END_MARK, this)
               && m_editFill.SubclassDlgItem(IDC_EDIT_TRANSPARENCY_FILL,this) // Opacity/Transparency - Infinisys Ltd
               && m_editPFill.SubclassDlgItem(IDC_EDIT_TRANSPARENCY_PFILL,this) // Opacity/Transparency - Infinisys Ltd
               && m_editPen.SubclassDlgItem(IDC_EDIT_TRANSPARENCY_PEN,this) // Opacity/Transparency - Infinisys Ltd
               && m_wndSep1.SubclassDlgItem(IDC_STATIC_SEP1,this) // Opacity/Transparency - Infinisys Ltd
               && m_wndSep2.SubclassDlgItem(IDC_STATIC_SEP2,this) // Opacity/Transparency - Infinisys Ltd
               && m_wndSep3.SubclassDlgItem(IDC_STATIC_SEP3,this); // Opacity/Transparency - Infinisys Ltd
   // Opacity/Transparency - Infinisys Ltd
   {
      CRect r;
      m_wndSep1.GetWindowRect(r);
      m_wndSep1.SetWindowPos(NULL,0,0,r.Width(),12, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
      m_wndSep2.SetWindowPos(NULL,0,0,r.Width(),12,SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
      m_wndSep3.SetWindowPos(NULL,0,0,r.Width(),12,SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
   }
   if(bResult)
	{
		CToolTipCtrl* tips=((CMainFrame*)pParentWnd)->GetToolTip();

		tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_FILL_COLOR));
		tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_FILL_PATTERN));
      tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_FILL_TRANSPARENCY)); // Opacity/Transparency - Infinisys Ltd
		tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_PFILL_COLOR));
		tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_PFILL_PATTERN));
      tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_PFILL_TRANSPARENCY)); // Opacity/Transparency - Infinisys Ltd
		tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_PEN_COLOR));
		tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_PEN_PATTERN));
      tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_PEN_TRANSPARENCY)); // Opacity/Transparency - Infinisys Ltd
		tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_LINE_STYLE));
		tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_LINE_WEIGTH));
		tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_END_MARK));

		SetBorders(1,1,1,1);
		EnableDocking(0);
	}

	return bResult;
}

void CAttribPalette::OffsetWindow(UINT nID,int nDelta)
{
	CRect rc;
	CWnd* pWnd=GetDlgItem(nID);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	pWnd->SetWindowPos(NULL,rc.left,rc.top+nDelta,0,0,SWP_NOSIZE|SWP_NOZORDER);
}

// Interface Update - Infinisys Ltd.
//static bool g_bRegister = false;
//
//BOOL CAttribPalette::PreCreateWindow(CREATESTRUCT & cs)
//{
//   if(!CDialogBar::PreCreateWindow(cs))
//      return FALSE;
//
//   if(!g_bRegister)
//   {
//
//      g_bRegister = true;
//
//
//      LONG fRegisteredClasses = 0;
//
//      // common initialization
//      WNDCLASS wndcls;
//      memset(&wndcls,0,sizeof(WNDCLASS));   // start with NULL defaults
//      wndcls.lpfnWndProc =::DefWindowProcA;
//      wndcls.hInstance = AfxGetInstanceHandle();
//      wndcls.hCursor = ::LoadCursor(NULL,IDC_ARROW);
//
//      INITCOMMONCONTROLSEX init;
//      init.dwSize = sizeof(init);
//
//      wndcls.style = CS_HREDRAW | CS_VREDRAW;;   // control bars don't handle double click
//      wndcls.lpszClassName = "PCDRAFT_WINDOW_CONTROL_BAR";
//      //wndcls.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
//      AfxRegisterClass(&wndcls);
//
//   }
//
//
//   cs.lpszClass = "PCDRAFT_WINDOW_CONTROL_BAR";
//
//   return TRUE;
//}

// Interface Update - Infinisys Ltd.
void CAttribPalette::OnNcLButtonDown(UINT i, CPoint pt)
{


   CDialogBar::OnNcLButtonDown(i,pt);

   //return 0;

}

// Interface Update - Infinisys Ltd.
void CAttribPalette::OnPaint()
{
   Default();
}
// Interface Update - Infinisys Ltd.
void CAttribPalette::OnNcPaint()
{
   Default();
}


void CAttribPalette::ShowHideParallel()
{
	static UINT gShowHide[] = { ID_TOOL_ATTRIB_PFILL,ID_TOOL_ATTRIB_PFILL_COLOR,ID_TOOL_ATTRIB_PFILL_PATTERN,0 };
	static UINT gOffset[] = { ID_TOOL_ATTRIB_PEN,ID_TOOL_ATTRIB_PEN_COLOR,ID_TOOL_ATTRIB_PEN_PATTERN,ID_TOOL_ATTRIB_LINE_WEIGTH,ID_TOOL_ATTRIB_LINE_STYLE,ID_TOOL_ATTRIB_END_MARK,0 };
	int nDelta;
	if(m_bShowParallel)
	{
		nDelta=-21;
		m_bShowParallel=false;
	}
	else
	{
		nDelta=21;
		m_bShowParallel=true;
	}
	for(int i=0;gShowHide[i];i++)
		GetDlgItem(gShowHide[i])->ShowWindow(m_bShowParallel?SW_SHOW:SW_HIDE);
	for(int i=0;gOffset[i];i++)
		OffsetWindow(gOffset[i],nDelta);
	m_sizeDefault.cy+=nDelta;
}

LRESULT CAttribPalette::OnMaximizeClick(WPARAM wParam,LPARAM lParam)
{
	((CCustomMiniDockFrameWnd*)GetParentFrame())->InvertMaximized();
	ShowHideParallel();
	GetParentFrame()->RecalcLayout();
	return 0;
}


// Opacity/Transparency - Infinisys Ltd
void CAttribPalette::OnOK()
{

   CEdit * pedit = (CEdit *) GetFocus();

   CString strText;

   pedit->GetWindowText(strText);

   int iPercentage = atoi(strText);

   int iOpacity = (int) round((double) iPercentage * 255.0 / 100.0);

   if(pedit == &m_editFill)
   {

      m_pview->ApplyOpacity(iOpacity,false, true);

   }
   else if(pedit == &m_editPFill)
   {

      m_pview->ApplyOpacity(iOpacity,true, true);

   }
   else if(pedit == &m_editPen)
   {

      PenModel pm;
      pm.m_iPenOpacity = iOpacity;
      m_pview->ApplyPen(pm,pmUseOpacity);

   }


}

