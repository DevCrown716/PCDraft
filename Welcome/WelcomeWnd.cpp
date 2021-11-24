// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "WelcomeWnd.h"
#include "../resource.h"
#include "../PCDraft.h"
#include "../SerialNumber.h"



const	LPCTSTR	gApplicationDefaults = "ApplicationDefaults";
const	LPCTSTR	gWindowAtStartUp = "WindowAtStartUp";


Gdiplus::Bitmap * LoadPNG(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);


WelcomeWnd::WelcomeWnd()
{

   m_strSerialNumber = "XXX";
   m_pfontTopTitle = nullptr;
   m_pfontVersion = nullptr;
   m_pfontMessage = nullptr;

}


WelcomeWnd::~WelcomeWnd()
{

   delete m_pfontTopTitle;

   m_pfontTopTitle = nullptr;

}


BEGIN_MESSAGE_MAP(WelcomeWnd, BaseWnd)
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_WM_NCCALCSIZE()
   ON_WM_ACTIVATE()
END_MESSAGE_MAP()


void WelcomeWnd::Check()
{

   GetDlgItem(IDOK)->EnableWindow(!(m_strName.IsEmpty() || IsBadSerialNumber(m_strSerialNumber)));

}


void WelcomeWnd::OnLButtonUp(UINT nFlags, CPoint point)
{

   BaseWnd::OnLButtonUp(nFlags, point);

}


void WelcomeWnd::OnEditChangeText(int nID)
{

   if (nID == IDC_NAME)
   {

      m_editName.m_edit.GetWindowTextA(m_strName);

      if (m_strName == m_editName.m_edit.m_strEmpty)
      {

         m_strName = "";

      }

      Check();

   }
   else if (nID == IDC_SERIAL)
   {

      m_editSerial.m_edit.GetWindowTextA(m_strSerialNumber);

      if (m_strSerialNumber == m_editSerial.m_edit.m_strEmpty)
      {

         m_strSerialNumber = "";

      }

      Check();

   }


}


void WelcomeWnd::OnMButtonDblClk(UINT nFlags, CPoint point)
{

   BaseWnd::OnMButtonDblClk(nFlags, point);

}


void WelcomeWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{

   BaseWnd::OnLButtonDblClk(nFlags, point);

}


int WelcomeWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (BaseWnd::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   CString str;

   str.LoadString(IDS_WELCOME_CANCEL);

   m_buttonCancel.Create(NULL, str, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDCANCEL);

   str.LoadString(IDS_WELCOME_OK);

   m_buttonOk.Create(NULL, str, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDOK);

   m_editName.m_edit.m_strEmpty.LoadString(IDS_WELCOME_EDIT_NAME);

   str.LoadStringA(IDS_WELCOME_FULL_NAME);

   m_editName.Create(NULL, str, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDC_NAME);

   if (m_strName.IsEmpty())
   {

      m_editName.SetWindowText(m_editName.m_edit.m_strEmpty);

   }
   else
   {

      m_editName.SetWindowText(m_strName);

   }

   m_editName.Invalidate();

   m_editSerial.m_edit.m_strEmpty = "PDFT-V700-XXXX-XXXX-XXXX-XXXX";

   str.LoadString(IDS_WELCOME_SERIAL_NO);

   m_editSerial.Create(NULL, str, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDC_SERIAL);

   if (m_strName.IsEmpty())
   {

      m_editSerial.SetWindowText(m_editSerial.m_edit.m_strEmpty);

   }
   else
   {

      m_editSerial.SetWindowText(m_strSerialNumber);

   }

   m_editSerial.Invalidate();

   m_editName.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

   m_editSerial.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

   m_buttonCancel.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

   m_buttonOk.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

   m_pWndFocus = &m_buttonOk;

   return 0;

}


void WelcomeWnd::OnSize(UINT nType, int cx, int cy)
{

   BaseWnd::OnSize(nType, cx, cy);

   CRect rectClient;

   GetClientRect(rectClient);

   // Body
   {

      int y = (50 + 100 + 65 + 50) * size_scaler();

      int iEditHeight = 40 * size_scaler();

      int iEditSpacing = 10 * size_scaler();

      int x = 130 * size_scaler();

      int w = rectClient.Width() - x * 2;

      m_editName.SetWindowPos(nullptr, x, y, w, iEditHeight, SWP_SHOWWINDOW | SWP_NOZORDER);

      y += iEditHeight;

      y += iEditSpacing;

      m_editSerial.SetWindowPos(nullptr, x, y, w, iEditHeight, SWP_SHOWWINDOW | SWP_NOZORDER);

      y += iEditHeight;

      y += iEditSpacing;

      int iExpand = 30 * size_scaler();

      m_rectMessage.X = x - iExpand;
      
      m_rectMessage.Y = y;
      
      m_rectMessage.Width = w + iExpand * 2;

      m_rectMessage.Height = iEditHeight * 2.5;

   }

   // Footer
   {

      int wButton = 80 * size_scaler();

      int HSpacing = 12 * size_scaler();

      int x = (rectClient.Width() - (wButton * 2) - HSpacing) / 2;

      int hFooter = 45 * size_scaler();

      int y = rectClient.Height() - hFooter;

      m_buttonCancel.SetWindowPos(nullptr, x, y, wButton, 30 * size_scaler(), SWP_SHOWWINDOW | SWP_NOZORDER);

      m_buttonOk.SetWindowPos(nullptr, x + wButton + HSpacing, y, wButton, 30 * size_scaler(), SWP_SHOWWINDOW | SWP_NOZORDER);

   }

   Invalidate();

}


BOOL WelcomeWnd::Create()
{

   DWORD dwDefaultStyle = 0;

   UINT nIDResource = IDR_WELCOME;

   LPCTSTR lpszClass = GetIconWndClass(dwDefaultStyle, nIDResource);
   DWORD dwStyle = WS_VISIBLE | WS_THICKFRAME;

   CRect rect;

   rect.left = 0;
   rect.top = 0;
   rect.right = 600 * m_dSizeScaler;
   rect.bottom = 550 * m_dSizeScaler;

   if (!CreateEx(0, lpszClass, "", dwStyle, rect, nullptr, 0))
   {

      return FALSE;

   }

   SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE);

   ModifyStyle(WS_MAXIMIZEBOX, 0, SWP_FRAMECHANGED);

   ModifyStyle(WS_MINIMIZEBOX, 0, SWP_FRAMECHANGED);

   CenterWindow();

   CRect rectWindow;

   GetWindowRect(rectWindow);

   return true;

}


void WelcomeWnd::OnButtonPress(int i)
{

   if (i == IDOK)
   {

      m_editName.m_edit.GetWindowTextA(m_strName);

      if (m_strName == m_editName.m_edit.m_strEmpty)
      {
         m_strName.Empty();

      }
      
      m_editSerial.m_edit.GetWindowTextA(m_strSerialNumber);

      if (m_strSerialNumber == m_editSerial.m_edit.m_strEmpty)
      {
         m_strSerialNumber.Empty();

      }

      EndModalLoop(IDOK);

   }
   else if(i == IDCANCEL)
   {

      EndModalLoop(IDCANCEL);

   }
   else if (i == ID_FILE_CLOSE)
   {

      EndModalLoop(IDCANCEL);

      return;

   }
   
   BaseWnd::OnButtonPress(i);
   
}


void WelcomeWnd::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   CRect rectClient;

   GetClientRect(rectClient);

   // Top Center Icon
   {

      HICON hicon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 256, 256, 0);

      Gdiplus::Rect rectIcon;

      rectIcon.Width = 100 * size_scaler();
      rectIcon.Height = 100 * size_scaler();
      rectIcon.Y = 50 * size_scaler();
      rectIcon.X = (rectClient.Width() - rectIcon.Width) / 2;

      Gdiplus::Bitmap* pbitmap = LoadPNG(MAKEINTRESOURCE(IDB_PNG_MAINFRAME_256), "PNG", AfxGetApp()->m_hInstance);

      pgraphics->DrawImage(pbitmap, rectIcon);

      delete pbitmap;

      ::DestroyIcon(hicon);

   }

   // Top Center Title
   {

      Gdiplus::Color colorTopTitle(255, 223, 88, 56);

      if (m_pfontTopTitle == nullptr)
      {

         Gdiplus::InstalledFontCollection installedFontCollection;

         auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI Light", &installedFontCollection);

         m_pfontTopTitle = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (60.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

      }

      if (m_pfontMessage == nullptr)
      {

         Gdiplus::InstalledFontCollection installedFontCollection;

         auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI", &installedFontCollection);

         m_pfontMessage = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (16.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

      }

      CStringW wstr;
      
      wstr.LoadString(IDS_WELCOME_TO_PCDRAFT);

      Gdiplus::SolidBrush brush(colorTopTitle);

      Gdiplus::RectF        rectF(0, (50 + 100)  * size_scaler(), rectClient.Width(), rectClient.Height() / 2);

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

      wstr.LoadString(IDS_WELCOME_REGISTER_SERIAL);

      Gdiplus::SolidBrush brush(colorTopTitle);

      Gdiplus::RectF        rectF(0,( 50 + 100 + 65) * size_scaler(), rectClient.Width(), rectClient.Height() / 2);

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

   // Border
   {
      
      Gdiplus::Rect rectBorder;
      
      rectBorder.Width = rectClient.Width() - 1;

      rectBorder.Height = rectClient.Height() - 1;

      rectBorder.X = 0;
      
      rectBorder.Y = 0;

      Gdiplus::Pen penBorder(Gdiplus::Color(255, 196, 196, 196), 1.0);

      pgraphics->DrawRectangle(&penBorder, rectBorder);

   }

   // Message
   {

      //CString str = ;

      CStringW wstr;

      wstr.LoadString(IDS_WELCOME_REGISTER_REMARK);

      Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 0));

      auto pformat = Gdiplus::StringFormat::GenericTypographic()->Clone();

      pformat->SetAlignment(::Gdiplus::StringAlignmentCenter);

      pformat->SetLineAlignment(::Gdiplus::StringAlignmentCenter);

      pgraphics->DrawString(wstr, wstr.GetLength(), m_pfontMessage, m_rectMessage, pformat, &brush);

      delete pformat;

   }

}


void WelcomeWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{

   BaseWnd::OnActivate(nState, pWndOther, bMinimized);

}



