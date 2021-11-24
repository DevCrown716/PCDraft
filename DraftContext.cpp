// DraftContext.cpp: implementation of the CDraftContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "pcdraft.h"
#include "DraftContext.h"

#include "phmath.h"
#include "DocMapper.h"
#include "PCDraftDoc.h"
#include "StyledText.h"
#include "MDLineDim.h"
#include "MDAngularDim.h"
#include "MDRadialDim.h"

#include <math.h>

#define COMPOSITION_UNDERLINE 1
#define COMPOSITION_FULL_UNDERLINE_ON_END 0

bool segment_intersects(int & a1, int & a2, int b1, int b2)
{

   int A1 = min(a1, a2);
   int A2 = max(a1, a2);
   int B1 = min(b1, b2);
   int B2 = max(b1, b2);
   int I1 = max(A1, B1);
   int I2 = min(A2, B2);
   if (I1 > I2)
   {
      return false;
   }
   a1 = I1;
   a2 = I2;
   return true;

}



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
extern struct
{
   unsigned char num;
   unsigned char seg[15];
} defaultDashStyles[16];

//////////////////////////////////////////////////////////////////////
HRGN BitmapToRegion(HBITMAP hBmp,COLORREF cTransparentColor = 0);
void DrawTransparent(CDC * pDC,CBitmap *pBMP,int x,int y,COLORREF crColour);
void DrawTransparent(CDC * pDC,int x,int y,int nWidth,int nHeight,CBitmap *pBMP,int xSrc,int ySrc,int nSrcWidth,int nSrcHeight,COLORREF crColour);
bool is_breaking_space(int i)
{

   return i == ' ' || i == '\r' || i == '\n' || i == '\t';

}
CDraftContext::CDraftContext(CDC* pDC,CMapper* pMapper,CPCDraftDoc* pDoc, CVersionDoc * pVersionDoc): m_pDC(pDC),m_pMapper(pMapper),m_pDoc(pDoc),m_pVersionDoc(pVersionDoc), m_pDimStd(NULL)
{

   m_DrawPrim.m_bUsePolyHelperIfDashed = false;
   m_bFinalizeDash = false;
   m_bSwapInsideOutside = false;
   m_bCheckRectVisible = true;
   m_iPathAccumul = 0;
   m_ecompositionstate = e_composition_state_none;
   m_bPrinting = false;
   m_bUseGDI = false;
   m_prect = nullptr;
   m_bOutlineForFill = false;
   m_pfont = nullptr;
   m_ppath = nullptr;
   m_iPathCount = 0;
   m_bTransparentBackground = false; // Interface Update - Infinisys Ltd.
   if(m_pVersionDoc == NULL)
   {
      m_pVersionDoc = pDoc;
   }
   if(m_pDoc) m_DrawPrim.setDashTable((const CDrawPrim::DashTable*)m_pDoc->m_DashStyles);
   else m_DrawPrim.setDashTable((const CDrawPrim::DashTable*)defaultDashStyles);
   m_pOldFont=NULL;
   m_TextColor=RGB(0,0,0);
   m_bCreatingPath=false;
   m_bOutline=false;
   m_bGreyMode=false;
   m_bJoint=false;
   m_pgraphics = nullptr;
}

CDraftContext::~CDraftContext()
{
   if (m_ppath)
   {

      delete m_ppath;
      m_ppath = nullptr;

   }
   if (m_pgraphics)
   {

      delete m_pgraphics;
      m_pgraphics = nullptr;

   }
}

#undef new

void CDraftContext::graphics_on()
{

   if (!m_pgraphics)
   {

      m_pgraphics = new Gdiplus::Graphics(m_pDC->GetSafeHdc());

      graphics_high();


      if (m_pgraphics)
      {

         if (!m_bPrinting)
         {

            m_pgraphics->SetPageScale(1.0f);

         }

         m_pgraphics->SetPageUnit(Gdiplus::UnitPixel);

      }


   }

}


void CDraftContext::graphics_high()
{

   m_pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   m_pgraphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
   m_pgraphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
   m_pgraphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

}


void CDraftContext::graphics_joint()
{

   m_pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias8x8);
   m_pgraphics->SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
   m_pgraphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
   m_pgraphics->SetInterpolationMode(Gdiplus::InterpolationModeLowQuality);

}

void CDraftContext::graphics_off()
{

   if (m_pgraphics)
   {

      delete m_pgraphics;
      m_pgraphics = nullptr;

   }

}

void CDraftContext::path_new(bool bPathAccumul, Gdiplus::FillMode fillMode /*= FillModeAlternate*/)
{

   if (bPathAccumul)
   {

      m_iPathAccumul = true;

   }

   if (!m_iPathAccumul)
   {

      path_destroy(false);

   }

   if (m_ppath == nullptr)
   {

      m_ppath = new Gdiplus::GraphicsPath(fillMode);

      m_ppath->StartFigure();

   }

   if (m_iPathAccumul <= 0)
   {
    
      m_bCreatingPath = true;

   }

}


bool  CDraftContext::path_destroy(bool bCloseAccumul)
{

   if (bCloseAccumul)
   {

      m_iPathAccumul--;

   }

   if (m_iPathAccumul <= 0)
   {

      m_iPathAccumul = 0;

      if (m_ppath)
      {


            delete m_ppath;

      }

      m_ppath = nullptr;

      m_bCreatingPath = false;

      if (bCloseAccumul)
      {

         return true;

      }

   }

   return false;

}


Gdiplus::GraphicsPath * CDraftContext::path_detach()
{

   auto ppath = m_ppath;

   m_ppath = nullptr;

   m_bCreatingPath = false;

   return ppath;

}

void CDraftContext::SetFont(const LOGFONT& lf)
{
   if(m_Font.GetSafeHandle()) m_Font.DeleteObject();
   m_Font.CreateFontIndirect(&lf);
}

void CDraftContext::SetTextParams(const TextParams& tp)
{
   m_TextParams=tp;
}


void CDraftContext::SetPen(const PenModel &pm)
{
   ResetJoint();
   m_bOutline = false;
   //if(m_Pen.GetSafeHandle()) m_Pen.DeleteObject();
   //if(m_PenBrush.GetSafeHandle()) m_PenBrush.DeleteObject();
   //if(m_PenPlus != NULL) m_PenPlus.release();
   // if (m_PenBrushPlus != NULL) m_PenPlus.release();

   m_PenModel=pm;
   if(m_bGreyMode)
   {
      if(m_PenModel.m_nPenType == kPenPatEmbed) DeleteObject(m_PenModel.m_hDib);
      m_PenModel.m_nPenType=kPenColor;
      m_PenModel.m_crColor=RGB(210,210,210);
      m_PenModel.m_nStyle=0;
   }

   bool bUseBrush=false;
   int nWidth=m_pMapper->ScreenPenW(m_PenModel.m_nWidth);
   bool useDash =  shouldUseDash();
   m_iPackedItemPenFill = -1;
   if(m_PenModel.m_nPenType == kPenInvis)
   {
  //    m_Pen.CreateStockObject(NULL_PEN);
//      m_PenBrush.CreateStockObject(HOLLOW_BRUSH);
#undef new
      m_PenBrushPlus.reset(new Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0,0)));
      m_PenPlus.reset(new Gdiplus::Pen(Gdiplus::Color(0, 0, 0, 0), nWidth));
      m_colorPen.SetValue(Gdiplus::Color::MakeARGB(0, 0, 0, 0));
   }
   else
   {
      CBitmap bmp;
      int nPenStyle = PS_GEOMETRIC | PS_JOIN_ROUND;
      int val = (m_PenModel.isPenStyleRound() ? PS_ENDCAP_ROUND: PS_ENDCAP_FLAT);
      nPenStyle |=  val;

      switch(m_PenModel.m_nPenType)
      {
      case kPenColor:
         m_TextColor=m_PenModel.m_crColor;
         COLORREF cr;
         cr = m_PenModel.m_crColor;

         if(cr & 0x01000000)
         {

            PALETTEENTRY palEntry;

            if(m_pDoc != NULL) // Interface Update - Infinisys Ltd.
            {
               m_pDoc->m_Palette.GetPaletteEntries(cr & 0xffff,1,&palEntry);

            }

            cr = RGB(palEntry.peRed,palEntry.peGreen,palEntry.peBlue);

         }

         // Commented for GDI+
//         if(m_PenModel.m_iPenOpacity < 255 || m_bTransparentBackground) // Interface Update - Infinisys Ltd.
         {
#undef new

            

            m_PenBrushPlus.reset(new Gdiplus::SolidBrush(
                Gdiplus::Color(
                  (BYTE)m_PenModel.m_iPenOpacity,
                  (BYTE)GetRValue(cr),
                  (BYTE)GetGValue(cr),
                  (BYTE)GetBValue(cr))));

            m_PenPlus.reset(new Gdiplus::Pen(Gdiplus::Color(
               m_PenModel.m_iPenOpacity,
               (BYTE)GetRValue(cr),
               (BYTE)GetGValue(cr),
               (BYTE)GetBValue(cr)), nWidth));

            m_colorPen.SetValue(Gdiplus::Color::MakeARGB(m_PenModel.m_iPenOpacity,
               (BYTE)GetRValue(cr), (BYTE)GetGValue(cr), (BYTE)GetBValue(cr)));
         }
         //else
         {
            //			if(useDash || m_PenModel.AnyCaps())
            {
//               m_PenBrush.CreateSolidBrush(cr);
            }
            //temporaraly, should also decide question with patterns
            //			if(useDash) { 

            //	m_Pen.CreateStockObject(NULL_PEN); 
            //			}
            //			else
         {
            //m_Pen.CreatePen(PS_GEOMETRIC|PS_ENDCAP_FLAT,nWidth,m_PenModel.m_crColor);
        //    LOGBRUSH lb;
      //      lb.lbStyle=BS_SOLID;
    //        lb.lbColor=cr;
  //          lb.lbHatch=0;
//            m_Pen.CreatePen(nPenStyle,nWidth,&lb);
         }
         }
         break;
      case kPenPat:
         if(m_pDoc)
         {

            CRect rect;

            rect = m_pMapper->LPtoDP(*m_prect);


            m_PenBrushPlus.reset(m_pDoc->m_packeditema[m_PenModel.m_nIndex].proper_brush(rect, m_PenModel.m_iPenOpacity));

            m_PenPlus.reset(new Gdiplus::Pen(m_PenBrushPlus.get(), nWidth));

            m_colorPen.SetValue(Gdiplus::Color::MakeARGB(255,
               (BYTE)GetRValue(0), (BYTE)GetGValue(0), (BYTE)GetBValue(0)));


            //m_pbitmapPenPattern.release();
            //if(m_PenModel.m_nIndex >= 0
            //   && m_PenModel.m_nIndex < m_pDoc->m_packeditema.GetCount()
            //   && m_pDoc->m_packeditema[m_PenModel.m_nIndex].m_etype == packed_type_gradient)
            //{

            //   m_iPackedItemPenFill = m_PenModel.m_nIndex;

            //}
            //else
            //{
            //   ///m_pbitmapPenPattern.reset();
            //   // m_PenPattern.DeleteObject();

            //   HBITMAP hbitmap = nullptr;

            //   if(m_pDC->IsPrinting())
            //   {

            //      hbitmap = m_pDoc->m_packeditema[m_PenModel.m_nIndex].m_pdib->CreateDIBSection();

            //   }
            //   else
            //   {

            //      hbitmap = m_pDoc->GetPatternBitmap(m_PenModel.m_nIndex, m_PenModel.m_iPenOpacity);

            //   }

            //   Gdiplus::Bitmap * pbitmap = new Gdiplus::Bitmap(hbitmap, nullptr);

            //   m_pbitmapPenPattern.reset(pbitmap);
            //   // TODO GDI+
            //   //m_PenBrush.CreatePatternBrush(&m_PenPattern);
            //   //if(!useDash)
            //   //{
            //   //   LOGBRUSH lb;
            //   //   lb.lbStyle=BS_PATTERN;
            //   //   lb.lbColor=0;
            //   //   lb.lbHatch=(LONG)m_PenPattern.GetSafeHandle();
            //   //   m_Pen.CreatePen(nPenStyle,nWidth,&lb);
            //   //}
            //   // END TODO GDI+
            //}

            //if (m_pbitmapPenPattern.get())
            //{

            //   m_PenBrushPlus.reset(new Gdiplus::TextureBrush(m_pbitmapPenPattern.get()));

            //}
            //else
            //{

            //   m_PenBrushPlus.release();

            //}

         }
         break;
      case kPenPatEmbed:
      {
         m_pbitmapPenPattern.release();
         m_colorPen.SetValue(Gdiplus::Color::MakeARGB(255,
            (BYTE)GetRValue(0), (BYTE)GetGValue(0), (BYTE)GetBValue(0)));


         HBITMAP hbitmap = nullptr;

         if (m_pDC->IsPrinting())
         {

            hbitmap = CopyDIB(m_PenModel.m_hDib);

         }
         else
         {
            DIBSECTION ds;
            GetObject(m_PenModel.m_hDib,sizeof(DIBSECTION),&ds);
            if(ds.dsBmih.biBitCount == 1) ds.dsBitfields[1]=0xFFFFFF;		// tweak
            hbitmap =CreateDIBitmap(m_pDC->GetSafeHdc(),&ds.dsBmih,CBM_INIT,ds.dsBm.bmBits,(BITMAPINFO*)&ds.dsBmih,DIB_RGB_COLORS);

         }

         Gdiplus::Bitmap * pbitmap = nullptr;

         if (hbitmap)
         {

            pbitmap = new ::Gdiplus::Bitmap(hbitmap, nullptr);

         }

         if (pbitmap)
         {

            m_pbitmapPenPattern.reset(pbitmap);

         }

         if (m_pbitmapPenPattern.get())
         {

            m_PenBrushPlus.reset(new Gdiplus::TextureBrush(m_pbitmapPenPattern.get()));

         }
         else
         {

            m_PenBrushPlus.release();

         }

         // TODO GDI+
         /*m_PenBrush.CreatePatternBrush(&m_PenPattern);
         if(!useDash)
         {
            LOGBRUSH lb;
            lb.lbStyle=BS_PATTERN;
            lb.lbColor=0;
            lb.lbHatch=(LONG)m_PenPattern.GetSafeHandle();
            m_Pen.CreatePen(nPenStyle,nWidth,&lb);
         }
         */
         // END TODO GDI+
      }
      break;
      default:
         ASSERT(FALSE);
         break;
      }
   }
   if (m_PenPlus.get() != nullptr)
   {

      m_PenPlus.get()->SetAlignment(Gdiplus::PenAlignmentInset);

   }
   //if(m_iPackedItemPenFill < 0)
   //{
   //   if(!m_PenBrush.GetSafeHandle()) m_PenBrush.CreateStockObject(BLACK_BRUSH);
   //   if(!m_Pen.GetSafeHandle()) m_Pen.CreateStockObject(BLACK_PEN);

   //}
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

}

void CDraftContext::SetFill(const FillModel &fm)
{

   if (m_bUseGDI)
   {

      //if (m_Pen.GetSafeHandle())
      //{

      //   m_Pen.DeleteObject();

      //}
      //
      //m_Pen.CreateStockObject(BLACK_PEN);
      //
      //if (m_PenBrush.GetSafeHandle())
      //{

      //   m_PenBrush.DeleteObject();

      //}

   }

   PenModel pmDummy;	// temp fix
   m_PenModel=pmDummy;
   m_FillModel=fm;
   m_iPackedItemFill = -1;

   if (m_bUseGDI)
   {
      //if (m_Brush.GetSafeHandle()) m_Brush.DeleteObject();

   }
   
#undef new
   if (m_bGreyMode || fm.m_nFillType == kNoFill)
   {

      if (m_bUseGDI)
      {

//         m_Brush.CreateStockObject(HOLLOW_BRUSH);

      }
      else
      {

         m_BrushPlus.reset(new Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0, 0)));

      }

   }
   else
   {
      //		CBitmap bmp;
      switch(fm.m_nFillType)
      {
      case kFillColor:
         if (m_FillModel.m_crColor & PALETTEINDEX(0))
         {
            PALETTEENTRY pe;
            m_pDoc->m_Palette.GetPaletteEntries(m_FillModel.m_nIndex & 0xFF, 1, &pe);
            if (m_bUseGDI)
            {
               //m_Brush.CreateSolidBrush(RGB(pe.peRed, pe.peGreen, pe.peBlue));
               
            }
            else
            {
               m_BrushPlus.reset(new Gdiplus::SolidBrush(Gdiplus::Color(m_FillModel.m_iFillOpacity, pe.peRed, pe.peGreen, pe.peBlue)));
            }
         }
         else
         {

            if (m_bUseGDI)
            {

               //m_Brush.CreateSolidBrush(fm.m_crColor);

            }
            else
            {

               m_BrushPlus.reset(new Gdiplus::SolidBrush(Gdiplus::Color(m_FillModel.m_iFillOpacity, GetRValue(fm.m_crColor), GetGValue(fm.m_crColor), GetBValue(fm.m_crColor))));;

            }

         }
         break;
      case kFillPat:
         if(m_pVersionDoc)
         {
            //m_FillPattern.DeleteObject();
            //m_pbitmapFillPattern.release();
            CRect rect;

            rect = m_pMapper->LPtoDP(*m_prect);

            if (fm.m_nIndex >= 0 && fm.m_nIndex < m_pVersionDoc->m_packeditema.GetCount())
            {

               Gdiplus::Brush * pbrush = m_pVersionDoc->m_packeditema[fm.m_nIndex].proper_brush(
                  rect, fm.m_iFillOpacity);

               m_BrushPlus.reset(pbrush);

            }

            //if(fm.m_nIndex >= m_pVersionDoc->m_packeditema.GetCount()
            //   || m_pVersionDoc->m_packeditema[fm.m_nIndex].m_etype == packed_type_dib)
            //{
            //   HBITMAP hbitmap = nullptr;
            //   if(m_pDC->IsPrinting()
            //      && fm.m_nIndex < m_pVersionDoc->m_packeditema.GetCount()
            //      && m_pVersionDoc->m_Patterns[fm.m_nIndex].GetHandle())
            //   {
            //      hbitmap = m_pVersionDoc->m_Patterns[fm.m_nIndex].CreateDIBSection();
            //   }
            //   else
            //   {

            //      hbitmap = m_pVersionDoc->GetPatternBitmap(fm.m_nIndex, fm.m_iFillOpacity);
            //      //&m_FillPattern
            //   // TODO GDI+

            //   //m_Brush.CreatePatternBrush(&m_FillPattern);
            //   // END TODO GDI+
            //   }

            //   if (hbitmap)
            //   {

            //      m_pbitmapFillPattern.reset(new Gdiplus::Bitmap(hbitmap, nullptr));

            //   }

            //   if (m_FillModel.m_iFillOpacity < 255)
            //   {

            //      Gdiplus::Bitmap * pbitmap = new Gdiplus::Bitmap(m_pbitmapFillPattern->GetWidth(),
            //         m_pbitmapFillPattern->GetHeight());

            //      Gdiplus::ColorMatrix colorMatrix = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            //                                 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            //                                 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            //                                 0.0f, 0.0f, 0.0f, m_FillModel.m_iFillOpacity /255.f, 0.0f,
            //                                 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
            //      // Create an ImageAttributes object and set its color matrix.
            //      Gdiplus::ImageAttributes imageAtt;
            //      imageAtt.SetColorMatrix(&colorMatrix, Gdiplus::ColorMatrixFlagsDefault,
            //         Gdiplus::ColorAdjustTypeBitmap);

            //      Gdiplus::Graphics g(pbitmap);

            //      g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);

            //      Gdiplus::SolidBrush br(Gdiplus::Color(0, 0, 0, 0));
            //      Gdiplus::RectF r(0, 0, pbitmap->GetWidth(), pbitmap->GetHeight());

            //      g.FillRectangle(&br, r);

            //      g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

            //      g.DrawImage(m_pbitmapFillPattern.get(), &r, nullptr, nullptr, &imageAtt, Gdiplus::UnitPixel);

            //      m_pbitmapFillPattern.reset(pbitmap);

            //   }

            //   if (m_pbitmapFillPattern.get())
            //   {

            //      m_BrushPlus.reset(new Gdiplus::TextureBrush(m_pbitmapFillPattern.get()));

            //   }


            //}
            //else
            //{
            //   m_iPackedItemFill = fm.m_nIndex;
            //}

         }
         break;
      case kFillPatEmbed:
      {
         m_pbitmapFillPattern.release();
         //m_FillPattern.DeleteObject();
         HBITMAP hbitmap = nullptr;
         if (m_pDC->IsPrinting())
         {
            hbitmap = CopyDIB(fm.m_hDib);
         }
         else
         {
            DIBSECTION ds;
            GetObject(fm.m_hDib,sizeof(DIBSECTION),&ds);
            if(ds.dsBmih.biBitCount == 1) ds.dsBitfields[1]=0xFFFFFF;		// tweak
            HBITMAP hBmp=CreateDIBitmap(m_pDC->GetSafeHdc(),&ds.dsBmih,CBM_INIT,ds.dsBm.bmBits,(BITMAPINFO*)&ds.dsBmih,DIB_RGB_COLORS);
            hbitmap = hBmp;
         }
         //m_Brush.CreatePatternBrush(&m_FillPattern);
         // TODO GDI+

         if (hbitmap)
         {

            m_pbitmapFillPattern.reset(new Gdiplus::Bitmap(hbitmap, nullptr));

         }

         if (m_pbitmapFillPattern.get())
         {

            m_BrushPlus.reset(new Gdiplus::TextureBrush(m_pbitmapFillPattern.get()));

         }



//m_Brush.CreatePatternBrush(&m_FillPattern);
// END TODO GDI+

      }
      break;
      default:
         ASSERT(FALSE);
         break;
      }
   }
   if (!m_BrushPlus.get())
      m_BrushPlus.reset(new Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0, 0)));
}

void CDraftContext::MoveTo(const LongPoint &lpt)
{

   if (m_bUseGDI)
   {

      auto pt = m_pMapper->LPtoDP(lpt);

      if (m_pDC->GetCurrentPosition() != pt)
      {

         m_pDC->MoveTo(pt);
         ResetJoint();

      }

      //if (m_ppath)
      //{

      //   m_ppath->StartFigure();

      //}

   }

   m_lptCurrent=lpt;

}


void CDraftContext::ResetJoint()
{
   m_bJoint=false;
}

void CDraftContext::OpenJoint(const LongPoint& lpt, pointd* pt)
{
   if(!m_bOutline)
   {
      m_lptJoint[0]=lpt;
      m_ptJoint[0]=pt[0];
      m_ptJoint[1]=pt[1];
      m_bJoint=true;
   }
}

void CDraftContext::CloseJoint(const LongPoint& lpt, pointd* pt)
{
   if(!m_bOutline)
   {
      if(m_bJoint)
      {
         m_bJoint=false;
         if(pt)
         {
            //			if(lpt!=m_lptJoint[0]) return;
            m_ptJoint[2]=pt[0];
            m_ptJoint[3]=pt[1];
         }
         else
         {
            //			if(lpt!=m_lptJoint[1]) return;
            m_ptJoint[2]=m_ptJoint[4];
            m_ptJoint[3]=m_ptJoint[5];
         }
//         CRgn rgn;
  //       rgn.CreatePolygonRgn(m_ptJoint,4,ALTERNATE);
    //     PaintPenRgn(&rgn);
         path_new(false);
         // rgn.CreatePolygonRgn(pt, 4, ALTERNATE);
          //pointd ptJoint2[4];


          //memcpy(ptJoint2, m_ptJoint, sizeof(ptJoint2));
          //pointd p(0., 0.);
          //for (int i = 0; i < 4; i++)
          //{
          //   p.x += ptJoint2[i].x;
          //   p.y += ptJoint2[i].y;
          //}
          //p.x /= 4.0;
          //p.y /= 4.0;
          //for (int i = 0; i < 4; i++)
          //{
          //   ptJoint2[i].x -=p.x;
          //   ptJoint2[i].y -=p.y;
          //}
          //for (int i = 0; i < 4; i++)
          //{
          //   ptJoint2[i].x *= 1.25;
          //   ptJoint2[i].y *= 1.25;
          //}

          //for (int i = 0; i < 4; i++)
          //{
          //   ptJoint2[i].x += p.x;
          //   ptJoint2[i].y += p.y;
          //}

          ////m_ppath->Transform(&m);

          pointf ptJoint[4];

          if (m_iPathAccumul <= 0)
          {

             //__copy(ptJoint, ptJoint2, 4);
             __copy(ptJoint, m_ptJoint, 4);
             m_ppath->AddPolygon(ptJoint, 4);
             graphics_joint();
             PaintPenPath();
             graphics_high();

          }
      }
      else
      {
         if(pt)
         {
            m_lptJoint[1]=lpt;
            m_ptJoint[4]=pt[0];
            m_ptJoint[5]=pt[1];
         }
      }
   }
}

void CDraftContext::DrawLine(const LongPoint &lpt1,const LongPoint &lpt2)
{

   if (lpt1 == lpt2)
   {

      return;

   }


   short nPenW=m_pMapper->ScreenPenW(m_PenModel.m_nWidth);
   // by TSM
   //if(!IsPenPatternFillEx() && (m_bCreatingPath || m_bOutline || (nPenW == 1 && m_PenModel.m_nPenType < kPenPat)))

   //if (m_bCreatingPath || m_bOutline || (m_PenModel.m_nPenType < kPenPat))
   if (m_bCreatingPath || m_bOutline || nPenW == 1)
   //   if((m_bCreatingPath || m_bOutline || (nPenW == 1 && m_PenModel.m_nPenType < kPenPat)))
      //if(m_bCreatingPath || m_bOutline || (m_PenModel.m_nPenType<kPenPat))
   {
      if(m_PenPlus == NULL || m_bOutline)
      {
         
         pointf pt = m_pMapper->LPtoDPF(lpt1);
         
         pointf pt2 = m_pMapper->LPtoDPF(lpt2);

         if (m_ppath)
         {

            m_ppath->AddLine(pt.X, pt.Y, pt2.X, pt2.Y);

         }
         else if (m_bUseGDI)
         {

            if (m_pDC->GetCurrentPosition() != pt) m_pDC->MoveTo(pt);

            m_pDC->LineTo(m_pMapper->LPtoDP(lpt2));

         }
         else
         {

            graphics_on();

            if (!m_PenPlus.get())
            {

               m_PenPlus.reset(new Gdiplus::Pen(Gdiplus::Color::Black, 1.0f));

            }

            m_pgraphics->DrawLine(m_PenPlus.get(), pt.X, pt.Y, pt2.X, pt2.Y);

         }
      }
      else
      {
         pointd pt1;
         pointd pt2;
         m_pMapper->LPtoDP(pt1, lpt1);
         m_pMapper->LPtoDP(pt2, lpt2);
         pointf pf1;
         pointf pf2;
         pf1 =m_pMapper->LPtoDP(lpt1);
         pf2=m_pMapper->LPtoDP(lpt2);
         graphics_on();
         //m_pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
         //m_pgraphics->SetCompositingQuality(Gdiplus::CompositingQualityGammaCorrected);
         //m_pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
         // m_pgraphics->SetCompositingQuality(Gdiplus::CompositingQualityGammaCorrected);
         m_pgraphics->DrawLine(m_PenPlus.get(), (FLOAT) pt1.x, (FLOAT)pt1.y, (FLOAT)pt2.x, (FLOAT)pt2.y);
      }
   }
   else
   {

      LongPoint pp[4];

      guParallelLn(lpt1,lpt2,m_pMapper->PenW(m_PenModel.m_nWidth),kCenterLine,pp,pp + 3,pp + 1,pp + 2);
      pointd pa[4];
      for(int i=0;i < 4;i++) m_pMapper->LPtoDP(pa[i], pp[i]);
      //if(pa[0].x == pa[1].x && pa[0].y == pa[1].y)
      //{
        // pa[1].x--;
         //pa[1].y--;
      //}
      //if(pa[3].x == pa[2].x && pa[3].y == pa[2].y)
      //{
        // pa[2].x--;
         //pa[2].y--;
      //}

      if(!m_PenModel.isPenStyleRound()){
         CloseJoint(lpt1,pa);
         OpenJoint(lpt2,pa + 2);
#undef new
         //m_ppath = new Gdiplus::GraphicsPath();
         UsePath usepath(this);
         
         // rgn.CreatePolygonRgn(pt, 4, ALTERNATE);

         pointf pfa[4];

         __copy(pfa, pa, 4);

         m_ppath->AddPolygon(pfa, 4);

         PaintPenPath();

      }
      else{
         CRgn rgn;
         ResetJoint();
         graphics_off();
         CGDIArcDirection dir(m_pDC,AD_CLOCKWISE);

         CPoint pt[4];
         pt[0].x = pa[0].x;
         pt[0].y = pa[0].y;
         pt[1].x = pa[1].x;
         pt[1].y = pa[1].y;
         pt[2].x = pa[2].x;
         pt[2].y = pa[2].y;
         pt[3].x = pa[3].x;
         pt[3].y = pa[3].y;

         if(m_pDC->GetCurrentPosition() != pt[0]) m_pDC->MoveTo(pt[0]);

         m_pDC->BeginPath();

         WrFixed halfWidth = m_pMapper->PenW(m_PenModel.m_nWidth) / 2;

         // Force new settings
         int nAlignment = m_PenModel.m_nAlignment;
         m_PenModel.m_nAlignment = kAlignCenter;
         bool oldOutline = m_bOutline;
         m_bOutline=true;
         
         UsePath usepath(this);

         DrawArc(lpt1,LongPoint(halfWidth,halfWidth),pp[0],pp[1],AD_CLOCKWISE,0,0,0,false);
         m_pDC->LineTo(pt[2]);

         DrawArc(lpt2,LongPoint(halfWidth,halfWidth),pp[2],pp[3],AD_CLOCKWISE,0,0,0,false);

         // restore settings
         m_bOutline = oldOutline;
         m_PenModel.m_nAlignment = nAlignment;

         m_pDC->LineTo(pt[0]);
         m_pDC->EndPath();
         //if(rgn.CreateFromPath(m_pDC)){
         //   CPoint ptOrg;
         //   GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
         //   rgn.OffsetRgn(-ptOrg);	// temporary fix, should not be here
         //}
         PaintPenPath();
      }
   }
   m_lptCurrent=lpt2;
}

void CDraftContext::DrawLineCaps( const LongPoint &lpt,short cap1,short cap2,short border)
{
   CPoint pt[2];
   LongPoint lPt1,lPt2,trueLPt1,trueLPt2;

   bool useDash =  shouldUseDash();

   lPt1 = m_lptCurrent;
   lPt2 = lpt;

   trueLPt1 = lPt1;
   trueLPt2 = lPt2;

   if(cap1 || cap2)
   {
      m_DrawPrim.lineCapped(m_pMapper,&lPt1,&lPt2,cap1,cap2,kBothEnd,m_PenModel.m_nWidth & 0x1F,&lPt1,&lPt2);
   }
   if(m_PenModel.m_nAlignment != kCenterLine && border && !m_DrawPrim.m_bUsePolyHelperIfDashed)
      m_DrawPrim.LineBorderPos(m_pMapper,&m_PenModel,cap1,cap2,border,&lPt1,&lPt2,&lPt1,&lPt2);

   if(useDash && !IsOutline())
   {
      pointd p1;
      pointd p2;
      m_pMapper->LPtoDP(p1, lPt1);
      m_pMapper->LPtoDP(p2, lPt2);
      m_DrawPrim.dashLine(this,p1,p2); // uses brush
   }
   else DrawLine(lPt1,lPt2);

   if(!m_bGreyMode && (cap1 || cap2))
   {
      if(cap1)
      {
         m_DrawPrim.drawEndcap(this,trueLPt1,trueLPt2,cap1,kEnd1 /*| kDimEndmarkOut*/,m_PenModel,false,false,kDimLineOut);
      }
      if(cap2)
      {
         m_DrawPrim.drawEndcap(this,trueLPt1,trueLPt2,cap2,kEnd2 /*| kDimEndmarkOut*/,m_PenModel,false,false,kDimLineOut);
      }
      graphics_off();
      m_pDC->MoveTo(m_pMapper->LPtoDP(lpt));
   }

   m_lptCurrent=lpt;
}

void CDraftContext::LineTo(const LongPoint &lpt,short cap1,short cap2,short border)
{
   // TODO GDI+
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
   //CGDIObject<CBrush> gdiBrush(m_pDC,&m_PenBrush);
   // END TODO GDI+
   if(cap1 < 0) cap1=m_PenModel.EndCap1();
   if(cap2 < 0) cap2=m_PenModel.EndCap2();
   DrawLineCaps( lpt,cap1,cap2,border);
}

void CDraftContext::OutsideLineTo(const LongPoint &lptTo)
{
   if(m_bOutline) LineTo( lptTo);
   else
   {
      ResetJoint();
      short nSavedAlign=m_PenModel.m_nAlignment;
      m_PenModel.m_nAlignment=m_PenModel.isPenStyleRound() ? kCenterLine : kOutsideLine;
      LineTo( lptTo,0,0,kBothEnd);
      m_PenModel.m_nAlignment=nSavedAlign;
      ResetJoint();
   }
}

void CDraftContext::Line(const LongPoint &lptFrom,const LongPoint &lptTo,short cap1,short cap2,short border)
{
   if(!m_PenModel.isPenStyleRound()) m_PenModel.m_nAlignment++;
   MoveTo(lptFrom);
   LineTo(lptTo,cap1,cap2,border);
   if(!m_PenModel.isPenStyleRound()) m_PenModel.m_nAlignment--;
}

void CDraftContext::ParallelLine(const LongPoint& lptFrom,const LongPoint& lptTo,const ParallelAttrs& pa)
{
   LongPoint lPt1=lptFrom;
   LongPoint lPt2=lptTo;
   if(m_PenModel.m_nAlignment != kCenterLine && pa.m_nEndCap)
      m_DrawPrim.LineBorderPos(m_pMapper,&m_PenModel,pa.m_nEndCap & 1?0:1,pa.m_nEndCap & 2?0:1,kBothEnd,&lPt1,&lPt2,&lPt1,&lPt2);
   LongPoint pp1,pp2,pp3,pp4;
   guParallelLn(lPt1,lPt2,pa.m_fWidth,LineAttribute(pa.m_nAlign),&pp1,&pp2,&pp3,&pp4);
   MoveTo(pp1);
   LineTo(pp2,0,0,(pa.m_nEndCap & 1?0:kEnd1) | (pa.m_nEndCap & 2?0:kEnd2));
   ResetJoint();
   if(pa.m_nEndCap & 2){
      OutsideLineTo(pp4);
   }
   else MoveTo(pp4);
   LineTo(pp3,0,0,(pa.m_nEndCap & 1?0:kEnd2) | (pa.m_nEndCap & 2?0:kEnd1));
   ResetJoint();
   if(pa.m_nEndCap & 1){
      OutsideLineTo(pp1);
   }
}

void CDraftContext::ParallelCaps(const LongPoint& lptFrom,const LongPoint& lptTo,const ParallelAttrs& pa,short border)
{
   LongPoint lPt1=lptFrom;
   LongPoint lPt2=lptTo;
   if(m_PenModel.m_nAlignment != kCenterLine && pa.m_nEndCap)
      m_DrawPrim.LineBorderPos(m_pMapper,&m_PenModel,pa.m_nEndCap & 1?0:1,pa.m_nEndCap & 2?0:1,border,&lPt1,&lPt2,&lPt1,&lPt2);
   LongPoint pp1,pp2,pp3,pp4;
   guParallelLn(lPt1,lPt2,pa.m_fWidth,LineAttribute(pa.m_nAlign),&pp1,&pp2,&pp3,&pp4);

   if(pa.m_nEndCap & 1 && border&kEnd1)
   {
      MoveTo(pp3);
      OutsideLineTo(pp1);
   }
   if(pa.m_nEndCap & 2 && border&kEnd2)
   {
      MoveTo(pp2);
      OutsideLineTo(pp4);
   }
}


void CDraftContext::PolyLine(const LongPointArray& pt,int nCount,bool bClosed,short cap1,short cap2)
{

   // TODO GDI+
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
   //CGDIObject<CBrush> gdiBrush(m_pDC,&m_PenBrush);
   // END TODO GDI+

   m_DrawPrim.m_bUsePolyHelperIfDashed = true;

   bool bOutline = m_bCreatingPath || m_bOutline;

   if (bOutline && m_ppath != nullptr)
   {

      CArray < pointf > pta;

      pta.SetSize(nCount);

      for (int i = 0; i < nCount; i++)
      {

         pointd p;

         m_pMapper->LPtoDP(p, pt[i]);

         pta[i].X = p.x;

         pta[i].Y = p.y;

      }

      if (bClosed)// Infinisys Ltd.
      {

         m_ppath->AddPolygon(pta.GetData(), pta.GetCount());

      }
      else
      {

         m_ppath->AddLines(pta.GetData(), pta.GetCount());

      }

      MoveTo(pt[pt.GetUpperBound()]);

      return;

   }

   if(cap1 < 0) cap1 = m_PenModel.EndCap1();

   if(cap2 < 0) cap2 = m_PenModel.EndCap2();

   int i = 0;

   int iInnerCount = nCount;

   short nPenW = m_pMapper->ScreenPenW(m_PenModel.m_nWidth);

   bool bDrawEx = false;

   if(nCount == 2)
   {
      
      MoveTo(pt[0]);

      DrawLineCaps(pt[1],cap1,cap2,kBothEnd);

   }
   else
   {

      LongPoint lpt1, lpt2;

      lpt1 = pt[0];

      lpt2 = pt[1];

      MoveTo(lpt1);

      if (!bClosed)
      {

         if (cap1)
         {

            m_DrawPrim.lineCapped(m_pMapper, &lpt1, &lpt2, cap1, cap2, kEnd1, m_PenModel.m_nWidth & 0x1F, &lpt1, &lpt2);

            DrawLineCaps(lpt2, cap1, 0, bClosed ? 0 : kEnd1);

            i = 1;

         }

         if (cap2)
         {

            iInnerCount--;

         }

      }

      // Infinisys Ltd.
      if ((bOutline || !m_PenModel.isPenStyleRound()) && !shouldUseDash())
      {

         bDrawEx = true;

         CArray < pointf > pta;

         for (; i < iInnerCount; i++)
         {

            pointd p;

            m_pMapper->LPtoDP(p, pt[i]);

            pta.Add(Gdiplus::PointF(p.x, p.y));

         }

         graphics_on();

         if (bClosed)// Infinisys Ltd.
         {

            m_pgraphics->DrawPolygon(m_PenPlus.get(), pta.GetData(), pta.GetCount());

         }
         else
         {

            // does Gdiplus need computing for kEnd1 and kEnd2 when polyline is not closed,
            // and pen is not center aligned?
            m_pgraphics->DrawLines(m_PenPlus.get(), pta.GetData(), pta.GetCount());

         }

         MoveTo(pt[i - 1]);

      }
      else
      {

         i++;

         for (; i < iInnerCount; i++)
         {

            DrawLineCaps(pt[i], 0, 0);

            m_DrawPrim.m_helperPoly.DrawNow(this);
            m_DrawPrim.m_helperPoly.Reset();

         }

      }

   }

   if(bClosed)
   {

      if(!bDrawEx)// Infinisys Ltd.
      {

         DrawLineCaps(pt[0],0,0);

      }

      CloseJoint(pt[0]);

   }
   else
   {

      if (cap2)
      {

         DrawLineCaps(pt[i], 0, cap2, kEnd2);

      }

      if(!m_bGreyMode && cap1)
      {

         m_DrawPrim.drawEndcap(this,pt[0],pt[1],cap1,kEnd1,m_PenModel,false,false,kDimLineOut);

      }
      
      //graphics_off();
      //m_pDC->MoveTo(m_pMapper->LPtoDP(pt[i]));

      //MoveTo(pt[i]);

   }

   m_DrawPrim.m_bUsePolyHelperIfDashed = false;

   m_DrawPrim.m_helperPoly.DrawNow(this);

   m_DrawPrim.m_helperPoly.Reset();
   
}


void CDraftContext::MetaPoly(const CEdgeList* pEdges,bool bClosed)
{
   // TODO GDI+
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
   //CGDIObject<CBrush> gdiBrush(m_pDC,&m_PenBrush);
   // END TODO GDI+

   bool bUsePath=canUsePath();
   if(bUsePath) bUsePath=BeginPath();

   POSITION pos=pEdges->GetHeadPosition();
   while(pos)
   {
      CEdge* pEdge=pEdges->GetNext(pos);
      pEdge->Draw(this);
   }

   if(bUsePath)
   {
      if(bClosed) m_pDC->CloseFigure();
      m_bCreatingPath=false;
      if(m_pDC->EndPath())
         m_pDC->StrokePath();
   }
   else if(bClosed) CloseJoint(pEdges->GetHead()->m_lptFrom);
}

void CDraftContext::BezierDash(const LongPoint& lpt0,const LongPoint& lpt1,const LongPoint& lpt2,const LongPoint& lpt3,short cap1,short cap2)
{
   // TODO GDI+
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
   //CGDIObject<CBrush> gdiBrush(m_pDC,&m_PenBrush);
   // END TODO GDI+
   pointd pt[4];
   m_pMapper->LPtoDP(pt[0], lpt0);
   m_pMapper->LPtoDP(pt[1], lpt1);
   m_pMapper->LPtoDP(pt[2], lpt2);
   m_pMapper->LPtoDP(pt[3], lpt3);

   MoveTo(lpt0);	// temp fix
   m_DrawPrim.drawBezier(this,pt);

   BezierDashCaps(lpt0, lpt1, lpt2, lpt3, cap1, cap2);
   //?	m_pDC->MoveTo(pt3);	// update current position
   m_lptCurrent=lpt3;

   //if (m_DrawPrim.m_helperPoly.has_points())
   //{

   //   m_DrawPrim.m_helperPoly.DrawNow(this);
   //   m_DrawPrim.m_helperPoly.Reset();

   //}

}


void CDraftContext::BezierDashCaps(const LongPoint& lpt0, const LongPoint& lpt1, const LongPoint& lpt2, const LongPoint& lpt3, short cap1, short cap2)
{

   if (!m_bGreyMode)
   {
      if (cap1 < 0) cap1 = m_PenModel.EndCap1();
      if (cap2 < 0) cap2 = m_PenModel.EndCap2();

      if (cap1 || cap2)
      {
         if (cap1)
         {
            if (lpt0 != lpt1)
               m_DrawPrim.drawEndcap(this, lpt0, lpt1, cap1, kEnd1, m_PenModel, true, false, kDimLineOut);
            else
               m_DrawPrim.drawEndcap(this, lpt0, lpt2, cap1, kEnd1, m_PenModel, true, false, kDimLineOut);
         }

         if (cap2)
         {
            if (lpt2 != lpt3)
               m_DrawPrim.drawEndcap(this, lpt2, lpt3, cap2, kEnd2, m_PenModel, true, false, kDimLineOut);
            else
               m_DrawPrim.drawEndcap(this, lpt1, lpt3, cap2, kEnd2, m_PenModel, true, false, kDimLineOut);
         }
      }
   }
}

void CDraftContext::Bezier(const LongPoint& lpt0,const LongPoint& lpt1,const LongPoint& lpt2,const LongPoint& lpt3)
{
   // TODO GDI+
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
   // END TODO GDI+

   graphics_off();
   CPoint pt[4];
   pt[0]=m_pMapper->LPtoDP(lpt0);
   pt[1]=m_pMapper->LPtoDP(lpt1);
   pt[2]=m_pMapper->LPtoDP(lpt2);
   pt[3]=m_pMapper->LPtoDP(lpt3);
   m_pDC->PolyBezier(pt,4);

   MoveTo(lpt3);
   //	m_pDC->MoveTo(pt[3]);	// update current position
   //	m_lptCurrent=lpt3;

   //	MoveTo(lpt0);
   //	BezierTo(lpt1,lpt2,lpt3);
}

void CDraftContext::BezierTo(const LongPoint& lpt1,const LongPoint& lpt2,const LongPoint& lpt3)
{
   if (m_bUseGDI)
   {
      CPen pen;
      pen.CreatePen(PS_COSMETIC, 1, RGB(0, 0, 0));
      CGDIObject<CPen> gdiPen(m_pDC, &pen);
      CPoint pt[3];
      pt[0]=m_pMapper->LPtoDP(lpt1);
      pt[1]=m_pMapper->LPtoDP(lpt2);
      pt[2]=m_pMapper->LPtoDP(lpt3);
      m_pDC->PolyBezierTo(pt, 3);
   }
   else
   {
      pointd pt[4];
      m_pMapper->LPtoDP(pt[0], m_lptCurrent);
      m_pMapper->LPtoDP(pt[1], lpt1);
      m_pMapper->LPtoDP(pt[2], lpt2);
      m_pMapper->LPtoDP(pt[3], lpt3);
      m_ppath->AddBezier(__pointf(pt[0]), __pointf(pt[1]), __pointf(pt[2]), __pointf(pt[3]));
   }
   m_lptCurrent=lpt3;
}


void CDraftContext::DrawHandle(const LongPoint &lpt,bool bHalftone)
{

   CPoint pt = m_pMapper->LPtoDP(lpt);

   if (bHalftone)
   {

      CGDIBackColor bc(m_pDC, RGB(255, 255, 255));
      CGDITextColor tc(m_pDC, RGB(0, 0, 0));
      CGDIObject < CBrush > br(m_pDC, m_pDC->GetHalftoneBrush());

      //Gdiplus::Rect r(pt.x - kTrackHandleSize / 2, pt.y - kTrackHandleSize / 2,kTrackHandleSize, kTrackHandleSize);
      //graphics_on();
      //Gdiplus::SolidBrush brush(Gdiplus::Color::Black);
      //m_pgraphics->FillRectangle(&brush, r);
      //CBrush brush;
      //brush.CreateSolidBrush(0);
      //auto pbrushOld = m_pDC->SelectObject(&brush);

      m_pDC->PatBlt(pt.x - kTrackHandleSize / 2, pt.y - kTrackHandleSize / 2, kTrackHandleSize, kTrackHandleSize, PATINVERT);

   }
   else
   {

      m_pDC->PatBlt(pt.x - kTrackHandleSize / 2, pt.y - kTrackHandleSize / 2, kTrackHandleSize, kTrackHandleSize, DSTINVERT);

   }

}


//void CDraftContext::PatternFill(Gdiplus::Brush* pbrush)
//{
//
//   m_pgraphics->FillPath(pbrush, m_ppath);
//
////   if(iPackedFill >= 0)
////   {
////
////      graphics_on();
////
////      //Gdiplus::GraphicsState state = m_pgraphics->Save();
////
////      if(m_pDC->IsPrinting())
////      {
////
////         m_pgraphics->SetPageUnit(Gdiplus::UnitPixel);
////
////      }
////
////      //CPoint ptOrg;
////      //GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
////      //pRgn->OffsetRgn(ptOrg);	// temporary fix, should not be here
////
////
////      //Gdiplus::Region * prgn = Gdiplus::Region::FromHRGN((HRGN)pRgn->GetSafeHandle());
////
////      //if (!prgn && m_ppath)
////      //{
////      //   // GDI+ extra
////      //   prgn = new Gdiplus::Region(m_ppath);
////
////      //}
////
////      //m_pgraphics->IntersectClip(prgn);
////
////
////      //Gdiplus::Rect r;
////      //prgn->GetBounds(&r,m_pgraphics);
////
////      //CRect rc;
////      //if(m_prect == NULL)
////      //{
////      //   rc.left = r.X;
////      //   rc.top = r.Y;
////      //   rc.right = rc.left + r.Width;
////      //   rc.bottom = rc.top + r.Height;
////      //}
////      //else
////      //{
////
////      //   rc.left =  m_pMapper->LPtoDP(m_prect->left);
////      //   rc.top = m_pMapper->LPtoDP(m_prect->top);
////      //   rc.right = m_pMapper->LPtoDP(m_prect->right);
////      //   rc.bottom = m_pMapper->LPtoDP(m_prect->bottom);
////
////      //   if(rc.Height() <= 0)// Interface Update - Infinisys Ltd.
////      //   {
////      //      rc.top--;
////      //      rc.bottom++;
////      //   }
////      //   if(rc.Width() <= 0)// Interface Update - Infinisys Ltd.
////      //   {
////      //      rc.left--;
////      //      rc.right++;
////      //   }
////
////      //}
////
////
////
////
////
////      //double iScale = m_pMapper->LPtoDP((int)1000);
////
////      if (m_prect)
////      {
////
////         CRect rc;
////
////         rc = m_pMapper->LPtoDP(*m_prect);
////
////         if (m_pVersionDoc != NULL)
////         {
////            m_pVersionDoc->m_packeditema[iPackedFill].m_pgradient->draw_proper(*m_pgraphics, m_ppath, rc, iOpacity);
////         }
////         else if (m_pDoc != NULL)
////         {
////            m_pDoc->m_packeditema[iPackedFill].m_pgradient->draw_proper(*m_pgraphics, m_ppath, rc, iOpacity);
////         }
////
////      }
////      else
////      {
////
////
////         if (m_pVersionDoc != NULL)
////         {
////            m_pVersionDoc->m_packeditema[iPackedFill].m_pgradient->draw_proper(*m_pgraphics, m_ppath, NULL, iOpacity);
////         }
////         else if (m_pDoc != NULL)
////         {
////            m_pDoc->m_packeditema[iPackedFill].m_pgradient->draw_proper(*m_pgraphics, m_ppath, NULL, iOpacity);
////         }
////
////
////      }
////
//////#if 1
//////      r.x = rc.left;
//////      r.y = rc.top;
//////      r.Width = rc.right - rc.left;
//////      r.Height = rc.bottom - rc.top;
//////      Gdiplus::Pen pen(Gdiplus::Color(40,0,0,0),1.0);
//////      m_pgraphics->DrawRectangle(&pen,r);
//////#endif
////  //    m_pgraphics->Restore(state);
//////      delete prgn;
////   }
////   else
////   {
////
////      //CRgn oldClip;
////      //SaveClip(oldClip);
////
////      //CPoint ptOrg;
////      //GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
////      //pRgn->OffsetRgn(-ptOrg);	// temporary fix, should not be here
////      //m_pDC->SelectClipRgn(pRgn,RGN_AND);
////
////      graphics_on();
////
////
////
////      //Gdiplus::GraphicsState state = m_pgraphics->Save();
////      if(m_pDC->IsPrinting())
////      {
////
////         m_pgraphics->SetPageUnit(Gdiplus::UnitPixel);
////         //m_pgraphics->ScaleTransform((double)iScale / 1000.0,(double)iScale / 1000.0);
////
////      }
////
////      //CPoint ptOrg;
////      //GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
////      //pRgn->OffsetRgn(ptOrg);	// temporary fix, should not be here
////
////      //HRGN hrgn = (HRGN)pRgn->GetSafeHandle();
////      ASSERT(pRgn == nullptr);
////
//////      Gdiplus::Region * prgn = nullptr;
//////
//////      if (hrgn == NULL)
//////      {
//////#undef new
//////         prgn = new Gdiplus::Region(m_ppath);
//////
//////      }
//////      else
//////      {
//////
//////         prgn = Gdiplus::Region::FromHRGN(hrgn);
//////
//////      }
////
////      //m_pgraphics->IntersectClip(prgn);
////
////
////      Gdiplus::Rect r;
////
////      CRect rc;
////      if(m_prect == NULL)
////      {
////         m_ppath->GetBounds(&r);
////         rc.left = r.X;
////         rc.top = r.Y;
////         rc.right = rc.left + r.Width;
////         rc.bottom = rc.top + r.Height;
////      }
////      else
////      {
////
////         rc.left =  m_pMapper->LPtoDP(m_prect->GetCorner(0)).x;
////         rc.top = m_pMapper->LPtoDP(m_prect->GetCorner(0)).y;
////         rc.right = m_pMapper->LPtoDP(m_prect->GetCorner(2)).x;
////         rc.bottom = m_pMapper->LPtoDP(m_prect->GetCorner(2)).y;
////
////      }
////
////      //RECT rc;
////      //if(GetObjectType(*m_pDC) == OBJ_ENHMETADC) pRgn->GetRgnBox(&rc);
////      //else m_pDC->GetClipBox(&rc);
////      //CGDIBitmapDC patDC(m_pDC,pPattern);
////      //BITMAP bm;
////      //pPattern->GetBitmap(&bm);
////     
////
////      //	if(m_pDC->IsPrinting())
////      {
////         /*		long nHeight=m_pMapper->LPtoDP((int)bm.bmHeight);
////               long nWidth=m_pMapper->LPtoDP((int)bm.bmWidth);
////               m_pDC->SetStretchBltMode(HALFTONE);
////               for(int y=rc.top-rc.top%nHeight;y<rc.bottom;y+=nHeight)
////               for(int x=rc.left-rc.left%nWidth;x<rc.right;x+=nWidth)
////               m_pDC->StretchBlt(x,y,nWidth,nHeight,&patDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
////               */
////
////         if(m_pDC->IsPrinting())
////         {
////            rc.left-=abs(rc.left%m_pMapper->LPtoDP((int)pbitmap->GetWidth()));
////            rc.top-=abs(rc.top%m_pMapper->LPtoDP((int)pbitmap->GetHeight()));
////         }
////         else
////         {
////            rc.left-=abs(rc.left % ((int)pbitmap->GetWidth()));
////            rc.top-=abs(rc.top % ((int)pbitmap->GetHeight()));
////         }
////
////         long nWidth=rc.Width();
////         long nHeight=rc.Height();
////         CBitmap bmp;
////         if(m_pDC->IsPrinting())
////         {
////            nWidth=m_pMapper->DPtoLP(nWidth).GetLong() + 1;
////            nHeight=m_pMapper->DPtoLP(nHeight).GetLong() + 1;
////            rc.right=rc.left + m_pMapper->LPtoDP((int)nWidth);
////            rc.bottom=rc.top + m_pMapper->LPtoDP((int)nHeight);
////
////            // TODO
////            //DIBSECTION ds;
////            //GetObject(pPattern->GetSafeHandle(),sizeof(DIBSECTION),&ds);
////            //ds.dsBmih.biWidth=nWidth;
////            //ds.dsBmih.biHeight=nHeight;
////            //if(ds.dsBmih.biBitCount == 1) ds.dsBitfields[1]=0xFFFFFF;		// tweak
////            //LPVOID lpBits;
////            //HBITMAP hDib=CreateDIBSection(NULL,LPBITMAPINFO(&ds.dsBmih),DIB_RGB_COLORS,&lpBits,NULL,0);
////            //bmp.Attach(hDib);
////            //graphics_off();
////            //CGDIBitmapDC memDC(m_pDC,&bmp);
////
////            //CBrush br1; // Interface Update - Infinisys Ltd.
////
////            //br1.CreatePatternBrush(pPattern); // Interface Update - Infinisys Ltd.
////            //memDC.FillRect(CRect(0,0,nWidth,nHeight),&br1);// Interface Update - Infinisys Ltd.
////            //for(int y=0;y < nHeight;y+=bm.bmHeight)
////            //   for(int x=0;x < nWidth;x+=bm.bmWidth)
////            //   {
////
////            //      memDC.BitBlt(x,y,bm.bmWidth,bm.bmHeight,&patDC,0,0,SRCCOPY);
////
////            //   }
////         }
////         else
////         {
////
////            //graphics_off();
////
////            //bmp.CreateCompatibleBitmap(m_pDC,nWidth,nHeight);// Interface Update - Infinisys Ltd.
////
////            //CGDIBitmapDC memDC(m_pDC,&bmp);// Interface Update - Infinisys Ltd.
////            //CBrush br1;// Interface Update - Infinisys Ltd.
////            //br1.CreatePatternBrush(pPattern);// Interface Update - Infinisys Ltd.
////            //br1.CreateSolidBrush(RGB(255, 200, 255));
////            //memDC.FillRect(CRect(0,0,nWidth,nHeight),&br1);// Interface Update - Infinisys Ltd.
////            //for(int y=0;y < nHeight;y+=bm.bmHeight)
////            //   for(int x=0;x < nWidth;x+=bm.bmWidth)
////            //   {
////
////            //      memDC.BitBlt(x,y,bm.bmWidth,bm.bmHeight,&patDC,0,0,SRCCOPY);
////
////            //   }
////            //graphics_on();
////
////         }
////
////
////         Gdiplus::ColorMatrix ClrMatrix ={
////            1.0f,0.0f,0.0f,0.0f,0.0f,
////            0.0f,1.0f,0.0f,0.0f,0.0f,
////            0.0f,0.0f,1.0f,0.0f,0.0f,
////            0.0f,0.0f,0.0f,(float)iOpacity/255.0f,0.0f,
////            0.0f,0.0f,0.0f,0.0f,1.0f
////         };
////         //Gdiplus::Bitmap * pb = Gdiplus::Bitmap::FromHBITMAP((HBITMAP) bmp.GetSafeHandle(),m_pDoc == NULL ? NULL : (HPALETTE) m_pDoc->m_Palette.GetSafeHandle());
////
////         Gdiplus::ImageAttributes attr;
////
////         attr.SetColorMatrix(&ClrMatrix,Gdiplus::ColorMatrixFlagsDefault,Gdiplus::ColorAdjustTypeBitmap);
////
////         Gdiplus::TextureBrush texturebrush(pbitmap);
////
////         if(m_pDC->IsPrinting())
////         {
////            //m_pDC->SetStretchBltMode(HALFTONE);
////          //m_pDC->StretchBlt(rc.left,rc.top,Width(rc),Height(rc),&memDC,0,0,nWidth,nHeight,SRCCOPY);
////          //m_pgraphics->DrawImage(pb,Gdiplus::Rect(rc.left, rc.top, Width(rc), Height(rc)),0,0,nWidth,nHeight,Gdiplus::UnitPixel,&attr);
////            m_pgraphics->FillPath(&texturebrush, m_ppath);
////         }
////         else
////         {
////
////            //m_pgraphics->DrawImage(pb,r,0,0,r.Width,r.Height,Gdiplus::UnitPixel,&attr);
////            m_pgraphics->FillPath(&texturebrush, m_ppath);
////
////         }
////
////         //delete pb;
////
////      }
////      /*	else
////         {
////         for(int y=rc.top-rc.top%bm.bmHeight;y<rc.bottom;y+=bm.bmHeight)
////         for(int x=rc.left-rc.left%bm.bmWidth;x<rc.right;x+=bm.bmWidth)
////         m_pDC->BitBlt(x,y,bm.bmWidth,bm.bmHeight,&patDC,0,0,SRCCOPY);
////         }
////         */
//////      m_pDC->SelectClipRgn(&oldClip);
////      //m_pgraphics->Restore(state);
////      //delete prgn;
////   }
////
//
//}


//void CDraftContext::PatternFill(Gdiplus::Region *pRgn,CBitmap* pPattern,int iPackedFill,int iOpacity)
//{
//
//   if(iPackedFill >= 0)
//   {
//
//      graphics_on();
//
//      Gdiplus::GraphicsState state = m_pgraphics->Save();
//
//      //graphics_off();
//
//      if(m_pDC->IsPrinting())
//      {
//
//         m_pgraphics->SetPageUnit(Gdiplus::UnitPixel);
//
//      }
//
//      //CPoint ptOrg;
//      //GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
//      //pRgn->OffsetRgn(ptOrg);	// temporary fix, should not be here
//
//
//
//      // GDI++
//      //Gdiplus::Region * prgn = pRgn;
//
//      //if (prgn == nullptr && m_ppath)
//      //{
//      // 
//      //   prgn = new Gdiplus::Region(m_ppath);
//
//      //}
//
//      //m_pgraphics->IntersectClip(prgn);
//
//
//      //Gdiplus::Rect r;
//      //prgn->GetBounds(&r,m_pgraphics);
//
//      //CRect rc;
//      //if(m_prect == NULL)
//      //{
//      //   rc.left = r.X;
//      //   rc.top = r.Y;
//      //   rc.right = rc.left + r.Width;
//      //   rc.bottom = rc.top + r.Height;
//      //}
//      //else
//      //{
//
//      //   rc.left =  m_pMapper->LPtoDP(m_prect->left);
//      //   rc.top = m_pMapper->LPtoDP(m_prect->top);
//      //   rc.right = m_pMapper->LPtoDP(m_prect->right);
//      //   rc.bottom = m_pMapper->LPtoDP(m_prect->bottom);
//
//      //   if(rc.Height() <= 0)// Interface Update - Infinisys Ltd.
//      //   {
//      //      rc.top--;
//      //      rc.bottom++;
//      //   }
//      //   if(rc.Width() <= 0)// Interface Update - Infinisys Ltd.
//      //   {
//      //      rc.left--;
//      //      rc.right++;
//      //   }
//
//      //}
//
//
//
//
//
//      //double iScale = m_pMapper->LPtoDP((int)1000);
//      //double iScale = m_pMapper->LPtoDP((int)1000);
//
//      if (m_prect)
//      {
//         // Use context rectangle (m_prect)
//         // to define the boundaries of the gradient fill
//
//         CRect rc;
//
//         rc = m_pMapper->LPtoDP(*m_prect);
//
//         if (m_pVersionDoc != NULL)
//         {
//            m_pVersionDoc->m_packeditema[iPackedFill].m_pgradient->draw_proper(*m_pgraphics, m_ppath, rc, iOpacity);
//         }
//         else if (m_pDoc != NULL)
//         {
//            m_pDoc->m_packeditema[iPackedFill].m_pgradient->draw_proper(*m_pgraphics, m_ppath, rc, iOpacity);
//         }
//      }
//      else
//      {
//         // Use current path bounding box
//         // to define the boundaries of the gradient fill
//
//         if (m_pVersionDoc != NULL)
//         {
//            m_pVersionDoc->m_packeditema[iPackedFill].m_pgradient->draw_proper(*m_pgraphics, m_ppath, nullptr, iOpacity);
//         }
//         else if (m_pDoc != NULL)
//         {
//            m_pDoc->m_packeditema[iPackedFill].m_pgradient->draw_proper(*m_pgraphics, m_ppath, nullptr, iOpacity);
//         }
//
//      }
//      //#if 1
//      //      r.x = rc.left;
//      //      r.y = rc.top;
//      //      r.Width = rc.right - rc.left;
//      //      r.Height = rc.bottom - rc.top;
//      //      Gdiplus::Pen pen(Gdiplus::Color(40,0,0,0),1.0);
//      //      m_pgraphics->DrawRectangle(&pen,r);
//      //#endif
//      m_pgraphics->Restore(state);
//      //      delete prgn;
//   }
//   else
//   {
//
//      //CRgn oldClip;
//      //SaveClip(oldClip);
//
//      //CPoint ptOrg;
//      //GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
//      //pRgn->OffsetRgn(-ptOrg);	// temporary fix, should not be here
//      //m_pDC->SelectClipRgn(pRgn,RGN_AND);
//
//      graphics_on();
//
//
//
//      Gdiplus::GraphicsState state = m_pgraphics->Save();
//      if(m_pDC->IsPrinting())
//      {
//
//         m_pgraphics->SetPageUnit(Gdiplus::UnitPixel);
//         //m_pgraphics->ScaleTransform((double)iScale / 1000.0,(double)iScale / 1000.0);
//
//      }
//
//      //CPoint ptOrg;
//      //GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
//      //pRgn->OffsetRgn(ptOrg);	// temporary fix, should not be here
//
//      //Gdiplus::Region * prgn = Gdiplus::Region::FromHRGN((HRGN)pRgn->GetSafeHandle());
//      Gdiplus::Region * prgn = pRgn;
//
//      if (!prgn && m_ppath)
//      {
//
//         prgn = new Gdiplus::Region(m_ppath);
//
//      }
//
//      m_pgraphics->IntersectClip(prgn);
//
//
//      Gdiplus::Rect r;
//      prgn->GetBounds(&r,m_pgraphics);
//
//      CRect rc;
//      if(m_prect == NULL)
//      {
//         rc.left = r.X;
//         rc.top = r.Y;
//         rc.right = rc.left + r.Width;
//         rc.bottom = rc.top + r.Height;
//      }
//      else
//      {
//
//         rc.left =  m_pMapper->LPtoDP(m_prect->GetCorner(0)).x;
//         rc.top = m_pMapper->LPtoDP(m_prect->GetCorner(0)).y;
//         rc.right = m_pMapper->LPtoDP(m_prect->GetCorner(2)).x;
//         rc.bottom = m_pMapper->LPtoDP(m_prect->GetCorner(2)).y;
//
//      }
//
//      //RECT rc;
//      //if(GetObjectType(*m_pDC) == OBJ_ENHMETADC) pRgn->GetRgnBox(&rc);
//      //else m_pDC->GetClipBox(&rc);
//      CGDIBitmapDC patDC(m_pDC,pPattern);
//      BITMAP bm;
//      pPattern->GetBitmap(&bm);
//
//
//      //	if(m_pDC->IsPrinting())
//      {
//         /*		long nHeight=m_pMapper->LPtoDP((int)bm.bmHeight);
//         long nWidth=m_pMapper->LPtoDP((int)bm.bmWidth);
//         m_pDC->SetStretchBltMode(HALFTONE);
//         for(int y=rc.top-rc.top%nHeight;y<rc.bottom;y+=nHeight)
//         for(int x=rc.left-rc.left%nWidth;x<rc.right;x+=nWidth)
//         m_pDC->StretchBlt(x,y,nWidth,nHeight,&patDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
//         */
//
//         if(m_pDC->IsPrinting())
//         {
//            rc.left-=abs(rc.left%m_pMapper->LPtoDP((int)bm.bmWidth));
//            rc.top-=abs(rc.top%m_pMapper->LPtoDP((int)bm.bmHeight));
//         }
//         else
//         {
//            rc.left-=abs(rc.left % ((int)bm.bmWidth));
//            rc.top-=abs(rc.top % ((int)bm.bmHeight));
//         }
//
//         long nWidth=rc.Width();
//         long nHeight=rc.Height();
//         CBitmap bmp;
//         if(m_pDC->IsPrinting())
//         {
//            nWidth=m_pMapper->DPtoLP(nWidth).GetLong() + 1;
//            nHeight=m_pMapper->DPtoLP(nHeight).GetLong() + 1;
//            rc.right=rc.left + m_pMapper->LPtoDP((int)nWidth);
//            rc.bottom=rc.top + m_pMapper->LPtoDP((int)nHeight);
//
//            DIBSECTION ds;
//            GetObject(pPattern->GetSafeHandle(),sizeof(DIBSECTION),&ds);
//            ds.dsBmih.biWidth=nWidth;
//            ds.dsBmih.biHeight=nHeight;
//            if(ds.dsBmih.biBitCount == 1) ds.dsBitfields[1]=0xFFFFFF;		// tweak
//            LPVOID lpBits;
//            HBITMAP hDib=CreateDIBSection(NULL,LPBITMAPINFO(&ds.dsBmih),DIB_RGB_COLORS,&lpBits,NULL,0);
//            bmp.Attach(hDib);
//            CGDIBitmapDC memDC(m_pDC,&bmp);
//
//            CBrush br1; // Interface Update - Infinisys Ltd.
//
//            br1.CreatePatternBrush(pPattern); // Interface Update - Infinisys Ltd.
//            memDC.FillRect(CRect(0,0,nWidth,nHeight),&br1);// Interface Update - Infinisys Ltd.
//            for(int y=0;y < nHeight;y+=bm.bmHeight)
//               for(int x=0;x < nWidth;x+=bm.bmWidth)
//               {
//
//                  memDC.BitBlt(x,y,bm.bmWidth,bm.bmHeight,&patDC,0,0,SRCCOPY);
//
//               }
//         }
//         else
//         {
//
//            bmp.CreateCompatibleBitmap(m_pDC,nWidth,nHeight);// Interface Update - Infinisys Ltd.
//
//            CGDIBitmapDC memDC(m_pDC,&bmp);// Interface Update - Infinisys Ltd.
//            CBrush br1;// Interface Update - Infinisys Ltd.
//            br1.CreatePatternBrush(pPattern);// Interface Update - Infinisys Ltd.
//            //br1.CreateSolidBrush(RGB(255, 200, 255));
//            memDC.FillRect(CRect(0,0,nWidth,nHeight),&br1);// Interface Update - Infinisys Ltd.
//            //for(int y=0;y < nHeight;y+=bm.bmHeight)
//            //   for(int x=0;x < nWidth;x+=bm.bmWidth)
//            //   {
//
//            //      memDC.BitBlt(x,y,bm.bmWidth,bm.bmHeight,&patDC,0,0,SRCCOPY);
//
//            //   }
//
//         }
//
//
//         Gdiplus::ColorMatrix ClrMatrix ={
//            1.0f,0.0f,0.0f,0.0f,0.0f,
//            0.0f,1.0f,0.0f,0.0f,0.0f,
//            0.0f,0.0f,1.0f,0.0f,0.0f,
//            0.0f,0.0f,0.0f,(float)iOpacity / 255.0f,0.0f,
//            0.0f,0.0f,0.0f,0.0f,1.0f
//         };
//         Gdiplus::Bitmap * pb = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)bmp.GetSafeHandle(),m_pDoc == NULL ? NULL : (HPALETTE)m_pDoc->m_Palette.GetSafeHandle());
//
//         Gdiplus::ImageAttributes attr;
//
//         attr.SetColorMatrix(&ClrMatrix,Gdiplus::ColorMatrixFlagsDefault,Gdiplus::ColorAdjustTypeBitmap);
//
//         if(m_pDC->IsPrinting())
//         {
//            //m_pDC->SetStretchBltMode(HALFTONE);
//            //m_pDC->StretchBlt(rc.left,rc.top,Width(rc),Height(rc),&memDC,0,0,nWidth,nHeight,SRCCOPY);
//            m_pgraphics->DrawImage(pb,Gdiplus::Rect(rc.left,rc.top,Width(rc),Height(rc)),0,0,nWidth,nHeight,Gdiplus::UnitPixel,&attr);
//         }
//         else
//         {
//
//            m_pgraphics->DrawImage(pb,r,0,0,r.Width,r.Height,Gdiplus::UnitPixel,&attr);
//
//         }
//
//         delete pb;
//
//      }
//      /*	else
//      {
//      for(int y=rc.top-rc.top%bm.bmHeight;y<rc.bottom;y+=bm.bmHeight)
//      for(int x=rc.left-rc.left%bm.bmWidth;x<rc.right;x+=bm.bmWidth)
//      m_pDC->BitBlt(x,y,bm.bmWidth,bm.bmHeight,&patDC,0,0,SRCCOPY);
//      }
//      */
//      //      m_pDC->SelectClipRgn(&oldClip);
//      m_pgraphics->Restore(state);
//      //delete prgn;
//   }
//
//
//}
//

void CDraftContext::BrushFill(Gdiplus::Brush* pBrush)
{

   graphics_on();

   if (m_ppath)
   {

      m_pgraphics->FillPath(pBrush, m_ppath);

   }

}

//void CDraftContext::BrushFill(CRgn *pRgn,Gdiplus::Brush* pBrush, int iOpacity)
//{
//   
//   // TODO GDI+
//   //LOGBRUSH lb;
//   //memset(&lb,0,sizeof(lb));
//   //pBrush->GetLogBrush(&lb);
//   // TODO GDI+
//
//
////   if(iOpacity == 255 || lb.lbStyle != BS_SOLID)
//   {
//   //  
//   //   /*	CGDIObject<CBrush> gdiBrush(m_pDC,pBrush);
//   //      m_pDC->PaintRgn(pRgn);*/
//
//   //   CRgn oldClip;
//   //   SaveClip(oldClip);
//
//   //   CPoint ptOrg;
//   //   GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
//   //   pRgn->OffsetRgn(ptOrg);	// temporary fix, should not be here
//   //   m_pDC->SelectClipRgn(pRgn,RGN_AND);
//   //   //m_pDC->SelectClipRgn(pRgn,RGN_COPY);
//
//   //   RECT rc;
//   //   if(GetObjectType(*m_pDC) == OBJ_ENHMETADC) pRgn->GetRgnBox(&rc);
//   //   else m_pDC->GetClipBox(&rc);
//   //   m_pDC->FillRect(&rc,pBrush);
//   //   m_pDC->SelectClipRgn(&oldClip);
//
//   //}
//   //else
//   //{
//
//      //// TODO GDI+
//      //       COLORREF cr;
//      //
//      //cr = lb.lbColor;
//
//      //if(cr & 0x01000000)
//      //{
//
//      //   PALETTEENTRY palEntry;
//
//      //   m_pDoc->m_Palette.GetPaletteEntries(cr& 0xffff,1,&palEntry);
//
//      //   cr = RGB(palEntry.peRed,palEntry.peGreen,palEntry.peBlue);
//
//      //}
//
//      //Gdiplus::Color c(iOpacity,GetRValue(cr),GetGValue(cr),GetBValue(cr));
//      //
//      //// END TODO GDI+
//
//      graphics_on();
//
//
//      //Gdiplus::SolidBrush br(c);
//
//      //Gdiplus::GraphicsState state = m_pgraphics->Save();
//      if(m_pDC->IsPrinting())
//      {
//
//         m_pgraphics->SetPageUnit(Gdiplus::UnitPixel);
//         //m_pgraphics->ScaleTransform((double)iScale / 1000.0,(double)iScale / 1000.0);
//
//      }
//
//      //CPoint ptOrg;
//      //GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
//      //pRgn->OffsetRgn(ptOrg);	// temporary fix, should not be here
//
//      //m_pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
//      //m_pgraphics->SetCompositingQuality(Gdiplus::CompositingQualityGammaCorrected);
//
//
//      if (m_ppath)
//      {
//
//         m_pgraphics->FillPath(pBrush, m_ppath);
//
//      }
//      else
//      {
//
//         Gdiplus::Region * prgn = Gdiplus::Region::FromHRGN((HRGN)pRgn->GetSafeHandle());
//
//         m_pgraphics->IntersectClip(prgn);
//
//         m_pgraphics->FillRegion(pBrush, prgn);
//
//         delete prgn;
//
//
//      }
//
//
//      //Gdiplus::Rect r;
//      //prgn->GetBounds(&r,m_pgraphics);
//
//      //CRect rc;
//      //if(m_prect == NULL)
//      //{
//      //   rc.left = r.x;
//      //   rc.top = r.y;
//      //   rc.right = rc.left + r.Width;
//      //   rc.bottom = rc.top + r.Height;
//      //}
//      //else
//      //{
//
//      //   rc.left =  m_pMapper->LPtoDP(m_prect->GetCorner(0)).x;
//      //   rc.top = m_pMapper->LPtoDP(m_prect->GetCorner(0)).y;
//      //   rc.right = m_pMapper->LPtoDP(m_prect->GetCorner(2)).x;
//      //   rc.bottom = m_pMapper->LPtoDP(m_prect->GetCorner(2)).y;
//
//      //}
//
//
//
//      //r.x = rc.left;
//      //r.y = rc.top;
//      //r.Width = rc.right - rc.left;
//      //r.Height = rc.bottom - rc.top;
//
//
//      //double iScale = m_pMapper->LPtoDP((int)1000);
//      //m_pgraphics->FillRectangle(&br, r);
//#if 0
//      r.x = rc.left;
//      r.y = rc.top;
//      r.Width = rc.right - rc.left;
//      r.Height = rc.bottom - rc.top;
//      Gdiplus::Pen pen(Gdiplus::Color(40,0,0,0),1.0);
//      m_pgraphics->DrawRectangle(&pen,r);
//#endif
//      //m_pgraphics->Restore(state);
//   }
//
//}
//
//void CDraftContext::BrushFill(Gdiplus::Region *pRgn,Gdiplus::Brush* pBrush,int iOpacity)
//{
//
//   // TODO GDI+
//   //LOGBRUSH lb;
//   //memset(&lb,0,sizeof(lb));
//   //pBrush->GetLogBrush(&lb);
//   // END TODO GDI+
//
//   //   if(iOpacity == 255 || lb.lbStyle != BS_SOLID)
//   {
//      //  
//      //   /*	CGDIObject<CBrush> gdiBrush(m_pDC,pBrush);
//      //      m_pDC->PaintRgn(pRgn);*/
//
//      //   CRgn oldClip;
//      //   SaveClip(oldClip);
//
//      //   CPoint ptOrg;
//      //   GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
//      //   pRgn->OffsetRgn(ptOrg);	// temporary fix, should not be here
//      //   m_pDC->SelectClipRgn(pRgn,RGN_AND);
//      //   //m_pDC->SelectClipRgn(pRgn,RGN_COPY);
//
//      //   RECT rc;
//      //   if(GetObjectType(*m_pDC) == OBJ_ENHMETADC) pRgn->GetRgnBox(&rc);
//      //   else m_pDC->GetClipBox(&rc);
//      //   m_pDC->FillRect(&rc,pBrush);
//      //   m_pDC->SelectClipRgn(&oldClip);
//
//      //}
//      //else
//      //{
//
//      // TODO GDI+
//      //COLORREF cr;
//
//      //cr = lb.lbColor;
//
//      //if(cr & 0x01000000)
//      //{
//
//      //   PALETTEENTRY palEntry;
//
//      //   m_pDoc->m_Palette.GetPaletteEntries(cr & 0xffff,1,&palEntry);
//
//      //   cr = RGB(palEntry.peRed,palEntry.peGreen,palEntry.peBlue);
//
//      //}
//
//      //Gdiplus::Color c(iOpacity,GetRValue(cr),GetGValue(cr),GetBValue(cr));
//
//
//      graphics_on();
//
//
//      //Gdiplus::SolidBrush br(c);
//// END TODO GDI+
//      Gdiplus::GraphicsState state;
//      if(m_pDC->IsPrinting())
//      {
//         state = m_pgraphics->Save();
//
//         m_pgraphics->SetPageUnit(Gdiplus::UnitPixel);
//         //m_pgraphics->ScaleTransform((double)iScale / 1000.0,(double)iScale / 1000.0);
//
//      }
//
//      //CPoint ptOrg;
//      //GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
//      //pRgn->OffsetRgn(ptOrg);	// temporary fix, should not be here
//
//      //Gdiplus::Region * prgn = Gdiplus::Region::FromHRGN((HRGN)pRgn->GetSafeHandle());
//
//
//      if (m_ppath)
//      {
//
//         m_pgraphics->FillPath(pBrush, m_ppath);
//
//      }
//      else
//      {
//
//         Gdiplus::Region * prgn = pRgn;
//         m_pgraphics->IntersectClip(prgn);
//
//         m_pgraphics->FillRegion(pBrush, prgn);
//
//      }
//
//
//      //Gdiplus::Rect r;
//      //prgn->GetBounds(&r,m_pgraphics);
//
//      //CRect rc;
//      //if(m_prect == NULL)
//      //{
//      //   rc.left = r.x;
//      //   rc.top = r.y;
//      //   rc.right = rc.left + r.Width;
//      //   rc.bottom = rc.top + r.Height;
//      //}
//      //else
//      //{
//
//      //   rc.left =  m_pMapper->LPtoDP(m_prect->GetCorner(0)).x;
//      //   rc.top = m_pMapper->LPtoDP(m_prect->GetCorner(0)).y;
//      //   rc.right = m_pMapper->LPtoDP(m_prect->GetCorner(2)).x;
//      //   rc.bottom = m_pMapper->LPtoDP(m_prect->GetCorner(2)).y;
//
//      //}
//
//
//
//      //r.x = rc.left;
//      //r.y = rc.top;
//      //r.Width = rc.right - rc.left;
//      //r.Height = rc.bottom - rc.top;
//
//
//      //double iScale = m_pMapper->LPtoDP((int)1000);
//      //m_pgraphics->FillRectangle(&br, r);
//#if 0
//      r.x = rc.left;
//      r.y = rc.top;
//      r.Width = rc.right - rc.left;
//      r.Height = rc.bottom - rc.top;
//      Gdiplus::Pen pen(Gdiplus::Color(40,0,0,0),1.0);
//      m_pgraphics->DrawRectangle(&pen,r);
//#endif
//      if (m_pDC->IsPrinting())
//      {
//
//         m_pgraphics->Restore(state);
//         
//      }
//   }
//
//}
//
//void CDraftContext::PaintRgn(CRgn *pRgn)
//{
//   
//   if (m_FillModel.m_nFillType == kNoFill || m_bGreyMode || (m_pDoc && m_pDoc->m_bHideFills))
//   {
//
//      return;
//
//   }
//
//   BrushFill(m_BrushPlus.get());
//
//   //if(GetObjectType(*m_pDC) == OBJ_ENHMETADC) 
//   {
//
//      //BrushFill(pRgn,m_BrushPlus.get(), m_FillModel.m_iFillOpacity);
//
//   }
//   //else 
//   //{
//
//   //   if (m_FillModel.m_nFillType > kFillColor)
//   //   {
//
//   //      PatternFill(pRgn, m_pbitmapFillPattern.get(), m_FillModel.m_nFillType == kFillPat ? m_iPackedItemFill : -1, m_FillModel.m_iFillOpacity);
//
//   //   }
//   //   else
//   //   {
//
//   //      BrushFill(pRgn, m_BrushPlus.get(), m_FillModel.m_iFillOpacity);
//
//   //   }
//
//   //}
//
//}

void CDraftContext::PaintPath()
{
   
   if (m_FillModel.m_nFillType == kNoFill || m_bGreyMode || (m_pDoc && m_pDoc->m_bHideFills))
   {

      return;

   }

   BrushFill(m_BrushPlus.get());

   //if(GetObjectType(*m_pDC) == OBJ_ENHMETADC) 
   {

      //BrushFill(pRgn,m_BrushPlus.get(), m_FillModel.m_iFillOpacity);

   }
   //else 
   //{

   //   if (m_FillModel.m_nFillType > kFillColor)
   //   {

   //      PatternFill(pRgn, m_pbitmapFillPattern.get(), m_FillModel.m_nFillType == kFillPat ? m_iPackedItemFill : -1, m_FillModel.m_iFillOpacity);

   //   }
   //   else
   //   {

   //      BrushFill(pRgn, m_BrushPlus.get(), m_FillModel.m_iFillOpacity);

   //   }

   //}

}

//void CDraftContext::PaintPenRgn(CRgn *pRgn)
//{
//   
//   if (m_PenModel.m_nPenType == kPenInvis)
//   {
//
//      return;
//
//   }
//
//   BrushFill(m_PenBrushPlus.get());
//
//   //if(GetObjectType(*m_pDC) == OBJ_ENHMETADC) 
//   //{
//
//   //   BrushFill(pRgn,m_PenBrushPlus.get(),m_PenModel.m_iPenOpacity);
//
//   //}
//   //else 
//   //{
//   //   
//   //   if (m_PenModel.m_nPenType > kPenColor)
//   //   {
//
//   //      PatternFill(pRgn, m_pbitmapPenPattern.get(), m_iPackedItemPenFill, m_PenModel.m_iPenOpacity);
//
//   //   }
//   //   else
//   //   {
//
//   //      BrushFill(pRgn, m_PenBrushPlus.get(), m_PenModel.m_iPenOpacity);
//
//   //   }
//
//   //}
//
//}
//
//
//void CDraftContext::PaintPenPath()
//{
//
//
//   BrushFill(m_BrushPlus.get());
//
//   //if (GetObjectType(*m_pDC) == OBJ_ENHMETADC)
//   //{
//
//   //   BrushFill((Gdiplus::Region *)nullptr, m_PenBrushPlus.get(), m_PenModel.m_iPenOpacity);
//
//   //}
//   //else 
//   //{
//
//   //   if (m_PenModel.m_nPenType > kPenColor)
//   //   {
//
//   //      PatternFill((CRgn *)nullptr, m_pbitmapPenPattern.get(), m_iPackedItemPenFill, m_PenModel.m_iPenOpacity);
//
//   //   }
//   //   else
//   //   {
//
//   //      BrushFill((Gdiplus::Region *)nullptr, m_PenBrushPlus.get(), m_PenModel.m_iPenOpacity);
//
//   //   }
//
//   //}
//
//}

void CDraftContext::PaintPenPath()
{
   
   if (m_PenModel.m_nPenType == kPenInvis)
   {

      return;

   }

   BrushFill(m_PenBrushPlus.get());

   //if (GetObjectType(*m_pDC) == OBJ_ENHMETADC)
   //{

   //   BrushFill((Gdiplus::Region *)nullptr, m_PenBrushPlus.get(), m_PenModel.m_iPenOpacity);

   //}
   //else 
   //{

   //   if (m_PenModel.m_nPenType > kPenColor)
   //   {

   //      PatternFill((CRgn *)nullptr, m_pbitmapPenPattern.get(), m_iPackedItemPenFill, m_PenModel.m_iPenOpacity);

   //   }
   //   else
   //   {

   //      BrushFill((Gdiplus::Region *)nullptr, m_PenBrushPlus.get(), m_PenModel.m_iPenOpacity);

   //   }

   //}

}



//void CDraftContext::PaintPenRgn(Gdiplus::Region *pRgn)
//{
//   
//   if (m_PenModel.m_nPenType == kPenInvis)
//   {
//
//      return;
//
//   }
//
//
//
//   //if(GetObjectType(*m_pDC) == OBJ_ENHMETADC) 
//   //{
//   //   
//   //   BrushFill(pRgn,m_PenBrushPlus.get(),m_PenModel.m_iPenOpacity);
//
//   //}
//   //else 
//   //{
//
//   //   if (m_PenModel.m_nPenType > kPenColor)
//   //   {
//
//   //      PatternFill(nullptr, m_pbitmapPenPattern.get(), m_iPackedItemPenFill, m_PenModel.m_iPenOpacity);
//
//   //   }
//   //   else
//   //   {
//   //      
//   //      BrushFill(pRgn, m_PenBrushPlus.get(), m_PenModel.m_iPenOpacity);
//
//   //   }
//
//   //}
//}

void CDraftContext::Arc(const LongPoint &lptCenter,const LongPoint &lptRadii,const LongPoint &lptFrom,const LongPoint &lptTo,int nDirection,Angle rotAngle,short cap1,short cap2,bool bAdjustAlign, bool bClose)
{
   // TODO GDI+
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
   //CGDIObject<CBrush> gdiBrush(m_pDC,&m_PenBrush);
   // END TODO GDI+

   //	MoveTo(lptFrom);
   DrawArc(lptCenter,lptRadii,lptFrom,lptTo,nDirection,rotAngle,cap1,cap2,bAdjustAlign, 0, false, bClose);
}

void CDraftContext::Arc(const LongRect &lrc,const LongPoint &lptFrom,const LongPoint &lptTo,int nDirection)
{
   // TODO GDI+
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
   // END TODO GDI+
   CGDIArcDirection gdiArcDir(m_pDC,nDirection);
   m_pDC->Arc(m_pMapper->LPtoDP(lrc),m_pMapper->LPtoDP(lptFrom),m_pMapper->LPtoDP(lptTo));
}

void CDraftContext::Ellipse(const LongPoint& lptCenter,const LongPoint& lptRadii,Angle rotAngle)
{
   // TODO GDI+
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
   // END TODO GDI+
   LongPoint center=lptCenter;
   //	if(rotAngle!=0 || m_bCreatingPath || shouldUseDash() || m_PenModel.m_nPenType>kPenColor)
   {
      // TODO GDI+
      //CGDIObject<CBrush> gdiBrush(m_pDC,&m_PenBrush);
      // END TODO GDI+
      //bool bUsePath=canUsePath();
      //if(bUsePath) bUsePath=BeginPath();

      m_DrawPrim.drawEllipse(this,&center,lptRadii.x,lptRadii.y,rotAngle,true);


      //if(bUsePath)
      //{
        // m_bCreatingPath=false;
        // if(m_pDC->CloseFigure() && m_pDC->EndPath()) // always closed
          //  m_pDC->StrokePath();
      //}
   }
   /*	else
      {
      Gdiplus::Brush brush;
      brush.CreateStockObject(NULL_BRUSH);
      CGDIObject<CBrush> gdiBrush(m_pDC,&brush);

      CRect rc=m_pMapper->LPtoDP(LongRect(lptCenter-lptRadii,lptCenter+lptRadii));
      rc.right++;
      rc.bottom++;
      if(m_PenModel.m_nAlignment!=kCenterLine)
      {
      int nAdjust=m_pMapper->ScreenPenW(m_PenModel.m_nWidth)>>1;
      if(nAdjust)
      {
      if(m_PenModel.m_nAlignment==kInsideLine) nAdjust=-nAdjust;
      rc.InflateRect(nAdjust,nAdjust);
      }
      }

      m_pDC->Ellipse(rc);
      }*/
}

void CDraftContext::RoundRect(const LongRect &lrc,const LongPoint &lpt)
{
   //Gdiplus::Brush brush;
   //brush.CreateStockObject(NULL_BRUSH);
   //CGDIObject<CBrush> gdiBrush(m_pDC,&brush);
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);

   // TODO GDI+
//CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
//CGDIObject<CBrush> gdiBrush(m_pDC,&brush);
// END TODO GDI+
   CRect rc=m_pMapper->LPtoDP(LongRect(LongPoint::kZeroPt,lpt));
   m_pDC->RoundRect(m_pMapper->LPtoDP(lrc),CPoint(rc.Width(),rc.Height()));
}

void CDraftContext::DrawText(const CString& sText,const LongRect& lrc,UINT nFormat)
{
   CGDITextColor tc(m_pDC,m_PenModel.m_crColor);
   CGDIBackMode bm(m_pDC,TRANSPARENT);
   CGDIObject<CFont> font(m_pDC,&m_Font);
   CRect rc=m_pMapper->LPtoDP(lrc);
   m_pDC->DrawText(sText,&rc,nFormat);
}

void CDraftContext::DrawAnnot(const LongPoint& lpt,int nID)
{
   CPoint pt=m_pMapper->LPtoDP(lpt);
   CBitmap bmp;
   bmp.LoadBitmap(nID);
   CGDIBitmapDC mdc(m_pDC,&bmp);
   m_pDC->BitBlt(pt.x,pt.y - 4,7,9,&mdc,0,0,SRCINVERT);
}

CMapper* CDraftContext::GetMapper()
{
   return m_pMapper;
}

bool CDraftContext::BeginPath()
{
   m_bCreatingPath=m_pDC->BeginPath()?true:false;
   if (m_bCreatingPath)
   {

      if (m_ppath)
      {

         m_ppath->StartFigure();

      }

   }
   return m_bCreatingPath;
}

bool CDraftContext::EndPath(CRgn *pRgn)
{
   m_bCreatingPath=false;
   m_pDC->CloseFigure();
   if (m_ppath)
   {

      m_ppath->CloseFigure();

   }
   if(m_pDC->EndPath())
   {
      if(!m_ppath && pRgn && pRgn->CreateFromPath(m_pDC))
      {
         CPoint ptOrg;
         GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
         pRgn->OffsetRgn(-ptOrg);	// temporary fix, should not be here
      }
      return true;
   }
   return false;
}

//bool CDraftContext::IsPenPatternFillEx()
//{
//
//   if(m_PenModel.m_nPenType != kPenPat)
//      return false;
//
//   if(m_pVersionDoc != NULL)
//   {
//    
//      return 
//         m_PenModel.m_nIndex >= 0
//         && m_PenModel.m_nIndex < m_pVersionDoc->m_packeditema.GetCount()
//         && m_pVersionDoc->m_packeditema[m_PenModel.m_nIndex].m_etype == packed_type_gradient;
//
//   }
//
//   if(m_pDoc != NULL)
//   {
//
//      return 
//         m_PenModel.m_nIndex >= 0
//         && m_PenModel.m_nIndex < m_pVersionDoc->m_packeditema.GetCount()
//         && m_pDoc->m_packeditema[m_PenModel.m_nIndex].m_etype == packed_type_gradient;
//
//   }
//
//   return false;
//
//}

Gdiplus::Status HBitmapToBitmap(HBITMAP source,Gdiplus::PixelFormat pixel_format,Gdiplus::Bitmap** result_out)
{
   BITMAP source_info ={0};
   if(!::GetObject(source,sizeof(source_info),&source_info))
      return Gdiplus::GenericError;

   Gdiplus::Status s;
#undef new
   std::auto_ptr< Gdiplus::Bitmap > target(new Gdiplus::Bitmap(source_info.bmWidth,source_info.bmHeight,pixel_format));
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
   if(!target.get())
      return Gdiplus::OutOfMemory;
   if((s = target->GetLastStatus()) != Gdiplus::Ok)
      return s;

   Gdiplus::BitmapData target_info;
   Gdiplus::Rect rect(0,0,source_info.bmWidth,source_info.bmHeight);

   s = target->LockBits(&rect,Gdiplus::ImageLockModeWrite,pixel_format,&target_info);
   if(s != Gdiplus::Ok)
      return s;

   if(target_info.Stride != source_info.bmWidthBytes)
      return Gdiplus::InvalidParameter; // pixel_format is wrong!

   for(int i = 0; i < source_info.bmHeight; i++)
   {
      CopyMemory(&((unsigned char *)target_info.Scan0)[(source_info.bmHeight - i - 1) *source_info.bmWidthBytes],
         &((unsigned char *)source_info.bmBits)[i * source_info.bmWidthBytes],source_info.bmWidthBytes);

   }

   s = target->UnlockBits(&target_info);
   if(s != Gdiplus::Ok)
      return s;

   *result_out = target.release();

   return Gdiplus::Ok;
}

void CDraftContext::StretchBlt(const CRect& src,const LongRect &lrc,HBITMAP hDib,bool bUseFillModel)
{
   if(m_bGreyMode)
   {
      MoveTo(lrc.GetCorner(3));
      for(int i = 0; i < 4; i++) LineTo(lrc.GetCorner(i));
      return;
   }

   CRect rc = m_pMapper->LPtoDP(lrc);
   CRect rcClip = rc;
   //CRect rcClip;
   //m_pDC->GetClipBox(rcClip);
   //Gdiplus::Matrix m;
   //rcClip.IntersectRect(rcClip,rc);
   //if(rcClip.IsRectEmpty())
   //   return;

   LongRect lrcClip = m_pMapper->DPtoLP(rcClip);
   //	rcClip = m_pMapper->LPtoDP(lrcClip);

   CRect srcClip;
   WrFixed wrTmp;
   wrTmp = (lrcClip.left - lrc.left) / (lrc.right - lrc.left) * src.Width();
   srcClip.left = wrTmp.GetLong() + src.left - 1;
   wrTmp = (lrcClip.right - lrc.left) / (lrc.right - lrc.left) * src.Width();
   srcClip.right = wrTmp.GetLong() + src.left + 1;
   wrTmp = (lrcClip.top - lrc.top) / (lrc.bottom - lrc.top) * src.Height();
   srcClip.top = wrTmp.GetLong() + src.top - 1;
   wrTmp = (lrcClip.bottom - lrc.top) / (lrc.bottom - lrc.top) * src.Height();
   srcClip.bottom = wrTmp.GetLong() + src.top + 1;

   srcClip &= src;
   lrcClip.left = lrc.left + (lrc.right - lrc.left) / src.Width() * (srcClip.left - src.left) ;
   //	lrcClip.left = lrc.left + (lrc.right.GetLong() - lrc.left.GetLong()) * (srcClip.left - src.left) / src.Width();

   lrcClip.right = lrc.left + (lrc.right - lrc.left) / src.Width() * (srcClip.right - src.left) ;
   //	lrcClip.right = lrc.left + (lrc.right.GetLong() - lrc.left.GetLong()) * (srcClip.right - src.left) / src.Width();

   lrcClip.top = lrc.top + (lrc.bottom - lrc.top) / src.Height() * (srcClip.top - src.top);
   //	lrcClip.top = lrc.top + (lrc.bottom.GetLong() - lrc.top.GetLong()) * (srcClip.top - src.top) / src.Height();

   lrcClip.bottom = lrc.top + (lrc.bottom - lrc.top) / src.Height() * (srcClip.bottom - src.top);
   //	lrcClip.bottom = lrc.top + (lrc.bottom.GetLong() - lrc.top.GetLong()) * (srcClip.bottom - src.top) / src.Height();

   rcClip = m_pMapper->LPtoDP(lrcClip);

   //if(m_FillModel.m_nFillType == kFillColor && (m_FillModel.m_crColor == PALETTEINDEX(0) || m_FillModel.m_crColor == RGB(255,255,255)))
   {

      // if we fill the transparent region by the custom color/pattern
      {
         //CRgn Rgn;
         //Rgn.CreateRectRgn(rcClip.left,rcClip.top,rcClip.right,rcClip.bottom);

         Gdiplus::Rect r(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height());

         path_new(false);
         
         path()->AddRectangle(r);

         PaintPath();

      }

      // no fill or transparency - simly draw
      //CGDIBitmapDC dcImage(m_pDC,CBitmap::FromHandle(hDib));
      // m_pDC->StretchBlt(rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height(),&dcImage,srcClip.left,srcClip.top,srcClip.Width(),srcClip.Height(),SRCCOPY);
      //return ;
      
      Gdiplus::Bitmap * pbitmap = NULL;

      HBitmapToBitmap(hDib,PixelFormat32bppARGB,&pbitmap);

      graphics_on();

      Gdiplus::GraphicsState state = m_pgraphics->Save();

      if(m_pDC->IsPrinting())
      {

         m_pgraphics->SetPageUnit(Gdiplus::UnitPixel);

         double dQuality = (double)(m_pDoc->m_PrinterSettings.GetQuality());

         double dRatio = m_pMapper->LPtoDP(100.0);

         double dpiRatio = 80000.0 * dQuality / (dRatio * 300.0);

         //m_pgraphics->SetPageScale(dpiRatio);

         //m_pgraphics->ScaleTransform(dQuality,300.0 / dQuality);

      }

      Gdiplus::Rect destRect;

      destRect.X = rcClip.left;

      destRect.Y = rcClip.top;

      destRect.Width = rcClip.Width();

      destRect.Height = rcClip.Height();


      m_pgraphics->DrawImage(pbitmap,destRect,srcClip.left,srcClip.top,srcClip.Width(),srcClip.Height(),Gdiplus::UnitPixel);

      m_pgraphics->Restore(state);

   }
   /*else
   {
      // 1. Create mask:  m
      // 2. Put mask to the background: bckgrnd * mask
      // 3. Make the combination  image * !mask
      // 4. Put the combination to the modified background
      // Note: one blink will occur

      // if we fill the transparent region by the custom color/pattern
      {
         CRgn Rgn;
         Rgn.CreateRectRgn(rcClip.left,rcClip.top,rcClip.right,rcClip.bottom);
         PaintRgn(&Rgn);
      }

      // make the necessary conversions
      {
         m_pDC->SetStretchBltMode(COLORONCOLOR);
         DrawTransparent(m_pDC,rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height(),CBitmap::FromHandle(hDib),srcClip.left,srcClip.top,srcClip.Width(),srcClip.Height(),RGB(255,255,255));
      }
   }*/
}

void CDraftContext::DrawDimMarker(const LongPoint &lpt)
{
   CPoint pt=m_pMapper->LPtoDP(lpt);
   CRect rc(pt.x - 4,pt.y - 4,pt.x + 5,pt.y + 5);

   CPen pen;
   pen.CreateStockObject(BLACK_PEN);
   CGDIObject<CPen> gdiPen(m_pDC,&pen);

   m_pDC->MoveTo(pt.x - 5,pt.y - 5);
   m_pDC->LineTo(pt.x + 6,pt.y + 6);
   m_pDC->MoveTo(pt.x + 5,pt.y - 5);
   m_pDC->LineTo(pt.x - 6,pt.y + 6);

   CBrush brush;
   brush.CreateStockObject(NULL_BRUSH);
   CGDIObject<CBrush> gdiBrush(m_pDC,&brush);

   m_pDC->Ellipse(&rc);
}

void CDraftContext::SetGreyPen()
{
   // TODO GDI+
//CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
//CGDIObject<CBrush> gdiBrush(m_pDC,&m_PenBrush);
// END TODO GDI+
 /*  if(m_Pen.GetSafeHandle()) m_Pen.DeleteObject();
   if(m_PenBrush.GetSafeHandle()) m_PenBrush.DeleteObject();
   if(m_Brush.GetSafeHandle()) m_Brush.DeleteObject();

   m_Pen.CreatePen(PS_DOT,1,RGB(0,0,0));*/
   // TODO GDI+
//CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
//CGDIObject<CBrush> gdiBrush(m_pDC,&m_PenBrush);
// END TODO GDI+
}

void CDraftContext::SetOutlinePen(bool bReset)
{
   //if(bReset)
   //{
   //   PenModel pm;
   //   //		pm.m_nWidth=0x81;
   //   SetPen(pm);
   //}
   m_bOutline=true;
}


bool CDraftContext::IsOutlinePen() const
{

   return m_bOutline;

}


void CDraftContext::CalcBoundRect(const CStyledText& st,const TextParams& tp,CRect& rcBound,bool bRelaxedBox,bool bAutoAdjustBoundRect,int nWidth)
{
   
   rcBound.SetRect(0, 0, 0, 0);

   int pos = st.GetFirstTextPart();

   double lastLineAscent = -1.0;

   double dLastDescent = 0.0;

   while(pos != -1)
   {
      
      double dLineSpacing, dAscent, dDescent, width;

      pos = CalcLineExtent(st, pos, dLineSpacing, dAscent, dDescent, width, bRelaxedBox, bAutoAdjustBoundRect, nWidth);

      if(lastLineAscent < 0.0)
      {

         lastLineAscent = dLineSpacing;

      }
      else
      {
         
         double n = dLineSpacing;

         switch(tp.m_nSpacing)
         {
         case kSingleSpace: break;
         case k1_5Space: n *= 1.5; break;
         case kDoubleSpace: n *= 2.0; break;
         default:
            ASSERT(FALSE);
         }
         lastLineAscent +=n;
      }
      if(width > rcBound.right) rcBound.right=width;
   }
   if(lastLineAscent == -1)
   {
      SelectFont(tp.m_lf);
      TEXTMETRIC tm;
      GetTextMetrics(&tm);
      UnselectFont();
      rcBound.right=1;
      rcBound.bottom=tm.tmAscent + tm.tmDescent + tm.tmExternalLeading;
   }
   else
   {

      rcBound.bottom = lastLineAscent + dLastDescent;

   }

}


int CDraftContext::CalcLineExtent(const CStyledText& st,int pos,double &dLineSpacing, double &dAscent, double &dDescent,double &width,bool bRelaxedBox, bool bAutoAdjustBoundRect,int nWidth)
{
   int aPos=pos;
   double dLineSpacingLocal = -1;
   dAscent = 0;
   dDescent=0;
   width=0;
   int breakPos=-1;
   int breakWidth;
   short nWhere=0;
   graphics_on();
   double dWidth = 0.;
   while(pos != -1)
   {
      CString theText;
      CString theTextPlus1;
      CStyledTextStyle style;
      bool bLineFeed;
      int oldPos=pos;
      st.GetNextTextPart(pos,theText, theTextPlus1, style,bLineFeed);
      if (pos < 0)
      {

         break;

      }

      SelectFont(style.m_LogFont);

      //TEXTMETRIC tm;
      //m_pDC->GetTextMetrics(&tm);

      Gdiplus::Font * pfont = font();

      Gdiplus::FontFamily fm;

      pfont->GetFamily(&fm);

      int iStyle = pfont->GetStyle();

      double dFontSize = pfont->GetSize();

      UINT16 uAscent = fm.GetCellAscent(iStyle);

      UINT16 uHeight = fm.GetEmHeight(iStyle);

      double tmAscent = pfont->GetSize() * uAscent / uHeight;

      if (dAscent == 0.0)
      {

         dAscent = tmAscent;

      }

      UINT16 uDescent = fm.GetCellDescent(iStyle);

      UINT16 uLineSpacing = fm.GetLineSpacing(iStyle);

      UINT16 uAscentPlusDescent = uAscent + uDescent;

      double dHeight = pfont->GetHeight(0.0f);

      double tmLineSpacing = pfont->GetSize() * uLineSpacing / uHeight;

      if (dLineSpacingLocal < tmLineSpacing)
      {

         dLineSpacingLocal = tmLineSpacing;

      }

      if (dDescent < (tmLineSpacing - tmAscent))
      {
      
         dDescent = (tmLineSpacing - tmAscent);

      }

      CStringW wstr(get_uni(theText, CP_ACP));

      Gdiplus::RectF rBounds;

      Gdiplus::PointF pointNull(0, 0);

      bool bVertical = style.m_LogFont.lfFaceName[0] == '@';

      m_pgraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

      if (bAutoAdjustBoundRect)
      {

         Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericDefault());

         if (bVertical)
         {

            format.SetLineAlignment(Gdiplus::StringAlignmentNear);

            format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsDirectionVertical);

         }

         if (bRelaxedBox)
         {

            CStringW wstrText(get_uni(theText + ".", CP_ACP));

            m_pgraphics->MeasureString(wstrText, wstrText.GetLength(), pfont, Gdiplus::PointF(0, 0), &format, &rBounds);

            dWidth = ceil(dWidth) + ceil((bVertical ? rBounds.Height : rBounds.Width));

         }
         else
         {

            CStringW wstrText(get_uni(theText, CP_ACP));

            CStringW wstrTextPlus1(get_uni(theTextPlus1, CP_ACP));

            Gdiplus::CharacterRange cr;

            cr.First = 0;

            cr.Length = wstrText.GetLength();

            Gdiplus::RectF layoutRect(0.f, 0.f, 65535.f, 65535.f);

            format.SetMeasurableCharacterRanges(1, &cr);

            Gdiplus::Region r;

            m_pgraphics->MeasureCharacterRanges(wstrTextPlus1, wstrTextPlus1.GetLength(), pfont, layoutRect, &format, 1, &r);

            r.GetBounds(&rBounds, m_pgraphics);

            dWidth += bVertical ? rBounds.Height : rBounds.Width;

         }

      }
      else
      {

         int iSkip;

         CStringW wstrTextPlus1(get_uni(theTextPlus1, CP_ACP));

         double w = dWidth;

         const char * pStart = theText;
         int nPosStart = 0;
         int nWideCharStart = 0;
         double wStart = dWidth;
         int oldPosStart = oldPos;
         double dWidthStart = dWidth;
         int breakPosStart = breakPos;
         int breakWidthStart = breakWidth;
         int nWhereStart = nWhere;

         int iPhaseStart;

         if (wstrTextPlus1.GetLength() > 24)
         {

            iPhaseStart = 0;

         }
         else if (wstrTextPlus1.GetLength() > 8)
         {

            iPhaseStart = 1;

         }
         else
         {
         
            iPhaseStart = 2;
         
         }


         for (int iPhase = iPhaseStart; iPhase < 3; iPhase++)
         {

            if (iPhase == 0)
            {

               iSkip = 16;

            }
            else if (iPhase == 1)
            {

               iSkip = 4;

            }
            else
            {

               iSkip = 1;

            }

            int nPos = nPosStart;
            w = wStart;
            int nWideChar = nWideCharStart;
            oldPos = oldPosStart;
            dWidth = dWidthStart;
            breakPos = breakPosStart;
            breakWidth = breakWidthStart;
            nWhere = nWhereStart;

            
            for (LPCTSTR p = pStart; p[nPos];)
            {
               int nCharLen = _tcsinc(p + nPos) - (p + nPos);
               nPos += nCharLen;
               nWideChar++;

               bool bMeasure = iPhase == 2 || (nWideChar % iSkip) == 0;

               if (bMeasure)
               {
                  
                  Gdiplus::CharacterRange cr;

                  cr.First = 0;
                  cr.Length = nWideChar;

                  Gdiplus::RectF layoutRect(0.f, 0.f, 65535.f, 65535.f);

                  Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericDefault());

                  if (bVertical)
                  {

                     format.SetLineAlignment(Gdiplus::StringAlignmentNear);

                     format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsDirectionVertical);

                  }


                  format.SetMeasurableCharacterRanges(1, &cr);

                  Gdiplus::Region r;
                  m_pgraphics->MeasureCharacterRanges(wstrTextPlus1, min(wstrTextPlus1.GetLength(), nWideChar + 1), pfont, layoutRect, &format, 1, &r);

                  //m_pgraphics->MeasureString(wstrTextPlus1, min(wstrTextPlus1.GetLength(), nWideChar ), pfont, Gdiplus::PointF(0, 0), &format, &rBounds);

                  r.GetBounds(&rBounds, m_pgraphics);
//                  CStringW wstrPart(wstrTextPlus1.Mid(0, nWideChar));
                  //CSize size=m_pDC->GetTextExtent(theText);
//width+=r.cx;
//width += rBounds.Width;
                  if (dWidth + rBounds.Width > nWidth)
                  {
                     
                     if (iPhase < 2)
                     {


                        break;

                     }

                     bLineFeed = true;
                     if (breakPos == -1)
                     {
                        pos = oldPos;
                     }
                     else
                     {
                        pos = breakPos;
                        w = breakWidth;
                     }
                     break;
                  }

               }
         
               auto charNext = _tcsnextc(p + nPos);
               oldPos += nCharLen;

               if (charNext == '\r'  
                  || charNext == '\n'
                  || charNext == '\t'
                  || charNext == ' ')
               {
                  nWhere = 2;
               }
               else
               {
                  if (nWhere == 2)
                  {
                     breakPos = oldPos;
                     breakWidth = w;
                  }
                  nWhere = 1;
               }
               if (bRelaxedBox)
               {
                  w = ceil(dWidth) + ceil(rBounds.Width);
               }
               else
               {
                  w = dWidth + rBounds.Width;
               }

               if(bMeasure)
               {
                  nPosStart = nPos;
                  nWideCharStart = nWideChar;
                  wStart = w;
                  oldPosStart = oldPos;
                  dWidthStart = dWidth;
                  breakPosStart = breakPos;
                  breakWidthStart = breakWidth;
                  nWhereStart = nWhere;
               }



            }
         }

         if (bRelaxedBox)
         {
            dWidth = ceil(w);
         }
         else
         {
            dWidth = w;
         }

      }
      UnselectFont();
      if(bLineFeed)
         break;
   }
   width = dWidth;
   if (dLineSpacingLocal >= 0)
   {
      dLineSpacing = dLineSpacingLocal;
   }
   return aPos == pos?(st.GetNextPos(pos)):pos;
}

HBITMAP CreateAlphaTextBitmap(LPCSTR inText,HFONT inFont,COLORREF inColour)
{
   int TextLength = (int)strlen(inText);
   if(TextLength <= 0) return NULL;

   // Create DC and select font into it 
   HDC hTextDC = CreateCompatibleDC(NULL);
   HFONT hOldFont = (HFONT)SelectObject(hTextDC,inFont);
   HBITMAP hMyDIB = NULL;

   // Get text area 
   RECT TextArea ={0,0,0,0};
   DrawText(hTextDC,inText,TextLength,&TextArea,DT_CALCRECT);
   if((TextArea.right > TextArea.left) && (TextArea.bottom > TextArea.top))
   {
      BITMAPINFOHEADER BMIH;
      memset(&BMIH,0x0,sizeof(BITMAPINFOHEADER));
      void *pvBits = NULL;

      // Specify DIB setup 
      BMIH.biSize = sizeof(BMIH);
      BMIH.biWidth = TextArea.right - TextArea.left;
      BMIH.biHeight = TextArea.bottom - TextArea.top;
      BMIH.biPlanes = 1;
      BMIH.biBitCount = 32;
      BMIH.biCompression = BI_RGB;

      // Create and select DIB into DC 
      hMyDIB = CreateDIBSection(hTextDC,(LPBITMAPINFO)&BMIH,0,(LPVOID*)&pvBits,NULL,0);
      HBITMAP hOldBMP = (HBITMAP)SelectObject(hTextDC,hMyDIB);
      if(hOldBMP != NULL)
      {
         // Set up DC properties 
         SetTextColor(hTextDC,0x00FFFFFF);
         SetBkColor(hTextDC,0x00000000);
         SetBkMode(hTextDC,OPAQUE);

         // Draw text to buffer 
         DrawText(hTextDC,inText,TextLength,&TextArea,DT_NOCLIP);
         BYTE* DataPtr = (BYTE*)pvBits;
         BYTE FillR = GetRValue(inColour);
         BYTE FillG = GetGValue(inColour);
         BYTE FillB = GetBValue(inColour);
         BYTE ThisA;
         for(int LoopY = 0; LoopY < BMIH.biHeight; LoopY++) {
            for(int LoopX = 0; LoopX < BMIH.biWidth; LoopX++) {
               ThisA = *DataPtr; // Move alpha and pre-multiply with RGB 
               *DataPtr++ = (FillB * ThisA) >> 8;
               *DataPtr++ = (FillG * ThisA) >> 8;
               *DataPtr++ = (FillR * ThisA) >> 8;
               *DataPtr++ = ThisA; // Set Alpha 
            }
         }

         // De-select bitmap 
         SelectObject(hTextDC,hOldBMP);
      }
   }

   // De-select font and destroy temp DC 
   SelectObject(hTextDC,hOldFont);
   DeleteDC(hTextDC);

   // Return DIBSection 
   return hMyDIB;
}
void TestAlphaText(HDC inDC,int inX,int inY,const char *DemoText, int iAlpha)
{
   RECT TextArea ={0,0,0,0};
   HFONT TempFont = (HFONT) ::GetCurrentObject(inDC,OBJ_FONT);
   HBITMAP MyBMP = CreateAlphaTextBitmap(DemoText,TempFont,0xFF);
   DeleteObject(TempFont);
   if(MyBMP)
   {
      // Create temporary DC and select new Bitmap into it 
      HDC hTempDC = CreateCompatibleDC(inDC);
      HBITMAP hOldBMP = (HBITMAP)SelectObject(hTempDC,MyBMP);
      if(hOldBMP)
      {
         // Get Bitmap image size
         BITMAP BMInf;
         GetObject(MyBMP,sizeof(BITMAP),&BMInf);

         // Fill blend function and blend new text to window 
         BLENDFUNCTION bf;
         bf.BlendOp = AC_SRC_OVER;
         bf.BlendFlags = 0;
         bf.SourceConstantAlpha =iAlpha;
         bf.AlphaFormat = AC_SRC_ALPHA;
         AlphaBlend(inDC,inX,inY,BMInf.bmWidth,BMInf.bmHeight,hTempDC,0,0,BMInf.bmWidth,BMInf.bmHeight,bf);

         // Clean up 
         SelectObject(hTempDC,hOldBMP);
         DeleteObject(MyBMP);
         DeleteDC(hTempDC);
      }
   }
}


void CDraftContext::DrawStyledText(int iOpacity, const CStyledText &st,const LongPoint *lptCorners,bool bAutoAdjustBoundRect,CRgn* pClip,int nStartChar,int nEndChar)
{

   if(m_bGreyMode)
   {

      MoveTo(lptCorners[3]); 

      for (int i = 0; i < 4; i++)
      {

         LineTo(lptCorners[i]);

      }

   }
   else
   {

      Angle rotAngle = mtAngle(lptCorners[0],lptCorners[1]);

      LongPoint lptCenter = Average(lptCorners[0],lptCorners[2]);

      int nWidth = m_pMapper->LPtoDP(Pythag(lptCorners[1] - lptCorners[0]) - 6);

      //CRgn oldClip;
      //if(pClip)
      //{
      //   SaveClip(oldClip);
      //   CPoint ptOrg;
      //   GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
      //   pClip->OffsetRgn(ptOrg);	// temporary fix, should not be here
      //   m_pDC->SelectClipRgn(pClip,RGN_AND);
      //}

      int nLastBkMode=m_pDC->SetBkMode(TRANSPARENT);
      UINT nLastAlign=m_pDC->SetTextAlign(TA_LEFT | TA_BASELINE | TA_UPDATECP);

      CStringW wstrCompositionBeg;
         
      CStringW wstrCompositionEnd;

      int nWideCompositionBeg = -1;
      int nWideCompositionEnd = -1;

      if (m_ecompositionstate != e_composition_state_none)
      {
         
         wstrCompositionBeg = get_uni(CString(st.GetTextPtr(), max(0, m_nCompositionStartChar)), CP_ACP);
         wstrCompositionEnd = get_uni(CString(st.GetTextPtr(), max(0, m_nCompositionEndChar)), CP_ACP);

         nWideCompositionBeg = wstrCompositionBeg.GetLength();
         nWideCompositionEnd = wstrCompositionEnd.GetLength();

      }

      Gdiplus::Color color;
      int pos=st.GetFirstTextPart();
      double lastBaseLine=-1;
      double lastLineAscent = -1;
      double dLastDescent=0;
      int nStart;
      double xLastEnd = 0.;

      CStringW wstr;
      CStringW wstrSel;
      CStringW wstrSelPlus1;
      CStringW wstrBeg;
      CStringW wstrEnd;
      CStringW wstrEndPlus1;
      CStringW wstrChar;
      bool bDraw = false;
      pointf pointDraw;
      int iCharCount = 0;
      double dLineSpacing;
      double dAscent;
      double dDescent = 0.0;
      double width;

      while(pos != -1)
      {
         dLastDescent = dDescent;

         int newPos=CalcLineExtent(st,pos,dLineSpacing,dAscent,dDescent,width,false,bAutoAdjustBoundRect,nWidth);
         if (lastBaseLine <0.0)
         {
            lastBaseLine = dLineSpacing;
         }
         else
         {
            double n = dLineSpacing;
            switch(m_TextParams.m_nSpacing)
            {
            case kSingleSpace: break;
            case k1_5Space: n*= 1.5; break;
            case kDoubleSpace: n*= 2.0; break;
            default:
               ASSERT(FALSE);
            }
            lastBaseLine+=n;
         }
         int offset;
         switch(m_TextParams.m_nJust)
         {
         case kAlignLeft: offset=0; break;
         case kAlignCenter: offset=(nWidth - width) / 2;  break;
         case kAlignRight: offset=nWidth - width; break;
         default:
            ASSERT(FALSE);
         }
         pointf point = m_pMapper->LPtoDP(lptCorners[0]);
         //pointf point = m_pMapper->LPtoDP(lptCorners[0]) + mtRotPoint(LongPoint(m_pMapper->DPtoLP(offset) + 3, m_pMapper->DPtoLP(lastBaseLine) + 3), rotAngle)); + mtRotPoint(LongPoint(m_pMapper->DPtoLP(offset) + 3,m_pMapper->DPtoLP(lastBaseLine ) + 3),rotAngle));
         double dMargin = m_pMapper->LPtoDPF(3);
         bool bNewLine = true;

         int nWidePartStart = 0;

         while(pos != -1)
         {
            CString theText;
            CString theTextPlus1;
            CStyledTextStyle style;
            bool bLineFeed;
            bLineFeed = false;
            int n=pos;
            nStart = pos;
            CStringW wstrStartPart = get_uni(CString(st.GetTextPtr(), nStart), CP_ACP);
            nWidePartStart = wstrStartPart.GetLength();
            st.GetNextTextPart(pos,theText, theTextPlus1,style,bLineFeed);
            if (pos < 0)
            {
               break;
            }
            SelectFont(style.m_LogFont,rotAngle);
            CGDITextColor theTextColor(m_pDC,style.m_TextColor);
            //pointf point = m_pDC->GetCurrentPosition();
            color.SetFromCOLORREF(m_pDoc->translate_color(style.m_TextColor));
            Gdiplus::SolidBrush brush(color);

            int nPos = 0;

            Gdiplus::Font * pfont = font();
            
            Gdiplus::FontFamily fm;
            
            pfont->GetFamily(&fm);

            int iStyle = pfont->GetStyle();

            Gdiplus::PointF pointNull(0, 0);
            Gdiplus::RectF r;

            bool bVertical = style.m_LogFont.lfFaceName[0] == '@';
            bool bIntersectsComposition;

            wstr.Empty();
            wstrSel.Empty();
            wstrBeg.Empty();
            wstrEnd.Empty();
            wstrChar.Empty();

            bDraw = false;
            pointDraw = point;
            iCharCount = 0;
            LPCTSTR p2 = theTextPlus1;
            for(LPCTSTR p=theText;;)
            {

               if (p[nPos])
               {

                  int nCharLen = _tcsinc(p + nPos) - (p + nPos);

                  int nCharLen2 = _tcsinc(p2 + nPos+nCharLen) - (p2 +nPos + nCharLen);

                  wstrChar = get_uni(CString(p + nPos, nCharLen), CP_ACP);

                  CStringW wstrChar2 = get_uni(CString(p2 + nPos + nCharLen, nCharLen2), CP_ACP);

                  nPos += nCharLen;

                  if (n < nStartChar)
                  {

                     wstrBeg += wstrChar;

                  }
                  else if (n >= nStartChar && n < nEndChar)
                  {

                     wstrSel += wstrChar;

                     wstrSelPlus1 = wstrSel + wstrChar2;

                  }
                  else
                  {

                     wstrEnd += wstrChar;

                     wstrEndPlus1 = wstrEnd + wstrChar2;

                  }

                  iCharCount++;

                  graphics_on();

                  wstr += wstrChar;

                  n += nCharLen;

               }

               bool bOverflow = n >= newPos && newPos > 0;

               if((!bAutoAdjustBoundRect && bOverflow) || !p[nPos])
               {

                  bLineFeed = (n + 1 == newPos) || bOverflow;

                  int iStyle = pfont->GetStyle();

                  double dFontSize = pfont->GetSize();

                  UINT16 uAscent = fm.GetCellAscent(iStyle);

                  UINT16 uHeight = fm.GetEmHeight(iStyle);

                  double dAscent = pfont->GetSize() * uAscent / uHeight;

                  auto state = m_pgraphics->Save();

                  Gdiplus::Rect rClip;

                  CRect rc = GetMapper()->LPtoDP(*m_prect);

                  rClip.X = rc.left;
                  rClip.Y = rc.top;
                  rClip.Width = rc.Width();
                  rClip.Height = rc.Height();

                  m_pgraphics->SetClip(rClip, Gdiplus::CombineModeIntersect);

                  m_pgraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

                  if (wstrBeg.GetLength() > 0)
                  {

                     bDraw = true;

                     Gdiplus::RectF rBounds;

                     Gdiplus::CharacterRange cr[3];

                     cr[0].First = 0;

                     cr[0].Length = wstrBeg.GetLength();

                     int iStart = nWidePartStart;

                     int iBeg = iStart;

                     int iEnd = iStart + wstrBeg.GetLength();

#if COMPOSITION_UNDERLINE

                     bIntersectsComposition = m_ecompositionstate != e_composition_state_none
                        && segment_intersects(iBeg, iEnd, nWideCompositionBeg, nWideCompositionEnd);

#else

                     bIntersectsComposition = false;

#endif

                     if (bIntersectsComposition)
                     {

                        iBeg -= iStart;

                        iEnd -= iStart;

                        cr[1].First = 0;

                        cr[1].Length = iBeg;

                        cr[2].First = 0;

                        cr[2].Length = iEnd;

                     }

                     CStringW wstrBegPlus1;

                     if (wstrSel.GetLength() > 0)
                     {

                        wstrBegPlus1 = wstrBeg + wstrSel;

                     }
                     else
                     {

                        wstrBegPlus1 = wstrBeg;
                     }


                     Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericDefault());

                     if (bVertical)
                     {

                        format.SetLineAlignment(Gdiplus::StringAlignmentNear);

                        format.SetAlignment(Gdiplus::StringAlignmentNear);

                        format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsDirectionVertical);

                     }
                     // Set string format.
                     format.SetMeasurableCharacterRanges(bIntersectsComposition ? 3 : 1, cr);

                     Gdiplus::Region r[3];

                     Gdiplus::RectF layoutRect(0.f, 0.f, 65535.f, 65535.f);

                     m_pgraphics->MeasureCharacterRanges(wstrBegPlus1, wstrBegPlus1.GetLength(), pfont, layoutRect, &format, bIntersectsComposition ? 3 : 1, r);

                     r[0].GetBounds(&rBounds, m_pgraphics);

                     m_pgraphics->TranslateTransform(pointDraw.X+offset, pointDraw.Y);

                     if (rotAngle != 0)
                     {

                        m_pgraphics->RotateTransform(-rotAngle.GetDouble());

                     }

                     m_pgraphics->TranslateTransform(dMargin + xLastEnd - (bNewLine ? 0.0 : (bVertical ? rBounds.Y : rBounds.X)), dMargin + lastBaseLine + (bVertical ? 0 : -dAscent));


                     if (bVertical)
                     {

                        m_pgraphics->RotateTransform(-90.0);

                     }

                     m_pgraphics->DrawString(wstrBeg, wstrBeg.GetLength(), pfont, Gdiplus::PointF(0, 0), &format, &brush);

#if !COMPOSITION_FULL_UNDERLINE_ON_END

                     if (m_ecompositionstate == e_composition_state_selected)
                     {

                        bIntersectsComposition = false;

                     }

#endif
#undef new
                     if (bIntersectsComposition)
                     {

                        Gdiplus::Pen * ppen = nullptr;

                        if (m_ecompositionstate == e_composition_state_composing)
                        {

                           ppen = new Gdiplus::Pen(Gdiplus::Color::Black, 1.0);

                           ppen->SetDashOffset(dLineSpacing / 3.0);

                           ppen->SetDashStyle(Gdiplus::DashStyle::DashStyleDash);

                        }
                        else
                        {

                           ppen = new Gdiplus::Pen(Gdiplus::Color::Black, 2.0);

                        }

                        Gdiplus::RectF rectBounds[2];

                        r[1].GetBounds(&rectBounds[0], m_pgraphics);

                        r[2].GetBounds(&rectBounds[1], m_pgraphics);

                        m_pgraphics->DrawLine(ppen, Gdiplus::PointF(rectBounds[0].Width, dLineSpacing), Gdiplus::PointF(rectBounds[1].Width, dLineSpacing));

                        delete ppen;

                     }

                     if (bVertical)
                     {

                        m_pgraphics->RotateTransform(90.0);

                     }

                     m_pgraphics->TranslateTransform(-dMargin - xLastEnd + (bNewLine ? 0.0 : (bVertical ? rBounds.Y : rBounds.X)), -dMargin - lastBaseLine + (bVertical ? 0 : dAscent));

                     if (rotAngle != 0)
                     {

                        m_pgraphics->RotateTransform(rotAngle.GetDouble());

                     }

                     m_pgraphics->TranslateTransform(-pointDraw.X- offset, -pointDraw.Y);

                     // m_pgraphics->TranslateTransform(-xLastEnd - pointDraw.X + (bNewLine ? 0.0 : (bVertical ? rBounds.Y : rBounds.X)), -pointDraw.Y + (bVertical ? 0 : dAscent));

                     xLastEnd += bVertical ? (rBounds.Height + (bNewLine ? rBounds.Y : 0.0)) : (rBounds.Width + (bNewLine ? rBounds.X : 0.0));

                     bNewLine = false;

                  }

                  if (wstrSel.GetLength() > 0)
                  {

                     bDraw = true;
                     
                     Gdiplus::RectF rBounds;

                     Gdiplus::CharacterRange cr[3];

                     cr[0].First = 0;

                     cr[0].Length = wstrSel.GetLength();

                     int iStart = nWidePartStart + wstrBeg.GetLength();

                     int iBeg = iStart;

                     int iEnd = iStart + wstrBeg.GetLength();

#if COMPOSITION_UNDERLINE

                     bIntersectsComposition = m_ecompositionstate != e_composition_state_none
                        && segment_intersects(iBeg, iEnd, nWideCompositionBeg, nWideCompositionEnd);

#else

bIntersectsComposition = false;

#endif

                     if (bIntersectsComposition)
                     {

                        iBeg -= iStart;

                        iEnd -= iStart;

                        cr[1].First = 0;

                        cr[1].Length = iBeg;

                        cr[2].First = 0;

                        cr[2].Length = iEnd;

                     }

                     Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericDefault());

                     if (bVertical)
                     {

                        format.SetLineAlignment(Gdiplus::StringAlignmentNear);

                        format.SetAlignment(Gdiplus::StringAlignmentNear);

                        format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsDirectionVertical);

                     }
                     // Set string format.
                     format.SetMeasurableCharacterRanges(bIntersectsComposition ? 3 : 1, cr);

                     Gdiplus::Region r[3];

                     Gdiplus::RectF layoutRect(0.f, 0.f, 65535.f, 65535.f);

                     m_pgraphics->MeasureCharacterRanges(wstrSelPlus1, wstrSelPlus1.GetLength(), pfont, layoutRect, &format, bIntersectsComposition ? 3 : 1, r);

                     r[0].GetBounds(&rBounds, m_pgraphics);

                     m_pgraphics->TranslateTransform(pointDraw.X+ offset, pointDraw.Y);

                     if (rotAngle != 0)
                     {

                        m_pgraphics->RotateTransform(-rotAngle.GetDouble());

                     }

                     m_pgraphics->TranslateTransform(dMargin + xLastEnd - (bNewLine ? 0.0 : (bVertical ? rBounds.Y : rBounds.X)), dMargin + lastBaseLine + (bVertical ? 0 : -dAscent));

                     if (bVertical)
                     {

                        m_pgraphics->RotateTransform(-90.0);

                     }

                     //Gdiplus::RectF rectInvert(xLastEnd + pointDraw.X, pointDraw.Y - dLineSpacing, rBounds.Width, rBounds.Height);
                     Gdiplus::RectF rectInvert(bVertical ?  (bNewLine ? rBounds.Y : rBounds.Y) : (bNewLine ? rBounds.X : rBounds.X), 0, rBounds.Width, rBounds.Height);

                     //Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255 - m_colorPen.GetR(),
                     //   255 - m_colorPen.GetG(), 255 - m_colorPen.GetB()));

                     m_pgraphics->FillRectangle(&brush, rectInvert);

                     Gdiplus::SolidBrush brushInvert(Gdiplus::Color(255, 255 - color.GetR(),
                        255 - color.GetG(), 255 - color.GetB()));

                     m_pgraphics->DrawString(wstrSel, wstrSel.GetLength(), pfont, Gdiplus::PointF(0, 0), &format, &brushInvert);

                     //CPoint ptEnd=m_pDC->GetCurrentPosition();
                     //CRect rcInvert(pointDraw.X, pointDraw.Y - dLineSpacing,ptEnd.x, point.Y + dDescent);
                     //m_pDC->InvertRect(&rcInvert);

#if !COMPOSITION_FULL_UNDERLINE_ON_END

                     if (m_ecompositionstate == e_composition_state_selected)
                     {

                        bIntersectsComposition = false;

                     }

#endif

#undef new
                     if (bIntersectsComposition)
                     {

                        Gdiplus::Pen * ppen = nullptr;

                        if (m_ecompositionstate == e_composition_state_composing)
                        {

                           ppen = new Gdiplus::Pen(Gdiplus::Color::Black, 1.0);

                           ppen->SetDashOffset(dLineSpacing / 3.0);

                           ppen->SetDashStyle(Gdiplus::DashStyle::DashStyleDash);

                        }
                        else
                        {

                           ppen = new Gdiplus::Pen(Gdiplus::Color::Black, 2.0);

                        }

                        Gdiplus::RectF rectBounds[2];

                        r[1].GetBounds(&rectBounds[0], m_pgraphics);

                        r[2].GetBounds(&rectBounds[1], m_pgraphics);

                        m_pgraphics->DrawLine(ppen, Gdiplus::PointF(rectBounds[0].Width, dLineSpacing), Gdiplus::PointF(rectBounds[1].Width, dLineSpacing));

                        delete ppen;

                     }

                     m_pgraphics->TranslateTransform(-dMargin - xLastEnd + (bNewLine ? 0.0 : (bVertical ? rBounds.Y : rBounds.X)), -dMargin - lastBaseLine + (bVertical ? 0 : dAscent));

                     if (rotAngle != 0)
                     {

                        m_pgraphics->RotateTransform(rotAngle.GetDouble());

                     }

                     m_pgraphics->TranslateTransform(-pointDraw.X- offset, -pointDraw.Y);

                     xLastEnd += bVertical ? (rBounds.Height + (bNewLine ? rBounds.Y : 0.0)) : (rBounds.Width + (bNewLine ? rBounds.X : 0.0));

                     bNewLine = false;

                  }
                  
                  if (wstrEnd.GetLength() > 0)
                  {

                     Gdiplus::RectF rBounds;

                     Gdiplus::CharacterRange cr[3];

                     cr[0].First = 0;

                     cr[0].Length = wstrEnd.GetLength();

                     int iStart = nWidePartStart + wstrBeg.GetLength() + wstrSel.GetLength();

                     int iBeg = iStart;

                     int iEnd = iStart + wstrBeg.GetLength();

#if COMPOSITION_UNDERLINE

                     bIntersectsComposition = m_ecompositionstate != e_composition_state_none
                        && segment_intersects(iBeg, iEnd, nWideCompositionBeg, nWideCompositionEnd);

#else

bIntersectsComposition = false;

#endif


                     if (bIntersectsComposition)
                     {

                        iBeg -= iStart;

                        iEnd -= iStart;

                        cr[1].First = 0;

                        cr[1].Length = iBeg;

                        cr[2].First = 0;

                        cr[2].Length = iEnd;

                     }

                     Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericDefault());

                     if (bVertical)
                     {

                        format.SetLineAlignment(Gdiplus::StringAlignmentNear);

                        format.SetAlignment(Gdiplus::StringAlignmentNear);

                        format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsDirectionVertical);

                     }
                     // Set string format.
                     format.SetMeasurableCharacterRanges(bIntersectsComposition ? 3 : 1, cr);

                     Gdiplus::Region r[3];

                     Gdiplus::RectF layoutRect(0.f, 0.f, 65535.f, 65535.f);

                     m_pgraphics->MeasureCharacterRanges(wstrEndPlus1, wstrEndPlus1.GetLength(), pfont, layoutRect, &format, bIntersectsComposition ? 3 : 1, r);

                     r[0].GetBounds(&rBounds, m_pgraphics);

                     m_pgraphics->TranslateTransform(pointDraw.X+ offset, pointDraw.Y);

                     if (rotAngle != 0)
                     {

                        m_pgraphics->RotateTransform(-rotAngle.GetDouble());

                     }

                     m_pgraphics->TranslateTransform(dMargin + xLastEnd - (bNewLine ? 0.0 : (bVertical ? rBounds.Y : rBounds.X)), dMargin + lastBaseLine + (bVertical ? 0 : -dAscent));

                     if (bVertical)
                     {

                        m_pgraphics->RotateTransform(-90.0);

                     }
                     
                     m_pgraphics->DrawString(wstrEnd, wstrEnd.GetLength(), pfont, Gdiplus::PointF(0, 0), &format, &brush);

#if !COMPOSITION_FULL_UNDERLINE_ON_END

                     if (m_ecompositionstate == e_composition_state_selected)
                     {

                        bIntersectsComposition = false;

                     }

#endif

#undef new
                     if (bIntersectsComposition)
                     {

                        Gdiplus::Pen * ppen = nullptr;

                        if (m_ecompositionstate == e_composition_state_composing)
                        {

                           ppen = new Gdiplus::Pen(Gdiplus::Color::Black, 1.0);

                           ppen->SetDashOffset(dLineSpacing / 3.0);

                           ppen->SetDashStyle(Gdiplus::DashStyle::DashStyleDash);

                        }
                        else
                        {
                           ppen = new Gdiplus::Pen(Gdiplus::Color::Black, 2.0);

                        }

                        Gdiplus::RectF rectBounds[2];

                        r[1].GetBounds(&rectBounds[0], m_pgraphics);

                        r[2].GetBounds(&rectBounds[1], m_pgraphics);

                        m_pgraphics->DrawLine(ppen, Gdiplus::PointF(rectBounds[0].Width, dLineSpacing), Gdiplus::PointF(rectBounds[1].Width, dLineSpacing));

                        delete ppen;

                     }
                     
                     m_pgraphics->TranslateTransform(-dMargin -xLastEnd + (bNewLine ? 0.0 : (bVertical ? rBounds.Y : rBounds.X)),-dMargin -lastBaseLine + (bVertical ? 0 : dAscent));

                     if (rotAngle != 0)
                     {

                        m_pgraphics->RotateTransform(rotAngle.GetDouble());

                     }

                     m_pgraphics->TranslateTransform(-pointDraw.X- offset, -pointDraw.Y);

                     xLastEnd += bVertical ? (rBounds.Height + (bNewLine ? rBounds.Y : 0.0)) : (rBounds.Width + (bNewLine ? rBounds.X : 0.0));

                     bNewLine = false;

                  }

                  //if (!bDraw)
                  //{

                  //   if (rotAngle == 0)
                  //   {

                  //      m_pgraphics->DrawString(wstr, wstr.GetLength(), pfont, pointf(xLastEnd +pointDraw.X, pointDraw.Y-dLineSpacing), &brush);

                  //   }
                  //   else
                  //   {

                  //      m_pgraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

                  //      m_pgraphics->TranslateTransform(xLastEnd +pointDraw.X, pointDraw.Y - dLineSpacing);

                  //      m_pgraphics->RotateTransform(-rotAngle.GetDouble());

                  //      m_pgraphics->DrawString(wstr, wstr.GetLength(), pfont, pointf(0, 0) , m_PenBrushPlus.get());

                  //      m_pgraphics->RotateTransform(rotAngle.GetDouble());

                  //      m_pgraphics->TranslateTransform(-pointDraw.X- pointDraw.X, - pointDraw.Y);

                  //   }

                  //   wstr.Empty();

                  //}
                  //
                  //pos = newPos;
                  
                  if (bLineFeed || (!bAutoAdjustBoundRect && n == newPos))
                  {

                     xLastEnd = 0.0;

                     bLineFeed = true;

                     pos = newPos;

                  }

                  m_pgraphics->Restore(state);

                  break;

               }

            }

          

            //if (!bDraw)
            //{

            //   m_pgraphics->DrawString(wstr, wstr.GetLength(), pfont, pointf(pointDraw.X, pointDraw.Y), m_PenBrushPlus.get());

            //}
            point = pointDraw;
//            UnselectFont();
            if(bLineFeed) break;
         }
      }
      m_pDC->SetBkMode(nLastBkMode);
      m_pDC->SetTextAlign(nLastAlign);

      //if(pClip) m_pDC->SelectClipRgn(&oldClip);
   }
}

void CDraftContext::DrawDimText(const CString &sValue,const CString &sTol1,const CString &sTol2,const LongPoint *lptCorners,const LOGFONT& lf)
{
   if(m_bGreyMode)
   {
      MoveTo(lptCorners[3]);
      for(int i=0;i < 4;i++) LineTo(lptCorners[i]);
   }
   else
   {
      Angle rotAngle=mtAngle(lptCorners[0],lptCorners[1]);
      LongPoint lptCenter=Average(lptCorners[0],lptCorners[2]);
      int nWidth=m_pMapper->LPtoDP(Pythag(lptCorners[1] - lptCorners[0]));
      int nHeight=m_pMapper->LPtoDP(Pythag(lptCorners[2] - lptCorners[1]));

      CGDIBackMode bm(m_pDC,TRANSPARENT);
      CGDITextColor tc(m_pDC,m_TextColor);

      LongPoint lptRef,lptExtent;
      Coord textGap;
      if(m_pDimStd) textGap=m_pMapper->Scalar2Long(m_pDimStd->DimTextGap);
      else textGap=3;

      CPoint pt;
      if(sValue.GetLength())
      {
         SelectFont(lf,rotAngle);
         lptExtent=GetTextExtent(sValue);
         lptRef=Average(lptCorners[0],lptCorners[3]) + mtRotPoint(LongPoint(textGap,-(lptExtent.y >> 1)),rotAngle);
         CGDITextAlign ta(m_pDC,TA_LEFT | TA_TOP);
         pt=m_pMapper->LPtoDP(lptRef);
         //m_pDC->TextOut(pt.x,pt.y,sValue);
         CStringW wstr = get_uni(sValue, CP_UTF8); // Infinisys Ltd. (power two sign/square sign)
         Gdiplus::Font * pfont = font();
         Gdiplus::PointF p;
         p.X = __pointf(pt).X;
         p.Y = __pointf(pt).Y;
         Gdiplus::PointF p2;
         p2.X =- __pointf(pt).X;
         p2.Y = -__pointf(pt).Y;
         m_pgraphics->TranslateTransform(p.X, p.Y);
         m_pgraphics->RotateTransform(-rotAngle.GetLong());
         m_pgraphics->DrawString(wstr, wstr.GetLength(), pfont, Gdiplus::PointF(0,0), m_PenBrushPlus.get());
         m_pgraphics->RotateTransform(+rotAngle.GetLong());
         m_pgraphics->TranslateTransform(p2.X, p2.Y);
      }

      if(sTol1.GetLength())
      {
         SelectFont(lf,rotAngle,true);
         lptExtent=GetTextExtent(sTol1);
         if(sTol2.GetLength() == 0) lptRef=Average(lptCorners[1],lptCorners[2]) + mtRotPoint(LongPoint(-textGap,-(lptExtent.y >> 1)),rotAngle);
         else
         {
            if(sValue.GetLength() == 0) lptRef=Average(lptCorners[0],lptCorners[1]) + mtRotPoint(LongPoint(lptExtent.x >> 1,3),rotAngle);
            else
            {
               if(m_pDimStd)
               {
                  LongRatio ratio;
                  ratio.fN=m_pDimStd->nToleranceSpaceScale;
                  ratio.fD=100;
                  lptExtent.y+=(lptExtent.y*ratio - lptExtent.y) >> 1;
               }
               lptRef=Average(lptCorners[1],lptCorners[2]) + mtRotPoint(LongPoint(-textGap,-lptExtent.y),rotAngle);
            }
         }
         CGDITextAlign ta(m_pDC,TA_RIGHT | TA_TOP);
         CPoint pt=m_pMapper->LPtoDP(lptRef);
         m_pDC->TextOut(pt.x,pt.y,sTol1);

         if(sTol2.GetLength())
         {
            lptExtent=GetTextExtent(sTol2);
            if(sValue.GetLength() == 0) lptRef=Average(lptCorners[2],lptCorners[3]) + mtRotPoint(LongPoint(lptExtent.x >> 1,-3),rotAngle);
            else
            {
               if(m_pDimStd)
               {
                  LongRatio ratio;
                  ratio.fN=m_pDimStd->nToleranceSpaceScale;
                  ratio.fD=100;
                  lptExtent.y+=(lptExtent.y*ratio - lptExtent.y) >> 1;
               }
               lptRef=Average(lptCorners[1],lptCorners[2]) + mtRotPoint(LongPoint(-textGap,lptExtent.y),rotAngle);
            }
            CGDITextAlign ta(m_pDC,TA_RIGHT | TA_BOTTOM);
            CPoint pt=m_pMapper->LPtoDP(lptRef);
            m_pDC->TextOut(pt.x,pt.y,sTol2);
         }
      }
      UnselectFont();
   }
}

LongPoint CDraftContext::GetTextExtent(const CString &sText,LOGFONT *lf,bool bTolerance)
{
   
   if(lf) SelectFont(*lf,0,bTolerance);
   
   //CSize ext=m_pDC->GetTextExtent(sText);

   ASSERT(m_pDC->m_hAttribDC != NULL);

   SIZE size;

   CStringW wstr = get_uni(sText, CP_ACP);

   Gdiplus::PointF origin(0, 0);

   Gdiplus::RectF r;

   Gdiplus::Font * pfont = font();

   graphics_on();

   bool bVertical = false;

   if (lf)
   {

      bVertical = lf->lfFaceName[0] == '@';

   }

   Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericDefault());

   if (bVertical)
   {

      format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsDirectionVertical);

   }

   m_pgraphics->MeasureString(wstr, wstr.GetLength(), pfont, origin, &format, &r);

   //::GetTextExtentPoint32W(m_pDC->m_hAttribDC,wstr,(int)wstr.GetLength(),&size);

   if(lf) UnselectFont();

   return LongPoint(m_pMapper->DPtoLP(r.Width),m_pMapper->DPtoLP(r.Height));

}


CSize CDraftContext::GetTextExtent(LPCSTR sText,int nCount)
{
   return m_pDC->GetTextExtent(sText,nCount);
}

BOOL CDraftContext::GetTextMetrics(LPTEXTMETRIC lpTm)
{
   return m_pDC->GetTextMetrics(lpTm);
}

bool CDraftContext::SelectFont(const LOGFONT &lf,Angle rotAngle,bool bTolerance)
{
   UnselectFont();
   m_pgdifont.reset(new CFont);
   LOGFONT lfScaled=lf;
   lfScaled.lfHeight=m_pMapper->NoZoomToZoom(lfScaled.lfHeight);
   lfScaled.lfHeight=MulDiv(lfScaled.lfHeight,72,/*m_pDC->GetDeviceCaps(LOGPIXELSY)*/96);
   if(bTolerance && m_pDimStd)
   {
      lfScaled.lfHeight=MulDiv(lfScaled.lfHeight,m_pDimStd->nToleranceTextScale,100);
   }
   if(lfScaled.lfHeight == 0) lfScaled.lfHeight=1;
   if(rotAngle != 0)
   {
      lfScaled.lfEscapement=lfScaled.lfOrientation=(rotAngle * 10).GetLong();
   }
#undef new
   m_pfont.reset(new Gdiplus::Font(m_pDC->GetSafeHdc(), &lfScaled));
   m_pgdifont->CreateFontIndirectA(&lfScaled);
   m_pOldFont = m_pDC->SelectObject(m_pgdifont.get());
   ASSERT(m_pOldFont != NULL);
   return false;
}

void CDraftContext::UnselectFont()
{
   if(m_pOldFont)
   {
      CFont* pFont=m_pDC->SelectObject(m_pOldFont);
      m_pOldFont=NULL;
   }
}

void CDraftContext::Rectangle(const LongPoint &lptTopLeft,const LongPoint &lptRightBottom,Angle rotAngle)
{
   LongRect lrc;
   lrc.SetToBound(lptTopLeft,mtRotPoint(lptRightBottom,-rotAngle,lptTopLeft));
   lrc.Normalize();	// ???
   AdjustBorder(lrc);
   LongPoint lpt[4];
   LongPoint lptIn[4];
   LongPoint lptOut[4];
//   LongPoint lpa[8];
   int lineThickness = 0;
   if (!IsOutline())
   {
      const PenModel* penModelPtr = &GetPen();
      int penWidth = penModelPtr->m_nWidth;
      lineThickness = m_pMapper->ScreenPenW(penWidth);
      if (lineThickness <= 0)
         lineThickness = 1;


   }

   for(int i=0;i < 4;i++)
   {
      LongRect lrcIn(lrc);
      LongRect lrcOut(lrc);
      AdjustInside(lrcIn, i % 2 == 0 ? e_orientation_horizontal : e_orientation_vertical);
      AdjustOutside(lrcOut, i % 2 == 0 ? e_orientation_horizontal : e_orientation_vertical);
      LongRect lrcBound1(lrc);
      LongRect lrcBound2;
      AdjustBounds(lrcBound1);
      const LongPoint lptTopLeft2 = lrcBound1.GetCorner(0);
      lrcBound2.left.from_raw_long(INT_MAX);
      lrcBound2.top.from_raw_long(INT_MAX);
      lrcBound2.bottom.from_raw_long(INT_MIN);
      lrcBound2.right.from_raw_long(INT_MIN);
      lpt[i] = mtRotPoint(lrc.GetCorner(i), rotAngle, lptTopLeft);
      lptIn[i] = mtRotPoint(lrcIn.GetCorner(i), rotAngle, lptTopLeft);
      lptOut[i] = mtRotPoint(lrcOut.GetCorner(i), rotAngle, lptTopLeft);
      const LongPoint lptTopLeft3 = mtRotPoint(lrcBound1.GetCorner(i),rotAngle,lptTopLeft);
      lrcBound2.left = min(lrcBound2.left,lptTopLeft3.x);
      lrcBound2.right = max(lrcBound2.right,lptTopLeft3.x);
      lrcBound2.top = min(lrcBound2.top,lptTopLeft3.y);
      lrcBound2.bottom = max(lrcBound2.bottom,lptTopLeft3.y);

   }
   m_DrawPrim.m_bUsePolyHelperIfDashed = true;

   for (int i = 1; i < 5; i++)
   {

      auto p1 = lpt[(i + 3) % 4];
      auto p2 = lpt[i % 4];

      if (!GetPen().isPenStyleRound())
      {

         auto differencey = p2.y - p1.y;

         auto differencex = p2.x - p1.x;

         auto angle = atan2(differencey.GetDouble(), differencex.GetDouble());

         auto cosine = cos(angle) * m_pMapper->DPtoLP(lineThickness / 2);

         auto sine = sin(angle) * m_pMapper->DPtoLP(lineThickness / 2);

         p1.x -= cosine;
         p1.y -= sine;

         p2.x += cosine;
         p2.y += sine;

      }

      MoveTo(p1);
      OutsideLineTo(p2);

      //m_bSwapInsideOutside = !m_bSwapInsideOutside;

      //if (!IsOutline() && lineThickness > 1 && shouldUseDash() && m_DrawPrim.isCurrentDashFilled())
      //{
      //   
      //   pointd p1;
      //   pointd p2;
      //   pointd p3;

      //   m_pMapper->LPtoDP(p1, lptOut[i % 4]);
      //   m_pMapper->LPtoDP(p2, lptIn[i % 4]);
      //   m_pMapper->LPtoDP(p3, lpt[i % 4]);

      //   m_DrawPrim.m_helperPoly.PushHeadPoint(p2, this);
      //   m_DrawPrim.m_helperPoly.PushTailPoint(p1, this);
      //   m_DrawPrim.m_CurrentPt = p3;

      //}
   }
   if (!IsOutline() && lineThickness > 1 && shouldUseDash())
   {

      m_DrawPrim.m_helperPoly.DrawNow(this);
      m_DrawPrim.m_helperPoly.Reset();
   }
   m_DrawPrim.m_bUsePolyHelperIfDashed = false;
}

void CDraftContext::RoundRect(const LongPoint &lptTopLeft,const LongPoint &lptBottomRight,const LongPoint &lptDelta,Angle rotAngle)
{
   // TODO GDI+
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
   //CGDIObject<CBrush> gdiBrush(m_pDC,&m_PenBrush);
   // END TODO GDI+
   LongRect lrc;
   lrc.SetToBound(lptTopLeft,mtRotPoint(lptBottomRight,-rotAngle,lptTopLeft));
   lrc.Normalize();	// ???
   LongPoint delta=lptDelta;
   AdjustBorder(lrc,&delta);
   LongRect lrcBound1(lrc);
   LongRect lrcBound2;
   AdjustBounds(lrcBound1);
   const LongPoint lptTopLeft2 = lrcBound1.GetCorner(0);
   lrcBound2.left.from_raw_long(INT_MAX);
   lrcBound2.top.from_raw_long(INT_MAX);
   lrcBound2.bottom.from_raw_long(INT_MIN);
   lrcBound2.right.from_raw_long(INT_MIN);

   /*		   r0------1r
           r          r
           0--1     10 11     2 3
           |  |  >>  |          |
           3--2      9 8      5 4
           r          r
           r7------6r
           */
   LongPoint lptDeltaX=Abs(LongPoint(delta.x,0));
   LongPoint lptDeltaY=Abs(LongPoint(0,delta.y));

   LongPoint lpt[12];
   lpt[10]=lrc.GetCorner(0) + lptDeltaY;
   lpt[11]=lrc.GetCorner(0) + lptDeltaX + lptDeltaY;
   lpt[0] =lrc.GetCorner(0) + lptDeltaX;
   lpt[1] =lrc.GetCorner(1) - lptDeltaX;
   lpt[2] =lrc.GetCorner(1) - lptDeltaX + lptDeltaY;
   lpt[3] =lrc.GetCorner(1) + lptDeltaY;
   lpt[4] =lrc.GetCorner(2) - lptDeltaY;
   lpt[5] =lrc.GetCorner(2) - lptDeltaY - lptDeltaX;
   lpt[6] =lrc.GetCorner(2) - lptDeltaX;
   lpt[7] =lrc.GetCorner(3) + lptDeltaX;
   lpt[8] =lrc.GetCorner(3) + lptDeltaX - lptDeltaY;
   lpt[9] =lrc.GetCorner(3) - lptDeltaY;
   if(rotAngle != 0)
   {
      for(int i=0;i < 12;i++)
      {
         lpt[i]=mtRotPoint(lpt[i],rotAngle,lptTopLeft);
         const LongPoint lptTopLeft3 =mtRotPoint(lpt[i],rotAngle,lptTopLeft);
         lrcBound2.left = min(lrcBound2.left,lptTopLeft3.x);
         lrcBound2.right = max(lrcBound2.right,lptTopLeft3.x);
         lrcBound2.top = min(lrcBound2.top,lptTopLeft3.y);
         lrcBound2.bottom = max(lrcBound2.bottom,lptTopLeft3.y);

      }
   }
   else
   {
      for(int i=0;i < 12;i++)
      {
         const LongPoint lptTopLeft3 =lpt[i];
         lrcBound2.left = min(lrcBound2.left,lptTopLeft3.x);
         lrcBound2.right = max(lrcBound2.right,lptTopLeft3.x);
         lrcBound2.top = min(lrcBound2.top,lptTopLeft3.y);
         lrcBound2.bottom = max(lrcBound2.bottom,lptTopLeft3.y);

      }
   }

   Coord penW = m_pMapper->DPtoLP((long)m_PenModel.m_nWidth);

   lrcBound2.left -= penW / 2 + 1;
   lrcBound2.top -= penW / 2 + 1;
   lrcBound2.right += penW / 2 + 1;
   lrcBound2.bottom += penW / 2 + 1;

   bool bUsePath=canUsePath();
   Gdiplus::GraphicsPath * ppathDummy = nullptr;
   if(bUsePath) bUsePath=BeginPath();

   Line(lpt[0],lpt[1]);
   DrawArc(lpt[2],delta,lpt[1],lpt[3],AD_CLOCKWISE,rotAngle,0,0,false);
   m_bSwapInsideOutside = true;
   Line(lpt[3],lpt[4]);
   //m_bSwapInsideOutside = false;
   DrawArc(lpt[5],delta,lpt[4],lpt[6],AD_CLOCKWISE,rotAngle,0,0,false);
   Line(lpt[6],lpt[7]);
   DrawArc(lpt[8],delta,lpt[7],lpt[9],AD_CLOCKWISE,rotAngle,0,0,false);
   //m_bSwapInsideOutside = true;
   Line(lpt[9],lpt[10]);
   m_bSwapInsideOutside = false;
   m_bFinalizeDash = true;
   DrawArc(lpt[11],delta,lpt[10],lpt[0],AD_CLOCKWISE,rotAngle,0,0,false, false, false,  true);
   m_bFinalizeDash = false;

   if(bUsePath)
   {
      m_bCreatingPath=false;
      // commented out because it caused extra line to be drawn and 
      // draw arc now draws exactly start and end point
      //		m_pDC->LineTo(m_pMapper->LPtoDP(lpt[0])); // close path
      if(m_pDC->EndPath())
         m_pDC->StrokePath();
   }
   else{
      CloseJoint(lpt[0]);
   }

   int dashThickness = 0;

   if (shouldUseDash())
   {

      const PenModel* penModelPtr = &GetPen();

      int penWidth = penModelPtr->m_nWidth;

      dashThickness = m_pMapper->ScreenPenW(penWidth);

      if (dashThickness <= 0)
      {

         dashThickness = 1;

      }

   }

 /*  if (dashThickness > 1)
   {

      m_DrawPrim.m_helperPoly.DrawNow(this);
      m_DrawPrim.m_helperPoly.Reset();

   }*/

}


void CDraftContext::DrawArc(const LongPoint &lptCenter,const LongPoint &lptRadii,const LongPoint &lptFrom,const LongPoint &lptTo,int nDirection,Angle rotAngle,short cap1,short cap2,bool bAdjustAlign,short nDimEndmarkOut,bool bForceGDI, bool bClose)
{
   if(nDimEndmarkOut == 0 || m_bGreyMode)
   {
      //		if(bForceGDI && (lptRadii.x==lptRadii.y || rotAngle==FixedZero))
      //		{
      //			m_pDC->Arc()
      //		}
      //		else
      {
         //bool bUsePath=canUsePath();
         //if(bUsePath) bUsePath=BeginPath();

         LongPoint center=lptCenter;
         //			MoveTo(lptFrom);
         m_DrawPrim.drawArc(this,&center,lptRadii.x,lptRadii.y,lptFrom,lptTo,rotAngle,true,nDirection,bAdjustAlign, bClose);

         //if(bUsePath)
         //{
         //   m_bCreatingPath=false;
         //   if(m_pDC->EndPath())
         //      m_pDC->StrokePath();
         //}
      }
   }

   if(!m_bGreyMode)
   {
      if(cap1 < 0) cap1=m_PenModel.EndCap1();
      if(cap2 < 0) cap2=m_PenModel.EndCap2();

      if(cap1 || cap2)
      {
         if(cap1)
         {
            LongPoint deltaFrom = lptCenter - lptFrom;
            if(nDirection != AD_CLOCKWISE)
            {
               deltaFrom = -deltaFrom;
            }
            LongPoint lptFrom2(lptFrom.x + deltaFrom.y,lptFrom.y - deltaFrom.x);
            m_DrawPrim.drawEndcap(this,lptFrom,lptFrom2,cap1,kEnd1 | nDimEndmarkOut,m_PenModel,false,false,kDimLineOut);
         }

         if(cap2)
         {
            LongPoint deltaTo = lptCenter - lptTo;
            if(nDirection == AD_CLOCKWISE)
            {
               deltaTo = -deltaTo;
            }
            LongPoint lptTo2(lptTo.x + deltaTo.y,lptTo.y - deltaTo.x);
            m_DrawPrim.drawEndcap(this,lptTo,lptTo2,cap2,kEnd1 | nDimEndmarkOut,m_PenModel,false,false,kDimLineOut);
         }
      }
   }
   m_lptCurrent=lptTo;
}


void CDraftContext::AdjustOutside(LongRect &lrc, enum_orientation eorientation)
{
   //if (m_PenModel.m_nAlignment != kInsideLine)
   {
      
      WrFixed shift = (m_pMapper->PenW(m_PenModel.m_nWidth) - FixedMinDelta);
      
      if (m_PenModel.m_nAlignment == kCenterLine)
      {

         shift >>= 1;

      }


      if (eorientation == e_orientation_horizontal)
      {
         lrc.left -= shift;
         lrc.right += shift;

      }
      else
      {
         lrc.left -= shift;
         lrc.right -= shift;
      }
      if (eorientation == e_orientation_vertical)
      {
         lrc.top += shift;
         lrc.bottom += shift;
      }
   }

}


void CDraftContext::AdjustInside(LongRect &lrc, enum_orientation eorientation)
{
   //if (m_PenModel.m_nAlignment != kOutsideLine)
   {

      WrFixed shift = (m_pMapper->PenW(m_PenModel.m_nWidth) - FixedMinDelta);

      //WrFixed shiftMid(shift);

     /* if (m_PenModel.m_nAlignment == kCenterLine)
      {

         shiftMid >>= 1;

      }
      else if (m_PenModel.m_nAlignment == kOutsideLine)
      {

         shiftMid = 0;

      }*/

      if (eorientation == e_orientation_horizontal)
      {
         auto shift2 = shift << 1;
         lrc.left -= shift2;
         lrc.right += shift2;
      }
      else
      {
      }
      if (eorientation == e_orientation_vertical)
      {
      }
      else
      {
         lrc.top -= shift;
         lrc.bottom -= shift;
      }
   }
}


void CDraftContext::AdjustBorder(LongRect &lrc,LongPoint* lptCorner)
{
   if(m_PenModel.m_nAlignment != kCenterLine)
   {
      WrFixed shift=(m_pMapper->PenW(m_PenModel.m_nWidth) - FixedMinDelta) >> 1;
      if(m_PenModel.m_nAlignment == kInsideLine) shift=-shift;
      lrc.left-=shift;
      lrc.top-=shift;
      lrc.right+=shift;
      lrc.bottom+=shift;
      if(lptCorner)
      {
         lptCorner->x+=shift;
         lptCorner->y+=shift;
      }
   }
}
void CDraftContext::AdjustBounds(LongRect &lrc,LongPoint* lptCorner)
{
   if(m_PenModel.m_nAlignment != kInsideLine)
   {
      WrFixed shift=(m_pMapper->PenW(m_PenModel.m_nWidth) - FixedMinDelta) >> 1;
      if(m_PenModel.m_nAlignment == kOutsideLine) shift*=2;
      lrc.left-=shift;
      lrc.top-=shift;
      lrc.right+=shift;
      lrc.bottom+=shift;
      if(lptCorner)
      {
         lptCorner->x-=shift;
         lptCorner->y-=shift;
      }
   }
}


void CDraftContext::DrawLineDim(const LineDimPoints &ldp,short cap1,short cap2, Gdiplus::GraphicsPath* pClip)
{
   // TODO GDI+
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
   //CGDIObject<CBrush> gdiBrush(m_pDC,&m_PenBrush);
   // END TODO GDI+
   //CRgn oldClip;
   ////	if(m_pDoc && m_pDoc->m_bHideFills) pClip=NULL;
   //if(pClip)
   //{
   //   SaveClip(oldClip);
   //   CPoint ptOrg;
   //   GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
   //   pClip->OffsetRgn(ptOrg);	// temporary fix, should not be here
   //   m_pDC->SelectClipRgn(pClip,RGN_DIFF);
   //}


   int nSavedAccurateWidth = m_PenModel.m_nAccurateWidth;
   int nSavedSquareEnds = m_PenModel.m_nSquareEnds;
   m_PenModel.makePenStyleBlade();

   ResetJoint();
   if(ldp.pDimFmt->bUseWitness)
   {
      if(ldp.lptWit1Start != ldp.lptWit1End)
      {
         MoveTo(ldp.lptWit1Start);
         DrawLineCaps(ldp.lptWit1End);
         ResetJoint();
      }
      if(ldp.lptWit2Start != ldp.lptWit2End)
      {
         MoveTo(ldp.lptWit2Start);
         DrawLineCaps(ldp.lptWit2End);
         ResetJoint();
      }
   }

   MoveTo(ldp.lptDimStart);
   if(ldp.pDimFmt->bArrowsInside)
   {
      DrawLineCaps(ldp.lptDimEnd,cap1,cap2);
      ResetJoint();
   }
   else
   {
      LongPoint lpt;
      Coord outside=m_pMapper->Scalar2Long(ldp.pDimStd->OutsideLineLen);
      Angle angle=mtAngle(ldp.lptDimStart,ldp.lptDimEnd);
      if(cap1)
      {
         lpt=ldp.lptDimStart;
         lpt.x-=outside;
         lpt=mtRotPoint(lpt,angle,ldp.lptDimStart);
         DrawLineCaps(lpt,cap1);
         ResetJoint();
      }
      MoveTo(ldp.lptDimEnd);

      if (ldp.lptDimEnd == ldp.lptExtent && cap2)
      {
         lpt=ldp.lptDimEnd;
         lpt.x+=outside;
         lpt=mtRotPoint(lpt,angle,ldp.lptDimEnd);
         DrawLineCaps(lpt,cap2);
         ResetJoint();
      }
   }

   if(ldp.lptDimEnd != ldp.lptExtent)
   {
      DrawLineCaps(ldp.lptExtent,ldp.pDimFmt->bArrowsInside?0:cap2);
      if(ldp.lptExtent != ldp.lptLeader) DrawLineCaps(ldp.lptLeader);
   }

   m_PenModel.m_nAccurateWidth=nSavedAccurateWidth;
   m_PenModel.m_nSquareEnds=nSavedSquareEnds;

   //if(pClip) m_pDC->SelectClipRgn(&oldClip);
}

void CDraftContext::DrawAngDim(const AngDimPoints &adp,short cap1,short cap2, Gdiplus::GraphicsPath* pClip)
{
   // TODO GDI+
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
   //CGDIObject<CBrush> gdiBrush(m_pDC,&m_PenBrush);
   // END TODO GDI+
   //CRgn oldClip;
   ////	if(m_pDoc && m_pDoc->m_bHideFills) pClip=NULL;
   //if(pClip)
   //{
   //   SaveClip(oldClip);
   //   CPoint ptOrg;
   //   GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
   //   pClip->OffsetRgn(ptOrg);	// temporary fix, should not be here
   //   m_pDC->SelectClipRgn(pClip,RGN_DIFF);
   //}

   ResetJoint();
   if(adp.pDimFmt->bUseWitness)
   {
      if(adp.lptWit1Start != adp.lptWit1End)
      {
         MoveTo(adp.lptWit1Start);
         DrawLineCaps(adp.lptWit1End);
         ResetJoint();
         if (path() != nullptr)
         {

            path()->StartFigure();

         }
      }
      if(adp.lptWit2Start != adp.lptWit2End)
      {
         MoveTo(adp.lptWit2Start);
         DrawLineCaps(adp.lptWit2End);
         ResetJoint();
         if (path() != nullptr)
         {

            path()->StartFigure();

         }
      }
   }

   Coord radius=Pythag(adp.lptDimStart - adp.lptCenter);

   DrawArc(adp.lptCenter,LongPoint(radius,radius),adp.lptDimStart,adp.lptDimEnd,adp.nDirection,0,cap1,cap2,false,adp.pDimFmt->bArrowsInside?0:kDimEndmarkOut);
   if (path() != nullptr)
   {

      path()->StartFigure();

   }
   if(adp.lptDimEnd != adp.lptExtent)
   {
      //m_DrawPrim.m_bPrev1 = true;
      DrawArc(adp.lptCenter,LongPoint(radius,radius),adp.lptDimEnd,adp.lptExtent,adp.nDirection,0,0,0,false);
      //m_DrawPrim.m_bPrev1 = false;
      if (path() != nullptr)
      {

         path()->StartFigure();

      }
   }
   //if(pClip) m_pDC->SelectClipRgn(&oldClip);
}

void CDraftContext::DrawRadialDim(const RadialDimPoints &rdp,short cap1,short cap2, Gdiplus::GraphicsPath* pClip)
{
   // TODO GDI+
   //CGDIObject<CPen> gdiPen(m_pDC,&m_Pen);
   //CGDIObject<CBrush> gdiBrush(m_pDC,&m_PenBrush);
   // END TODO GDI+
   //CRgn oldClip;
   ////	if(m_pDoc && m_pDoc->m_bHideFills) pClip=NULL;
   //if(pClip)
   //{
   //   SaveClip(oldClip);
   //   CPoint ptOrg;
   //   GetViewportOrgEx(m_pDC->GetSafeHdc(),&ptOrg);
   //   pClip->OffsetRgn(ptOrg);	// temporary fix, should not be here
   //   m_pDC->SelectClipRgn(pClip,RGN_DIFF);
   //}

   MoveTo(rdp.lptDimStart);
   if(rdp.pDimFmt->bArrowsInside)
   {
      DrawLineCaps(rdp.lptDimEnd,cap1,cap2);
      ResetJoint();
   }
   else
   {
      LongPoint lpt;
      Coord outside=m_pMapper->Scalar2Long(rdp.pDimStd->OutsideLineLen);
      Angle angle=mtAngle(rdp.lptDimStart,rdp.lptDimEnd);
      if(cap1)
      {
         lpt=rdp.lptDimStart;
         lpt.x-=outside;
         lpt=mtRotPoint(lpt,angle,rdp.lptDimStart);
         DrawLineCaps(lpt,cap1);
         ResetJoint();
      }
      MoveTo(rdp.lptDimEnd);

      if(rdp.lptDimEnd == rdp.lptExtent && cap2)
      {
         lpt=rdp.lptDimEnd;
         lpt.x+=outside;
         lpt=mtRotPoint(lpt,angle,rdp.lptDimEnd);
         DrawLineCaps(lpt,cap2);
         ResetJoint();
      }
   }

   if(rdp.lptDimEnd != rdp.lptExtent)
   {
      DrawLineCaps(rdp.lptExtent,rdp.pDimFmt->bArrowsInside?0:cap2);
      if(rdp.lptExtent != rdp.lptLeader) DrawLineCaps(rdp.lptLeader);
   }
//   if(pClip) m_pDC->SelectClipRgn(&oldClip);
}

bool CDraftContext::RectVisible(const LongRect& lrc)
{
   CRect rc=m_pMapper->LPtoDP(lrc);
   //rc.InflateRect(-1,-1,1,1);
   rc.InflateRect(0,0,1,1);
   return m_pDC->RectVisible(&rc)?true:false;
}

bool CDraftContext::RectActiveClient(const LongRect& lrc)
{
   CRect rc = m_pMapper->LPtoDP(lrc);
   //rc.InflateRect(-1,-1,1,1);
   rc.InflateRect(0, 0, 1, 1);
   CRect r;
   return r.IntersectRect(m_rcClientActive, rc) ? true : false;
}

HRGN BitmapToRegion(HBITMAP hBmp,COLORREF cTransparentColor)
{
   HRGN hRgn = NULL;

   if(hBmp)
   {
      // Create a memory DC inside which we will scan the bitmap content
      HDC hMemDC = CreateCompatibleDC(NULL);
      if(hMemDC)
      {
         // Get bitmap size
         BITMAP bm;
         GetObject(hBmp,sizeof(bm),&bm);

         // Create a 32 bits depth bitmap and select it into the memory DC 
         BITMAPINFOHEADER RGB32BITSBITMAPINFO ={
            sizeof(BITMAPINFOHEADER),	// biSize 
            bm.bmWidth,					// biWidth; 
            bm.bmHeight,				// biHeight; 
            1,							// biPlanes; 
            32,							// biBitCount 
            BI_RGB,						// biCompression; 
            0,							// biSizeImage; 
            0,							// biXPelsPerMeter; 
            0,							// biYPelsPerMeter; 
            0,							// biClrUsed; 
            0							// biClrImportant; 
         };
         VOID * pbits32;
         HBITMAP hbm32 = CreateDIBSection(hMemDC,(BITMAPINFO *)&RGB32BITSBITMAPINFO,DIB_RGB_COLORS,&pbits32,NULL,0);
         if(hbm32)
         {
            HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC,hbm32);

            // Create a DC just to copy the bitmap into the memory DC
            HDC hDC = CreateCompatibleDC(hMemDC);
            if(hDC)
            {
               // Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
               BITMAP bm32;
               GetObject(hbm32,sizeof(bm32),&bm32);
               while(bm32.bmWidthBytes % 4)
                  bm32.bmWidthBytes++;

               // Copy the bitmap into the memory DC
               HBITMAP holdBmp = (HBITMAP)SelectObject(hDC,hBmp);
               BitBlt(hMemDC,0,0,bm.bmWidth,bm.bmHeight,hDC,0,0,SRCCOPY);

               // For better performances, we will use the ExtCreateRegion() function to create the
               // region. This function take a RGNDATA structure on entry. We will add rectangles by
               // amount of ALLOC_UNIT number in this structure.
#define ALLOC_UNIT	100
               DWORD maxRects = ALLOC_UNIT;
               HANDLE hData = GlobalAlloc(GMEM_MOVEABLE,sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
               RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
               pData->rdh.dwSize = sizeof(RGNDATAHEADER);
               pData->rdh.iType = RDH_RECTANGLES;
               pData->rdh.nCount = pData->rdh.nRgnSize = 0;
               SetRect(&pData->rdh.rcBound,MAXLONG,MAXLONG,0,0);

               BYTE lr = GetRValue(cTransparentColor);
               BYTE lg = GetGValue(cTransparentColor);
               BYTE lb = GetBValue(cTransparentColor);
               // Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
               BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
               for(int y = 0; y < bm.bmHeight; y++)
               {
                  // Scan each bitmap pixel from left to right
                  for(int x = 0; x < bm.bmWidth; x++)
                  {
                     // Search for a continuous range of "non transparent pixels"
                     int x0 = x;
                     LONG *p = (LONG *)p32 + x;
                     while(x < bm.bmWidth)
                     {
                        if(*p == 0xFFFFFF) break;
                        p++;
                        x++;
                     }

                     if(x > x0)
                     {
                        // Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
                        if(pData->rdh.nCount >= maxRects)
                        {
                           GlobalUnlock(hData);
                           maxRects += ALLOC_UNIT;
                           hData = GlobalReAlloc(hData,sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects),GMEM_MOVEABLE);
                           pData = (RGNDATA *)GlobalLock(hData);
                        }
                        RECT *pr = (RECT *)&pData->Buffer;
                        SetRect(&pr[pData->rdh.nCount],x0,y,x,y + 1);
                        if(x0 < pData->rdh.rcBound.left)
                           pData->rdh.rcBound.left = x0;
                        if(y < pData->rdh.rcBound.top)
                           pData->rdh.rcBound.top = y;
                        if(x > pData->rdh.rcBound.right)
                           pData->rdh.rcBound.right = x;
                        if(y + 1 > pData->rdh.rcBound.bottom)
                           pData->rdh.rcBound.bottom = y + 1;
                        pData->rdh.nCount++;

                        // On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
                        // large (ie: > 4000). Therefore, we have to create the region by multiple steps.
                        if(pData->rdh.nCount == 2000)
                        {
                           HRGN h = ExtCreateRegion(NULL,sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects),pData);
                           if(hRgn)
                           {
                              CombineRgn(hRgn,hRgn,h,RGN_OR);
                              DeleteObject(h);
                           }
                           else
                              hRgn = h;
                           pData->rdh.nCount = 0;
                           SetRect(&pData->rdh.rcBound,MAXLONG,MAXLONG,0,0);
                        }
                     }
                  }

                  // Go to next row (remember, the bitmap is inverted vertically)
                  p32 -= bm32.bmWidthBytes;
               }

               // Create or extend the region with the remaining rectangles
               HRGN h = ExtCreateRegion(NULL,sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects),pData);
               if(hRgn)
               {
                  CombineRgn(hRgn,hRgn,h,RGN_OR);
                  DeleteObject(h);
               }
               else
                  hRgn = h;

               // Clean up
               GlobalFree(hData);
               SelectObject(hDC,holdBmp);
               VERIFY(DeleteDC(hDC));
            }

            DeleteObject(SelectObject(hMemDC,holdBmp));
         }

         VERIFY(DeleteDC(hMemDC));
      }
   }

   return hRgn;
}

void DrawTransparent(CDC * pDC,int x,int y,int nWidth,int nHeight,CBitmap *pBMP,int xSrc,int ySrc,int nSrcWidth,int nSrcHeight,COLORREF crColour)
{
   CGDIBitmapDC dcImage(pDC,pBMP);
   // Create the mask bitmap
   CBitmap bitmapTrans;
   bitmapTrans.CreateBitmap(nSrcWidth,nSrcHeight,1,1,NULL);
   CGDIBitmapDC dcTrans(pDC,&bitmapTrans);

   // Build mask based on transparent colour
   {
      CGDIBackColor BkClr(&dcImage,crColour);
      dcTrans.BitBlt(0,0,nSrcWidth,nSrcHeight,&dcImage,xSrc,ySrc,SRCCOPY);
   }

   CBitmap bmpImgTrans;

   OSVERSIONINFO osvi;
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&osvi);
   if(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
      bmpImgTrans.CreateCompatibleBitmap(&dcImage,nSrcWidth,nSrcHeight);
   else
      bmpImgTrans.CreateCompatibleBitmap(/*&dcImage*/pDC,nSrcWidth,nSrcHeight);

   CGDIBitmapDC dcImgTrans(pDC,&bmpImgTrans);

   {
      dcImgTrans.BitBlt(0,0,nSrcWidth,nSrcHeight,&dcImage,xSrc,ySrc,SRCCOPY);

      CGDIBackColor BkClr(&dcImgTrans,RGB(0,0,0));
      CGDITextColor TxtClr(&dcImgTrans,RGB(255,255,255));
      dcImgTrans.BitBlt(0,0,nSrcWidth,nSrcHeight,&dcTrans,0,0,SRCAND);
   }

   {
      CGDIBackColor BkClr(pDC,RGB(255,255,255));
      CGDITextColor TxtClr(pDC,RGB(0,0,0));
      pDC->StretchBlt(x,y,nWidth,nHeight,&dcTrans,0,0,nSrcWidth,nSrcHeight,SRCAND);

   }

   pDC->StretchBlt(x,y,nWidth,nHeight,&dcImgTrans,0,0,nSrcWidth,nSrcHeight,SRCINVERT);
}

bool CDraftContext::canUsePath()
{
   return (m_PenModel.m_nPenType == kPenColor) && !(m_bCreatingPath || m_bOutline) && !(shouldUseDash());
}

bool CDraftContext::shouldUseDash()
{
   return m_PenModel.Dashing() && !((CPCDraftApp*)AfxGetApp())->m_bHideLineStyles;
}

void CDraftContext::SaveClip(CRgn &oldClip)
{
   if(oldClip.GetSafeHandle()) oldClip.DeleteObject();
   CRect rcClip;
   m_pDC->GetClipBox(&rcClip);
   HRGN hRgn=::CreateRectRgnIndirect(&rcClip);
   ::GetClipRgn(m_pDC->GetSafeHdc(),hRgn);
   oldClip.Attach(hRgn);
}
