#include "StdAfx.h"
#include "memory_dc.h"


memory_dc::memory_dc(CDC& dc, CWnd* pWnd) :
   m_dc(dc), m_pOldBmp(NULL)
{
   ASSERT_VALID(pWnd);

   pWnd->GetClientRect(m_rect);

   //m_rect.right += pWnd->GetScrollPos(SB_HORZ);
   //m_rect.bottom += pWnd->GetScrollPos(SB_VERT);

   HDC hdcPaint = NULL;

   if (CreateCompatibleDC(&m_dc) && m_bmp.CreateCompatibleBitmap(&m_dc, m_rect.Width(), m_rect.Height()))
   {
      //-------------------------------------------------------------
      // Off-screen DC successfully created. Better paint to it then!
      //-------------------------------------------------------------
      m_pOldBmp = SelectObject(&m_bmp);
   }
}

memory_dc::memory_dc(CDC& dc, const CRect& rect) :
   m_dc(dc), m_pOldBmp(NULL), m_rect(rect)
{
   ASSERT(!m_rect.IsRectEmpty());

   HDC hdcPaint = NULL;

   if (CreateCompatibleDC(&m_dc) && m_bmp.CreateCompatibleBitmap(&m_dc, m_rect.Width(), m_rect.Height()))
   {
      //-------------------------------------------------------------
      // Off-screen DC successfully created. Better paint to it then!
      //-------------------------------------------------------------
      m_pOldBmp = SelectObject(&m_bmp);
   }
}

memory_dc::~memory_dc()
{
   m_dc.BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), this, m_rect.left, m_rect.top, SRCCOPY);
   SelectObject(m_pOldBmp);
}
