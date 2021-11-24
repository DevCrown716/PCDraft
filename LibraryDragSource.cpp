// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"  
#include "resource.h"  
#include <afxole.h>
#include "LibraryDragSource.h"
#include "LibraryObject.h"
#include "LibraryPhoto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Gdiplus::Bitmap * LoadPNG(LPCTSTR pName,LPCTSTR pType,HMODULE hInst);

CLibraryDragSource::CLibraryDragSource()
{
   
   m_bDrag     = false;
   m_pitem     = NULL;
   m_hcursor   = NULL;

}


CLibraryDragSource::~CLibraryDragSource()
{
   if(m_hcursor != NULL)
   {

      ::DeleteObject(m_hcursor);

   }

}

SCODE CLibraryDragSource::GiveFeedback(DROPEFFECT dropEffect)
{
   if(dropEffect == DROPEFFECT_COPY)
   {
      SetCursor(m_hcursor);
      return S_OK;
   }
   else if(dropEffect == DROPEFFECT_NONE)
   {
      SetCursor(LoadCursor(NULL, IDC_NO ));
      return S_OK;
   }
   else
   {
      return DRAGDROP_S_USEDEFAULTCURSORS;
   }

}

HBITMAP CreateDIB(int x,int y,COLORREF * pdata,int nStride,COLORREF ** p, bool bSwap);
HBITMAP CreateDIB(Gdiplus::Bitmap * pbitmap, bool bSwap)
{
   
   int x = pbitmap->GetWidth();

   int y= pbitmap->GetHeight();

   Gdiplus::Rect r;

   r.X        = 0;

   r.Y        = 0;

   r.Width    = x;

   r.Height   = y;

   Gdiplus::BitmapData data;

   pbitmap->LockBits(&r,0,PixelFormat32bppARGB,&data);

   HBITMAP hDib = CreateDIB(r.Width,r.Height,(COLORREF *)data.Scan0,data.Stride,NULL, bSwap);

   pbitmap->UnlockBits(&data);

   return hDib;

}


HBITMAP CreateDIB(int x,int y,COLORREF * pdata,int nStride,COLORREF ** p)
{
   
   return CreateDIB(x,y,pdata,nStride,p,false);

}


HBITMAP CreateDIB(int x,int y,COLORREF * pdata,int nStride,COLORREF ** p, bool bSwap)
{

   COLORREF * pcolorref = NULL;

   BITMAPINFO info;

   memset(&info,0,sizeof(info));

   info.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
   info.bmiHeader.biWidth         = x;
   info.bmiHeader.biHeight        = -y;
   info.bmiHeader.biPlanes        = 1;
   info.bmiHeader.biBitCount      = 32;
   info.bmiHeader.biCompression   = BI_RGB;
   info.bmiHeader.biSizeImage     = x * y * 4;

   HBITMAP hDib = ::CreateDIBSection(NULL,&info,DIB_RGB_COLORS,(void **)&pcolorref,NULL,0);

   if(hDib == NULL)
   {

      return NULL;

   }

   if(pdata != NULL)
   {
      for(int i = 0;i < y; i++)
      {
         if(bSwap)
         {
            memcpy(&pcolorref[(y - i -1) * x],&((byte *)pdata)[nStride * i],x * sizeof(COLORREF));
         }
         else
         {
            memcpy(&pcolorref[i * x],&((byte *)pdata)[nStride * i],x * sizeof(COLORREF));
         }
      }
   }
   else
   {
      memset(pcolorref,0,info.bmiHeader.biSizeImage);
   }
   if(p != NULL)
   {
      *p = pcolorref;
   }
   return hDib;
}
HICON CreateAlphaIcon(HBITMAP hBitmap, int x, int y)
{

   BITMAP b;
   ::GetObject(hBitmap,sizeof(b),&b);
   DWORD dwWidth = b.bmWidth;  // width of cursor
   DWORD dwHeight = b.bmHeight;  // height of cursor

   //hMemDC = CreateCompatibleDC(NULL);

   //// Draw something on the DIB section.
   //hOldBitmap = (HBITMAP)SelectObject(hMemDC,hBitmap);
   //PatBlt(hMemDC,0,0,dwWidth,dwHeight,WHITENESS);
   //SetTextColor(hMemDC,RGB(0,0,0));
   //SetBkMode(hMemDC,TRANSPARENT);
   //TextOutA(hMemDC,0,9,"rgba",4);
   //DWORD *lpdwPixel;
   //lpdwPixel = (DWORD *)lpBits;
   //for(x = 0; x<dwWidth; x++)
   //   for(y = 0; y<dwHeight; y++)
   //   {
   //      // Clear the alpha bits
   //      *lpdwPixel &= 0x00FFFFFF;
   //      // Set the alpha bits to 0x9F (semi-transparent)
   //      if((*lpdwPixel & 0x00FFFFFF) == 0)
   //         *lpdwPixel |= 0xFF000000;
   //      lpdwPixel++;
   //   }

   //SelectObject(hMemDC,hOldBitmap);
   //DeleteDC(hMemDC);

   // Create an empty mask bitmap.
   HBITMAP hMonoBitmap = CreateBitmap(dwWidth,dwHeight,1,1,NULL);

   // Set the alpha values for each pixel in the cursor so that
   // the complete cursor is semi-transparent.
   ICONINFO ii;
   ii.fIcon = FALSE;  // Change fIcon to TRUE to create an alpha icon
   ii.xHotspot = x;
   ii.yHotspot = y;
   ii.hbmMask = hMonoBitmap;
   ii.hbmColor = hBitmap;

   // Create the alpha cursor with the alpha DIB section.
   HICON hAlphaIcon = CreateIconIndirect(&ii);

   DeleteObject(hBitmap);
   DeleteObject(hMonoBitmap);

   return hAlphaIcon;
}

DROPEFFECT CLibraryDragSource::StartDragging(CWnd * pwnd,CLibraryItem * pitem,CPoint* MousePos)
{

   m_pitem = pitem;

   CRect rDrag(pitem->m_pphoto == NULL ? m_pitem->m_rectDraw : pitem->m_pphoto->m_rectDraw);

   BITMAP b;

   double dLess = 7.0 / 8.0;

   double dRate = min(144.0 / rDrag.Width(),144.0 / rDrag.Height());

   if(dRate > 1.0)
   {

      dRate = 1.0;

   }

   Gdiplus::Bitmap * p2;

   if(pitem->m_pphoto != NULL)
   {

      p2 = m_pitem->m_pphoto->m_pbitmapThumbnail;

   }
   else
   {

      p2 = m_pitem->m_pbitmapThumbnail;

   }

   Gdiplus::Bitmap & b2 = *p2;

   double dRateLess = dRate * dLess;

   Gdiplus::Bitmap b3(max(16,(INT)(rDrag.Width()* dRate)),max(16,(INT)(rDrag.Height() * dRate)));

   {

      Gdiplus::Graphics g(&b3);

      Gdiplus::RectF r;

      g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
      g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

      r.Width  = (INT)(rDrag.Width() * dRateLess);

      r.Height = (INT)(rDrag.Height() * dRateLess);

      r.X      = (INT)(b3.GetWidth() - r.Width) / 2;

      r.Y      = (INT)(b3.GetHeight() - r.Height) / 2;

      float fAlpha = 0.67;

      Gdiplus::ColorMatrix clrMatrix ={
         1.0f,0.0f,0.0f,0.0f,0.0f,
         0.0f,1.0f,0.0f,0.0f,0.0f,
         0.0f,0.0f,1.0f,0.0f,0.0f,
         0.0f,0.0f,0.0f,fAlpha,0.0f,
         0.0f,0.0f,0.0f,0.0f,1.0f
      };

      Gdiplus::ImageAttributes imgAttr;
      imgAttr.SetColorMatrix(&clrMatrix);

      g.DrawImage(&b2,r,0,0,b2.GetWidth(),b2.GetHeight(),Gdiplus::UnitPixel,&imgAttr);

      r.X      = 0;

      r.Y      = b3.GetHeight() - 16;

      r.Width  = 16;

      r.Height = 16;

      {

         Gdiplus::Bitmap * pbitmapShortcut = LoadPNG(MAKEINTRESOURCE(IDB_SHORTCUT),"PNG",NULL);

         g.DrawImage(pbitmapShortcut,r,0,0,pbitmapShortcut->GetWidth(),pbitmapShortcut->GetHeight(),Gdiplus::UnitPixel);

         delete pbitmapShortcut;

      }

      g.Flush();

   }

   CRect rClient;

   rClient.left = MousePos->x - b3.GetWidth() / 2;
   rClient.right = MousePos->x + b3.GetWidth() / 2;
   rClient.top = MousePos->y - b3.GetHeight() / 2;
   rClient.bottom = MousePos->y + b3.GetHeight() / 2;

   {

      Gdiplus::Rect r3;

      r3.X        = 0;

      r3.Y        = 0;

      r3.Width    = b3.GetWidth();

      r3.Height   = b3.GetHeight();

      Gdiplus::BitmapData data;

      b3.LockBits(&r3,0,PixelFormat32bppARGB,&data);

      HBITMAP h = CreateDIB(r3.Width,r3.Height,(COLORREF *)data.Scan0,data.Stride,NULL);

      if(m_hcursor != NULL)
      {

         ::DeleteObject(m_hcursor);

      }

      m_hcursor = (HCURSOR)CreateAlphaIcon(h,(MousePos->x - rClient.left) * dRate,(MousePos->y - rClient.top) * dRate);

      ::DeleteObject(h);

      b3.UnlockBits(&data);

   }


   //Use the following if you are only using text
   //HGLOBAL hgData=GlobalAlloc(GPTR,10 );   
   HGLOBAL hgData=GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,64);
   ASSERT(hgData != NULL);


   LPCSTR  lpsz=(LPCSTR)GlobalLock(hgData);
   ASSERT(lpsz != NULL);
   _ui64toa((UINT_PTR)m_pitem,(char*)lpsz,10);

   //Use the following if you are only using text
   //CacheGlobalData(CF_TEXT, hgData);   
   COleDataSource    datasource;

   datasource.CacheGlobalData(CF_OWNERDISPLAY,hgData);
   m_bDrag = true;


   DROPEFFECT dropEffect=datasource.DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_MOVE,(LPCRECT)rClient,this);

   datasource.Empty();

   if((dropEffect&DROPEFFECT_COPY) == 0 && m_pitem->m_pphoto == NULL)
   {

      delete m_pitem;

   }

   m_pitem = NULL;

   LPARAM lparam;

   lparam=MousePos->y;
   lparam=lparam << 16;
   lparam &= MousePos->x;


   SendMessage(GetActiveWindow(),WM_LBUTTONUP,0,lparam);

   datasource.ExternalDisconnect();

   return dropEffect;

}

void CLibraryDragSource::CompleteMove()
{


}

void CLibraryDragSource::OnMouseMove(POINT pt)
{
   if(m_bDrag && m_pitem != NULL)
   {
   }
}