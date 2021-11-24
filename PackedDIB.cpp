// PackedDIB.cpp: implementation of the CPackedDIB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PackedDIB.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPackedDIB::CPackedDIB(HANDLE hDib,bool bFree)
{
	m_hDib=hDib;
	m_bFree=bFree;
}

CPackedDIB::~CPackedDIB()
{
	FreeDIB();
}

void CPackedDIB::FromClipboard(HANDLE hDib)
{
	DWORD dwSize=GlobalSize(hDib);
	m_hDib=GlobalAlloc(GHND,dwSize);
	memcpy(GlobalLock(m_hDib),GlobalLock(hDib),dwSize);
	GlobalUnlock(m_hDib);
	GlobalUnlock(hDib);
	m_bFree=true;
}

bool CPackedDIB::LoadResource(int nID)
{
	FreeDIB();
	HRSRC hResInfo=::FindResource(AfxGetResourceHandle(),MAKEINTRESOURCE(nID),RT_BITMAP);
	if(hResInfo)
	{
		DWORD dwSize=SizeofResource(AfxGetResourceHandle(),hResInfo);
		m_hDib=::GlobalAlloc(GHND,dwSize);
		if(m_hDib)
		{
			HGLOBAL hResData=::LoadResource(AfxGetResourceHandle(),hResInfo);
			memcpy(GlobalLock(m_hDib),LockResource(hResData),dwSize);
			GlobalUnlock(m_hDib);
			m_bFree=true;
		}
	}
	return m_hDib!=NULL;
}

void CPackedDIB::FreeDIB()
{
	if(m_bFree && m_hDib)
	{
		::GlobalFree(m_hDib);
		m_hDib=NULL;
	}
}


Gdiplus::Brush * CPackedDIB::proper_brush(LPCRECT lpcrect, int iOpacity)
{
   
   HBITMAP hbitmap = CreateDIBSection();

#undef new

   ::std::unique_ptr < Gdiplus::Bitmap > pbitmapPattern;
   
   pbitmapPattern.reset(new Gdiplus::Bitmap(hbitmap, nullptr));

   ::DeleteObject(hbitmap);

   if (iOpacity >= 255)
   {

      return new Gdiplus::TextureBrush(pbitmapPattern.get());

   }

   ::std::unique_ptr < Gdiplus::Bitmap > pbitmap;

   pbitmap.reset(new Gdiplus::Bitmap(pbitmapPattern->GetWidth(), pbitmapPattern->GetHeight()));

   float f = iOpacity / 255.f;

   Gdiplus::ColorMatrix colorMatrix = 
   { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.0f,    f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

   Gdiplus::ImageAttributes imageAtt;

   imageAtt.SetColorMatrix(&colorMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

   Gdiplus::Graphics g(pbitmap.get());

   g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);

   Gdiplus::SolidBrush br(Gdiplus::Color(0, 0, 0, 0));

   Gdiplus::RectF r(0, 0, pbitmap->GetWidth(), pbitmap->GetHeight());

   g.FillRectangle(&br, r);

   g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

   g.DrawImage(pbitmapPattern.get(), &r, nullptr, nullptr, &imageAtt, Gdiplus::UnitPixel);

   return new Gdiplus::TextureBrush(pbitmap.get());

}



HBITMAP CPackedDIB::CreateBitmap(int iOpacity)
{
	ASSERT(m_hDib);
	LPSTR lpDib=(LPSTR)GlobalLock(m_hDib);
	HDC hdc=::GetDC(NULL);
	HBITMAP hBmp=CreateDIBitmap(hdc,(LPBITMAPINFOHEADER)lpDib,CBM_INIT,FindDIBBits(lpDib),(LPBITMAPINFO)lpDib,DIB_RGB_COLORS);
	::ReleaseDC(NULL,hdc);
	::GlobalUnlock(m_hDib);
	return hBmp;
}

HBITMAP CPackedDIB::CreateDIBSection()
{
	ASSERT(m_hDib);
	HBITMAP hDib;
	LPBITMAPINFO lpBmi=LPBITMAPINFO(GlobalLock(m_hDib));
	ASSERT(lpBmi->bmiHeader.biSize==sizeof(BITMAPINFOHEADER));
	LPVOID lpBits;
	hDib=::CreateDIBSection(NULL,lpBmi,DIB_RGB_COLORS,&lpBits,NULL,0);
	if(lpBmi->bmiHeader.biSizeImage==0)
	{
		DIBSECTION ds;
		::GetObject(hDib,sizeof(DIBSECTION),&ds);
		lpBmi->bmiHeader.biSizeImage=ds.dsBmih.biSizeImage;
	}
	memcpy(lpBits,FindDIBBits(LPSTR(lpBmi)),lpBmi->bmiHeader.biSizeImage);
	::GlobalUnlock(m_hDib);
	return hDib;
}

void CPackedDIB::FromBitmap(HBITMAP hBmp)
{
	ASSERT(0);
}

void CPackedDIB::FromDIBSection(HBITMAP hDib)
{
	DIBSECTION ds;
	GetObject(hDib,sizeof(DIBSECTION),&ds);
	FreeDIB();
	WORD nPalSize=PaletteSize((LPSTR)&ds.dsBmih);
	DWORD dwSize=ds.dsBmih.biSize+nPalSize+ds.dsBmih.biSizeImage;
	m_hDib=GlobalAlloc(GHND,dwSize);
	if(m_hDib)
	{
		LPBITMAPINFOHEADER lpbi=(LPBITMAPINFOHEADER)GlobalLock(m_hDib);
		*lpbi=ds.dsBmih;
		if(nPalSize)
		{
			CWindowDC dcScreen(NULL);
			CDC memDC;
			memDC.CreateCompatibleDC(&dcScreen);
			HGDIOBJ hOldBmp=memDC.SelectObject(hDib);
			GetDIBColorTable(memDC,0,nPalSize,LPRGBQUAD(lpbi+1));
			memDC.SelectObject(hOldBmp);
		}
		memcpy(FindDIBBits(LPSTR(lpbi)),ds.dsBm.bmBits,ds.dsBmih.biSizeImage);
		m_bFree=true;
		::GlobalUnlock(m_hDib);
	}
}

LPSTR CPackedDIB::FindDIBBits(LPSTR lpDib)
{
	return (lpDib + *(LPDWORD)lpDib + PaletteSize(lpDib));
}

WORD CPackedDIB::PaletteSize(LPSTR lpDib)
{
	if(IS_WIN30_DIB(lpDib)) return (DIBNumColors(lpDib)*sizeof(RGBQUAD));
	else return (DIBNumColors(lpDib)*sizeof(RGBTRIPLE)); 

}

WORD CPackedDIB::DIBNumColors(LPSTR lpDib)
{
	WORD wBitCount;
	if(IS_WIN30_DIB(lpDib))
	{
		DWORD dwClrUsed=((LPBITMAPINFOHEADER)lpDib)->biClrUsed;
		if(dwClrUsed) return (WORD)dwClrUsed;
		wBitCount=((LPBITMAPINFOHEADER)lpDib)->biBitCount;
		if(wBitCount>8 && ((LPBITMAPINFOHEADER)lpDib)->biCompression==BI_BITFIELDS)
			return 3;
	}
	else wBitCount=((LPBITMAPCOREHEADER)lpDib)->bcBitCount;
	return (wBitCount>8)?0:(1<<wBitCount);
}

void CPackedDIB::GetBitmapInfo(LPBITMAPINFOHEADER lpBi)
{
	ASSERT(m_hDib);
	LPBITMAPINFOHEADER p=LPBITMAPINFOHEADER(GlobalLock(m_hDib));
	memcpy(lpBi,p,sizeof(BITMAPINFOHEADER));
	GlobalUnlock(m_hDib);
}

CArchive& operator<<(CArchive& ar, CPackedDIB& dib)
{
//	ASSERT(dib.m_hDib);
	DWORD dwSize=GlobalSize(dib.m_hDib);
	ar << dwSize;
	ar.Write(GlobalLock(dib.m_hDib),dwSize);
	GlobalUnlock(dib.m_hDib);
	return ar;
}

CArchive& operator>>(CArchive& ar, CPackedDIB& dib)
{
	dib.FreeDIB();
	DWORD dwSize;
	ar >> dwSize;
	dib.m_hDib=GlobalAlloc(GHND,dwSize);
	ar.Read(GlobalLock(dib.m_hDib),dwSize);
	GlobalUnlock(dib.m_hDib);
	return ar;
}

void CPackedDIB::SetDIB(HANDLE hDib)
{
	FreeDIB();
	m_hDib = hDib;
	m_bFree = TRUE;
}
