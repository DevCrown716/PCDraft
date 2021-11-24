// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "TemplateLeftWnd.h"
#include "../resource.h"

Gdiplus::Bitmap * LoadPNG(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);

BEGIN_MESSAGE_MAP(TemplateLeftWnd, BaseControl)
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

TemplateLeftWnd::TemplateLeftWnd()
{

   m_pbitmap = LoadPNG(MAKEINTRESOURCEA(IDB_PNG_WELCOME_ICON), "PNG", NULL);

}
TemplateLeftWnd::~TemplateLeftWnd()
{

   delete m_pbitmap;

}


BOOL TemplateLeftWnd::OnEraseBkgnd(CDC* pDC)
{

   return TRUE;

}



void TemplateLeftWnd::OnLButtonUp(UINT nFlags, CPoint point)
{

   BaseControl::OnLButtonUp(nFlags, point);

}


void TemplateLeftWnd::OnMButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnMButtonDblClk(nFlags, point);

}


void TemplateLeftWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnLButtonDblClk(nFlags, point);

}


int TemplateLeftWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (BaseControl::OnCreate(lpCreateStruct) == -1)
      return -1;

   m_buttonNew.Create(NULL, "New", WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_FILE_TEMPLATE);
   m_buttonRecent.Create(NULL, "Recent", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_FILE_MRU_FILE1);
   m_buttonOpen.Create(NULL, "Open", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_FILE_OPEN);

   m_buttonWhatsNew.Create(NULL, "What's New", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1000);
   m_buttonHelp.Create(NULL, "Help & Support", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1001);
   m_buttonCommunity.Create(NULL, "Community", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1002);

   // TODO:  Add your specialized creation code here

   return 0;
}


void TemplateLeftWnd::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   CRect rectClient;

   GetClientRect(rectClient);

   pdc->FillSolidRect(rectClient, RGB(180, 60, 32));

   Gdiplus::Graphics g(pdc->GetSafeHdc());

   Gdiplus::RectF rectSrc(0, 0,
      m_pbitmap->GetWidth() ,
      m_pbitmap->GetHeight());

   Gdiplus::RectF rectDst(
      (rectClient.Width() - rectSrc.Width * size_scaler()) / 2,
      40 * size_scaler(),
      rectSrc.Width * size_scaler(),
      rectSrc.Height * size_scaler());

   pgraphics->DrawImage(m_pbitmap, rectDst, rectSrc, Gdiplus::UnitPixel );

   int left = 25;

   Gdiplus::Pen pen(Gdiplus::Color(255,190, 70, 42), 4.0);

   pgraphics->DrawLine(&pen, left, m_xSeparator, rectClient.Width() - left, m_xSeparator);
}



void TemplateLeftWnd::OnSize(UINT nType, int cx, int cy)
{
   BaseControl::OnSize(nType, cx, cy);

   // TODO: Add your message handler code here

   int iButtonHeight = 60;

   int y = 150;

   m_buttonNew.SetWindowPos(NULL, 0, y, cx, iButtonHeight, SWP_SHOWWINDOW);

   y += iButtonHeight;

   m_buttonRecent.SetWindowPos(NULL, 0, y, cx, iButtonHeight, SWP_SHOWWINDOW);

   y += iButtonHeight;

   m_buttonOpen.SetWindowPos(NULL, 0, y, cx, iButtonHeight, SWP_SHOWWINDOW);

   y += iButtonHeight;

   m_xSeparator = y + iButtonHeight / 2;

   y += iButtonHeight;

   m_buttonWhatsNew.SetWindowPos(NULL, 0, y, cx, iButtonHeight, SWP_SHOWWINDOW);

   y += iButtonHeight;

   m_buttonHelp.SetWindowPos(NULL, 0, y, cx, iButtonHeight, SWP_SHOWWINDOW);

   y += iButtonHeight;

   m_buttonCommunity.SetWindowPos(NULL, 0, y, cx, iButtonHeight, SWP_SHOWWINDOW);



}


