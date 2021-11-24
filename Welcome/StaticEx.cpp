// Infinisys Ltd. 2021 - Unicode File names
#include "StdAfx.h"
#include "StaticEx.h"
#include "memory_dc.h"



BEGIN_MESSAGE_MAP(StaticEx, CWnd)
   ON_WM_LBUTTONDOWN()
   ON_WM_PAINT()
END_MESSAGE_MAP()

StaticEx::StaticEx()
{
   m_pfont = nullptr;
   m_crBackground = RGB(242, 242, 242);

}


StaticEx::~StaticEx()
{

}


void StaticEx::OnPaint()
{
   CPaintDC dcPaint(this); // device context for painting
                   // TODO: Add your message handler code here
                   // Do not call CWnd::OnPaint() for painting messages


   // Get the client rect.
/*  CRect rect;
  GetClientRect(&rect);
*/
//// Prepare for memory DC drawing.
//CDC memoryDC;
//if (rect.Width() > 0 && rect.Height() > 0 &&
//   memoryDC.CreateCompatibleDC(&dc))
//{
//   CBitmap bitmap;
//   if (bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height()))
//   {
//      CDC* pDC = &memoryDC;

//      // Select bitmap into memory DC.
//      CBitmap* pOldBitmap = pDC->SelectObject(&bitmap);

      // Set up the memory DC for drawing graphics and text.
      //CPen pen(PS_SOLID, 1, RGB(0, 0, 255));
      //CPen* pOldPen = pDC->SelectObject(&pen);
      //CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
      //COLORREF oldTextColor = pDC->SetTextColor(RGB(0, 0, 255));
      //int oldBkMode = pDC->SetBkMode(TRANSPARENT);

      // Fill the background.
//      pDC->FillSolidRect(&rect, RGB(0, 255, 0));

//      // Draw a rectangle representing the display size.
//      // The rectangle needs to be offset due to scrolling position.
//      CRect dispRect;
//      dispRect.SetRect(0, 0,
//         m_scrollHelper->GetDisplaySize().cx,
//         m_scrollHelper->GetDisplaySize().cy);
//      dispRect.OffsetRect(-m_scrollHelper->GetScrollPos().cx,
//         -m_scrollHelper->GetScrollPos().cy);
//      pDC->Rectangle(&dispRect);

//      // Draw text information.
//      DrawScrollInfo(pDC);

//      // Do the bitblt.
//      dc.BitBlt(0, 0, rect.Width(), rect.Height(),
//         pDC, 0, 0, SRCCOPY);

//      // Restore DC state.
//      pDC->SetBkMode(oldBkMode);
//      pDC->SetTextColor(oldTextColor);
//      pDC->SelectObject(pOldBrush);
//      pDC->SelectObject(pOldPen);
//      pDC->SelectObject(pOldBitmap);
//   }
//}

   memory_dc dc(dcPaint, this);


   ////Define a bitmap object
   //CBitmap MemBitmap;
   ////Create a memory device description table handle (backup buffer) compatible with the screen device description table (front buffer)
   //dc.CreateCompatibleDC(&dcPaint);
   ////Cannot draw at this time, because the device description table without bitmap cannot be drawn
   ////The following creates a bitmap compatible with the screen device description table (or memory device description table)
   //MemBitmap.CreateCompatibleBitmap(&dc, nWidth, nHeight);
   ////Select the bitmap into the memory device description table
   ////Only the device description table with the bitmap selected has a place to draw on the specified bitmap
   //CBitmap *pOldBit = dc.SelectObject(&MemBitmap);
   ////First clear the bitmap with a background color, here I use white as the background
   ////You can also use the colors you should use
   ////MemDC.FillSolidRect(0, 0, nWidth, nHeight, RGB(255, 255, 255));
   ////Drawing

   CDC * pdc = &dc;

   Gdiplus::Graphics g(pdc->GetSafeHdc());

   Gdiplus::Graphics * pgraphics = &g;

   CRect rectClient;

   GetClientRect(rectClient);


   dc.FillSolidRect(rectClient, m_crBackground);

   pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   pgraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
   pgraphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
   pgraphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
   pgraphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);


   //OnDraw(pdc, pgraphics);

   ////Copy the graphics in the back buffer to the front buffer
   //dcPaint.BitBlt(0, 0, nWidth, nHeight, &dc, 0, 0, SRCCOPY);
   ////Clean up after drawing
   ////MemBitmap.DeleteObject();
   ////MemDC.DeleteDC();


   Gdiplus::RectF rect;

   rect.X = 0;

   rect.Y = 0;

   rect.Width = rectClient.Width();

   rect.Height = rectClient.Height();

   if (m_pfont == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI", &installedFontCollection);

      m_pfont = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (12.0), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }

   // Message
   {

      CStringW wstr = m_wstr;

      Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 0));

      Gdiplus::StringFormat format;

      format.SetAlignment(::Gdiplus::StringAlignmentCenter);

      format.SetLineAlignment(::Gdiplus::StringAlignmentCenter);

      pgraphics->SetCompositingMode(::Gdiplus::CompositingModeSourceOver);

      pgraphics->DrawString(wstr, wstr.GetLength(), m_pfont, rect, &format, &brush);

   }

}

   




void StaticEx::OnLButtonDown(UINT nFlags, CPoint point)
{

   CWnd::OnLButtonUp(nFlags, point);

   GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID());

}
