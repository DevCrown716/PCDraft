// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "WelcomeEdit.h"
#include "BaseWnd.h"
#include "../resource.h"


IMPLEMENT_DYNAMIC(WelcomeEdit, BaseControl);

WelcomeEdit::WelcomeEdit()
{

   m_pfont = nullptr;

}


WelcomeEdit::~WelcomeEdit()
{

   delete m_pfont;

}


BEGIN_MESSAGE_MAP(WelcomeEdit, BaseControl)
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


int WelcomeEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (BaseControl::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   CString str;

   GetWindowText(str);

   m_static.Create(NULL, str, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 10);
   m_edit.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 20);
   m_edit.SetWindowText(m_edit.m_strEmpty);
   return 0;

}



void WelcomeEdit::OnSize(UINT nType, int cx, int cy)
{

   BaseControl::OnSize(nType, cx, cy);

   int iLeft = 25 * size_scaler();

   int wLabel = 150 * size_scaler();

   m_static.SetWindowPos(nullptr, iLeft, 2, wLabel, cy - 4, SWP_SHOWWINDOW);

   int iHeight = 25 * size_scaler() + 5;

   m_rectEdit.X = 5 + wLabel;

   m_rectEdit.Y = (cy - iHeight) / 2;

   m_rectEdit.Width = cx - m_rectEdit.X - 5;

   m_rectEdit.Height = iHeight;

   m_edit.SetWindowPos(nullptr, m_rectEdit.X, m_rectEdit.Y, m_rectEdit.Width, m_rectEdit.Height, SWP_SHOWWINDOW);

}


HBRUSH WelcomeEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
   HBRUSH hbr = BaseControl::OnCtlColor(pDC, pWnd, nCtlColor);

   switch (nCtlColor)
   {
   case CTLCOLOR_EDIT:

      if (pWnd->GetDlgCtrlID() == 20)
      {

         CString str;

         pWnd->GetWindowText(str);

         if (str == m_edit.m_strEmpty)
         {

            pDC->SetTextColor(RGB(186, 186, 186));

         }
         else
         {

            pDC->SetTextColor(RGB(0, 0, 0));

         }

      }
      break;

   default:

      break;
   }
   return hbr;
}



void WelcomeEdit::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   CRect rectClient;

   GetClientRect(rectClient);

   Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));

   Gdiplus::RectF rClient;

   rClient.X = 0;
   rClient.Y = 0;
   rClient.Width = rectClient.Width();
   rClient.Height = rectClient.Height();

   pgraphics->FillRectangle(&brush, rClient);


   {

      Gdiplus::Color colorBorder;

      CWnd * pedit = &m_edit;

      if (GetFocus() == pedit)
      {

         colorBorder = Gdiplus::Color(255, 200, 80, 52);

      }
      else
      {

         colorBorder = Gdiplus::Color(255, 192, 192, 192);

      }

      Gdiplus::Pen pen(colorBorder, 1.0);
      int iLight = 255;

      int iRadius = 11 * size_scaler();

      Gdiplus::SolidBrush brush(Gdiplus::Color(255, iLight, iLight, iLight));

      Gdiplus::GraphicsPath path;

      Gdiplus::RectF        rect1(rectClient.Width() - iRadius - 2, 1, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect1, -90, 90);

      Gdiplus::RectF        rect2(rectClient.Width() - iRadius - 2, rectClient.Height() - iRadius - 2, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect2, 0, 90);

      Gdiplus::RectF        rect3(1, rectClient.Height() - iRadius - 2, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect3, 90, 90);

      Gdiplus::RectF        rect4(1, 1, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect4, 180, 90);

      path.CloseFigure();

      pgraphics->FillPath(&brush, &path);

      pgraphics->DrawPath(&pen, &path);

   }

   {

      Gdiplus::Pen pen(Gdiplus::Color(255, 192, 192, 192), 1.0);

      int iLight = 255;

      int iRadius = 11 * size_scaler();

      Gdiplus::SolidBrush brush(Gdiplus::Color(255, iLight, iLight, iLight));

      Gdiplus::GraphicsPath path;

      Gdiplus::RectF rect1(rectClient.Width() - iRadius - 2, 1, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect1, -90, 90);

      Gdiplus::RectF        rect2(rectClient.Width() - iRadius - 2, rectClient.Height() - iRadius - 2, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect2, 0, 90);


      Gdiplus::RectF        rect3(1, rectClient.Height() - iRadius - 2, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect3, 90, 90);


      Gdiplus::RectF        rect4(1, 1, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect4, 180, 90);

      path.CloseFigure();

      pgraphics->FillPath(&brush, &path);

      pgraphics->DrawPath(&pen, &path);

   }


}