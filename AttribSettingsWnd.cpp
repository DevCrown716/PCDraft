// AttribSettingsWnd.cpp : implementation file
//

#include "stdafx.h"
#include "AttribSettingsWnd.h"
#include "resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttribSettingsWnd

CAttribSettingsWnd::CAttribSettingsWnd()
{
   m_pedit = NULL;
   m_bOpacity = false;
   m_rect.SetRect(-1,-1,-1,-1);
   m_bSimple = false;
}

CAttribSettingsWnd::CAttribSettingsWnd(UINT ResNullBmpID, UINT ResMainBmpID, UINT ResMask1BmpID, UINT ResMask2BmpID, COLORREF crBack, bool bOpacity)
{
   m_bSimple = false;
   m_rect.SetRect(-1,-1,-1,-1);
   m_pedit = NULL;
   m_bOpacity = bOpacity;
   m_crBack = crBack;
	HANDLE hMainBmp, hNullBmp, hMask1Bmp, hMask2Bmp;
	HINSTANCE hApp = ::AfxGetInstanceHandle();
	hMainBmp = ::LoadImage( hApp, MAKEINTRESOURCE(ResMainBmpID), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
	hNullBmp = ::LoadImage( hApp, MAKEINTRESOURCE(ResNullBmpID), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
	
	hMask1Bmp = ::LoadImage(hApp, MAKEINTRESOURCE(ResMask1BmpID), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	hMask2Bmp = ::LoadImage(hApp, MAKEINTRESOURCE(ResMask2BmpID), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	
	MainBmp.Attach(hMainBmp);
	NullBmp.Attach(hNullBmp);
	Mask1.Attach(hMask1Bmp);
	Mask2.Attach(hMask2Bmp);
	m_nDrawRegim = 0;
	BITMAP infoBmp;
	NullBmp.GetBitmap(&infoBmp);
	m_nWidth = infoBmp.bmWidth;
	m_nHeight = infoBmp.bmHeight;
	m_nNullBmpID = ResNullBmpID;
	m_nMainBmpID = ResMainBmpID;
}

CAttribSettingsWnd::~CAttribSettingsWnd()
{
}

UINT UM_ATTRIBSETTINGS_UPDATE = RegisterWindowMessage("UM_ATTRIBSETTINGS_UPDATE");
UINT UM_ATTRIBSETTINGS_INFO = RegisterWindowMessage("UM_ATTRIBSETTINGS_INFO");

BEGIN_MESSAGE_MAP(CAttribSettingsWnd, CWnd)
	//{{AFX_MSG_MAP(CAttribSettingsWnd)
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(UM_ATTRIBSETTINGS_UPDATE, OnRecieveBmp)
   ON_REGISTERED_MESSAGE(UM_ATTRIBSETTINGS_INFO,OnAttribSettingsInfo)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAttribSettingsWnd message handlers

void CAttribSettingsWnd::OnPaint() 
{
	CPaintDC theDC(this); // device context for painting
    CDC dcMemoryMask, dcMemoryImage, dcMemoryCombine;
    CDC dcMemoryNull; // Infinisys Ltd.
    dcMemoryMask.CreateCompatibleDC(&theDC);
	dcMemoryImage.CreateCompatibleDC(&theDC);
	dcMemoryCombine.CreateCompatibleDC(&theDC);
   dcMemoryNull.CreateCompatibleDC(&theDC);// Infinisys Ltd.
   if(GetDlgCtrlID() == ID_TOOL_ATTRIB_PFILL_MENU)
   {
      TRACE("AMENU");
   }
	CDC dc;
	dc.CreateCompatibleDC(&theDC);
	CBitmap memoryBmp;
	memoryBmp.CreateCompatibleBitmap(&theDC, m_nWidth, m_nHeight);
	CBitmap* pOldMemoryBmp=dc.SelectObject(&memoryBmp);
   if(m_crBack == 0) // Interface Update - Infinisys Ltd.
   {
      dc.FillSolidRect(0,0,m_nWidth,m_nHeight,::GetSysColor(COLOR_BTNFACE));
   }
   else
   {
      dc.FillSolidRect(0,0,m_nWidth,m_nHeight,m_crBack); // Interface Update - Infinisys Ltd.
   }

	CBitmap *pOldMemoryImage;
   if(m_bSimple)
   {
		pOldMemoryImage = dcMemoryImage.SelectObject(&MainBmp);
      dc.BitBlt(0,0,m_nWidth,m_nHeight,&dcMemoryImage,0,0,SRCCOPY);
      if(RecievedBmp.GetSafeHandle() != NULL)
      {
         dcMemoryImage.SelectObject(&RecievedBmp);
         dc.BitBlt(m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height(),
            &dcMemoryImage,0,0,SRCCOPY);
      }
   }
	else if (m_nDrawRegim)
	{
		pOldMemoryImage = dcMemoryImage.SelectObject(&MainBmp);
		CBitmap *pOldMemoryMask = dcMemoryMask.SelectObject((m_nDrawRegim==1)?&Mask1:&Mask2);
		CBitmap CombineBmp;
		CombineBmp.CreateCompatibleBitmap(&dcMemoryImage, m_nWidth, m_nHeight);
		CBitmap* pOldBmp=dcMemoryCombine.SelectObject(&CombineBmp);
		dcMemoryCombine.BitBlt( 0, 0, m_nWidth, m_nHeight, &dcMemoryImage, 0, 0, SRCCOPY);
		dcMemoryCombine.BitBlt( 0, 0, m_nWidth, m_nHeight, &dcMemoryMask, 0, 0, SRCAND );
		dc.BitBlt( 0, 0, m_nWidth, m_nHeight, &dcMemoryCombine, 0, 0, SRCCOPY);
		
		dcMemoryImage.SelectObject(&RecievedBmp);
		dcMemoryCombine.BitBlt( 0, 0, m_nWidth, m_nHeight, &dcMemoryImage, 0, 0, SRCCOPY);
		dcMemoryMask.BitBlt( 0, 0 , m_nWidth, m_nHeight, NULL, 0, 0, DSTINVERT);
		dcMemoryCombine.BitBlt( 0, 0, m_nWidth, m_nHeight, &dcMemoryMask, 0, 0, SRCAND);
		dcMemoryMask.BitBlt( 0, 0 , m_nWidth, m_nHeight, NULL, 0, 0, DSTINVERT);
		dc.BitBlt( 0, 0, m_nWidth, m_nHeight, &dcMemoryCombine, 0, 0, SRCPAINT);

		dcMemoryMask.SelectObject(pOldMemoryMask);
		dcMemoryCombine.SelectObject(pOldBmp);
	}else
	{
		pOldMemoryImage = dcMemoryImage.SelectObject(&NullBmp);
		dc.BitBlt( 0, 0, m_nWidth, m_nHeight, &dcMemoryImage, 0, 0, SRCCOPY);
	}
   dcMemoryImage.SelectObject(pOldMemoryImage);
   //if(!m_bSimple)
   {
      if(m_crBack != 0) // Interface Update - Infinisys Ltd.
      {
         CBitmap *pOldMemoryNull = dcMemoryNull.SelectObject(&NullBmp);
         BITMAP bm;
         dc.GetCurrentBitmap()->GetBitmap(&bm);
         for(int x = 0; x < bm.bmWidth; x++)
         {
            for(int y = 0; y < bm.bmHeight; y++)
            {
               COLORREF cr = dcMemoryNull.GetPixel(x,y);
               if(cr == RGB(0xf0,0xf0,0xf0))
               {
                  dc.SetPixel(x,y,m_crBack);
               }
            }
         }
         dcMemoryNull.SelectObject(pOldMemoryNull);
      }
   }
	theDC.BitBlt( 0, 0, m_nWidth, m_nHeight, &dc, 0, 0, SRCCOPY);
	dc.SelectObject(pOldMemoryBmp);
}

LPARAM CAttribSettingsWnd::OnRecieveBmp(WPARAM wParam, LPARAM lParam)
{
   if(m_bOpacity)
   {
      if(GetFocus() == m_pedit)
      {

         return 0;

      }

      int iOpacity = (int)wParam;

      CString strOld;

      CString strNew;

      if(m_pedit != NULL)
      {

         m_pedit->GetWindowText(strOld);

      }
      else
      {

         strOld = m_str;

      }

      if(iOpacity < 0)
      {

         strNew.Empty();

      }
      else
      {

         int iPercentage = (int)round((double)iOpacity * 100.0 / 255.0);

         CString str;

         strNew.Format("%d",iPercentage);

      }

      if(strOld != strNew)
      {

         if(m_pedit != NULL)
         {

            m_pedit->SetWindowText(strNew);

         }
         else
         {

            m_str = strNew;

         }

      }


   }
   else
   {
      if(RecievedBmp.GetSafeHandle())
         VERIFY(RecievedBmp.DeleteObject());
      RecievedBmp.Attach((HANDLE)lParam);
  
      m_nDrawRegim = wParam;
      Invalidate();
   }
	return 1;
}



void CAttribSettingsWnd::OnSysColorChange() 
{
	CWnd::OnSysColorChange();

	HANDLE hMainBmp, hNullBmp;
	HINSTANCE hApp = ::AfxGetInstanceHandle();
	hMainBmp = ::LoadImage( hApp, MAKEINTRESOURCE(m_nMainBmpID), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
	hNullBmp = ::LoadImage( hApp, MAKEINTRESOURCE(m_nNullBmpID), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);

	if (MainBmp.GetSafeHandle()) MainBmp.DeleteObject();
	MainBmp.Attach(hMainBmp);
	if (NullBmp.GetSafeHandle()) NullBmp.DeleteObject();
	NullBmp.Attach(hNullBmp);
	Invalidate();
}


LPARAM CAttribSettingsWnd::OnAttribSettingsInfo(WPARAM wParam,LPARAM lParam)
{

   if(wParam == 0)
   {

      CRect * prect = (CRect *) lParam;

      if(m_rect.left < 0 
         && m_rect.top < 0
         && m_rect.right < 0
         && m_rect.bottom < 0)
      {

         GetClientRect(prect);

      }
      else
      {
       
         *prect = m_rect;

      }

      return 1;

   }
   else if(wParam == 1)
   {

      if(m_rect.left < 0
         && m_rect.top < 0
         && m_rect.right < 0
         && m_rect.bottom < 0)
      {
         GetClientRect(m_rect);
      }

      return 1;

   }

   return 0;
}