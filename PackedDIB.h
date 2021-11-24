// PackedDIB.h: interface for the CPackedDIB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKEDDIB_H__516A1CE6_BB0F_11D3_9815_000000000000__INCLUDED_)
#define AFX_PACKEDDIB_H__516A1CE6_BB0F_11D3_9815_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	IS_WIN30_DIB(lpbi)	((*(LPDWORD)(lpbi))==sizeof(BITMAPINFOHEADER))

class CPackedDIB  
{
public:
	void SetDIB(HANDLE hDib);
	CPackedDIB(HANDLE hDib=NULL,bool bFree=true);
	virtual ~CPackedDIB();

	bool LoadResource(int nID);
	void FromClipboard(HANDLE hDib);
	void FromDIBSection(HBITMAP hDib);
	void FromBitmap(HBITMAP hBmp);
	HBITMAP CreateDIBSection();
   HBITMAP CreateBitmap(int iOpacity);// Opacity/Transparency - Infinisys Ltd
	void GetBitmapInfo(LPBITMAPINFOHEADER lpBi);
	HANDLE GetHandle() {return m_hDib;}
   Gdiplus::Brush * proper_brush(LPCRECT lpcrect, int iOpacity);

	friend CArchive& operator>>(CArchive& ar, CPackedDIB& dib);
	friend CArchive& operator<<(CArchive& ar, CPackedDIB& dib);

protected:
	HANDLE m_hDib;
	bool m_bFree;

	WORD PaletteSize(LPSTR lpDib);
	WORD DIBNumColors(LPSTR lpDib);
	LPSTR FindDIBBits(LPSTR lpDib);
	void FreeDIB();
};

#endif // !defined(AFX_PACKEDDIB_H__516A1CE6_BB0F_11D3_9815_000000000000__INCLUDED_)
