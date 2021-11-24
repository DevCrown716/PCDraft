// AttribPalette.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "RibbonProperties.h"
#include "CustomMenus.h"
#include "CustomMiniDockFrameWnd.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MainFrm.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CRibbonProperties

CRibbonProperties::CRibbonProperties(CRibbonBar * pbar):
m_pbar(pbar),
m_brush(::CreateSolidBrush(RGB(250,250,250))),
m_wndFillIcon(IDB_BUCKET,IDB_BUCKET,IDB_BUCKET_MASK,IDB_BUCKET_MASK,RGB(250,250,250)),
m_wndPFillIcon(IDB_PFILL,IDB_PFILL,IDB_PFILL_MASK,IDB_PFILL_MASK,RGB(250,250,250)),
m_wndPenIcon(IDB_PEN,IDB_PEN,IDB_PEN_MASK,IDB_PEN_MASK2,RGB(250,250,250)),
m_wndFill(IDB_COLOR,IDB_COLOR,IDB_COLOR_MASK,IDB_COLOR_MASK,RGB(250,250,250)),
m_wndPFill(IDB_COLOR,IDB_COLOR,IDB_COLOR_MASK,IDB_COLOR_MASK,RGB(250,250,250)),
m_wndPen(IDB_COLOR,IDB_COLOR,IDB_COLOR_MASK,IDB_COLOR_MASK,RGB(250,250,250)),
m_wndFillTransparency(IDB_ALPHA,IDB_ALPHA,IDB_ALPHA_MASK,IDB_ALPHA_MASK,RGB(250,250,250), true),
m_wndPFillTransparency(IDB_ALPHA,IDB_ALPHA,IDB_ALPHA_MASK,IDB_ALPHA_MASK,RGB(250,250,250), true),
m_wndPenTransparency(IDB_ALPHA,IDB_ALPHA,IDB_ALPHA_MASK,IDB_ALPHA_MASK,RGB(250,250,250), true),
m_wndLineStyle(1),
m_font2(L"Segoe UI",13.0,0,Gdiplus::UnitPixel)
{

   LOGFONTW lfw;

   {

      HDC hdc = ::GetDC(NULL);

      {

         Gdiplus::Graphics g(hdc);

         m_font2.GetLogFontW(&g,&lfw);

         m_font.Attach(::CreateFontIndirectW(&lfw));

      }

      ::ReleaseDC(NULL,hdc);

   }
   m_pview = NULL;
   m_bShowParallel=true;
   
}

CRibbonProperties::~CRibbonProperties()
{
   ::DeleteObject(m_brush);
}

LRESULT CRibbonProperties::WindowProc(UINT nMsg,WPARAM wParam,LPARAM lParam)
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

   LRESULT lResult;
   switch(nMsg)
   {
   case WM_NOTIFY:
   case WM_COMMAND:
   case WM_DRAWITEM:
   case WM_MEASUREITEM:
   case WM_DELETEITEM:
   case WM_COMPAREITEM:
   case WM_VKEYTOITEM:
   case WM_CHARTOITEM:
      // send these messages to the owner if not handled
      if(OnWndMsg(nMsg,wParam,lParam,&lResult))
         return lResult;
      else
      {

         lResult = GetOwner()->SendMessage(nMsg,wParam,lParam);

         // special case for TTN_NEEDTEXTA and TTN_NEEDTEXTW
         if(nMsg == WM_NOTIFY)
         {
            NMHDR* pNMHDR = (NMHDR*)lParam;
            if(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW)
            {
               TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
               TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

               if(pNMHDR->code == TTN_NEEDTEXTA)
               {
                  if(pTTTA->hinst == 0 && (!pTTTA->lpszText || !*pTTTA->lpszText))
                  {
                     // not handled by owner, so let bar itself handle it
                     lResult = CWnd::WindowProc(nMsg,wParam,lParam);
                  }
               }
               else if(pNMHDR->code == TTN_NEEDTEXTW)
               {
                  if(pTTTW->hinst == 0 && (!pTTTW->lpszText || !*pTTTW->lpszText))
                  {
                     // not handled by owner, so let bar itself handle it
                     lResult = CWnd::WindowProc(nMsg,wParam,lParam);
                  }
               }
            }
         }
         return lResult;
      }
   }
   return CDialog::WindowProc(nMsg,wParam,lParam);
}

BEGIN_MESSAGE_MAP(CRibbonProperties,CDialog)
   //{{AFX_MSG_MAP(CRibbonProperties)
   //}}AFX_MSG_MAP
   ON_REGISTERED_MESSAGE(UM_MAXIMIZE_CLICK,OnMaximizeClick)
   ON_COMMAND(IDOK,&CRibbonProperties::OnOK)
   ON_WM_NCLBUTTONDOWN()
   ON_WM_PAINT()
   ON_WM_NCPAINT()
   ON_WM_CTLCOLOR()
   ON_MESSAGE(WM_IDLEUPDATECMDUI,&CRibbonProperties::OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRibbonProperties message handlers


void CRibbonProperties::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CRibbonProperties)
   //}}AFX_DATA_MAP
}

BOOL CRibbonProperties::Create(CWnd *pParentWnd)
{
   BOOL bResult=CDialog::Create(IDD_RIBBON_PROPERTIES,pParentWnd)
      && m_wndFillIcon.SubclassDlgItem(ID_TOOL_ATTRIB_FILL_ICON,this)
      && m_wndPFillIcon.SubclassDlgItem(ID_TOOL_ATTRIB_PFILL_ICON,this)
      && m_wndPenIcon.SubclassDlgItem(ID_TOOL_ATTRIB_PEN_ICON,this)
      && m_wndFill.SubclassDlgItem(ID_TOOL_ATTRIB_FILL_MENU,this)
      && m_wndPFill.SubclassDlgItem(ID_TOOL_ATTRIB_PFILL_MENU,this)
      && m_wndPen.SubclassDlgItem(ID_TOOL_ATTRIB_PEN_MENU,this)
      && m_wndFillTransparency.SubclassDlgItem(ID_TOOL_ATTRIB_FILL_TRANSPARENCY,this) // Opacity/Transparency - Infinisys Ltd
      && m_wndPFillTransparency.SubclassDlgItem(ID_TOOL_ATTRIB_PFILL_TRANSPARENCY,this) // Opacity/Transparency - Infinisys Ltd
      && m_wndPenTransparency.SubclassDlgItem(ID_TOOL_ATTRIB_PEN_TRANSPARENCY,this) // Opacity/Transparency - Infinisys Ltd
      && m_wndLineWidth.SubclassDlgItem(ID_TOOL_ATTRIB_LINE_WEIGTH,this)
      && m_wndLineStyle.SubclassDlgItem(ID_TOOL_ATTRIB_LINE_STYLE,this)
      && m_wndEndmarks.SubclassDlgItem(ID_TOOL_ATTRIB_END_MARK,this);
      //&& m_wndSep1.SubclassDlgItem(IDC_STATIC_SEP1,this) // Opacity/Transparency - Infinisys Ltd
      //&& m_wndSep2.SubclassDlgItem(IDC_STATIC_SEP2,this) // Opacity/Transparency - Infinisys Ltd
      //&& m_wndSep3.SubclassDlgItem(IDC_STATIC_SEP3,this); // Opacity/Transparency - Infinisys Ltd
   // Opacity/Transparency - Infinisys Ltd
   {
      //CRect r;
      //m_wndSep1.GetWindowRect(r);
      //m_wndSep1.SetWindowPos(NULL,0,0,r.Width(),12,SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
      //m_wndSep2.SetWindowPos(NULL,0,0,r.Width(),12,SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
      //m_wndSep3.SetWindowPos(NULL,0,0,r.Width(),12,SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
   }
   m_wndFill.m_bSimple = true;
   m_wndPFill.m_bSimple = true;
   m_wndPen.m_bSimple = true;
   m_wndFill.m_rect.SetRect(5,5,27,17);
   m_wndPFill.m_rect.SetRect(5,5,27,17);
   m_wndPen.m_rect.SetRect(5,5,27,17);
   SetOwner(pParentWnd->GetParentFrame());
   if(bResult)
   {
      //CToolTipCtrl* tips=((CMainFrame*)GetParentFrame())->GetToolTip();

      //tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_FILL_COLOR));
      //tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_FILL_PATTERN));
      //tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_FILL_TRANSPARENCY)); // Opacity/Transparency - Infinisys Ltd
      //tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_PFILL_COLOR));
      //tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_PFILL_PATTERN));
      //tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_PFILL_TRANSPARENCY)); // Opacity/Transparency - Infinisys Ltd
      //tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_PEN_COLOR));
      //tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_PEN_PATTERN));
      //tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_PEN_TRANSPARENCY)); // Opacity/Transparency - Infinisys Ltd
      //tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_LINE_STYLE));
      //tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_LINE_WEIGTH));
      //tips->AddTool(GetDlgItem(ID_TOOL_ATTRIB_END_MARK));

//      SetBorders(1,1,1,1);
  //    EnableDocking(0);
   }

   CWnd * pwndResizer = GetDlgItem(IDC_STATIC_RESIZER);

   CRect rResizer;

   pwndResizer->GetWindowRect(rResizer);

   int w = rResizer.Width();

   int h = rResizer.Height();

   int wRef = 116;

   int hRef = 89;

   CWnd * pwnd = GetWindow(GW_CHILD);

   while(pwnd != NULL)
   {

      CRect r;

      if(pwnd->GetDlgCtrlID() != IDC_STATIC_RESIZER)
      {

         pwnd->GetWindowRect(r);

         pwnd->SendMessage(UM_ATTRIBSETTINGS_INFO,1,0);

         ScreenToClient(r);

         r.left = r.left * wRef / w;
         r.top = r.top * hRef / h;
         r.right = r.right * wRef / w;
         r.bottom = r.bottom * hRef / h;

         pwnd->SetFont(&m_font);

         pwnd->SetWindowPos(NULL,r.left,r.top,r.Width(),r.Height(),SWP_NOZORDER);

      }

      pwnd = pwnd->GetWindow(GW_HWNDNEXT);

   }

   SetFont(&m_font);
   return bResult;
}

void CRibbonProperties::OffsetWindow(UINT nID,int nDelta)
{
   CRect rc;
   CWnd* pWnd=GetDlgItem(nID);
   pWnd->GetWindowRect(&rc);
   ScreenToClient(&rc);
   pWnd->SetWindowPos(NULL,rc.left,rc.top + nDelta,0,0,SWP_NOSIZE | SWP_NOZORDER);
}

//static bool g_bRegister = false;
//
//BOOL CRibbonProperties::PreCreateWindow(CREATESTRUCT & cs)
//{
//   if(!CDialog::PreCreateWindow(cs))
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


void CRibbonProperties::OnNcLButtonDown(UINT i,CPoint pt)
{


   CDialog::OnNcLButtonDown(i,pt);

   //return 0;

}


void CRibbonProperties::OnPaint()
{
   Default();
}

void CRibbonProperties::OnNcPaint()
{
   Default();
}


void CRibbonProperties::ShowHideParallel()
{
   static UINT gShowHide[] ={ID_TOOL_ATTRIB_PFILL,ID_TOOL_ATTRIB_PFILL_COLOR,ID_TOOL_ATTRIB_PFILL_PATTERN,0};
   static UINT gOffset[] ={ID_TOOL_ATTRIB_PEN,ID_TOOL_ATTRIB_PEN_COLOR,ID_TOOL_ATTRIB_PEN_PATTERN,ID_TOOL_ATTRIB_LINE_WEIGTH,ID_TOOL_ATTRIB_LINE_STYLE,ID_TOOL_ATTRIB_END_MARK,0};
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
//   m_sizeDefault.cy+=nDelta;
}

LRESULT CRibbonProperties::OnMaximizeClick(WPARAM wParam,LPARAM lParam)
{
   ((CCustomMiniDockFrameWnd*)GetParentFrame())->InvertMaximized();
   ShowHideParallel();
   GetParentFrame()->RecalcLayout();
   return 0;
}


// Opacity/Transparency - Infinisys Ltd
void CRibbonProperties::OnOK()
{

   CEdit * pedit = (CEdit *)GetFocus();

   CString strText;

   pedit->GetWindowText(strText);

   int iPercentage = atoi(strText);

   int iOpacity = (int)round((double)iPercentage * 255.0 / 100.0);


}



HBRUSH CRibbonProperties::OnCtlColor(CDC* pDC,CWnd* pWnd,UINT nCtlColor)
{
   HBRUSH hbr = CDialog::OnCtlColor(pDC,pWnd,nCtlColor);

   //hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor); //Commet it and add
   hbr = m_brush;
   // TODO:  Change any attributes of the DC here
   if(nCtlColor == CTLCOLOR_STATIC)
   {
      pDC->SetBkColor(RGB(250, 250, 250));

   }
   // TODO:  Return a different brush if the default is not desired
   return hbr;
}


afx_msg LRESULT CRibbonProperties::OnIdleUpdateCmdUI(WPARAM wParam,LPARAM lParam)
{

   // the style must be visible and if it is docked
   // the dockbar style must also be visible
   if(GetStyle() & WS_VISIBLE)
   {
      CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
      if(pTarget == NULL || !pTarget->IsFrameWnd())
         pTarget = GetParentFrame();
      if(pTarget != NULL)
         OnUpdateCmdUI(pTarget,(BOOL)wParam);
   }
   return 0L;
}


void CRibbonProperties::OnUpdateCmdUI(CFrameWnd* pTarget,BOOL bDisableIfNoHndler)
{
   UpdateDialogControls(pTarget,bDisableIfNoHndler);
}


BOOL CRibbonProperties::OnInitDialog()
{
   CDialog::OnInitDialog();

   // TODO:  Add extra initialization here
   //GetDlgItem(ID_TOOL_RESIZE_ABS)->SetFont(&m_pbar->m_font);
   //GetDlgItem(IDC_RELATIVE)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_LABEL1)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_LABEL2)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_LABEL3)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_LABEL4)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_LABEL5)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_LABEL6)->SetFont(&m_pbar->m_font);
   //GetDlgItem(IDC_STATIC_X)->SetFont(&m_pbar->m_font);
   //GetDlgItem(IDC_STATIC_Y)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_VALUE1)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_VALUE2)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_VALUE3)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_VALUE4)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_VALUE5)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_VALUE6)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_POSITION_X)->SetFont(&m_pbar->m_font);
   //GetDlgItem(ID_TOOL_RESIZE_POSITION_Y)->SetFont(&m_pbar->m_font);
   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}
