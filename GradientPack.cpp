// GradientPack.cpp: implementation of the CGradientPack class.
// Infinisys Ltd
//

#include "stdafx.h"
#include "GradientPack.h"

#include <gdiplus.h>
using namespace Gdiplus;
void __copy(Gdiplus::RectF & r, LPCRECT lpcrect);
double almost_equal_grad_rotation(double d1,double d2) 	  // MD6.2.1 - Infinisys Ltd.
{
   return fabs(d2 - d1) < 0.5;
}
#undef new
double almost_equal_grad_rate(double d1,double d2) // MD6.2.1 - Infinisys Ltd.
{
   return fabs(d2 - d1) < 0.001;
}

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGradientPack::CGradientPack()
{

   reset();

}

CGradientPack::~CGradientPack()
{
}

void CGradientPack::reset()
{

   m_da.RemoveAll();
   m_cra.RemoveAll();
   m_dRotation = 0.0;
   m_bRadial = false;
   m_dX = 0.0;
   m_dY = 0.0;

}


Gdiplus::LinearGradientBrush * CGradientPack::GetLinearBrush()
{

   return NULL;

}

Gdiplus::PathGradientBrush * CGradientPack::GetRadialBrush()
{

   return NULL;

}

//void CGradientPack::draw_linear(HDC hdc,LPCRECT lpcrect,double dRotation, int iOpacity)
//{
//
//   Graphics g(hdc);
//
//   draw_linear(g,lpcrect,dRotation, iOpacity);
//
//}
//
//
//void CGradientPack::draw_linear(HDC hdc,LPCRECT lpcrect,int iOpacity)
//{
//
//   Graphics g(hdc);
//
//   draw_linear(g,lpcrect, iOpacity);
//
//}
//
//
//void CGradientPack::draw_radial(HDC hdc,LPCRECT lpcrect, int iOpacity)
//{
//
//   Graphics g(hdc);
//
//   draw_radial(g,lpcrect, iOpacity);
//
//
//}

//
//void CGradientPack::draw_proper(HDC hdc, int iOpacity)
//{
//
//   Graphics g(hdc);
//
//   draw_proper(g, iOpacity);
//
//}
#undef new

Gdiplus::Brush * CGradientPack::radial_brush(LPCRECT lpcrect,int iOpacity)
{

   int iAlpha = min(255,max(0,iOpacity));

   Rect r;

   r.X = lpcrect->left;
   r.Y = lpcrect->top;
   r.Width = lpcrect->right - lpcrect->left;
   r.Height = lpcrect->bottom - lpcrect->top;

   int iSize = min(m_cra.GetSize(),m_da.GetSize());

   Rect rSq(r);

   int iSizeMax = max(r.Width,r.Height);
   int iSizeMin = min(r.Width,r.Height);


   //double dX = min(0.5,max(-0.5,m_dX));
   //double dY = min(0.5,max(-0.5,m_dY));
   double dX = min(100.0, max(-100.0, m_dX));
   double dY = min(100.0,max(-100.0,m_dY));

   double dRadius;

   double rX = (0.5 + fabs(dX))* (double)r.Width;
   double rY = (0.5 + fabs(dY))* (double)r.Height;

   dRadius = sqrt(rX*rX+rY*rY);

   int iSafe = (int)(dRadius / 20.0);

   rSq.X = (r.X - dRadius) + r.Width * (0.5 + dX) - iSafe;
   rSq.Y = (r.Y - dRadius) + r.Height* (0.5 + dY) - iSafe;
   rSq.Width  = dRadius * 2 + iSafe * 2;
   rSq.Height = dRadius * 2 + iSafe * 2;

   CArray<Color> ca;
   CArray<REAL> ra;

   ra.Add(0.0f);
   ca.Add(Color(
      iAlpha,
      GetRValue(m_cra[0]),
      GetGValue(m_cra[0]),
      GetBValue(m_cra[0])));
   int i = 0;
   for(int i = 0; i < iSize; i++)
   {
      if(i == 0 && m_da[0] == 0.f)
      {

         continue;
      }
      else if(i == iSize - 1 && m_da[iSize - 1] == 1.0f)
      {

         continue;
      }

      ra.Add((REAL)m_da[i]);
      ca.Add(Color(
         iAlpha,
         GetRValue(m_cra[i]),
         GetGValue(m_cra[i]),
         GetBValue(m_cra[i])));

   }
   ra.Add(1.0f);
   ca.Add(Color(
      iAlpha, 
      GetRValue(m_cra[iSize - 1]),
      GetGValue(m_cra[iSize - 1]),
      GetBValue(m_cra[iSize - 1])));

   

   GraphicsPath path;

   path.AddEllipse(rSq);

   PathGradientBrush * pb = new PathGradientBrush(&path);

   SolidBrush br(ca[0]);



   pb->SetCenterPoint(Point(rSq.X + rSq.Width / 2.0,rSq.Y + rSq.Height /2.0));

   pb->SetCenterColor(ca[ca.GetSize() - 1]);
   INT iSurroundColorCount = 1;
   pb->SetSurroundColors(&ca[0],&iSurroundColorCount);
      
   pb->SetInterpolationColors(ca.GetData(),ra.GetData(),ca.GetSize());
   
   //GraphicsState s = g.Save();

   //g.SetClip(r,CombineModeIntersect);

   //g.FillRectangle(&br,r);
   //g.FillPath(&b, ppath);

   //g.Restore(s);
   return pb;
}


Gdiplus::Brush *  CGradientPack::linear_brush(LPCRECT lpcrect, int iOpacity)
{

   return linear_brush(lpcrect, m_dRotation, iOpacity);

}


Gdiplus::Brush *  CGradientPack::linear_brush(LPCRECT lpcrect,double dRotation, int iOpacity)
{

   int iAlpha = min(255,max(0,iOpacity));

   //Rect r(rc.left,rc.top,rc.Width(),rc.Height());
   Rect r;

      r.X = lpcrect->left;
      r.Y = lpcrect->top;
      r.Width = lpcrect->right - lpcrect->left;
      r.Height = lpcrect->bottom - lpcrect->top;

   int iSize = min(m_da.GetSize(),m_cra.GetSize());

   CArray<Color> ca;
   CArray<REAL> ra;

   ra.Add(0.0f);
   ca.Add(Color(
      iOpacity,
      GetRValue(m_cra[0]),
      GetGValue(m_cra[0]),
      GetBValue(m_cra[0])));
   int i = 0;
   for(int i = 0; i < iSize; i++)
   {
      if(i == 0 && m_da[0] == 0.f)
      {

         continue;
      }
      else if(i == iSize - 1 && m_da[iSize - 1] == 1.0f)
      {

         continue;
      }

      ra.Add((REAL)m_da[i]);
      ca.Add(Color(
         iOpacity,
         GetRValue(m_cra[i]),
         GetGValue(m_cra[i]),
         GetBValue(m_cra[i])));

   }
   ra.Add(1.0f);
   ca.Add(Color(
      iOpacity,
      GetRValue(m_cra[iSize - 1]),
      GetGValue(m_cra[iSize - 1]),
      GetBValue(m_cra[iSize - 1])));

   const double dPi = atan(1) * 4.0;

   double dAngle = fmod(fmod(dRotation + 270.0,360.0) + 360.0,360.);

   double dT = tan(dAngle * dPi / 180.0);

   bool bSwapX = (dAngle >= 90.0 && dAngle <= 270);

   bool bSwapY = !(dAngle >= 0.0 && dAngle <= 180.0);

   double oX = r.X - 1;

   double oY = r.Y - 1;

   double dH = r.Height + 2;

   double dW = r.Width + 2;

   double x;

   double dA;

   double a;

   double b;

   double y;

   if(dAngle == 180.0)
   {

      x = -dW/2.0;

      y = 0;

   }
   else if(dAngle == 0.0)
   {

      x = dW / 2.0;

      y = 0;

   }
   else
   {
      if(!bSwapY && !bSwapX)
      {

         dA = tan((90.0 + dAngle) * dPi / 180.0);

         a = dW / 2.0;

         b = dH / 2.0;

      }
      else if(!bSwapY && bSwapX)
      {

         dA = tan((dAngle - 90.0) * dPi / 180.0);

         a = - dW / 2.0;

         b = dH / 2.0;

      }
      else if(bSwapY && bSwapX)
      {

         dA = tan((270.0 + dAngle) * dPi / 180.0);

         a = - dW / 2.0;

         b = - dH / 2.0;

      }
      else
      {

         dA = tan((dAngle - 270.0) * dPi / 180.0);

         a = dW / 2.0;

         b = - dH / 2.0;

      }

      x = (b - a * dA) / (dT - dA);

      y = dT * x;

   }

   PointF p1;

   p1.X = oX + dW / 2.0 + x;

   p1.Y = oY + dH / 2.0 + y;

   PointF p2;

   p2.X = oX + dW / 2.0 -x;

   p2.Y = oY + dH / 2.0 - y;

   auto pb =new LinearGradientBrush(p1,p2,ca[0],ca[ca.GetSize() - 1]);

   pb->SetInterpolationColors(ca.GetData(),ra.GetData(),ca.GetSize());

   r.X = min(p1.X,min(p2.X,r.X));
   r.Y = min(p1.Y,min(p2.Y,r.Y));

   int right = max(p2.X,max(p1.X,r.X + r.Width));
   int bottom = max(p2.Y,max(p1.Y,r.Y + r.Height));

   r.Width = right - r.X;
   r.Height = bottom-r.Y;

   //g.FillPath(&br,ppath);

   return pb;


}

HBITMAP CGradientPack::CreateBitmap(int iOpacity)
{

   CDC dc;

   dc.CreateCompatibleDC(NULL);

   CBitmap bmp;

   bmp.CreateCompatibleBitmap(&dc,32,32);

   HGDIOBJ hOld = dc.SelectObject(bmp);

   CRect r;

   r.left = 0;
   r.top = 0;
   r.right = 32;
   r.bottom = 32;

   dc.FillSolidRect(r,RGB(255,255,255));
   Gdiplus::GraphicsPath path;
   Gdiplus::RectF r1;
   __copy(r1, r);
   path.AddRectangle(r1);
   Gdiplus::Graphics g(dc.GetSafeHdc());
   ::std::unique_ptr < Gdiplus::Brush> brush;
   brush.reset(proper_brush(r, iOpacity));

   g.FillPath(brush.get(), &path);

   dc.SelectObject(hOld);

   HGDIOBJ h = bmp.Detach();

   return (HBITMAP) h;
}



Gdiplus::Brush * CGradientPack::proper_brush(LPCRECT lpcrect, int iOpacity)
{

   if(m_bRadial)
   {

      return radial_brush(lpcrect, iOpacity);

   }
   else
   {

      return linear_brush(lpcrect,iOpacity);

   }

}

CGradientPack & CGradientPack::operator = (const CGradientPack & pack)
{

   if(this == &pack)
   {

      return *this;

   }


   m_bRadial = pack.m_bRadial;
   m_da.Copy(pack.m_da);
   m_cra.Copy(pack.m_cra);
   m_dRotation = pack.m_dRotation;
   m_dX = pack.m_dX;
   m_dY = pack.m_dY;

   return *this;

}


bool CGradientPack::operator == (const CGradientPack & pack) const
{

   if(this == &pack)
   {

      return true;

   }


   if(m_bRadial != pack.m_bRadial
      || !almost_equal_grad_rotation(m_dRotation, pack.m_dRotation) // MD6.2.1 - Infinisys Ltd.

      || !almost_equal_grad_rate(m_dX, pack.m_dX) // MD6.2.1 - Infinisys Ltd.

      || !almost_equal_grad_rate(m_dY, pack.m_dY)) // MD6.2.1 - Infinisys Ltd.

      return false;
   
   if(m_da.GetCount() != pack.m_da.GetCount()
      || m_cra.GetCount() != pack.m_cra.GetCount())
      return false;

   for(int i = 0;i < m_da.GetCount(); i++)
   {
      if(!almost_equal_grad_rate(m_da[i], pack.m_da[i]))  // MD6.2.1 - Infinisys Ltd.
      {
         return false;
      }
   }
   for(int i = 0;i < m_cra.GetCount(); i++)
   {
      if(m_cra[i] != pack.m_cra[i])
      {
         return false;
      }
   }
   
   return true;

}


void CGradientPack::Save55(CMemFile & f)
{
   f.Write(&m_bRadial,sizeof(m_bRadial));
   int iSize = min(m_da.GetCount(),m_cra.GetCount());
   f.Write(&iSize,sizeof(iSize));
   for(int i = 0; i < iSize; i++)
   {
      f.Write(&m_da[i],sizeof(m_da[i]));
      f.Write(&m_cra[i],sizeof(m_cra[i]));
   }
   f.Write(&m_dRotation,sizeof(m_dRotation));
   f.Write(&m_dX,sizeof(m_dX));
   f.Write(&m_dY,sizeof(m_dY));
}
void CGradientPack::Load55(CMemFile & f)
{
   f.Read(&m_bRadial,sizeof(m_bRadial));
   int iSize;
   f.Read(&iSize,sizeof(iSize));
   m_da.SetSize(iSize);
   m_cra.SetSize(iSize);
   for(int i = 0; i < iSize; i++)
   {
      f.Read(&m_da[i],sizeof(m_da[i]));
      f.Read(&m_cra[i],sizeof(m_cra[i]));
   }
   f.Read(&m_dRotation,sizeof(m_dRotation));
   f.Read(&m_dX,sizeof(m_dX));
   f.Read(&m_dY,sizeof(m_dY));
}
CArchive& operator >> (CArchive & ar, CGradientPack & gradient)
{
   int iRadial;
   ar >> iRadial;
   gradient.m_bRadial = iRadial != 0;
   int iSize;
   ar >> iSize;
   gradient.m_da.SetSize(iSize);
   gradient.m_cra.SetSize(iSize);
   for(int i = 0; i < iSize; i++)
   {
      ar >> gradient. m_da[i];
      DWORD dw;
         ar >> dw;
      gradient.m_cra[i] = dw;
   }
   ar >> gradient.m_dRotation;
   ar >> gradient.m_dX;
   ar >> gradient.m_dY;
   return ar;
}
CArchive& operator<<(CArchive& ar,CGradientPack& gradient)
{
   int iRadial = gradient.m_bRadial ? 1:0;
   ar << iRadial;
   int iSize = min(gradient.m_da.GetCount(),gradient.m_cra.GetCount());
   ar << iSize;
   for(int i = 0; i < iSize; i++)
   {
      ar << gradient.m_da[i];
      DWORD dw = gradient.m_cra[i];
      ar << dw;
   }
   ar << gradient.m_dRotation;
   ar << gradient.m_dX;
   ar << gradient.m_dY;

   return ar;
}
