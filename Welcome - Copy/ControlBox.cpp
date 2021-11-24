// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "ControlBox.h"
#include "../resource.h"


IMPLEMENT_DYNAMIC(ControlBox, BaseControl);


ControlBox::ControlBox()
{

   m_colorBackground = Gdiplus::Color(255, 255, 255, 255);
   m_colorForeground = Gdiplus::Color(255, 0, 0, 0);

}


ControlBox::~ControlBox()
{

}



BEGIN_MESSAGE_MAP(ControlBox, BaseControl)
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()



int ControlBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   m_buttonMinimize.m_colorBackground = m_colorBackground;
   m_buttonClose.m_colorBackground = m_colorBackground;

   m_buttonMinimize.m_colorForeground = m_colorForeground;
   m_buttonClose.m_colorForeground = m_colorForeground;

   if (BaseControl::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   m_buttonMinimize.Create(NULL, "0", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, SW_MINIMIZE);
   
   m_buttonClose.Create(NULL, "r", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_FILE_CLOSE);

   return 0;

}



void ControlBox::OnSize(UINT nType, int cx, int cy)
{

   BaseControl::OnSize(nType, cx, cy);

   CRect rectClient;

   GetClientRect(rectClient);

   m_buttonMinimize.SetWindowPos(nullptr, 0, 0, rectClient.Height(), rectClient.Width() / 2, SWP_SHOWWINDOW);

   m_buttonClose.SetWindowPos(nullptr, rectClient.Width() / 2, 0, rectClient.Height(), rectClient.Width() / 2, SWP_SHOWWINDOW);

}


void ControlBox::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   CRect rectClient;

   GetClientRect(rectClient);

   pgraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
   
   //Gdiplus::SolidBrush brush(m_colorBackground);

   //Gdiplus::Rect rect;

   //rect.X = 0;
   //rect.Y = 0;
   //rect.Width = rectClient.Width();
   //rect.Height = rectClient.Height();

   //pgraphics->FillRectangle(&brush, rect);

}