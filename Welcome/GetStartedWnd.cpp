// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "GetStartedWnd.h"
#include "../resource.h"
#include "../PCDraft.h"



const	LPCTSTR	gApplicationDefaults = "ApplicationDefaults";
const	LPCTSTR	gWindowAtStartUp = "WindowAtStartUp2";




Gdiplus::Bitmap * LoadPNG(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);


GetStartedWnd::GetStartedWnd()
{

   m_bHideOnButtonPress = true;

   m_pfontTopTitle = nullptr;
   m_pfontVersion = nullptr;
   

}


GetStartedWnd::~GetStartedWnd()
{

   delete m_pfontTopTitle;
   
   m_pfontTopTitle  = nullptr;

}



BEGIN_MESSAGE_MAP(GetStartedWnd, BaseWnd)
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   // ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_WM_NCCALCSIZE()
   ON_WM_ACTIVATE()
END_MESSAGE_MAP()


void GetStartedWnd::OnLButtonUp(UINT nFlags, CPoint point)
{

   CWnd::OnLButtonUp(nFlags, point);

}


void GetStartedWnd::OnMButtonDblClk(UINT nFlags, CPoint point)
{

   CWnd::OnMButtonDblClk(nFlags, point);

}


void GetStartedWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{

   CWnd::OnLButtonDblClk(nFlags, point);

}


void GetStartedWnd::OnButtonPress(int i)
{

   bool bClose = false;
      
   if (i == ID_FILE_CLOSE)
   {

      bClose = true;

      if (m_pWndFocus)
      {

         i = m_pWndFocus->GetDlgCtrlID();

      }

   }

   auto papplication = ((CPCDraftApp*)AfxGetApp());

   if (i == ID_FILE_NEW)
   {

      papplication->ShowMainFrame();

      papplication->OnFileNewDrawing();

      return;

   }
   else if (i == ID_FILE_TEMPLATE)
   {

      papplication->ShowTemplateWnd();

      return;

   }
   else if (i == 5000)
   {

      int nWindowAtStartup = kNew;

      if (m_checkbox.m_bChecked)
      {

         nWindowAtStartup = kWelcomeScreen;

      }

      papplication->WriteProfileInt(gApplicationDefaults, gWindowAtStartUp, nWindowAtStartup);

      return;

   }
   else if(bClose)
   {

      papplication->ShowMainFrame();

      return;

   }

   BaseWnd::OnButtonPress(i);

}


int GetStartedWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   CString str;

   str.LoadString(IDS_GETSTARTED_ALWAYS_SHOW_ON_STARTUP);

   m_checkbox.Create(NULL, str, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 5000);

   m_checkbox.m_bChecked = ((CPCDraftApp*)AfxGetApp())->m_nWindowAtStartUp == kWelcomeScreen;

   m_buttonNew.m_pimage44 = LoadPNG(MAKEINTRESOURCE(IDB_PNG_WELCOME_NEW), "PNG", AfxGetApp()->m_hInstance);
   m_buttonTemplate.m_pimage44 = LoadPNG(MAKEINTRESOURCE(IDB_PNG_WELCOME_TEMPLATE), "PNG", AfxGetApp()->m_hInstance);
   m_buttonOpen.m_pimage44 = LoadPNG(MAKEINTRESOURCE(IDB_PNG_WELCOME_FOLDER), "PNG", AfxGetApp()->m_hInstance);
   m_helpbutton.m_pimage44 = LoadPNG(MAKEINTRESOURCE(IDB_PNG_WELCOME_HELP), "PNG", AfxGetApp()->m_hInstance);

   str.LoadString(IDS_GETSTARTED_CREATE_NEW);

   m_buttonNew.Create(NULL, str, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_FILE_NEW);

   str.LoadString(IDS_GETSTARTED_CHOOSE_TEMPLATE);

   m_buttonTemplate.Create(NULL, str, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_FILE_TEMPLATE);

   str.LoadString(IDS_GETSTARTED_BROWSE_DOCUMENTS);

   m_buttonOpen.Create(NULL, str, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_FILE_OPEN);

   str.LoadString(IDS_GETSTARTED_SHORT_HELP_AND_SUPPORT);

   m_helpbutton.Create(NULL, str, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_HELP);

   if (BaseWnd::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   m_pWndFocus = &m_buttonNew;

   return 0;

}



//void GetStartedWnd::OnSize(UINT nType, int cx, int cy)
//{
//
//   BaseWnd::OnSize(nType, cx, cy);
//
//   layout();
//
//}


void GetStartedWnd::layout()
{

   CRect rectClient;

   GetClientRect(rectClient);

   int cx = rectClient.Width();

   int cy = rectClient.Height();

   int footer_height = 60 * size_scaler();

   int checkbox_height = 24 * size_scaler();

   m_checkbox.SetWindowPos(nullptr, 30 * size_scaler(), cy - ((footer_height + checkbox_height) / 2.0f), 200 * size_scaler(), checkbox_height, SWP_SHOWWINDOW);

   int help_height = 30 * size_scaler();
   int help_width = 200 * size_scaler();

   m_helpbutton.SetWindowPos(nullptr, cx - help_width - 30 * size_scaler(), cy - ((footer_height + help_height) / 2.0f), help_width, help_height, SWP_SHOWWINDOW);


   Gdiplus::REAL y = (Gdiplus::REAL)((50 + 100 + 65 + 50) * size_scaler());

   Gdiplus::REAL iButtonHeight = (Gdiplus::REAL)(65 * size_scaler());

   Gdiplus::REAL iButtonSpacing = (Gdiplus::REAL)(12 * size_scaler());

   Gdiplus::REAL x = (Gdiplus::REAL)(95 * size_scaler());

   Gdiplus::REAL w = (Gdiplus::REAL)(cx) - x * 2.0f;

   m_buttonNew.SetWindowPos(nullptr, (LONG)x, (LONG)y, (LONG)w, (LONG)iButtonHeight, SWP_SHOWWINDOW);

   y += iButtonHeight;

   y += iButtonSpacing;

   m_buttonTemplate.SetWindowPos(nullptr, (LONG)x, (LONG)y, (LONG)w, (LONG)iButtonHeight, SWP_SHOWWINDOW);

   y += iButtonHeight;

   y += iButtonSpacing;

   m_buttonOpen.SetWindowPos(nullptr, (LONG)x, (LONG)y, (LONG)w, (LONG)iButtonHeight, SWP_SHOWWINDOW);

   //y += iButtonHeight;

   //y += iButtonSpacing;

   Invalidate();

}


BOOL GetStartedWnd::Create()
{

   CRect rect;

   rect.left = 0;
   rect.top = 0;
   rect.right = 600 * m_dSizeScaler;
   rect.bottom = 580 * m_dSizeScaler;

   DWORD dwDefaultStyle = 0;

   UINT nIDResource = IDR_WELCOME;

   LPCTSTR lpszClass = GetIconWndClass(dwDefaultStyle, nIDResource);

   DWORD dwStyle = WS_VISIBLE | WS_THICKFRAME;

   if (!CreateEx(0, lpszClass, "", dwStyle, rect, nullptr, 0))
   {

      return FALSE;

   }

   ModifyStyle(WS_MAXIMIZEBOX, 0, SWP_FRAMECHANGED);

   ModifyStyle(WS_MINIMIZEBOX, 0, SWP_FRAMECHANGED);

   CenterWindow();

   return true;

}


void GetStartedWnd::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   CRect rectClient;

   GetClientRect(rectClient);

   // Top Center Icon
   {

      HICON hicon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 256, 256, 0);

      Gdiplus::Rect rectIcon;

      rectIcon.Width = (LONG ) (100 * m_dSizeScaler);
      
      rectIcon.Height = (LONG) (100 * m_dSizeScaler);
      
      rectIcon.Y = (LONG) (50 * m_dSizeScaler);

      rectIcon.X = (LONG)((rectClient.Width()) - rectIcon.Width) / 2;

      Gdiplus::Bitmap* pbitmap = LoadPNG(MAKEINTRESOURCE(IDB_PNG_MAINFRAME_256), "PNG", AfxGetApp()->m_hInstance);

      pgraphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

      pgraphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);

      pgraphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);

      pgraphics->DrawImage(pbitmap, rectIcon);

      delete pbitmap;
   
      ::DestroyIcon(hicon);

   }

   pgraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

   // Top Center Title
   {

      Gdiplus::Color colorTopTitle(255, 223, 88, 56);

      if (m_pfontTopTitle == nullptr)
      {

         Gdiplus::InstalledFontCollection installedFontCollection;

         auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI Light", &installedFontCollection);

         m_pfontTopTitle = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (60.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

      }

      CStringW wstr;

      wstr.LoadString(IDS_GETSTARTED_WITH_PCDRAFT);

      Gdiplus::SolidBrush brush(colorTopTitle);

      Gdiplus::RectF        rectF(0, (50 + 100) *  m_dSizeScaler, rectClient.Width(), rectClient.Height() /2);

      Gdiplus::StringFormat format;

      format.SetAlignment(::Gdiplus::StringAlignmentCenter);
      
      format.SetLineAlignment(::Gdiplus::StringAlignmentNear);

      pgraphics->DrawString(wstr, wstr.GetLength(), m_pfontTopTitle, rectF, &format, &brush);

   }

   // Version
   {

      Gdiplus::Color colorTopTitle(255, 0, 0, 0);

      if (m_pfontVersion == nullptr)
      {

         Gdiplus::InstalledFontCollection installedFontCollection;

         auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI", &installedFontCollection);

         m_pfontVersion = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (22.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

      }

      CStringW wstr;

      wstr.LoadString(IDS_GETSTARTED_VERSION_70);

      Gdiplus::SolidBrush brush(colorTopTitle);

      Gdiplus::RectF        rectF(0, (50 + 100 + 65) * size_scaler(), rectClient.Width(), rectClient.Height() / 2);

      Gdiplus::StringFormat format;

      format.SetAlignment(::Gdiplus::StringAlignmentCenter);
      
      format.SetLineAlignment(::Gdiplus::StringAlignmentNear);

      pgraphics->DrawString(wstr, wstr.GetLength(), m_pfontVersion, rectF, &format, &brush);

   }

   // Footer 
   {

      Gdiplus::SolidBrush brushFooter(Gdiplus::Color(255, 242, 242, 242));

      Gdiplus::Rect rectFooter;

      rectFooter.X = 0;
      
      rectFooter.Width = rectClient.Width();
      
      rectFooter.Height = 60 * size_scaler();

      rectFooter.Y = rectClient.bottom - rectFooter.Height;

      pgraphics->FillRectangle(&brushFooter, rectFooter);

   }

   
}





void GetStartedWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   BaseWnd::OnActivate(nState, pWndOther, bMinimized);

}
