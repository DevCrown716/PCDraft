// PackedDIB.h: interface for the CGradientPack class.
// Infinisys Ltd
//
#pragma once

#include <gdiplus.h>

class CGradientPack
{
public:


   bool                 m_bRadial;
   CArray<double>       m_da; // stops 0.0-1.0
   CArray<COLORREF>     m_cra; // colors
   double               m_dRotation; // 0.0 linear gradient top down, increase clockwise
   double               m_dX; // radial percent x 0.0-1.0
   double               m_dY; // radial percent y 0.0-1.0

   CGradientPack();
   virtual ~CGradientPack();

   friend CArchive& operator>>(CArchive& ar,CGradientPack& dib);
   friend CArchive& operator<<(CArchive& ar,CGradientPack& dib);

   Gdiplus::LinearGradientBrush * GetLinearBrush();
   Gdiplus::PathGradientBrush * GetRadialBrush();


   //void draw_linear(HDC hdc,LPCRECT lpcrect,double dRotation,int iOpacity);
   //void draw_linear(HDC hdc,LPCRECT lpcrect,int iOpacity);
   //void draw_radial(HDC hdc,LPCRECT lpcrect,int iOpacity);
   //void draw_proper(HDC hdc,LPCRECT lpcrect,int iOpacity);
   Gdiplus::Brush * linear_brush(LPCRECT lpcrect, double dRotation,int iOpacity);
   Gdiplus::Brush * linear_brush(LPCRECT lpcrect, int iOpacity);
   Gdiplus::Brush * radial_brush(LPCRECT lpcrect, int iOpacity);
   //void draw_proper(Gdiplus::Graphics & g, int iOpacity);
   Gdiplus::Brush * proper_brush(LPCRECT lpcrect, int iOpacity);

   WORD PaletteSize(LPSTR lpDib);
   WORD DIBNumColors(LPSTR lpDib);
   LPSTR FindDIBBits(LPSTR lpDib);
   void FreeDIB();

   void reset();

   HBITMAP CreateBitmap(int iOpacity);

   CGradientPack & operator = (const CGradientPack & pack);

   bool operator == (const CGradientPack & pack) const;

   void Save55(CMemFile & f);
   void Load55(CMemFile & f);

};



CArchive& operator>>(CArchive& ar, CGradientPack & pack);
CArchive& operator<<(CArchive& ar, CGradientPack & pack);

