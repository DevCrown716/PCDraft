// CustomMenus.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "CustomMenus.h"

#include "GradientPack.h"
#include "PackedDIB.h"
#include "PackedItem.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void __copy(Gdiplus::RectF & r, LPCRECT lpcrect)
{

   r.X = lpcrect->left;
   r.Y = lpcrect->top;
   r.Width = lpcrect->right - lpcrect->left;
   r.Height = lpcrect->bottom - lpcrect->top;

}
CRect get_main_screen_rect()
{
   
   CWnd * pwnd = AfxGetApp()->m_pMainWnd;

   CRect rect;

   if(pwnd != NULL && pwnd->GetSafeHwnd())
   {

      HMONITOR hmon = MonitorFromWindow(pwnd->GetSafeHwnd(),MONITOR_DEFAULTTONEAREST);

      MONITORINFO mi;

      memset(&mi,0,sizeof(mi));

      mi.cbSize = sizeof(mi);

      GetMonitorInfo(hmon,&mi);

      rect = mi.rcWork;

   }
   else
   {

      HWND hwnd = GetDesktopWindow();

      GetWindowRect(hwnd,rect);

   }

   return rect;
     

}
/////////////////////////////////////////////////////////////////////////////
// CToolMenu implementation

CToolMenu::CToolMenu(CBitmap* pBitmap)
{
   m_bitmap=pBitmap;
   VERIFY(CreatePopupMenu());
   BITMAP bmp;
   m_bitmap->GetBitmap(&bmp);
   int count=bmp.bmWidth/20;
   for(int i=0; i<count; i++)
   {
      VERIFY(AppendMenu(MF_ENABLED | MF_OWNERDRAW | MF_MENUBREAK,ID_TOOL_MENU_BASE+i,LPCSTR(i)));
   }
//	VERIFY(AppendMenu(MF_OWNERDRAW | MF_GRAYED | MF_MENUBREAK,1,LPSTR(-1)));
}

void CToolMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
   int cxCheck=GetSystemMetrics(SM_CXMENUCHECK);
   lpMIS->itemWidth = (lpMIS->itemData==-1)?-cxCheck:26-cxCheck;
   lpMIS->itemHeight = 24;
}

void CToolMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);

   if (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))
   {
      CRect rc(lpDIS->rcItem);

      rc.InflateRect(1,0,0,0);

      int shift=3;
      COLORREF ltColor=GetSysColor(COLOR_3DHILIGHT),rbColor=GetSysColor(COLOR_3DSHADOW);

      if(lpDIS->itemState & ODS_SELECTED)
      {
         ltColor=GetSysColor(COLOR_3DSHADOW);
         rbColor=GetSysColor(COLOR_3DHILIGHT);
         shift++;
      }

      pDC->Draw3dRect(&rc,ltColor,rbColor);

      CGDIBitmapDC memDC(pDC,m_bitmap);
      pDC->BitBlt(rc.left+shift,rc.top+shift,20,18,&memDC,lpDIS->itemData*20,0,SRCCOPY);
   }
}

int CToolMenu::TrackPopupMenu(CPoint &pt, CWnd *pWnd)
{
   int cmd=CMenu::TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,pt.x,pt.y,pWnd);
   return (cmd-ID_TOOL_MENU_BASE);
}

/////////////////////////////////////////////////////////////////////////////
// CColorMenu implementation
CColorMenu::CColorMenu()
{
}

void CColorMenu::Create(CPalette* pPalette,UINT nTitle,int nBase)
{
   m_pPalette=pPalette;
   m_sTitle.LoadString(nTitle);
   m_nBase=nBase;

   int iEntryCount = pPalette->GetEntryCount(); // New Color/New Pattern - Infinisys Ltd.
   int iLineCount = (iEntryCount / 8) + 1; // New Color/New Pattern - Infinisys Ltd.
   //if(pPalette->GetEntryCount() % 8 == 0)
   //{
   //   iLineCount++;
   //}
   // Gradient/Pattern - Infinisys Ltd
   PALETTEENTRY palEntry;
   VERIFY(CreatePopupMenu());
   for(int x=0; x<8; x++)
   {
      VERIFY(AppendMenu(MF_OWNERDRAW | MF_GRAYED | MF_MENUBREAK, 1, LPSTR(x<7?0:-1)));	// for title
      for(int y=0; y<iLineCount; y++) // Interface Update - Infinisys Ltd.
      {
         int color=8*y+x;
         if(color) pPalette->GetPaletteEntries(color-1,1,&palEntry);
         VERIFY(AppendMenu(MF_ENABLED | MF_OWNERDRAW,m_nBase+color,LPSTR(RGB(palEntry.peRed,palEntry.peGreen,palEntry.peBlue))));
      }
   }


//	VERIFY(AppendMenu(MF_OWNERDRAW | MF_GRAYED | MF_MENUBREAK ,-2,LPSTR(0)));

}

void CColorMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
   int cxCheck=GetSystemMetrics(SM_CXMENUCHECK);
   lpMIS->itemWidth = (lpMIS->itemData==-1?17:14)-cxCheck;
   lpMIS->itemHeight = (LOWORD(lpMIS->itemID)==1)?14:16;
}

void CColorMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);

   if(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))
   {
      CPalette* pOldPal=pDC->SelectPalette(m_pPalette,FALSE);
//		pDC->RealizePalette();

      CRect rc(lpDIS->rcItem);
      rc.right=rc.left+13;

      if(LOWORD(lpDIS->itemID)==1)
      {
         rc.InflateRect(2,0);
         CGDIStockObject font(pDC,DEFAULT_GUI_FONT);
         CGDITextAlign align(pDC,TA_CENTER|TA_TOP);
         pDC->ExtTextOut(18*4,0,ETO_OPAQUE/*|ETO_CLIPPED*/,&rc,m_sTitle,NULL);
      }
      else
      {
         rc.InflateRect(1,-1);
         CGDIStockObject pen(pDC,(lpDIS->itemState & (ODS_SELECTED|ODS_CHECKED))?WHITE_PEN:BLACK_PEN);
         CGDIStockObject brush(pDC,HOLLOW_BRUSH);
         pDC->Rectangle(&rc);

         rc.InflateRect(-1,-1);
         if(LOWORD(lpDIS->itemID)>m_nBase)
         {
            CGDIBackColor bk(pDC,PALETTEINDEX(lpDIS->itemID-m_nBase-1)); //lpDIS->itemData);
            pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
         }
         else
         {
            CGDIStockObject font(pDC,DEFAULT_GUI_FONT);
            CGDITextAlign align(pDC,TA_CENTER|TA_TOP);
            pDC->ExtTextOut((rc.left+rc.right)/2,rc.top,ETO_OPAQUE|ETO_CLIPPED,&rc,"N",1,NULL);
         }
         if(lpDIS->itemState & ODS_CHECKED)
         {
            rc.InflateRect(2,2);
            pDC->Rectangle(&rc);
         }
      }
      pDC->SelectPalette(pOldPal,FALSE);
   }
}

int CColorMenu::Track(CWnd* pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   pt.Offset(18,9); // Interface Update - Infinisys Ltd.
   int cmd=CMenu::TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,pt.x,pt.y,pWnd);
   cmd-=(cmd?m_nBase:1);
   return cmd;
}

void CColorMenu::CmdTrack(CWnd *pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   pt.Offset(18,9); // Interface Update - Infinisys Ltd.
   CMenu::TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,pt.x,pt.y,pWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CPatternOnlyMenu implementation
// Pattern/Gradient - Infinisys Ltd
//

CPatternOnlyMenu::CPatternOnlyMenu()
{
}

void CPatternOnlyMenu::Create(CPackedDIB * pitems,int nCount,UINT nTitle,int nBase)
{

   m_nBase = nBase;

   VERIFY(CreatePopupMenu());

   m_nCount = nCount + 1;

   CRect rect = get_main_screen_rect();

   int iH = max(400,rect.Height() - 300);

   int m_iColumnCount = max(8,nCount / max(1,(iH / 22)));

   int iLineCount = m_nCount / 8;

   if(m_nCount % m_iColumnCount > 0)
   {

      iLineCount++;

   }

   for(int x=0; x<m_iColumnCount; x++)
   {
      VERIFY(AppendMenu(MF_OWNERDRAW | MF_GRAYED | MF_MENUBREAK,1,LPSTR(x<(m_iColumnCount - 1)?0:-1)));	// for title
      for(int y=0; y<iLineCount; y++)
      {
         int iItem=m_iColumnCount*y + x;
         if(iItem >= m_nCount)
            continue;
         VERIFY(AppendMenu(MF_ENABLED | MF_OWNERDRAW,m_nBase + iItem,LPSTR(iItem?(pitems + iItem - 1):0)));
      }
   }
   //	VERIFY(AppendMenu(MF_OWNERDRAW | MF_GRAYED | MF_MENUBREAK,-1,LPSTR(-1)));

   m_sTitle.LoadString(nTitle);
}

void CPatternOnlyMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
   int cxCheck=GetSystemMetrics(SM_CXMENUCHECK);
   lpMIS->itemWidth = (lpMIS->itemData == -1?20:17) - cxCheck;
   lpMIS->itemHeight = (LOWORD(lpMIS->itemID) == 1)?14:20;
}

void CPatternOnlyMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);

   if(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))
   {
      CRect rc(lpDIS->rcItem);
      rc.right=rc.left + 16;

      if(LOWORD(lpDIS->itemID) == 1)
      {
         rc.InflateRect(2,0);
         CGDIStockObject font(pDC,DEFAULT_GUI_FONT);
         CGDITextAlign align(pDC,TA_CENTER | TA_TOP);
         pDC->ExtTextOut(20 * 4,0,ETO_OPAQUE/*|ETO_CLIPPED*/,&rc,m_sTitle,NULL);
      }
      else
      {
         rc.InflateRect(1,-1);
         CGDIStockObject pen(pDC,(lpDIS->itemState & (ODS_SELECTED | ODS_CHECKED))?WHITE_PEN:BLACK_PEN);
         CGDIStockObject brush(pDC,HOLLOW_BRUSH);
         pDC->Rectangle(&rc);

         rc.InflateRect(-1,-1);

         if(LOWORD(lpDIS->itemID)>m_nBase)
         {

            CPackedDIB* pitem=(CPackedDIB*)lpDIS->itemData;

#undef new
            CPackedDIB * pDib = pitem;
            Gdiplus::Bitmap * pdib=new ::Gdiplus::Bitmap(pDib->CreateBitmap(255), nullptr);

            // TODO
            //BITMAP bm;
            //GetObject(hDib,sizeof(BITMAP),&bm);

            //CDC memDC;
            //memDC.CreateCompatibleDC(pDC);
            //HGDIOBJ hOldBmp=memDC.SelectObject(hDib);

            //for(int x=rc.left; x < rc.right; x+=bm.bmWidth)
            //{
            //   int cx=x + bm.bmWidth > rc.right?(rc.right - x):bm.bmWidth;
            //   for(int y=rc.top; y < rc.bottom; y+=bm.bmHeight)
            //   {
            //      int cy=y + bm.bmHeight > rc.bottom?(rc.bottom - y):bm.bmHeight;
            //      pDC->BitBlt(x,y,cx,cy,&memDC,0,0,SRCCOPY);
            //   }
            //}

            //memDC.SelectObject(hOldBmp);
            //::DeleteObject(hDib);

            //if(lpDIS->itemState & ODS_CHECKED)
            //{
            //   rc.InflateRect(2,2);
            //   pDC->Rectangle(&rc);
            //}

         }
         else
         {
            CGDIStockObject font(pDC,DEFAULT_GUI_FONT);
            CGDITextAlign align(pDC,TA_CENTER | TA_TOP);
            pDC->ExtTextOut((rc.left + rc.right) / 2,rc.top + 2,ETO_OPAQUE | ETO_CLIPPED,&rc,"N",1,NULL);
         }
      }
   }
}

int CPatternOnlyMenu::Track(CWnd* pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   pt.Offset(18,9); // Interface Update - Infinisys Ltd.
   int cmd=CMenu::TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,pt.x,pt.y,pWnd);
   cmd-=(cmd?m_nBase:1);
   return cmd;
}

void CPatternOnlyMenu::CmdTrack(CWnd *pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   pt.Offset(18,9); // Interface Update - Infinisys Ltd.
   CMenu::TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,pt.x,pt.y,pWnd); // Interface Update - Infinisys Ltd.
}



/////////////////////////////////////////////////////////////////////////////
// CPatternMenu implementation

CPatternMenu::CPatternMenu()
{
}

void CPatternMenu::Create(CPackedItem * pitems, int nCount,UINT nTitle,int nBase)
{
   
   m_nBase = nBase;

   VERIFY(CreatePopupMenu());

   m_nCount = nCount + 1;
   
   CRect rect = get_main_screen_rect();

   int iH = max(400,rect.Height() - 300);

   int m_iColumnCount = max(8, nCount / max(1,(iH / 22)));

   int iLineCount = m_nCount / 8;

   if(m_nCount % m_iColumnCount > 0)
   {

      iLineCount++;

   }

   for(int x=0; x<m_iColumnCount; x++)
   {
      VERIFY(AppendMenu(MF_OWNERDRAW | MF_GRAYED | MF_MENUBREAK,1,LPSTR(x<(m_iColumnCount-1)?0:-1)));	// for title
      for(int y=0; y<iLineCount; y++)
      {
         int iItem=m_iColumnCount*y + x;
         if(iItem >= m_nCount)
            continue;
         VERIFY(AppendMenu(MF_ENABLED | MF_OWNERDRAW,m_nBase + iItem,LPSTR(iItem?(pitems + iItem - 1):0)));
      }
   }
//	VERIFY(AppendMenu(MF_OWNERDRAW | MF_GRAYED | MF_MENUBREAK,-1,LPSTR(-1)));

   m_sTitle.LoadString(nTitle);
}

void CPatternMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
   int cxCheck=GetSystemMetrics(SM_CXMENUCHECK);
   lpMIS->itemWidth = (lpMIS->itemData==-1?20:17)-cxCheck;
   lpMIS->itemHeight = (LOWORD(lpMIS->itemID)==1)?14:20;
}

void CPatternMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);

   if(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))
   {
      CRect rc(lpDIS->rcItem);
      rc.right=rc.left+16;

      if(LOWORD(lpDIS->itemID)==1)
      {
         rc.InflateRect(2,0);
         CGDIStockObject font(pDC,DEFAULT_GUI_FONT);
         CGDITextAlign align(pDC,TA_CENTER|TA_TOP);
         pDC->ExtTextOut(20*4,0,ETO_OPAQUE/*|ETO_CLIPPED*/,&rc,m_sTitle,NULL);
      }
      else
      {
         rc.InflateRect(1,-1);
         CGDIStockObject pen(pDC,(lpDIS->itemState & (ODS_SELECTED|ODS_CHECKED))?WHITE_PEN:BLACK_PEN);
         CGDIStockObject brush(pDC,HOLLOW_BRUSH);
         pDC->Rectangle(&rc);

         rc.InflateRect(-1,-1);

         if(LOWORD(lpDIS->itemID)>m_nBase)
         {

            CPackedItem* pitem=(CPackedItem*)lpDIS->itemData;

            //if(pitem->m_etype == packed_type_gradient)
            {
               CGradientPack * pgradient = pitem->m_pgradient;
               Gdiplus::Graphics g(pDC->GetSafeHdc());
               Gdiplus::GraphicsPath path;
               Gdiplus::RectF r;
               __copy(r, rc);
               path.AddRectangle(r);
               ::std::unique_ptr < Gdiplus::Brush > br;
               br.reset(pitem->proper_brush(rc, 255));
               g.FillPath(br.get(), &path);
            }
            //else if(pitem->m_etype == packed_type_dib)
            //{

            //   CPackedDIB * pDib = pitem->m_pdib;
            //   //HBITMAP hDib=pDib->CreateBitmap(255);
            //   // TODO
            //   Gdiplus::Bitmap * pbitmap = new ::Gdiplus::Bitmap(pDib->CreateBitmap(255), nullptr);
            //   Gdiplus::TextureBrush b(pbitmap);
            //      //BITMAP bm;
            //      //GetObject(hDib,sizeof(BITMAP),&bm);

            //      Gdiplus::Graphics g(pDC->GetSafeHdc());
            //      Gdiplus::Rect r;
            //      r.X = rc.left;
            //      r.Y = rc.top;
            //      r.Width = rc.Width();
            //      r.Height= rc.Height();
            //      g.FillRectangle(&b, r);
            //      //CDC memDC;
            //      //memDC.CreateCompatibleDC(pDC);
            //      //HGDIOBJ hOldBmp=memDC.SelectObject(hDib);

            //      //for(int x=rc.left; x < rc.right; x+=bm.bmWidth)
            //      //{
            //      //   int cx=x + bm.bmWidth > rc.right?(rc.right - x):bm.bmWidth;
            //      //   for(int y=rc.top; y < rc.bottom; y+=bm.bmHeight)
            //      //   {
            //      //      int cy=y + bm.bmHeight > rc.bottom?(rc.bottom - y):bm.bmHeight;
            //      //      pDC->BitBlt(x,y,cx,cy,&memDC,0,0,SRCCOPY);
            //      //   }
            //      //}

            //      //memDC.SelectObject(hOldBmp);
            //      //::DeleteObject(hDib);

            //      if(lpDIS->itemState & ODS_CHECKED)
            //      {
            //         rc.InflateRect(2,2);
            //         pDC->Rectangle(&rc);
            //      }
            //      delete pbitmap;
            //}
         }
         else
         {
            CGDIStockObject font(pDC,DEFAULT_GUI_FONT);
            CGDITextAlign align(pDC,TA_CENTER|TA_TOP);
            pDC->ExtTextOut((rc.left+rc.right)/2,rc.top+2,ETO_OPAQUE|ETO_CLIPPED,&rc,"N",1,NULL);
         }
      }
   }
}

int CPatternMenu::Track(CWnd* pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   pt.Offset(18,9); // Interface Update - Infinisys Ltd.
   int cmd=CMenu::TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,pt.x,pt.y,pWnd);
   cmd-=(cmd?m_nBase:1);
   return cmd;
}

void CPatternMenu::CmdTrack(CWnd *pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   pt.Offset(18,9); // Interface Update - Infinisys Ltd.
   CMenu::TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,pt.x,pt.y,pWnd); // Interface Update - Infinisys Ltd.
}


/////////////////////////////////////////////////////////////////////////////
// CColorPatternGradientMenu implementation - Interface Update - Infinisys Ltd.
CColorPatternGradientMenu::CColorPatternGradientMenu()
{
}

void CColorPatternGradientMenu::Create(
   CPalette* pPalette,
   UINT nTitleColor,
   int nBaseColor,
   CPackedItem * pitems,
   int nCount,
   UINT nTitlePatternGradient,
   int nBasePatternGradient)
{
   m_pPalette=pPalette;
   m_sTitleColor.LoadString(nTitleColor);
   m_nBasePatternGradient = nBasePatternGradient;
   m_nBaseColor=nBaseColor;
   m_sTitlePatternGradient.LoadString(nTitlePatternGradient);

   m_nCountPatternGradient = nCount + 1;

   CRect rect = get_main_screen_rect();

   int iH = max(400,rect.Height() - 300);

   int iEntryCountColor = pPalette->GetEntryCount(); // New Color/New Pattern - Infinisys Ltd.
   //int m_iColumnCount = max(max(8,iEntryCountColor / max(1,(iH / 22))),
   int m_iColumnCount = max(max(8, iEntryCountColor / max(1, (iH / 22))),
      m_nCountPatternGradient / max(1,(iH / 22)));
   //int m_iColumnCount = 8;

   int iLineCountColor = (iEntryCountColor / m_iColumnCount) + 1; // New Color/New Pattern - Infinisys Ltd.
   int iLineCountPatternGradient = m_nCountPatternGradient / m_iColumnCount;
   if(m_nCountPatternGradient % m_iColumnCount > 0)
   {
      iLineCountPatternGradient++;
   }
   //if(pPalette->GetEntryCount() % 8 == 0)
   //{
   //   iLineCount++;
   //}
   // Gradient/Pattern - Infinisys Ltd
   PALETTEENTRY palEntry;
   VERIFY(CreatePopupMenu());
   for(int x=0; x<m_iColumnCount; x++)
   {
      VERIFY(AppendMenu(MF_OWNERDRAW | MF_GRAYED | MF_MENUBREAK,x == 0?1:5,LPSTR(x<m_iColumnCount - 1?0:-1)));	// for color title
      for(int y=0; y<iLineCountColor; y++)
      {
         int color=m_iColumnCount * y + x;
         if(color >= iEntryCountColor)
         {
            palEntry.peRed = 255;
            palEntry.peGreen = 255;
            palEntry.peBlue = 255;
         }
         else if(color) 
         {
            pPalette->GetPaletteEntries(color - 1,1,&palEntry);
         }
         VERIFY(AppendMenu(MF_ENABLED | MF_OWNERDRAW,m_nBaseColor + color,LPSTR(RGB(palEntry.peRed,palEntry.peGreen,palEntry.peBlue))));
      }
      VERIFY(AppendMenu(MF_OWNERDRAW | MF_GRAYED,x == 0?2:6,LPSTR(x<m_iColumnCount - 1?0:-1)));	// for pattern/gradient title
      for(int y=0; y<iLineCountPatternGradient; y++)
      {
         int iItem=m_iColumnCount * y + x;
         if(iItem >= m_nCountPatternGradient)
            continue;
         VERIFY(AppendMenu(MF_ENABLED | MF_OWNERDRAW,m_nBasePatternGradient + iItem,LPSTR(iItem?(pitems + iItem - 1):0)));
      }
   }



}

void CColorPatternGradientMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
 
   int cxCheck=GetSystemMetrics(SM_CXMENUCHECK);
   if(LOWORD(lpMIS->itemID) < 10)
   {
      lpMIS->itemHeight = 22;
   }
   else
   {
      lpMIS->itemHeight = 17;
   }
   lpMIS->itemWidth = (lpMIS->itemData == -1?20:17) - cxCheck;
}

void CColorPatternGradientMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);

   if(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))
   {
      CPalette* pOldPal=pDC->SelectPalette(m_pPalette,FALSE);
      //		pDC->RealizePalette();

      CRect rc(lpDIS->rcItem);
      rc.right=rc.left + 13;

      if(LOWORD(lpDIS->itemID) == 1)
      {
         rc.top += 6;
         rc.bottom += 6;
         rc.right = rc.left + (GetSystemMetrics(SM_CXMENUCHECK) + 4) * m_iColumnCount;
         //CGDIStockObject font(pDC,DEFAULT_GUI_FONT);
         //CGDITextAlign align(pDC,TA_CENTER | TA_TOP);
         pDC->DrawText(m_sTitleColor, rc, DT_CENTER | DT_VCENTER);
      }
      else if(LOWORD(lpDIS->itemID) == 2)
      {
         rc.top += 6;
         rc.bottom += 6;
         rc.right = rc.left + (GetSystemMetrics(SM_CXMENUCHECK) + 4) * m_iColumnCount;
         //CGDIStockObject font(pDC,DEFAULT_GUI_FONT);
         //CGDITextAlign align(pDC,TA_CENTER | TA_TOP);
         pDC->DrawText(m_sTitlePatternGradient,rc,DT_CENTER | DT_VCENTER);
      }
      else if(LOWORD(lpDIS->itemID)< 10)
      {
   
      }
      else
      {
         rc.InflateRect(1,-1);
         if((LOWORD(lpDIS->itemID) >= m_nBasePatternGradient
            && LOWORD(lpDIS->itemID) < m_nBasePatternGradient + m_nCountPatternGradient)
            ||
            (LOWORD(lpDIS->itemID)>=m_nBaseColor
            && LOWORD(lpDIS->itemID)<=m_nBaseColor + m_pPalette->GetEntryCount()))
         {
            CGDIStockObject pen(pDC,(lpDIS->itemState & (ODS_SELECTED | ODS_CHECKED))?WHITE_PEN:BLACK_PEN);
            CGDIStockObject brush(pDC,HOLLOW_BRUSH);
            pDC->Rectangle(&rc);

         }
         rc.InflateRect(-1,-1);
         bool bRect = true;

         if(LOWORD(lpDIS->itemID)>m_nBasePatternGradient
            && LOWORD(lpDIS->itemID)<m_nBasePatternGradient + m_nCountPatternGradient)
         {

            CPackedItem* pitem=(CPackedItem*)lpDIS->itemData;

            if(pitem->m_etype == packed_type_gradient)
            {
               CGradientPack * pgradient = pitem->m_pgradient;
               Gdiplus::Graphics g(pDC->GetSafeHdc());
               Gdiplus::GraphicsPath path;
               Gdiplus::RectF r;
               __copy(r, rc);
               path.AddRectangle(r);
               ::std::unique_ptr < Gdiplus::Brush > br;
               br.reset(pitem->proper_brush(rc, 255));
               g.FillPath(br.get(), &path);
            }
            else if(pitem->m_etype == packed_type_dib)
            {

               //CPackedDIB * pDib = pitem->m_pdib;
               ////HBITMAP hDib=pDib->CreateBitmap(255);
               //// TODO
               //auto pbitmap = pDib->CreateBitmap(255);

               ////BITMAP bm;
               ////GetObject(hDib,sizeof(BITMAP),&bm);

               ////CDC memDC;
               ////memDC.CreateCompatibleDC(pDC);
               ////HGDIOBJ hOldBmp=memDC.SelectObject(hDib);

               ////for(int x=rc.left; x < rc.right; x+=bm.bmWidth)
               ////{
               ////   int cx=x + bm.bmWidth > rc.right?(rc.right - x):bm.bmWidth;
               ////   for(int y=rc.top; y < rc.bottom; y+=bm.bmHeight)
               ////   {
               ////      int cy=y + bm.bmHeight > rc.bottom?(rc.bottom - y):bm.bmHeight;
               ////      pDC->BitBlt(x,y,cx,cy,&memDC,0,0,SRCCOPY);
               ////   }
               ////}

               ////memDC.SelectObject(hOldBmp);
               ////::DeleteObject(hDib);

               ////if(lpDIS->itemState & ODS_CHECKED)
               ////{
               ////   rc.InflateRect(2,2);
               ////   pDC->Rectangle(&rc);
               ////}
               CPackedDIB * pDib = pitem->m_pdib;
               //HBITMAP hDib=pDib->CreateBitmap(255);
               // TODO
               Gdiplus::Bitmap * pbitmap = new ::Gdiplus::Bitmap(pDib->CreateBitmap(255), nullptr);
               Gdiplus::TextureBrush b(pbitmap);
               //BITMAP bm;
               //GetObject(hDib,sizeof(BITMAP),&bm);

               Gdiplus::Graphics g(pDC->GetSafeHdc());
               Gdiplus::Rect r;
               r.X = rc.left;
               r.Y = rc.top;
               r.Width = rc.Width();
               r.Height = rc.Height();
               g.FillRectangle(&b, r);
               //CDC memDC;
               //memDC.CreateCompatibleDC(pDC);
               //HGDIOBJ hOldBmp=memDC.SelectObject(hDib);

               //for(int x=rc.left; x < rc.right; x+=bm.bmWidth)
               //{
               //   int cx=x + bm.bmWidth > rc.right?(rc.right - x):bm.bmWidth;
               //   for(int y=rc.top; y < rc.bottom; y+=bm.bmHeight)
               //   {
               //      int cy=y + bm.bmHeight > rc.bottom?(rc.bottom - y):bm.bmHeight;
               //      pDC->BitBlt(x,y,cx,cy,&memDC,0,0,SRCCOPY);
               //   }
               //}

               //memDC.SelectObject(hOldBmp);
               //::DeleteObject(hDib);

               if (lpDIS->itemState & ODS_CHECKED)
               {
                  rc.InflateRect(2, 2);
                  pDC->Rectangle(&rc);
               }
               delete pbitmap;

            }
         }
         else if(LOWORD(lpDIS->itemID)>m_nBaseColor && LOWORD(lpDIS->itemID)<=m_nBaseColor+m_pPalette->GetEntryCount())
         {
            CGDIBackColor bk(pDC,PALETTEINDEX(lpDIS->itemID - m_nBaseColor - 1)); //lpDIS->itemData);
            pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
         }
         else if(LOWORD(lpDIS->itemID)==m_nBaseColor 
            || LOWORD(lpDIS->itemID) == m_nBasePatternGradient)
         {
            CGDIStockObject font(pDC,DEFAULT_GUI_FONT);
            CGDITextAlign align(pDC,TA_CENTER | TA_TOP);
            pDC->ExtTextOut((rc.left + rc.right) / 2,rc.top,ETO_OPAQUE | ETO_CLIPPED,&rc,"N",1,NULL);
         }
         else
         {
            bRect = false;
         }
         if(bRect && lpDIS->itemState & ODS_CHECKED)
         {

            rc.InflateRect(2,2);
            pDC->Rectangle(&rc);
         }
      }
      pDC->SelectPalette(pOldPal,FALSE);
   }
}

int CColorPatternGradientMenu::Track(CWnd* pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   pt.Offset(18,9);
   int cmd=CMenu::TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,pt.x,pt.y,pWnd);
   //cmd-=(cmd?m_nBase:1);
   return cmd;
}

void CColorPatternGradientMenu::CmdTrack(CWnd *pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   pt.Offset(18,9);
   CMenu::TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,pt.x,pt.y,pWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CEndmarksMenu implementation

CEndmarksMenu::CEndmarksMenu()
{
   m_bitmap.LoadBitmap(IDB_ENDMARKS);
   VERIFY(CreatePopupMenu());
   for(int x=0; x<3; x++)
   {
      for(int y=0; y<16; y++)
      {
         int nFlags=MF_ENABLED | MF_OWNERDRAW;
         if(y==0) nFlags|=MF_MENUBREAK;
         VERIFY(AppendMenu(nFlags,ID_ENDMARKS_BASE+y*3+x,LPSTR(MAKELONG(x,y))));
      }
   }
   VERIFY(AppendMenu(MF_OWNERDRAW | MF_GRAYED | MF_MENUBREAK,0,LPSTR(-1)));
}

void CEndmarksMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
   int cxCheck=GetSystemMetrics(SM_CXMENUCHECK);
   lpMIS->itemWidth = (lpMIS->itemData==-1)?-cxCheck:14-cxCheck;
   lpMIS->itemHeight = 13;
}

void CEndmarksMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);

   if(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))
   {
      COLORREF txColor=GetSysColor((lpDIS->itemState & ODS_SELECTED)?COLOR_HIGHLIGHTTEXT:COLOR_MENUTEXT);
      COLORREF bkColor=GetSysColor((lpDIS->itemState & ODS_SELECTED)?COLOR_HIGHLIGHT:COLOR_MENU);

      CGDITextColor tc(pDC,txColor);
      CGDIBackColor bc(pDC,bkColor);

      CGDIBitmapDC memDC(pDC,&m_bitmap);
      CRect rc(lpDIS->rcItem);
      if(MAKEPOINTS(lpDIS->itemData).x==1 && (lpDIS->itemAction&ODA_SELECT))
      {
         rc.InflateRect(20,0);
         pDC->BitBlt(rc.left,rc.top,54,13,&memDC,0,MAKEPOINTS(lpDIS->itemData).y*13,SRCCOPY);
      }
      else
      {
         rc.InflateRect(2,0);
         pDC->BitBlt(rc.left,rc.top,18,13,&memDC,MAKEPOINTS(lpDIS->itemData).x*18,MAKEPOINTS(lpDIS->itemData).y*13,SRCCOPY);
      }
   }
}

int CEndmarksMenu::Track(CWnd* pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   int cmd=CMenu::TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,pt.x,pt.y,pWnd);
   cmd-=(cmd?ID_ENDMARKS_BASE:1);
   return cmd;
}

/////////////////////////////////////////////////////////////////////////////
// CLineStyleMenu implementation

CLineStyleMenu::CLineStyleMenu()
{
}

void CLineStyleMenu::Create(unsigned char* styles,int nCmdBase,bool withSolid)
{
   m_nCmdBase=nCmdBase;
   VERIFY(CreatePopupMenu());
   for(int y=withSolid?0:1; y<16; y++)
   {
      VERIFY(AppendMenu(MF_ENABLED | MF_OWNERDRAW,m_nCmdBase+y,LPSTR(styles+y*16)));
   }
}

void CLineStyleMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
   lpMIS->itemWidth = 135;
   lpMIS->itemHeight = 16;
}

void CLineStyleMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);

   if(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))
   {
      int cxCheck=GetSystemMetrics(SM_CXMENUCHECK);
      CRect rc(lpDIS->rcItem);

      COLORREF txColor=GetSysColor((lpDIS->itemState & ODS_SELECTED)?COLOR_HIGHLIGHTTEXT:COLOR_MENUTEXT);
      COLORREF bkColor=GetSysColor((lpDIS->itemState & ODS_SELECTED)?COLOR_HIGHLIGHT:COLOR_MENU);

      {
         CGDITextColor tc(pDC,txColor);
         CGDIBackColor bc(pDC,bkColor);

         CString str=" P";
         int i=LOWORD(lpDIS->itemID)-m_nCmdBase;
         if(i>0) str.Format("%2d",i);
         pDC->ExtTextOut(rc.left+cxCheck,rc.top+1,ETO_OPAQUE|ETO_CLIPPED,&rc,str,NULL);

         if(lpDIS->itemState&ODS_CHECKED)
         {
            CBitmap check;
            check.LoadOEMBitmap(OBM_CHECK);
            CGDIBitmapDC memDC(pDC,&check);
            pDC->BitBlt(rc.left,rc.top+1,cxCheck,GetSystemMetrics(SM_CYMENUCHECK),&memDC,0,0,SRCCOPY);
         }
      }

      CGDIBackColor bc(pDC,txColor);
      rc.top=(rc.top+rc.bottom)/2;
      rc.bottom=rc.top+1;
      rc.left+=cxCheck+20;

      unsigned char* style=(unsigned char*)lpDIS->itemData;

      if(*style)
      {
         int i=1;
         int right=lpDIS->rcItem.right-7;
         while(rc.left<right)
         {
            rc.right=rc.left+style[i];
            if(rc.right>right) rc.right=right;
            if(i%2) pDC->ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
            if(style[++i]==0xFF) i=1;
            rc.left=rc.right;
         }
      }
      else
      {
         rc.right=lpDIS->rcItem.right-7;
         pDC->ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
      }
   }
}

int CLineStyleMenu::Track(CWnd* pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   int cmd=CMenu::TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,pt.x,pt.y,pWnd);
   cmd-=(cmd?m_nCmdBase:1);
   return cmd;
}

void CLineStyleMenu::CmdTrack(CWnd *pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   CMenu::TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,pt.x,pt.y,pWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CBitmapMenu implementation

CBitmapMenu::CBitmapMenu(int nResID,int nItemNum,int nCmdBase)
{
   m_nCmdBase=nCmdBase;

   m_bitmap.LoadBitmap(nResID);

   BITMAP bmp;
   m_bitmap.GetBitmap(&bmp);
   m_cx=bmp.bmWidth;
   m_cy=bmp.bmHeight/nItemNum;

   VERIFY(CreatePopupMenu());
   for(int y=0; y<nItemNum; y++)
   {
      VERIFY(AppendMenu(MF_ENABLED | MF_OWNERDRAW,m_nCmdBase+y,LPSTR(y)));
   }
}

void CBitmapMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
   lpMIS->itemWidth = m_cx;
   lpMIS->itemHeight = m_cy;
}

void CBitmapMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);

   if(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))
   {
      int cxCheck=GetSystemMetrics(SM_CXMENUCHECK);
      CRect rc(lpDIS->rcItem);

      COLORREF txColor=GetSysColor((lpDIS->itemState & ODS_SELECTED)?COLOR_HIGHLIGHTTEXT:COLOR_MENUTEXT);
      COLORREF bkColor=GetSysColor((lpDIS->itemState & ODS_SELECTED)?COLOR_HIGHLIGHT:COLOR_MENU);

      CGDITextColor tc(pDC,txColor);
      CGDIBackColor bc(pDC,bkColor);

      if(lpDIS->itemState&ODS_CHECKED)
      {
         CBitmap check;
         check.LoadOEMBitmap(OBM_CHECK);
         CGDIBitmapDC memDC(pDC,&check);
         pDC->BitBlt(rc.left,rc.top+1,cxCheck,GetSystemMetrics(SM_CYMENUCHECK),&memDC,0,0,SRCCOPY);
      }

      CGDIBitmapDC memDC(pDC,&m_bitmap);

      pDC->BitBlt(rc.left+cxCheck,rc.top,m_cx,m_cy,&memDC,0,lpDIS->itemData*m_cy,SRCCOPY);
   }
}

int CBitmapMenu::Track(CWnd* pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   int cmd=CMenu::TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,pt.x,pt.y,pWnd);
   cmd-=(cmd?m_nCmdBase:1);
   return cmd;
}

void CBitmapMenu::CmdTrack(CWnd *pWnd)
{
   CPoint pt;
   GetCursorPos(&pt);
   CMenu::TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,pt.x,pt.y,pWnd);
}
