// EditPatternWnd.cpp : implementation file
// Infinisys Ltd
//

#include "stdafx.h"
#include "pcdraft.h"
#include "GradientEditWnd.h"
#include "GradientDlg.h"
#include "GradientPack.h"
#include "ChooseColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void __copy(Gdiplus::RectF & r, LPCRECT lpcrect);
/////////////////////////////////////////////////////////////////////////////
// CGradientEditWnd

CGradientEditWnd::CGradientEditWnd()
{
   m_iDrag = -1;
   m_bMove = false;
}

CGradientEditWnd::~CGradientEditWnd()
{

   m_dc.SelectObject(m_hOld);

}


BEGIN_MESSAGE_MAP(CGradientEditWnd,CWnd)
   //{{AFX_MSG_MAP(CGradientEditWnd)
   ON_WM_PAINT()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_MOUSEMOVE()
   ON_WM_ERASEBKGND()
   ON_WM_SETCURSOR()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGradientEditWnd message handlers

BOOL CGradientEditWnd::OnEraseBkgnd(CDC* pDC)
{

   return TRUE;

}


double CGradientEditWnd::calc_rate(CPoint pt)
{

   return  ((double)pt.x - m_iMin) / (double)(m_iMax - m_iMin);

}


CPoint CGradientEditWnd::calc_point(double dRate)
{

   CRect rc;

   GetClientRect(&rc);

   return   CPoint(dRate * (double)(m_iMax - m_iMin) + m_iMin,rc.bottom - m_iSquareSize/2);

}


void CGradientEditWnd::get_between_rect(int i,LPRECT lprect)
{

   if(i < 0 || i >m_pParentDlg->m_ppack->m_da.GetCount())
   {

      memset(lprect,0,sizeof(RECT));

      return ;

   }

   GetClientRect(lprect);

   lprect->top = lprect->bottom - m_iSquareSize;

   CRect r;

   if(i == 0)
   {

      lprect->left = m_iMin;

      get_rect(0,r);
      
      lprect->right = r.left;

   }
   else if(i == m_pParentDlg->m_ppack->m_da.GetCount())
   {

      get_rect(m_pParentDlg->m_ppack->m_da.GetCount() - 1,r);

      lprect->left = r.right;

      lprect->right = m_iMax;

   }
   else
   {

      get_rect(i-1,r);

      lprect->left = r.right;

      get_rect(i,r);

      lprect->right = r.left;

   }

}


int CGradientEditWnd::between_hit_test(CPoint pt)
{

   for(int i = 0; i <= m_pParentDlg->m_ppack->m_da.GetCount();i++)
   {

      CRect r;

      get_between_rect(i,r);

      if(r.PtInRect(pt))
      {

         if(r.Width() >= m_iSquareSize)
         {

            return i;

         }

         return -1;

      }


   }

   return -1;

}


void CGradientEditWnd::get_rect(int i,LPRECT lprect)
{

   CRect rc2;

   m_pParentDlg->m_previewwnd.GetWindowRect(&rc2);

   ScreenToClient(&rc2);

   CRect rc;
   GetClientRect(&rc);

   m_iSquareSize = rc2.left;

   m_iMin = rc2.left;
   m_iMax = rc2.right;
   double dSize = round(m_pParentDlg->m_ppack->m_da[i] * (double)(m_iMax - m_iMin));
   rc.left = dSize + m_iMin - m_iSquareSize / 2;
   rc.right = rc.left + m_iSquareSize;
   rc.top = rc.bottom - m_iSquareSize;
   *lprect = rc;

}


void CGradientEditWnd::OnPaint()
{

   CPaintDC dcPaint(this); // device context for painting

   CDC & dc = m_dc;

   if(dc.GetSafeHdc() == NULL)
   {

      dc.CreateCompatibleDC(&dcPaint);

   }

   CRect rectClient;
   
   GetClientRect(&rectClient);
   
   CRect rc(rectClient);

   CBitmap & bitmap = m_bitmap;

   if(bitmap.GetSafeHandle() == NULL)
   {

      bitmap.CreateCompatibleBitmap(&dcPaint,rectClient.Width(),rectClient.Height());

      m_hOld = dc.SelectObject(bitmap);

   }
   else
   {

      BITMAP b;

      bitmap.GetBitmap(&b);

      if(b.bmWidth != rectClient.Width() || b.bmHeight != rectClient.Height())
      {

         dc.SelectObject(m_hOld);

         bitmap.CreateCompatibleBitmap(&dcPaint,rectClient.Width(),rectClient.Height());

         m_hOld = dc.SelectObject(bitmap);

      }

   }

   CGDIBackColor bc(&dc,GetSysColor(COLOR_BTNFACE));

   dc.ExtTextOut(0,0,ETO_OPAQUE | ETO_CLIPPED,&rectClient,NULL,0,NULL);

   CRect rc2;

   m_pParentDlg->m_previewwnd.GetWindowRect(&rc2);

   ScreenToClient(&rc2);

   m_iSquareSize = rc2.left;

   rc.left = rc2.left;

   rc.right = rc2.right;

   rc.bottom -= m_iSquareSize / 2;

   Gdiplus::GraphicsPath path;
   Gdiplus::RectF r;
   __copy(r, rc);
   path.AddRectangle(r);
   Gdiplus::Graphics g(dc.GetSafeHdc());
   ::std::unique_ptr < Gdiplus::Brush  > brush;
   brush.reset(m_pParentDlg->m_ppack->linear_brush(rc, -90.0, 255));
   g.FillRectangle(brush.get(), r);

   for(int i = 0; i < m_pParentDlg->m_ppack->m_da.GetCount(); i++)
   {

      CRect r;

      get_rect(i,r);

      COLORREF cr = m_pParentDlg->m_ppack->m_cra[i];

      CBrush br(cr & 0x00ffffff);

      CPen pen(PS_SOLID, 1, RGB(255- GetRValue(cr), 255 - GetGValue(cr), 255 - GetBValue(cr)));

      dc.SelectObject(br);

      dc.SelectObject(pen);

      dc.Rectangle(r);

   }

   dcPaint.BitBlt(0,0,rectClient.Width(),rectClient.Height(),&dc,0,0,SRCCOPY);

}


int CGradientEditWnd::hit_test(CPoint pt)
{

   for(int i = 0; i < m_pParentDlg->m_ppack->m_da.GetCount(); i++)
   {

      CRect r;

      get_rect(i,r);

      if(r.PtInRect(pt))
      {

         return i;

      }

   }

   return -1;

}


void CGradientEditWnd::OnLButtonDown(UINT nFlags,CPoint pt)
{

   m_iDrag = hit_test(pt);

   if(m_iDrag >= 0)
   {

      if(m_iDrag == 0)
      {

         m_iDragMin = m_iMin;

      }
      else
      {

         CRect r;

         get_rect(m_iDrag - 1,r);

         m_iDragMin = r.right + m_iSquareSize / 2 ;

      }

      if(m_iDrag < m_pParentDlg->m_ppack->m_da.GetCount() - 1)
      {

         CRect r;

         get_rect(m_iDrag + 1,r);

         m_iDragMax = r.left - m_iSquareSize / 2;

      }
      else
      {

         m_iDragMax = m_iMax;

      }

      m_bMove = false;

      SetCapture();

      m_dwLButtonDownStart = ::GetTickCount();

   }
   else
   {

      int iBetween = between_hit_test(pt);

      if(iBetween >= 0)
      {

         CRect rConstraint;

         get_between_rect(iBetween,rConstraint);

         CRect r;

         GetClientRect(r);

         r.left = pt.x - m_iSquareSize / 2;

         r.right= pt.x + m_iSquareSize / 2;

         if(r.left < rConstraint.left)
         {

            r.left = rConstraint.left;

            r.right = r.left + m_iSquareSize;

         }
         
         if(r.right > rConstraint.right)
         {

            r.right = rConstraint.right;

            r.left = r.right - m_iSquareSize;

         }

         if(r.left >= rConstraint.left)
         {

            double d = calc_rate(r.CenterPoint());

            m_pParentDlg->m_ppack->m_da.InsertAt(iBetween,d);

            m_pParentDlg->m_ppack->m_cra.InsertAt(iBetween,m_dc.GetPixel(r.CenterPoint().x, r.CenterPoint().y));

            m_pParentDlg->defer_redraw();

         }

      }

   }

}


void CGradientEditWnd::OnLButtonUp(UINT nFlags,CPoint pt)
{

   int iDrag = m_iDrag;

   if(iDrag >= 0)
   {

      if(GetTickCount() - m_dwLButtonDownStart< 500)
      {

         COLORREF cr = m_pParentDlg->m_ppack->m_cra[iDrag];

         CChooseColorDlg theDlg(cr,AfxGetMainWnd());

         if(theDlg.DoModal() == IDOK)
         {

            COLORREF cr = theDlg.GetColor();

            m_pParentDlg->m_ppack->m_cra[iDrag] = cr;

            //if(m_iDrag == 0 && m_pParentDlg->m_ppack->m_da[m_iDrag] == 0.0)
            //{

            //   m_pParentDlg->m_ppack->m_cra[m_iDrag + 1] = cr;

            //}
            //else if(m_iDrag == m_pParentDlg->m_ppack->m_cra.GetUpperBound() && m_pParentDlg->m_ppack->m_da[m_iDrag] == 1.0)
            //{

            //   m_pParentDlg->m_ppack->m_cra[m_iDrag - 1] = cr;

            //}


            m_pParentDlg->defer_redraw();

         }

      }
      else
      {

         move_rate(pt);

      }

      m_bMove = false;

      ReleaseCapture();

      m_iDrag=-1;

   }

}


void CGradientEditWnd::move_rate(CPoint pt)
{
   
   if(m_iDrag < 0)
   {

      return;

   }

   if(m_iDrag >= m_pParentDlg->m_ppack->m_da.GetCount())
   {

      return;

   }

   if(pt.x < m_iDragMin)
   {

      pt.x = m_iDragMin;

   }

   if(pt.x > m_iDragMax)
   {

      pt.x = m_iDragMax;

   }

   if(pt.x < m_iDragMin || pt.x > m_iDragMax)
   {

      return;

   }

   m_pParentDlg->m_ppack->m_da[m_iDrag] = calc_rate(pt);

   m_pParentDlg->defer_redraw();

}

void CGradientEditWnd::OnMouseMove(UINT nFlags,CPoint pt)
{

   if(m_iDrag >= 0)
   {

      if(!m_bMove)
      {

         if(m_pParentDlg->m_ppack->m_da.GetCount() >= 3)
         {

            CRect rectClient;

            GetClientRect(rectClient);

            if(pt.y >= rectClient.bottom || pt.y <= rectClient.bottom - m_iSquareSize)
            {

               m_pParentDlg->m_ppack->m_da.RemoveAt(m_iDrag);

               m_pParentDlg->m_ppack->m_cra.RemoveAt(m_iDrag);

               m_pParentDlg->defer_redraw();

               ReleaseCapture();

               m_iDrag = -1;

               return;

            }
            else
            {

               int iHit = hit_test(pt);

               if(iHit < 0)
               {

                  m_bMove = true;

               }

            }

         }
         else
         {

            m_bMove = true;

         }
      }
      
      if(m_bMove)
      {
         move_rate(pt);

      }

   }

}



